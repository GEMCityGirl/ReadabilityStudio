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

#include "../Wisteria-Dataviz/src/util/donttranslate.h"
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

    // quneiform-suppress-begin
    // clang-format off
    // NOTE: these must all be single-line for the build script to properly create new topics from these.

    // Helper functions
    int MsgBox(lua_State* L /*string message*/); // Displays a message dialog.
    int /*string*/ GetLuaConstantsPath(lua_State* L); // Returns the path of the file containing the Lua data types used by the program.
    int /*string*/ GetProgramPath(lua_State* L); // Returns the program's filepath.
    int /*string*/ GetDocumentsPath(lua_State* L); // Returns the path of the user's documents folder.
    int /*string*/ GetAbsoluteFilePath(lua_State* L /*string filePath, string baseFilePath*/); // Returns the absolute filepath for the first path, relative to the second.
    int /*string*/ DownloadFile(lua_State* L /*string Url, string downloadPath*/); // Downloads a webpage to the provided local path.
    int /*table*/ FindFiles(lua_State* L /*string directory, string filePattern*/); // Returns a list of all files from a folder matching the provided file pattern.
    int Close(lua_State*); // Closes the program.
    int LogMessage(lua_State* L /*string message*/); // Sends a message to the program's log report.
    int LogError(lua_State* L /*string errorMessage*/); // Sends a warning message to the program's log report.
    int /*boolean*/ WriteToFile(lua_State* L /*string outputFilePath, string content*/); // Writes a string to a file.
    int RemoveAllCustomTests(lua_State*); // Clears all custom tests from the program.
    int RemoveAllCustomTestBundles(lua_State*); // Clears all custom test bundles from the program.
    int /*boolean*/ MergeWordLists(lua_State* L /*string outputFile, string inputFile1, string inputFile2, ...*/); // Creates a new word list file from a list of other word list files.
    int /*boolean*/ MergePhraseLists(lua_State* L /*string outputFile, string inputFile1, string inputFile2, ...*/); // Creates a new phrase list file from a list of other phrase list files.
    // Append given suffixes to words in a list.
    // Note that this is an internal function used for expanding our proper noun list.
    int /*boolean*/ /*INTERNAL!!!*/ ExpandWordList(lua_State* L /*string inputWordList, string outputFile, ... suffixesToAddToEachWord*/); // INTERNAL FUNCTION!!! SHOULD NOT BE DOCUMENTED.
    int /*boolean*/ /*INTERNAL!!!*/ PhraseListToWordList(lua_State* L /*string inputPhraseList, string outputFile*/); // INTERNAL FUNCTION!!! SHOULD NOT BE DOCUMENTED.
    int /*boolean*/ CrossReferenceWordLists(lua_State* L /*string wordList, string otherWordList, string outputFile*/); // Compares two word lists and creates a new one that contains only the words that appear in both of them.
    int SetWindowSize(lua_State* L /*number width, number height*/); // Sets the size of the program's main window.
    int /*number*/ GetTestId(lua_State* L /*string testName*/); // Converts a test name (a string) into it's enumeration equivalent.
    int /*boolean*/ SplashScreen(lua_State* L /*number imageIndex*/); // Displays the splashscreen, based on the provided splashscreen's index.
    int /*boolean*/ CheckHtmlLinks(lua_State* L /*string path, boolean includeExternalLinks*/); // Checks for broken links in folder of HTML documents. (All broken links will be sent to the log report.)
    // Internal testing functions
    int /*boolean*/ QAVerify /*INTERNAL!!!*/ (lua_State* L); // INTERNAL FUNCTION!!! SHOULD NOT BE DOCUMENTED.
    int /*number*/ GetFileCheckSum /*INTERNAL!!!*/ (lua_State* L /*string filePath*/); // INTERNAL FUNCTION!!! SHOULD NOT BE DOCUMENTED.
    int Crash /*INTERNAL!!!*/ (lua_State*); // INTERNAL FUNCTION!!! SHOULD NOT BE DOCUMENTED.
    // Setting functions
    int /*boolean*/ ImportSettings(lua_State* L /*string filePath*/); // Loads the program's settings from a configuration file.
    int /*boolean*/ ExportSettings(lua_State* L /*string outputFilePath*/); // Saves the program's settings to a configuration file.
    int ResetSettings(lua_State*); // Resets the program's settings to the factory default.
    int DisableAllWarnings(lua_State*); // Suppresses all warnings that can appear in the program.
    int EnableAllWarnings(lua_State*); // Enables all warnings that can appear in the program.
    int EnableWarning(lua_State* L /*string warningId*/); // Enables a specific warning that can appear in the program.
    int DisableWarning(lua_State* L /*string warningId*/); // Disables a specific warning that can appear in the program.
    int SetSpellCheckerOptions(lua_State* L /*boolean ignoreProperNouns, boolean ignoreUppercased, boolean ignoreNumerals, boolean ignoreFileAddresses, boolean ignoreProgrammerCode, boolean ignoreSocialMediaTags, boolean allowColloquialisms*/); // Sets spell-checker options for new projects.
    // PROJECT SETTINGS
    int SetReviewer(lua_State* L /*string reviewer*/); // Sets the reviewer's name for new projects.
    int /*string*/ GetReviewer(lua_State* L); // Returns the reviewer's name.
    int SetProjectLanguage(lua_State* L /*Language lang*/); // Sets the default language for new projects. This will affect syllable counting and also which tests are available.
    int /*Language*/ GetProjectLanguage(lua_State* L); // Returns the default language for new projects.
    int /*TextStorage*/ GetDocumentStorageMethod(lua_State* L); // Returns whether the project embeds its documents or links to them for new projects.
    int SetDocumentStorageMethod(lua_State* L /*TextStorage storageMethod*/); // Sets whether new projects embed their documents or link to them.
    int /*ParagraphParse*/ GetParagraphsParsingMethod(lua_State* L); // Returns the default method for how paragraphs are parsed for new projects.
    int SetParagraphsParsingMethod(lua_State* L /*ParagraphParse parseMethod*/); // Sets how hard returns help determine how paragraphs and sentences are detected for new projects.
    int SetMinDocWordCountForBatch(lua_State* L /*number minWordCount*/); // Sets the minimum number of words a document must have for new batch projects.
    int /*number*/ GetMinDocWordCountForBatch(lua_State* L); // Returns the minimum number of words a document must have for new batch projects.
    int SetFilePathDisplayMode(lua_State* L /*FilePathDisplayMode displayMode*/); // Sets how filepaths are displayed for new batch projects.
    int /*FilePathDisplayMode*/ GetFilePathDisplayMode(lua_State* L); // Returns how filepaths are displayed for new batch projects.
    // TEXT EXCLUSION OPTIONS
    int SetTextExclusion(lua_State* L /*TextExclusionType exclusionType*/); // Sets how text should be excluded for new projects.
    int SetIncludeIncompleteTolerance(lua_State* L /*number minWordsForCompleteSentence*/); // Sets the incomplete-sentence tolerance for new projects. This is the minimum number of words that will make a sentence missing terminating punctuation be considered complete.
    int AggressivelyExclude(lua_State* L /*boolean beAggressive*/); // Sets whether to aggressively exclude for new projects.
    int ExcludeCopyrightNotices(lua_State* L /*boolean exclude*/); // Sets whether to exclude copyright notices for new projects.
    int ExcludeTrailingCitations(lua_State* L /*boolean exclude*/); // Sets whether to exclude trailing citations for new projects.
    int ExcludeFileAddress(lua_State* L /*boolean exclude*/); // Sets whether to exclude file addresses for new projects.
    int ExcludeNumerals(lua_State* L ); // Sets whether to exclude numerals for new projects.
    int ExcludeProperNouns(lua_State* L /*boolean exclude*/); // Sets whether to exclude proper nouns for new projects.
    int SetPhraseExclusionList(lua_State* L /*string exclusionListPath*/); // Sets the filepath to the phrase exclusion list for new projects.
    // Sets the tags to exclude blocks of text.
    int SetBlockExclusionTags(lua_State* L /*string tagString*/); // Sets the text exclusion tags for new projects. This should be a two-character string containing a pair of exclusion tags.
    // GRAPH OPTIONS
    int SetGraphBackgroundColor(lua_State* L /*number red, number green, number blue*/); // Sets the graph background color for new projects.
    int ApplyGraphBackgroundFade(lua_State* L /*bool applyFade*/); // Sets whether to apply a fade to graph background colors for new projects.
    int SetPlotBackgroundImage(lua_State* L /*string imagePath*/); // Sets the graph background (plot area) image for new projects.
    int SetPlotBackgroundImageOpacity(lua_State* L /*number opacity*/); // Sets the graph background (plot area) image opacity for new projects.
    int SetPlotBackgroundColor(lua_State* L /*number red, number green, number blue*/); // Sets the graph background (plot area) color for new projects.
    int SetPlotBackgroundOpacity(lua_State* L /*number opacity*/); // Sets the graph background (plot area) color opacity for new projects.
    int SetWatermark(lua_State* L /*string watermark*/); // Sets the watermark drawn on graphs for new projects.
    int SetGraphLogoImage(lua_State* L /*string imagePath*/); // Sets the logo image for new projects.
    int SetStippleImage(lua_State* L /*string imagePath*/);// Sets the stipple image used to draw bars in graphs for new projects.
    int SetStippleShape(lua_State* L /*string shapeId*/); // Sets the stipple shape used to draw bars in graphs for new projects.
    int DisplayGraphDropShadows(lua_State* L /*bool displayShadows*/); // Sets whether to display shadows on graphs for new projects.
    int SetBarChartBarColor(lua_State* L /*number red, number green, number blue*/); // Sets bar color (in bar charts) for new projects.
    int SetBarChartBarOpacity(lua_State* L /*number opacity*/); // Sets bar opacity (in bar charts) for new projects.
    int SetBarChartBarEffect(lua_State* L /*BoxEffect barEffect*/); // Sets bar appearance (in bar charts) for new projects.
    int SetBarChartOrientation(lua_State* L /*Orientation barOrientation*/); // Sets the orientation for bars for new projects.

    int /*table*/ GetImageInfo(lua_State* /*string imagePath*/); // Returns width and height for an image.
    int /*boolean*/ ApplyImageEffect(lua_State* /*string inputImagePath, string outputImagePath, ImageEffect effect*/); // Applies an effect to an image and saves the result to another image file. Returns true if image successfully saved.
    int /*boolean*/ StitchImages(lua_State* /*string outputImagePath, Orientation direction, string inputImage1, string inputImage2, ...*/); // Combines a list of images vertically or horizontally. Returns true if image successfully saved.

    // Gets the active projects
    int /*StandardProject*/ GetActiveStandardProject(lua_State* L); // Returns the active standard project.
    int /*BatchProject*/ GetActiveBatchProject(lua_State* L); // Returns the active batch project.

    // printing
    int SetLeftPrintHeader(lua_State* L /*string label*/); // Sets the left print header.
    int SetCenterPrintHeader(lua_State* L /*string label*/); // Sets the center print header.
    int SetRightPrintHeader(lua_State* L /*string label*/); // Sets the right print header.
    int SetLeftPrintFooter(lua_State* L /*string label*/); // Sets the left print footer.
    int SetCenterPrintFooter(lua_State* L /*string label*/); // Sets the center print footer.
    int SetRightPrintFooter(lua_State* L /*string label*/); // Sets the right print footer.
    // clang-format on
    // quneiform-suppress-end

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
        { "SetWatermark", SetWatermark },
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
        { "ApplyImageEffect", ApplyImageEffect },
        { "StitchImages", StitchImages },
        { nullptr, nullptr }
    };
    } // namespace LuaScripting

/** @}*/

#endif //__LUAAPPLICATION_H__
