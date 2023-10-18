#ifndef __BASE_PROJECT_VIEW_H__
#define __BASE_PROJECT_VIEW_H__

#include <wx/wx.h>
#include <wx/docmdi.h>
#include <wx/aui/aui.h>
#include <wx/filename.h>
#include <wx/toolbar.h>
#include <wx/splitter.h>
#include <wx/html/htmlwin.h>
#include <wx/html/htmlcell.h>
#include <wx/fdrepdlg.h>
#include <wx/string.h>
#include <wx/mimetype.h>
#include <wx/ribbon/bar.h>
#include <wx/ribbon/buttonbar.h>
#include <wx/ribbon/gallery.h>
#include <wx/ribbon/toolbar.h>
#include <wx/richmsgdlg.h>
#include "wx/taskbarbutton.h"
#include "../results_format/readability_messages.h"
#include "../ui/controls/explanationlistctrl.h"
#include "../Wisteria-Dataviz/src/ui/controls/formattedtextctrl.h"
#include "../Wisteria-Dataviz/src/util/formulaformat.h"
#include "../Wisteria-Dataviz/src/ui/controls/listctrlex.h"
#include "../Wisteria-Dataviz/src/ui/controls/infobarex.h"
#include "../Wisteria-Dataviz/src/ui/controls/sidebar.h"
#include "../Wisteria-Dataviz/src/ui/controls/searchpanel.h"
#include "../Wisteria-Dataviz/src/util/windowcontainer.h"
#include "../Wisteria-Dataviz/src/util/parentblocker.h"
#include "../Wisteria-Dataviz/src/util/idhelpers.h"
#include "../Wisteria-Dataviz/src/util/warningmanager.h"

class BaseProjectView : public wxView
    {
    wxDECLARE_NO_COPY_CLASS(BaseProjectView);
public:
    BaseProjectView();
    virtual ~BaseProjectView();
    void SetLeftPaneSize(const int leftPaneWidth)
        {
        m_splitter->SetMinimumPaneSize(leftPaneWidth);
        m_splitter->SplitVertically(m_splitter->GetWindow1(), m_splitter->GetWindow2(), leftPaneWidth);
        }
    void ShowSideBar(const bool show = true);
    [[nodiscard]]
    wxMenuBar* GetMenuBar() noexcept
        { return m_menuBar; }
    [[nodiscard]]
    wxSplitterWindow* GetSplitter() noexcept
        { return m_splitter; }
    [[nodiscard]]
    wxBoxSizer* GetWorkSpaceSizer() noexcept
        { return m_workSpaceSizer; }
    [[nodiscard]]
    Wisteria::UI::SideBar* GetSideBar() noexcept
        { return m_sideBar; }
    [[nodiscard]]
    const Wisteria::UI::SideBar* GetSideBar() const noexcept
        { return m_sideBar; }
    [[nodiscard]]
    Wisteria::UI::InfoBarEx* GetInfoBar() noexcept
        { return m_infoBar; }
    [[nodiscard]]
    const wxWindow* GetActiveProjectWindow() const noexcept
        { return m_activeWindow; }
    [[nodiscard]]
    wxWindow* GetActiveProjectWindow() noexcept
        { return m_activeWindow; }

    /// Fills the menus in the ribbon and checks/toggles all the options on the ribbon to match the project's settings.
    /// Also autofits the sidebar and makes the project the active document/window.
    void Present();

    void ShowInfoMessage(const WarningMessage& message);

    bool OnCreate(wxDocument* doc, [[maybe_unused]] long flags) override;
    void OnCustomTest(wxCommandEvent& event);

    wxMenu m_numeralSyllabicationMenu;
    wxMenu m_longSentencesMenu;
    wxMenu m_textExclusionMenu;
    wxMenu m_exclusionTagsMenu;
    wxMenu m_lineEndsMenu;
    wxMenu m_histobarLabelsMenu;
    wxMenu m_zoomMenu;
    wxMenu m_barOrientationMenu;
    wxMenu m_barStyleMenu;
    wxMenu m_histoBarStyleMenu;
    wxMenu m_boxStyleMenu;
    wxMenu m_graphFontsMenu;
    wxMenu m_graphBackgroundMenu;
    wxMenu m_raygorStyleMenu;
    wxMenu m_gradeScaleMenu;
    wxMenu m_graphSortMenu;
    wxMenu m_wordListMenu;
    wxMenu m_blankGraphMenu;
    wxMenu m_fileOpenMenu;
    wxMenu m_exportMenu;
    wxMenu m_copyMenu;
    wxMenu m_printMenu;
    wxMenu m_exampleMenu;
    wxMenu m_primaryAgeTestsMenu;
    wxMenu m_secondaryAgeTestsMenu;
    wxMenu m_adultTestsMenu;
    wxMenu m_secondLanguageTestsMenu;
    wxMenu m_customTestsMenu;
    wxMenu* m_customTestsRegularMenu{ nullptr };
    wxMenu m_testsBundleMenu;
    wxMenu* m_testsBundleRegularMenu{ nullptr };
    [[nodiscard]]
    wxDocChildFrame* CreateChildFrame(wxDocument* doc, wxView* view);
    [[nodiscard]]
    wxDocChildFrame* GetDocFrame() noexcept
        { return m_frame; }
    [[nodiscard]]
    wxRibbonBar* GetRibbon() noexcept
        { return m_ribbon; }

    [[nodiscard]]
    wxAuiToolBar* GetQuickToolbar() noexcept
        { return m_quickToolbar; }

    [[nodiscard]]
    Wisteria::UI::SearchPanel* GetSearchPanel() noexcept
        { return m_searchCtrl; }

    // labels for the icons on the left-side pane used for project navigation
    [[nodiscard]]
    static wxString GetFormattedReportLabel()
        { return _(L"Formatted Report"); }
    [[nodiscard]]
    static wxString GetTabularReportLabel()
        { return _(L"Tabular Report"); }
    [[nodiscard]]
    static wxString GetWordsBreakdownLabel()
        { return _(L"Words Breakdown"); }
    [[nodiscard]]
    static wxString GetSentencesBreakdownLabel()
        { return _(L"Sentences Breakdown"); }
    [[nodiscard]]
    static wxString GetDifficultWordsLabel()
        { return _(L"Difficult Words"); }
    [[nodiscard]]
    static wxString GetReadabilityScoresLabel()
        { return _(L"Readability Scores"); }
    [[nodiscard]]
    static wxString GetHighlightedReportsLabel()
        { return _(L"Highlighted Reports"); }
    [[nodiscard]]
    static wxString GetGraphsLabel()
        { return _(L"Graphs"); }
    [[nodiscard]]
    static wxString GetGrammarLabel()
        { return _(L"Grammar"); }
    [[nodiscard]]
    static wxString GetDolchLabel()
        { return _(L"Sight Words"); }
    [[nodiscard]]
    static wxString GetBoxPlotsLabel()
        { return _(L"Box Plots"); }
    [[nodiscard]]
    static wxString GetTestGraphsLabel()
        { return _(L"Readability Graphs"); }
    [[nodiscard]]
    static wxString GetWarningLabel()
        { return _(L"Warnings"); }
    // label functions
    [[nodiscard]]
    static wxString GetSentenceStartingWithConjunctionsLabel()
        { return _(L"Sentences that begin with conjunctions"); }
    [[nodiscard]]
    static wxString GetSentenceStartingWithConjunctionsTabLabel()
        { return _(L"Conjunction-starting Sentences"); }
    [[nodiscard]]
    static wxString GetSentenceStartingWithLowercaseLabel()
        { return _(L"Sentences that begin with lowercased words"); }
    [[nodiscard]]
    static wxString GetSentenceStartingWithLowercaseTabLabel()
        { return _(L"Lowercased Sentences"); }
    [[nodiscard]]
    static wxString GetWordyPhrasesTabLabel()
        { return _(L"Wordy Items"); }
    [[nodiscard]]
    static wxString GetRedundantPhrasesTabLabel()
        { return _(L"Redundant Phrases"); }
    [[nodiscard]]
    static wxString GetClichesTabLabel()
        { return _(L"Clich\351s"); }
    [[nodiscard]]
    static wxString GetPhrasingErrorsTabLabel()
        { return _(L"Wording Errors"); }
    [[nodiscard]]
    static wxString GetSummaryStatisticsLabel()
        { return _(L"Summary Statistics"); }
    [[nodiscard]]
    static wxString GetHistogramsLabel()
        { return _(L"Histograms"); }
    [[nodiscard]]
    static wxString GetCoverageChartTabLabel()
        { return _(L"Coverage Chart"); }
    [[nodiscard]]
    static wxString GetWordCountsLabel()
        { return _(L"Word Counts"); }
    [[nodiscard]]
    static wxString GetSyllableCountsLabel()
        { return _(L"Syllable Counts"); }
    [[nodiscard]]
    static wxString GetSentenceLengthBoxPlotLabel()
        { return _(L"Lengths (Spread)"); }
    [[nodiscard]]
    static wxString GetSentenceLengthHistogramLabel()
        { return _(L"Lengths (Distribution)"); }
    [[nodiscard]]
    static wxString GetSentenceLengthHeatmapLabel()
        { return _(L"Lengths (Density)"); }
    [[nodiscard]]
    static wxString GetWordCloudLabel()
        { return _(L"Key Word Cloud"); }
    // Dolch labels
    [[nodiscard]]
    static wxString GetDolchWordTabLabel()
        { return _(L"Dolch Words"); }
    [[nodiscard]]
    static wxString GetNonDolchWordTabLabel()
        { return _(L"Non-Dolch Words"); }
    [[nodiscard]]
    static wxString GetUnusedDolchWordTabLabel()
        { return _(L"Unused Dolch Words"); }
    [[nodiscard]]
    static wxString GetDolchCoverageTabLabel()
        { return _(L"Dolch Word Coverage"); }
    // batch tab labels
    [[nodiscard]]
    static wxString GetRawScoresTabLabel()
        { return _(L"Raw Scores"); }
    // stat headers
    [[nodiscard]]
    static wxString GetAverageLabel()
        { return _(L"Average (Mean)"); }
    [[nodiscard]]
    static wxString GetMedianLabel()
        { return _(L"Median"); }
    [[nodiscard]]
    static wxString GetModeLabel()
        { return _(L"Mode(s)"); }
    [[nodiscard]]
    static wxString GetStdDevLabel()
        { return _(L"Std. Dev."); }
    // errors
    [[nodiscard]]
    static wxString GetFailedLabel()
        { return L"\x26A0 " + _(L"Failed"); } // includes warning emoji
    // tab labels
    [[nodiscard]]
    static wxString GetAllWordsLabel()
        { return _(L"All Words"); }
    [[nodiscard]]
    static wxString GetKeyWordsLabel()
        { return _(L"Key Words"); }
    [[nodiscard]]
    static wxString GetRepeatedWordsLabel()
        { return _(L"Repeated Words"); }
    [[nodiscard]]
    static wxString GetLongSentencesLabel()
        { return _(L"Long Sentences"); }
    [[nodiscard]]
    static wxString GetRaygorLabel()
        { return _(L"Raygor Estimate"); }
    [[nodiscard]]
    static wxString GetFryLabel()
        { return _(L"Fry Graph"); }
    [[nodiscard]]
    static wxString GetGilliamPenanMountainFryLabel()
        { return _(L"Gilliam-Pe\U000000F1a-Mountain Graph"); }
    [[nodiscard]]
    static wxString GetFraseLabel()
        { return _(L"FRASE Graph"); }
    [[nodiscard]]
    static wxString GetSchwartzLabel()
        { return _(L"Schwartz Graph"); }
    [[nodiscard]]
    static wxString GetLixGaugeLabel()
        { return _(L"Lix Gauge"); }
    [[nodiscard]]
    static wxString GetGermanLixGaugeLabel()
        { return _(L"German Lix Gauge"); }
    [[nodiscard]]
    static wxString GetFleschChartLabel()
        { return _(L"Flesch Chart"); }
    [[nodiscard]]
    static wxString GetDB2Label()
        { return _(L"Danielson-Bryan 2"); }
    [[nodiscard]]
    static wxString GetCrawfordGraphLabel()
        { return _(L"Crawford Graph"); }
    [[nodiscard]]
    static wxString GetSixCharWordsLabel()
        { return _(L"6+ Characters"); }
    [[nodiscard]]
    static wxString GetSixCharWordsListLabel()
        { return _(L"6+ Characters List"); }
    [[nodiscard]]
    static wxString GetSixCharWordsReportLabel()
        { return _(L"6+ Characters Report"); }
    [[nodiscard]]
    static wxString GetThreeSyllableWordsLabel()
        { return _(L"3+ Syllables"); }
    [[nodiscard]]
    static wxString GetThreeSyllableListWordsLabel()
        { return _(L"3+ Syllables List"); }
    [[nodiscard]]
    static wxString GetThreeSyllableReportWordsLabel()
        { return _(L"3+ Syllables Report"); }
    [[nodiscard]]
    static wxString GetProperNounsLabel()
        { return _(L"Proper Nouns"); }
    [[nodiscard]]
    static wxString GetContractionsLabel()
        { return _(L"Contractions"); }
    [[nodiscard]]
    static wxString GetOverusedWordsBySentenceLabel()
        { return _(L"Overused Words (x Sentence)"); }
    [[nodiscard]]
    static wxString GetMisspellingsLabel()
        { return _(L"Misspellings"); }
    [[nodiscard]]
    static wxString GetArticleMismatchesLabel()
        { return _(L"Article Mismatches"); }
    [[nodiscard]]
    static wxString GetPassiveLabel()
        { return _(L"Passive Voice"); }
    [[nodiscard]]
    static wxString GetDaleChallLabel()
        { return _DT(L"Dale-Chall"); }
    [[nodiscard]]
    static wxString GetSpacheLabel()
        { return _DT(L"Spache"); }
    [[nodiscard]]
    static wxString GetHarrisJacobsonLabel()
        { return _DT(L"Harris-Jacobson"); }
    // icon emojis used for goals, reports, and test results
    [[nodiscard]]
    static wxString GetCheckmarkEmoji()
        { return L"\x2714"; }
    [[nodiscard]]
    static wxString GetWarningEmoji()
        { return L"\x26A0"; }
    [[nodiscard]]
    static wxString GetNoteEmoji()
        { return L"\U0001F4DD"; }

    [[nodiscard]]
    long GetMaxColumnWidth() const noexcept
        { return m_maxColumnWidth; }

    // list IDs
    //----------
    static constexpr int SENTENCES_CONJUNCTION_START_LIST_PAGE_ID = wxID_HIGHEST;
    static constexpr int SENTENCES_LOWERCASE_START_LIST_PAGE_ID = wxID_HIGHEST + 1;
    static constexpr int WORDY_PHRASES_LIST_PAGE_ID = wxID_HIGHEST + 2;
    static constexpr int REDUNDANT_PHRASE_LIST_PAGE_ID = wxID_HIGHEST + 3;
    static constexpr int CLICHES_LIST_PAGE_ID = wxID_HIGHEST + 4;
    static constexpr int DUPLICATES_LIST_PAGE_ID = wxID_HIGHEST + 5;
    static constexpr int INCORRECT_ARTICLE_PAGE_ID = wxID_HIGHEST + 6;
    static constexpr int PASSIVE_VOICE_PAGE_ID = wxID_HIGHEST + 7;
    static constexpr int LONG_SENTENCES_LIST_PAGE_ID = wxID_HIGHEST + 8;
    static constexpr int HARD_WORDS_LIST_PAGE_ID = wxID_HIGHEST + 9;
    static constexpr int LONG_WORDS_LIST_PAGE_ID = wxID_HIGHEST + 10;
    static constexpr int DC_WORDS_LIST_PAGE_ID = wxID_HIGHEST + 11;
    static constexpr int SPACHE_WORDS_LIST_PAGE_ID = wxID_HIGHEST + 12;
    static constexpr int ALL_WORDS_LIST_PAGE_ID = wxID_HIGHEST + 13;
    static constexpr int ALL_WORDS_CONDENSED_LIST_PAGE_ID = wxID_HIGHEST + 14;
    static constexpr int PROPER_NOUNS_LIST_PAGE_ID = wxID_HIGHEST + 15;
    static constexpr int CONTRACTIONS_LIST_PAGE_ID = wxID_HIGHEST + 16;
    static constexpr int OVERUSED_WORDS_BY_SENTENCE_LIST_PAGE_ID = wxID_HIGHEST + 17;
    static constexpr int HARRIS_JACOBSON_WORDS_LIST_PAGE_ID = wxID_HIGHEST + 18;
    static constexpr int MISSPELLED_WORD_LIST_PAGE_ID = wxID_HIGHEST + 19;
    static constexpr int WORDING_ERRORS_LIST_PAGE_ID = wxID_HIGHEST + 20;
    // graph IDs
    //----------
    static constexpr int WORD_BREAKDOWN_PAGE_ID = wxID_HIGHEST + 30;
    static constexpr int FRY_PAGE_ID = wxID_HIGHEST + 31;
    static constexpr int RAYGOR_PAGE_ID = wxID_HIGHEST + 32;
    static constexpr int FRASE_PAGE_ID = wxID_HIGHEST + 33;
    static constexpr int SCHWARTZ_PAGE_ID = wxID_HIGHEST + 34;
    static constexpr int LIX_GAUGE_PAGE_ID = wxID_HIGHEST + 35;
    static constexpr int LIX_GAUGE_GERMAN_PAGE_ID = wxID_HIGHEST + 36;
    static constexpr int DB2_PAGE_ID = wxID_HIGHEST + 37;
    static constexpr int FLESCH_CHART_PAGE_ID = wxID_HIGHEST + 38;
    static constexpr int CRAWFORD_GRAPH_PAGE_ID = wxID_HIGHEST + 39;
    static constexpr int DOLCH_COVERAGE_CHART_PAGE_ID = wxID_HIGHEST + 40;
    static constexpr int DOLCH_BREAKDOWN_PAGE_ID = wxID_HIGHEST + 41;
    static constexpr int GPM_FRY_PAGE_ID = wxID_HIGHEST + 42;
    static constexpr int SENTENCE_BOX_PLOT_PAGE_ID = wxID_HIGHEST + 43;
    static constexpr int SENTENCE_HISTOGRAM_PAGE_ID = wxID_HIGHEST + 44;
    static constexpr int SYLLABLE_HISTOGRAM_PAGE_ID = wxID_HIGHEST + 45;
    static constexpr int SENTENCE_HEATMAP_PAGE_ID = wxID_HIGHEST + 46;
    static constexpr int WORD_CLOUD_PAGE_ID = wxID_HIGHEST + 47;
    // batch IDs
    //----------
    static constexpr int ID_SCORE_LIST_PAGE_ID = wxID_HIGHEST + 60;
    static constexpr int ID_DIFFICULT_WORDS_LIST_PAGE_ID = wxID_HIGHEST + 61;
    static constexpr int ID_DOLCH_COVERAGE_LIST_PAGE_ID = wxID_HIGHEST + 62;
    static constexpr int ID_DOLCH_WORDS_LIST_PAGE_ID = wxID_HIGHEST + 63;
    static constexpr int ID_NON_DOLCH_WORDS_LIST_PAGE_ID = wxID_HIGHEST + 64;
    // only window in the warnings section,
    // so we will reuse this ID for the sidebar section also (below)
    static constexpr int ID_WARNING_LIST_PAGE_ID = wxID_HIGHEST + 65;
    static constexpr int ID_SCORE_STATS_LIST_PAGE_ID = wxID_HIGHEST + 66;
    static constexpr int ID_AGGREGATED_DOC_SCORES_LIST_PAGE_ID = wxID_HIGHEST + 67;
    static constexpr int ID_AGGREGATED_CLOZE_SCORES_LIST_PAGE_ID = wxID_HIGHEST + 68;
    // Dolch IDs
    //----------
    static constexpr int DOLCH_WORDS_LIST_PAGE_ID = wxID_HIGHEST + 70;
    static constexpr int NON_DOLCH_WORDS_LIST_PAGE_ID = wxID_HIGHEST + 71;
    static constexpr int UNUSED_DOLCH_WORDS_LIST_PAGE_ID = wxID_HIGHEST + 72;
    static constexpr int DOLCH_WORDS_TEXT_PAGE_ID = wxID_HIGHEST + 73;
    static constexpr int NON_DOLCH_WORDS_TEXT_PAGE_ID = wxID_HIGHEST + 74;
    static constexpr int DOLCH_STATS_PAGE_ID = wxID_HIGHEST + 75;
    // stats/scores IDs
    //----------
    static constexpr int STATS_REPORT_PAGE_ID = wxID_HIGHEST + 80;
    static constexpr int STATS_LIST_PAGE_ID = wxID_HIGHEST + 81;
    static constexpr int READABILITY_SCORES_PAGE_ID = wxID_HIGHEST + 82;
    static constexpr int READABILITY_SCORES_SUMMARY_REPORT_PAGE_ID = wxID_HIGHEST + 83;
    static constexpr int READABILITY_GOALS_PAGE_ID = wxID_HIGHEST + 84;
    // highlighted words IDs
    //----------
    static constexpr int LONG_WORDS_TEXT_PAGE_ID = wxID_HIGHEST + 85;
    static constexpr int DC_WORDS_TEXT_PAGE_ID = wxID_HIGHEST + 86;
    static constexpr int SPACHE_WORDS_TEXT_PAGE_ID = wxID_HIGHEST + 87;
    static constexpr int HARRIS_JACOBSON_WORDS_TEXT_PAGE_ID = wxID_HIGHEST + 88;
    static constexpr int HARD_WORDS_TEXT_PAGE_ID = wxID_HIGHEST + 89;
    static constexpr int LONG_SENTENCES_AND_WORDINESS_TEXT_PAGE_ID = wxID_HIGHEST + 90;
    // sidebar IDs (main categories)
    //----------
    static constexpr int SIDEBAR_WORDS_BREAKDOWN_SECTION_ID = wxID_HIGHEST + 91;
    static constexpr int SIDEBAR_READABILITY_SCORES_SECTION_ID = wxID_HIGHEST + 92;
    static constexpr int SIDEBAR_GRAMMAR_SECTION_ID = wxID_HIGHEST + 93;
    static constexpr int SIDEBAR_DOLCH_SECTION_ID = wxID_HIGHEST + 94;
    static constexpr int SIDEBAR_WARNINGS_SECTION_ID = ID_WARNING_LIST_PAGE_ID;
    static constexpr int SIDEBAR_BOXPLOTS_SECTION_ID = wxID_HIGHEST + 95;
    static constexpr int SIDEBAR_HISTOGRAMS_SECTION_ID = wxID_HIGHEST + 96;
    static constexpr int SIDEBAR_STATS_SUMMARY_SECTION_ID = wxID_HIGHEST + 97;
    static constexpr int SIDEBAR_SENTENCES_BREAKDOWN_SECTION_ID = wxID_HIGHEST + 98;
    // Sidebar IDs for custom test windows.
    // Don't add any IDs between here and 1,000 above it; we will lock this whole range for that.
    static constexpr int SIDEBAR_CUSTOM_TESTS_START_ID = wxID_HIGHEST + 100;
    static IdRange& GetCustomTestSidebarIdRange() noexcept
        { return m_customTestSidebarIdRange; }
protected:
    wxWindow* m_activeWindow{ nullptr };
    static IdRange m_customTestSidebarIdRange;
    Wisteria::UI::SearchPanel* m_searchCtrl{ nullptr };
    Wisteria::UI::InfoBarEx* m_infoBar{ nullptr };
    wxMenuBar* m_menuBar{ nullptr };

    static constexpr int LEFT_PANE = wxID_HIGHEST + 73;
    static constexpr int SPLITTER_ID = wxID_HIGHEST + 74;
private:
    void OnActivateView(bool activate, wxView*, wxView*) override;

    void OnProjectSettings([[maybe_unused]] wxRibbonButtonBarEvent& event);

    void OnDocumentRefresh([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnDocumentRefreshMenu([[maybe_unused]] wxCommandEvent& event);

    void OnEditStatsReportButton([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnDropShadow([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnGraphWatermark([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnGraphLogo([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnInvalidRegionColor(wxRibbonButtonBarEvent& event);
    void OnEnglishLabels(wxRibbonButtonBarEvent& event);
    void OnFleschConnectLinesButton([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnBarLabelsButton([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnZoomButton(wxRibbonButtonBarEvent& event);
    void OnBarOrientationButton(wxRibbonButtonBarEvent& event);
    void OnBarOrientationSelected(wxCommandEvent& event);
    void OnBarStyleSelected(wxCommandEvent& event);
    void OnBarSelectStippleBrush([[maybe_unused]] wxCommandEvent& event);
    void OnBarSelectCommonImage([[maybe_unused]] wxCommandEvent& event);
    void OnBarStyleButton(wxRibbonButtonBarEvent& event);
    void OnHistoBarStyleSelected(wxCommandEvent& event);
    void OnHistoBarSelectStippleBrush([[maybe_unused]] wxCommandEvent& event);
    void OnHistoBarSelectStippleShape([[maybe_unused]] wxCommandEvent& event);
    void OnBarSelectStippleShape([[maybe_unused]] wxCommandEvent& event);
    void OnBoxSelectStippleShape([[maybe_unused]] wxCommandEvent& event);
    void OnHistoBarSelectCommonImage([[maybe_unused]] wxCommandEvent& event);
    void OnHistoBarsLabelsButton(wxRibbonButtonBarEvent& event);
    void OnHistoBarLabelSelected(wxCommandEvent& event);
    void OnHistoBarStyleButton(wxRibbonButtonBarEvent& event);
    void OnBoxStyleSelected(wxCommandEvent& event);
    void OnBoxSelectStippleBrush([[maybe_unused]] wxCommandEvent& event);
    void OnBoxSelectCommonImage([[maybe_unused]] wxCommandEvent& event);
    void OnBoxStyleButton(wxRibbonButtonBarEvent& event);
    void OnBoxPlotShowAllPointsButton([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnBoxPlotShowLabelsButton([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnEditGraphBackgroundButton(wxRibbonButtonBarEvent& event);
    void OnEditGraphBackgroundImage([[maybe_unused]] wxCommandEvent& event);
    void OnEditGraphBackgroundImageEffect(wxCommandEvent& event);
    void OnGraphColorFade([[maybe_unused]] wxCommandEvent& event);
    void OnEditGraphRaygorStyleButton([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnRaygorStyleSelected([[maybe_unused]] wxCommandEvent& event);
    void OnEditGraphColor(wxCommandEvent& event);
    void OnEditGraphOpacity(wxCommandEvent& event);
    void OnEditGraphFontsButton(wxRibbonButtonBarEvent& event);
    void OnEditGraphFont(wxCommandEvent& event);
    void OnEditDictionaryButton([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnEditEnglishDictionary([[maybe_unused]] wxCommandEvent& event);
    void OnEditDictionaryProjectSettings([[maybe_unused]] wxCommandEvent& event);
    void OnEditDictionary([[maybe_unused]] wxCommandEvent& event);
    // document indexing options
    void OnNumeralSyllabication([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnNumeralSyllabicationOptions([[maybe_unused]] wxCommandEvent& event);
    void OnLongSentences([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnLongSentencesOptions([[maybe_unused]] wxCommandEvent& event);
    void OnIncompleteThreshold([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnExcludeWordsList([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnTextExclusion([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnExclusionTags(wxRibbonButtonBarEvent& event);
    void OnExclusionTagsOptions([[maybe_unused]] wxCommandEvent& event);
    void OnTextExclusionOptions([[maybe_unused]] wxCommandEvent& event);
    void OnLineEnds([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnLineEndOptions(wxCommandEvent& event);
    void OnIgnoreBlankLines([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnIgnoreIdenting([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnStrictCapitalization([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnAggressivelyExclude([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnIgnoreCopyrights([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnIgnoreCitations([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnIgnoreFileAddresses([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnIgnoreNumerals([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnIgnoreProperNouns([[maybe_unused]] wxRibbonButtonBarEvent& event);

    bool OnClose(bool deleteWindow = true) override;
    void OnFindMenu([[maybe_unused]] wxCommandEvent& event);
    void OnFindNext([[maybe_unused]] wxCommandEvent& event);
    void OnDraw(wxDC*) override {};
    // custom test functions
    void OnAddCustomTest(wxCommandEvent& event);
    void OnEditCustomTest(wxCommandEvent& event);
    void OnRemoveCustomTest(wxCommandEvent& event);
    void OnTestBundle(wxCommandEvent& event);
    void OnCustomTestBundle(wxCommandEvent& event);
    void OnAddCustomTestBundle(wxCommandEvent& event);
    void OnEditCustomTestBundle(wxCommandEvent& event);
    void OnRemoveCustomTestBundle(wxCommandEvent& event);

    void OnWordListDropdown(wxRibbonButtonBarEvent& evt);
    void OnBlankGraphDropdown(wxRibbonButtonBarEvent& evt);
    void OnOpenDropdown(wxRibbonButtonBarEvent& evt);
    void OnSaveDropdown(wxRibbonButtonBarEvent& evt);
    void OnPrintDropdown(wxRibbonButtonBarEvent& evt);
    void OnCopyDropdown(wxRibbonButtonBarEvent& evt);
    void OnDictionaryDropdown(wxRibbonButtonBarEvent& evt);
    void OnExampleDropdown(wxRibbonButtonBarEvent& evt);
    void OnGraphSortDropdown(wxRibbonButtonBarEvent& evt);
    void OnGradeScaleDropdown(wxRibbonButtonBarEvent& evt);
    void OnPrimaryAgeTestsDropdown(wxRibbonButtonBarEvent& evt);
    void OnSecondaryAgeTestsDropdown(wxRibbonButtonBarEvent& evt);
    void OnAdultTestsDropdown(wxRibbonButtonBarEvent& evt);
    void OnSecondLanguageTestsDropdown(wxRibbonButtonBarEvent& evt);
    void OnCustomTestsDropdown(wxRibbonButtonBarEvent& evt);
    void OnTestBundlesDropdown(wxRibbonButtonBarEvent& evt);
    void OnDClickRibbonBar([[maybe_unused]] wxRibbonBarEvent& event);
    void OnClickRibbonBar([[maybe_unused]] wxRibbonBarEvent& event);
    void OnCloseInfoBar(wxCommandEvent& event);

    wxDocChildFrame* m_frame{ nullptr };
    wxRibbonBar* m_ribbon{ nullptr };
    Wisteria::UI::SideBar* m_sideBar{ nullptr };
    wxAuiToolBar* m_quickToolbar{ nullptr };
    wxSplitterWindow* m_splitter{ nullptr };
    wxBoxSizer* m_workSpaceSizer{ nullptr };
    // status flags
    bool m_presentedSuccessfully{ false };
    bool m_sidebarShown{ true };

    long m_maxColumnWidth{ 200 };

    // logged messages
    std::set<WarningMessage> m_queuedMessages;
    /// @return the messages that won't be shown until client asks from them to be shown.
    [[nodiscard]]
    const std::set<WarningMessage>& GetQueuedMessages() const noexcept
        { return m_queuedMessages; }
    /// @brief Saves a message to be shown later, when ShowQueuedMessages() is called.
    /// @param message The message to queue.
    void AddQueuedMessage(const WarningMessage& message)
        { m_queuedMessages.insert(message); }

    wxString m_lastShownMessageId;

    wxDECLARE_DYNAMIC_CLASS(BaseProjectView);
    wxDECLARE_EVENT_TABLE();
    };

#endif //__BASE_PROJECT_VIEW_H__
