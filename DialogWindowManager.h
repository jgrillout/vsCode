// Version-02.20.25.16.07
// File:DialogWindowManager.h
#ifndef DialogWindowManager_H
#define DialogWindowManager_H
#include <assert.h>
#include <vector>
#include "DataEntry.h"
#include "CursesCompat.h"


class DialogWindowManager {
public:
    friend class DataEntry;
    static WINDOW* getwinDialog(int DialogHeight, int DialogWidth);
    static void destroywinDialog();
    static void destroywinDialogShadow();
    static void hidewinDialog();
    static WINDOW* createShadowForWindow(WINDOW* win,int winHeight, int winWidth, int startY, int startX);



private:
    static WINDOW* winDialog_;
    static WINDOW* winDialogShadow_;
   DialogWindowManager() = default;  // Private constructor to prevent instantiation

    
};


#endif


