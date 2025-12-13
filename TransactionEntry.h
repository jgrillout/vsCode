//Version-02.20.25.16.07
//File: TransactionEntry.h

#ifndef TRANSACTION_ENTRY_H
#define TRANSACTION_ENTRY_H
#include <map>   
#include <vector>
#include <string>
#include <fstream>
#include "DataEntry.h"
#include "SharedWindowManager.h"
#include "Dialog.h"
class TransactionEntry {
    friend class DataEntryUtils;
    friend class DataEntry;
private:
    std::vector<DataEntryFieldProperties>& EntryFields_; // Reference to the EntryFields_ vector
    std::ofstream& debugFile_;             // Reference to the debug file
    //static bool askTransType;

    sqlite3* db_;  // Database connection
    DataEntry dataEntryScreen_;  // DataEntry instance
public:
    //TransactionEntry(std::vector<DataEntryFieldProperties>& EntryFields_, std::ofstream& debugFile, sqlite3* db);
    //TransactionEntry(std::ofstream& debugFile, sqlite3* db);
    TransactionEntry(std::vector<DataEntryFieldProperties>& fields, std::ofstream& debugFile, sqlite3* db);
     
    static ISAMWrapperLib& dbLib_;  // Declare only (no initialization here)


    int askTransType(std::vector<ScreenProperties>& screenProps);
    RunResult run(std::map<std::string, std::string>& headerRecord, const PassParams& inputParams, std::ifstream& screenXmlFile, std::string& screenXmlFileName,
        std::string& nextRunType, std::string& nextRunClass, std::ofstream& debugFile);
    ~TransactionEntry();  
};

extern ISAMWrapperLib dummyDbLib;
//extern ISAMWrapperLib dbLib_;// = dummyDbLib




#endif // TRANSACTION_ENTRY_H
