// Test.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//

#include <iostream>
#include <regex>
#include "suma.h"
bool isNumber(std::string str);
int main()
{
   
	std::string t = "T44";
	if (isNumber(t)) {
		std::cout << "si es un numero " << t << std::endl;
	} else {
		std::cout << "NO es un numero " << t << std::endl;
	}
    std::cout << "Hello World!\n" << suma(3,4);
}
bool isNumber(std::string ss) {

	std::smatch m;
	//std::string ss(buffer);

	//std::regex Pala("[0-9.a-zA-ZñÑáéíóúÁÉÍÓÚäëïöüÄËÏÖÜ]+");
	std::regex re("^-?[0-9][0-9,\.]+$");
	
	if (std::regex_search(ss, m, re)) {
		return true;
		
	}
	return false;

}
// Ejecutar programa: Ctrl + F5 o menú Depurar > Iniciar sin depurar
// Depurar programa: F5 o menú Depurar > Iniciar depuración

// Sugerencias para primeros pasos: 1. Use la ventana del Explorador de soluciones para agregar y administrar archivos
//   2. Use la ventana de Team Explorer para conectar con el control de código fuente
//   3. Use la ventana de salida para ver la salida de compilación y otros mensajes
//   4. Use la ventana Lista de errores para ver los errores
//   5. Vaya a Proyecto > Agregar nuevo elemento para crear nuevos archivos de código, o a Proyecto > Agregar elemento existente para agregar archivos de código existentes al proyecto
//   6. En el futuro, para volver a abrir este proyecto, vaya a Archivo > Abrir > Proyecto y seleccione el archivo .sln
