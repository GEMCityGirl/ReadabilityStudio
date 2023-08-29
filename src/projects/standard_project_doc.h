/** @addtogroup Projects
    @brief Classes for readability projects.
    @date 2005-2023
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __PROJECT_DOC_H__
#define __PROJECT_DOC_H__

#include "base_project_doc.h"
#include "../app/readability_app.h"

/// @brief Standard project document.
class ProjectDoc final : public BaseProjectDoc
    {
public:
    /// @brief Constructor.
    ProjectDoc() = default;
    /// @private
    ProjectDoc(const ProjectDoc&) = delete;
    /// @private
    ProjectDoc& operator=(const ProjectDoc&) = delete;
    /// @private
    ~ProjectDoc()
        {
        wxDELETE(m_dupWordData);
        wxDELETE(m_misspelledWordData);
        wxDELETE(m_incorrectArticleData);
        wxDELETE(m_passiveVoiceData);
        wxDELETE(m_wordyPhraseData);
        wxDELETE(m_overusedWordsBySentenceData);
        wxDELETE(m_redundantPhraseData);
        wxDELETE(m_wordingErrorData);
        wxDELETE(m_clichePhraseData);
        wxDELETE(m_overlyLongSentenceData);
        wxDELETE(m_sentenceStartingWithConjunctionsData);
        wxDELETE(m_sentenceStartingWithLowercaseData);
        DeleteExcludedPhrases();
        }
    /// @private
    bool OnOpenDocument(const wxString& filename) final;
    /// @private
    bool OnNewDocument() final;
    /// @private
    bool OnSaveDocument(const wxString& filename) final;

    // refresh functions
    void RefreshProject() final;
    // only refresh the graphs, this assumes that no windows are being added or removed from the project
    void RefreshGraphs() final;
    // only refresh statistics reports
    void RefreshStatisticsReports() final;

    void ShowQueuedMessages() final;

    // Name is all that is really needed if we know that the test is already loaded globally
    std::vector<CustomReadabilityTestInterface>::iterator
        RemoveCustomReadabilityTest(const wxString& testName, const int Id) final;

    bool AddFryTest(const bool setFocus = true) final;
    bool AddRaygorTest(const bool setFocus = true) final;
    bool AddGilliamPenaMountainFryTest(const bool setFocus = true) final;
    bool AddFraseTest(const bool setFocus = true) final;
    bool AddSchwartzTest(const bool setFocus = true) final;
    bool AddDolchSightWords();

    void ExcludeAllCustomTestsTests() final;

    bool RunProjectWizard(const wxString& path);
    void RemoveMisspellings([[maybe_unused]] const wxArrayString& misspellingsToRemove) final;
    void DisplayReadabilityScores(const bool setFocus = true);
    void DisplayHighlightedText(const wxColour& highlightColor, const wxFont& textViewFont);
    [[nodiscard]]
    const ListCtrlExNumericDataProvider* GetMisspelledWordData() const noexcept
        { return m_misspelledWordData; }
    [[nodiscard]]
    const ListCtrlExNumericDataProvider* GetOverusedWordsBySentenceData() const noexcept
        { return m_overusedWordsBySentenceData; }
    [[nodiscard]]
    const ListCtrlExNumericDataProvider* GetPassiveVoiceData() const noexcept
        { return m_passiveVoiceData; }
private:
    [[nodiscard]]
    ListCtrlExNumericDataProvider* GetOverusedWordsBySentenceData() noexcept
        { return m_overusedWordsBySentenceData; }
    void CalculateGraphData();
    void AddDB2Plot(const bool setFocus = true) final;
    void AddFleschChart(const bool setFocus = true) final;
    void AddLixGermanGuage(const bool setFocus) final;
    void AddLixGauge(const bool setFocus = true) final;
    void AddCrawfordGraph(const bool setFocus = true) final;
    void LoadManuallyEnteredText();
    bool LoadProjectFile(const char* projectFileText, const size_t textLength);
    void LoadMetaFile(const wchar_t* settingsFileText);
    void DisplayStatistics();
    void DisplaySentencesBreakdown();
    void DisplayWordCharts();
    void DisplaySentenceCharts();
    void DisplayReadabilityGraphs();
    void DisplayWordsBreakdown();
    /// @brief Simply adds/removes word-list text windows when a word-list test is removed.
    /// @details This won't reformat anything, it just shows or hides a window if its
    ///     respective test is added or removed.
    void UpdateHighlightedTextWindows();
    void DisplayGrammar();
    void DisplayOverlyLongSentences();
    void DisplaySightWords();
    void SetReadabilityTestResult(const wxString& testId,
                                    const wxString& testName,
                                    const wxString& description,
                                    // score and display label
                                    const std::pair<double, wxString>& USGradeLevel,
                                    const wxString& readerAge,
                                    const double indexScore,
                                    const double clozeScore,
                                    const bool setFocus) final;
    // Text window formatting helpers
    //-------------------------------
    struct HighlighterColors
        {
        wxColour highlightColor;
        wxColour errorHighlightColor;
        wxColour styleHighlightColor;
        wxColour excludedTextHighlightColor;
        wxColour dolchConjunctionsTextHighlightColor;
        wxColour dolchPrepositionsTextHighlightColor;
        wxColour dolchPronounsTextHighlightColor;
        wxColour dolchAdverbsTextHighlightColor;
        wxColour dolchAdjectivesTextHighlightColor;
        wxColour dolchVerbsTextHighlightColor;
        wxColour dolchNounTextHighlightColor;
        };
    struct HighlighterTags
        {
        wxString HIGHLIGHT_BEGIN;
        wxString HIGHLIGHT_END;
        wxString ERROR_HIGHLIGHT_BEGIN;
        wxString PHRASE_HIGHLIGHT_BEGIN;
        wxString IGNORE_HIGHLIGHT_BEGIN;
        wxString DOLCH_CONJUNCTION_BEGIN;
        wxString DOLCH_PREPOSITIONS_BEGIN;
        wxString DOLCH_PRONOUN_BEGIN;
        wxString DOLCH_ADVERB_BEGIN;
        wxString DOLCH_ADJECTIVE_BEGIN;
        wxString DOLCH_VERB_BEGIN;
        wxString DOLCH_NOUN_BEGIN;
        wxString HIGHLIGHT_BEGIN_LEGEND;
        wxString HIGHLIGHT_END_LEGEND;
        wxString DUPLICATE_HIGHLIGHT_BEGIN_LEGEND;
        wxString PHRASE_HIGHLIGHT_BEGIN_LEGEND;
        wxString IGNORE_HIGHLIGHT_BEGIN_LEGEND;
        wxString DOLCH_CONJUNCTION_BEGIN_LEGEND;
        wxString DOLCH_PREPOSITIONS_BEGIN_LEGEND;
        wxString DOLCH_PRONOUN_BEGIN_LEGEND;
        wxString DOLCH_ADVERB_BEGIN_LEGEND;
        wxString DOLCH_ADJECTIVE_BEGIN_LEGEND;
        wxString DOLCH_VERB_BEGIN_LEGEND;
        wxString DOLCH_NOUN_BEGIN_LEGEND;
        wxString BOLD_BEGIN;
        wxString BOLD_END;
        wxString TAB_SYMBOL;
        wxString CRLF;
        };
    /// @brief Builds the colors and RTF color table (used by Windows and macOS).
    [[nodiscard]]
    HighlighterColors BuildReportColors(
        const wxColour& highlightColor, const wxFont& textViewFont,
        const wxColour& backgroundColor);
    /// @brief Builds the tags used to highlight words in RTF or Pango.
    /// @param highlightColor The default highlight color.
    /// @param highlighterColors Highlight colors used for the tags when
    ///     building for Pango (not used for RTF, since that uses indices into a color table).
    /// @returns The tags used to build RTF or Pango content.
    [[nodiscard]]
    HighlighterTags BuildHighlighterTags(const wxColour& highlightColor,
        [[maybe_unused]] HighlighterColors& highlighterColors);
    /// @brief Formats the main font for an RTF's header.
    std::pair<wxString, wxString> ProjectDoc::FormatRtfHeaderFont(
        const wxFont& textViewFont,
        const size_t mainFontColorIndex);
    /// @brief Builds the RTF color table (used by Windows and macOS).
    /// @returns RTF-formatted header sections (used by Windows and macOS).
    std::tuple<wxString, wxString, wxString> BuildColorTable(
        const wxColour& highlightColor, const wxFont& textViewFont,
        HighlighterColors& highlighterColors,
        const wxColour& backgroundColor);

    bool OnCreate(const wxString& path, long flags) final;

    ListCtrlExNumericDataProvider* m_dupWordData{ new ListCtrlExNumericDataProvider };
    ListCtrlExNumericDataProvider* m_misspelledWordData{ new ListCtrlExNumericDataProvider };
    ListCtrlExNumericDataProvider* m_incorrectArticleData{ new ListCtrlExNumericDataProvider };
    ListCtrlExNumericDataProvider* m_passiveVoiceData{ new ListCtrlExNumericDataProvider };
    ListCtrlExNumericDataProvider* m_wordyPhraseData{ new ListCtrlExNumericDataProvider };
    ListCtrlExNumericDataProvider* m_overusedWordsBySentenceData{ new ListCtrlExNumericDataProvider };
    ListCtrlExNumericDataProvider* m_clichePhraseData{ new ListCtrlExNumericDataProvider };
    ListCtrlExNumericDataProvider* m_redundantPhraseData{ new ListCtrlExNumericDataProvider };
    ListCtrlExNumericDataProvider* m_wordingErrorData{ new ListCtrlExNumericDataProvider };
    ListCtrlExNumericDataProvider* m_overlyLongSentenceData{ new ListCtrlExNumericDataProvider };
    ListCtrlExNumericDataProvider* m_sentenceStartingWithConjunctionsData{ new ListCtrlExNumericDataProvider };
    ListCtrlExNumericDataProvider* m_sentenceStartingWithLowercaseData{ new ListCtrlExNumericDataProvider };
    FormattedTextCtrl* m_dcTextWindow{ nullptr };
    FormattedTextCtrl* m_spacheTextWindow{ nullptr };
    FormattedTextCtrl* m_hjTextWindow{ nullptr };

    wxString GetSentenceWordCountsColumnName() const
        { return L"SENTENCE_WORD_COUNTS"; }
    wxString GetSentenceIndicesColumnName() const
        { return L"SENTENCE_INDICES"; }
    wxString GetSyllableCountsColumnName() const
        { return L"SYLLABLE_COUNTS"; }
    wxString GetGroupColumnName() const
        { return L"GROUP"; }

    std::shared_ptr<Wisteria::Data::Dataset> m_sentenceWordLengths
        { std::make_shared<Wisteria::Data::Dataset>() };
    std::shared_ptr<Wisteria::Data::Dataset> m_syllableCounts
        { std::make_shared<Wisteria::Data::Dataset>() };
public:
    wxDECLARE_DYNAMIC_CLASS(ProjectDoc);
    };

/** @}*/

#endif //__PROJECT_DOC_H__
