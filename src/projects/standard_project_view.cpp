#include "standard_project_view.h"
#include "../Wisteria-Dataviz/src/graphs/heatmap.h"
#include "../Wisteria-Dataviz/src/graphs/piechart.h"
#include "../Wisteria-Dataviz/src/graphs/wordcloud.h"
#include "../Wisteria-Dataviz/src/import/html_encode.h"
#include "../Wisteria-Dataviz/src/ui/dialogs/gridexportdlg.h"
#include "../Wisteria-Dataviz/src/ui/dialogs/radioboxdlg.h"
#include "../app/readability_app.h"
#include "../results-format/project_report_format.h"
#include "../ui/dialogs/export_all_dlg.h"
#include "../ui/dialogs/filtered_text_export_options_dlg.h"
#include "../ui/dialogs/filtered_text_preview_dlg.h"
#include "../ui/dialogs/tools_options_dlg.h"
#include "standard_project_doc.h"

using namespace lily_of_the_valley;
using namespace Wisteria;
using namespace Wisteria::Graphs;
using namespace Wisteria::UI;

wxDECLARE_APP(ReadabilityApp);

wxIMPLEMENT_DYNAMIC_CLASS(ProjectView, BaseProjectView)

    //------------------------------------------------------
    ProjectView::ProjectView()
    : m_statsListData(new ListCtrlExDataProvider)
    {
    Bind(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this,
         SIDEBAR_CUSTOM_TESTS_START_ID, SIDEBAR_CUSTOM_TESTS_START_ID + 1000);

    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnRealTimeUpdate, this,
         XRCID("ID_REALTIME_UPDATE"));

    Bind(wxEVT_SIDEBAR_CLICK, &ProjectView::OnItemSelected, this, BaseProjectView::LEFT_PANE);

    Bind(wxEVT_WISTERIA_CANVAS_DCLICK, &ProjectView::OnEditGraphOptions, this);

    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnTestDelete, this, XRCID("ID_REMOVE_TEST"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnLongFormat, this, XRCID("ID_LONG_FORMAT"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnTextWindowColorsChange, this,
         XRCID("ID_TEXT_WINDOW_COLORS"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnTextWindowFontChange, this,
         XRCID("ID_TEXT_WINDOW_FONT"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnLaunchSourceFile, this,
         XRCID("ID_LAUNCH_SOURCE_FILE"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnSummation, this, XRCID("ID_SUMMATION"));

    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnRibbonButtonCommand, this, wxID_SELECTALL);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnRibbonButtonCommand, this, wxID_NEW);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnRibbonButtonCommand, this, wxID_OPEN);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnRibbonButtonCommand, this, wxID_SAVE);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnRibbonButtonCommand, this, wxID_COPY);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnRibbonButtonCommand, this, wxID_PREVIEW);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnRibbonButtonCommand, this, wxID_PRINT);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnRibbonButtonCommand, this, wxID_ZOOM_IN);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnRibbonButtonCommand, this, wxID_ZOOM_OUT);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnRibbonButtonCommand, this, wxID_ZOOM_FIT);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnRibbonButtonCommand, this,
         XRCID("ID_EXCLUDE_SELECTED"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnRibbonButtonCommand, this,
         XRCID("ID_SORT_DESCENDING"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnRibbonButtonCommand, this,
         XRCID("ID_SORT_ASCENDING"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnRibbonButtonCommand, this,
         XRCID("ID_LIST_SORT"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnRibbonButtonCommand, this,
         XRCID("ID_VIEW_ITEM"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnRibbonButtonCommand, this,
         XRCID("ID_COPY_FIRST_COLUMN"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnRibbonButtonCommand, this,
         XRCID("ID_COPY_WITH_COLUMN_HEADERS"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnRibbonButtonCommand, this,
         XRCID("ID_COPY_ALL"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnRibbonButtonCommand, this,
         XRCID("ID_SAVE_ITEM"));

    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this, HARD_WORDS_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this, LONG_WORDS_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this, DC_WORDS_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this,
         HARRIS_JACOBSON_WORDS_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this, SPACHE_WORDS_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this, ALL_WORDS_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this,
         ALL_WORDS_CONDENSED_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this, PROPER_NOUNS_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this,
         OVERUSED_WORDS_BY_SENTENCE_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this, DOLCH_WORDS_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this,
         NON_DOLCH_WORDS_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this,
         LONG_SENTENCES_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this,
         SENTENCES_CONJUNCTION_START_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this,
         SENTENCES_LOWERCASE_START_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this, DUPLICATES_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this, INCORRECT_ARTICLE_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this, PASSIVE_VOICE_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this,
         MISSPELLED_WORD_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this, CLICHES_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this, WORDY_PHRASES_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this,
         REDUNDANT_PHRASE_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this,
         WORDING_ERRORS_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnTestListDblClick, this,
         READABILITY_SCORES_PAGE_ID);

    Bind(wxEVT_FIND, &ProjectView::OnFind, this);
    Bind(wxEVT_FIND_NEXT, &ProjectView::OnFind, this);
    Bind(wxEVT_FIND_CLOSE, &ProjectView::OnFind, this);

    Bind(wxEVT_HTML_LINK_CLICKED, &ProjectView::OnHyperlinkClicked, this);

    Bind(wxEVT_MENU, &ProjectView::OnMenuCommand, this, XRCID("ID_PRINT"));
    Bind(wxEVT_MENU, &ProjectView::OnGradeScale, this, XRCID("ID_K12_US"));
    Bind(wxEVT_MENU, &ProjectView::OnGradeScale, this, XRCID("ID_K12_NEWFOUNDLAND"));
    Bind(wxEVT_MENU, &ProjectView::OnGradeScale, this, XRCID("ID_K12_BC"));
    Bind(wxEVT_MENU, &ProjectView::OnGradeScale, this, XRCID("ID_K12_NEW_BRUNSWICK"));
    Bind(wxEVT_MENU, &ProjectView::OnGradeScale, this, XRCID("ID_K12_NOVA_SCOTIA"));
    Bind(wxEVT_MENU, &ProjectView::OnGradeScale, this, XRCID("ID_K12_ONTARIO"));
    Bind(wxEVT_MENU, &ProjectView::OnGradeScale, this, XRCID("ID_K12_SASKATCHEWAN"));
    Bind(wxEVT_MENU, &ProjectView::OnGradeScale, this, XRCID("ID_K12_PE"));
    Bind(wxEVT_MENU, &ProjectView::OnGradeScale, this, XRCID("ID_K12_MANITOBA"));
    Bind(wxEVT_MENU, &ProjectView::OnGradeScale, this, XRCID("ID_K12_NT"));
    Bind(wxEVT_MENU, &ProjectView::OnGradeScale, this, XRCID("ID_K12_ALBERTA"));
    Bind(wxEVT_MENU, &ProjectView::OnGradeScale, this, XRCID("ID_K12_NUNAVUT"));
    Bind(wxEVT_MENU, &ProjectView::OnGradeScale, this, XRCID("ID_QUEBEC"));
    Bind(wxEVT_MENU, &ProjectView::OnGradeScale, this, XRCID("ID_ENGLAND"));

    Bind(wxEVT_MENU, &ProjectView::OnAddToDictionary, this, XRCID("ID_ADD_ITEM_TO_DICTIONARY"));
    Bind(wxEVT_MENU, &ProjectView::OnTestDeleteMenu, this, XRCID("ID_REMOVE_TEST"));
    Bind(wxEVT_MENU, &ProjectView::OnExportFilteredDocument, this,
         XRCID("ID_EXPORT_FILTERED_DOCUMENT"));
    Bind(wxEVT_MENU, &ProjectView::OnExportAll, this, XRCID("ID_EXPORT_ALL"));
    // not actually a test (it doesn't have an ID), but we'll add it in this function
    Bind(wxEVT_MENU, &ProjectView::OnAddTest, this, XRCID("ID_DOLCH"));

    Bind(wxEVT_MENU, &ProjectView::OnMenuCommand, this, wxID_SELECTALL);
    Bind(wxEVT_MENU, &ProjectView::OnMenuCommand, this, wxID_COPY);
    Bind(wxEVT_MENU, &ProjectView::OnMenuCommand, this, wxID_PREVIEW);
    Bind(wxEVT_MENU, &ProjectView::OnMenuCommand, this, wxID_PRINT);
    Bind(wxEVT_MENU, &ProjectView::OnMenuCommand, this, wxID_ZOOM_IN);
    Bind(wxEVT_MENU, &ProjectView::OnMenuCommand, this, wxID_ZOOM_OUT);
    Bind(wxEVT_MENU, &ProjectView::OnMenuCommand, this, wxID_ZOOM_FIT);
    Bind(wxEVT_MENU, &ProjectView::OnMenuCommand, this, XRCID("ID_SAVE_ITEM"));
    Bind(wxEVT_MENU, &ProjectView::OnMenuCommand, this, XRCID("ID_COPY_ALL"));
    Bind(wxEVT_MENU, &ProjectView::OnMenuCommand, this, XRCID("ID_COPY_WITH_COLUMN_HEADERS"));
    Bind(wxEVT_MENU, &ProjectView::OnMenuCommand, this, XRCID("ID_COPY_FIRST_COLUMN"));
    Bind(wxEVT_MENU, &ProjectView::OnMenuCommand, this, XRCID("ID_VIEW_ITEM"));
    Bind(wxEVT_MENU, &ProjectView::OnMenuCommand, this, XRCID("ID_LIST_SORT"));
    Bind(wxEVT_MENU, &ProjectView::OnMenuCommand, this, XRCID("ID_SORT_ASCENDING"));
    Bind(wxEVT_MENU, &ProjectView::OnMenuCommand, this, XRCID("ID_SORT_DESCENDING"));
    Bind(wxEVT_MENU, &ProjectView::OnMenuCommand, this, XRCID("ID_EXCLUDE_SELECTED"));

    Bind(
        wxEVT_RIBBONBUTTONBAR_CLICKED,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            ProjectDoc* projDoc = dynamic_cast<ProjectDoc*>(GetDocument());
            if (projDoc != nullptr)
                {
                projDoc->Save();
                }
        },
        XRCID("ID_SAVE_PROJECT"));
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            ProjectDoc* projDoc = dynamic_cast<ProjectDoc*>(GetDocument());
            if (projDoc != nullptr)
                {
                projDoc->Save();
                }
        },
        // don't use wxID_SAVE for a hybrid ribbon button because it becomes disabled
        // when the document isn't dirty and then you can't access the export menu
        XRCID("ID_SAVE_PROJECT"));
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            ProjectDoc* projDoc = dynamic_cast<ProjectDoc*>(GetDocument());
            if (projDoc != nullptr)
                {
                projDoc->SaveAs();
                }
        },
        XRCID("ID_SAVE_PROJECT_AS"));
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
        viewDlg.AddValue(
            _(L"Total Possible Misspellings"),
            wxNumberFormatter::ToString(doc->GetMisspelledWordData()->GetColumnSum(1), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep));
        }
    else if (GetActiveProjectWindow()->GetId() == PASSIVE_VOICE_PAGE_ID)
        {
        viewDlg.AddValue(
            _(L"Total Passive Phrases"),
            wxNumberFormatter::ToString(doc->GetPassiveVoiceData()->GetColumnSum(1), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep));
        }
    else if (GetActiveProjectWindow()->GetId() == HARD_WORDS_LIST_PAGE_ID)
        {
        viewDlg.AddValue(
            _(L"Total 3+ Syllable Words"),
            wxNumberFormatter::ToString(doc->Get3SyllablePlusData()->GetColumnSum(2), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep));
        }
    else if (GetActiveProjectWindow()->GetId() == LONG_WORDS_LIST_PAGE_ID)
        {
        viewDlg.AddValue(
            _(L"Total 6+ Character Words"),
            wxNumberFormatter::ToString(doc->Get6CharacterPlusData()->GetColumnSum(2), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep));
        }
    else if (GetActiveProjectWindow()->GetId() == DC_WORDS_LIST_PAGE_ID)
        {
        viewDlg.AddValue(
            _(L"Total Dale-Chall Unfamiliar Words"),
            wxNumberFormatter::ToString(doc->GetDaleChallHardWordData()->GetColumnSum(1), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep));
        }
    else if (GetActiveProjectWindow()->GetId() == SPACHE_WORDS_LIST_PAGE_ID)
        {
        viewDlg.AddValue(
            _(L"Total Spache Unfamiliar Words"),
            wxNumberFormatter::ToString(doc->GetSpacheHardWordData()->GetColumnSum(1), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep));
        }
    else if (GetActiveProjectWindow()->GetId() == HARRIS_JACOBSON_WORDS_LIST_PAGE_ID)
        {
        viewDlg.AddValue(_(L"Total Harris-Jacobson Unfamiliar Words"),
                         wxNumberFormatter::ToString(
                             doc->GetHarrisJacobsonHardWordDataData()->GetColumnSum(1), 0,
                             wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                 wxNumberFormatter::Style::Style_WithThousandsSep));
        }
    else if (GetActiveProjectWindow()->GetId() == ALL_WORDS_LIST_PAGE_ID)
        {
        viewDlg.AddValue(
            _(L"Total Words"),
            wxNumberFormatter::ToString(doc->GetAllWordsBaseData()->GetColumnSum(1), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep));
        }
    else if (GetActiveProjectWindow()->GetId() == ALL_WORDS_CONDENSED_LIST_PAGE_ID)
        {
        viewDlg.AddValue(
            _(L"Total Words"),
            wxNumberFormatter::ToString(doc->GetKeyWordsBaseData()->GetColumnSum(1), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep));
        }
    else if (GetActiveProjectWindow()->GetId() == PROPER_NOUNS_LIST_PAGE_ID)
        {
        viewDlg.AddValue(
            _(L"Total Proper Nouns"),
            wxNumberFormatter::ToString(doc->GetProperNounsData()->GetColumnSum(1), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep));
        }
    else if (GetActiveProjectWindow()->GetId() == DOLCH_WORDS_LIST_PAGE_ID)
        {
        viewDlg.AddValue(
            _(L"Total Dolch Words"),
            wxNumberFormatter::ToString(doc->GetDolchWordData()->GetColumnSum(1), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep));
        }
    else if (GetActiveProjectWindow()->GetId() == NON_DOLCH_WORDS_LIST_PAGE_ID)
        {
        viewDlg.AddValue(
            _(L"Total Non-Dolch Words"),
            wxNumberFormatter::ToString(doc->GetNonDolchWordData()->GetColumnSum(1), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep));
        }
    else
        {
        wxString customTestName;
        for (size_t i = 0; i < BaseProject::m_custom_word_tests.size(); ++i)
            {
            if (BaseProject::m_custom_word_tests[i].get_interface_id() ==
                GetActiveProjectWindow()->GetId())
                {
                customTestName = BaseProject::m_custom_word_tests[i].get_name().c_str();
                break;
                }
            }
        if (doc->HasCustomTest(customTestName))
            {
            viewDlg.AddValue(
                wxString::Format(_(L"Total %s Unfamiliar Words"), customTestName),
                wxNumberFormatter::ToString(
                    doc->GetCustomTest(customTestName)->GetListViewData()->GetColumnSum(1), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes |
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
        {
        return;
        }
    wxArrayString choices, descriptions;
    choices.Add(_(L"Single report"));
    descriptions.Add(
        _(L"Saves the results into a single <span style='font-weight: bold;'>HTML</span> report."));
    choices.Add(_(L"Separate files"));
    descriptions.Add(_(L"Saves each result window to a separate file."));
    Wisteria::UI::RadioBoxDlg exportTypesDlg(GetDocFrame(), _(L"Select How to Export"), wxString{},
                                             _(L"Export methods:"), _(L"Export All"), choices,
                                             descriptions);
    if (exportTypesDlg.ShowModal() != wxID_OK)
        {
        return;
        }

    ExportAllDlg dlg(GetDocFrame(), doc, (exportTypesDlg.GetSelection() == 0));
    if (m_activeWindow && m_activeWindow->GetClientSize().IsFullySpecified())
        {
        dlg.GetImageExportOptions().m_imageSize = m_activeWindow->GetClientSize();
        }
    dlg.SetHelpTopic(wxGetApp().GetMainFrame()->GetHelpDirectory(), L"export-all-options.html");
    if (dlg.ShowModal() != wxID_OK || dlg.GetFolderPath().empty())
        {
        return;
        }

    if (exportTypesDlg.GetSelection() == 0)
        {
        ExportAllToHtml(dlg.GetFilePath(), dlg.GetExportGraphExt(), dlg.IsExportingHardWordLists(),
                        dlg.IsExportingSentencesBreakdown(), dlg.IsExportingTestResults(),
                        dlg.IsExportingStatistics(), dlg.IsExportingGrammar(),
                        dlg.IsExportingSightWords(), dlg.IsExportingLists(),
                        dlg.IsExportingTextReports(), dlg.GetImageExportOptions());
        }
    else
        {
        ExportAll(dlg.GetFolderPath(), dlg.GetExportListExt(), dlg.GetExportTextViewExt(),
                  dlg.GetExportGraphExt(), dlg.IsExportingHardWordLists(),
                  dlg.IsExportingSentencesBreakdown(), dlg.IsExportingTestResults(),
                  dlg.IsExportingStatistics(), dlg.IsExportingGrammar(),
                  dlg.IsExportingSightWords(), dlg.IsExportingLists(), dlg.IsExportingTextReports(),
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
        wxMessageBox(_(L"Filtered document exporting is only available in the "
                       "Professional Edition of Readability Studio."),
                     _(L"Feature Not Licensed"), wxOK | wxICON_INFORMATION);
        return;
        }
    const BaseProjectDoc* doc = dynamic_cast<const BaseProjectDoc*>(GetDocument());
    wxFileDialog fdialog(GetDocFrame(), _(L"Export Filtered Document"), wxString{}, doc->GetTitle(),
                         _(L"Text Files (*.txt)|*.txt"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (fdialog.ShowModal() != wxID_OK)
        {
        return;
        }

    FilteredTextExportOptionsDlg optDlg(GetDocFrame());
    optDlg.SetHelpTopic(wxGetApp().GetMainFrame()->GetHelpDirectory(), L"filtering-export.html");
    if (optDlg.ShowModal() != wxID_OK)
        {
        return;
        }

    wxString validDocText;
    doc->FormatFilteredText(validDocText, optDlg.IsReplacingCharacters(),
                            optDlg.IsRemovingEllipses(), optDlg.IsRemovingBullets(),
                            optDlg.IsRemovingFilePaths(), optDlg.IsStrippingAbbreviations(),
                            optDlg.IsNarrowingFullWidthCharacters());

    FilteredTextPreviewDlg dlg(GetDocFrame(), doc->GetInvalidSentenceMethod(),
                               doc->IsIgnoringTrailingCopyrightNoticeParagraphs(),
                               doc->IsIgnoringTrailingCitations(), optDlg.IsReplacingCharacters(),
                               optDlg.IsRemovingEllipses(), optDlg.IsRemovingBullets(),
                               optDlg.IsRemovingFilePaths(), optDlg.IsStrippingAbbreviations());
    dlg.SetHelpTopic(wxGetApp().GetMainFrame()->GetHelpDirectory(), L"how-text-is-excluded.html");
    dlg.SetFilteredValue(validDocText);
    if (dlg.ShowModal() == wxID_OK)
        {
        wxFileName(fdialog.GetPath()).SetPermissions(wxS_DEFAULT);
        wxFile filteredFile(fdialog.GetPath(), wxFile::write);
        if (!filteredFile.Write(validDocText))
            {
            wxMessageBox(_(L"Unable to write to output file."), _(L"Error"),
                         wxOK | wxICON_EXCLAMATION);
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
        {
        dynamic_cast<ProjectDoc*>(GetDocument())->RefreshGraphs();
        }
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
            {
            theProject->RefreshStatisticsReports();
            }
        }
    else if (event.GetLinkInfo().GetHref() == L"#FryGraph")
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(FRY_PAGE_ID));
        }
    else if (event.GetLinkInfo().GetHref() == L"#flesch-chart")
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(FLESCH_CHART_PAGE_ID));
        }
    else if (event.GetLinkInfo().GetHref() == L"#DB2")
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(DB2_PAGE_ID));
        }
    else if (event.GetLinkInfo().GetHref() == L"#lix-gauge")
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(LIX_GAUGE_PAGE_ID));
        }
    else if (event.GetLinkInfo().GetHref() == L"#german-lix-gauge")
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(LIX_GAUGE_GERMAN_PAGE_ID));
        }
    else if (event.GetLinkInfo().GetHref() == L"#crawford-graph")
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
    else if (event.GetLinkInfo().GetHref() == L"#schwartz")
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
        GetSideBar()->SelectSubItem(
            GetSideBar()->FindSubItem(SENTENCES_CONJUNCTION_START_LIST_PAGE_ID));
        }
    else if (event.GetLinkInfo().GetHref() == L"#SentenceStartingWithLowercase")
        {
        GetSideBar()->SelectSubItem(
            GetSideBar()->FindSubItem(SENTENCES_LOWERCASE_START_LIST_PAGE_ID));
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
        GetSideBar()->SelectSubItem(
            GetSideBar()->FindSubItem(OVERUSED_WORDS_BY_SENTENCE_LIST_PAGE_ID));
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
        {
        wxGetApp().GetMainFrame()->DisplayHelp(L"gunning-fog-test.html");
        }
    else if (event.GetLinkInfo().GetHref().length() &&
             event.GetLinkInfo().GetHref().GetChar(0) == L'#')
        {
        event.Skip();
        }
    else
        {
        wxGetApp().GetMainFrame()->DisplayHelp(event.GetLinkInfo().GetHref());
        }
    }

//------------------------------------------------------
void ProjectView::OnTestListDblClick([[maybe_unused]] wxListEvent& event)
    {
    const wxString selectedTest =
        GetReadabilityScoresList()->GetResultsListCtrl()->GetSelectedText();
    CustomReadabilityTestCollection::iterator testIter =
        std::find(BaseProject::m_custom_word_tests.begin(), BaseProject::m_custom_word_tests.end(),
                  selectedTest);
    const std::pair<std::vector<readability::readability_test>::const_iterator, bool> testPos =
        BaseProject::GetDefaultReadabilityTestsTemplate().find_test(selectedTest);

    if (testIter != BaseProject::m_custom_word_tests.end())
        {
        wxGetApp().EditCustomTest(*testIter);
        }
    else if (testPos.second)
        {
        wxGetApp().GetMainFrame()->DisplayHelp(
            wxString::Format(L"%s.html", testPos.first->get_id().c_str()));
        }
    else if (selectedTest == _(L"Dolch Sight Words"))
        {
        wxGetApp().GetMainFrame()->DisplayHelp(L"reviewing-dolch.html.html");
        }
    }

/// Double clicking on an item in the hard word list will jump to the respective text window and
/// find the word that you clicked on.
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
            {
            searchText = dynamic_cast<const ListCtrlEx*>(foundWindow)->GetSelectedText();
            }
        textId = HARD_WORDS_TEXT_PAGE_ID;
        break;
    case OVERUSED_WORDS_BY_SENTENCE_LIST_PAGE_ID:
        foundWindow = GetGrammarView().FindWindowById(event.GetId());
        if (foundWindow)
            {
            const auto selectedItem =
                dynamic_cast<const ListCtrlEx*>(foundWindow)->GetFirstSelected();
            searchText =
                (selectedItem == wxNOT_FOUND) ?
                    wxString{} :
                    dynamic_cast<const ListCtrlEx*>(foundWindow)->GetItemTextEx(selectedItem, 1);
            }
        textId = HARD_WORDS_TEXT_PAGE_ID;
        break;
    case LONG_SENTENCES_LIST_PAGE_ID:
        foundWindow = GetSentencesBreakdownView().FindWindowById(event.GetId());
        if (foundWindow)
            {
            searchText = dynamic_cast<const ListCtrlEx*>(foundWindow)->GetSelectedText();
            }
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
            {
            searchText = dynamic_cast<const ListCtrlEx*>(foundWindow)->GetSelectedText();
            }
        textId = LONG_SENTENCES_AND_WORDINESS_TEXT_PAGE_ID;
        break;
    case DOLCH_WORDS_LIST_PAGE_ID:
        foundWindow = GetDolchSightWordsView().FindWindowById(event.GetId());
        if (foundWindow)
            {
            searchText = dynamic_cast<const ListCtrlEx*>(foundWindow)->GetSelectedText();
            }
        textId = DOLCH_WORDS_TEXT_PAGE_ID;
        break;
    case NON_DOLCH_WORDS_LIST_PAGE_ID:
        foundWindow = GetDolchSightWordsView().FindWindowById(event.GetId());
        if (foundWindow)
            {
            searchText = dynamic_cast<const ListCtrlEx*>(foundWindow)->GetSelectedText();
            }
        textId = NON_DOLCH_WORDS_TEXT_PAGE_ID;
        break;
    default:
        // custom test
        foundWindow = GetWordsBreakdownView().FindWindowById(event.GetId());
        if (foundWindow)
            {
            searchText = dynamic_cast<const ListCtrlEx*>(foundWindow)->GetSelectedText();
            }
        textId = event.GetId();
        }

    searchText.Trim(true);

    // if the embedded editor is open, then select the text in there
    if (m_embeddedTextEditor != nullptr && m_embeddedTextEditor->IsShown())
        {
        m_embeddedTextEditor->SelectString(searchText, replacementText);
        }
    else
        {
        // Find the first occurrence of the selected word.
        // First, look in the word breakdown section for the respective test window,
        // then the grammar section and finally the Dolch section.
        wxWindow* theWindow =
            GetWordsBreakdownView().FindWindowById(textId, CLASSINFO(FormattedTextCtrl));
        if (!theWindow)
            {
            theWindow = GetGrammarView().FindWindowById(textId, CLASSINFO(FormattedTextCtrl));
            }
        if (!theWindow)
            {
            theWindow =
                GetDolchSightWordsView().FindWindowById(textId, CLASSINFO(FormattedTextCtrl));
            }
        if (theWindow && theWindow->IsKindOf(CLASSINFO(FormattedTextCtrl)))
            {
            FormattedTextCtrl* textWindow = dynamic_cast<FormattedTextCtrl*>(theWindow);
            textWindow->SetSelection(0, 0);
            // If looking for an entire sentence, then don't use whole-word search.
            // Whole-word search behaves differently between platforms and won't work for
            // sentences under GTK+ as expected (because of the terminal period).
            if (event.GetId() == LONG_SENTENCES_LIST_PAGE_ID ||
                event.GetId() == SENTENCES_LOWERCASE_START_LIST_PAGE_ID ||
                event.GetId() == SENTENCES_CONJUNCTION_START_LIST_PAGE_ID ||
                event.GetId() == OVERUSED_WORDS_BY_SENTENCE_LIST_PAGE_ID)
                {
                textWindow->FindText(searchText, true, false, false);
                }
            else
                {
                textWindow->FindText(searchText, true, true, false);
                }
            // Search by label for custom word-list tests (the list and report have the same ID);
            // otherwise, search by ID.
            GetSideBar()->SelectSubItem((event.GetId() == textId) ?
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
            // the embedded text. Also, don't bother asking if the file is a file inside of an
            // archive.
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
                        wxLaunchDefaultApplication(doc->GetOriginalDocumentFilePath());
                        return;
                        }
                    }
                // or if they said "yes" before, then use the found path
                else if (warningIter != WarningManager::GetWarnings().end() &&
                         warningIter->GetPreviousResponse() == wxID_YES)
                    {
                    doc->SetModifiedFlag();
                    doc->SetDocumentStorageMethod(TextStorage::NoEmbedText);
                    wxLaunchDefaultApplication(doc->GetOriginalDocumentFilePath());
                    return;
                    }
                }
            if (m_embeddedTextEditor == nullptr)
                {
                m_embeddedTextEditor = new EditTextDlg(
                    GetDocFrame(), doc, doc->GetDocumentText(), wxID_ANY,
                    _(L"Edit Embedded Document"),
                    doc->GetAppendedDocumentText().length() ?
                        _(L"Note: The appended template document is not included here.\n"
                          "Only the embedded text is editable from this dialog.") :
                        wxString{});
                }

            m_embeddedTextEditor->Show();
            }
        else
            {
            if (resolvePath.IsArchivedFile())
                {
                wxMessageBox(_(L"Files inside of archives files cannot be edited."), wxString{},
                             wxOK | wxICON_INFORMATION);
                return;
                }
            else if (resolvePath.IsExcelCell())
                {
                const size_t excelTag =
                    resolvePath.GetResolvedPath().MakeLower().find(_DT(L".xlsx#"));
                wxFileName fn(resolvePath.GetResolvedPath().substr(0, excelTag + 5));
                wxLaunchDefaultApplication(fn.GetFullPath());
                }
            else
                {
                wxLaunchDefaultApplication(resolvePath.GetResolvedPath());
                }
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
        for (auto* window : GetReadabilityResultsView().GetWindows())
            {
            const bool isGraph = typeid(*window) == typeid(Wisteria::Canvas);

            GetSideBar()->InsertSubItemById(
                SIDEBAR_READABILITY_SCORES_SECTION_ID, window->GetName(), window->GetId(),
                window->GetId() == READABILITY_SCORES_SUMMARY_REPORT_PAGE_ID ? 17 :
                window->GetId() == READABILITY_SCORES_PAGE_ID                ? 23 :
                window->GetId() == READABILITY_GOALS_PAGE_ID                 ? 28 :
                (isGraph && typeid(*dynamic_cast<Wisteria::Canvas*>(window)->GetFixedObject(
                                0, 0)) == typeid(FleschChart)) ?
                                                               18 :
                (isGraph && typeid(*dynamic_cast<Wisteria::Canvas*>(window)->GetFixedObject(
                                0, 0)) == typeid(FraseGraph)) ?
                                                               19 :
                (isGraph && typeid(*dynamic_cast<Wisteria::Canvas*>(window)->GetFixedObject(
                                0, 0)) == typeid(FryGraph)) ?
                                                               20 :
                (isGraph && typeid(*dynamic_cast<Wisteria::Canvas*>(window)->GetFixedObject(
                                0, 0)) == typeid(RaygorGraph)) ?
                                                               21 :
                (isGraph && typeid(*dynamic_cast<Wisteria::Canvas*>(window)->GetFixedObject(
                                0, 0)) == typeid(CrawfordGraph)) ?
                                                               22 :
                (isGraph && typeid(*dynamic_cast<Wisteria::Canvas*>(window)->GetFixedObject(
                                0, 0)) == typeid(SchwartzGraph)) ?
                                                               25 :
                (isGraph && typeid(*dynamic_cast<Wisteria::Canvas*>(window)->GetFixedObject(
                                0, 0)) == typeid(LixGauge)) ?
                                                               26 :
                (isGraph && typeid(*dynamic_cast<Wisteria::Canvas*>(window)->GetFixedObject(
                                0, 0)) == typeid(LixGaugeGerman)) ?
                                                               26 :
                (isGraph && typeid(*dynamic_cast<Wisteria::Canvas*>(window)->GetFixedObject(
                                0, 0)) == typeid(DanielsonBryan2Plot)) ?
                                                               27 :
                                                               9);
            }
        }

    // Summary statistics window
    if (GetSummaryView().GetWindowCount() > 0)
        {
        GetSideBar()->InsertItem(GetSideBar()->GetFolderCount(), GetSummaryStatisticsLabel(),
                                 SIDEBAR_STATS_SUMMARY_SECTION_ID, 2);
        for (const auto* window : GetSummaryView().GetWindows())
            {
            GetSideBar()->InsertSubItemById(SIDEBAR_STATS_SUMMARY_SECTION_ID, window->GetName(),
                                            window->GetId(),
                                            window->IsKindOf(CLASSINFO(HtmlTableWindow)) ? 17 :
                                            window->IsKindOf(CLASSINFO(ListCtrlEx))      ? 15 :
                                                                                           9);
            }
        }

    // Words breakdown
    if (GetWordsBreakdownView().GetWindowCount() > 0)
        {
        GetSideBar()->InsertItem(GetSideBar()->GetFolderCount(), GetWordsBreakdownLabel(),
                                 SIDEBAR_WORDS_BREAKDOWN_SECTION_ID, 13);
        for (auto* window : GetWordsBreakdownView().GetWindows())
            {
            const bool isGraph = typeid(*window) == typeid(Wisteria::Canvas);

            GetSideBar()->InsertSubItemById(
                SIDEBAR_WORDS_BREAKDOWN_SECTION_ID, window->GetName(), window->GetId(),
                window->IsKindOf(CLASSINFO(FormattedTextCtrl)) ? 0 :
                window->IsKindOf(CLASSINFO(ListCtrlEx))        ? 15 :
                (isGraph && typeid(*dynamic_cast<Wisteria::Canvas*>(window)->GetFixedObject(
                                0, 0)) == typeid(Wisteria::Graphs::Histogram)) ?
                                                          6 :
                (isGraph && typeid(*dynamic_cast<Wisteria::Canvas*>(window)->GetFixedObject(
                                0, 0)) == typeid(Wisteria::Graphs::BarChart)) ?
                                                          16 :
                (isGraph && typeid(*dynamic_cast<Wisteria::Canvas*>(window)->GetFixedObject(
                                0, 0)) == typeid(Wisteria::Graphs::WordCloud)) ?
                                                          29 :
                (isGraph && typeid(*dynamic_cast<Wisteria::Canvas*>(window)->GetFixedObject(
                                0, 0)) == typeid(Wisteria::Graphs::PieChart)) ?
                                                          30 :
                                                          9);
            }
        }

    // Sentences breakdown
    if (GetSentencesBreakdownView().GetWindowCount() > 0)
        {
        GetSideBar()->InsertItem(GetSideBar()->GetFolderCount(), GetSentencesBreakdownLabel(),
                                 SIDEBAR_SENTENCES_BREAKDOWN_SECTION_ID, 14);
        for (auto* window : GetSentencesBreakdownView().GetWindows())
            {
            const bool isGraph = typeid(*window) == typeid(Wisteria::Canvas);

            GetSideBar()->InsertSubItemById(
                SIDEBAR_SENTENCES_BREAKDOWN_SECTION_ID, window->GetName(), window->GetId(),
                window->IsKindOf(CLASSINFO(ListCtrlEx)) ? 15 :
                (isGraph && typeid(*dynamic_cast<Wisteria::Canvas*>(window)->GetFixedObject(
                                0, 0)) == typeid(Wisteria::Graphs::BoxPlot)) ?
                                                          7 :
                (isGraph && typeid(*dynamic_cast<Wisteria::Canvas*>(window)->GetFixedObject(
                                0, 0)) == typeid(Wisteria::Graphs::Histogram)) ?
                                                          6 :
                (isGraph && typeid(*dynamic_cast<Wisteria::Canvas*>(window)->GetFixedObject(
                                0, 0)) == typeid(Wisteria::Graphs::HeatMap)) ?
                                                          24 :
                                                          9);
            }
        }

    // grammar windows
    if (GetGrammarView().GetWindowCount() > 0)
        {
        GetSideBar()->InsertItem(GetSideBar()->GetFolderCount(), GetGrammarLabel(),
                                 SIDEBAR_GRAMMAR_SECTION_ID, 4);
        for (const auto* window : GetGrammarView().GetWindows())
            {
            GetSideBar()->InsertSubItemById(SIDEBAR_GRAMMAR_SECTION_ID, window->GetName(),
                                            window->GetId(),
                                            window->IsKindOf(CLASSINFO(FormattedTextCtrl)) ? 0 :
                                            window->IsKindOf(CLASSINFO(ListCtrlEx))        ? 15 :
                                                                                             9);
            }
        }

    // sight words
    if (GetDolchSightWordsView().GetWindowCount() > 0)
        {
        GetSideBar()->InsertItem(GetSideBar()->GetFolderCount(), GetDolchLabel(),
                                 SIDEBAR_DOLCH_SECTION_ID, 5);
        for (auto* window : GetDolchSightWordsView().GetWindows())
            {
            const bool isGraph = typeid(*window) == typeid(Wisteria::Canvas);

            GetSideBar()->InsertSubItemById(
                SIDEBAR_DOLCH_SECTION_ID, window->GetName(), window->GetId(),
                window->IsKindOf(CLASSINFO(FormattedTextCtrl)) ? 0 :
                window->IsKindOf(CLASSINFO(HtmlTableWindow))   ? 17 :
                (isGraph && typeid(*dynamic_cast<Wisteria::Canvas*>(window)->GetFixedObject(
                                0, 0)) == typeid(Wisteria::Graphs::BarChart)) ?
                                                               16 :
                window->IsKindOf(CLASSINFO(ListCtrlEx)) ? 15 :
                                                          9);
            }
        }

    GetSideBar()->ResetState();
    }

//-------------------------------------------------------
void ProjectView::OnAddTest(wxCommandEvent& event)
    {
    ProjectDoc* doc = dynamic_cast<ProjectDoc*>(GetDocument());
    if (!doc->IsSafeToUpdate())
        {
        return;
        }

    if (event.GetId() == XRCID("ID_DOLCH"))
        {
        doc->AddDolchSightWords();
        }
    else
        {
        doc->GetReadabilityTests().include_test(
            doc->GetReadabilityTests().get_test_id(event.GetId()).c_str(), true);
        // refresh
        doc->RefreshRequired(ProjectRefresh::Minimal);
        doc->RefreshProject();
        const long testToSelect = GetReadabilityScoresList()->GetResultsListCtrl()->FindEx(
            doc->GetReadabilityTests().get_test_long_name(event.GetId()).c_str());
        if (testToSelect != wxNOT_FOUND)
            {
            GetReadabilityScoresList()->GetResultsListCtrl()->Select(testToSelect);
            }
        GetSideBar()->SelectSubItem(
            GetSideBar()->FindSubItem(BaseProjectView::READABILITY_SCORES_PAGE_ID));
        // show any warning messages from the test being ran
        doc->ShowQueuedMessages();
        if (WarningManager::HasWarning(_DT(L"click-test-to-view")))
            {
            ShowInfoMessage(*WarningManager::GetWarning(_DT(L"click-test-to-view")));
            }
        }
    }

//---------------------------------------------------
void ProjectView::OnGradeScale(wxCommandEvent& event)
    {
    ProjectDoc* doc = dynamic_cast<ProjectDoc*>(GetDocument());
    if (!doc->IsSafeToUpdate())
        {
        return;
        }
    wxWindowUpdateLocker noUpdates(doc->GetDocumentWindow());
    BaseProjectProcessingLock processingLock(doc);

    for (size_t i = 0; i < GetDocFrame()->m_gradeScaleMenu.GetMenuItemCount(); ++i)
        {
        GetDocFrame()->m_gradeScaleMenu.FindItemByPosition(i)->Check(false);
        }

    readability::grade_scale gs = doc->GetReadabilityMessageCatalog().GetGradeScale();
    if (event.GetId() == XRCID("ID_K12_US"))
        {
        gs = readability::grade_scale::k12_plus_united_states;
        }
    else if (event.GetId() == XRCID("ID_K12_NEWFOUNDLAND"))
        {
        gs = readability::grade_scale::k12_plus_newfoundland_and_labrador;
        }
    else if (event.GetId() == XRCID("ID_K12_BC"))
        {
        gs = readability::grade_scale::k12_plus_british_columbia;
        }
    else if (event.GetId() == XRCID("ID_K12_NEW_BRUNSWICK"))
        {
        gs = readability::grade_scale::k12_plus_newbrunswick;
        }
    else if (event.GetId() == XRCID("ID_K12_NOVA_SCOTIA"))
        {
        gs = readability::grade_scale::k12_plus_nova_scotia;
        }
    else if (event.GetId() == XRCID("ID_K12_ONTARIO"))
        {
        gs = readability::grade_scale::k12_plus_ontario;
        }
    else if (event.GetId() == XRCID("ID_K12_SASKATCHEWAN"))
        {
        gs = readability::grade_scale::k12_plus_saskatchewan;
        }
    else if (event.GetId() == XRCID("ID_K12_PE"))
        {
        gs = readability::grade_scale::k12_plus_prince_edward_island;
        }
    else if (event.GetId() == XRCID("ID_K12_MANITOBA"))
        {
        gs = readability::grade_scale::k12_plus_manitoba;
        }
    else if (event.GetId() == XRCID("ID_K12_NT"))
        {
        gs = readability::grade_scale::k12_plus_northwest_territories;
        }
    else if (event.GetId() == XRCID("ID_K12_ALBERTA"))
        {
        gs = readability::grade_scale::k12_plus_alberta;
        }
    else if (event.GetId() == XRCID("ID_K12_NUNAVUT"))
        {
        gs = readability::grade_scale::k12_plus_nunavut;
        }
    else if (event.GetId() == XRCID("ID_QUEBEC"))
        {
        gs = readability::grade_scale::quebec;
        }
    else if (event.GetId() == XRCID("ID_ENGLAND"))
        {
        gs = readability::grade_scale::key_stages_england_wales;
        }
    GetDocFrame()->m_gradeScaleMenu.Check(event.GetId(), true);
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
                {
                break;
                }
            newWords.Add(listView->GetItemText(item));
            }
        if (newWords.GetCount() == 0)
            {
            wxMessageBox(_(L"Please select a word (or words) to add to your dictionary."),
                         _(L"Add to Dictionary"), wxOK | wxICON_INFORMATION);
            return;
            }
        wxGetApp().AddWordsToDictionaries(
            newWords, dynamic_cast<BaseProjectDoc*>(GetDocument())->GetProjectLanguage());
        wxList docs = wxGetApp().GetDocManager()->GetDocuments();
        for (size_t i = 0; i < docs.GetCount(); ++i)
            {
            BaseProjectDoc* doc = dynamic_cast<BaseProjectDoc*>(docs.Item(i)->GetData());
            doc->RemoveMisspellings(newWords);
            }
        }
    else
        {
        wxMessageBox(_(L"There are no misspellings in this document."), _(L"Add to Dictionary"),
                     wxOK | wxICON_INFORMATION);
        return;
        }
    }

//---------------------------------------------------
void ProjectView::OnRibbonButtonCommand(wxRibbonButtonBarEvent& event)
    {
    wxCommandEvent cmd(wxEVT_MENU, event.GetId());
    // the document frame needs to handle document events
    if (event.GetId() == wxID_OPEN || event.GetId() == wxID_NEW || event.GetId() == wxID_SAVE)
        {
        GetDocFrame()->ProcessWindowEvent(cmd);
        }
    else
        {
        OnMenuCommand(cmd);
        }
    }

// Handles all menu events for the document and propagates to the active window
//---------------------------------------------------
void ProjectView::OnMenuCommand(wxCommandEvent& event)
    {
    ProjectDoc* doc = dynamic_cast<ProjectDoc*>(GetDocument());
    if (!doc->IsSafeToUpdate())
        {
        return;
        }
    // show a message about zooming in and how the background image won't scale larger than its size
    if (event.GetId() == wxID_ZOOM_IN)
        {
        if (WarningManager::HasWarning(_DT(L"bkimage-zoomin-noupscale")))
            {
            ShowInfoMessage(*WarningManager::GetWarning(_DT(L"bkimage-zoomin-noupscale")));
            }
        }

    // propagate standard save command to active subwindow if "export window" option selected
    if (event.GetId() == XRCID("ID_SAVE_ITEM"))
        {
        event.SetId(wxID_SAVE);
        }
    else if (event.GetId() == XRCID("ID_PRINT"))
        {
        event.SetId(wxID_PRINT);
        }
    else if (event.GetId() == XRCID("ID_SORT_ASCENDING") && GetActiveProjectWindow() &&
             typeid(*GetActiveProjectWindow()) == typeid(Wisteria::Canvas) &&
             (typeid(*dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow())
                          ->GetFixedObject(0, 0)) == typeid(Wisteria::Graphs::BarChart) ||
              typeid(*dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow())
                          ->GetFixedObject(0, 0)) == typeid(Wisteria::Graphs::Histogram)))
        {
        Wisteria::Canvas* barChart = dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow());
        std::dynamic_pointer_cast<Wisteria::Graphs::BarChart>(barChart->GetFixedObject(0, 0))
            ->SortBars(BarChart::BarSortComparison::SortByBarLength,
                       Wisteria::SortDirection::SortAscending);
        barChart->Refresh();
        barChart->Update();
        return;
        }
    else if (event.GetId() == XRCID("ID_SORT_DESCENDING") && GetActiveProjectWindow() &&
             typeid(*GetActiveProjectWindow()) == typeid(Wisteria::Canvas) &&
             (typeid(*dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow())
                          ->GetFixedObject(0, 0)) == typeid(Wisteria::Graphs::BarChart) ||
              typeid(*dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow())
                          ->GetFixedObject(0, 0)) == typeid(Wisteria::Graphs::Histogram)))
        {
        Wisteria::Canvas* barChart = dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow());
        std::dynamic_pointer_cast<Wisteria::Graphs::BarChart>(barChart->GetFixedObject(0, 0))
            ->SortBars(BarChart::BarSortComparison::SortByBarLength,
                       Wisteria::SortDirection::SortDescending);
        barChart->Refresh();
        barChart->Update();
        return;
        }

    if (GetActiveProjectWindow() && GetActiveProjectWindow()->IsKindOf(CLASSINFO(ListCtrlEx)))
        {
        ListCtrlEx* list = dynamic_cast<ListCtrlEx*>(GetActiveProjectWindow());
        /* just in case this is a print or preview command, update the window's headers
           and footer to whatever the global options currently are*/
        BaseProjectDoc::UpdateListOptions(list);
        // in case we are exporting the window, set its label to include the name of the document,
        // and then reset it
        list->SetLabel(wxString::Format(L"%s [%s]", list->GetName(),
                                        wxFileName::StripExtension(doc->GetTitle())));
        if (event.GetId() == XRCID("ID_EXCLUDE_SELECTED"))
            {
            if (list->GetSelectedItemCount() == 0)
                {
                wxMessageBox(_(L"Please select an item to exclude."), _(L"Error"),
                             wxOK | wxICON_WARNING);
                return;
                }
            if (doc->GetInvalidSentenceMethod() == InvalidSentence::IncludeAsFullSentences)
                {
                if (wxMessageBox(_(L"This project is not currently excluding text. "
                                   "Do you wish to change this?"),
                                 _(L"Text Exclusion Method"), wxYES_NO | wxICON_QUESTION) == wxYES)
                    {
                    doc->SetInvalidSentenceMethod(InvalidSentence::ExcludeFromAnalysis);
                    }
                else
                    {
                    wxMessageBox(_(L"Text exclusion not enabled. Items will not be excluded."),
                                 _(L"Error"), wxOK | wxICON_WARNING);
                    return;
                    }
                }
            if (doc->GetExcludedPhrasesPath().empty())
                {
                wxFileDialog dialog(GetActiveProjectWindow(),
                                    _(L"Specify Where to Save Word Exclusion List"), wxString{},
                                    wxString{}, _(L"Text files (*.txt)|*.txt"),
                                    wxFD_SAVE | wxFD_PREVIEW);
                if (dialog.ShowModal() != wxID_OK)
                    {
                    return;
                    }

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
                        wxRichMessageDialog msg(wxGetApp().GetMainFrame(),
                                                warningIter->GetMessage(), warningIter->GetTitle(),
                                                warningIter->GetFlags());
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
                    {
                    break;
                    }
                selectedStrings += list->GetItemTextEx(item, 0) + L"\n";
                }

            wxString buffer;
            wxString filePath = doc->GetExcludedPhrasesPath();
            if (!Wisteria::TextStream::ReadFile(filePath, buffer))
                {
                wxMessageBox(_(L"Error loading excluded word list file."), _(L"Error"),
                             wxOK | wxICON_EXCLAMATION);
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
                        {
                        expStr.append(exp.c_str()).append(L";");
                        }
                    if (expStr.ends_with(L";"))
                        {
                        expStr.RemoveLast(1);
                        }
                    outputStr += expStr;
                    outputStr += L'\t';

                    expStr.clear();
                    for (const auto& exp : phrase.first.get_trailing_exceptions())
                        {
                        expStr.append(exp.c_str()).append(L";");
                        }
                    if (expStr.ends_with(L";"))
                        {
                        expStr.RemoveLast(1);
                        }
                    outputStr += expStr;
                    }
                outputStr += L"\r\n";
                }

            outputStr.Trim(true);
            outputStr.Trim(false);
            wxFileName(filePath).SetPermissions(wxS_DEFAULT);
            wxFile outputFile(filePath, wxFile::write);
            if (!outputFile.IsOpened())
                {
                wxMessageBox(wxString::Format(
                                 _(L"Unable to save \"%s\".\nVerify that you have write access to "
                                   "this file or that it is not in use."),
                                 filePath),
                             _(L"Error"), wxOK | wxICON_ERROR);
                }
            else
                {
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
    else if (GetActiveProjectWindow() &&
             GetActiveProjectWindow()->IsKindOf(CLASSINFO(HtmlTableWindow)))
        {
        HtmlTableWindow* html = dynamic_cast<HtmlTableWindow*>(GetActiveProjectWindow());
        BaseProjectDoc::UpdatePrinterHeaderAndFooters(html);
        html->SetLabel(wxString::Format(L"%s [%s]", html->GetName(),
                                        wxFileName::StripExtension(doc->GetTitle())));
        ParentEventBlocker blocker(html);
        html->ProcessWindowEvent(event);
        }
    else if (GetActiveProjectWindow() &&
             GetActiveProjectWindow()->IsKindOf(CLASSINFO(FormattedTextCtrl)))
        {
        FormattedTextCtrl* text = dynamic_cast<FormattedTextCtrl*>(GetActiveProjectWindow());
        doc->UpdateTextWindowOptions(text);
        text->SetTitleName(wxString::Format(L"%s [%s]", text->GetName(),
                                            wxFileName::StripExtension(doc->GetTitle())));
        ParentEventBlocker blocker(text);
        text->ProcessWindowEvent(event);
        }
    else if (GetActiveProjectWindow() &&
             GetActiveProjectWindow()->IsKindOf(CLASSINFO(ExplanationListCtrl)))
        {
        ExplanationListCtrl* elist = dynamic_cast<ExplanationListCtrl*>(GetActiveProjectWindow());
        BaseProjectDoc::UpdateExplanationListOptions(elist);
        elist->SetLabel(wxString::Format(L"%s [%s]", elist->GetName(),
                                         wxFileName::StripExtension(doc->GetTitle())));
        ParentEventBlocker blocker(elist);
        elist->ProcessWindowEvent(event);
        }
    else if (GetActiveProjectWindow() &&
             typeid(*GetActiveProjectWindow()) == typeid(Wisteria::Canvas))
        {
        Wisteria::Canvas* graph = dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow());
        doc->UpdateGraphOptions(graph);
        graph->SetLabel(wxString::Format(L"%s [%s]", graph->GetName(),
                                         wxFileName::StripExtension(doc->GetTitle())));
        ParentEventBlocker blocker(graph);
        graph->ProcessWindowEvent(event);
        }
    }

//---------------------------------------------------
void ProjectView::OnFind(wxFindDialogEvent& event)
    {
    // if they were just hitting Cancel then close
    if (event.GetEventType() == wxEVT_COMMAND_FIND_CLOSE)
        {
        return;
        }

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
    wxWindow* projectButtonBarWindow =
        GetRibbon()->FindWindow(MainFrame::ID_PROJECT_RIBBON_BUTTON_BAR);
    if (projDoc && projectButtonBarWindow &&
        projectButtonBarWindow->IsKindOf(CLASSINFO(wxRibbonButtonBar)))
        {
        auto projBar = dynamic_cast<wxRibbonButtonBar*>(projectButtonBarWindow);
        assert(projBar);
        if (projBar)
            {
            projBar->ToggleButton(XRCID("ID_REALTIME_UPDATE"), projDoc->IsRealTimeUpdating());
            projBar->EnableButton(XRCID("ID_REALTIME_UPDATE"),
                                  projDoc->GetDocumentStorageMethod() ==
                                      TextStorage::LoadFromExternalDocument);
            }
        }
    }

//-------------------------------------------------------
bool ProjectView::OnCreate(wxDocument* doc, long flags)
    {
    if (!BaseProjectView::OnCreate(doc, flags))
        {
        return false;
        }

    // Results view
    ExplanationListCtrl* readabilityScoresView = new ExplanationListCtrl(
        GetSplitter(), READABILITY_SCORES_PAGE_ID, wxDefaultPosition, wxDefaultSize, _(L"Scores"));
    readabilityScoresView->Hide();
    readabilityScoresView->GetDataProvider()->SetNumberFormatter(
        dynamic_cast<BaseProjectDoc*>(doc)->GetReadabilityMessageCatalogPtr());
    readabilityScoresView->GetResultsListCtrl()->SetVirtualDataSize(0, 5);
    readabilityScoresView->GetResultsListCtrl()->InsertColumn(0, _(L"Test"), wxLIST_FORMAT_LEFT,
                                                              wxLIST_AUTOSIZE_USEHEADER);
    readabilityScoresView->GetResultsListCtrl()->InsertColumn(
        1, _(L"Grade Level"), wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE_USEHEADER);
    readabilityScoresView->GetResultsListCtrl()->InsertColumn(
        2, _(L"Reader Age"), wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE_USEHEADER);
    readabilityScoresView->GetResultsListCtrl()->InsertColumn(
        3, _(L"Scale Value"), wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE_USEHEADER);
    readabilityScoresView->GetResultsListCtrl()->InsertColumn(
        4, _(L"Predicted Cloze Score"), wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE_USEHEADER);
    readabilityScoresView->GetExplanationView()->SetPage(
        wxString(L"<html><body>") + _(L"No readability test results currently available.") +
        wxString(L"</body></html>"));
    readabilityScoresView->GetResultsListCtrl()->AssignContextMenu(
        wxXmlResource::Get()->LoadMenu(L"IDM_READABILITY_SCORE_LIST"));
    readabilityScoresView->SetPrinterSettings(wxGetApp().GetPrintData());
    readabilityScoresView->SetLeftPrinterHeader(wxGetApp().GetAppOptions().GetLeftPrinterHeader());
    readabilityScoresView->SetCenterPrinterHeader(
        wxGetApp().GetAppOptions().GetCenterPrinterHeader());
    readabilityScoresView->SetRightPrinterHeader(
        wxGetApp().GetAppOptions().GetRightPrinterHeader());
    readabilityScoresView->SetLeftPrinterFooter(wxGetApp().GetAppOptions().GetLeftPrinterFooter());
    readabilityScoresView->SetCenterPrinterFooter(
        wxGetApp().GetAppOptions().GetCenterPrinterFooter());
    readabilityScoresView->SetRightPrinterFooter(
        wxGetApp().GetAppOptions().GetRightPrinterFooter());
    readabilityScoresView->SetResources(wxGetApp().GetMainFrame()->GetHelpDirectory(),
                                        L"column-sorting.html");
    GetReadabilityResultsView().AddWindow(readabilityScoresView);
    GetSplitter()->SplitVertically(GetSideBar(), readabilityScoresView,
                                   GetSideBar()->GetMinWidth());

#ifdef __WXOSX__
    // just load the menubar right now, we will set it in Present after the document has
    // successfully loaded
    m_menuBar = wxXmlResource::Get()->LoadMenuBar(L"ID_DOCMENUBAR");
#endif

    // connect the test events
    for (auto rTest =
             dynamic_cast<const BaseProjectDoc*>(doc)->GetReadabilityTests().get_tests().begin();
         rTest != dynamic_cast<const BaseProjectDoc*>(doc)->GetReadabilityTests().get_tests().end();
         ++rTest)
        {
        Connect(rTest->get_test().get_interface_id(), wxEVT_MENU,
                wxCommandEventHandler(ProjectView::OnAddTest));
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

    const wxString selectedItem =
        GetReadabilityScoresList()->GetResultsListCtrl()->GetSelectedText();
    // remove stats rows if already in here because we have to recalculate everything
    long statIconLocation =
        GetReadabilityScoresList()->GetResultsListCtrl()->FindEx(GetAverageLabel());
    if (statIconLocation != wxNOT_FOUND)
        {
        GetReadabilityScoresList()->GetResultsListCtrl()->DeleteItem(statIconLocation);
        }
    statIconLocation = GetReadabilityScoresList()->GetResultsListCtrl()->FindEx(GetMedianLabel());
    if (statIconLocation != wxNOT_FOUND)
        {
        GetReadabilityScoresList()->GetResultsListCtrl()->DeleteItem(statIconLocation);
        }
    statIconLocation = GetReadabilityScoresList()->GetResultsListCtrl()->FindEx(GetModeLabel());
    if (statIconLocation != wxNOT_FOUND)
        {
        GetReadabilityScoresList()->GetResultsListCtrl()->DeleteItem(statIconLocation);
        }
    statIconLocation = GetReadabilityScoresList()->GetResultsListCtrl()->FindEx(GetStdDevLabel());
    if (statIconLocation != wxNOT_FOUND)
        {
        GetReadabilityScoresList()->GetResultsListCtrl()->DeleteItem(statIconLocation);
        }
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
                {
                grades.push_back(value);
                }
            if (ReadabilityMessages::GetScoreValue(
                    GetReadabilityScoresList()->GetResultsListCtrl()->GetItemTextEx(i, 2), value))
                {
                ages.push_back(value);
                }
            if (ReadabilityMessages::GetScoreValue(
                    GetReadabilityScoresList()->GetResultsListCtrl()->GetItemTextEx(i, 4), value))
                {
                clozeScores.push_back(value);
                }
            }

        wxString gradeAverage(_(L"N/A")), ageAverage(_(L"N/A")), clozeAverage(_(L"N/A")),
            gradeMedian(_(L"N/A")), ageMedian(_(L"N/A")), clozeMedian(_(L"N/A")),
            gradeMode(_(L"N/A")), ageMode(_(L"N/A")), clozeMode(_(L"N/A"));
        // get the average grade
        if (grades.size() > 0)
            {
            gradeAverage = wxNumberFormatter::ToString(
                statistics::mean(grades), 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
            gradeMedian = wxNumberFormatter::ToString(
                statistics::median(grades), 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
            std::set<double> modes = statistics::mode(grades, floor_value<double>{});
            gradeMode.Clear();
            for (auto modesIter = modes.cbegin(); modesIter != modes.cend(); ++modesIter)
                {
                gradeMode +=
                    doc->GetReadabilityMessageCatalog().GetFormattedValue(
                        wxNumberFormatter::ToString(
                            *modesIter, 0, wxNumberFormatter::Style::Style_NoTrailingZeroes),
                        NumberFormatInfo::NumberFormatType::CustomFormatting) +
                    L"; ";
                }
            // chop off the last "; "
            if (gradeMode.length() > 2)
                {
                gradeMode.RemoveLast(2);
                }
            }
        // get the average grade level
        if (ages.size() > 0)
            {
            ageAverage = wxNumberFormatter::ToString(
                statistics::mean(ages), 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
            ageMedian = wxNumberFormatter::ToString(
                statistics::median(ages), 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
            std::set<double> modes = statistics::mode(ages, floor_value<double>{});
            ageMode.Clear();
            for (auto modesIter = modes.cbegin(); modesIter != modes.cend(); ++modesIter)
                {
                ageMode += wxNumberFormatter::ToString(
                               *modesIter, 0, wxNumberFormatter::Style::Style_NoTrailingZeroes) +
                           L"; ";
                }
            // chop off the last "; "
            if (ageMode.length() > 2)
                {
                ageMode.RemoveLast(2);
                }
            }
        // get the average cloze score
        if (clozeScores.size() > 0)
            {
            clozeAverage = wxNumberFormatter::ToString(
                statistics::mean(clozeScores), 2, wxNumberFormatter::Style::Style_NoTrailingZeroes);
            clozeMedian =
                wxNumberFormatter::ToString(statistics::median(clozeScores), 2,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes);
            std::set<double> modes = statistics::mode(clozeScores, floor_value<double>{});
            clozeMode.Clear();
            for (auto modesIter = modes.cbegin(); modesIter != modes.cend(); ++modesIter)
                {
                clozeMode += wxNumberFormatter::ToString(
                                 *modesIter, 0, wxNumberFormatter::Style::Style_NoTrailingZeroes) +
                             L"; ";
                }
            // chop off the last "; "
            if (clozeMode.length() > 2)
                {
                clozeMode.RemoveLast(2);
                }
            }

        const int firstStatLocation =
            GetReadabilityScoresList()->GetResultsListCtrl()->AddRow(GetAverageLabel());
        GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(
            firstStatLocation, 1, gradeAverage,
            NumberFormatInfo(NumberFormatInfo::NumberFormatType::CustomFormatting, 1));
        GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(firstStatLocation, 2,
                                                                      ageAverage);
        GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(firstStatLocation, 3,
                                                                      _(L"N/A"));
        GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(firstStatLocation, 4,
                                                                      clozeAverage);
        GetReadabilityScoresList()->GetResultsListCtrl()->SetRowAttributes(firstStatLocation,
                                                                           statRowAttribs);

        // format the explanation of the averages
        wxString explanationString =
            L"<table class='minipage' style='width:100%;'>" +
            wxString::Format(
                L"\n\t<thead><tr><td style='background:%s;'><span style='color:%s;'>",
                ProjectReportFormat::GetReportHeaderColor().GetAsString(wxC2S_HTML_SYNTAX),
                ProjectReportFormat::GetReportHeaderFontColor().GetAsString(wxC2S_HTML_SYNTAX));
        explanationString += _(L"Averages");
        explanationString += L"</span></td></tr></thead>\n\t<tr><td>";
        explanationString +=
            wxString::Format(
                _(L"<p>Average grade level: %s<br />Average reading age: %s<br />"
                  "Average predicted cloze score: %s</p><p>Note that an average of the "
                  "scale values is not applicable because the scales used between tests are "
                  "different.</p>"),
                doc->GetReadabilityMessageCatalog().GetFormattedValue(
                    gradeAverage, NumberFormatInfo::NumberFormatType::CustomFormatting),
                ageAverage, clozeAverage) +
            L"</td></tr>\n</table>";

        GetReadabilityScoresList()->GetExplanations()[GetAverageLabel()] = explanationString;

        if (doc->GetStatisticsReportInfo().IsExtendedInformationEnabled())
            {
            // Mode
            long statLocation =
                GetReadabilityScoresList()->GetResultsListCtrl()->AddRow(GetModeLabel());
            GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(statLocation, 1,
                                                                          gradeMode);
            GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(statLocation, 2, ageMode);
            GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(statLocation, 3,
                                                                          _(L"N/A"));
            GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(statLocation, 4,
                                                                          clozeMode);
            GetReadabilityScoresList()->GetResultsListCtrl()->SetRowAttributes(statLocation,
                                                                               statRowAttribs);

            // format the explanation of the modes
            explanationString =
                L"<table class='minipage' style='width:100%;'>" +
                wxString::Format(
                    L"\n\t<thead><tr><td style='background:%s;'><span style='color:%s;'>",
                    ProjectReportFormat::GetReportHeaderColor().GetAsString(wxC2S_HTML_SYNTAX),
                    ProjectReportFormat::GetReportHeaderFontColor().GetAsString(wxC2S_HTML_SYNTAX));
            explanationString += _(L"Modes");
            explanationString += L"</span></td></tr></thead>\n\t<tr><td>";
            explanationString +=
                wxString::Format(
                    _(L"<p>Grade level mode(s): %s<br />Reading age mode(s): %s<br />"
                      "Predicted cloze score mode(s): %s</p><p>The mode is the most frequently "
                      "occurring value in a range of data. Note that grade-level scores are "
                      "rounded down when searching for the mode.</p><p>Note that a mode of the "
                      "scale values is not applicable because the scales used between "
                      "tests are different.</p>"),
                    gradeMode, ageMode, clozeMode) +
                L"</td></tr>\n</table>";

            GetReadabilityScoresList()->GetExplanations()[GetModeLabel()] = explanationString;

            // Median
            statLocation =
                GetReadabilityScoresList()->GetResultsListCtrl()->AddRow(GetMedianLabel());
            GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(
                statLocation, 1, gradeMedian,
                NumberFormatInfo(NumberFormatInfo::NumberFormatType::CustomFormatting, 1));
            GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(statLocation, 2,
                                                                          ageMedian);
            GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(statLocation, 3,
                                                                          _(L"N/A"));
            GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(statLocation, 4,
                                                                          clozeMedian);
            GetReadabilityScoresList()->GetResultsListCtrl()->SetRowAttributes(statLocation,
                                                                               statRowAttribs);

            // format the explanation of the medians
            explanationString =
                L"<table class='minipage' style='width:100%;'>" +
                wxString::Format(
                    L"\n\t<thead><tr><td style='background:%s;'><span style='color:%s;'>",
                    ProjectReportFormat::GetReportHeaderColor().GetAsString(wxC2S_HTML_SYNTAX),
                    ProjectReportFormat::GetReportHeaderFontColor().GetAsString(wxC2S_HTML_SYNTAX));
            explanationString += _(L"Medians");
            explanationString += L"</span></td></tr></thead>\n\t<tr><td>";
            explanationString +=
                wxString::Format(
                    _(L"<p>Grade level median: %s<br />Reading age median: %s<br />"
                      "Predicted cloze score median: %s</p><p>The median is the midpoint of a "
                      "given range of values that divides them into lower and higher halves.</p><p>"
                      "Note that a median of the scale values is not applicable because the scales "
                      "used between tests are different.</p>"),
                    doc->GetReadabilityMessageCatalog().GetFormattedValue(
                        gradeMedian, NumberFormatInfo::NumberFormatType::CustomFormatting),
                    ageMedian, clozeMedian) +
                L"</td></tr>\n</table>";

            GetReadabilityScoresList()->GetExplanations()[GetMedianLabel()] = explanationString;

            // get the standard deviation
            wxString gradeStdDev =
                ((grades.size() < 2) ?
                     _(L"N/A") :
                     wxNumberFormatter::ToString(
                         statistics::standard_deviation(grades, doc->GetVarianceMethod() ==
                                                                    VarianceMethod::SampleVariance),
                         1, wxNumberFormatter::Style::Style_NoTrailingZeroes));

            const wxString ageStdDev =
                ((ages.size() < 2) ?
                     _(L"N/A") :
                     wxNumberFormatter::ToString(
                         statistics::standard_deviation(ages, doc->GetVarianceMethod() ==
                                                                  VarianceMethod::SampleVariance),
                         1, wxNumberFormatter::Style::Style_NoTrailingZeroes));

            const wxString clozeStdDev =
                ((clozeScores.size() < 2) ?
                     _(L"N/A") :
                     wxNumberFormatter::ToString(
                         statistics::standard_deviation(clozeScores,
                                                        doc->GetVarianceMethod() ==
                                                            VarianceMethod::SampleVariance),
                         2, wxNumberFormatter::Style::Style_NoTrailingZeroes));

            statLocation =
                GetReadabilityScoresList()->GetResultsListCtrl()->AddRow(GetStdDevLabel());
            GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(
                statLocation, 1, gradeStdDev,
                NumberFormatInfo(NumberFormatInfo::NumberFormatType::CustomFormatting, 1));
            GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(statLocation, 2,
                                                                          ageStdDev);
            GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(statLocation, 3,
                                                                          _(L"N/A"));
            GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(statLocation, 4,
                                                                          clozeStdDev);
            GetReadabilityScoresList()->GetResultsListCtrl()->SetRowAttributes(statLocation,
                                                                               statRowAttribs);

            // format the explanation of the variances
            explanationString =
                L"<table class='minipage' style='width:100%'>" +
                wxString::Format(
                    L"\n    <thead><tr><td style='background:%s;'><span style='color:%s;'>",
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
                    doc->GetReadabilityMessageCatalog().GetFormattedValue(
                        gradeStdDev, NumberFormatInfo::NumberFormatType::CustomFormatting),
                    ageStdDev, clozeStdDev) +
                L"</td></tr>\n</table>";

            GetReadabilityScoresList()->GetExplanations()[GetStdDevLabel()] = explanationString;
            }

        GetReadabilityScoresList()->GetResultsListCtrl()->SetSortableRange(0,
                                                                           firstStatLocation - 1);
        }
    else
        {
        GetReadabilityScoresList()->GetResultsListCtrl()->SetSortableRange(0, 0);
        }

    if (GetReadabilityScoresList()->GetResultsListCtrl()->GetSortedColumn() == -1)
        {
        GetReadabilityScoresList()->GetResultsListCtrl()->SetSortedColumn(
            0, Wisteria::SortDirection::SortAscending);
        }
    GetReadabilityScoresList()->GetResultsListCtrl()->Resort();

    // select the item user had selected before the update
    const auto selectedItemLocation =
        GetReadabilityScoresList()->GetResultsListCtrl()->FindEx(selectedItem);
    if (selectedItemLocation != wxNOT_FOUND)
        {
        GetReadabilityScoresList()->GetResultsListCtrl()->Select(selectedItemLocation);
        }
    }

//-------------------------------------------------------
void ProjectView::OnTextWindowColorsChange([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    ProjectDoc* doc = dynamic_cast<ProjectDoc*>(GetDocument());
    if (!doc->IsSafeToUpdate())
        {
        return;
        }

    ToolsOptionsDlg optionsDlg(GetDocFrame(), dynamic_cast<ProjectDoc*>(GetDocument()),
                               ToolsOptionsDlg::TextSection);
    if (GetSideBar()->GetSelectedFolderId() == SIDEBAR_DOLCH_SECTION_ID)
        {
        optionsDlg.SelectPage(ToolsOptionsDlg::DOCUMENT_DISPLAY_DOLCH_PAGE);
        }
    else
        {
        optionsDlg.SelectPage(ToolsOptionsDlg::DOCUMENT_DISPLAY_GENERAL_PAGE);
        }

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
        {
        return;
        }

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
        {
        return;
        }
    wxWindowUpdateLocker noUpdates(doc->GetDocumentWindow());
    BaseProjectProcessingLock processingLock(doc);

    doc->GetReadabilityMessageCatalog().SetLongGradeScaleFormat(
        !doc->GetReadabilityMessageCatalog().IsUsingLongGradeScaleFormat());
    GetReadabilityScoresList()->GetResultsListCtrl()->Refresh();
    GetReadabilityScoresList()->GetResultsListCtrl()->SetColumnWidth(
        1, GetReadabilityScoresList()->GetResultsListCtrl()->EstimateColumnWidth(1));
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
        const auto [parentId, childId] = GetSideBar()->FindSubItem(
            SIDEBAR_READABILITY_SCORES_SECTION_ID, READABILITY_SCORES_PAGE_ID);
        if (!childId.has_value())
            {
            return;
            }
        GetSideBar()->SelectSubItem(parentId.value(), childId.value());
        }
    const auto selectedIndex = GetReadabilityScoresList()->GetResultsListCtrl()->GetFirstSelected();
    if (selectedIndex != wxNOT_FOUND)
        {
        const wxString testToRemove =
            GetReadabilityScoresList()->GetResultsListCtrl()->GetItemText(selectedIndex);
        if (testToRemove == GetAverageLabel() || testToRemove == GetMedianLabel() ||
            testToRemove == GetStdDevLabel() || testToRemove == GetModeLabel())
            {
            return;
            }

        auto warningIter = WarningManager::GetWarning(_DT(L"remove-test-from-project"));
        // if they really want to remove this test
        if (warningIter != WarningManager::GetWarnings().end() && warningIter->ShouldBeShown())
            {
            wxRichMessageDialog msg(
                GetDocFrame(),
                wxString::Format(_(L"Do you wish to remove \"%s\" from the project?"),
                                 testToRemove),
                _(L"Remove Test"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);
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
                {
                return;
                }
            }

        GetReadabilityScoresList()->GetResultsListCtrl()->DeleteItem(selectedIndex);
        UpdateStatistics();
        GetReadabilityScoresList()->GetExplanationView()->SetPage(wxString{});

        ProjectDoc* doc = dynamic_cast<ProjectDoc*>(GetDocument());
        doc->RemoveTest(testToRemove);
        // if removing Dolch, we need remove the Dolch section
        if (testToRemove == ReadabilityMessages::GetDolchLabel())
            {
            GetDolchSightWordsView().Clear();
            }
        // remove the Averages row if there are no tests left
        if (GetReadabilityScoresList()->GetResultsListCtrl()->GetItemCount() == 1)
            {
            const long location =
                GetReadabilityScoresList()->GetResultsListCtrl()->FindEx(GetAverageLabel(), 0);
            if (location != wxNOT_FOUND)
                {
                GetReadabilityScoresList()->GetResultsListCtrl()->DeleteItem(location);
                }
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
        {
        wxMessageBox(_(L"Please select a test to remove."), wxGetApp().GetAppName(),
                     wxOK | wxICON_INFORMATION);
        }
    }

/// project view side bar was clicked
//-------------------------------------------------------
void ProjectView::OnItemSelected(wxCommandEvent& event)
    {
    assert(GetRibbon() != nullptr);
    const auto hideEditPanel = [this](const wxWindowID windowId)
    {
        wxWindow* editButtonBarWindow = GetRibbon()->FindWindow(windowId);
        assert(editButtonBarWindow != nullptr);
        assert(editButtonBarWindow->IsKindOf(CLASSINFO(wxRibbonPanel)));
        editButtonBarWindow->Show(false);
        return dynamic_cast<wxRibbonPanel*>(editButtonBarWindow);
    };

    const auto getEditButtonBar = [](wxRibbonPanel* panel)
    {
        auto buttonBar = panel->FindWindow(MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);
        assert(buttonBar != nullptr && buttonBar->IsKindOf(CLASSINFO(wxRibbonButtonBar)));
        return dynamic_cast<wxRibbonButtonBar*>(buttonBar);
    };

    const auto resetActiveCanvasResizeDelay = [this]()
    {
        if (GetActiveProjectWindow() != nullptr &&
            typeid(*GetActiveProjectWindow()) == typeid(Wisteria::Canvas))
            {
            assert(dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow()));
            dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow())->ResetResizeDelay();
            }
    };

    wxRibbonPanel* editListButtonBarWindow = hideEditPanel(MainFrame::ID_EDIT_RIBBON_LIST_PANEL);
    wxRibbonPanel* editSummaryReportButtonBarWindow =
        hideEditPanel(MainFrame::ID_EDIT_RIBBON_SUMMARY_REPORT_PANEL);
    wxRibbonPanel* editExpListButtonBarWindow =
        hideEditPanel(MainFrame::ID_EDIT_RIBBON_EXPLANATION_LIST_PANEL);
    wxRibbonPanel* editReportButtonBarWindow =
        hideEditPanel(MainFrame::ID_EDIT_RIBBON_REPORT_PANEL);
    wxRibbonPanel* editStatsListButtonBarWindow =
        hideEditPanel(MainFrame::ID_EDIT_RIBBON_STATS_LIST_PANEL);
    wxRibbonPanel* editStatsReportButtonBarWindow =
        hideEditPanel(MainFrame::ID_EDIT_RIBBON_STATS_SUMMARY_REPORT_PANEL);
    wxRibbonPanel* editSimpleListWithSummationButtonBarWindow =
        hideEditPanel(MainFrame::ID_EDIT_RIBBON_LIST_SIMPLE_WITH_SUM_PANEL);
    wxRibbonPanel* editSimpleListWithSummationAndExcludButtonBarWindow =
        hideEditPanel(MainFrame::ID_EDIT_RIBBON_LIST_SIMPLE_WITH_SUM_AND_EXCLUDE_PANEL);
    wxRibbonPanel* editSimpleListButtonBarWindow =
        hideEditPanel(MainFrame::ID_EDIT_RIBBON_LIST_SIMPLE_PANEL);
    wxRibbonPanel* editBarChartButtonBarWindow =
        hideEditPanel(MainFrame::ID_EDIT_RIBBON_BAR_CHART_PANEL);
    wxRibbonPanel* editBoxPlotButtonBarWindow =
        hideEditPanel(MainFrame::ID_EDIT_RIBBON_BOX_PLOT_PANEL);
    wxRibbonPanel* editHistogramButtonBarWindow =
        hideEditPanel(MainFrame::ID_EDIT_RIBBON_HISTOGRAM_PANEL);
    wxRibbonPanel* editSyllableHistogramButtonBarWindow =
        hideEditPanel(MainFrame::ID_EDIT_RIBBON_SYLLABLE_HISTOGRAM_PANEL);
    wxRibbonPanel* editPieChartButtonBarWindow =
        hideEditPanel(MainFrame::ID_EDIT_RIBBON_PIE_CHART_PANEL);
    wxRibbonPanel* editWordCloudButtonBarWindow =
        hideEditPanel(MainFrame::ID_EDIT_RIBBON_WORDCLOUD_PANEL);
    wxRibbonPanel* editGraphButtonBarWindow = hideEditPanel(MainFrame::ID_EDIT_RIBBON_GRAPH_PANEL);
    wxRibbonPanel* editLixGermanButtonBarWindow =
        hideEditPanel(MainFrame::ID_EDIT_RIBBON_LIX_GERMAN_PANEL);
    wxRibbonPanel* editRaygorButtonBarWindow =
        hideEditPanel(MainFrame::ID_EDIT_RIBBON_RAYGOR_PANEL);
    wxRibbonPanel* editFrySchwartzButtonBarWindow =
        hideEditPanel(MainFrame::ID_EDIT_RIBBON_FRY_PANEL);
    // hide batch panels that we don't use here
    hideEditPanel(MainFrame::ID_EDIT_RIBBON_FLESCH_PANEL);
    hideEditPanel(MainFrame::ID_EDIT_RIBBON_LIST_CSVSS_PANEL);
    hideEditPanel(MainFrame::ID_EDIT_RIBBON_LIST_TEST_SCORES_PANEL);
    hideEditPanel(MainFrame::ID_EDIT_RIBBON_HISTOGRAM_BATCH_PANEL);

    if (event.GetInt() == READABILITY_SCORES_PAGE_ID ||
        event.GetInt() == READABILITY_SCORES_SUMMARY_REPORT_PAGE_ID ||
        event.GetInt() == READABILITY_GOALS_PAGE_ID)
        {
        m_activeWindow = GetReadabilityResultsView().FindWindowById(event.GetInt());
        resetActiveCanvasResizeDelay();
        assert(m_activeWindow != nullptr);

        if (GetActiveProjectWindow())
            {
            GetSplitter()->GetWindow2()->Hide();
            GetSplitter()->ReplaceWindow(GetSplitter()->GetWindow2(), GetActiveProjectWindow());
            GetActiveProjectWindow()->Show();

            if (GetRibbon())
                {
                if (GetActiveProjectWindow()->IsKindOf(CLASSINFO(wxHtmlWindow)))
                    {
                    editSummaryReportButtonBarWindow->Show();
                    }
                else if (GetActiveProjectWindow()->IsKindOf(CLASSINFO(ExplanationListCtrl)))
                    {
                    editExpListButtonBarWindow->Show();
                    getEditButtonBar(editExpListButtonBarWindow)
                        ->ToggleButton(XRCID("ID_LONG_FORMAT"),
                                       dynamic_cast<ProjectDoc*>(GetDocument())
                                           ->GetReadabilityMessageCatalog()
                                           .IsUsingLongGradeScaleFormat());
                    }
                else if (GetActiveProjectWindow()->IsKindOf(CLASSINFO(ListCtrlEx)))
                    {
                    editListButtonBarWindow->Show();
                    }
                }
            }
        }
    else if (event.GetInt() == FLESCH_CHART_PAGE_ID || event.GetInt() == DB2_PAGE_ID ||
             event.GetInt() == FRY_PAGE_ID || event.GetInt() == RAYGOR_PAGE_ID ||
             event.GetInt() == CRAWFORD_GRAPH_PAGE_ID || event.GetInt() == FRASE_PAGE_ID ||
             event.GetInt() == SCHWARTZ_PAGE_ID || event.GetInt() == LIX_GAUGE_PAGE_ID ||
             event.GetInt() == LIX_GAUGE_GERMAN_PAGE_ID || event.GetInt() == GPM_FRY_PAGE_ID)
        {
        m_activeWindow = GetReadabilityResultsView().FindWindowById(event.GetInt());
        resetActiveCanvasResizeDelay();
        assert(m_activeWindow != nullptr);

        if (GetActiveProjectWindow())
            {
            GetSplitter()->GetWindow2()->Hide();
            GetSplitter()->ReplaceWindow(GetSplitter()->GetWindow2(), GetActiveProjectWindow());
            GetActiveProjectWindow()->Show();

            if (GetRibbon())
                {
                const auto graphType =
                    dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow())->GetFixedObject(0, 0);

                if (typeid(*graphType) == typeid(LixGaugeGerman))
                    {
                    editLixGermanButtonBarWindow->Show();
                    getEditButtonBar(editLixGermanButtonBarWindow)
                        ->ToggleButton(
                            XRCID("ID_DROP_SHADOW"),
                            dynamic_cast<ProjectDoc*>(GetDocument())->IsDisplayingDropShadows());
                    getEditButtonBar(editLixGermanButtonBarWindow)
                        ->ToggleButton(XRCID("ID_USE_ENGLISH_LABELS"),
                                       dynamic_cast<ProjectDoc*>(GetDocument())
                                           ->IsUsingEnglishLabelsForGermanLix());
                    }
                else if (typeid(*graphType) == typeid(RaygorGraph))
                    {
                    editRaygorButtonBarWindow->Show();
                    getEditButtonBar(editRaygorButtonBarWindow)
                        ->ToggleButton(
                            XRCID("ID_DROP_SHADOW"),
                            dynamic_cast<ProjectDoc*>(GetDocument())->IsDisplayingDropShadows());
                    }
                else if (typeid(*graphType) == typeid(FryGraph) ||
                         typeid(*graphType) == typeid(SchwartzGraph))
                    {
                    editFrySchwartzButtonBarWindow->Show();
                    getEditButtonBar(editFrySchwartzButtonBarWindow)
                        ->ToggleButton(
                            XRCID("ID_DROP_SHADOW"),
                            dynamic_cast<ProjectDoc*>(GetDocument())->IsDisplayingDropShadows());
                    }
                else if (typeid(*GetActiveProjectWindow()) == typeid(Wisteria::Canvas))
                    {
                    editGraphButtonBarWindow->Show();
                    getEditButtonBar(editGraphButtonBarWindow)
                        ->ToggleButton(
                            XRCID("ID_DROP_SHADOW"),
                            dynamic_cast<ProjectDoc*>(GetDocument())->IsDisplayingDropShadows());
                    }
                }
            }
        }
    else if (event.GetExtraLong() == SIDEBAR_SENTENCES_BREAKDOWN_SECTION_ID)
        {
        m_activeWindow = GetSentencesBreakdownView().FindWindowById(event.GetInt());
        resetActiveCanvasResizeDelay();
        assert(m_activeWindow != nullptr);

        if (GetActiveProjectWindow())
            {
            GetSplitter()->GetWindow2()->Hide();
            GetSplitter()->ReplaceWindow(GetSplitter()->GetWindow2(), GetActiveProjectWindow());
            GetActiveProjectWindow()->Show();

            if (GetRibbon())
                {
                if (typeid(*GetActiveProjectWindow()) == typeid(Wisteria::Canvas) &&
                    typeid(*dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow())
                                ->GetFixedObject(0, 0)) == typeid(Wisteria::Graphs::BoxPlot))
                    {
                    editBoxPlotButtonBarWindow->Show();
                    getEditButtonBar(editBoxPlotButtonBarWindow)
                        ->ToggleButton(
                            XRCID("ID_BOX_PLOT_DISPLAY_ALL_POINTS"),
                            dynamic_cast<ProjectDoc*>(GetDocument())->IsShowingAllBoxPlotPoints());
                    getEditButtonBar(editBoxPlotButtonBarWindow)
                        ->ToggleButton(
                            XRCID("ID_BOX_PLOT_DISPLAY_LABELS"),
                            dynamic_cast<ProjectDoc*>(GetDocument())->IsDisplayingBoxPlotLabels());
                    getEditButtonBar(editBoxPlotButtonBarWindow)
                        ->ToggleButton(
                            XRCID("ID_DROP_SHADOW"),
                            dynamic_cast<ProjectDoc*>(GetDocument())->IsDisplayingDropShadows());
                    }
                else if (typeid(*GetActiveProjectWindow()) == typeid(Wisteria::Canvas) &&
                         typeid(*dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow())
                                     ->GetFixedObject(0, 0)) == typeid(Wisteria::Graphs::Histogram))
                    {
                    editHistogramButtonBarWindow->Show();
                    getEditButtonBar(editHistogramButtonBarWindow)
                        ->ToggleButton(
                            XRCID("ID_DROP_SHADOW"),
                            dynamic_cast<ProjectDoc*>(GetDocument())->IsDisplayingDropShadows());
                    }
                else if (typeid(*GetActiveProjectWindow()) == typeid(Wisteria::Canvas))
                    {
                    editGraphButtonBarWindow->Show();
                    getEditButtonBar(editGraphButtonBarWindow)
                        ->ToggleButton(
                            XRCID("ID_DROP_SHADOW"),
                            dynamic_cast<ProjectDoc*>(GetDocument())->IsDisplayingDropShadows());
                    }
                else if (GetActiveProjectWindow()->IsKindOf(CLASSINFO(ListCtrlEx)))
                    {
                    editListButtonBarWindow->Show();
                    }
                }
            }
        }
    else if (event.GetExtraLong() == SIDEBAR_STATS_SUMMARY_SECTION_ID)
        {
        m_activeWindow = GetSummaryView().FindWindowById(event.GetInt());
        resetActiveCanvasResizeDelay();
        assert(m_activeWindow != nullptr);

        if (GetActiveProjectWindow())
            {
            GetSplitter()->GetWindow2()->Hide();
            GetSplitter()->ReplaceWindow(GetSplitter()->GetWindow2(), GetActiveProjectWindow());
            GetActiveProjectWindow()->Show();

            if (GetRibbon())
                {
                if (GetActiveProjectWindow()->IsKindOf(CLASSINFO(ListCtrlEx)))
                    {
                    editStatsListButtonBarWindow->Show();
                    }
                else if (GetActiveProjectWindow()->IsKindOf(CLASSINFO(HtmlTableWindow)))
                    {
                    editStatsReportButtonBarWindow->Show();
                    }
                }
            }
        }
    else if (event.GetExtraLong() == SIDEBAR_WORDS_BREAKDOWN_SECTION_ID)
        {
        // Note that word-list tests can have a list control and highlighted report
        // with the same integral ID, so rely on searching by the ID and name of the window.
        m_activeWindow =
            GetWordsBreakdownView().FindWindowByIdAndLabel(event.GetInt(), event.GetString());
        if (!GetActiveProjectWindow())
            {
            m_activeWindow = GetWordsBreakdownView().FindWindowById(event.GetInt());
            }
        resetActiveCanvasResizeDelay();
        assert(m_activeWindow != nullptr);

        if (GetActiveProjectWindow())
            {
            GetSplitter()->GetWindow2()->Hide();
            GetSplitter()->ReplaceWindow(GetSplitter()->GetWindow2(), GetActiveProjectWindow());
            GetActiveProjectWindow()->Show();

            if (GetRibbon())
                {
                if (typeid(*GetActiveProjectWindow()) == typeid(Wisteria::Canvas) &&
                    typeid(*dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow())
                                ->GetFixedObject(0, 0)) == typeid(Wisteria::Graphs::Histogram))
                    {
                    editSyllableHistogramButtonBarWindow->Show();
                    getEditButtonBar(editSyllableHistogramButtonBarWindow)
                        ->ToggleButton(
                            XRCID("ID_DROP_SHADOW"),
                            dynamic_cast<ProjectDoc*>(GetDocument())->IsDisplayingDropShadows());
                    }
                else if (typeid(*GetActiveProjectWindow()) == typeid(Wisteria::Canvas) &&
                         typeid(*dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow())
                                     ->GetFixedObject(0, 0)) == typeid(Wisteria::Graphs::BarChart))
                    {
                    editBarChartButtonBarWindow->Show();
                    getEditButtonBar(editBarChartButtonBarWindow)
                        ->ToggleButton(
                            XRCID("ID_DROP_SHADOW"),
                            dynamic_cast<ProjectDoc*>(GetDocument())->IsDisplayingDropShadows());
                    getEditButtonBar(editBarChartButtonBarWindow)
                        ->ToggleButton(
                            XRCID("ID_EDIT_BAR_LABELS"),
                            dynamic_cast<ProjectDoc*>(GetDocument())->IsDisplayingBarChartLabels());
                    }
                else if (typeid(*GetActiveProjectWindow()) == typeid(Wisteria::Canvas) &&
                         typeid(*dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow())
                                     ->GetFixedObject(0, 0)) == typeid(Wisteria::Graphs::PieChart))
                    {
                    editPieChartButtonBarWindow->Show();
                    getEditButtonBar(editPieChartButtonBarWindow)
                        ->ToggleButton(
                            XRCID("ID_EDIT_GRAPH_SHOWCASE_COMPLEX_WORDS"),
                            dynamic_cast<ProjectDoc*>(GetDocument())->IsShowcasingComplexWords());
                    getEditButtonBar(editPieChartButtonBarWindow)
                        ->ToggleButton(
                            XRCID("ID_DROP_SHADOW"),
                            dynamic_cast<ProjectDoc*>(GetDocument())->IsDisplayingDropShadows());
                    }
                else if (typeid(*GetActiveProjectWindow()) == typeid(Wisteria::Canvas) &&
                         typeid(*dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow())
                                     ->GetFixedObject(0, 0)) == typeid(Wisteria::Graphs::WordCloud))
                    {
                    editWordCloudButtonBarWindow->Show();
                    getEditButtonBar(editWordCloudButtonBarWindow)
                        ->ToggleButton(
                            XRCID("ID_DROP_SHADOW"),
                            dynamic_cast<ProjectDoc*>(GetDocument())->IsDisplayingDropShadows());
                    }
                else if (typeid(*GetActiveProjectWindow()) == typeid(Wisteria::Canvas))
                    {
                    editGraphButtonBarWindow->Show();
                    getEditButtonBar(editGraphButtonBarWindow)
                        ->ToggleButton(
                            XRCID("ID_DROP_SHADOW"),
                            dynamic_cast<ProjectDoc*>(GetDocument())->IsDisplayingDropShadows());
                    }
                else if (GetActiveProjectWindow()->IsKindOf(CLASSINFO(ListCtrlEx)))
                    {
                    // compressed list of words combine stemmed words into a list,
                    // so it's not a list of individual words that a user can ignore
                    if (event.GetInt() == ALL_WORDS_CONDENSED_LIST_PAGE_ID)
                        {
                        editSimpleListWithSummationButtonBarWindow->Show();
                        }
                    else
                        {
                        editSimpleListWithSummationAndExcludButtonBarWindow->Show();
                        }
                    }
                else if (GetActiveProjectWindow()->IsKindOf(CLASSINFO(FormattedTextCtrl)))
                    {
                    editReportButtonBarWindow->Show();
                    }
                }
            }
        }
    else if (event.GetExtraLong() == SIDEBAR_GRAMMAR_SECTION_ID)
        {
        m_activeWindow = GetGrammarView().FindWindowById(event.GetInt());
        resetActiveCanvasResizeDelay();
        assert(m_activeWindow != nullptr);

        if (GetActiveProjectWindow())
            {
            GetSplitter()->GetWindow2()->Hide();
            GetSplitter()->ReplaceWindow(GetSplitter()->GetWindow2(), GetActiveProjectWindow());
            GetActiveProjectWindow()->Show();

            if (GetRibbon())
                {
                if (GetActiveProjectWindow()->IsKindOf(CLASSINFO(FormattedTextCtrl)))
                    {
                    editReportButtonBarWindow->Show();
                    }
                else
                    {
                    if (event.GetInt() == OVERUSED_WORDS_BY_SENTENCE_LIST_PAGE_ID)
                        {
                        editListButtonBarWindow->Show();
                        }
                    else if (event.GetInt() == MISSPELLED_WORD_LIST_PAGE_ID ||
                             event.GetInt() == PASSIVE_VOICE_PAGE_ID ||
                             event.GetInt() == PROPER_NOUNS_LIST_PAGE_ID)
                        {
                        editSimpleListWithSummationButtonBarWindow->Show();
                        }
                    else
                        {
                        editSimpleListButtonBarWindow->Show();
                        }
                    }
                }
            }
        }
    else if (event.GetExtraLong() == SIDEBAR_DOLCH_SECTION_ID)
        {
        m_activeWindow = GetDolchSightWordsView().FindWindowById(event.GetInt());
        resetActiveCanvasResizeDelay();
        assert(m_activeWindow != nullptr);

        if (GetActiveProjectWindow())
            {
            GetSplitter()->GetWindow2()->Hide();
            GetSplitter()->ReplaceWindow(GetSplitter()->GetWindow2(), GetActiveProjectWindow());
            GetActiveProjectWindow()->Show();
            if (GetMenuBar())
                {
                MenuBarEnableAll(GetMenuBar(), wxID_SELECTALL, true);
                }
            if (GetRibbon())
                {
                if (GetActiveProjectWindow()->IsKindOf(CLASSINFO(FormattedTextCtrl)))
                    {
                    editReportButtonBarWindow->Show();
                    }
                else if (GetActiveProjectWindow()->IsKindOf(CLASSINFO(HtmlTableWindow)))
                    {
                    editStatsReportButtonBarWindow->Show();
                    }
                else if (GetActiveProjectWindow()->IsKindOf(CLASSINFO(ListCtrlEx)))
                    {
                    if (event.GetInt() == NON_DOLCH_WORDS_LIST_PAGE_ID ||
                        event.GetInt() == DOLCH_WORDS_LIST_PAGE_ID)
                        {
                        editSimpleListWithSummationButtonBarWindow->Show();
                        }
                    else
                        {
                        editSimpleListButtonBarWindow->Show();
                        }
                    }
                else if (typeid(*GetActiveProjectWindow()) == typeid(Wisteria::Canvas))
                    {
                    editBarChartButtonBarWindow->Show();
                    getEditButtonBar(editBarChartButtonBarWindow)
                        ->ToggleButton(
                            XRCID("ID_DROP_SHADOW"),
                            dynamic_cast<ProjectDoc*>(GetDocument())->IsDisplayingDropShadows());
                    getEditButtonBar(editBarChartButtonBarWindow)
                        ->ToggleButton(
                            XRCID("ID_EDIT_BAR_LABELS"),
                            dynamic_cast<ProjectDoc*>(GetDocument())->IsDisplayingBarChartLabels());
                    }
                }
            }
        }

    // add the label for the window type to the export menu item
    assert(GetActiveProjectWindow());
    if (wxMenuItem *
            exportMenuItem{ GetDocFrame()->m_exportMenu.FindChildItem(XRCID("ID_SAVE_ITEM")) };
        exportMenuItem != nullptr && GetActiveProjectWindow() != nullptr)
        {
        exportMenuItem->SetItemLabel(
            wxString::Format(_(L"Export %s..."), GetActiveProjectWindow()->GetName()));
        }
    if (GetMenuBar())
        {
        GetMenuBar()->SetLabel(
            XRCID("ID_SAVE_ITEM"),
            wxString::Format(_(L"Export %s..."), GetActiveProjectWindow()->GetName()));
        MenuBarEnableAll(GetMenuBar(), wxID_SELECTALL, true);
        }

    GetRibbon()->Realize();
    GetRibbon()->Layout();

    event.Skip();
    }

//-------------------------------------------------------
bool ProjectView::ExportAll(const wxString& folder, wxString listExt, wxString textExt,
                            wxString graphExt, const bool includeWordsBreakdown,
                            const bool includeSentencesBreakdown, const bool includeTestScores,
                            const bool includeStatistics, const bool includeGrammar,
                            const bool includeSightWords, const bool includeLists,
                            const bool includeTextReports,
                            const Wisteria::UI::ImageExportOptions& graphOptions)
    {
    const ProjectDoc* doc = dynamic_cast<const ProjectDoc*>(GetDocument());

    if (!wxFileName::DirExists(folder))
        {
        if (folder.empty() || !wxFileName::Mkdir(folder, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL))
            {
            return false;
            }
        }
    // validate the extensions
    if (listExt.empty())
        {
        listExt = L".htm";
        }
    else if (listExt[0] != L'.')
        {
        listExt.insert(0, L".");
        }

    if (textExt.empty())
        {
        textExt = L".htm";
        }
    else if (textExt[0] != L'.')
        {
        textExt.insert(0, L".");
        }

    if (graphExt.empty())
        {
        graphExt = L".png";
        }
    else if (graphExt[0] != L'.')
        {
        graphExt.insert(0, L".");
        }

    BaseProjectProcessingLock processingLock(dynamic_cast<ProjectDoc*>(GetDocument()));

    wxBusyCursor bc;
    wxBusyInfo bi(wxBusyInfoFlags().Text(_(L"Exporting project...")));

    // the results window
    if (includeTestScores)
        {
        if (!wxFileName::Mkdir(folder + wxFileName::GetPathSeparator() +
                                   GetReadabilityScoresLabel(),
                               wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL))
            {
            wxMessageBox(wxString::Format(_(L"Unable to create \"%s\" folder."),
                                          GetReadabilityScoresLabel()),
                         wxGetApp().GetAppName(), wxOK | wxICON_EXCLAMATION);
            }
        else
            {
            for (size_t i = 0; i < GetReadabilityResultsView().GetWindowCount(); ++i)
                {
                wxWindow* activeWindow = GetReadabilityResultsView().GetWindow(i);
                if (activeWindow)
                    {
                    if (activeWindow->IsKindOf(CLASSINFO(ExplanationListCtrl)))
                        {
                        ExplanationListCtrl* list =
                            dynamic_cast<ExplanationListCtrl*>(activeWindow);
                        list->SetLabel(
                            wxString::Format(L"%s [%s]", list->GetName(),
                                             wxFileName::StripExtension(doc->GetTitle())));
                        list->Save(folder + wxFileName::GetPathSeparator() +
                                       GetReadabilityScoresLabel() +
                                       wxFileName::GetPathSeparator() + list->GetLabel() + L".htm",
                                   ExplanationListExportOptions::ExportGrid);
                        }
                    else if (activeWindow->IsKindOf(CLASSINFO(Wisteria::Canvas)))
                        {
                        Wisteria::Canvas* graphWindow =
                            dynamic_cast<Wisteria::Canvas*>(activeWindow);
                        graphWindow->SetLabel(
                            wxString::Format(L"%s [%s]", graphWindow->GetName(),
                                             wxFileName::StripExtension(doc->GetTitle())));
                        graphWindow->Save(
                            folder + wxFileName::GetPathSeparator() + GetReadabilityScoresLabel() +
                                wxFileName::GetPathSeparator() + graphWindow->GetLabel() + graphExt,
                            graphOptions);
                        }
                    else if (activeWindow->IsKindOf(CLASSINFO(HtmlTableWindow)))
                        {
                        HtmlTableWindow* reportWindow =
                            dynamic_cast<HtmlTableWindow*>(activeWindow);
                        reportWindow->SetLabel(
                            wxString::Format(L"%s [%s]", reportWindow->GetName(),
                                             wxFileName::StripExtension(doc->GetTitle())));
                        reportWindow->Save(
                            folder + wxFileName::GetPathSeparator() + GetReadabilityScoresLabel() +
                            wxFileName::GetPathSeparator() + reportWindow->GetLabel() + L".htm");
                        }
                    else if (activeWindow->IsKindOf(CLASSINFO(ListCtrlEx)))
                        {
                        ListCtrlEx* list = dynamic_cast<ListCtrlEx*>(activeWindow);
                        list->SetLabel(
                            wxString::Format(L"%s [%s]", list->GetName(),
                                             wxFileName::StripExtension(doc->GetTitle())));
                        list->Save(folder + wxFileName::GetPathSeparator() +
                                       GetReadabilityScoresLabel() +
                                       wxFileName::GetPathSeparator() + list->GetLabel() + listExt,
                                   GridExportOptions());
                        }
                    }
                }
            }
        }
    // the statistics
    if (includeStatistics && GetSummaryView().GetWindowCount())
        {
        if (!wxFileName::Mkdir(folder + wxFileName::GetPathSeparator() +
                                   GetSummaryStatisticsLabel(),
                               wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL))
            {
            wxMessageBox(wxString::Format(_(L"Unable to create \"%s\" folder."),
                                          GetSummaryStatisticsLabel()),
                         wxGetApp().GetAppName(), wxOK | wxICON_EXCLAMATION);
            }
        else
            {
            for (size_t i = 0; i < GetSummaryView().GetWindowCount(); ++i)
                {
                wxWindow* activeWindow = GetSummaryView().GetWindow(i);
                if (activeWindow)
                    {
                    if (activeWindow->IsKindOf(CLASSINFO(HtmlTableWindow)))
                        {
                        HtmlTableWindow* html = dynamic_cast<HtmlTableWindow*>(activeWindow);
                        html->SetLabel(
                            wxString::Format(L"%s [%s]", html->GetName(),
                                             wxFileName::StripExtension(doc->GetTitle())));
                        html->Save(folder + wxFileName::GetPathSeparator() +
                                   GetSummaryStatisticsLabel() + wxFileName::GetPathSeparator() +
                                   html->GetLabel() + L".htm");
                        }
                    else if (activeWindow->IsKindOf(CLASSINFO(ListCtrlEx)) && includeLists)
                        {
                        ListCtrlEx* list = dynamic_cast<ListCtrlEx*>(activeWindow);
                        list->SetLabel(
                            wxString::Format(L"%s [%s]", list->GetName(),
                                             wxFileName::StripExtension(doc->GetTitle())));
                        list->Save(folder + wxFileName::GetPathSeparator() +
                                       GetSummaryStatisticsLabel() +
                                       wxFileName::GetPathSeparator() + list->GetLabel() + listExt,
                                   GridExportOptions());
                        }
                    else if (typeid(*activeWindow) == typeid(Wisteria::Canvas))
                        {
                        Wisteria::Canvas* graphWindow =
                            dynamic_cast<Wisteria::Canvas*>(activeWindow);
                        graphWindow->SetLabel(
                            wxString::Format(L"%s [%s]", graphWindow->GetName(),
                                             wxFileName::StripExtension(doc->GetTitle())));
                        graphWindow->Save(
                            folder + wxFileName::GetPathSeparator() + GetSummaryStatisticsLabel() +
                                wxFileName::GetPathSeparator() + graphWindow->GetLabel() + graphExt,
                            graphOptions);
                        }
                    }
                }
            }
        }
    if (includeSentencesBreakdown && GetSentencesBreakdownView().GetWindowCount())
        {
        if (!wxFileName::Mkdir(folder + wxFileName::GetPathSeparator() +
                                   GetSentencesBreakdownLabel(),
                               wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL))
            {
            wxMessageBox(wxString::Format(_(L"Unable to create \"%s\" folder."),
                                          GetSentencesBreakdownLabel()),
                         wxGetApp().GetAppName(), wxOK | wxICON_EXCLAMATION);
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
                        list->Save(folder + wxFileName::GetPathSeparator() +
                                       GetSentencesBreakdownLabel() +
                                       wxFileName::GetPathSeparator() + list->GetLabel() + listExt,
                                   GridExportOptions());
                        }
                    else if (typeid(*activeWindow) == typeid(Wisteria::Canvas))
                        {
                        Wisteria::Canvas* graphWindow =
                            dynamic_cast<Wisteria::Canvas*>(activeWindow);
                        graphWindow->SetLabel(
                            wxString::Format(L"%s [%s]", graphWindow->GetName(),
                                             wxFileName::StripExtension(doc->GetTitle())));
                        graphWindow->Save(
                            folder + wxFileName::GetPathSeparator() + GetSentencesBreakdownLabel() +
                                wxFileName::GetPathSeparator() + graphWindow->GetLabel() + graphExt,
                            graphOptions);
                        }
                    }
                }
            }
        }
    // the words breakdown section
    if (includeWordsBreakdown && GetWordsBreakdownView().GetWindowCount())
        {
        if (!wxFileName::Mkdir(folder + wxFileName::GetPathSeparator() + GetWordsBreakdownLabel(),
                               wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL))
            {
            wxMessageBox(
                wxString::Format(_(L"Unable to create \"%s\" folder."), GetWordsBreakdownLabel()),
                wxGetApp().GetAppName(), wxOK | wxICON_EXCLAMATION);
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
                        list->Save(folder + wxFileName::GetPathSeparator() +
                                       GetWordsBreakdownLabel() + wxFileName::GetPathSeparator() +
                                       list->GetLabel() + listExt,
                                   GridExportOptions());
                        }
                    else if (activeWindow->IsKindOf(CLASSINFO(Wisteria::Canvas)))
                        {
                        Wisteria::Canvas* graphWindow =
                            dynamic_cast<Wisteria::Canvas*>(activeWindow);
                        graphWindow->SetLabel(
                            wxString::Format(L"%s [%s]", graphWindow->GetName(),
                                             wxFileName::StripExtension(doc->GetTitle())));
                        graphWindow->Save(
                            folder + wxFileName::GetPathSeparator() + GetWordsBreakdownLabel() +
                                wxFileName::GetPathSeparator() + graphWindow->GetLabel() + graphExt,
                            graphOptions);
                        }
                    else if (activeWindow->IsKindOf(CLASSINFO(FormattedTextCtrl)) &&
                             includeTextReports)
                        {
                        FormattedTextCtrl* text = dynamic_cast<FormattedTextCtrl*>(activeWindow);
                        text->SetTitleName(
                            wxString::Format(L"%s [%s]", text->GetName(),
                                             wxFileName::StripExtension(doc->GetTitle())));
                        text->Save(folder + wxFileName::GetPathSeparator() +
                                   GetWordsBreakdownLabel() + wxFileName::GetPathSeparator() +
                                   text->GetTitleName() + textExt);
                        }
                    }
                }
            }
        }
    // grammar
    if (includeGrammar && GetGrammarView().GetWindowCount())
        {
        if (!wxFileName::Mkdir(folder + wxFileName::GetPathSeparator() + GetGrammarLabel(),
                               wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL))
            {
            wxMessageBox(wxString::Format(_(L"Unable to create \"%s\" folder."), GetGrammarLabel()),
                         wxGetApp().GetAppName(), wxOK | wxICON_EXCLAMATION);
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
                        list->Save(folder + wxFileName::GetPathSeparator() + GetGrammarLabel() +
                                       wxFileName::GetPathSeparator() + list->GetLabel() + listExt,
                                   GridExportOptions());
                        }
                    else if (activeWindow->IsKindOf(CLASSINFO(HtmlTableWindow)))
                        {
                        HtmlTableWindow* html = dynamic_cast<HtmlTableWindow*>(activeWindow);
                        html->SetLabel(
                            wxString::Format(L"%s [%s]", html->GetName(),
                                             wxFileName::StripExtension(doc->GetTitle())));
                        html->Save(folder + wxFileName::GetPathSeparator() + GetGrammarLabel() +
                                   wxFileName::GetPathSeparator() + html->GetLabel() + L".htm");
                        }
                    else if (activeWindow->IsKindOf(CLASSINFO(FormattedTextCtrl)) &&
                             includeTextReports)
                        {
                        FormattedTextCtrl* text = dynamic_cast<FormattedTextCtrl*>(activeWindow);
                        text->SetTitleName(
                            wxString::Format(L"%s [%s]", text->GetName(),
                                             wxFileName::StripExtension(doc->GetTitle())));
                        text->Save(folder + wxFileName::GetPathSeparator() + GetGrammarLabel() +
                                   wxFileName::GetPathSeparator() + text->GetTitleName() + textExt);
                        }
                    }
                }
            }
        }
    // Sight Words
    if (includeSightWords && GetDolchSightWordsView().GetWindowCount())
        {
        if (!wxFileName::Mkdir(folder + wxFileName::GetPathSeparator() + GetDolchLabel(),
                               wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL))
            {
            wxMessageBox(wxString::Format(_(L"Unable to create \"%s\" folder."), GetDolchLabel()),
                         wxGetApp().GetAppName(), wxOK | wxICON_EXCLAMATION);
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
                        list->Save(folder + wxFileName::GetPathSeparator() + GetDolchLabel() +
                                       wxFileName::GetPathSeparator() + list->GetLabel() + listExt,
                                   GridExportOptions());
                        }
                    else if (activeWindow->IsKindOf(CLASSINFO(HtmlTableWindow)))
                        {
                        HtmlTableWindow* html = dynamic_cast<HtmlTableWindow*>(activeWindow);
                        html->SetLabel(
                            wxString::Format(L"%s [%s]", html->GetName(),
                                             wxFileName::StripExtension(doc->GetTitle())));
                        html->Save(folder + wxFileName::GetPathSeparator() + GetDolchLabel() +
                                   wxFileName::GetPathSeparator() + html->GetLabel() + L".htm");
                        }
                    else if (activeWindow->IsKindOf(CLASSINFO(FormattedTextCtrl)) &&
                             includeTextReports)
                        {
                        FormattedTextCtrl* text = dynamic_cast<FormattedTextCtrl*>(activeWindow);
                        text->SetTitleName(
                            wxString::Format(L"%s [%s]", text->GetName(),
                                             wxFileName::StripExtension(doc->GetTitle())));
                        text->Save(folder + wxFileName::GetPathSeparator() + GetDolchLabel() +
                                   wxFileName::GetPathSeparator() + text->GetTitleName() + textExt);
                        }
                    else if (typeid(*activeWindow) == typeid(Wisteria::Canvas))
                        {
                        Wisteria::Canvas* graph = dynamic_cast<Wisteria::Canvas*>(activeWindow);
                        graph->SetLabel(
                            wxString::Format(L"%s [%s]", graph->GetName(),
                                             wxFileName::StripExtension(doc->GetTitle())));
                        graph->Save(folder + wxFileName::GetPathSeparator() + GetDolchLabel() +
                                        wxFileName::GetPathSeparator() + graph->GetLabel() +
                                        graphExt,
                                    graphOptions);
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
                                  const bool includeTestScores, const bool includeStatistics,
                                  const bool includeGrammar, const bool includeSightWords,
                                  const bool includeLists, const bool includeTextReports,
                                  const Wisteria::UI::ImageExportOptions& graphOptions)
    {
    if (filePath.GetPath().empty())
        {
        return false;
        }
    const ProjectDoc* doc = dynamic_cast<const ProjectDoc*>(GetDocument());

    if (!wxFileName::DirExists(filePath.GetPathWithSep() + _DT(L"images")))
        {
        if (!wxFileName::Mkdir(filePath.GetPathWithSep() + _DT(L"images"), wxS_DIR_DEFAULT,
                               wxPATH_MKDIR_FULL))
            {
            return false;
            }
        }
    // validate the extension
    if (graphExt.empty())
        {
        graphExt = L".png";
        }
    else if (graphExt[0] != L'.')
        {
        graphExt.insert(0, L".");
        }

    BaseProjectProcessingLock processingLock(dynamic_cast<ProjectDoc*>(GetDocument()));

    wxBusyCursor bc;
    wxBusyInfo bi(wxBusyInfoFlags().Text(_(L"Exporting project...")));

    lily_of_the_valley::html_encode_text htmlEncode;
    wxString outputText, textWindowStyleSection;
    wxString headSection =
        L"<head>" +
        wxString::Format(
            L"\n    <meta name='generator' content='%s %s' />"
            "\n    <title>%s</title>"
            "\n    <meta http-equiv='content-type' content='text/html; charset=utf-8' />"
            "\n    <link rel='stylesheet' href='style.css'>"
            "\n</head>",
            wxGetApp().GetAppDisplayName(), wxGetApp().GetAppVersion(), doc->GetTitle());

    wchar_t textWindowStyleCounter = L'a';
    size_t sectionCounter = 0;
    size_t figureCounter = 0;
    size_t tableCounter = 0;

    const wxString pageBreak = L"<div style='page-break-before:always'></div><br />\n";

    const auto formatImageOutput =
        [&outputText, &sectionCounter, &figureCounter, pageBreak, doc, htmlEncode, graphExt,
         graphOptions, filePath](Wisteria::Canvas* canvas, const bool includeLeadingPageBreak)
    {
        if (!canvas)
            {
            return;
            }
        canvas->SetLabel(wxString::Format(L"%s [%s]", canvas->GetName(),
                                          wxFileName::StripExtension(doc->GetTitle())));
        canvas->Save(filePath.GetPathWithSep() + _DT(L"images") + wxFileName::GetPathSeparator() +
                         canvas->GetLabel() + graphExt,
                     graphOptions);

        outputText += wxString::Format(
            L"%s\n<div class='minipage figure'>\n<img src='images\\%s' />\n"
            "<div class='caption'>%s</div>\n</div>\n",
            (includeLeadingPageBreak ? pageBreak : wxString{}), canvas->GetLabel() + graphExt,
            wxString::Format(_(L"Figure %zu.%zu: %s"), sectionCounter, figureCounter++,
                             htmlEncode({ canvas->GetName().wc_str() }, true).c_str()));
    };

    const auto formatList = [&outputText, &htmlEncode, &sectionCounter, &tableCounter,
                             pageBreak](ListCtrlEx* list, const bool includeLeadingPageBreak)
    {
        if (!list)
            {
            return;
            }

        BaseProjectDoc::UpdateListOptions(list);
        wxString buffer;
        list->FormatToHtml(
            buffer, true, ListCtrlEx::ExportRowSelection::ExportAll, 0, -1, 0, -1, true, false,
            wxString::Format(_(L"Table %zu.%zu: %s"), sectionCounter, tableCounter++,
                             htmlEncode({ list->GetName().wc_str() }, true).c_str()));
        std::wstring htmlText{ buffer.wc_string() };
        lily_of_the_valley::html_format::strip_hyperlinks(htmlText);

        outputText += (includeLeadingPageBreak ? pageBreak : wxString{}) +
                      html_extract_text::get_body(htmlText);
    };

    const auto formatTextWindow =
        [&outputText, &htmlEncode, &textWindowStyleCounter, &textWindowStyleSection,
         pageBreak](FormattedTextCtrl* textWindow, const bool includeLeadingPageBreak)
    {
        if (!textWindow)
            {
            return;
            }
        std::wstring htmlText =
            textWindow->GetUnthemedFormattedTextHtml(wxString(textWindowStyleCounter++))
                .wc_string();
        textWindowStyleSection += L"\n" + wxString(html_extract_text::get_style_section(htmlText));
        htmlText = html_extract_text::get_body(htmlText);

        outputText += wxString::Format(
            L"\n%s<div class='caption'>%s</div>\n<div class='text-report-body'>%s</div>\n",
            (includeLeadingPageBreak ? pageBreak : wxString{}),
            htmlEncode({ textWindow->GetLabel().wc_str() }, true).c_str(), htmlText);
    };

    const auto formatHTMLReport = [&outputText, &htmlEncode, pageBreak](
                                      HtmlTableWindow* html, const bool includeLeadingPageBreak)
    {
        if (!html)
            {
            return;
            }
        std::wstring htmlText = (html->GetParser()->GetSource())->wc_string();
        lily_of_the_valley::html_format::strip_hyperlinks(htmlText);
        htmlText = html_extract_text::get_body(htmlText);
        outputText +=
            wxString::Format(L"\n%s<div class='caption'>%s</div>\n%s\n",
                             (includeLeadingPageBreak ? pageBreak : wxString{}),
                             htmlEncode({ html->GetName().wc_str() }, true).c_str(), htmlText);
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
                if (activeWindow->IsKindOf(CLASSINFO(ExplanationListCtrl)))
                    {
                    formatList(
                        dynamic_cast<ExplanationListCtrl*>(activeWindow)->GetResultsListCtrl(),
                        includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                else if (activeWindow->IsKindOf(CLASSINFO(Wisteria::Canvas)))
                    {
                    formatImageOutput(dynamic_cast<Wisteria::Canvas*>(activeWindow),
                                      includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                else if (activeWindow->IsKindOf(CLASSINFO(HtmlTableWindow)))
                    {
                    formatHTMLReport(dynamic_cast<HtmlTableWindow*>(activeWindow),
                                     includeLeadingPageBreak);
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
        outputText +=
            wxString::Format(L"\n\n%s<div class='report-section'><a name='stats'></a>%s</div>\n",
                             (hasSections ? pageBreak : wxString{}),
                             htmlEncode({ GetSummaryStatisticsLabel().wc_str() }, true).c_str());
        hasSections = true;
        for (size_t i = 0; i < GetSummaryView().GetWindowCount(); ++i)
            {
            wxWindow* activeWindow = GetSummaryView().GetWindow(i);
            if (activeWindow)
                {
                if (activeWindow->IsKindOf(CLASSINFO(HtmlTableWindow)))
                    {
                    formatHTMLReport(dynamic_cast<HtmlTableWindow*>(activeWindow),
                                     includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                else if (activeWindow->IsKindOf(CLASSINFO(ListCtrlEx)) && includeLists)
                    {
                    formatList(dynamic_cast<ListCtrlEx*>(activeWindow), includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                else if (activeWindow->IsKindOf(CLASSINFO(Wisteria::Canvas)))
                    {
                    formatImageOutput(dynamic_cast<Wisteria::Canvas*>(activeWindow),
                                      includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                }
            }
        }
    // words breakdown section
    if (includeWordsBreakdown && GetWordsBreakdownView().GetWindowCount())
        {
        bool includeLeadingPageBreak{ false };
        ++sectionCounter;
        figureCounter = tableCounter = 1;
        outputText += wxString::Format(
            L"\n\n%s<div class='report-section'><a name='wordsbreakdown'></a>%s</div>\n",
            (hasSections ? pageBreak : wxString{}),
            htmlEncode({ GetWordsBreakdownLabel().wc_str() }, true).c_str());
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
                    formatImageOutput(dynamic_cast<Wisteria::Canvas*>(activeWindow),
                                      includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                else if (activeWindow->IsKindOf(CLASSINFO(FormattedTextCtrl)) && includeTextReports)
                    {
                    formatTextWindow(dynamic_cast<FormattedTextCtrl*>(activeWindow),
                                     includeLeadingPageBreak);
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
                else if (activeWindow->IsKindOf(CLASSINFO(Wisteria::Canvas)))
                    {
                    formatImageOutput(dynamic_cast<Wisteria::Canvas*>(activeWindow),
                                      includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                }
            }
        }
    // grammar section
    if (includeGrammar && (includeLists || includeTextReports) && GetGrammarView().GetWindowCount())
        {
        bool includeLeadingPageBreak{ false };
        ++sectionCounter;
        figureCounter = tableCounter = 1;
        outputText +=
            wxString::Format(L"\n\n%s<div class='report-section'><a name='grammar'></a>%s</div>\n",
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
                    formatTextWindow(dynamic_cast<FormattedTextCtrl*>(activeWindow),
                                     includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                }
            }
        }
    // Sight Words
    if (includeSightWords && GetDolchSightWordsView().GetWindowCount())
        {
        bool includeLeadingPageBreak{ false };
        ++sectionCounter;
        figureCounter = tableCounter = 1;
        outputText +=
            wxString::Format(L"\n\n%s<div class='report-section'><a name='dolch'></a>%s</div>\n",
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
                else if (activeWindow->IsKindOf(CLASSINFO(HtmlTableWindow)))
                    {
                    formatHTMLReport(dynamic_cast<HtmlTableWindow*>(activeWindow),
                                     includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                else if (activeWindow->IsKindOf(CLASSINFO(FormattedTextCtrl)) && includeTextReports)
                    {
                    formatTextWindow(dynamic_cast<FormattedTextCtrl*>(activeWindow),
                                     includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                else if (typeid(*activeWindow) == typeid(Wisteria::Canvas))
                    {
                    formatImageOutput(dynamic_cast<Wisteria::Canvas*>(activeWindow),
                                      includeLeadingPageBreak);
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
        _(L"Project Title"), doc->GetTitle(), _(L"Status"), doc->GetStatus(), _(L"Reviewer"),
        doc->GetReviewer(), _(L"Date"), wxDateTime().Now().FormatDate());

    if (includeTestScores && GetReadabilityResultsView().GetWindowCount())
        {
        TOC += L"<a href='#scores'>" + GetReadabilityScoresLabel() + L"</a><br />\n";
        }
    if (includeStatistics && GetSummaryView().GetWindowCount())
        {
        TOC += L"<a href='#stats'>" + GetSummaryStatisticsLabel() + L"</a><br />\n";
        }
    if (includeWordsBreakdown && GetWordsBreakdownView().GetWindowCount())
        {
        TOC += L"<a href='#wordsbreakdown'>" + GetWordsBreakdownLabel() + L"</a><br />\n";
        }
    if (includeSentencesBreakdown && GetSentencesBreakdownView().GetWindowCount())
        {
        TOC += L"<a href='#sentencesbreakdown'>" + GetSentencesBreakdownLabel() + L"</a><br />\n";
        }
    // grammar section only has text and list windows, so don't include that if
    // not including those types of windows
    if (includeGrammar && (includeLists || includeTextReports) && GetGrammarView().GetWindowCount())
        {
        TOC += L"<a href='#grammar'>" + GetGrammarLabel() + L"</a><br />\n";
        }
    if (includeSightWords && GetDolchSightWordsView().GetWindowCount())
        {
        TOC += L"<a href='#dolch'>" + GetDolchLabel() + L"</a><br />\n";
        }
    outputText.insert(0, L"<!DOCTYPE html>\n<html>\n" + headSection + L"\n<body>\n" + infoTable +
                             L"\n<div class='toc-section no-print'>" + TOC + L"</div>");
    outputText += L"\n</body>\n</html>";

    // copy over the CSS file
    const wxString cssTemplatePath = wxGetApp().FindResourceDirectory(_DT(L"report-themes")) +
                                     wxFileName::GetPathSeparator() + L"default.css";
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
                {
                cssFile.Write(textWindowStyleSection);
                }
            }
        }

    wxFileName(filePath.GetFullPath()).SetPermissions(wxS_DEFAULT);
    wxFile file(filePath.GetFullPath(), wxFile::write);
    return file.Write(outputText);
    }
