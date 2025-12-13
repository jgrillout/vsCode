# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## User Note

The user has cognitive deficits and sometimes needs things explained more than once. Check work carefully before sending patched files.

## Build Commands

Build with Visual Studio Code (Ctrl+Shift+B runs default build task):
```
cl.exe /Zi /EHsc /std:c++17 /Fe:DataEntryDevTool.exe *.cpp /link pdcurses.lib sqlite3.lib user32.lib advapi32.lib
```

Or use MSBuild with the vcxproj:
```
msbuild DataEntryDevTool.vcxproj /p:Configuration=Debug /p:Platform=Win32
```

Run the application:
```
DataEntryDevTool.exe [RunType] [RunClass]
```
Without arguments, defaults to `Menu MainMenuScreen`.

## Architecture Overview

This is a legacy-style ncurses (PDCurses on Windows) C++ application for data entry screens with transaction processing. It uses SQLite for data storage.

### Core Components

- **main.cpp**: Entry point and main loop. Routes between Menu, DataEntry, and Transaction screens based on `nextRunType` and `nextRunClass`.

- **DataEntry** (`DataEntry.cpp/.h`): Master/detail data entry screen handler. Reads XML screen definitions, sets up fields, labels, and boxes, then runs input collection loop. Handles CRUD operations on database records.

- **MenuEntry** (`MenuEntry.cpp/.h`): Menu screen handler. Parses menu XML files and displays navigable menu choices.

- **Dialog** (`Dialog.cpp/.h`): Popup dialog handler for choices and confirmations.

- **TransactionEntry** (`TransactionEntry.cpp/.h`): Transaction entry orchestration combining header and line entry.

- **TransEntryControl** (`TransEntryControl.cpp/.h`): Control screen for transaction line listing with scrolling, navigation, and command keys.

- **TransactionEntryHeader/Lines**: Header record entry and line item entry for transactions.

- **ISAMWrapperLib** (`ISAMWrapperLib.cpp/.h`): SQLite wrapper providing ISAM-style operations (insert, update, delete, select).

### Window Managers

- **SharedWindowManager**: Base window creation/destruction
- **MenuWindowManager**: Menu-specific window handling
- **DialogWindowManager**: Dialog window handling

### Input Handlers

- **StringInput.cpp**: String field input
- **NumericInput.cpp**: Numeric field input with decimal handling
- **MaskedInput.cpp**: Masked field input (dates, formatted fields)

### XML Configuration

Screens are defined via XML files in the project root:
- `*Screen.xml`: Screen layouts with fields, labels, boxes
- `*DataDictionary.xml`: Field definitions (type, length, primary key, mask)
- Menu screens (e.g., `MainMenuScreen.xml`, `ARMenuScreen.xml`): Menu choices and navigation

Key XML elements:
- `<Screen>`: Screen properties (type, title, dimensions, borders)
- `<Fields>`: Data entry field definitions
- `<Labels>`: Display labels with optional lookups
- `<Boxes>`: Box drawing coordinates
- `<Choices>`: Menu/dialog options

### Data Flow

1. `main()` loop reads `nextRunType` (Menu, DataEntry, Transaction) and `nextRunClass` (XML filename base)
2. Opens corresponding XML file and database
3. Instantiates appropriate handler class
4. Handler's `run()` method returns result with next screen to navigate to
5. Loop continues until `nextRunClass` is empty or "Exit"

### Key Structs

- `DataEntryFieldProperties`: Complete field definition including position, type, value, validation
- `PassParams`: Parameter passing between screens (key-value pairs)
- `RunResult`: Screen execution result with status and next navigation
- `ScreenProperties`: Screen dimensions, title, border settings

## Dependencies

- **PDCurses 3.8**: Console UI (included in `PDCurses-3.8/`, pre-built `pdcurses.lib`)
- **SQLite3**: Database (included `sqlite3.h`, `sqlite3.lib`, `sqlite3.dll`)
- **C++17**: Language standard

## Database

SQLite database file: `Company.db` (in project root)
