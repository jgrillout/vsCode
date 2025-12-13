// Version-02.20.25.16.07
// File: Dialog.cpp
#pragma once
#include "Dialog.h"
#include "DialogWindowManager.h"
#include "SharedWindowManager.h"
#include "DataEntry.h"


// Define static members of Dialog class
WINDOW* Dialog::winDialog_ = nullptr;
WINDOW* Dialog::winDialogShadow_ = nullptr;
int Dialog::stdscrRows_ = 0;
int Dialog::stdscrCols_ = 0;
int Dialog::len_ = 0;

std::string  Dialog::DialogType_="";
std::string  Dialog::DialogTitle_ = "";
std::string  Dialog::DialogBoxed_ ="Y";
int  Dialog::DialogWidth_ = 0;
int  Dialog::DialogHeight_ = 0;
std::string Dialog::nextRunType_ = "";
std::string Dialog::nextRunClass_ = "";
std::string Dialog::inputType_="";
std::string Dialog::inputMask_="";
int Dialog::inputLen_=0;
int Dialog::inputRow_=0;
int Dialog::inputlabelCol_=0;
std::string Dialog::inputlabelText_="";
namespace fs = std::filesystem;

Dialog::Dialog(std::vector<DialogFieldProperties>& dialogFieldProperties)
    :dialogFieldProperties(dialogFieldProperties) {
    
}
    Dialog::~Dialog() {

        if (Dialog::winDialog_ != nullptr) {
            delwin(Dialog::winDialog_);
            Dialog::winDialog_ = nullptr;
        }
        if (Dialog::winDialogShadow_ != nullptr) {
            delwin(Dialog::winDialogShadow_);
            Dialog::winDialogShadow_ = nullptr;
        }

    }
    int Dialog::run(std::string& xmlFileName,std::ofstream & debugFile) {
        
        //WINDOW* winFormScreen = SharedWindowManager::getwinFormScreen();
        //WINDOW* winMsgArea = SharedWindowManager::getwinMsgArea();
        WINDOW* winDialog;
        char* errMsg = nullptr;  
        std::ifstream xmlFile;
        echo();
        std::vector<DialogFieldProperties> dialogFieldProperties;
        std::string slash = "";
#ifdef _WIN32
        slash = "\\";
#else
        slash = "/";
#endif

        namespace fs = std::filesystem;
        fs::path currentPath = fs::current_path();
        std::string fileName = currentPath.string();
        fileName.append(slash + xmlFileName);

        xmlFile.open(fileName);

        if (!xmlFile.is_open()) {
            debugFile << "Failed to open " + fileName + " file." << std::endl;
            return -1;           
        }
        // the next statement adds a empty element to the Dialog structure.
        // since this structure doesn't exist yet, the construtor is execuated
        // this empty element will be removed and the real values
        // will be added down futher in this function 

    
        dialogFieldProperties.emplace_back(
            nullptr,          // WINDOW* winFormScreen
            nullptr,          // WINDOW* winDialog
            1,                // int Choice
            "DialogOption1",    // const std::string& name
            "LABEL",          // const std::string& type
            10,               // int row
            20,               // int col
            20,               // int labelCol
            "1. Dummy",       // const std::string& labelText
            "N",              // std::string selectionIsEvent
            "",               // const std::string& nextRunType
            "",               // const std::string& nextRunClass
            "",               // const std::string& inputType
            "",               // const std::string& inputMask
            0,                // int inputLen
            0,                // int inputRow
            0,                // int inputCol
            0,                // int inputlabelCol
            ""                // const std::string& inputlabelText
        );
        std::string selectedChoice = "";
        stdscrRows_ = 0;
        stdscrCols_ = 0;
        bool valid = false;
        getmaxyx(stdscr, stdscrRows_, stdscrCols_);
        //xmlFile.clear(); // Clear any error flags
        //xmlFile.seekg(0, std::ios::beg); // Go back to the beginning of the file
        //valid = validateDialogSettingsXml(xmlFile, debugFile);
        //if (!valid)
        //   return -2;//"ScreenPropertiesXml Not Valid";
        //else
        //    winDialog = DialogWindowManager::getwinDialog(DialogHeight_, DialogWidth_);

        xmlFile.clear(); // Clear any error flags
        xmlFile.seekg(0, std::ios::beg); // Go back to the beginning of the file
        // TODO: validateDialogSettingsXml looks like it was copied from another func and
        // needs to be fixed
        valid = validateDialogSettingsXml(xmlFile, debugFile);
        valid = true;// debug 
        if (!valid)
            return -3;// "validateDialogSettingsXml Not Valid";
        else {
            winDialog = DialogWindowManager::getwinDialog(DialogHeight_, DialogWidth_);
            xmlFile.clear(); // Clear any error flags
            xmlFile.seekg(0, std::ios::beg); // Go back to the beginning of the file
            setupDialog(dialogFieldProperties, xmlFile, debugFile);

            // the first element is not needed because it is emplaced with nulls
            if (!dialogFieldProperties.empty()) {
                dialogFieldProperties.erase(dialogFieldProperties.begin());
            }

            selectedChoice = displayDialogAndGetChoice(dialogFieldProperties, debugFile);
          
        }
        DialogWindowManager::hidewinDialog();
        return std::stoi(selectedChoice);
     }
    
    int Dialog::setupDialog(std::vector<DialogFieldProperties>& dialogFieldProperties, std::ifstream& xmlFile, std::ofstream& debugFile) {
        std::string selectionName = "";
        std::string selectionType = "";
        int Choice = 0;
        //std::string Name = "";   
        int row = 0;
        int col = 0;
        int labelCol = 0;
        std::string labelText = "";
        std::string selectionisEvent = "N";
        std::string nextRunType = "";
        std::string nextRunClass = "";
        std::string inputType = "";
        std::string inputMask = "";
        int inputLen = 0;
        int inputRow = 0;
        int inputCol = 0;
        int inputlabelCol = 0;
        std::string inputlabelText = "";
        std::vector<std::string> selectionNames;

        if (!xmlFile.is_open()) {
            debugFile << "Error inside Dialog::setupDialog line 158 xmlfile is not open." << std::endl;
            return false;
        }
        // Read data from the file and initialize Dialog objects
        std::string line;
        while (std::getline(xmlFile, line)) {

            if (line.find("</selection>") != std::string::npos) {
                //We're at the end of the selection XML element, so add the selection to the vector
                if (!selectionName.empty()) {
                    selectionNames.emplace_back(selectionName);
                    //DialogOptions.emplace_back(WindowManager::winFormScreen_, winDialog_, "",Choice, selectionName, selectionType, row, col, labelCol, labelText, selectionisEvent, nextRunType, nextRunClass, inputType, inputMask, inputLen, inputRow, inputCol, inputlabelCol, inputlabelText);
                    dialogFieldProperties.emplace_back(
                        SharedWindowManager::winFormScreen_, // WINDOW* winFormScreen
                        Dialog::winDialog_,       // WINDOW* winDialog
                        Choice,                        // int Choice
                        selectionName,                 // const std::string& name
                        selectionType,                 // const std::string& type
                        row,                           // int row
                        col,                           // int col
                        labelCol,                      // int labelCol
                        labelText,                     // const std::string& labelText
                        selectionisEvent,              // std::string selectionIsEvent
                        nextRunType,                   // const std::string& nextRunType
                        nextRunClass,                  // const std::string& nextRunClass
                        inputType,                     // const std::string& inputType
                        inputMask,                     // const std::string& inputMask
                        inputLen,                      // int inputLen
                        inputRow,                      // int inputRow
                        inputCol,                      // int inputCol
                        inputlabelCol,                 // int inputlabelCol
                        inputlabelText                 // const std::string& inputlabelText
                    );


                }
            }
            std::string tag, value;
            //debugFile << "line" << line << std::endl;
            Dialog::extractTagAndValue(line, tag, value);
            //debugFile << "Tag: =" << tag << std::endl;
            if (tag == "name") {
                //debugFile << "Tag: =" << tag << std::endl;
            }
            //debugFile << " Value: =" << value << std::endl;
            //if (tag == "inputType") {
                //debugFile << " Value: =" << value << std::endl;
            //}

            if (tag == "Choice") {
                Choice = std::stoi(value);
            }
            else if (tag == "name") {
                selectionName = Dialog::removeLeadingSpaces(value, debugFile);
                selectionName.erase(std::remove(selectionName.begin(), selectionName.end(), '\"'));
                selectionNames.push_back(selectionName);
            }
            else if (tag == "type") {
                selectionType = value;
            }
            else if (tag == "row") {
                row = std::stoi(value);
            }
            else if (tag == "col") {
                col = std::stoi(value);
            }
            else if (tag == "labelCol") {
                labelCol = std::stoi(value);
            }
            else if (tag == "labelText") {
                labelText = Dialog::removeLeadingSpaces(value, debugFile);
                labelText.erase(std::remove(labelText.begin(), labelText.end(), '\"'));
            }
            else if (tag == "isEvent") {
                selectionisEvent = value;
                //if (Dialog::removeLeadingSpaces(value, debugFile) == "Y")
                //    selectionisEvent = true;
                //else
                //    selectionisEvent = false;
            }
            else if (tag == "nextRunType") {
                Dialog::removeLeadingSpaces(value, debugFile);
                nextRunType = value;
                nextRunType.erase(std::remove(nextRunType.begin(), nextRunType.end(), '\"'));

            }
            else if (tag == "nextRunClass") {
                Dialog::removeLeadingSpaces(value, debugFile);
                nextRunClass = value;
                nextRunClass.erase(std::remove(nextRunClass.begin(), nextRunClass.end(), '\"'));

            }

            else if (tag == "inputType") {
                Dialog::removeLeadingSpaces(value, debugFile);
                inputType = value;
                inputType.erase(std::remove(inputType.begin(), inputType.end(), '\"'));

            }
            else if (tag == "inputMask" && selectionType == "INPUT") {
                Dialog::removeLeadingSpaces(value, debugFile);
                inputMask = value;
                inputMask.erase(std::remove(inputMask.begin(), inputMask.end(), '\"'));

            }
            else if (tag == "inputLen" && selectionType == "INPUT") {
                inputLen = std::stoi(value);
            }
            else if (tag == "inputRow" && selectionType == "INPUT") {
                inputRow = std::stoi(value);
            }
            else if (tag == "inputCol" && selectionType == "INPUT") {
                inputCol = std::stoi(value);
            }
            else if (tag == "inputlabelCol" && selectionType == "INPUT") {
                inputlabelCol = std::stoi(value);
            }
            else if (tag == "inputlabelText" && selectionType == "INPUT") {
                Dialog::removeLeadingSpaces(value, debugFile);
                inputlabelText = value;
                inputlabelText.erase(std::remove(inputlabelText.begin(), inputlabelText.end(), '\"'));

            }

        }


        return true;
    }

    std::string  Dialog::displayDialogAndGetChoice(std::vector<DialogFieldProperties>& dialogFieldOptions, std::ofstream & debugFile) {
        WINDOW* winMsgArea = SharedWindowManager::getwinMsgArea();   
        
        WINDOW* winDialog= DialogWindowManager::getwinDialog(DialogHeight_, DialogWidth_);
        assert(winDialog != NULL);
        int startX = winDialog->_begx;
        int startY = winDialog->_begy; 
        //WINDOW* winDialogShadow=DialogWindowManager::createShadowForWindow(winDialog,DialogHeight_, DialogWidth_, startY, startX);
        wrefresh(winDialog);
        wrefresh(SharedWindowManager::winFormScreen_);
        //wrefresh(winDialogShadow);
        // Draw box if enabled
        if (DialogBoxed_ == "Y") {
            box(winDialog, 0, 0);
        }

        // Display the title
        mvwprintw(Dialog::winDialog_, 1, (DialogWidth_ - DialogTitle_.size()) / 2, "%s", DialogTitle_.c_str());
        //int y = 0;
        //int x = 0;
        std::string saveInputType = "";
        std::string saveInputMask = "";
        int saveInputLen = 0;
        int saveInputRow = 0;
        int saveInputCol = 0;
        int saveInputlabelCol = 0;
        std::string saveInputlabelText = "";
        // Display the Dialog options
        
        for (const auto& option : dialogFieldOptions) {
            //if (y == 0)
            //    y = option.row;

            if (option.type_ == "LABEL") {
                mvwprintw(winDialog, option.row_, option.labelCol_, "%s", option.labelText_.c_str());
                //y++;
               //x = option.labelCol;
            }
            if (option.type_ == "INPUT") {
                saveInputType = option.inputType_;
                saveInputMask = option.inputMask_;                
                saveInputLen = option.inputLen_;
                int x = option.inputRow_;
                x++;
                saveInputRow = x;//option.inputRow;
                saveInputCol = option.inputCol_;
                saveInputlabelCol = option.inputlabelCol_;
                saveInputlabelText = option.inputlabelText_;
            }
        }
     

        int ch = 0;
        // note this is deliberate to pass winDialog_ as the parameter 
        // elsewhere winFormScreen_ is passed 
        // 
        //DataEntryFieldProperties field(winDialog, winMsgArea, "", "","DialogChoice", saveInputType, saveInputMask,saveInputLen, saveInputRow, saveInputlabelCol,"",1, saveInputlabelText,"", "", "N","","","N","N","N","","","","");
        DialogFieldProperties dialogFieldProperties(

            winDialog,              // WINDOW* winDialog
            winMsgArea,          // WINDOW* winMsgArea
            1,                      // int Choice
            "DialogChoice",         // name
            saveInputType,          // type
            0,                      // row
            0,                      // col
            0,                      // labelCol
            "",                     // labelText
            "N",                    // selectionIsEvent
            "",                     // nextRunType
            "",                     // nextRunClass
            saveInputType,          // inputType
            saveInputMask,          // inputMask
            saveInputLen,           // inputLen
            saveInputRow,           // inputRow
            saveInputCol,           // inputCol
            saveInputlabelCol,      // inputlabelCol
            ""                      // inputlabelText
        );
        //DataEntryFieldProperties tempField(
        //    dialogFieldProperties.winFormScreen_,     // winFormScreen_
        //    dialogFieldProperties.winDialog_,         // winMsgArea_ (can substitute winDialog_ or nullptr)
        //    "",                             // inputKeyPressed_
        //    dialogFieldProperties.name_,              // fieldName_
        //    dialogFieldProperties.inputType_,         // fieldType_
        //    dialogFieldProperties.inputMask_,         // fieldMask_
        //    dialogFieldProperties.inputLen_,          // len_
        //    "N",                            // primaryKey_
        //    dialogFieldProperties.inputRow_,          // row_
        //    dialogFieldProperties.inputCol_,          // fieldColumn_
        //    "",                             // calc_
        //    "",                             // fieldValue_
        //    "",                             // allowedChoices_
        //    "",                             // choiceDescriptions_
        //    "N",                            // inquiryAllowed_
        //    "",                             // inquiryOrderBy_
        //    "",                             // inquiryFields_
        //    "Y",                            // fieldRequired_
        //    "N",                            // isDisplayOnly_
        //    "N"                             // hidden_
        //);

       
        //std::vector<DialogFieldProperties> dialogFieldProperties_;
       
        //dialogFieldProperties_.push_back(dialogFieldProperties);
        // Refresh to show the Dialog
        wrefresh(winDialog);
    
        int selectedChoice = -1;
        //DataEntry::displayLabels(field, debugFile);
        Dialog::displayLabels(dialogFieldProperties, debugFile);
    
        //do {
            DataEntryFieldProperties tempField = createDataEntryFieldFromDialogField(dialogFieldProperties);
            bool success = InputUtils::numericInput(tempField, debugFile);
           
            if (success) {
                selectedChoice = std::stoi(tempField.fieldValue_);
                //for (const auto& option : dialogFieldProperties) {
                //    if (option.Choice_ == selectedChoice) {  // Assuming Dialog choices are single digits
                //        selectedChoice = option.Choice_;
                //        break;
                //    }
                //}
            }
        //} while (selectedChoice == -1);  // Loop until a valid choice is made
    
     // pause for debugging    
    //while (ch = wgetch(Dialog::winDialog_) != '\n');
    DialogWindowManager::destroywinDialog();
    DialogWindowManager::destroywinDialogShadow();
    wrefresh(SharedWindowManager::winFormScreen_);
    std::string picked = std::to_string(selectedChoice);
    return picked;
}


std::string Dialog::removeLeadingSpaces(std::string& str, std::ofstream& debugFile) {
    // Find the position of the first non-space character
    size_t start = str.find_first_not_of(" \t");
    // If all characters are spaces, return an empty string
    if (start == std::string::npos) {
        return "";
    }
    // Extract the substring starting from the first non-space character
    return str.substr(start);
}
// Function to extract the value between XML tags
std::string Dialog::extractValue(const std::string& line) {
    // Extract the value between tags
    size_t startTagPos = line.find('>') + 1;
    size_t endTagPos = line.find('<', startTagPos);
    std::string valueStr = line.substr(startTagPos, endTagPos - startTagPos);
    return valueStr;
}
// Function to extract the tag and value between XML tags
void Dialog::extractTagAndValue(const std::string& line, std::string& tag, std::string& value) {
    
    std::string trimmedValue = line;
    trimmedValue.erase(0, trimmedValue.find_first_not_of(" \n\r\t"));
    trimmedValue.erase(trimmedValue.find_last_not_of(" \n\r\t") + 1);
    size_t startPos = trimmedValue.find("<");
    size_t endPos = trimmedValue.find(">");
    if (startPos != std::string::npos && endPos != std::string::npos) {
        tag = trimmedValue.substr(startPos + 1, endPos - startPos - 1);
        tag.erase(0, tag.find_first_not_of(" \n\r\t"));
        tag.erase(tag.find_last_not_of(" \n\r\t") + 1);
        size_t valueStartPos = trimmedValue.find(">", endPos);
        if (valueStartPos != std::string::npos) {
            value = trimmedValue.substr(valueStartPos + 1);
            size_t valueEndPos = value.find("</" + tag);
            if (valueEndPos != std::string::npos) { 
                value.erase(0, value.find_first_not_of(" \n\r\t"));
                value.erase(value.find_last_not_of(" \n\r\t") + 1);
                value = value.substr(0, valueEndPos);
            }
        }
    }
}
//this must have been copied and i forgot to change it to be for the different xml tags 
bool Dialog::validateDialogSettingsXml(std::ifstream& xmlFile, std::ofstream& debugFile) {
    if (!xmlFile.is_open()) {
        debugFile << "Error: XML file is not open." << std::endl;
        return false;
    }

    std::string line;
    bool ScreenPropertiesOpenFound = false;
    bool ScreenPropertiesCloseFound = false;
    bool typeOpenFound = false;
    bool titleOpenFound = false;
    bool borderOpenFound = false;
    bool widthOpenFound = false;
    bool heightOpenFound = false;
    bool typeCloseFound = false;
    bool titleCloseFound = false;
    bool borderCloseFound = false;
    bool widthCloseFound = false;
    bool heightCloseFound = false;
   
    

    int dummy1 = 0;
    int dummy2 = 0;

    // Loop through each line of the XML file to validate <ScreenProperties>
    while (std::getline(xmlFile, line)) {
        
        if (line.find("<ScreenProperties>") != std::string::npos) {
            ScreenPropertiesOpenFound = true;
        }

        
        if (ScreenPropertiesOpenFound) {
            if (line.find("<Type>") != std::string::npos) {
                typeOpenFound = true;
               
                DialogType_ = Dialog::extractValue(line);
                 
            }
            if (line.find("</Type>") != std::string::npos) {
                typeCloseFound = true;
            }
            if (line.find("<Title>") != std::string::npos) {
                titleOpenFound = true;
                DialogTitle_ = Dialog::extractValue(line);
            }
            if (line.find("</Title>") != std::string::npos) {
                titleCloseFound = true;
            }
            if (line.find("<Border>") != std::string::npos) {
                DialogBoxed_ = Dialog::extractValue(line);
                borderOpenFound = true;
            }
            if (line.find("</Border>") != std::string::npos) {
                borderCloseFound = true;
            }
            if (line.find("<Width>") != std::string::npos) {
                widthOpenFound = true;
                DialogWidth_ = std::stoi(Dialog::extractValue(line));
            }
            if (line.find("</Width>") != std::string::npos) {
                widthCloseFound = true;
            }
            if (line.find("<Height>") != std::string::npos) {
                heightOpenFound = true;
                DialogHeight_ = std::stoi(Dialog::extractValue(line));
            }
            if (line.find("</Height>") != std::string::npos) {
                heightCloseFound = true;
            }
            if (line.find("</ScreenProperties>") != std::string::npos) {
                ScreenPropertiesCloseFound = true;

            }
        }
                    
    }
  
 /*   debugFile << "Error: Missing closing tag for <Type> in <ScreenProperties>." << std::endl;
    debugFile << "Error: Missing closing tag for <Title> in <ScreenProperties>." << std::endl;
    debugFile << "Error: Missing closing tag for <Border> in <ScreenProperties>." << std::endl;
    debugFile << "Error: Missing closing tag for <Width> in <ScreenProperties>." << std::endl;
    debugFile << "Error: Missing closing tag for <Height> in <ScreenProperties>." << std::endl;*/

    // Check if all required elements were found
    if (!ScreenPropertiesOpenFound) {
        debugFile << "Error: <ScreenProperties> node not found in <ScreenConfiguration>." << std::endl;
       }
    if (!ScreenPropertiesCloseFound) {        
        debugFile << "Error: </ScreenProperties> in <ScreenConfiguration>." << std::endl;
       }
    if (!titleOpenFound) {
        debugFile << "Error: <Title> element missing in <ScreenProperties>." << std::endl;
    }
    if (!borderOpenFound) {
        debugFile << "Error: <Border> element missing in <ScreenProperties>." << std::endl;
    }
    if (!widthOpenFound) {
        debugFile << "Error: <Width> element missing in <ScreenProperties>." << std::endl;
    }
    if (!heightOpenFound) {
        debugFile << "Error: <Height> element missing in <ScreenProperties>." << std::endl;
    }
    if (!ScreenPropertiesCloseFound) {
        debugFile << "Error:  </ScreenProperties> missing in <ScreenConfiguration>." << std::endl;
        
    }
    if (!typeCloseFound) {
        debugFile << "Error: </Type> element missing in <ScreenProperties>." << std::endl;
    }
    if (!titleCloseFound) {
        debugFile << "Error: </Title> element missing in <ScreenProperties>." << std::endl;
    }
    if (!borderCloseFound) {
        debugFile << "Error: </Border> element missing in <ScreenProperties>." << std::endl;
    }
    if (!widthCloseFound) {
        debugFile << "Error: </Width> element missing in <ScreenProperties>." << std::endl;
    }
    if (!heightCloseFound) {
        debugFile << "Error: </Height> element missing in <ScreenProperties>." << std::endl;
    }

    // Return false if any required elements are missing or missing their closing tags
    if (!typeOpenFound || !titleOpenFound || !borderOpenFound || !widthOpenFound || !heightOpenFound||
        !typeCloseFound|| !titleCloseFound|| !borderCloseFound|| !widthCloseFound|| !heightCloseFound||
        !ScreenPropertiesOpenFound || !ScreenPropertiesCloseFound) {
        debugFile << "Error: Invalid XML structure in <ScreenProperties>." << std::endl;
        return false;
    }

    //  All elements are present, the validation is successful
    return true;
}
void Dialog::displayLabels(const DialogFieldProperties& dialogFieldProperties, std::ofstream& debugFile) {

    wattron(dialogFieldProperties.winFormScreen_, COLOR_PAIR(3));
    mvwprintw(dialogFieldProperties.winFormScreen_, dialogFieldProperties.inputlabelCol_, dialogFieldProperties.inputRow_, "%s", dialogFieldProperties.labelText_.c_str());
    wrefresh(dialogFieldProperties.winFormScreen_);
}
//DataEntryFieldProperties Dialog::createDataEntryFieldFromDialogField(const DialogFieldProperties& dialogFieldProperties) {
//    return DataEntryFieldProperties(
//        dialogFieldProperties.winFormScreen_,     // winFormScreen_
//        dialogFieldProperties.winDialog_,         // winMsgArea_
//        "",                             // inputKeyPressed_
//        dialogFieldProperties.name_,              // fieldName_
//        dialogFieldProperties.inputType_,         // fieldType_
//        dialogFieldProperties.inputMask_,         // fieldMask_
//        dialogFieldProperties.inputLen_,          // len_
//        "N",                            // primaryKey_
//        dialogFieldProperties.inputRow_,          // row_
//        dialogFieldProperties.inputCol_,          // fieldColumn_
//        "",                             // calc_
//        "",                             // fieldValue_
//        "",                             // allowedChoices_
//        "",                             // choiceDescriptions_
//        "N",                            // inquiryAllowed_
//        "",                             // inquiryOrderBy_
//        "",                             // inquiryFields_
//        "Y",                            // fieldRequired_
//        "N",                            // isDisplayOnly_
//        "N"                             // hidden_
//    );
//}
DataEntryFieldProperties Dialog::createDataEntryFieldFromDialogField(const DialogFieldProperties& dialogField) {
    return DataEntryFieldProperties(
        dialogField.winFormScreen_,
        dialogField.winDialog_,
        "",                         // inputKeyPressed_
        dialogField.name_,
        dialogField.inputType_,
        dialogField.inputMask_,
        dialogField.inputLen_,
        "N",                        // primaryKey_
        dialogField.inputRow_,
        dialogField.inputCol_,
        "",                         // calc_
        "",                         // fieldValue_
        "",                         // allowedChoices_
        "",                         // choiceDescriptions_
        "N",                        // inquiryAllowed_
        "",                         // inquiryOrderBy_
        "",                         // inquiryFields_
        "Y",                        // fieldRequired_
        "N"                        // isDisplayOnly_
        //"N"                         // hidden_
    );
}





