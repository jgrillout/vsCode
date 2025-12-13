// Version-02.20.25.16.07
// File: MenuEntry.cpp
#pragma once
#include "MenuEntry.h"
#include "MenuWindowManager.h"
#include "SharedWindowManager.h"

// Define static members of MenuEntry class

std::string MenuEntry::nextRunType_ = "";
std::string MenuEntry::nextRunClass_ = "";
std::string  MenuEntry::exitRunType_ = "";
std::string  MenuEntry::exitRunClass_ = "";

WINDOW* MenuEntry::winMenu_ = nullptr;
WINDOW* MenuEntry::winMenuShadow_ = nullptr;
int MenuEntry::stdscrRows_ = 0;
int MenuEntry::stdscrCols_ = 0;
int MenuEntry::len_ = 0;

std::string  MenuEntry::menuType_="";
std::string  MenuEntry::menuTitle_ = "";
std::string  MenuEntry::menuBordered_ ="Y";
int  MenuEntry::menuWidth_ = 0;
int  MenuEntry::menuHeight_ = 0;

std::string MenuEntry::inputType_="";
std::string MenuEntry::inputMask_="";
int MenuEntry::inputLen_=0;
int MenuEntry::inputRow_=0;
int MenuEntry::inputlabelCol_=0;
std::string MenuEntry::inputlabelText_="";
namespace fs = std::filesystem;

MenuEntry::MenuEntry(std::vector<MenuOptions>& menus)
    :menus_(menus) {
    
}
    MenuEntry::~MenuEntry() {

        if (MenuEntry::winMenu_ != nullptr) {
            delwin(MenuEntry::winMenu_);
            MenuEntry::winMenu_ = nullptr;
        }
        if (MenuEntry::winMenuShadow_ != nullptr) {
            delwin(MenuEntry::winMenuShadow_);
            MenuEntry::winMenuShadow_ = nullptr;
        }

    }
    std::string MenuEntry::run(std::ifstream & screenXmlFile, std::ofstream & debugFile, std::string& nextRunType, std::string& nextRunClass) {
       
        WINDOW* winFormScreen = SharedWindowManager::getwinFormScreen();        
        WINDOW* winMsgArea = SharedWindowManager::getwinMsgArea();
        WINDOW* winMenu;
        char* errMsg = nullptr;  
        echo();
        std::vector<MenuOptions> menus;

        // the next statement adds a empty element to the menu structure.
        // since this structure doesn't exist yet, the construtor is execuated
        // this empty element will be removed and the real values
        // will be added down futher in this function 

    
        menus.emplace_back(
            nullptr,          // WINDOW* winFormScreen
            nullptr,          // WINDOW* winMenu
            1,                // int Choice
            "MenuOption1",    // const std::string& name
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
        
        stdscrRows_ = 0;
        stdscrCols_ = 0;
        bool valid = false;
        getmaxyx(stdscr, stdscrRows_, stdscrCols_);
        screenXmlFile.clear(); // Clear any error flags
        screenXmlFile.seekg(0, std::ios::beg); // Go back to the beginning of the file
        valid = validateMenuSettingsXml(screenXmlFile, debugFile);
        if (!valid)
            return "Error";//"validateMenuSettingsXml Not Valid";
        else
            winMenu = MenuWindowManager::getwinMenu(menuHeight_, menuWidth_);

        screenXmlFile.clear(); // Clear any error flags
        screenXmlFile.seekg(0, std::ios::beg); // Go back to the beginning of the file
        valid = validateMenuChoicesXml(screenXmlFile, debugFile);
        if (!valid)
            return "Error";// "validateMenuChoicesXml Not Valid";
        else {
            screenXmlFile.clear(); // Clear any error flags
            screenXmlFile.seekg(0, std::ios::beg); // Go back to the beginning of the file
            setupMenu(menus, screenXmlFile, debugFile);

            // the first element is not needed because it is emplaced with nulls
            if (!menus.empty()) {
                menus.erase(menus.begin());
            }

            int selectedChoice = displayMenuAndGetChoice(menus, debugFile);
            if (selectedChoice == -9) {
                // -9 means F7 was pressed in numeric input
                if (DataEntry::confirmAction(2, 2, stdscrCols_, "Exit", COLOR_PAIR(1), KEY_F(7), debugFile)) {
                    return "Exit";
                    nextRunClass = exitRunClass_;
                    nextRunType = nextRunType_;
                    return nextRunClass;
                }
                
            }
            for (const auto& option : menus) {
                if (option.type == "LABEL" && option.Choice == selectedChoice) {
                    nextRunClass=option.nextRunClass;
                    nextRunType = option.nextRunType;
                    break;
                }
            }
        }
        return nextRunClass;
     }
    bool MenuEntry::validateMenuChoicesXml(std::ifstream & screenXmlFile, std::ofstream & debugFile) {

        if (!screenXmlFile.is_open()) {
            return false;
        }
        std::string line;
        bool result = true;
        bool menuChoicesOpenTagFound = false;
        bool menuChoicesCloseTagFound = false;
        bool selectionOpenTagFound = false;
        bool selectionCloseTagFound = false;
        bool allTagsFound = true;

        std::vector<std::pair<std::string, bool>> selectionTags = {
        {"Choice", false}, {"name", false}, {"type", false}, {"row", false},
        {"col", false}, {"labelCol", false}, {"labelText", false}, {"isEvent", false},{"nextRunType", false}, {"nextRunClass", false},
        {"inputType", false}, {"inputMask", false}, {"inputLen", false},
        {"inputRow", false}, {"inputlabelCol", false},{"inputlabelText", false}

        };

        while (std::getline(screenXmlFile, line)) {
            std::string cleanline = DataEntry::cleanString(line);
            
            //debugFile << line << std::endl;   
            //debugFile << "cleanline = " << cleanline << std::endl;

            if (line.find("<selection>") != std::string::npos) {
                selectionOpenTagFound = true;
            }
            if (line.find("</menuChoices>") != std::string::npos) {
                menuChoicesCloseTagFound = true;
                if (!menuChoicesOpenTagFound) {
                    debugFile << "Error: Found closing </menuChoices> tag  without opening <menuChoices> tag" << std::endl;
                    result = false;
                }
                else {

                    if (line.find("</selection>") != std::string::npos) {
                        selectionCloseTagFound = true;
                        if (!selectionOpenTagFound) {
                            debugFile << "Error: Found closing </selection> tag without opening <selection> tag" << std::endl;
                            result = false;
                        }

                    }
                }
            }
            if (!menuChoicesOpenTagFound) {
                std::string msg = " line = " + line;
                // Check if line contains the opening tag for selection
                if (line.find("<selection>") != std::string::npos) {
                    selectionOpenTagFound = true;
                }
                if (line.find("<menuChoices>") != std::string::npos) {
                    menuChoicesOpenTagFound = true;
                }
            }
            else {
                for (auto& tag : selectionTags) {
                    //debugFile << "***** tag.first " << tag.first << " tag.second " << tag.second << std::endl;
                    //if (tag.first == "nextRunClass")
                        //debugFile << "nextRunClass" << std::endl;
                    
                   
                    if (!cleanline.empty()) {
                        if (cleanline.find("<" + tag.first + ">") != std::string::npos) {
                           /* if (tag.first == "nextRunClass") {
                                debugFile << "nextRunClass" << std::endl;
                                debugFile << "!!!!! tag.first " << tag.first << " tag.second " << tag.second << std::endl;
                            }*/
                            if (!selectionOpenTagFound) {
                                debugFile << "Error: Found tag <" << tag.first << "> outside <selection> element" << std::endl;
                                result = false;
                            }
                            else {
                                tag.second = true;
                                //break;
                            }
                        }
                    }
                }
            }
        }

        
        if (!menuChoicesCloseTagFound) {
            debugFile << "Error: missing </menuChoices> tag" << std::endl;
            result = false;
        }
        else {
            allTagsFound = true;
            for (auto& tag : selectionTags) {
                if (tag.second == false) {
                    allTagsFound = false;
                    break;
                }
            }
        }
        if (allTagsFound == false) {
            debugFile << "Error: missing a selection tag" << std::endl;
            result = false;
        }
        if (!selectionCloseTagFound) {
            if (!selectionOpenTagFound) {
                debugFile << "Error: Found opening <selection> tag  without  closing </selection> tag " << std::endl;
                result = false;
            }
        }
        else {
            debugFile << "Error: missing closing </selection> tag" << std::endl;
            result = false;
        }
        return result;
    }
    int MenuEntry::setupMenu(std::vector<MenuOptions>&menuOptions, std::ifstream & screenXmlFile, std::ofstream & debugFile) {
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
        std::string inputType="";
        std::string inputMask="";
        int inputLen=0;
        int inputRow=0;
        int inputCol = 0;
        int inputlabelCol=0;
        std::string inputlabelText="";
        std::vector<std::string> selectionNames;

        if (!screenXmlFile.is_open()) {
            debugFile << "Error inside  MenuEntry::setupMenu line 258 screenXmlFile is not open." << std::endl;
            return false;
        }
        // Read data from the file and initialize MenuEntry objects
        std::string line;
        while (std::getline(screenXmlFile, line)) {

            if (line.find("</selection>") != std::string::npos) {
                //We're at the end of the selection XML element, so add the selection to the vector
                if (!selectionName.empty()) {
                    selectionNames.emplace_back(selectionName);
                    //menuOptions.emplace_back(WindowManager::winFormScreen_, winMenu_, "",Choice, selectionName, selectionType, row, col, labelCol, labelText, selectionisEvent, nextRunType, nextRunClass, inputType, inputMask, inputLen, inputRow, inputCol, inputlabelCol, inputlabelText);
                    menuOptions.emplace_back(
                        SharedWindowManager::winFormScreen_, // WINDOW* winFormScreen
                        MenuEntry::winMenu_,       // WINDOW* winMenu
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
            MenuEntry::extractTagAndValue(line, tag, value);
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
                    selectionName = MenuEntry::removeLeadingSpaces(value, debugFile);
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
                    labelText = MenuEntry::removeLeadingSpaces(value, debugFile);
                    labelText.erase(std::remove(labelText.begin(), labelText.end(), '\"'));
                }
                else if (tag == "isEvent") {
                    selectionisEvent = value;
                    //if (MenuEntry::removeLeadingSpaces(value, debugFile) == "Y")
                    //    selectionisEvent = true;
                    //else
                    //    selectionisEvent = false;
                }
                else if (tag == "nextRunType") {
                    MenuEntry::removeLeadingSpaces(value, debugFile);
                    nextRunType = value;
                    nextRunType.erase(std::remove(nextRunType.begin(), nextRunType.end(), '\"'));

                }
                else if (tag == "nextRunClass") {
                    MenuEntry::removeLeadingSpaces(value, debugFile);
                    nextRunClass = value;
                    nextRunClass.erase(std::remove(nextRunClass.begin(), nextRunClass.end(), '\"'));

                }
              
                else if (tag == "inputType") {
                    MenuEntry::removeLeadingSpaces(value, debugFile);
                    inputType = value;
                    inputType.erase(std::remove(inputType.begin(), inputType.end(), '\"'));

                }
                else if (tag == "inputMask" && selectionType =="INPUT") {
                    MenuEntry::removeLeadingSpaces(value, debugFile);
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
                    MenuEntry::removeLeadingSpaces(value, debugFile);
                    inputlabelText = value;
                    inputlabelText.erase(std::remove(inputlabelText.begin(), inputlabelText.end(), '\"'));

                }
                
            }
        

        return true;
    }

    int  MenuEntry::displayMenuAndGetChoice(std::vector<MenuOptions>&menuOptions, std::ofstream & debugFile) {
        WINDOW* winMsgArea = SharedWindowManager::getwinMsgArea();   
      
        WINDOW* winMenu= MenuWindowManager::getwinMenu(menuHeight_, menuWidth_);
        assert(winMenu != NULL);
        int startX = winMenu->_begx;
        int startY = winMenu->_begy; 
        WINDOW* winMenuShadow=MenuWindowManager::createShadowForWindow(winMenu,menuHeight_, menuWidth_, startY, startX);
        wrefresh(winMenu);
        wrefresh(SharedWindowManager::winFormScreen_);
        wrefresh(winMenuShadow);
        // Draw Border if enabled
        if (menuBordered_ == "Y") {
            box(winMenu, 0, 0);
        }

        // Display the title
        mvwprintw(MenuEntry::winMenu_, 1, (menuWidth_ - menuTitle_.size()) / 2, "%s", menuTitle_.c_str());
        //int y = 0;
        //int x = 0;
        std::string saveInputType = "";
        std::string saveInputMask = "";
        int saveInputLen = 0;
        int saveInputRow = 0;
        int saveInputlabelCol = 0;
        int inputRow = 0;
        std::string saveInputlabelText = "";
        // Display the menu options
        for (const auto& option : menuOptions) {
            //if (y == 0)
            //    y = option.row;

            if (option.type == "LABEL") {
                mvwprintw(winMenu, option.row, option.labelCol, "%s", option.labelText.c_str());
                //y++;
               //x = option.labelCol;
            }
            //inputRow = 0;
            if (option.type == "INPUT") {
                mvwprintw(winMenu, option.row, option.labelCol, "%s", option.inputlabelText.c_str());//option.labelText.c_str());
                saveInputType = option.inputType;
                saveInputMask = option.inputMask;                
                saveInputLen = option.inputLen;                 
                //inputRow++;
                saveInputRow = option.inputRow;//inputRow;
                saveInputlabelCol = option.inputlabelCol;
                //saveInputlabelText = option.labelText.c_str(); //option.inputlabelText;
            }
        }
        int ch = 0;
        // note this is deliberate to pass winMenu_ as the parameter 
        // elsewhere winFormScreen_ is passed 
        // 
        //DataEntryFieldProperties field(winMenu, winMsgArea, "", "","MenuChoice", saveInputType, saveInputMask, saveInputLen, saveInputRow, saveInputlabelCol,"",1, saveInputlabelText, "", "", "N","","","N","N","N","","","","");
        DataEntryFieldProperties field(
            winMenu,              // WINDOW* winFormScreen_
            winMsgArea,            // WINDOW* winMsgArea_
            "",                    // inputKeyPressed_
            "CUST",                // fieldName_
            "STRING",              // fieldType_
            "999999",              // fieldMask_
            6,                     // len_
            "Y",                   // primaryKey_
            saveInputRow,          // row_
            saveInputlabelCol,     // Column_
            "",                    // calc_
            "",                    // fieldValue_
            "",                    // allowedChoices_
            "",                    // choiceDescriptions_
            "Y",                   // inquiryAllowed_
            "CUST",                // inquiryOrderBy_
            "CUST,NAME",           // inquiryFields_
            "Y",                   // fieldRequired_
             "N"                   // isDisplayOnly_
          //""                     // hidden_
        );

        std::vector<DataEntryFieldProperties> EntryFields_;
       
        EntryFields_.push_back(field);
        // Refresh to show the menu
        wrefresh(winMenu);
    
        int selectedChoice = -1;
        //DataEntry::displayLabels(EntryFields_, debugFile);
        //MenuEntry::displayLabels(option, debugFile);
        do {

            bool success = InputUtils::numericInput(field, debugFile);
            
if (success) {
                if (field.inputKeyPressed_ != "KEY_F(7)") {
                    selectedChoice = std::stoi(field.fieldValue_);
                    for (const auto& option : menuOptions) {
                        if (option.Choice == selectedChoice) {  // Assuming menu choices are single digits
                            selectedChoice = option.Choice;
                            break;
                        }
                    }
                }
                else {
                    selectedChoice = -9;
                }
            }
        } while (selectedChoice == -1);  // Loop until a valid choice is made
    
     // pause for debugging    
    //while (ch = wgetch(MenuEntry::winMenu_) != '\n');
    MenuWindowManager::destroywinMenu();
    MenuWindowManager::destroywinMenuShadow();
    wrefresh(SharedWindowManager::winFormScreen_);
    return selectedChoice;
}


std::string MenuEntry::removeLeadingSpaces(std::string& str, std::ofstream& debugFile) {
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
std::string MenuEntry::extractValue(const std::string& line) {
    // Extract the value between tags
    size_t startTagPos = line.find('>') + 1;
    size_t endTagPos = line.find('<', startTagPos);
    std::string valueStr = line.substr(startTagPos, endTagPos - startTagPos);
    return valueStr;
}
// Function to extract the tag and value between XML tags
void MenuEntry::extractTagAndValue(const std::string& line, std::string& tag, std::string& value) {
    
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
bool MenuEntry::validateMenuSettingsXml(std::ifstream& screenXmlFile, std::ofstream& debugFile) {
    if (!screenXmlFile.is_open()) {
        debugFile << "Error: XML file is not open." << std::endl;
        return false;
    }

    std::string line;
    bool ScreenPropertiesOpenFound = false;
    bool ScreenPropertiesCloseFound = false;
    bool typeOpenFound = false;
    bool titleOpenFound = false;
    bool BorderOpenFound = false;
    bool widthOpenFound = false;
    bool heightOpenFound = false;
    bool typeCloseFound = false;
    bool titleCloseFound = false;
    bool BorderCloseFound = false;
    bool widthCloseFound = false;
    bool heightCloseFound = false;
    bool exitRunTypeFound = false;
    bool exitRunClassFound = false;
   
    

    int dummy1 = 0;
    int dummy2 = 0;

    // Loop through each line of the XML file to validate <ScreenProperties>
    while (std::getline(screenXmlFile, line)) {
        
        if (line.find("<ScreenProperties>") != std::string::npos) {
            ScreenPropertiesOpenFound = true;
        }

        // Validate elements within <ScreenProperties>
        if (ScreenPropertiesOpenFound) {
            if (line.find("<exitRunClass>") != std::string::npos) {
                exitRunClassFound = true;
                exitRunClass_ = MenuEntry::extractValue(line);

            }
            if (line.find("</exitRunClass>") != std::string::npos) {
                exitRunClassFound = true;                

            }
            if (line.find("<exitRunType>") != std::string::npos) {
                exitRunTypeFound = true;
                exitRunType_ = MenuEntry::extractValue(line);

            }
            if (line.find("</exitRunType>") != std::string::npos) {
                exitRunTypeFound = true;
            }
            if (line.find("<Type>") != std::string::npos) {
                typeOpenFound = true;               
                menuType_ = MenuEntry::extractValue(line);
                 
            }
            if (line.find("</Type>") != std::string::npos) {
                typeCloseFound = true;
            }
            if (line.find("<Title>") != std::string::npos) {
                titleOpenFound = true;
                menuTitle_ = MenuEntry::extractValue(line);
            }
            if (line.find("</Title>") != std::string::npos) {
                titleCloseFound = true;
            }
            if (line.find("<Border>") != std::string::npos) {
                menuBordered_ = MenuEntry::extractValue(line);
                BorderOpenFound = true;
            }
            if (line.find("</Border>") != std::string::npos) {
                BorderCloseFound = true;
            }
            if (line.find("<Width>") != std::string::npos) {
                widthOpenFound = true;
                menuWidth_ = std::stoi(MenuEntry::extractValue(line));
            }
            if (line.find("</Width>") != std::string::npos) {
                widthCloseFound = true;
            }
            if (line.find("<Height>") != std::string::npos) {
                heightOpenFound = true;
                menuHeight_ = std::stoi(MenuEntry::extractValue(line));
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
    if (!BorderOpenFound) {
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
    if (!BorderCloseFound) {
        debugFile << "Error: </Border> element missing in <ScreenProperties>." << std::endl;
    }
    if (!widthCloseFound) {
        debugFile << "Error: </Width> element missing in <ScreenProperties>." << std::endl;
    }
    if (!exitRunTypeFound) {
        debugFile << "Error: </exitRunType> element missing in <ScreenProperties>." << std::endl;
    }
    if (!exitRunClassFound) {
        debugFile << "Error: </exitRunClass> element missing in <ScreenProperties>." << std::endl;
    }

    // Return false if any required elements are missing or missing their closing tags
    if (!typeOpenFound || !titleOpenFound || !BorderOpenFound || !widthOpenFound || !heightOpenFound||
        !typeCloseFound|| !titleCloseFound|| !BorderCloseFound|| !widthCloseFound|| !heightCloseFound||
        !ScreenPropertiesOpenFound || !ScreenPropertiesCloseFound||
        !exitRunTypeFound || !exitRunClassFound)
    {
        debugFile << "Error: Invalid XML structure in <ScreenProperties>." << std::endl;
        return false;
    }

    //  All elements are present, the validation is successful
    return true;
}
//void MenuEntry::displayLabels(MenuOptions& menuOptions, std::ofstream& debugFile) {
//    WINDOW* winFormScreen = SharedWindowManager::getwinFormScreen();
//    wattron(winFormScreen, COLOR_PAIR(3));
//    mvwprintw(winFormScreen, menuOptions.row, menuOptions.labelCol, "%s", menuOptions.labelText.c_str());
//    wrefresh(winFormScreen);
//
//}



