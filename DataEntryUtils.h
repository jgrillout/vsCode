// Version-02.20.25.16.07
// File: DataEntryUtils.h
// the function declarations in this program must match those in DataEntry.h
// if you change  Dialog.h you must make the change in the program too
#pragma once
#ifndef DATA_ENTRY_UTILS_H
#define DATA_ENTRY_UTILS_H

#include "DataEntry.h"
#include "DataEntryFieldProperties.h"

class DataEntryUtils {
public:
    //static bool acceptInput(DataEntryFieldProperties& field, PassParams& passParams, std::ofstream& debugFile);
    static bool acceptInput(DataEntryFieldProperties& EntryField, PassParams& passParams, std::ofstream& debugFile);
    static bool isLeapYear(int year);
    static bool isValidDate(const std::string& date, std::ofstream& debugFile);
    static void parseCSVWithQuotes(std::string& line, std::vector<std::string>& tokens);
    static void debugMsg(std::string string1, std::string string2, int val);
    static bool errMsg(int row, int col, std::string& msg, std::ofstream& debugFile);
    static void printInMiddle(WINDOW* win, int startRow, int startCol, int width, std::string& msg, chtype color, std::ofstream& debugFile);
    static void findMiddle(WINDOW* win, int startRow, int startCol, int& outRow, int& outCol, int width, std::string msg);
    static std::string askQuestion(WINDOW* win, int startRow, int startCol, int width, std::string prompt, chtype color);
    static bool confirmAction(int startRow, int startCol, int width, std::string prompt, chtype color, int keyToPress, std::ofstream& debugFile);
    static std::string doFunctionKeys(DataEntryFieldProperties& field, const std::string& tbl, bool addingNew, ISAMWrapperLib& lib, std::string& condition, std::string& inquiryOrderBy, std::string& inquiryFields, std::vector<DataEntryFieldProperties>& EntryFields_, std::ofstream& debugFile, const std::string& inputAction);
    static std::string readRecord(std::vector<DataEntryFieldProperties>& DataEntryFields, std::string tbl, bool addingNew, bool prompt, ISAMWrapperLib& lib, const std::string condition, std::vector<DataEntryFieldProperties>& EntryFields_, PassParams& passParams, std::ofstream& debugFile);
    static int stringSwitchNumber(const std::string& key);
    static bool exportTableToCSV(sqlite3* db, const std::string& screenXmlFileName, const std::string& fileName);
    static std::vector<std::pair<std::string, std::string>> transformVector(const std::vector<DataTuple>& inputVector);
    static std::string displayLookupWindow(WINDOW* mainWindow, int lookupHeight, int lookupWidth, int lookupStartY, int lookupStartX, std::vector<std::tuple<std::string, std::string>>& data, const std::string& inquiryFields, std::string condition, std::ofstream& debugFile);
    static void displayRightToLeft(WINDOW* win, const std::string& input, int row, int col, int inputSize, int calledFrom, std::ofstream& debugFile);
    static void hideWindow(DataEntryFieldProperties& field);
    static void displayData(DataEntryFieldProperties& field, std::ofstream& debugFile);
    //static void displayLabels(LabelProperties& label, std::ofstream& debugFile);
    //static void displayLabels(DataEntryFieldProperties& field, std::ofstream& debugFile);
    static KeyType checkSpecialKeys(int ch);
    static void displayMask(DataEntryFieldProperties& field, std::string& input, std::ofstream& debugFile);
    static void inspectMask(const std::string& format, size_t& leftSize, size_t& rightSize, std::ofstream& debugFile);
    static std::string combineInputWithMask(const std::string& mask, const std::string& input, std::ofstream& debugFile);
    static std::string generateDisplayMask(const std::string& mask);
    static std::string removeMask(const std::string& maskedInput, std::ofstream& debugFile);
    static std::string RemoveCharacters(std::string input);
    static std::string rightJustifyString(const std::string& str, int len, std::ofstream& debugFile);
    static std::string removeLeadingSpaces(std::string& str, std::ofstream& debugFile);
};

#endif
