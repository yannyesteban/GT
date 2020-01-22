// GTClient.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//

#include <iostream>
#include "Color.h"
#include "Client.h"



using namespace GT;

void startConsole();
int main()
{
    setlocale(LC_CTYPE, "Spanish");
    system("cls");
    Color::set(4);
    std::cout << "GT Client v1.0 (2020)!\n";
    Color::set(0);
    std::cout << "Hello World!\n";
    CSInfo Info;
    Info.host = (char*)"127.0.0.1";
    Info.port = "3311";

    Client* C = new Client(Info);
    C->start();

	

}

// Ejecutar programa: Ctrl + F5 o menú Depurar > Iniciar sin depurar
// Depurar programa: F5 o menú Depurar > Iniciar depuración

// Sugerencias para primeros pasos: 1. Use la ventana del Explorador de soluciones para agregar y administrar archivos
//   2. Use la ventana de Team Explorer para conectar con el control de código fuente
//   3. Use la ventana de salida para ver la salida de compilación y otros mensajes
//   4. Use la ventana Lista de errores para ver los errores
//   5. Vaya a Proyecto > Agregar nuevo elemento para crear nuevos archivos de código, o a Proyecto > Agregar elemento existente para agregar archivos de código existentes al proyecto
//   6. En el futuro, para volver a abrir este proyecto, vaya a Archivo > Abrir > Proyecto y seleccione el archivo .sln


void startConsole() {
	
	return ;
}
