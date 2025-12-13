// Version 02.20.25.16.07
// File: ISAMWrapperLib.h

#ifndef ISAMWRAPPERLIB_H
#define ISAMWRAPPERLIB_H

#include "sqlite3.h"
#include <string>
#include <vector>
#include <map>
#include <tuple>
#include <sstream>
#include <algorithm>
#include <iostream>


class ISAMWrapperLib {
public:
    ISAMWrapperLib(sqlite3* db, const std::string& pkColumn);
    ISAMWrapperLib(sqlite3* db) : db_(db) {}
    ~ISAMWrapperLib();

    int executeQuery(const std::string& query);
    int createTable(const std::string& tableName, std::vector<std::pair<std::string, std::string>>& columns);
    int insert(const std::string& tableName, const std::vector<std::string>& fieldNames, const std::vector<std::string>& values);
    int updateRow(const std::string& tableName, const std::vector<std::string>& fieldNames, const std::vector<std::string>& values, const std::string& primaryKeyValue);
    int deleteRow(const std::string& tableName, const std::string& primaryKeyValue);    
    std::vector<std::tuple<std::string, std::string>> selectRows(const std::string& tableName, const std::string& condition, const std::string& inquiryFields, const std::string& orderBy);
    std::string execScalar(const std::string& sql, const std::string& condition);
    std::pair<std::vector<std::map<std::string, std::string>>, std::vector<std::string>>selectDynamicRecords(const std::string& tableName, const std::string& condition, const std::string& inquiryFields, const std::string& orderBy);
    bool getRecordByCondition(const std::string& table, const std::string& condition, std::vector<std::string>& record);
    std::string sanitizeFieldName(const std::string& fieldName);
    bool isValid() const {
        return db_ != nullptr;
    }
    sqlite3* getDb() const { return db_; }  

private:
    sqlite3* db_;
    std::string primaryKeyColumn_;
};

#endif // ISAMWRAPPERLIB_H
