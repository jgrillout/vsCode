// Version-02.20.25.16.07
// File: TransactionEntryHeader.cpp

#include "TransactionEntryHeader.h"

// Constructor
TransactionEntryHeader::TransactionEntryHeader(std::vector<DataEntryFieldProperties>& fields, std::ofstream& debugFile, sqlite3* db)
    : EntryFields_(fields), debugFile_(debugFile), db_(db), dataEntryScreen_(fields, debugFile, db) {
    //: debugFile_(debugFile), db_(db), dataEntryScreen_(debugFile, db) {
    //: EntryFields_(fields), debugFile_(debugFile), db_(db), dataEntryScreen_(debugFile, db) {

}

// Destructor
TransactionEntryHeader::~TransactionEntryHeader() {
    // Destroy shared windows
    SharedWindowManager::destroywinMsgArea();
    SharedWindowManager::destroywinFormScreen();
}


RunResult TransactionEntryHeader::run(std::map<std::string, std::string>& headerRecord,PassParams& passParams, std::vector<ScreenProperties> screenProps,std::ifstream& screenXmlFile, std::string& screenXmlFileName,  std::string& runClass, std::string& nextRunType, std::string& nextRunClass, std::ofstream& debugFile) {
    
    debugFile << "entering TransactionEntryHeader::run  on line 24  " << "screenXmlFileName " << screenXmlFileName << std::endl;
    debugFile_ << "[DEBUG] Line 25 headerRecord values in TransactionEntryHeader.cpp" << std::endl;
    debugFile_ << "------------------------------------------------------------------------------------" << std::endl;

    for (const auto& kv : headerRecord) {
        debugFile_ << "  " << kv.first << " = " << kv.second << std::endl;
    }

    namespace fs = std::filesystem;
    fs::path currentPath = fs::current_path();
    std::string entryLinesxmlFileName = currentPath.string();  
    std::string result = "";
    int nextTranNum = 0;
    std::string nextTranNo = "";
    std::string exitRunType = "";
    std::string exitRunClass = "";


    std::string slash = "";
    #ifdef _WIN32
        slash = "\\";
    #else
        slash = "/";
    #endif


   
    int TrxType= std::stoi(passParams.fields["TTYP"].value);

    
    std::ostringstream oss;
    
    std::string nextSeqFileName = nextRunClass + "NextSeqNo";

    if (TrxType==1) {
        
            nextTranNum = dataEntryScreen_.getNextNumber(nextSeqFileName, debugFile_);
            oss << std::setw(4) << std::setfill('0') << nextTranNum;
            nextTranNo = oss.str(); // Store formatted output in a string       
            //passParams.fields["TRANS"].value = nextTranNo;
            if (passParams.fields.find(screenProps[0].scrollKey_) != passParams.fields.end()) {
                //TODO: HARD CODED  CHANGE
                passParams.fields["TRANS"].value = nextTranNo;
               
                passParams.fields.at(screenProps[0].scrollKey_).value= nextTranNo;
            }
            
    }
    else if (TrxType == 3) {
        passParams.fields.at("TTYP").value = "5"; // credit 
    }
    else if (TrxType == 1) {
        passParams.fields.at("TTYP").value = "1"; // invoice
    }
    WINDOW* winFormScreen = SharedWindowManager::getwinFormScreen();
    wattroff(winFormScreen, A_REVERSE);
    wrefresh(winFormScreen);
    RunResult runresult;
    debugFile << "before dataEntryScreen_.run  on line 77 in TransactionEntryHeader.cpp  " << std::endl;
    runresult = dataEntryScreen_.run(passParams, screenXmlFile,screenXmlFileName, nextRunType, nextRunClass, debugFile);
                               
    //result = runresult.status;
    if (runresult.status == "Error") {
        //TODO: MAKE ERROR DISPLAY
        nextRunType = "";
        nextRunClass = "";
        return runresult;

    }
    // Populate headerRecord from the collected screen values in EntryFields_
    headerRecord = dataEntryScreen_.getHeaderRecord();
    debugFile << "[DEBUG] Line 90 headerRecord returned from dataEntryScreen_.getHeaderRecord():\n";
    for (const auto& kv : headerRecord) {
        debugFile << "kv.first =" << kv.first << "kv.second = " << kv.second << "kv.second.len = " << kv.second.length() <<"\n";
    }
    std::vector<ScreenProperties> entryLineScreenProps;
    std::vector<DataEntryFieldProperties>entryLinesfields;
    std::vector<std::string> entryLinesfieldNames;   

    //passParams.fields["TRANS"].value = dataEntryScreen_.scrollKey_;
    debugFile_ << "[DEBUG] Line 99 headerRecord values before TransControl in TransactionEntryHeader.cpp" << std::endl;
    debugFile_ << "------------------------------------------------------------------------------------" << std::endl;
    for (const auto& kv : headerRecord) {
        debugFile << "kv.first =" << kv.first << "kv.second = " << kv.second << "\n";
    }
    screenXmlFile.close();
    if (runresult.status == "HeaderDeleted") {
        // TODO: find way so these are not hard-coded

        runresult.nextRunType = "TransactionEntry", runresult.nextRunClass = "ARTransHeaderScreen";

    }
    else if (runresult.status == "HeaderSaved") {
        // TODO: find way so this not hard-coded
      
        runresult.nextRunType = "TransControl", runresult.nextRunClass = screenProps[0].controlClass_;



    }
    else if (runresult.status == "Exit") {
        runresult.nextRunType = screenProps[0].screenExitRunType_;
        runresult.nextRunClass = screenProps[0].screenExitRunClass_;
        if (runresult.nextRunType.empty() || runresult.nextRunClass.empty())
        {
            runresult.nextRunType = "Menu";
            runresult.nextRunClass = "MainMenuScreen";
        }

    }
    
    // TODO: find way so this is not hard-coded
    else if (runresult.status == "Menu") {
        runresult.nextRunType = "Menu";
        runresult.nextRunClass = "MainMenuScreen";
    }
    else if (runresult.status != "OK" && !runresult.status.empty()) {
        //TODO: not sure if this would happen
        runresult.nextRunType = "Menu";
        runresult.nextRunClass = "MainMenuScreen";
    }

   
    return runresult;

}
