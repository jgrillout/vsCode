
#pragma once
// Version-02.20.25.16.07
// File: main.cpp
#include "TransactionEntry.h"
#include "DataEntry.h"
#include "MenuEntry.h"
#include "ISAMWrapperLib.h"
#include "SharedWindowManager.h"
#include "MenuWindowManager.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "sqlite3.h"
sqlite3* dummySqliteDb = nullptr;
ISAMWrapperLib dummyDbLib(dummySqliteDb);
// forward declrations
//bool Begin(PassParams& passParams,bool opendb, sqlite3*& db, std::ifstream& dictionaryXmlFile, const std::string& dictionaryXmlFileName, std::ifstream& screenXmlFile, std::string& nextRunClass, std::ofstream& debugFile);
bool Begin(PassParams& passParams, bool opendb, sqlite3*& db,std::ifstream& screenXmlFile, std::string& screenXmlFileName,std::string& nextRunClass, std::ofstream& debugFile);

//int startDataEntryProgram(int argc, char* argv[]);

int main(int argc, char* argv[]) {
    
    initscr();
    noecho();
    start_color();
    init_pair(2, COLOR_BLUE, COLOR_WHITE);
    init_pair(3, COLOR_WHITE, COLOR_BLUE);
    bkgd(COLOR_PAIR(3));

    std::string nextRunType = "";
    std::string nextRunClass = "";
    std::string exitRunType = "";
    std::string exitRunClass = "";
    std::string prevRunType = "";
    std::string prevRunClass = "";
    std::string screenXmlFileName = "";
    std::string dictionaryXmlFileName = "";
    
 
    std::map<std::string, std::string> header;
    PassParams passParams;
    if (argc < 3) {
        nextRunType = "Menu";
        nextRunClass = "MainMenuScreen";
        // std::cout << "No screen information provided." << std::endl;
        //return false;
    }
    else {
        nextRunType = argv[1];
        nextRunClass = argv[2];

    }

    std::vector<DataEntryFieldProperties> EntryFields_;
    std::ofstream debugFile("debug.txt");
    std::ifstream screenXmlFile;
    std::ifstream dictionaryXmlFile;
    sqlite3* db = nullptr;
 
    //ISAMWrapperLib dbLibInstance(db);
    //DataEntry::dbLib_ = dbLibInstance;

    bool rc = 0; 
    std::string result = "";
    bool opendb = false;
    
    
    while (!nextRunClass.empty()) {
        if (nextRunClass == "Exit")
            goto exitHere;
        //ISAMWrapperLib dbLib_(db);
        if (nextRunType == "Menu") {
            prevRunClass = nextRunClass;
            prevRunType = nextRunType;
            screenXmlFileName = nextRunClass +".xml";
            rc = Begin(passParams, opendb, db,screenXmlFile, screenXmlFileName, nextRunClass, debugFile);
            if (!rc) {
                return 1;
            }

            std::vector<MenuOptions> menus;
            MenuEntry MenuEntryScreen(menus);
            debugFile << "inside main () before   MenuEntryScreen.run on line 86" << std::endl;
            result = MenuEntryScreen.run(screenXmlFile, debugFile, nextRunType, nextRunClass);
            screenXmlFile.close();
            debugFile << "inside main () after  MenuEntryScreen.run on line 89 nextRunType= " << nextRunType << " nextRunClass= "<< nextRunClass << std::endl;
            if (result == "Error") {
                //TODO: MAKE ERROR DISPLAY
                nextRunClass = "";
            }
           
            else if (result == "Exit" && MenuEntry::exitRunType_ == "Menu")
            {
                debugFile << "inside main () after  MenuEntryScreen.run on line 97 MenuEntry::exitRunType_= "  << MenuEntry::exitRunType_ << " MenuEntry::exitRunClass= "<< MenuEntry::exitRunClass_ << std::endl;
                nextRunType = MenuEntry::exitRunType_;
                nextRunClass = MenuEntry::exitRunClass_;                
            }
            else if (result == "Exit" && MenuEntry::exitRunType_ != "Menu")
            {
                debugFile << "inside main () after  MenuEntryScreen.run on line 103 MenuEntry::exitRunType_= " << MenuEntry::exitRunType_ << " MenuEntry::exitRunClass= " << MenuEntry::exitRunClass_ << std::endl;
                nextRunType = MenuEntry::exitRunType_;
                nextRunClass = MenuEntry::exitRunClass_;
                goto exitHere;
            }
            else {
                screenXmlFile.close();

                //if (!nextRunClass.empty()) {                
                //    MenuWindowManager::hidewinMenu;
                //}
            }
        }
            else if (nextRunType == "DataEntry") {
            prevRunClass = nextRunClass;
            prevRunType = nextRunType;
            screenXmlFileName = nextRunClass + ".xml";
            //dictionaryXmlFileName = nextRunClass + "DataDictionary.xml";
                opendb = true;
                //rc = Begin(passParams, opendb, db, dictionaryXmlFile, dictionaryXmlFileName,screenXmlFile, screenXmlFileName, debugFile);
                rc = Begin(passParams, opendb, db,screenXmlFile, screenXmlFileName, nextRunClass, debugFile);
                if (!rc) {
                    return 1;
                }
                MenuWindowManager::destroywinMenu();                
                ISAMWrapperLib dbLibInstance(db);
                DataEntry::dbLib_ = dbLibInstance;
                
                if (true == true)
                    int debug = 0;

                DataEntry dataEntryScreen(EntryFields_, debugFile,db);
                //DataEntry dataEntryScreen(debugFile, db);
                RunResult runresult;
                debugFile << "inside main () before dataEntryScreen.run on line 138 screenXmlFileName = " << screenXmlFileName << std::endl;
                runresult = dataEntryScreen.run(passParams,screenXmlFile, screenXmlFileName, nextRunType, nextRunClass, debugFile);
                result = runresult.status;
                screenXmlFile.close();
                
                if (result == "Error") {
                    //TODO: MAKE ERROR DISPLAY
                    nextRunType = "";
                    nextRunClass = "";

                }
                else if (result == "Exit" && DataEntry::screenExitRunType_ =="Menu") {
                    debugFile << "inside main () after  dataEntryScreen.run on line 150 MenuEntry::exitRunType_= " << MenuEntry::exitRunType_ << " MenuEntry::exitRunClass= " << MenuEntry::exitRunClass_ << std::endl;
                    nextRunType = DataEntry::screenExitRunType_;
                    nextRunClass = DataEntry::screenExitRunClass_;
                    //goto exitHere;
                }
                else if (result == "Exit" && DataEntry::screenExitRunType_ != "Menu") {
                    debugFile << "inside main () after  dataEntryScreen.run on line 156 nextRunType = " << nextRunType << " nextRunClass " << nextRunClass << std::endl;
                    nextRunType = "";
                    nextRunClass = "";
                    goto exitHere;
                }
                else if (result != "OK" && !result.empty()) {
                    //TODO: not sure if this would happen
                    nextRunType = "Menu";
                    nextRunClass = "MainMenuScreen";
                }

                else if (nextRunClass.empty()) {
                    nextRunType = "Menu";
                    nextRunClass = "MainMenuScreen";
                }
                // TODO: find way so this is not hard-coded
                else if (result == "Menu") {
                    nextRunType = "Menu";
                    nextRunClass = "MainMenuScreen";
                }
                else if (result.empty()) {
                    nextRunType = prevRunType;
                    nextRunClass = prevRunClass;
                }
                
            }
        
            else if (nextRunType == "TransactionEntry") {
                prevRunClass = nextRunClass;
                prevRunType = nextRunType;
                screenXmlFileName = nextRunClass + ".xml";
                opendb = true;

                rc = Begin(passParams, opendb, db, screenXmlFile, screenXmlFileName,nextRunClass,debugFile);
                if (!rc) {
                    return 1;
                }
                MenuWindowManager::destroywinMenu();
                std::vector<DataEntryFieldProperties> EntryFields_;
                ISAMWrapperLib dbLibInstance(db);
                DataEntry::dbLib_ = dbLibInstance;    
                if (true == true)
                    int debug = 0;

                TransactionEntry TransactionEntry(EntryFields_, debugFile, db);
                
                RunResult runresult;
                debugFile << "inside main () before TransactionEntry.run on line 203 screenXmlFileName = " << screenXmlFileName << std::endl;
                runresult = TransactionEntry.run(header, passParams,screenXmlFile, screenXmlFileName, nextRunType, nextRunClass, debugFile);
                result = runresult.status;
                screenXmlFile.close();
                if (result == "Error") {
                    //TODO: MAKE ERROR DISPLAY
                    nextRunType = "";
                    nextRunClass = "";
                }
            if (result == "Exit" && nextRunType == "Menu") {
                debugFile << "inside main () after  TransactionEntry.run on line 213 MenuEntry::exitRunType_= " << MenuEntry::exitRunType_ << " MenuEntry::exitRunClass= " << MenuEntry::exitRunClass_ << std::endl;
                nextRunType = MenuEntry::exitRunType_;
                nextRunClass = MenuEntry::exitRunClass_;
            }    
            else if (result == "Exit" && nextRunType != "Menu") {
                debugFile << "inside main () after  TransactionEntry.run on line 218 nextRunType= " << nextRunType << " nextRunClass= " << nextRunClass << std::endl;
                //nextRunType = "";
                //nextRunClass = "";
            }
            
      
                else if (result != "OK" && !result.empty()) {
                    //TODO: not sure if this would happen
                    nextRunType = "Menu";
                    nextRunClass = "MainMenuScreen";
                }
               
                else if (nextRunClass.empty()) {
                    nextRunType = "Menu";
                    nextRunClass = "MainMenuScreen";
                }
                // TODO: find way so this is not hard-coded
                else if (result == "Menu") {
                    nextRunType = "Menu";
                    nextRunClass = "MainMenuScreen";
                }
                else if (result.empty()) {
                    nextRunType = prevRunType;
                    nextRunClass = prevRunClass;
                }
    

            }
        }
    
exitHere:
    screenXmlFile.close();
    if (db != nullptr) {
        sqlite3_close(db);  // Properly close the SQLite database
        db = nullptr;       // Optional: set pointer to nullptr after closing
    }
    debugFile.close();
    SharedWindowManager::destroywinFormScreen();
    SharedWindowManager::destroywinMsgArea();
    MenuWindowManager::destroywinMenu();

    endwin(); // End curses mode

    return 0;
}

//bool Begin(PassParams& passParams, bool opendb, sqlite3*& db, std::ifstream& dictionaryXmlFile, const std::string& dictionaryXmlFileName, std::ifstream& screenXmlFile, std::string& nextRunClass, std::ofstream& debugFile)
bool Begin(PassParams& passParams, bool opendb, sqlite3*& db, std::ifstream& screenXmlFile, std::string& screenXmlFileName, std::string& nextRunClass, std::ofstream& debugFile)
{
    std::string slash = "";
#ifdef _WIN32
    slash = "\\";
#else
    slash = "/";
#endif
    if (screenXmlFile.is_open()) {
        screenXmlFile.close();
    }
    

    std::string dbname = "";
    namespace fs = std::filesystem;
    fs::path currentPath = fs::current_path();
    
    screenXmlFileName = currentPath.string();
    screenXmlFileName.append(slash + nextRunClass+".xml");
     
    if (screenXmlFile.is_open()) {
        debugFile << screenXmlFileName + " file. is already open" << std::endl;
        return false;
    }
    screenXmlFile.open(screenXmlFileName);
    
    if (!screenXmlFile.is_open()) {
        debugFile << "Failed to open " + screenXmlFileName + " file." << std::endl;
        return false;
    }
    
    passParams = DataEntry::parseParamXML(screenXmlFile, screenXmlFileName, debugFile);

    if (opendb) {
        db = nullptr;
        dbname = currentPath.string() + slash + "Company.db";
        int rc = sqlite3_open(dbname.c_str(), &db);
        if (rc) {
            debugFile << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }
        const char* dbPath = sqlite3_db_filename(db, "main");
        debugFile << "dbPath: " << dbPath << std::endl;


    }
    return true;
}