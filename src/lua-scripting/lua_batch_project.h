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
        int DelayReloading(lua_State* L);
        // OPTIONS
        int SetSpellCheckerOptions(lua_State* L);

        // PROJECT SETTINGS
        int SetProjectLanguage(lua_State* L);
        // cppcheck-suppress functionConst
        int GetProjectLanguage(lua_State* L);
        int SetReviewer(lua_State* L);
        int GetReviewer(lua_State* L);
        // cppcheck-suppress functionConst
        int SetStatus(lua_State* L);
        // cppcheck-suppress functionConst
        int GetDocumentStorageMethod(lua_State* L);
        int SetDocumentStorageMethod(lua_State* L);
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
        // Specifies how text should be excluded.
        int SetTextExclusion(lua_State* L);
        // Specifies the minimum number of words that will make a sentence missing
        // terminating punctuation be considered complete.
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
        /// Sets the tags to exclude blocks of text.
        int SetBlockExclusionTags(lua_State* L);

        /// Sets the file path to the document being appended for analysis.
        int SetAppendedDocumentFilePath(lua_State* L);

        // GRAPH OPTIONS
        int SetGraphBackgroundColor(lua_State* L);
        int ApplyGraphBackgroundFade(lua_State* L);
        int SetGraphCommonImage(lua_State* L);
        int SetPlotBackgroundImage(lua_State* L);
        int SetPlotBackgroundImageEffect(lua_State* L);
        int SetPlotBackgroundImageFit(lua_State* L);
        int SetPlotBackgroundImageOpacity(lua_State* L);
        int SetPlotBackgroundColor(lua_State* L);
        int SetPlotBackgroundColorOpacity(lua_State* L);
        int SetGraphWatermark(lua_State* L);
        int SetGraphLogoImage(lua_State* L);
        int SetStippleImage(lua_State* L);
        int SetStippleShape(lua_State* L);
        int SetXAxisFont(lua_State* L);
        int SetYAxisFont(lua_State* L);
        int SetGraphTopTitleFont(lua_State* L);
        int DisplayBarChartLabels(lua_State* L);
        int DisplayGraphDropShadows(lua_State* L);
        int SetGraphInvalidRegionColor(lua_State* L);
        int SetStippleShapeColor(lua_State* L);

        // Adds a test to the project.
        // TestName The name of the test to add to the project.
        int AddTest(lua_State* L);
        // Reanalyzes the documents.
        int Reload(lua_State*);
        // Closes the project.
        // SaveChanges Specifies whether to save any changes made to the project before closing it.
        // Default is to not save any changes.
        int Close(lua_State* L);
        // Exports all of the results from the project into a folder.
        // FolderPath The folder to save the project's results.
        int ExportAll(lua_State* L);
        // Saves a list from the project.
        // ListType Which list to save.
        // FilePath The file path to save the list.
        int ExportList(lua_State* L);
        // Saves a graph from the project as an image.
        // SideBarSection The section that the graph is in.
        // GraphId Which graph to save.
        // FilePath The file path to save the graph.
        // GrayScale Whether to save the image in black & white.
        // Width The width of the output image.
        // Height The height of the output image.
        int ExportGraph(lua_State* L);
        /*Selects a window in the project.*/
        int SelectWindow(lua_State*);
        int ShowSidebar(lua_State* L);
        /*Selects and sorts a list.
        WindowToSelect The list window to sort.
        ColumnToSort The column in the list to sort.
        Order The order to sort.*/
        int SortList(lua_State* L);
        // hidden functions just used for screenshots
        int OpenProperties(lua_State* L);
        int CloseProperties(lua_State*);
        };
    } // namespace LuaScripting

/** @}*/

#endif //__LUA_BATCH_PROJECTS_H__
