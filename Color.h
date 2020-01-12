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



};

