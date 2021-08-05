// WSServer.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "Conn.h"
#include "WebServer.h"
#include "JsonConfig.h"
#include "Color.h"
#include <iomanip> // para la fecha


using namespace GT;


int main()
{
    setlocale(LC_CTYPE, "Spanish");
    system("cls");

    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::cout << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") << std::endl;

    auto appInfo = GT::JsonConfig::load("wsserver.json");
    printf("APP NAME: %s\n", appInfo.appname);
    printf("Version: %s \n", appInfo.version);
    printf("DB Name: %s\n", appInfo.db.name);
    printf("Socket Port: %d\n\n", appInfo.port);
    printf("Max Clients: %d\n\n", appInfo.max_clients);

    Color::set(4);
    std::cout << "GT WEB-Socket v1.0 (2020)!\n\n\n";
    Color::set(0);
    SocketInfo Info;
    Info.host = (char*)"127.0.0.1";
    Info.port = 3310;
    Info.maxClients = appInfo.max_clients;

    WebServer* S = new WebServer(Info);
    S->init();
    S->start();
    
    //S->startListen();
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
