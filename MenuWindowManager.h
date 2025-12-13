// Version-02.20.25.16.07
// File: MenuWindowManager.h
#ifndef MenuWindowManager_H
#define MenuWindowManager_H
#include <assert.h>
#include <vector>
#include "DataEntry.h"
#include "CursesCompat.h"


class MenuWindowManager {
public:
    friend class DataEntry;
    static WINDOW* getwinMenu(int menuHeight, int menuWidth);
    static void destroywinMenu();
    static void destroywinMenuShadow();
    static void hidewinMenu();
    static WINDOW* createShadowForWindow(WINDOW* win,int winHeight, int winWidth, int startY, int startX);
    static WINDOW* winMenu_;    
    static WINDOW* winMenuShadow_;


private:
    MenuWindowManager() = default;  // Private constructor to prevent instantiation

    
};


#endif


