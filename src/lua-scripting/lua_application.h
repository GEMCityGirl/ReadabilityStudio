/** @addtogroup Scripting
    @brief Classes for the scripting.
    @date 2005-2023
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __LUAAPPLICATION_H__
#define __LUAAPPLICATION_H__

#include "../Wisteria-Dataviz/src/i18n-check/src/donttranslate.h"
#include "lua.hpp"
#include "lua_debug.h"
#include "lua_projects.h"
#include <map>
#include <wx/wx.h>

namespace LuaScripting
    {
    void VerifyLink(const wchar_t* link, const size_t length, const bool isImage,
                    const wxString& currentFile, std::multimap<wxString, wxString>& badLinks,
                    std::multimap<wxString, wxString>& badImageSizes,
                    const bool IncludeExternalLinks);
    // Helper functions
    int MsgBox(lua_State* L);
    int GetLuaConstantsPath(lua_State* L);
    int GetProgramPath(lua_State* L);
    int GetDocumentsPath(lua_State* L);
    int GetAbsoluteFilePath(lua_State* L);
    int DownloadFile(lua_State* L);
    int FindFiles(lua_State* L);
    int Close(lua_State*);
    int LogMessage(lua_State* L);
    int LogError(lua_State* L);
    int WriteToFile(lua_State* L);
    int RemoveAllCustomTests(lua_State*);
    int RemoveAllCustomTestBundles(lua_State*);
    int MergeWordLists(lua_State* L);
    int MergePhraseLists(lua_State* L);
    // Append given suffixes to words in a list.
    // Note that this is an internal function used for expanding our proper noun list.
    int ExpandWordList(lua_State* L);
    int PhraseListToWordList(lua_State* L);
    int CrossReferenceWordLists(lua_State* L);
    int SetWindowSize(lua_State* L);
    int GetTestId(lua_State* L);
    int SplashScreen(lua_State* L);
    int CheckHtmlLinks(lua_State* L);
    // Internal testing functions
    int QAVerify(lua_State* L);
    int GetFileCheckSum(lua_State* L);
    int Crash(lua_State*);
    // Setting functions
    int ImportSettings(lua_State* L);
    int ExportSettings(lua_State* L);
    int ResetSettings(lua_State*);
    int DisableAllWarnings(lua_State*);
    int EnableAllWarnings(lua_State*);
    int EnableWarning(lua_State* L);
    int DisableWarning(lua_State* L);
    int SetSpellCheckerOptions(lua_State* L);
    // PROJECT SETTINGS
    int SetReviewer(lua_State* L);
    int GetReviewer(lua_State* L);
    int SetProjectLanguage(lua_State* L);
    int GetProjectLanguage(lua_State* L);
    int GetDocumentStorageMethod(lua_State* L);
    int SetDocumentStorageMethod(lua_State* L);
    int GetParagraphsParsingMethod(lua_State* L);
    int SetParagraphsParsingMethod(lua_State* L);
    int SetMinDocWordCountForBatch(lua_State* L);
    int GetMinDocWordCountForBatch(lua_State* L);
    int SetFilePathDisplayMode(lua_State* L);
    int GetFilePathDisplayMode(lua_State* L);
    // TEXT EXCLUSION OPTIONS
    int SetTextExclusion(lua_State* L);
    // Specifies the minimum number of words that will make a
    // sentence missing terminating punctuation be considered complete.
    int SetIncludeIncompleteTolerance(lua_State* L);
    // Specifies whether or not to aggressively exclude.
    int AggressivelyExclude(lua_State* L);
    // Specifies whether or not to exclude copyright notices.
    int ExcludeCopyrightNotices(lua_State* L);
    // Specifies whether or not to exclude trailing citations.
    int ExcludeTrailingCitations(lua_State* L);
    // Specifies whether or not to exclude file addresses.
    int ExcludeFileAddress(lua_State* L);
    // Specifies whether or not to exclude numerals.
    int ExcludeNumerals(lua_State* L);
    // Specifies whether or not to exclude proper nouns.
    int ExcludeProperNouns(lua_State* L);
    // Sets the filepath to the phrase exclusion list
    // filePath Path to phrase exclusion list
    int SetPhraseExclusionList(lua_State* L);
    // Sets the tags to exclude blocks of text.
    int SetBlockExclusionTags(lua_State* L);
    // GRAPH OPTIONS
    int SetGraphBackgroundColor(lua_State* L);
    int ApplyGraphBackgroundFade(lua_State* L);
    int SetPlotBackgroundImage(lua_State* L);
    int SetPlotBackgroundImageOpacity(lua_State* L);
    int SetPlotBackgroundColor(lua_State* L);
    int SetPlotBackgroundOpacity(lua_State* L);
    int SetGraphWatermark(lua_State* L);
    int SetGraphLogoImage(lua_State* L);
    int SetStippleImage(lua_State* L);
    int SetStippleShape(lua_State* L);
    int DisplayGraphDropShadows(lua_State* L);
    int SetBarChartBarColor(lua_State* L);
    int SetBarChartBarOpacity(lua_State* L);
    int SetBarChartBarEffect(lua_State* L);
    int SetBarChartOrientation(lua_State* L);

    int GetImageInfo(lua_State*);

    // Gets the active standard project
    int GetActiveStandardProject(lua_State* L);
    int GetActiveBatchProject(lua_State* L);

    // printing
    int SetLeftPrintHeader(lua_State* L);
    int SetCenterPrintHeader(lua_State* L);
    int SetRightPrintHeader(lua_State* L);
    int SetLeftPrintFooter(lua_State* L);
    int SetCenterPrintFooter(lua_State* L);
    int SetRightPrintFooter(lua_State* L);

    static const luaL_Reg ApplicationLib[] = {
        { _DT("Close"), Close },
        { "GetLuaConstantsPath", GetLuaConstantsPath },
        { "GetProgramPath", GetProgramPath },
        { "GetDocumentsPath", GetDocumentsPath },
        { "GetAbsoluteFilePath", GetAbsoluteFilePath },
        { "GetActiveStandardProject", GetActiveStandardProject },
        { "GetActiveBatchProject", GetActiveBatchProject },
        { "LogMessage", LogMessage },
        { "LogError", LogError },
        { "MsgBox", MsgBox },
        { "DownloadFile", DownloadFile },
        { "FindFiles", FindFiles },
        { "GetTestId", GetTestId },
        { "GetFileCheckSum", GetFileCheckSum },
        { "SetProjectLanguage", SetProjectLanguage },
        { "GetProjectLanguage", GetProjectLanguage },
        { "SetReviewer", SetReviewer },
        { "GetReviewer", GetReviewer },
        { "GetDocumentStorageMethod", GetDocumentStorageMethod },
        { "SetDocumentStorageMethod", SetDocumentStorageMethod },
        { "GetParagraphsParsingMethod", GetParagraphsParsingMethod },
        { "SetParagraphsParsingMethod", SetParagraphsParsingMethod },
        { "SetMinDocWordCountForBatch", SetMinDocWordCountForBatch },
        { "GetMinDocWordCountForBatch", GetMinDocWordCountForBatch },
        { "SetFilePathDisplayMode", SetFilePathDisplayMode },
        { "GetFilePathDisplayMode", GetFilePathDisplayMode },
        { "ImportSettings", ImportSettings },
        { "ExportSettings", ExportSettings },
        { "ResetSettings", ResetSettings },
        { "DisableAllWarnings", DisableAllWarnings },
        { "EnableAllWarnings", EnableAllWarnings },
        { "EnableWarning", EnableWarning },
        { "DisableWarning", DisableWarning },
        { "SetTextExclusion", SetTextExclusion },
        { "SetIncludeIncompleteTolerance", SetIncludeIncompleteTolerance },
        { "AggressivelyExclude", AggressivelyExclude },
        { "ExcludeCopyrightNotices", ExcludeCopyrightNotices },
        { "ExcludeTrailingCitations", ExcludeTrailingCitations },
        { "ExcludeFileAddress", ExcludeFileAddress },
        { "ExcludeNumerals", ExcludeNumerals },
        { "ExcludeProperNouns", ExcludeProperNouns },
        { "SetPhraseExclusionList", SetPhraseExclusionList },
        { "SetBlockExclusionTags", SetBlockExclusionTags },
        { "SetGraphBackgroundColor", SetGraphBackgroundColor },
        { "ApplyGraphBackgroundFade", ApplyGraphBackgroundFade },
        { "SetPlotBackgroundImage", SetPlotBackgroundImage },
        { "SetPlotBackgroundImageOpacity", SetPlotBackgroundImageOpacity },
        { "SetPlotBackgroundColor", SetPlotBackgroundColor },
        { "SetPlotBackgroundOpacity", SetPlotBackgroundOpacity },
        { "SetGraphWatermark", SetGraphWatermark },
        { "SetGraphLogoImage", SetGraphLogoImage },
        { "SetStippleImage", SetStippleImage },
        { "SetStippleShape", SetStippleShape },
        { "DisplayGraphDropShadows", DisplayGraphDropShadows },
        { "SetBarChartBarColor", SetBarChartBarColor },
        { "SetBarChartBarOpacity", SetBarChartBarOpacity },
        { "SetBarChartBarEffect", SetBarChartBarEffect },
        { "SetBarChartOrientation", SetBarChartOrientation },
        { "SetSpellCheckerOptions", SetSpellCheckerOptions },
        { "SetWindowSize", SetWindowSize },
        { "RemoveAllCustomTests", RemoveAllCustomTests },
        { "RemoveAllCustomTestBundles", RemoveAllCustomTestBundles },
        { "WriteToFile", WriteToFile },
        { "ExpandWordList", ExpandWordList },
        { "PhraseListToWordList", PhraseListToWordList },
        { "CrossReferenceWordLists", CrossReferenceWordLists },
        { "SplashScreen", SplashScreen },
        { "MergeWordLists", MergeWordLists },
        { "MergePhraseLists", MergePhraseLists },
        { "QAVerify", QAVerify },
        { "CheckHtmlLinks", CheckHtmlLinks },
#ifndef NDEBUG
        // just used in debug builds for testing crash reporter
        { _DT("Crash"), Crash },
#endif
        { "SetLeftPrintHeader", SetLeftPrintHeader },
        { "SetCenterPrintHeader", SetCenterPrintHeader },
        { "SetRightPrintHeader", SetRightPrintHeader },
        { "SetLeftPrintFooter", SetLeftPrintFooter },
        { "SetCenterPrintFooter", SetCenterPrintFooter },
        { "SetRightPrintFooter", SetRightPrintFooter },
        { "GetImageInfo", GetImageInfo },
        { nullptr, nullptr }
    };
    } // namespace LuaScripting

/** @}*/

#endif //__LUAAPPLICATION_H__
