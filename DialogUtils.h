// Version 01.20.25.18.24
// File: DialogUtils.h
// the function declarations in this program must match those in Dialog.h
// if you change  Dialog.h you must make the change in the program too
#pragma once
#include "Dialog.h"

class DialogUtils
{
public:    
    static int run(std::string& screenXmlFileName, std::ofstream& debugFile);
    static int setupDialog(std::vector<DialogFieldProperties>& DialogFieldProperties, std::ifstream& screenXmlFile, std::ofstream& debugFile);
    static std::string displayDialogAndGetChoice(std::vector<DialogFieldProperties>& DialogFieldProperties, std::ofstream& debugFile);
    static std::string removeLeadingSpaces(std::string& str, std::ofstream& debugFile);
    static std::string extractValue(const std::string& line);
    static void extractTagAndValue(const std::string& line, std::string& tag, std::string& value);
    static bool validateDialogSettingsXml(std::ifstream& screenXmlFile, std::ofstream& debugFile);

  };