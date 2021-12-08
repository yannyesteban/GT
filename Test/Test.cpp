// Test.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <regex>

#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <regex>

#include "suma.h"
bool isNumber(std::string str);
void getItem(std::string w[], int& len, const char* buffer);
std::vector<std::string> split(std::string str, const char* token);
template <class Container>
void split1(const std::string& str, Container& cont);
template <class Container>
void split2(const std::string& str, Container& cont, char delim = ' ');

char* GenerateCheckSum(char* buf, long bufLen)
{
	static char tmpBuf[4];
	long idx;
	unsigned int cks;

	for (idx = 0L, cks = 0; idx < bufLen; cks += (unsigned int)buf[idx++]);
	sprintf(tmpBuf, "%03d", (unsigned int)(cks % 256));
	return(tmpBuf);
}

int getCheckSum(char* buf, long bufLen) {
	static char tmpBuf[4];
	long idx;
	unsigned int cks;

	for (idx = 0L, cks = 0; idx < bufLen; cks += (unsigned int)buf[idx++]);
	//sprintf(tmpBuf, "%03d", (unsigned int)(cks % 256));
	return(cks % 256);
}

int main()
{
   
	
	

	std::string str = "jose augusto";

	std::cout << " check 1 " << GenerateCheckSum((char*)str.c_str(), str.size()) << "\n";
	std::cout << " checksum 2" << str.size() << "..." << getCheckSum((char*)str.c_str(), str.size());
	int check = getCheckSum((char*)str.c_str(), str.size());
	

	
	return 0;



	
	std::vector<std::string> result;
	std::smatch m;
	std::string ss("\xFA\xF8\xE7\x03\xB1yanny nunez,4.5,-8.369898,esteban");

	std::regex Pala("([a-zA-Z0-9\\-\\,\\.\\s]+)");

	while (std::regex_search(ss, m, Pala)) {
		for (int i = 0; i < m.size(); i++) {
			std::cout << " *** " << m[i].str() << "\n";

		}

		ss = m.suffix().str();
	}
	return 1;

	std::string t = "T44";
	if (isNumber(t)) {
		std::cout << "si es un numero " << t << std::endl;
	} else {
		std::cout << "NO es un numero " << t << std::endl;
	}
    std::cout << "Hello World!\n" << suma(3,4);
	std::string paramsList[20];
	int length = 0;

	const std::string cmd = "one,two,three,,,,,seven";

	std::vector<std::string> v;

	split2(cmd, v, ',');

	for (std::vector<std::string>::iterator it = v.begin(); it != v.end(); ++it) {
		std::cout << " : " << *it << std::endl;
	}

	/*
	getItem(paramsList, length, "uno,dos,tres,cinco,seis");

	for (int i = 0; i < length; i++) {
		std::cout << "param: " << paramsList[i] << std::endl;
	}
	*/
	std::cout << "bye" << std::endl;
}
bool isNumber(std::string ss) {

	std::smatch m;
	//std::string ss(buffer);

	//std::regex Pala("[0-9.a-zA-ZñÑáéíóúÁÉÍÓÚäëïöüÄËÏÖÜ]+");
	std::regex re("^-?[0-9][0-9,\\.]+$");
	
	if (std::regex_search(ss, m, re)) {
		return true;
		
	}
	return false;

}

std::vector<std::string> split(std::string str, const char* token) {
	const char* cstr = (const char*)str.c_str();


	
	char* current = strtok((char *)cstr, token);
	std::vector<std::string> v;
	while (current != NULL) {
		v.push_back(current);
		current = strtok(NULL, token);
	}
	return v;
}

void getItem(std::string w[], int& len, const char* buffer) {

	std::smatch m;
	std::string ss(buffer);

	//std::regex Pala("[0-9.a-zA-ZñÑáéíóúÁÉÍÓÚäëïöüÄËÏÖÜ]+");
	//std::regex Pala("(?:[^,]+)");
	std::regex Pala("(?:[^,]+)");
	len = 0;
	while (std::regex_search(ss, m, Pala)) {

		for (int i = 0; i < m.size(); i++) {
			w[len++] = m[i].str();
		}

		ss = m.suffix().str();

	}



}

template <class Container>
void split1(const std::string& str, Container& cont) {
	std::istringstream iss(str);
	std::copy(std::istream_iterator<std::string>(iss),
		std::istream_iterator<std::string>(),
		std::back_inserter(cont));
}

template <class Container>
void split2(const std::string& str, Container& cont, char delim ) {
	std::stringstream ss(str);
	std::string token;
	while (std::getline(ss, token, delim)) {
		cont.push_back(token);
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

