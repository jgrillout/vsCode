#pragma once
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include "ISAMWrapperLib.h"
//#include "PassParam.h"
//#include "DataEntryFieldProperties.h"
#include "DataEntry.h"
#include "sqlite3.h"
#include <cctype>

class TransactionEntryLines {
public:
    void setHeaderRecord(const std::map<std::string, std::string>& headerRecord);
    //TransactionEntryLines(std::ofstream& debugFile, sqlite3* db, const std::map<std::string, std::string>& headerRecord);
    
    //TransactionEntryLines(std::vector<DataEntryFieldProperties>& lineRecord, std::ofstream& debugFile, sqlite3* db, const std::map<std::string, std::string>& headerRecord);
    
    TransactionEntryLines(std::vector<DataEntryFieldProperties>& lineRecord, std::vector<ScreenProperties>& screenProps, std::ofstream& debugFile, sqlite3* db, const std::map<std::string, std::string>& headerRecord);
      
    RunResult addLineItem(std::map<std::string, std::string>& LineRecords);
    RunResult editLineItem(std::map<std::string, std::string>& LineRecords);

private:
    std::vector<DataEntryFieldProperties>& lineRecord_;
    std::vector<ScreenProperties>& screenProps_;
    std::map<std::string, std::string> headerRecord_;        
    std::ofstream& debugFile_;
    sqlite3* db_;

    //ISAMWrapperLib& db_;
    DataEntry dataEntryScreen_;
    std::string tableName_;
    std::string xmlFileName_;

    RunResult runDataEntry(std::map<std::string, std::string>& LineRecords);
    std::string buildInsertSQL(std::map<std::string, std::string>& LineRecords);
    std::string buildUpdateSQL(std::map<std::string, std::string>& LineRecords);

    static ISAMWrapperLib& dbLib_; // Declare for static usage
};


