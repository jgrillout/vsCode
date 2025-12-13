// Version-02.20.25.16.07
//File: InputUtils.h
#pragma once
#ifndef INPUT_UTILS_H
#define INPUT_UTILS_H
#include "DataEntry.h"  // For DataEntryFieldProperties
//struct DataEntryFieldProperties; // Forward declaration
#include "DataEntryFieldProperties.h"

class InputUtils {
public:
    static bool stringInput(DataEntryFieldProperties& field, std::ofstream& debugFile);
    static bool numericInput(DataEntryFieldProperties& field, std::ofstream& debugFile);
    static bool maskedInput(DataEntryFieldProperties& field, std::ofstream& debugFile);
    
    
    static std::string removeMask(const std::string& maskedInput, std::ofstream& debugFile);
    static void inspectMask(const std::string& format, size_t& leftSize, size_t& rightSize, std::ofstream& debugFile);
    static std::string combineInputWithMask(const std::string& mask, const std::string& input, std::ofstream& debugFile);
    static std::string generateDisplayMask(const std::string& mask);
    static void displayMask(DataEntryFieldProperties& field, std::string& input, std::ofstream& debugFile);
};
#endif

