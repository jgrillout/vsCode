// Version-02.20.25.16.07
// File:DialogWindowManager.cpp
#pragma once
#include "DialogWindowManager.h"
 WINDOW* DialogWindowManager::winDialog_ = nullptr;
 WINDOW* DialogWindowManager::winDialogShadow_ = nullptr;

 WINDOW* DialogWindowManager::getwinDialog(int DialogHeight, int DialogWidth) {
     if (winDialog_ == nullptr) {
         winDialog_ = newwin(DialogHeight, DialogWidth, (LINES - DialogHeight) / 2, (COLS - DialogWidth) / 2);
         assert(winDialog_ != NULL);
         wattron(winDialog_, COLOR_PAIR(3));
         wbkgd(winDialog_, COLOR_PAIR(3));
         box(winDialog_, ACS_VLINE, ACS_HLINE);
         keypad(winDialog_, TRUE);
         wrefresh(winDialog_);
         return winDialog_;
     }
     else
         return winDialog_;
 }

 WINDOW* DialogWindowManager::createShadowForWindow(WINDOW* win, int winHeight, int winWidth, int startY, int startX) {

     init_pair(4, COLOR_BLACK, COLOR_BLACK); // Shadow color
      if   (DialogWindowManager::winDialogShadow_ == nullptr) {
         // Create the shadow window slightly offset from the main window
          DialogWindowManager::winDialogShadow_ = newwin(winHeight, winWidth, startY + 1, startX + 2);
        wattron(DialogWindowManager::winDialogShadow_, COLOR_PAIR(4));
        wbkgd(DialogWindowManager::winDialogShadow_, COLOR_PAIR(4));
     }
     //Refresh the main window to ensure it's on top of the shadow
     wattron(winDialog_, COLOR_PAIR(3));  // Assuming winDialog uses COLOR_PAIR(3)
     return DialogWindowManager::winDialogShadow_;
 }

 void DialogWindowManager::destroywinDialogShadow() {
     if (DialogWindowManager::winDialogShadow_ != nullptr) {
         delwin(DialogWindowManager::winDialogShadow_);
         DialogWindowManager::winDialogShadow_ = nullptr;
     }
 }
 
 void DialogWindowManager::destroywinDialog() {
     if (DialogWindowManager::winDialog_ != nullptr) {
         delwin(DialogWindowManager::winDialog_);
         DialogWindowManager::winDialog_ = nullptr;
     }
 }
 void DialogWindowManager::hidewinDialog() {
     if (DialogWindowManager::winDialog_ != nullptr) {
         werase(DialogWindowManager::winDialog_);  // Erase the window's contents
         wrefresh(DialogWindowManager::winDialog_);  // Refresh the window to apply the changes
     }
     if (DialogWindowManager::winDialogShadow_ != nullptr) {
         werase(DialogWindowManager::winDialogShadow_);  // Erase the window's contents
         wrefresh(DialogWindowManager::winDialogShadow_);  // Refresh the window to apply the changes
     }
 }