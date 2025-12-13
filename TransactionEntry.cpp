// Version-02.20.25.16.07
//File: TransactionEntry.cpp
#include "TransactionEntry.h"
#include "TransactionEntryHeader.h"
#include "TransEntryControl.h"
#include "Dialog.h"
#include <iostream>

#include <filesystem>


//#include <optional>

// Constructor

TransactionEntry::TransactionEntry(std::vector<DataEntryFieldProperties>& fields, std::ofstream& debugFile, sqlite3* db)
    : EntryFields_(fields), debugFile_(debugFile), db_(db), dataEntryScreen_(fields,debugFile, db) {
//TransactionEntry::TransactionEntry(std::vector<DataEntryFieldProperties>& fields, std::ofstream& debugFile, sqlite3* db)
//    : EntryFields_(fields),
//    debugFile_(debugFile),
//    db_(db),
//    dataEntryScreen_(debugFile, db) {
    if (db_ == nullptr) {
        std::cerr << "Error: Database connection is null in TransactionEntry constructor!" << std::endl;
    }
}
// Destructor
TransactionEntry::~TransactionEntry() {
    // Destroy shared windows
    SharedWindowManager::destroywinMsgArea();
    SharedWindowManager::destroywinFormScreen();
}
//

RunResult TransactionEntry::run(std::map<std::string, std::string>& headerRecord, const PassParams& inputParams, std::ifstream& screenXmlFile, std::string& screenXmlFileName,
    std::string& nextRunType, std::string& nextRunClass, std::ofstream& debugFile) {

    RunResult runresult;
    std::vector<ScreenProperties> screenProps;
    std::vector<LabelProperties> labels;
    std::vector<DataEntryFieldProperties> headerFields;
    std::vector<DataEntryFieldProperties> DataEntryFields;
    //TODO: is allFields is obsolete? after spliiting xml files into screen & data dictionary
    std::vector<DataEntryFieldProperties> allFields;
    // used to temp store vector DataEntryFields;
    std::vector<DataEntryFieldProperties> EntryFields_; 
    std::vector<std::string> fieldNames;
    std::vector<BoxProperties> boxes;
    WINDOW* winFormScreen = SharedWindowManager::getwinFormScreen();
    DataEntry::setupScreen(screenProps, screenXmlFile, screenXmlFileName, debugFile_);
    //  
        // the first element is not needed because it is emplaced with nulls
    if (!screenProps.empty()) {
        screenProps.erase(screenProps.begin());
    }
    screenXmlFile.clear(); // Clear any error flags
    screenXmlFile.seekg(0, std::ios::beg); // Go back to the beginning of the file

    // need to display header screen before prompting for trx type so
    // setupScreen is called above & displayScreen is called here & again within the TransactionEntryHeader.run below
    debugFile_ << "inside TransactionEntry::run on line 60 before DataEntry::displayScreen screenXmlFileName is " << screenXmlFileName << std::endl;
    headerFields = EntryFields_; // store a copy for later use

    DataEntry::displayScreen(screenXmlFile, screenXmlFileName, screenProps, boxes, labels, EntryFields_, debugFile_);

    PassParams passParams = DataEntry::parseParamXML(screenXmlFile, screenXmlFileName, debugFile_);

    
    std::string saveRunType = "";
    std::string saveRunClass = "";
    std::string scrollKey = "";
    saveRunType = nextRunType;
    saveRunClass = nextRunClass;
    std::string slash = "";
    namespace fs = std::filesystem;
    fs::path currentPath = fs::current_path();
    //std::string status = "";
    


    ISAMWrapperLib dbLibInstance(db_);
    //TransEntryControl control(dbLibInstance, screenXmlFileName, maxrows, scrollKey);
    //std::optional<TransEntryControl> control;


    //DataEntry::dbLib_ = dbLibInstance;
    TransactionEntryHeader TransactionEntryHeader(EntryFields_, debugFile_, db_);

    //std::string nextRunType = saveRunType;
    //std::string nextRunClass = saveRunClass;
    std::string ask = std::to_string(askTransType(screenProps));
    int TrxType = std::stoi(ask);
    std::string key = "TTYP";
    passParams.fields[key].value = std::to_string(TrxType);
    key = "TRANS";
    passParams.fields[key].value = "";
    //int nextTranNum = 0;
    //std::string nextTranNo = "";
    //std::ostringstream oss;
    //std::string dbname = nextRunClass + "NextSeqNo";

    switch (TrxType) {

    case 1: // new invoice                

        // TODO: find way so these are not hard-coded        
        nextRunType = "TransactionEntryHeader", nextRunClass = "ARTransHeaderScreen";
        passParams.fields["AllowAdd"].value = "Y";
        debugFile_ << "inside TransactionEntry::run  on line 109 before TransactionEntryHeader.run" << std::endl;

        runresult = TransactionEntryHeader.run(headerRecord, passParams, screenProps, screenXmlFile, screenXmlFileName, nextRunClass, nextRunType, nextRunClass, debugFile_);
        debugFile_ << "[DEBUG] Line 112 headerRecord values upon return from TransactionEntryHeader.run in TransactionEntry.cpp" << std::endl;

        for (const auto& kv : headerRecord) {
            debugFile_ << "  " << kv.first << " = " << kv.second << std::endl;
        }
        break;
    case 2://new credit
        //TODO: ??   
        // askTrxType = false;
        //passParams.fields["ALLOWADD"].value = "Y";
        break;
    case 3: //edit trans
        // TODO: find way so these are not hard-coded
        nextRunType = "TransactionEntry", nextRunClass = "ARTransHeaderScreen";
        //passParams.fields[key].value = nextTranNo;
        passParams.fields["ALLOWADD"].value = "N";
        debugFile_ << "inside TransactionEntry::run  on line 128 before TransactionEntryHeader.run" << std::endl;

        runresult = TransactionEntryHeader.run(headerRecord, passParams, screenProps, screenXmlFile, screenXmlFileName, nextRunClass, nextRunType, nextRunClass, debugFile_);
        break;
    default:
        //TODO: ??
        break;
    }
    if (runresult.status == "Error") {
        //TODO: MAKE ERROR DISPLAY
        runresult.nextRunType = "Error";
        runresult.nextRunClass = "Error";

    }
    else if (runresult.status == "Exit") {
        if (runresult.nextRunType.empty() || runresult.nextRunClass.empty())
        {
            runresult.nextRunType = "Menu";
            runresult.nextRunClass = "MainMenuScreen";
        }

    }


    else if (runresult.status == "LineDeleted") {
        // TODO: find way so this is not hard - coded        
        runresult.nextRunType = "TransControl";
        runresult.nextRunClass = screenProps[0].controlClass_;//"ARTransControl";

    }
    else if (runresult.status == "HeaderDeleted") {
        // TODO: find way so these are not hard-coded
        runresult.nextRunType = "TransactionEntry";
        runresult.nextRunClass = "ARTransHeaderScreen";

    }
    else if (runresult.status == "HeaderSaved") {
        debugFile_ << "[DEBUG] Line 162 headerRecord values upon return from TransactionEntryHeader.run in TransactionEntry.cpp" << std::endl;
        debugFile_ << "------------------------------------------------------------------------------------" << std::endl;

        for (const auto& kv : headerRecord) {
            debugFile_ << "  " << kv.first << " = " << kv.second << std::endl;
            if (kv.first == screenProps[0].scrollKey_) {

                scrollKey = kv.second;
                passParams.fields.at(screenProps[0].scrollKey_).value = scrollKey;


            }
        }

        std::vector<ScreenProperties> entryLineScreenProps;
        std::vector<DataEntryFieldProperties>entryLinesfields;
        std::vector<std::string> entryLinesfieldNames;
        // TODO: find way so this is not hard-coded
        nextRunType = "TransControl";
        nextRunClass = screenProps[0].controlClass_;

    }
    // TODO: find way so this is not hard-coded
    else if (runresult.status == "Menu") {
        runresult.nextRunType = "Menu";
        runresult.nextRunClass = "MainMenuScreen";
        goto Exit;
    }
    else if (runresult.status != "OK" && !runresult.status.empty()) {
        //TODO: not sure if this would happen
        runresult.nextRunType = "Menu";
        runresult.nextRunClass = "MainMenuScreen";
        goto Exit;
    }

    //TODO: this is where the control screen would take over
    if (runresult.nextRunType == "TransControl") {
        SharedWindowManager::destroywinFormScreen();
        WINDOW* winFormScreen = SharedWindowManager::getwinFormScreen(screenProps);
        if (passParams.fields.find(screenProps[0].scrollKey_) != passParams.fields.end()) {

            scrollKey = passParams.fields.at(screenProps[0].scrollKey_).value;
            
        }

    }

    if (screenXmlFile.is_open()) {
        screenXmlFile.close();
    }


    //screenXmlFile.open(screenXmlFileName);
    if (runresult.status != "Exit") {
        screenXmlFileName = screenProps[0].controlClass_;
        screenXmlFileName.append(".xml");
    }
    else
        goto Exit;


#ifdef _WIN32
    slash = "\\";
#else
    slash = "/";
#endif


    screenXmlFileName = currentPath.string();
    screenXmlFileName.append(slash + runresult.nextRunClass + ".xml");

    screenXmlFile.open(screenXmlFileName);


    if (!screenXmlFile.is_open()) {
        debugFile_ << "Failed to open " + screenXmlFileName + " file." << std::endl;
        return RunResult{ "Error" };
    }


    if (true == true) // necessary to prevent build error when using a goto statement


    {
        //PassParams passControlParams = DataEntry::parseParamXML(screenXmlFile, screenXmlFileName, debugFile_);
        int maxrows = screenProps[0].scrollMaxRows_;        
        std::string scrollTableFileName = screenProps[0].scrollTable_;
        std::vector<controlFieldProperties> controlFields;
        std::string ControlxmlFileName;


        ControlxmlFileName = currentPath.string();
        
        ControlxmlFileName.append(slash + screenProps[0].controlClass_ + ".xml");//ARTransControlScreen.xml");
        
        
        std::ifstream controlxmlFile;
        controlxmlFile.open(ControlxmlFileName);
        //allFields = DataEntryFields; // or eventually load separately
        
    
        TransEntryControl control(*winFormScreen,dbLibInstance, scrollTableFileName, maxrows, scrollKey);
        control.setupControlFields(controlFields, controlxmlFile, ControlxmlFileName, debugFile_);
        //std::map<std::string, std::string> headerRecord;
        //for (const auto& [key, field] : passParams.fields) {
        //    headerRecord[key] = field.value;
        //}
        //headerRecord = dataEntryScreen_.getHeaderRecord();

        control.run(headerRecord, screenProps, passParams, nextRunClass, controlxmlFile, ControlxmlFileName, controlFields, debugFile);
             
        
        
        headerRecord = dataEntryScreen_.getHeaderRecord();
    }



Exit:    
     
    return runresult;
}
   
int TransactionEntry::askTransType(std::vector<ScreenProperties>& screenProps) {
    // ex. new invoice,credit or edit
    std::vector<DialogFieldProperties> dialogOptions;
    Dialog DialogEntryScreen(dialogOptions);
    //DialogFieldProperties
    int result = 0;     
    std::string screenXmlFileName = screenProps[0].screenDialog_; //"ARTransChoice.xml";
    result = DialogEntryScreen.run(screenXmlFileName, debugFile_);
    if (result < 0) {
        //TODO: MAKE ERROR DISPLAY

    }
    return result;

}



