// GTServer.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//


#include <iostream>
#include "Color.h"

#include "Server.h"
#include "Config.h"
using namespace GT;

int main()
{
    setlocale(LC_CTYPE, "Spanish");
    system("cls");


    Color::set(4);
    std::cout << "GT v1.0 (2020)!\n";
    Color::set(0);

    auto appInfo = GT::Config::load("C:\\source\\cpp\\GT\\GTServer\\config.json");
    printf("APP NAME: %s\n", appInfo.appname);
    printf("Version: %s \n", appInfo.version);
    printf("DB Name: %s\n", appInfo.db.name);
    ///printf("Time: %s", XT::Time::now());

    SocketInfo Info;
    Info.host = (char*)"127.0.0.1";
    Info.port = 3311;
    Info.maxClients = 30;

    Server * S = new Server(Info);
    S->init(appInfo);
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
