// Device.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>

#include "Device.h"
#include "DeviceAdmin.h"

#include "GTServer/Config.h"

#include <iomanip> // para la fecha

int main()
{
    setlocale(LC_CTYPE, "Spanish");

    /*
    struct timespec ts;
    int k = timespec_get(&ts, TIME_UTC);
    char buff[100];

    std::cout << "tiempo en SEGUNDOS " << ts.tv_sec  << std::endl << std::endl;
    std::cout << "tiempo en nano " << ts.tv_nsec * 1E-9 << std::endl  << std::endl;
    strftime(buff, sizeof buff, "%D %T", gmtime(&ts.tv_sec));
    printf("Current time: %s.%09ld UTC\n", buff, ts.tv_nsec);

    return 0;
    */
    system("cls");
    Color::set(4);
    std::cout << "GT Device v1.0 (2020)!\n";
    Color::set(0);
    std::cout << "Hello World!\n";

    auto appInfo = GT::Config::load("device.json");
    std::cout << appInfo.appname << std::endl;
    std::cout << appInfo.port << std::endl;

    
    //auto db = new GT::DB2(appInfo.db);
    //db->connect();
    //db->init();
    
    //return 0;

    GT::CSInfo Info;
    Info.host = (char*)"127.0.0.1";
    Info.port = appInfo.port;

    bool option = true;

    if (option) {
        auto A = new GT::DeviceAdmin();
        //A->db = db;
        A->run(&appInfo);
    } else {
        auto db = new GT::DB2(appInfo.db);
        db->connect();
        //db->init();
        auto C = new GT::Device(Info, db);
        C->start();
    }

    /*
    
    return 0;
    */
    
    
    
}

// Ejecutar programa: Ctrl + F5 o menú Depurar > Iniciar sin depurar
// Depurar programa: F5 o menú Depurar > Iniciar depuración

// Sugerencias para primeros pasos: 1. Use la ventana del Explorador de soluciones para agregar y administrar archivos
//   2. Use la ventana de Team Explorer para conectar con el control de código fuente
//   3. Use la ventana de salida para ver la salida de compilación y otros mensajes
//   4. Use la ventana Lista de errores para ver los errores
//   5. Vaya a Proyecto > Agregar nuevo elemento para crear nuevos archivos de código, o a Proyecto > Agregar elemento existente para agregar archivos de código existentes al proyecto
//   6. En el futuro, para volver a abrir este proyecto, vaya a Archivo > Abrir > Proyecto y seleccione el archivo .sln
