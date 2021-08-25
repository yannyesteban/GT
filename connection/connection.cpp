// connection.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//
#define _CRT_SECURE_NO_WARNINGS
#define MULTILINE(...) #__VA_ARGS__


#include <iostream>
#include <chrono>
#include <thread>
#include <ctime>
#include "Color.h"
#include "Types.h"
#include "Tool.h"
#include "ConsoleColors.h"
#include "DB.h"



;
int main()
{
	
	using std::chrono::system_clock;

    std::cout << "Hello World!\n";
	GT::InfoDB infoDB = {
		"localhost",
		"3306",
		"gt",
		"root",
		"123456",
		false


	};
	std::cout << infoDB.pass << "\n\n";
	GT::DB*  db = new GT::DB(infoDB);
    db->connect();

	while (true) {

		
		db->updateUnitConnected();
		std::cout << "are conected\n";

		std::this_thread::sleep_for(std::chrono::seconds(10));
	}
}

// Ejecutar programa: Ctrl + F5 o menú Depurar > Iniciar sin depurar
// Depurar programa: F5 o menú Depurar > Iniciar depuración

// Sugerencias para primeros pasos: 1. Use la ventana del Explorador de soluciones para agregar y administrar archivos
//   2. Use la ventana de Team Explorer para conectar con el control de código fuente
//   3. Use la ventana de salida para ver la salida de compilación y otros mensajes
//   4. Use la ventana Lista de errores para ver los errores
//   5. Vaya a Proyecto > Agregar nuevo elemento para crear nuevos archivos de código, o a Proyecto > Agregar elemento existente para agregar archivos de código existentes al proyecto
//   6. En el futuro, para volver a abrir este proyecto, vaya a Archivo > Abrir > Proyecto y seleccione el archivo .sln
