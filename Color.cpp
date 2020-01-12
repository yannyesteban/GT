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