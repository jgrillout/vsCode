// Version-02.20.25.16.07
// File: SharedWindowManager.cpp
#pragma once
#include "SharedWindowManager.h"
 WINDOW* SharedWindowManager::winFormScreen_ = nullptr;
 WINDOW* SharedWindowManager::winMsgArea_ = nullptr;
 WINDOW* SharedWindowManager::winScroll_ = nullptr;
 WINDOW* SharedWindowManager::winMsgBackup_ = nullptr;




WINDOW* SharedWindowManager::getwinFormScreen(std::vector<ScreenProperties>& screenProps) {
/*    int stdscrRows = 0;
    int stdscrCols = 0;   
    getmaxyx(stdscr, stdscrRows, stdscrCols);*/
    int stdscrRows = screenProps[0].screenHeight_;
    int stdscrCols = screenProps[0].screenWidth_;
    if (!winFormScreen_) {
        winFormScreen_ = newwin(stdscrRows, stdscrCols, 0, 0); // Create window with desired size and position
        assert(winFormScreen_ != NULL);
            wattron(winFormScreen_, COLOR_PAIR(3));
            wbkgd(winFormScreen_, COLOR_PAIR(3));
            box(winFormScreen_, ACS_VLINE, ACS_HLINE);
            keypad(winFormScreen_, TRUE);
        }
    
    return winFormScreen_;
}
WINDOW* SharedWindowManager::getwinFormScreen() {
        int stdscrRows = 0;
        int stdscrCols = 0;
        getmaxyx(stdscr, stdscrRows, stdscrCols);
    if (winFormScreen_ == nullptr) {
        
        winFormScreen_ = newwin(stdscrRows, stdscrCols, 0, 0); // Create window with desired size and position
        assert(winFormScreen_ != NULL);
        wattron(winFormScreen_, COLOR_PAIR(3));
        wbkgd(winFormScreen_, COLOR_PAIR(3));
        box(winFormScreen_, ACS_VLINE, ACS_HLINE);
        keypad(winFormScreen_, TRUE);
    }

    return winFormScreen_;
}

WINDOW* SharedWindowManager::getwinMsgArea() {
    int stdscrRows = 0;
    int stdscrCols = 0;
    getmaxyx(stdscr, stdscrRows, stdscrCols);
    
    if (winMsgArea_ == nullptr) {
        //winMsgArea_ = newwin(stdscrRows-5, stdscrCols - 2, stdscrRows - 4 , 1);        
        winMsgArea_ = newwin(4, stdscrCols - 2, stdscrRows - 5, 1);
        assert(winMsgArea_ != NULL);
        wattron(winMsgArea_, COLOR_PAIR(3));
        wbkgd(winMsgArea_, COLOR_PAIR(3));
        box(winMsgArea_, ACS_VLINE, ACS_HLINE);
        keypad(winMsgArea_, TRUE);
    }

    return winMsgArea_;
}
void SharedWindowManager::backupWinFormScreenUnderMsgArea() {
    if (winFormScreen_ != nullptr && winMsgBackup_ == nullptr) {
        int stdscrRows, stdscrCols;
        getmaxyx(stdscr, stdscrRows, stdscrCols);

        // Backup the area that will be covered by winMsgArea_
        winMsgBackup_ = newwin(4, stdscrCols - 2, stdscrRows - 5, 1);
        overwrite(winFormScreen_, winMsgBackup_);
    }
}

WINDOW* SharedWindowManager::getwinScroll(int winHeight, int winWidth, int starty, int startx) {
    int stdscrRows = 0;
    int stdscrCols = 0;
    getmaxyx(stdscr, stdscrRows, stdscrCols);

    if (winScroll_ == nullptr) {
        winScroll_ = newwin(winHeight, winWidth, starty, startx);
        assert(winScroll_ != NULL);
        wattron(winScroll_, COLOR_PAIR(3));
        wbkgd(winScroll_, COLOR_PAIR(3));
        //box(winScroll_, ACS_VLINE, ACS_HLINE);
        keypad(winScroll_, TRUE);
    }

    return winScroll_;
}
//WINDOW* SharedWindowManager::getwinScroll(std::vector<ScreenProperties>& screenProps) {
//    /*    int stdscrRows = 0;
//        int stdscrCols = 0;
//        getmaxyx(stdscr, stdscrRows, stdscrCols);*/
//    int stdscrRows = screenProps[0].screenHeight;
//    int stdscrCols = screenProps[0].screenWidth;
//    if (!winScroll_ == nullptr) {
//        winScroll = newwin(stdscrRows, stdscrCols, 0, 0); // Create window with desired size and position
//        assert(winFormScreen_ != NULL);
//        wattron(winFormScreen_, COLOR_PAIR(3));
//        wbkgd(winFormScreen_, COLOR_PAIR(3));
//        box(winFormScreen_, ACS_VLINE, ACS_HLINE);
//        keypad(winFormScreen_, TRUE);
//    }
//
//    return winFormScreen_;
//}

void SharedWindowManager::destroywinFormScreen() {
    if (SharedWindowManager::winFormScreen_ != nullptr) {
        delwin(SharedWindowManager::winFormScreen_);
        SharedWindowManager::winFormScreen_ = nullptr;
    }
}
void SharedWindowManager::destroywinMsgArea() {
    if (SharedWindowManager::winMsgArea_ != nullptr) {
        delwin(SharedWindowManager::winMsgArea_);
        SharedWindowManager::winMsgArea_ = nullptr;
    }
}
void SharedWindowManager::destroywinScroll() {
    if (SharedWindowManager::winScroll_ != nullptr) {
        delwin(SharedWindowManager::winScroll_);
        SharedWindowManager::winScroll_ = nullptr;
    }
}

void  SharedWindowManager::hidewinFormScreen() {
    if (SharedWindowManager::winFormScreen_ != nullptr) {
        werase(SharedWindowManager::winFormScreen_);  // Erase the window's contents
        wrefresh(SharedWindowManager::winFormScreen_);  // Refresh the window to apply the changes
    }

}
void  SharedWindowManager::hidewinMsgArea() {
    if (SharedWindowManager::winMsgArea_ != nullptr) {
        werase(SharedWindowManager::winMsgArea_);  // Erase the window's contents
        wrefresh(SharedWindowManager::winMsgArea_);  // Refresh the window to apply the changes
    }
    // Restore the area from backup
    if (winMsgBackup_ != nullptr && winFormScreen_ != nullptr) {
        overwrite(winMsgBackup_, winFormScreen_);
        wrefresh(winFormScreen_);
        delwin(winMsgBackup_);
        winMsgBackup_ = nullptr;
    }
}   
void  SharedWindowManager::hidewinScroll() {
    if (SharedWindowManager::winScroll_ != nullptr) {
        werase(SharedWindowManager::winScroll_);  // Erase the window's contents
        wrefresh(SharedWindowManager::winScroll_);  // Refresh the window to apply the changes
    }
}

    

