// Version-02.20.25.16.07
//File: DataEntry.h
// the function declarations in this program must match those in DataEntryUtils.h
// if you change  DataEntry.h you must make the change in the program too DataEntryUtils.h
#pragma once
#ifndef DATA_ENTRY_H
#define DATA_ENTRY_H
#include "CursesCompat.h"
#include "ISAMWrapperLib.h"
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
#include <functional>
#include <algorithm> // Include the header for std::max
#include <tuple>
#include <utility> // For std::pair
#include "Dialog.h"
#include "DataEntryFieldProperties.h"
//#include "PassParam.h"
#include <map>
#include <regex>


using DataTuple = std::tuple<std::string, std::string>;

//enum KeyType {
//    FUNCTION_KEY,
//    UP_ARROW,
//    ENTER_KEY,
//    ALPHA_NUM_KEY,
//    OTHER_KEY
//};
enum class KeyType {
    FUNCTION_KEY,
    UP_ARROW,
    ENTER_KEY,
    ALPHA_NUM_KEY,
    OTHER_KEY
};

struct parmProperties {
    std::string parm;
    std::string fieldName;
    std::string value;
};

struct PassParams {
    std::map<std::string, parmProperties> fields;  // Use map for string key lookup
};

struct ScreenProperties {
    std::string screenType_;
    std::string screenTitle_;
    int screencol_;
    int screenrow_;
    int screenWidth_;
    int screenHeight_;
    std::string screenDialog_;
    std::string screenBorder_;
    std::string screenNextType_;
    std::string screenNextClass_;
    std::string screenExitRunType_;
    std::string screenExitRunClass_;
    std::string hasControl_;
    std::string controlClass_;
    std::string scrollTable_;
    std::string scrollKey_;
    int scrollMaxRows_;
    int scrollWidth_;
    int scrollHeight_;
    int scrollStartY_;
    int scrollStartX_;
    std::string calc_;

    // Constructor
    ScreenProperties(std::string screenType, std::string screenTitle, int screencol, int screenrow, int screenWidth, int screenHeight,
        std::string screenBorder, std::string screenDialog, std::string screenNextType, std::string screenNextClass, std::string screenExitRunType, std::string screenExitRunClass, std::string hasControl, std::string controlClass,
        std::string scrollTable, int scrollMaxRows, std::string scrollKey,
        int scrollWidth, int scrollHeight, int scrollStartY, int scrollStartX)
        : screenType_(screenType), screenTitle_(screenTitle), screencol_(screencol), screenrow_(screenrow), screenWidth_(screenWidth), screenHeight_(screenHeight),
        screenBorder_(screenBorder), screenDialog_(screenDialog),screenNextType_(screenNextType),screenNextClass_(screenNextClass),
        screenExitRunType_(screenExitRunType), screenExitRunClass_(screenExitRunClass),
        hasControl_(hasControl), controlClass_(controlClass),
        scrollTable_(scrollTable), scrollKey_(scrollKey), scrollMaxRows_(scrollMaxRows),
        scrollWidth_(scrollWidth), scrollHeight_(scrollHeight), scrollStartY_(scrollStartY), scrollStartX_(scrollStartX) {}
};
struct BoxProperties {
    int col_;
    int row_;
    int cols_;
    int rows_;

    // Constructor
    BoxProperties(int col, int row, int cols, int rows)
        : col_(col), row_(row), cols_(cols), rows_(rows) {}
};
struct DataDictionaryFieldProperties {
    std::string fieldName;
    std::string fieldType;
    std::string primaryKey;
    std::string fieldMask;
    int length;

    // Default constructor
    DataDictionaryFieldProperties() = default;

    // Parameterized constructor
    DataDictionaryFieldProperties(const std::string& name,
        const std::string& type,
        const std::string& primaryKey,
        const std::string& mask,
        int length)
        : fieldName(name), fieldType(type), primaryKey(primaryKey),
        fieldMask(mask), length(length) {}
};

struct LabelProperties {
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
    LabelProperties() = default;
    // Constructor
    LabelProperties(int col, int row, std::string labelText, std::string labelFieldName,
        std::string labelParmName, int labelFieldCol, int labelFieldRow,
        std::string choiceValues, std::string choiceDescriptions, std::string labelFieldWHERE, std::string labelFieldSQL)
        : col(col), row(row), labelText(labelText), labelFieldName(labelFieldName),
        labelParmName(labelParmName), labelFieldCol(labelFieldCol), labelFieldRow(labelFieldRow),
        choiceValues(choiceValues), choiceDescriptions(choiceDescriptions), 
        labelFieldWHERE(labelFieldWHERE), labelFieldSQL(labelFieldSQL){}
};
struct RunResult {
    std::string status;      // "OK", "CANCEL", "error", etc.
    PassParams values;       // user-entered values
    std::string nextRunClass;
    std::string nextRunType;
};

class DataEntry {
    friend class DataEntryUtils;
    friend class TransactionEntryHeader;

private:
    std::ofstream& debugFile_;
    //std::vector<DataEntryFieldProperties>& EntryFields_;
    std::map<std::string, std::string> headerRecord_;
    sqlite3* db_;
    //ISAMWrapperLib& db_;
    static int stdscrRows_;
    static int stdscrCols_;
    static std::string runClass_;
    static std::string scrollKey_;
    std::string tableName_;
    bool hasEditRecord_ = false;
    std::map<std::string, std::string> editRecord_;



public:
    DataEntry(std::vector<DataEntryFieldProperties>& EntryFields_, std::ofstream& debugFile, sqlite3* db);
    //DataEntry(std::ofstream& debugFile, sqlite3* db);
    ~DataEntry();
    const std::map<std::string, std::string>& getHeaderRecord() const;
    static ISAMWrapperLib& dbLib_;
    static const int STRING = 0;
    static const int NUMBER = 1;
    static const int MASKED = 2;
    std::map<std::string, std::string> headerRecord;
    static std::string screenExitRunType_;
    static std::string screenExitRunClass_;



    RunResult run(const PassParams& inputParams,  std::ifstream& screenXmlFile, const std::string& screenXmlFileName, const std::string& nextRunType, const std::string& nextRunClass, std::ofstream& debugFile);
    static std::string getTableName(std::ifstream& screenXmlFile, std::string screenXmlFileName, std::ofstream& debugFile);
    //static bool validateDataEntryXml(std::ifstream& dictionaryXmlFile, const std::string& dictionaryXmlFileName, std::ifstream& screenXmlFile, const std::string& screenXmlFileName, std::ofstream& debugFile);
    //static bool validateScreenXml(std::ifstream& screenXmlFile, const std::string& screenXmlFileName, std::ofstream& debugFile);
    static int setupScreen(std::vector<ScreenProperties>& screenProps, std::ifstream& screenXmlFile, const std::string& screenXmlFileName, std::ofstream& debugFile);
    static int setupBoxes(std::vector<BoxProperties>& boxes, std::ifstream& screenXmlFile, const std::string& screenXmlFileName, std::ofstream& debugFile);
    static bool setupLabels(std::vector<LabelProperties>& labels, std::ifstream& screenXmlFile, const std::string& screenXmlFileName, std::ofstream& debugFile);   
    static int setupFields(std::vector<DataEntryFieldProperties>& DataEntryFields, std::ifstream& localDictionaryXmlFile, const std::string& localDictionaryXmlFileName, std::ifstream& localScreenXmlFile, const std::string& localScreenXmlFileName, std::ofstream& debugFile);
    static int setupDataEntryWindow(std::vector<ScreenProperties>& screenProps, std::ofstream& debugFile);
    std::string collectData(PassParams& passParams, std::vector<ScreenProperties>& screenProps, std::vector<BoxProperties>& boxes, std::vector<LabelProperties>& labels, std::vector<DataEntryFieldProperties>& DataEntryFields, std::ofstream& debugFile);
    static void drawBoxes(std::vector<BoxProperties>& boxes);
    static bool acceptInput(DataEntryFieldProperties& EntryField, PassParams& passParams, std::ofstream& debugFile);
    static std::vector<chtype> readAfterLineWithAttributes(WINDOW* win, int row, int startCol);
    static void restoreAfterLineWithAttributes(WINDOW* win, int row, int startCol, const std::vector<chtype>& lineContent);
    static void displayRightToLeft(WINDOW* win, const std::string& input, int row, int col, int inputSize, int calledFrom, std::ofstream& debugFile);
    static void displayData(DataEntryFieldProperties& EntryField, std::ofstream& debugFile);
    //static void displayLabels(WINDOW* win, std::map<std::string, std::string>& record, const LabelProperties& L,PassParams& PassParams,std::ofstream& dbg);
      static void displayLabels(WINDOW* win, const std::map<std::string, std::string> record, const LabelProperties& L, PassParams& PassParams, std::ofstream& dbg);

    static int displayScreen(std::ifstream& screenXmlFile, const std::string& screenXmlFileName, std::vector<ScreenProperties>& screenProps, std::vector<BoxProperties>& boxes, std::vector<LabelProperties>& labels, std::vector<DataEntryFieldProperties>& EntryFields_, std::ofstream& debugFile);
    static std::vector<std::string> splitCSV(const std::string& s);
    static std::string choiceDescriptionFor(const std::string& value, const std::string& choiceValuesCSV, const std::string& choiceDescriptionsCSV);
    static std::string cleanString(const std::string& input);
    static bool isValidDate(const std::string& date, std::ofstream& debugFile);
    static bool isLeapYear(int year);
    static std::string rightJustifyString(const std::string& str, int len, std::ofstream& debugFile);
    static std::string displayLookupWindow(WINDOW* mainWindow, int lookupHeight, int lookupWidth, int lookupStartY, int lookupStartX, std::vector<std::tuple<std::string, std::string>>& data, const std::string& inquiryFields, std::string condition, std::ofstream& debugFile);
    std::string doFunctionKeys(DataEntryFieldProperties& EntryField, const std::string& tbl, bool addingNew, ISAMWrapperLib& lib, std::string& condition, std::string& inquiryOrderBy, std::string& inquiryFields, std::vector<DataEntryFieldProperties>& EntryFields_, std::ofstream& debugFile, const std::string& inputAction);
    //static std::string readRecord(DataEntryFieldProperties& EntryField, std::string tbl, bool addingNew, bool prompt, ISAMWrapperLib& lib, const std::string condition, std::vector<DataEntryFieldProperties>& EntryFields_, PassParams& passParams, std::ofstream& debugFile);
    //std::string readRecord(std::vector<DataEntryFieldProperties>& DataEntryFields, std::string tbl, bool addingNew, bool prompt, ISAMWrapperLib& lib, const std::string condition, PassParams& passParams, std::ofstream& debugFile);
    static std::string readRecord(std::vector<DataEntryFieldProperties>& DataEntryFields,std::string tbl, bool addingNew, bool prompt,ISAMWrapperLib& lib, const std::string condition,PassParams& passParams, std::ofstream& debugFile, std::map<std::string, std::string>* outRow = nullptr);

    static void displayFkeys();
    static std::string askQuestion(WINDOW* win, int startRow, int startCol, int width, std::string prompt, chtype color);
    static bool confirmAction(int startRow, int startCol, int width, std::string prompt, chtype color, int keyToPress, std::ofstream& debugFile);
    static void findMiddle(WINDOW* win, int startRow, int startCol, int& outRow, int& outCol, int width, std::string msg);
    static void printInMiddle(WINDOW* win, int startRow, int startCol, int width, std::string& msg, chtype color, std::ofstream& debugFile);
    static void debugMsg(std::string string1, std::string string2, int val);
    static bool errMsg(int row, int col, std::string& action,std::string& msg, std::ofstream& debugFile);
    static int stringSwitchNumber(const std::string& key);
    static KeyType checkSpecialKeys(int ch);
    static PassParams parseParamXML(std::ifstream& screenXmlFile, std::string& screenXmlFileName, std::ofstream& debugFile);
    static std::map<std::string, std::string> extractAttributes(const std::string& line);
    static std::vector<std::pair<std::string, std::string>> transformVector(const std::vector<DataTuple>& inputVector);
    static std::string evaluateCalcFormula(const std::string& formula, const std::map<std::string, std::string>& record);
    static double evaluateSimpleMath(const std::string& expression);
    int getNextNumber(const std::string& screenXmlFileName, std::ofstream& debugFile);
    void initializeNumberCounter(const std::string& screenXmlFileName);
    void createNumberTable(const std::string& screenXmlFileName);
    int setupDataDictionaryFields(
        std::vector<DataDictionaryFieldProperties>& dictionaryFields,
        std::ifstream& screenXmlFile,
        const std::string& screenXmlFileName,
        std::ofstream& debugFile);
    //bool setupDataEntryFields(
    //    std::vector<DataEntryFieldProperties>& DataEntryFields,
    //    std::ifstream& screenXmlFile,
    //    const std::string& screenXmlFileName,
    //    std::ofstream& debugFile);
    void mergeDataDictionaryWithDataEntryFields(
        std::vector<DataEntryFieldProperties>& DataEntryFields,
        const std::vector<DataDictionaryFieldProperties>& dictionaryFields,
        std::ofstream& debugFile);
    static int parseInteger(const std::string& value, int defaultValue);
    void setEditRecord(const std::map<std::string, std::string>& rec);

};

extern ISAMWrapperLib dummyDbLib;

#endif
