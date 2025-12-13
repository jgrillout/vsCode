// Version-02.20.25.16.07
// File: TransEntryControl.cpp
#include "TransEntryControl.h"
#include "TransactionEntryLines.h"
#include <set>
// set by Parm (no hardcoded "TRXTOT")
static bool setByParm(PassParams& pp, const std::string& parm, const std::string& val) {
    for (auto& kv : pp.fields) if (kv.second.parm == parm) { kv.second.value = val; return true; }
    return false;
}

static double toDouble(std::string s) {
    s.erase(std::remove(s.begin(), s.end(), ','), s.end());
    s = DataEntry::cleanString(s);
    try { return std::stod(s); }
    catch (...) { return 0.0; }
}
// Constructor WINDOW* winFormScreen = SharedWindowManager::getwinFormScreen(screenProps);
TransEntryControl::TransEntryControl(WINDOW& winFormScreen, ISAMWrapperLib& db, const std::string& tableName, int maxRows, const std::string& scrollKey)
    :_win(nullptr), _db(db), _tableName(tableName), _scrollMaxRows(maxRows), _scrollKey(scrollKey), _currentTopIndex(0), _selectedIndex(0) {

}

// Destructor
TransEntryControl::~TransEntryControl() {
    // Destroy shared windows
    SharedWindowManager::destroywinScroll();

}
std::string  TransEntryControl::run(std::map<std::string, std::string>& headerRecord, std::vector<ScreenProperties>& screenProps, PassParams& transControlPassParams, std::string& nextRunClass, std::ifstream& xmlFile, std::string& xmlFileName, std::vector<controlFieldProperties>& controlFields, std::ofstream& debugFile) {

    debugFile << "entering TransEntryControl::run  on line 73 before DataEntry::setupControlFields " << "xmlFileName " << xmlFileName << std::endl;
    headerRecord_ = headerRecord;
    debugFile << "[DEBUG] Line 75 headerRecord_ in TransEntryControl::run:\n";
    for (const auto& kv : headerRecord_) {
        debugFile << "  " << kv.first << " = " << kv.second << "\n";
    }
    std::string runClass = nextRunClass;
    std::string result = "";
    std::string scrollKey = "";
    char* errMsg = nullptr;
    std::vector<LabelProperties> controlLabels;
    // boxes is now a member variable boxes_
    // std::vector<BoxProperties> boxes; // REMOVED - now using member variable
    //TODO can allFields be replaced by a struct that merged the screen & data dictionary properties?
    std::vector<DataEntryFieldProperties> allFields;
    runClass_ = runClass;
    std::ifstream tmpxmlFile;

    std::string slash = "";
#ifdef _WIN32
    slash = "\\";
#else
    slash = "/";
#endif
    if (xmlFile.is_open()) {
        xmlFile.close();
    }


    namespace fs = std::filesystem;
    fs::path currentPath = fs::current_path();
    std::string tmpxmlFileName = currentPath.string();


    tmpxmlFileName.append(slash + screenProps[0].controlClass_ + ".xml"); //"ARTransControlScreen.xml");

    tmpxmlFile.open(tmpxmlFileName);

    if (!tmpxmlFile.is_open()) {
        debugFile << "Failed to open " + tmpxmlFileName + " file." << std::endl;
        result = "Failed to open " + tmpxmlFileName + " file.";
        return result;
    }


    debugFile << "inside TransEntryControl::run  on line 99 before DataEntry::setupScreen " << "tmpxmlFileName " << tmpxmlFileName << std::endl;
    DataEntry::setupScreen(screenProps, tmpxmlFile, tmpxmlFileName, debugFile);
    //the first element is not needed because it is emplaced with nulls
    if (!screenProps.empty()) {
        screenProps.erase(screenProps.begin());
    }

    int stdscrRows = screenProps[0].screenHeight_;
    int stdscrCols = screenProps[0].screenWidth_;
    WINDOW* winFormScreen = SharedWindowManager::getwinFormScreen(screenProps);
    //wrefresh(winFormScreen);

    WINDOW* winMsgArea = SharedWindowManager::getwinMsgArea();

    int winWidth = screenProps[0].scrollWidth_;
    int winHeight = screenProps[0].scrollHeight_;
    int starty = screenProps[0].scrollStartY_;
    int startx = screenProps[0].scrollStartX_;
    std::string tableName = screenProps[0].scrollTable_;

    WINDOW* winScroll = SharedWindowManager::getwinScroll(winHeight, winWidth, starty, startx);


    //wclear(winFormScreen);
    wrefresh(winFormScreen);
    if (boxes_.size() > 0) {
        boxes_.clear();
    }
    // this line creates the boxes structure we have no real values to use yet
    //this fake elements will be removed and the real values will be added down futher in this function 

    boxes_.emplace_back(0, 0, 0, 0);
    tmpxmlFile.clear(); // Clear any error flags
    tmpxmlFile.seekg(0, std::ios::beg); // Go back to the beginning of the file
    debugFile << "inside TransEntryControl::run  on line 138 before DataEntry::setupBoxes " << "xmlFileName " << xmlFileName << std::endl;
    DataEntry::setupBoxes(boxes_, tmpxmlFile, tmpxmlFileName, debugFile);
    // the first element is not needed because it is emplaced with nulls
    if (!boxes_.empty()) {
        boxes_.erase(boxes_.begin());
    }
    DataEntry::setupLabels(controlLabels, tmpxmlFile, tmpxmlFileName, debugFile);




    //wrefresh(winScroll);
    wrefresh(winFormScreen);
    DataEntry::drawBoxes(boxes_);
    wrefresh(winFormScreen);
    tmpxmlFile.clear(); // Clear any error flags
    tmpxmlFile.seekg(0, std::ios::beg); // Go back to the beginning of the file
    std::string entryLinesxmlFileName = "";

    //if (!screenProps[0].scrollTable_.empty()) {
    entryLinesxmlFileName = currentPath.string();
    entryLinesxmlFileName.append(slash);
    entryLinesxmlFileName.append(screenProps[0].scrollTable_);
    entryLinesxmlFileName.append("Screen.xml");


    std::ifstream entryLinesxmlFile(entryLinesxmlFileName);

    entryLinesxmlFile.open(entryLinesxmlFileName);


    if (!entryLinesxmlFile.is_open()) {
        debugFile << "Failed to open " + entryLinesxmlFileName + " file." << std::endl;
        std::string result = "Failed to open " + entryLinesxmlFileName + " file.";
        return result;
    }
    std::vector<ScreenProperties> entryLineScreenProps;
    debugFile << "inside TransEntryControl::run  on line 152 before  DataEntry::setupScreen " << "entryLinesxmlFileName " << entryLinesxmlFileName << std::endl;
    DataEntry::setupScreen(entryLineScreenProps, entryLinesxmlFile, entryLinesxmlFileName, debugFile);


    // the first element is not needed because it is emplaced with nulls
    if (!entryLineScreenProps.empty()) {
        entryLineScreenProps.erase(entryLineScreenProps.begin());
    }
    entryLinesxmlFile.clear(); // Clear any error flags
    entryLinesxmlFile.seekg(0, std::ios::beg); // Go back to the beginning of the file
    std::vector<DataEntryFieldProperties>entryLinesfields;
    debugFile << "inside TransEntryControl::run  on line 187 before  DataEntry::setupFields " << "entryLinesxmlFileName " << entryLinesxmlFileName << std::endl;
    DataEntry::setupFields(entryLinesfields, entryLinesxmlFile, entryLinesxmlFileName, tmpxmlFile, tmpxmlFileName, debugFile);


    debugFile << "inside TransEntryControl::run  on line 162 before TransEntryControl::setupControlFields " << "xmlFileName " << xmlFileName << std::endl;
    TransEntryControl::setupControlFields(controlFields, tmpxmlFile, tmpxmlFileName, debugFile);


    if (!selectFields_.empty() && selectFields_.back() == ',') {
        selectFields_.pop_back();
    }
    for (auto& field : controlFields) {
        // Assign scrollKey value from parameter if provided
        if (transControlPassParams.fields.find(screenProps[0].scrollKey_) != transControlPassParams.fields.end()) {

            scrollKey = transControlPassParams.fields.at(screenProps[0].scrollKey_).value;
        }
    }
    CommandLineProperties commandLine;
    commandLine = parseCommandLineXML(entryLinesxmlFile, debugFile);

    //debugFile << "BEFORE CALL TO loadRecords";
    loadRecords(transControlPassParams, screenProps, controlFields, debugFile);
    displayControlLabels(winFormScreen, headerRecord, controlLabels, transControlPassParams, screenProps, controlFields, commandLine, debugFile); // or winScroll?
    displayRecords(screenProps, controlFields, controlLabels, winFormScreen, winScroll, commandLine, debugFile);

    handleInput(transControlPassParams, screenProps, controlLabels, controlFields, winFormScreen, winScroll, commandLine, entryLinesfields, debugFile);

}

void TransEntryControl::loadRecords(
    PassParams& transControlPassParams,
    std::vector<ScreenProperties>& screenProps,
    std::vector<controlFieldProperties>& controlFields,
    std::ofstream& debugFile)
{
    if (screenProps.empty()) {
        // Optional: handle error/log; keeping silent here to match original signature.
        return;
    }

    // Build WHERE and ORDER BY
    const std::string condition = screenProps[0].scrollKey_ + "='" + _scrollKey + "'";
    // TODO: find way to not hard code
    const std::string orderBy = "TRANS,ENT";



    // Fetch: pair< vector<map<string,string>>, vector<string> >
    records_ = _db.selectDynamicRecords(_tableName, condition, selectFields_, orderBy);


    // Reset dest buffers

    recordFieldValues_.clear();
    trxTotal_ = 0.0;
    fieldNames_ = records_.second; // column names returned by select

    // Build display rows
    for (const auto& record : records_.first) {
        std::vector<std::string> rowValues;
        rowValues.reserve(controlFields.size());

        for (const auto& field : controlFields) {
            if (field.hidden == "Y") {
                continue; // skip hidden fields entirely
            }

            std::string value;
            const std::string& fieldName = field.fieldName;
            //if (fieldName == "EXTPRC") {
            //    //std::cout << "debug - fieldName equals " << fieldName;
            //}

            if (!field.calc.empty()) {
                // Compute on the fly from the current record
                value = DataEntry::evaluateCalcFormula(field.calc, record);
                if (!field.fieldMask.empty()) {
                    value = applyMask(value, field.fieldMask);
                }
            }
            else {
                // Pull raw value from the fetched record
                auto it = record.find(field.fieldName);
                value = (it != record.end()) ? it->second : "";

            }


            rowValues.push_back(std::move(value));


        }

        recordFieldValues_.push_back(std::move(rowValues));

    }
    //// TODO: logic that runs after all records are loaded & interated
    //std::string sql = "";// TODO get sql from XML
    //const std::string value = _db.execScalar(sql, condition);
    //trxTotal_ += toDouble(value);
    //// TODO: fix hard code
    //setByParm(transControlPassParams, "TRXTOT", std::to_string(trxTotal_));
}

void TransEntryControl::displayControlLabels(WINDOW* win, std::map<std::string, std::string> record, const std::vector<LabelProperties>& controlLabels, PassParams& transControlPassParams, std::vector<ScreenProperties>& screenProps, std::vector<controlFieldProperties>& controlFields, CommandLineProperties& commandLine, std::ofstream& debugFile) {
    std::string value;
    for (auto& label : controlLabels) {
        if (label.labelFieldSQL.empty()) {
            DataEntry::displayLabels(win, record, label, transControlPassParams, debugFile);
        }
        else {

            std::string sql = label.labelFieldSQL;
            std::string condition = label.labelFieldWHERE + " = ";
            auto it = record.find(label.labelFieldName);
            value = (it != record.end()) ? it->second : "";
            const std::string value = _db.execScalar(sql, condition);
            condition.append(value);
            wattroff(win, A_REVERSE);
            wrefresh(win);
            wattrset(win, A_NORMAL);// ensure background has no A_REVERSE
            wbkgd(win, COLOR_PAIR(3));
            mvwprintw(win, label.labelFieldRow, label.labelFieldCol, "%s", value.c_str());
            wrefresh(win);
        }
    }

}
void TransEntryControl::displayRecords(std::vector<ScreenProperties>& screenProps,
    std::vector<controlFieldProperties>& controlFields,
    std::vector<LabelProperties>& controlLabels,
    WINDOW* winFormScreen,
    WINDOW* windScroll,
    CommandLineProperties& commandLine,
    std::ofstream& debugFile)
{
    windScroll = SharedWindowManager::getwinScroll(screenProps[0].screenHeight_, screenProps[0].screenWidth_, screenProps[0].scrollStartY_, screenProps[0].scrollStartX_);
    int numRecords = static_cast<int>(recordFieldValues_.size());
    if (numRecords <= 0) {
        // Still update the row-count label to clear it or show blanks if desired
        updateRowCountLabel(controlLabels, winFormScreen);
        return;
    }

    // Use the member _scrollMaxRows so paging and display agree
    int maxVisibleRecords = _scrollMaxRows / 2;  // each record uses 2 rows
    if (maxVisibleRecords <= 0) {
        updateRowCountLabel(controlLabels, winFormScreen);
        return;
    }

    int displayLimit = std::min(maxVisibleRecords,
        static_cast<int>(numRecords - _currentTopIndex));

    for (int i = 0; i < displayLimit; ++i) {
        int recordIndex = static_cast<int>(_currentTopIndex) + i;
        const std::vector<std::string>& rowValues = recordFieldValues_[recordIndex];
        bool isSelected = (recordIndex == static_cast<int>(_selectedIndex));
        int visibleIndex = 0;

        for (controlFieldProperties& field : controlFields) {
            if (field.hidden == "Y") {
                continue;
            }

            int adjustedRow = field.row + (i * 2);  // two rows per record
            int col = field.col;

            std::string value;
            if (visibleIndex < static_cast<int>(rowValues.size())) {
                value = rowValues[visibleIndex];
            }
            else {
                value = "";
            }
            ++visibleIndex;

            int width = field.len;
            if (field.fieldType != "NUMERIC") {
                if ((int)value.length() < width) {
                    value.append(width - value.length(), ' ');
                }
                else {
                    value = value.substr(0, width);
                }
            }
            else {
                if ((int)value.length() < width) {
                    value = std::string(width - value.length(), ' ') + value;
                }
                else {
                    value = value.substr(0, width);
                }
            }

            if (field.hidden == "N") {
                if (isSelected) {
                    wattron(windScroll, A_REVERSE);
                }

                mvwprintw(windScroll, adjustedRow, col, "%s", value.c_str());

                if (isSelected) {
                    wattroff(windScroll, A_REVERSE);
                }
                wrefresh(windScroll);
            }
        }

        wrefresh(winFormScreen);
    }

    // Clear any leftover rows so stale data disappears on the last page
    for (int rowIndex = displayLimit; rowIndex < maxVisibleRecords; ++rowIndex) {
        for (controlFieldProperties& field : controlFields) {
            if (field.hidden == "Y") {
                continue;
            }

            int adjustedRow = field.row + (rowIndex * 2);
            int col = field.col;
            int width = field.len;

            std::string blanks(width, ' ');
            mvwprintw(windScroll, adjustedRow, col, "%s", blanks.c_str());
        }
    }

    wrefresh(windScroll);

    // NEW: update the "Entry (xxx) of (yyy)" label using the labels vector
    updateRowCountLabel(controlLabels, winFormScreen);
}

void TransEntryControl::updateRowCountLabel(std::vector<LabelProperties>& controlLabels,
    WINDOW* winFormScreen)
{
    int totalRecords = static_cast<int>(recordFieldValues_.size());

    // Find the label whose labelText contains the placeholder "Entry (xxxx) of (yyyy)"
    LabelProperties* targetLabel = nullptr;
    for (auto& lbl : controlLabels) {
        if (lbl.labelText.find("Entry (xxxx) of (yyyy)") != std::string::npos) {
            targetLabel = &lbl;
            break;
        }
    }


    if (!targetLabel) {
        // No matching label defined; nothing to update
        return;
    }

    int row = targetLabel->row;
    int col = targetLabel->col;
    int width = static_cast<int>(targetLabel->labelText.size());


    if (totalRecords <= 0) {
        // Clear the label area if there are no records
        std::string blanks(width, ' ');
        mvwprintw(winFormScreen, row, col, "%s", blanks.c_str());
        wrefresh(winFormScreen);
        return;
    }

    // xxx = CURRENT SELECTED record index (1-based)
    int currentIndex = static_cast<int>(_selectedIndex) + 1;






    // Format as 3-digit zero-padded numbers: 001, 020, etc.
    std::ostringstream ossCur, ossTotal, oss;
    ossCur << std::setw(3) << std::setfill('0') << currentIndex;
    ossTotal << std::setw(3) << std::setfill('0') << totalRecords;

    oss << "Entry (" << ossCur.str() << ") of (" << ossTotal.str() << ")";
    std::string labelText = oss.str();



    // Fit/pad to the label's original width
    std::string toPrint = labelText;
    if ((int)toPrint.size() < width) {
        toPrint.append(width - toPrint.size(), ' ');
    }
    else if ((int)toPrint.size() > width) {
        toPrint = toPrint.substr(0, width);
    }

    mvwprintw(winFormScreen, row, col, "%s", toPrint.c_str());
    wrefresh(winFormScreen);


}












void TransEntryControl::handleInput(PassParams& transControlPassParams, std::vector<ScreenProperties>& screenProps, std::vector<LabelProperties>& labels, std::vector<controlFieldProperties>& controlFields, WINDOW* winFormScreen, WINDOW* winScroll, CommandLineProperties& commandLine, std::vector<DataEntryFieldProperties>& entryLinesfields, std::ofstream& debugFile) {

    int ch;
    RunResult runresult;
    while ((ch = wgetch(winScroll)) != 'q') {
        switch (ch) {
        case KEY_UP:
            moveSelectionUpOne();
            break;
        case KEY_DOWN:
            moveSelectionDownOne();
            break;
        case KEY_PPAGE:
            pageUp();
            break;
        case KEY_NPAGE:
            pageDown();
            break;
        case KEY_HOME:
            moveToTop();
            break;
        case KEY_END:
            moveToBottom();
            break;
        case '\n':
            runresult = handleCommandKey(ch, transControlPassParams, screenProps, labels, controlFields, winScroll, commandLine, entryLinesfields, debugFile);
            break;
        default:
            runresult = handleCommandKey(ch, transControlPassParams, screenProps, labels, controlFields, winScroll, commandLine, entryLinesfields, debugFile);
            break;
        }


        // redraw the control screen content.
        if (runresult.status == "Exit") {
            // Clear both form and scroll windows, then recreate and redraw boxes
            SharedWindowManager::hidewinFormScreen();
            SharedWindowManager::hidewinScroll();
            // Recreate the windows
            winFormScreen = SharedWindowManager::getwinFormScreen(screenProps);
            winScroll = SharedWindowManager::getwinScroll(
                screenProps[0].scrollHeight_, screenProps[0].scrollWidth_,
                screenProps[0].scrollStartY_, screenProps[0].scrollStartX_);
            // Clear any leftover attributes from winScroll
            wattrset(winFormScreen, A_NORMAL);
            wattrset(winScroll, A_NORMAL);
            // Redraw the winFormScreen border (lost when window was cleared)
            box(winFormScreen, ACS_VLINE, ACS_HLINE);
            
            // Redraw the box borders
            DataEntry::drawBoxes(boxes_);

            displayControlLabels(winFormScreen, headerRecord_, labels,
                transControlPassParams, screenProps, controlFields,
                commandLine, debugFile);

            displayRecords(screenProps, controlFields, labels,
                winFormScreen, winScroll,
                commandLine, debugFile);

            continue;
        }
        displayRecords(screenProps, controlFields, labels, winFormScreen, winScroll, commandLine, debugFile);
    }
}


void TransEntryControl::moveSelectionDownOne() {
    int numRecords = static_cast<int>(recordFieldValues_.size());
    if (numRecords <= 0) {
        return;
    }

    int maxVisibleRecords = _scrollMaxRows / 2;
    if (maxVisibleRecords <= 0) {
        return;



    }

    if (_selectedIndex < static_cast<size_t>(numRecords - 1)) {
        _selectedIndex++;
    }

    int bottomIndex = static_cast<int>(_currentTopIndex) + maxVisibleRecords - 1;
    if (static_cast<int>(_selectedIndex) > bottomIndex) {
        if (_currentTopIndex + 1 < static_cast<size_t>(numRecords)) {
            _currentTopIndex++;
        }
    }
}
void TransEntryControl::moveSelectionUpOne() {
    int numRecords = static_cast<int>(recordFieldValues_.size());
    if (numRecords <= 0) {
        return;
    }


    int maxVisibleRecords = _scrollMaxRows / 2;
    if (maxVisibleRecords <= 0) {
        return;
    }

    if (_selectedIndex > 0) {
        _selectedIndex--;

    }

    if (_selectedIndex < _currentTopIndex) {
        if (_currentTopIndex > 0) {
            _currentTopIndex--;
        }
    }
}

void TransEntryControl::pageUp() {
    int pageSize = _scrollMaxRows / 2;
    int totalRecords = static_cast<int>(recordFieldValues_.size());

    if (totalRecords <= 0 || pageSize <= 0) {
        return;
    }

    // Move the top index up by one page, but not before the first record
    int newTopIndex = static_cast<int>(_currentTopIndex) - pageSize;
    if (newTopIndex < 0) {
        newTopIndex = 0;
    }

    _currentTopIndex = static_cast<size_t>(newTopIndex);

    // On page up, select the first record on the new page
    _selectedIndex = _currentTopIndex;

    //logDebug("pageUp", _selectedIndex, _currentTopIndex, recordFieldValues_.size(), _scrollMaxRows);
}
void TransEntryControl::pageDown() {
    int pageSize = _scrollMaxRows / 2;  // each record uses 2 rows
    int totalRecords = static_cast<int>(recordFieldValues_.size());

    if (totalRecords <= 0 || pageSize <= 0) {
        return;
    }

    // Maximum top index so we can still show up to pageSize records
    int maxTopIndex = totalRecords - pageSize;
    if (maxTopIndex < 0) {
        maxTopIndex = 0;
    }

    // Move the top index down by one page, but not past the last page
    int newTopIndex = static_cast<int>(_currentTopIndex) + pageSize;
    if (newTopIndex > maxTopIndex) {
        newTopIndex = maxTopIndex;
    }

    _currentTopIndex = static_cast<size_t>(newTopIndex);

    // DESIGN CHANGE: on page down, select the FIRST visible record on the new page
    _selectedIndex = _currentTopIndex;

    //logDebug("pageDown", _selectedIndex, _currentTopIndex, recordFieldValues_.size(), _scrollMaxRows);
}

void TransEntryControl::moveToTop() {
    _selectedIndex = 0;
    _currentTopIndex = 0;
    //logDebug("moveToTop", _selectedIndex, _currentTopIndex, records_.first.size(), _scrollMaxRows);
}

void TransEntryControl::moveToBottom() {
    _selectedIndex = records_.first.size() - 1;
    _currentTopIndex = std::max<size_t>(0, _selectedIndex - (_scrollMaxRows / 2) + 1);
}



RunResult  TransEntryControl::handleCommandKey(
    int key,
    PassParams& transControlPassParams,
    std::vector<ScreenProperties>& screenProps, const std::vector<LabelProperties>& labels,
    std::vector<controlFieldProperties>& controlFields, WINDOW* winScroll,
    CommandLineProperties& commandLine,
    std::vector<DataEntryFieldProperties>& entryLinesfields,
    std::ofstream& debugFile)
{
    RunResult runresult;
    TransactionEntryLines lineEditor(entryLinesfields, screenProps, debugFile, _db.getDb(), headerRecord_);
    std::map<std::string, std::string> selectedRecord;
    if (key == 'A' || key == 'a') {
        // Append: start a new line record, prefilled from header where names match
        std::map<std::string, std::string> newLineRec;
        copyHeaderFieldsToRecord(entryLinesfields, headerRecord_, newLineRec);
        runresult = lineEditor.addLineItem(newLineRec);
    }
    else if (key == '\n') {
        // Edit: ensure any matching header fields are present, but don't overwrite existing line values
        if (_selectedIndex < records_.first.size()) {
            //auto selectedRecord = records_.first[_selectedIndex];
            selectedRecord = records_.first[_selectedIndex];
            copyHeaderFieldsToRecord(entryLinesfields, headerRecord_, selectedRecord);
            runresult = lineEditor.editLineItem(selectedRecord);
        }
    }

    // If the line screen exited via F7, just bubble the status up.
    if (runresult.status == "Exit") {
        return runresult;
    }



    loadRecords(transControlPassParams, screenProps, controlFields, debugFile);
    WINDOW* winFormScreen = SharedWindowManager::getwinFormScreen(screenProps);
    displayControlLabels(winFormScreen, headerRecord_, labels,
        transControlPassParams, screenProps, controlFields,
        commandLine, debugFile);



    return runresult;
}



int TransEntryControl::setupBoxes(std::vector<BoxProperties>& boxes, std::ifstream& xmlFile, std::string& xmlFileName, std::ofstream& debugFile) {

    bool inBoxes = false;

    // Define variables to store the properties of each box
    int col = 0;
    int row = 0;
    int cols = 0;
    int rows = 0;

    // Ensure the XML file is open
    if (!xmlFile.is_open()) {
        debugFile << "Error inside  TransEntryControl::setupBoxes line 437 xmlfile is not open." << std::endl;
        return -1;
    }

    std::string line;
    while (std::getline(xmlFile, line)) {
        //debugFile << line << std::endl;
        if (line.find("<Boxes>") != std::string::npos) {
            inBoxes = true;
            continue;
        }

        if (inBoxes == true && line.find("</Box>") != std::string::npos) {
            // At the end of a <Box> tag, push the collected properties into the vector
            boxes_.emplace_back(col, row, cols, rows);

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
            if (tag == "Col") {
                col = std::stoi(value);
            }
            else if (tag == "Row") {
                row = std::stoi(value);
            }
            else if (tag == "Cols") {
                cols = std::stoi(value);
            }
            else if (tag == "Rows") {
                rows = std::stoi(value);
            }
        }
    }

    return true;
}


int TransEntryControl::setupControlFields(std::vector<controlFieldProperties>& controlFields, std::ifstream& xmlFile, std::string& xmlFileName, std::ofstream& debugFile) {
    std::string fieldName = "";
    std::string fieldType = "";
    // fieldValue is a reserved spot to store values from a db record or calculated field NOT from the xml file like all the others
    std::string fieldValue = "";
    std::string calc = "";
    std::string fieldMask = "";
    int len = 0;
    int row = 0;
    int col = 0;
    //int labelColumn = 0;
    //std::string labelText = "";
    std::string hidden = "";
    bool isDisplayField = false;


    // bool askTrxType = true;
    // std::vector<std::string> fieldNames;
    // std::vector<std::pair<std::string, std::string>> columns;
    std::string query = "";
    std::string keys = "";
    WINDOW* winFormScreen = SharedWindowManager::getwinFormScreen();
    WINDOW* winMsgArea = SharedWindowManager::getwinMsgArea();
    debugFile << "line 721 inside TransEntryControl::setupControlFields " << std::endl;
    if (!xmlFile.is_open()) {
        debugFile << "Error inside  TransEntryControl::setupControlFields line 723 xmlfile is not open." << std::endl;
        return -1;
    }

    bool inFields = false;
    bool found = false;
    int linecnt = 0;
    std::string line;
    selectFields_.clear();
    while (std::getline(xmlFile, line)) {
        if (line.find("<field>") != std::string::npos) {
            bool inFields = true;
            // debugFile << "TransEntryControl::setupFields Outter while inFields became true line = " << line << std::endl;
            // Read data from the file and initialize DataEntry objects
            while (std::getline(xmlFile, line)) {
                if (line.find("<field>") != std::string::npos) {
                    if (inFields == false) {
                        inFields = true;
                        // debugFile << "TransEntryControl::setupFields Inner while inFields became trueLine = " << line << std::endl;
                    }
                }
                if (inFields == true) {
                    // debugFile << "TransEntryControl::setupFields Inner while inFields == true.  Line = " << line << std::endl;

                    if (line.find("</field>") != std::string::npos) {
                        // We're at the end of the field XML element, so add the field to the vector
                        if (!fieldName.empty()) {
                            controlFieldProperties field;
                            field.fieldName = fieldName;
                            field.fieldType = fieldType;
                            field.fieldValue = fieldValue;
                            field.calc = calc;
                            field.fieldMask = fieldMask;
                            field.len = len;
                            field.col = col;
                            field.row = row;
                            field.hidden = hidden;

                            controlFields.emplace_back(winFormScreen, winMsgArea,
                                fieldName, fieldType, fieldValue, calc,
                                fieldMask, len, col, row,
                                hidden);

                            inFields = false;
                            fieldName = "";
                            fieldType = "";
                            fieldValue = "";
                            calc = "";
                            fieldMask = "";
                            len = 0;
                            col = 0;
                            row = 0;
                            //labelColumn = 0;
                            //labelText = "";
                            hidden = "";
                            isDisplayField = false;
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

                        // debugFile << "setupFields Tag: " << tag << ", Value: " << value << std::endl;
                        if (tag == "name") {
                            fieldName = DataEntry::cleanString(value);
                        }
                        else if (tag == "type") {
                            fieldType = value;
                        }
                        else if (tag == "calc") {
                            calc = value;
                        }
                        else if (tag == "mask") {
                            fieldMask = value;
                        }
                        else if (tag == "length") {
                            len = std::stoi(value);
                        }
                        else if (tag == "col") {
                            col = std::stoi(value);
                        }
                        else if (tag == "row") {
                            row = std::stoi(value);
                        }
                        else if (tag == "hidden") {
                            hidden = value;
                        }

                    }
                }
                else {

                }
            }
        }

    }
    std::string value = "";
    // making list of fields to select from the table
    // skip those that are calculated 
    for (const auto& entry : controlFields) {
        if (entry.calc.empty()) {
            value = entry.fieldName;

        }
        else {
            value = "0 AS " + entry.fieldName;
        }
        // CANT DO THIS OTHERWISE IT WILL CHANGE "0 AS" TO "0_AS_" 
        //std::replace(value.begin(), value.end(), ' ', '_');  // Replace spaces with underscores
        selectFields_.append(value + ',');
    }


    // ensure TRANS and ENT are always included
     //TODO: find way to not hardcode
    std::set<std::string> requiredKeys = { "TRANS", "ENT" };
    for (const auto& key : requiredKeys) {
        if (selectFields_.find(key) == std::string::npos) {
            selectFields_.append(key + ',');
        }
    }

    if (!selectFields_.empty() && selectFields_.back() == ',') {
        selectFields_.pop_back();
    }

    return 1; // success
}
CommandLineProperties TransEntryControl::parseCommandLineXML(std::ifstream& xmlFile, std::ofstream& debugFile) {
    CommandLineProperties commandLine;
    bool inCommandKeys = false;
    CommandKeyProperties currentKey;
    xmlFile.clear(); // Clear any error flags
    xmlFile.seekg(0, std::ios::beg); // Go back to the beginning of the file

    std::string line;
    while (std::getline(xmlFile, line)) {
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

            if (tag == "CommandText") {
                commandLine.commandText = value;
            }
            else if (tag == "Col") {
                commandLine.col = std::stoi(value);
            }
            else if (tag == "Row") {
                commandLine.row = std::stoi(value);
            }
            else if (tag == "CommandKeys") {
                inCommandKeys = true;
            }
            else if (inCommandKeys) {
                if (tag == "KeyValue") {
                    currentKey.keyValue = value;
                }
                else if (tag == "nextRunType") {
                    currentKey.nextRunType = value;
                }
                else if (tag == "nextRunClass") {
                    currentKey.nextRunClass = value;
                    // All 3 fields collected; push to list
                    commandLine.commandKeys.push_back(currentKey);
                    currentKey = {}; // Reset for next <KeyValue> group
                }
            }

            if (line.find("</CommandKeys>") != std::string::npos) {
                inCommandKeys = false;
            }

            if (line.find("</CommandLine>") != std::string::npos) {
                break;
            }
        }
    }

    return commandLine;
}
// Copy values from headerRecord into lineFields for any fields that exist on the line entry screen.
void TransEntryControl::copyHeaderFieldsToRecord(
    const std::vector<DataEntryFieldProperties>& lineFields,
    const std::map<std::string, std::string>& headerRecord,
    std::map<std::string, std::string>& record)
{
    for (const auto& f : lineFields) {
        const std::string& name = f.fieldName_;
        if (name.empty()) continue;

        auto h = headerRecord.find(name);
        if (h != headerRecord.end()) {
            record[name] = h->second;
        }
    }
}
std::string TransEntryControl::applyMask(const std::string& rawValue, const std::string& mask)
{
    // Find how many fractional places the mask wants (9 or # after the dot)
    std::size_t dotPosMask = mask.find('.');
    int fracPlaces = 0;
    if (dotPosMask != std::string::npos) {
        std::size_t j = dotPosMask + 1;
        while (j < mask.size() && (mask[j] == '9' || mask[j] == '#')) {
            ++fracPlaces;
            ++j;
        }
    }

    // Format numeric value with correct decimal places
    std::string formatted = rawValue;
    if (fracPlaces > 0) {
        try {
            double v = std::stod(rawValue);
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(fracPlaces) << v;
            formatted = oss.str();
        }
        catch (...) {
            formatted = rawValue;  // if parse fails, just use raw
        }
    }

    // Extract digits from formatted value
    std::string digits;
    for (char ch : formatted) {
        unsigned char uch = static_cast<unsigned char>(ch);
        if (std::isdigit(uch)) {
            digits.push_back(ch);
        }
    }

    std::string result(mask.size(), ' ');

    int di = static_cast<int>(digits.size()) - 1;
    int mi = static_cast<int>(mask.size()) - 1;
    int decimalIndexMask = (dotPosMask == std::string::npos)
        ? static_cast<int>(mask.size())
        : static_cast<int>(dotPosMask);

    // Walk mask from right to left, filling placeholders
    while (mi >= 0) {
        char mc = mask[mi];

        if (mc == '9' || mc == '#') {
            if (di >= 0) {
                result[mi] = digits[di];
                --di;
            }
            else {
                // For decimals, pad missing with 0; for integer side, pad with space
                if (dotPosMask != std::string::npos && mi > decimalIndexMask) {
                    result[mi] = '0';
                }
                else {
                    result[mi] = ' ';
                }
            }
        }
        else {
            // Non-placeholder characters (., commas, etc.) are copied as-is
            result[mi] = mc;
        }

        --mi;
    }

    return result;
}

