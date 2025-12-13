//Version-02.20.25.16.07
//File: TransactionEntryHeader.h

#ifndef TRANSACTION_ENTRY_HEADER_H
#define TRANSACTION_ENTRY_HEADER_H

#include <vector>
#include <string>
#include <fstream>
#include "DataEntry.h"
#include "SharedWindowManager.h"
#include "ISAMWrapperLib.h"
#include "Dialog.h"
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <map>

class TransactionEntryHeader {
    friend class DataEntryUtils;
    friend class DataEntry;
private:
    std::vector<DataEntryFieldProperties>& EntryFields_; // Reference to the EntryFields_ vector
    std::ofstream& debugFile_;             // Reference to the debug file
    static ISAMWrapperLib& dbLib_;  // Declare only (no initialization here)
    sqlite3* db_;  // Database connection
    DataEntry dataEntryScreen_;  // DataEntry instance
 
public:
    TransactionEntryHeader(std::vector<DataEntryFieldProperties>& EntryFields_, std::ofstream& debugFile, sqlite3* db);
    //static int askTransType(std::ofstream& debugFile);



    RunResult run(std::map<std::string, std::string>& headerRecord, PassParams& passParams, std::vector<ScreenProperties> screenProps,std::ifstream& screenXmlFile, std::string& screenXmlFileName, std::string& runClass, std::string& nextRunType, std::string& nextRunClass, std::ofstream& debugFile);
   
    ~TransactionEntryHeader();  // Add this line to declare the destructor
    //bool  createTransactionEntryLinesTable(std::vector<DataEntryFieldProperties>& EntryFields_, std::string& screenXmlFileName, std::ofstream& debugFile);
};

extern ISAMWrapperLib dummyDbLib;

#endif // TRANSACTION_ENTRY_HEADER_H
