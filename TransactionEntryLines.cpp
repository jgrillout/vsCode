// File: TransactionEntryLines.cpp
#include "TransactionEntryLines.h"
#include <sstream>
#include <iostream>
#include <fstream>

ISAMWrapperLib& TransactionEntryLines::dbLib_ = dummyDbLib;

TransactionEntryLines::TransactionEntryLines(std::vector<DataEntryFieldProperties>& lineRecord, std::vector<ScreenProperties>& screenProps, std::ofstream& debugFile,sqlite3* db,const std::map<std::string, std::string>& headerRecord)
    : lineRecord_(lineRecord), screenProps_(screenProps),debugFile_(debugFile), db_(db), headerRecord_(headerRecord),
     dataEntryScreen_(lineRecord_, debugFile_, db_)


    
{   // TODO: find way so this is not hard-coded
    tableName_ = "ARTransLines";
    xmlFileName_ = "ARTransLinesScreen.xml";
                    
}


RunResult TransactionEntryLines::addLineItem(std::map<std::string, std::string>& LineRecord) {

    RunResult runresult;
    //std::string result;

    // TODO: find way so this is not hard-coded
    std::string batchId = headerRecord_["BATCH"];
    std::string transNo = headerRecord_["TRANS"];
    int maxEnt = 0;
    auto LineRecords = dbLib_.selectDynamicRecords(tableName_, "TRANS='" + transNo + "'", "ENT", "ENT");
    for (const auto& LineRecord : LineRecords.first) {
        int ent = std::stoi(LineRecord.at("ENT"));
        if (ent > maxEnt) maxEnt = ent;
    }
    //params.fields["ENT"] = { "ENT", "ENT", std::to_string(maxEnt + 1) };

    runresult = runDataEntry(LineRecord);
    debugFile_ << "after  runDataEntry(LineRecord)  on line 39  " << std::endl;
    debugFile_ << "[DEBUG] Line 40 LineRecord values in TransactionEntryLines.cpp" << std::endl;
    debugFile_ << "------------------------------------------------------------------------------------" << std::endl;
    for (const auto& kv : LineRecord) {
        debugFile_ << "LineRecord  " << kv.first << " = " << kv.second << std::endl;
    }
    if (runresult.status == "Error") {
        //TODO: MAKE ERROR DISPLAY

        runresult.nextRunClass = "Error";
        runresult.nextRunType = "Error";
    }
    else if (runresult.status == "Exit") {
        if (runresult.nextRunType.empty() || runresult.nextRunClass.empty())
        {
            bool test;
            runresult.nextRunClass = screenProps_[0].screenExitRunClass_;
            runresult.nextRunType = screenProps_[0].screenExitRunType_;
        }
    }
    else if (runresult.status == "LineDeleted") {
        bool test;
        // TODO: find way so this not hard-coded
        //runresult.nextRunType = "TransControl", runresult.nextRunClass = screenProps[0].controlClass_;

    }


    
    // why is this here? dataEntry class should have added the record
    //std::string sql = buildInsertSQL(LineRecord);
    //dbLib_.executeQuery(sql);

    return runresult;
}

RunResult TransactionEntryLines::editLineItem(std::map<std::string, std::string>& LineRecord) {
    
    RunResult runresult;
      
    runresult = runDataEntry(LineRecord);
    if (runresult.status == "Error") {
        //TODO: MAKE ERROR DISPLAY

        runresult.nextRunClass = "Error";
        runresult.nextRunType = "Error";
    }
    else if (runresult.status == "Exit") {
        if (runresult.nextRunClass.empty() || runresult.nextRunType.empty())
        {
            runresult.nextRunClass = screenProps_[0].screenExitRunClass_;
            runresult.nextRunType = screenProps_[0].screenExitRunType_;
        }
    }
    // why is this here? dataEntry class should have updated table
    //std::string sql = buildUpdateSQL(LineRecord);
    //dbLib_.executeQuery(sql);

    return runresult;
}
RunResult TransactionEntryLines::runDataEntry(std::map<std::string, std::string>& LineRecord) {

    RunResult runresult;
    
    std::ifstream screenXmlFile(xmlFileName_);
    if (!screenXmlFile.is_open()) {
        debugFile_ << "Failed to open XML file: " << xmlFileName_ << std::endl;
        //TODO: MAKE ERROR DISPLAY
        runresult.status = "Error";
        runresult.nextRunClass = "";
        runresult.nextRunType = "";
        return runresult;
    }

    // TODO: find way so this is not hard-coded
    const std::string nextRunType = "DataEntry";
    const std::string nextRunClass = "ARTransLinesScreen";
    PassParams params;       
    dataEntryScreen_.setEditRecord(LineRecord);    
    runresult = dataEntryScreen_.run(params, screenXmlFile,xmlFileName_, nextRunType, nextRunClass, debugFile_);
    return runresult;
}

std::string TransactionEntryLines::buildInsertSQL(std::map<std::string, std::string>& LineRecord) {
    std::ostringstream sql;
    sql << "INSERT INTO " << tableName_ << " (";
    /*for (auto it = fieldData.begin(); it != fieldData.end(); ++it) {
        sql << it->first;
        if (std::next(it) != fieldData.end()) sql << ",";
    }
    sql << ") VALUES (";
    for (auto it = fieldData.begin(); it != fieldData.end(); ++it) {
        sql << "'" << it->second << "'";
        if (std::next(it) != fieldData.end()) sql << ",";
    }*/
    sql << ")";
    return sql.str();
}

std::string TransactionEntryLines::buildUpdateSQL(std::map<std::string, std::string>& LineRecord) {
    std::ostringstream sql;
    sql << "UPDATE " << tableName_ << " SET ";
    bool first = true;
    /* for (const auto& it : fieldData) {
        if (it.first == "TRANS" || it.first == "ENT") continue;
        if (!first) sql << ", ";
        sql << it.first << "='" << it.second << "'";
        first = false;
    }
    sql << " WHERE TRANS='" << fieldData.at("TRANS") << "' AND ENT='" << fieldData.at("ENT") << "'";*/
    return sql.str();
}
void TransactionEntryLines::setHeaderRecord(const std::map<std::string, std::string>& headerRecord) {
    headerRecord_ = headerRecord;
}
