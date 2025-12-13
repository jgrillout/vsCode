#pragma once
#pragma message("&&&&&&&& Including curses.h")
//#include "C:\\c\\DataEntryDevTool\\DataEntryDevTool\\curses.h"
#include "curses.h"

#ifdef _WIN32

//#pragma message("&&&&&&&& Including curses.h")
//#include "curses.h"
#define BACKSPACE_KEY 8
#define PADENTER_KEY  0x1cb // Enter on keypad in pdcurses
#define PLATFORM_ENTER_KEY 13
#define PLATFORM_INSERT_KEY PAD0  /* keypad 0 AKA Insert*/
#define PLATFORM_STANDARD_INSERT_KEY 0x14b  /* Standard Insert key */
//#pragma message("&&&&&&&& Finished including curses")
#else
//#pragma message("&&&&&&&& Including ncurses.h")
//#include <ncurses.h>
#define BACKSPACE_KEY KEY_BACKSPACE
#define PADENTER_KEY  KEY_ENTER
#define PLATFORM_ENTER_KEY PADENTER_KEY
#define PLATFORM_INSERT_KEY 331
//#pragma message("&&&&&&&& Finished including ncurses")
#endif


