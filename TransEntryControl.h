#pragma once
#ifndef RECORDLISTSCREEN_H
#define RECORDLISTSCREEN_H
#include <tuple>
#include "CursesCompat.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>// For debug logging
#include <sstream>
#include "DataEntry.h"
#include "SharedWindowManager.h"
#include "ISAMWrapperLib.h"

struct TransControlParmProperties {
    std::string parm;
    std::string fieldName;
    std::string value;
};

struct TransControlPassParams {
    std::map<std::string, TransControlParmProperties> controlFields;  // Use map for string key lookup
};

struct controlLabelProperties {
    int col;
    int row;
    std::string labelText;
    std::string labelFieldName;
    std::string labelParmName;
    int labelFieldCol;
    int labelFieldRow;
    std::string choiceValues;
    std::string choiceDescriptions;
    std::string labelFieldWHERE;
    std::string labelFieldSQL;
    controlLabelProperties() = default;
    // Constructor
    controlLabelProperties(int col, int row, std::string labelText, std::string labelFieldName,
        std::string labelParmName, int labelFieldCol, int labelFieldRow,
        std::string choiceValues, std::string choiceDescriptions, std::string labelFieldWHERE, std::string labelFieldSQL)
        : col(col), row(row), labelText(labelText), labelFieldName(labelFieldName),
        labelParmName(labelParmName), labelFieldCol(labelFieldCol), labelFieldRow(labelFieldRow),
        choiceValues(choiceValues), choiceDescriptions(choiceDescriptions),
        labelFieldWHERE(labelFieldWHERE), labelFieldSQL(labelFieldSQL) {}
};

//struct controlLabelProperties {
//    int col;
//    int row;
//    std::string labelText;
//    std::string labelFieldName;
//    std::string labelParmName;
//    int labelFieldCol;
//    int labelFieldRow;
//    std::string choiceValues;
//    std::string choiceDescriptions;
//    controlLabelProperties() = default;
//    // Constructor
//    controlLabelProperties(int col, int row, std::string labelText,std::string labelFieldName,
//        std::string labelParmName, int labelFieldCol, int labelFieldRow,
//        std::string choiceValues, std::string choiceDescriptions)
//        : col(col), row(row),labelText(labelText), labelFieldName(labelFieldName), 
//        labelParmName(labelParmName), labelFieldCol(labelFieldCol), labelFieldRow(labelFieldRow),
//        choiceValues(choiceValues), choiceDescriptions(choiceDescriptions){}
//};
struct controlFieldProperties {
    WINDOW* winFormScreen;
    WINDOW* winMsgArea;
    std::string fieldName;
    std::string fieldType;
    std::string fieldValue;
    std::string calc;
    std::string fieldMask;
    int len;
    int col;
    int row;
    std::string hidden;
    controlFieldProperties() = default;
    // Constructor
    controlFieldProperties(WINDOW* winFormScreen, WINDOW* winMsgArea,
        const std::string& fieldName, const std::string& fieldType, const std::string& fieldValue, const std::string& calc,
        const std::string& fieldMask, int len, int col, int row, const std::string& hidden)
        : winFormScreen(winFormScreen), winMsgArea(winMsgArea), fieldName(fieldName), fieldType(fieldType), fieldValue(fieldValue),
        calc(calc), fieldMask(fieldMask), len(len), col(col), row(row), hidden(hidden) {}
};

struct CommandKeyProperties {
    std::string keyValue;
    std::string nextRunType;
    std::string nextRunClass;
};

struct CommandLineProperties {
    std::string commandText;
    int row = 0;
    int col = 0;
    std::vector<CommandKeyProperties> commandKeys;
};


class TransEntryControl {
private:
    std::map<std::string, std::string> headerRecord_;
    WINDOW* _win;
    ISAMWrapperLib& _db;
    std::string _tableName;
    std::pair<std::vector<std::map<std::string, std::string>>, std::vector<std::string>> records_;
    std::vector<std::vector<std::string>> recordFieldValues_;
    std::vector<std::string> fieldNames_;
    std::string selectFields_;
    std::string runClass_;
    std::string _scrollKey;
    int _scrollMaxRows;
    size_t _currentTopIndex;
    size_t _selectedIndex;
    double trxTotal_ = 0.0;
    std::vector<BoxProperties> boxes_;  // Store boxes for redrawing when needed
    static std::string applyMask(const std::string& rawValue, const std::string& mask);
    void loadRecords(PassParams& transControlPassParams, std::vector<ScreenProperties>& screenProps, std::vector<controlFieldProperties>& controlFields, std::ofstream& debugFile);
    void displayRecords(std::vector<ScreenProperties>& screenProps, std::vector<controlFieldProperties>& controlFields, std::vector<LabelProperties>& controlLabels, WINDOW* winFormScreen, WINDOW* windScroll, CommandLineProperties& commandLine, std::ofstream& debugFile);
    void displayControlLabels(WINDOW* win, std::map<std::string, std::string> record, const std::vector<LabelProperties>& controlLabels, PassParams& transControlPassParams, std::vector<ScreenProperties>& screenProps, std::vector<controlFieldProperties>& controlFields, CommandLineProperties& commandLine, std::ofstream& debugFile);
    void updateRowCountLabel(std::vector<LabelProperties>& controlLabels, WINDOW* winFormScreen);
    void moveSelectionUpOne();
    void moveSelectionDownOne();
    void pageUp();
    void pageDown();
    void moveToTop();
    void moveToBottom();
    std::string handleInput(PassParams& transControlPassParams, std::vector<ScreenProperties>& screenProps, std::vector<LabelProperties>& labels, std::vector<controlFieldProperties>& controlFields, WINDOW* winFormScreen, WINDOW* winScroll, CommandLineProperties& commandLine, std::vector<DataEntryFieldProperties>& allFields, std::ofstream& debugFile);
    RunResult handleCommandKey(int key, PassParams& transControlPassParams, std::vector<ScreenProperties>& screenProps, const std::vector<LabelProperties>& labels, std::vector<controlFieldProperties>& controlFields, WINDOW* windScroll, CommandLineProperties& commandLine, std::vector<DataEntryFieldProperties>& allFields, std::ofstream& debugFile);

public:

    TransEntryControl(WINDOW& winFormScreen, ISAMWrapperLib& db, const std::string& tableName, int maxRows, const std::string& scrollKey);

    ~TransEntryControl();
    //control.run(headerRecord, screenProps,passControlParams, nextRunClass, controlxmlFile, ControlxmlFileName, controlFields, debugFile_)

    std::string run(std::map<std::string, std::string>& headerRecord, std::vector<ScreenProperties>& screenProps, PassParams& transControlPassParams, std::string& nextRunClass, std::ifstream& xmlFile, std::string& xmlFileName, std::vector<controlFieldProperties>& controlFields, std::ofstream& debugFile);

    int setupBoxes(std::vector<BoxProperties>& boxes, std::ifstream& xmlFile, std::string& xmlFileName, std::ofstream& debugFile);
    //bool setupControlLabels(std::vector<controlLabelProperties>& controlLabels, std::ifstream& xmlFile, std::ofstream& debugFile);
    //bool setupControlLabels(std::vector<controlLabelProperties>& controlLabels, std::ifstream& xmlFile, std::ofstream& debugFile);
    //bool setupControlLabels(std::vector<LabelProperties>& controlLabels, std::ifstream& xmlFile, std::ofstream& debugFile);
    //bool setupControlFields(std::vector<controlFieldProperties>& controlFields, std::ifstream& xmlFile, std::string& xmlFileName, std::ofstream& debugFile);
    int  setupControlFields(std::vector<controlFieldProperties>& controlFields, std::ifstream& xmlFile, std::string& xmlFileName, std::ofstream& debugFile);
    //static void displayLabelData(WINDOW* windScroll, controlLabelProperties& controlLabel, PassParams& transControlPassParams, std::map<std::string, std::string>* headerRecord, std::ofstream& debugFile);
    CommandLineProperties parseCommandLineXML(std::ifstream& xmlFile, std::ofstream& debugFile);
    static void copyHeaderFieldsToRecord(
        const std::vector<DataEntryFieldProperties>& lineFields,
        const std::map<std::string, std::string>& headerRecord,
        std::map<std::string, std::string>& record);
};

extern ISAMWrapperLib dummyDbLib;

#endif
