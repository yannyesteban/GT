// COM.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "windows.h"
#include "commctrl.h"
#include "stdio.h"
#include "conio.h"
#include "string.h"
#include <tchar.h>
HANDLE hSerial;
DCB dcbSerial;
DCB dcbSerialParams;

int init_com() {
    
    int status=0;


    LPCTSTR  p = TEXT("COM3");
    hSerial = CreateFile(p,GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING,  FILE_ATTRIBUTE_NORMAL, 0);
    if(hSerial==INVALID_HANDLE_VALUE) {
        if(GetLastError()==ERROR_FILE_NOT_FOUND){
            printf("PUERTO %p NO EXISTE !! \n", p);
            status=-1;
        }
    } else {
        dcbSerial.DCBlength=sizeof(dcbSerialParams);
        if (!GetCommState(hSerial, &dcbSerialParams)){
            printf("Error get Status!!\n");
            status=-2;
        }
        dcbSerialParams.BaudRate=CBR_9600;
        dcbSerialParams.ByteSize=8;
        dcbSerialParams.StopBits=ONESTOPBIT;
        dcbSerialParams.Parity = NOPARITY; //PARITY_NONE;// NOPARITY;

        dcbSerialParams.fBinary = FALSE;
        dcbSerialParams.fParity = TRUE;
        SetCommState(hSerial, &dcbSerialParams);
        
        
        COMMTIMEOUTS m_CommTimeouts;
        // Leer time-outs actuales:
        GetCommTimeouts(hSerial, &m_CommTimeouts);

        // Nuevos valores de timeout:
        m_CommTimeouts.ReadIntervalTimeout = 500;
        m_CommTimeouts.ReadTotalTimeoutConstant = 500;
        m_CommTimeouts.ReadTotalTimeoutMultiplier = 100;
        m_CommTimeouts.WriteTotalTimeoutConstant = 500;
        m_CommTimeouts.WriteTotalTimeoutMultiplier = 100;

        // Establecer timeouts:
        SetCommTimeouts(hSerial, &m_CommTimeouts);
        /**/
        status=0;
    }   return(status);
} 
char* rec_comChar() {
    DWORD dwBytesRead = 0;
    char szBuff[100]; // debe almacenar los datos
    char n=100;               // Cantidad de datos rx
    char *pBuff=NULL;       // Puntero a un buf para devolver
    if(!ReadFile(hSerial, szBuff, n, &dwBytesRead, NULL)){
        // Error occurred. Report to user
        pBuff=NULL;
        printf("error:  %d ",GetLastError());
    }
    //szBuff[n] = 0x00;
    printf("Recibiendo : %s\n%d, %d\n", szBuff, n, dwBytesRead);
    return (pBuff);
}
int env_com(char *data) {
    DWORD dwBytesWrite = 0;
    char szBuff[1024], n=1;
    strcpy(&szBuff[0],data);
    n=strlen(&szBuff[0]);
    szBuff[n]=0x00;
    
    WriteFile(hSerial, szBuff, n, &dwBytesWrite, NULL);
    printf("enviando: %s, %d - %d\n", szBuff, n, dwBytesWrite);
    //printf("bytes:%d\n", dwBytesWrite);
    return 0;
}
int main(void) {
    /*
    HANDLE hSerial;
    LPCTSTR  pcCommPort = TEXT("COM3");
    hSerial = CreateFile(pcCommPort, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (hSerial == INVALID_HANDLE_VALUE) {
        if (GetLastError() == ERROR_FILE_NOT_FOUND) {
            printf("-- TTY - BASICO 1.0-jhuircan 2011--\n");
            //serial port does not exist.  Inform user.
        }//some other error occurred. Inform user.
    }
   
    */
    char c, * pbuff;
    char buff[50] = "$WP+VER=0000,?";
    printf("-- TTY - BASICO 1.0-jhuircan 2011--\n");
    printf("-- Inicializando Puerto --\n");
    if(!init_com()){
        printf("-- Puerto Configurado --\n");
        printf("-- Puerto Abierto --\n");
        while(1){
            
             pbuff=rec_comChar(); // Lectura deshabilidada – para rx
             //printf("%s\n", pbuff);
             env_com(buff);

             continue;
            if(_kbhit()){
                //printf(".. \n", pbuff);
                c=_getch();
                putchar(c);
                env_com(buff);
                if(c==0x1b) break;
                //env_com(&c);
                c=0;
            }
        }
        CloseHandle(hSerial);
        printf("Puerto Cerrado\n");
        printf("Presione Cualquier Tecla\n");
    }
    _getch();
    return 0;
}
int main2()
{
    std::cout << "Hello World!\n";
    return 0;
}

// Ejecutar programa: Ctrl + F5 o menú Depurar > Iniciar sin depurar
// Depurar programa: F5 o menú Depurar > Iniciar depuración

// Sugerencias para primeros pasos: 1. Use la ventana del Explorador de soluciones para agregar y administrar archivos
//   2. Use la ventana de Team Explorer para conectar con el control de código fuente
//   3. Use la ventana de salida para ver la salida de compilación y otros mensajes
//   4. Use la ventana Lista de errores para ver los errores
//   5. Vaya a Proyecto > Agregar nuevo elemento para crear nuevos archivos de código, o a Proyecto > Agregar elemento existente para agregar archivos de código existentes al proyecto
//   6. En el futuro, para volver a abrir este proyecto, vaya a Archivo > Abrir > Proyecto y seleccione el archivo .sln
