// GTServer.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "Color.h"

#include "Server.h"
#include "Config.h"

#include <iomanip> // para la fecha

#include "GTComm.h"


using namespace GT;

int main(int argc, char* argv[])
{
    setlocale(LC_CTYPE, "Spanish");

    std::string configFile = "setting.json";
    if (argc > 1) {
        configFile = std::string(argv[1]);
    }
    
    
        


    

    system("cls");

    auto inf = GTAppConfig({

        });
    auto admin = new GTComm(inf);
    admin->start(configFile.c_str());
    return 12474737;

    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::cout << std::put_time(&tm, "%d/%m/%Y %H:%M:%S") << std::endl;

    Color::set(4);
    std::cout << "GT v1.0 (2022)!\n";
    Color::set(0);

    auto appInfo = GT::Config::load("config.json");
    std::cout << Color::_yellow() << "NAME: " << appInfo.appname << Color::_reset() << std::endl;
    printf("Version: %s \n", appInfo.version);
    printf("DB Name: %s\n", appInfo.db.name);
    printf("Socket Port: %d\n\n", appInfo.port);
    printf("Max Clients: %d\n\n", appInfo.max_clients);
    //printf("Time: %s\n\n", XT::Time::now());

    SocketInfo Info;
    Info.host = (char*)"127.0.0.1";
    Info.port = appInfo.port;
    Info.maxClients = appInfo.max_clients;

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
