#include "Color.h"

void Color::set(int color, int back) {
	switch (color)
	{
	case 0:
		printf(ANSI_COLOR_RESET);
		break;
	case 1:
		printf(ANSI_COLOR_BLACK);
		break;
	case 2:
		printf(ANSI_COLOR_RED);
		break;
	case 3:
		printf(ANSI_COLOR_GREEN);
		break;
	case 4:
		printf(ANSI_COLOR_YELLOW);
		break;
	case 5:
		printf(ANSI_COLOR_BLUE);
		break;
	case 6:
		printf(ANSI_COLOR_MAGENTA);
		break;
	case 7:
		printf(ANSI_COLOR_CYAN);
		break;
	case 8:
		printf(ANSI_COLOR_GRAY);
		break;
	default:
		printf(ANSI_COLOR_RESET);
		break;
	}

	if (back < 0) {
		return;
	}
		

	switch (back)
	{
	case 0:
		printf(ANSI_COLOR_RESET);
		break;
	case 1:
		printf(ANSI_COLOR_BLACK_);
		break;
	case 2:
		printf(ANSI_COLOR_RED_);
		break;
	case 3:
		printf(ANSI_COLOR_GREEN_);
		break;
	case 4:
		printf(ANSI_COLOR_YELLOW_);
		break;
	case 5:
		printf(ANSI_COLOR_BLUE_);
		break;
	case 6:
		printf(ANSI_COLOR_MAGENTA_);
		break;
	case 7:
		printf(ANSI_COLOR_CYAN_);
		break;
	default:
		printf(ANSI_COLOR_WHITE_);
		break;
	}

}

void Color::set(int color) {
	set(color, -1);
}

int Color::black = 1;
int Color::red = 2;
int Color::green = 3;
int Color::yellow = 4;
int Color::blue = 5;
int Color::magenta = 6;
int Color::cyan = 7;
int Color::gray = 8;
int Color::white = 9;
int Color::reset = 0;

const char* Color::_black() {
	return ANSI_COLOR_BLACK;
}
const char* Color::_red() {
	return ANSI_COLOR_RED;
}
const char* Color::_green() {
	return ANSI_COLOR_GREEN;
}
const char* Color::_yellow() {
	return ANSI_COLOR_YELLOW;
}
const char* Color::_blue() {
	return ANSI_COLOR_BLUE;
}
const char* Color::_magenta() {
	return ANSI_COLOR_MAGENTA;
}

const char* Color::_cyan() {
	return ANSI_COLOR_CYAN;
}
const char* Color::_gray() {
	return ANSI_COLOR_GRAY;
}
const char* Color::_white() {
	return ANSI_COLOR_RESET;
}
const char* Color::_reset() {
	return ANSI_COLOR_RESET;
}


const char* Color::bblack() {
	return ANSI_COLOR_BLACK_;
}
const char* Color::bred() {
	return ANSI_COLOR_RED_;
}
const char* Color::bgreen() {
	return ANSI_COLOR_GREEN_;
}
const char* Color::byellow() {
	return ANSI_COLOR_YELLOW_;
}
const char* Color::bblue() {
	return ANSI_COLOR_BLUE_;
}
const char* Color::bmagenta() {
	return ANSI_COLOR_MAGENTA_;
}

const char* Color::bcyan() {
	return ANSI_COLOR_CYAN_;
}
const char* Color::bgray() {
	return ANSI_COLOR_BOLD;
}
const char* Color::bwhite() {
	return ANSI_COLOR_WHITE_;
}