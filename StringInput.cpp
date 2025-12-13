// Version-02.20.25.16.07
// File: StringInput.cpp
#include "InputUtils.h"
#include "DataEntry.h"
#include "SharedWindowManager.h"

bool InputUtils::stringInput(DataEntryFieldProperties& field, std::ofstream& debugFile) {
    std::string xStr = "";
    std::string editStr = "";
    std::string action = "";
    std::string before = "";
    std::string after = "";
    WINDOW* savewinFormScreen = SharedWindowManager::getwinFormScreen();
    size_t saveLength = field.len_;
    int saveRow = field.row_;
    int saveColumn = field.fieldColumn_;
    int rows = savewinFormScreen->_maxy;
    int cols = savewinFormScreen->_maxx;

    bool masked = false;
    bool end = false;
    bool insert = false;
    size_t pos = 0;
    size_t savePos = 0;
    size_t inputSize = 0;
    size_t index = 0;
    // Retrieve existing value (if any)
    editStr = field.fieldValue_;
    inputSize = editStr.size(); // Size of the initial value
    if (inputSize < saveLength) {
        editStr.append(saveLength - inputSize, ' ');
    }
    pos = editStr.find_last_not_of(' ') + 1;
    savePos = pos;
    // Enable keypad mode to capture function keys
    keypad(savewinFormScreen, TRUE);
    int c = 0;
    int test = 0;
    bool quit = false;
    mvwprintw(savewinFormScreen, saveRow, saveColumn, "%s", editStr.c_str());

    // a bug was causing everything on the line of the input field
    // was disappearing so this may fix the problem
    // Read and save the contents of the line after the input field ends
    std::vector<chtype> savedAfterLine = DataEntry::readAfterLineWithAttributes(savewinFormScreen, saveRow, saveColumn + saveLength);
    // Capture user input until Enter key is pressed
    while (!quit) {
        wattron(savewinFormScreen, COLOR_PAIR(2));
        mvwprintw(savewinFormScreen, saveRow, saveColumn, "%s", editStr.c_str());
        wmove(savewinFormScreen, saveRow, saveColumn + pos);
        wrefresh(savewinFormScreen);
        c = wgetch(savewinFormScreen);
        action = keyname(c);
        if (action == "^J") {
            action = "ENTER_KEY";
            // needed to work around a bug
            // After the Enter key is pressed, restore the part of the line after the input field
            DataEntry::restoreAfterLineWithAttributes(savewinFormScreen, saveRow, saveColumn + saveLength, savedAfterLine);

        }

        KeyType keyType = DataEntry::checkSpecialKeys(c);

        if (keyType == KeyType::FUNCTION_KEY) {
            wrefresh(savewinFormScreen);
            quit = true;
            break;
        }
        else if (keyType == KeyType::UP_ARROW) {
            wrefresh(savewinFormScreen);
            quit = true;
            break;
        }
        else if (keyType == KeyType::ENTER_KEY) {
            action = "KEY_ENTER";
            quit = true;
            break;
        }
        else if (keyType == KeyType::ALPHA_NUM_KEY) {
            action = "ALPHA_NUM_KEY";            
            //break;
        }
        else {
                action = "KEY_NOT"; // Deal with keys I haven't planned on handling            
                quit = true;
                break;
            }
        
        //---------------------------------------------------------
        switch (c) {
        case KEY_LEFT:
            if (pos == 0) {
                beep();
            }
            else {
                --pos;
            }
            break;
        case KEY_RIGHT:
            if (pos + 1 >= saveLength) {
                beep();
                pos = saveLength;
            }
            else {
                pos++;
            }
            break;
        case KEY_END:
            index = editStr.find_last_not_of(' ');
            pos = index + 1;
            break;
        case KEY_HOME:
            pos = 0;
            break;
        case KEY_DC:  // Delete
            if (saveLength <= 1)
                editStr = " ";
            else {
                if (pos < editStr.length()) //(pos - 1 >= 0)
                    before = editStr.substr(0, pos);
                if (pos + 1 <= editStr.length())//(pos + 1 <= editStr.length())
                    after = editStr.substr(pos + 1);
                editStr = before + after+" ";
            }
            break;
        case PLATFORM_STANDARD_INSERT_KEY:
        case PLATFORM_INSERT_KEY:
            insert = !insert;
            if (insert) {
                wattron(savewinFormScreen, COLOR_PAIR(2));
                mvwprintw(savewinFormScreen, rows - 3, cols - 8, "Insert");
            }
            else {
                wattron(savewinFormScreen, COLOR_PAIR(3));
                mvwprintw(savewinFormScreen, rows - 3, cols - 8, "       ");
                wattron(savewinFormScreen, COLOR_PAIR(2));
            }
            break;
        case BACKSPACE_KEY:// Handle backspace
            before = "";
            after = "";
            if (pos == 1) {
                editStr = " ";
                pos = 0;
            }
            else {
                if (pos == 0)
                    beep();
                else {
                    savePos = pos;
                    --pos;
                }
                if (savePos != pos) {
                    if (pos != saveLength) {
                        if (pos - 1 >= 0)
                            //before = editStr.substr(0, pos - 1);
                            before = editStr.substr(0, pos);
                        if (pos + 1 <= editStr.length())
                            after = editStr.substr(pos + 1);
                        editStr = before + " " + after;
                    }
                }
            }
            break;
            //---------------------------------------------------------
        default:
            if (pos >= saveLength) {
                beep();
                break;
            }
            if (c >= 0 && c <= 127) {
                if (pos > saveLength) {
                    beep();
                    break;
                }
            }
            
            if (std::isprint(c)) {
                char character = static_cast<char>(c);
                if (test == 24) { // Input type is CHOICE
                    if (field.allowedChoices_.find(character) == std::string::npos) {
                        beep();
                        break;
                    }
                }
                std::string chString(1, character);
                if (insert) {
                    if (pos != character) {
                        xStr = editStr.substr(pos, 1);
                        before = editStr.substr(0, pos);
                        after = editStr.substr(pos);
                        editStr = before + chString + after;
                        editStr = editStr.substr(0, character);
                    }
                    else {
                        beep();
                    }
                }
                else {
                    editStr.replace(pos, 1, chString);
                }
                pos++;
                if (pos >= saveLength) {
                    //break;
                    //end = true;
                    pos = saveLength;
                }
            }
            wrefresh(savewinFormScreen);
        }
    }

    if (quit) {
        field.fieldValue_ = editStr;

        field.inputKeyPressed_ = action;
        // Indicates a function was pressed and InputKeyPressed indicates which one
        return true;
    }

    field.fieldValue_ = editStr;
    field.inputKeyPressed_ = action;
    wattroff(savewinFormScreen, COLOR_PAIR(2));
    wattron(savewinFormScreen, COLOR_PAIR(3));
    wrefresh(savewinFormScreen);
    return false; // Indicates we have a string and a function was NOT pressed
}
