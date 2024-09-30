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
#include "lua_batch_project.h"
#include "lua_debug.h"
#include "lua_standard_project.h"
#include <map>
#include <wx/wx.h>

namespace LuaScripting
    {
    void VerifyLink(const wchar_t* link, const size_t length, const bool isImage,
                    const wxString& currentFile, std::multimap<wxString, wxString>& badLinks,
                    std::multimap<wxString, wxString>& badImageSizes,
                    const bool IncludeExternalLinks);
    // clang-format off
    // Helper functions
    int MsgBox(lua_State* L /*string message*/);
    int /*string*/ GetLuaConstantsPath(lua_State* L);
    int /*string*/ GetProgramPath(lua_State* L);
    int /*string*/ GetDocumentsPath(lua_State* L);
    int /*string*/ GetAbsoluteFilePath(lua_State* L);
    int /*string*/ DownloadFile(lua_State* L /*string Url, string downloadPath*/);
    int /*table*/ FindFiles(lua_State* L /*string directory, string filePattern*/);
    int Close(lua_State*);
    int LogMessage(lua_State* L /*string message*/);
    int LogError(lua_State* L /*string errorMessage*/);
    int /*boolean*/ WriteToFile(lua_State* L /*string outputFilePath, string content*/);
    int RemoveAllCustomTests(lua_State*);
    int RemoveAllCustomTestBundles(lua_State*);
    int /*boolean*/ MergeWordLists(lua_State* L /*string outputFile, ... inputFiles*/);
    int /*boolean*/ MergePhraseLists(lua_State* L /*string outputFile, ... inputFiles*/);
    // Append given suffixes to words in a list.
    // Note that this is an internal function used for expanding our proper noun list.
    int /*boolean*/ ExpandWordList(lua_State* L /*string inputWordList, string outputFile, ... suffixesToAddToEachWord*/);
    int /*boolean*/ PhraseListToWordList(lua_State* L /*string inputPhraseList, string outputFile*/);
    int /*boolean*/ CrossReferenceWordLists(lua_State* L /*string wordList, string otherWordList, string outputFile*/);
    int SetWindowSize(lua_State* L /*number width, number height*/);
    int /*number*/ GetTestId(lua_State* L /*string testName*/);
    int /*boolean*/ SplashScreen(lua_State* L /*number imageIndex*/);
    int /*boolean*/ CheckHtmlLinks(lua_State* L /*string path, boolean includeExternalLinks*/);
    // Internal testing functions
    int /*boolean*/ QAVerify /*INTERNAL*/ (lua_State* L);
    int /*number*/ GetFileCheckSum /*INTERNAL*/ (lua_State* L /*string filePath*/);
    int Crash /*INTERNAL*/ (lua_State*);
    // Setting functions
    int /*boolean*/ ImportSettings(lua_State* L /*string filePath*/);
    int /*boolean*/ ExportSettings(lua_State* L /*string outputFilePath*/);
    int ResetSettings(lua_State*);
    int DisableAllWarnings(lua_State*);
    int EnableAllWarnings(lua_State*);
    int EnableWarning(lua_State* L /*string messageId*/);
    int DisableWarning(lua_State* L /*string messageId*/);
    int SetSpellCheckerOptions(lua_State* L /*boolean ignoreProperNouns, boolean ignoreUppercased, boolean ignoreNumerals, boolean ignoreFileAddresses, boolean ignoreProgrammerCode, boolean ignoreSocialMediaTags, boolean allowColloquialisms*/);
    // PROJECT SETTINGS
    int SetReviewer(lua_State* L /*string reviewer*/);
    int /*string*/ GetReviewer(lua_State* L);
    int SetProjectLanguage(lua_State* L /*Language lang*/);
    int /*Language*/ GetProjectLanguage(lua_State* L);
    int /*TextStorage*/ GetDocumentStorageMethod(lua_State* L);
    int SetDocumentStorageMethod(lua_State* L /*TextStorage storageMethod*/);
    int /*ParagraphParse*/ GetParagraphsParsingMethod(lua_State* L);
    int SetParagraphsParsingMethod(lua_State* L /*ParagraphParse parseMethod*/);
    int SetMinDocWordCountForBatch(lua_State* L /*number minWordCount*/);
    int /*number*/ GetMinDocWordCountForBatch(lua_State* L);
    int SetFilePathDisplayMode(lua_State* L /*FilePathDisplayMode displayMode*/);
    int /*FilePathDisplayMode*/ GetFilePathDisplayMode(lua_State* L);
    // TEXT EXCLUSION OPTIONS
    int SetTextExclusion(lua_State* L /*TextExclusionType exclusionType*/);
    // Specifies the minimum number of words that will make a
    // sentence missing terminating punctuation be considered complete.
    int SetIncludeIncompleteTolerance(lua_State* L /*number minWordsForCompleteSentence*/);
    // Specifies whether or not to aggressively exclude.
    int AggressivelyExclude(lua_State* L /*boolean beAggressive*/);
    // Specifies whether or not to exclude copyright notices.
    int ExcludeCopyrightNotices(lua_State* L /*boolean exclude*/);
    // Specifies whether or not to exclude trailing citations.
    int ExcludeTrailingCitations(lua_State* L /*boolean exclude*/);
    // Specifies whether or not to exclude file addresses.
    int ExcludeFileAddress(lua_State* L /*boolean exclude*/);
    // Specifies whether or not to exclude numerals.
    int ExcludeNumerals(lua_State* L );
    // Specifies whether or not to exclude proper nouns.
    int ExcludeProperNouns(lua_State* L /*boolean exclude*/);
    // Sets the filepath to the phrase exclusion list
    // filePath Path to phrase exclusion list
    int SetPhraseExclusionList(lua_State* L /*string exclusionListPath*/);
    // Sets the tags to exclude blocks of text.
    int SetBlockExclusionTags(lua_State* L /*string tagString*/);
    // GRAPH OPTIONS
    int SetGraphBackgroundColor(lua_State* L /*number red, number green, number blue*/);
    int ApplyGraphBackgroundFade(lua_State* L /*bool applyFade*/);
    int SetPlotBackgroundImage(lua_State* L /*string imagePath*/);
    int SetPlotBackgroundImageOpacity(lua_State* L /*number opacity*/);
    int SetPlotBackgroundColor(lua_State* L /*number red, number green, number blue*/);
    int SetPlotBackgroundOpacity(lua_State* L /*number opacity*/);
    int SetGraphWatermark(lua_State* L /*string watermark*/);
    int SetGraphLogoImage(lua_State* L /*string imagePath*/);
    int SetStippleImage(lua_State* L /*string imagePath*/);
    int SetStippleShape(lua_State* L /*string shapeId*/);
    int DisplayGraphDropShadows(lua_State* L /*bool displayShadows*/);
    int SetBarChartBarColor(lua_State* L /*number red, number green, number blue*/);
    int SetBarChartBarOpacity(lua_State* L /*number opacity*/);
    int SetBarChartBarEffect(lua_State* L /*BoxEffect barEffect*/);
    int SetBarChartOrientation(lua_State* L /*Orientation barOriention*/);

    int /*table*/ GetImageInfo(lua_State* /*string imagePath*/);

    // Gets the active projects
    int /*StandardProject*/ GetActiveStandardProject(lua_State* L);
    int /*BatchProject*/ GetActiveBatchProject(lua_State* L);

    // printing
    int SetLeftPrintHeader(lua_State* L /*string label*/);
    int SetCenterPrintHeader(lua_State* L /*string label*/);
    int SetRightPrintHeader(lua_State* L /*string label*/);
    int SetLeftPrintFooter(lua_State* L /*string label*/);
    int SetCenterPrintFooter(lua_State* L /*string label*/);
    int SetRightPrintFooter(lua_State* L /*string label*/);
    // clang-format on

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
