// Version-02.20.25.16.07
// File:MenuWindowManager.cpp
#pragma once
#include "MenuWindowManager.h"
 WINDOW* MenuWindowManager::winMenu_ = nullptr;
 WINDOW* MenuWindowManager::winMenuShadow_ = nullptr;

 WINDOW* MenuWindowManager::getwinMenu(int menuHeight, int menuWidth) {
     if (winMenu_ == nullptr) {
         winMenu_ = newwin(menuHeight, menuWidth, (LINES - menuHeight) / 2, (COLS - menuWidth) / 2);
         assert(winMenu_ != NULL);
         wattron(winMenu_, COLOR_PAIR(3));
         wbkgd(winMenu_, COLOR_PAIR(3));
         box(winMenu_, ACS_VLINE, ACS_HLINE);
         keypad(winMenu_, TRUE);
         wrefresh(winMenu_);
         return winMenu_;
     }
     else
         return winMenu_;
 }

 WINDOW* MenuWindowManager::createShadowForWindow(WINDOW* win, int winHeight, int winWidth, int startY, int startX) {

     init_pair(4, COLOR_BLACK, COLOR_BLACK); // Shadow color
      if   (MenuWindowManager::winMenuShadow_ == nullptr) {
         // Create the shadow window slightly offset from the main window
          MenuWindowManager::winMenuShadow_ = newwin(winHeight, winWidth, startY + 1, startX + 2);
        wattron(MenuWindowManager::winMenuShadow_, COLOR_PAIR(4));
        wbkgd(MenuWindowManager::winMenuShadow_, COLOR_PAIR(4));
     }
     //Refresh the main window to ensure it's on top of the shadow
     wattron(winMenu_, COLOR_PAIR(3));  // Assuming winMenu uses COLOR_PAIR(3)
     return MenuWindowManager::winMenuShadow_;
 }

 void MenuWindowManager::destroywinMenuShadow() {
     if (MenuWindowManager::winMenuShadow_ != nullptr) {
         delwin(MenuWindowManager::winMenuShadow_);
         MenuWindowManager::winMenuShadow_ = nullptr;
     }
 }
 
 void MenuWindowManager::destroywinMenu() {
     if (MenuWindowManager::winMenu_ != nullptr) {
         delwin(MenuWindowManager::winMenu_);
         MenuWindowManager::winMenu_ = nullptr;
     }
 }
 void MenuWindowManager::hidewinMenu() {
     if (MenuWindowManager::winMenu_ != nullptr) {
         werase(MenuWindowManager::winMenu_);  // Erase the window's contents
         wrefresh(MenuWindowManager::winMenu_);  // Refresh the window to apply the changes
     }
     if (MenuWindowManager::winMenuShadow_ != nullptr) {
         werase(MenuWindowManager::winMenuShadow_);  // Erase the window's contents
         wrefresh(MenuWindowManager::winMenuShadow_);  // Refresh the window to apply the changes
     }
 }