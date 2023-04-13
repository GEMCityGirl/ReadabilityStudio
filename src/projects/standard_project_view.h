#ifndef __PROJECT_VIEW_H__
#define __PROJECT_VIEW_H__

#include <vector>
#include <map>
#include <wx/utils.h> 
#include "../results_format/readability_messages.h"
#include "../graphs/frygraph.h"
#include "../Wisteria-Dataviz/src/math/statistics.h"
#include "base_project_view.h"

/// @brief The results window interface for a standard project.
class ProjectView final : public BaseProjectView
    {
public:
    /// ProjectDoc has friend access to this class.
    friend class ProjectDoc;
    /// @brief Constructor.
    ProjectView() :
        m_statsListData(new ListCtrlExDataProvider)
        {
        Bind(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this, SIDEBAR_CUSTOM_TESTS_START_ID, SIDEBAR_CUSTOM_TESTS_START_ID+1000);
        }
    ProjectView(const ProjectView&) = delete;
    ProjectView(ProjectView&&) = delete;
    ProjectView& operator=(const ProjectView&) = delete;
    ProjectView& operator=(ProjectView&&) = delete;
    /// @brief Destructor.
    ~ProjectView()
        { wxDELETE(m_statsListData); }

    /** @brief Saves all the results to an HTML report.
        @param filePath The path to save the report to.
        @param graphExt The file extension to save graphs as (e.g., PNG).
        @param includeWordsBreakdown True to save the words breakdown section.
        @param includeSentencesBreakdown True to save the sentences breakdown section.
        @param includeTestScores True to save the test scores section.
        @param includeStatistics True to save the statistics section.
        @param includeGrammar True to save the grammar section.
        @param includeSightWords True to save the Dolch Sight Words section.
        @param includeLists True to save lists (not recommended for large source documents).
        @param includeTextReports True to save text reports (not recommended for large source documents).
        @param graphOptions Additional options for how to export the graphs.*/
    bool ExportAllToHtml(const wxFileName& filePath, wxString graphExt,
                         const bool includeWordsBreakdown,
                         const bool includeSentencesBreakdown,
                         const bool includeTestScores,
                         const bool includeStatistics,
                         const bool includeGrammar,
                         const bool includeSightWords,
                         const bool includeLists,
                         const bool includeTextReports,
                         const Wisteria::UI::ImageExportOptions& graphOptions);

    /** @brief Saves all the results to a set of files in a folder.
        @param folder The folder to save the results to.
        @param listExt The file extension to save the lists as (HTML or TXT).
        @param textExt The file extension to save the text windows as (RTF or TXT).
        @param graphExt The file extension to save graphs as (e.g., PNG).
        @param includeWordsBreakdown True to save the words breakdown section.
        @param includeSentencesBreakdown True to save the sentences breakdown section.
        @param includeTestScores True to save the test scores section.
        @param includeStatistics True to save the statistics section.
        @param includeGrammar True to save the grammar section.
        @param includeSightWords True to save the Dolch Sight Words section.
        @param includeLists True to save lists (not recommended for large source documents).
        @param includeTextReports True to save text reports (not recommended for large source documents).
        @param graphOptions Additional options for how to export the graphs.*/
    bool ExportAll(const wxString& folder, wxString listExt, wxString textExt, wxString graphExt,
                   const bool includeWordsBreakdown,
                   const bool includeSentencesBreakdown,
                   const bool includeTestScores,
                   const bool includeStatistics,
                   const bool includeGrammar,
                   const bool includeSightWords,
                   const bool includeLists,
                   const bool includeTextReports,
                   const Wisteria::UI::ImageExportOptions& graphOptions);

    //view classes
    [[nodiscard]] WindowContainer& GetReadabilityResultsView() noexcept
        { return m_readabilityResultsView; }
    [[nodiscard]] const WindowContainer& GetReadabilityResultsView() const noexcept
        { return m_readabilityResultsView; }

    [[nodiscard]] WindowContainer& GetSummaryView() noexcept
        { return m_summaryView; }
    [[nodiscard]] const WindowContainer& GetSummaryView() const noexcept
        { return m_summaryView; }

    [[nodiscard]] WindowContainer& GetWordsBreakdownView() noexcept
        { return m_WordsBreakdownView; }
    [[nodiscard]] const WindowContainer& GetWordsBreakdownView() const noexcept
        { return m_WordsBreakdownView; }

    [[nodiscard]] WindowContainer& GetSentencesBreakdownView() noexcept
        { return m_sentencesBreakdownView; }
    [[nodiscard]] const WindowContainer& GetSentencesBreakdownView() const noexcept
        { return m_sentencesBreakdownView; }

    [[nodiscard]] WindowContainer& GetGrammarView() noexcept
        { return m_grammarView; }
    [[nodiscard]] const WindowContainer& GetGrammarView() const noexcept
        { return m_grammarView; }

    [[nodiscard]] WindowContainer& GetDolchSightWordsView() noexcept
        { return m_sightWordView; }
    [[nodiscard]] const WindowContainer& GetDolchSightWordsView() const noexcept
        { return m_sightWordView; }

    //Shortcuts to the sub-windows
    [[nodiscard]] ListCtrlExDataProvider* GetSummaryStatisticsListData() noexcept
        { return m_statsListData; }
    [[nodiscard]] ExplanationListCtrl* GetReadabilityScoresList()
        { return dynamic_cast<ExplanationListCtrl*>(GetReadabilityResultsView().FindWindowById(READABILITY_SCORES_PAGE_ID)); }

    void OnAddTest(wxCommandEvent& event);
private:
    //menu commands
    void OnTextWindowColorsChange([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnTextWindowFontChange([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnGradeScale(wxCommandEvent& event);
    void OnExportAll([[maybe_unused]] wxCommandEvent& event);
    void OnExportFilteredDocument([[maybe_unused]] wxCommandEvent& event);
    void OnLongFormat([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnTestDelete([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnTestDeleteMenu([[maybe_unused]] wxCommandEvent& event);
    void OnAddToDictionary([[maybe_unused]] wxCommandEvent& event);
    void OnLaunchSourceFile([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnEditGraphOptions(wxCommandEvent& event);
    //handles more generic events that may need to be handled by the window itself (e.g., a graph printing)
    void OnMenuCommand(wxCommandEvent& event);
    void OnRibbonButtonCommand(wxRibbonButtonBarEvent& event);
    void OnItemSelected(wxCommandEvent& event);
    bool OnCreate(wxDocument* doc, long flags) final;
    void OnListDblClick(wxListEvent& event);
    void OnTestListDblClick([[maybe_unused]] wxListEvent& event);
    void OnHyperlinkClicked(wxHtmlLinkEvent& event);
    void OnSummation([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnFind(wxFindDialogEvent &event);

    void UpdateSideBarIcons();
    void UpdateStatistics();
    //view classes
    WindowContainer m_readabilityResultsView;
    WindowContainer m_summaryView;
    WindowContainer m_WordsBreakdownView;
    WindowContainer m_sentencesBreakdownView;
    WindowContainer m_sightWordView;
    WindowContainer m_grammarView;

    ListCtrlExDataProvider* m_statsListData{ nullptr };

    wxDECLARE_DYNAMIC_CLASS(ProjectView);
    wxDECLARE_EVENT_TABLE();
    };

#endif //__PROJECT_VIEW_H__
