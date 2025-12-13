// Version-02.20.25.16.07
//File: Dialog.h
// the function declarations in this program must match those in DialogUtils.h
// if you change  Dialog.h you must make the change in the program too DialogUtils.h
#pragma once
#ifndef DIALOG_ENTRY_H
#define DIALOG_ENTRY_H
#include "InputUtils.h"
//#include "DataEntry.h"
#include "DataEntryFieldProperties.h"
#include "DialogWindowManager.h"

// these are already in dataentry.h
//#include "ISAMWrapperLib.h"
//#include <string>
//#include <vector>
//#include <iostream>
//#include <fstream>
//#include <sstream>
//#include <filesystem>
//#include <cstring>
//#include <cstdlib>
//#include <cctype>
//#include <assert.h>
//#include <numeric>
//#include <unordered_map>
//#include <algorithm> // Include the header for std::max
//#include <tuple>
//#include <utility> // For std::pair
#include <functional>
#include <algorithm> // Include the header for std::max
#include <tuple>
#include <utility> // For std::pair
#include "CursesCompat.h"

using DataTuple = std::tuple<std::string, std::string>;
struct DialogFieldProperties {
    WINDOW* winFormScreen_;
    WINDOW* winDialog_;
    int Choice_;
    std::string name_;
    std::string type_;
    int row_;
    int col_;
    int labelCol_;
    std::string labelText_;
    std::string selectionIsEvent_;
    std::string nextRunType_;
    std::string nextRunClass_;
    std::string inputType_;
    std::string inputMask_;
    int inputLen_;
    int inputRow_;
    int inputCol_;
    int inputlabelCol_;
    std::string inputlabelText_;


    // Constructor to initialize all members
    DialogFieldProperties(WINDOW* winFormScreen = nullptr, WINDOW* winDialog = nullptr, int Choice = 0,
        const std::string& name = "", const std::string& type = "", int row = 0, int col = 0,
        int labelCol = 0, const std::string& labelText = "", const std::string& selectionIsEvent = "N",
        const std::string& nextRunType = "", const std::string& nextRunClass = "", const std::string& inputType = "",
        const std::string& inputMask = "", int inputLen = 0, int inputRow = 0, int inputCol = 0,
        int inputlabelCol = 0, const std::string& inputlabelText = "")
        : winFormScreen_(winFormScreen), winDialog_(winDialog), Choice_(Choice), name_(name), type_(type),
        row_(row), col_(col), labelCol_(labelCol), labelText_(labelText), selectionIsEvent_(selectionIsEvent),
        nextRunType_(nextRunType), nextRunClass_(nextRunClass), inputType_(inputType), inputMask_(inputMask),
        inputLen_(inputLen), inputRow_(inputRow), inputCol_(inputCol), inputlabelCol_(inputlabelCol),
        inputlabelText_(inputlabelText) {}


};

class Dialog {
    friend class DialogUtils;
    

private:
    std::vector<DialogFieldProperties> dialogFieldProperties;
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
     Dialog(std::vector<DialogFieldProperties>& dialogFieldProperties);
    ~Dialog();
    static WINDOW* winFormScreen_;
    static WINDOW* winDialog_;
    static WINDOW* winDialogShadow_;
    static WINDOW* winMsgArea_;
    static int stdscrRows_;
    static int stdscrCols_;
    static int len_;
    static std::string DialogType_;
    static std::string DialogTitle_;
    static std::string DialogBoxed_;
    static int DialogWidth_;
    static int DialogHeight_;
    static WINDOW* shadowWin_;    
    static int run(std::string& xmlFileName,std::ofstream& debugFile);
    static int setupDialog(std::vector<DialogFieldProperties>& dialogFieldProperties, std::ifstream& xmlFile, std::ofstream& debugFile);
    static std::string displayDialogAndGetChoice(std::vector<DialogFieldProperties>& dialogFieldProperties, std::ofstream& debugFile);
    static std::string removeLeadingSpaces(std::string& str, std::ofstream& debugFile);
    static std::string extractValue(const std::string& line);
    static void extractTagAndValue(const std::string& line, std::string& tag, std::string& value);
    static bool validateDialogSettingsXml(std::ifstream& xmlFile, std::ofstream& debugFile);
    static void displayLabels(const DialogFieldProperties& dialogFieldProperties, std::ofstream& debugFile);
    //typedef DataEntryFieldProperties TestType;
    static DataEntryFieldProperties createDataEntryFieldFromDialogField(const DialogFieldProperties& dialogFieldProperties);   
};
#endif
