//Version-02.20.25.16.07
// File: ISAMWrapperLib.cpp
// Defines the functions for the static library.
#include "ISAMWrapperLib.h"

//sqlite3* ISAMWrapperLib::db = nullptr;
// Constructor implementation
ISAMWrapperLib::ISAMWrapperLib(sqlite3* db, const std::string& pkColumn)
    : db_(db), primaryKeyColumn_(pkColumn) {
    // The database connection is  passed in, so we don't open it here.
}

// Destructor implementation
ISAMWrapperLib::~ISAMWrapperLib() {
    // Close the database connection
    //sqlite3_close(db);
}
// Execute SQL query
int ISAMWrapperLib::executeQuery(const std::string& query) {

    char* errMsg = nullptr;
    std::cout << "in executeQuery query.c_str() = " << query.c_str() << std::endl;
    int rc = sqlite3_exec(db_, query.c_str(), NULL, 0, &errMsg);
    SQLITE_API const char* msg = sqlite3_errmsg(db_);
    std::cout << "sqlite3_errmsg = " << msg << std::endl;
    if (errMsg != nullptr) {
        sqlite3_free(errMsg);
    }
    return rc;
}


// Create a table
// columns[i].first is expected to be the column name. 
// columns[i].second is a YN field that indicates if a column is part of the primary key
int ISAMWrapperLib::createTable(const std::string& tableName, std::vector<std::pair<std::string, std::string>>& columns) {


    std::string query = "CREATE TABLE IF NOT EXISTS '" + tableName + "' (";
    std::string keys = "";
    for (size_t i = 0; i < columns.size(); ++i) {
        query += "'" + sanitizeFieldName(columns[i].first) + "',";
        /*if (i != columns.size() - 1)
            query += ", ";*/
        if (columns[i].second == "Y") {
            keys.append(columns[i].first);
            if (i < columns.size()) {
                keys.append(",");
            }
        }
    }
    if (!keys.empty() && keys.back() == ',') {
        keys.pop_back(); // Removes the last character if it's a comma
    }
    query.append(" PRIMARY KEY (");
    query.append(keys);
    query.append("))");

    return executeQuery(query);
}

//int ISAMWrapperLib::insert(const std::string& tableName, const std::vector<std::string>& fieldNames, const std::vector<std::string>& fieldValues) {
//
//
//
//    if (fieldValues.empty()) {
//        // No fieldValues provided, return an error code
//        return SQLITE_ERROR;
//    }
//
//    std::string query = "INSERT INTO " + tableName + " VALUES (";
//    for (size_t i = 0; i < fieldNames.size(); ++i) {
//        query += "'" + fieldValues[i] + "'";
//        if (i != fieldNames.size() - 1) {
//            query += ", ";
//        }
//    }
//    query += ")";
//
//    return executeQuery(query);
//}

int ISAMWrapperLib::insert(const std::string& tableName, const std::vector<std::string>& fieldNames, const std::vector<std::string>& fieldValues) {
    if (fieldValues.empty() || fieldNames.size() != fieldValues.size()) {
        // Mismatch or empty input, return an error
        return SQLITE_ERROR;
    }

    // Start building the query with column list
    std::string query = "INSERT INTO " + tableName + " (";
    for (size_t i = 0; i < fieldNames.size(); ++i) {
        query += fieldNames[i];
        if (i != fieldNames.size() - 1) {
            query += ", ";
        }
    }
    query += ") VALUES (";

    // Now add the values
    for (size_t i = 0; i < fieldValues.size(); ++i) {
        query += "'" + fieldValues[i] + "'";
        if (i != fieldValues.size() - 1) {
            query += ", ";
        }
    }
    query += ")";

    return executeQuery(query);
}

int ISAMWrapperLib::updateRow(const std::string& tableName, const std::vector<std::string>& columnNames, const std::vector<std::string>& newValues, const std::string& condition) {
    if (newValues.empty()) {
        // Invalid input, return an error code
        return SQLITE_ERROR;
    }

    std::string query = "UPDATE " + tableName + " SET ";
    for (size_t i = 0; i < columnNames.size(); ++i) {
        query += columnNames[i] + " = '" + newValues[i] + "'";
        if (i != columnNames.size() - 1) {
            query += ", ";
        }
    }
    if (!condition.empty()) {
        query += " WHERE " + condition;
    }

    return executeQuery(query);
}

// Delete a row
int ISAMWrapperLib::deleteRow(const std::string& tableName, const std::string& condition) {
    std::string query = "DELETE FROM " + tableName;
    if (!condition.empty()) {
        query += " WHERE " + condition;
    }

    return executeQuery(query);
}

std::vector<std::tuple<std::string, std::string>> ISAMWrapperLib::selectRows(const std::string& tableName, const std::string& condition, const std::string& inquiryFields, const std::string& orderBy) {
    std::string selFields = inquiryFields.empty() ? "*" : inquiryFields;
    std::string query = "SELECT " + selFields + " FROM " + tableName;

    if (!condition.empty()) query += " WHERE " + condition;
    if (!orderBy.empty()) query += " ORDER BY " + orderBy;

    sqlite3_stmt* stmt;
    std::vector<std::tuple<std::string, std::string>> results;

    if (sqlite3_prepare_v2(db_, query.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        int numColumns = sqlite3_column_count(stmt);

        // Ensure we are only selecting exactly two fields
        if (numColumns < 2) {
            std::cerr << "Error: selectRows() expected at least two fields but got " << numColumns << std::endl;
            sqlite3_finalize(stmt);
            return results;  // Return empty results
        }

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string firstField = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            std::string secondField = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            results.emplace_back(firstField, secondField);  // Store as a tuple
        }
        sqlite3_finalize(stmt);
    }
    return results;
}


//std::vector<std::vector<std::string>> ISAMWrapperLib::selectRowsMultipleFields(const std::string& tableName, const std::string& condition, const std::string& inquiryFields, const std::string& orderBy) {
//    std::string selFields = inquiryFields.empty() ? "*" : inquiryFields;
//    std::string query = "SELECT " + selFields + " FROM " + tableName;
//
//    if (!condition.empty()) query += " WHERE " + condition;
//    if (!orderBy.empty()) query += " ORDER BY " + orderBy;
//
//    sqlite3_stmt* stmt;
//    std::vector<std::vector<std::string>> results;
//
//    if (sqlite3_prepare_v2(db_, query.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
//        int numColumns = sqlite3_column_count(stmt);  // Get number of fields per record
//
//        while (sqlite3_step(stmt) == SQLITE_ROW) {
//            std::vector<std::string> row;  // New row for this record
//            for (int i = 0; i < numColumns; ++i) {
//                const char* colText = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
//                row.push_back(colText ? colText : "");  // Store each field in this row
//            }
//            results.push_back(row);  // Add the full row (record) to results
//        }
//        sqlite3_finalize(stmt);
//    }
//    return results;
//}

std::string ISAMWrapperLib::execScalar(const std::string& sql, const std::string& condition) {
    sqlite3_stmt* stmt = nullptr;
    std::string out;
    std::string q = sql;
    if (!condition.empty()) q.append(condition);
 
    if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            const unsigned char* v = sqlite3_column_text(stmt, 0);
            out = v ? reinterpret_cast<const char*>(v) : "";
        }
    }
    if (stmt) sqlite3_finalize(stmt);
    return out;
}
std::pair<std::vector<std::map<std::string, std::string>>, std::vector<std::string>>
ISAMWrapperLib::selectDynamicRecords(const std::string& tableName,const std::string& condition,const std::string& inquiryFields,const std::string& orderBy)
{
    std::string selFields = inquiryFields.empty() ? "*" : inquiryFields;
    std::string query = "SELECT " + selFields + " FROM " + tableName;

    if (!condition.empty()) query += " WHERE " + condition;
    if (!orderBy.empty())   query += " ORDER BY " + orderBy;

    sqlite3_stmt* stmt;
    std::vector<std::map<std::string, std::string>> records;
    std::vector<std::string> fieldNames;

    if (sqlite3_prepare_v2(db_, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        int numColumns = sqlite3_column_count(stmt);

        // Capture field names once
        for (int i = 0; i < numColumns; ++i) {
            const char* colName = sqlite3_column_name(stmt, i);
            fieldNames.push_back(colName ? colName : "");
        }

        // Capture rows
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::map<std::string, std::string> record;
            for (int i = 0; i < numColumns; ++i) {
                const char* val = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
                record[fieldNames[i]] = val ? val : "";
            }
            records.push_back(record);
        }
        sqlite3_finalize(stmt);
    }
    else {
        std::cerr << "SQLite error: " << sqlite3_errmsg(db_) << std::endl;
    }

    return { records, fieldNames };
}

bool ISAMWrapperLib::getRecordByCondition(const std::string& table, const std::string& condition, std::vector<std::string>& record) {
    std::string query = "SELECT * FROM " + table + " WHERE " + condition;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, query.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
        return false; // Query preparation failure
    }
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        int columns = sqlite3_column_count(stmt);
        for (int i = 0; i < columns; ++i) {
            const unsigned char* colValue = sqlite3_column_text(stmt, i);
            record.push_back(colValue ? reinterpret_cast<const char*>(colValue) : "");
        }
        sqlite3_finalize(stmt);
        return true; // Record found and populated
    }

    sqlite3_finalize(stmt);
    record.clear(); // Clear any existing data in the record vector
    return false; // No record found
}
// Helper function to sanitize field names by removing symbols and spaces
std::string ISAMWrapperLib::sanitizeFieldName(const std::string& fieldName) {
    std::string sanitized;
    for (char c : fieldName) {
        if (std::isalnum(static_cast<unsigned char>(c))) {
            sanitized += c; // Add only alphanumeric characters to sanitized string
        }
    }
    return sanitized;
}

