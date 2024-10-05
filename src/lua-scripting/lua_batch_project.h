/** @addtogroup Scripting
    @brief Classes for the scripting.
    @date 2005-2023
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __LUA_BATCH_PROJECTS_H__
#define __LUA_BATCH_PROJECTS_H__

#include "lua_debug.h"
#include "luna.h"
#include <wx/wx.h>

class ProjectDoc;
class BatchProjectDoc;
class ToolsOptionsDlg;

namespace LuaScripting
    {
    /// @brief Batch project interface.
    /// @private
    /// @internal This is documented in the readability-studio-api help.
    class BatchProject
        {
        BatchProjectDoc* m_project{ nullptr };
        ToolsOptionsDlg* m_settingsDlg{ nullptr };

        bool VerifyProjectIsOpen(const wxString& functionName) const
            {
            if (!m_project)
                {
                wxMessageBox(wxString::Format(_(L"%s: Accessing project that is already closed."),
                                              functionName),
                             _(L"Warning"), wxOK | wxICON_INFORMATION);
                return false;
                }
            else
                {
                return true;
                }
            }

        /// @note This takes into account the boolean parameter in the front
        ///     passed to Lunar objects, so @c minParemeterCount should be the actual
        ///     expected number of parameters.
        [[nodiscard]]
        static bool VerifyParameterCount(lua_State* L, const int minParemeterCount,
                                         const wxString& functionName)
            {
            assert(L);
            assert(minParemeterCount >= 0);
            // skip over the boolean param passed in the front indicating success of
            // routing the function to the class object.
            if ((lua_gettop(L) - 1) < minParemeterCount)
                {
                wxMessageBox(
                    wxString::Format(
                        _(L"%s: Invalid number of arguments.\n\n%d expected, %d provided."),
                        functionName, minParemeterCount, (lua_gettop(L) - 1)),
                    _(L"Script Error"), wxOK | wxICON_EXCLAMATION);
                return false;
                }
            else
                {
                return true;
                }
            }

        bool ReloadIfNotDelayed();
        bool ReloadIfNotDelayedSimple();
        bool m_delayReloading{ false };

      public:
        static const char className[];
        static Luna<BatchProject>::FunctionType methods[];
        static Luna<BatchProject>::PropertyType properties[];

        explicit BatchProject(lua_State* L);

        void SetProject(BatchProjectDoc* doc) noexcept { m_project = doc; }

        int GetTitle(lua_State* L);
        int SetWindowSize(lua_State* L);
        int DelayReloading(lua_State* L/*boolean delay*/); // Prevents a project from updating while settings are being changed. 
        // OPTIONS
        int SetSpellCheckerOptions(lua_State* L);

        // PROJECT SETTINGS
        int SetProjectLanguage(lua_State* L);
        // cppcheck-suppress functionConst
        int /*Language*/ GetProjectLanguage(lua_State* L); // Returns the project's language.
        int SetReviewer(lua_State* L /*string reviewer*/); // Sets the user name for the software.
        int /*string*/ GetReviewer(lua_State* L); // Returns the reviewer's name.
        // cppcheck-suppress functionConst
        int SetStatus(lua_State* L);
        // cppcheck-suppress functionConst
        int /*TextStorage*/ GetDocumentStorageMethod(lua_State* L); // Returns the default method for how documents are stored for new projects.
        int SetDocumentStorageMethod(lua_State* L /*TextStorage storageMethod*/); // Sets the default method for how documents are stored for new projects.
        int SetParagraphsParsingMethod(lua_State* L);
        // cppcheck-suppress functionConst
        int GetParagraphsParsingMethod(lua_State* L);
        int SetMinDocWordCountForBatch(lua_State* L);
        // cppcheck-suppress functionConst
        int GetMinDocWordCountForBatch(lua_State* L);
        int SetFilePathDisplayMode(lua_State* L);
        // cppcheck-suppress functionConst
        int GetFilePathDisplayMode(lua_State* L);
        // TEXT EXCLUSION OPTIONS
        int SetTextExclusion(lua_State* L/*TextExclusionType exclusionType*/); // Specifies how text should be excluded while parsing the source document.
        // Specifies the minimum number of words that will make a sentence missing
        // terminating punctuation be considered complete.
        int SetIncludeIncompleteTolerance(lua_State* L);
        int AggressivelyExclude(lua_State* L/*boolean beAggressive*/); // Specifies whether to use aggressive text exclusion.
        int ExcludeCopyrightNotices(lua_State* L/*boolean exclude*/); // Specifies whether or not to exclude copyright notices.
        int ExcludeTrailingCitations(lua_State* L/*boolean exclude*/); // Specifies whether or not to exclude trailing citations.
        int ExcludeFileAddress(lua_State* L/*boolean exclude*/); // Specifies whether or not to exclude file addresses.
        int ExcludeNumerals(lua_State* L/*boolean exclude*/); // Specifies whether or not to exclude numerals.
        int ExcludeProperNouns(lua_State* L/*boolean exclude*/); // Specifies whether or not to exclude proper nouns.
        // Sets the filepath to the phrase exclusion list
        // filePath Path to phrase exclusion list
        int SetPhraseExclusionList(lua_State* L);
        int SetBlockExclusionTags(lua_State* L /*string tagString*/); // Sets the text exclusion tags for new projects. This should be a two-character string containing a pair of exclusion tags.
        int SetAppendedDocumentFilePath(lua_State* L/*string filePath*/); // Sets the file path to the document being appended for analysis.

        // GRAPH OPTIONS
        int SetGraphBackgroundColor(lua_State* L);
        int ApplyGraphBackgroundFade(lua_State* L/*boolean useColorFade*/); // Sets whether to apply a fade to graph background colors.
        int SetGraphCommonImage(lua_State* L);
        int SetPlotBackgroundImage(lua_State* L);
        int SetPlotBackgroundImageEffect(lua_State* L);
        int SetPlotBackgroundImageFit(lua_State* L);
        int SetPlotBackgroundImageOpacity(lua_State* L);
        int SetPlotBackgroundColor(lua_State* L);
        int SetPlotBackgroundColorOpacity(lua_State* L);
        int SetWatermark(lua_State* L);
        int SetGraphLogoImage(lua_State* L);
        int SetStippleImage(lua_State* L);
        int SetStippleShape(lua_State* L);
        int SetXAxisFont(lua_State* L);
        int SetYAxisFont(lua_State* L);
        int SetGraphTopTitleFont(lua_State* L);
        int DisplayBarChartLabels(lua_State* L);
        int DisplayGraphDropShadows(lua_State* L /*bool displayShadows*/); // Sets whether to display shadows on graphs
        int SetGraphInvalidRegionColor(lua_State* L);
        int SetStippleShapeColor(lua_State* L);

        int AddTest(lua_State* L/*Test test*/); // Adds a test to the project.
        int Reload(lua_State*); // Reanalyzes the project's document.
        int Close(lua_State* L /*boolean saveChanges*/); // Closes the project.
        int ExportAll(lua_State* L/*string outputFolder*/); // Exports all of the results from the project into a folder.
        int ExportList(lua_State* L /*ListType type, string outputFilePath, number fromRow, number toRow, number fromColumn, number toColumn, boolean includeHeaders, boolean IncludePageBreaks*/); // Saves the specified list as an HTML, text, or LaTeX file to *OutputFilePath*.
        int ExportGraph(lua_State* L /*SideBarSection section, GraphType type, string outputFilePath,boolean grayScale, number width, number height*/); // Saves the specified graph as an image.
        /*Selects a window in the project.*/
        int SelectWindow(lua_State*);
        int ShowSidebar(lua_State* L /*boolean show*/); // Show or hides the project's sidebar.
        int SortList(lua_State* L /*ListType list, number columnToSort, SortOrder order, ...*/); // Sorts the specified list using a series of columns and sorting orders.

        // hidden functions just used for screenshots
        int /*INTERNAL!!!*/ OpenProperties(lua_State* L);
        int /*INTERNAL!!!*/ CloseProperties(lua_State*);
        };
    } // namespace LuaScripting

/** @}*/

#endif //__LUA_BATCH_PROJECTS_H__
