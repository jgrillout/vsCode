// Version-02.20.25.16.07
// File: MaskedInput.cpp
//#pragma once
#include "DataEntry.h"
#include "InputUtils.h"
#include "SharedWindowManager.h"
bool InputUtils::maskedInput(DataEntryFieldProperties& field, std::ofstream& debugFile)
{
    std::string input = removeMask(field.fieldValue_, debugFile);
    input = DataEntry::cleanString(input);
    std::string mask = field.fieldMask_;
    mask = DataEntry::cleanString(mask);
    WINDOW* savewinFormScreen = SharedWindowManager::getwinFormScreen();
    size_t saveLen = field.len_;
    int saveRow = field.row_;
    int saveColumn = field.fieldColumn_;
    int rows = savewinFormScreen->_maxy;
    int cols = savewinFormScreen->_maxx;
    keypad(savewinFormScreen, TRUE); // Enable arrow keys for win
    //std::string mask = "(999) 999-9999";
    //std::string mask = "99/99/9999";
    std::string displayMaskStr = InputUtils::generateDisplayMask(mask);
    wattroff(savewinFormScreen, COLOR_PAIR(3));
    wattron(savewinFormScreen, COLOR_PAIR(2));
    InputUtils::displayMask(field, input, debugFile);
    // Set cursor to the first '9' position in the mask
    //int pos = 0;
    size_t pos = 0;
    while (pos < mask.size() && mask[pos] != '9') {
        ++pos;
    }
    wmove(savewinFormScreen, saveRow, saveColumn + pos);
    wrefresh(savewinFormScreen);
    int c = 0;
    int column = saveColumn;
    int charsEntered = 0;
    int inputPos = 0;
    size_t  erasePos = 0;
    bool isBackspace = false;
    bool quit = false;
    bool GotoRestart = false;
    bool firstpass = true;
    std::string Action = "";
    wattron(savewinFormScreen, COLOR_PAIR(3));
    wrefresh(savewinFormScreen);
    noecho(); // Disable echoing of characters
    wrefresh(savewinFormScreen);
    while (quit != true)
    {
        wattroff(savewinFormScreen, COLOR_PAIR(3));
        wattron(savewinFormScreen, COLOR_PAIR(2));
        wrefresh(savewinFormScreen);
        c = wgetch(savewinFormScreen);
        Action = keyname(c);
        if (Action == "^J")
            Action = "ENTER_KEY";                
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
            Action = "KEY_ENTER";
            quit = true;
        }
        else {
#pragma region //start of switch
            switch (c)
            {
#pragma region// deal with delete key or known values representing backspace
            case KEY_DC:  // delete key dec 330 hex 14
                Action = "KEY_DC";
                [[fallthrough]];
            case 8: // Handle backspace Windows    
                [[fallthrough]];
            case KEY_BACKSPACE:// Handle backspace Linux
                Action = "CASE_8";  
                [[fallthrough]];
            case 127:// backspace
                Action = "CASE_127";
                [[fallthrough]];
            case 462:// backspace
                Action = "CASE_462";
                isBackspace = true;
                if (pos == 0)
                    input = "";
                
                if (isBackspace == true && pos > 0) {
                    
                    while (pos > 0 && mask[pos - 1] != '9') {
                        --pos; // Move to the previous '9' position in the mask
                    }
                   
                    if (pos == 0)
                        input = "";
                    else if (pos > 0) {
                        --pos;
                        erasePos = std::count(mask.begin(), mask.begin() + pos, '9') + 1;
                        if (erasePos < input.size()) {
                            input.erase(erasePos, 1); // Erase character from input
                        }
                    }
                }
                InputUtils::displayMask(field,input,debugFile);
                break;
#pragma endregion // end of logic to deal with delete key or known values representing backspace
                // This is the end of the core logic that handles filtering funtion key or special key
                // we have determined the key pressed  by user is a digit
                //---------------------------------------------------------
#pragma region //beginning of the core logic that handles building the masked input field.
            default:
                bool digit = false;
                if (c >= 0 && c <= 127)
                {
                    if (isdigit(c))
                        digit = true;
                }
                if (digit == true || c == 32) {
                    if (pos < mask.size()) {
                        while (pos < mask.size() && mask[pos] != '9') {
                            ++pos; // Move to the next '9' position in the mask
                        }
                        if (pos < mask.size() && mask[pos] == '9') {
                            size_t  inputPos = std::count(mask.begin(), mask.begin() + pos, '9');
                            if (inputPos < input.size()) {
                                input[inputPos] = c; // Replace digit at correct position in input
                            }
                            else {
                                input.insert(inputPos, 1, c); // Insert digit at correct position in input
                            }
                            ++pos; // Move to the next position
                        }
                    }
                }
                else if (c == KEY_LEFT && pos > 0) {
                    do {
                        --pos; // Move left to the previous position
                    } while (pos > 0 && mask[pos] != '9');
                }
                else if (c == KEY_RIGHT && pos < mask.size() - 1) {
                    do {
                        ++pos; // Move right to the next position
                    } while (pos < mask.size() && mask[pos] != '9');
                }
                displayMask(field, input, debugFile);
                if (pos <= mask.size()) {
                    wmove(savewinFormScreen, saveRow, saveColumn + pos); // Move cursor to the next input position
                }
                wrefresh(savewinFormScreen);
            }
        }
    }
    if (quit == true) {
        std::string inputResult;
        //DataEntry::removeTrailingSpaces(input,debugFile);
        inputResult = DataEntry::cleanString(input);
        if (!input.empty()) {
            inputResult = InputUtils::combineInputWithMask(mask, input, debugFile);
            field.fieldValue_ = inputResult;           
        }
        else {
            inputResult = "";
            field.fieldValue_ = inputResult;
        }
        //dataEntry.setFieldValue(input);       
        field.inputKeyPressed_= Action;
        wattroff(savewinFormScreen, COLOR_PAIR(2));
        wattron(savewinFormScreen, COLOR_PAIR(3));
        wrefresh(savewinFormScreen);
        DataEntry::displayData(field,debugFile);
        // true indicates a funtion was  pressed and InputKeyPressed indicates which one
        return true;
    }

// not sure why this is here it's not referenced via a goto and returns are above
// exitField:
    std::string inputResult = "";
    //DataEntry::removeTrailingSpaces(input,debugFile);
    input = DataEntry::cleanString(input);
    if (!input.empty()) {
        inputResult = InputUtils::combineInputWithMask(mask, input, debugFile);
        field.fieldValue_ = inputResult;
    }
    else {
        inputResult = "";
        field.fieldValue_ = inputResult;
    }    
    //field.fieldValue_ = input;
    field.inputKeyPressed_ = Action;    
    DataEntry::displayData(field, debugFile);
    
    // false indicates exit field and  a funtion was NOT  pressed
    // most likely enter or up arrow got us here
    return false;
}
// Inspect the mask to determine the sizes on either side of a decimal point

// Function to combine the input with the mask
std::string InputUtils::combineInputWithMask(const std::string& mask, const std::string& input, std::ofstream& debugFile) {
    std::string combined = mask;
    size_t  inputIndex = 0;
    for (char& ch : combined) {
        if (ch == '9' && inputIndex < input.size()) {
            ch = input[inputIndex++]; // Replace '9' with input character
        }
    }
    return combined;
}
// Function to generate the initial display mask string
std::string InputUtils::generateDisplayMask(const std::string& mask) {
    std::string displayMaskStr = mask;
    for (char& ch : displayMaskStr) {
        if (ch == '9') {
            ch = ' '; // Replace '9' with space
        }
    }
    return displayMaskStr;
}

std::string InputUtils::removeMask(const std::string& maskedInput, std::ofstream& debugFile) {
    std::string result;
    std::copy_if(maskedInput.begin(), maskedInput.end(), std::back_inserter(result), [](char ch) {
        return std::isdigit(ch);
        });
    return result;
}
void InputUtils::displayMask(DataEntryFieldProperties& field, std::string& input, std::ofstream& debugFile) {
    
    //std::string unWanted = "\"";
    WINDOW* savewinFormScreen = SharedWindowManager::getwinFormScreen();
    int saveRow = field.row_;
    int saveColumn = field.fieldColumn_;

    //DataEntry::RemoveCharacters(input, unWanted, debugFile);
    input = DataEntry::cleanString(input);
    std::string mask = field.fieldMask_;
    //mask = DataEntry::RemoveCharacters(mask, unWanted, debugFile);
    mask = DataEntry::cleanString(mask);
    std::string displayMaskStr = generateDisplayMask(mask);
    mvwprintw(savewinFormScreen, saveRow, saveColumn, "%s", displayMaskStr.c_str()); // Print the display mask with spaces instead of '9'
    for (size_t  i = 0, j = 0; i < mask.size(); ++i) {
        wrefresh(savewinFormScreen);
        if (mask[i] == '9' && j < input.size()) {
            mvwaddch(savewinFormScreen, saveRow, saveColumn + i, input[j++]); // Display input character
        }
        else if (mask[i] != '9') {
            mvwaddch(savewinFormScreen, saveRow, saveColumn + i, mask[i]); // Display mask character
        }
    }

    int pos = input.find_first_of(" ");
    int adj = 0;
    if (pos <= 0) {
        pos = input.length();
        if (mask[pos] != '9' && pos > 0)
            pos++;
        else
            adj++;
        wmove(savewinFormScreen, saveRow, saveColumn + pos + adj);
    }
    else {
        wmove(savewinFormScreen, saveRow, saveColumn + pos);
    }
    if (input.empty()) {
        // Set cursor to the first '9' position in the mask
        size_t  pos = 0;
        while (pos < mask.size() && mask[pos] != '9') {
            ++pos;
        }
        wmove(savewinFormScreen, saveRow, saveColumn + pos);
    }
    wrefresh(savewinFormScreen);
    //****************************************    
    // if (pos < mask.size())                
    //     wmove(savewinFormScreen, saveRow, saveColumn + pos); // Move cursor to the next input position                
    // else if (pos = mask.size())
    //     wmove(savewinFormScreen, saveRow, saveColumn + pos-1); // Move cursor to the next input position
    // wrefresh(savewinFormScreen); // Refresh window to show changes
}
