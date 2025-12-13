//Version: 01.20.25.18.24
// File: DataEntryUtils.cpp

//#include "DataEntry.h"
//int DataEntryUtils::collectData(std::ofstream& debugFile) {
//RecordEntryStart:
//    std::string Result = "";
//    wclear(winFullScreen_);
//    wattron(winFullScreen_, COLOR_PAIR(3));
//    wbkgd(winFullScreen_, COLOR_PAIR(3));
//    box(winFullScreen_, ACS_VLINE, ACS_HLINE);
//    DataEntryUtils::printInMiddle(winFullScreen_, 0, 0, 0, screenTitle_, COLOR_PAIR(3), debugFile);
//    int functionKeyRow = stdscrRows_ - 2;
//    int functionKeyCount = 5;
//    bool firstTime = true, quit = false;
//    bool EditingRecord = false, AddingNew = false, prompt = false;
//    int test = 0;
//    std::string msg = "";
//    std::string temp = "";
//    // Display all fields Labels & defualt data(if any)
//    for (auto& field : fields_) {
//        field.displayLabels(debugFile);
//        DataEntry::displayData(debugFile);
//    }
//    size_t  index = 0;
//    std::vector<std::string>  fieldValues;
//    while (index < fields_.size()) {
//    BeginField:
//        //// weird bug have to turn off before on
//        //wattroff(winFullScreen, A_REVERSE);
//        wattron(winFullScreen_, A_REVERSE);
//        int functionKeyRow = stdscrRows_ - 2;
//        int functionKeyCount = 5;
//        int functionKeygaps = 1;
//        // for spacing to look good, the length of  the text for the fuction key labels must be the same
//        mvwprintw(winFullScreen_, functionKeyRow, functionKeygaps * 2, " F2 Lookup  ");
//        wrefresh(winFullScreen_);
//        functionKeygaps = stdscrCols_ / functionKeyCount;
//        mvwprintw(winFullScreen_, functionKeyRow, functionKeygaps * 1, " F3  Delete ");
//        mvwprintw(winFullScreen_, functionKeyRow, functionKeygaps * 2, "  F4  Save  ");
//        mvwprintw(winFullScreen_, functionKeyRow, functionKeygaps * 3, " F5 Restart ");
//        mvwprintw(winFullScreen_, functionKeyRow, functionKeygaps * 4, "  F7  Exit  ");
//        wrefresh(winFullScreen_);
//        wattroff(winFullScreen_, A_REVERSE);
//
//        wrefresh(winFullScreen_);
//
//        // test to prevent out of range error/core dump. 
//        // this this is only necessay because I am using a goto statement within the while
//        // I don't know how to avoid using the goto in this case
//        if (index > fields.size())
//            index--;
//        auto& field = fields[index];
//
//        //**********ACCEPT INPUT ************
//        std::string InputKeyPressed;
//        bool FunctionKey = field.acceptInput(field, debugFile);
//        InputKeyPressed = field.getInputKeyPressed();
//        // FunctionKey is true for enter key. May have to revist this decision
//        if (!FunctionKey)
//            goto notFunctionKey;
//        temp = field.getFieldValue();
//        if (InputKeyPressed == "KEY_F(2)" && field.getInquiryAllowed() == "N") {
//            goto BeginField;
//        }
//        if (InputKeyPressed == "KEY_F(2)" || InputKeyPressed == "KEY_F(5)" || InputKeyPressed == "KEY_F(7)")
//            goto functionKeys;
//        //TODO: use field.getPrimaryKey instead of idex==0?
//    // first field should always be the primary key and cant allow blank value
//        if (index == 0 && DataEntryUtils::removeLeadingSpaces(temp) == "") {
//            goto BeginField;
//        }
//        field.setFieldValue(temp);
//        fieldValues.push_back(temp);
//        DataEntry::displayData();
//        wrefresh(winFullScreen_);
//    functionKeys:
//        // not sure why i decided to classify the enter key as a function key.
//        // apparently necessary at the time
//        if (InputKeyPressed != "KEY_ENTER") {
//            std::string inquiryOrderBy = field.getInquiryOrderBy();
//            Result = DataEntryUtils::doFunctionKeys(field, arg1, AddingNew, lib, condition, inquiryOrderBy, fields, debugFile, InputKeyPressed);
//            if (Result == "RecordEntryStart") {
//                for (auto& field : fields_) {
//                    field.fieldValue = "";
//                }
//                goto  RecordEntryStart; // displayData; 
//            }
//            if (Result == "Exit")
//                goto Exit;
//            if (Result == "Error")
//                goto FunctionKeyError;
//            //TryReadRecord means F2 was used to lookup a record
//            // condition contains the key to read
//            if (Result == "TryReadRecord") {
//                Result = "";
//                AddingNew = false;
//                EditingRecord = false;
//                prompt = false;
//                Result = DataEntryUtils::readRecord(field, arg1, AddingNew, prompt, lib, condition, fields);
//                if (Result == "Error")
//                    goto handleError;
//                if (Result == "RecordNotFound" || Result == "RecordEntryStart") {
//                    index = 0;
//                    for (auto& field : fields_) {
//                        if (index > 0) {
//                            field.fieldValue = "";
//                            index++;
//                        }
//                        goto RecordEntryStart;
//                    }
//                    goto doDisplayData;
//                }
//                if (Result == "EditingRecord") {
//                    AddingNew = false;
//                    EditingRecord = true;
//                    if (index == 0)
//                        index = 1;
//                    goto doDisplayData;
//                }
//                //yes this is needed again
//                //if (Result == "RecordEntryStart") {
//                //    for (auto& field : fields) {
//                //        field.setFieldValue("");
//                //    }
//                //    goto  RecordEntryStart; // displayData; 
//                //}
//            }
//        }
//    notFunctionKey:
//        temp = field.getFieldValue();
//        int test = DataEntryUtils::stringSwitchNumber(field.getInputKeyPressed());
//        if (test != -99) {
//            switch (test) {
//            case -1:  //"KEY_UP"
//                if (index > 1) {
//                    --index;
//                    field.setFieldValue(temp);
//                    fieldValues.push_back(temp);
//                    std::string type = field.getFieldType();
//                    DataEntry::displayData();
//                    if (index < fields.size()) {
//                        auto& field = fields[index];
//                        std::string tempnam = field.getFieldName();
//                        std::string tempval = field.getFieldValue();
//                        //debugFile << "index = " << index << " tempnam = " << tempnam << " tempval " << tempval << std::endl;
//                        goto BeginField;
//                    }
//                }
//                else
//                    index = 1;
//                goto BeginField;
//#pragma region //start of code for 'KEY_ENTER' case
//            case 0: //KEY_ENTER
//            case PLATFORM_ENTER_KEY:
//            case 16://KEY_ENTER under special circumstance not sure why getch returned 16
//            // here we can check the value input by the user after they press enter
//                if (field.getFieldRequired() == "Y") {
//                    std::string temp = field.getFieldValue();
//                    DataEntryUtils::removeTrailingSpaces(temp);
//                    if (temp == "") {
//                        std::string msg = "Entry is required";
//                        bool fKey = DataEntryUtils::errMsg(field, 2, stdscrCols_, msg, debugFile);
//
//                        if (fKey == true && field.getInputKeyPressed() == "KEY_F(7)") {
//                            if (DataEntryUtils::confirmAction(field, 2, 2, stdscrCols_, "Exit", COLOR_PAIR(1), KEY_F(7), debugFile) == true)
//                            {
//                                goto Exit;
//                            }
//                        }
//                        goto BeginField;
//                    }
//                }
//                //primary key  should always be the first field
//                if (index == 0 && !fieldNames.empty() && field.getFieldName() == fieldNames[index]) {
//                    condition = fieldNames[0] + " = \'" + fields[0].getFieldValue() + "\'";
//                    Result = "";
//                    AddingNew = false;
//                    EditingRecord = false;
//                    bool prompt = true;
//                    Result = DataEntryUtils::readRecord(field, arg1, AddingNew, prompt, lib, condition, fields);
//                    if (Result == "Error")
//                        goto handleError;
//                    if (Result == "EditingRecord") {
//                        AddingNew = false;
//                        EditingRecord = true;
//                        if (index == 0)
//                            index = 1;
//                        goto doDisplayData;
//                    }
//
//                    //if (Result == "RecordEntryStart") {
//                    //    for (auto& field : fields) {
//                    //        field.setFieldValue("");
//                    //    }
//                    //    goto RecordEntryStart;
//                    //}
//
//
//
//                    //// yes this is needed again
//                    //if (Result == "RecordEntryStart") {
//                    //    for (auto& field : fields) {
//                    //        field.setFieldValue("");
//                    //    }
//                    //    goto  RecordEntryStart; // displayData; 
//                    //}
//
//                    if (Result == "AddNewRecord") {
//                        if (index == 0)
//                            index = 1;
//                        AddingNew = true;
//                        EditingRecord = false;
//                        wattron(winFullScreen_, COLOR_PAIR(3));
//
//                        mvwprintw(winFullScreen_, field.row, field.col, "%s", field.fieldValue.c_str());
//                        wrefresh(winFullScreen_);
//                        goto BeginField;
//                    }
//                    if (Result == "RecordNotFound" || Result == "RecordEntryStart") {
//                        index = 0;
//                        for (auto& field : fields_) {
//                            if (index > 0) {
//                                field.fieldValue = "";
//                                index++;
//                            }
//                        }
//                        goto RecordEntryStart;
//                    }
//                }
//                DataEntry::displayData();
//                index++;
//#pragma endregion // end of code for 'KEY_ENTER' case
//            }
//        }
//    }
//    // if we reached this point that all field have been entered
//    // loop back around to 2nd field on screen (without clearing fields)
//    // another option,instead of looping, would be to prompt the user to save the record
//    index = 1; // index 1 is second field we don't want index 0  because it's the primary key field
//    goto BeginField;
//doDisplayData:
//    for (auto& field : fields_) {
//        DataEntry::displayData();
//    }
//    wrefresh(winFullScreen_);
//    wattroff(winFullScreen_, COLOR_PAIR(2));
//    if (EditingRecord) {
//        index = 1;
//        goto BeginField;
//    }
//    if (!AddingNew)
//        goto RecordEntryStart;
//    else {
//        if (index < fields.size() - 1) {
//            index++;
//            goto BeginField;
//        }
//    }
//FunctionKeyError:
//    msg = "FunctionKeyError occured";
//    DataEntryUtils::errMsg(field, 2, stdscrCols_, msg, debugFile);
//    if (DataEntryUtils::confirmAction(field, 2, 2, stdscrCols_, "Exit", COLOR_PAIR(1), KEY_F(7), debugFile) == true)
//    {
//        goto Exit;
//    }
//handleError:
//    msg = "handleError occured";
//    DataEntryUtils::errMsg(field, 2, stdscrCols_, msg, debugFile);
//    if (DataEntryUtils::confirmAction(dummy, 2, 2, stdscrCols_, "Exit", COLOR_PAIR(1), KEY_F(7), debugFile) == true)
//    {
//        goto Exit;
//    }
//Exit:
//    mvwprintw(winFullScreen_, 7, 1, "Press Enter to exit...");
//    debugFile.close();
//    xmlFile_.close();
//    wrefresh(winFullScreen_);
//    wgetch(winFullScreen_);
//    // End curses mode
//    endwin();
//    //TODO: what memory clean up should be done?
//    return 1;
//}
//
//bool DataEntryUtils::acceptInput(FieldProperties& field, std::ofstream& debugFile) {
//    int winRows = 0, winCols = 0;
//    getmaxyx(field.winFullScreen, winRows, winCols);
//    bool result = false;
//    bool validDate = false;
//    bool fKey = false;
//    std::string inputAction;
//    std::string type = field.fieldType;
//    std::string temp = field.fieldValue;
//    int test = DataEntryUtils::stringSwitchNumber(type);
//    switch (test) {
//    case 17: // STRING
//        fKey = DataEntryUtils::stringInput(field, debugFile, test);
//        break;
//    case 18: // NUMERIC
//        fKey = DataEntryUtils::numericInput(field, debugFile);
//        break;
//    case 19: // MASKED
//        fKey = DataEntryUtils::maskedInput(field, debugFile);
//        break;
//    case 23: // DATE
//        do {
//            fKey = DataEntryUtils::maskedInput(field, debugFile);
//            if (fKey) break;
//            validDate = DataEntryUtils::isValidDate(field.fieldValue.c_str());
//            if (!validDate) {
//                std::string msg = "Invalid Date";
//                fKey = DataEntryUtils::errMsg(field, 2, winCols, msg, debugFile);
//                inputAction = field.inputKeyPressed;
//                if (fKey == true && (inputAction == "KEY_F(5)" || inputAction == "KEY_F(7)")) break;
//            }
//        } while (!validDate);
//        if (validDate) {
//            std::string temp = field.fieldValue.c_str();
//            field.fieldValue = temp;
//        }
//        break;
//    case 24: // CHOICE
//        //herehere
//        std::string temp = field.choiceDescriptions;
//        DataEntryUtils::printInMiddle(field.winFullScreen, 1, 1, 79, temp, COLOR_PAIR(3), debugFile);
//        fKey = DataEntryUtils::stringInput(field, debugFile, test);
//        DataEntryUtils::hideWindow(field);
//        break;
//    }
//    return fKey;
//}
//bool DataEntryUtils::isLeapYear(int year) {
//    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
//}
//bool DataEntryUtils::isValidDate(const std::string& date) {
//    int day, month, year;
//    char delimiter1, delimiter2;
//    std::istringstream dateStream(date);
//    dateStream >> std::setw(2) >> month >> delimiter1 >> std::setw(2) >> day >> delimiter2 >> std::setw(4) >> year;
//    if (delimiter1 != '/' || delimiter2 != '/' || dateStream.fail() || month < 1 || month > 12 || day < 1 || year < 1) {
//        return false;
//    }
//    // Days in each month
//    int daysInMonth[] = { 31, (isLeapYear(year) ? 29 : 28), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
//    return day <= daysInMonth[month - 1];
//}
//
//void DataEntryUtils::parseCSVWithQuotes(std::string& line, std::vector<std::string>& tokens) {
//    std::string token;
//    std::istringstream stream(line);
//    char c;
//    bool inQuotes = false;
//    while (stream.get(c)) {
//        if (c == '"' && (stream.peek() != '"' || !inQuotes)) { // Handle quote toggle
//            inQuotes = !inQuotes;
//        }
//        else if (c == ',' && !inQuotes) { // End of a token
//            tokens.push_back(token);
//            token.clear();
//        }
//        else if (c == '"' && inQuotes && stream.peek() == '"') { // Handle escaped quote
//            token += c; // Add one quote of the escaped pair
//            stream.get(c); // Skip the next quote
//        }
//        else {
//            token += c;
//        }
//    }
//    if (!token.empty()) tokens.push_back(token); // Add the last token
//}
//
//void DataEntryUtils::debugMsg(FieldProperties& field, std::string string1, std::string string2, int val) {
//    WINDOW* WinMsgArea = dataEntry.getWinMsgArea();
//    WINDOW* winfullScreen = dataEntry.getWinFullScreen();
//    std::string result;
//    // Create a stringstream
//    std::stringstream ss;
//    // Insert the string and integer into the stringstream
//    ss << "string1 = " << string1 << "val " << val;
//    // Extract the concatenated string from the stringstream
//    result = ss.str();
//    wmove(WinMsgArea, 22, 2);
//    //clrtoeol();
//    const char* myCString = result.c_str();
//    wmove(WinMsgArea, 22, 2);
//    wclrtoeol(WinMsgArea);
//    mvwprintw(WinMsgArea, 22, 2, "%s", myCString);
//    wrefresh(WinMsgArea);
//    wrefresh(winfullScreen);
//    wgetch(WinMsgArea);
//}
//bool DataEntryUtils::errMsg(FieldProperties& field, int row, int col, std::string& msg, std::ofstream& debugFile) {
//    WINDOW* winMsgArea = field.winMsgArea;
//    WINDOW* winfullScreen = field.winFullScreen;
//    std::string action = "";
//    int outRow, outCol, width;
//    int winRows = 0, winCols = 0, ch = 0;
//    if (winMsgArea == nullptr) {
//        //debugMsg(dataEntry,"winMsgArea == nullptr", "", 0);
//        winMsgArea = stdscr;
//    }
//    getmaxyx(winMsgArea, winRows, winCols);
//    width = winCols;
//    beep();
//    wattron(winMsgArea, COLOR_PAIR(3));
//    wmove(winMsgArea, row, 1);
//    wclrtoeol(winMsgArea);
//    findMiddle(winMsgArea, row, col, outRow, outCol, width, msg);
//    printInMiddle(winMsgArea, row, col, 79, msg, COLOR_PAIR(1), debugFile);
//    box(winMsgArea, 0, 0);
//    //wattroff(winMsgArea, A_BLINK);
//    wrefresh(winMsgArea);
//    ch = wgetch(winMsgArea);
//    switch (ch) {
//        //case PADENTER:
//    case 10:  // Enter key
//        action = "KEY_ENTER";
//        break;
//    case KEY_F(5):
//        action = "KEY_F(5)";
//        break;
//    case KEY_F(7):
//        action = "KEY_F(7)";
//        break;
//    }
//    hideWindow(dataEntry);
//    dataEntry.setInputKeyPressed(action);
//    // indicates a function was pressed and inputKeyPressed indicates which one
//    return true;
//    // indicates a function was pressed and inputAction indicates which one
//    if (action == "KEY_ENTER")
//        return false;
//    else
//        return true;
//}
//void DataEntryUtils::printInMiddle(WINDOW* win, int startRow, int startCol, int width, std::string& msg, chtype color, std::ofstream& debugFile) {
//    if (win == nullptr)
//        win = stdscr;
//    width = getmaxx(win);
//    int outRow = 0, outCol = 0;
//    findMiddle(win, startRow, startCol, outRow, outCol, width, msg);
//    mvwprintw(win, outRow, outCol, "%s", msg.c_str());
//    wrefresh(win);
//}
//void DataEntryUtils::findMiddle(WINDOW* win, int startRow, int startCol, int& outRow, int& outCol, int width, std::string msg) {
//    int length, col, row;
//    float temp;
//    //if (win == nullptr)
//    //debugMsg(dataEntry,"findMiddle", "nullptr window passed", 0);
//    if (win == nullptr)
//        win = stdscr;
//    //getyx(win, row, col);
//    getmaxyx(win, row, col);
//    if (width == 0)
//        width = col;
//    if (startCol != 0)
//        col = startCol;
//    if (startRow != 0)
//        row = startRow;
//    if (width == 0)
//        width = col;
//    temp = ((width - int(msg.length())) / 2);
//    //outCol = startCol + (int)temp;
//    outCol = (int)temp;
//    outRow = row;
//}
//std::string DataEntryUtils::askQuestion(WINDOW* win, int startRow, int startCol, int width, std::string prompt, chtype color) {
//restart:
//    echo();
//    char xstr[] = { ' ' };
//    int input;
//    wmove(win, startRow, startCol);
//    wclrtoeol(win);
//    box(win, 0, 0);
//    int outRow, outCol;
//    findMiddle(win, startRow, startCol, outRow, outCol, width, prompt);
//    //wattron(win, color);
//    mvwprintw(win, outRow, outCol, "%s", prompt.c_str());
//    //wattroff(win, color);
//    box(win, 0, 0);
//    wrefresh(win);
//    input = wgetch(win);
//    wmove(win, outRow, outCol);
//    //wclrtoeol(win);
//    werase(win);
//    wrefresh(win);
//    noecho();
//    if (input == 'y' || input == 'Y') {
//        return "Yes";
//    }
//    else {
//        return "No";
//    }
//}
//bool DataEntryUtils::confirmAction(FieldProperties& field, int startRow, int startCol, int width, std::string prompt, chtype color, int keyToPress, std::ofstream& debugFile) {
//    WINDOW* winMsgArea = dataEntry.getWinMsgArea();
//    WINDOW* winfullScreen = dataEntry.getWinFullScreen();
//    std::string keyMessage, message;
//    int c, outRow, outCol;
//    bool result = false;
//    wattron(winMsgArea, COLOR_PAIR(3));
//    wmove(winMsgArea, startRow, startCol);
//    wclrtoeol(winMsgArea);
//    wmove(winMsgArea, startRow - 1, startCol);
//    wclrtoeol(winMsgArea);
//    //refresh();
//    switch (keyToPress) {
//    case KEY_F(3):
//        keyMessage = "Press F3 to ";
//        break;
//    case KEY_F(4):
//        keyMessage = "Press F4 to ";
//        break;
//    case KEY_F(5):
//        keyMessage = "Press F5 to ";
//        break;
//    case KEY_F(7):
//        keyMessage = "Press F7 to ";
//        break;
//    case KEY_NPAGE:
//        keyMessage = "Press Page Down to ";
//        break;
//    }
//    message = keyMessage;
//    message.append(prompt);
//    printInMiddle(winMsgArea, startRow, startCol, width, message, COLOR_PAIR(1), debugFile);
//    box(winMsgArea, 0, 0);
//    keypad(winMsgArea, TRUE);
//    wbkgd(winMsgArea, COLOR_PAIR(3));
//    wrefresh(winMsgArea);
//    //wrefresh(winFormArea);
//    wrefresh(winMsgArea);
//    c = wgetch(winMsgArea);
//    wmove(winMsgArea, startRow, startCol);
//    wclrtoeol(winMsgArea);
//    //refresh();
//    if (c == keyToPress) {
//        result = true;
//    }
//    else {
//        if (winMsgArea != stdscr) {
//            result = false;
//        }
//    }
//    return result;
//}
//std::string DataEntryUtils::doFunctionKeys(FieldProperties& field, std::string tbl, bool addingNew, ISAMWrapperLib& lib, std::string& condition, std::string& inquiryOrderBy, std::vector<FieldProperties>& fields, std::ofstream& debugFile, std::string inputAction) {
//    std::string result = "";
//    field.winMsgArea;
//    WINDOW* winMsgArea = field.winMsgArea;
//    WINDOW* winFullScreen = field.winFullScreen;
//    int index = 0;
//    //TODO: find way so these are not hard-coded
//    int lookupHeight = 10;
//    int lookupWidth = 50;
//    int lookupStartY = 5;
//    int lookupStartX = 5;
//    std::vector<DataTuple> tupleVectorData;
//    std::vector<std::pair<std::string, std::string>> pairVectorData;
//    std::vector<std::string> fieldNames;
//    std::vector<std::string> fieldValues;
//    std::string selectedElement = "";
//    int winRows = 0, winCols = 0;
//    getmaxyx(winFullScreen, winRows, winCols);
//    for (const auto& entry : fields) {
//        std::string fieldName = entry.fieldName;
//        std::replace(fieldName.begin(), fieldName.end(), ' ', '_');  // Replace spaces with underscores
//        fieldNames.push_back(fieldName);
//        std::string fieldValue = entry.fieldValue;
//        std::replace(fieldValue.begin(), fieldValue.end(), '"', ' ');  // Replace quotes with space
//        fieldValues.push_back(fieldValue);
//    }
//    int test = DataEntryUtils::stringSwitchNumber(inputAction);
//    switch (test) {
//    case 1: break; //KEY_F(1)
//    case 2: //KEY_F(2)
//        if (!fieldNames.empty()) {
//            tupleVectorData = lib.selectRows(tbl, "", inquiryOrderBy);
//            // Transform vector of tuples into vector of pairs
//            pairVectorData = DataEntryUtils::transformVector(tupleVectorData);
//            selectedElement = DataEntryUtils::displayLookupWindow(winFullScreen, lookupHeight, lookupWidth, lookupStartY, lookupStartX, pairVectorData, condition);
//            result = "TryReadRecord";
//            condition = fieldNames[0] + " = '" + selectedElement + "'";
//            return result;
//        }
//        break;
//    case 3: //KEY_F(3)
//
//        if (field.confirmAction(dataEntry, 2, 2, winCols, "Delete Record", COLOR_PAIR(1), KEY_F(3), debugFile)) {
//            lib.deleteRow(tbl, condition);
//            result = "RecordEntryStart";
//        }
//        break;
//    case 4: //KEY_F(4)
//        if (DataEntryUtils::confirmAction(dataEntry, 2, 2, winCols, "Save Record", COLOR_PAIR(1), KEY_F(4), debugFile)) {
//            if (addingNew) {
//                lib.insert(tbl, fieldNames, fieldValues);
//            }
//            else {
//                lib.updateRow(tbl, fieldNames, fieldValues, condition);
//            }
//            // Clear field values after update or insert
//            for (auto& field : fields) {
//                field.
//                    field.fieldValue = "";
//            }
//            result = "RecordEntryStart";
//        }
//        else {
//            wclear(winMsgArea);
//            wrefresh(winMsgArea);
//        }
//        break;
//    case 5: //KEY_F(5)
//        if (DataEntryUtils::confirmAction(dataEntry, 2, 2, winCols, "Restart", COLOR_PAIR(1), KEY_F(5), debugFile)) {
//            result = "RecordEntryStart";
//        }
//        else {
//            wclear(winMsgArea);
//            wrefresh(winMsgArea);
//        }
//        break;
//    case 6: break; //KEY_F(6)
//    case 7: //KEY_F(7)
//        if (DataEntryUtils::confirmAction(dataEntry, 2, 2, winCols, "Exit", COLOR_PAIR(1), KEY_F(7), debugFile)) {
//            result = "Exit";
//        }
//        else {
//            wclear(winMsgArea);
//            wrefresh(winMsgArea);
//        }
//        break;
//    default:
//        //TODO add other cases?
//        result = "";
//    }
//    return result;
//}
//std::string DataEntryUtils::readRecord(FieldProperties& field, std::string tbl, bool addingNew, bool prompt, ISAMWrapperLib& lib, const std::string condition, std::vector<FieldProperties>& fields) {
//    std::string result = "";
//    WINDOW* winMsgArea = field.winMsgArea;
//    WINDOW* winFullScreen = field.winFullScreen;
//    std::vector<std::string> fieldNames;
//    std::vector<std::string> fieldValues;
//    int index = 0;
//    for (const auto& entry : fields) {
//        std::string fieldName = entry.fieldName;
//        std::replace(fieldName.begin(), fieldName.end(), ' ', '_');  // Replace spaces with underscores
//        fieldNames.push_back(fieldName);
//        std::string fieldValue = entry.fieldValue;
//        std::replace(fieldValue.begin(), fieldValue.end(), '"', ' ');  // Replace quote with space
//        fieldValues.push_back(fieldValue);
//    }
//    bool exists = lib.getRecordByCondition(tbl, condition, fieldValues);
//    if (exists) {
//        index = 0;
//        for (auto& entry : fields) {
//            if (index < fieldValues.size()) {
//                entry.fieldValue = fieldValues[index];
//                index++;
//            }
//        }
//        result = "EditingRecord";
//    }
//    else {
//        if (prompt) {
//            std::string message = "Record not found. Do you want to add it?";
//            std::string answer = askQuestion(winMsgArea, 1, 2, 0, message, COLOR_PAIR(1));
//            if (answer == "Yes")
//                result = "AddNewRecord";
//            else
//                result = "RecordNotFound";
//        }
//    }
//    //wrefresh(winFullScreen);
//    wattron(winFullScreen, A_REVERSE);
//    wrefresh(winFullScreen);
//    wattroff(winFullScreen, A_REVERSE);
//    wrefresh(winFullScreen);
//    //box(winFullScreen, 0, 0);
//    wrefresh(winFullScreen);
//    return result;
//}
//int DataEntryUtils::stringSwitchNumber(const std::string& key) {
//    // Because the switch statement doesn't support alpha values,
//    // this function is helpful for mapping an alpha value to a number.
//    // Originally, I used this for function key values but
//    // found this technique will be useful for any alpha test.
//    static const std::unordered_map<std::string, int> keyMap = {
//        {"KEY_UP", -1},
//        {"KEY_ENTER", 0},
//        {"KEY_F(1)", 1},
//        {"KEY_F(2)", 2},
//        {"KEY_F(3)", 3},
//        {"KEY_F(4)", 4},
//        {"KEY_F(5)", 5},
//        {"KEY_F(7)", 7},
//        {"BeginField", 13},
//        {"AddNewRecord", 14},
//        {"RecordEntryStart", 15},
//        {"exitField_NOT_KEY_F", 16},
//        {"STRING", 17},
//        {"NUMERIC", 18},
//        {"MASKED", 19},
//        {"UPPERCASE", 20},
//        {"DIGITS", 21},
//        {"QUESTION", 22},
//        {"DATE", 23},
//        {"CHOICE", 24}
//    };
//    // Check if the key exists in the map
//    auto it = keyMap.find(key);
//    if (it != keyMap.end()) {
//        // Return the corresponding numeric value
//        return it->second;
//    }
//    else {
//        // Return a default value or handle the case when the key is not found
//        return -99; // Default value indicating key not found
//    }
//}
//
//bool DataEntryUtils::exportTableToCSV(sqlite3* db, const std::string& tableName, const std::string& fileName) {
//    std::ofstream outputFile(fileName);
//    if (!outputFile.is_open()) {
//        //std::cerr << "Error opening output file: " << fileName << std::endl;
//        return false;
//    }
//    // Prepare the SQL query to fetch data from the table
//    //std::stringstream query;
//    std::string query = "SELECT * FROM " + tableName + ";";
//    // Execute the query
//    sqlite3_stmt* stmt;
//    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
//    if (rc != SQLITE_OK) {
//        //std::cerr << "Error preparing SQL statement: " << sqlite3_errmsg(db) << std::endl;
//        return false;
//    }
//    // Write column names as the header row in the CSV file
//    int numColumns = sqlite3_column_count(stmt);
//    for (int i = 0; i < numColumns; ++i) {
//        if (i > 0) outputFile << ",";
//        outputFile << sqlite3_column_name(stmt, i);
//    }
//    outputFile << std::endl;
//    // Fetch and write data rows to the CSV file
//    while (sqlite3_step(stmt) == SQLITE_ROW) {
//        for (int i = 0; i < numColumns; ++i) {
//            if (i > 0) outputFile << ",";
//            const unsigned char* columnText = sqlite3_column_text(stmt, i);
//            if (columnText) {
//                outputFile << columnText;
//            }
//        }
//        outputFile << std::endl;
//    }
//    // Finalize the statement and close the output file
//    sqlite3_finalize(stmt);
//    outputFile.close();
//    return true;
//}
//// Function to transform vector of tuples into vector of pairs
//std::vector<std::pair<std::string, std::string>> DataEntryUtils::transformVector(const std::vector<DataTuple>& inputVector) {
//    std::vector<std::pair<std::string, std::string>> outputVector;
//    for (const auto& tuple : inputVector) {
//        std::string key = std::get<0>(tuple);
//        std::string value = std::get<1>(tuple);
//        outputVector.push_back(std::make_pair(key, value));
//    }
//    return outputVector;
//}
//std::string DataEntryUtils::displayLookupWindow(WINDOW* mainWindow, int lookupHeight, int lookupWidth, int lookupStartY, int lookupStartX, std::vector<std::pair<std::string, std::string>>& data, std::string condition) {
//    int maxLookupWidth = 0;
//    // get the value of the widest values about to be displayed
//    for (int i = 0; i < static_cast<int>(data.size()); ++i) {
//        std::string first = data[i].first;
//        std::string second = data[i].second;
//        removeTrailingSpaces(first);
//        removeTrailingSpaces(second);
//        int x = first.length() + second.length();
//        maxLookupWidth = std::max(maxLookupWidth, x);
//    }
//    maxLookupWidth += 10;
//    WINDOW* lookupWin = newwin(lookupHeight, maxLookupWidth, lookupStartY, lookupStartX);
//    keypad(lookupWin, TRUE);
//    box(lookupWin, 0, 0);
//    wrefresh(lookupWin);
//    wrefresh(mainWindow);
//    int currentRow = 1;
//    int selectedRow = 0;
//    int topRow = 0;
//    std::string firstValue = "";
//    std::string blankLine(maxLookupWidth - 5, ' ');
//    while (true) {
//        // Clear previous content
//        werase(lookupWin);
//        box(lookupWin, 0, 0);
//        // Display data
//        for (int i = topRow; i < std::min(topRow + lookupHeight - 2, static_cast<int>(data.size())); ++i) {
//            if (i >= data.size()) {
//                break; // Prevent out-of-bounds access
//            }
//            mvwprintw(lookupWin, i - topRow + 1, 2, "%s", blankLine.c_str());
//            if (i == selectedRow) {
//                wattron(lookupWin, A_REVERSE);
//            }
//            std::string first = data[i].first;
//            std::string second = data[i].second;
//            removeTrailingSpaces(first);
//            removeTrailingSpaces(second);
//            mvwprintw(lookupWin, i - topRow + 1, 2, "%s %s", first.c_str(), second.c_str());
//            wattroff(lookupWin, A_REVERSE);
//        }
//        wmove(lookupWin, currentRow - topRow + 1, 2); // Move cursor to currentRow
//        wrefresh(lookupWin);
//        int ch = wgetch(lookupWin);
//        switch (ch) {
//        case KEY_UP:
//            if (selectedRow > 0) {
//                selectedRow--;
//                if (currentRow > 1) {
//                    currentRow--;
//                }
//                else if (topRow > 0) {
//                    topRow--;
//                }
//            }
//            break;
//        case KEY_DOWN:
//            if (selectedRow < static_cast<int>(data.size()) - 1) {
//                selectedRow++;
//                if (currentRow < lookupHeight - 2 && currentRow < static_cast<int>(data.size()) - topRow) {
//                    currentRow++;
//                }
//                else if (topRow < static_cast<int>(data.size()) - lookupHeight + 2) {
//                    topRow++;
//                }
//            }
//            break;
//        case '\n': // Enter key pressed
//            delwin(lookupWin);
//            touchwin(mainWindow);
//            wrefresh(mainWindow);
//            if (selectedRow < data.size()) {
//                firstValue = data[selectedRow].first;
//                return firstValue;
//            }
//            break;
//        case 27: // Escape key pressed
//            delwin(lookupWin);
//            touchwin(mainWindow);
//            wrefresh(mainWindow);
//            return "";
//        }
//    }
//    return 0;
//}
//void DataEntryUtils::displayRightToLeft(WINDOW* win, const std::string& input, int row, int col, int inputSize) {
//    int startPos = 0;
//    // Clear the area where the input will be displayed in reverse video
//    wattron(win, A_REVERSE);
//    startPos = col + inputSize - 1;
//    for (int i = 0; i < inputSize; ++i) {
//        mvwaddch(win, row, startPos - i, ' ');
//    }
//    wrefresh(win);
//    std::string inputResult = input;
//    std::reverse(inputResult.begin(), inputResult.end());
//    //inputResult = input;
//    // Display the input from right to left
//    //startPos = startPos + -1; // Calculate the starting position for display
//    //wattron(win, A_REVERSE);
//    for (int i = 0; i < input.size(); ++i) {
//        mvwaddch(win, row, startPos - i, inputResult[i]);
//        wrefresh(win);
//    }
//    wattroff(win, A_REVERSE);
//    wrefresh(win);
//    //Move the cursor to the end of the input string
//    wmove(win, row, col + inputSize); //startPos
//    //int x = input.find_last_not_of(' ') + 1;
//    //wmove(win, row, col + x);
//    //wrefresh(win);
//}
//
//void DataEntryUtils::hideWindow(FieldProperties& field) {
//    WINDOW* winMsgArea = dataEntry.getWinMsgArea();
//    WINDOW* winfullScreen = dataEntry.getWinFullScreen();
//    if (winMsgArea != nullptr) {
//        werase(winMsgArea);  // Clear the window
//        wrefresh(winMsgArea); // Refresh the window to update the display
//        if (winfullScreen != nullptr) {
//            touchwin(winfullScreen);
//            wrefresh(winfullScreen);
//        }
//    }
//}
//void DataEntryUtils::displayData(std::ofstream& debugFile) {
//    std::string unWanted = "\"";
//
//    std::string myString = dataEntryInstance->RemoveCharacters(fieldValue, unWanted);
//    fieldValue = myString;
//    wattron(winFullScreen, COLOR_PAIR(3));
//
//    int test = this->stringSwitchNumber(fieldType);
//    if (test == 18) { // numeric field
//        std::string myStringPaddedRight = this=>rightJustifyString(myString, len);
//        mvwprintw(winFullScreen, row, fieldColumn, "%s", myStringPaddedRight.c_str());
//    }
//    else {
//        std::string myStringPaddedLeft = myString;
//        myStringPaddedLeft.resize(len, ' ');
//        mvwprintw(winFullScreen, row, fieldColumn, "%s", myStringPaddedLeft.c_str());
//    }
//    wrefresh(winFullScreen);
//}
//void DataEntryUtils::displayLabels(std::ofstream& debugFile) {
//    //wattron(winFullScreen_, COLOR_PAIR(3));
//    wattron(winFullScreen, COLOR_PAIR(3));
//    mvwprintw(winFullScreen, row, labelColumn, "%s", labelText.c_str());
//    wrefresh(winFullScreen);
//}
//KeyType checkSpecialKeys(int ch) {
//    switch (ch) {
//    case KEY_F(1):
//    case KEY_F(2):
//    case KEY_F(3):
//    case KEY_F(4):
//    case KEY_F(5):
//    case KEY_F(6):
//    case KEY_F(7):
//    case KEY_F(8):
//    case KEY_F(9):
//    case KEY_F(10):
//    case KEY_F(11):
//    case KEY_F(12):
//        return FUNCTION_KEY;
//    case KEY_UP:
//        return UP_ARROW;
//    case 10: // Enter key
//    case KEY_ENTER:
//        //case PADENTER:
//        return ENTER_KEY;
//    default:
//        return OTHER_KEY;
//    }
//}
//void DataEntryUtils::displayMask(std::string& input, std::ofstream& debugFile) {
//    //std::string unWanted = '\"';
//    std::string unWanted = "\"";
//    WINDOW* saveWinFullScreen = winFullScreen;
//    int saveRow = row;
//    int saveColumn = fieldColumn;
//    input = RemoveCharacters(input, unWanted);
//    std::string mask = fieldMask;
//    mask = RemoveCharacters(mask, unWanted);
//    std::string displayMaskStr = generateDisplayMask(mask);
//    mvwprintw(saveWinFullScreen, saveRow, saveColumn, "%s", displayMaskStr.c_str()); // Print the display mask with spaces instead of '9'
//    for (int i = 0, j = 0; i < mask.size(); ++i) {
//        wrefresh(saveWinFullScreen);
//        if (mask[i] == '9' && j < input.size()) {
//            mvwaddch(saveWinFullScreen, saveRow, saveColumn + i, input[j++]); // Display input character
//        }
//        else if (mask[i] != '9') {
//            mvwaddch(saveWinFullScreen, saveRow, saveColumn + i, mask[i]); // Display mask character
//        }
//    }
//    int pos = input.find_first_of(" ");
//    int adj = 0;
//    if (pos <= 0) {
//        pos = input.length();
//        if (mask[pos] != '9' && pos > 0)
//            pos++;
//        else
//            adj++;
//        wmove(saveWinFullScreen, saveRow, saveColumn + pos + adj);
//    }
//    else {
//        wmove(saveWinFullScreen, saveRow, saveColumn + pos);
//    }
//    if (input.empty()) {
//        // Set cursor to the first '9' position in the mask
//        int pos = 0;
//        while (pos < mask.size() && mask[pos] != '9') {
//            ++pos;
//        }
//        wmove(saveWinFullScreen, saveRow, saveColumn + pos);
//    }
//    wrefresh(saveWinFullScreen);
//    //****************************************    
//    // if (pos < mask.size())                
//    //     wmove(saveWinFullScreen, saveRow, saveColumn + pos); // Move cursor to the next input position                
//    // else if (pos = mask.size())
//    //     wmove(saveWinFullScreen, saveRow, saveColumn + pos-1); // Move cursor to the next input position
//    // wrefresh(saveWinFullScreen); // Refresh window to show changes
//}
//// Inspect the mask to determine the sizes on either side of a decimal point
//void DataEntryUtils::inspectMask(const std::string& format, int& leftSize, int& rightSize) {
//    size_t decimalPos = format.find('.');
//    if (decimalPos != std::string::npos) {
//        leftSize = decimalPos;
//        rightSize = format.size() - decimalPos - 1;
//    }
//    else {
//        // If no decimal point is found, all characters are considered to the left of the decimal
//        leftSize = format.size();
//        rightSize = 0;
//    }
//}
//// Function to combine the input with the mask
//std::string DataEntryUtils::combineInputWithMask(const std::string& mask, const std::string& input) {
//    std::string combined = mask;
//    int inputIndex = 0;
//    for (char& ch : combined) {
//        if (ch == '9' && inputIndex < input.size()) {
//            ch = input[inputIndex++]; // Replace '9' with input character
//        }
//    }
//    return combined;
//}
//// Function to generate the initial display mask string
//std::string generateDisplayMask(const std::string& mask) {
//    std::string displayMaskStr = mask;
//    for (char& ch : displayMaskStr) {
//        if (ch == '9') {
//            ch = ' '; // Replace '9' with space
//        }
//    }
//    return displayMaskStr;
//}
//// Function to remove masked characters
//std::string removeMask(const std::string& maskedInput) {
//    std::string result;
//    std::copy_if(maskedInput.begin(), maskedInput.end(), std::back_inserter(result), [](char ch) {
//        return std::isdigit(ch);
//        });
//    return result;
//}
//std::string RemoveCharacters(const std::string& str, const std::string& unWanted) {
//    std::string result = DataEntryUtils::removeLeadingSpaces(str);
//    for (char unwantedChar : unWanted) {
//        result.erase(std::remove(result.begin(), result.end(), unwantedChar), result.end());
//    }
//    return result;
//}
//std::string rightJustifyString(const std::string& str, int len) {
//    if (str.length() >= len) {
//        return str; // If the string is already longer than or equal to len, return it as is
//    }
//    return std::string(len - str.length(), ' ') + str; // Pad with spaces on the left
//}
//std::string removeLeadingSpaces(const std::string& str) {
//    // Find the position of the first non-space character
//    size_t start = str.find_first_not_of(" \t");
//    // If all characters are spaces, return an empty string
//    if (start == std::string::npos) {
//        return "";
//    }
//    // Extract the substring starting from the first non-space character
//    return str.substr(start);
//}
//void removeTrailingSpaces(std::string& str) {
//    // Find the position of the last non-space character
//    size_t pos = str.find_last_not_of(' ');
//    // Erase all characters from pos+1 to the end of the string
//    if (pos != std::string::npos) {
//        str.erase(pos + 1);
//    }
//    else {
//        // If the entire string is spaces, clear it
//        str.clear();
//    }
//}