#include "standard_project_doc.h"
#include "standard_project_view.h"
#include "../app/readability_app.h"
#include "../ui/dialogs/export_all_dlg.h"
#include "../ui/dialogs/tools_options_dlg.h"
#include "../ui/dialogs/filtered_text_preview_dlg.h"
#include "../ui/dialogs/filtered_text_export_options_dlg.h"
#include "../results_format/project_report_format.h"
#include "../Wisteria-Dataviz/src/ui/dialogs/radioboxdlg.h"
#include "../Wisteria-Dataviz/src/ui/dialogs/gridexportdlg.h"
#include "../Wisteria-Dataviz/src/graphs/heatmap.h"
#include "../Wisteria-Dataviz/src/graphs/wordcloud.h"
#include "../Wisteria-Dataviz/src/graphs/piechart.h"
#include "../Wisteria-Dataviz/src/import/html_encode.h"

using namespace lily_of_the_valley;
using namespace Wisteria;
using namespace Wisteria::Graphs;
using namespace Wisteria::UI;

wxDECLARE_APP(ReadabilityApp);

wxIMPLEMENT_DYNAMIC_CLASS(ProjectView, BaseProjectView)

wxBEGIN_EVENT_TABLE(ProjectView, BaseProjectView)
    EVT_HTML_LINK_CLICKED(wxID_ANY, ProjectView::OnHyperlinkClicked)
    EVT_MENU(XRCID("ID_EXPORT_FILTERED_DOCUMENT"), ProjectView::OnExportFilteredDocument)
    EVT_MENU(XRCID("ID_EXPORT_ALL"), ProjectView::OnExportAll)
    EVT_RIBBONBUTTONBAR_CLICKED(XRCID("ID_LAUNCH_SOURCE_FILE"), ProjectView::OnLaunchSourceFile)
    EVT_RIBBONBUTTONBAR_CLICKED(XRCID("ID_TEXT_WINDOW_FONT"), ProjectView::OnTextWindowFontChange)
    EVT_RIBBONBUTTONBAR_CLICKED(XRCID("ID_TEXT_WINDOW_COLORS"), ProjectView::OnTextWindowColorsChange)
    EVT_MENU(XRCID("ID_ADD_ITEM_TO_DICTIONARY"), ProjectView::OnAddToDictionary)
    EVT_MENU(XRCID("ID_K12_US"), ProjectView::OnGradeScale)
    EVT_MENU(XRCID("ID_K12_NEWFOUNDLAND"), ProjectView::OnGradeScale)
    EVT_MENU(XRCID("ID_K12_BC"), ProjectView::OnGradeScale)
    EVT_MENU(XRCID("ID_K12_NEW_BRUNSWICK"), ProjectView::OnGradeScale)
    EVT_MENU(XRCID("ID_K12_NOVA_SCOTIA"), ProjectView::OnGradeScale)
    EVT_MENU(XRCID("ID_K12_ONTARIO"), ProjectView::OnGradeScale)
    EVT_MENU(XRCID("ID_K12_SASKATCHEWAN"), ProjectView::OnGradeScale)
    EVT_MENU(XRCID("ID_K12_PE"), ProjectView::OnGradeScale)
    EVT_MENU(XRCID("ID_K12_MANITOBA"), ProjectView::OnGradeScale)
    EVT_MENU(XRCID("ID_K12_NT"), ProjectView::OnGradeScale)
    EVT_MENU(XRCID("ID_K12_ALBERTA"), ProjectView::OnGradeScale)
    EVT_MENU(XRCID("ID_K12_NUNAVUT"), ProjectView::OnGradeScale)
    EVT_MENU(XRCID("ID_QUEBEC"), ProjectView::OnGradeScale)
    EVT_MENU(XRCID("ID_ENGLAND"), ProjectView::OnGradeScale)
    EVT_RIBBONBUTTONBAR_CLICKED(XRCID("ID_LONG_FORMAT"), ProjectView::OnLongFormat)
    EVT_RIBBONBUTTONBAR_CLICKED(XRCID("ID_REMOVE_TEST"), ProjectView::OnTestDelete)
    EVT_MENU(XRCID("ID_REMOVE_TEST"), ProjectView::OnTestDeleteMenu)
    EVT_RIBBONBUTTONBAR_CLICKED(wxID_SELECTALL, ProjectView::OnRibbonButtonCommand)
    EVT_RIBBONBUTTONBAR_CLICKED(wxID_NEW, ProjectView::OnRibbonButtonCommand)
    EVT_RIBBONBUTTONBAR_CLICKED(wxID_OPEN, ProjectView::OnRibbonButtonCommand)
    EVT_RIBBONBUTTONBAR_CLICKED(wxID_SAVE, ProjectView::OnRibbonButtonCommand)
    EVT_RIBBONBUTTONBAR_CLICKED(wxID_COPY, ProjectView::OnRibbonButtonCommand)
    EVT_RIBBONBUTTONBAR_CLICKED(wxID_PREVIEW, ProjectView::OnRibbonButtonCommand)
    EVT_RIBBONBUTTONBAR_CLICKED(wxID_PRINT, ProjectView::OnRibbonButtonCommand)
    EVT_RIBBONBUTTONBAR_CLICKED(wxID_ZOOM_IN, ProjectView::OnRibbonButtonCommand)
    EVT_RIBBONBUTTONBAR_CLICKED(wxID_ZOOM_OUT, ProjectView::OnRibbonButtonCommand)
    EVT_RIBBONBUTTONBAR_CLICKED(wxID_ZOOM_FIT, ProjectView::OnRibbonButtonCommand)
    EVT_RIBBONBUTTONBAR_CLICKED(XRCID("ID_SAVE_ITEM"), ProjectView::OnRibbonButtonCommand)
    EVT_RIBBONBUTTONBAR_CLICKED(XRCID("ID_COPY_ALL"), ProjectView::OnRibbonButtonCommand)
    EVT_RIBBONBUTTONBAR_CLICKED(XRCID("ID_COPY_WITH_COLUMN_HEADERS"), ProjectView::OnRibbonButtonCommand)
    EVT_RIBBONBUTTONBAR_CLICKED(XRCID("ID_COPY_FIRST_COLUMN"), ProjectView::OnRibbonButtonCommand)
    EVT_RIBBONBUTTONBAR_CLICKED(XRCID("ID_VIEW_ITEM"), ProjectView::OnRibbonButtonCommand)
    EVT_RIBBONBUTTONBAR_CLICKED(XRCID("ID_LIST_SORT"), ProjectView::OnRibbonButtonCommand)
    EVT_RIBBONBUTTONBAR_CLICKED(XRCID("ID_SORT_ASCENDING"), ProjectView::OnRibbonButtonCommand)
    EVT_RIBBONBUTTONBAR_CLICKED(XRCID("ID_SORT_DESCENDING"), ProjectView::OnRibbonButtonCommand)
    EVT_RIBBONBUTTONBAR_CLICKED(XRCID("ID_EXCLUDE_SELECTED"), ProjectView::OnRibbonButtonCommand)
    EVT_RIBBONBUTTONBAR_CLICKED(XRCID("ID_MULTI_COLUMN_SORT_ASCENDING"), ProjectView::OnRibbonButtonCommand)
    EVT_RIBBONBUTTONBAR_CLICKED(XRCID("ID_MULTI_COLUMN_SORT_DESCENDING"), ProjectView::OnRibbonButtonCommand)
    EVT_MENU(wxID_SELECTALL, ProjectView::OnMenuCommand)
    EVT_MENU(wxID_COPY, ProjectView::OnMenuCommand)
    EVT_MENU(wxID_PREVIEW, ProjectView::OnMenuCommand)
    EVT_MENU(wxID_PRINT, ProjectView::OnMenuCommand)
    EVT_MENU(wxID_ZOOM_IN, ProjectView::OnMenuCommand)
    EVT_MENU(wxID_ZOOM_OUT, ProjectView::OnMenuCommand)
    EVT_MENU(wxID_ZOOM_FIT, ProjectView::OnMenuCommand)
    EVT_MENU(XRCID("ID_SAVE_ITEM"), ProjectView::OnMenuCommand)
    EVT_MENU(XRCID("ID_COPY_ALL"), ProjectView::OnMenuCommand)
    EVT_MENU(XRCID("ID_COPY_WITH_COLUMN_HEADERS"), ProjectView::OnMenuCommand)
    EVT_MENU(XRCID("ID_COPY_FIRST_COLUMN"), ProjectView::OnMenuCommand)
    EVT_MENU(XRCID("ID_VIEW_ITEM"), ProjectView::OnMenuCommand)
    EVT_MENU(XRCID("ID_LIST_SORT"), ProjectView::OnMenuCommand)
    EVT_MENU(XRCID("ID_SORT_ASCENDING"), ProjectView::OnMenuCommand)
    EVT_MENU(XRCID("ID_SORT_DESCENDING"), ProjectView::OnMenuCommand)
    EVT_MENU(XRCID("ID_EXCLUDE_SELECTED"), ProjectView::OnMenuCommand)
    EVT_MENU(XRCID("ID_MULTI_COLUMN_SORT_ASCENDING"), ProjectView::OnMenuCommand)
    EVT_MENU(XRCID("ID_MULTI_COLUMN_SORT_DESCENDING"), ProjectView::OnMenuCommand)
    // not actually a test (it doesn't have an ID), but we'll add it in this function
    EVT_MENU(XRCID("ID_DOLCH"), ProjectView::OnAddTest)
    EVT_FIND(wxID_ANY, ProjectView::OnFind)
    EVT_FIND_NEXT(wxID_ANY, ProjectView::OnFind)
    EVT_FIND_CLOSE(wxID_ANY, ProjectView::OnFind)
    EVT_COMMAND(ProjectView::LEFT_PANE, EVT_SIDEBAR_CLICK, ProjectView::OnItemSelected)
    EVT_LIST_ITEM_ACTIVATED(HARD_WORDS_LIST_PAGE_ID, ProjectView::OnListDblClick)
    EVT_LIST_ITEM_ACTIVATED(LONG_WORDS_LIST_PAGE_ID, ProjectView::OnListDblClick)
    EVT_LIST_ITEM_ACTIVATED(DC_WORDS_LIST_PAGE_ID, ProjectView::OnListDblClick)
    EVT_LIST_ITEM_ACTIVATED(HARRIS_JACOBSON_WORDS_LIST_PAGE_ID, ProjectView::OnListDblClick)
    EVT_LIST_ITEM_ACTIVATED(SPACHE_WORDS_LIST_PAGE_ID, ProjectView::OnListDblClick)
    EVT_LIST_ITEM_ACTIVATED(ALL_WORDS_LIST_PAGE_ID, ProjectView::OnListDblClick)
    EVT_LIST_ITEM_ACTIVATED(ALL_WORDS_CONDENSED_LIST_PAGE_ID, ProjectView::OnListDblClick)
    EVT_LIST_ITEM_ACTIVATED(PROPER_NOUNS_LIST_PAGE_ID, ProjectView::OnListDblClick)
    EVT_LIST_ITEM_ACTIVATED(OVERUSED_WORDS_BY_SENTENCE_LIST_PAGE_ID, ProjectView::OnListDblClick)
    EVT_LIST_ITEM_ACTIVATED(DOLCH_WORDS_LIST_PAGE_ID, ProjectView::OnListDblClick)
    EVT_LIST_ITEM_ACTIVATED(NON_DOLCH_WORDS_LIST_PAGE_ID, ProjectView::OnListDblClick)
    EVT_LIST_ITEM_ACTIVATED(LONG_SENTENCES_LIST_PAGE_ID, ProjectView::OnListDblClick)
    EVT_LIST_ITEM_ACTIVATED(SENTENCES_CONJUNCTION_START_LIST_PAGE_ID, ProjectView::OnListDblClick)
    EVT_LIST_ITEM_ACTIVATED(SENTENCES_LOWERCASE_START_LIST_PAGE_ID, ProjectView::OnListDblClick)
    EVT_LIST_ITEM_ACTIVATED(DUPLICATES_LIST_PAGE_ID, ProjectView::OnListDblClick)
    EVT_LIST_ITEM_ACTIVATED(INCORRECT_ARTICLE_PAGE_ID, ProjectView::OnListDblClick)
    EVT_LIST_ITEM_ACTIVATED(PASSIVE_VOICE_PAGE_ID, ProjectView::OnListDblClick)
    EVT_LIST_ITEM_ACTIVATED(MISSPELLED_WORD_LIST_PAGE_ID, ProjectView::OnListDblClick)
    EVT_LIST_ITEM_ACTIVATED(CLICHES_LIST_PAGE_ID, ProjectView::OnListDblClick)
    EVT_LIST_ITEM_ACTIVATED(WORDY_PHRASES_LIST_PAGE_ID, ProjectView::OnListDblClick)
    EVT_LIST_ITEM_ACTIVATED(REDUNDANT_PHRASE_LIST_PAGE_ID, ProjectView::OnListDblClick)
    EVT_LIST_ITEM_ACTIVATED(WORDING_ERRORS_LIST_PAGE_ID, ProjectView::OnListDblClick)
    EVT_LIST_ITEM_ACTIVATED(READABILITY_SCORES_PAGE_ID, ProjectView::OnTestListDblClick)
    EVT_COMMAND(wxID_ANY, EVT_WISTERIA_CANVAS_DCLICK, ProjectView::OnEditGraphOptions)
    EVT_RIBBONBUTTONBAR_CLICKED(XRCID("ID_SUMMATION"), ProjectView::OnSummation)
wxEND_EVENT_TABLE()

//------------------------------------------------------
ProjectView::ProjectView() :
        m_statsListData(new ListCtrlExDataProvider)
        {
        Bind(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this,
             SIDEBAR_CUSTOM_TESTS_START_ID, SIDEBAR_CUSTOM_TESTS_START_ID + 1000);

        Bind(wxEVT_RIBBONBUTTONBAR_CLICKED,
            &ProjectView::OnRealTimeUpdate, this,
            XRCID("ID_REALTIME_UPDATE"));
        }

//------------------------------------------------------
void ProjectView::OnRealTimeUpdate([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    ProjectDoc* projDoc = dynamic_cast<ProjectDoc*>(GetDocument());
    if (projDoc)
        {
        if (projDoc->IsRealTimeUpdating())
            {
            projDoc->UseRealTimeUpdate(false);
            projDoc->StopRealtimeUpdate();
            }
        else
            {
            projDoc->UseRealTimeUpdate(true);
            projDoc->RestartRealtimeUpdate();
            }
        }
    }

//------------------------------------------------------
void ProjectView::OnSummation([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    const ProjectDoc* doc = dynamic_cast<const ProjectDoc*>(GetDocument());
    ListCtrlItemViewDlg viewDlg;
    if (GetActiveProjectWindow()->GetId() == MISSPELLED_WORD_LIST_PAGE_ID)
        {
        viewDlg.AddValue(_(L"Total Misspellings"),
            wxNumberFormatter::ToString(doc->GetMisspelledWordData()->GetColumnSum(1), 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|wxNumberFormatter::Style::Style_WithThousandsSep));
        }
    else if (GetActiveProjectWindow()->GetId() == PASSIVE_VOICE_PAGE_ID)
        {
        viewDlg.AddValue(_(L"Total Passive Phrases"),
            wxNumberFormatter::ToString(doc->GetPassiveVoiceData()->GetColumnSum(1), 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|wxNumberFormatter::Style::Style_WithThousandsSep));
        }
    else if (GetActiveProjectWindow()->GetId() == HARD_WORDS_LIST_PAGE_ID)
        {
        viewDlg.AddValue(_(L"Total 3+ Syllable Words"),
            wxNumberFormatter::ToString(doc->Get3SyllablePlusData()->GetColumnSum(2), 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|wxNumberFormatter::Style::Style_WithThousandsSep));
        }
    else if (GetActiveProjectWindow()->GetId() == LONG_WORDS_LIST_PAGE_ID)
        {
        viewDlg.AddValue(_(L"Total 6+ Character Words"),
            wxNumberFormatter::ToString(doc->Get6CharacterPlusData()->GetColumnSum(2), 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|wxNumberFormatter::Style::Style_WithThousandsSep));
        }
    else if (GetActiveProjectWindow()->GetId() == DC_WORDS_LIST_PAGE_ID)
        {
        viewDlg.AddValue(_(L"Total Dale-Chall Unfamiliar Words"),
            wxNumberFormatter::ToString(doc->GetDaleChallHardWordData()->GetColumnSum(1), 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|wxNumberFormatter::Style::Style_WithThousandsSep));
        }
    else if (GetActiveProjectWindow()->GetId() == SPACHE_WORDS_LIST_PAGE_ID)
        {
        viewDlg.AddValue(_(L"Total Spache Unfamiliar Words"),
            wxNumberFormatter::ToString(doc->GetSpacheHardWordData()->GetColumnSum(1), 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|wxNumberFormatter::Style::Style_WithThousandsSep));
        }
    else if (GetActiveProjectWindow()->GetId() == HARRIS_JACOBSON_WORDS_LIST_PAGE_ID)
        {
        viewDlg.AddValue(_(L"Total Harris-Jacobson Unfamiliar Words"),
            wxNumberFormatter::ToString(doc->GetHarrisJacobsonHardWordDataData()->GetColumnSum(1), 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|wxNumberFormatter::Style::Style_WithThousandsSep));
        }
    else if (GetActiveProjectWindow()->GetId() == ALL_WORDS_LIST_PAGE_ID)
        {
        viewDlg.AddValue(_(L"Total Words"),
            wxNumberFormatter::ToString(doc->GetAllWordsBaseData()->GetColumnSum(1), 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|wxNumberFormatter::Style::Style_WithThousandsSep));
        }
    else if (GetActiveProjectWindow()->GetId() == ALL_WORDS_CONDENSED_LIST_PAGE_ID)
        {
        viewDlg.AddValue(_(L"Total Words"),
            wxNumberFormatter::ToString(doc->GetKeyWordsBaseData()->GetColumnSum(1), 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|wxNumberFormatter::Style::Style_WithThousandsSep)); }
    else if (GetActiveProjectWindow()->GetId() == PROPER_NOUNS_LIST_PAGE_ID)
        {
        viewDlg.AddValue(_(L"Total Proper Nouns"),
            wxNumberFormatter::ToString(doc->GetProperNounsData()->GetColumnSum(1), 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|wxNumberFormatter::Style::Style_WithThousandsSep));
        }
    else if (GetActiveProjectWindow()->GetId() == DOLCH_WORDS_LIST_PAGE_ID)
        {
        viewDlg.AddValue(_(L"Total Dolch Words"),
            wxNumberFormatter::ToString(doc->GetDolchWordData()->GetColumnSum(1), 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|wxNumberFormatter::Style::Style_WithThousandsSep));
        }
    else if (GetActiveProjectWindow()->GetId() == NON_DOLCH_WORDS_LIST_PAGE_ID)
        {
        viewDlg.AddValue(_(L"Total Non-Dolch Words"),
            wxNumberFormatter::ToString(doc->GetNonDolchWordData()->GetColumnSum(1), 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|wxNumberFormatter::Style::Style_WithThousandsSep));
        }
    else
        {
        wxString customTestName;
        for (size_t i = 0; i < BaseProject::m_custom_word_tests.size(); ++i)
            {
            if (BaseProject::m_custom_word_tests[i].get_interface_id() == GetActiveProjectWindow()->GetId())
                {
                customTestName = BaseProject::m_custom_word_tests[i].get_name().c_str();
                break;
                }
            }
        if (doc->HasCustomTest(customTestName))
            {
            viewDlg.AddValue(wxString::Format(_(L"Total %s Unfamiliar Words"), customTestName),
                wxNumberFormatter::ToString(doc->GetCustomTest(customTestName)->GetListViewData()->GetColumnSum(1), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep));
            }
        }
    viewDlg.Create(GetDocFrame(), wxID_ANY, _(L"Column Summations"));
    viewDlg.ShowModal();
    }

//------------------------------------------------------
void ProjectView::OnExportAll([[maybe_unused]] wxCommandEvent& event)
    {
    ProjectDoc* doc = dynamic_cast<ProjectDoc*>(GetDocument());
    if (!doc->IsSafeToUpdate())
        { return; }
    wxArrayString choices, descriptions;
    choices.Add(_(L"Single report"));
    descriptions.Add(_(L"Saves the results into a single <span style='font-weight: bold;'>HTML</span> report."));
    choices.Add(_(L"Separate files"));
    descriptions.Add(_(L"Saves each result window to a separate file."));
    Wisteria::UI::RadioBoxDlg exportTypesDlg(GetDocFrame(),
        _(L"Select How to Export"), wxString{}, _(L"Export methods:"), _(L"Export All"),
        choices, descriptions);
    if (exportTypesDlg.ShowModal() != wxID_OK)
        { return; }

    ExportAllDlg dlg(GetDocFrame(), doc, (exportTypesDlg.GetSelection() == 0));
    if (m_activeWindow && m_activeWindow->GetClientSize().IsFullySpecified())
        { dlg.GetImageExportOptions().m_imageSize = m_activeWindow->GetClientSize(); }
    dlg.SetHelpTopic(wxGetApp().GetMainFrame()->GetHelpDirectory(), L"export-all-options.html");
    if (dlg.ShowModal() != wxID_OK || dlg.GetFolderPath().empty())
        { return; }

    if (exportTypesDlg.GetSelection() == 0)
        {
        ExportAllToHtml(dlg.GetFilePath(), dlg.GetExportGraphExt(),
            dlg.IsExportingHardWordLists(), dlg.IsExportingSentencesBreakdown(),
            dlg.IsExportingTestResults(), dlg.IsExportingStatistics(), dlg.IsExportingGrammar(),
            dlg.IsExportingSightWords(),
            dlg.IsExportingLists(),
            dlg.IsExportingTextReports(),
            dlg.GetImageExportOptions());
        }
    else
        {
        ExportAll(dlg.GetFolderPath(), dlg.GetExportListExt(), dlg.GetExportTextViewExt(), dlg.GetExportGraphExt(),
            dlg.IsExportingHardWordLists(), dlg.IsExportingSentencesBreakdown(),
            dlg.IsExportingTestResults(), dlg.IsExportingStatistics(), dlg.IsExportingGrammar(),
            dlg.IsExportingSightWords(),
            dlg.IsExportingLists(),
            dlg.IsExportingTextReports(),
            dlg.GetImageExportOptions());
        }
    doc->SetExportFile(dlg.GetFilePath());
    doc->SetExportFolder(dlg.GetFolderPath());
    doc->SetExportListExt(dlg.GetExportListExt());
    doc->SetExportTextViewExt(dlg.GetExportTextViewExt());
    doc->SetExportGraphExt(dlg.GetExportGraphExt());
    doc->ExportHardWordLists(dlg.IsExportingHardWordLists());
    doc->ExportSentencesBreakdown(dlg.IsExportingSentencesBreakdown());
    doc->ExportTestResults(dlg.IsExportingTestResults());
    doc->ExportStatistics(dlg.IsExportingStatistics());
    doc->ExportWordiness(dlg.IsExportingGrammar());
    doc->ExportSightWords(dlg.IsExportingSightWords());
    doc->ExportLists(dlg.IsExportingLists());
    doc->ExportTextReports(dlg.IsExportingTextReports());
    doc->GetImageExportOptions() = dlg.GetImageExportOptions();
    // export folder paths may have changed
    doc->SetModifiedFlag();
    }

//------------------------------------------------------
void ProjectView::OnExportFilteredDocument([[maybe_unused]] wxCommandEvent& event)
    {
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureProfessionalCode()))
        {
        wxMessageBox(
            _(L"Filtered document exporting is only available in the Professional Edition of Readability Studio."),
            _(L"Feature Not Licensed"), wxOK|wxICON_INFORMATION);
        return;
        }
    const BaseProjectDoc* doc = dynamic_cast<const BaseProjectDoc*>(GetDocument());
    wxFileDialog fdialog(GetDocFrame(),
                    _(L"Export Filtered Document"),
                    wxString{},
                    doc->GetTitle(),
                    _(L"Text Files (*.txt)|*.txt"),
                    wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
    if (fdialog.ShowModal() != wxID_OK)
        { return; }

    FilteredTextExportOptionsDlg optDlg(GetDocFrame());
    optDlg.SetHelpTopic(wxGetApp().GetMainFrame()->GetHelpDirectory(), L"filtering-export.html");
    if (optDlg.ShowModal() != wxID_OK)
        { return; }

    wxString validDocText;
    doc->FormatFilteredText(validDocText, optDlg.IsReplacingCharacters(),
        optDlg.IsRemovingEllipses(), optDlg.IsRemovingBullets(),
        optDlg.IsRemovingFilePaths(), optDlg.IsStrippingAbbreviations(),
        optDlg.IsNarrowingFullWidthCharacters());

    FilteredTextPreviewDlg dlg(GetDocFrame(), doc->GetInvalidSentenceMethod(),
        doc->IsIgnoringTrailingCopyrightNoticeParagraphs(), doc->IsIgnoringTrailingCitations(),
        optDlg.IsReplacingCharacters(), optDlg.IsRemovingEllipses(), optDlg.IsRemovingBullets(),
        optDlg.IsRemovingFilePaths(), optDlg.IsStrippingAbbreviations());
    dlg.SetHelpTopic(wxGetApp().GetMainFrame()->GetHelpDirectory(), L"how-text-is-excluded.html");
    dlg.SetFilteredValue(validDocText);
    if (dlg.ShowModal() == wxID_OK)
        {
        wxFileName(fdialog.GetPath()).SetPermissions(wxS_DEFAULT);
        wxFile filteredFile(fdialog.GetPath(), wxFile::write);
        if (!filteredFile.Write(validDocText))
            {
            wxMessageBox(_(L"Unable to write to output file."),
                _(L"Error"), wxOK|wxICON_EXCLAMATION);
            }
        }
    }

//------------------------------------------------------
void ProjectView::OnEditGraphOptions(wxCommandEvent& event)
    {
    ToolsOptionsDlg optionsDlg(GetDocFrame(), dynamic_cast<ProjectDoc*>(GetDocument()),
                               ToolsOptionsDlg::GraphsSection);
    switch (event.GetId())
        {
    case FRY_PAGE_ID:
        [[fallthrough]];
    case RAYGOR_PAGE_ID:
        [[fallthrough]];
    case GPM_FRY_PAGE_ID:
        [[fallthrough]];
    case FRASE_PAGE_ID:
        [[fallthrough]];
    case FLESCH_CHART_PAGE_ID:
        [[fallthrough]];
    case CRAWFORD_GRAPH_PAGE_ID:
        [[fallthrough]];
    case SCHWARTZ_PAGE_ID:
        [[fallthrough]];
    case LIX_GAUGE_PAGE_ID:
        [[fallthrough]];
    case LIX_GAUGE_GERMAN_PAGE_ID:
        [[fallthrough]];
    case DB2_PAGE_ID:
        optionsDlg.SelectPage(ToolsOptionsDlg::GRAPH_READABILITY_GRAPHS_PAGE);
        break;
    case WORD_BREAKDOWN_PAGE_ID:
        [[fallthrough]];
    case DOLCH_COVERAGE_CHART_PAGE_ID:
        [[fallthrough]];
    case DOLCH_BREAKDOWN_PAGE_ID:
        optionsDlg.SelectPage(ToolsOptionsDlg::GRAPH_BAR_CHART_PAGE);
        break;
    case SENTENCE_BOX_PLOT_PAGE_ID:
        optionsDlg.SelectPage(ToolsOptionsDlg::GRAPH_BOX_PLOT_PAGE);
        break;
    case SENTENCE_HISTOGRAM_PAGE_ID:
        [[fallthrough]];
    case SYLLABLE_HISTOGRAM_PAGE_ID:
        optionsDlg.SelectPage(ToolsOptionsDlg::GRAPH_HISTOGRAM_PAGE);
        break;
    default:
        optionsDlg.SelectPage(ToolsOptionsDlg::GRAPH_GENERAL_PAGE);
        };
    // don't refresh the whole project, just update the graphs
    if (optionsDlg.ShowModal() == wxID_OK)
        { dynamic_cast<ProjectDoc*>(GetDocument())->RefreshGraphs(); }
    }

//------------------------------------------------------
void ProjectView::OnHyperlinkClicked(wxHtmlLinkEvent& event)
    {
    if (event.GetLinkInfo().GetHref() == L"#UnusedDolchWords")
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(UNUSED_DOLCH_WORDS_LIST_PAGE_ID));
        }
    else if (event.GetLinkInfo().GetHref() == L"#Dolch")
        {
        GetSideBar()->SelectFolder(GetSideBar()->FindFolder(SIDEBAR_DOLCH_SECTION_ID));
        }
    else if (event.GetLinkInfo().GetHref() == L"#SelectStatistics")
        {
        BaseProjectDoc* theProject = dynamic_cast<BaseProjectDoc*>(GetDocument());
        ToolsOptionsDlg optionsDlg(GetDocFrame(), theProject, ToolsOptionsDlg::Statistics);
        optionsDlg.SelectPage(ToolsOptionsDlg::ANALYSIS_STATISTICS_PAGE);
        if (optionsDlg.ShowModal() == wxID_OK)
            { theProject->RefreshStatisticsReports(); }
        }
    else if (event.GetLinkInfo().GetHref() == L"#FryGraph")
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(FRY_PAGE_ID));
        }
    else if (event.GetLinkInfo().GetHref() == L"#FleschChart")
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(FLESCH_CHART_PAGE_ID));
        }
    else if (event.GetLinkInfo().GetHref() == L"#DB2")
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(DB2_PAGE_ID));
        }
    else if (event.GetLinkInfo().GetHref() == L"#LixGauge")
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(LIX_GAUGE_PAGE_ID));
        }
    else if (event.GetLinkInfo().GetHref() == L"#GermanLixGauge")
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(LIX_GAUGE_GERMAN_PAGE_ID));
        }
    else if (event.GetLinkInfo().GetHref() == L"#CrawfordGraph")
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(CRAWFORD_GRAPH_PAGE_ID));
        }
    else if (event.GetLinkInfo().GetHref() == L"#GPMFryGraph")
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(GPM_FRY_PAGE_ID));
        }
    else if (event.GetLinkInfo().GetHref() == L"#FRASE")
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(FRASE_PAGE_ID));
        }
    else if (event.GetLinkInfo().GetHref() == L"#Schwartz")
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(SCHWARTZ_PAGE_ID));
        }
    else if (event.GetLinkInfo().GetHref() == L"#RaygorGraph")
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(RAYGOR_PAGE_ID));
        }
    else if (event.GetLinkInfo().GetHref() == L"#DifficultSentences")
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(LONG_SENTENCES_LIST_PAGE_ID));
        }
    else if (event.GetLinkInfo().GetHref() == L"#SentenceStartingWithConjunctions")
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(SENTENCES_CONJUNCTION_START_LIST_PAGE_ID));
        }
    else if (event.GetLinkInfo().GetHref() == L"#SentenceStartingWithLowercase")
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(SENTENCES_LOWERCASE_START_LIST_PAGE_ID));
        }
    else if (event.GetLinkInfo().GetHref() == L"#Misspellings")
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(MISSPELLED_WORD_LIST_PAGE_ID));
        }
    else if (event.GetLinkInfo().GetHref() == L"#RepeatedWords")
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(DUPLICATES_LIST_PAGE_ID));
        }
    else if (event.GetLinkInfo().GetHref() == L"#MismatchedArtcles")
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(INCORRECT_ARTICLE_PAGE_ID));
        }
    else if (event.GetLinkInfo().GetHref() == L"#PassiveVoice")
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(PASSIVE_VOICE_PAGE_ID));
        }
    else if (event.GetLinkInfo().GetHref() == L"#WordyPhrases")
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(WORDY_PHRASES_LIST_PAGE_ID));
        }
    else if (event.GetLinkInfo().GetHref() == L"#RedundantPhrases")
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(REDUNDANT_PHRASE_LIST_PAGE_ID));
        }
    else if (event.GetLinkInfo().GetHref() == L"#OverusedWordsBySentence")
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(OVERUSED_WORDS_BY_SENTENCE_LIST_PAGE_ID));
        }
    else if (event.GetLinkInfo().GetHref() == L"#WordingErrors")
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(WORDING_ERRORS_LIST_PAGE_ID));
        }
    else if (event.GetLinkInfo().GetHref() == L"#Cliches")
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(CLICHES_LIST_PAGE_ID));
        }
    else if (event.GetLinkInfo().GetHref() == L"#LongWords")
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(LONG_WORDS_LIST_PAGE_ID));
        }
    else if (event.GetLinkInfo().GetHref() == L"#HardWords")
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(HARD_WORDS_LIST_PAGE_ID));
        }
    else if (event.GetLinkInfo().GetHref() == L"#DaleChallWords")
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(DC_WORDS_LIST_PAGE_ID));
        }
    else if (event.GetLinkInfo().GetHref() == L"#SpacheWords")
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(SPACHE_WORDS_LIST_PAGE_ID));
        }
    else if (event.GetLinkInfo().GetHref() == L"#HarrisJacobsonWords")
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(HARRIS_JACOBSON_WORDS_LIST_PAGE_ID));
        }
    else if (event.GetLinkInfo().GetHref() == L"#FogHelp")
        { wxGetApp().GetMainFrame()->DisplayHelp(L"gunning-fog-test.html"); }
    else if (event.GetLinkInfo().GetHref().length() && event.GetLinkInfo().GetHref().GetChar(0) == L'#')
        { event.Skip(); }
    else
        { wxGetApp().GetMainFrame()->DisplayHelp(event.GetLinkInfo().GetHref()); }
    }

//------------------------------------------------------
void ProjectView::OnTestListDblClick([[maybe_unused]] wxListEvent& event)
    {
    const wxString selectedTest = GetReadabilityScoresList()->GetResultsListCtrl()->GetSelectedText();
    CustomReadabilityTestCollection::iterator testIter =
        std::find(BaseProject::m_custom_word_tests.begin(), BaseProject::m_custom_word_tests.end(), selectedTest);
    const std::pair<std::vector<readability::readability_test>::const_iterator, bool> testPos =
        BaseProject::GetDefaultReadabilityTestsTemplate().find_test(selectedTest);

    if (testIter != BaseProject::m_custom_word_tests.end())
        { wxGetApp().EditCustomTest(*testIter); }
    else if (testPos.second)
        { wxGetApp().GetMainFrame()->DisplayHelp(wxString::Format(L"%s.html", testPos.first->get_id().c_str())); }
    else if (selectedTest == _(L"Dolch Sight Words"))
        { wxGetApp().GetMainFrame()->DisplayHelp(L"reviewing-dolch.html.html"); }
    }

/// Double clicking on an item in the hard word list will jump to the respective text window and find the word
/// that you clicked on.
//------------------------------------------------------
void ProjectView::OnListDblClick(wxListEvent& event)
    {
    wxString searchText;
    wxString replacementText;
    wxWindow* foundWindow{ nullptr };
    wxWindowID textId{ wxNOT_FOUND };

    const auto readSuggestionColumn = [&replacementText](const auto listCtrl)
    {
        replacementText.clear();
        const long selected = listCtrl->GetFirstSelected();
        if (selected != wxNOT_FOUND)
            {
            replacementText = listCtrl->GetItemText(selected, listCtrl->GetColumnCount() - 1);
            const auto tokens = wxStringTokenize(replacementText, L";,");
            if (tokens.size())
                {
                replacementText = tokens[0];
                }
            }
    };

    switch (event.GetId())
        {
    case HARD_WORDS_LIST_PAGE_ID:
        foundWindow = GetWordsBreakdownView().FindWindowById(event.GetId());
        if (foundWindow)
            {
            auto listCtrl{ dynamic_cast<const ListCtrlEx*>(foundWindow) };
            searchText = listCtrl->GetSelectedText();
            readSuggestionColumn(listCtrl);
            }
        textId = HARD_WORDS_TEXT_PAGE_ID;
        break;
    case LONG_WORDS_LIST_PAGE_ID:
        foundWindow = GetWordsBreakdownView().FindWindowById(event.GetId());
        if (foundWindow)
            {
            auto listCtrl{ dynamic_cast<const ListCtrlEx*>(foundWindow) };
            searchText = listCtrl->GetSelectedText();
            readSuggestionColumn(listCtrl);
            }
        textId = LONG_WORDS_TEXT_PAGE_ID;
        break;
    case DC_WORDS_LIST_PAGE_ID:
        foundWindow = GetWordsBreakdownView().FindWindowById(event.GetId());
        if (foundWindow)
            {
            auto listCtrl{ dynamic_cast<const ListCtrlEx*>(foundWindow) };
            searchText = listCtrl->GetSelectedText();
            readSuggestionColumn(listCtrl);
            }
        textId = DC_WORDS_TEXT_PAGE_ID;
        break;
    case HARRIS_JACOBSON_WORDS_LIST_PAGE_ID:
        foundWindow = GetWordsBreakdownView().FindWindowById(event.GetId());
        if (foundWindow)
            {
            auto listCtrl{ dynamic_cast<const ListCtrlEx*>(foundWindow) };
            searchText = listCtrl->GetSelectedText();
            readSuggestionColumn(listCtrl);
            }
        textId = HARRIS_JACOBSON_WORDS_TEXT_PAGE_ID;
        break;
    case SPACHE_WORDS_LIST_PAGE_ID:
        foundWindow = GetWordsBreakdownView().FindWindowById(event.GetId());
        if (foundWindow)
            {
            auto listCtrl{ dynamic_cast<const ListCtrlEx*>(foundWindow) };
            searchText = listCtrl->GetSelectedText();
            readSuggestionColumn(listCtrl);
            }
        textId = SPACHE_WORDS_TEXT_PAGE_ID;
        break;
    case ALL_WORDS_CONDENSED_LIST_PAGE_ID:
        [[fallthrough]];
    case ALL_WORDS_LIST_PAGE_ID:
        [[fallthrough]];
    case PROPER_NOUNS_LIST_PAGE_ID:
        foundWindow = GetWordsBreakdownView().FindWindowById(event.GetId());
        if (foundWindow)
            { searchText = dynamic_cast<const ListCtrlEx*>(foundWindow)->GetSelectedText(); }
        textId = HARD_WORDS_TEXT_PAGE_ID;
        break;
    case OVERUSED_WORDS_BY_SENTENCE_LIST_PAGE_ID:
        foundWindow = GetGrammarView().FindWindowById(event.GetId());
        if (foundWindow)
            {
            const auto selectedItem = dynamic_cast<const ListCtrlEx*>(foundWindow)->GetFirstSelected();
            searchText = (selectedItem == wxNOT_FOUND) ? wxString{} :
                dynamic_cast<const ListCtrlEx*>(foundWindow)->GetItemTextEx(selectedItem,1);
            }
        textId = HARD_WORDS_TEXT_PAGE_ID;
        break;
    case LONG_SENTENCES_LIST_PAGE_ID:
        foundWindow = GetSentencesBreakdownView().FindWindowById(event.GetId());
        if (foundWindow)
            { searchText = dynamic_cast<const ListCtrlEx*>(foundWindow)->GetSelectedText(); }
        textId = LONG_SENTENCES_AND_WORDINESS_TEXT_PAGE_ID;
        break;
    // grammar lists will just use the same text window
    case SENTENCES_CONJUNCTION_START_LIST_PAGE_ID:
        [[fallthrough]];
    case SENTENCES_LOWERCASE_START_LIST_PAGE_ID:
        [[fallthrough]];
    case DUPLICATES_LIST_PAGE_ID:
        [[fallthrough]];
    case INCORRECT_ARTICLE_PAGE_ID:
        [[fallthrough]];
    case PASSIVE_VOICE_PAGE_ID:
        [[fallthrough]];
    case MISSPELLED_WORD_LIST_PAGE_ID:
        [[fallthrough]];
    case WORDY_PHRASES_LIST_PAGE_ID:
        [[fallthrough]];
    case REDUNDANT_PHRASE_LIST_PAGE_ID:
        [[fallthrough]];
    case WORDING_ERRORS_LIST_PAGE_ID:
        [[fallthrough]];
    case CLICHES_LIST_PAGE_ID:
        foundWindow = GetGrammarView().FindWindowById(event.GetId());
        if (foundWindow)
            { searchText = dynamic_cast<const ListCtrlEx*>(foundWindow)->GetSelectedText(); }
        textId = LONG_SENTENCES_AND_WORDINESS_TEXT_PAGE_ID;
        break;
    case DOLCH_WORDS_LIST_PAGE_ID:
        foundWindow = GetDolchSightWordsView().FindWindowById(event.GetId());
        if (foundWindow)
            { searchText = dynamic_cast<const ListCtrlEx*>(foundWindow)->GetSelectedText(); }
        textId = DOLCH_WORDS_TEXT_PAGE_ID;
        break;
    case NON_DOLCH_WORDS_LIST_PAGE_ID:
        foundWindow = GetDolchSightWordsView().FindWindowById(event.GetId());
        if (foundWindow)
            { searchText = dynamic_cast<const ListCtrlEx*>(foundWindow)->GetSelectedText(); }
        textId = NON_DOLCH_WORDS_TEXT_PAGE_ID;
        break;
    default:
        // custom test
        foundWindow = GetWordsBreakdownView().FindWindowById(event.GetId());
        if (foundWindow)
            { searchText = dynamic_cast<const ListCtrlEx*>(foundWindow)->GetSelectedText(); }
        textId = event.GetId();
        }

    searchText.Trim(true);

    // if the embedded editor is open, then select the text in there
    if (m_embeddedTextEditor != nullptr &&
        m_embeddedTextEditor->IsShown())
        {
        m_embeddedTextEditor->SelectString(searchText, replacementText);
        }
    else
        {
        // Find the first occurrence of the selected word.
        // First, look in the word breakdown section for the respective test window,
        // then the grammar section and finally the Dolch section.
        wxWindow* theWindow = GetWordsBreakdownView().FindWindowById(textId, CLASSINFO(FormattedTextCtrl));
        if (!theWindow)
            { theWindow = GetGrammarView().FindWindowById(textId, CLASSINFO(FormattedTextCtrl)); }
        if (!theWindow)
            { theWindow = GetDolchSightWordsView().FindWindowById(textId, CLASSINFO(FormattedTextCtrl)); }
        if (theWindow && theWindow->IsKindOf(CLASSINFO(FormattedTextCtrl)) )
            {
            FormattedTextCtrl* textWindow = dynamic_cast<FormattedTextCtrl*>(theWindow);
            textWindow->SetSelection(0, 0);
            textWindow->FindText(searchText, true, true, false);
            // Search by label for custom word-list tests (the list and report have the same ID);
            // otherwise, search by ID.
            GetSideBar()->SelectSubItem(
                (event.GetId() == textId) ?
                GetSideBar()->FindSubItem(textWindow->GetLabel()) :
                GetSideBar()->FindSubItem(textId));
            }

        // update the search panel to remember the string we searched for
        m_searchCtrl->SetFindString(searchText);
        }
    }

//-------------------------------------------------------
void ProjectView::OnLaunchSourceFile([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    ProjectDoc* doc = dynamic_cast<ProjectDoc*>(GetDocument());
    if (doc)
        {
        FilePathResolver resolvePath(doc->GetOriginalDocumentFilePath(), false);
        if (doc->GetDocumentStorageMethod() == TextStorage::EmbedText)
            {
            // if the text was embedded but actually came from a file (i.e., not manually entered)
            // then ask if they prefer to relink to the file and then edit that, rather than editing
            // the embedded text. Also, don't bother asking if the file is a file inside of an archive.
            if (doc->GetTextSource() == TextSource::FromFile && !resolvePath.IsArchivedFile())
                {
                auto warningIter = WarningManager::GetWarning(_DT(L"linked-document-is-embedded"));
                // if they want to be prompted for this...
                if (warningIter != WarningManager::GetWarnings().end() &&
                    warningIter->ShouldBeShown())
                    {
                    wxRichMessageDialog msg(GetDocFrame(), warningIter->GetMessage(),
                                warningIter->GetTitle(), warningIter->GetFlags());
                    msg.ShowCheckBox(_(L"Remember my answer"));
                    const int dlgResponse = msg.ShowModal();
                    // save the checkbox status
                    if (msg.IsCheckBoxChecked())
                        {
                        warningIter->Show(false);
                        warningIter->SetPreviousResponse(dlgResponse);
                        }
                    // now see if they said "Yes" or "No"
                    if (dlgResponse == wxID_YES)
                        {
                        doc->SetModifiedFlag();
                        doc->SetDocumentStorageMethod(TextStorage::NoEmbedText);
                        wxLaunchDefaultApplication(doc->GetOriginalDocumentFilePath() );
                        return;
                        }
                    }
                // or if they said "yes" before, then use the found path
                else if (warningIter != WarningManager::GetWarnings().end() &&
                    warningIter->GetPreviousResponse() == wxID_YES)
                    {
                    doc->SetModifiedFlag();
                    doc->SetDocumentStorageMethod(TextStorage::NoEmbedText);
                    wxLaunchDefaultApplication(doc->GetOriginalDocumentFilePath() );
                    return;
                    }
                }
            if (m_embeddedTextEditor == nullptr)
                {
                m_embeddedTextEditor = new EditTextDlg(GetDocFrame(), doc,
                    doc->GetDocumentText(),
                    wxID_ANY, _(L"Edit Embedded Document"),
                    doc->GetAppendedDocumentText().length() ?
                    _(L"Note: The appended template document is not included here.\n"
                       "Only the embedded text is editable from this dialog.") : wxString{});
                }

            m_embeddedTextEditor->Show();
            }
        else
            {
            if (resolvePath.IsArchivedFile())
                {
                wxMessageBox(_(L"Files inside of archives files cannot be edited."),
                    wxString{}, wxOK|wxICON_INFORMATION);
                return;
                }
            else if (resolvePath.IsExcelCell())
                {
                const size_t excelTag = resolvePath.GetResolvedPath().MakeLower().find(_DT(L".xlsx#"));
                wxFileName fn(resolvePath.GetResolvedPath().substr(0, excelTag+5));
                wxLaunchDefaultApplication(fn.GetFullPath());
                }
            else
                { wxLaunchDefaultApplication(resolvePath.GetResolvedPath()); }
            }
        }
    }

// add/remove the icon to the list view on the side panel
//-------------------------------------------------------
void ProjectView::UpdateSideBarIcons()
    {
    GetSideBar()->SaveState();
    GetSideBar()->DeleteAllFolders();

    // readability tests
    //-----------------
    // Note: refer to ReadabilityApp::InitProjectSidebar() for the icon indices.
    if (GetReadabilityResultsView().GetWindowCount() > 0)
        {
        GetSideBar()->InsertItem(GetSideBar()->GetFolderCount(), GetReadabilityScoresLabel(),
                                 SIDEBAR_READABILITY_SCORES_SECTION_ID, 1);
        for (size_t i = 0; i < GetReadabilityResultsView().GetWindowCount(); ++i)
            {
            const bool isGraph = typeid(*GetReadabilityResultsView().GetWindow(i)) == typeid(Wisteria::Canvas);

            GetSideBar()->InsertSubItemById(SIDEBAR_READABILITY_SCORES_SECTION_ID,
                GetReadabilityResultsView().GetWindow(i)->GetName(),
                GetReadabilityResultsView().GetWindow(i)->GetId(),
                GetReadabilityResultsView().GetWindow(i)->GetId() == READABILITY_SCORES_SUMMARY_REPORT_PAGE_ID ?
                    17 :
                GetReadabilityResultsView().GetWindow(i)->GetId() == READABILITY_SCORES_PAGE_ID ?
                    23 :
                GetReadabilityResultsView().GetWindow(i)->GetId() == READABILITY_GOALS_PAGE_ID ?
                    28 :
                (isGraph &&
                     typeid(*dynamic_cast<Wisteria::Canvas*>(
                         GetReadabilityResultsView().GetWindow(i))->GetFixedObject(0, 0)) == typeid(FleschChart)) ?
                    18 :
                (isGraph &&
                     typeid(*dynamic_cast<Wisteria::Canvas*>(
                         GetReadabilityResultsView().GetWindow(i))->GetFixedObject(0, 0)) == typeid(FraseGraph)) ?
                    19 :
                (isGraph &&
                     typeid(*dynamic_cast<Wisteria::Canvas*>(
                         GetReadabilityResultsView().GetWindow(i))->GetFixedObject(0, 0)) == typeid(FryGraph)) ?
                    20 :
                (isGraph &&
                     typeid(*dynamic_cast<Wisteria::Canvas*>(
                         GetReadabilityResultsView().GetWindow(i))->GetFixedObject(0, 0)) == typeid(RaygorGraph)) ?
                    21 :
                (isGraph &&
                     typeid(*dynamic_cast<Wisteria::Canvas*>(
                         GetReadabilityResultsView().GetWindow(i))->GetFixedObject(0, 0)) == typeid(CrawfordGraph)) ?
                    22 :
                (isGraph &&
                     typeid(*dynamic_cast<Wisteria::Canvas*>(
                         GetReadabilityResultsView().GetWindow(i))->GetFixedObject(0, 0)) == typeid(SchwartzGraph)) ?
                    25 :
                (isGraph &&
                     typeid(*dynamic_cast<Wisteria::Canvas*>(
                         GetReadabilityResultsView().GetWindow(i))->GetFixedObject(0, 0)) == typeid(LixGauge)) ?
                    26 :
                (isGraph &&
                     typeid(*dynamic_cast<Wisteria::Canvas*>(
                         GetReadabilityResultsView().GetWindow(i))->GetFixedObject(0, 0)) == typeid(LixGaugeGerman)) ?
                    26 :
                (isGraph &&
                     typeid(*dynamic_cast<Wisteria::Canvas*>(
                         GetReadabilityResultsView().GetWindow(i))->GetFixedObject(0, 0)) ==
                         typeid(DanielsonBryan2Plot)) ?
                    27 :
                    9);
            }
        }

    // Summary statistics window
    if (GetSummaryView().GetWindowCount() > 0)
        {
        GetSideBar()->InsertItem(GetSideBar()->GetFolderCount(), GetSummaryStatisticsLabel(),
                                 SIDEBAR_STATS_SUMMARY_SECTION_ID, 2);
        for (size_t i = 0; i < GetSummaryView().GetWindowCount(); ++i)
            {
            GetSideBar()->InsertSubItemById(SIDEBAR_STATS_SUMMARY_SECTION_ID, GetSummaryView().GetWindow(i)->GetName(),
                GetSummaryView().GetWindow(i)->GetId(),
                GetSummaryView().GetWindow(i)->IsKindOf(CLASSINFO(HtmlTableWindow)) ? 17 :
                    GetSummaryView().GetWindow(i)->IsKindOf(CLASSINFO(ListCtrlEx)) ? 15 : 9);
            }
        }

    // Words breakdown
    if (GetWordsBreakdownView().GetWindowCount() > 0)
        {
        GetSideBar()->InsertItem(GetSideBar()->GetFolderCount(), GetWordsBreakdownLabel(),
                                 SIDEBAR_WORDS_BREAKDOWN_SECTION_ID, 13);
        for (size_t i = 0; i < GetWordsBreakdownView().GetWindowCount(); ++i)
            {
            const bool isGraph = typeid(*GetWordsBreakdownView().GetWindow(i)) == typeid(Wisteria::Canvas);

            GetSideBar()->InsertSubItemById(SIDEBAR_WORDS_BREAKDOWN_SECTION_ID,
                GetWordsBreakdownView().GetWindow(i)->GetName(),
                GetWordsBreakdownView().GetWindow(i)->GetId(),
                GetWordsBreakdownView().GetWindow(i)->IsKindOf(CLASSINFO(FormattedTextCtrl)) ? 0 :
                    GetWordsBreakdownView().GetWindow(i)->IsKindOf(CLASSINFO(ListCtrlEx)) ? 15 :
                    (isGraph &&
                        typeid(*dynamic_cast<Wisteria::Canvas*>(
                            GetWordsBreakdownView().GetWindow(i))->GetFixedObject(0, 0)) ==
                        typeid(Wisteria::Graphs::Histogram)) ? 6 :
                    (isGraph &&
                        typeid(*dynamic_cast<Wisteria::Canvas*>(
                            GetWordsBreakdownView().GetWindow(i))->GetFixedObject(0, 0)) ==
                        typeid(Wisteria::Graphs::BarChart)) ? 16 :
                    (isGraph &&
                        typeid(*dynamic_cast<Wisteria::Canvas*>(
                            GetWordsBreakdownView().GetWindow(i))->GetFixedObject(0, 0)) ==
                        typeid(Wisteria::Graphs::WordCloud)) ? 29 :
                    (isGraph &&
                        typeid(*dynamic_cast<Wisteria::Canvas*>(
                            GetWordsBreakdownView().GetWindow(i))->GetFixedObject(0, 0)) ==
                        typeid(Wisteria::Graphs::PieChart)) ? 30 :
                    9);
            }
        }

    // Sentences breakdown
    if (GetSentencesBreakdownView().GetWindowCount() > 0)
        {
        GetSideBar()->InsertItem(GetSideBar()->GetFolderCount(), GetSentencesBreakdownLabel(),
                                 SIDEBAR_SENTENCES_BREAKDOWN_SECTION_ID, 14);
        for (size_t i = 0; i < GetSentencesBreakdownView().GetWindowCount(); ++i)
            {
            const bool isGraph = typeid(*GetSentencesBreakdownView().GetWindow(i)) == typeid(Wisteria::Canvas);

            GetSideBar()->InsertSubItemById(SIDEBAR_SENTENCES_BREAKDOWN_SECTION_ID,
                GetSentencesBreakdownView().GetWindow(i)->GetName(),
                GetSentencesBreakdownView().GetWindow(i)->GetId(),
                GetSentencesBreakdownView().GetWindow(i)->IsKindOf(CLASSINFO(ListCtrlEx)) ? 15 :
                (isGraph &&
                    typeid(*dynamic_cast<Wisteria::Canvas*>(
                        GetSentencesBreakdownView().GetWindow(i))->GetFixedObject(0, 0)) ==
                    typeid(Wisteria::Graphs::BoxPlot)) ? 7 :
                (isGraph &&
                    typeid(*dynamic_cast<Wisteria::Canvas*>(
                        GetSentencesBreakdownView().GetWindow(i))->GetFixedObject(0, 0)) ==
                    typeid(Wisteria::Graphs::Histogram)) ? 6 :
                (isGraph &&
                    typeid(*dynamic_cast<Wisteria::Canvas*>(
                        GetSentencesBreakdownView().GetWindow(i))->GetFixedObject(0, 0)) ==
                    typeid(Wisteria::Graphs::HeatMap)) ? 24 :
                9);
            }
        }

    // grammar windows
    if (GetGrammarView().GetWindowCount() > 0)
        {
        GetSideBar()->InsertItem(GetSideBar()->GetFolderCount(), GetGrammarLabel(), SIDEBAR_GRAMMAR_SECTION_ID, 4);
        for (size_t i = 0; i < GetGrammarView().GetWindowCount(); ++i)
            {
            GetSideBar()->InsertSubItemById(SIDEBAR_GRAMMAR_SECTION_ID,
                GetGrammarView().GetWindow(i)->GetName(),
                GetGrammarView().GetWindow(i)->GetId(),
                GetGrammarView().GetWindow(i)->IsKindOf(CLASSINFO(FormattedTextCtrl)) ? 0 :
                    GetGrammarView().GetWindow(i)->IsKindOf(CLASSINFO(ListCtrlEx)) ? 15 : 9);
            }
        }

    // sight words
    if (GetDolchSightWordsView().GetWindowCount() > 0)
        {
        GetSideBar()->InsertItem(GetSideBar()->GetFolderCount(), GetDolchLabel(), SIDEBAR_DOLCH_SECTION_ID, 5);
        for (size_t i = 0; i < GetDolchSightWordsView().GetWindowCount(); ++i)
            {
            const bool isGraph = typeid(*GetDolchSightWordsView().GetWindow(i)) == typeid(Wisteria::Canvas);

            GetSideBar()->InsertSubItemById(SIDEBAR_DOLCH_SECTION_ID,
                GetDolchSightWordsView().GetWindow(i)->GetName(),
                GetDolchSightWordsView().GetWindow(i)->GetId(),
                GetDolchSightWordsView().GetWindow(i)->IsKindOf(CLASSINFO(FormattedTextCtrl)) ? 0 :
                    GetDolchSightWordsView().GetWindow(i)->IsKindOf(CLASSINFO(HtmlTableWindow)) ? 17 :
                    (isGraph &&
                        typeid(*dynamic_cast<Wisteria::Canvas*>(
                            GetDolchSightWordsView().GetWindow(i))->GetFixedObject(0, 0)) ==
                        typeid(Wisteria::Graphs::BarChart)) ? 16 :
                    GetDolchSightWordsView().GetWindow(i)->IsKindOf(CLASSINFO(ListCtrlEx)) ? 15 : 9);
            }
        }

    GetSideBar()->ResetState();
    }

//-------------------------------------------------------
void ProjectView::OnAddTest(wxCommandEvent& event)
    {
    ProjectDoc* doc = dynamic_cast<ProjectDoc*>(GetDocument());
    if (!doc->IsSafeToUpdate())
        { return; }

    if (event.GetId() == XRCID("ID_DOLCH"))
        { doc->AddDolchSightWords(); }
    else
        {
        doc->GetReadabilityTests().include_test(
            doc->GetReadabilityTests().get_test_id(event.GetId()).c_str(), true);
        // refresh
        doc->RefreshRequired(ProjectRefresh::Minimal);
        doc->RefreshProject();
        const long testToSelect =
            GetReadabilityScoresList()->GetResultsListCtrl()->FindEx(
                doc->GetReadabilityTests().get_test_long_name(event.GetId()).c_str());
        if (testToSelect != wxNOT_FOUND)
            { GetReadabilityScoresList()->GetResultsListCtrl()->Select(testToSelect); }
        GetSideBar()->SelectSubItem(
            GetSideBar()->FindSubItem(BaseProjectView::READABILITY_SCORES_PAGE_ID));
        // show any warning messages from the test being ran
        doc->ShowQueuedMessages();
        if (WarningManager::HasWarning(_DT(L"click-test-to-view")))
            { ShowInfoMessage(*WarningManager::GetWarning(_DT(L"click-test-to-view"))); }
        }
    }

//---------------------------------------------------
void ProjectView::OnGradeScale(wxCommandEvent& event)
    {
    ProjectDoc* doc = dynamic_cast<ProjectDoc*>(GetDocument());
    if (!doc->IsSafeToUpdate())
        { return; }
    wxWindowUpdateLocker noUpdates(doc->GetDocumentWindow());
    BaseProjectProcessingLock processingLock(doc);

    for (size_t i = 0; i < m_gradeScaleMenu.GetMenuItemCount(); ++i)
        { m_gradeScaleMenu.FindItemByPosition(i)->Check(false); }

    readability::grade_scale gs = doc->GetReadabilityMessageCatalog().GetGradeScale();
    if (event.GetId() == XRCID("ID_K12_US"))
        { gs = readability::grade_scale::k12_plus_united_states; }
    else if (event.GetId() == XRCID("ID_K12_NEWFOUNDLAND"))
        { gs = readability::grade_scale::k12_plus_newfoundland_and_labrador; }
    else if (event.GetId() == XRCID("ID_K12_BC"))
        { gs = readability::grade_scale::k12_plus_british_columbia; }
    else if (event.GetId() == XRCID("ID_K12_NEW_BRUNSWICK"))
        { gs = readability::grade_scale::k12_plus_newbrunswick; }
    else if (event.GetId() == XRCID("ID_K12_NOVA_SCOTIA"))
        { gs = readability::grade_scale::k12_plus_nova_scotia; }
    else if (event.GetId() == XRCID("ID_K12_ONTARIO"))
        { gs = readability::grade_scale::k12_plus_ontario; }
    else if (event.GetId() == XRCID("ID_K12_SASKATCHEWAN"))
        { gs = readability::grade_scale::k12_plus_saskatchewan; }
    else if (event.GetId() == XRCID("ID_K12_PE"))
        { gs = readability::grade_scale::k12_plus_prince_edward_island; }
    else if (event.GetId() == XRCID("ID_K12_MANITOBA"))
        { gs = readability::grade_scale::k12_plus_manitoba; }
    else if (event.GetId() == XRCID("ID_K12_NT"))
        { gs = readability::grade_scale::k12_plus_northwest_territories; }
    else if (event.GetId() == XRCID("ID_K12_ALBERTA"))
        { gs = readability::grade_scale::k12_plus_alberta; }
    else if (event.GetId() == XRCID("ID_K12_NUNAVUT"))
        { gs = readability::grade_scale::k12_plus_nunavut; }
    else if (event.GetId() == XRCID("ID_QUEBEC"))
        { gs = readability::grade_scale::quebec; }
    else if (event.GetId() == XRCID("ID_ENGLAND"))
        { gs = readability::grade_scale::key_stages_england_wales; }
    m_gradeScaleMenu.Check(event.GetId(), true);
    doc->GetReadabilityMessageCatalog().SetGradeScale(gs);
    const long SelectedTest = GetReadabilityScoresList()->GetResultsListCtrl()->GetFirstSelected();
    doc->DisplayReadabilityScores(false);
    GetReadabilityScoresList()->GetResultsListCtrl()->Select(SelectedTest);
    doc->SetModifiedFlag();
    }

//---------------------------------------------------
void ProjectView::OnAddToDictionary([[maybe_unused]] wxCommandEvent& event)
    {
    const ListCtrlEx* listView =
        dynamic_cast<ListCtrlEx*>(GetGrammarView().FindWindowById(MISSPELLED_WORD_LIST_PAGE_ID));
    if (listView)
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(MISSPELLED_WORD_LIST_PAGE_ID));

        long item = wxNOT_FOUND;
        wxArrayString newWords;
        while (true)
            {
            item = listView->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
            if (item == wxNOT_FOUND)
                { break; }
            newWords.Add(listView->GetItemText(item));
            }
        if (newWords.GetCount() == 0)
            {
            wxMessageBox(_(L"Please select a word (or words) to add to your dictionary."),
                _(L"Add to Dictionary"), wxOK|wxICON_INFORMATION);
            return;
            }
        wxGetApp().AddWordsToDictionaries(newWords,
            dynamic_cast<BaseProjectDoc*>(GetDocument())->GetProjectLanguage());
        wxList docs = wxGetApp().GetDocManager()->GetDocuments();
        for (size_t i = 0; i < docs.GetCount(); ++i)
            {
            BaseProjectDoc* doc = dynamic_cast<BaseProjectDoc*>(docs.Item(i)->GetData());
            doc->RemoveMisspellings(newWords);
            }
        }
    else
        {
        wxMessageBox(_(L"There are no misspellings in this document."),
                _(L"Add to Dictionary"), wxOK|wxICON_INFORMATION);
        return;
        }
    }

//---------------------------------------------------
void ProjectView::OnRibbonButtonCommand(wxRibbonButtonBarEvent& event)
    {
    wxCommandEvent cmd(wxEVT_MENU, event.GetId());
    // the document frame needs to handle document events
    if (event.GetId() == wxID_OPEN ||
        event.GetId() == wxID_NEW ||
        event.GetId() == wxID_SAVE)
        { GetDocFrame()->ProcessWindowEvent(cmd); }
    else
        { OnMenuCommand(cmd); }
    }

// Handles all menu events for the document and propagates to the active window
//---------------------------------------------------
void ProjectView::OnMenuCommand(wxCommandEvent& event)
    {
    ProjectDoc* doc = dynamic_cast<ProjectDoc*>(GetDocument());
    if (!doc->IsSafeToUpdate())
        { return; }
    // show a message about zooming in and how the background image won't scale larger than its size
    if (event.GetId() == wxID_ZOOM_IN)
        {
        if (WarningManager::HasWarning(_DT(L"bkimage-zoomin-noupscale")))
            { ShowInfoMessage(*WarningManager::GetWarning(_DT(L"bkimage-zoomin-noupscale"))); }
        }

    // propagate standard save command to active subwindow if "export window" option selected
    if (event.GetId() == XRCID("ID_SAVE_ITEM") )
        { event.SetId(wxID_SAVE); }
    else if (event.GetId() == XRCID("ID_SORT_ASCENDING") &&
        GetActiveProjectWindow() &&
        typeid(*GetActiveProjectWindow()) == typeid(Wisteria::Canvas) &&
        (typeid(*dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow())->GetFixedObject(0, 0)) ==
            typeid(Wisteria::Graphs::BarChart) ||
         typeid(*dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow())->GetFixedObject(0, 0)) ==
            typeid(Wisteria::Graphs::Histogram)) )
        {
        Wisteria::Canvas* barChart = dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow());
        std::dynamic_pointer_cast<Wisteria::Graphs::BarChart>(
            barChart->GetFixedObject(0, 0))->SortBars(
                BarChart::BarSortComparison::SortByBarLength, Wisteria::SortDirection::SortAscending);
        barChart->Refresh();
        barChart->Update();
        return;
        }
    else if (event.GetId() == XRCID("ID_SORT_DESCENDING") &&
        GetActiveProjectWindow() &&
         typeid(*GetActiveProjectWindow()) == typeid(Wisteria::Canvas) &&
        (typeid(*dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow())->GetFixedObject(0, 0)) ==
            typeid(Wisteria::Graphs::BarChart) ||
         typeid(*dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow())->GetFixedObject(0, 0)) ==
            typeid(Wisteria::Graphs::Histogram)) )
        {
        Wisteria::Canvas* barChart = dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow());
        std::dynamic_pointer_cast<Wisteria::Graphs::BarChart>(
            barChart->GetFixedObject(0, 0))->SortBars(
                BarChart::BarSortComparison::SortByBarLength, Wisteria::SortDirection::SortDescending);
        barChart->Refresh();
        barChart->Update();
        return;
        }

    if (GetActiveProjectWindow() && GetActiveProjectWindow()->IsKindOf(CLASSINFO(ListCtrlEx)) )
        {
        ListCtrlEx* list = dynamic_cast<ListCtrlEx*>(GetActiveProjectWindow());
        /* just in case this is a print or preview command, update the window's headers
           and footer to whatever the global options currently are*/
        BaseProjectDoc::UpdateListOptions(list);
        // in case we are exporting the window, set its label to include the name of the document, and then reset it
        list->SetLabel(wxString::Format(L"%s [%s]", list->GetName(), wxFileName::StripExtension(doc->GetTitle())));
        if (event.GetId() == XRCID("ID_EXCLUDE_SELECTED"))
            {
            if (list->GetSelectedItemCount() == 0)
                {
                wxMessageBox(_(L"Please select an item to exclude."),
                    _(L"Error"), wxOK|wxICON_WARNING);
                return;
                }
            if (doc->GetInvalidSentenceMethod() == InvalidSentence::IncludeAsFullSentences)
                {
                if (wxMessageBox(_(L"This project is not currently excluding text. Do you wish to change this?"),
                    _(L"Text Exclusion Method"), wxYES_NO|wxICON_QUESTION) == wxYES)
                    {
                    doc->SetInvalidSentenceMethod(InvalidSentence::ExcludeFromAnalysis);
                    }
                else
                    {
                    wxMessageBox(_(L"Text exclusion not enabled. Items will not be excluded."),
                        _(L"Error"), wxOK|wxICON_WARNING);
                    return;
                    }
                }
            if (doc->GetExcludedPhrasesPath().empty())
                {
                wxFileDialog dialog
                        (GetActiveProjectWindow(),
                        _(L"Specify Where to Save Word Exclusion List"),
                        wxString{},
                            wxString{},
                        _(L"Text files (*.txt)|*.txt"),
                        wxFD_SAVE|wxFD_PREVIEW);
                if (dialog.ShowModal() != wxID_OK)
                    { return; }

                doc->SetExcludedPhrasesPath(dialog.GetPath());

                // if no application-level exclusion list is being used, then ask
                // if we want this new one to used for that as well
                if (wxGetApp().GetAppOptions().GetExcludedPhrasesPath().empty())
                    {
                    auto warningIter =
                        WarningManager::GetWarning(_DT(L"set-app-exclusion-list-from-project"));
                    if (warningIter != WarningManager::GetWarnings().end() &&
                        warningIter->ShouldBeShown())
                        {
                        wxRichMessageDialog msg(wxGetApp().GetMainFrame(), warningIter->GetMessage(),
                                                        warningIter->GetTitle(), warningIter->GetFlags());
                        msg.ShowCheckBox(_(L"Remember my answer"));
                        const int dlgResponse = msg.ShowModal();
                        if (warningIter != WarningManager::GetWarnings().end() &&
                            msg.IsCheckBoxChecked())
                            {
                            warningIter->Show(false);
                            warningIter->SetPreviousResponse(dlgResponse);
                            }
                        if (dlgResponse == wxID_YES)
                            {
                            wxGetApp().GetAppOptions().SetExcludedPhrasesPath(dialog.GetPath());
                            }
                        }
                    }
                }

            wxString selectedStrings;
            long item = wxNOT_FOUND;
            for (;;)
                {
                item = list->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
                if (item == wxNOT_FOUND)
                    { break; }
                selectedStrings += list->GetItemTextEx(item, 0) + L"\n";
                }

            wxString buffer;
            wxString filePath = doc->GetExcludedPhrasesPath();
            if (!Wisteria::TextStream::ReadFile(filePath, buffer) )
                {
                wxMessageBox(_(L"Error loading excluded word list file."),
                    _(L"Error"), wxOK|wxICON_EXCLAMATION);
                return;
                }
            grammar::phrase_collection phrases;
            phrases.load_phrases(buffer, false, false);
            phrases.load_phrases(selectedStrings, true, true);
            phrases.remove_duplicates();

            wxString outputStr;
            wxString expStr;
            /// @todo with c++20's format library, move this save code into phrase_collection
            for (const auto& phrase : phrases.get_phrases())
                {
                outputStr += phrase.first.to_string().c_str();
                // don't bother exporting blank columns
                if (phrase.second.length() || phrase.first.get_proceeding_exceptions().size() ||
                    phrase.first.get_trailing_exceptions().size())
                    {
                    outputStr += L'\t';
                    outputStr += phrase.second.c_str();
                    outputStr += L'\t';
                    outputStr += wxString::Format(L"%d", static_cast<int>(phrase.first.get_type()));
                    outputStr += L'\t';
                    expStr.clear();
                    for (const auto& exp : phrase.first.get_proceeding_exceptions())
                        { expStr.append(exp.c_str()).append(L";"); }
                    if (expStr.ends_with(L";"))
                        { expStr.RemoveLast(1); }
                    outputStr += expStr;
                    outputStr += L'\t';

                    expStr.clear();
                    for (const auto& exp : phrase.first.get_trailing_exceptions())
                        { expStr.append(exp.c_str()).append(L";"); }
                    if (expStr.ends_with(L";"))
                        { expStr.RemoveLast(1); }
                    outputStr += expStr;
                    }
                outputStr += L"\r\n";
                }

            outputStr.Trim(true); outputStr.Trim(false);
            wxFileName(filePath).SetPermissions(wxS_DEFAULT);
            wxFile outputFile(filePath, wxFile::write);
            if (!outputFile.IsOpened())
                {
                wxMessageBox(wxString::Format(
                    _(L"Unable to save \"%s\".\nVerify that you have write access to "
                       "this file or that it is not in use."), filePath), _(L"Error"), wxOK|wxICON_ERROR);
                }
            else
                {
            #ifdef __WXMSW__
                if (outputStr.length())
                    { outputFile.Write(utf8::bom, sizeof(utf8::bom)); }
            #endif
                outputFile.Write(outputStr, wxConvUTF8);
                doc->SetExcludedPhrasesPath(filePath);
                }

            list->DeselectAll();
            doc->RefreshRequired(ProjectRefresh::RefreshRequirement::FullReindexing);
            doc->RefreshProject();
            }
        else
            {
            ParentEventBlocker blocker(list);
            list->ProcessWindowEvent(event);
            }
        }
    else if (GetActiveProjectWindow() && GetActiveProjectWindow()->IsKindOf(CLASSINFO(HtmlTableWindow)) )
        {
        HtmlTableWindow* html = dynamic_cast<HtmlTableWindow*>(GetActiveProjectWindow());
        BaseProjectDoc::UpdatePrinterHeaderAndFooters(html);
        html->SetLabel(wxString::Format(L"%s [%s]", html->GetName(), wxFileName::StripExtension(doc->GetTitle())));
        ParentEventBlocker blocker(html);
        html->ProcessWindowEvent(event);
        }
    else if (GetActiveProjectWindow() && GetActiveProjectWindow()->IsKindOf(CLASSINFO(FormattedTextCtrl)) )
        {
        FormattedTextCtrl* text = dynamic_cast<FormattedTextCtrl*>(GetActiveProjectWindow());
        doc->UpdateTextWindowOptions(text);
        text->SetTitleName(wxString::Format(L"%s [%s]", text->GetName(), wxFileName::StripExtension(doc->GetTitle())));
        ParentEventBlocker blocker(text);
        text->ProcessWindowEvent(event);
        }
    else if (GetActiveProjectWindow() && GetActiveProjectWindow()->IsKindOf(CLASSINFO(ExplanationListCtrl)) )
        {
        ExplanationListCtrl* elist = dynamic_cast<ExplanationListCtrl*>(GetActiveProjectWindow());
        BaseProjectDoc::UpdateExplanationListOptions(elist);
        elist->SetLabel(wxString::Format(L"%s [%s]", elist->GetName(), wxFileName::StripExtension(doc->GetTitle())));
        ParentEventBlocker blocker(elist);
        elist->ProcessWindowEvent(event);
        }
    else if (GetActiveProjectWindow() && typeid(*GetActiveProjectWindow()) == typeid(Wisteria::Canvas) )
        {
        Wisteria::Canvas* graph = dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow());
        doc->UpdateGraphOptions(graph);
        graph->SetLabel(wxString::Format(L"%s [%s]", graph->GetName(), wxFileName::StripExtension(doc->GetTitle())));
        ParentEventBlocker blocker(graph);
        graph->ProcessWindowEvent(event);
        }
    }

//---------------------------------------------------
void ProjectView::OnFind(wxFindDialogEvent &event)
    {
    // if they were just hitting Cancel then close
    if (event.GetEventType() == wxEVT_COMMAND_FIND_CLOSE)
        { return; }

    if (GetActiveProjectWindow())
        {
        ParentEventBlocker blocker(GetActiveProjectWindow());
        GetActiveProjectWindow()->ProcessWindowEvent(event);
        }
    }

//-------------------------------------------------------
void ProjectView::UpdateRibbonState()
    {
    ProjectDoc* projDoc = dynamic_cast<ProjectDoc*>(GetDocument());
    wxWindow* projectButtonBarWindow = GetRibbon()->FindWindow(MainFrame::ID_PROJECT_RIBBON_BUTTON_BAR);
    if (projDoc && projectButtonBarWindow && projectButtonBarWindow->IsKindOf(CLASSINFO(wxRibbonButtonBar)))
        {
        auto projBar = dynamic_cast<wxRibbonButtonBar*>(projectButtonBarWindow);
        assert(projBar);
        if (projBar)
            {
            projBar->ToggleButton(XRCID("ID_REALTIME_UPDATE"),
                projDoc->IsRealTimeUpdating());
            projBar->EnableButton(XRCID("ID_REALTIME_UPDATE"),
                projDoc->GetDocumentStorageMethod() == TextStorage::LoadFromExternalDocument);
            }
        }
    }

//-------------------------------------------------------
bool ProjectView::OnCreate(wxDocument* doc, long flags)
    {
    if (!BaseProjectView::OnCreate(doc, flags))
        { return false; }

    // Results view
    ExplanationListCtrl* readabilityScoresView = new ExplanationListCtrl(GetSplitter(), READABILITY_SCORES_PAGE_ID,
        wxDefaultPosition, wxDefaultSize, _(L"Scores"));
    readabilityScoresView->Hide();
    readabilityScoresView->GetDataProvider()->SetNumberFormatter(
        dynamic_cast<BaseProjectDoc*>(doc)->GetReadabilityMessageCatalogPtr());
    readabilityScoresView->GetResultsListCtrl()->SetVirtualDataSize(0, 5);
    readabilityScoresView->GetResultsListCtrl()->InsertColumn(0,
        _(L"Test"), wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE_USEHEADER);
    readabilityScoresView->GetResultsListCtrl()->InsertColumn(1,
        _(L"Grade Level"), wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE_USEHEADER);
    readabilityScoresView->GetResultsListCtrl()->InsertColumn(2,
        _(L"Reader Age"), wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE_USEHEADER);
    readabilityScoresView->GetResultsListCtrl()->InsertColumn(3,
        _(L"Scale Value"), wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE_USEHEADER);
    readabilityScoresView->GetResultsListCtrl()->InsertColumn(4,
        _(L"Predicted Cloze Score"), wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE_USEHEADER);
    readabilityScoresView->GetExplanationView()->SetPage(
        wxString(L"<html><body>") + _(L"No readability test results currently available.") +
        wxString(L"</body></html>"));
    readabilityScoresView->GetResultsListCtrl()->AssignContextMenu(
        wxXmlResource::Get()->LoadMenu(L"IDM_READABILITY_SCORE_LIST") );
    readabilityScoresView->GetExplanationView()->AssignContextMenu(
        wxXmlResource::Get()->LoadMenu(L"IDM_HTML_MENU_NO_SAVE") );
    readabilityScoresView->SetPrinterSettings(wxGetApp().GetPrintData());
    readabilityScoresView->SetLeftPrinterHeader(wxGetApp().GetAppOptions().GetLeftPrinterHeader());
    readabilityScoresView->SetCenterPrinterHeader(wxGetApp().GetAppOptions().GetCenterPrinterHeader());
    readabilityScoresView->SetRightPrinterHeader(wxGetApp().GetAppOptions().GetRightPrinterHeader());
    readabilityScoresView->SetLeftPrinterFooter(wxGetApp().GetAppOptions().GetLeftPrinterFooter());
    readabilityScoresView->SetCenterPrinterFooter(wxGetApp().GetAppOptions().GetCenterPrinterFooter());
    readabilityScoresView->SetRightPrinterFooter(wxGetApp().GetAppOptions().GetRightPrinterFooter());
    readabilityScoresView->SetResources(
        wxGetApp().GetMainFrame()->GetHelpDirectory(),
        L"column-sorting.html");
    GetReadabilityResultsView().AddWindow(readabilityScoresView);
    GetSplitter()->SplitVertically(GetSideBar(), readabilityScoresView, GetSideBar()->GetMinWidth());

#ifdef __WXOSX__
    // just load the menubar right now, we will set it in Present after the document has successfully loaded
    m_menuBar = wxXmlResource::Get()->LoadMenuBar(L"ID_DOCMENUBAR");
#endif

    // connect the test events
    for (auto rTest = dynamic_cast<const BaseProjectDoc*>(doc)->GetReadabilityTests().get_tests().begin();
         rTest != dynamic_cast<const BaseProjectDoc*>(doc)->GetReadabilityTests().get_tests().end();
         ++rTest)
        {
        Connect(rTest->get_test().get_interface_id(), wxEVT_MENU,
            wxCommandEventHandler(ProjectView::OnAddTest) );
        }

    return true;
    }

//-------------------------------------------------------
void ProjectView::UpdateStatistics()
    {
    ProjectDoc* doc = dynamic_cast<ProjectDoc*>(GetDocument());

    // Determine the stats rows colour from the list control's background colour.
    const wxColour listRowColour = GetReadabilityScoresList()->GetBackgroundColour();
    // Depending on the background, stats row color
    // will be 3% more dark or 50% brighter.
    const int alpha = listRowColour.GetRGB() > 0x808080 ? 97 : 150;

    wxListItemAttr statRowAttribs;
    statRowAttribs.SetBackgroundColour(listRowColour.ChangeLightness(alpha));

    const wxString selectedItem = GetReadabilityScoresList()->GetResultsListCtrl()->GetSelectedText();
    // remove stats rows if already in here because we have to recalculate everything
    long statIconLocation = GetReadabilityScoresList()->GetResultsListCtrl()->FindEx(GetAverageLabel());
    if (statIconLocation != wxNOT_FOUND)
        { GetReadabilityScoresList()->GetResultsListCtrl()->DeleteItem(statIconLocation); }
    statIconLocation = GetReadabilityScoresList()->GetResultsListCtrl()->FindEx(GetMedianLabel());
    if (statIconLocation != wxNOT_FOUND)
        { GetReadabilityScoresList()->GetResultsListCtrl()->DeleteItem(statIconLocation); }
    statIconLocation = GetReadabilityScoresList()->GetResultsListCtrl()->FindEx(GetModeLabel());
    if (statIconLocation != wxNOT_FOUND)
        { GetReadabilityScoresList()->GetResultsListCtrl()->DeleteItem(statIconLocation); }
    statIconLocation = GetReadabilityScoresList()->GetResultsListCtrl()->FindEx(GetStdDevLabel());
    if (statIconLocation != wxNOT_FOUND)
        { GetReadabilityScoresList()->GetResultsListCtrl()->DeleteItem(statIconLocation); }
    // update the averages of the scores
    const int rowCount = GetReadabilityScoresList()->GetResultsListCtrl()->GetItemCount();
    if (rowCount > 1)
        {
        std::vector<double> grades, ages, clozeScores;
        // tally up the numbers in the age and grade columns
        for (int i = 0; i < rowCount; ++i)
            {
            double value = 0;
            if (ReadabilityMessages::GetScoreValue(
                GetReadabilityScoresList()->GetResultsListCtrl()->GetItemTextEx(i, 1), value))
                { grades.push_back(value); }
            if (ReadabilityMessages::GetScoreValue(
                GetReadabilityScoresList()->GetResultsListCtrl()->GetItemTextEx(i, 2), value))
                { ages.push_back(value); }
            if (ReadabilityMessages::GetScoreValue(
                GetReadabilityScoresList()->GetResultsListCtrl()->GetItemTextEx(i, 4), value))
                { clozeScores.push_back(value); }
            }

        wxString gradeAverage(_(L"N/A")), ageAverage(_(L"N/A")), clozeAverage(_(L"N/A")),
            gradeMedian(_(L"N/A")), ageMedian(_(L"N/A")), clozeMedian(_(L"N/A")),
            gradeMode(_(L"N/A")), ageMode(_(L"N/A")), clozeMode(_(L"N/A"));
        // get the average grade
        if (grades.size() > 0)
            {
            gradeAverage = wxNumberFormatter::ToString(statistics::mean(grades), 1,
                                wxNumberFormatter::Style::Style_NoTrailingZeroes);
            gradeMedian = wxNumberFormatter::ToString(
                statistics::median(grades), 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes);
            std::set<double> modes = statistics::mode(grades, floor_value<double>() );
            gradeMode.Clear();
            for (auto modesIter = modes.cbegin();
                 modesIter != modes.cend();
                 ++modesIter)
                {
                gradeMode +=
                    doc->GetReadabilityMessageCatalog().GetFormattedValue(
                        wxNumberFormatter::ToString(*modesIter, 0, wxNumberFormatter::Style::Style_NoTrailingZeroes),
                        NumberFormatInfo::NumberFormatType::CustomFormatting) +
                    L"; ";
                }
            // chop off the last "; "
            if (gradeMode.length() > 2)
                { gradeMode.RemoveLast(2); }
            }
        // get the average grade level
        if (ages.size() > 0)
            {
            ageAverage = wxNumberFormatter::ToString(statistics::mean(ages), 1,
                            wxNumberFormatter::Style::Style_NoTrailingZeroes);
            ageMedian = wxNumberFormatter::ToString(
                statistics::median(ages), 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes);
            std::set<double> modes = statistics::mode(ages, floor_value<double>() );
            ageMode.Clear();
            for (auto modesIter = modes.cbegin();
                 modesIter != modes.cend();
                 ++modesIter)
                {
                ageMode +=
                    wxNumberFormatter::ToString(*modesIter, 0, wxNumberFormatter::Style::Style_NoTrailingZeroes) +
                    L"; ";
                }
            // chop off the last "; "
            if (ageMode.length() > 2)
                { ageMode.RemoveLast(2); }
            }
        // get the average cloze score
        if (clozeScores.size() > 0)
            {
            clozeAverage = wxNumberFormatter::ToString(statistics::mean(clozeScores), 2,
                                wxNumberFormatter::Style::Style_NoTrailingZeroes);
            clozeMedian = wxNumberFormatter::ToString(
                statistics::median(clozeScores), 2,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes);
            std::set<double> modes = statistics::mode(clozeScores, floor_value<double>() );
            clozeMode.Clear();
            for (auto modesIter = modes.cbegin();
                 modesIter != modes.cend();
                 ++modesIter)
                {
                clozeMode += wxNumberFormatter::ToString(*modesIter, 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes) +
                    L"; ";
                }
            // chop off the last "; "
            if (clozeMode.length() > 2)
                { clozeMode.RemoveLast(2); }
            }

        const int firstStatLocation = GetReadabilityScoresList()->GetResultsListCtrl()->AddRow(GetAverageLabel());
        GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(
            firstStatLocation, 1, gradeAverage,
            NumberFormatInfo(NumberFormatInfo::NumberFormatType::CustomFormatting, 1));
        GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(firstStatLocation, 2, ageAverage);
        GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(firstStatLocation, 3, _(L"N/A"));
        GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(firstStatLocation, 4, clozeAverage);
        GetReadabilityScoresList()->GetResultsListCtrl()->SetRowAttributes(firstStatLocation, statRowAttribs);

        // format the explanation of the averages
        wxString explanationString = L"<table class='minipage' style='width:100%;'>" +
            wxString::Format(L"\n\t<thead><tr><td style='background:%s;'><span style='color:%s;'>",
                ProjectReportFormat::GetReportHeaderColor().GetAsString(wxC2S_HTML_SYNTAX),
                ProjectReportFormat::GetReportHeaderFontColor().GetAsString(wxC2S_HTML_SYNTAX));
        explanationString += _(L"Averages");
        explanationString += L"</span></td></tr></thead>\n\t<tr><td>";
        explanationString +=
            wxString::Format(
                _(L"<p>Average grade level: %s<br />Average reading age: %s<br />"
                   "Average predicted cloze score: %s</p><p>Note that an average of the "
                   "scale values is not applicable because the scales used between tests are different.</p>"),
            doc->GetReadabilityMessageCatalog().GetFormattedValue(gradeAverage,
                NumberFormatInfo::NumberFormatType::CustomFormatting), ageAverage,
            clozeAverage) + L"</td></tr>\n</table>";

        GetReadabilityScoresList()->GetExplanations()[GetAverageLabel()] = explanationString;

        if (doc->GetStatisticsReportInfo().IsExtendedInformationEnabled())
            {
            // Mode
            long statLocation = GetReadabilityScoresList()->GetResultsListCtrl()->AddRow(GetModeLabel());
            GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(statLocation, 1, gradeMode);
            GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(statLocation, 2, ageMode);
            GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(statLocation, 3, _(L"N/A"));
            GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(statLocation, 4, clozeMode);
            GetReadabilityScoresList()->GetResultsListCtrl()->SetRowAttributes(statLocation, statRowAttribs);

            // format the explanation of the modes
            explanationString = L"<table class='minipage' style='width:100%;'>" +
                wxString::Format(L"\n\t<thead><tr><td style='background:%s;'><span style='color:%s;'>",
                    ProjectReportFormat::GetReportHeaderColor().GetAsString(wxC2S_HTML_SYNTAX),
                    ProjectReportFormat::GetReportHeaderFontColor().GetAsString(wxC2S_HTML_SYNTAX));
            explanationString += _(L"Modes");
            explanationString += L"</span></td></tr></thead>\n\t<tr><td>";
            explanationString +=
                wxString::Format(
                    _(L"<p>Grade level mode(s): %s<br />Reading age mode(s): %s<br />"
                       "Predicted cloze score mode(s): %s</p><p>The mode is the most frequently "
                       "occurring value in a range of data. Note that grade-level scores are rounded down "
                       "when searching for the mode.</p><p>Note that a mode of the scale values is not "
                       "applicable because the scales used between tests are different.</p>"),
                gradeMode,
                ageMode, clozeMode) +
                L"</td></tr>\n</table>";

            GetReadabilityScoresList()->GetExplanations()[GetModeLabel()] = explanationString;

            // Median
            statLocation = GetReadabilityScoresList()->GetResultsListCtrl()->AddRow(GetMedianLabel());
            GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(statLocation, 1, gradeMedian,
                NumberFormatInfo(NumberFormatInfo::NumberFormatType::CustomFormatting, 1));
            GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(statLocation, 2, ageMedian);
            GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(statLocation, 3, _(L"N/A"));
            GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(statLocation, 4, clozeMedian);
            GetReadabilityScoresList()->GetResultsListCtrl()->SetRowAttributes(statLocation, statRowAttribs);

            // format the explanation of the medians
            explanationString = L"<table class='minipage' style='width:100%;'>" +
                wxString::Format(L"\n\t<thead><tr><td style='background:%s;'><span style='color:%s;'>",
                    ProjectReportFormat::GetReportHeaderColor().GetAsString(wxC2S_HTML_SYNTAX),
                    ProjectReportFormat::GetReportHeaderFontColor().GetAsString(wxC2S_HTML_SYNTAX));
            explanationString += _(L"Medians");
            explanationString += L"</span></td></tr></thead>\n\t<tr><td>";
            explanationString +=  wxString::Format(
                _(L"<p>Grade level median: %s<br />Reading age median: %s<br />"
                   "Predicted cloze score median: %s</p><p>The median is the midpoint of a "
                   "given range of values that divides them into lower and higher halves.</p><p>"
                   "Note that a median of the scale values is not applicable because the scales "
                   "used between tests are different.</p>"),
                doc->GetReadabilityMessageCatalog().GetFormattedValue(gradeMedian,
                    NumberFormatInfo::NumberFormatType::CustomFormatting),
                ageMedian, clozeMedian) +
                L"</td></tr>\n</table>";

            GetReadabilityScoresList()->GetExplanations()[GetMedianLabel()] = explanationString;

            // get the standard deviation
            wxString gradeStdDev = ((grades.size() < 2) ?
                _(L"N/A") :
                wxNumberFormatter::ToString(statistics::standard_deviation(grades,
                                doc->GetVarianceMethod() == VarianceMethod::SampleVariance), 1,
                                wxNumberFormatter::Style::Style_NoTrailingZeroes));

            const wxString ageStdDev =
                ((ages.size() < 2) ?
                 _(L"N/A") :
                 wxNumberFormatter::ToString(statistics::standard_deviation(ages,
                                doc->GetVarianceMethod() == VarianceMethod::SampleVariance), 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes));

            const wxString clozeStdDev =
                ((clozeScores.size() < 2) ?
                 _(L"N/A") :
                 wxNumberFormatter::ToString(statistics::standard_deviation(clozeScores,
                                doc->GetVarianceMethod() == VarianceMethod::SampleVariance), 2,
                                wxNumberFormatter::Style::Style_NoTrailingZeroes));

            statLocation = GetReadabilityScoresList()->GetResultsListCtrl()->AddRow(GetStdDevLabel());
            GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(statLocation, 1, gradeStdDev,
                NumberFormatInfo(NumberFormatInfo::NumberFormatType::CustomFormatting, 1));
            GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(statLocation, 2, ageStdDev);
            GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(statLocation, 3, _(L"N/A"));
            GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(statLocation, 4, clozeStdDev);
            GetReadabilityScoresList()->GetResultsListCtrl()->SetRowAttributes(statLocation, statRowAttribs);

            // format the explanation of the variances
            explanationString = L"<table class='minipage' style='width:100%'>" +
                wxString::Format(L"\n    <thead><tr><td style='background:%s;'><span style='color:%s;'>",
                    ProjectReportFormat::GetReportHeaderColor().GetAsString(wxC2S_HTML_SYNTAX),
                    ProjectReportFormat::GetReportHeaderFontColor().GetAsString(wxC2S_HTML_SYNTAX));
            explanationString += _(L"Standard Deviations");
            explanationString += L"</span></td></tr></thead>\n    <tr><td>";
            explanationString +=
                wxString::Format(
                    _(L"<p>Grade level std. dev.: %s<br />Reading age std. dev.: %s<br />"
                       "Predicted cloze score std. dev.: %s</p><p>"
                       "Standard deviation is the measurement of how far values in a range "
                       "of data are spread apart from each other.</p><p>Note that at least two "
                       "valid test scores are required to have any standard deviation. "
                       "Also note that a standard deviation of the scale values is not applicable "
                       "because the scales used between tests are different.</p>"),
                doc->GetReadabilityMessageCatalog().GetFormattedValue(gradeStdDev,
                    NumberFormatInfo::NumberFormatType::CustomFormatting),
                ageStdDev, clozeStdDev) +
                L"</td></tr>\n</table>";

            GetReadabilityScoresList()->GetExplanations()[GetStdDevLabel()] = explanationString;
            }

        GetReadabilityScoresList()->GetResultsListCtrl()->SetSortableRange(0, firstStatLocation-1);
        }
    else
        { GetReadabilityScoresList()->GetResultsListCtrl()->SetSortableRange(0, 0); }

    if (GetReadabilityScoresList()->GetResultsListCtrl()->GetSortedColumn() == -1)
        {
        GetReadabilityScoresList()->GetResultsListCtrl()->SetSortedColumn(0, Wisteria::SortDirection::SortAscending);
        }
    GetReadabilityScoresList()->GetResultsListCtrl()->Resort();

    // select the item user had selected before the update
    const auto selectedItemLocation = GetReadabilityScoresList()->GetResultsListCtrl()->FindEx(selectedItem);
    if (selectedItemLocation != wxNOT_FOUND)
        { GetReadabilityScoresList()->GetResultsListCtrl()->Select(selectedItemLocation); }
    }

//-------------------------------------------------------
void ProjectView::OnTextWindowColorsChange([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    ProjectDoc* doc = dynamic_cast<ProjectDoc*>(GetDocument());
    if (!doc->IsSafeToUpdate())
        { return; }

    ToolsOptionsDlg optionsDlg(GetDocFrame(), dynamic_cast<ProjectDoc*>(GetDocument()), ToolsOptionsDlg::TextSection);
    if (GetSideBar()->GetSelectedFolderId() == SIDEBAR_DOLCH_SECTION_ID)
        { optionsDlg.SelectPage(ToolsOptionsDlg::DOCUMENT_DISPLAY_DOLCH_PAGE); }
    else
        { optionsDlg.SelectPage(ToolsOptionsDlg::DOCUMENT_DISPLAY_GENERAL_PAGE); }

    if (optionsDlg.ShowModal() == wxID_OK)
        {
        wxWindowUpdateLocker noUpdates(doc->GetDocumentWindow());
        BaseProjectProcessingLock processingLock(doc);
        wxBusyCursor wait;
        doc->DisplayHighlightedText(doc->GetTextHighlightColor(), doc->GetTextViewFont());
        doc->ResetRefreshRequired();
        doc->SetModifiedFlag();
        }
    }

//-------------------------------------------------------
void ProjectView::OnTextWindowFontChange([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    ProjectDoc* doc = dynamic_cast<ProjectDoc*>(GetDocument());
    if (!doc->IsSafeToUpdate())
        { return; }

    wxFontData data;
    data.SetInitialFont(doc->GetTextViewFont());

    wxFontDialog dialog(GetDocFrame(), data);
    if (dialog.ShowModal() == wxID_OK)
        {
        wxWindowUpdateLocker noUpdates(doc->GetDocumentWindow());
        BaseProjectProcessingLock processingLock(doc);
        doc->SetTextViewFont(dialog.GetFontData().GetChosenFont());
        wxBusyCursor wait;
        doc->DisplayHighlightedText(doc->GetTextHighlightColor(), doc->GetTextViewFont());
        doc->SetModifiedFlag();
        }
    doc->GetDocumentWindow()->Refresh();
    }

//-------------------------------------------------------
void ProjectView::OnLongFormat([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    ProjectDoc* doc = dynamic_cast<ProjectDoc*>(GetDocument());
    if (!doc->IsSafeToUpdate())
        { return; }
    wxWindowUpdateLocker noUpdates(doc->GetDocumentWindow());
    BaseProjectProcessingLock processingLock(doc);

    doc->GetReadabilityMessageCatalog().SetLongGradeScaleFormat(
        !doc->GetReadabilityMessageCatalog().IsUsingLongGradeScaleFormat());
    GetReadabilityScoresList()->GetResultsListCtrl()->Refresh();
    GetReadabilityScoresList()->GetResultsListCtrl()->SetColumnWidth(1,
        GetReadabilityScoresList()->GetResultsListCtrl()->EstimateColumnWidth(1));
    doc->SetModifiedFlag();
    }

// A test result is being removed from the project
//-------------------------------------------------------
void ProjectView::OnTestDeleteMenu([[maybe_unused]] wxCommandEvent& event)
    {
    wxRibbonButtonBarEvent cmd;
    OnTestDelete(cmd);
    }

//-------------------------------------------------------
void ProjectView::OnTestDelete([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    if (GetActiveProjectWindow() && GetActiveProjectWindow()->GetId() != READABILITY_SCORES_PAGE_ID)
        {
        const auto [parentId, childId] =
            GetSideBar()->FindSubItem(SIDEBAR_READABILITY_SCORES_SECTION_ID, READABILITY_SCORES_PAGE_ID);
        if (!childId.has_value())
            { return; }
        GetSideBar()->SelectSubItem(parentId.value(), childId.value());
        }
    const auto selectedIndex = GetReadabilityScoresList()->GetResultsListCtrl()->GetFirstSelected();
    if (selectedIndex != wxNOT_FOUND)
        {
        const wxString testToRemove = GetReadabilityScoresList()->GetResultsListCtrl()->GetItemText(selectedIndex);
        if (testToRemove == GetAverageLabel() || testToRemove == GetMedianLabel() ||
            testToRemove == GetStdDevLabel() || testToRemove == GetModeLabel())
            { return; }

        auto warningIter = WarningManager::GetWarning(_DT(L"remove-test-from-project"));
        // if they really want to remove this test
        if (warningIter != WarningManager::GetWarnings().end() &&
            warningIter->ShouldBeShown())
            {
            wxRichMessageDialog msg(GetDocFrame(),
                wxString::Format(_(L"Do you wish to remove \"%s\" from the project?"), testToRemove),
                _(L"Remove Test"), wxYES_NO|wxNO_DEFAULT|wxICON_QUESTION);
            msg.SetEscapeId(wxID_NO);
            msg.ShowCheckBox(_(L"Always delete without prompting"));
            const int dlgResponse = msg.ShowModal();
            // save the checkbox status
            if (msg.IsCheckBoxChecked() && (dlgResponse == wxID_YES))
                {
                warningIter->Show(false);
                warningIter->SetPreviousResponse(dlgResponse);
                }
            // now see if they said "Yes" or "No"
            if (dlgResponse == wxID_NO)
                { return; }
            }

        GetReadabilityScoresList()->GetResultsListCtrl()->DeleteItem(selectedIndex);
        UpdateStatistics();
        GetReadabilityScoresList()->GetExplanationView()->SetPage(wxString{});

        ProjectDoc* doc = dynamic_cast<ProjectDoc*>(GetDocument());
        doc->RemoveTest(testToRemove);
        // if removing Dolch, we need remove the Dolch section
        if (testToRemove == ReadabilityMessages::GetDolchLabel())
            { GetDolchSightWordsView().Clear(); }
        // remove the Averages row if there are no tests left
        if (GetReadabilityScoresList()->GetResultsListCtrl()->GetItemCount() == 1)
            {
            const long location = GetReadabilityScoresList()->GetResultsListCtrl()->FindEx(GetAverageLabel(), 0);
            if (location != wxNOT_FOUND)
                { GetReadabilityScoresList()->GetResultsListCtrl()->DeleteItem(location); }
            }
        if (GetReadabilityScoresList()->GetResultsListCtrl()->GetItemCount() == 0)
            {
            GetReadabilityScoresList()->GetExplanationView()->SetPage(
                wxString(L"<html><body>") + _(L"No readability test results currently available.") +
                wxString(L"</body></html>"));
            }
        // which tests are included may affect which stats and bars on the bar chart are included
        doc->RefreshRequired(ProjectRefresh::Minimal);
        doc->RefreshProject();
        }
    else
        { wxMessageBox(_(L"Please select a test to remove."), wxGetApp().GetAppName(), wxOK|wxICON_INFORMATION); }
    }

/// project view side bar was clicked
//-------------------------------------------------------
void ProjectView::OnItemSelected(wxCommandEvent& event)
    {
    // menu icons
    const auto copyIcon = wxGetApp().GetResourceManager().GetSVG(L"ribbon/copy.svg");
    const auto listIcon = wxGetApp().GetResourceManager().GetSVG(L"ribbon/list.svg");
    const auto histogramIcon = wxGetApp().GetResourceManager().GetSVG(L"ribbon/histogram.svg");
    const auto barChartIcon = wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-chart.svg");
    const auto boxPlotIcon = wxGetApp().GetResourceManager().GetSVG(L"ribbon/boxplot.svg");
    const auto heatmapIcon = wxGetApp().GetResourceManager().GetSVG(L"ribbon/heatmap.svg");
    const auto wordCloudIcon = wxGetApp().GetResourceManager().GetSVG(L"ribbon/word-cloud.svg");
    const auto pieChartIcon = wxGetApp().GetResourceManager().GetSVG(L"ribbon/donut-subgrouped.svg");
    const auto reportIcon = wxGetApp().GetResourceManager().GetSVG(L"ribbon/report.svg");
    const auto textIcon = wxGetApp().GetResourceManager().GetSVG(L"ribbon/document.svg");

    if (event.GetInt() == READABILITY_SCORES_PAGE_ID ||
        event.GetInt() == READABILITY_SCORES_SUMMARY_REPORT_PAGE_ID ||
        event.GetInt() == READABILITY_GOALS_PAGE_ID)
        {
        m_activeWindow = GetReadabilityResultsView().FindWindowById(event.GetInt());

        assert(m_activeWindow != nullptr);
        if (GetActiveProjectWindow())
            {
            GetSplitter()->GetWindow2()->Hide();
            GetSplitter()->ReplaceWindow(GetSplitter()->GetWindow2(), GetActiveProjectWindow());
            GetActiveProjectWindow()->Show();
            if (GetMenuBar())
                {
                GetMenuBar()->SetLabel(XRCID("ID_SAVE_ITEM"),
                    wxString::Format(_(L"Export %s..."), GetActiveProjectWindow()->GetName()) );
                MenuBarEnableAll(GetMenuBar(), wxID_SELECTALL, true);
                }
            if (GetRibbon())
                {
                wxWindowUpdateLocker noUpdates(GetRibbon());
                wxWindow* editButtonBarWindow = GetRibbon()->FindWindow(MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);
                if (editButtonBarWindow && editButtonBarWindow->IsKindOf(CLASSINFO(wxRibbonButtonBar)))
                    {
                    auto editButtonBar = dynamic_cast<wxRibbonButtonBar*>(editButtonBarWindow);
                    assert(editButtonBar != nullptr);
                    editButtonBar->ClearButtons();
                    if (GetActiveProjectWindow()->IsKindOf(CLASSINFO(wxHtmlWindow)) )
                        {
                        editButtonBar->AddButton(wxID_COPY, _(L"Copy"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/copy.svg"),
                            _(L"Copy the report."));
                        editButtonBar->AddButton(wxID_SELECTALL,
                            _(L"Select All"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/select-all.svg"),
                            _(L"Select All"));
                        }
                    else if (GetActiveProjectWindow()->IsKindOf(CLASSINFO(ExplanationListCtrl)) )
                        {
                        editButtonBar->AddToggleButton(XRCID("ID_LONG_FORMAT"),
                            _(L"Long Format"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/grade-display.svg"),
                            _(L"Display scores in long format."));
                        editButtonBar->ToggleButton(XRCID("ID_LONG_FORMAT"),
                            dynamic_cast<ProjectDoc*>(
                                GetDocument())->GetReadabilityMessageCatalog().IsUsingLongGradeScaleFormat());
                        editButtonBar->AddDropdownButton(XRCID("ID_GRADE_SCALES"),
                            _(L"Grade Scale"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/education.svg"),
                            _(L"Change the grade scale display of the scores."));
                        editButtonBar->AddButton(wxID_COPY, _(L"Copy"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/copy.svg"),
                            _(L"Copy the selected row(s)."));
                        editButtonBar->AddButton(XRCID("ID_LIST_SORT"),
                            _(L"Sort"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/sort.svg"),
                            _(L"Sort the list."));
                        }
                    else if (GetActiveProjectWindow()->IsKindOf(CLASSINFO(ListCtrlEx)) )
                        {
                        editButtonBar->AddHybridButton(wxID_COPY, _(L"Copy"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/copy.svg"),
                            _(L"Copy the selected row(s)."));
                        editButtonBar->AddButton(wxID_SELECTALL,
                            _(L"Select All"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/select-all.svg"),
                            _(L"Select All"));
                        editButtonBar->AddButton(XRCID("ID_LIST_SORT"),
                            _(L"Sort"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/sort.svg"),
                            _(L"Sort the list."));
                        }
                    GetRibbon()->GetPage(0)->Realize();
                    }
                }
            }
        }
    else if (event.GetInt() == FLESCH_CHART_PAGE_ID ||
        event.GetInt() == DB2_PAGE_ID ||
        event.GetInt() == FRY_PAGE_ID ||
        event.GetInt() == RAYGOR_PAGE_ID ||
        event.GetInt() == CRAWFORD_GRAPH_PAGE_ID ||
        event.GetInt() == FRASE_PAGE_ID ||
        event.GetInt() == SCHWARTZ_PAGE_ID ||
        event.GetInt() == LIX_GAUGE_PAGE_ID ||
        event.GetInt() == LIX_GAUGE_GERMAN_PAGE_ID ||
        event.GetInt() == GPM_FRY_PAGE_ID)
        {
        m_activeWindow = GetReadabilityResultsView().FindWindowById(event.GetInt());
        assert(GetActiveProjectWindow());
        if (GetActiveProjectWindow())
            {
            GetSplitter()->GetWindow2()->Hide();
            GetSplitter()->ReplaceWindow(GetSplitter()->GetWindow2(), GetActiveProjectWindow());
            GetActiveProjectWindow()->Show();
            if (GetMenuBar())
                {
                GetMenuBar()->SetLabel(XRCID("ID_SAVE_ITEM"),
                    wxString::Format(_(L"Export %s..."), GetActiveProjectWindow()->GetName()) );
                MenuBarEnableAll(GetMenuBar(), wxID_SELECTALL, false);
                }
            if (GetRibbon())
                {
                wxWindowUpdateLocker noUpdates(GetRibbon());
                wxWindow* editButtonBarWindow = GetRibbon()->FindWindow(MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);
                if (editButtonBarWindow && editButtonBarWindow->IsKindOf(CLASSINFO(wxRibbonButtonBar)))
                    {
                    auto editButtonBar = dynamic_cast<wxRibbonButtonBar*>(editButtonBarWindow);
                    assert(editButtonBar);

                    editButtonBar->ClearButtons();
                    editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_BACKGROUND"), _(L"Background"),
                                                     wxGetApp().ReadRibbonSvgIcon(L"ribbon/photos.svg"),
                                                     _(L"Set the graph's background."));
                    editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_FONTS"), _(L"Font"),
                                                     wxGetApp().ReadRibbonSvgIcon(L"ribbon/font.svg"),
                                                     _(L"Change the graph's fonts."));
                    editButtonBar->AddButton(XRCID("ID_EDIT_WATERMARK"), _(L"Watermark"),
                                             wxGetApp().ReadRibbonSvgIcon(L"ribbon/watermark.svg"),
                                             _(L"Add a watermark to the graph."));
                    editButtonBar->AddButton(XRCID("ID_EDIT_LOGO"), _(L"Logo"),
                                             wxGetApp().ReadRibbonSvgIcon(L"ribbon/logo.svg"),
                                             _(L"Add a logo to the graph."));
                    editButtonBar->AddToggleButton(XRCID("ID_DROP_SHADOW"), _(L"Shadows"),
                                                   wxGetApp().ReadRibbonSvgIcon(L"ribbon/shadow.svg"),
                                                   _(L"Display drop shadows on the graphs."));
                    editButtonBar->ToggleButton(XRCID("ID_DROP_SHADOW"),
                        dynamic_cast<ProjectDoc*>(GetDocument())->IsDisplayingDropShadows());

                    const auto graphType = dynamic_cast<Wisteria::Canvas*>(
                        GetActiveProjectWindow())->GetFixedObject(0, 0);

                    if (typeid(*graphType) == typeid(LixGaugeGerman))
                        {
                        editButtonBar->AddToggleButton(XRCID("ID_USE_ENGLISH_LABELS"),
                            _(L"English Labels"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/german2english.svg"),
                            _(L"Use translated (English) labels for the brackets."));
                        editButtonBar->ToggleButton(XRCID("ID_USE_ENGLISH_LABELS"),
                            dynamic_cast<ProjectDoc*>(GetDocument())->IsUsingEnglishLabelsForGermanLix());
                        }
                    else if (typeid(*graphType) == typeid(FryGraph) ||
                        typeid(*graphType) == typeid(RaygorGraph) ||
                        typeid(*graphType) == typeid(SchwartzGraph))
                        {
                        editButtonBar->AddButton(XRCID("ID_INVALID_REGION_COLOR"),
                            _(L"Invalid Region"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/invalid-region.svg"),
                            _(L"Change the color of the invalid regions."));
                        }
                    if (typeid(*graphType) == typeid(RaygorGraph))
                        {
                        editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_RAYGOR_STYLE"),
                            _(L"Raygor Style"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/raygor-style.svg"),
                            _(L"Change the layout style of the Raygor graph."));
                        }
                    // not showing connection lines for a Flesch Chart with only one score doesn't make much sense,
                    // so don't bother adding a button for that on a standard project.

                    editButtonBar->AddButton(wxID_COPY, _(L"Copy"),
                                             wxGetApp().ReadRibbonSvgIcon(L"ribbon/copy.svg"),
                                             _(L"Copy the graph."));
                    editButtonBar->AddHybridButton(wxID_ZOOM_IN, _(L"Zoom"),
                                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/zoom-in.svg"),
                                            _(L"Zoom"));
                    GetRibbon()->GetPage(0)->Realize();
                    }
                }
            }
        }
    else if (event.GetExtraLong() == SIDEBAR_SENTENCES_BREAKDOWN_SECTION_ID)
        {
        m_activeWindow = GetSentencesBreakdownView().FindWindowById(event.GetInt());
        assert(GetActiveProjectWindow());
        if (GetActiveProjectWindow())
            {
            GetSplitter()->GetWindow2()->Hide();
            GetSplitter()->ReplaceWindow(GetSplitter()->GetWindow2(), GetActiveProjectWindow());
            GetActiveProjectWindow()->Show();
            if (GetMenuBar())
                {
                GetMenuBar()->SetLabel(XRCID("ID_SAVE_ITEM"),
                    wxString::Format(_(L"Export %s..."), GetActiveProjectWindow()->GetName()));
                MenuBarEnableAll(GetMenuBar(), wxID_SELECTALL, true);
                }
            if (GetRibbon())
                {
                wxWindowUpdateLocker noUpdates(GetRibbon());
                wxWindow* editButtonBarWindow = GetRibbon()->FindWindow(MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);
                if (editButtonBarWindow && editButtonBarWindow->IsKindOf(CLASSINFO(wxRibbonButtonBar)))
                    {
                    auto editButtonBar = dynamic_cast<wxRibbonButtonBar*>(editButtonBarWindow);
                    assert(editButtonBar);
                    editButtonBar->ClearButtons();

                    if (typeid(*GetActiveProjectWindow()) == typeid(Wisteria::Canvas))
                        {
                        editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_BACKGROUND"),
                            _(L"Background"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/photos.svg"),
                            _(L"Set the graph's background."));
                        editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_FONTS"),
                            _(L"Font"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/font.svg"),
                            _(L"Change the graph's fonts."));
                        editButtonBar->AddButton(XRCID("ID_EDIT_WATERMARK"),
                            _(L"Watermark"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/watermark.svg"),
                            _(L"Add a watermark to the graph."));
                        editButtonBar->AddButton(XRCID("ID_EDIT_LOGO"),
                            _(L"Logo"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/logo.svg"),
                            _(L"Add a logo to the graph."));
                        editButtonBar->AddToggleButton(XRCID("ID_DROP_SHADOW"),
                            _(L"Shadows"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/shadow.svg"),
                            _(L"Display drop shadows on the graphs."));
                        editButtonBar->ToggleButton(XRCID("ID_DROP_SHADOW"),
                            dynamic_cast<ProjectDoc*>(GetDocument())->IsDisplayingDropShadows());
                        if (typeid(*GetActiveProjectWindow()) == typeid(Wisteria::Canvas) &&
                            typeid(*dynamic_cast<Wisteria::Canvas*>(
                                GetActiveProjectWindow())->GetFixedObject(0, 0)) ==
                                    typeid(Wisteria::Graphs::Histogram))
                            {
                            editButtonBar->AddDropdownButton(XRCID("ID_EDIT_HISTOGRAM_BAR_STYLE"),
                                _(L"Bar Style"),
                                wxGetApp().ReadRibbonSvgIcon(L"ribbon/bar-top-to-bottom.svg"),
                                _(L"Changes the bar appearance."));
                            editButtonBar->AddDropdownButton(XRCID("ID_EDIT_HISTOBAR_LABELS"),
                                _(L"Labels"),
                                wxGetApp().ReadRibbonSvgIcon(L"ribbon/bar-labels.svg"),
                                _(L"Changes what is displayed on the bars' labels."));
                            }
                        else if (typeid(*GetActiveProjectWindow()) == typeid(Wisteria::Canvas) &&
                            typeid(*dynamic_cast<Wisteria::Canvas*>(
                                GetActiveProjectWindow())->GetFixedObject(0, 0)) ==
                                    typeid(Wisteria::Graphs::BoxPlot))
                            {
                            editButtonBar->AddDropdownButton(XRCID("ID_EDIT_BOX_STYLE"),
                                _(L"Box Style"),
                                wxGetApp().ReadRibbonSvgIcon(L"ribbon/bar-top-to-bottom.svg"),
                                _(L"Changes the box appearance."));
                            editButtonBar->AddToggleButton(XRCID("ID_BOX_PLOT_DISPLAY_LABELS"),
                                _(L"Display Labels"),
                                wxGetApp().ReadRibbonSvgIcon(L"ribbon/show-all-labels.svg"),
                                _(L"Displays labels on the box and whiskers."));
                            editButtonBar->ToggleButton(XRCID("ID_BOX_PLOT_DISPLAY_LABELS"),
                                dynamic_cast<ProjectDoc*>(GetDocument())->IsDisplayingBoxPlotLabels());
                            editButtonBar->AddToggleButton(XRCID("ID_BOX_PLOT_DISPLAY_ALL_POINTS"),
                                _(L"Display Points"),
                                wxGetApp().ReadRibbonSvgIcon(L"ribbon/show-all-points.svg"),
                                _(L"Displays all data points onto the plot."));
                            editButtonBar->ToggleButton(XRCID("ID_BOX_PLOT_DISPLAY_ALL_POINTS"),
                                dynamic_cast<ProjectDoc*>(GetDocument())->IsShowingAllBoxPlotPoints());
                            }
                        editButtonBar->AddButton(wxID_COPY, _(L"Copy"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/copy.svg"),
                            _(L"Copy the graph."));
                        editButtonBar->AddHybridButton(wxID_ZOOM_IN,
                            _(L"Zoom"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/zoom-in.svg"),
                            _(L"Zoom"));
                        }
                    else if (GetActiveProjectWindow()->IsKindOf(CLASSINFO(ListCtrlEx)) )
                        {
                        editButtonBar->AddHybridButton(wxID_COPY, _(L"Copy"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/copy.svg"),
                            _(L"Copy the selected row(s)."));
                        editButtonBar->AddButton(wxID_SELECTALL,
                            _(L"Select All"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/select-all.svg"),
                            _(L"Select All"));
                        editButtonBar->AddButton(XRCID("ID_VIEW_ITEM"),
                            _(L"View Item"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/view-spreadsheet-item.svg"),
                            _(L"View the selected row in tabular format."));
                        editButtonBar->AddButton(XRCID("ID_LIST_SORT"),
                            _(L"Sort"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/sort.svg"),
                            _(L"Sort the list."));
                        }
                    GetRibbon()->GetPage(0)->Realize();
                    }
                }
            }
        }
    else if (event.GetExtraLong() == SIDEBAR_STATS_SUMMARY_SECTION_ID)
        {
        m_activeWindow = GetSummaryView().FindWindowById(event.GetInt());
        assert(GetActiveProjectWindow());
        if (GetActiveProjectWindow())
            {
            GetSplitter()->GetWindow2()->Hide();
            GetSplitter()->ReplaceWindow(GetSplitter()->GetWindow2(), GetActiveProjectWindow());
            GetActiveProjectWindow()->Show();
            if (GetMenuBar())
                {
                GetMenuBar()->SetLabel(XRCID("ID_SAVE_ITEM"), wxString::Format(_(L"Export %s..."),
                                       GetActiveProjectWindow()->GetName()));
                MenuBarEnableAll(GetMenuBar(), wxID_SELECTALL, true);
                }
            if (GetRibbon())
                {
                wxWindowUpdateLocker noUpdates(GetRibbon());
                wxWindow* editButtonBarWindow = GetRibbon()->FindWindow(MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);
                if (editButtonBarWindow && editButtonBarWindow->IsKindOf(CLASSINFO(wxRibbonButtonBar)))
                    {
                    auto editButtonRibbonBar = dynamic_cast<wxRibbonButtonBar*>(editButtonBarWindow);
                    assert(editButtonRibbonBar);
                    editButtonRibbonBar->ClearButtons();

                    if (GetActiveProjectWindow()->IsKindOf(CLASSINFO(ListCtrlEx)) )
                        {
                        editButtonRibbonBar->AddButton(XRCID("ID_EDIT_STATS_REPORT"),
                            _(L"Edit Report"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/edit-report.svg"),
                            _(L"Select which statistics to include in the report."));
                        editButtonRibbonBar->AddHybridButton(wxID_COPY,
                            _(L"Copy"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/copy.svg"),
                            _(L"Copy the selected row(s)."));
                        editButtonRibbonBar->AddButton(wxID_SELECTALL,
                            _(L"Select All"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/select-all.svg"),
                            _(L"Select All"));
                        editButtonRibbonBar->AddButton(XRCID("ID_LIST_SORT"),
                            _(L"Sort"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/sort.svg"),
                            _(L"Sort the list."));
                        }
                    else if (GetActiveProjectWindow()->IsKindOf(CLASSINFO(HtmlTableWindow)) )
                        {
                        editButtonRibbonBar->AddButton(XRCID("ID_EDIT_STATS_REPORT"),
                            _(L"Edit Report"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/edit-report.svg"),
                            _(L"Select which statistics to include in the report."));
                        editButtonRibbonBar->AddButton(wxID_COPY,
                            _(L"Copy"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/copy.svg"),
                            _(L"Copy"));
                        editButtonRibbonBar->AddButton(wxID_SELECTALL,
                            _(L"Select All"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/select-all.svg"),
                            _(L"Select All"));
                        }
                    GetRibbon()->GetPage(0)->Realize();
                    }
                }
            }
        }
    else if (event.GetExtraLong() == SIDEBAR_WORDS_BREAKDOWN_SECTION_ID)
        {
        // Note that word-list tests can have a list control and highlighted report
        // with the same integral ID, so rely on searching by the ID and name of the window.
        m_activeWindow = GetWordsBreakdownView().FindWindowByIdAndLabel(event.GetInt(), event.GetString());
        if (!GetActiveProjectWindow())
            { m_activeWindow = GetWordsBreakdownView().FindWindowById(event.GetInt()); }

        assert(m_activeWindow != nullptr);
        if (GetActiveProjectWindow())
            {
            GetSplitter()->GetWindow2()->Hide();
            GetSplitter()->ReplaceWindow(GetSplitter()->GetWindow2(), GetActiveProjectWindow());
            GetActiveProjectWindow()->Show();
            if (GetMenuBar())
                {
                GetMenuBar()->SetLabel(XRCID("ID_SAVE_ITEM"), wxString::Format(_(L"Export %s..."),
                                       GetActiveProjectWindow()->GetName()) );
                MenuBarEnableAll(GetMenuBar(), wxID_SELECTALL, true);
                }
            if (GetRibbon())
                {
                wxWindowUpdateLocker noUpdates(GetRibbon());
                wxWindow* editButtonBarWindow = GetRibbon()->FindWindow(MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);
                if (editButtonBarWindow && editButtonBarWindow->IsKindOf(CLASSINFO(wxRibbonButtonBar)))
                    {
                    auto editButtonRibbonBar = dynamic_cast<wxRibbonButtonBar*>(editButtonBarWindow);
                    assert(editButtonRibbonBar);
                    editButtonRibbonBar->ClearButtons();

                    if (typeid(*GetActiveProjectWindow()) == typeid(Wisteria::Canvas))
                        {
                        if (typeid(*GetActiveProjectWindow()) == typeid(Wisteria::Canvas) &&
                            typeid(*dynamic_cast<Wisteria::Canvas*>(
                                GetActiveProjectWindow())->GetFixedObject(0, 0)) ==
                            typeid(Wisteria::Graphs::PieChart))
                            {
                            editButtonRibbonBar->AddButton(XRCID("ID_EDIT_GRAPH_COLOR_SCHEME"), _(L"Colors"),
                                wxGetApp().ReadRibbonSvgIcon(L"ribbon/color-wheel.svg"),
                                _(L"Select the color scheme for the pie chart."));
                            editButtonRibbonBar->AddToggleButton(
                                XRCID("ID_EDIT_GRAPH_SHOWCASE_COMPLEX_WORDS"), _(L"Showcase Complexity"),
                                wxGetApp().ReadRibbonSvgIcon(L"ribbon/showcase.svg"),
                                _(L"Toggle whether complex word slices are being showcased."));
                            editButtonRibbonBar->ToggleButton(XRCID("ID_EDIT_GRAPH_SHOWCASE_COMPLEX_WORDS"),
                                dynamic_cast<ProjectDoc*>(GetDocument())->IsShowcasingComplexWords());
                            }
                        if (typeid(*GetActiveProjectWindow()) == typeid(Wisteria::Canvas) &&
                            typeid(*dynamic_cast<Wisteria::Canvas*>(
                                GetActiveProjectWindow())->GetFixedObject(0, 0)) ==
                            typeid(Wisteria::Graphs::Histogram))
                            {
                            editButtonRibbonBar->AddToggleButton(
                                XRCID("ID_EDIT_GRAPH_SHOWCASE_COMPLEX_WORDS"), _(L"Showcase Complexity"),
                                wxGetApp().ReadRibbonSvgIcon(L"ribbon/showcase.svg"),
                                _(L"Toggle whether complex word bars are being showcased."));
                            editButtonRibbonBar->ToggleButton(XRCID("ID_EDIT_GRAPH_SHOWCASE_COMPLEX_WORDS"),
                                dynamic_cast<ProjectDoc*>(GetDocument())->IsShowcasingComplexWords());
                            }
                        editButtonRibbonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_BACKGROUND"),
                            _(L"Background"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/photos.svg"),
                            _(L"Set the graph's background."));
                        editButtonRibbonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_FONTS"),
                            _(L"Font"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/font.svg"),
                            _(L"Change the graph's fonts."));
                        editButtonRibbonBar->AddButton(XRCID("ID_EDIT_WATERMARK"),
                            _(L"Watermark"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/watermark.svg"),
                            _(L"Add a watermark to the graph."));
                        editButtonRibbonBar->AddButton(XRCID("ID_EDIT_LOGO"),
                            _(L"Logo"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/logo.svg"),
                            _(L"Add a logo to the graph."));
                        editButtonRibbonBar->AddToggleButton(XRCID("ID_DROP_SHADOW"),
                            _(L"Shadows"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/shadow.svg"),
                            _(L"Display drop shadows on the graphs."));
                        editButtonRibbonBar->ToggleButton(XRCID("ID_DROP_SHADOW"),
                            dynamic_cast<ProjectDoc*>(GetDocument())->IsDisplayingDropShadows());
                        if (typeid(*GetActiveProjectWindow()) == typeid(Wisteria::Canvas) &&
                            typeid(*dynamic_cast<Wisteria::Canvas*>(
                                GetActiveProjectWindow())->GetFixedObject(0, 0)) ==
                                    typeid(Wisteria::Graphs::Histogram))
                            {
                            editButtonRibbonBar->AddDropdownButton(XRCID("ID_EDIT_HISTOGRAM_BAR_STYLE"),
                                _(L"Bar Style"),
                                wxGetApp().ReadRibbonSvgIcon(L"ribbon/bar-top-to-bottom.svg"),
                                _(L"Changes the bar appearance."));
                            editButtonRibbonBar->AddDropdownButton(XRCID("ID_EDIT_HISTOBAR_LABELS"),
                                _(L"Labels"),
                                wxGetApp().ReadRibbonSvgIcon(L"ribbon/bar-labels.svg"),
                                _(L"Changes what is displayed on the bars' labels."));
                            }
                        else if (typeid(*GetActiveProjectWindow()) == typeid(Wisteria::Canvas) &&
                            typeid(*dynamic_cast<Wisteria::Canvas*>(
                                GetActiveProjectWindow())->GetFixedObject(0, 0)) ==
                                    typeid(Wisteria::Graphs::BarChart))
                            {
                            editButtonRibbonBar->AddDropdownButton(XRCID("ID_EDIT_BAR_STYLE"),
                                _(L"Bar Style"),
                                wxGetApp().ReadRibbonSvgIcon(L"ribbon/bar-top-to-bottom.svg"),
                                _(L"Changes the bar appearance."));
                            editButtonRibbonBar->AddDropdownButton(XRCID("ID_EDIT_BAR_ORIENTATION"),
                                _(L"Orientation"),
                                wxGetApp().ReadRibbonSvgIcon(L"ribbon/axis-orientation.svg"),
                                _(L"Changes the axis orientation."));
                            editButtonRibbonBar->AddToggleButton(XRCID("ID_EDIT_BAR_LABELS"),
                                _(L"Labels"),
                                wxGetApp().ReadRibbonSvgIcon(L"ribbon/bar-labels.svg"),
                                _(L"Shows or hides the bars' labels."));
                            editButtonRibbonBar->ToggleButton(XRCID("ID_EDIT_BAR_LABELS"),
                                dynamic_cast<ProjectDoc*>(GetDocument())->IsDisplayingBarChartLabels());
                            editButtonRibbonBar->AddDropdownButton(XRCID("ID_GRAPH_SORT"),
                                _(L"Sort"),
                                wxGetApp().ReadRibbonSvgIcon(L"ribbon/bar-sort.svg"),
                                _(L"Sort the bars in the graph."));
                            }
                        editButtonRibbonBar->AddButton(wxID_COPY,
                            _(L"Copy"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/copy.svg"),
                            _(L"Copy the graph."));
                        editButtonRibbonBar->AddHybridButton(wxID_ZOOM_IN,
                            _(L"Zoom"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/zoom-in.svg"),
                            _(L"Zoom"));
                        }
                    else if (GetActiveProjectWindow()->IsKindOf(CLASSINFO(ListCtrlEx)) )
                        {
                        editButtonRibbonBar->AddHybridButton(wxID_COPY,
                            _(L"Copy"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/copy.svg"),
                            _(L"Copy the selected row(s)."));
                        editButtonRibbonBar->AddButton(wxID_SELECTALL,
                            _(L"Select All"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/select-all.svg"),
                            _(L"Select All"));
                        editButtonRibbonBar->AddButton(XRCID("ID_LIST_SORT"),
                            _(L"Sort"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/sort.svg"),
                            _(L"Sort the list."));
                        // compressed list of words combine stemmed words into a list,
                        // so it's not a list of individual words that a user can add
                        if (event.GetInt() != ALL_WORDS_CONDENSED_LIST_PAGE_ID)
                            {
                            editButtonRibbonBar->AddButton(XRCID("ID_EXCLUDE_SELECTED"),
                                _(L"Exclude Selected"),
                                wxGetApp().ReadRibbonSvgIcon(L"ribbon/exclude-selected.svg"),
                                _(L"Exclude selected word(s)."));
                            }
                        editButtonRibbonBar->AddButton(XRCID("ID_SUMMATION"),
                            _(L"Sum"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/sum.svg"),
                            _(L"Total the values from the selected column."));
                        }
                    else if (GetActiveProjectWindow()->IsKindOf(CLASSINFO(FormattedTextCtrl)) )
                        {
                        editButtonRibbonBar->AddButton(XRCID("ID_TEXT_WINDOW_FONT"), _(L"Font"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/font.svg"),
                            _(L"Change the font."));
                        editButtonRibbonBar->AddButton(XRCID("ID_TEXT_WINDOW_COLORS"),
                            _(L"Highlight"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/highlighting.svg"),
                            _(L"Change the highlight colors."));
                        editButtonRibbonBar->AddButton(wxID_COPY, _(L"Copy"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/copy.svg"),
                            _(L"Copy"));
                        editButtonRibbonBar->AddButton(wxID_SELECTALL, _(L"Select All"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/select-all.svg"),
                            _(L"Select All"));
                        }
                    GetRibbon()->GetPage(0)->Realize();
                    }
                }
            }
        }
    else if (event.GetExtraLong() == SIDEBAR_GRAMMAR_SECTION_ID)
        {
        m_activeWindow = GetGrammarView().FindWindowById(event.GetInt());

        assert(m_activeWindow != nullptr);
        if (GetActiveProjectWindow())
            {
            GetSplitter()->GetWindow2()->Hide();
            GetSplitter()->ReplaceWindow(GetSplitter()->GetWindow2(), GetActiveProjectWindow());
            GetActiveProjectWindow()->Show();
            if (GetMenuBar())
                {
                GetMenuBar()->SetLabel(XRCID("ID_SAVE_ITEM"),
                    wxString::Format(_(L"Export %s..."), GetActiveProjectWindow()->GetName()) );
                MenuBarEnableAll(GetMenuBar(), wxID_SELECTALL, true);
                }
            if (GetRibbon())
                {
                wxWindowUpdateLocker noUpdates(GetRibbon());
                wxWindow* editButtonBarWindow = GetRibbon()->FindWindow(MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);
                if (editButtonBarWindow && editButtonBarWindow->IsKindOf(CLASSINFO(wxRibbonButtonBar)))
                    {
                    auto editButtonRibbonBar = dynamic_cast<wxRibbonButtonBar*>(editButtonBarWindow);
                    assert(editButtonRibbonBar != nullptr);
                    editButtonRibbonBar->ClearButtons();
                    if (GetActiveProjectWindow()->IsKindOf(CLASSINFO(FormattedTextCtrl)) )
                        {
                        editButtonRibbonBar->AddButton(XRCID("ID_TEXT_WINDOW_FONT"),
                            _(L"Font"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/font.svg"),
                            _(L"Change the font."));
                        editButtonRibbonBar->AddButton(XRCID("ID_TEXT_WINDOW_COLORS"),
                            _(L"Highlight"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/highlighting.svg"),
                            _(L"Change the highlight colors."));
                        }
                    if (GetActiveProjectWindow()->IsKindOf(CLASSINFO(ListCtrlEx)))
                        {
                        editButtonRibbonBar->AddHybridButton(wxID_COPY, _(L"Copy"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/copy.svg"),
                            _(L"Copy"));
                        }
                    else
                        {
                        editButtonRibbonBar->AddButton(wxID_COPY, _(L"Copy"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/copy.svg"),
                            _(L"Copy"));
                        }
                    editButtonRibbonBar->AddButton(wxID_SELECTALL,
                        _(L"Select All"),
                        wxGetApp().ReadRibbonSvgIcon(L"ribbon/select-all.svg"),
                        _(L"Select All"));
                    if (GetActiveProjectWindow()->IsKindOf(CLASSINFO(ListCtrlEx)) )
                        {
                        if (event.GetInt() == OVERUSED_WORDS_BY_SENTENCE_LIST_PAGE_ID)
                            { editButtonRibbonBar->AddButton(XRCID("ID_VIEW_ITEM"),
                                _(L"View Item"),
                                wxGetApp().ReadRibbonSvgIcon(L"ribbon/view-spreadsheet-item.svg"),
                                _(L"View the selected row in tabular format.")); }
                        editButtonRibbonBar->AddButton(XRCID("ID_LIST_SORT"),
                            _(L"Sort"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/sort.svg"),
                            _(L"Sort the list."));
                        }
                    if (event.GetInt() == MISSPELLED_WORD_LIST_PAGE_ID ||
                        event.GetInt() == PASSIVE_VOICE_PAGE_ID ||
                        event.GetInt() == PROPER_NOUNS_LIST_PAGE_ID)
                        { editButtonRibbonBar->AddButton(XRCID("ID_SUMMATION"),
                            _(L"Sum"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/sum.svg"),
                            _(L"Total the values from the selected column.")); }
                    GetRibbon()->GetPage(0)->Realize();
                    }
                }
            }
        }
    else if (event.GetExtraLong() == SIDEBAR_DOLCH_SECTION_ID)
        {
        m_activeWindow = GetDolchSightWordsView().FindWindowById(event.GetInt());

        assert(m_activeWindow != nullptr);
        if (GetActiveProjectWindow())
            {
            GetSplitter()->GetWindow2()->Hide();
            GetSplitter()->ReplaceWindow(GetSplitter()->GetWindow2(), GetActiveProjectWindow());
            GetActiveProjectWindow()->Show();
            if (GetMenuBar())
                {
                GetMenuBar()->SetLabel(XRCID("ID_SAVE_ITEM"),
                    wxString::Format(_(L"Export %s..."), GetActiveProjectWindow()->GetName()) );
                MenuBarEnableAll(GetMenuBar(), wxID_SELECTALL, true);
                }
            if (GetRibbon())
                {
                wxWindowUpdateLocker noUpdates(GetRibbon());
                wxWindow* editButtonBarWindow = GetRibbon()->FindWindow(MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);
                if (editButtonBarWindow && editButtonBarWindow->IsKindOf(CLASSINFO(wxRibbonButtonBar)))
                    {
                    auto editButtonRibbonBar = dynamic_cast<wxRibbonButtonBar*>(editButtonBarWindow);
                    assert(editButtonRibbonBar != nullptr);
                    editButtonRibbonBar->ClearButtons();
                    if (GetActiveProjectWindow()->IsKindOf(CLASSINFO(FormattedTextCtrl)) )
                        {
                        editButtonRibbonBar->AddButton(XRCID("ID_TEXT_WINDOW_FONT"),
                            _(L"Font"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/font.svg"),
                            _(L"Change the font."));
                        editButtonRibbonBar->AddButton(XRCID("ID_TEXT_WINDOW_COLORS"),
                            _(L"Highlight"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/highlighting.svg"),
                            _(L"Change the highlight colors."));
                        }
                    else if (typeid(*GetActiveProjectWindow()) == typeid(Wisteria::Canvas) &&
                             typeid(*dynamic_cast<Wisteria::Canvas*>(
                                 GetActiveProjectWindow())->GetFixedObject(0, 0)) ==
                                    typeid(Wisteria::Graphs::BarChart))
                        {
                        editButtonRibbonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_BACKGROUND"),
                            _(L"Background"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/photos.svg"),
                            _(L"Set the graph's background."));
                        editButtonRibbonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_FONTS"),
                            _(L"Font"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/font.svg"),
                            _(L"Change the graph's fonts."));
                        editButtonRibbonBar->AddButton(XRCID("ID_EDIT_WATERMARK"),
                            _(L"Watermark"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/watermark.svg"),
                            _(L"Add a watermark to the graph."));
                        editButtonRibbonBar->AddButton(XRCID("ID_EDIT_LOGO"),
                            _(L"Logo"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/logo.svg"),
                            _(L"Add a logo to the graph."));
                        editButtonRibbonBar->AddToggleButton(XRCID("ID_DROP_SHADOW"),
                            _(L"Shadows"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/shadow.svg"),
                            _(L"Display drop shadows on the graphs."));
                        editButtonRibbonBar->ToggleButton(XRCID("ID_DROP_SHADOW"),
                            dynamic_cast<ProjectDoc*>(GetDocument())->IsDisplayingDropShadows());
                        editButtonRibbonBar->AddDropdownButton(XRCID("ID_EDIT_BAR_STYLE"),
                            _(L"Bar Style"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/bar-top-to-bottom.svg"),
                            _(L"Changes the bar appearance."));
                        editButtonRibbonBar->AddToggleButton(XRCID("ID_EDIT_BAR_LABELS"),
                            _(L"Labels"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/bar-labels.svg"),
                            _(L"Shows or hides the bars' labels."));
                        editButtonRibbonBar->ToggleButton(XRCID("ID_EDIT_BAR_LABELS"),
                            dynamic_cast<ProjectDoc*>(GetDocument())->IsDisplayingBarChartLabels());
                        editButtonRibbonBar->AddDropdownButton(XRCID("ID_EDIT_BAR_ORIENTATION"),
                            _(L"Orientation"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/axis-orientation.svg"),
                            _(L"Changes the axis orientation."));
                        editButtonRibbonBar->AddDropdownButton(XRCID("ID_GRAPH_SORT"),
                            _(L"Sort"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/bar-sort.svg"),
                            _(L"Sort the bars in the graph."));
                        }
                    else if (GetActiveProjectWindow()->IsKindOf(CLASSINFO(HtmlTableWindow)) )
                        {
                        editButtonRibbonBar->AddButton(XRCID("ID_EDIT_STATS_REPORT"),
                            _(L"Edit Report"),
                           wxGetApp().ReadRibbonSvgIcon(L"ribbon/edit-report.svg"),
                            _(L"Select which statistics to include in the report."));
                        }
                    if (typeid(*GetActiveProjectWindow()) != typeid(Wisteria::Canvas))
                        {
                        if (GetActiveProjectWindow()->IsKindOf(CLASSINFO(ListCtrlEx)))
                            {
                            editButtonRibbonBar->AddHybridButton(wxID_COPY, _(L"Copy"),
                                wxGetApp().ReadRibbonSvgIcon(L"ribbon/copy.svg"),
                                _(L"Copy"));
                            }
                        else
                            {
                            editButtonRibbonBar->AddButton(wxID_COPY, _(L"Copy"),
                                wxGetApp().ReadRibbonSvgIcon(L"ribbon/copy.svg"),
                                _(L"Copy"));
                            }
                        editButtonRibbonBar->AddButton(wxID_SELECTALL,
                            _(L"Select All"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/select-all.svg"),
                            _(L"Select All"));
                        if (GetActiveProjectWindow()->IsKindOf(CLASSINFO(ListCtrlEx)) )
                            {
                            editButtonRibbonBar->AddButton(XRCID("ID_LIST_SORT"),
                                _(L"Sort"),
                                wxGetApp().ReadRibbonSvgIcon(L"ribbon/sort.svg"),
                                _(L"Sort the list."));
                            if (event.GetInt() == NON_DOLCH_WORDS_LIST_PAGE_ID ||
                                event.GetInt() == DOLCH_WORDS_LIST_PAGE_ID)
                                { editButtonRibbonBar->AddButton(XRCID("ID_SUMMATION"),
                                    _(L"Sum"),
                                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/sum.svg"),
                                    _(L"Total the values from the selected column.")); }
                            }
                        }
                    else
                        {
                        editButtonRibbonBar->AddButton(wxID_COPY, _(L"Copy"),
                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/copy.svg"),
                            _(L"Copy"));
                        }
                    GetRibbon()->GetPage(0)->Realize();
                    }
                }
            }
        }

    // add the label for the window type to the export menu item
    assert(GetActiveProjectWindow());
    if (wxMenuItem* exportMenuItem{ m_exportMenu.FindChildItem(XRCID("ID_SAVE_ITEM")) };
        exportMenuItem != nullptr && GetActiveProjectWindow() != nullptr)
        {
        exportMenuItem->SetItemLabel(
            wxString::Format(_(L"Export %s..."), GetActiveProjectWindow()->GetName()));
        }

    event.Skip();
    }

//-------------------------------------------------------
bool ProjectView::ExportAll(const wxString& folder, wxString listExt, wxString textExt, wxString graphExt,
                            const bool includeWordsBreakdown,
                            const bool includeSentencesBreakdown,
                            const bool includeTestScores, const bool includeStatistics, const bool includeGrammar,
                            const bool includeSightWords,
                            const bool includeLists,
                            const bool includeTextReports,
                            const Wisteria::UI::ImageExportOptions& graphOptions)
    {
    const ProjectDoc* doc = dynamic_cast<const ProjectDoc*>(GetDocument());

    if (!wxFileName::DirExists(folder))
        {
        if (folder.empty() || !wxFileName::Mkdir(folder, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL) )
            { return false; }
        }
    // validate the extensions
    if (listExt.empty())
        { listExt = L".htm"; }
    else if (listExt[0] != L'.')
        { listExt.insert(0, L"."); }

    if (textExt.empty())
        { textExt = L".htm"; }
    else if (textExt[0] != L'.')
        { textExt.insert(0, L"."); }

    if (graphExt.empty())
        { graphExt = L".png"; }
    else if (graphExt[0] != L'.')
        { graphExt.insert(0, L"."); }

    BaseProjectProcessingLock processingLock(dynamic_cast<ProjectDoc*>(GetDocument()));

    wxBusyCursor bc;
    wxBusyInfo bi(wxBusyInfoFlags().Text(_(L"Exporting project...")));

    // the results window
    if (includeTestScores)
        {
        if (!wxFileName::Mkdir(folder + wxFileName::GetPathSeparator() + GetReadabilityScoresLabel(),
                               wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL))
            {
            wxMessageBox(wxString::Format(_(L"Unable to create \"%s\" folder."), GetReadabilityScoresLabel()),
                wxGetApp().GetAppName(), wxOK|wxICON_EXCLAMATION);
            }
        else
            {
            for (size_t i = 0; i < GetReadabilityResultsView().GetWindowCount(); ++i)
                {
                wxWindow* activeWindow = GetReadabilityResultsView().GetWindow(i);
                if (activeWindow)
                    {
                    if (activeWindow->IsKindOf(CLASSINFO(ExplanationListCtrl)) )
                        {
                        ExplanationListCtrl* list = dynamic_cast<ExplanationListCtrl*>(activeWindow);
                        list->SetLabel(
                            wxString::Format(L"%s [%s]", list->GetName(),
                                             wxFileName::StripExtension(doc->GetTitle())));
                        list->Save(
                            folder + wxFileName::GetPathSeparator() + GetReadabilityScoresLabel() +
                            wxFileName::GetPathSeparator() +
                            list->GetLabel() + L".htm", ExplanationListExportOptions::ExportGrid);
                        }
                    else if (activeWindow->IsKindOf(CLASSINFO(Wisteria::Canvas)) )
                        {
                        Wisteria::Canvas* graphWindow = dynamic_cast<Wisteria::Canvas*>(activeWindow);
                        graphWindow->SetLabel(wxString::Format(L"%s [%s]", graphWindow->GetName(),
                            wxFileName::StripExtension(doc->GetTitle())));
                        graphWindow->Save(folder + wxFileName::GetPathSeparator() + GetReadabilityScoresLabel() +
                            wxFileName::GetPathSeparator() +
                            graphWindow->GetLabel() + graphExt, graphOptions);
                        }
                    else if (activeWindow->IsKindOf(CLASSINFO(HtmlTableWindow)) )
                        {
                        HtmlTableWindow* reportWindow = dynamic_cast<HtmlTableWindow*>(activeWindow);
                        reportWindow->SetLabel(
                            wxString::Format(L"%s [%s]", reportWindow->GetName(),
                                wxFileName::StripExtension(doc->GetTitle())));
                        reportWindow->Save(folder + wxFileName::GetPathSeparator() + GetReadabilityScoresLabel() +
                            wxFileName::GetPathSeparator() +
                            reportWindow->GetLabel() + L".htm");
                        }
                    else if (activeWindow->IsKindOf(CLASSINFO(ListCtrlEx)))
                        {
                        ListCtrlEx* list = dynamic_cast<ListCtrlEx*>(activeWindow);
                        list->SetLabel(
                            wxString::Format(L"%s [%s]", list->GetName(),
                                             wxFileName::StripExtension(doc->GetTitle())));
                        list->Save(
                            folder + wxFileName::GetPathSeparator() + GetReadabilityScoresLabel() +
                            wxFileName::GetPathSeparator() +
                            list->GetLabel() + listExt, GridExportOptions());
                        }
                    }
                }
            }
        }
    // the statistics
    if (includeStatistics && GetSummaryView().GetWindowCount())
        {
        if (!wxFileName::Mkdir(folder + wxFileName::GetPathSeparator() + GetSummaryStatisticsLabel(),
                               wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL))
            {
            wxMessageBox(wxString::Format(_(L"Unable to create \"%s\" folder."), GetSummaryStatisticsLabel()),
                wxGetApp().GetAppName(), wxOK|wxICON_EXCLAMATION);
            }
        else
            {
            for (size_t i = 0; i < GetSummaryView().GetWindowCount(); ++i)
                {
                wxWindow* activeWindow = GetSummaryView().GetWindow(i);
                if (activeWindow)
                    {
                    if (activeWindow->IsKindOf(CLASSINFO(HtmlTableWindow)) )
                        {
                        HtmlTableWindow* html = dynamic_cast<HtmlTableWindow*>(activeWindow);
                        html->SetLabel(
                            wxString::Format(L"%s [%s]", html->GetName(),
                                             wxFileName::StripExtension(doc->GetTitle())));
                        html->Save(
                            folder + wxFileName::GetPathSeparator() + GetSummaryStatisticsLabel() +
                            wxFileName::GetPathSeparator() +
                            html->GetLabel() + L".htm");
                        }
                    else if (activeWindow->IsKindOf(CLASSINFO(ListCtrlEx)) && includeLists)
                        {
                        ListCtrlEx* list = dynamic_cast<ListCtrlEx*>(activeWindow);
                        list->SetLabel(
                            wxString::Format(L"%s [%s]", list->GetName(),
                                             wxFileName::StripExtension(doc->GetTitle())));
                        list->Save(
                            folder + wxFileName::GetPathSeparator() + GetSummaryStatisticsLabel() +
                            wxFileName::GetPathSeparator() +
                            list->GetLabel() + listExt, GridExportOptions());
                        }
                    else if (typeid(*activeWindow) == typeid(Wisteria::Canvas))
                        {
                        Wisteria::Canvas* graphWindow = dynamic_cast<Wisteria::Canvas*>(activeWindow);
                        graphWindow->SetLabel(
                            wxString::Format(L"%s [%s]", graphWindow->GetName(),
                                wxFileName::StripExtension(doc->GetTitle())));
                        graphWindow->Save(folder + wxFileName::GetPathSeparator() + GetSummaryStatisticsLabel() +
                            wxFileName::GetPathSeparator() +
                            graphWindow->GetLabel() + graphExt, graphOptions);
                        }
                    }
                }
            }
        }
    if (includeSentencesBreakdown && GetSentencesBreakdownView().GetWindowCount())
        {
        if (!wxFileName::Mkdir(folder + wxFileName::GetPathSeparator() + GetSentencesBreakdownLabel(),
                               wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL))
            {
            wxMessageBox(wxString::Format(_(L"Unable to create \"%s\" folder."), GetSentencesBreakdownLabel()),
                wxGetApp().GetAppName(), wxOK|wxICON_EXCLAMATION);
            }
        else
            {
            for (size_t i = 0; i < GetSentencesBreakdownView().GetWindowCount(); ++i)
                {
                wxWindow* activeWindow = GetSentencesBreakdownView().GetWindow(i);
                if (activeWindow)
                    {
                    if (activeWindow->IsKindOf(CLASSINFO(ListCtrlEx)) && includeLists)
                        {
                        ListCtrlEx* list = dynamic_cast<ListCtrlEx*>(activeWindow);
                        list->SetLabel(
                            wxString::Format(L"%s [%s]", list->GetName(),
                                wxFileName::StripExtension(doc->GetTitle())));
                        list->Save(
                            folder + wxFileName::GetPathSeparator() + GetSentencesBreakdownLabel() +
                            wxFileName::GetPathSeparator() +
                            list->GetLabel() + listExt, GridExportOptions());
                        }
                    else if (typeid(*activeWindow) == typeid(Wisteria::Canvas))
                        {
                        Wisteria::Canvas* graphWindow = dynamic_cast<Wisteria::Canvas*>(activeWindow);
                        graphWindow->SetLabel(
                            wxString::Format(L"%s [%s]", graphWindow->GetName(),
                                wxFileName::StripExtension(doc->GetTitle())));
                        graphWindow->Save(folder + wxFileName::GetPathSeparator() + GetSentencesBreakdownLabel() +
                            wxFileName::GetPathSeparator() +
                            graphWindow->GetLabel() + graphExt, graphOptions);
                        }
                    }
                }
            }
        }
    // the words breakdown section
    if (includeWordsBreakdown && GetWordsBreakdownView().GetWindowCount() )
        {
        if (!wxFileName::Mkdir(folder + wxFileName::GetPathSeparator() + GetWordsBreakdownLabel(),
                               wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL))
            {
            wxMessageBox(wxString::Format(_(L"Unable to create \"%s\" folder."), GetWordsBreakdownLabel()),
                wxGetApp().GetAppName(), wxOK|wxICON_EXCLAMATION);
            }
        else
            {
            for (size_t i = 0; i < GetWordsBreakdownView().GetWindowCount(); ++i)
                {
                wxWindow* activeWindow = GetWordsBreakdownView().GetWindow(i);
                if (activeWindow)
                    {
                    if (activeWindow->IsKindOf(CLASSINFO(ListCtrlEx)) && includeLists)
                        {
                        ListCtrlEx* list = dynamic_cast<ListCtrlEx*>(activeWindow);
                        list->SetLabel(
                            wxString::Format(L"%s [%s]", list->GetName(),
                                wxFileName::StripExtension(doc->GetTitle())));
                        list->Save(
                            folder + wxFileName::GetPathSeparator() + GetWordsBreakdownLabel() +
                            wxFileName::GetPathSeparator() +
                            list->GetLabel() + listExt, GridExportOptions());
                        }
                    else if (activeWindow->IsKindOf(CLASSINFO(Wisteria::Canvas)))
                        {
                        Wisteria::Canvas* graphWindow = dynamic_cast<Wisteria::Canvas*>(activeWindow);
                        graphWindow->SetLabel(
                            wxString::Format(L"%s [%s]", graphWindow->GetName(),
                                wxFileName::StripExtension(doc->GetTitle())));
                        graphWindow->Save(folder + wxFileName::GetPathSeparator() + GetWordsBreakdownLabel() +
                            wxFileName::GetPathSeparator() +
                            graphWindow->GetLabel() + graphExt, graphOptions);
                        }
                    else if (activeWindow->IsKindOf(CLASSINFO(FormattedTextCtrl)) && includeTextReports)
                        {
                        FormattedTextCtrl* text = dynamic_cast<FormattedTextCtrl*>(activeWindow);
                        text->SetTitleName(
                            wxString::Format(L"%s [%s]", text->GetName(),
                                wxFileName::StripExtension(doc->GetTitle())));
                        text->Save(
                            folder + wxFileName::GetPathSeparator() + GetWordsBreakdownLabel() +
                            wxFileName::GetPathSeparator() +
                            text->GetTitleName() + textExt);
                        }
                    }
                }
            }
        }
    // grammar
    if (includeGrammar && GetGrammarView().GetWindowCount() )
        {
        if (!wxFileName::Mkdir(folder + wxFileName::GetPathSeparator() + GetGrammarLabel(),
                               wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL))
            {
            wxMessageBox(wxString::Format(_(L"Unable to create \"%s\" folder."), GetGrammarLabel()),
                wxGetApp().GetAppName(), wxOK|wxICON_EXCLAMATION);
            }
        else
            {
            for (size_t i = 0; i < GetGrammarView().GetWindowCount(); ++i)
                {
                wxWindow* activeWindow = GetGrammarView().GetWindow(i);
                if (activeWindow)
                    {
                    if (activeWindow->IsKindOf(CLASSINFO(ListCtrlEx)) && includeLists)
                        {
                        ListCtrlEx* list = dynamic_cast<ListCtrlEx*>(activeWindow);
                        list->SetLabel(
                            wxString::Format(L"%s [%s]", list->GetName(),
                                wxFileName::StripExtension(doc->GetTitle())));
                        list->Save(
                            folder + wxFileName::GetPathSeparator() + GetGrammarLabel() +
                            wxFileName::GetPathSeparator() +
                            list->GetLabel() + listExt, GridExportOptions());
                        }
                    else if (activeWindow->IsKindOf(CLASSINFO(HtmlTableWindow)) )
                        {
                        HtmlTableWindow* html = dynamic_cast<HtmlTableWindow*>(activeWindow);
                        html->SetLabel(
                            wxString::Format(L"%s [%s]", html->GetName(),
                                wxFileName::StripExtension(doc->GetTitle())));
                        html->Save(
                            folder + wxFileName::GetPathSeparator() + GetGrammarLabel() +
                            wxFileName::GetPathSeparator() +
                            html->GetLabel() + L".htm");
                        }
                    else if (activeWindow->IsKindOf(CLASSINFO(FormattedTextCtrl)) && includeTextReports)
                        {
                        FormattedTextCtrl* text = dynamic_cast<FormattedTextCtrl*>(activeWindow);
                        text->SetTitleName(
                            wxString::Format(L"%s [%s]", text->GetName(),
                                wxFileName::StripExtension(doc->GetTitle())));
                        text->Save(
                            folder + wxFileName::GetPathSeparator() + GetGrammarLabel() +
                            wxFileName::GetPathSeparator() +
                            text->GetTitleName() + textExt);
                        }
                    }
                }
            }
        }
    // Sight Words
    if (includeSightWords && GetDolchSightWordsView().GetWindowCount() )
        {
        if (!wxFileName::Mkdir(folder + wxFileName::GetPathSeparator() + GetDolchLabel(),
                               wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL))
            {
            wxMessageBox(wxString::Format(_(L"Unable to create \"%s\" folder."), GetDolchLabel()),
                wxGetApp().GetAppName(), wxOK|wxICON_EXCLAMATION);
            }
        else
            {
            for (size_t i = 0; i < GetDolchSightWordsView().GetWindowCount(); ++i)
                {
                wxWindow* activeWindow = GetDolchSightWordsView().GetWindow(i);
                if (activeWindow)
                    {
                    if (activeWindow->IsKindOf(CLASSINFO(ListCtrlEx)) && includeLists)
                        {
                        ListCtrlEx* list = dynamic_cast<ListCtrlEx*>(activeWindow);
                        list->SetLabel(
                            wxString::Format(L"%s [%s]", list->GetName(),
                                wxFileName::StripExtension(doc->GetTitle())));
                        list->Save(
                            folder + wxFileName::GetPathSeparator() + GetDolchLabel() +
                            wxFileName::GetPathSeparator() +
                            list->GetLabel() + listExt, GridExportOptions());
                        }
                    else if (activeWindow->IsKindOf(CLASSINFO(HtmlTableWindow)) )
                        {
                        HtmlTableWindow* html = dynamic_cast<HtmlTableWindow*>(activeWindow);
                        html->SetLabel(
                            wxString::Format(L"%s [%s]", html->GetName(),
                                wxFileName::StripExtension(doc->GetTitle())));
                        html->Save(
                            folder + wxFileName::GetPathSeparator() + GetDolchLabel() +
                            wxFileName::GetPathSeparator() +
                            html->GetLabel() + L".htm");
                        }
                    else if (activeWindow->IsKindOf(CLASSINFO(FormattedTextCtrl)) && includeTextReports)
                        {
                        FormattedTextCtrl* text = dynamic_cast<FormattedTextCtrl*>(activeWindow);
                        text->SetTitleName(
                            wxString::Format(L"%s [%s]", text->GetName(),
                                wxFileName::StripExtension(doc->GetTitle())));
                        text->Save(
                            folder + wxFileName::GetPathSeparator() + GetDolchLabel() +
                            wxFileName::GetPathSeparator() +
                            text->GetTitleName() + textExt);
                        }
                    else if (typeid(*activeWindow) == typeid(Wisteria::Canvas))
                        {
                        Wisteria::Canvas* graph = dynamic_cast<Wisteria::Canvas*>(activeWindow);
                        graph->SetLabel(
                            wxString::Format(L"%s [%s]", graph->GetName(),
                                wxFileName::StripExtension(doc->GetTitle())));
                        graph->Save(
                            folder + wxFileName::GetPathSeparator() + GetDolchLabel() +
                            wxFileName::GetPathSeparator() +
                            graph->GetLabel() + graphExt, graphOptions);
                        }
                    }
                }
            }
        }
    return true;
    }

//-------------------------------------------------------
bool ProjectView::ExportAllToHtml(const wxFileName& filePath, wxString graphExt,
                            const bool includeWordsBreakdown,
                            const bool includeSentencesBreakdown,
                            const bool includeTestScores, const bool includeStatistics, const bool includeGrammar,
                            const bool includeSightWords,
                            const bool includeLists,
                            const bool includeTextReports,
                            const Wisteria::UI::ImageExportOptions& graphOptions)
    {
    if (filePath.GetPath().empty())
        { return false; }
    const ProjectDoc* doc = dynamic_cast<const ProjectDoc*>(GetDocument());

    if (!wxFileName::DirExists(filePath.GetPathWithSep() + _DT(L"images")))
        {
        if (!wxFileName::Mkdir(filePath.GetPathWithSep() + _DT(L"images"), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL) )
            { return false; }
        }
    // validate the extension
    if (graphExt.empty())
        { graphExt = L".png"; }
    else if (graphExt[0] != L'.')
        { graphExt.insert(0, L"."); }

    BaseProjectProcessingLock processingLock(dynamic_cast<ProjectDoc*>(GetDocument()));

    wxBusyCursor bc;
    wxBusyInfo bi(wxBusyInfoFlags().Text(_(L"Exporting project...")));

    lily_of_the_valley::html_encode_text htmlEncode;
    wxString outputText, textWindowStyleSection;
    wxString headSection = L"<head>" +
        wxString::Format(
            L"\n    <meta name='generator' content='%s %s' />"
             "\n    <title>%s</title>"
             "\n    <meta http-equiv='content-type' content='text/html; charset=utf-8' />"
             "\n    <link rel='stylesheet' href='style.css'>"
             "\n</head>",
            wxGetApp().GetAppDisplayName(), wxGetApp().GetAppVersion(),
            doc->GetTitle());

    wchar_t textWindowStyleCounter = L'a';
    size_t sectionCounter = 0;
    size_t figureCounter = 0;
    size_t tableCounter = 0;

    const wxString pageBreak = L"<div style='page-break-before:always'></div><br />\n";

    const auto formatImageOutput =
        [&outputText, &sectionCounter, &figureCounter, pageBreak, doc, htmlEncode, graphExt, graphOptions, filePath]
        (Wisteria::Canvas* canvas, const bool includeLeadingPageBreak)
        {
        if (!canvas)
            { return; }
        canvas->SetLabel(wxString::Format(L"%s [%s]", canvas->GetName(), wxFileName::StripExtension(doc->GetTitle())));
        canvas->Save(filePath.GetPathWithSep() + _DT(L"images") + wxFileName::GetPathSeparator() +
            canvas->GetLabel() + graphExt, graphOptions);

        outputText += wxString::Format(
            L"%s\n<div class='minipage figure'>\n<img src='images\\%s' />\n<div class='caption'>%s</div>\n</div>\n",
            (includeLeadingPageBreak ? pageBreak : wxString{}),
            canvas->GetLabel()+graphExt,
            wxString::Format(_(L"Figure %zu.%zu: %s"), sectionCounter, figureCounter++,
                htmlEncode({ canvas->GetName().wc_str() }, true).c_str()));
        };

    const auto formatList =
        [&outputText, &htmlEncode, &sectionCounter, &tableCounter, pageBreak]
        (ListCtrlEx* list, const bool includeLeadingPageBreak)
        {
        if (!list)
            { return; }

        BaseProjectDoc::UpdateListOptions(list);
        wxString buffer;
        list->FormatToHtml(buffer, true, ListCtrlEx::ExportRowSelection::ExportAll,
            0, -1, 0, -1, true, false,
            wxString::Format(_(L"Table %zu.%zu: %s"), sectionCounter, tableCounter++,
                htmlEncode({ list->GetName().wc_str() }, true).c_str()));
        std::wstring htmlText{ buffer.ToStdWstring() };
        lily_of_the_valley::html_format::strip_hyperlinks(htmlText);

        outputText += (includeLeadingPageBreak ? pageBreak : wxString{}) +
            html_extract_text::get_body(htmlText);
        };

    const auto formatTextWindow =
        [&outputText, &htmlEncode, &textWindowStyleCounter, &textWindowStyleSection, pageBreak]
        (FormattedTextCtrl* textWindow, const bool includeLeadingPageBreak)
        {
        if (!textWindow)
            { return; }
        std::wstring htmlText =
            textWindow->GetUnthemedFormattedTextHtml(wxString(textWindowStyleCounter++)).ToStdWstring();
        textWindowStyleSection += L"\n" +
            wxString(html_extract_text::get_style_section(htmlText));
        htmlText = html_extract_text::get_body(htmlText);

        outputText += wxString::Format(L"\n%s<div class='caption'>%s</div>\n<div class='text-report-body'>%s</div>\n",
            (includeLeadingPageBreak ? pageBreak : wxString{}),
            htmlEncode({ textWindow->GetLabel().wc_str() }, true).c_str(),
            htmlText);
        };

    const auto formatHTMLReport =
        [&outputText, &htmlEncode, pageBreak]
        (HtmlTableWindow* html, const bool includeLeadingPageBreak)
        {
        if (!html)
            { return; }
        std::wstring htmlText = (html->GetParser()->GetSource())->ToStdWstring();
        lily_of_the_valley::html_format::strip_hyperlinks(htmlText);
        htmlText = html_extract_text::get_body(htmlText);
        outputText += wxString::Format(L"\n%s<div class='caption'>%s</div>\n%s\n",
            (includeLeadingPageBreak ? pageBreak : wxString{}),
            htmlEncode({ html->GetName().wc_str() }, true).c_str(),
            htmlText);
        };

    bool hasSections{ false };

    // scores section
    if (includeTestScores && GetReadabilityResultsView().GetWindowCount())
        {
        // the first output in this section will not have a leading pagebreak, but the rest will
        bool includeLeadingPageBreak{ false };
        // indicates that a section has already been written out after the TOC so that we
        // know if we need to insert a page break in front of the next section
        hasSections = true;
        // update/reset counters for sections, tables, and figures
        ++sectionCounter;
        figureCounter = tableCounter = 1;
        outputText += L"\n\n<div class='report-section'><a name='scores'></a>" +
            htmlEncode({ GetReadabilityScoresLabel().wc_str() }, true) + L"</div>\n";
        for (size_t i = 0; i < GetReadabilityResultsView().GetWindowCount(); ++i)
            {
            wxWindow* activeWindow = GetReadabilityResultsView().GetWindow(i);
            if (activeWindow)
                {
                if (activeWindow->IsKindOf(CLASSINFO(ExplanationListCtrl)) )
                    {
                    formatList(dynamic_cast<ExplanationListCtrl*>(activeWindow)->GetResultsListCtrl(),
                        includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                else if (activeWindow->IsKindOf(CLASSINFO(Wisteria::Canvas)) )
                    {
                    formatImageOutput(dynamic_cast<Wisteria::Canvas*>(activeWindow), includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                else if (activeWindow->IsKindOf(CLASSINFO(HtmlTableWindow)) )
                    {
                    formatHTMLReport(dynamic_cast<HtmlTableWindow*>(activeWindow), includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                else if (activeWindow->IsKindOf(CLASSINFO(ListCtrlEx)))
                    {
                    formatList(dynamic_cast<ListCtrlEx*>(activeWindow), includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                }
            }
        }
    // the statistics
    if (includeStatistics && GetSummaryView().GetWindowCount())
        {
        bool includeLeadingPageBreak{ false };
        ++sectionCounter;
        figureCounter = tableCounter = 1;
        outputText += wxString::Format(L"\n\n%s<div class='report-section'><a name='stats'></a>%s</div>\n",
            (hasSections ? pageBreak : wxString{}),
            htmlEncode({ GetSummaryStatisticsLabel().wc_str() }, true).c_str());
        hasSections = true;
        for (size_t i = 0; i < GetSummaryView().GetWindowCount(); ++i)
            {
            wxWindow* activeWindow = GetSummaryView().GetWindow(i);
            if (activeWindow)
                {
                if (activeWindow->IsKindOf(CLASSINFO(HtmlTableWindow)) )
                    {
                    formatHTMLReport(dynamic_cast<HtmlTableWindow*>(activeWindow), includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                else if (activeWindow->IsKindOf(CLASSINFO(ListCtrlEx)) && includeLists)
                    {
                    formatList(dynamic_cast<ListCtrlEx*>(activeWindow), includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                else if (activeWindow->IsKindOf(CLASSINFO(Wisteria::Canvas)) )
                    {
                    formatImageOutput(dynamic_cast<Wisteria::Canvas*>(activeWindow), includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                }
            }
        }
    // words breakdown section
    if (includeWordsBreakdown && GetWordsBreakdownView().GetWindowCount() )
        {
        bool includeLeadingPageBreak{ false };
        ++sectionCounter;
        figureCounter = tableCounter = 1;
        outputText += wxString::Format(L"\n\n%s<div class='report-section'><a name='wordsbreakdown'></a>%s</div>\n",
            (hasSections ? pageBreak : wxString{}),
            htmlEncode({GetWordsBreakdownLabel().wc_str() }, true).c_str());
        hasSections = true;
        for (size_t i = 0; i < GetWordsBreakdownView().GetWindowCount(); ++i)
            {
            wxWindow* activeWindow = GetWordsBreakdownView().GetWindow(i);
            if (activeWindow)
                {
                if (activeWindow->IsKindOf(CLASSINFO(ListCtrlEx)) && includeLists)
                    {
                    formatList(dynamic_cast<ListCtrlEx*>(activeWindow), includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                else if (typeid(*activeWindow) == typeid(Wisteria::Canvas))
                    {
                    formatImageOutput(dynamic_cast<Wisteria::Canvas*>(activeWindow), includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                else if (activeWindow->IsKindOf(CLASSINFO(FormattedTextCtrl)) && includeTextReports)
                    {
                    formatTextWindow(dynamic_cast<FormattedTextCtrl*>(activeWindow), includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                }
            }
        }
    // sentence section
    if (includeSentencesBreakdown && GetSentencesBreakdownView().GetWindowCount())
        {
        bool includeLeadingPageBreak{ false };
        ++sectionCounter;
        figureCounter = tableCounter = 1;
        outputText += wxString::Format(
            L"\n\n%s<div class='report-section'><a name='sentencesbreakdown'></a>%s</div>\n",
            (hasSections ? pageBreak : wxString{}),
            htmlEncode({ GetSentencesBreakdownLabel().wc_str() }, true).c_str());
        hasSections = true;
        for (size_t i = 0; i < GetSentencesBreakdownView().GetWindowCount(); ++i)
            {
            wxWindow* activeWindow = GetSentencesBreakdownView().GetWindow(i);
            if (activeWindow)
                {
                if (activeWindow->IsKindOf(CLASSINFO(ListCtrlEx)) && includeLists)
                    {
                    formatList(dynamic_cast<ListCtrlEx*>(activeWindow), includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                else if (activeWindow->IsKindOf(CLASSINFO(Wisteria::Canvas)) )
                    {
                    formatImageOutput(dynamic_cast<Wisteria::Canvas*>(activeWindow), includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                }
            }
        }
    // grammar section
    if (includeGrammar && (includeLists || includeTextReports) && GetGrammarView().GetWindowCount() )
        {
        bool includeLeadingPageBreak{ false };
        ++sectionCounter;
        figureCounter = tableCounter = 1;
        outputText += wxString::Format(L"\n\n%s<div class='report-section'><a name='grammar'></a>%s</div>\n",
            (hasSections ? pageBreak : wxString{}),
            htmlEncode({ GetGrammarLabel().wc_str() }, true).c_str());
        hasSections = true;
        for (size_t i = 0; i < GetGrammarView().GetWindowCount(); ++i)
            {
            wxWindow* activeWindow = GetGrammarView().GetWindow(i);
            if (activeWindow)
                {
                if (activeWindow->IsKindOf(CLASSINFO(ListCtrlEx)) && includeLists)
                    {
                    formatList(dynamic_cast<ListCtrlEx*>(activeWindow), includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                else if (activeWindow->IsKindOf(CLASSINFO(FormattedTextCtrl)) && includeTextReports)
                    {
                    formatTextWindow(dynamic_cast<FormattedTextCtrl*>(activeWindow), includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                }
            }
        }
    // Sight Words
    if (includeSightWords && GetDolchSightWordsView().GetWindowCount() )
        {
        bool includeLeadingPageBreak{ false };
        ++sectionCounter;
        figureCounter = tableCounter = 1;
        outputText += wxString::Format(L"\n\n%s<div class='report-section'><a name='dolch'></a>%s</div>\n",
            (hasSections ? pageBreak : wxString{}),
            htmlEncode({ GetDolchLabel().wc_str() }, true).c_str());
        hasSections = true;
        for (size_t i = 0; i < GetDolchSightWordsView().GetWindowCount(); ++i)
            {
            wxWindow* activeWindow = GetDolchSightWordsView().GetWindow(i);
            if (activeWindow)
                {
                if (activeWindow->IsKindOf(CLASSINFO(ListCtrlEx)) && includeLists)
                    {
                    formatList(dynamic_cast<ListCtrlEx*>(activeWindow), includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                else if (activeWindow->IsKindOf(CLASSINFO(HtmlTableWindow)) )
                    {
                    formatHTMLReport(dynamic_cast<HtmlTableWindow*>(activeWindow), includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                else if (activeWindow->IsKindOf(CLASSINFO(FormattedTextCtrl)) && includeTextReports)
                    {
                    formatTextWindow(dynamic_cast<FormattedTextCtrl*>(activeWindow), includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                else if (typeid(*activeWindow) == typeid(Wisteria::Canvas))
                    {
                    formatImageOutput(dynamic_cast<Wisteria::Canvas*>(activeWindow), includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                }
            }
        }

    wxString TOC, infoTable;
    infoTable = wxString::Format(
        L"<div style='display:flex;'>\n"
        "<div class='report-header'>\n"
        "<div class='report-header-inner-cell report-header-first-column'>%s</div>\n"
        "<div class='report-header-inner-cell'>%s</div>\n"
        "<div class='report-header-inner-cell report-header-first-column'>%s</div>\n"
        "<div class='report-header-inner-cell'>%s</div>\n"
        "<div class='report-header-inner-cell report-header-first-column'>%s</div>\n"
        "<div class='report-header-inner-cell'>%s</div>\n"
        "<div class='report-header-first-column'>%s</div>\n"
        "<div>%s</div>\n"
        "</div>\n"
        "</div>",
        _(L"Project Title"), doc->GetTitle(), _(L"Status"), doc->GetStatus(),
        _(L"Reviewer"), doc->GetReviewer(), _(L"Date"), wxDateTime().Now().FormatDate()
        );

    if (includeTestScores && GetReadabilityResultsView().GetWindowCount())
        { TOC += L"<a href='#scores'>" + GetReadabilityScoresLabel() + L"</a><br />\n"; }
    if (includeStatistics && GetSummaryView().GetWindowCount())
        { TOC += L"<a href='#stats'>" + GetSummaryStatisticsLabel() + L"</a><br />\n"; }
    if (includeWordsBreakdown && GetWordsBreakdownView().GetWindowCount())
        { TOC += L"<a href='#wordsbreakdown'>" + GetWordsBreakdownLabel() + L"</a><br />\n"; }
    if (includeSentencesBreakdown && GetSentencesBreakdownView().GetWindowCount())
        { TOC += L"<a href='#sentencesbreakdown'>" + GetSentencesBreakdownLabel() + L"</a><br />\n"; }
    // grammar section only has text and list windows, so don't include that if not including those types of windows
    if (includeGrammar && (includeLists || includeTextReports) && GetGrammarView().GetWindowCount())
        { TOC += L"<a href='#grammar'>" + GetGrammarLabel() + L"</a><br />\n"; }
    if (includeSightWords && GetDolchSightWordsView().GetWindowCount())
        { TOC += L"<a href='#dolch'>" + GetDolchLabel() + L"</a><br />\n"; }
    outputText.insert(0, L"<!DOCTYPE html>\n<html>\n" + headSection +
         L"\n<body>\n" +
        infoTable + L"\n<div class='toc-section no-print'>" + TOC + L"</div>");
    outputText += L"\n</body>\n</html>";

    // copy over the CSS file
    const wxString cssTemplatePath =
        wxGetApp().FindResourceDirectory(_DT(L"ReportThemes")) +
        wxFileName::GetPathSeparator() + L"Default.css";
    const wxString cssPath = filePath.GetPathWithSep() + L"style.css";
    if (wxFileName::FileExists(cssTemplatePath))
        {
        if (!wxCopyFile(cssTemplatePath, cssPath, true))
            {
            wxLogWarning(L"Failed to copy CSS file '%s' to '%s'.", cssTemplatePath, cssPath);
            }
        else
            {
            // add text window styling to it
            wxFile cssFile(cssPath, wxFile::OpenMode::write_append);
            if (cssFile.IsOpened())
                { cssFile.Write(textWindowStyleSection); }
            }
        }

    wxFileName(filePath.GetFullPath()).SetPermissions(wxS_DEFAULT);
    wxFile file(filePath.GetFullPath(), wxFile::write);
    return file.Write(outputText);
    }
