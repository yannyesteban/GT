#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdio.h>
#include <string>
#include "ConsoleColors.h"
class Color {
public:

	static void set(int color, int back);
	static void set(int color);

	static int black;
	static int red;
	static int green;
	static int yellow;
	static int blue;
	static int magenta;
	static int cyan;
	static int gray;
	static int white;
	static int reset;

	static const char* _black();
	static const char* _red();
	static const char* _green();
	static const char* _yellow();
	static const char* _blue();
	static const char* _magenta();
	static const char* _cyan();
	static const char* _gray();
	static const char* _white();
	static const char* _reset();

	static const char* bblack();
	static const char* bred();
	static const char* bgreen();
	static const char* byellow();
	static const char* bblue();
	static const char* bmagenta();
	static const char* bcyan();
	static const char* bgray();
	static const char* bwhite();
	



};

