// File: DataEntryFieldProperties.h
#pragma once


#include "CursesCompat.h"
#include <string>

struct DataEntryFieldProperties {
    WINDOW* winFormScreen_;
    WINDOW* winMsgArea_;
    std::string inputKeyPressed_;
    std::string fieldName_;
    std::string fieldType_;
    std::string fieldMask_;      // from DataDictionaryFieldProperties
    int len_;                    // from DataDictionaryFieldProperties
    std::string primaryKey_;     // from DataDictionaryFieldProperties
    int fieldColumn_;
    int row_;
    std::string calc_;
    std::string fieldValue_;
    std::string allowedChoices_;
    std::string choiceDescriptions_;
    std::string inquiryAllowed_;
    std::string inquiryOrderBy_;
    std::string inquiryFields_;
    std::string fieldRequired_;
    std::string isDisplayOnly_;
    //std::string hidden_;
    //std::string defaultValue_;

    DataEntryFieldProperties() = default;

    DataEntryFieldProperties(
        WINDOW* winFormScreen,
        WINDOW* winMsgArea,
        const std::string& inputKeyPressed,
        const std::string& fieldName,
        const std::string& fieldType,
        const std::string& fieldMask,
        int len,
        const std::string& primaryKey,
        int row,
        int fieldColumn,
        const std::string& calc,
        const std::string& fieldValue,
        const std::string& allowedChoices,
        const std::string& choiceDescriptions,
        const std::string& inquiryAllowed,
        const std::string& inquiryOrderBy,
        const std::string& inquiryFields,
        const std::string& fieldRequired,
        const std::string& isDisplayOnly
        //const std::string& hidden
        
    )
        : winFormScreen_(winFormScreen), winMsgArea_(winMsgArea),
        inputKeyPressed_(inputKeyPressed), fieldName_(fieldName),
        fieldType_(fieldType), fieldMask_(fieldMask), len_(len), primaryKey_(primaryKey),
        row_(row), fieldColumn_(fieldColumn), calc_(calc), fieldValue_(fieldValue),
        allowedChoices_(allowedChoices), choiceDescriptions_(choiceDescriptions),
        inquiryAllowed_(inquiryAllowed), inquiryOrderBy_(inquiryOrderBy),
        inquiryFields_(inquiryFields), fieldRequired_(fieldRequired),
        isDisplayOnly_(isDisplayOnly)//, hidden_(hidden)
    {}
};
