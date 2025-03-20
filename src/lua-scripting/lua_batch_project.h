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
                        // TRANSLATORS: %s is a function name that failed from a script
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

        BatchProject() = default;
        // Opens a project file.
        // File path to the project to open.
        explicit BatchProject(lua_State* L);
        BatchProject(const BatchProject&) = delete;
        BatchProject& operator=(const BatchProject&) = delete;

        void SetProject(BatchProjectDoc* doc) noexcept { m_project = doc; }

        // quneiform-suppress-begin
        // clang-format off
        // NOTE: these must all be single-line for the build script to properly create new topics from these.

        int /*string*/ GetTitle(lua_State* L); // Returns the title of the project.
        int SetWindowSize(lua_State* L /*number width, number height*/); // Sets the size of the project window.
        int DelayReloading(lua_State* L /*boolean delay*/); // Prevents a project from updating while settings are being changed. 
        // OPTIONS
        int SetSpellCheckerOptions(lua_State* L /*boolean ignoreProperNouns, boolean ignoreUppercased, boolean ignoreNumerals, boolean ignoreFileAddresses, boolean ignoreProgrammerCode, boolean ignoreSocialMediaTags, boolean allowColloquialisms*/); // Sets spell-checker options.

        // PROJECT SETTINGS
        int SetProjectLanguage(lua_State* L /*Language lang*/); // Sets the project language. This will affect syllable counting and also which tests are available.
        // cppcheck-suppress functionConst
        int /*Language*/ GetProjectLanguage(lua_State* L); // Returns the project's language.
        int SetReviewer(lua_State* L /*string reviewer*/); // Sets the user name for the software.
        int /*string*/ GetReviewer(lua_State* L); // Returns the reviewer's name.
        // cppcheck-suppress functionConst
        int SetStatus(lua_State* L /*string status*/); // Sets the status of the project. This can be freeform text.
        int /*string*/ GetStatus(lua_State* L); // Returns the status of the project.
        // cppcheck-suppress functionConst
        int /*TextStorage*/ GetDocumentStorageMethod(lua_State* L); // Returns whether the project embeds its documents or links to them.
        int SetDocumentStorageMethod(lua_State* L /*TextStorage storageMethod*/); // Sets whether the project embeds its documents or links to them.
        int SetParagraphsParsingMethod(lua_State* L /*ParagraphParse parseMethod*/); // Sets how hard returns help determine how paragraphs and sentences are detected.
        // cppcheck-suppress functionConst
        int /*ParagraphParse*/ GetParagraphsParsingMethod(lua_State* L); // Returns the method for how paragraphs are parsed.
        int SetMinDocWordCountForBatch(lua_State* L /*number wordCount*/); // Sets the minimum number of words a document must have to be included in the project.
        // cppcheck-suppress functionConst
        int /*number*/ GetMinDocWordCountForBatch(lua_State* L); // Returns the minimum number of words a document must have to be included in the project.
        int SetFilePathDisplayMode(lua_State* L /*FilePathDisplayMode displayMode*/); // Sets how filepaths are displayed.
        // cppcheck-suppress functionConst
        int /*FilePathDisplayMode*/ GetFilePathDisplayMode(lua_State* L); // Returns how filepaths are displayed for new batch projects.
        int SetAppendedDocumentFilePath(lua_State* L/*string filePath*/); // Sets the file path to the document being appended for analysis.
        int /*string*/ GetAppendedDocumentFilePath(lua_State* L); // Returns the file path to the document being appended for analysis.

        // TEXT EXCLUSION OPTIONS
        int SetTextExclusion(lua_State* L/*TextExclusionType exclusionType*/); // Specifies how text should be excluded while parsing the source document.
        int SetIncludeIncompleteTolerance(lua_State* L /*number minWordsForCompleteSentence*/); // Sets the incomplete-sentence tolerance. This is the minimum number of words that will make a sentence missing terminating punctuation be considered complete.
        int AggressivelyExclude(lua_State* L/*boolean beAggressive*/); // Specifies whether to use aggressive text exclusion.
        int ExcludeCopyrightNotices(lua_State* L/*boolean exclude*/); // Specifies whether or not to exclude copyright notices.
        int ExcludeTrailingCitations(lua_State* L/*boolean exclude*/); // Specifies whether or not to exclude trailing citations.
        int ExcludeFileAddress(lua_State* L/*boolean exclude*/); // Specifies whether or not to exclude file addresses.
        int ExcludeNumerals(lua_State* L/*boolean exclude*/); // Specifies whether or not to exclude numerals.
        int ExcludeProperNouns(lua_State* L/*boolean exclude*/); // Specifies whether or not to exclude proper nouns.
        int SetPhraseExclusionList(lua_State* L /*string exclusionListPath*/); // Sets the filepath to the phrase exclusion list.
        int SetBlockExclusionTags(lua_State* L /*string tagString*/); // Sets the text exclusion tags. This should be a two-character string containing a pair of exclusion tags.

        // GRAPH OPTIONS
        int SetGraphBackgroundColor(lua_State* L /*number red, number green, number blue*/); // Sets the graph background color.
        int ApplyGraphBackgroundFade(lua_State* L/*boolean useColorFade*/); // Sets whether to apply a fade to graph background colors.
        int SetGraphCommonImage(lua_State* L /*string imagePath*/); // Sets the common image drawn across all bars.
        int SetPlotBackgroundImage(lua_State* L /*string imagePath*/); // Sets the graph background (plot area) image.
        int SetPlotBackgroundImageEffect(lua_State* L /*ImageEffect imageEffect*/); // Sets the effect applied to an image when drawn as a graph's background.
        int SetPlotBackgroundImageFit(lua_State* L /*ImageFit fitType*/); // Specifies how to adjust an image to fit within a graph's background.
        int SetPlotBackgroundImageOpacity(lua_State* L /*number opacity*/); // Sets the graph background (plot area) image opacity.
        int SetPlotBackgroundColor(lua_State* L /*number red, number green, number blue*/); // Sets the graph background (plot area) color.
        int SetPlotBackgroundColorOpacity(lua_State* L /*number opacity*/); // Sets the graph background color opacity.
        int SetWatermark(lua_State* L /*string watermark*/); // Sets the watermark drawn on graphs.
        int SetGraphLogoImage(lua_State* L /*string imagePath*/); // Sets the logo image, shown in the bottom left corner.
        int SetStippleImage(lua_State* L /*string imagePath*/);// Sets the stipple image used to draw bars in graphs.
        int SetStippleShape(lua_State* L /*string shapeId*/); // Sets the stipple shape used to draw bars in graphs.
        int SetXAxisFont(lua_State* L /*string fontName, number pointSize, number fontWeight, string color*/); // Sets the font for the graphs' X axes.
        int SetYAxisFont(lua_State* L /*string fontName, number pointSize, number fontWeight, string color*/); // Sets the font for the graphs' Y axes.
        int SetGraphTopTitleFont(lua_State* L /*string fontName, number pointSize, number fontWeight, string color*/); // Sets the font for the graphs' top titles.
        int DisplayBarChartLabels(lua_State* L /*boolean display*/); // Specifies whether to display labels above each bar in a bar chart.
        int DisplayGraphDropShadows(lua_State* L /*bool displayShadows*/); // Sets whether to display shadows on graphs.
        int SetGraphInvalidRegionColor(lua_State* L /*number red, number green, number blue*/); // Sets the color for the invalid score regions for Fry-like graphs.
        int SetStippleShapeColor(lua_State* L /*number red, number green, number blue*/); // If using stipple shapes for bars, sets the color for certain shapes.

        int AddTest(lua_State* L/*Test test*/); // Adds a test to the project.
        int Reload(lua_State*); // Reanalyzes the project's document.
        int Close(lua_State* L /*boolean saveChanges*/); // Closes the project.
        int ExportAll(lua_State* L/*string outputFolder*/); // Exports all of the results from the project into a folder.
        int ExportList(lua_State* L /*ListType type, string outputFilePath, number fromRow, number toRow, number fromColumn, number toColumn, boolean includeHeaders, boolean includePageBreaks*/); // Saves the specified list as an HTML, text, or LaTeX file to *outputFilePath*.
        int ExportGraph(lua_State* L /*SideBarSection section, GraphType type, string outputFilePath,boolean grayScale, number width, number height*/); // Saves the specified graph as an image.
        int SelectWindow(lua_State* L /*SideBarSection section, number windowId*/); // Selects a window in the project.
        int ShowSidebar(lua_State* L /*boolean show*/); // Show or hides the project's sidebar.
        int SortList(lua_State* L /*ListType list, number columnToSort, SortOrder order, ...*/); // Sorts the specified list using a series of columns and sorting orders.

        // hidden functions just used for screenshots
        int /*INTERNAL!!!*/ OpenProperties(lua_State* L);
        int /*INTERNAL!!!*/ CloseProperties(lua_State*);
        // clang-format on
        // quneiform-suppress-end
        };
    } // namespace LuaScripting

/** @}*/

#endif //__LUA_BATCH_PROJECTS_H__
