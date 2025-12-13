// Version-02.20.25.16.07
// File: SharedWindowManager.h
#ifndef SharedWindowManager_H
#define SharedWindowManager_H
#include <assert.h>
#include <vector>
#include "DataEntry.h"
//#include "TransEntryControl.h"



class SharedWindowManager {
public:
    //friend class DataEntry;

    static WINDOW* getwinFormScreen(std::vector<ScreenProperties>& screenProps);
    static WINDOW* getwinFormScreen();      
    static WINDOW* getwinMsgArea();
    static WINDOW* getwinScroll(int winHeight, int winWidth, int starty, int startx);
    static void backupWinFormScreenUnderMsgArea();

    static void destroywinFormScreen();
    static void destroywinMsgArea();
    static void destroywinScroll();        
    static void hidewinFormScreen();
    static void hidewinMsgArea();
    static void hidewinScroll();
    static WINDOW* winFormScreen_; 
    static WINDOW* winMsgArea_;
    static WINDOW* winScroll_;
    static WINDOW* winMsgBackup_;

private:
    SharedWindowManager() = default;  // Private constructor to prevent instantiation

    
};


#endif


