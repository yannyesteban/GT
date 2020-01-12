#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include "ConsoleColors.h"
namespace GT {


#define ANSI_COLOR_BOLD  "\x1b[1m"
#define ANSI_COLOR_DIM "\x1b[2m"
#define ANSI_COLOR_ITALIC  "\x1b[3m"
#define ANSI_COLOR_UNDERSCORE "\x1b[4m"
#define ANSI_COLOR_BLINK "\x1b[5m"
#define ANSI_COLOR_REVERSE  "\x1b[7m"
#define ANSI_COLOR_HIDDEN   "\x1b[8m"
	enum {
		BLACK,
		RED,
		GREEN,
		YELLOW,
		BLUE,
		MAGNETA,
		CYAN,
		RESET
	} ;

	class Debug {
	public:
		Debug();
		~Debug();
		static const char* black(const char*);
		static const char* red(const char*);
		static const char* green(const char*);
		static const char* yellow(const char*);
		static const char* blue(const char*);
		static const char* magenta(const char*);
		static const char* cyan(const char*);
		static const char* gray(const char*);

		void db(const char* msg, int color, int background);
		const char* colors[9] = {
			"\x1b[30m", //black
			"\x1b[31m", // red
			"\x1b[32m", // green
			"\x1b[33m", // yellow
			"\x1b[34m", //blue
			"\x1b[35m", // magneta
			"\x1b[36m", //cyan
			"\x1b[37m", //gray
			"\x1b[0m" //resey
		};
	};
}
