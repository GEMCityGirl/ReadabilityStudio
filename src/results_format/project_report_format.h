/** @addtogroup Formatting
    @brief Classes for parsing and formatting results output.
    @date 2005-2023
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __PROJECT_REPORT_FORMAT_H__
#define __PROJECT_REPORT_FORMAT_H__

#include "../projects/base_project.h"
#include "../Wisteria-Dataviz/src/import/html_encode.h"

/// @brief Class to format a project's information into reports.
class ProjectReportFormat
    {
public:
    /** @returns HTML text with its trailing &lt;br /&gt; removed.
        @param text The HTML text to trim.*/
    [[nodiscard]]
    static wxString TrimTrailingBreaks(const wxString& text);
    /// @returns The html/head/body start sections for a report.
    /// @param bgColor The color for the page. (Only used for theming under Windows.)
    /// @param textColor The text color for the entire page.
    ///     (Only used for theming under Windows.)
    /// @param title The page's title.
    /// @note This will include various (internal) CSS styling necessary for the HTML.
    [[nodiscard]]
    static wxString FormatHtmlReportStart(
        [[maybe_unused]] const wxColour bgColor,
        [[maybe_unused]] const wxColour textColor,
        const wxString& title = wxEmptyString);
    /// @returns The html/head/body start sections for a report.
    [[nodiscard]]
    static wxString FormatHtmlReportEnd();
    /** @brief Formats a message into a fancy "Note" tip (in the form of an HTML table).
        @param note The message to format.
        @returns The note formatted in HTML.*/
    [[nodiscard]]
    static wxString FormatHtmlNoteSection(const wxString& note);
    /** @brief Formats a message into a fancy "Warning" (in the form of an HTML table).
        @param note The message to format.
        @returns The note formatted in HTML.*/
    [[nodiscard]]
    static wxString FormatHtmlWarningSection(const wxString& note);
    /** @returns A test's results, description, and notes formatted into an HTML table.
        @param score The calculated score.
        @param theTest The test to format.
        @param note An optional note to include.*/
    [[nodiscard]]
    static wxString FormatTestResult(const wxString& score,
                                     const readability::readability_test& theTest,
                                     const wxString& note = wxEmptyString);
    /** @returns Project statistics information from a project.
        @param project The project to analyze statistics from.
        @param attentionColor Color to highlight important details.
        @param[out] listData An optional data grid to store tabular results.\n
            Pass in null to ignore this parameter.*/
    [[nodiscard]]
    static wxString FormatStatisticsInfo(const BaseProject* project,
                                         const wxColour attentionColor,
                                         ListCtrlExDataProviderBase* listData);
    /** @returns Dolch statistics information from a project.
        @param project The project to analyze Dolch statistics from.
        @param includeExplanation True to include detailed explanations in the report.
        @param attentionColor Color to highlight important details.
        @param[out] listData An optional data grid to store tabular results.\n
               Pass in null to ignore this parameter.\n
               Caller is responsible for clearing this data grid before calling this function
               because rows will be appended to what was already in there.
               This is because FormatStatisticsInfo() uses this function to append\n
               Dolch statistics to its own @c listData argument.*/
    [[nodiscard]]
    static wxString FormatDolchStatisticsInfo(const BaseProject* project,
                                              const bool includeExplanation,
                                              const wxColour attentionColor,
                                              ListCtrlExDataProviderBase* listData);
    /** @brief Formats a full sentence from a project's sentence information structure.
        @param project The project containing the sentence and words.
        @param sentence The iterator to the sentence structure.
        @param[out] punctStart The current position of the punctuation info for the document.
            This is updated (i.e., moved) by this function so that in the next call it can
            start off from the same place. This prevents having to scan through the entire
            collection of punctuations to "catch up" to the current sentence every time
            this function needs to be called.
        @param punctEnd The end of the punctuation info.
        @returns The formatted sentence.*/
    [[nodiscard]]
    static wxString FormatSentence(
        const BaseProject* project,
        const grammar::sentence_info& sentence,
        std::vector<punctuation::punctuation_mark>::const_iterator& punctStart,
        const std::vector<punctuation::punctuation_mark>::const_iterator& punctEnd);
    /// @returns The header color for a report table.
    [[nodiscard]]
    static wxColour GetReportHeaderColor()
        { return wxColour(L"#C3D7D7"); }
    /// @returns The header color for a report table.
    [[nodiscard]]
    static wxColour GetReportHeaderFontColor()
        { return wxColour(L"#000000"); }
    /** @returns A formula formatted into HTML.
        @param formula The formula to format.*/
    [[nodiscard]]
    static wxString FormatFormulaToHtml(const wxString& formula);
    /** @returns The display name of a stemming language (e.g., English).
        @param stemType The stemmer to review.*/
    [[nodiscard]]
    static wxString GetStemmingDisplayName(const stemming::stemming_type stemType);
    /// maximum number of summary statistics
    static constexpr int MAX_SUMMARY_STAT_ROWS = 500;
    static constexpr int MAX_DOLCH_CONJUNCTION_WORDS = 6;
    static constexpr int MAX_DOLCH_PREPOSITION_WORDS = 16;
    static constexpr int MAX_DOLCH_PRONOUN_WORDS = 26;
    static constexpr int MAX_DOLCH_ADVERB_WORDS = 34;
    static constexpr int MAX_DOLCH_ADJECTIVE_WORDS = 46;
    static constexpr int MAX_DOLCH_VERBS = 92;
    /// should be 95, but we count "Santa" and "Claus" as two words instead of one.
    static constexpr int MAX_DOLCH_NOUNS = 96;
private:
    [[nodiscard]]
    static wxColour GetReportNoteHeaderColor()
        { return wxColour(L"#55A8E6"); }
    /** @returns A test's factors formatted into an HTML table.
        @param test The test to format.*/
    [[nodiscard]]
    static wxString FormatTestFactors(const readability::readability_test& test);
    };

/** @}*/

#endif //__PROJECT_REPORT_FORMAT_H__
