// test3.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//

#include <iostream>
#include <regex>


void getItem(std::string exp, std::string str) {

	std::smatch m;
	

	
	std::regex Exp(exp);
	
	while (std::regex_search(str, m, Exp)) {

		std::cout << m[1].str() << " ";

		for (int i = 0; i < m.size(); i++) {
			
				
				

		}

		str = m.suffix().str();

	}

}

struct GTClient1 {
	
	short int type = 0;
	int id = -2;
	std::string name;
	std::string address;
	int verionId = -2;

	

};

int main()
{
    
	std::vector<std::string> exps = {
		"&&(?:\\w)(?:\\d+),(\\w+),",
		"^(\\d{10}),(?:\\d){14}"
	};
	

	std::string exp = "&&(?:\\w)(?:\\d+),(\\w+),";
	std::string message = "&&w44,1525,ttt";

	getItem(exp,message );


	exp = "^(\\d{10}),(?:[^,]+),";
	message = "1012000299,2022-05-03 20:36:44,-67.609488,10.231263,0,176,440,0,100,0,1,0.000000,0.000000,143";
	
	getItem(exp, message);
	
}

// Ejecutar programa: Ctrl + F5 o menú Depurar > Iniciar sin depurar
// Depurar programa: F5 o menú Depurar > Iniciar depuración

// Sugerencias para primeros pasos: 1. Use la ventana del Explorador de soluciones para agregar y administrar archivos
//   2. Use la ventana de Team Explorer para conectar con el control de código fuente
//   3. Use la ventana de salida para ver la salida de compilación y otros mensajes
//   4. Use la ventana Lista de errores para ver los errores
//   5. Vaya a Proyecto > Agregar nuevo elemento para crear nuevos archivos de código, o a Proyecto > Agregar elemento existente para agregar archivos de código existentes al proyecto
//   6. En el futuro, para volver a abrir este proyecto, vaya a Archivo > Abrir > Proyecto y seleccione el archivo .sln
