// COMII.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
// Comunicación a través del puerto serie
// usando el API de Windows
// Modo consola.
// (C) Enero de 2013, Salvador Pozo Coronado
// Con Clase: http://www.conclase.net
// salvador@conclase.net

#include <iostream>
#include <cstring>
#include <windows.h>

using namespace std;

// Tipos de datos:
typedef struct {
    char Puerto[5];
    int Baudios;
    int BitsDatos;
    int BitsStop;
    char Paridad[25];
} tipoOpciones;

bool ocupado;
// Prototipos:
HANDLE InicioComunicacion(tipoOpciones*);
bool FinComunicacion(HANDLE);
DWORD Hilo(LPDWORD lpdwParam);
void EscribirSerie(HANDLE, char*);

int main(int argc, char* argv[]) {
    bool salir = false;
    DWORD id;
    char cad[80];
    tipoOpciones Ops;         // Opciones
    HANDLE idComDev;
    HANDLE hHilo;             // Hilo del puerto serie

    ocupado = true;
    // Inicializar opciones del puerto serie:
    strcpy(Ops.Puerto, "COM3");
    Ops.Baudios = CBR_9600;
    Ops.BitsDatos = 8;
    Ops.BitsStop = 1;
    strcpy(Ops.Paridad, "Sin paridad");

    // No se ha establecido comunicación:
    idComDev = InicioComunicacion(&Ops);
    if (idComDev == INVALID_HANDLE_VALUE) {
        cout << "Inicialización puerto serie" << endl;
        cout << "ERROR: No se puede acceder al puerto serie." << endl;
        return 1;
    }
    // Lanzar hilo de lectura del puerto serie:
    hHilo = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Hilo, (LPDWORD)&idComDev, 0, &id);
    if (!hHilo) cout << "Error" << endl;
    // Bucle principal:
    ocupado = false;
    while (!salir) {
        // Leer un comando:
        cin.getline(cad, 80);
        // Si es "salir", abandonar el bucle:
        if (!strcmp(cad, "salir")) salir = true;
        else {
            // Si no, enviar cadena por el puerto serie:
            strcat(cad, "\r");
            EscribirSerie(idComDev, cad);
        }
    }
    // Liberar hilo:
    CloseHandle(hHilo);
    // Liberar puerto serie:
    FinComunicacion(idComDev);
    return 0;
}

// Iniciar el puerto serie:
HANDLE InicioComunicacion(tipoOpciones* Ops) {
    bool fSuccess;
    HANDLE idComDev;
    DCB dcb;                  // Puerto serie

    // Abrir el fichero asociado al puerto:
    LPCTSTR  p = TEXT("COM3");
    idComDev = CreateFile(p, GENERIC_READ | GENERIC_WRITE,
        0, NULL, OPEN_EXISTING, 0, NULL);
    if (idComDev == INVALID_HANDLE_VALUE) {
        cout << "ERROR: CreateFile. Inicialización puerto serie" << endl;
        return INVALID_HANDLE_VALUE;
    }
    PurgeComm(idComDev, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);

    // Leer estructura de control del puerto serie, cdb:
    fSuccess = GetCommState(idComDev, &dcb);
    if (!fSuccess) {
        cout << "ERROR: GetCommState. Inicialización puerto serie" << endl;
        // Cerrar el puerto, regresar con 0.
        CloseHandle(idComDev);
        return INVALID_HANDLE_VALUE;
    }

    // Modificar el dcb según las opciones definidas:
    dcb.BaudRate = Ops->Baudios;
    dcb.ByteSize = Ops->BitsDatos;

    if (!strcmp(Ops->Paridad, "Sin paridad")) dcb.Parity = NOPARITY;
    if (!strcmp(Ops->Paridad, "Paridad par")) dcb.Parity = EVENPARITY;
    if (!strcmp(Ops->Paridad, "Paridad impar")) dcb.Parity = ODDPARITY;

    switch (Ops->BitsStop) {
    case 1:
        dcb.StopBits = ONESTOPBIT;
        break;
    case 2:
        dcb.StopBits = TWOSTOPBITS;
        break;
    }

    // Modificar la estructura de control del puerto serie:
    fSuccess = SetCommState(idComDev, &dcb);
    if (!fSuccess) {
        cout << "ERROR: SetCommStatus. Inicialización puerto serie" << endl;
        // Cerrar el puerto, regresar con 0.
        CloseHandle(idComDev);
        return INVALID_HANDLE_VALUE;
    }

    //// ASIGNAR TIMOUTS!!!

    return idComDev;
}

// Finalizar comunicación por puerto serie:
bool FinComunicacion(HANDLE idComDev) {
    // Cerrar el puerto serie:
    CloseHandle(idComDev);
    return true;
}

// Hilo de escucha del puerto serie:
DWORD Hilo(LPDWORD lpdwParam) {
    DWORD leidos;
    COMSTAT cs;
    char* cad;
    DWORD dwCommEvent;
    HANDLE idComDev = *((HANDLE*)lpdwParam);

    if (!SetCommMask(idComDev, EV_RXCHAR)) {
        cout << "Error al iniciar captura de evento" << endl;
        return 0;
    }
    do {
        if (WaitCommEvent(idComDev, &dwCommEvent, NULL)) {
            SetCommMask(idComDev, EV_RXCHAR);
            while (ocupado);
            ocupado = true;
            if (dwCommEvent & EV_RXCHAR) {
                ClearCommError(idComDev, &leidos, &cs);
                leidos = 0;
                cout << "Detectados " << cs.cbInQue << " caracteres" << endl;
                /* Leer buffer desde puerto serie */
                if (cs.cbInQue) {
                    cad = new char[cs.cbInQue + 3]; // Caracteres en buffer, más retorno de línea, más nulo
                    ReadFile(idComDev, cad, cs.cbInQue, &leidos, NULL);
                    cad[leidos] = '\n'; // Terminar cadena con salto de línea y nulo
                    cad[leidos + 1] = '\r';
                    cad[leidos + 2] = 0;
                    cout << cad;
                    delete[] cad;
                }
            } else {
                cout << "Evento: EV_BREAK o EV_ERR" << endl;
                ClearCommBreak(idComDev);
            }
            ocupado = false;
        } else {
            cout << "Error en WaitCommEvent" << endl;
            ClearCommError(idComDev, NULL, NULL);
        }
        Sleep(10);
    } while (true);
    return 0;
}

void EscribirSerie(HANDLE idComDev, char* buf) {
    char oBuffer[256];  /* Buffer de salida */
    DWORD iBytesWritten;

    iBytesWritten = 0;
    strcpy(oBuffer, buf);
    while (ocupado);
    ocupado = true;
    WriteFile(idComDev, oBuffer, strlen(oBuffer), &iBytesWritten, NULL);
    ocupado = false;
}
