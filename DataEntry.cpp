// Version-02.20.25.16.07
// File: DataEntry.cpp
#include "DataEntry.h"
#include "InputUtils.h"
#include "SharedWindowManager.h"
#include "menuWindowManager.h"
//#include "PassParam.h"
#include "DataEntryUtils.h"
// Define static members of DataEntry class

std::string DataEntry::runClass_ = "";
std::string DataEntry::screenExitRunType_ = "";
std::string DataEntry::screenExitRunClass_ = "";

std::string DataEntry::scrollKey_ = "";
int DataEntry::stdscrRows_ = 0;
int DataEntry::stdscrCols_ = 0;

namespace fs = std::filesystem;

ISAMWrapperLib& DataEntry::dbLib_ = dummyDbLib;
std::string InputKeyPressed = "";
// herehere
//std::string DataEntry::scrollKey;

// Constructor
DataEntry::DataEntry(std::vector<DataEntryFieldProperties>& EntryFields, std::ofstream& debugFile, sqlite3* db)
//: EntryFields_(EntryFields), debugFile_(debugFile), db_(db) {
//DataEntry::DataEntry(std::ofstream& debugFile, sqlite3* db)
    :debugFile_(debugFile), db_(db) {
    if (db_ == nullptr) {
        std::cerr << "Error: Database connection is null in DataEntry constructor!" << std::endl;
    }
}
// Destructor
DataEntry::~DataEntry() {
    // Shared windows should NOT be destroyed here!
    // Multiple DataEntry objects may exist (e.g., as members of TransactionEntry)
    // and destroying shared windows in the destructor causes access violations
    // when other code tries to use the already-freed windows.
    // 
    // Shared windows are properly destroyed at program exit in main.cpp
    // 
    // SharedWindowManager::destroywinMsgArea();     // REMOVED - causes double-free
    // SharedWindowManager::destroywinFormScreen();  // REMOVED - causes access violation
}

RunResult DataEntry::run(const PassParams& inputParams, std::ifstream& screenXmlFile, const std::string& screenXmlFileName,
    const std::string& nextRunType, const std::string& nextRunClass, std::ofstream& debugFile) {

    tableName_ = DataEntry::getTableName(screenXmlFile, screenXmlFileName, debugFile);

    std::string slash = "";
#ifdef _WIN32
    slash = "\\";
#else
    slash = "/";
#endif
    RunResult result;
    namespace fs = std::filesystem;
    fs::path currentPath = fs::current_path();
    std::string localDictionaryXmlFileName = currentPath.string();
    localDictionaryXmlFileName.append(slash + tableName_ + "DataDictionary.xml");

    std::ifstream localDictionaryXmlFile(localDictionaryXmlFileName);
    if (!localDictionaryXmlFile.is_open()) {
        debugFile << "ERROR: Could not open Dictionary XML file: " << localDictionaryXmlFileName << std::endl;
        result.status = "failed to open localDictionary copy of XML file";
        return result;
    }
    std::string localScreenXmlFileName = screenXmlFileName;
    //debugFile << "entering  DataEntry::run  on line 68 " << "localScreenXmlFileName " << localScreenXmlFileName << std::endl;
    //debugFile << "entering  DataEntry::run  on line 69 &localScreenXmlFileName = " << static_cast<const void*>(&localScreenXmlFileName) << std::endl;
    std::ifstream localScreenXmlFile(localScreenXmlFileName);
    if (!localScreenXmlFile.is_open()) {
        debugFile << "ERROR: Could not open Screen XML file: " << localScreenXmlFileName << std::endl;
        result.status = "failed to open localScreen copy of XML file";
        return result;
    }
    result.nextRunClass = nextRunClass;
    result.nextRunType = nextRunType;
    char* errMsg = nullptr;  // reserved for future SQLite error messages
    result.status = "unknown";
    PassParams params = inputParams; // local modifiable copy
    std::vector<ScreenProperties> screenProps;
    std::vector<LabelProperties> labels;
    //std::vector<DataEntryFieldProperties> EntryFields_;
    std::vector<DataEntryFieldProperties> DataEntryFields;
    std::vector<std::string> fieldNames;
    std::vector<BoxProperties> boxes;

    DataEntry::runClass_ = nextRunClass;

    std::vector<DataDictionaryFieldProperties> dataDictionaryFields;

    DataEntry::setupDataDictionaryFields(dataDictionaryFields, localDictionaryXmlFile, localDictionaryXmlFileName, debugFile);

    // when this function is called from TransactionEntry we
    // need to display header screen before prompting for trx type so  10/11/25
    //  setupScreen is called here. it is called again within the DataEntry.displayScreen below
    // because the DataEntry won't  prompt for trx type															
    DataEntry::setupScreen(screenProps, localScreenXmlFile, localScreenXmlFileName, debugFile);
    // the first element is not needed because it is emplaced with nulls																	
    if (!screenProps.empty()) {
        screenProps.erase(screenProps.begin());
    }

    debugFile << "inside DataEntry::run on line 104 before DataEntry::displayScreen" << std::endl;
    DataEntry::displayScreen(localScreenXmlFile, localScreenXmlFileName, screenProps, boxes, labels, DataEntryFields, debugFile);
    debugFile << "inside DataEntry::run on line 106 after displayScreen" << std::endl;

    localScreenXmlFile.clear(); // Clear any error flags
    localScreenXmlFile.seekg(0, std::ios::beg); // Clear any error flags
    // TODO: is this function really necessary?
    //validateScreenXml(localScreenXmlFile, localScreenXmlFileName, debugFile);

    // this line creates the boxes structure we have no real values to use yet
   // this fake elements will be removed and the real values will be added down futher in this function 															  
    boxes.emplace_back(0, 0, 0, 0);
    localScreenXmlFile.clear(); // Clear any error flags
    localScreenXmlFile.seekg(0, std::ios::beg);// Go back to the beginning of the file
    setupBoxes(boxes, localScreenXmlFile, localScreenXmlFileName, debugFile);
    // the first element is not needed because it is emplaced with nulls																	
    if (!boxes.empty()) {
        boxes.erase(boxes.begin());
    }

    setupDataEntryWindow(screenProps, debugFile);
    // this line creates the label structure we have no real values to use yet
    // this fake elements will be removed and the real values will be added down futher in this function 
    labels.emplace_back(0, 0, "", "", "", 0, 0, "", "", "", "");
    localScreenXmlFile.clear(); // Clear any error flags
    localScreenXmlFile.seekg(0, std::ios::beg); // Clear any error flags
    setupLabels(labels, localScreenXmlFile, localScreenXmlFileName, debugFile);
    if (!labels.empty()) {
        labels.erase(labels.begin());
    }
    // done in collect data instead
   //for (auto& label : labels) {
   //    DataEntry::displayLabels(label, debugFile);
   //}
    // this line creates the DataEntryField structure we have no real values to use yet
    // this fake elements will be removed and the real values will be added down futher in this function 
    //std::string hidden = "N";

    DataEntryFields.emplace_back(
        nullptr,        // winFormScreen
        nullptr,        // winMsgArea
        "",             // inputKeyPressed
        "",             // fieldName
        "",             // fieldType
        "",             // fieldMask
        0,              // len
        "",             // primaryKey
        0,              // fieldColumn
        0,              // row
        "",             // calc
        "",             // fieldValue
        "",             // allowedChoices
        "",             // choiceDescriptions
        "",             // inquiryAllowed
        "",             // inquiryOrderBy
        "",             // inquiryFields
        "",             // fieldRequired
        "N"            // isDisplayOnly
        //hidden          // hidden
    );
    localScreenXmlFile.clear();// Clear any error flags
    localScreenXmlFile.seekg(0, std::ios::beg);// Go back to the beginning of the file

    if (DataEntry::setupFields(DataEntryFields, localDictionaryXmlFile, localDictionaryXmlFileName, localScreenXmlFile, localScreenXmlFileName, debugFile) != 1) {
        result.status = "setupFields failed";
        return result;
    }
    // the first element is not needed because it is emplaced with nulls
    if (!DataEntryFields.empty()) {
        DataEntryFields.erase(DataEntryFields.begin());
    }
    mergeDataDictionaryWithDataEntryFields(DataEntryFields, dataDictionaryFields, debugFile);
    //setupDataEntryWindow(screenProps, debugFile);

     //std::vector<std::string> fieldNames;										  
    for (const auto& entry : DataEntryFields) {
        std::string value = entry.fieldName_;
        std::replace(value.begin(), value.end(), ' ', '_');// Replace spaces with underscores
        fieldNames.push_back(value);
    }
    //TODO: result.status=="OK" maybe hard coded in collectData for testing  chaining to next screen should we undo hard code?
    result.status = collectData(params, screenProps, boxes, labels, DataEntryFields, debugFile);

    headerRecord_ = this->getHeaderRecord();
    debugFile << "[DEBUG] Line 188 headerRecord_ values upon RETURN FROM collectData in DataEntry.cpp" << std::endl;
    debugFile << "------------------------------------------------------------------------------------" << std::endl;
    for (const auto& kv : headerRecord_) {
        debugFile << " kv.first= " << kv.first << " kv.second= " << kv.second << std::endl;
    }
    //for (auto& EntryField : DataEntryFields) {

    //    debugFile << "EntryField.fieldName_ = " << EntryField.fieldName_ << " EntryField.fieldValue_ = " << EntryField.fieldValue_ << std::endl;
    //}

    if (result.status == "HeaderSaved") {
        if (!screenProps[0].screenNextClass_.empty()) {

            if (screenProps[0].hasControl_ == "Y")
                result.nextRunClass = screenProps[0].controlClass_;
            // TODO: find way so this is not hard-coded
            result.nextRunType = "TransControl";// screenProps[0].;
        }
    }
    else if (result.status == "OK") {
        // 
        if (!screenProps[0].screenNextClass_.empty()) {
            result.nextRunType = screenProps[0].screenNextType_;
            result.nextRunClass = screenProps[0].screenNextClass_;

        }
    }
    else if (result.status == "Exit") {
        result.nextRunType = screenProps[0].screenExitRunType_;
        result.nextRunClass = screenProps[0].screenExitRunClass_;

        if (result.nextRunType.empty() || result.nextRunClass.empty())
        {
            result.nextRunType = "Menu";
            result.nextRunClass = "MainMenuScreen";
        }

    }
    /* headerRecord_.clear();
     for (const auto& EntryField : DataEntryFields) {
         headerRecord_[EntryField.fieldName_] = EntryField.fieldValue_;
     }*/

    result.values = params;
    return result;
}

std::string DataEntry::getTableName(std::ifstream& screenXmlFile, std::string screenXmlFileName, std::ofstream& debugFile) {
    if (!screenXmlFile.is_open()) {
        return "Error: screenXmlFile Not Open";
    }
    screenXmlFile.clear();// Clear any error flags
    screenXmlFile.seekg(0, std::ios::beg);// Go back to the beginning of the file

    std::string line = "";
    std::string tableName = "";
    bool tableOpenTagFound = false;
    int x = 0;
    while (std::getline(screenXmlFile, line) && tableOpenTagFound == false) {
        std::string msg = " line = " + line;
        //debugFile << line<< std::endl;
        x++;

        // Check if line contains the opening tag for Table
        if (line.find("<Table>") != std::string::npos) {
            tableOpenTagFound = true;
        }
        // Check if line contains the closing tag for Table
        if (line.find("</Table>") != std::string::npos) {
            tableOpenTagFound = false;
        }
        std::string tag, value;
        size_t startPos = line.find("<");
        size_t endPos = line.find(">");
        if (startPos != std::string::npos && endPos != std::string::npos) {
            tag = line.substr(startPos + 1, endPos - startPos - 1);
            tag.erase(0, tag.find_first_not_of(" \n\r\t"));
            tag.erase(tag.find_last_not_of(" \n\r\t") + 1);
            size_t valueStartPos = line.find(">", endPos);
            if (valueStartPos != std::string::npos) {
                value = line.substr(valueStartPos + 1);
                size_t valueEndPos = value.find("</" + tag + ">");
                if (valueEndPos != std::string::npos) {
                    value.erase(0, value.find_first_not_of(" \n\r\t"));
                    value.erase(value.find_last_not_of(" \n\r\t") + 1);
                    value = value.substr(0, valueEndPos);
                }
            }
            //debugFile << "Tag: " << tag << ", Value: " << value << std::endl;
            if (tag == "Table") {
                tableName = DataEntry::cleanString(value);
                //tableName = value;
            }
        }



    }
    return tableName;

}
// TODO: is this function really necessary?
//bool DataEntry::validateScreenXml(std::ifstream& screenXmlFile, const std::string& screenXmlFileName, std::ofstream& debugFile) {
//    debugFile << "inside  DataEntry::validateDataEntryXml  on line 279 " << "screenXmlFileName " << screenXmlFileName << std::endl;
//
//    if (!screenXmlFile.is_open()) {
//        return false;
//    }
//
//    std::string line;
//    bool dataDictionaryFound = false;
//    bool fieldOpenTagFound = false;
//    std::vector<std::string> fieldTags = {
//        "name", "type", "mask", "length", "row", "col", "defaultValue",
//        "label_column", "label_text", "AllowedChoices", "ChoiceDescriptions",
//        "inquiryAllowed", "fieldRequired","isDisplayOnly","isEvent"
//    };
//    int x = 0;
//    while (std::getline(screenXmlFile, line)) {
//        std::string msg = " line = " + line;
//        //debugFile << line<< std::endl;
//        x++;
//        /* if(x == 513) {
//             bool test = true;
//         }*/
//        if (line.find("<dataDictionary>") != std::string::npos) {
//            dataDictionaryFound = true;
//        }
//        // Check if line contains the opening tag for field
//        if (line.find("<field>") != std::string::npos) {
//            fieldOpenTagFound = true;
//        }
//        // Check if line contains the closing tag for field
//        if (line.find("</field>") != std::string::npos) {
//            fieldOpenTagFound = false;
//        }
//        // Check if line contains any field tags
//        for (const auto& tag : fieldTags) {
//            if (line.find("<" + tag + ">") != std::string::npos && !fieldOpenTagFound) {
//                debugFile << "Error: Found tag <" + tag + "> outside <field> element" << std::endl;
//                return false;
//            }
//        }
//        // Check if line contains the closing tag for dataDictionary
//        if (line.find("</dataDictionary>") != std::string::npos) {
//            if (!dataDictionaryFound) {
//                debugFile << "Error: Found </dataDictionary> without opening <dataDictionary> tag" << std::endl;
//                return false;
//            }
//            else {
//                return true; // XML validation successful
//            }
//        }
//    }
//    debugFile << "Error: Missing </dataDictionary> tag" << std::endl;
//    return false;
//}

int DataEntry::setupScreen(std::vector<ScreenProperties>& screenProps, std::ifstream& screenXmlFile, const std::string& screenXmlFileName, std::ofstream& debugFile) {
    debugFile << "enter DataEntry::setupScreen on line 335 screenXmlFileName is " << screenXmlFileName << std::endl;
    // this line creates the screenProps structure we have no real values to use yet
     //this fake elements will be removed and the real values will be added down futher in this function 
    if (screenProps.empty()) {

        screenProps.emplace_back("", "", 0, 0, 0, 0, "", "", "", "", "", "", "", "", "", 0, "", 0, 0, 0, 0);
    }
    screenXmlFile.clear(); // Clear any error flags
    screenXmlFile.seekg(0, std::ios::beg); // Go back to the beginning of the file




    // Define variables to store the properties of the screen
    std::string screenTitle = "";
    std::string screenType = "";
    std::string screenBorder = "";
    std::string screenDialog = "";
    int screencol = 0;
    int screenrow = 0;
    int screenWidth = 0;
    int screenHeight = 0;
    std::string screenNextType = "";
    std::string screenNextClass = "";
    std::string screenExitRunType = "";
    std::string screenExitRunClass = "";
    std::string screenHasControl = "";
    std::string screenControlClass = "";
    std::string scrollTable = "";
    std::string scrollKey = "";


    int scrollMaxRows = 0;
    int scrollWidth;
    int scrollHeight;
    int scrollStartY;
    int scrollStartX;



    // Ensure the XML file is open
    if (!screenXmlFile.is_open()) {
        debugFile << "Error inside DataEntry::setupScreen line 242 screenXmlFile is not open." << std::endl;
        return -1;
    }

    std::string line;
    while (std::getline(screenXmlFile, line)) {
        //debugFile << line << std::endl;
        // only planning on having one ScreenProperties element
        // if that increases then the next 2 lines would need to be uncommented
        //if (line.find("</ScreenProperties>") != std::string::npos)
            //break;
        if (line.find("</ScreenProperties>") != std::string::npos) {

            // At the end of a <Box> tag, push the collected properties into the vector
            screenProps.emplace_back(screenType, screenTitle, screencol, screenrow, screenWidth, screenHeight,
                screenBorder, screenDialog, screenNextType, screenNextClass,
                screenExitRunType, screenExitRunClass, screenHasControl, screenControlClass,
                scrollTable, scrollMaxRows, scrollKey,
                scrollWidth, scrollHeight, scrollStartY, scrollStartX);
            break;

            /* only planning on having one ScreenProperties element
            if that increases then the next 23 lines would need to be uncommented
            Reset the values
            std::string screenTitle = "";
            std::string screenType = "";
            std::string screenBorder = "";
            std::string screenDialog="";
            int screencol = 0;
            int screenrow = 0;
            int screenWidth = 0;
            int screenHeight = 0;
            std::string screenNextType = "";
            std::string screenNextClass = "";
            std::string screenExitRunType = "";
            std::string screenExitRunClass = "";
            std::string screenHasControl="";
            std::string  screenControlClass="";
            std::string  scrollTable = "";
            std::string scrollKey = "";
            int scrollMaxRows=0;
            int scrollWidth;
            int scrollHeight;
            int scrollStartY;
            int scrollStartX;
            */

        }

        std::string tag, value;
        size_t startPos = line.find("<");
        size_t endPos = line.find(">");
        if (startPos != std::string::npos && endPos != std::string::npos) {
            tag = line.substr(startPos + 1, endPos - startPos - 1);
            tag.erase(0, tag.find_first_not_of(" \n\r\t"));
            tag.erase(tag.find_last_not_of(" \n\r\t") + 1);

            size_t valueStartPos = line.find(">", endPos);
            if (valueStartPos != std::string::npos) {
                value = line.substr(valueStartPos + 1);
                size_t valueEndPos = value.find("</" + tag + ">");
                if (valueEndPos != std::string::npos) {
                    value.erase(0, value.find_first_not_of(" \n\r\t"));
                    value.erase(value.find_last_not_of(" \n\r\t") + 1);
                    value = value.substr(0, valueEndPos);
                }
            }
            //debugFile<< tag << std::endl;
            // Assign the parsed value to the corresponding screen property

            /* only planning on having one ScreenProperties element
            if that increases then the next 23 lines would need to be uncommented
            Reset the values
            std::string screenTitle = "";
            std::string screenType = "";
            std::string screenBorder = "";
            std::string screenDialog="";
            int screencol = 0;
            int screenrow = 0;
            int screenWidth = 0;
            int screenHeight = 0;
            std::string screenNextType = "";
            std::string screenNextClass = "";
            std::string screenExitRunType = "";
            std::string screenExitRunClass = "";
            std::string screenHasControl="";
            std::string  screenControlClass="";
            std::string  scrollTable = "";
            std::string scrollKey = "";
            std::string screenExitRunType = "";
            std::string screenExitRunClass = "";
            int scrollMaxRows=0;
            int scrollWidth;
            int scrollHeight;
            int scrollStartY;
            int scrollStartX;

            */
            if (tag == "Type") {
                screenType = value;
            }
            if (tag == "Title") {
                screenTitle = value;
            }
            if (tag == "Col") {
                screencol = parseInteger(value, 0);
            }
            else if (tag == "Row") {
                screenrow = parseInteger(value, 0);
            }

            else if (tag == "Width") {
                screenWidth = parseInteger(value, 0);
            }
            else if (tag == "Height") {
                screenHeight = parseInteger(value, 0);
            }
            else if (tag == "Border") {
                screenBorder = value;
            }

            else if (tag == "Dialog") {
                screenDialog = value;
            }
            else if (tag == "NextType") {
                screenNextType = value;
            }
            else if (tag == "NextClass") {
                screenNextClass = value;
            }
            else if (tag == "exitRunType") {
                screenExitRunType = value;
            }
            else if (tag == "exitRunClass") {
                screenExitRunClass = value;
            }
            else if (tag == "hasControl") {
                screenHasControl = value;
            }
            else if (tag == "controlClass") {
                screenControlClass = value;
            }
            else if (tag == "scrollTable") {
                scrollTable = value;
            }
            else if (tag == "scrollKey") {
                scrollKey = value;
            }
            else if (tag == "scrollMaxRows") {
                scrollMaxRows = parseInteger(value, 0);
            }
            else if (tag == "scrollWidth") {
                scrollWidth = parseInteger(value, 0);
            }
            else if (tag == "scrollHeight") {
                scrollHeight = parseInteger(value, 0);
            }
            else if (tag == "scrollstarty") {
                scrollStartY = parseInteger(value, 0);
            }
            else if (tag == "scrollstartx") {
                scrollStartX = parseInteger(value, 0);
            }

        }
    }
    return true;
}


int DataEntry::setupBoxes(std::vector<BoxProperties>& boxes, std::ifstream& screenXmlFile, const std::string& screenXmlFileName, std::ofstream& debugFile) {
    debugFile << "inside  DataEntry::setupBoxes  on line 541 " << "screenXmlFileName " << screenXmlFileName << std::endl;
    //boxes.clear();
    bool inBoxes = false;
    // Define variables to store the properties of each box
    int col = 0;
    int row = 0;
    int cols = 0;
    int rows = 0;

    // Ensure the XML file is open
    if (!screenXmlFile.is_open()) {
        debugFile << "Error inside DataEntry::setupBoxes line 502 screenXmlFile is not open." << std::endl;
        return -1;
    }

    std::string line;
    while (std::getline(screenXmlFile, line)) {
        if (line.find("</Boxes>") != std::string::npos) {
            inBoxes = true;
            break;
        }
        //debugFile << line << std::endl;
        if (line.find("</Boxes>") != std::string::npos)
            break;
        //if (inBoxes == true && line.find("</Box>") != std::string::npos) {
        if (line.find("</Box>") != std::string::npos) {
            // At the end of a <Box> tag, push the collected properties into the vector
            boxes.emplace_back(col, row, cols, rows);

            // Reset the values for the next box
            col = 0;
            row = 0;
            cols = 0;
            rows = 0;
        }

        std::string tag, value;
        size_t startPos = line.find("<");
        size_t endPos = line.find(">");
        if (startPos != std::string::npos && endPos != std::string::npos) {
            tag = line.substr(startPos + 1, endPos - startPos - 1);
            tag.erase(0, tag.find_first_not_of(" \n\r\t"));
            tag.erase(tag.find_last_not_of(" \n\r\t") + 1);

            size_t valueStartPos = line.find(">", endPos);
            if (valueStartPos != std::string::npos) {
                value = line.substr(valueStartPos + 1);
                size_t valueEndPos = value.find("</" + tag + ">");
                if (valueEndPos != std::string::npos) {
                    value.erase(0, value.find_first_not_of(" \n\r\t"));
                    value.erase(value.find_last_not_of(" \n\r\t") + 1);
                    value = value.substr(0, valueEndPos);
                }
            }
            //debugFile<< tag << std::endl;
            // Assign the parsed value to the corresponding box property
            if (tag == "Row") {
                row = parseInteger(value, 0);
            }
            else if (tag == "Col") {
                col = parseInteger(value, 0);
            }

            else if (tag == "Cols") {
                cols = parseInteger(value, 0);
            }
            else if (tag == "Rows") {
                rows = parseInteger(value, 0);
            }
        }
    }

    return true;
}
bool DataEntry::setupLabels(std::vector<LabelProperties>& labels, std::ifstream& screenXmlFile, const std::string& screenXmlFileName, std::ofstream& debugFile) {
    debugFile << "inside  DataEntry::setupLabels  on line 615 " << "screenXmlFileName " << screenXmlFileName << std::endl;
    int row = 0;
    int col = 0;
    std::string labelText = "";
    std::string labelFieldName = "";
    std::string labelParmName = "";
    int labelFieldCol = 0;
    int labelFieldRow = 0;
    std::string choiceValues = "";
    std::string choiceDescriptions = "";
    std::string labelFieldWHERE = "";
    std::string labelFieldSQL = "";

    //std::vector<std::string> labelNames;

    //debugFile << "inside setupLabels " << std::endl;
    if (!screenXmlFile.is_open()) {
        debugFile << "Error inside DataEntry::setupLabels line 625 screenXmlFile is not open." << std::endl;
        return false;
    }
    // Read data from the file and initialize DataEntry Label objects
    std::string line;
    while (std::getline(screenXmlFile, line)) {
        int x = 0;

        x = line.find("</Label>");
        if (x != std::string::npos) {
            // We're at the end of the field XML element, so add the field to the vector
            if (!labelFieldSQL.empty()) {
                if (!labelFieldSQL.empty()) {
                    bool test = 0;
                }
            }

            if (!labelText.empty()) {
                if (!choiceDescriptions.empty()) {
                    bool test = 0;
                }
                labels.emplace_back(col, row, labelText, labelFieldName, labelParmName, labelFieldCol,
                    labelFieldRow, choiceValues, choiceDescriptions, labelFieldWHERE, labelFieldSQL);
                col = 0;
                row = 0;
                labelText = "";
                labelFieldName = "";
                labelParmName = "";
                labelFieldCol = 0;
                labelFieldRow = 0;
                choiceValues = "";
                choiceDescriptions = "";
                labelFieldWHERE = "";
                labelFieldSQL = "";
                continue;
            }
        }
        std::string tag, value;
        size_t startPos = line.find("<");
        size_t endPos = line.find(">");
        if (startPos != std::string::npos && endPos != std::string::npos) {
            tag = line.substr(startPos + 1, endPos - startPos - 1);
            tag.erase(0, tag.find_first_not_of(" \n\r\t"));
            tag.erase(tag.find_last_not_of(" \n\r\t") + 1);
            size_t valueStartPos = line.find(">", endPos);
            if (valueStartPos != std::string::npos) {
                value = line.substr(valueStartPos + 1);
                size_t valueEndPos = value.find("</" + tag + ">");
                if (valueEndPos != std::string::npos) {
                    value.erase(0, value.find_first_not_of(" \n\r\t"));
                    value.erase(value.find_last_not_of(" \n\r\t") + 1);
                    value = value.substr(0, valueEndPos);
                }
            }
            //debugFile << "Tag: " << tag << ", Value: " << value << std::endl;
            if (tag == "Row") {
                row = parseInteger(value, 0);
            }
            else if (tag == "Col") {
                col = parseInteger(value, 0);
            }
            if (tag == "labelText") {
                labelText = DataEntry::cleanString(value);
                labelText = value;
            }

            else if (tag == "labelFieldName") {
                labelFieldName = DataEntry::cleanString(value);
                labelFieldName = value;
            }
            else if (tag == "labelParmName") {
                labelParmName = DataEntry::cleanString(value);
                labelParmName = value;
            }
            else if (tag == "labelFieldCol") {
                labelFieldCol = parseInteger(value, 0);
            }
            else if (tag == "labelFieldRow") {
                labelFieldRow = parseInteger(value, 0);
            }
            else if (tag == "choiceValues") {
                choiceValues = DataEntry::cleanString(value);
            }
            else if (tag == "choiceDescriptions") {
                choiceDescriptions = DataEntry::cleanString(value);
            }
            else if (tag == "labelFieldSQL") {
                labelFieldSQL = DataEntry::cleanString(value);
            }

            else if (tag == "labelFieldWHERE") {
                labelFieldWHERE = DataEntry::cleanString(value);
            }
        }
    }
    return true;
}
int DataEntry::setupDataDictionaryFields(
    std::vector<DataDictionaryFieldProperties>& dictionaryFields,
    std::ifstream& dictionaryXmlFile,
    const std::string& dictionaryXmlFileName,
    std::ofstream& debugFile)
{
    debugFile << "inside DataEntry::setupDataDictionaryFields for " << dictionaryXmlFileName << std::endl;

    if (!dictionaryXmlFile.is_open()) {
        debugFile << "Error: XML file not open in setupDataDictionaryFields." << std::endl;
        return false;
    }
    dictionaryXmlFile.clear(); // clear EOF flag
    dictionaryXmlFile.seekg(0); // rewind to beginning

    std::string line;
    std::string fieldName = "";
    std::string primaryKey = "N";
    std::string fieldType = "";
    std::string fieldMask = "";
    int length = 0;
    std::vector<std::pair<std::string, std::string>> columns;
    //std::string query = "";
    //std::string keys = "";

    while (std::getline(dictionaryXmlFile, line)) {
        debugFile << line << std::endl;
        if (line.find("<field>") != std::string::npos) {
            // Reset for new field
            fieldName = "";
            primaryKey = "N";
            fieldType = "";
            fieldMask = "";
            length = 0;
            continue;
        }

        if (line.find("</field>") != std::string::npos) {

            // End of <field> - push to vector
            if (!fieldName.empty()) {
                dictionaryFields.emplace_back(fieldName, fieldType, primaryKey, fieldMask, length);
            }
            continue;
        }

        std::string tag, value;
        size_t startPos = line.find("<");
        size_t endPos = line.find(">");
        if (startPos != std::string::npos && endPos != std::string::npos) {
            tag = line.substr(startPos + 1, endPos - startPos - 1);
            tag.erase(0, tag.find_first_not_of(" \t\r\n"));
            tag.erase(tag.find_last_not_of(" \t\r\n") + 1);

            size_t valueStartPos = line.find(">", endPos);
            if (valueStartPos != std::string::npos) {
                value = line.substr(valueStartPos + 1);
                size_t valueEndPos = value.find("</" + tag + ">");
                if (valueEndPos != std::string::npos) {
                    value = value.substr(0, valueEndPos);
                    value.erase(0, value.find_first_not_of(" \t\r\n"));
                    value.erase(value.find_last_not_of(" \t\r\n") + 1);
                }
            }

            if (tag == "name") {
                fieldName = value;
            }

            else if (tag == "type") {
                fieldType = value;
            }
            else if (tag == "primaryKey") {
                primaryKey = value;
            }
            else if (tag == "mask") {
                fieldMask = value;
            }
            else if (tag == "length") {
                length = parseInteger(value, 0);
            }
        }

        debugFile << "*** fieldName " << fieldName << std::endl;
    }
    for (size_t i = 0; i < dictionaryFields.size(); ++i) {

        columns.emplace_back(dictionaryFields[i].fieldName, dictionaryFields[i].primaryKey);
    }

    if (!dictionaryFields.empty()) {
        //dictionaryFields.erase(dictionaryFields.begin());
        dbLib_.createTable(tableName_, columns);
        debugFile << "setupDataDictionaryFields completed with " << dictionaryFields.size() << " fields loaded." << std::endl;
        return 1; //sucess
    }
    else
        return -3; // dictionaryFields names vector is empty

}
//    bool DataEntry::setupDataEntryFields(std::vector<DataEntryFieldProperties>&DataEntryFields,std::ifstream & screenXmlFile,const std::string & screenXmlFileName,std::ofstream & debugFile)
//    {
//    debugFile << "inside DataEntry::setupDataEntryFields for " << screenXmlFileName << std::endl;
//
//    if (!screenXmlFile.is_open()) {
//        debugFile << "Error: XML file not open in setupDataEntryFields." << std::endl;
//        return false;
//    }
//
//   
//    std::string line,tag, value;
//    DataEntryFieldProperties field;
//    
//    while (std::getline(screenXmlFile, line)) {
//        if (line.find("<field>") != std::string::npos) {
//            field = DataEntryFieldProperties();  // reset
//            continue;
//        }
//
//        if (line.find("</field>") != std::string::npos) {
//            if (!field.fieldName_.empty()) {
//                DataEntryFields.emplace_back(field);
//            }
//            continue;
//        }
//
//
//        value = "";
//        size_t startPos = line.find("<");
//        size_t endPos = line.find(">");
//        if (startPos != std::string::npos && endPos != std::string::npos) {
//            tag = line.substr(startPos + 1, endPos - startPos - 1);
//            tag.erase(0, tag.find_first_not_of(" \t\r\n"));
//            tag.erase(tag.find_last_not_of(" \t\r\n") + 1);
//
//            size_t valueStartPos = line.find(">", endPos);
//            if (valueStartPos != std::string::npos) {
//                value = line.substr(valueStartPos + 1);
//                size_t valueEndPos = value.find("</" + tag + ">");
//                if (valueEndPos != std::string::npos) {
//                    value = value.substr(0, valueEndPos);
//                    value.erase(0, value.find_first_not_of(" \t\r\n"));
//                    value.erase(value.find_last_not_of(" \t\r\n") + 1);
//                }
//            }
//            if (tag == "name" && value == "TTYP") {
//                bool test = true;
//            }
//            
//            // Assign values to DataEntryFieldProperties members
//            if (tag == "name") field.fieldName_ = value;
//            else if (tag == "type") field.fieldType_ = value;
//            else if (tag == "mask") field.fieldMask_ = value;
//            else if (tag == "length") field.len_ = parseInteger(value,0);
//            else if (tag == "row") field.row_ = parseInteger(value,0);
//            else if (tag == "col") field.fieldColumn_ = parseInteger(value,0);
//            else if (tag == "calc") field.calc_ = value;
//            else if (tag == "defaultValue") field.fieldValue_ = value;
//            else if (tag == "AllowedChoices") field.allowedChoices_ = value;
//            else if (tag == "ChoiceDescriptions") field.choiceDescriptions_ = value;
//            else if (tag == "inquiryAllowed") field.inquiryAllowed_ = value;
//            else if (tag == "inquiryOrderBy") field.inquiryOrderBy_ = value;
//            else if (tag == "inquiryFields") field.inquiryFields_ = value;
//            else if (tag == "fieldRequired") field.fieldRequired_ = value;
//            else if (tag == "isDisplayOnly") field.isDisplayOnly_ = value;
//            
//        }
//    }
//
//    debugFile << "setupDataEntryFields completed with " << DataEntryFields.size() << " fields loaded." << std::endl;
//    return true;
//}
void DataEntry::mergeDataDictionaryWithDataEntryFields(
    std::vector<DataEntryFieldProperties>& DataEntryFields,
    const std::vector<DataDictionaryFieldProperties>& dictionaryFields,
    std::ofstream& debugFile)
{
    debugFile << "Merging Data Dictionary into Control Fields..." << std::endl;

    for (auto& DataEntryField : DataEntryFields) {
        if (DataEntryField.fieldName_ == "CUST") {
            debugFile << "fieldName_" << DataEntryField.fieldName_;
        }
        // Find the corresponding dictionary entry
        auto it = std::find_if(dictionaryFields.begin(), dictionaryFields.end(),
            [&DataEntryField](const DataDictionaryFieldProperties& dictField) {
                return dictField.fieldName == DataEntryField.fieldName_;
            });

        if (it != dictionaryFields.end()) {
            const DataDictionaryFieldProperties& dictField = *it;

            // Always set primaryKey from dictionary
            DataEntryField.primaryKey_ = dictField.primaryKey;


            if (DataEntryField.fieldType_.empty()) {
                DataEntryField.fieldType_ = dictField.fieldType;
            }
            if (DataEntryField.fieldMask_.empty()) {
                DataEntryField.fieldMask_ = dictField.fieldMask;
            }
            if (DataEntryField.len_ == 0) {
                DataEntryField.len_ = dictField.length;
            }

            debugFile << "Merged field: " << DataEntryField.fieldName_
                << " [fieldType=" << DataEntryField.fieldType_
                << " [mask=" << DataEntryField.fieldMask_
                << ", len=" << DataEntryField.len_
                << ", primaryKey=" << DataEntryField.primaryKey_
                << "]" << std::endl;
        }
        else {
            debugFile << "Warning: No matching dictionary field found for " << DataEntryField.fieldName_ << std::endl;
        }
    }

    debugFile << "Merging complete." << std::endl;
}

int DataEntry::setupFields(std::vector<DataEntryFieldProperties>& DataEntryFields, std::ifstream& dictionaryXmlFile, const std::string& dictionaryXmlFileName, std::ifstream& screenXmlFile, const std::string& screenXmlFileName, std::ofstream& debugFile) {
    debugFile << "inside  DataEntry::setupFields  on line 954 " << "screenXmlFileName " << screenXmlFileName << std::endl;

    std::string fieldName = "";
    std::string fieldType = "";
    std::string fieldMask = "";
    int len = 0;
    std::string primaryKey = "N";
    int col = 0;
    int row = 0;
    std::string calc = "";
    std::string defaultValue = ""; // not sure if this is needed or maybe name should be fieldValue
    std::string allowedChoices = "";
    std::string choiceDescriptions = "";
    std::string inquiryAllowed = "";
    std::string inquiryOrderBy = "";
    std::string inquiryFields = "";
    std::string Required = "";
    std::string isDisplayOnly = "N";
    //bool askTrxType = true;
    //std::vector<std::string> fieldNames;
    std::vector<std::pair<std::string, std::string>> columns;
    std::string query = "";
    std::string keys = "";
    WINDOW* winFormScreen = SharedWindowManager::getwinFormScreen();
    WINDOW* winMsgArea = SharedWindowManager::getwinMsgArea();
    //debugFile << "inside SetupFields " << std::endl;
    if (!screenXmlFile.is_open()) {
        debugFile << "Error: Unable to open XML file. " << screenXmlFileName << std::endl;
        return -1;
    }
    bool inFields = false;
    bool found = false;
    std::string line, tag;
    DataEntryFieldProperties field;

    while (std::getline(screenXmlFile, line)) {
        if (line.find("<field>") != std::string::npos) {
            bool inFields = true;
            //debugFile << "setupFields Outter while inFields became true line = " << line << std::endl;
            // Read data from the file and initialize DataEntry objects
            while (std::getline(screenXmlFile, line)) {
                if (line.find("<field>") != std::string::npos) {
                    if (inFields == false) {
                        inFields = true;
                        //debugFile << "setupFields Inner while inFields became trueLine = " << line << std::endl;
                    }
                }

                if (inFields == true) {

                    debugFile << "setupFields Inner while inFields == true.  Line = " << line << " fieldName= " << fieldName << std::endl;

                    if (line.find("</field>") != std::string::npos) {

                        // We're at the end of the field XML element, so add the field to the vector
                        if (!fieldName.empty()) {
                            //fieldNames.emplace_back(fieldName);


                            debugFile << "DataEntryFields.emplace_back  fieldName = " << fieldName << ", disDisplayOnly: " << isDisplayOnly << std::endl;

                            DataEntryFields.emplace_back(
                                winFormScreen,          // WINDOW*
                                winMsgArea,             // WINDOW*
                                "",                     // inputKeyPressed
                                fieldName,              // fieldName_
                                fieldType,              // fieldType_
                                fieldMask,              // fieldMask_
                                len,                    // len_
                                primaryKey,             // primaryKey_
                                row,                    // row_
                                col,                    // fieldColumn_                                
                                calc,                   // calc_
                                defaultValue,           // fieldValue_
                                allowedChoices,         // allowedChoices_
                                choiceDescriptions,     // choiceDescriptions_
                                inquiryAllowed,         // inquiryAllowed_
                                inquiryOrderBy,         // inquiryOrderBy_
                                inquiryFields,          // inquiryFields_
                                Required,          // fieldRequired_
                                isDisplayOnly     // isDisplayOnly_
                                //hidden                  // hidden_
                            );

                            tag = "";
                            fieldName = "";
                            fieldType = "";
                            fieldMask = "";
                            len = 0;
                            primaryKey = "N";
                            col = 0;
                            row = 0;
                            calc = "";
                            defaultValue = ""; // not sure if this is needed or maybe name should be fieldValue
                            allowedChoices = "";
                            choiceDescriptions = "";
                            inquiryAllowed = "";
                            inquiryOrderBy = "";
                            inquiryFields = "";
                            Required = "";
                            isDisplayOnly = "";
                            //std::string hidden = "";
                            //bool askTrxType = true;

                            inFields = false;
                            continue;
                        }
                    }


                    std::string tag, value;
                    size_t startPos = line.find("<");
                    size_t endPos = line.find(">");
                    if (startPos != std::string::npos && endPos != std::string::npos) {
                        tag = line.substr(startPos + 1, endPos - startPos - 1);
                        tag.erase(0, tag.find_first_not_of(" \n\r\t"));
                        tag.erase(tag.find_last_not_of(" \n\r\t") + 1);
                        size_t valueStartPos = line.find(">", endPos);
                        if (valueStartPos != std::string::npos) {
                            value = line.substr(valueStartPos + 1);
                            size_t valueEndPos = value.find("</" + tag + ">");
                            if (valueEndPos != std::string::npos) {
                                value.erase(0, value.find_first_not_of(" \n\r\t"));
                                value.erase(value.find_last_not_of(" \n\r\t") + 1);
                                value = value.substr(0, valueEndPos);
                            }
                        }


                        debugFile << "_______setupFields Tag: " << tag << ", Value: " << value << std::endl;
                        value = DataEntry::cleanString(value);
                        if (tag == "name") fieldName = value;
                        else if (tag == "type") fieldType = value;
                        else if (tag == "mask") fieldMask = value;
                        else if (tag == "length") len = parseInteger(value, 0);
                        else if (tag == "row") row = parseInteger(value, 0);
                        else if (tag == "col") col = parseInteger(value, 0);
                        else if (tag == "calc") calc = value;
                        else if (tag == "defaultValue") defaultValue = value;
                        else if (tag == "AllowedChoices") allowedChoices = value;
                        else if (tag == "ChoiceDescriptions") choiceDescriptions = value;
                        else if (tag == "inquiryAllowed") inquiryAllowed = value;
                        else if (tag == "inquiryOrderBy") inquiryOrderBy = value;                        else if (tag == "inquiryFields") field.inquiryFields_ = value;
                        else if (tag == "fieldRequired")  Required = value;
                        else if (tag == "isDisplayOnly") isDisplayOnly = value;
                    }
                }
                else {
                }
            }



        }


    }
    debugFile << "setupFields completed with " << DataEntryFields.size() << " fields loaded." << std::endl;
    return 1; //sucess

}

int DataEntry::setupDataEntryWindow(std::vector<ScreenProperties>& screenProps, std::ofstream& debugFile) {

    //int stdscrRows = 0;
    //int stdscrCols = 0;
    //getmaxyx(stdscr, stdscrRows, stdscrCols);    
    int stdscrRows = screenProps[0].screenHeight_;
    int stdscrCols = screenProps[0].screenWidth_;
    WINDOW* winFormScreen = SharedWindowManager::getwinFormScreen(screenProps);
    WINDOW* winMsgArea = SharedWindowManager::getwinMsgArea();

    return 0;
}
//std::string DataEntry::collectData(std::vector<BoxProperties>& boxes, std::vector<LabelProperties>& labels, std::vector<DataEntryFieldProperties>& EntryFields_, std::ofstream& debugFile, std::function<void(std::ofstream&)> customLogic) {
    ////TODO: this shows a pop up to pick what they want to do 
    //// Call custom logic, if provided
    //if (customLogic) {
    //    customLogic(debugFile);
    //}

void DataEntry::setEditRecord(const std::map<std::string, std::string>& rec) {
    editRecord_ = rec;
    hasEditRecord_ = !editRecord_.empty();
}

std::string DataEntry::collectData(PassParams& passParams,
    std::vector<ScreenProperties>& screenProps,
    std::vector<BoxProperties>& boxes, std::vector<LabelProperties>& labels,
    std::vector<DataEntryFieldProperties>& DataEntryFields, std::ofstream& debugFile) {

    debugFile << "entering   dataEntryScreen_.collectData EntryField values in DataEntry.cpp" << std::endl;
    debugFile << "------------------------------------------------------------------------------------" << std::endl;
    for (auto& EntryField : DataEntryFields) {
        debugFile << "------------------------------------------------------------------------------------" << std::endl;
        debugFile << "EntryField.fieldName_ = " << EntryField.fieldName_ << " EntryField.isDisplayOnly_ = " << EntryField.isDisplayOnly_ << " EntryField.fieldValue_ = " << EntryField.fieldValue_ << std::endl;
    }
    //debugFile << this->EntryFields_.size();

    getmaxyx(stdscr, stdscrRows_, stdscrCols_);
    std::vector<std::string> fieldNames;
    std::string Result = "";
    int functionKeyRow = stdscrRows_ - 2;
    int functionKeyCount = 5;
    bool firstTime = true, quit = false;
    bool EditingRecord = false, AddingNew = false, prompt = false, readRecord = false;
    int test = 0;
    std::string action = "";
    std::string msg = "";
    std::string temp = "";
    std::string condition = "";
    int lastPrimaryKeyIndex = 0;
    size_t  index = 0;
    std::vector<std::string>  fieldValues;
    fieldValues.push_back("");
    fieldValues.clear();

    std::map<std::string, std::string> dummyrec;
    std::map<std::string, std::string> row; // possible dummy needed to use displayLabels

    WINDOW* winFormScreen = SharedWindowManager::getwinFormScreen();
    WINDOW* winMsgArea = SharedWindowManager::getwinMsgArea();

    for (const auto& entry : DataEntryFields) {
        std::string value = entry.fieldName_;
        std::replace(value.begin(), value.end(), ' ', '_');  // Replace spaces with underscores
        fieldNames.push_back(value);
    }
RecordEntryStart:
    if (runClass_.find("HeaderScreen") != std::string::npos && (Result == "HeaderSaved"))
    {
        goto Exit;
    }
    else if (runClass_.find("HeaderScreen") != std::string::npos && Result == "HeaderDeleted")
    {
        //TODO: ???
        //Result = "HeaderDeleted";
    }
    //Result = "";

    wclear(winFormScreen);
    wattron(winFormScreen, COLOR_PAIR(3));
    wbkgd(winFormScreen, COLOR_PAIR(3));
    box(winFormScreen, ACS_VLINE, ACS_HLINE);
    drawBoxes(boxes);
    displayFkeys();
    //TODO: need to find spot to set  screenTitle_ value 
    screenProps[0].screenTitle_ = "TEST SCREEN TITLE";

    //DataEntry::printInMiddle(winFormScreen, 0, 0, 0, screenProps[0].screenTitle_, COLOR_PAIR(3), debugFile);
    //functionKeyRow = stdscrRows_ - 2;
    //functionKeyCount = 5;
    firstTime = true, quit = false;
    EditingRecord = false, AddingNew = false, prompt = false, readRecord = false;
    test = 0;
    msg = "";
    temp = "";
    condition = "";
    action = "";
    lastPrimaryKeyIndex = 0;


    for (auto& label : labels) {
        DataEntry::displayLabels(winFormScreen, row, label, passParams, debugFile);
    }


    wrefresh(winFormScreen);
    index = 0;
    for (auto& EntryField : DataEntryFields) {
        // Assign  value from parameter if provided
        if (passParams.fields.find(EntryField.fieldName_) != passParams.fields.end()) {

            EntryField.fieldValue_ = passParams.fields.at(EntryField.fieldName_).value;
        }
        // Here is the place to set variables for based on the info about the EntryFields_ that will be useful for the rest of this function
        // TODO: need to deal with when there is a composite key
        if (EntryField.primaryKey_ == "Y") {
            lastPrimaryKeyIndex = index;
        }
        index++;

    }

    wrefresh(winFormScreen);
    index = 0;
    //---------------------------------------------------------------------------------------
    if (hasEditRecord_) {
        // Build WHERE from PK fields using values from editRecord_
        condition.clear();
        bool haveAnyPK = false;
        bool first = true;
        for (const auto& f : DataEntryFields) {
            if (f.primaryKey_ == "Y" || f.primaryKey_ == "y") {
                haveAnyPK = true;
                auto it = editRecord_.find(f.fieldName_);
                if (it != editRecord_.end() && !it->second.empty()) {
                    if (!first) condition += " AND ";
                    condition += f.fieldName_ + " = '" + it->second + "'";
                    first = false;
                }
            }
        }

        if (haveAnyPK && !condition.empty()) {
            bool AddingNew_local = false;
            bool prompt_local = false;  // critical: do NOT prompt for PK

            std::string res = DataEntry::readRecord(DataEntryFields, tableName_, AddingNew_local, prompt_local, dbLib_, condition, passParams, debugFile);

            if (res == "EditingRecord" || res == "OK" || res.empty()) {
                EditingRecord = true;
                index = lastPrimaryKeyIndex + 1;   // jump to first non-PK field
                goto doDisplayData;
            }
            if (res == "RecordNotFound") {
                // Treat like add; PKs can remain in place and proceed normally
                AddingNew = true;
                EditingRecord = false;
                index = lastPrimaryKeyIndex + 1;
                //TODO:find way to not hard code
                if (runClass_.find("HeaderScreen") != std::string::npos && DataEntryFields[0].fieldName_ == ("TTYP")) {
                    DataEntryFields[0].fieldValue_ = passParams.fields["TTYP"].value;
                }
                // fall through into normal loop
            }
            if (res == "Exit")  goto Exit;
            if (res == "Error") goto handleError;
        }
    }

    while (index < DataEntryFields.size()) {
    BeginField:
        displayFkeys();
        wattroff(winFormScreen, A_REVERSE);
        wrefresh(winFormScreen);
        // TODO: change to loop back around to first non primary index EntryField
        if (index >= DataEntryFields.size()) index = 1;
        // test to prevent out of range error/core dump. 
        // this this is only necessay because I am using a goto statement within the while
        // I don't know how to avoid using the goto in this case
        if (index > DataEntryFields.size()) index--;
        auto& EntryField = DataEntryFields[index];


        // Skip input if the EntryField is display-only
        if (EntryField.isDisplayOnly_ == "N") {


            //**********ACCEPT INPUT ************
            bool FunctionKey = acceptInput(EntryField, passParams, debugFile);
            debugFile << "[acceptInput] after input, fieldName = " << EntryField.fieldName_ << " fieldValue_ = " << EntryField.fieldValue_ << "fieldValue_ .length " << EntryField.fieldValue_.length() << std::endl;
            // TODO: CHANGE THIS TO IMPLEMENT CALL BACK FEATURE when written                 
            /*if (EntryField.isEvent) {

            }*/
            // TODO: should i have condition this only is done if coming from TransactionEntryHeader
            if (EntryField.fieldName_ == screenProps[0].scrollKey_ && passParams.fields.find(screenProps[0].scrollKey_) != passParams.fields.end()) {

                scrollKey_ = EntryField.fieldValue_;
            }



            InputKeyPressed = EntryField.inputKeyPressed_;
            // FunctionKey is true for enter key. May have to revist this decision
            if (!FunctionKey)
                goto notFunctionKey;

            temp = DataEntry::cleanString(EntryField.fieldValue_);

            if (InputKeyPressed == "KEY_F(2)" && EntryField.inquiryAllowed_ == "N") {
                goto BeginField;
            }
            if (InputKeyPressed == "KEY_F(2)" || InputKeyPressed == "KEY_F(5)" || InputKeyPressed == "KEY_F(7)")
                goto functionKeys;

            // primary key field  can't allow blank value           
            if (EntryField.primaryKey_ == "Y" && temp == "") {
                goto BeginField;
            }

            EntryField.fieldValue_ = temp;
            fieldValues.push_back(temp);
            wrefresh(winFormScreen);
        functionKeys:

            // not sure why i decided to classify the enter key as a function key.
            // apparently necessary at the time
            if (InputKeyPressed != "KEY_ENTER") {
                //std::string inquiryOrderBy = EntryField.inquiryOrderBy; 

                Result = DataEntry::doFunctionKeys(EntryField, tableName_, AddingNew, dbLib_, condition, EntryField.inquiryOrderBy_, EntryField.inquiryFields_, DataEntryFields, debugFile, InputKeyPressed);
                if (Result == "RecordSaved") {
                    std::map<std::string, std::string> tempHeaderRecord;

                    if (runClass_.find("HeaderScreen") != std::string::npos)
                    {
                        Result = "HeaderSaved";
                        // Back up the current EntryField values so we can save them later


                        debugFile << "[DEBUG] line 1281 headerRecord_ updated before tempHeaderRecord:" << std::endl;
                        for (const auto& kv : headerRecord_) {
                            debugFile << "  " << kv.first << " = " << kv.second << std::endl;
                        }
                        for (const auto& EntryField : DataEntryFields) {
                            tempHeaderRecord[EntryField.fieldName_] = EntryField.fieldValue_;
                        }

                        headerRecord_.clear();
                        for (const auto& EntryField : DataEntryFields) {

                            headerRecord_[EntryField.fieldName_] = tempHeaderRecord[EntryField.fieldName_];//EntryField.fieldValue_;
                            headerRecord_ = tempHeaderRecord;
                        }
                    }
                    debugFile << "[DEBUG] Line 1296 headerRecord_ updated before clearing EntryFields_:" << std::endl;
                    for (const auto& kv : headerRecord_) {
                        debugFile << "  " << kv.first << " = " << kv.second << std::endl;
                    }
                    for (auto& EntryField : DataEntryFields) {
                        EntryField.fieldValue_ = "";
                    }
                    headerRecord_ = tempHeaderRecord;
                    goto  RecordEntryStart; // displayData; 
                }
                if (Result == "RecordEntryStart") {

                    if (runClass_.find("HeaderScreen") != std::string::npos)
                    {
                        headerRecord_.clear();
                    }
                    for (auto& EntryField : DataEntryFields) {
                        EntryField.fieldValue_ = "";
                    }

                    goto  RecordEntryStart; // displayData; 
                }

                if (Result == "Exit")
                    goto Exit;
                if (Result == "Error")
                    goto FunctionKeyError;
                // JRG 2/21/25
                if (temp == "RecordNotFound") {
                    goto BeginField;
                }
                // Empty means F2 was used but no record was selected
                if (Result == "Empty") {

                    goto  RecordEntryStart; // displayData; 
                }
                //TryReadRecord means F2 was used to lookup a record
                // condition set above contains the key to read
                if (Result == "TryReadRecord") {
                    goto jumpToENTER_KEY;
                }

            }




        notFunctionKey:
            temp = EntryField.fieldValue_;
            test = DataEntry::stringSwitchNumber(EntryField.inputKeyPressed_);
            // -99 indicating key code we are checking isn't in the list i'm expecting
            if (test != -99) {
                switch (test) {
                case -1:  //"KEY_UP"
                    if (index > 1) {
                        --index;
                        EntryField.fieldValue_ = temp;
                        fieldValues.push_back(temp);
                        std::string type = EntryField.fieldType_;
                        DataEntry::displayData(EntryField, debugFile);
                        if (index < DataEntryFields.size()) {
                            auto& EntryField = DataEntryFields[index];
                            std::string tempnam = EntryField.fieldName_;
                            std::string tempval = EntryField.fieldValue_;
                            //debugFile << "index = " << index << " tempnam = " << tempnam << " tempval " << tempval << std::endl;
                            goto BeginField;
                        }
                    }
                    else
                        index = 1;
                    goto BeginField;
#pragma region //start of code for 'KEY_ENTER' case
                case 0: //KEY_ENTER
                case PLATFORM_ENTER_KEY:
                case 16://KEY_ENTER under special circumstance not sure why getch returned 16
                        // here we can check the value input by the user after they press enter
                jumpToENTER_KEY:
                    if (EntryField.fieldRequired_ == "Y") {
                        temp = DataEntry::cleanString(EntryField.fieldValue_);
                        if (temp == "") {
                            std::string msg = "Entry is required";
                            std::string action = "";
                            bool fKey = DataEntry::errMsg(2, stdscrCols_, action, msg, debugFile);

                            if (fKey == true && action == "KEY_F(7)") {
                                if (DataEntry::confirmAction(2, 2, stdscrCols_, "Exit", COLOR_PAIR(1), KEY_F(7), debugFile) == true)
                                {
                                    goto Exit;
                                }
                            }
                            goto BeginField;
                        }
                    }

                    if (EntryField.primaryKey_ == "Y" && !fieldNames.empty() && EntryField.fieldName_ == fieldNames[index]) {
                        condition.append(fieldNames[index] + " = \'" + DataEntryFields[index].fieldValue_ + "\'");
                        if (index != lastPrimaryKeyIndex) {
                            condition.append(" AND ");
                        }
                    }



                    if (index == lastPrimaryKeyIndex) {
                        Result = "";
                        AddingNew = false;
                        EditingRecord = false;
                        bool prompt = true;


                        Result = DataEntry::readRecord(DataEntryFields, tableName_, AddingNew, prompt, dbLib_, condition, passParams, debugFile, &row);
                        if (Result == "EditingRecord") {
                            auto& label = labels[0];
                            DataEntry::displayLabels(winFormScreen, row, label, passParams, debugFile);
                        }

                        //for (auto& label : labels) {
                            //DataEntry::displayLabels(winFormScreen,row, label,  passParams, debugFile);
                        //}

                        if (Result == "Error")
                            goto handleError;
                        if (Result == "Exit")
                            goto Exit;
                        if (Result == "RecordNotFound") {
                        }

                        if (Result == "RecordNotFound" || Result == "RecordEntryStart") {
                            index = 0;
                            for (auto& EntryField : DataEntryFields) {
                                if (index > 0) {
                                    EntryField.fieldValue_ = "";
                                    index++;
                                }
                            }
                            goto RecordEntryStart;
                        }
                        if (Result == "EditingRecord") {
                            AddingNew = false;
                            EditingRecord = true;
                            if (index == lastPrimaryKeyIndex)
                                index = lastPrimaryKeyIndex + 1;
                            goto doDisplayData;
                        }


                        //// yes this is needed again
                        //if (Result == "RecordEntryStart") {
                        //    for (auto& EntryField : EntryFields_) {
                        //        EntryField.setFieldValue("");
                        //    }
                        //    goto  RecordEntryStart; // displayData; 
                        //}

                        if (Result == "AddNewRecord") {
                            if (index == lastPrimaryKeyIndex)
                                index = lastPrimaryKeyIndex + 1;
                            AddingNew = true;
                            EditingRecord = false;
                            //debugFile <<"8) winFormScreen_ Address: " << winFormScreen_ << std::endl;
                            wattron(winFormScreen, COLOR_PAIR(3));

                            mvwprintw(winFormScreen, EntryField.row_, EntryField.fieldColumn_, "%s", EntryField.fieldValue_.c_str());
                            wrefresh(winFormScreen);
                            goto BeginField;
                        }

                    }
                }
            }

            DataEntry::displayData(EntryField, debugFile);
            index++;
        }
#pragma endregion // end of code for 'KEY_ENTER' case
        else  // This is a display only EntryField
            index++;
    }
    // if we reached this point that all EntryField have been entered        
    // loop back around to first non primary  key field on screen (without clearing EntryFields_)
    // another option,instead of looping, would be to prompt the user to save the record
    // 
    //Loop back around to first non primary key  field
    //if (index == lastPrimaryKeyIndex)
    index = lastPrimaryKeyIndex + 1;// index 1 is second field we don't want index 0  because it's the primary key field
    //else
    //    index++;

    goto BeginField;

doDisplayData:
    for (auto& EntryField : DataEntryFields) {
        if (EntryField.isDisplayOnly_ == "N")
            DataEntry::displayData(EntryField, debugFile);
        else {
            wattroff(winFormScreen, A_REVERSE);
            wrefresh(winFormScreen);
            for (auto& label : labels) {
                DataEntry::displayLabels(winFormScreen, row, label, passParams, debugFile);// mod

            }
        }

    }


    wrefresh(winFormScreen);
    wattroff(winFormScreen, COLOR_PAIR(2));
    if (EditingRecord) {

        index = lastPrimaryKeyIndex + 1;
        goto BeginField;
    }
    if (!AddingNew)
        goto RecordEntryStart;

    if (index < DataEntryFields.size() - 1) {
        index++;
        goto BeginField;
    }


FunctionKeyError:
    msg = "FunctionKeyError occured";


    DataEntry::errMsg(2, stdscrCols_, action, msg, debugFile);
    if (DataEntry::confirmAction(2, 2, stdscrCols_, "Exit", COLOR_PAIR(1), KEY_F(7), debugFile) == true)
    {
        //goto Exit;
        return "Error";
    }

handleError:
    msg = "handleError occured";

    DataEntry::errMsg(2, stdscrCols_, action, msg, debugFile);
    if (DataEntry::confirmAction(2, 2, stdscrCols_, "Exit", COLOR_PAIR(1), KEY_F(7), debugFile) == true)
    {
        //goto Exit;
        return "Error";
    }

Exit:
    headerRecord = this->getHeaderRecord();

    debugFile << "[DEBUG] Line 1521 EntryField values At Exit: of collectData in DataEntry.cpp" << std::endl;
    debugFile << "------------------------------------------------------------------------------------" << std::endl;
    /*for (auto& EntryField : EntryFields_) {

        debugFile << "EntryField.fieldName_ = " << EntryField.fieldName_ << " EntryField.fieldValue_ = " << EntryField.fieldValue_ << std::endl;
    }*/
    for (const auto& kv : headerRecord_) {
        debugFile << "  " << kv.first << " = " << kv.second << std::endl;
    }

    if (Result == "Exit") {
        Result = "Exit";
    }
    else if (Result != "HeaderSaved") {
        Result = "OK";
    }
    return Result;
}




void DataEntry::drawBoxes(std::vector<BoxProperties>& boxes) {
    WINDOW* winFormScreen = SharedWindowManager::getwinFormScreen();
    WINDOW* winMsgArea = SharedWindowManager::getwinMsgArea();
    wattroff(winFormScreen, A_REVERSE);
    wrefresh(winFormScreen);
    for (const auto& box : boxes) {
        if (box.cols_ > 0 && box.rows_ == 0) { // Horizontal line
            mvwhline(winFormScreen, box.row_, box.col_, ACS_HLINE, box.cols_);
        }
        else if (box.cols_ == 0 && box.rows_ > 0) { // Vertical line
            mvwvline(winFormScreen, box.row_, box.col_, ACS_VLINE, box.rows_);
        }
        else if (box.cols_ > 0 && box.rows_ > 0) { // Box
            mvwaddch(winFormScreen, box.row_, box.col_, ACS_ULCORNER);               // Upper-left corner
            mvwhline(winFormScreen, box.row_, box.col_ + 1, ACS_HLINE, box.cols_ - 2); // Top horizontal line
            mvwaddch(winFormScreen, box.row_, box.col_ + box.cols_ - 1, ACS_URCORNER); // Upper-right corner

            mvwvline(winFormScreen, box.row_ + 1, box.col_, ACS_VLINE, box.rows_ - 2); // Left vertical line
            mvwvline(winFormScreen, box.row_ + 1, box.col_ + box.cols_ - 1, ACS_VLINE, box.rows_ - 2); // Right vertical line

            mvwaddch(winFormScreen, box.row_ + box.rows_ - 1, box.col_, ACS_LLCORNER); // Lower-left corner
            mvwhline(winFormScreen, box.row_ + box.rows_ - 1, box.col_ + 1, ACS_HLINE, box.cols_ - 2); // Bottom horizontal line
            mvwaddch(winFormScreen, box.row_ + box.rows_ - 1, box.col_ + box.cols_ - 1, ACS_LRCORNER); // Lower-right corner
        }
    }

    wrefresh(winFormScreen);  // Refresh the winFormScreen window to show the changes
}

bool DataEntry::acceptInput(DataEntryFieldProperties& EntryField, PassParams& passParams, std::ofstream& debugFile) {
    DataEntry::stdscrRows_ = 0, DataEntry::stdscrCols_ = 0;
    getmaxyx(EntryField.winFormScreen_, DataEntry::stdscrRows_, DataEntry::stdscrCols_);
    bool result = false;
    bool validDate = false;
    bool fKey = false;
    std::string inputAction;
    std::string type = EntryField.fieldType_;
    std::string temp = EntryField.fieldValue_;
    //if (EntryField.fieldName == "test") {
    //    bool test = true;
    //}
    int test = DataEntry::stringSwitchNumber(type);
    switch (test) {
    case 17: // STRING
        fKey = InputUtils::stringInput(EntryField, debugFile);
        break;
    case 18: // NUMERIC
        fKey = InputUtils::numericInput(EntryField, debugFile);
        break;
    case 19: // MASKED
        fKey = InputUtils::maskedInput(EntryField, debugFile);
        break;
    case 23: // DATE
        do {
            fKey = InputUtils::maskedInput(EntryField, debugFile);
            if (fKey) break;
            validDate = DataEntry::isValidDate(EntryField.fieldValue_.c_str(), debugFile);
            if (!validDate) {
                std::string msg = "Invalid Date";
                std::string action = "";
                fKey = DataEntry::errMsg(2, stdscrCols_, action, msg, debugFile);

                if (fKey == true && (action == "KEY_F(5)" || action == "KEY_F(7)")) break;
            }
        } while (!validDate);
        if (validDate) {
            std::string temp = EntryField.fieldValue_.c_str();
            EntryField.fieldValue_ = temp;
        }
        break;
    case 24: // CHOICE        
        std::string temp = EntryField.choiceDescriptions_;
        DataEntry::printInMiddle(EntryField.winMsgArea_, 2, 1, 79, temp, COLOR_PAIR(3), debugFile);
        fKey = InputUtils::stringInput(EntryField, debugFile);
        SharedWindowManager::hidewinMsgArea();
        break;
        //TODO: ADD QUESTION TYPE
    }
    return fKey;
}

bool DataEntry::isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}
bool DataEntry::isValidDate(const std::string& date, std::ofstream& debugFile) {
    int day, month, year;
    char delimiter1, delimiter2;
    std::istringstream dateStream(date);
    dateStream >> std::setw(2) >> month >> delimiter1 >> std::setw(2) >> day >> delimiter2 >> std::setw(4) >> year;
    if (delimiter1 != '/' || delimiter2 != '/' || dateStream.fail() || month < 1 || month > 12 || day < 1 || year < 1) {
        return false;
    }
    // Days in each month
    int daysInMonth[] = { 31, (isLeapYear(year) ? 29 : 28), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    return day <= daysInMonth[month - 1];
}

void DataEntry::debugMsg(std::string string1, std::string string2, int val) {
    WINDOW* winMsgArea = SharedWindowManager::getwinMsgArea();
    WINDOW* winFormScreen = SharedWindowManager::getwinFormScreen();
    std::string result;
    // Create a stringstream
    std::stringstream ss;
    // Insert the string and integer into the stringstream
    ss << "string1 = " << string1 << "val " << val;
    // Extract the concatenated string from the stringstream
    result = ss.str();
    wmove(winMsgArea, 22, 2);
    //clrtoeol();
    const char* myCString = result.c_str();
    wmove(winMsgArea, 22, 2);
    wclrtoeol(winMsgArea);
    mvwprintw(winMsgArea, 22, 2, "%s", myCString);
    wrefresh(winMsgArea);
    wrefresh(winFormScreen);
    wgetch(winMsgArea);
}
bool DataEntry::errMsg(int row, int col, std::string& action, std::string& msg, std::ofstream& debugFile) {
    WINDOW* winMsgArea = SharedWindowManager::getwinMsgArea();

    //std::string action = "";
    int outRow, outCol, width;
    int winRows = 0, winCols = 0, ch = 0;
    if (winMsgArea == nullptr) {
        //debugMsg(dataEntry,"winMsgArea == nullptr", "", 0);
        winMsgArea = stdscr;
    }
    getmaxyx(winMsgArea, winRows, winCols);
    width = winCols;
    beep();
    wattron(winMsgArea, COLOR_PAIR(3));
    wmove(winMsgArea, row, 1);
    wclrtoeol(winMsgArea);
    findMiddle(winMsgArea, row, col, outRow, outCol, width, msg);
    printInMiddle(winMsgArea, row, col, 79, msg, COLOR_PAIR(1), debugFile);
    box(winMsgArea, 0, 0);
    //wattroff(winMsgArea, A_BLINK);
    wrefresh(winMsgArea);
    ch = wgetch(winMsgArea);
    switch (ch) {
        //case PADENTER:
    case 10:  // Enter key
        action = "KEY_ENTER";
        break;
    case KEY_F(5):
        action = "KEY_F(5)";
        break;
    case KEY_F(7):
        action = "KEY_F(7)";
        break;
    }
    SharedWindowManager::hidewinMsgArea();
    //indicates a function was pressed and inputAction indicates which one
    if (action == "KEY_ENTER")
        return false;
    else
        return true;
}
void DataEntry::printInMiddle(WINDOW* win, int startRow, int startCol, int width, std::string& msg, chtype color, std::ofstream& debugFile) {

    if (win == nullptr)
        win = stdscr;
    // Backup the screen if writing to winMsgArea
    //SharedWindowManager::backupWinFormScreenUnderMsgArea();

    // Backup the screen if writing to winMsgArea
    if (win == SharedWindowManager::getwinMsgArea()) {
        SharedWindowManager::backupWinFormScreenUnderMsgArea();
    }

    width = getmaxx(win);
    int outRow = 0, outCol = 0;
    findMiddle(win, startRow, startCol, outRow, outCol, width, msg);
    mvwprintw(win, outRow, outCol, "%s", msg.c_str());
    wrefresh(win);
}
void DataEntry::findMiddle(WINDOW* win, int startRow, int startCol, int& outRow, int& outCol, int width, std::string msg) {
    //int length, 
    int col, row;
    int temp;
    //if (win == nullptr)
    //debugMsg(dataEntry,"findMiddle", "nullptr window passed", 0);
    if (win == nullptr)
        win = stdscr;
    //getyx(win, row, col);

    getmaxyx(win, row, col);
    if (width == 0)
        width = col;
    if (startCol != 0)
        col = startCol;
    //if (startRow != 0)
    row = startRow;
    if (width == 0)
        width = col;
    temp = ((width - int(msg.length())) / 2);
    //outCol = startCol + (int)temp;
    outCol = (int)temp;
    outRow = row;
}
std::string DataEntry::askQuestion(WINDOW* win, int startRow, int startCol, int width, std::string prompt, chtype color) {
    //restart:
    echo();
    char xstr[] = { ' ' };
    int input;
    wmove(win, startRow, startCol);
    wclrtoeol(win);
    box(win, 0, 0);
    int outRow, outCol;
    findMiddle(win, startRow, startCol, outRow, outCol, width, prompt);
    //wattron(win, color);
    mvwprintw(win, outRow, outCol, "%s", prompt.c_str());
    //wattroff(win, color);
    box(win, 0, 0);
    wrefresh(win);
    input = wgetch(win);
    wmove(win, outRow, outCol);
    //wclrtoeol(win);
    werase(win);
    wrefresh(win);
    noecho();
    if (input == 'y' || input == 'Y') {
        return "Yes";
    }
    else {
        return "No";
    }
}
bool DataEntry::confirmAction(int startRow, int startCol, int width, std::string prompt, chtype color, int keyToPress, std::ofstream& debugFile) {
    WINDOW* winMsgArea = SharedWindowManager::getwinMsgArea();
    std::string keyMessage, message;
    //int outCol, outRow;
    int c;

    bool result = false;
    wattron(winMsgArea, COLOR_PAIR(3));
    wmove(winMsgArea, startRow, startCol);
    wclrtoeol(winMsgArea);
    wmove(winMsgArea, startRow - 1, startCol);
    wclrtoeol(winMsgArea);
    //refresh();
    switch (keyToPress) {
    case KEY_F(3):
        keyMessage = "Press F3 to ";
        break;
    case KEY_F(4):
        keyMessage = "Press F4 to ";
        break;
    case KEY_F(5):
        keyMessage = "Press F5 to ";
        break;
    case KEY_F(7):
        keyMessage = "Press F7 to ";
        break;
    case KEY_NPAGE:
        keyMessage = "Press Page Down to ";
        break;
    }
    message = keyMessage;
    message.append(prompt);
    printInMiddle(winMsgArea, startRow, startCol, width, message, COLOR_PAIR(1), debugFile);
    box(winMsgArea, 0, 0);
    keypad(winMsgArea, TRUE);
    wbkgd(winMsgArea, COLOR_PAIR(3));
    wrefresh(winMsgArea);
    wrefresh(winMsgArea);
    c = wgetch(winMsgArea);
    wmove(winMsgArea, startRow, startCol);
    wclrtoeol(winMsgArea);
    //refresh();
    if (c == keyToPress) {
        result = true;
    }
    else {
        if (winMsgArea != stdscr) {
            result = false;
        }
    }
    return result;
}
std::string DataEntry::doFunctionKeys(DataEntryFieldProperties& EntryField, const std::string& tbl, bool addingNew, ISAMWrapperLib& lib, std::string& condition, std::string& inquiryOrderBy, std::string& inquiryFields, std::vector<DataEntryFieldProperties>& EntryFields_, std::ofstream& debugFile, const std::string& inputAction) {
    std::string result = "";
    WINDOW* winMsgArea = EntryField.winMsgArea_;
    WINDOW* winFormScreen = EntryField.winFormScreen_;

    // TODO: find way so these are not hard-coded
    int lookupHeight = 10, lookupWidth = 50, lookupStartY = 5, lookupStartX = 5;
    //std::vector<std::vector<std::string>> records;  // Updated: Uses vector of vectors
    std::vector<std::tuple<std::string, std::string>>records;


    std::vector<std::string> fieldNames, fieldValues;

    int winRows = 0, winCols = 0;
    getmaxyx(winFormScreen, winRows, winCols);

    for (const auto& entry : EntryFields_) {
        std::string fieldName = entry.fieldName_;
        std::replace(fieldName.begin(), fieldName.end(), ' ', '_');  // Replace spaces with underscores
        fieldNames.push_back(fieldName);

        std::string fieldValue = entry.fieldValue_;
        std::replace(fieldValue.begin(), fieldValue.end(), '"', ' ');  // Replace quotes with space
        fieldValues.push_back(fieldValue);
    }

    int test = DataEntry::stringSwitchNumber(inputAction);
    switch (test) {
    case 1: break; // KEY_F(1)
    case 2: // KEY_F(2) - Lookup window
        if (!fieldNames.empty()) {
            records = lib.selectRows(tbl, "", inquiryFields, inquiryOrderBy);
            EntryField.fieldValue_ = DataEntry::displayLookupWindow(winFormScreen, lookupHeight, lookupWidth, lookupStartY, lookupStartX, records, inquiryFields, condition, debugFile);

            if (EntryField.fieldValue_.empty())
                // 5-28-25 
                //result = "RecordEntryStart";
                result = "Empty";
            else {
                result = "TryReadRecord";
            }
            return result;
        }
        break;
    case 3: // KEY_F(3) - Delete Record
        if (DataEntry::confirmAction(2, 2, winCols, "Delete Record", COLOR_PAIR(1), KEY_F(3), debugFile)) {
            lib.deleteRow(tbl, condition);
            // 5-28-25 
            //result = "RecordEntryStart";
            result = "RecordDeleted";
        }
        break;
    case 4: // KEY_F(4) - Save Record
        if (DataEntry::confirmAction(2, 2, winCols, "Save Record", COLOR_PAIR(1), KEY_F(4), debugFile)) {
            if (addingNew) {
                lib.insert(tbl, fieldNames, fieldValues);
            }
            else {
                lib.updateRow(tbl, fieldNames, fieldValues, condition);
            }
            // save values before clearing
            // //std::map<std::string, std::string> headerRecord;
            headerRecord_.clear();
            for (const auto& EntryField : EntryFields_) {//this->EntryFields_) {
                headerRecord_[EntryField.fieldName_] = EntryField.fieldValue_;

            }

            // 5-28-25  THIS IS DONE AFTER CALL TO THIS FUNCTION
            //// Clear EntryField values after update or insert
            //for (auto& EntryField : EntryFields_) {
            //    EntryField.fieldValue_ = "";
            //}
            //result = "RecordEntryStart";
            result = "RecordSaved";
        }
        else {
            wclear(winMsgArea);
            wrefresh(winMsgArea);
        }
        break;
    case 5: // KEY_F(5) - Restart
        if (DataEntry::confirmAction(2, 2, winCols, "Restart", COLOR_PAIR(1), KEY_F(5), debugFile)) {
            result = "RecordEntryStart";
        }
        else {
            wclear(winMsgArea);
            wrefresh(winMsgArea);
        }
        break;
    case 6: break; // KEY_F(6)
    case 7: // KEY_F(7) - Exit
        if (DataEntry::confirmAction(2, 2, winCols, "Exit", COLOR_PAIR(1), KEY_F(7), debugFile)) {
            result = "Exit";
            wclear(winMsgArea);
            wrefresh(winMsgArea);
        }
        else {
            wclear(winMsgArea);
            wrefresh(winMsgArea);
        }
        break;
    default:
        // TODO: add other cases?
        DataEntry::displayFkeys();
        result = "";
    }
    return result;
}

std::string DataEntry::readRecord(std::vector<DataEntryFieldProperties>& DataEntryFields,
    std::string tbl, bool addingNew, bool prompt,
    ISAMWrapperLib& lib, const std::string condition,
    PassParams& passParams, std::ofstream& debugFile,
    std::map<std::string, std::string>* outRow)
{
    std::string result;
    WINDOW* winFormScreen = SharedWindowManager::getwinFormScreen();
    WINDOW* winMsgArea = SharedWindowManager::getwinMsgArea();

    //  using selectDynamicRecords to get a name-keyed row ----
    // Assumes condition will fetch exactly one row
    auto rs = lib.selectDynamicRecords(tbl, condition, "*", "");

    if (!rs.first.empty()) {
        const auto& row = rs.first.front();  // map<string,string> colName->value

        // Populate screen fields in screen order by LOOKING UP BY NAME
        for (auto& entry : DataEntryFields) {
            if (entry.fieldName_ == "TTYP") {
                bool test = true;
            }

            auto it = row.find(entry.fieldName_);
            entry.fieldValue_ = (it != row.end()) ? it->second : "";
        }

        if (outRow) *outRow = row;          // hand raw row back to caller       


        result = "EditingRecord";
    }
    else {
        // ---- NOT FOUND branch: 
        std::string AllowAdd = "N";
        // TTYP 3 means editing					   
        auto it = passParams.fields.find("ALLOWADD");
        if (it != passParams.fields.end()) {
            AllowAdd = it->second.value;
        }
        if (prompt && AllowAdd == "Y") {
            std::string message = "Record not found. Do you want to add it?";
            std::string answer = askQuestion(winMsgArea, 1, 2, 0, message, COLOR_PAIR(1));
            result = (answer == "Yes") ? "AddNewRecord" : "RecordNotFound";
        }
        else {
            std::string message = "Invalid Entry, Record not found.";
            std::string action;
            bool fKey = DataEntry::errMsg(2, stdscrCols_, action, message, debugFile);
            result = "RecordNotFound";
            if (fKey == true && action == "KEY_F(7)") {
                if (DataEntry::confirmAction(2, 2, stdscrCols_, "Exit", COLOR_PAIR(1), KEY_F(7), debugFile) == true) {
                    result = "Exit";
                }
            }
        }

        wattroff(winFormScreen, A_REVERSE);
        wrefresh(winFormScreen);
        return result;
    }
    wattroff(winFormScreen, A_REVERSE);
    wrefresh(winFormScreen);
    return result;
}


int DataEntry::stringSwitchNumber(const std::string& key) {
    // Because the switch statement doesn't support alpha values,
    // this function is helpful for mapping an alpha value to a number.
    // Originally, I used this for function key values but
    // found this technique will be useful for any alpha test.
    static const std::unordered_map<std::string, int> keyMap = {
        {"KEY_UP", -1},
        {"KEY_ENTER", 0},
        {"KEY_F(1)", 1},
        {"KEY_F(2)", 2},
        {"KEY_F(3)", 3},
        {"KEY_F(4)", 4},
        {"KEY_F(5)", 5},
        {"KEY_F(7)", 7},
        {"BeginField", 13},
        {"AddNewRecord", 14},
        {"RecordEntryStart", 15},
        {"exitField_NOT_KEY_F", 16},
        {"STRING", 17},
        {"NUMERIC", 18},
        {"MASKED", 19},
        {"UPPERCASE", 20},
        {"DIGITS", 21},
        {"QUESTION", 22},
        {"DATE", 23},
        {"CHOICE", 24},
        {"DISPLAY", 25}
    };
    // Check if the key exists in the map
    auto it = keyMap.find(key);
    if (it != keyMap.end()) {
        // Return the corresponding numeric value
        return it->second;
    }
    else {
        // Return a default value or handle the case when the key is not found
        return -99; // Default value indicating key not found
    }
}

// Function to transform vector of tuples into vector of pairs
std::vector<std::pair<std::string, std::string>> DataEntry::transformVector(const std::vector<DataTuple>& inputVector) {
    std::vector<std::pair<std::string, std::string>> outputVector;
    for (const auto& tuple : inputVector) {
        std::string key = std::get<0>(tuple);
        std::string value = std::get<1>(tuple);
        outputVector.push_back(std::make_pair(key, value));
    }
    return outputVector;
}
std::string DataEntry::displayLookupWindow(WINDOW* mainWindow, int lookupHeight, int lookupWidth, int lookupStartY, int lookupStartX, std::vector<std::tuple<std::string, std::string>>& data, const std::string& inquiryFields, std::string condition, std::ofstream& debugFile) {
    if (data.empty()) return ""; // No data to display

    // Parse `inquiryFields` into a vector of EntryField indexes
    std::vector<int> fieldIndexes;
    std::istringstream fieldStream(inquiryFields);
    std::string EntryField;
    int fieldIndex = 0;
    while (std::getline(fieldStream, EntryField, ',')) {
        fieldIndexes.push_back(fieldIndex++);
    }

    int maxLookupWidth = 0;
    int rowWidth = 0;

    // Calculate width based only on the inquiry fields
    for (const auto& [field1, field2] : data) {
        // Process each EntryField directly
        rowWidth += field1.length();
        rowWidth += field2.length();

        // You can print or do something with the fields
        std::cout << "EntryField 1: " << field1 << ", EntryField 2: " << field2 << ", Width: " << rowWidth << std::endl;
        maxLookupWidth = std::max(maxLookupWidth, rowWidth);
    }


    maxLookupWidth += 10;

    WINDOW* lookupWin = newwin(lookupHeight, maxLookupWidth, lookupStartY, lookupStartX);
    keypad(lookupWin, TRUE);
    box(lookupWin, 0, 0);
    wrefresh(lookupWin);
    wrefresh(mainWindow);

    int currentRow = 1, selectedRow = 0, topRow = 0;
    std::string firstValue = "";
    std::string blankLine(maxLookupWidth - 5, ' ');

    while (true) {
        werase(lookupWin);
        box(lookupWin, 0, 0);

        //// Display only the fields specified in `inquiryFields`
        //for (int i = topRow; i < std::min(topRow + lookupHeight - 2, static_cast<int>(data.size())); ++i) {
        //    if (i >= static_cast<int>(data.size())) break;

        //    mvwprintw(lookupWin, i - topRow + 1, 2, "%s", blankLine.c_str());
        //    if (i == selectedRow) wattron(lookupWin, A_REVERSE);

        //    int colPos = 2;
        //    for (int index : fieldIndexes) {
        //        if (index < data[i].size()) {
        //            mvwprintw(lookupWin, i - topRow + 1, colPos, "%s", data[i][index].c_str());
        //            colPos += data[i][index].length() + 2;
        //        }
        //    }

        //    wattroff(lookupWin, A_REVERSE);
        //}

        for (int i = topRow; i < std::min(topRow + lookupHeight - 2, static_cast<int>(data.size())); ++i) {
            const auto& [field1, field2] = data[i];  // structured binding

            int rowY = i - topRow + 1;  // curses row
            int colPos = 1;             // starting X position

            // Print field1
            mvwprintw(lookupWin, rowY, colPos, "%s", field1.c_str());
            colPos += field1.length() + 2;

            // Print field2
            mvwprintw(lookupWin, rowY, colPos, "%s", field2.c_str());
            colPos += field2.length() + 2;
        }
        wattroff(lookupWin, A_REVERSE);
        wmove(lookupWin, currentRow - topRow + 1, 2);
        wrefresh(lookupWin);

        int ch = wgetch(lookupWin);
        switch (ch) {
        case KEY_UP:
            if (selectedRow > 0) {
                selectedRow--;
                if (currentRow > 1) currentRow--;
                else if (topRow > 0) topRow--;
            }
            break;
        case KEY_DOWN:
            if (selectedRow < static_cast<int>(data.size()) - 1) {
                selectedRow++;
                if (currentRow < lookupHeight - 2 && currentRow < static_cast<int>(data.size()) - topRow) currentRow++;
                else if (topRow < static_cast<int>(data.size()) - lookupHeight + 2) topRow++;
            }
            break;
        case '\n': // Enter key pressed
            delwin(lookupWin);
            touchwin(mainWindow);
            wrefresh(mainWindow);
            return (selectedRow < static_cast<int>(data.size())) ? std::get<0>(data[selectedRow]) : "";

        case 27: // Escape key pressed
            delwin(lookupWin);
            touchwin(mainWindow);
            wrefresh(mainWindow);
            return "";
        }
    }
    return "";
}
void DataEntry::displayRightToLeft(WINDOW* win, const std::string& input, int row, int col, int inputSize, int calledFrom, std::ofstream& debugFile) {
    // Clear the input area first
    for (size_t i = 0; i < static_cast<size_t>(inputSize); ++i) {

        mvwaddch(win, row, col + i, ' ');
    }

    // Start writing from the right end of the input area
    int startCol = col + inputSize - input.length();
    for (size_t i = 0; i < input.length(); ++i) {
        mvwaddch(win, row, startCol + i, input[i]);
    }

    wrefresh(win);
}
void DataEntry::displayLabels(WINDOW* win, const std::map<std::string, std::string> record, const LabelProperties& L, PassParams& PassParams, std::ofstream& dbg) {
    std::string val;
    std::string labelFieldValue;
    //10/21/25
    //wattroff(win, A_REVERSE);
    //wrefresh(win);
    if (L.labelText == "Trans Type" || L.labelText == "Transaction Type") {
        bool test = true;
    }
    if (L.labelText == "Sold To:") {
        bool test = true;
    }
    if (!L.labelFieldName.empty()) {
        auto it = record.find(L.labelFieldName);
        val = (it != record.end()) ? it->second : "";
    }
    else
    {
        if (PassParams.fields.find(L.labelParmName) != PassParams.fields.end()) {

            val = PassParams.fields.at(L.labelParmName).value;
        }

    }

    if (!L.choiceValues.empty() && !L.choiceDescriptions.empty()) {

        val = DataEntry::choiceDescriptionFor(val, L.choiceValues, L.choiceDescriptions);

    }
    labelFieldValue = val;
    wattroff(win, A_REVERSE);
    wrefresh(win);
    mvwprintw(win, L.labelFieldRow, L.labelFieldCol, "%s", labelFieldValue.c_str());
    wrefresh(win);
    wattroff(win, A_REVERSE);
    wrefresh(win);
    wattrset(win, A_NORMAL);// ensure background has no A_REVERSE
    wbkgd(win, COLOR_PAIR(3));
    mvwprintw(win, L.row, L.col, "%s", L.labelText.c_str());
    wrefresh(win);

}




void DataEntry::displayData(DataEntryFieldProperties& EntryField, std::ofstream& debugFile) {
    //debugFile << "Entering displayData EntryField.fieldName = "<< EntryField.fieldName << std::endl;
    //if (EntryField.fieldName == "15")
        //bool test = true;
    WINDOW* winFormScreen = SharedWindowManager::getwinFormScreen();
    WINDOW* winMsgArea = SharedWindowManager::getwinMsgArea();
    std::string myString = DataEntry::cleanString(EntryField.fieldValue_);
    EntryField.fieldValue_ = myString;
    wattron(winFormScreen, COLOR_PAIR(3));
    // emptyString used to erase text on screen from previous field value (if any)
    std::string emptyString = "";
    emptyString.resize(EntryField.len_, ' ');
    wattroff(winFormScreen, A_REVERSE);
    mvwprintw(winFormScreen, EntryField.row_, EntryField.fieldColumn_, "%s", emptyString.c_str());
    /*std::string myStringPaddedLeft = myString;
    myStringPaddedLeft.resize(EntryField.len_, ' ');
    mvwprintw(winFormScreen, EntryField.row_, EntryField.fieldColumn_, "%s", myStringPaddedLeft.c_str());*/

    int test = DataEntry::stringSwitchNumber(EntryField.fieldType_);
    //if (test == 18) { // numeric EntryField
        //std::string emptyString = "";
        //emptyString.resize(EntryField.len_, ' ');
        //mvwprintw(winFormScreen, EntryField.row_, EntryField.fieldColumn_, "%s", emptyString.c_str());
        //debugFile << "In displayData before displayRightToLeft " << std::endl;
        //displayRightToLeft(winFormScreen, EntryField.fieldValue_, EntryField.row_, EntryField.fieldColumn_, EntryField.len_,1, debugFile);
        /*std::string myStringPaddedLeft = myString;
        myStringPaddedLeft.resize(EntryField.len_, ' ');
        mvwprintw(winFormScreen, EntryField.row_, EntryField.fieldColumn_, "%s", myStringPaddedLeft.c_str());*/

        //std::string myStringPaddedRight = DataEntry::rightJustifyString(myString, len_, debugFile);
        //mvwprintw(winFormScreen_, EntryField.row_, EntryField.fieldColumn_, "%s", myStringPaddedRight.c_str());
    //}
    //else {
    // now display actual field value
    std::string myStringPaddedLeft = myString;
    myStringPaddedLeft.resize(EntryField.len_, ' ');
    mvwprintw(winFormScreen, EntryField.row_, EntryField.fieldColumn_, "%s", myStringPaddedLeft.c_str());
    //}
    wattroff(winFormScreen, A_REVERSE);
    wrefresh(winFormScreen);
}

KeyType DataEntry::checkSpecialKeys(int ch) {
    switch (ch) {
    case KEY_F(1):
    case KEY_F(2):
    case KEY_F(3):
    case KEY_F(4):
    case KEY_F(5):
    case KEY_F(6):
    case KEY_F(7):
    case KEY_F(8):
    case KEY_F(9):
    case KEY_F(10):
    case KEY_F(11):
    case KEY_F(12):
        return KeyType::FUNCTION_KEY;
    case KEY_UP:
        return KeyType::UP_ARROW;
    case 10: // Enter key
    case KEY_ENTER:
        //case PADENTER:
        return KeyType::ENTER_KEY;
    default:
        if (ch >= 0 && ch <= 127)
            return KeyType::ALPHA_NUM_KEY;
        else
            return KeyType::OTHER_KEY;
    }
}



std::string DataEntry::rightJustifyString(const std::string& str, int len, std::ofstream& debugFile) {

    if (str.length() >= static_cast<size_t>(len)) {
        return str; // If the string is already longer than or equal to len, return it as is
    }
    return std::string(len - str.length(), ' ') + str; // Pad with spaces on the left
}
std::string DataEntry::cleanString(const std::string& input) {

    std::string result;

    // Remove special characters and double quotes
    for (char ch : input) {
        if (ch != '\n' && ch != '\r' && ch != '\t' && ch != '\"') {
            result += ch;
        }
    }

    // Remove leading spaces
    size_t start = result.find_first_not_of(" ");
    if (start != std::string::npos) {
        result = result.substr(start);
    }
    else {
        result.clear(); // If there are no non-space characters, return an empty string
    }

    // Remove trailing spaces
    size_t end = result.find_last_not_of(" ");
    if (end != std::string::npos) {
        result = result.substr(0, end + 1);
    }

    return result;
}


void DataEntry::displayFkeys() {
    WINDOW* winFormScreen = SharedWindowManager::getwinFormScreen();
    WINDOW* winMsgArea = SharedWindowManager::getwinMsgArea();
    // weird bug have to turn off before on (in some cases)
    wattroff(winFormScreen, A_REVERSE);
    wattron(winFormScreen, A_REVERSE);
    int functionKeyRow = stdscrRows_ - 2;
    int functionKeyCount = 5;
    int functionKeygaps = 1;
    // for spacing to look good, the length of  the text for the fuction key labels must be the same
    mvwprintw(winFormScreen, functionKeyRow, functionKeygaps * 2, " F2 Lookup  ");
    wrefresh(winFormScreen);
    functionKeygaps = stdscrCols_ / functionKeyCount;
    mvwprintw(winFormScreen, functionKeyRow, functionKeygaps * 1, " F3  Delete ");
    mvwprintw(winFormScreen, functionKeyRow, functionKeygaps * 2, "  F4  Save  ");
    mvwprintw(winFormScreen, functionKeyRow, functionKeygaps * 3, " F5 Restart ");
    mvwprintw(winFormScreen, functionKeyRow, functionKeygaps * 4, "  F7  Exit  ");
    wrefresh(winFormScreen);
    wattroff(winFormScreen, A_REVERSE);
    wrefresh(winFormScreen);
}
// needed to fix bug
// Read characters and their attributes after the input EntryField ends
std::vector<chtype> DataEntry::readAfterLineWithAttributes(WINDOW* win, int row, int startCol) {
    int maxCols = win->_maxx; // Get the maximum number of columns in the window
    std::vector<chtype> lineContent;

    // Read characters one by one using mvwinch() to capture both character and attributes
    for (int col = startCol; col < maxCols; ++col) {
        chtype ch = mvwinch(win, row, col);  // Read the character and attributes at (row, col)
        lineContent.push_back(ch);
    }

    return lineContent;
}
// needed to fix bug
// Restore characters and their attributes after the input EntryField ends
void DataEntry::restoreAfterLineWithAttributes(WINDOW* win, int row, int startCol, const std::vector<chtype>& lineContent) {
    // Restore each character and its attributes
    for (size_t i = 0; i < lineContent.size(); ++i) {
        mvwaddch(win, row, startCol + i, lineContent[i]);  // Restore character with attributes
    }
    wrefresh(win);
}

int DataEntry::displayScreen(std::ifstream& screenXmlFile, const std::string& screenXmlFileName, std::vector<ScreenProperties>& screenProps, std::vector<BoxProperties>& boxes, std::vector<LabelProperties>& labels, std::vector<DataEntryFieldProperties>& DataEntryFields, std::ofstream& debugFile) {

    screenXmlFile.clear(); // Clear any error flags
    screenXmlFile.seekg(0, std::ios::beg); // Go back to the beginning of the file

    //validateDataEntryXml(screenXmlFile, screenXmlFileName, debugFile);


    // this line creates the boxes structure we have no real values to use yet
    //this fake elements will be removed and the real values will be added down futher in this function 
    boxes.emplace_back(0, 0, 0, 0);
    screenXmlFile.clear(); // Clear any error flags
    screenXmlFile.seekg(0, std::ios::beg); // Go back to the beginning of the file
    setupBoxes(boxes, screenXmlFile, screenXmlFileName, debugFile);

    // the first element is not needed because it is emplaced with nulls
    if (!boxes.empty()) {
        boxes.erase(boxes.begin());
    }




    setupDataEntryWindow(screenProps, debugFile);
    // this line creates the label structure we have no real values to use yet
   // this fake elements will be removed and the real values will be added down futher in this function 

    int row = 0;
    int col = 0;
    std::string labelText = "";
    std::string labelFieldName = "";
    std::string labelParmName = "";
    int labelFieldCol = 0;
    int labelFieldRow = 0;
    std::string choiceValues = "";
    std::string choiceDescriptions = "";
    std::string labelFieldWHERE = "";
    std::string labelFieldSQL = "";
    labels.emplace_back(row, col, labelText, labelFieldName, labelParmName, labelFieldCol,
        labelFieldRow, choiceValues, choiceDescriptions, labelFieldWHERE, labelFieldSQL);
    screenXmlFile.clear(); // Clear any error flags
    screenXmlFile.seekg(0, std::ios::beg); // Go back to the beginning of the file
    setupLabels(labels, screenXmlFile, screenXmlFileName, debugFile);
    if (!labels.empty()) {
        labels.erase(labels.begin());
    }
    //---------------------------------------------------------------------
    getmaxyx(stdscr, stdscrRows_, stdscrCols_);
    std::vector<std::string> fieldNames;
    WINDOW* winFormScreen = SharedWindowManager::getwinFormScreen();
    WINDOW* winMsgArea = SharedWindowManager::getwinMsgArea();

    for (const auto& entry : DataEntryFields) {
        std::string value = entry.fieldName_;
        std::replace(value.begin(), value.end(), ' ', '_');  // Replace spaces with underscores
        fieldNames.push_back(value);
    }


    std::string Result = "";
    wclear(winFormScreen);
    wattron(winFormScreen, COLOR_PAIR(3));
    wbkgd(winFormScreen, COLOR_PAIR(3));
    box(winFormScreen, ACS_VLINE, ACS_HLINE);
    DataEntry::drawBoxes(boxes);
    DataEntry::displayFkeys();

    //TODO: need to find spot to set  screenTitle_ value 
    screenProps[0].screenTitle_ = "TEST SCREEN TITLE";
    //DataEntry::printInMiddle(winFormScreen, 0, 0, 0, screenProps[0].screenTitle_, COLOR_PAIR(3), debugFile);
    int functionKeyRow = stdscrRows_ - 2;
    int functionKeyCount = 5;
    //bool firstTime = true, quit = false;
    //bool EditingRecord = false, AddingNew = false, prompt = false;
    int test = 0;
    std::string msg = "";
    std::string temp = "";
    // done in collect data
    //for (auto& label : labels) {
    //    DataEntry::displayLabels(label, debugFile);
    //}
    wrefresh(winFormScreen);
    for (auto& EntryField : DataEntryFields) {
        if (EntryField.isDisplayOnly_ == "Y")
            DataEntry::displayData(EntryField, debugFile);
    }
    wrefresh(winFormScreen);
    size_t  index = 0;
    std::vector<std::string>  fieldValues;
    return 0;


}

//int DataEntry::AssignNextSeqNum(std::string& NextSeqNumtable, ISAMWrapperLib& lib,std::ofstream& debugFile) {
//    int result = 0;
//    //result = lib.selectRows(tbl, "", inquiryOrderBy);
//
//    return result;
//
//}

PassParams DataEntry::parseParamXML(std::ifstream& screenXmlFile, std::string& screenXmlFileName, std::ofstream& debugFile) {

    PassParams passParams;
    std::string line;
    bool inStruct = false;
    screenXmlFile.clear(); // Clear any error flags
    screenXmlFile.seekg(0, std::ios::beg); // Go back to the beginning of the file

    while (std::getline(screenXmlFile, line)) {
        line = cleanString(line);

        if (line.find("<Struct") != std::string::npos) {
            inStruct = true;
            continue;
        }
        if (line.find("</Struct>") != std::string::npos) {
            inStruct = false;
            continue;
        }
        //


        if (inStruct == true) {
            std::map<std::string, std::string> attributes = extractAttributes(line);
            //if (attributes.count("Parm") && attributes.count("fieldName") && attributes.count("value")) {
            if (attributes.count("Parm") && attributes.count("fieldName")) {
                parmProperties EntryField;
                EntryField.fieldName = attributes["fieldName"];
                EntryField.parm = attributes["Parm"];
                EntryField.value = attributes["value"];

                passParams.fields[attributes["fieldName"]] = EntryField;  // Use Parm as the key
            }
        }
    }





    //screenXmlFile.close();
    return passParams;
}
// Function to extract attribute values from a line
std::map<std::string, std::string> DataEntry::extractAttributes(const std::string& line) {
    std::map<std::string, std::string> attributes;

    //std::regex attrRegex(R"(\b(\w+)\s*=\s*\"([^\"]+)\")");
    std::regex attrRegex(R"(\b(\w+)\s*=\s*([^\s/>]+))");

    std::smatch matches;

    std::string::const_iterator searchStart(line.cbegin());
    while (std::regex_search(searchStart, line.cend(), matches, attrRegex)) {
        attributes[matches[1].str()] = matches[2].str();
        searchStart = matches.suffix().first;
    }

    return attributes;
}
void DataEntry::createNumberTable(const std::string& screenXmlFileName) {
    std::string createTableSQL = "CREATE TABLE IF NOT EXISTS " + screenXmlFileName +
        " (id INTEGER PRIMARY KEY, next_number INTEGER DEFAULT 0);";
    dbLib_.executeQuery(createTableSQL);
    /*char* errMsg = nullptr;
    if (sqlite3_exec(db_, createTableSQL.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        SQLITE_API const char* msg = sqlite3_errmsg(db_);
        std::cout << "sqlite3_errmsg = " << msg << std::endl;
        if (errMsg != nullptr) {
            sqlite3_free(errMsg);
        }
        std::cerr << "Error creating table: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }*/
}

void DataEntry::initializeNumberCounter(const std::string& screenXmlFileName) {
    std::string checkSQL = "SELECT COUNT(*) FROM " + screenXmlFileName + " WHERE id = 1;";
    dbLib_.executeQuery(checkSQL);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, checkSQL.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW && sqlite3_column_int(stmt, 0) == 0) {
            sqlite3_finalize(stmt);
            std::string insertSQL = "INSERT INTO " + screenXmlFileName + " (id, next_number) VALUES (1, 1);";
            char* errMsg = nullptr;
            if (sqlite3_exec(db_, insertSQL.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
                std::cerr << "Error inserting initial number record: " << errMsg << std::endl;
                sqlite3_free(errMsg);
            }
        }
        else {
            sqlite3_finalize(stmt);
        }
    }
}


int DataEntry::getNextNumber(const std::string& screenXmlFileName, std::ofstream& debugFile) {
    createNumberTable(screenXmlFileName);
    initializeNumberCounter(screenXmlFileName);


    int nextNumber = 0;
    std::string selectSQL = "SELECT next_number FROM " + screenXmlFileName + " WHERE id = 1;";
    dbLib_.executeQuery(selectSQL);

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, selectSQL.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        SQLITE_API const char* msg = sqlite3_errmsg(db_);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            nextNumber = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }
    else {
        std::cerr << "Error selecting next number: " << sqlite3_errmsg(db_) << std::endl;
        return -1;
    }

    nextNumber++;

    std::string updateSQL = "UPDATE " + screenXmlFileName + " SET next_number = ? WHERE id = 1;";
    // TODO:
    // MIGHT WANT TO ADD ISAM FUNCTION THAT CAN 
    // RETURN A nextNumber
    //dbLib_.executeQuery(updateSQL);
    if (sqlite3_prepare_v2(db_, updateSQL.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, nextNumber);
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            std::cerr << "Error updating next number: " << sqlite3_errmsg(db_) << std::endl;
            sqlite3_finalize(stmt);
            return -1;
        }
        sqlite3_finalize(stmt);
    }
    else {
        std::cerr << "Error preparing update statement: " << sqlite3_errmsg(db_) << std::endl;
        return -1;
    }

    return nextNumber;
}
int DataEntry::parseInteger(const std::string& value, int defaultValue = 0) {
    try {
        return std::stoi(value);
    }
    catch (const std::invalid_argument&) {
        return defaultValue;
    }
    catch (const std::out_of_range&) {
        return defaultValue;
    }
}
std::vector<std::string> DataEntry::splitCSV(const std::string& s) {
    std::vector<std::string> out; std::string cur;
    for (char c : s) {
        if (c == ',') { out.push_back(DataEntry::cleanString(cur)); cur.clear(); }
        else cur.push_back(c);
    }
    out.push_back(DataEntry::cleanString(cur));
    return out;
}

std::string DataEntry::choiceDescriptionFor(const std::string& value,
    const std::string& choiceValuesCSV,
    const std::string& choiceDescriptionsCSV)
{
    auto vals = DataEntry::splitCSV(choiceValuesCSV);
    auto descs = DataEntry::splitCSV(choiceDescriptionsCSV);
    if (vals.size() != descs.size()) return value; // mismatch; fall back
    const std::string key = DataEntry::cleanString(value);
    for (size_t i = 0; i < vals.size(); ++i) {
        if (vals[i] == key) return descs[i];
    }
    return value; // not found; fall back to raw value
}
// Replace EntryField names in formula with values from the record
std::string DataEntry::evaluateCalcFormula(const std::string& formula, const std::map<std::string, std::string>& record) {
    // Start with the raw formula, e.g., "OQTY*UPRC"
    std::string expr = formula;
    std::ostringstream ss;

    // Replace each EntryField name in the formula with its value from the record
    for (const auto& [fieldName, fieldValue] : record) {


        size_t pos = 0;
        while ((pos = expr.find(fieldName, pos)) != std::string::npos) {
            std::string replacement = fieldValue.empty() ? "0" : fieldValue;
            expr.replace(pos, fieldName.length(), replacement);
            pos += replacement.length();
        }
    }


    // Evaluate the math expression (e.g., "2*4.5")
    double result = evaluateSimpleMath(expr);




    // Convert result to string with 2 decimal places
    std::ostringstream ssResult;
    ssResult << std::fixed << std::setprecision(2) << result;

    return ssResult.str();
}

// Very basic math evaluator: supports + - * / (left-to-right only, no precedence or parentheses)
double DataEntry::evaluateSimpleMath(const std::string& expression) {
    std::istringstream iss(expression);
    double result;
    iss >> result;

    char op;
    double number;

    while (iss >> op >> number) {
        switch (op) {
        case '+': result += number; break;
        case '-': result -= number; break;
        case '*': result *= number; break;
        case '/': result = (number != 0) ? result / number : 0; break;
        default: break;
        }
    }

    return result;
}
const std::map<std::string, std::string>& DataEntry::getHeaderRecord() const {
    return headerRecord_;
}