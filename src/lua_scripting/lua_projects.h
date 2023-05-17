/** @addtogroup Scripting
    @brief Classes for the scripting.
    @date 2005-2023
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __LUAPROJECTS_H__
#define __LUAPROJECTS_H__

#include <wx/wx.h>
#include "luna.h"
#include "lua_debug.h"

class ProjectDoc;
class BatchProjectDoc;
class ToolsOptionsDlg;

namespace LuaScripting
    {
    /// @brief Standard project interface.
    /// @private
    /// @internal This is documented in the ReadabilityStudioAPI help.
    class StandardProject
        {
        ProjectDoc* m_project{ nullptr };
        ToolsOptionsDlg* m_settingsDlg{ nullptr };
        bool VerifyProjectIsOpen(const wxString& functionName) const
            {
            if (!m_project)
                {
                wxMessageBox(
                    wxString::Format(_(L"%s: Accessing project that is already closed."), functionName),
                    _(L"Warning"), wxOK|wxICON_INFORMATION);
                return false;
                }
            else
                { return true; }
            }
        /// @note This takes into account the boolean parameter in the front
        ///     passed to Lunar objects, so @c minParemeterCount should be the actual
        ///     expected number of parameters.
        bool VerifyParameterCount(lua_State* L, const int minParemeterCount,
                                  const wxString& functionName)
            {
            wxASSERT(L);
            wxASSERT(minParemeterCount >= 0);
            // skip over the boolean param passed in the front indicating success of
            // routing the function to the class object.
            if ((lua_gettop(L) - 1) < minParemeterCount)
                {
                wxMessageBox(wxString::Format(
                    _(L"%s: Invalid number of arguments.\n\n%d expected, %d provided."),
                        functionName, minParemeterCount, (lua_gettop(L) - 1)),
                    _(L"Script Error"), wxOK|wxICON_EXCLAMATION);
                return false;
                }
            else
                { return true; }
            }
        bool ReloadIfNotDelayed();
        bool ReloadIfNotDelayedSimple();
        bool m_delayReloading{ false };
    public:
        StandardProject() = default;
        void SetProject(ProjectDoc* doc) noexcept
            { m_project = doc; }
        static const char className[];
        static Luna<StandardProject>::FunctionType methods[];
        static Luna<StandardProject>::PropertyType properties[];

        int DelayReloading(lua_State* L);
        // Opens a project file.
        // FilePath File path to the project to open.
        explicit StandardProject(lua_State *L);
        int GetTitle(lua_State* L);
        int SetWindowSize(lua_State* L);
        // Stats functions
        int GetSentenceCount(lua_State* L);
        int GetIndependentClauseCount(lua_State* L);
        int GetNumeralCount(lua_State* L);
        int GetProperNounCount(lua_State* L);
        int GetUniqueWordCount(lua_State* L);
        int GetWordCount(lua_State* L);
        int GetCharacterAndPunctuationCount(lua_State* L);
        int GetCharacterCount(lua_State* L);
        int GetSyllableCount(lua_State* L);
        int GetUnique3SyllablePlusWordCount(lua_State* L);
        int Get3SyllablePlusWordCount(lua_State* L);
        int GetUnique1SyllableWordCount(lua_State* L);
        int Get1SyllableWordCount(lua_State* L);
        int Get7CharacterPlusWordCount(lua_State* L);
        int GetUnique6CharPlusWordCount(lua_State* L);
        int Get6CharacterPlusWordCount(lua_State* L);
        int GetUnfamSpacheWordCount(lua_State* L);
        int GetUnfamDCWordCount(lua_State* L);
        int GetUnfamHJWordCount(lua_State* L);
        // OPTIONS
        int SetSpellCheckerOptions(lua_State *L);

        // PROJECT SETTINGS
        int SetProjectLanguage(lua_State *L);
        // cppcheck-suppress functionConst
        int GetProjectLanguage(lua_State *L);
        int SetReviewer(lua_State *L);
        // cppcheck-suppress functionConst
        int GetReviewer(lua_State *L);
        // cppcheck-suppress functionConst
        int GetDocumentStorageMethod(lua_State *L);
        int SetDocumentStorageMethod(lua_State *L);
        int SetParagraphsParsingMethod(lua_State *L);
        // cppcheck-suppress functionConst
        int GetParagraphsParsingMethod(lua_State *L);
        // TEXT EXCLUSION OPTIONS
        /// Specifies how text should be excluded.
        int SetTextExclusion(lua_State *L);
        /// Specifies the minimum number of words that will make a sentence
        /// missing terminating punctuation be considered complete.
        int SetIncludeIncompleteTolerance(lua_State *L);
        /// Specifies whether or not to aggressively exclude.
        int AggressivelyDeduceLists(lua_State *L);
        /// Specifies whether or not to exclude copyright notices.
        int ExcludeCopyrightNotices(lua_State *L);
        /// Specifies whether or not to exclude trailing citations.
        int ExcludeTrailingCitations(lua_State *L);
        /// Specifies whether or not to exclude file addresses.
        int ExcludeFileAddress(lua_State *L);
        /// Specifies whether or not to exclude numerals.
        int ExcludeNumerals(lua_State *L);
        /// Specifies whether or not to exclude proper nouns.
        int ExcludeProperNouns(lua_State *L);
        /// Sets the filepath to the phrase exclusion list
        /// filePath Path to phrase exclusion list.
        int SetPhraseExclusionList(lua_State *L);
        /// Sets the tags to exclude blocks of text.
        int SetBlockExclusionTags(lua_State *L);

        /// Sets the file path to the document being appended for analysis.
        int SetAppendedDocumentFilePath(lua_State *L);

        // GRAPH OPTIONS
        int SetGraphBackgroundColor(lua_State *L);
        int ApplyGraphBackgroundFade(lua_State *L);
        int SetGraphBackgroundImage(lua_State *L);
        int SetGraphBackgroundOpacity(lua_State *L);
        int SetPlotBackgroundColor(lua_State *L);
        int SetPlotBackgroundOpacity(lua_State *L);
        int SetGraphWatermark(lua_State *L);
        int SetGraphLogoImage(lua_State *L);
        int SetGraphCustomBrushImage(lua_State *L);
        int DisplayGraphDropShadows(lua_State *L);
        int SetBarChartBarColor(lua_State *L);
        int SetBarChartBarOpacity(lua_State *L);
        int SetBarChartBarEffect(lua_State *L);
        int SetBarChartOrientation(lua_State *L);

        // TEST OPTIONS
        // Adds a test to the project.
        // TestName The name of the test to add to the project.
        int AddTest(lua_State *L);
        // Reanalyzes the documents.
        int Reload(lua_State*);
        // Closes the project.
        // SaveChanges Specifies whether to save any changes made to the project before closing it.
        // Default is to not save any changes.
        int Close(lua_State *L);
        // Exports all of the results from the project into a folder.
        // FolderPath The folder to save the project's results.
        int ExportAll(lua_State *L);
        // Saves a graph from the project as an image.
        // GraphType Which graph to save. Available types are: GraphsTypes.WordBarChart,
        // GraphsTypes.Fry, GraphsTypes.GpmFry, GraphsTypes.FRASE, GraphsTypes.Raygor,
        // GraphsTypes.DolchCoverageChart, GraphsTypes.DolchWordBarChart, GraphsTypes.Flesch.
        // FilePath The file path to save the graph.
        // GrayScale Whether to save the image in black & white.
        // Width The width of the output image.
        // Height The height of the output image.
        int ExportGraph(lua_State *L);
        // Saves the scores to an HTML file.
        // FilePath The file path to save the scores.
        int ExportScores(lua_State *L);
        // Saves a highlighted words report from the project.
        // HighlightedReportType Which report to save.
        // FilePath The file path to save the report.
        int ExportHighlightedWords(lua_State *L);
        // Saves a summary report from the project.
        // ReportType Which report to save.
        // FilePath The file path to save the report.
        int ExportReport(lua_State *L);
        // Saves a list from the project.
        // ListType Which list to save.
        // FilePath The file path to save the list.
        int ExportList(lua_State *L);
        // Saves a copy of the project's document with excluded text (and other optional items) filtered out.
        int ExportFilteredText(lua_State *L);
        /*Sorts a list.
        ListToSort The list window to sort. Refer to ListTypes enumeration.
        ColumnToSort The column in the list to sort.
        Order The order to sort.*/
        int SortList(lua_State *L);
        /*Sorts a graph.
        GraphToSort The graph window to sort. Refer to GraphTypes enumeration.
        Order The order to sort.*/
        int SortGraph(lua_State *L);
        /*Selects the specified section and subwindow.
        Section The section to select.
        Window The subwindow in the section to select.*/
        int SelectWindow(lua_State *L);
        int ShowSidebar(lua_State *L);

        // HIDDEN interfaces for testing and screenshots
        /*Selects and sorts a list in the Words Breakdown section.
        WindowToSelect The list window to select items in. Refer to ListType enumeration.
        RowsToSelect Rows to select.*/
        int SelectRowsInWordsBreakdownList(lua_State *L);
        /*Selects a range of text in a text window.
        WindowToSelect The text window to select. Refer to HighlightedReportType enumeration.
        StartPosition Character position to begin selection.
        EndPosition Character position to end selection.*/
        int SelectHighlightedWordReport(lua_State *L);
        /*Selects a range of text in the text window in the Grammar section.
        StartPosition Character position to begin selection.
        EndPosition Character position to end selection.*/
        int SelectTextGrammarWindow(lua_State *L);
        /*Selects and scrolls down a text window.
        WindowToSelect The text window to select. Refer to HighlightedReportType enumeration.
        Position Character position to scroll into view.*/
        int ScrollTextWindow(lua_State *L);
        /*Selects the Readability Results section of the project and highlights a test by index.
        TestToSelect The test to select, based on position in the list.*/
        int SelectReadabilityTest(lua_State *L);
        // Opens the properties dialog and the specified page
        int OpenProperties(lua_State *L);
        int CloseProperties(lua_State*);
        };

    /// @brief Batch project interface.
    /// @private
    /// @internal This is documented in the ReadabilityStudioAPI help.
    class BatchProject
        {
        BatchProjectDoc* m_project{ nullptr };
        ToolsOptionsDlg* m_settingsDlg{ nullptr };
        bool VerifyProjectIsOpen(const wxString& functionName) const
            {
            if (!m_project)
                {
                wxMessageBox(
                    wxString::Format(_(L"%s: Accessing project that is already closed."), functionName),
                    _(L"Warning"), wxOK|wxICON_INFORMATION);
                return false;
                }
            else
                { return true; }
            }
        /// @note This takes into account the boolean parameter in the front
        ///     passed to Lunar objects, so @c minParemeterCount should be the actual
        ///     expected number of parameters.
        bool VerifyParameterCount(lua_State* L, const int minParemeterCount,
                                  const wxString& functionName)
            {
            wxASSERT(L);
            wxASSERT(minParemeterCount >= 0);
            // skip over the boolean param passed in the front indicating success of
            // routing the function to the class object.
            if ((lua_gettop(L) - 1) < minParemeterCount)
                {
                wxMessageBox(wxString::Format(
                    _(L"%s: Invalid number of arguments.\n\n%d expected, %d provided."),
                        functionName, minParemeterCount, (lua_gettop(L) - 1)),
                    _(L"Script Error"), wxOK|wxICON_EXCLAMATION);
                return false;
                }
            else
                { return true; }
            }
        bool ReloadIfNotDelayed();
        bool ReloadIfNotDelayedSimple();
        bool m_delayReloading{ false };
    public:
        static const char className[];
        static Luna<BatchProject>::FunctionType methods[];
        static Luna<BatchProject>::PropertyType properties[];

        explicit BatchProject(lua_State *L);
        void SetProject(BatchProjectDoc* doc) noexcept
            { m_project = doc; }
        int GetTitle(lua_State *L);
        int SetWindowSize(lua_State* L);
        int DelayReloading(lua_State* L);
        // OPTIONS
        int SetSpellCheckerOptions(lua_State *L);

        // PROJECT SETTINGS
        int SetProjectLanguage(lua_State *L);
        // cppcheck-suppress functionConst
        int GetProjectLanguage(lua_State *L);
        int SetReviewer(lua_State *L);
        // cppcheck-suppress functionConst
        int GetReviewer(lua_State *L);
        // cppcheck-suppress functionConst
        int GetDocumentStorageMethod(lua_State *L);
        int SetDocumentStorageMethod(lua_State *L);
        int SetParagraphsParsingMethod(lua_State *L);
        // cppcheck-suppress functionConst
        int GetParagraphsParsingMethod(lua_State *L);
        int SetMinDocWordCountForBatch(lua_State *L);
        // cppcheck-suppress functionConst
        int GetMinDocWordCountForBatch(lua_State *L);
        int SetFilePathDisplayMode(lua_State *L);
        // cppcheck-suppress functionConst
        int GetFilePathDisplayMode(lua_State *L);
        // TEXT EXCLUSION OPTIONS
        // Specifies how text should be excluded.
        int SetTextExclusion(lua_State *L);
        // Specifies the minimum number of words that will make a sentence missing
        // terminating punctuation be considered complete.
        int SetIncludeIncompleteTolerance(lua_State *L);
        // Specifies whether or not to aggressively exclude.
        int AggressivelyDeduceLists(lua_State *L);
        // Specifies whether or not to exclude copyright notices.
        int ExcludeCopyrightNotices(lua_State *L);
        // Specifies whether or not to exclude trailing citations.
        int ExcludeTrailingCitations(lua_State *L);
        // Specifies whether or not to exclude file addresses.
        int ExcludeFileAddress(lua_State *L);
        // Specifies whether or not to exclude numerals.
        int ExcludeNumerals(lua_State *L);
        // Specifies whether or not to exclude proper nouns.
        int ExcludeProperNouns(lua_State *L);
        // Sets the filepath to the phrase exclusion list
        // filePath Path to phrase exclusion list
        int SetPhraseExclusionList(lua_State *L);
        /// Sets the tags to exclude blocks of text.
        int SetBlockExclusionTags(lua_State *L);

        /// Sets the file path to the document being appended for analysis.
        int SetAppendedDocumentFilePath(lua_State *L);

        // GRAPH OPTIONS
        int SetGraphBackgroundColor(lua_State *L);
        int ApplyGraphBackgroundFade(lua_State *L);
        int SetGraphBackgroundImage(lua_State *L);
        int SetGraphBackgroundOpacity(lua_State *L);
        int SetPlotBackgroundColor(lua_State *L);
        int SetPlotBackgroundOpacity(lua_State *L);
        int SetGraphWatermark(lua_State *L);
        int SetGraphLogoImage(lua_State *L);
        int SetGraphCustomBrushImage(lua_State *L);
        int DisplayGraphDropShadows(lua_State *L);

        // Adds a test to the project.
        // TestName The name of the test to add to the project.
        int AddTest(lua_State *L);
        // Reanalyzes the documents.
        int Reload(lua_State*);
        // Closes the project.
        // SaveChanges Specifies whether to save any changes made to the project before closing it.
        // Default is to not save any changes.
        int Close(lua_State *L);
        // Exports all of the results from the project into a folder.
        // FolderPath The folder to save the project's results.
        int ExportAll(lua_State *L);
        // Saves a list from the project.
        // ListType Which list to save.
        // FilePath The file path to save the list.
        int ExportList(lua_State *L);
        // Saves a graph from the project as an image.
        // SideBarSection The section that the graph is in.
        // GraphId Which graph to save.
        // FilePath The file path to save the graph.
        // GrayScale Whether to save the image in black & white.
        // Width The width of the output image.
        // Height The height of the output image.
        int ExportGraph(lua_State *L);
        /*Selects a window in the project.*/
        int SelectWindow(lua_State*);
        int ShowSidebar(lua_State *L);
        /*Selects and sorts a list.
        WindowToSelect The list window to sort.
        ColumnToSort The column in the list to sort.
        Order The order to sort.*/
        int SortList(lua_State *L);
        // hidden functions just used for screenshots
        int OpenProperties(lua_State *L);
        int CloseProperties(lua_State*);
        };
    }

/** @}*/

#endif //__LUAPROJECTS_H__
