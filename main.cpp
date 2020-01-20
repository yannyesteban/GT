// GT.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//

#include <iostream>
#include "Conn.h"
#include "WSocket.h"
#include "CSocket.h"
#include "Color.h"

using namespace GT;
int main2()
{
    
    setlocale(LC_CTYPE, "Spanish");
    system("cls");
    Color::set(4);
    std::cout << "GT v1.0 (2020)!\n" ;
    Color::set(0);
    SocketInfo Info;
    Info.host = (char*)"127.0.0.1";
    Info.port = 3311;
    Info.maxClients = 30;

    WSocket *S = new WSocket(Info);
    S->start();
    //S->startListen();
    return 1;
}

int main() {
    setlocale(LC_CTYPE, "Spanish");
    system("cls");
    Color::set(4);
    std::cout << "GT Client v1.0 (2020)!\n";
    Color::set(0);
    CSInfo Info;
    Info.host = (char*)"127.0.0.1";
    Info.port = "3311";
    Info.maxClients = 30;

    CSocket* S = new CSocket(Info);
    S->start();

    return 1;
}

// Ejecutar programa: Ctrl + F5 o menú Depurar > Iniciar sin depurar
// Depurar programa: F5 o menú Depurar > Iniciar depuración

// Sugerencias para primeros pasos: 1. Use la ventana del Explorador de soluciones para agregar y administrar archivos
//   2. Use la ventana de Team Explorer para conectar con el control de código fuente
//   3. Use la ventana de salida para ver la salida de compilación y otros mensajes
//   4. Use la ventana Lista de errores para ver los errores
//   5. Vaya a Proyecto > Agregar nuevo elemento para crear nuevos archivos de código, o a Proyecto > Agregar elemento existente para agregar archivos de código existentes al proyecto
//   6. En el futuro, para volver a abrir este proyecto, vaya a Archivo > Abrir > Proyecto y seleccione el archivo .sln
