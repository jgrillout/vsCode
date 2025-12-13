// Version-02.20.25.16.07
// File: MenuEntry.h
// the function declarations in this program must match those in MenuEntryUtils.h
// if you change  MenuEntry.h you must make the change in the program too MenuEntryUtils.h
#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <assert.h>
#include <numeric>
#include <unordered_map>
#include <algorithm> // Include the header for std::max
#include <tuple>
#include <utility> // For std::pair
#include "InputUtils.h"
#include "DataEntry.h"
#include "MenuWindowManager.h"

#include "CursesCompat.h"

using DataTuple = std::tuple<std::string, std::string>;

struct MenuOptions {
    WINDOW* winFormScreen;
    WINDOW* winMenu;
    int Choice;
    std::string name;
    std::string type;
    int row;
    int col;
    int labelCol;
    std::string labelText;
    std::string selectionIsEvent;
    std::string nextRunType;
    std::string nextRunClass;
    std::string inputType;
    std::string inputMask;
    int inputLen;
    int inputRow;
    int inputCol;
    int inputlabelCol;
    std::string inputlabelText;

    // Constructor to initialize all members
    MenuOptions(WINDOW* winFormScreen = nullptr, WINDOW* winMenu = nullptr, int Choice = 0,
        const std::string& name = "", const std::string& type = "", int row = 0, int col = 0,
        int labelCol = 0, const std::string& labelText = "", const std::string& selectionIsEvent = "N",
        const std::string& nextRunType = "", const std::string& nextRunClass = "", const std::string& inputType = "",
        const std::string& inputMask = "", int inputLen = 0, int inputRow = 0, int inputCol = 0,
        int inputlabelCol = 0, const std::string& inputlabelText = "")
        : winFormScreen(winFormScreen), winMenu(winMenu), Choice(Choice), name(name), type(type),
        row(row), col(col), labelCol(labelCol), labelText(labelText), selectionIsEvent(selectionIsEvent),
        nextRunType(nextRunType), nextRunClass(nextRunClass), inputType(inputType), inputMask(inputMask),
        inputLen(inputLen), inputRow(inputRow), inputCol(inputCol), inputlabelCol(inputlabelCol),
        inputlabelText(inputlabelText) {}
};

class MenuEntry {
    friend class MenuEntryUtils;

private:
    std::vector<MenuOptions> menuOptions;
    std::vector<MenuOptions> menus_;

    static int Choice_;
    static std::string name_;
    static std::string type_;
    static int row_;
    static int col_;
    static int labelCol_;
    static std::string labelText_;
    static std::string selectionIsEvent_;
    static std::string nextRunType_;
    static std::string nextRunClass_;
    static std::string inputType_;
    static std::string inputMask_;
    static int inputLen_;
    static int inputRow_;
    static int inputlabelCol_;
    static std::string inputlabelText_;

public:
    MenuEntry(std::vector<MenuOptions>& menus);
    ~MenuEntry();

    static WINDOW* winMenu_;
    static WINDOW* winMenuShadow_;
    static int stdscrRows_;
    static int stdscrCols_;
    static int len_;
    static std::string menuType_;
    static std::string menuTitle_;
    static std::string menuBordered_;
    static std::string exitRunType_;
    static std::string exitRunClass_;
    static int menuWidth_;
    static int menuHeight_;
    static WINDOW* shadowWin_;

    static std::string run(std::ifstream& screenXmlFile, std::ofstream& debugFile, std::string& nextRunType, std::string& nextRunClass);
    static bool validateMenuChoicesXml(std::ifstream& screenXmlFile, std::ofstream& debugFile);
    static int setupMenu(std::vector<MenuOptions>& menus, std::ifstream& screenXmlFile, std::ofstream& debugFile);
    static int displayMenuAndGetChoice(std::vector<MenuOptions>& menuOptions, std::ofstream& debugFile);
    static std::string removeLeadingSpaces(std::string& str, std::ofstream& debugFile);
    static std::string extractValue(const std::string& line);
    static void extractTagAndValue(const std::string& line, std::string& tag, std::string& value);
    static bool validateMenuSettingsXml(std::ifstream& screenXmlFile, std::ofstream& debugFile);
    //static void displayLabels(MenuOptions& menuOptions, std::ofstream& debugFile);
};

