// Version-02.20.25.16.07
// File: NumericInput.cpp
#include "DataEntry.h"
#include "SharedWindowManager.h"
#include "InputUtils.h"
bool InputUtils::numericInput(DataEntryFieldProperties& field, std::ofstream& debugFile)
{
    std::string input = DataEntry::cleanString(field.fieldValue_);
    std::string mask = DataEntry::cleanString(field.fieldMask_);
    //WINDOW* savewinFormScreen = WindowManager::getwinFormScreen();
    WINDOW* savewinFormScreen = field.winFormScreen_;
    size_t saveLen = field.len_;
    int saveRow = field.row_;
    int saveColumn = field.fieldColumn_;
    int rows = savewinFormScreen->_maxy;
    int cols = savewinFormScreen->_maxx;
    size_t leftSize = 0;
    size_t rightSize = 0;
    bool decimalAllowed = false;
    size_t decimalPos = mask.find('.');
    if (decimalPos != std::string::npos) {
        decimalAllowed = true;    
    }

   inspectMask(mask, leftSize, rightSize,debugFile);
   DataEntry::displayRightToLeft(savewinFormScreen, input, saveRow, saveColumn, saveLen, 1, debugFile);

    int c = 0;
    int column = saveColumn;
    int charsEntered = 0;
    //int inputIndex = 0;
    bool isBackspace = false;
    bool decimalEntered = false;
    bool quit = false;
    bool gotoRestart = false;
    bool hasDecimalPoint = false;
    bool firstPass = true;
    std::string action = "";
    int pos = 0;

    wattron(savewinFormScreen, COLOR_PAIR(3));
    wrefresh(savewinFormScreen);
    noecho(); // Disable echoing of characters
    wrefresh(savewinFormScreen);

    while (quit != true)
    {
      
        wattroff(savewinFormScreen, COLOR_PAIR(3));
        wattron(savewinFormScreen, COLOR_PAIR(2));
        wmove(savewinFormScreen, saveRow, saveColumn + saveLen);
        wrefresh(savewinFormScreen);

        c = wgetch(savewinFormScreen);
        action = keyname(c);
        if (action == "^J")
        action = "ENTER_KEY";

        KeyType keyType = DataEntry::checkSpecialKeys(c);

        if (keyType == KeyType::FUNCTION_KEY) {
        wrefresh(savewinFormScreen);
        quit = true;
        }
        else if (keyType == KeyType::UP_ARROW) {
        wrefresh(savewinFormScreen);
        quit = true;
        }
        else if (keyType == KeyType::ENTER_KEY) {
        action = "KEY_ENTER";
        quit = true;
        }
        else {
        #pragma region //start of switch
        switch (c)
        {
        #pragma region// deal with delete key or known values representing backspace
        case KEY_DC: // delete key dec 330 hex 14
        action = "KEY_DC";
        [[fallthrough]];
        case 8: // Handle backspace Windows
            [[fallthrough]];
        case KEY_BACKSPACE:// Handle backspace Linux         
        action = "CASE_8";
        [[fallthrough]];
        case 127:// backspace
        action = "CASE_127";
        [[fallthrough]];
        case 462:// backspace
        action = "CASE_462";
        if (!input.empty()) {
        input.pop_back();
        }
        isBackspace = true;
        #pragma endregion // end of logic to deal with delete key or known values representing backspace
        // This is the end of the core logic that handles filtering function key or special key
        // we have determined the key pressed by user is a digit
        //---------------------------------------------------------
        #pragma region //beginning of the core logic that handles building the numeric input field.
        break;
        default:
        bool digit = false;
        // prevents error caused by testing isdigit
        // when function or control keys are pressed
        if (c >= 0 && c <= 127)
        {
        if (isdigit(c))
        digit = true;
        }
        if (digit == true) {
        size_t decimalPos = input.find('.');
        if (decimalPos == std::string::npos) {
        // No decimal point yet
        if (input.size() < leftSize) {
        input.push_back(c);
        }
        else if (input.size() == leftSize ) {
        // Automatically add the decimal point
            if (decimalAllowed == true)
                input.push_back('.');

        input.push_back(c);
        }
        }
        else {
        // Decimal point already present
        if (input.size() - decimalPos - 1 < rightSize) {
        input.push_back(c);
        }
        }
        }
        else if (c == '.' && input.find('.') == std::string::npos && input.size() <= leftSize && decimalAllowed == true) {
        // Allow manually entering the decimal point if it's not already present and it's valid to add it
        input.push_back(c);
        }
        }
        #pragma endregion //end of switch
        }

            DataEntry::displayRightToLeft(savewinFormScreen, input, saveRow, saveColumn, saveLen,1,debugFile);
        }
    if (quit == true) {
    //std::reverse(input.begin(), input.end());
    std::string inputResult = input;
    field.fieldValue_ = inputResult;
    field.inputKeyPressed_ = action;
    // may not need this here because it's done upon return to the collectData function
    //DataEntry::displayData(field, debugFile);
    wattroff(savewinFormScreen, COLOR_PAIR(2));
    wattron(savewinFormScreen, COLOR_PAIR(3));
    wrefresh(savewinFormScreen);
    // indicates a function was pressed and inputAction indicates which one
    return true;
}
// not sure how we need up here it's not referenced via a goto and returns are above
// however i stepped through code and it did reach this
// exitField:
    std::string inputResult = input;
    field.fieldValue_=inputResult;
    field.inputKeyPressed_=action;
    return false;// indicates exit field and a function was NOT pressed
}
void InputUtils::inspectMask(const std::string& format, size_t& leftSize, size_t& rightSize, std::ofstream& debugFile) {
    size_t decimalPos = format.find('.');
    if (decimalPos != std::string::npos) {
        leftSize = decimalPos;
        rightSize = format.size() - decimalPos - 1;
    }
    else {
        // If no decimal point is found, all characters are considered to the left of the decimal
        leftSize = format.size();
        rightSize = 0;
    }
}
