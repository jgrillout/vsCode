// Version-02.20.25.16.07
// File: MenuEntryUtils.h
// the function declarations in this program must match those in MenuEntry.h
// if you change  MenuEntry.h you must make the change in the program too
#pragma once
#include "MenuEntry.h"

class MenuEntryUtils {
public:
    static std::string run(std::ifstream& screenXmlFile, std::ofstream& debugFile, std::string& nextRunType, std::string& nextRunClass);
    static bool validateMenuChoicesXml(std::ifstream& screenXmlFile, std::ofstream& debugFile);
    static int setupMenu(std::vector<MenuOptions>& menus, std::ifstream& screenXmlFile, std::ofstream& debugFile);
    //static int displayMenuAndGetChoice(std::vector<MenuOptions>& menuOptions, std::ofstream& debugFile);
    static std::string removeLeadingSpaces(std::string& str, std::ofstream& debugFile);
    static std::string extractValue(const std::string& line);
    static void extractTagAndValue(const std::string& line, std::string& tag, std::string& value);
    static bool validateMenuSettingsXml(std::ifstream& screenXmlFile, std::ofstream& debugFile);
};
