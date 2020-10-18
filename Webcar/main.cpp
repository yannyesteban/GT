// Webcar.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//


#include <iostream>
#include "Webcar.h"

int main()
{
    std::cout << "Webcar Sync Module!\n\n";

   
    WC::Webcar * webcar = new WC::Webcar("webcar.json");
    
    WC::TrackParam P({1, 2087, "200210001","2020-10-13 06:51:00", 10.2121, 60.121,80,15});
    webcar->insertTrack("2012000316", "2012000750,20201017171030,-68.587445,9.655353,0,0,162.0,0,2,0.0,0,0.01,0.01,0");
    //webcar->evalTrack(&P);
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

