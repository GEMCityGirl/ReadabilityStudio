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
    int /*string*/ GetUserPath(lua_State* L /*UserPath path*/); // Returns the path of a given folder in the user's directory.
    int /*string*/ GetAbsoluteFilePath(lua_State* L /*string filePath, string baseFilePath*/); // Returns the absolute filepath for the first path, relative to the second.
    int /*string*/ DownloadFile(lua_State* L /*string Url, string downloadPath*/); // Downloads a webpage to the provided local path.
    int /*table*/ FindFiles(lua_State* L /*string directory, string filePattern, boolean recursive*/); // Returns a list of all files from a folder matching the provided file pattern.
    int Close(lua_State*); // Closes the program.
    int LogMessage(lua_State* L /*string message*/); // Sends a message to the program's log report.
    int LogError(lua_State* L /*string errorMessage*/); // Sends a warning message to the program's log report.
    int /*boolean*/ WriteToFile(lua_State* L /*string outputFilePath, string content*/); // Writes a string to a file.
    int RemoveAllCustomTests(lua_State*); // Clears all custom tests from the program.
    int RemoveAllCustomTestBundles(lua_State*); // Clears all custom test bundles from the program.
    int /*boolean*/ MergeWordLists(lua_State* L /*string inputFile1, string inputFile2, ..., string outputFile*/); // Creates a new word list file from a list of other word list files.
    int /*boolean*/ MergePhraseLists(lua_State* L /*string inputFile1, string inputFile2, ..., string outputFile*/); // Creates a new phrase list file from a list of other phrase list files.
    // Append given suffixes to words in a list.
    // Note that these are internal functions used for expanding our proper noun list.
    int /*boolean*/ /*INTERNAL!!!*/ ExpandWordList(lua_State* L /*string inputWordList, string outputFile, ... suffixesToAddToEachWord*/); // INTERNAL FUNCTION!!! SHOULD NOT BE DOCUMENTED.
    int /*boolean*/ /*INTERNAL!!!*/ PhraseListToWordList(lua_State* L /*string inputPhraseList, string outputFile*/); // INTERNAL FUNCTION!!! SHOULD NOT BE DOCUMENTED.
    int /*boolean*/ CrossReferencePhraseLists(lua_State* L /*string phraseList, string otherPhraseList, string outputFile*/); // Compares two phrase lists and creates a new one that contains only the words that appear in both of them.
    int SetWindowSize(lua_State* L /*number width, number height*/); // Sets the size of the program's main window.
    int /*number*/ GetTestId(lua_State* L /*string testName*/); // Converts a test name (a string) into it's enumeration equivalent.
    int /*boolean*/ SplashScreen(lua_State* L /*number imageIndex*/); // Displays the splashscreen, based on the provided splashscreen's index.
    int /*boolean*/ CheckHtmlLinks(lua_State* L /*string path, boolean includeExternalLinks*/); // Checks for broken links in folder of HTML documents. (All broken links will be sent to the log report.)
    // Internal testing functions
    int /*boolean*/ QAVerify /*INTERNAL!!!*/ (lua_State* L); // INTERNAL FUNCTION!!! SHOULD NOT BE DOCUMENTED.
    int /*number*/ GetFileCheckSum /*INTERNAL!!!*/ (lua_State* L /*string filePath*/); // INTERNAL FUNCTION!!! SHOULD NOT BE DOCUMENTED.

    // GENERAL SETTINGS
    int /*boolean*/ ImportSettings(lua_State* L /*string filePath*/); // Loads the program's settings from a configuration file.
    int /*boolean*/ ExportSettings(lua_State* L /*string outputFilePath*/); // Saves the program's settings to a configuration file.
    int ResetSettings(lua_State*); // Resets the program's settings to the factory default.
    int DisableAllWarnings(lua_State*); // Suppresses all warnings that can appear in the program.
    int EnableAllWarnings(lua_State*); // Enables all warnings that can appear in the program.
    int EnableWarning(lua_State* L /*string warningId*/); // Enables a specific warning that can appear in the program.
    int DisableWarning(lua_State* L /*string warningId*/); // Disables a specific warning that can appear in the program.
    int SetUserAgent(lua_State* L /*string userAgent*/); // Sets the user agent used by the web harvester.
    int /*string*/ GetUserAgent(lua_State* L); // Returns the user agent used by the web harvester.
    int DisableSSLVerification(lua_State* L /*bool disable*/); // Toggles whether SSL verification used by the web harvester is disabled.
    int /*boolean*/ IsSSLVerificationDisabled(lua_State* L); // Returns whether SSL verification used by the web harvester is disabled.
    int UseJavaScriptCookies(lua_State* L /*bool useJSCookies*/); // Toggles whether the web harvester parses JavaScript for cookies needed by the website.
    int /*boolean*/ IsUsingJavaScriptCookies(lua_State* L); // Returns whether the web harvester parses JavaScript for cookies needed by the website.
    int PersistCookies(lua_State* L /*bool keepCookies*/); // Toggles whether the web harvester uses keeps cookies read from JavaScript between harvests.
    int /*boolean*/ IsPersistingCookies(lua_State* L); // Returns whether the web harvester uses keeps cookies read from JavaScript between harvests.
    int EnableVerboseLogging(lua_State* L /*bool enable*/); // Toggles whether verbose logging is enabled.
    int /*boolean*/ IsLoggingVerbose(lua_State* L); // Returns whether verbose logging is enabled.
    int AppendDailyLog(lua_State* L /*bool append*/); // Toggles whether the current log file should be appended when the program restarts.
    int /*boolean*/ IsAppendingDailyLog(lua_State* L); // Returns whether the current log file is being appended when the program restarts.

    // PROJECT SETTINGS
    int SetReviewer(lua_State* L /*string reviewer*/); // Sets the reviewer's name for new projects.
    int /*string*/ GetReviewer(lua_State* L); // Returns the reviewer's name.
    int SetProjectLanguage(lua_State* L /*Language lang*/); // Sets the default language for new projects. This will affect syllable counting and also which tests are available.
    int /*Language*/ GetProjectLanguage(lua_State* L); // Returns the default language for new projects.
    int /*TextStorage*/ GetDocumentStorageMethod(lua_State* L); // Returns whether the project embeds its documents or links to them for new projects.
    int SetDocumentStorageMethod(lua_State* L /*TextStorage storageMethod*/); // Sets whether new projects embed their documents or link to them.
    int SetMinDocWordCountForBatch(lua_State* L /*number minWordCount*/); // Sets the minimum number of words a document must have for new batch projects.
    int /*number*/ GetMinDocWordCountForBatch(lua_State* L); // Returns the minimum number of words a document must have for new batch projects.
    int SetFilePathDisplayMode(lua_State* L /*FilePathDisplayMode displayMode*/); // Sets how filepaths are displayed for new batch projects.
    int /*FilePathDisplayMode*/ GetFilePathDisplayMode(lua_State* L); // Returns how filepaths are displayed for new batch projects.
    int SetAppendedDocumentFilePath(lua_State* L /*string filePath*/); // Sets the file path to the document being appended for analysis.
    int /*string*/ GetAppendedDocumentFilePath(lua_State* L); // Returns the file path to the document being appended for analysis.
    int UseRealTimeUpdate(lua_State* L /*bool use*/); // Toggles whether documents are being re-analyzed as they change.
    int /*boolean*/ IsRealTimeUpdating(lua_State* L); // Returns whether documents are being re-analyzed as they change.

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
    int /*ParagraphParse*/ GetParagraphsParsingMethod(lua_State* L); // Returns the default method for how paragraphs are parsed for new projects.
    int SetParagraphsParsingMethod(lua_State* L /*ParagraphParse parseMethod*/); // Sets how hard returns help determine how paragraphs and sentences are detected for new projects.
    // Sets the tags to exclude blocks of text.
    int SetBlockExclusionTags(lua_State* L /*string tagString*/); // Sets the text exclusion tags for new projects. This should be a two-character string containing a pair of exclusion tags.

    // GRAMMAR
    int SetSpellCheckerOptions(lua_State* L /*boolean ignoreProperNouns, boolean ignoreUppercased, boolean ignoreNumerals, boolean ignoreFileAddresses, boolean ignoreProgrammerCode, boolean ignoreSocialMediaTags, boolean allowColloquialisms*/); // Sets spell-checker options for new projects.
    int SetGrammarResultsOptions(lua_State* L /*boolean includeHighlightedReport, boolean includeErrors, boolean includePossibleMisspellings, boolean includeRepeatedWords, boolean includeArticleMismatches, boolean includeRedundantPhrases, boolean includeOverusedWords, boolean includeWordiness, boolean includeCliches, boolean includePassiveVoice, boolean includeConjunctionStartingSentences, boolean includeLowercasedSentences*/); // Sets which grammar results should be included for new projects.

    // GRAPH OPTIONS
    int SetGraphColorScheme(lua_State* L /*string colorScheme*/); // Sets the graph color scheme for new projects.
    int /*string*/ GetGraphColorScheme(lua_State* L); // Returns the graph color scheme for new projects.
    int SetGraphBackgroundColor(lua_State* L /*number red, number green, number blue*/); // Sets the graph background color for new projects.
    int ApplyGraphBackgroundFade(lua_State* L /*bool applyFade*/); // Sets whether to apply a fade to graph background colors for new projects.
    int SetGraphCommonImage(lua_State* L /*string imagePath*/); // Sets the common image drawn across all bars.
    int SetPlotBackgroundImage(lua_State* L /*string imagePath*/); // Sets the graph background (plot area) image for new projects.
    int SetPlotBackgroundImageEffect(lua_State* L /*ImageEffect imageEffect*/); // Sets the effect applied to an image when drawn as a graph's background for new projects.
    int SetPlotBackgroundImageFit(lua_State* L /*ImageFit fitType*/); // Specifies how to adjust an image to fit within a graph's background for new projects.
    int SetPlotBackgroundImageOpacity(lua_State* L /*number opacity*/); // Sets the graph background (plot area) image opacity for new projects.
    int SetPlotBackgroundColor(lua_State* L /*number red, number green, number blue*/); // Sets the graph background (plot area) color for new projects.
    int SetPlotBackgroundOpacity(lua_State* L /*number opacity*/); // Sets the graph background (plot area) color opacity for new projects.
    int SetWatermark(lua_State* L /*string watermark*/); // Sets the watermark drawn on graphs for new projects.
    int SetGraphLogoImage(lua_State* L /*string imagePath*/); // Sets the logo image for new projects.
    int SetStippleImage(lua_State* L /*string imagePath*/);// Sets the stipple image used to draw bars in graphs for new projects.
    int SetStippleShape(lua_State* L /*string shapeId*/); // Sets the stipple shape used to draw bars in graphs for new projects.
    int SetXAxisFont(lua_State* L /*string fontName, number pointSize, number fontWeight, string color*/); // Sets the font for the graphs' X axes for new projects.
    int SetYAxisFont(lua_State* L /*string fontName, number pointSize, number fontWeight, string color*/); // Sets the font for the graphs' Y axes for new projects.
    int SetGraphTopTitleFont(lua_State* L /*string fontName, number pointSize, number fontWeight, string color*/); // Sets the font for the graphs' top titles for new projects.
    int SetGraphBottomTitleFont(lua_State* L /*string fontName, number pointSize, number fontWeight, string color*/); // Sets the font for the graphs' bottom titles.
    int SetGraphLeftTitleFont(lua_State* L /*string fontName, number pointSize, number fontWeight, string color*/); // Sets the font for the graphs' left titles.
    int SetGraphRightTitleFont(lua_State* L /*string fontName, number pointSize, number fontWeight, string color*/); // Sets the font for the graphs' right titles.
    int DisplayBarChartLabels(lua_State* L /*boolean display*/); // Specifies whether to display labels above each bar in a bar chart for new projects.
    int DisplayGraphDropShadows(lua_State* L /*bool displayShadows*/); // Sets whether to display shadows on graphs for new projects.
    int SetBarChartBarColor(lua_State* L /*number red, number green, number blue*/); // Sets bar color (in bar charts) for new projects.
    int SetBarChartBarOpacity(lua_State* L /*number opacity*/); // Sets bar opacity (in bar charts) for new projects.
    int SetBarChartBarEffect(lua_State* L /*BoxEffect barEffect*/); // Sets bar appearance (in bar charts) for new projects.
    int SetBarChartOrientation(lua_State* L /*Orientation barOrientation*/); // Sets the orientation for bars for new projects.
    int SetGraphInvalidRegionColor(lua_State* L /*number red, number green, number blue*/); // Sets the color for the invalid score regions for Fry-like graphs for new projects.
    int SetStippleShapeColor(lua_State* L /*number red, number green, number blue*/); // If using stipple shapes for bars, sets the color for certain shapes for new projects.
    int ShowcaseKeyItems(lua_State* L /*boolean showcase*/); // Specifies whether important parts of certain graphs should be highlighted for new projects.

    int /*table*/ GetImageInfo(lua_State* /*string imagePath*/); // Returns width and height for an image.
    int /*boolean*/ ApplyImageEffect(lua_State* /*string inputImagePath, string outputImagePath, ImageEffect effect*/); // Applies an effect to an image and saves the result to another image file. Returns true if image was successfully saved.
    int /*boolean*/ StitchImages(lua_State* /*string inputImage1, ..., string outputImagePath, Orientation direction*/); // Combines a list of images vertically or horizontally. Returns true if image was successfully saved.

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
    int /*string*/ GetLeftPrintHeader(lua_State* L); // Returns the left print header.
    int /*string*/ GetCenterPrintHeader(lua_State* L); // Returns the center print header.
    int /*string*/ GetRightPrintHeader(lua_State* L); // Returns the right print header.
    int /*string*/ GetLeftPrintFooter(lua_State* L); // Returns the left print footer.
    int /*string*/ GetCenterPrintFooter(lua_State* L); // Returns the center print footer.
    int /*string*/ GetRightPrintFooter(lua_State* L); // Returns the right print footer.
    // clang-format on
    // quneiform-suppress-end

    static const luaL_Reg ApplicationLib[] = {
        { _DT("Close"), Close },
        { "GetLuaConstantsPath", GetLuaConstantsPath },
        { "GetProgramPath", GetProgramPath },
        { "GetUserPath", GetUserPath },
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
        { "SetUserAgent", SetUserAgent },
        { "GetUserAgent", GetUserAgent },
        { "DisableSSLVerification", DisableSSLVerification },
        { "IsSSLVerificationDisabled", IsSSLVerificationDisabled },
        { "UseJavaScriptCookies", UseJavaScriptCookies },
        { "IsUsingJavaScriptCookies", IsUsingJavaScriptCookies },
        { "PersistCookies", PersistCookies },
        { "IsPersistingCookies", IsPersistingCookies },
        { "EnableVerboseLogging", EnableVerboseLogging },
        { "IsLoggingVerbose", IsLoggingVerbose },
        { "AppendDailyLog", AppendDailyLog },
        { "IsAppendingDailyLog", IsAppendingDailyLog },
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
        { "SetAppendedDocumentFilePath", SetAppendedDocumentFilePath },
        { "GetAppendedDocumentFilePath", GetAppendedDocumentFilePath },
        { "UseRealTimeUpdate", UseRealTimeUpdate },
        { "IsRealTimeUpdating", IsRealTimeUpdating },
        { "SetGraphColorScheme", SetGraphColorScheme },
        { "GetGraphColorScheme", GetGraphColorScheme },
        { "SetGraphBackgroundColor", SetGraphBackgroundColor },
        { "ApplyGraphBackgroundFade", ApplyGraphBackgroundFade },
        { "SetGraphCommonImage", SetGraphCommonImage },
        { "SetPlotBackgroundImage", SetPlotBackgroundImage },
        { "SetPlotBackgroundImageEffect", SetPlotBackgroundImageEffect },
        { "SetPlotBackgroundImageFit", SetPlotBackgroundImageFit },
        { "SetPlotBackgroundImageOpacity", SetPlotBackgroundImageOpacity },
        { "SetPlotBackgroundColor", SetPlotBackgroundColor },
        { "SetPlotBackgroundOpacity", SetPlotBackgroundOpacity },
        { "SetWatermark", SetWatermark },
        { "SetGraphLogoImage", SetGraphLogoImage },
        { "SetStippleImage", SetStippleImage },
        { "SetStippleShape", SetStippleShape },
        { "SetXAxisFont", SetXAxisFont },
        { "SetYAxisFont", SetYAxisFont },
        { "SetGraphTopTitleFont", SetGraphTopTitleFont },
        { "SetGraphBottomTitleFont", SetGraphBottomTitleFont },
        { "SetGraphLeftTitleFont", SetGraphLeftTitleFont },
        { "SetGraphRightTitleFont", SetGraphRightTitleFont },
        { "DisplayBarChartLabels", DisplayBarChartLabels },
        { "DisplayGraphDropShadows", DisplayGraphDropShadows },
        { "SetBarChartBarColor", SetBarChartBarColor },
        { "SetBarChartBarOpacity", SetBarChartBarOpacity },
        { "SetBarChartBarEffect", SetBarChartBarEffect },
        { "SetBarChartOrientation", SetBarChartOrientation },
        { "SetGraphInvalidRegionColor", SetGraphInvalidRegionColor },
        { "SetStippleShapeColor", SetStippleShapeColor },
        { "ShowcaseKeyItems", ShowcaseKeyItems },
        { "SetSpellCheckerOptions", SetSpellCheckerOptions },
        { "SetGrammarResultsOptions", SetGrammarResultsOptions },
        { "SetWindowSize", SetWindowSize },
        { "RemoveAllCustomTests", RemoveAllCustomTests },
        { "RemoveAllCustomTestBundles", RemoveAllCustomTestBundles },
        { "WriteToFile", WriteToFile },
        { "ExpandWordList", ExpandWordList },
        { "PhraseListToWordList", PhraseListToWordList },
        { "CrossReferencePhraseLists", CrossReferencePhraseLists },
        { "SplashScreen", SplashScreen },
        { "MergeWordLists", MergeWordLists },
        { "MergePhraseLists", MergePhraseLists },
        { "QAVerify", QAVerify },
        { "CheckHtmlLinks", CheckHtmlLinks },
        { "SetLeftPrintHeader", SetLeftPrintHeader },
        { "SetCenterPrintHeader", SetCenterPrintHeader },
        { "SetRightPrintHeader", SetRightPrintHeader },
        { "SetLeftPrintFooter", SetLeftPrintFooter },
        { "SetCenterPrintFooter", SetCenterPrintFooter },
        { "SetRightPrintFooter", SetRightPrintFooter },
        { "GetLeftPrintHeader", GetLeftPrintHeader },
        { "GetCenterPrintHeader", GetCenterPrintHeader },
        { "GetRightPrintHeader", GetRightPrintHeader },
        { "GetLeftPrintFooter", GetLeftPrintFooter },
        { "GetCenterPrintFooter", GetCenterPrintFooter },
        { "GetRightPrintFooter", GetRightPrintFooter },
        { "GetImageInfo", GetImageInfo },
        { "ApplyImageEffect", ApplyImageEffect },
        { "StitchImages", StitchImages },
        { nullptr, nullptr }
    };
    } // namespace LuaScripting

/** @}*/

#endif //__LUAAPPLICATION_H__
