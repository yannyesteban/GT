#include "Debug.h"


namespace GT {
	Debug::Debug() {

	}
	Debug::~Debug() {
	}
	const char* Debug::black(const char* msg) {
		char m[100] = "";
		strcat(m, ANSI_COLOR_BLACK);
		strcat(m, msg);
		strcat(m, ANSI_COLOR_RESET);
		return m;
	}
	const char* Debug::red(const char* msg) {
		char m[100] = "";
		strcat(m, ANSI_COLOR_RED);
		strcat(m, msg);
		strcat(m, ANSI_COLOR_RESET);
		return m;
	}
	const char* Debug::green(const char* msg) {
		char m[100] = "";
		strcat(m, ANSI_COLOR_GREEN);
		strcat(m, msg);
		strcat(m, ANSI_COLOR_RESET);
		return m;
	}
	const char* Debug::yellow(const char* msg) {
		char m[100] = "";
		strcat(m, ANSI_COLOR_YELLOW);
		strcat(m, msg);
		strcat(m, ANSI_COLOR_RESET);
		return m;
	}
	const char* Debug::blue(const char* msg) {
		char m[100] = "";
		strcat(m, ANSI_COLOR_BLUE);
		strcat(m, msg);
		strcat(m, ANSI_COLOR_RESET);
		return m;
	}
	const char* Debug::magenta(const char* msg) {
		char m[100] = "";
		strcat(m, ANSI_COLOR_MAGENTA);
		strcat(m, msg);
		strcat(m, ANSI_COLOR_RESET);
		return m;
	}
	const char* Debug::cyan(const char* msg) {
		char m[100] = "";
		strcat(m, ANSI_COLOR_CYAN);
		strcat(m, msg);
		strcat(m, ANSI_COLOR_RESET);
		return m;
	}
	const char* Debug::gray(const char* msg) {
		char m[100] = "";
		strcat(m, ANSI_COLOR_GRAY);
		strcat(m, msg);
		strcat(m, ANSI_COLOR_RESET);
		return m;
	}
	void Debug::db(const char* msg, int color, int background) {
		char m[100]="";
		switch (color)
		{
		case RED:
			strcat(m, ANSI_COLOR_RED);
			strcat(m, msg);
			strcat(m, ANSI_COLOR_RESET);
		default:
			break;
		}
		

		printf(m);
	}
}