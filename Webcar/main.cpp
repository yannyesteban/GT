// Webcar.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//


#include <iostream>
#include "Webcar.h"

int main()
{
    std::cout << "Hello World 2!\n";
    InfoDB info = {
        "localhost",
        "3306",
        "cota",
        "root",
        "123456"
    };
    Webcar * webcar = new Webcar(info);
    webcar->connect();
    InfoParam P({1, 2087, 1, 1, 10, 84,"200210001","2020-10-13 06:51:00"});
    webcar->evalTrack(&P);
    //webcar->evalTrack(1, 2087, 1, 1, 10, 84);
}

// Ejecutar programa: Ctrl + F5 o menú Depurar > Iniciar sin depurar
// Depurar programa: F5 o menú Depurar > Iniciar depuración

// Sugerencias para primeros pasos: 1. Use la ventana del Explorador de soluciones para agregar y administrar archivos
//   2. Use la ventana de Team Explorer para conectar con el control de código fuente
//   3. Use la ventana de salida para ver la salida de compilación y otros mensajes
//   4. Use la ventana Lista de errores para ver los errores
//   5. Vaya a Proyecto > Agregar nuevo elemento para crear nuevos archivos de código, o a Proyecto > Agregar elemento existente para agregar archivos de código existentes al proyecto
//   6. En el futuro, para volver a abrir este proyecto, vaya a Archivo > Abrir > Proyecto y seleccione el archivo .sln

