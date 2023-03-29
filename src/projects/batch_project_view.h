#ifndef __BATCH_PROJECT_VIEW_H__
#define __BATCH_PROJECT_VIEW_H__

#include "base_project_view.h"
#include "../graphs/frygraph.h"
#include "../graphs/raygorgraph.h"
#include "../graphs/frasegraph.h"
#include "../../../SRC/Wisteria-Dataviz/src/graphs/fleschchart.h"
#include "../../../SRC/Wisteria-Dataviz/src/graphs/crawfordgraph.h"
#include "../../../SRC/Wisteria-Dataviz/src/graphs/histogram.h"
#include "../../../SRC/Wisteria-Dataviz/src/ui/dialogs/htmltablepanel.h"
#include "../../../SRC/Wisteria-Dataviz/src/ui/dialogs/listdlg.h"

class BatchProjectView final : public BaseProjectView
    {
    wxDECLARE_NO_COPY_CLASS(BatchProjectView);
public:
    BatchProjectView() :
        m_warningsView(nullptr),
        m_fleschChart(nullptr), m_db2Plot(nullptr), m_fryGraph(nullptr), m_GpmFryGraph(nullptr),
        m_FraseGraph(nullptr), m_SchwartzGraph(nullptr), m_raygorGraph(nullptr), m_crawfordGraph(nullptr),
        m_LixGauge(nullptr), m_GermanLixGauge(nullptr),
        m_testExplanations(nullptr), m_statsReport(nullptr) {}

    void OnGradeScale(wxCommandEvent& event);
    void OnLongFormat([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnSummation([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnAddToDictionary([[maybe_unused]] wxCommandEvent& event);
    void OnDblClick(wxListEvent& event);
    bool OnCreate(wxDocument* doc, long flags) final;
    void OnPaneShowOrHide(wxRibbonButtonBarEvent& event);
    void OnDocumentDelete([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnTestDelete([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnTestDeleteMenu([[maybe_unused]] wxCommandEvent& event);
    void OnItemSelected(wxCommandEvent& event);
    void OnScoreItemSelected(wxListEvent& event);
    void OnNonScoreItemSelected(wxListEvent& event);
    void OnRibbonButtonCommand(wxRibbonButtonBarEvent& event);
    void OnMenuCommand(wxCommandEvent& event);
    void OnExportStatisticsReport([[maybe_unused]] wxCommandEvent& event);
    void OnExportScoresAndStatistics([[maybe_unused]] wxCommandEvent& event);
    void OnBatchExportFilteredDocuments([[maybe_unused]] wxCommandEvent& event);
    void OnFind(wxFindDialogEvent &event);
    void OnAddTest(wxCommandEvent& event);

    /// @returns @c true if ID is for a list window with filepaths listed in it.
    [[nodiscard]] bool IsFilenameList(const wxWindowID id) const noexcept
        {
        return (id == ID_SCORE_LIST_PAGE_ID ||
                id == READABILITY_GOALS_PAGE_ID ||
                id == ID_DIFFICULT_WORDS_LIST_PAGE_ID ||
                id == LONG_SENTENCES_LIST_PAGE_ID ||
                id == SENTENCES_CONJUNCTION_START_LIST_PAGE_ID ||
                id == SENTENCES_LOWERCASE_START_LIST_PAGE_ID ||
                id == DUPLICATES_LIST_PAGE_ID ||
                id == INCORRECT_ARTICLE_PAGE_ID ||
                id == PASSIVE_VOICE_PAGE_ID ||
                id == MISSPELLED_WORD_LIST_PAGE_ID ||
                id == REDUNDANT_PHRASE_LIST_PAGE_ID ||
                id == WORDING_ERRORS_LIST_PAGE_ID ||
                id == WORDY_PHRASES_LIST_PAGE_ID ||
                id == CLICHES_LIST_PAGE_ID ||
                id == ID_DOLCH_COVERAGE_LIST_PAGE_ID ||
                id == ID_DOLCH_WORDS_LIST_PAGE_ID ||
                id == ID_NON_DOLCH_WORDS_LIST_PAGE_ID ||
                id == ID_WARNING_LIST_PAGE_ID ||
                id == ID_AGGREGATED_DOC_SCORES_LIST_PAGE_ID ||
                id == ID_AGGREGATED_CLOZE_SCORES_LIST_PAGE_ID ||
                id == OVERUSED_WORDS_BY_SENTENCE_LIST_PAGE_ID);
        }
    [[nodiscard]] WindowContainer& GetScoresView() noexcept
        { return m_scoresView; }
    [[nodiscard]] const WindowContainer& GetScoresView() const noexcept
        { return m_scoresView; }
    [[nodiscard]] WindowContainer& GetHistogramsView() noexcept
        { return m_histogramsView; }
    [[nodiscard]] const WindowContainer& GetHistogramsView() const noexcept
        { return m_histogramsView; }
    [[nodiscard]] WindowContainer& GetBoxPlotView() noexcept
        { return m_boxPlotView; }
    [[nodiscard]] const WindowContainer& GetBoxPlotView() const noexcept
        { return m_boxPlotView; }
    [[nodiscard]] ListCtrlEx* GetWarningsView() noexcept
        { return m_warningsView; }
    [[nodiscard]] const ListCtrlEx* GetWarningsView() const noexcept
        { return m_warningsView; }
    [[nodiscard]] WindowContainer& GetGrammarView() noexcept
        { return m_grammarView; }
    [[nodiscard]] const WindowContainer& GetGrammarView() const noexcept
        { return m_grammarView; }
    [[nodiscard]] WindowContainer& GetWordsBreakdownView() noexcept
        { return m_wordsBreakdownView; }
    [[nodiscard]] const WindowContainer& GetWordsBreakdownView() const noexcept
        { return m_wordsBreakdownView; }
    [[nodiscard]] WindowContainer& GetSentencesBreakdownView() noexcept
        { return m_sentencesBreakdownView; }
    [[nodiscard]] const WindowContainer& GetSentencesBreakdownView() const noexcept
        { return m_sentencesBreakdownView; }
    [[nodiscard]] WindowContainer& GetDolchSightWordsView() noexcept
        { return m_sightWordView; }
    [[nodiscard]] const WindowContainer& GetDolchSightWordsView() const noexcept
        { return m_sightWordView; }
    //Fry
    [[nodiscard]] Wisteria::Canvas* GetFryGraph() noexcept
        { return m_fryGraph; }
    [[nodiscard]] const Wisteria::Canvas* GetFryGraph() const noexcept
        { return m_fryGraph; }
    void SetFryGraph(Wisteria::Canvas* graph) noexcept
        { m_fryGraph = graph; }
    //Flesch
    [[nodiscard]] Wisteria::Canvas* GetFleschChart() noexcept
        { return m_fleschChart; }
    [[nodiscard]] const Wisteria::Canvas* GetFleschChart() const noexcept
        { return m_fleschChart; }
    void SetFleschChart(Wisteria::Canvas* chart) noexcept
        { m_fleschChart = chart; }
    //DB2
    [[nodiscard]] Wisteria::Canvas* GetDB2Plot() noexcept
        { return m_db2Plot; }
    [[nodiscard]] const Wisteria::Canvas* GetDB2Plot() const noexcept
        { return m_db2Plot; }
    void SetDB2Plot(Wisteria::Canvas* chart) noexcept
        { m_db2Plot = chart; }
    //GPM (Spanish) Fry
    [[nodiscard]] Wisteria::Canvas* GetGpmFryGraph() noexcept
        { return m_GpmFryGraph; }
    [[nodiscard]] const Wisteria::Canvas* GetGpmFryGraph() const noexcept
        { return m_GpmFryGraph; }
    void SetGpmFryGraph(Wisteria::Canvas* graph) noexcept
        { m_GpmFryGraph = graph; }
    //Frase
    [[nodiscard]] Wisteria::Canvas* GetFraseGraph() noexcept
        { return m_FraseGraph; }
    [[nodiscard]] const Wisteria::Canvas* GetFraseGraph() const noexcept
        { return m_FraseGraph; }
    void SetFraseGraph(Wisteria::Canvas* graph) noexcept
        { m_FraseGraph = graph; }
    //Schwartz
    [[nodiscard]] Wisteria::Canvas* GetSchwartzGraph() noexcept
        { return m_SchwartzGraph; }
    [[nodiscard]] const Wisteria::Canvas* GetSchwartzGraph() const noexcept
        { return m_SchwartzGraph; }
    void SetSchwartzGraph(Wisteria::Canvas* graph) noexcept
        { m_SchwartzGraph = graph; }
    //Lix
    [[nodiscard]] Wisteria::Canvas* GetLixGauge() noexcept
        { return m_LixGauge; }
    [[nodiscard]] const Wisteria::Canvas* GetLixGauge() const noexcept
        { return m_LixGauge; }
    void SetLixGauge(Wisteria::Canvas* graph) noexcept
        { m_LixGauge = graph; }
    //German Lix
    [[nodiscard]] Wisteria::Canvas* GetGermanLixGauge() noexcept
        { return m_GermanLixGauge; }
    [[nodiscard]] const Wisteria::Canvas* GetGermanLixGauge() const noexcept
        { return m_GermanLixGauge; }
    void SetGermanLixGauge(Wisteria::Canvas* graph) noexcept
        { m_GermanLixGauge = graph; }
    //Raygor
    [[nodiscard]] Wisteria::Canvas* GetRaygorGraph() noexcept
        { return m_raygorGraph; }
    [[nodiscard]] const Wisteria::Canvas* GetRaygorGraph() const noexcept
        { return m_raygorGraph; }
    void SetRaygorGraph(Wisteria::Canvas* graph) noexcept
        { m_raygorGraph = graph; }
    //Crawford
    [[nodiscard]] Wisteria::Canvas* GetCrawfordGraph() noexcept
        { return m_crawfordGraph; }
    [[nodiscard]] const Wisteria::Canvas* GetCrawfordGraph() const noexcept
        { return m_crawfordGraph; }
    void SetCrawfordGraph(Wisteria::Canvas* graph) noexcept
        { m_crawfordGraph = graph; }

    void UpdateSideBarIcons();

    void OnEditGraphOptions([[maybe_unused]] wxCommandEvent& event);

    bool ExportAll(const wxString& folder, wxString listExt, wxString graphExt,
        const bool includeHardWordLists,
        const bool includeSentencesBreakdown,
        const bool includeGraphs,
        const bool includeTestScores, const bool includeWordiness, const bool includeSightWords,
        const bool includeWarnings,
        const Wisteria::UI::ImageExportOptions& graphOptions);
    bool ExportAllToHtml(const wxFileName& filePath, wxString graphExt,
        const bool includeHardWordLists,
        const bool includeSentencesBreakdown,
        const bool includeGraphs,
        const bool includeTestScores, const bool includeWordiness, const bool includeSightWords,
        const bool includeWarnings,
        const Wisteria::UI::ImageExportOptions& graphOptions);
    //label functions
    [[nodiscard]] static wxString FormatClozeValuesLabel(const wxString& testName)
        { return testName + _(" (pred. cloze scores)"); }
    [[nodiscard]] static wxString StripClozeValuesLabel(const wxString& testName)
        {
        wxString begin;
        if (testName.EndsWith(_(" (pred. cloze scores)"), &begin))
            { return begin.Strip(wxString::both); }
        else
            { return testName; }
        }
    [[nodiscard]] static wxString FormatIndexValuesLabel(const wxString& testName)
        { return testName + _(" (index values)"); }
    [[nodiscard]] static wxString StripIndexValuesLabel(const wxString& testName)
        {
        wxString begin;
        if (testName.EndsWith(_(" (index values)"), &begin))
            { return begin.Strip(wxString::both); }
        else
            { return testName; }
        }
    [[nodiscard]] static wxString FormatGradeLevelsLabel(const wxString& testName)
        { return testName + _(" (grade levels)"); }
    [[nodiscard]] static wxString StripGradeLevelsLabel(const wxString& testName)
        {
        wxString begin;
        if (testName.EndsWith(_(" (grade levels)"), &begin))
            { return begin.Strip(wxString::both); }
        else
            { return testName; }
        }
    [[nodiscard]] wxString StripToTestName(const wxString& testName);
    void UpdateStatAndTestPanes(const wxString& fileName);
    [[nodiscard]] wxString GetCurrentlySelectedFileName() const
        { return m_currentlySelectedFileName; }
private:
    wxWindow* FindWindowById(const int Id);
    void RemoveFromAllListCtrls(const wxString& valueToRemove);
    void UpdateStatAndTestPanes(const long scoreListItem);

    wxString m_currentlySelectedFileName;

    WindowContainer m_scoresView;
    WindowContainer m_histogramsView;
    WindowContainer m_boxPlotView;
    ListCtrlEx* m_warningsView { nullptr };
    WindowContainer m_wordsBreakdownView;
    WindowContainer m_sentencesBreakdownView;
    WindowContainer m_grammarView;
    WindowContainer m_sightWordView;
    Wisteria::Canvas* m_fleschChart { nullptr };
    Wisteria::Canvas* m_db2Plot { nullptr };
    Wisteria::Canvas* m_fryGraph { nullptr };
    Wisteria::Canvas* m_GpmFryGraph { nullptr };
    Wisteria::Canvas* m_FraseGraph { nullptr };
    Wisteria::Canvas* m_SchwartzGraph { nullptr };
    Wisteria::Canvas* m_LixGauge { nullptr };
    Wisteria::Canvas* m_GermanLixGauge { nullptr };
    Wisteria::Canvas* m_raygorGraph { nullptr };
    Wisteria::Canvas* m_crawfordGraph { nullptr };
    Wisteria::UI::HtmlTablePanel* m_testExplanations { nullptr };
    Wisteria::UI::HtmlTablePanel* m_statsReport { nullptr };

    wxDECLARE_DYNAMIC_CLASS(BatchProjectView);
    wxDECLARE_EVENT_TABLE();
    };

#endif //__BATCH_PROJECT_VIEW_H__
