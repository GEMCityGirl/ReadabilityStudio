/********************************************************************************
 * Copyright (c) 2005-2025 Blake Madden
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * https://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   Blake Madden - initial implementation
 ********************************************************************************/

#include "batch_project_doc.h"
#include "../Wisteria-Dataviz/src/base/reportenumconvert.h"
#include "../Wisteria-Dataviz/src/graphs/wordcloud.h"
#include "../Wisteria-Dataviz/src/util/string_util.h"
#include "../app/readability_app.h"
#include "../indexing/character_traits.h"
#include "../results-format/project_report_format.h"
#include "../ui/dialogs/project_wizard_dlg.h"
#include "batch_project_view.h"
#include <limits>

using namespace Wisteria;
using namespace Wisteria::Graphs;
using namespace Wisteria::GraphItems;
using namespace Wisteria::Colors;

wxDECLARE_APP(ReadabilityApp);

wxIMPLEMENT_DYNAMIC_CLASS(BatchProjectDoc, wxDocument)

//-------------------------------------------------------
void BatchProjectDoc::ShowQueuedMessages()
    {
    BaseProjectView* view = dynamic_cast<BaseProjectView*>(GetFirstView());
    for (auto queuedMsgIter = GetQueuedMessages().cbegin();
         queuedMsgIter != GetQueuedMessages().cend(); ++queuedMsgIter)
        {
        view->ShowInfoMessage(*queuedMsgIter);
        }
    }

//-------------------------------------------------------
void BatchProjectDoc::RemoveMisspellings(const wxArrayString& misspellingsToRemove)
    {
    traits::case_insensitive_wstring_ex reportStr;
    wxString searchStr;
    wxString multFactorValue;
    for (size_t i = 0; i < GetMisspelledWordData()->GetItemCount(); ++i)
        {
        double totalCount = GetMisspelledWordData()->GetItemValue(i, 2);
        double uniqueCount = GetMisspelledWordData()->GetItemValue(i, 3);
        reportStr = GetMisspelledWordData()->GetItemText(i, 4);
        for (size_t mspCounter = 0; mspCounter < misspellingsToRemove.GetCount(); ++mspCounter)
            {
            searchStr = L"\"" + misspellingsToRemove[mspCounter] + L"\"";
            size_t index = reportStr.find(searchStr);
            if (index != wxString::npos)
                {
                size_t endIndex = reportStr.find(L",", index + searchStr.length());
                if (endIndex != wxString::npos)
                    {
                    size_t multFactorIndex = reportStr.find(L"*", index + searchStr.length());
                    if (multFactorIndex != wxString::npos && multFactorIndex < endIndex)
                        {
                        // skip "* "
                        multFactorIndex += 2;
                        multFactorValue =
                            reportStr.substr(multFactorIndex, endIndex - multFactorIndex).c_str();
                        double val{ 0 };
                        if (multFactorValue.ToDouble(&val))
                            {
                            assert(val > 0);
                            totalCount -= (val - 1 /* we will subtract 1 later*/);
                            }
                        }
                    // skip trailing ", "
                    endIndex += 2;
                    reportStr.erase(index, endIndex - index);
                    }
                else
                    {
                    // we will need to strip the trailing ", " after removing this word at the end
                    if (index >= 2)
                        {
                        index -= 2;
                        }
                    size_t multFactorIndex = reportStr.find(L"*", index + searchStr.length());
                    if (multFactorIndex != wxString::npos)
                        {
                        // skip "* "
                        multFactorIndex += 2;
                        multFactorValue = reportStr.substr(multFactorIndex).c_str();
                        double val{ 0 };
                        if (multFactorValue.ToDouble(&val))
                            {
                            assert(val > 0);
                            totalCount -= (val - 1 /* we will subtract 1 later*/);
                            }
                        }
                    reportStr.erase(index);
                    }
                --uniqueCount;
                --totalCount;
                }
            }
        GetMisspelledWordData()->SetItemValue(i, 2, totalCount);
        GetMisspelledWordData()->SetItemValue(i, 3, uniqueCount);
        GetMisspelledWordData()->SetItemText(i, 4, reportStr.c_str());
        }
    // remove any blank rows
    for (size_t i = 0; i < GetMisspelledWordData()->GetItemCount(); /* handled in loop*/)
        {
        if (GetMisspelledWordData()->GetItemValue(i, 2) == 0)
            {
            // cppcheck-suppress assertWithSideEffect
            assert(GetMisspelledWordData()->GetItemValue(i, 3) == 0);
            // cppcheck-suppress assertWithSideEffect
            wxASSERT_LEVEL_2_MSG(GetMisspelledWordData()->GetItemText(i, 4).empty(),
                                 GetMisspelledWordData()->GetItemText(i, 4));
            GetMisspelledWordData()->DeleteItem(i);
            }
        else
            {
            ++i;
            }
        }
    BatchProjectView* view = dynamic_cast<BatchProjectView*>(GetFirstView());
    Wisteria::UI::ListCtrlEx* listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
        view->GetGrammarView().FindWindowById(BaseProjectView::MISSPELLED_WORD_LIST_PAGE_ID));
    if (listView)
        {
        if (GetMisspelledWordData()->GetItemCount() == 0)
            {
            view->GetGrammarView().RemoveWindowById(BaseProjectView::MISSPELLED_WORD_LIST_PAGE_ID);
            view->UpdateSideBarIcons();
            view->GetSideBar()->SelectFolder(0);
            }
        else
            {
            listView->SetVirtualDataSize(GetMisspelledWordData()->GetItemCount());
            if (listView->GetSortedColumn() == -1)
                {
                listView->SortColumn(0, Wisteria::SortDirection::SortAscending);
                }
            else
                {
                listView->Resort();
                }
            }
        }
    }

//-------------------------------------------------------
bool BatchProjectDoc::OnCreate(const wxString& path, long flags)
    {
    // see if anything is even licensed first
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureProfessionalCode()))
        {
        LogMessage(_(L"You are not currently licensed to create a new batch project."), _(L"Error"),
                   wxOK | wxICON_ERROR);
        return false;
        }
    if (flags & wxDOC_NEW)
        {
        // if a folder, load the all supported document types recursively
        if (!path.empty() && wxFileName{ path }.IsDir() && wxFileName::DirExists(path))
            {
            wxArrayString files;
                {
                wxWindowDisabler disableAll;
                wxBusyInfo wait(wxBusyInfoFlags().Text(_(L"Retrieving files..."))
                                                 .Parent(wxGetApp().GetParentingWindow()));
#ifdef __WXGTK__
                wxMilliSleep(100);
                wxTheApp->Yield();
#endif
                wxDir::GetAllFiles(path, &files, wxString{},
                                   wxDIR_FILES | wxDIR_DIRS);
                files = FilterFiles(files,
                    ExtractExtensionsFromFileFilter(ReadabilityAppOptions::GetDocumentFilter()));
                }
            GetSourceFilesInfo().clear();
            GetSourceFilesInfo().reserve(files.size());
            for (const auto& fl : files)
                {
                wxLogMessage(fl);
                GetSourceFilesInfo().push_back(comparable_first_pair{ fl, wxString{} });
                }
            ProjectWizardDlg::SetLastSelectedFolder(path);
            return wxDocument::OnCreate(wxString{}, flags);
            }
        // if passed a single, "regular" file (i.e., not an archive or spreadsheet), then just load
        // it with the defaults and bypass the wizard.
        else if (!path.empty() && FilePathResolver(path, false).IsLocalOrNetworkFile() &&
            !FilePathResolver::IsSpreadsheet(path) && !FilePathResolver::IsArchive(path))
            {
            GetSourceFilesInfo().clear();
            GetSourceFilesInfo().push_back(comparable_first_pair{ path, wxString{} });

            const wxArrayString folders = wxFileName(wxFileName(path).GetPathWithSep()).GetDirs();
            ProjectWizardDlg::SetLastSelectedFolder(folders.size() ? folders.back() : wxString{});
            return wxDocument::OnCreate(wxString{}, flags);
            }
        // scripting framework passes this in to create an empty project
        // that can have files added later
        else if (path == L"EMPTY_PROJECT")
            {
            return wxDocument::OnCreate(wxString{}, flags);
            }
        else if (!RunProjectWizard(path))
            {
            return false;
            }
        }
    return wxDocument::OnCreate(path, flags);
    }

//-------------------------------------------------------
bool BatchProjectDoc::OnNewDocument()
    {
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureProfessionalCode()))
        { return false; }
    if (!wxDocument::OnNewDocument() )
        { return false; }

    BaseProjectProcessingLock processingLock(this);

    // load the images now
    SetPlotBackGroundImagePath(GetPlotBackGroundImagePath());
    SetStippleImagePath(GetStippleImagePath());
    SetWatermarkLogoPath(GetWatermarkLogoPath());
    SetGraphCommonImagePath(GetGraphCommonImagePath());

    LoadExcludePhrases();

    // load appended template file (if there is one)
    LoadAppendedDocument();

    wxProgressDialog progressDlg(
        _(L"Creating Project"),
        wxString::Format(
            _(L"Analyzing %s documents..."),
            wxNumberFormatter::ToString(GetSourceFilesInfo().size(), 0,
                                        wxNumberFormatter::Style::Style_WithThousandsSep)),
        static_cast<int>(GetSourceFilesInfo().size() + 13), nullptr,
        wxPD_AUTO_HIDE | wxPD_SMOOTH | wxPD_ELAPSED_TIME | wxPD_CAN_ABORT | wxPD_APP_MODAL);
    progressDlg.Centre();
    int counter{ 1 };

    InitializeDocuments();

    if (!progressDlg.Update(counter++))
        { return false; }
    if (!LoadDocuments(progressDlg))
        { return false; }
    counter = progressDlg.GetValue();

    LoadGroupingLabelsFromDocumentsInfo();

    // prompt user about removing any failed documents.
    // If they request to leave them in, then load any warnings for all documents.
    if (!CheckForFailedDocuments())
        { LoadWarningsSection(); }

    if (!progressDlg.Update(counter++, _(L"Loading Dolch statistics...")))
        { return false; }
    LoadDolchSection();

    if (!progressDlg.Update(counter++, _(L"Loading difficult words...")))
        { return false; }
    LoadHardWordsSection();

    if (!progressDlg.Update(counter++, _(L"Loading graphs...")))
        { return false; }
    // needs to be called before LoadScores to calculate Fry and Raygor
    DisplayReadabilityGraphs();

    if (!progressDlg.Update(counter++, _(L"Loading scores...")))
        { return false; }
    LoadScoresSection();

    if (!progressDlg.Update(counter++, _(L"Loading summary statistics...")))
        { return false; }
    LoadSummaryStatsSection();

    if (!progressDlg.Update(counter++))
        { return false; }
    DisplayScores();

    if (!progressDlg.Update(counter++))
        { return false; }
    DisplayBoxPlots();

    if (!progressDlg.Update(counter++))
        { return false; }
    DisplayHistograms();

    if (!progressDlg.Update(counter++, _(L"Loading grammar information...")))
        { return false; }
    DisplayGrammar();

    if (!progressDlg.Update(counter++))
        { return false; }
    DisplayHardWords();
    DisplaySentencesBreakdown();
    DisplaySummaryStats();

    if (!progressDlg.Update(counter++))
        { return false; }
    DisplaySightWords();

    if (!progressDlg.Update(counter++))
        { return false; }
    DisplayWarnings();

    BatchProjectView* view = dynamic_cast<BatchProjectView*>(GetFirstView());
    view->UpdateSideBarIcons();
    view->UpdateRibbonState();
    view->Present();
    UpdateAllViews();

    view->GetSideBar()->SelectSubItem(
        view->GetSideBar()->FindSubItem(BaseProjectView::ID_SCORE_LIST_PAGE_ID));

    auto* scoresWindow = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
        view->GetScoresView().FindWindowById(BaseProjectView::ID_SCORE_LIST_PAGE_ID));
    if (scoresWindow != nullptr && scoresWindow->GetItemCount() > 0)
        {
        scoresWindow->Select(0);
        }

    // try to base the default name of this project from the folder/web domain of the first file
    if (GetSourceFilesInfo().size() > 0)
        {
        FilePathResolver resolvePath(GetOriginalDocumentFilePath(0), false);
        // if a local file, use the name of the last folder in the path as the project name
        if (resolvePath.IsLocalOrNetworkFile())
            {
            const wxArrayString dirs = wxFileName::DirName(ProjectWizardDlg::GetLastSelectedFolder()).GetDirs();
            if (dirs.size())
                {
                SetTitle(dirs.back());
                SetFilename(dirs.back(), true);
                }
            }
        else if (resolvePath.IsExcelCell() || resolvePath.IsArchivedFile())
            {
            const size_t subDocStart = resolvePath.GetResolvedPath().rfind(L"#");
            if (subDocStart != wxString::npos)
                {
                const wxFileName fn(resolvePath.GetResolvedPath().substr(0, subDocStart));
                SetTitle(fn.GetName());
                SetFilename(fn.GetName(), true);
                }
            }
        else if (resolvePath.IsWebFile())
            {
            const html_utilities::html_url_format hformat(resolvePath.GetResolvedPath().wc_str());
            wxString domain = hformat.get_root_domain().c_str();
            const auto dotPos = domain.find(L'.', true);
            if (dotPos != wxString::npos)
                { domain.Truncate(dotPos); }
            SetTitle(domain);
            SetFilename(domain, true);
            }
        }

    Modify(true);

    return true;
    }

//------------------------------------------------
void BatchProjectDoc::InitializeDocuments()
    {
    PROFILE();
    for (std::vector<BaseProject*>::iterator pos = m_docs.begin();
        pos != m_docs.end();
        ++pos)
        { wxDELETE(*pos); }
    // should certainly be the case
    if (GetSourceFilesInfo().size() > 0)
        { m_docs.resize(GetSourceFilesInfo().size(), nullptr); }
    for (size_t i = 0; i < GetSourceFilesInfo().size(); ++i)
        {
        m_docs[i] = new BaseProject();
        m_docs[i]->CopySettings(*this);
        m_docs[i]->SetAppendedDocumentText(GetAppendedDocumentText());
        m_docs[i]->ShareExcludePhrases(*this);
        m_docs[i]->SetUIMode(false);
        m_docs[i]->GetSourceFilesInfo().clear();
        m_docs[i]->GetSourceFilesInfo().push_back(GetSourceFilesInfo().at(i));
        }
    }

//------------------------------------------------
void BatchProjectDoc::LoadGroupingLabelsFromDocumentsInfo()
    {
    m_docLabels.clear();
    m_groupStringTable.clear();
    for (const auto doc : m_docs)
        {
        if (doc->LoadingOriginalTextSucceeded())
            {
            auto [item, inserted] = m_docLabels.try_emplace(doc->GetOriginalDocumentDescription().wc_str(), 0);
            // add a unique group ID to the label
            if (inserted)
                { item->second = m_docLabels.size()-1; }
            // if too many labels, then this probably isn't grouped data, so don't use grouping
            if (GetDocumentLabels().size() > GetMaxGroupCount())
                {
                m_docLabels.clear();
                break;
                }
            }
        }
    // build a string table for the graphs' datasets
    for (auto& [key, value] : m_docLabels)
        {
        m_groupStringTable.insert(std::make_pair(value, key.c_str()));
        }
    }

//------------------------------------------------
bool BatchProjectDoc::CheckForFailedDocuments()
    {
    wxArrayString failedDocs;
    for (std::vector<BaseProject*>::iterator pos = m_docs.begin(); pos != m_docs.end(); ++pos)
        {
        if (!(*pos)->LoadingOriginalTextSucceeded())
            {
            failedDocs.Add((*pos)->GetOriginalDocumentFilePath());
            }
        }

    BatchProjectView* view = dynamic_cast<BatchProjectView*>(GetFirstView());
    wxASSERT_MSG(view->GetFrame(), L"Invalid frame for newly created document!");
    // show the names of the failed documents somehow so the user can review it before removing them
    Wisteria::UI::ListDlg listDlg(
        view->GetFrame(), failedDocs, false,
        wxGetApp().GetAppOptions().GetRibbonActiveTabColor(),
        wxGetApp().GetAppOptions().GetRibbonHoverColor(),
        wxGetApp().GetAppOptions().GetRibbonActiveFontColor(), Wisteria::UI::LD_YES_NO_BUTTONS,
        wxID_ANY, _(L"Warning"),
        _(L"The following documents could not be loaded because they either do not contain "
          "enough valid text or could not be found. Do you wish to remove these documents "
          "from this project?"));
    if (failedDocs.GetCount() &&
        listDlg.ShowModal() == wxID_YES)
        {
        RemoveFailedDocuments();
        return true;
        }
    // user choose to leave failed documents in the project
    return false;
    }

//------------------------------------------------
void BatchProjectDoc::RemoveFailedDocuments()
    {
    GetSourceFilesInfo().clear();
    GetSourceFilesInfo().reserve(m_docs.size());
    for (std::vector<BaseProject*>::iterator pos = m_docs.begin();
        pos != m_docs.end();
        /* handled in loop*/)
        {
        if ((*pos)->LoadingOriginalTextSucceeded())
            {
            GetSourceFilesInfo().push_back((*pos)->GetSourceFilesInfo().at(0));
            ++pos;
            }
        else
            {
            wxDELETE(*pos);
            pos = m_docs.erase(pos);
            }
        }
    // reload the warnings here because we have thrown out the failed docs and
    // no point in showing their warnings anymore.
    LoadWarningsSection();

    Modify(true);
    }

//------------------------------------------------
void BatchProjectDoc::RefreshStatisticsReports()
    {
    if (!IsSafeToUpdate())
        {
        return;
        }

    // if refresh is not necessary then return
    if (IsRefreshRequired() == false)
        {
        return;
        }

    BatchProjectView* view = dynamic_cast<BatchProjectView*>(GetFirstView());
    const wxString currentlySelectedFile = view->GetCurrentlySelectedFileName();
    const auto selectedItem = view->GetSideBar()->GetSelectedFolderId();

    BaseProjectProcessingLock processingLock(this);
    wxWindowUpdateLocker noUpdates(GetDocumentWindow());

    LoadSummaryStatsSection();
    DisplaySummaryStats();

    view->UpdateSideBarIcons();
    view->UpdateRibbonState();
    view->Present();
    UpdateAllViews();

    auto selectedIndex = view->GetSideBar()->FindFolder(selectedItem);
    if (!selectedIndex.has_value())
        {
        selectedIndex = view->GetSideBar()->
            FindFolder(BaseProjectView::SIDEBAR_READABILITY_SCORES_SECTION_ID);
        }
    view->GetSideBar()->SelectFolder(selectedIndex, false);

    view->UpdateStatAndTestPanes(currentlySelectedFile);

    Modify(true);
    GetDocumentWindow()->Refresh();
    ResetRefreshRequired();
    }

//------------------------------------------------
void BatchProjectDoc::RefreshGraphs()
    {
    if (!IsSafeToUpdate())
        { return; }

    // if refresh is not necessary then return
    if (IsRefreshRequired() == false)
        { return; }
    BaseProjectProcessingLock processingLock(this);
    wxWindowUpdateLocker noUpdates(GetDocumentWindow());

    DisplayReadabilityGraphs();
    DisplayBoxPlots();
    DisplayHistograms();

    Modify(true);
    GetDocumentWindow()->Refresh();
    ResetRefreshRequired();
    }

//------------------------------------------------
void BatchProjectDoc::RefreshProject()
    {
    if (!IsSafeToUpdate())
        {
        return;
        }
    wxBusyCursor wait;

    // if refresh is not necessary then return
    if (IsRefreshRequired() == false)
        {
        return;
        }
    BaseProjectProcessingLock processingLock(this);
    wxWindowUpdateLocker noUpdates(GetDocumentWindow());

    // reload the excluded phrases
    LoadExcludePhrases();

    // load appended template file (if there is one)
    LoadAppendedDocument();

    BatchProjectView* view = dynamic_cast<BatchProjectView*>(GetFirstView());
    wxProgressDialog progressDlg(wxString::Format(_(L"Reloading \"%s\""), GetTitle()),
        _(L"Analyzing documents..."),
        IsDocumentReindexingRequired() ? static_cast<int>(GetSourceFilesInfo().size()+13) : 13,
        view->GetFrame(), wxPD_AUTO_HIDE | wxPD_SMOOTH | wxPD_ELAPSED_TIME | wxPD_APP_MODAL);
    progressDlg.Centre();
    int counter{ 1 };

    progressDlg.Update(counter++);

    if (IsDocumentReindexingRequired() && GetDocumentStorageMethod() == TextStorage::NoEmbedText)
        { InitializeDocuments(); }

    // reset all the sub documents so that they have the proper settings and tests included
    for (size_t i = 0; i < m_docs.size(); ++i)
        {
        // CopySettings will clear the embedded text, so back it up and then swap it back in
        std::wstring embeddedText = std::move(m_docs[i]->GetDocumentText());
        m_docs[i]->CopySettings(*this);
        m_docs[i]->SetDocumentText(std::move(embeddedText));
        m_docs[i]->SetAppendedDocumentText(GetAppendedDocumentText());
        m_docs[i]->ShareExcludePhrases(*this);
        m_docs[i]->SetUIMode(false);
        }
    if (IsDocumentReindexingRequired())
        { LoadDocuments(progressDlg); }
    counter = progressDlg.GetValue();

    LoadGroupingLabelsFromDocumentsInfo();

    // prompt user about removing any failed documents.
    // If they request to leave them in, then load any warnings for all documents.
    if (!CheckForFailedDocuments())
        { LoadWarningsSection(); }

    // get this before list controls are recreated
    const wxString currentlySelectedFile = view->GetCurrentlySelectedFileName();

    progressDlg.Update(counter++, _(L"Loading Dolch statistics..."));
    LoadDolchSection();

    progressDlg.Update(counter++, _(L"Loading difficult words..."));
    LoadHardWordsSection();

    progressDlg.Update(counter++, _(L"Loading graphs..."));
    // needs to be called before LoadScores to calculate Fry and Raygor
    DisplayReadabilityGraphs();

    progressDlg.Update(counter++, _(L"Loading scores..."));
    LoadScoresSection();

    progressDlg.Update(counter++, _(L"Loading summary statistics..."));
    LoadSummaryStatsSection();

    progressDlg.Update(counter++);
    DisplayScores();

    progressDlg.Update(counter++);
    DisplayBoxPlots();

    progressDlg.Update(counter++);
    DisplayHistograms();

    progressDlg.Update(counter++, _(L"Loading grammar information..."));
    DisplayGrammar();

    progressDlg.Update(counter++);
    DisplayHardWords();
    DisplaySentencesBreakdown();
    DisplaySummaryStats();

    progressDlg.Update(counter++);
    DisplaySightWords();

    progressDlg.Update(counter++);
    DisplayWarnings();

    const auto selectedItem = view->GetSideBar()->GetSelectedSubItemId();
    const auto selectedFolder = view->GetSideBar()->GetSelectedFolderId();
    view->UpdateSideBarIcons();
    view->UpdateRibbonState();
    view->Present();
    UpdateAllViews();

    if (!view->GetSideBar()->SelectSubItemById(selectedItem, true, true))
        {
        // fall back to folder that may not have subitems (e.g., the Warning section),
        // and then the score section if the folder isn't there anymore.
        if (!view->GetSideBar()->SelectFolder(selectedFolder.value_or(0), true, true))
            {
            view->GetSideBar()->SelectFolder(0, true, true);
            }
        }
    view->ShowSideBar(view->IsSideBarShown());
    auto* scoresWindow = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
        view->GetScoresView().FindWindowById(BaseProjectView::ID_SCORE_LIST_PAGE_ID));
    if (scoresWindow != nullptr && scoresWindow->GetItemCount() > 0)
        {
        scoresWindow->Select(0);
        }

    view->UpdateStatAndTestPanes(currentlySelectedFile);

    Modify(true);

    GetDocumentWindow()->Refresh();

    ResetRefreshRequired();
    }

//------------------------------------------------------------
void BatchProjectDoc::LoadDolchSection()
    {
    PROFILE();
    m_dolchCompletionData->DeleteAllItems();
    m_dolchCompletionData->SetSize(m_docs.size(), 9);
    m_dolchWordsBatchData->DeleteAllItems();
    m_dolchWordsBatchData->SetSize(m_docs.size(), (GetStatisticsReportInfo().IsExtendedInformationEnabled()) ? 16 : 9);
    m_NonDolchWordsData->DeleteAllItems();
    m_NonDolchWordsData->SetSize(m_docs.size(), (GetStatisticsReportInfo().IsExtendedInformationEnabled()) ? 4 : 3);

    size_t dolchDocumentCount = 0;

    for (std::vector<BaseProject*>::iterator pos = m_docs.begin();
        pos != m_docs.end();
        ++pos)
        {
        // dolch words
        if ((*pos)->LoadingOriginalTextSucceeded() )
            {
            // completion stats
            m_dolchCompletionData->SetItemText(dolchDocumentCount, 0, (*pos)->GetOriginalDocumentFilePath());
            m_dolchCompletionData->SetItemText(dolchDocumentCount, 1, (*pos)->GetOriginalDocumentDescription());
            const double dolchConjunctionPercentage =
                safe_divide<double>(
                    (ProjectReportFormat::MAX_DOLCH_CONJUNCTION_WORDS-(*pos)->GetUnusedDolchConjunctions()),
                     ProjectReportFormat::MAX_DOLCH_CONJUNCTION_WORDS)*100;
            const double dolchPrepositionsPercentage =
                safe_divide<double>(
                    (ProjectReportFormat::MAX_DOLCH_PREPOSITION_WORDS-(*pos)->GetUnusedDolchPrepositions()),
                     ProjectReportFormat::MAX_DOLCH_PREPOSITION_WORDS)*100;
            const double dolchPronounsPercentage =
                safe_divide<double>(
                    (ProjectReportFormat::MAX_DOLCH_PRONOUN_WORDS-(*pos)->GetUnusedDolchPronouns()),
                     ProjectReportFormat::MAX_DOLCH_PRONOUN_WORDS)*100;
            const double dolchAdverbsPercentage =
                safe_divide<double>(
                    (ProjectReportFormat::MAX_DOLCH_ADVERB_WORDS-(*pos)->GetUnusedDolchAdverbs()),
                     ProjectReportFormat::MAX_DOLCH_ADVERB_WORDS)*100;
            const double dolchAdjectivesPercentage =
                safe_divide<double>(
                    (ProjectReportFormat::MAX_DOLCH_ADJECTIVE_WORDS-(*pos)->GetUnusedDolchAdjectives()),
                     ProjectReportFormat::MAX_DOLCH_ADJECTIVE_WORDS)*100;
            const double dolchVerbsPercentage =
                safe_divide<double>(
                    (ProjectReportFormat::MAX_DOLCH_VERBS-(*pos)->GetUnusedDolchVerbs()),
                     ProjectReportFormat::MAX_DOLCH_VERBS)*100;
            const double dolchNounPercentage =
                safe_divide<double>(
                    (ProjectReportFormat::MAX_DOLCH_NOUNS-(*pos)->GetUnusedDolchNouns()),
                     ProjectReportFormat::MAX_DOLCH_NOUNS)*100;
            m_dolchCompletionData->SetItemValue(dolchDocumentCount, 2, dolchConjunctionPercentage,
                NumberFormatInfo(NumberFormatInfo::NumberFormatType::PercentageFormatting,1,true));
            m_dolchCompletionData->SetItemValue(dolchDocumentCount, 3, dolchPrepositionsPercentage,
                NumberFormatInfo(NumberFormatInfo::NumberFormatType::PercentageFormatting,1,true));
            m_dolchCompletionData->SetItemValue(dolchDocumentCount, 4, dolchPronounsPercentage,
                NumberFormatInfo(NumberFormatInfo::NumberFormatType::PercentageFormatting,1,true));
            m_dolchCompletionData->SetItemValue(dolchDocumentCount, 5, dolchAdverbsPercentage,
                NumberFormatInfo(NumberFormatInfo::NumberFormatType::PercentageFormatting,1,true));
            m_dolchCompletionData->SetItemValue(dolchDocumentCount, 6, dolchAdjectivesPercentage,
                NumberFormatInfo(NumberFormatInfo::NumberFormatType::PercentageFormatting,1,true));
            m_dolchCompletionData->SetItemValue(dolchDocumentCount, 7, dolchVerbsPercentage,
                NumberFormatInfo(NumberFormatInfo::NumberFormatType::PercentageFormatting,1,true));
            m_dolchCompletionData->SetItemValue(dolchDocumentCount, 8, dolchNounPercentage,
                NumberFormatInfo(NumberFormatInfo::NumberFormatType::PercentageFormatting,1,true));
            // word stats
            size_t columnCount = 0;
            m_dolchWordsBatchData->SetItemText(dolchDocumentCount, columnCount++,
                (*pos)->GetOriginalDocumentFilePath());
            m_dolchWordsBatchData->SetItemText(dolchDocumentCount, columnCount++,
                (*pos)->GetOriginalDocumentDescription());
            m_dolchWordsBatchData->SetItemValue(dolchDocumentCount, columnCount++,
                        safe_divide<double>((*pos)->GetDolchConjunctionCounts().second,(*pos)->GetTotalWords())*100,
                        NumberFormatInfo(NumberFormatInfo::NumberFormatType::PercentageFormatting, 1, true));
            if (GetStatisticsReportInfo().IsExtendedInformationEnabled())
                {
                m_dolchWordsBatchData->SetItemValue(dolchDocumentCount, columnCount++,
                        (*pos)->GetDolchConjunctionCounts().second,
                        NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 0, true));
                }

            m_dolchWordsBatchData->SetItemValue(dolchDocumentCount, columnCount++,
                        safe_divide<double>((*pos)->GetDolchPrepositionWordCounts().second,
                                            (*pos)->GetTotalWords())*100,
                        NumberFormatInfo(NumberFormatInfo::NumberFormatType::PercentageFormatting, 1, true));
            if (GetStatisticsReportInfo().IsExtendedInformationEnabled())
                {
                m_dolchWordsBatchData->SetItemValue(dolchDocumentCount, columnCount++,
                        (*pos)->GetDolchPrepositionWordCounts().second,
                        NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 0, true));
                }

            m_dolchWordsBatchData->SetItemValue(dolchDocumentCount, columnCount++,
                        safe_divide<double>((*pos)->GetDolchPronounCounts().second,(*pos)->GetTotalWords())*100,
                        NumberFormatInfo(NumberFormatInfo::NumberFormatType::PercentageFormatting, 1, true));
            if (GetStatisticsReportInfo().IsExtendedInformationEnabled())
                {
                m_dolchWordsBatchData->SetItemValue(dolchDocumentCount, columnCount++,
                        (*pos)->GetDolchPronounCounts().second,
                        NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 0, true));
                }

            m_dolchWordsBatchData->SetItemValue(dolchDocumentCount, columnCount++,
                        safe_divide<double>((*pos)->GetDolchAdverbCounts().second,(*pos)->GetTotalWords())*100,
                        NumberFormatInfo(NumberFormatInfo::NumberFormatType::PercentageFormatting, 1, true));
            if (GetStatisticsReportInfo().IsExtendedInformationEnabled())
                {
                m_dolchWordsBatchData->SetItemValue(dolchDocumentCount, columnCount++,
                        (*pos)->GetDolchAdverbCounts().second,
                        NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 0, true));
                }

            m_dolchWordsBatchData->SetItemValue(dolchDocumentCount, columnCount++,
                        safe_divide<double>((*pos)->GetDolchAdjectiveCounts().second,(*pos)->GetTotalWords())*100,
                        NumberFormatInfo(NumberFormatInfo::NumberFormatType::PercentageFormatting, 1, true));
            if (GetStatisticsReportInfo().IsExtendedInformationEnabled())
                {
                m_dolchWordsBatchData->SetItemValue(dolchDocumentCount, columnCount++,
                        (*pos)->GetDolchAdjectiveCounts().second,
                        NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 0, true));
                }

            m_dolchWordsBatchData->SetItemValue(dolchDocumentCount, columnCount++,
                        safe_divide<double>((*pos)->GetDolchVerbsCounts().second,(*pos)->GetTotalWords())*100,
                        NumberFormatInfo(NumberFormatInfo::NumberFormatType::PercentageFormatting, 1, true));
            if (GetStatisticsReportInfo().IsExtendedInformationEnabled())
                {
                m_dolchWordsBatchData->SetItemValue(dolchDocumentCount, columnCount++,
                        (*pos)->GetDolchVerbsCounts().second,
                        NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 0, true));
                }

            m_dolchWordsBatchData->SetItemValue(dolchDocumentCount, columnCount++,
                        safe_divide<double>((*pos)->GetDolchNounCounts().second,(*pos)->GetTotalWords())*100,
                        NumberFormatInfo(NumberFormatInfo::NumberFormatType::PercentageFormatting, 1, true));
            if (GetStatisticsReportInfo().IsExtendedInformationEnabled())
                {
                m_dolchWordsBatchData->SetItemValue(dolchDocumentCount, columnCount++,
                        (*pos)->GetDolchNounCounts().second,
                        NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 0, true));
                }

            // non-word stats
            m_NonDolchWordsData->SetItemText(dolchDocumentCount, 0, (*pos)->GetOriginalDocumentFilePath());
            m_NonDolchWordsData->SetItemText(dolchDocumentCount, 1, (*pos)->GetOriginalDocumentDescription());
            const size_t totalDolchWords =
                (*pos)->GetDolchConjunctionCounts().second + (*pos)->GetDolchPrepositionWordCounts().second +
                (*pos)->GetDolchPronounCounts().second + (*pos)->GetDolchAdverbCounts().second +
                (*pos)->GetDolchAdjectiveCounts().second +
                (*pos)->GetDolchVerbsCounts().second + (*pos)->GetDolchNounCounts().second;
            const double totalDolchPercentage = safe_divide<double>(totalDolchWords, (*pos)->GetTotalWords())*100;
            m_NonDolchWordsData->SetItemValue(dolchDocumentCount, 2,
                    100-totalDolchPercentage,
                NumberFormatInfo(NumberFormatInfo::NumberFormatType::PercentageFormatting,1,true));
            if (GetStatisticsReportInfo().IsExtendedInformationEnabled())
                {
                m_NonDolchWordsData->SetItemValue(dolchDocumentCount, 3,
                    (*pos)->GetTotalWords()-totalDolchWords,
                    NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting,0,true));
                }

            ++dolchDocumentCount;
            }
        }

    m_dolchCompletionData->SetSize(dolchDocumentCount);
    m_dolchWordsBatchData->SetSize(dolchDocumentCount);
    m_NonDolchWordsData->SetSize(dolchDocumentCount);
    }

//------------------------------------------------------------
void BatchProjectDoc::LoadHardWordsSection()
    {
    PROFILE();
    m_hardWordsData->DeleteAllItems();
    size_t extraColumnCount = 0;
    if (GetStatisticsReportInfo().IsExtendedWordsEnabled())
        {
        extraColumnCount += IsSmogLikeTestIncluded() ? 2 : 0;
        extraColumnCount += GetReadabilityTests().is_test_included(ReadabilityMessages::GUNNING_FOG()) ? 2 : 0;
        extraColumnCount += IsDaleChallLikeTestIncluded() ? 2 : 0;
        extraColumnCount += GetReadabilityTests().is_test_included(ReadabilityMessages::SPACHE()) ? 2 : 0;
        extraColumnCount += GetReadabilityTests().is_test_included(ReadabilityMessages::HARRIS_JACOBSON()) ? 2 : 0;
        extraColumnCount += GetCustTestsInUse().size()*2;
        }
    // doc name, description, overall words, complex words and %, long words and % = 7 
    m_hardWordsData->SetSize(m_docs.size(), 7 + extraColumnCount);

    size_t hardWordRowCount = 0;

    for (std::vector<BaseProject*>::iterator pos = m_docs.begin();
        pos != m_docs.end();
        ++pos)
        {
        // hard word statistics (note the ordering here must match the column ordering in DisplayHardWords())
        if ((*pos)->LoadingOriginalTextSucceeded())
            {
            size_t columnIndex = 0;
            m_hardWordsData->SetItemText(hardWordRowCount, columnIndex++, (*pos)->GetOriginalDocumentFilePath());
            m_hardWordsData->SetItemText(hardWordRowCount, columnIndex++, (*pos)->GetOriginalDocumentDescription());
            // total overall words
            m_hardWordsData->SetItemValue(hardWordRowCount, columnIndex++,
                (*pos)->GetTotalWords(),
                NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 0, true));
            // complex words
            m_hardWordsData->SetItemValue(hardWordRowCount, columnIndex++,
                safe_divide<double>((*pos)->GetTotal3PlusSyllabicWords(),(*pos)->GetTotalWords())*100,
                NumberFormatInfo(NumberFormatInfo::NumberFormatType::PercentageFormatting, 1, true));
            m_hardWordsData->SetItemValue(hardWordRowCount, columnIndex++,
                (*pos)->GetTotal3PlusSyllabicWords(),
                NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 0, true));
            // long words
            m_hardWordsData->SetItemValue(hardWordRowCount, columnIndex++,
                safe_divide<double>((*pos)->GetTotalLongWords(),(*pos)->GetTotalWords())*100,
                NumberFormatInfo(NumberFormatInfo::NumberFormatType::PercentageFormatting, 1, true));
            m_hardWordsData->SetItemValue(hardWordRowCount, columnIndex++,
                (*pos)->GetTotalLongWords(),
                NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 0, true));
            if (GetStatisticsReportInfo().IsExtendedWordsEnabled())
                {
                // hard SMOG words (numerals fully syllabized)
                if (IsSmogLikeTestIncluded())
                    {
                    m_hardWordsData->SetItemValue(hardWordRowCount, columnIndex++,
                        safe_divide<double>((*pos)->GetTotal3PlusSyllabicWordsNumeralsFullySyllabized(),
                                            (*pos)->GetTotalWords())*100,
                        NumberFormatInfo(NumberFormatInfo::NumberFormatType::PercentageFormatting, 1, true));
                    m_hardWordsData->SetItemValue(hardWordRowCount, columnIndex++,
                        (*pos)->GetTotal3PlusSyllabicWordsNumeralsFullySyllabized(),
                        NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 0, true));
                    }
                // hard FOG words
                if (GetReadabilityTests().is_test_included(ReadabilityMessages::GUNNING_FOG()) )
                    {
                    m_hardWordsData->SetItemValue(hardWordRowCount, columnIndex++,
                        safe_divide<double>((*pos)->GetTotalHardWordsFog(),(*pos)->GetTotalWords())*100,
                        NumberFormatInfo(NumberFormatInfo::NumberFormatType::PercentageFormatting, 1, true));
                    m_hardWordsData->SetItemValue(hardWordRowCount, columnIndex++,
                        (*pos)->GetTotalHardWordsFog(),
                        NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 0, true));
                    }
                // hard DC words
                const size_t totalWordCountForDC =
                    (GetDaleChallTextExclusionMode() ==
                     SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
                    (*pos)->GetTotalWordsFromCompleteSentencesAndHeaders() : (*pos)->GetTotalWords();
                const size_t totalWordCountForHJ =
                    (GetHarrisJacobsonTextExclusionMode() ==
                     SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
                    (*pos)->GetTotalWordsFromCompleteSentencesAndHeaders() : (*pos)->GetTotalWords();
                const size_t totalNumeralCountForHJ =
                    (GetHarrisJacobsonTextExclusionMode() ==
                     SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
                    (*pos)->GetTotalNumeralsFromCompleteSentencesAndHeaders() : (*pos)->GetTotalNumerals();
                // hard DC words
                if (IsDaleChallLikeTestIncluded())
                    {
                    m_hardWordsData->SetItemValue(hardWordRowCount, columnIndex++,
                        safe_divide<double>((*pos)->GetTotalHardWordsDaleChall(),totalWordCountForDC)*100,
                        NumberFormatInfo(NumberFormatInfo::NumberFormatType::PercentageFormatting, 1, true));
                    m_hardWordsData->SetItemValue(hardWordRowCount, columnIndex++,
                        (*pos)->GetTotalHardWordsDaleChall(),
                        NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 0, true));
                    }
                // hard spache words
                if (GetReadabilityTests().is_test_included(ReadabilityMessages::SPACHE()) )
                    {
                    m_hardWordsData->SetItemValue(hardWordRowCount, columnIndex++,
                        safe_divide<double>((*pos)->GetTotalHardWordsSpache(),(*pos)->GetTotalWords())*100,
                        NumberFormatInfo(NumberFormatInfo::NumberFormatType::PercentageFormatting, 1, true));
                    m_hardWordsData->SetItemValue(hardWordRowCount, columnIndex++,
                        (*pos)->GetTotalHardWordsSpache(),
                        NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 0, true));
                    }
                // hard HJ words
                if (GetReadabilityTests().is_test_included(ReadabilityMessages::HARRIS_JACOBSON()) )
                    {
                    m_hardWordsData->SetItemValue(hardWordRowCount, columnIndex++,
                        safe_divide<double>((*pos)->GetTotalHardWordsHarrisJacobson(),
                                            totalWordCountForHJ-totalNumeralCountForHJ)*100,
                        NumberFormatInfo(NumberFormatInfo::NumberFormatType::PercentageFormatting, 1, true));
                    m_hardWordsData->SetItemValue(hardWordRowCount, columnIndex++,
                        (*pos)->GetTotalHardWordsHarrisJacobson(),
                        NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 0, true));
                    }
                for (auto custTestPos = (*pos)->GetCustTestsInUse().begin();
                     custTestPos != (*pos)->GetCustTestsInUse().end();
                     ++custTestPos)
                    {
                    if (!custTestPos->GetIterator()->is_using_familiar_words())
                        { continue; }

                    // special logic for calculating word percentage if test is based on DC or HJ
                    const size_t totalWordCountForCustomTest =
                        custTestPos->IsDaleChallFormula() ?
                            totalWordCountForDC :
                        custTestPos->IsHarrisJacobsonFormula() ?
                            totalWordCountForHJ-totalNumeralCountForHJ : (*pos)->GetTotalWords();
                    m_hardWordsData->SetItemValue(hardWordRowCount, columnIndex++,
                        safe_divide<double>(custTestPos->GetUnfamiliarWordCount(),totalWordCountForCustomTest)*100,
                        NumberFormatInfo(NumberFormatInfo::NumberFormatType::PercentageFormatting,1,true));
                    m_hardWordsData->SetItemValue(hardWordRowCount, columnIndex++,
                        custTestPos->GetUnfamiliarWordCount(),
                        NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting,0,true));
                    }
                }
            ++hardWordRowCount;
            }
        }

    m_hardWordsData->SetSize(hardWordRowCount);
    }

//------------------------------------------------------------
void BatchProjectDoc::LoadSummaryStatsSection()
    {
    m_summaryStatsData->DeleteAllItems();

    m_summaryStatsColumnNames.clear();
    m_summaryStatsColumnNames = { _(L"Document"), _(L"Label") };
    if (GetStatisticsReportInfo().IsParagraphEnabled())
        {
        m_summaryStatsColumnNames.push_back(_(L"Number of paragraphs"));
        }
    if (GetStatisticsReportInfo().IsSentencesEnabled())
        {
        m_summaryStatsColumnNames.push_back(_(L"Number of sentences"));
        m_summaryStatsColumnNames.push_back(_(L"Number of units/independent clauses"));
        m_summaryStatsColumnNames.push_back(_(L"Number of difficult sentences"));
        m_summaryStatsColumnNames.push_back(_(L"Longest sentence"));
        m_summaryStatsColumnNames.push_back(_(L"Average sentence length"));
        m_summaryStatsColumnNames.push_back(_(L"Number of interrogative sentences (questions)"));
        m_summaryStatsColumnNames.push_back(_(L"Number of exclamatory sentences"));
        }
    if (GetStatisticsReportInfo().IsWordsEnabled())
        {
        m_summaryStatsColumnNames.push_back(_(L"Number of words"));
        m_summaryStatsColumnNames.push_back(_(L"Number of unique words"));
        m_summaryStatsColumnNames.push_back(_(L"Number of syllables"));
        m_summaryStatsColumnNames.push_back(_(L"Number of characters (punctuation excluded)"));
        m_summaryStatsColumnNames.push_back(_(L"Number of characters + punctuation"));
        m_summaryStatsColumnNames.push_back(_(L"Average number of characters"));
        m_summaryStatsColumnNames.push_back(_(L"Average number of syllables"));
        }
    if (GetStatisticsReportInfo().IsExtendedWordsEnabled())
        {
        m_summaryStatsColumnNames.push_back(_(L"Number of numerals"));
        m_summaryStatsColumnNames.push_back(_(L"Number of proper nouns"));
        m_summaryStatsColumnNames.push_back(_(L"Number of monosyllabic words"));
        m_summaryStatsColumnNames.push_back(_(L"Number of unique monosyllabic words"));
        m_summaryStatsColumnNames.push_back(_(L"Number of complex (3+ syllable) words"));
        m_summaryStatsColumnNames.push_back(_(L"Number of unique 3+ syllable words"));
        m_summaryStatsColumnNames.push_back(_(L"Number of long (6+ characters) words"));
        m_summaryStatsColumnNames.push_back(_(L"Number of unique long words"));
        m_summaryStatsColumnNames.push_back(_(L"Number of SMOG hard words"));
        m_summaryStatsColumnNames.push_back(_(L"Number of unique SMOG hard words"));
        m_summaryStatsColumnNames.push_back(_(L"Number of Fog hard words"));
        m_summaryStatsColumnNames.push_back(_(L"Number of unique Fog hard words"));
        if (IsIncludingDolchSightWords())
            {
            if (GetStatisticsReportInfo().IsDolchCoverageEnabled())
                {
                m_summaryStatsColumnNames.push_back(_(L"Number of conjunctions used"));
                m_summaryStatsColumnNames.push_back(_(L"Number of prepositions used"));
                m_summaryStatsColumnNames.push_back(_(L"Number of pronouns used"));
                m_summaryStatsColumnNames.push_back(_(L"Number of adverbs used"));
                m_summaryStatsColumnNames.push_back(_(L"Number of adjectives used"));
                m_summaryStatsColumnNames.push_back(_(L"Number of verbs used"));
                m_summaryStatsColumnNames.push_back(_(L"Number of nouns used"));
                }
            if (GetStatisticsReportInfo().IsDolchWordsEnabled())
                {
                m_summaryStatsColumnNames.push_back(_(L"Number of Dolch words"));
                m_summaryStatsColumnNames.push_back(_(L"Number of Dolch words (excluding nouns)"));
                m_summaryStatsColumnNames.push_back(_(L"Number of non-Dolch words"));
                m_summaryStatsColumnNames.push_back(_(L"Number of Dolch conjunctions"));
                m_summaryStatsColumnNames.push_back(_(L"Number of unique Dolch conjunctions"));
                m_summaryStatsColumnNames.push_back(_(L"Number of Dolch prepositions"));
                m_summaryStatsColumnNames.push_back(_(L"Number of unique Dolch prepositions"));
                m_summaryStatsColumnNames.push_back(_(L"Number of Dolch pronouns"));
                m_summaryStatsColumnNames.push_back(_(L"Number of unique Dolch pronouns"));
                m_summaryStatsColumnNames.push_back(_(L"Number of Dolch adverbs"));
                m_summaryStatsColumnNames.push_back(_(L"Number of unique Dolch adverbs"));
                m_summaryStatsColumnNames.push_back(_(L"Number of Dolch adjectives"));
                m_summaryStatsColumnNames.push_back(_(L"Number of unique Dolch adjectives"));
                m_summaryStatsColumnNames.push_back(_(L"Number of Dolch verbs"));
                m_summaryStatsColumnNames.push_back(_(L"Number of unique Dolch verbs"));
                m_summaryStatsColumnNames.push_back(_(L"Number of Dolch nouns"));
                m_summaryStatsColumnNames.push_back(_(L"Number of unique Dolch nouns"));
                }
            }
        if (IsDaleChallLikeTestIncluded() )
            {
            m_summaryStatsColumnNames.push_back(_(L"Number of Dale-Chall unfamiliar words"));
            m_summaryStatsColumnNames.push_back(_(L"Number of unique Dale-Chall unfamiliar words"));
            }
        if (GetReadabilityTests().is_test_included(ReadabilityMessages::HARRIS_JACOBSON()))
            {
            m_summaryStatsColumnNames.push_back(_(L"Number of Harris-Jacobson unfamiliar words"));
            m_summaryStatsColumnNames.push_back(_(L"Number of unique Harris-Jacobson unfamiliar words"));
            }
        if (GetReadabilityTests().is_test_included(ReadabilityMessages::SPACHE()))
            {
            m_summaryStatsColumnNames.push_back(_(L"Number of Spache unfamiliar words"));
            m_summaryStatsColumnNames.push_back(_(L"Number of unique Spache unfamiliar words"));
            }
        if (GetReadabilityTests().is_test_included(ReadabilityMessages::EFLAW()))
            {
            m_summaryStatsColumnNames.push_back(_(L"Number of McAlpine EFLAW miniwords"));
            m_summaryStatsColumnNames.push_back(_(L"Number of unique McAlpine EFLAW miniwords words"));
            }
        for (const auto& cTests : GetCustTestsInUse())
            {
            m_summaryStatsColumnNames.push_back(
                wxString::Format(_(L"Number of %s unfamiliar words"),
                                 cTests.GetIterator()->get_name().c_str()));
            m_summaryStatsColumnNames.push_back(
                wxString::Format(_(L"Number of unique %s unfamiliar words"),
                                 cTests.GetIterator()->get_name().c_str()));
            }
        }
    if (GetStatisticsReportInfo().IsGrammarEnabled() &&
        GetGrammarInfo().IsAnyFeatureEnabled())
        {
        m_summaryStatsColumnNames.push_back(_(L"Number of possible misspellings"));
        m_summaryStatsColumnNames.push_back(_(L"Number of repeated words"));
        m_summaryStatsColumnNames.push_back(_(L"Number of article mismatches"));
        m_summaryStatsColumnNames.push_back(_(L"Number of errors & misspellings"));
        m_summaryStatsColumnNames.push_back(_(L"Number of redundant phrases"));
        m_summaryStatsColumnNames.push_back(_(L"Number of overused words (x sentence)"));
        m_summaryStatsColumnNames.push_back(_(L"Number of wordy items"));
        m_summaryStatsColumnNames.push_back(_(L"Number of clich\u00E9s"));
        m_summaryStatsColumnNames.push_back(_(L"Number of passive voices"));
        m_summaryStatsColumnNames.push_back(_(L"Number of sentences that begin with conjunctions"));
        m_summaryStatsColumnNames.push_back(_(L"Number of Sentences that begin with lowercased words"));
        }
    if (GetStatisticsReportInfo().IsExtendedInformationEnabled())
        {
        m_summaryStatsColumnNames.push_back(_(L"Text size (Kbs.)"));
        }
    m_summaryStatsData->SetSize(m_docs.size(), m_summaryStatsColumnNames.size());

    // quneiform-suppress-begin
    size_t rowCount{ 0 };
    for (const auto& doc : m_docs)
        {
        size_t columnCount{ 0 };
        m_summaryStatsData->SetItemText(rowCount, columnCount++, doc->GetOriginalDocumentFilePath());
        m_summaryStatsData->SetItemText(rowCount, columnCount++, doc->GetOriginalDocumentDescription());
        if (GetStatisticsReportInfo().IsParagraphEnabled())
            {
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of paragraphs"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetTotalParagraphs());
            }
        if (GetStatisticsReportInfo().IsSentencesEnabled())
            {
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of sentences"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetTotalSentences());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of units/independent clauses"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetTotalSentenceUnits());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of difficult sentences"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetTotalOverlyLongSentences());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Longest sentence"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetLongestSentence());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Average sentence length"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                safe_divide<double>(doc->GetTotalWords(),
                                    doc->GetTotalSentences()),
                Wisteria::NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting,
                                           1, false));
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of interrogative sentences (questions)"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetTotalInterrogativeSentences());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of exclamatory sentences"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetTotalExclamatorySentences());
            }
        if (GetStatisticsReportInfo().IsWordsEnabled())
            {
            const double averageCharacterCount =
                safe_divide<double>(doc->GetTotalCharacters(), doc->GetTotalWords());
            const double averageSyllableCount =
                safe_divide<double>(doc->GetTotalSyllables(), doc->GetTotalWords());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of words"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetTotalWords());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of unique words"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetTotalUniqueWords());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of syllables"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetTotalSyllables());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of characters (punctuation excluded)"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetTotalCharacters());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of characters + punctuation"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetTotalCharactersPlusPunctuation());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Average number of characters"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, averageCharacterCount,
                Wisteria::NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting,
                                           1, false));
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Average number of syllables"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, averageSyllableCount,
                Wisteria::NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting,
                                           1, false));
            }
        if (GetStatisticsReportInfo().IsExtendedWordsEnabled())
            {
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of numerals"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetTotalNumerals());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of proper nouns"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetTotalProperNouns());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of monosyllabic words"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetTotalMonoSyllabicWords());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of unique monosyllabic words"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetTotalUniqueMonoSyllabicWords());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of complex (3+ syllable) words"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetTotal3PlusSyllabicWords());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of unique 3+ syllable words"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetTotalUnique3PlusSyllableWords());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of long (6+ characters) words"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetTotalLongWords());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of unique long words"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetTotalUnique6CharsPlusWords());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of SMOG hard words"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                doc->GetTotal3PlusSyllabicWordsNumeralsFullySyllabized());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of unique SMOG hard words"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                doc->GetUnique3PlusSyllabicWordsNumeralsFullySyllabized());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of Fog hard words"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetTotalHardWordsFog());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of unique Fog hard words"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetTotalUniqueHardFogWords());
            if (IsIncludingDolchSightWords())
                {
                if (GetStatisticsReportInfo().IsDolchCoverageEnabled())
                    {
                    assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of conjunctions used"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                        ProjectReportFormat::MAX_DOLCH_CONJUNCTION_WORDS - doc->GetUnusedDolchConjunctions());
                    assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of prepositions used"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                        ProjectReportFormat::MAX_DOLCH_PREPOSITION_WORDS - doc->GetUnusedDolchPrepositions());
                    assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of pronouns used"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                        ProjectReportFormat::MAX_DOLCH_PRONOUN_WORDS - doc->GetUnusedDolchPronouns());
                    assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of adverbs used"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                        ProjectReportFormat::MAX_DOLCH_ADVERB_WORDS - doc->GetUnusedDolchAdverbs());
                    assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of adjectives used"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                        ProjectReportFormat::MAX_DOLCH_ADJECTIVE_WORDS - doc->GetUnusedDolchAdjectives());
                    assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of verbs used"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                        ProjectReportFormat::MAX_DOLCH_VERBS - doc->GetUnusedDolchVerbs());
                    assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of nouns used"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                        ProjectReportFormat::MAX_DOLCH_NOUNS - doc->GetUnusedDolchNouns());
                    }
                if (GetStatisticsReportInfo().IsDolchWordsEnabled())
                    {
                    const size_t totalDolchWords =
                            doc->GetDolchConjunctionCounts().second +
                            doc->GetDolchPrepositionWordCounts().second +
                            doc->GetDolchPronounCounts().second +
                            doc->GetDolchAdverbCounts().second + doc->GetDolchAdjectiveCounts().second +
                            doc->GetDolchVerbsCounts().second + doc->GetDolchNounCounts().second;
                    const size_t totalDolchWordsExcludingNouns =
                        doc->GetDolchConjunctionCounts().second + doc->GetDolchPrepositionWordCounts().second +
                        doc->GetDolchPronounCounts().second +
                        doc->GetDolchAdverbCounts().second + doc->GetDolchAdjectiveCounts().second +
                        doc->GetDolchVerbsCounts().second;
                    assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of Dolch words"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++, totalDolchWords);
                    assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of Dolch words (excluding nouns)"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++, totalDolchWordsExcludingNouns);
                    assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of non-Dolch words"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                        doc->GetTotalWords() - totalDolchWords);
                    assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of Dolch conjunctions"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                        doc->GetDolchConjunctionCounts().second);
                    assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of unique Dolch conjunctions"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                        doc->GetDolchConjunctionCounts().first);
                    assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of Dolch prepositions"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                        doc->GetDolchPrepositionWordCounts().second);
                    assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of unique Dolch prepositions"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                        doc->GetDolchPrepositionWordCounts().first);
                    assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of Dolch pronouns"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                        doc->GetDolchPronounCounts().second);
                    assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of unique Dolch pronouns"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                        doc->GetDolchPronounCounts().first);
                    assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of Dolch adverbs"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                        doc->GetDolchAdverbCounts().second);
                    assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of unique Dolch adverbs"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                        doc->GetDolchAdverbCounts().first);
                    assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of Dolch adjectives"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                        doc->GetDolchAdjectiveCounts().second);
                    assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of unique Dolch adjectives"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                        doc->GetDolchAdjectiveCounts().first);
                    assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of Dolch verbs"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                        doc->GetDolchVerbsCounts().second);
                    assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of unique Dolch verbs"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                        doc->GetDolchVerbsCounts().first);
                    assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of Dolch nouns"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                        doc->GetDolchNounCounts().second);
                    assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of unique Dolch nouns"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                        doc->GetDolchNounCounts().first);
                    }
                }
            if (IsDaleChallLikeTestIncluded() )
                {
                assert(m_summaryStatsColumnNames[columnCount] ==
                    _(L"Number of Dale-Chall unfamiliar words"));
                m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetTotalHardWordsDaleChall());
                assert(m_summaryStatsColumnNames[columnCount] ==
                    _(L"Number of unique Dale-Chall unfamiliar words"));
                m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetTotalUniqueDCHardWords());
                }
            if (GetReadabilityTests().is_test_included(ReadabilityMessages::HARRIS_JACOBSON()))
                {
                assert(m_summaryStatsColumnNames[columnCount] ==
                    _(L"Number of Harris-Jacobson unfamiliar words"));
                m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                    doc->GetTotalHardWordsHarrisJacobson());
                assert(m_summaryStatsColumnNames[columnCount] ==
                    _(L"Number of unique Harris-Jacobson unfamiliar words"));
                m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                    doc->GetTotalUniqueHarrisJacobsonHardWords());
                }
            if (GetReadabilityTests().is_test_included(ReadabilityMessages::SPACHE()))
                {
                assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of Spache unfamiliar words"));
                m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                    doc->GetTotalHardWordsSpache());
                assert(m_summaryStatsColumnNames[columnCount] ==
                    _(L"Number of unique Spache unfamiliar words"));
                m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                    doc->GetTotalUniqueHardWordsSpache());
                }
            if (GetReadabilityTests().is_test_included(ReadabilityMessages::EFLAW()))
                {
                assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of McAlpine EFLAW miniwords"));
                m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                    doc->GetTotalMiniWords());
                assert(m_summaryStatsColumnNames[columnCount] ==
                    _(L"Number of unique McAlpine EFLAW miniwords words"));
                m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                    doc->GetTotalUniqueMiniWords());
                }
            for (const auto& cTest : doc->GetCustTestsInUse())
                {
                assert(m_summaryStatsColumnNames[columnCount] ==
                    wxString::Format(_(L"Number of %s unfamiliar words"),
                                 cTest.GetIterator()->get_name().c_str()));
                m_summaryStatsData->SetItemValue(rowCount, columnCount++, cTest.GetUnfamiliarWordCount());
                assert(m_summaryStatsColumnNames[columnCount] ==
                    wxString::Format(_(L"Number of unique %s unfamiliar words"),
                                 cTest.GetIterator()->get_name().c_str()));
                m_summaryStatsData->SetItemValue(rowCount, columnCount++, cTest.GetUniqueUnfamiliarWordCount());
                }
            }
        if (GetStatisticsReportInfo().IsGrammarEnabled() &&
            GetGrammarInfo().IsAnyFeatureEnabled())
            {
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of possible misspellings"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetMisspelledWordCount());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of repeated words"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetDuplicateWordCount());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of article mismatches"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetMismatchedArticleCount());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of errors & misspellings"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetWordingErrorCount());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of redundant phrases"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetRedundantPhraseCount());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of overused words (x sentence)"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetOverusedWordsBySentenceCount());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of wordy items"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetWordyPhraseCount());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of clich\u00E9s"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetClicheCount());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of passive voices"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetPassiveVoicesCount());
            assert(m_summaryStatsColumnNames[columnCount] ==
                _(L"Number of sentences that begin with conjunctions"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                doc->GetSentenceStartingWithConjunctionsCount());
            assert(m_summaryStatsColumnNames[columnCount] ==
                _(L"Number of Sentences that begin with lowercased words"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                doc->GetSentenceStartingWithLowercaseCount());
            }
        if (GetStatisticsReportInfo().IsExtendedInformationEnabled())
            {
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Text size (Kbs.)"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                safe_divide<double>(doc->GetTextSize(), 1024),
                Wisteria::NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting,
                2, true));
            }

        ++rowCount;
        }
    // quneiform-suppress-end

    m_summaryStatsData->SetSize(rowCount);
    }

//------------------------------------------------------------
void BatchProjectDoc::LoadWarningsSection()
    {
    m_warnings->DeleteAllItems();
    m_warnings->SetSize(m_docs.size() * 2 + GetSubProjectMessages().size(), 3);

    size_t warningCount = 0;

    // warnings from batch project itself (shouldn't really happen)
    for (const auto& message : GetSubProjectMessages())
        {
        // in case there are more warnings than expected, then resize it
        if (warningCount >= m_warnings->GetItemCount())
            {
            m_warnings->SetSize(m_warnings->GetItemCount() * 1.5);
            }
        m_warnings->SetItemText(warningCount, 0, message.GetMessage());
        ++warningCount;
        }
    for (std::vector<BaseProject*>::iterator pos = m_docs.begin();
        pos != m_docs.end();
        ++pos)
        {
        if ((*pos)->GetSubProjectMessages().size())
            {
            for (const auto& message : (*pos)->GetSubProjectMessages())
                {
                // in case there are more warnings than expected, then resize it
                if (warningCount >= m_warnings->GetItemCount())
                    {
                    m_warnings->SetSize(m_warnings->GetItemCount() * 1.5);
                    }
                m_warnings->SetItemText(warningCount, 0, (*pos)->GetOriginalDocumentFilePath());
                m_warnings->SetItemText(warningCount, 1, (*pos)->GetOriginalDocumentDescription());
                m_warnings->SetItemText(warningCount, 2, message.GetMessage());
                ++warningCount;
                }
            }
        }

    m_warnings->SetSize(warningCount);
    }

//------------------------------------------------------------
bool BatchProjectDoc::LoadDocuments(wxProgressDialog& progressDlg)
    {
    GetRepeatedWordData()->DeleteAllItems();
    GetRepeatedWordData()->SetSize(m_docs.size(), 4);
    m_incorrectArticleData->DeleteAllItems();
    m_incorrectArticleData->SetSize(m_docs.size(), 4);
    m_overusedWordBySentenceData->DeleteAllItems();
    m_overusedWordBySentenceData->SetSize(m_docs.size(), 4);
    m_passiveVoiceData->DeleteAllItems();
    m_passiveVoiceData->SetSize(m_docs.size(), 4);
    GetMisspelledWordData()->DeleteAllItems();
    GetMisspelledWordData()->SetSize(m_docs.size(), 5);
    m_overlyLongSentenceData->DeleteAllItems();
    m_overlyLongSentenceData->SetSize(m_docs.size(), 5);
    m_sentenceStartingWithConjunctionsData->DeleteAllItems();
    m_sentenceStartingWithConjunctionsData->SetSize(m_docs.size(), 4);
    m_sentenceStartingWithLowercaseData->DeleteAllItems();
    m_sentenceStartingWithLowercaseData->SetSize(m_docs.size(), 4);
    m_wordyPhraseData->DeleteAllItems();
    m_wordyPhraseData->SetSize(m_docs.size(), 5);
    m_redundantPhraseData->DeleteAllItems();
    m_redundantPhraseData->SetSize(m_docs.size(), 5);
    m_wordingErrorData->DeleteAllItems();
    m_wordingErrorData->SetSize(m_docs.size(), 5);
    m_clichePhraseData->DeleteAllItems();
    m_clichePhraseData->SetSize(m_docs.size(), 5);

    size_t dupWordCount = 0;
    size_t incorrectArticleCount = 0;
    size_t overusedWordBySentenceCount = 0;
    size_t passiveVoiceCount = 0;
    size_t misspelledWordCount = 0;
    size_t longSenteceCount = 0;
    size_t conjunctionSentencesCount = 0;
    size_t lowercaseSentencesCount = 0;
    size_t wordyPhraseCount = 0;
    size_t redundantPhraseCount = 0;
    size_t wordingErrorCount = 0;
    size_t clicheCount = 0;

    int counter{ progressDlg.GetValue() };

    double_frequency_set<word_case_insensitive_no_stem> wordsFromAllDocs;

    std::map<wxString,Wisteria::ZipCatalog*> archiveFiles;
    std::map<wxString,ExcelFile*> excelFiles;
    for (std::vector<BaseProject*>::iterator pos = m_docs.begin();
        pos != m_docs.end();
        ++pos)
        {
        // clear the document's text just in case the user switched from embedding to linking.
        // If the user switched from linking to embedded then note that the documents will need
        // to be externally loaded here to reacquire the text.
        if (GetDocumentStorageMethod() == TextStorage::NoEmbedText)
            { (*pos)->FreeDocumentText(); }
        // pre-2007 Microsoft Word files (*.doc) are difficult to detect lists in, so if we are
        // not explicitly specifying "fitted to the page" analysis for this project (above),
        // then override the global option and set it to treat all newlines as the
        // end of a paragraph.
        if (m_adjustParagraphParserForDocFiles &&
            wxFileName((*pos)->GetOriginalDocumentFilePath()).GetExt().CmpNoCase(_DT(L"doc")) == 0)
            {
            (*pos)->SetParagraphsParsingMethod(ParagraphParse::EachNewLineIsAParagraph);
            }

        FilePathResolver fileResolve((*pos)->GetOriginalDocumentFilePath(), false);
        if (fileResolve.IsExcelCell())
            {
            FilePathResolver fileResolver;
            size_t excelTag = (*pos)->GetOriginalDocumentFilePath().Lower().find(_DT(L".xlsx#"));
            assert(excelTag != std::wstring::npos);
            if (excelTag != std::wstring::npos)
                {
                wxFileName fn((*pos)->GetOriginalDocumentFilePath().substr(0, excelTag+5));
                if (!wxFile::Exists(fn.GetFullPath()) )
                    {
                    wxString fileBySameNameInProjectDirectory;
                    if (FindMissingFile(fn.GetFullPath(), fileBySameNameInProjectDirectory))
                        {
                        (*pos)->SetOriginalDocumentFilePath(
                            fileBySameNameInProjectDirectory+(*pos)->GetOriginalDocumentFilePath().substr(excelTag+5));
                        excelTag = (*pos)->GetOriginalDocumentFilePath().Lower().find(_DT(L".xlsx#"));
                        fn.Assign(fileBySameNameInProjectDirectory);
                        SetModifiedFlag();
                        }
                    }
                wxString worksheetName = (*pos)->GetOriginalDocumentFilePath().substr(excelTag+6);
                const size_t slash = worksheetName.find_last_of(L'#');
                if (slash != wxString::npos)
                    {
                    wxString CellName = worksheetName.substr(slash+1);
                    worksheetName.Truncate(slash);
                    const wxString workSheetPath = fn.GetFullPath() + L"#" + worksheetName;
                    std::map<wxString,ExcelFile*>::iterator excelFilePos = excelFiles.find(workSheetPath);
                    if (excelFilePos == excelFiles.end())
                        {
                        excelFilePos = excelFiles.insert(
                            std::pair<wxString,ExcelFile*>(workSheetPath, new ExcelFile(fn.GetFullPath()))).first;
                        // read in the worksheets
                        std::wstring workBookFileText =
                            excelFilePos->second->m_zip.ReadTextFile(L"xl/workbook.xml");
                        excelFilePos->second->m_xlsx_extract.read_worksheet_names(
                            workBookFileText.c_str(), workBookFileText.length());
                        // read in the string table
                        const std::wstring sharedStrings =
                            excelFilePos->second->m_zip.ReadTextFile(L"xl/sharedStrings.xml");
                        if (sharedStrings.length())
                            {
                            excelFilePos->second->m_xlsx_extract.read_shared_strings(
                                sharedStrings.c_str(), sharedStrings.length());
                            }
                        }

                    // find the sheet to get the cells from
                    auto sheetPos =
                        std::find(excelFilePos->second->m_xlsx_extract.get_worksheet_names().begin(),
                                  excelFilePos->second->m_xlsx_extract.get_worksheet_names().end(),
                                  worksheetName.wc_str());
                    if (sheetPos != excelFilePos->second->m_xlsx_extract.get_worksheet_names().end())
                        {
                        const wxString internalSheetName =
                            wxString::Format(L"xl/worksheets/sheet%zu.xml",
                                (sheetPos-excelFilePos->second->m_xlsx_extract.get_worksheet_names().begin())+1);
                        // see if this worksheet is already loaded
                        ExcelFile::Workbook::iterator internalSheetPos =
                            excelFilePos->second->m_worksheets.find(internalSheetName);
                        // wasn't loaded before, so load it now
                        if (internalSheetPos == excelFilePos->second->m_worksheets.end())
                            {
                            std::pair<ExcelFile::Workbook::iterator,bool> insertPos =
                                excelFilePos->second->m_worksheets.insert(
                                    std::pair<wxString,lily_of_the_valley::xlsx_extract_text::worksheet>(
                                        internalSheetName,
                                        lily_of_the_valley::xlsx_extract_text::worksheet()));
                            internalSheetPos = insertPos.first;
                            const std::wstring sheetFile =
                                excelFilePos->second->m_zip.ReadTextFile(internalSheetName);
                            if (sheetFile.length())
                                {
                                excelFilePos->second->m_xlsx_extract(
                                    sheetFile.c_str(),
                                    sheetFile.length(),
                                    internalSheetPos->second);
                                }
                            }
                        wxString cellText =
                            excelFilePos->second->m_xlsx_extract.get_cell_text(CellName.wc_str(),
                                                                               internalSheetPos->second);
                        fileResolver.ResolvePath(cellText, false);
                        if (!fileResolver.IsInvalidFile())
                            {
                            // this will change the spreadsheet cell path to the real file path
                            (*pos)->LoadDocumentAsSubProject(fileResolver.GetResolvedPath(),
                                std::wstring{}, GetMinDocWordCountForBatch() );
                            }
                        else
                            {
                            (*pos)->SetDocumentText(cellText.wc_string());
                            (*pos)->LoadDocumentAsSubProject((*pos)->GetOriginalDocumentFilePath(),
                                (*pos)->GetDocumentText(), GetMinDocWordCountForBatch() );
                            }
                        }
                    else
                        { (*pos)->SetLoadingOriginalTextSucceeded(false); }
                    }
                else
                    { (*pos)->SetLoadingOriginalTextSucceeded(false); }
                }
            else
                { (*pos)->SetLoadingOriginalTextSucceeded(false); }
            }
        else if (fileResolve.IsArchivedFile())
            {
            size_t archiveTag = (*pos)->GetOriginalDocumentFilePath().Lower().find(_DT(L".zip#"));
            assert(archiveTag != std::wstring::npos);
            if (archiveTag != std::wstring::npos)
                {
                wxFileName fn((*pos)->GetOriginalDocumentFilePath().substr(0, archiveTag+4));
                if (!wxFile::Exists(fn.GetFullPath()) )
                    {
                    wxString fileBySameNameInProjectDirectory;
                    if (FindMissingFile(fn.GetFullPath(), fileBySameNameInProjectDirectory))
                        {
                        (*pos)->SetOriginalDocumentFilePath(fileBySameNameInProjectDirectory +
                            (*pos)->GetOriginalDocumentFilePath().substr(archiveTag+4));
                        archiveTag = (*pos)->GetOriginalDocumentFilePath().Lower().find(_DT(L".zip#"));
                        fn.Assign(fileBySameNameInProjectDirectory);
                        SetModifiedFlag();
                        }
                    }
                auto archiveFilePos = archiveFiles.find(fn.GetFullPath());
                if (archiveFilePos == archiveFiles.end())
                    {
                    archiveFilePos = archiveFiles.insert(
                        std::pair<wxString,Wisteria::ZipCatalog*>(fn.GetFullPath(),
                                  new Wisteria::ZipCatalog(fn.GetFullPath()))).first;
                    }
                wxMemoryOutputStream memstream;
                if (!archiveFilePos->second->ReadFile(
                        (*pos)->GetOriginalDocumentFilePath().substr(archiveTag+5), memstream) &&
                    archiveFilePos->second->GetMessages().size())
                    {
                    AddQuietSubProjectMessage(archiveFilePos->second->GetMessages().back().m_message,
                                              archiveFilePos->second->GetMessages().back().m_icon);
                    archiveFilePos->second->ClearMessages();
                    }
                // Only load the document if the archive read didn't fail.
                // Otherwise, LoadDocumentNoUI() will try to load the ZIP file and
                // get the same error.
                if (memstream.GetLength())
                    {
                    const std::pair<bool, std::wstring> extractResult =
                        (*pos)->ExtractRawText(
                            {
                            static_cast<const char*>(memstream.GetOutputStreamBuffer()->GetBufferStart()),
                            static_cast<size_t>(memstream.GetLength())
                            },
                            wxFileName((*pos)->GetOriginalDocumentFilePath()).GetExt());
                    (*pos)->LoadDocumentAsSubProject((*pos)->GetOriginalDocumentFilePath(),
                        extractResult.second, GetMinDocWordCountForBatch() );
                    }
                else
                    { (*pos)->SetLoadingOriginalTextSucceeded(false); }
                }
            else
                { (*pos)->SetLoadingOriginalTextSucceeded(false); }
            }
        else
            {
            if (fileResolve.IsLocalOrNetworkFile() &&
                !wxFile::Exists((*pos)->GetOriginalDocumentFilePath()) )
                {
                wxString fileBySameNameInProjectDirectory;
                if (FindMissingFile((*pos)->GetOriginalDocumentFilePath(), fileBySameNameInProjectDirectory))
                    {
                    (*pos)->SetOriginalDocumentFilePath(fileBySameNameInProjectDirectory);
                    SetModifiedFlag();
                    }
                }
            (*pos)->LoadDocumentAsSubProject((*pos)->GetOriginalDocumentFilePath(),
                                             (*pos)->GetDocumentText(), GetMinDocWordCountForBatch() );
            }
        // passing in an archived file that we extracted here will cause the
        // subproject to use embedded text, see reset it after loading the document
        (*pos)->SetDocumentStorageMethod(GetDocumentStorageMethod());
        // free the text from the document to conserve memory
        // (unless we are embedding it in the project)
        if (GetDocumentStorageMethod() == TextStorage::NoEmbedText)
            {
            (*pos)->FreeDocumentText();
            }

        // NOTE: Grammar info needs to be loaded here before the documents'
        // word collections are deleted

        // misspellings
        if ((*pos)->LoadingOriginalTextSucceeded() &&
            (*pos)->GetWords()->get_misspelled_words().size())
            {
            GetMisspelledWordData()->SetItemText(misspelledWordCount, 0, (*pos)->GetOriginalDocumentFilePath());
            GetMisspelledWordData()->SetItemText(misspelledWordCount, 1, (*pos)->GetOriginalDocumentDescription());
            GetMisspelledWordData()->SetItemValue(misspelledWordCount, 2,
                (*pos)->GetWords()->get_misspelled_words().size());
            wxString misspelledWordsStr;
            frequency_set<traits::case_insensitive_wstring_ex> misspelledWords;
            const auto& misspelledWordIndices = (*pos)->GetWords()->get_misspelled_words();
            for (size_t i = 0; i < misspelledWordIndices.size(); ++i)
                {
                misspelledWords.insert(
                    (*pos)->GetWords()->get_word(misspelledWordIndices[i]).c_str());
                }
            GetMisspelledWordData()->SetItemValue(misspelledWordCount, 3, misspelledWords.get_data().size());
            // these must all be quoted for the Add to Dictionary dialog to pick them up correctly
            for (const auto& misspelled : misspelledWords.get_data())
                {
                if (misspelled.second > 1)
                    {
                    misspelledWordsStr.Append(L'\"')
                        .Append(misspelled.first.c_str())
                        .Append(wxString::Format(L"\" * %zu, ", misspelled.second));
                    }
                else
                    {
                    misspelledWordsStr.Append(L'\"')
                        .Append(misspelled.first.c_str())
                        .Append(L"\", ");
                    }
                }
            // chop off the last ", "
            if (misspelledWordsStr.length() > 2)
                { misspelledWordsStr.RemoveLast(2); }
            GetMisspelledWordData()->SetItemText(misspelledWordCount++, 4, misspelledWordsStr);
            }
        // repeated (duplicate) words
        if ((*pos)->LoadingOriginalTextSucceeded() &&
            (*pos)->GetWords()->get_duplicate_word_indices().size())
            {
            GetRepeatedWordData()->SetItemText(dupWordCount, 0, (*pos)->GetOriginalDocumentFilePath());
            GetRepeatedWordData()->SetItemText(dupWordCount, 1, (*pos)->GetOriginalDocumentDescription());
            GetRepeatedWordData()->SetItemValue(dupWordCount, 2,
                (*pos)->GetWords()->get_duplicate_word_indices().size());
            wxString doubleWordsStr;
            frequency_set<traits::case_insensitive_wstring_ex> doubleWords;
            const auto& dupWordIndices = (*pos)->GetWords()->get_duplicate_word_indices();
            for (size_t i = 0; i < dupWordIndices.size(); ++i)
                {
                doubleWords.insert(
                    (*pos)->GetWords()->get_word(dupWordIndices[i]).c_str());
                }
            const bool useQuotes{ doubleWords.get_data().size() > 1 };
            for (const auto& doubleWord : doubleWords.get_data())
                {
                if (doubleWord.second > 1)
                    {
                    doubleWordsStr.Append(L'\"')
                        .Append(doubleWord.first.c_str())
                        .Append(L' ')
                        .Append(doubleWord.first.c_str())
                        .Append(wxString::Format(L"\" * %zu, ", doubleWord.second));
                    }
                else
                {
                    if (useQuotes)
                        {
                        doubleWordsStr.Append(L'\"')
                            .Append(doubleWord.first.c_str())
                            .Append(L' ')
                            .Append(doubleWord.first.c_str())
                            .Append(L"\", ");
                        }
                else
                        {
                        doubleWordsStr
                            .Append(doubleWord.first.c_str())
                            .Append(L' ')
                            .Append(doubleWord.first.c_str())
                            .Append(L", ");
                        }
                    }
                }
            // chop off the last ", "
            if (doubleWordsStr.length() > 2)
                { doubleWordsStr.RemoveLast(2); }
            GetRepeatedWordData()->SetItemText(dupWordCount++, 3, doubleWordsStr);
            }
        // incorrect articles
        if ((*pos)->LoadingOriginalTextSucceeded() &&
            (*pos)->GetWords()->get_incorrect_article_indices().size())
            {
            m_incorrectArticleData->SetItemText(incorrectArticleCount, 0, (*pos)->GetOriginalDocumentFilePath());
            m_incorrectArticleData->SetItemText(incorrectArticleCount, 1, (*pos)->GetOriginalDocumentDescription());
            m_incorrectArticleData->SetItemValue(incorrectArticleCount, 2,
                (*pos)->GetWords()->get_incorrect_article_indices().size());
            wxString incorrectArticleStr;
            frequency_set<traits::case_insensitive_wstring_ex> incorrectArticles;
            const auto& incorrectArticleIndices = (*pos)->GetWords()->get_incorrect_article_indices();

            for (size_t i = 0; i < incorrectArticleIndices.size(); ++i)
                {
                incorrectArticles.insert(
                    (*pos)->GetWords()->get_word(incorrectArticleIndices[i]) + L' ' +
                    (*pos)->GetWords()->get_word(incorrectArticleIndices[i]+1));
                }
            const bool useQuotes{ incorrectArticles.get_data().size() > 1 };
            for (const auto& incorrectArticle : incorrectArticles.get_data())
                {
                if (incorrectArticle.second > 1)
                    {
                    incorrectArticleStr.Append(L'\"')
                        .Append(incorrectArticle.first.c_str())
                        .Append(wxString::Format(L"\" * %zu, ", incorrectArticle.second));
                    }
                else
                    {
                    if (useQuotes)
                        {
                        incorrectArticleStr.Append(L'\"')
                            .Append(incorrectArticle.first.c_str())
                            .Append(L"\", ");
                    }
                else
                        {
                        incorrectArticleStr
                            .Append(incorrectArticle.first.c_str())
                            .Append(L", ");
                        }
                    }
                }
            // chop off the last ", "
            if (incorrectArticleStr.length() > 2)
                { incorrectArticleStr.RemoveLast(2); }
            m_incorrectArticleData->SetItemText(incorrectArticleCount++, 3, incorrectArticleStr);
            }
        // overused words (by sentence)
        if ((*pos)->LoadingOriginalTextSucceeded() &&
            (*pos)->GetWords()->get_overused_words_by_sentence().size())
            {
            m_overusedWordBySentenceData->SetItemText(overusedWordBySentenceCount, 0,
                (*pos)->GetOriginalDocumentFilePath());
            m_overusedWordBySentenceData->SetItemText(overusedWordBySentenceCount, 1,
                (*pos)->GetOriginalDocumentDescription());
            m_overusedWordBySentenceData->SetItemValue(overusedWordBySentenceCount, 2,
                (*pos)->GetWords()->get_overused_words_by_sentence().size());

            wxString theWords;
            for (auto overUsedWordsListsIter = (*pos)->GetWords()->get_overused_words_by_sentence().cbegin();
                overUsedWordsListsIter != (*pos)->GetWords()->get_overused_words_by_sentence().cend();
                ++overUsedWordsListsIter)
                {
                theWords += L'\"';
                for (std::set<size_t>::const_iterator overusedWordsIter = overUsedWordsListsIter->second.cbegin();
                    overusedWordsIter != overUsedWordsListsIter->second.cend();
                    ++overusedWordsIter)
                    { theWords.append((*pos)->GetWords()->get_word((*overusedWordsIter)).c_str()).append(L" "); }
                theWords.Trim();
                theWords += L"\", ";
                }
            // chop off the last ", "
            if (theWords.length() > 2)
                { theWords.RemoveLast(2); }
            m_overusedWordBySentenceData->SetItemText(overusedWordBySentenceCount++, 3, theWords);
            }
        // passive Voice
        if ((*pos)->LoadingOriginalTextSucceeded() &&
            (*pos)->GetWords()->get_passive_voice_indices().size())
            {
            m_passiveVoiceData->SetItemText(passiveVoiceCount, 0, (*pos)->GetOriginalDocumentFilePath());
            m_passiveVoiceData->SetItemText(passiveVoiceCount, 1, (*pos)->GetOriginalDocumentDescription());
            m_passiveVoiceData->SetItemValue(passiveVoiceCount, 2,
                (*pos)->GetWords()->get_passive_voice_indices().size());
            wxString passiveVoiceStr;
            frequency_set<traits::case_insensitive_wstring_ex> passiveVoices;
            const auto& passiveVoiceIndices = (*pos)->GetWords()->get_passive_voice_indices();
            for (size_t i = 0; i < passiveVoiceIndices.size(); ++i)
                {
                traits::case_insensitive_wstring_ex currentPassivePhrase;
                for (size_t wordCounter = 0; wordCounter < passiveVoiceIndices[i].second; ++wordCounter)
                    {
                    currentPassivePhrase += (wordCounter == passiveVoiceIndices[i].second-1) ?
                        traits::case_insensitive_wstring_ex(
                            (*pos)->GetWords()->get_word(passiveVoiceIndices[i].first+wordCounter)) :
                        traits::case_insensitive_wstring_ex(
                            (*pos)->GetWords()->get_word(passiveVoiceIndices[i].first+wordCounter) + L' ');
                    }
                passiveVoices.insert(currentPassivePhrase);
                }
            const bool useQuotes{ passiveVoices.get_data().size() > 1 };
            for (const auto& passiveVoice : passiveVoices.get_data())
                {
                if (passiveVoice.second > 1)
                    {
                    passiveVoiceStr.Append(L'\"')
                        .Append(passiveVoice.first.c_str())
                        .Append(wxString::Format(L"\" * %zu, ", passiveVoice.second));
                    }
                else
                    {
                    if (useQuotes)
                        {
                        passiveVoiceStr.Append(L'\"')
                            .Append(passiveVoice.first.c_str())
                            .Append(L"\", ");
                        }
                    else
                        {
                        passiveVoiceStr
                            .Append(passiveVoice.first.c_str())
                            .Append(L", ");
                        }
                    }
                }
            // chop off the last ", "
            if (passiveVoiceStr.length() > 2)
                { passiveVoiceStr.RemoveLast(2); }
            m_passiveVoiceData->SetItemText(passiveVoiceCount++, 3, passiveVoiceStr);
            }
        // overly long sentences
        if ((*pos)->LoadingOriginalTextSucceeded() &&
            (*pos)->GetTotalOverlyLongSentences() > 0)
            {
            m_overlyLongSentenceData->SetItemText(longSenteceCount, 0, (*pos)->GetOriginalDocumentFilePath());
            m_overlyLongSentenceData->SetItemText(longSenteceCount, 1, (*pos)->GetOriginalDocumentDescription());
            m_overlyLongSentenceData->SetItemValue(longSenteceCount, 2, (*pos)->GetTotalOverlyLongSentences());
            m_overlyLongSentenceData->SetItemValue(longSenteceCount, 3, (*pos)->GetLongestSentence());
            // piece the sentence together
            const grammar::sentence_info& sentence =
                (*pos)->GetWords()->get_sentences()[(*pos)->GetLongestSentenceIndex()];
            std::vector<punctuation::punctuation_mark>::const_iterator punctPos =
                (*pos)->GetWords()->get_punctuation().begin();
            std::vector<punctuation::punctuation_mark>::const_iterator punctEnd =
                (*pos)->GetWords()->get_punctuation().end();
            wxString currentSentence = ProjectReportFormat::FormatSentence(*pos, sentence, punctPos, punctEnd);

            m_overlyLongSentenceData->SetItemText(longSenteceCount++, 4, currentSentence);
            }
        // sentences that start with conjunctions
        if ((*pos)->LoadingOriginalTextSucceeded() &&
            (*pos)->GetSentenceStartingWithConjunctionsCount() > 0)
            {
            m_sentenceStartingWithConjunctionsData->SetItemText(conjunctionSentencesCount, 0,
                (*pos)->GetOriginalDocumentFilePath());
            m_sentenceStartingWithConjunctionsData->SetItemText(conjunctionSentencesCount, 1,
                (*pos)->GetOriginalDocumentDescription());
            m_sentenceStartingWithConjunctionsData->SetItemValue(conjunctionSentencesCount, 2,
                (*pos)->GetSentenceStartingWithConjunctionsCount());
            wxString conjunctionsStr;
            frequency_set<traits::case_insensitive_wstring_ex> conjunctions;
            for (auto sentIter = (*pos)->GetWords()->get_conjunction_beginning_sentences().cbegin();
                sentIter != (*pos)->GetWords()->get_conjunction_beginning_sentences().cend();
                ++sentIter)
                {
                const size_t wordPos = (*pos)->GetWords()->get_sentences()[*sentIter].get_first_word_index();
                conjunctions.insert((*pos)->GetWords()->get_words()[wordPos].c_str());
                }
            for (auto conIter = conjunctions.get_data().cbegin();
                conIter != conjunctions.get_data().cend();
                ++conIter)
                {
                if (conIter->second > 1)
                    {
                    conjunctionsStr.Append(L'\"').Append(conIter->first.c_str()).
                        Append(wxString::Format(L"\" * %zu, ", conIter->second));
                    }
                else
                    { conjunctionsStr.Append(L'\"').Append(conIter->first.c_str()).Append(L"\", "); }
                }
            // chop off the last ", "
            if (conjunctionsStr.length() > 2)
                { conjunctionsStr.RemoveLast(2); }
            m_sentenceStartingWithConjunctionsData->SetItemText(conjunctionSentencesCount++, 3, conjunctionsStr);
            }
        // sentences that start with lowercase words
        if ((*pos)->LoadingOriginalTextSucceeded() &&
            (*pos)->GetSentenceStartingWithLowercaseCount() > 0)
            {
            m_sentenceStartingWithLowercaseData->SetItemText(lowercaseSentencesCount, 0,
                (*pos)->GetOriginalDocumentFilePath());
            m_sentenceStartingWithLowercaseData->SetItemText(lowercaseSentencesCount, 1,
                (*pos)->GetOriginalDocumentDescription());
            m_sentenceStartingWithLowercaseData->SetItemValue(lowercaseSentencesCount, 2,
                (*pos)->GetSentenceStartingWithLowercaseCount());
            wxString lowercasesStr;
            frequency_set<traits::case_insensitive_wstring_ex> lowercases;
            for (auto sentIter = (*pos)->GetWords()->get_lowercase_beginning_sentences().cbegin();
                sentIter != (*pos)->GetWords()->get_lowercase_beginning_sentences().cend();
                ++sentIter)
                {
                const size_t wordPos = (*pos)->GetWords()->get_sentences()[*sentIter].get_first_word_index();
                lowercases.insert((*pos)->GetWords()->get_words()[wordPos].c_str());
                }
            for (auto lcIter = lowercases.get_data().cbegin();
                lcIter != lowercases.get_data().cend();
                ++lcIter)
                {
                if (lcIter->second > 1)
                    {
                    lowercasesStr.Append(L'\"').Append(lcIter->first.c_str()).
                        Append(wxString::Format(L"\" * %zu, ", lcIter->second));
                    }
                else
                    { lowercasesStr.Append(L'\"').Append(lcIter->first.c_str()).Append(L"\", "); }
                }
            // chop off the last ", "
            if (lowercasesStr.length() > 2)
                { lowercasesStr.RemoveLast(2); }
            m_sentenceStartingWithLowercaseData->SetItemText(lowercaseSentencesCount++, 3, lowercasesStr);
            }
        // wordy items & cliches      
        if ((*pos)->LoadingOriginalTextSucceeded() &&
            (*pos)->GetWords()->get_known_phrase_indices().size() > 0)
            {
            const auto& wordyIndices = (*pos)->GetWords()->get_known_phrase_indices();
            const auto& wordyPhrases =
                (*pos)->GetWords()->get_known_phrases().get_phrases();
            frequency_map<traits::case_insensitive_wstring_ex,wxString> wordyPhrasesAndSuggestions;
            frequency_map<traits::case_insensitive_wstring_ex,wxString> redundantPhrasesAndSuggestions;
            frequency_map<traits::case_insensitive_wstring_ex,wxString> clichesAndSuggestions;
            frequency_map<traits::case_insensitive_wstring_ex,wxString> errorsAndSuggestions;
            // put together the phrases and their respective suggestions
            for (size_t i = 0; i < wordyIndices.size(); ++i)
                {
                switch (wordyPhrases[wordyIndices[i].second].first.get_type())
                    {
                case grammar::phrase_type::phrase_wordy:
                    wordyPhrasesAndSuggestions.insert(wordyPhrases[wordyIndices[i].second].first.to_string().c_str(),
                        wordyPhrases[wordyIndices[i].second].second.c_str());
                    break;
                case grammar::phrase_type::phrase_redundant:
                    redundantPhrasesAndSuggestions.insert(
                        wordyPhrases[wordyIndices[i].second].first.to_string().c_str(),
                        wordyPhrases[wordyIndices[i].second].second.c_str());
                    break;
                case grammar::phrase_type::phrase_cliche:
                    clichesAndSuggestions.insert(wordyPhrases[wordyIndices[i].second].first.to_string().c_str(),
                        wordyPhrases[wordyIndices[i].second].second.c_str());
                    break;
                case grammar::phrase_type::phrase_error:
                    errorsAndSuggestions.insert(wordyPhrases[wordyIndices[i].second].first.to_string().c_str(),
                        wordyPhrases[wordyIndices[i].second].second.c_str());
                    break;
                    };
                }

            // if anything was found in this document then add it to the lists
            if (errorsAndSuggestions.get_data().size())
                {
                wxString values;
                wxString suggestions;
                size_t totalCount{ 0 };
                const bool useQuotes{ errorsAndSuggestions.get_data().size() > 1 };
                for (const auto& errorAndSuggestion : errorsAndSuggestions.get_data())
                    {
                    if (errorAndSuggestion.second.second > 1)
                        {
                        // quotes will be needed if a multiplier is being added
                        values.Append(L'\"')
                            .Append(errorAndSuggestion.first.c_str())
                            .Append(
                                wxString::Format(L"\" * %zu, ", errorAndSuggestion.second.second));
                        }
                    else
                        {
                        // if there are multiple issues and suggestions, then wrap each
                        // one in quotes
                        if (useQuotes)
                            {
                            values.Append(L'\"')
                                .Append(errorAndSuggestion.first.c_str())
                                .Append(L"\", ");
                            }
                        else
                            {
                            values.Append(errorAndSuggestion.first.c_str())
                                .Append(L", ");
                            }
                        }
                    if (useQuotes)
                        {
                        suggestions.Append(L'\"')
                            .Append(errorAndSuggestion.second.first)
                            .Append(L"\", ");
                        }
                    else
                        {
                        suggestions.Append(errorAndSuggestion.second.first)
                            .Append(L", ");
                        }
                    totalCount += errorAndSuggestion.second.second;
                    }
                // trim off trailing comma and space
                if (values.length() > 2)
                    { values.RemoveLast(2); }
                if (suggestions.length() > 2)
                    { suggestions.RemoveLast(2); }
                m_wordingErrorData->SetItemText(wordingErrorCount, 0, (*pos)->GetOriginalDocumentFilePath());
                m_wordingErrorData->SetItemText(wordingErrorCount, 1, (*pos)->GetOriginalDocumentDescription());
                m_wordingErrorData->SetItemValue(wordingErrorCount, 2, totalCount);
                m_wordingErrorData->SetItemText(wordingErrorCount, 3, values);
                m_wordingErrorData->SetItemText(wordingErrorCount++, 4, suggestions);
                }
            if (wordyPhrasesAndSuggestions.get_data().size())
                {
                wxString values;
                wxString suggestions;
                size_t totalCount{ 0 };
                const bool useQuotes{ wordyPhrasesAndSuggestions.get_data().size() > 1 };
                for (const auto& wordyPhrase : wordyPhrasesAndSuggestions.get_data())
                    {
                    if (wordyPhrase.second.second > 1)
                        {
                        values.Append(L'\"')
                            .Append(wordyPhrase.first.c_str())
                            .Append(wxString::Format(L"\" * %zu, ", wordyPhrase.second.second));
                        }
                    else
                        {
                        if (useQuotes)
                            {
                            values.Append(L'\"').Append(wordyPhrase.first.c_str()).Append(L"\", ");
                            }
                        else
                            {
                            values.Append(wordyPhrase.first.c_str()).Append(L", ");
                            }
                        }
                    if (useQuotes)
                        {
                        suggestions.Append(L'\"').Append(wordyPhrase.second.first).Append(L"\", ");
                        }
                    else
                        {
                        suggestions.Append(wordyPhrase.second.first).Append(L", ");
                        }
                    totalCount += wordyPhrase.second.second;
                    }
                if (values.length() > 2)
                    { values.RemoveLast(2); }
                if (suggestions.length() > 2)
                    { suggestions.RemoveLast(2); }
                m_wordyPhraseData->SetItemText(wordyPhraseCount, 0, (*pos)->GetOriginalDocumentFilePath());
                m_wordyPhraseData->SetItemText(wordyPhraseCount, 1, (*pos)->GetOriginalDocumentDescription());
                m_wordyPhraseData->SetItemValue(wordyPhraseCount, 2, totalCount);
                m_wordyPhraseData->SetItemText(wordyPhraseCount, 3, values);
                m_wordyPhraseData->SetItemText(wordyPhraseCount++, 4, suggestions);
                }
            if (redundantPhrasesAndSuggestions.get_data().size())
                {
                wxString values;
                wxString suggestions;
                size_t totalCount{ 0 };
                const bool useQuotes{ redundantPhrasesAndSuggestions.get_data().size() > 1 };
                for (const auto& redundant : redundantPhrasesAndSuggestions.get_data())
                    {
                    if (redundant.second.second > 1)
                        {
                        values.Append(L'\"')
                            .Append(redundant.first.c_str())
                            .Append(wxString::Format(L"\" * %zu, ", redundant.second.second));
                        }
                    else
                        {
                        if (useQuotes)
                            {
                            values.Append(L'\"').Append(redundant.first.c_str()).Append(L"\", ");
                            }
                        else
                            {
                            values.Append(redundant.first.c_str()).Append(L", ");
                            }
                        }
                    if (useQuotes)
                        {
                        suggestions.Append(L'\"').Append(redundant.second.first).Append(L"\", ");
                        }
                    else
                        {
                        suggestions.Append(redundant.second.first).Append(L", ");
                        }
                    totalCount += redundant.second.second;
                    }
                if (values.length() > 2)
                    { values.RemoveLast(2); }
                if (suggestions.length() > 2)
                    { suggestions.RemoveLast(2); }
                m_redundantPhraseData->SetItemText(redundantPhraseCount, 0, (*pos)->GetOriginalDocumentFilePath());
                m_redundantPhraseData->SetItemText(redundantPhraseCount, 1, (*pos)->GetOriginalDocumentDescription());
                m_redundantPhraseData->SetItemValue(redundantPhraseCount, 2, totalCount);
                m_redundantPhraseData->SetItemText(redundantPhraseCount, 3, values);
                m_redundantPhraseData->SetItemText(redundantPhraseCount++, 4, suggestions);
                }
            if (clichesAndSuggestions.get_data().size())
                {
                wxString values;
                wxString suggestions;
                size_t totalCount{ 0 };
                const bool useQuotes{ clichesAndSuggestions.get_data().size() > 1 };
                for (const auto& cliche : clichesAndSuggestions.get_data())
                    {
                    if (cliche.second.second > 1)
                        {
                        values.Append(L'\"')
                            .Append(cliche.first.c_str())
                            .Append(wxString::Format(L"\" * %zu, ", cliche.second.second));
                        }
                    else
                        {
                        if (useQuotes)
                            {
                            values.Append(L'\"').Append(cliche.first.c_str()).Append(L"\", ");
                            }
                        else
                            {
                            values.Append(cliche.first.c_str()).Append(L", ");
                            }
                        }
                    if (useQuotes)
                        {
                        suggestions.Append(L'\"').Append(cliche.second.first).Append(L"\", ");
                        }
                    else
                        {
                        suggestions.Append(cliche.second.first).Append(L", ");
                        }
                    totalCount += cliche.second.second;
                    }
                if (values.length() > 2)
                    { values.RemoveLast(2); }
                if (suggestions.length() > 2)
                    { suggestions.RemoveLast(2); }
                m_clichePhraseData->SetItemText(clicheCount, 0, (*pos)->GetOriginalDocumentFilePath());
                m_clichePhraseData->SetItemText(clicheCount, 1, (*pos)->GetOriginalDocumentDescription());
                m_clichePhraseData->SetItemValue(clicheCount, 2, totalCount);
                m_clichePhraseData->SetItemText(clicheCount, 3, values);
                m_clichePhraseData->SetItemText(clicheCount++, 4, suggestions);
                }
            }

        if ((*pos)->LoadingOriginalTextSucceeded() &&
            (*pos)->GetWordsWithFrequencies())
            { wordsFromAllDocs.insert_with_custom_increment(*(*pos)->GetWordsWithFrequencies(), 1); }

        // free up some memory by destroying the indexed data in the document
        (*pos)->DeleteUniqueWordMap();
        (*pos)->DeleteWords();

        if (!progressDlg.Update(counter++) )
            { return false; }
        }

    // move all the words (from all documents) into lists
    multi_value_frequency_aggregate_map<traits::case_insensitive_wstring_ex,
                                        traits::case_insensitive_wstring_ex>
        keyWordsStemmedWithCounts;

    GetAllWordsBatchData()->DeleteAllItems();
    GetAllWordsBatchData()->SetSize(wordsFromAllDocs.get_data().size(), 3);

    auto stemmer = CreateStemmer();
    const auto& commonWords = GetStopList();
    size_t i = 0;
    for (auto wordPos = wordsFromAllDocs.get_data().cbegin();
        wordPos != wordsFromAllDocs.get_data().cend();
        ++wordPos, ++i)
        {
        GetAllWordsBatchData()->SetItemText(i, 0, wordPos->first.c_str());
        GetAllWordsBatchData()->SetItemValue(i, 1, wordPos->second.first);
        GetAllWordsBatchData()->SetItemValue(i, 2, wordPos->second.second);

        if (!wordPos->first.is_file_address() &&
            !wordPos->first.is_numeric() &&
            !commonWords.contains(wordPos->first.c_str()))
            {
            traits::case_insensitive_wstring_ex stemmedWord(wordPos->first.c_str());
            (*stemmer)(stemmedWord);
            keyWordsStemmedWithCounts.insert(
                // the stem and original word
                stemmedWord, wordPos->first,
                // overall frequency of current word
                wordPos->second.first);
            }
        }

    // prepare word cloud dataset
    if (m_keyWordsDataset == nullptr)
        { m_keyWordsDataset = std::make_shared<Wisteria::Data::Dataset>(); }
    m_keyWordsDataset->Clear();
    m_keyWordsDataset->AddCategoricalColumn(GetWordsColumnName());
    m_keyWordsDataset->AddContinuousColumn(GetWordsCountsColumnName());
    assert(m_keyWordsDataset->GetCategoricalColumns().size() == 1 &&
        L"Hard word dataset invalid!");
    assert(m_keyWordsDataset->GetRowCount() == 0 &&
        L"Hard word dataset should be empty!");
    m_keyWordsDataset->Resize(
        keyWordsStemmedWithCounts.get_data().size());
    auto keyWordsColumn =
        m_keyWordsDataset->GetCategoricalColumn(GetWordsColumnName());
    auto keydWordsFreqColumn =
        m_keyWordsDataset->GetContinuousColumn(GetWordsCountsColumnName());

    // condensed key words & word cloud
        {
        GetKeyWordsBatchData()->DeleteAllItems();
        GetKeyWordsBatchData()->SetSize(keyWordsStemmedWithCounts.get_data().size(), 2);

        size_t uniqueImportWordsCount{ 0 };
        size_t wordCloudWordsCount{ 0 };
        wxString allValuesStr;
        for (const auto& [keyWordStem, keyWordFreqInfo] : keyWordsStemmedWithCounts.get_data())
            {
            // aggregate all the variations of the current word that share a common stem
            allValuesStr.clear();
            for (const auto& subWord : keyWordFreqInfo.first.get_data())
                { allValuesStr.append(subWord.first.c_str()).append(L"; "); }
            allValuesStr.Trim().RemoveLast();

            GetKeyWordsBatchData()->SetItemText(uniqueImportWordsCount, 0, allValuesStr);
            GetKeyWordsBatchData()->SetItemValue(uniqueImportWordsCount++, 1, keyWordFreqInfo.second);

            // word cloud
            // which variation of the current stem occurs the most often
            auto mostFrequentWordVariation =
                std::max_element(keyWordFreqInfo.first.get_data().cbegin(),
                    keyWordFreqInfo.first.get_data().cend(),
                    [](const auto& lhv, const auto& rhv) noexcept
                    { return lhv.second < rhv.second; });
            assert(mostFrequentWordVariation != keyWordFreqInfo.first.get_data().cend() &&
                L"Empty word list for stemmed word?!");
            // add the next word to the dataset's string table
            const auto nextKey = keyWordsColumn->GetNextKey();
            if (mostFrequentWordVariation != keyWordFreqInfo.first.get_data().cend())
                {
                keyWordsColumn->GetStringTable().insert(
                    std::make_pair(nextKey, mostFrequentWordVariation->first.c_str()));
                }
            // could never happen, but for robustness sake use the stem word
            // if the word list for the stem is empty
            else
                {
                keyWordsColumn->GetStringTable().insert(
                    std::make_pair(nextKey, keyWordStem.c_str()));
                }
            // add the new string table ID (i.e., the current word) and
            // respective frequency to the current row
            keyWordsColumn->SetValue(wordCloudWordsCount, nextKey);
            keydWordsFreqColumn->SetValue(wordCloudWordsCount++, keyWordFreqInfo.second);
            }
        GetKeyWordsBatchData()->SetSize(uniqueImportWordsCount);
        }

    GetRepeatedWordData()->SetSize(dupWordCount);
    m_incorrectArticleData->SetSize(incorrectArticleCount);
    m_overusedWordBySentenceData->SetSize(overusedWordBySentenceCount);
    m_passiveVoiceData->SetSize(passiveVoiceCount);
    GetMisspelledWordData()->SetSize(misspelledWordCount);
    m_overlyLongSentenceData->SetSize(longSenteceCount);
    m_sentenceStartingWithConjunctionsData->SetSize(conjunctionSentencesCount);
    m_sentenceStartingWithLowercaseData->SetSize(lowercaseSentencesCount);
    m_wordyPhraseData->SetSize(wordyPhraseCount);
    m_redundantPhraseData->SetSize(redundantPhraseCount);
    m_wordingErrorData->SetSize(wordingErrorCount);
    m_clichePhraseData->SetSize(clicheCount);

    // in case any webpaths were redirected, we will need to recreate the list of document paths
    SyncFilePathsWithDocuments();

    for (std::map<wxString,Wisteria::ZipCatalog*>::iterator archivePos = archiveFiles.begin();
        archivePos != archiveFiles.end();
        ++archivePos)
        { wxDELETE(archivePos->second); }
    for (std::map<wxString,ExcelFile*>::iterator worksheetsPos = excelFiles.begin();
        worksheetsPos != excelFiles.end();
        ++worksheetsPos)
        { wxDELETE(worksheetsPos->second); }

    return true;
    }

//------------------------------------------------------------
void BatchProjectDoc::LoadScoresSection()
    {
    PROFILE();
    BatchProjectView* view = dynamic_cast<BatchProjectView*>(GetFirstView());

    // update any stats goals (test goals are reviewed as the tests are added below).
    for (auto doc : m_docs)
        { doc->ReviewStatGoals(); }

    m_customTestScores.clear();

    // clear out the aggregated (standard) test scores,
    // resize them (if needed), and reset their string tables
    for (auto& sTest : GetReadabilityTests().get_tests())
        {
        sTest.get_grade_point_collection()->Clear();
        sTest.get_index_point_collection()->Clear();
        sTest.get_cloze_point_collection()->Clear();
        // add columns for the scores and groups
        sTest.get_grade_point_collection()->AddContinuousColumn(GetScoreColumnName());
        sTest.get_index_point_collection()->AddContinuousColumn(GetScoreColumnName());
        sTest.get_cloze_point_collection()->AddContinuousColumn(GetScoreColumnName());

        sTest.get_grade_point_collection()->AddCategoricalColumn(GetGroupColumnName());
        sTest.get_index_point_collection()->AddCategoricalColumn(GetGroupColumnName());
        sTest.get_cloze_point_collection()->AddCategoricalColumn(GetGroupColumnName());
        if (sTest.is_included())
            {
            if (sTest.get_test().get_test_type() == readability::readability_test_type::grade_level)
                { sTest.get_grade_point_collection()->Reserve(m_docs.size()); }
            else if (sTest.get_test().get_test_type() == readability::readability_test_type::index_value)
                { sTest.get_index_point_collection()->Reserve(m_docs.size()); }
            else if (sTest.get_test().get_test_type() == readability::readability_test_type::predicted_cloze_score)
                { sTest.get_cloze_point_collection()->Reserve(m_docs.size()); }
            else if (sTest.get_test().get_test_type() ==
                readability::readability_test_type::index_value_and_grade_level)
                {
                sTest.get_grade_point_collection()->Reserve(m_docs.size());
                sTest.get_index_point_collection()->Reserve(m_docs.size());
                }
            else if (sTest.get_test().get_test_type() ==
                readability::readability_test_type::grade_level_and_predicted_cloze_score)
                {
                sTest.get_grade_point_collection()->Reserve(m_docs.size());
                sTest.get_cloze_point_collection()->Reserve(m_docs.size());
                }
            for (const auto& docLabel : GetDocumentLabels())
                {
                sTest.get_grade_point_collection()->GetCategoricalColumn(GetGroupColumnName())->
                    GetStringTable()[docLabel.second] = docLabel.first.c_str();
                sTest.get_index_point_collection()->GetCategoricalColumn(GetGroupColumnName())->
                    GetStringTable()[docLabel.second] = docLabel.first.c_str();
                sTest.get_cloze_point_collection()->GetCategoricalColumn(GetGroupColumnName())->
                    GetStringTable()[docLabel.second] = docLabel.first.c_str();
                }
            }
        }

    m_customTestScores.resize(GetCustTestsInUse().size());
    for (auto& customTestScores : m_customTestScores)
        {
        if (customTestScores == nullptr)
            { customTestScores = std::make_shared<Wisteria::Data::Dataset>(); }
        customTestScores->Clear();
        customTestScores->AddContinuousColumn(GetScoreColumnName());
        customTestScores->AddCategoricalColumn(GetGroupColumnName());
        customTestScores->Reserve(m_docs.size());
        for (const auto& docLabel : GetDocumentLabels())
            {
            customTestScores->GetCategoricalColumn(GetGroupColumnName())->
                GetStringTable()[docLabel.second] = docLabel.first.c_str();
            }
        }

    m_scoreRawData->SetNumberFormatter(GetReadabilityMessageCatalogPtr());
    m_scoreRawData->DeleteAllItems();
    // maximum test count + document path + document description (and any extra columns for tests with 2 results)
    //(we use their grade AND index values in the output)
    m_scoreRawData->SetSize(m_docs.size(), GetStandardTestCount()+GetCustomTestCount()+GetMultiResultTestCount()+2);

    size_t i = 0;
    for (auto pos = m_docs.begin();
         pos != m_docs.end();
         ++pos)
        {
        (*pos)->GetAggregatedGradeScores().clear();
        (*pos)->GetAggregatedClozeScores().clear();
        if (!(*pos)->LoadingOriginalTextSucceeded())
            { continue; }
        long currentColumn = 0;
        m_scoreRawData->SetItemText(i, currentColumn++, (*pos)->GetOriginalDocumentFilePath() );
        m_scoreRawData->SetItemText(i, currentColumn++, (*pos)->GetOriginalDocumentDescription());
        double value = 0;

        // if using groups for the documents
        auto docLabel = GetDocumentLabels().find((*pos)->GetOriginalDocumentDescription().wc_str());
        const Wisteria::Data::GroupIdType groupId = (docLabel != GetDocumentLabels().end()) ? docLabel->second : 0;

        // go through the standard tests
        for (auto rTests = GetReadabilityTests().get_tests().begin();
            rTests != GetReadabilityTests().get_tests().end();
            ++rTests)
            {
            // grade level tests
            if (rTests->is_included() &&
                rTests->get_test().get_test_type() == readability::readability_test_type::grade_level)
                {
                // have special logic for graphical tests
                if (rTests->get_test().get_id() == ReadabilityMessages::FRY().wc_str())
                    {
                    const auto fryGraph =
                        std::dynamic_pointer_cast<FryGraph>(view->GetFryGraph()->GetFixedObject(0, 0));
                    if (!fryGraph->GetScores().at(i).IsScoreInvalid() &&
                        !fryGraph->GetScores().at(i).IsScoreOutOfGradeRange())
                        {
                        rTests->get_grade_point_collection()->AddRow(Data::RowInfo().
                            Continuous({ static_cast<double>(fryGraph->GetScores().at(i).GetScore()) }).
                            Categoricals({ groupId }).
                            Id(wxFileName((*pos)->GetOriginalDocumentFilePath()).GetFullName()));
                        m_scoreRawData->SetItemText(i, currentColumn++,
                            wxNumberFormatter::ToString(fryGraph->GetScores().at(i).GetScore(), 0,
                                wxNumberFormatter::Style::Style_NoTrailingZeroes),
                            NumberFormatInfo(NumberFormatInfo::NumberFormatType::CustomFormatting, 1) );
                        (*pos)->GetAggregatedGradeScores().push_back(fryGraph->GetScores().at(i).GetScore());
                        (*pos)->ReviewTestGoal(ReadabilityMessages::FRY(), fryGraph->GetScores().at(i).GetScore());
                        }
                    else if (fryGraph->GetScores().at(i).IsScoreInvalid())
                        {
                        m_scoreRawData->SetItemText(i, currentColumn++, _(L"Text is too difficult to be plotted."));
                        (*pos)->ReviewTestGoal(ReadabilityMessages::FRY(), std::numeric_limits<double>::quiet_NaN());
                        }
                    else if (fryGraph->GetScores().at(i).IsScoreOutOfGradeRange())
                        {
                        const wxString TOO_DIFFICULT_DESCRIPTION = fryGraph->GetScores().at(i).IsWordsHard() ?
                            _(L"Text is too difficult to be classified to a specific grade "
                               "level because it contains too many high syllable words.") :
                            _(L"Text is too difficult to be classified to a specific grade "
                               "level because it contains too many long sentences.");
                        m_scoreRawData->SetItemText(i, currentColumn++, TOO_DIFFICULT_DESCRIPTION);
                        (*pos)->ReviewTestGoal(ReadabilityMessages::FRY(),
                            std::numeric_limits<double>::quiet_NaN());
                        }
                    else
                        {
                        m_scoreRawData->SetItemText(i, currentColumn++, wxString{});
                        (*pos)->ReviewTestGoal(ReadabilityMessages::FRY(),
                            std::numeric_limits<double>::quiet_NaN());
                        }
                    }
                else if (rTests->get_test().get_id() == ReadabilityMessages::GPM_FRY().wc_str())
                    {
                    const auto fryGraph =
                        std::dynamic_pointer_cast<FryGraph>(view->GetGpmFryGraph()->GetFixedObject(0, 0));
                    if (!fryGraph->GetScores().at(i).IsScoreInvalid() &&
                        !fryGraph->GetScores().at(i).IsScoreOutOfGradeRange())
                        {
                        rTests->get_grade_point_collection()->AddRow(Data::RowInfo().
                            Continuous({ static_cast<double>(fryGraph->GetScores().at(i).GetScore()) }).
                            Categoricals({ groupId }).
                            Id(wxFileName((*pos)->GetOriginalDocumentFilePath()).GetFullName()));
                        m_scoreRawData->SetItemText(i, currentColumn++,
                            wxNumberFormatter::ToString(fryGraph->GetScores().at(i).GetScore(), 0,
                                wxNumberFormatter::Style::Style_NoTrailingZeroes),
                            NumberFormatInfo(NumberFormatInfo::NumberFormatType::CustomFormatting, 1) );
                        (*pos)->GetAggregatedGradeScores().push_back(fryGraph->GetScores().at(i).GetScore());
                        (*pos)->ReviewTestGoal(ReadabilityMessages::GPM_FRY(),
                            fryGraph->GetScores().at(i).GetScore());
                        }
                    else if (fryGraph->GetScores().at(i).IsScoreInvalid())
                        {
                        m_scoreRawData->SetItemText(i, currentColumn++,
                            _(L"Text is too difficult to be plotted."));
                        (*pos)->ReviewTestGoal(ReadabilityMessages::GPM_FRY(),
                            std::numeric_limits<double>::quiet_NaN());
                        }
                    else if (fryGraph->GetScores().at(i).IsScoreOutOfGradeRange())
                        {
                        const wxString TOO_DIFFICULT_DESCRIPTION =
                            fryGraph->GetScores().at(i).IsWordsHard() ?
                                _(L"Text is too difficult to be classified to a specific "
                                  "grade level because it contains too many high syllable words.") :
                                _(L"Text is too difficult to be classified to a specific "
                                  "grade level because it contains too many long sentences.");

                        m_scoreRawData->SetItemText(i, currentColumn++, TOO_DIFFICULT_DESCRIPTION);
                        (*pos)->ReviewTestGoal(ReadabilityMessages::GPM_FRY(),
                                               std::numeric_limits<double>::quiet_NaN());
                        }
                    else
                        {
                        m_scoreRawData->SetItemText(i, currentColumn++, wxString{});
                        (*pos)->ReviewTestGoal(ReadabilityMessages::GPM_FRY(),
                            std::numeric_limits<double>::quiet_NaN());
                        }
                    }
                else if (rTests->get_test().get_id() == ReadabilityMessages::SCHWARTZ().wc_str())
                    {
                    const auto schwartzGraph =
                        std::dynamic_pointer_cast<SchwartzGraph>(
                            view->GetSchwartzGraph()->GetFixedObject(0, 0));
                    if (!schwartzGraph->GetScores().at(i).IsScoreInvalid() &&
                        !schwartzGraph->GetScores().at(i).IsScoreOutOfGradeRange())
                        {
                        rTests->get_grade_point_collection()->AddRow(Data::RowInfo().
                            Continuous({ schwartzGraph->GetScores().at(i).GetScoreAverage() }).
                            Categoricals({ groupId }).
                            Id(wxFileName((*pos)->GetOriginalDocumentFilePath()).GetFullName()));
                        m_scoreRawData->SetItemText(i, currentColumn++,
                            wxNumberFormatter::ToString(
                                schwartzGraph->GetScores().at(i).GetScoreAverage(), 1,
                                wxNumberFormatter::Style::Style_NoTrailingZeroes),
                            NumberFormatInfo(NumberFormatInfo::NumberFormatType::CustomFormatting, 1) );
                        (*pos)->GetAggregatedGradeScores().push_back(
                            schwartzGraph->GetScores().at(i).GetScoreAverage());
                        (*pos)->ReviewTestGoal(ReadabilityMessages::SCHWARTZ(),
                            schwartzGraph->GetScores().at(i).GetScoreAverage());
                        }
                    else if (schwartzGraph->GetScores().at(i).IsScoreInvalid())
                        {
                        m_scoreRawData->SetItemText(i, currentColumn++,
                            _(L"Text is too difficult to be plotted."));
                        (*pos)->ReviewTestGoal(ReadabilityMessages::SCHWARTZ(),
                            std::numeric_limits<double>::quiet_NaN());
                        }
                    else if (schwartzGraph->GetScores().at(i).IsScoreOutOfGradeRange())
                        {
                        const wxString TOO_DIFFICULT_DESCRIPTION =
                            schwartzGraph->GetScores().at(i).IsWordsHard() ?
                                _(L"Text is too difficult to be classified to a specific grade "
                                  "level because it contains too many high syllable words.") :
                                _(L"Text is too difficult to be classified to a specific grade "
                                  "level because it contains too many long sentences.");
                        m_scoreRawData->SetItemText(i, currentColumn++, TOO_DIFFICULT_DESCRIPTION);
                        (*pos)->ReviewTestGoal(ReadabilityMessages::SCHWARTZ(),
                                               std::numeric_limits<double>::quiet_NaN());
                        }
                    else
                        {
                        m_scoreRawData->SetItemText(i, currentColumn++, wxString{});
                        (*pos)->ReviewTestGoal(ReadabilityMessages::SCHWARTZ(),
                            std::numeric_limits<double>::quiet_NaN());
                        }
                    }
                else if (rTests->get_test().get_id() == ReadabilityMessages::RAYGOR().wc_str())
                    {
                    const auto raygorGraph =
                        std::dynamic_pointer_cast<RaygorGraph>(view->GetRaygorGraph()->GetFixedObject(0, 0));
                    if (!raygorGraph->GetScores().at(i).IsScoreInvalid() &&
                        !raygorGraph->GetScores().at(i).IsScoreOutOfGradeRange())
                        {
                        rTests->get_grade_point_collection()->AddRow(Data::RowInfo().
                            Continuous({ static_cast<double>(raygorGraph->GetScores().at(i).GetScore()) }).
                            Categoricals({ groupId }).
                            Id(wxFileName((*pos)->GetOriginalDocumentFilePath()).GetFullName()));
                        m_scoreRawData->SetItemText(i, currentColumn++,
                            wxNumberFormatter::ToString(raygorGraph->GetScores().at(i).GetScore(), 0,
                                wxNumberFormatter::Style::Style_NoTrailingZeroes),
                            NumberFormatInfo(NumberFormatInfo::NumberFormatType::CustomFormatting, 1) );
                        (*pos)->GetAggregatedGradeScores().push_back(
                            raygorGraph->GetScores().at(i).GetScore());
                        (*pos)->ReviewTestGoal(ReadabilityMessages::RAYGOR(),
                            raygorGraph->GetScores().at(i).GetScore());
                        }
                    else if (raygorGraph->GetScores().at(i).IsScoreInvalid())
                        {
                        m_scoreRawData->SetItemText(i, currentColumn++,
                            _(L"Text is too difficult to be plotted."));
                        (*pos)->ReviewTestGoal(ReadabilityMessages::RAYGOR(),
                            std::numeric_limits<double>::quiet_NaN());
                        }
                    else if (raygorGraph->GetScores().at(i).IsScoreOutOfGradeRange())
                        {
                        const wxString TOO_DIFFICULT_DESCRIPTION =
                            raygorGraph->GetScores().at(i).IsWordsHard() ?
                                _(L"Text is too difficult to be classified to a specific "
                                  "grade level because it contains too many 6+ character words.") :
                                _(L"Text is too difficult to be classified to a specific "
                                  "grade level because it contains too many long sentences.");

                        m_scoreRawData->SetItemText(i, currentColumn++, TOO_DIFFICULT_DESCRIPTION);
                        (*pos)->ReviewTestGoal(ReadabilityMessages::RAYGOR(),
                                               std::numeric_limits<double>::quiet_NaN());
                        }
                    else
                        {
                        m_scoreRawData->SetItemText(i, currentColumn++, wxString{});
                        (*pos)->ReviewTestGoal(ReadabilityMessages::RAYGOR(),
                            std::numeric_limits<double>::quiet_NaN());
                        }
                    }
                else if ((*pos)->AddStandardReadabilityTest(rTests->get_test().get_id().c_str()) &&
                ReadabilityMessages::GetScoreValue((*pos)->GetLastGradeLevel(), value))
                    {
                    rTests->get_grade_point_collection()->AddRow(Data::RowInfo().
                        Continuous({ value }).
                        Categoricals({ groupId }).
                        Id(wxFileName((*pos)->GetOriginalDocumentFilePath()).GetFullName()));
                    m_scoreRawData->SetItemText(i, currentColumn++, (*pos)->GetLastGradeLevel(),
                        NumberFormatInfo(NumberFormatInfo::NumberFormatType::CustomFormatting, 1) );
                    (*pos)->GetAggregatedGradeScores().push_back(value);
                    }
                else
                    { m_scoreRawData->SetItemText(i, currentColumn++, wxString{}); }
                }
            // index tests
            else if (rTests->is_included() &&
            rTests->get_test().get_test_type() == readability::readability_test_type::index_value)
                {
                if (rTests->get_test().get_id() == ReadabilityMessages::FRASE().wc_str())
                    {
                    auto fraseGraph =
                        std::dynamic_pointer_cast<FraseGraph>(
                            view->GetFraseGraph()->GetFixedObject(0, 0));
                    if (!fraseGraph->GetScores().at(i).IsScoreInvalid())
                        {
                        rTests->get_index_point_collection()->AddRow(Data::RowInfo().
                            Continuous({ static_cast<double>(fraseGraph->GetScores().at(i).GetScore()) }).
                            Categoricals({ groupId }).
                            Id(wxFileName((*pos)->GetOriginalDocumentFilePath()).GetFullName()));
                        m_scoreRawData->SetItemText(i, currentColumn++,
                            wxNumberFormatter::ToString(fraseGraph->GetScores().at(i).GetScore(), 0,
                                wxNumberFormatter::Style::Style_NoTrailingZeroes),
                            NumberFormatInfo(NumberFormatInfo::NumberFormatType::CustomFormatting, 1) );
                        (*pos)->ReviewTestGoal(ReadabilityMessages::FRASE(),
                            fraseGraph->GetScores().at(i).GetScore());
                        }
                    else if (fraseGraph->GetScores().at(i).IsScoreInvalid())
                        {
                        m_scoreRawData->SetItemText(i, currentColumn++,
                            _(L"Text is too difficult to be plotted."));
                        (*pos)->ReviewTestGoal(ReadabilityMessages::FRASE(),
                            std::numeric_limits<double>::quiet_NaN());
                        }
                    else
                        {
                        m_scoreRawData->SetItemText(i, currentColumn++, wxString{});
                        (*pos)->ReviewTestGoal(ReadabilityMessages::FRASE(),
                            std::numeric_limits<double>::quiet_NaN());
                        }
                    }
                else if ((*pos)->AddStandardReadabilityTest(rTests->get_test().get_id().c_str()) &&
                    !std::isnan((*pos)->GetLastIndexScore()) )
                    {
                    rTests->get_index_point_collection()->AddRow(Data::RowInfo().
                        Continuous({ (*pos)->GetLastIndexScore() }).
                        Categoricals({ groupId }).
                        Id(wxFileName((*pos)->GetOriginalDocumentFilePath()).GetFullName()));
                    m_scoreRawData->SetItemValue(i, currentColumn++, (*pos)->GetLastIndexScore(),
                        NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 1) );
                    }
                else
                    { m_scoreRawData->SetItemText(i, currentColumn++, wxString{}); }
                }
            // cloze tests
            else if (rTests->is_included() &&
            rTests->get_test().get_test_type() == readability::readability_test_type::predicted_cloze_score)
                {
                if ((*pos)->AddStandardReadabilityTest(rTests->get_test().get_id().c_str()) &&
                    !std::isnan((*pos)->GetLastClozeScore()))
                    {
                    rTests->get_cloze_point_collection()->AddRow(Data::RowInfo().
                        Continuous({ (*pos)->GetLastClozeScore() }).
                        Categoricals({ groupId }).
                        Id(wxFileName((*pos)->GetOriginalDocumentFilePath()).GetFullName()));
                    m_scoreRawData->SetItemValue(i, currentColumn++, (*pos)->GetLastClozeScore());
                    (*pos)->GetAggregatedClozeScores().push_back((*pos)->GetLastClozeScore());
                    }
                else
                    { m_scoreRawData->SetItemText(i, currentColumn++, wxString{}); }
                }
            // grade and index test
            else if (rTests->is_included() &&
            rTests->get_test().get_test_type() == readability::readability_test_type::index_value_and_grade_level)
                {
                if ((*pos)->AddStandardReadabilityTest(rTests->get_test().get_id().c_str()) )
                    {
                    if (!std::isnan((*pos)->GetLastIndexScore()))
                        {
                        rTests->get_index_point_collection()->AddRow(Data::RowInfo().
                            Continuous({ (*pos)->GetLastIndexScore() }).
                            Categoricals({ groupId }).
                            Id(wxFileName((*pos)->GetOriginalDocumentFilePath()).GetFullName()));
                        m_scoreRawData->SetItemValue(i, currentColumn++, (*pos)->GetLastIndexScore(),
                            NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 1) );
                        }
                    else
                        { m_scoreRawData->SetItemText(i, currentColumn++, wxString{}); }
                    if (ReadabilityMessages::GetScoreValue((*pos)->GetLastGradeLevel(), value))
                        {
                        rTests->get_grade_point_collection()->AddRow(Data::RowInfo().
                            Continuous({ value }).
                            Categoricals({ groupId }).
                            Id(wxFileName((*pos)->GetOriginalDocumentFilePath()).GetFullName()));
                        m_scoreRawData->SetItemText(i, currentColumn++, (*pos)->GetLastGradeLevel(),
                            NumberFormatInfo(NumberFormatInfo::NumberFormatType::CustomFormatting, 1) );
                        (*pos)->GetAggregatedGradeScores().push_back(value);
                        }
                    else
                        { m_scoreRawData->SetItemText(i, currentColumn++, wxString{}); }
                    }
                else
                    {
                    m_scoreRawData->SetItemText(i, currentColumn++, wxString{});
                    m_scoreRawData->SetItemText(i, currentColumn++, wxString{});
                    }
                }
            // grade and cloze score
            else if (rTests->is_included() &&
                     rTests->get_test().get_test_type() ==
                        readability::readability_test_type::grade_level_and_predicted_cloze_score)
                {
                if ((*pos)->AddStandardReadabilityTest(rTests->get_test().get_id().c_str()) )
                    {
                    if (ReadabilityMessages::GetScoreValue((*pos)->GetLastGradeLevel(), value))
                        {
                        rTests->get_grade_point_collection()->AddRow(Data::RowInfo().
                            Continuous({ value }).
                            Categoricals({ groupId }).
                            Id(wxFileName((*pos)->GetOriginalDocumentFilePath()).GetFullName()));
                        m_scoreRawData->SetItemText(i, currentColumn++, (*pos)->GetLastGradeLevel(),
                            NumberFormatInfo(NumberFormatInfo::NumberFormatType::CustomFormatting, 1) );
                        (*pos)->GetAggregatedGradeScores().push_back(value);
                        }
                    else
                        { m_scoreRawData->SetItemText(i, currentColumn++, wxString{}); }
                    if (!std::isnan((*pos)->GetLastClozeScore()))
                        {
                        rTests->get_cloze_point_collection()->AddRow(Data::RowInfo().
                            Continuous({ (*pos)->GetLastClozeScore() }).
                            Categoricals({ groupId }).
                            Id(wxFileName((*pos)->GetOriginalDocumentFilePath()).GetFullName()));
                        m_scoreRawData->SetItemValue(i, currentColumn++, (*pos)->GetLastClozeScore());
                        (*pos)->GetAggregatedClozeScores().push_back((*pos)->GetLastClozeScore());
                        }
                    else
                        { m_scoreRawData->SetItemText(i, currentColumn++, wxString{}); }
                    }
                else
                    {
                    m_scoreRawData->SetItemText(i, currentColumn++, wxString{});
                    m_scoreRawData->SetItemText(i, currentColumn++, wxString{});
                    }
                }
            }
        // go through the custom tests now
        for (std::vector<CustomReadabilityTestInterface>::const_iterator testPos = GetCustTestsInUse().cbegin();
            testPos != GetCustTestsInUse().cend();
            ++testPos)
            {
            if ((*pos)->AddCustomReadabilityTest(testPos->GetTestName(), true))
                {
                if (testPos->GetIterator()->get_test_type() == readability::readability_test_type::grade_level &&
                    ReadabilityMessages::GetScoreValue((*pos)->GetLastGradeLevel(), value))
                    {
                    m_customTestScores[(testPos-GetCustTestsInUse().begin())]->AddRow(Data::RowInfo().
                        Continuous({ value }).
                        Categoricals({ groupId }).
                        Id(wxFileName((*pos)->GetOriginalDocumentFilePath()).GetFullName()));
                    m_scoreRawData->SetItemText(i, currentColumn++, (*pos)->GetLastGradeLevel(),
                        NumberFormatInfo(NumberFormatInfo::NumberFormatType::CustomFormatting, 1) );
                    (*pos)->GetAggregatedGradeScores().push_back(value);
                    }
                else if (testPos->GetIterator()->get_test_type() == readability::readability_test_type::index_value &&
                    !std::isnan((*pos)->GetLastIndexScore()) )
                    {
                    m_customTestScores[(testPos-GetCustTestsInUse().begin())]->AddRow(Data::RowInfo().
                        Continuous({ (*pos)->GetLastIndexScore() }).
                        Categoricals({ groupId }).
                        Id(wxFileName((*pos)->GetOriginalDocumentFilePath()).GetFullName()));
                    m_scoreRawData->SetItemValue(i, currentColumn++, (*pos)->GetLastIndexScore(),
                        NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 1) );
                    }
                else if (testPos->GetIterator()->get_test_type() ==
                        readability::readability_test_type::predicted_cloze_score &&
                    !std::isnan((*pos)->GetLastClozeScore()) )
                    {
                    m_customTestScores[(testPos-GetCustTestsInUse().begin())]->AddRow(Data::RowInfo().
                        Continuous({ (*pos)->GetLastClozeScore() }).
                        Categoricals({ groupId }).
                        Id(wxFileName((*pos)->GetOriginalDocumentFilePath()).GetFullName()));
                    m_scoreRawData->SetItemValue(i, currentColumn++, (*pos)->GetLastClozeScore());
                    (*pos)->GetAggregatedClozeScores().push_back((*pos)->GetLastClozeScore());
                    }
                else
                    { m_scoreRawData->SetItemText(i, currentColumn++, wxString{}); }
                }
            else
                { m_scoreRawData->SetItemText(i, currentColumn++, wxString{}); }
            }
        // increment this here because our current index into the score data will not be the same
        // as the index into the current document if any of the documents failed
        ++i;
        }
    m_scoreRawData->SetSize(i);

    m_scoreStatsData->SetNumberFormatter(GetReadabilityMessageCatalogPtr());
    m_scoreStatsData->DeleteAllItems();
    m_scoreStatsData->SetSize(
        // maximum test count
        GetStandardTestCount()+GetCustomTestCount()+GetMultiResultTestCount(),
        // maximum stats count, statistics + test name
        CUMULATIVE_STATS_COUNT+1);
    long currentRow = 0;
    // Summarize the standard tests' scores
    for (auto rTests = GetReadabilityTests().get_tests().begin();
        rTests != GetReadabilityTests().get_tests().end();
        ++rTests)
        {
        if (rTests->is_included() && rTests->get_test().get_test_type() ==
            readability::readability_test_type::grade_level)
            {
            SetScoreStatsRow(m_scoreStatsData, rTests->get_test().get_long_name().c_str(),
                wxString{}, currentRow++,
                rTests->get_grade_point_collection()->GetContinuousColumn(GetScoreColumnName())->GetValues(),
                rTests->get_test().is_integral() ? 0 : 1, GetVarianceMethod(), true);
            }
        else if (rTests->is_included() && rTests->get_test().get_test_type() ==
            readability::readability_test_type::index_value)
            {
            SetScoreStatsRow(m_scoreStatsData, rTests->get_test().get_long_name().c_str(),
                wxString{}, currentRow++,
                rTests->get_index_point_collection()->GetContinuousColumn(GetScoreColumnName())->GetValues(),
                rTests->get_test().is_integral() ? 0 : 1, GetVarianceMethod(), false);
            }
        else if (rTests->is_included() && rTests->get_test().get_test_type() ==
            readability::readability_test_type::predicted_cloze_score)
            {
            SetScoreStatsRow(m_scoreStatsData, rTests->get_test().get_long_name().c_str(),
                wxString{}, currentRow++,
                rTests->get_cloze_point_collection()->GetContinuousColumn(GetScoreColumnName())->GetValues(),
                0, GetVarianceMethod(), false);
            }
        else if (rTests->is_included() && rTests->get_test().get_test_type() ==
            readability::readability_test_type::index_value_and_grade_level)
            {
            SetScoreStatsRow(m_scoreStatsData,
                BatchProjectView::FormatIndexValuesLabel(rTests->get_test().get_long_name().c_str()),
                wxString{}, currentRow++,
                rTests->get_index_point_collection()->GetContinuousColumn(GetScoreColumnName())->GetValues(),
                rTests->get_test().is_integral() ? 0 : 1, GetVarianceMethod(), false);
            SetScoreStatsRow(m_scoreStatsData,
                BatchProjectView::FormatGradeLevelsLabel(rTests->get_test().get_long_name().c_str()),
                wxString{}, currentRow++,
                rTests->get_grade_point_collection()->GetContinuousColumn(GetScoreColumnName())->GetValues(),
                rTests->get_test().is_integral() ? 0 : 1, GetVarianceMethod(), true);
            }
        else if (rTests->is_included() && rTests->get_test().get_test_type() ==
            readability::readability_test_type::grade_level_and_predicted_cloze_score)
            {
            SetScoreStatsRow(m_scoreStatsData,
                BatchProjectView::FormatGradeLevelsLabel(rTests->get_test().get_long_name().c_str()),
                wxString{}, currentRow++,
                rTests->get_grade_point_collection()->GetContinuousColumn(GetScoreColumnName())->GetValues(),
                rTests->get_test().is_integral() ? 0 : 1, GetVarianceMethod(), true);
            SetScoreStatsRow(m_scoreStatsData,
                BatchProjectView::FormatClozeValuesLabel(rTests->get_test().get_long_name().c_str()),
                wxString{}, currentRow++,
                rTests->get_cloze_point_collection()->GetContinuousColumn(GetScoreColumnName())->GetValues(),
                0, GetVarianceMethod(), false);
            }
        }
    if (IsIncludingDolchSightWords())
        {
        m_scoreStatsData->SetItemText(currentRow, 0, ReadabilityMessages::GetDolchLabel());
        for (size_t j = 1; j < m_scoreStatsData->GetColumnCount(); ++j)
            { m_scoreStatsData->SetItemText(currentRow, j, _(L"N/A")); }
        ++currentRow;
        }
    // Summarize the custom tests' scores
    for(auto testVectorsPos = m_customTestScores.cbegin();
        testVectorsPos != m_customTestScores.cend();
        ++testVectorsPos)
        {
        SetScoreStatsRow(m_scoreStatsData,
            GetCustTestsInUse()[(testVectorsPos-m_customTestScores.begin())].GetTestName(),
            wxString{}, currentRow++,
            (*testVectorsPos)->GetContinuousColumn(GetScoreColumnName())->GetValues(),
            1, GetVarianceMethod(),

            (GetCustTestsInUse()[(testVectorsPos-m_customTestScores.begin())].GetIterator()->get_test_type() ==
                readability::readability_test_type::grade_level));
        }
    m_scoreStatsData->SetSize(currentRow);

    // summarize the documents' aggregated grade scores
    m_aggregatedGradeScoresData->SetNumberFormatter(GetReadabilityMessageCatalogPtr());
    m_aggregatedGradeScoresData->DeleteAllItems();
    m_aggregatedGradeScoresData->SetSize(m_docs.size(), CUMULATIVE_STATS_COUNT+2/*doc and description*/);
    currentRow = 0;
    for (std::vector<BaseProject*>::iterator pos = m_docs.begin();
         pos != m_docs.end();
         ++pos)
        {
        if ((*pos)->LoadingOriginalTextSucceeded())
            {
            SetScoreStatsRow(m_aggregatedGradeScoresData, (*pos)->GetOriginalDocumentFilePath(),
                             // a bit of a hack--need to pass in something to force the use of description column.
                             (*pos)->GetOriginalDocumentDescription().length() ?
                                (*pos)->GetOriginalDocumentDescription() : wxString{ L"  " },
                             currentRow++,
                             (*pos)->GetAggregatedGradeScores(), 1, GetVarianceMethod(), true);
            }
        }
    m_aggregatedGradeScoresData->SetSize(currentRow);

    // summarize the documents' aggregated cloze scores
    m_aggregatedClozeScoresData->SetNumberFormatter(GetReadabilityMessageCatalogPtr());
    m_aggregatedClozeScoresData->DeleteAllItems();
    m_aggregatedClozeScoresData->SetSize(m_docs.size(), CUMULATIVE_STATS_COUNT+2/*doc and description*/);
    currentRow = 0;
    for (std::vector<BaseProject*>::iterator pos = m_docs.begin();
         pos != m_docs.end();
         ++pos)
        {
        if ((*pos)->LoadingOriginalTextSucceeded())
            {
            SetScoreStatsRow(m_aggregatedClozeScoresData, (*pos)->GetOriginalDocumentFilePath(),
                             (*pos)->GetOriginalDocumentDescription().length() ?
                                (*pos)->GetOriginalDocumentDescription() : wxString{ L"  " },
                             currentRow++,
                             (*pos)->GetAggregatedClozeScores(), 1, GetVarianceMethod(), false);
            }
        }
    m_aggregatedClozeScoresData->SetSize(currentRow);
    }

//------------------------------------------------------------
void BatchProjectDoc::DisplayWarnings()
    {
    BatchProjectView* view = dynamic_cast<BatchProjectView*>(GetFirstView());

    // initialize the warnings listctrl if it doesn't have any columns in it yet
    if (view->GetWarningsView()->GetColumnCount() == 0)
        {
        view->GetWarningsView()->InsertColumn(0, _(L"Document"));
        view->GetWarningsView()->InsertColumn(1, _(L"Label"));
        view->GetWarningsView()->InsertColumn(2, _(L"Warning"));
        view->GetWarningsView()->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
        }

    view->GetWarningsView()->SetName(BaseProjectView::GetWarningLabel());
    view->GetWarningsView()->SetColumnFilePathTruncationMode(0, GetFilePathTruncationMode());
    view->GetWarningsView()->EnableGridLines();
    view->GetWarningsView()->EnableItemViewOnDblClick();
    view->GetWarningsView()->SetVirtualDataProvider(m_warnings);
    view->GetWarningsView()->SetVirtualDataSize(m_warnings->GetItemCount());
    if (view->GetWarningsView()->GetSortedColumn() == -1)
        { view->GetWarningsView()->SortColumn(0, Wisteria::SortDirection::SortAscending); }
    else
        { view->GetWarningsView()->Resort(); }
    UpdateListOptions(view->GetWarningsView());
    if (m_warnings->GetItemCount() > 0)
        {
        view->GetWarningsView()->SetColumnWidth(0, view->GetWarningsView()->EstimateColumnWidth(0));
        view->GetWarningsView()->SetColumnWidth(1,
            std::min(view->GetWarningsView()->EstimateColumnWidth(1), view->GetMaxColumnWidth()));
        view->GetWarningsView()->SetColumnWidth(2, view->GetWarningsView()->EstimateColumnWidth(2));
        }
    view->GetWarningsView()->Refresh();
    }

//------------------------------------------------------------
void BatchProjectDoc::DisplayScores()
    {
    PROFILE();
    BatchProjectView* view = dynamic_cast<BatchProjectView*>(GetFirstView());

    // main scores grid
        {
        Wisteria::UI::ListCtrlEx* listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
            view->GetScoresView().FindWindowById(BaseProjectView::ID_SCORE_LIST_PAGE_ID));
        if (!listView)
            {
            listView = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::ID_SCORE_LIST_PAGE_ID, wxDefaultPosition,
                wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetRawScoresTabLabel());
            listView->SetName(BaseProjectView::GetRawScoresTabLabel());
            listView->EnableGridLines();
            listView->EnableItemViewOnDblClick();
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
            view->GetScoresView().AddWindow(listView);
            }
        listView->DeleteAllColumns();
        listView->InsertColumn(0, _(L"Document"));
        listView->InsertColumn(1, _(L"Label"));
        listView->SetColumnFilePathTruncationMode(0, GetFilePathTruncationMode());
        // Note, the ordering of these columns must match the ordering in LoadScores()
        // add columns for the included standard tests
        for (auto rTests = GetReadabilityTests().get_tests().begin();
            rTests != GetReadabilityTests().get_tests().end();
            ++rTests)
            {
            if (rTests->is_included())
                {
                if (rTests->get_test().get_test_type() == readability::readability_test_type::grade_level ||
                    rTests->get_test().get_test_type() == readability::readability_test_type::index_value ||
                    rTests->get_test().get_test_type() == readability::readability_test_type::predicted_cloze_score)
                    {
                    listView->InsertColumn(listView->GetColumnCount(), rTests->get_test().get_short_name().c_str());
                    }
                else if (rTests->get_test().get_test_type() ==
                    readability::readability_test_type::index_value_and_grade_level)
                    {
                    listView->InsertColumn(listView->GetColumnCount(),
                        BatchProjectView::FormatIndexValuesLabel(rTests->get_test().get_short_name().c_str()));
                    listView->InsertColumn(listView->GetColumnCount(),
                        BatchProjectView::FormatGradeLevelsLabel(rTests->get_test().get_short_name().c_str()));
                    }
                else if (rTests->get_test().get_test_type() ==
                    readability::readability_test_type::grade_level_and_predicted_cloze_score)
                    {
                    listView->InsertColumn(listView->GetColumnCount(),
                        BatchProjectView::FormatGradeLevelsLabel(rTests->get_test().get_short_name().c_str()));
                    listView->InsertColumn(listView->GetColumnCount(),
                        BatchProjectView::FormatClozeValuesLabel(rTests->get_test().get_short_name().c_str()));
                    }
                }
            }

        for (auto testPos = GetCustTestsInUse().cbegin();
                testPos != GetCustTestsInUse().cend();
                ++testPos)
            {
            listView->InsertColumn(listView->GetColumnCount(), testPos->GetTestName());
            }

        listView->SetVirtualDataProvider(m_scoreRawData);
        listView->SetVirtualDataSize(m_scoreRawData->GetItemCount());
        if (listView->GetSortedColumn() == -1)
            { listView->SortColumn(0, Wisteria::SortDirection::SortAscending); }
        else
            { listView->Resort(); }
        UpdateListOptions(listView);
        listView->SetColumnWidth(0, listView->EstimateColumnWidth(0));
        for (long i = 1; i < listView->GetColumnCount(); ++i)
            {
            listView->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);
            // sometimes an error description might make a column too wide
            if (listView->GetColumnWidth(i) > view->GetMaxColumnWidth())
                { listView->SetColumnWidth(i, view->GetMaxColumnWidth()); }
            else if (listView->GetColumnWidth(i) < view->GetMaxColumnWidth()/2)
                { listView->SetColumnWidth(i, view->GetMaxColumnWidth()/2); }
            }
        }

    // add/remove the goals
    if (GetTestGoals().size() || GetStatGoals().size())
        {
        Wisteria::UI::ListCtrlEx* goalsList = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
            view->GetScoresView().FindWindowById(BaseProjectView::READABILITY_GOALS_PAGE_ID));
        if (!goalsList)
            {
            goalsList = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::READABILITY_GOALS_PAGE_ID, wxDefaultPosition,
                wxDefaultSize, wxLC_REPORT | wxLC_VIRTUAL | wxBORDER_SUNKEN);
            goalsList->Hide();
            goalsList->SetLabel(_(L"Goals"));
            goalsList->SetName(_(L"Goals"));
            goalsList->SetSortable(true);
            goalsList->EnableItemViewOnDblClick();
            goalsList->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
            goalsList->SetVirtualDataProvider(m_goalsData);
            UpdateListOptions(goalsList);
            view->GetScoresView().InsertWindow(1, goalsList);
            }
        goalsList->DeleteAllColumns();
        // add columns for all the goals x document
        goalsList->InsertColumn(0, _(L"Document"));
        goalsList->SetColumnFilePathTruncationMode(0, GetFilePathTruncationMode());
        goalsList->InsertColumn(1, _(L"Label"));
        for (const auto& goal : GetTestGoals())
            {
            if (!goal.HasGoals())
                { continue; }
            auto [sTest, found] = GetReadabilityTests().find_test(goal.GetName().c_str());
            const wxString testName =
                found ? sTest->get_test().get_long_name().c_str() : goal.GetName().c_str();

            // only include min or max goal columns if there is an actual goal for it.
            if (!std::isnan(goal.GetMinGoal()))
                {
                goalsList->InsertColumn(goalsList->GetColumnCount(),
                                        wxString::Format(
                                            // TRANSLATORS: %s is test name
                                            _(L"%s Min"), testName));
                }
            if (!std::isnan(goal.GetMaxGoal()))
                {
                goalsList->InsertColumn(goalsList->GetColumnCount(),
                                        wxString::Format(
                                            // TRANSLATORS: %s is test name
                                            _(L"%s Max"), testName));
                }
            }
        for (const auto& goal : GetStatGoals())
            {
            if (!goal.HasGoals())
                { continue; }
            const auto statGoalLabel = GetStatGoalLabels().find({ goal.GetName(), goal.GetName() });
            const wxString goalName = (statGoalLabel != GetStatGoalLabels().cend()) ?
                statGoalLabel->first.second.c_str() : goal.GetName().c_str();

            // only include min or max goal columns if there is an actual goal for it.
            if (!std::isnan(goal.GetMinGoal()))
                {
                goalsList->InsertColumn(goalsList->GetColumnCount(),
                    wxString::Format(
                        // TRANSLATORS: %s is goal name
                        _(L"%s Min"), goalName));
                }
            if (!std::isnan(goal.GetMaxGoal()))
                {
                goalsList->InsertColumn(goalsList->GetColumnCount(),
                    wxString::Format(
                        // TRANSLATORS: %s is goal name
                        _(L"%s Max"), goalName));
                }
            }
        m_goalsData->DeleteAllItems();
        m_goalsData->SetSize(m_docs.size(), goalsList->GetColumnCount());

        size_t i = 0;
        for (const auto& doc : m_docs)
            {
            if (!doc->LoadingOriginalTextSucceeded())
                { continue; }
            bool includeDoc{ false };
            // If at least one failing goal, then include the doc in the results;
            // otherwise, leave it out.
            // Because there can be so many documents in a batch,
            // it's better to only show ones that are failing
            // to make finding issues easier.
            for (const auto& goal : doc->GetTestGoals())
                {
                if (goal.HasGoals() && !goal.GetPassFailFlags().all())
                    {
                    includeDoc = true;
                    break;
                    }
                }
            for (const auto& goal : doc->GetStatGoals())
                {
                if (goal.HasGoals() && !goal.GetPassFailFlags().all())
                    {
                    includeDoc = true;
                    break;
                    }
                }
            if (!includeDoc)
                { continue; }
            long currentColumn = 0;
            goalsList->SetItemText(i, currentColumn++, doc->GetOriginalDocumentFilePath() );
            goalsList->SetItemText(i, currentColumn++, doc->GetOriginalDocumentDescription());
            for (const auto& goal : doc->GetTestGoals())
                {
                if (!goal.HasGoals())
                    { continue; }
                // fill in pass/fail
                // (if min or max goal is unspecified, then skip it because the column won't be there)
                if (!std::isnan(goal.GetMinGoal()))
                    {
                    goalsList->SetItemText(i, currentColumn++,
                        wxString::Format(L"%s %s",
                        (goal.GetPassFailFlags()[0] ?
                            BaseProjectView::GetCheckmarkEmoji() : BaseProjectView::GetWarningEmoji()),
                        wxNumberFormatter::ToString(goal.GetMinGoal(), 1,
                            wxNumberFormatter::Style::Style_NoTrailingZeroes)));
                    }
                if (!std::isnan(goal.GetMaxGoal()))
                    {
                    goalsList->SetItemText(i, currentColumn++,
                        wxString::Format(L"%s %s",
                        (goal.GetPassFailFlags()[1] ?
                            BaseProjectView::GetCheckmarkEmoji() : BaseProjectView::GetWarningEmoji()),
                        wxNumberFormatter::ToString(goal.GetMaxGoal(), 1,
                            wxNumberFormatter::Style::Style_NoTrailingZeroes)));
                    }
                }
            for (const auto& goal : doc->GetStatGoals())
                {
                if (!goal.HasGoals())
                    { continue; }
                // fill in pass/fail
                // (if min or max goal is unspecified, then skip it because the column won't be there)
                if (!std::isnan(goal.GetMinGoal()))
                    {
                    goalsList->SetItemText(i, currentColumn++,
                        wxString::Format(L"%s %s",
                        (goal.GetPassFailFlags()[0] ?
                            BaseProjectView::GetCheckmarkEmoji() : BaseProjectView::GetWarningEmoji()),
                        wxNumberFormatter::ToString(goal.GetMinGoal(), 1,
                            wxNumberFormatter::Style::Style_NoTrailingZeroes)));
                    }
                if (!std::isnan(goal.GetMaxGoal()))
                    {
                    goalsList->SetItemText(i, currentColumn++,
                        wxString::Format(L"%s %s",
                        (goal.GetPassFailFlags()[1] ?
                            BaseProjectView::GetCheckmarkEmoji() : BaseProjectView::GetWarningEmoji()),
                         wxNumberFormatter::ToString(goal.GetMaxGoal(), 1,
                             wxNumberFormatter::Style::Style_NoTrailingZeroes)));
                    }
                }
            ++i;
            }
        goalsList->SetVirtualDataSize(i);
        goalsList->DistributeColumns();
        // everything is passing, so nothing to show
        if (i == 0)
            {
            goalsList->DeleteAllColumns();
            goalsList->InsertColumn(0, _(L"Status"));
            goalsList->SetVirtualDataSize(1);
            goalsList->SetItemText(0, 0, _(L"All documents passing."));
            goalsList->SetVirtualDataSize(1);
            goalsList->DistributeColumns();
            }
        }
    else
        {
        // we are getting rid of this window (if nothing in it)
        view->GetScoresView().RemoveWindowById(BaseProjectView::READABILITY_GOALS_PAGE_ID);
        }

    DisplayScoreStatisticsWindow(_(L"Score Summary"), BaseProjectView::ID_SCORE_STATS_LIST_PAGE_ID,
        m_scoreStatsData, _(L"Test"), wxString{}, false);
    // aggregated grade level scores, listed by document
    if (IsIncludingGradeTest())
        {
        DisplayScoreStatisticsWindow(_(L"Grade Score Summary (x Document)"),
            BaseProjectView::ID_AGGREGATED_DOC_SCORES_LIST_PAGE_ID,
            m_aggregatedGradeScoresData, _(L"Document"), _(L"Label"), true);
        }
    else
        { view->GetScoresView().RemoveWindowById(BaseProjectView::ID_AGGREGATED_DOC_SCORES_LIST_PAGE_ID); }

    // aggregated predicted cloze scores, listed by document
    if (IsIncludingClozeTest())
        {
        DisplayScoreStatisticsWindow(_(L"Cloze Score Summary (x Document)"),
            BaseProjectView::ID_AGGREGATED_CLOZE_SCORES_LIST_PAGE_ID,
            m_aggregatedClozeScoresData, _(L"Document"), _(L"Label"), true);
        }
    else
        { view->GetScoresView().RemoveWindowById(BaseProjectView::ID_AGGREGATED_CLOZE_SCORES_LIST_PAGE_ID); }

    // add these here so that they are ordered after the aggregated stats
    if (view->GetCrawfordGraph() &&
        GetReadabilityTests().is_test_included(ReadabilityMessages::CRAWFORD()) && GetDocuments().size())
        { view->GetScoresView().AddWindow(view->GetCrawfordGraph()); }
    if (view->GetFleschChart() &&
        GetReadabilityTests().is_test_included(ReadabilityMessages::FLESCH()) && GetDocuments().size())
        { view->GetScoresView().AddWindow(view->GetFleschChart()); }
    if (view->GetDB2Plot() &&
        GetReadabilityTests().is_test_included(ReadabilityMessages::DANIELSON_BRYAN_2()) && GetDocuments().size())
        { view->GetScoresView().AddWindow(view->GetDB2Plot()); }
    if (view->GetFryGraph() &&
        GetReadabilityTests().is_test_included(ReadabilityMessages::FRY()) && GetDocuments().size())
        { view->GetScoresView().AddWindow(view->GetFryGraph()); }
    if (view->GetGpmFryGraph() &&
        GetReadabilityTests().is_test_included(ReadabilityMessages::GPM_FRY()) && GetDocuments().size())
        { view->GetScoresView().AddWindow(view->GetGpmFryGraph()); }
    if (view->GetFraseGraph() &&
        GetReadabilityTests().is_test_included(ReadabilityMessages::FRASE()) && GetDocuments().size())
        { view->GetScoresView().AddWindow(view->GetFraseGraph()); }
    if (view->GetSchwartzGraph() &&
        GetReadabilityTests().is_test_included(ReadabilityMessages::SCHWARTZ()) &&
        GetDocuments().size())
        { view->GetScoresView().AddWindow(view->GetSchwartzGraph()); }
    if (view->GetLixGauge() &&
        GetReadabilityTests().is_test_included(ReadabilityMessages::LIX()) && GetDocuments().size())
        { view->GetScoresView().AddWindow(view->GetLixGauge()); }
    if (view->GetGermanLixGauge() &&
        (GetReadabilityTests().is_test_included(ReadabilityMessages::LIX_GERMAN_CHILDRENS_LITERATURE()) ||
         GetReadabilityTests().is_test_included(ReadabilityMessages::LIX_GERMAN_TECHNICAL())) &&
        GetDocuments().size())
        { view->GetScoresView().AddWindow(view->GetGermanLixGauge()); }
    if (view->GetRaygorGraph() &&
        GetReadabilityTests().is_test_included(ReadabilityMessages::RAYGOR()) && GetDocuments().size())
        { view->GetScoresView().AddWindow(view->GetRaygorGraph()); }
    }

//------------------------------------------------------------
void BatchProjectDoc::DisplayScoreStatisticsWindow(
    const wxString& windowName, const int windowId,
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> data,
    const wxString& firstColumnName, const wxString& optionalSecondColumnName,
    const bool multiSelectable)
    {
    BatchProjectView* view = dynamic_cast<BatchProjectView*>(GetFirstView());
    Wisteria::UI::ListCtrlEx* listView =
        dynamic_cast<Wisteria::UI::ListCtrlEx*>(view->GetScoresView().FindWindowById(windowId));
    if (!listView)
        {
        long style = wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN;
        if (!multiSelectable)
            {
            style |= wxLC_SINGLE_SEL;
            }
        listView = new Wisteria::UI::ListCtrlEx(view->GetSplitter(), windowId, wxDefaultPosition,
                                                wxDefaultSize, style);
        listView->Hide();
        listView->SetLabel(windowName);
        listView->SetName(windowName);
        listView->EnableGridLines();
        listView->EnableItemViewOnDblClick();
        view->GetScoresView().AddWindow(listView);
        }
    listView->DeleteAllColumns();
    long currentColumnCount = 0;
    listView->InsertColumn(currentColumnCount++, firstColumnName);
    if (optionalSecondColumnName.length())
        { listView->InsertColumn(currentColumnCount++, optionalSecondColumnName); }
    listView->InsertColumn(currentColumnCount++, _(L"Valid N"));
    listView->InsertColumn(currentColumnCount++, _(L"Minimum"));
    listView->InsertColumn(currentColumnCount++, _(L"Maximum"));
    listView->InsertColumn(currentColumnCount++, _(L"Range"));
    listView->InsertColumn(currentColumnCount++, _(L"Modes"));
    listView->InsertColumn(currentColumnCount++, _(L"Means"));
    // if verbose, then add the extra columns
    if (GetStatisticsReportInfo().IsExtendedInformationEnabled())
        {
        listView->InsertColumn(currentColumnCount++, _(L"Median"));
        listView->InsertColumn(currentColumnCount++, _(L"Skewness"));
        listView->InsertColumn(currentColumnCount++, _(L"Kurtosis"));
        listView->InsertColumn(currentColumnCount++, _(L"Std. Dev."));
        listView->InsertColumn(currentColumnCount++, _(L"Variance"));
        listView->InsertColumn(currentColumnCount++, _(L"Lower Quartile"));
        listView->InsertColumn(currentColumnCount++, _(L"Upper Quartile"));
        }
    listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
    listView->SetVirtualDataProvider(data);
    listView->SetVirtualDataSize(data->GetItemCount());
    UpdateListOptions(listView);
    listView->SetColumnFilePathTruncationMode(0, GetFilePathTruncationMode());
    listView->SetColumnWidth(0, listView->EstimateColumnWidth(0));
    listView->SetColumnWidth(1, std::min(listView->EstimateColumnWidth(1), view->GetMaxColumnWidth()));
    for (long i = 2; i < listView->GetColumnCount(); ++i)
        { listView->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER); }
    if (listView->GetSortedColumn() == -1)
        { listView->SortColumn(0, Wisteria::SortDirection::SortAscending); }
    else
        { listView->Resort(); }
    }

//------------------------------------------------------------
void BatchProjectDoc::DisplayCrawfordGraph()
    {
    BatchProjectView* view = dynamic_cast<BatchProjectView*>(GetFirstView());

    const wxString scoresColumnName{ _DT(L"SCORES") };
    const wxString syllablesColumnName{ _DT(L"SYLLABLES") };
    const wxString groupColumnName{ _DT(L"GROUP") };

    auto scoreDataset = std::make_shared<Wisteria::Data::Dataset>();
    scoreDataset->AddContinuousColumn(scoresColumnName);
    scoreDataset->AddContinuousColumn(syllablesColumnName);
    scoreDataset->AddCategoricalColumn(groupColumnName, m_groupStringTable);
    scoreDataset->GetIdColumn().SetName(_DT(L"DOCS"));
    scoreDataset->Reserve(GetDocuments().size());

    for (auto pos = GetDocuments().cbegin();
        pos != GetDocuments().cend();
        ++pos)
        {
        if ((*pos)->LoadingOriginalTextSucceeded())
            {
            const double gradeValue =
                readability::crawford((*pos)->GetTotalWords(), (*pos)->GetTotalSyllables(),
                (*pos)->GetTotalSentences());
            const double syllablesPer100Words =
                (*pos)->GetTotalSyllables()*(safe_divide<double>(100,(*pos)->GetTotalWords()));

            auto foundGroupId =
                GetDocumentLabels().find((*pos)->GetOriginalDocumentDescription().wc_str());
            assert((!IsShowingGroupLegends() || foundGroupId != GetDocumentLabels().cend()) &&
                L"Could not find group label for Crawford graph!");
            scoreDataset->AddRow(Data::RowInfo().
                Id(wxFileName((*pos)->GetOriginalDocumentFilePath()).GetFullName().wc_str()).
                Categoricals({ IsShowingGroupLegends() ? foundGroupId->second : 0 }).
                Continuous({ gradeValue, syllablesPer100Words }));
            }
        }

    // Crawford Graph
    if (GetReadabilityTests().is_test_included(ReadabilityMessages::CRAWFORD()) &&
        m_docs.size())
        {
        std::shared_ptr<CrawfordGraph> crawfordGraph{ nullptr };
        Wisteria::Canvas* crawfordGraphCanvas =
            dynamic_cast<Wisteria::Canvas*>(view->GetScoresView().
                FindWindowById(BaseProjectView::CRAWFORD_GRAPH_PAGE_ID));

        if (!crawfordGraphCanvas)
            {
            crawfordGraphCanvas = new Wisteria::Canvas(view->GetSplitter(),
                                                       BaseProjectView::CRAWFORD_GRAPH_PAGE_ID);
            crawfordGraphCanvas->SetFixedObjectsGridSize(1, 1);

            crawfordGraph = std::make_shared<CrawfordGraph>(crawfordGraphCanvas,
                std::make_shared<Wisteria::Colors::Schemes::ColorScheme>(
                    *std::make_shared<Wisteria::Colors::Schemes::EarthTones>()));
            crawfordGraph->SetData(scoreDataset, scoresColumnName, syllablesColumnName,
                IsShowingGroupLegends() ?
                    std::optional<const wxString>(groupColumnName) :
                    std::nullopt);
            crawfordGraphCanvas->SetFixedObject(0, 0, crawfordGraph);
            crawfordGraphCanvas->Hide();
            view->SetCrawfordGraph(crawfordGraphCanvas);
            view->GetCrawfordGraph()->SetLabel(GetReadabilityTests().
                get_test_long_name(ReadabilityMessages::CRAWFORD()).c_str());
            view->GetCrawfordGraph()->SetName(GetReadabilityTests().
                get_test_long_name(ReadabilityMessages::CRAWFORD()).c_str());
            }
        else
            {
            crawfordGraph = std::dynamic_pointer_cast<CrawfordGraph>
                (view->GetCrawfordGraph()->GetFixedObject(0, 0));
            assert(crawfordGraph);
            crawfordGraph->SetData(scoreDataset, scoresColumnName, syllablesColumnName,
                IsShowingGroupLegends() ?
                    std::optional<const wxString>(groupColumnName) :
                    std::nullopt);
            }
        UpdateGraphOptions(view->GetCrawfordGraph());

        // add legend if grouping
        if (IsShowingGroupLegends())
            {
            view->GetCrawfordGraph()->SetFixedObjectsGridSize(1, 2);
            view->GetCrawfordGraph()->SetFixedObject(0, 1,
                crawfordGraph->CreateLegend(Wisteria::Graphs::LegendOptions().PlacementHint(
                    Wisteria::LegendCanvasPlacementHint::RightOfGraph)));
            }
        // ...and if not grouping, use light blue for points
        else
            {
            crawfordGraph->SetColorScheme(
                std::make_shared<Colors::Schemes::ColorScheme>
                 (Colors::Schemes::ColorScheme{
                     ColorBrewer::GetColor(Colors::Color::CelestialBlue) }));
            }


        wxGCDC gdc(view->GetDocFrame());
        view->GetCrawfordGraph()->CalcAllSizes(gdc);
        }
    else
        {
        view->GetScoresView().RemoveWindowById(BaseProjectView::CRAWFORD_GRAPH_PAGE_ID);
        }
    }

//------------------------------------------------------------
void BatchProjectDoc::DisplayDB2Plot()
    {
    BatchProjectView* view = dynamic_cast<BatchProjectView*>(GetFirstView());

    const wxString scoresColumnName{ _DT(L"SCORES") };
    const wxString groupColumnName{ _DT(L"GROUP") };

    auto scoreDataset = std::make_shared<Wisteria::Data::Dataset>();
    scoreDataset->AddContinuousColumn(scoresColumnName);
    scoreDataset->AddCategoricalColumn(groupColumnName, m_groupStringTable);
    scoreDataset->GetIdColumn().SetName(_DT(L"DOCS"));
    scoreDataset->Reserve(GetDocuments().size());

    for (auto pos = GetDocuments().cbegin();
        pos != GetDocuments().cend();
        ++pos)
        {
        if ((*pos)->LoadingOriginalTextSucceeded())
            {
            const auto score =
                readability::danielson_bryan_2((*pos)->GetTotalWords(),
                                               (*pos)->GetTotalCharactersPlusPunctuation(),
                                               (*pos)->GetTotalSentences());

            auto foundGroupId =
                GetDocumentLabels().find((*pos)->GetOriginalDocumentDescription().wc_str());
            assert((!IsShowingGroupLegends() || foundGroupId != GetDocumentLabels().cend()) &&
                L"Could not find group label for DB Plot!");
            scoreDataset->AddRow(Data::RowInfo().
                Id(wxFileName((*pos)->GetOriginalDocumentFilePath()).GetFullName().wc_str()).
                Categoricals({ IsShowingGroupLegends() ? foundGroupId->second : 0 }).
                Continuous({ score }));
            }
        }

    // DB2
    if (GetReadabilityTests().is_test_included(ReadabilityMessages::DANIELSON_BRYAN_2()) &&
        m_docs.size())
        {
        std::shared_ptr<DanielsonBryan2Plot> db2Plot{ nullptr };
        Wisteria::Canvas* db2PlotCanvas =
            dynamic_cast<Wisteria::Canvas*>(view->GetScoresView().
                FindWindowById(BaseProjectView::DB2_PAGE_ID));

        if (!db2PlotCanvas)
            {
            db2PlotCanvas = new Wisteria::Canvas(view->GetSplitter(),
                                                 BaseProjectView::DB2_PAGE_ID);
            db2PlotCanvas->SetFixedObjectsGridSize(1, 1);

            db2Plot = std::make_shared<DanielsonBryan2Plot>(db2PlotCanvas,
                std::make_shared<Wisteria::Colors::Schemes::ColorScheme>(
                    *std::make_shared<Wisteria::Colors::Schemes::EarthTones>()));
            db2Plot->SetData(scoreDataset, scoresColumnName,
                IsShowingGroupLegends() ?
                    std::optional<const wxString>(groupColumnName) :
                    std::nullopt);
            db2PlotCanvas->SetFixedObject(0, 0, db2Plot);
            db2PlotCanvas->Hide();
            view->SetDB2Plot(db2PlotCanvas);
            view->GetDB2Plot()->SetLabel(GetReadabilityTests().
                get_test_long_name(ReadabilityMessages::DANIELSON_BRYAN_2()).c_str());
            view->GetDB2Plot()->SetName(GetReadabilityTests().
                get_test_long_name(ReadabilityMessages::DANIELSON_BRYAN_2()).c_str());
            }
        else
            {
            db2Plot = std::dynamic_pointer_cast<DanielsonBryan2Plot>
                (view->GetDB2Plot()->GetFixedObject(0, 0));
            assert(db2Plot);
            db2Plot->SetData(scoreDataset, scoresColumnName,
                IsShowingGroupLegends() ?
                    std::optional<const wxString>(groupColumnName) :
                    std::nullopt);
            }
        UpdateGraphOptions(view->GetDB2Plot());

        // add legend if grouping
        if (IsShowingGroupLegends())
            {
            view->GetDB2Plot()->SetFixedObjectsGridSize(1, 2);
            view->GetDB2Plot()->SetFixedObject(0, 1,
                db2Plot->CreateLegend(Wisteria::Graphs::LegendOptions().PlacementHint(
                    Wisteria::LegendCanvasPlacementHint::RightOfGraph)));
            }
        // ...and if not grouping, use light blue for points
        else
            {
            db2Plot->SetColorScheme(
                std::make_shared<Colors::Schemes::ColorScheme>
                 (Colors::Schemes::ColorScheme{
                     ColorBrewer::GetColor(Colors::Color::CelestialBlue) }));
            }

        wxGCDC gdc(view->GetDocFrame());
        view->GetDB2Plot()->CalcAllSizes(gdc);
        }
    else
        {
        view->GetScoresView().RemoveWindowById(BaseProjectView::DB2_PAGE_ID);
        }
    }

//------------------------------------------------------------
void BatchProjectDoc::DisplayFleschChart()
    {
    BatchProjectView* view = dynamic_cast<BatchProjectView*>(GetFirstView());

    const wxString wordsColumnName{ _DT(L"WORDS") };
    const wxString scoresColumnName{ _DT(L"SCORES") };
    const wxString syllablesColumnName{ _DT(L"SYLLABLES") };
    const wxString groupColumnName{ _DT(L"GROUP") };

    auto scoreDataset = std::make_shared<Wisteria::Data::Dataset>();
    scoreDataset->AddContinuousColumn(wordsColumnName);
    scoreDataset->AddContinuousColumn(scoresColumnName);
    scoreDataset->AddContinuousColumn(syllablesColumnName);
    scoreDataset->AddCategoricalColumn(groupColumnName, m_groupStringTable);
    scoreDataset->GetIdColumn().SetName(_DT(L"DOCS"));
    scoreDataset->Reserve(GetDocuments().size());

    for (auto pos = GetDocuments().cbegin();
        pos != GetDocuments().cend();
        ++pos)
        {
        if ((*pos)->LoadingOriginalTextSucceeded())
            {
            const double ASL = safe_divide<double>((*pos)->GetTotalWords(), (*pos)->GetTotalSentences());
            const double ASW =
                safe_divide<double>(((*pos)->GetFleschNumeralSyllabizeMethod() ==
                                    FleschNumeralSyllabize::NumeralIsOneSyllable) ?
                            (*pos)->GetTotalSyllablesNumeralsOneSyllable() : (*pos)->GetTotalSyllables(),
                (*pos)->GetTotalWords());
            readability::flesch_difficulty diffLevel;
            const size_t score =
                readability::flesch_reading_ease((*pos)->GetTotalWords(),
                ((*pos)->GetFleschNumeralSyllabizeMethod() == FleschNumeralSyllabize::NumeralIsOneSyllable) ?
                    (*pos)->GetTotalSyllablesNumeralsOneSyllable() : (*pos)->GetTotalSyllables(),
                (*pos)->GetTotalSentences(),
                diffLevel);

            auto foundGroupId =
                GetDocumentLabels().find((*pos)->GetOriginalDocumentDescription().wc_str());
            assert((!IsShowingGroupLegends() || foundGroupId != GetDocumentLabels().cend()) &&
                L"Could not find group label for Flesch Chart!");
            scoreDataset->AddRow(Data::RowInfo().
                Id(wxFileName((*pos)->GetOriginalDocumentFilePath()).GetFullName().wc_str()).
                Categoricals({ IsShowingGroupLegends() ? foundGroupId->second : 0 }).
                Continuous({ ASL, static_cast<double>(score), ASW }));
            }
        }

    // Flesch Chart
    if (GetReadabilityTests().is_test_included(ReadabilityMessages::FLESCH()) &&
        m_docs.size())
        {
        std::shared_ptr<FleschChart> fleschChart{ nullptr };
        Wisteria::Canvas* fleschChartCanvas =
            dynamic_cast<Wisteria::Canvas*>(view->GetScoresView().
                FindWindowById(BaseProjectView::FLESCH_CHART_PAGE_ID));

        // document name brackets next to syllable ruler will
        // will override the legend. (It would be to busy showing both of these.)
        const bool showLegend = (IsShowingGroupLegends() && !IsIncludingFleschRulerDocGroups());

        if (!fleschChartCanvas)
            {
            fleschChartCanvas = new Wisteria::Canvas(view->GetSplitter(),
                                                     BaseProjectView::FLESCH_CHART_PAGE_ID);
            fleschChartCanvas->SetFixedObjectsGridSize(1, 1);

            fleschChart =
                std::make_shared<FleschChart>(fleschChartCanvas,
                    std::make_shared<Wisteria::Colors::Schemes::ColorScheme>(
                    *std::make_shared<Wisteria::Colors::Schemes::EarthTones>()));
            fleschChart->SetData(scoreDataset, wordsColumnName,
                scoresColumnName, syllablesColumnName,
                showLegend ?
                    std::optional<const wxString>(groupColumnName) :
                    std::nullopt,
                IsIncludingFleschRulerDocGroups());
            fleschChartCanvas->SetFixedObject(0, 0, fleschChart);
            fleschChartCanvas->Hide();
            view->SetFleschChart(fleschChartCanvas);
            view->GetFleschChart()->SetLabel(GetReadabilityTests().
                get_test_long_name(ReadabilityMessages::FLESCH()).c_str());
            view->GetFleschChart()->SetName(GetReadabilityTests().
                get_test_long_name(ReadabilityMessages::FLESCH()).c_str());
            }
        else
            {
            fleschChart = std::dynamic_pointer_cast<FleschChart>
                (view->GetFleschChart()->GetFixedObject(0, 0));
            assert(fleschChart);
            fleschChart->SetData(scoreDataset, wordsColumnName,
                scoresColumnName, syllablesColumnName,
                showLegend ?
                    std::optional<const wxString>(groupColumnName) :
                    std::nullopt,
                IsIncludingFleschRulerDocGroups());
            }
        assert(fleschChart);
        UpdateGraphOptions(view->GetFleschChart());

        fleschChart->ShowConnectionLine(IsConnectingFleschPoints());

        if (showLegend)
            {
            view->GetFleschChart()->SetFixedObjectsGridSize(1, 2);
            view->GetFleschChart()->SetFixedObject(0, 1,
                fleschChart->CreateLegend(Wisteria::Graphs::LegendOptions().PlacementHint(
                    Wisteria::LegendCanvasPlacementHint::RightOfGraph)));
            }

        wxGCDC gdc(view->GetDocFrame());
        view->GetFleschChart()->CalcAllSizes(gdc);
        }
    else
        {
        view->GetScoresView().RemoveWindowById(BaseProjectView::FLESCH_CHART_PAGE_ID);
        }
    }

//------------------------------------------------------------
void BatchProjectDoc::DisplayGermanLixGauge()
    {
    BatchProjectView* view = dynamic_cast<BatchProjectView*>(GetFirstView());

    const wxString scoresColumnName{ _DT(L"SCORES") };
    const wxString groupColumnName{ _DT(L"GROUP") };

    auto scoreDataset = std::make_shared<Wisteria::Data::Dataset>();
    scoreDataset->AddContinuousColumn(scoresColumnName);
    scoreDataset->AddCategoricalColumn(groupColumnName, m_groupStringTable);
    scoreDataset->GetIdColumn().SetName(_DT(L"DOCS"));
    scoreDataset->Reserve(GetDocuments().size());

    for (auto pos = GetDocuments().cbegin();
        pos != GetDocuments().cend();
        ++pos)
        {
        if ((*pos)->LoadingOriginalTextSucceeded())
            {
            readability::german_lix_difficulty diffLevel;
            const size_t score = readability::german_lix(diffLevel, (*pos)->GetTotalWords(),
                                                            (*pos)->GetTotalHardLixRixWords(),
                                                            (*pos)->GetTotalSentences() );

            auto foundGroupId =
                GetDocumentLabels().find((*pos)->GetOriginalDocumentDescription().wc_str());
            assert((!IsShowingGroupLegends() || foundGroupId != GetDocumentLabels().cend()) &&
                L"Could not find group label for German Lix gauge!");
            scoreDataset->AddRow(Data::RowInfo().
                Id(wxFileName((*pos)->GetOriginalDocumentFilePath()).GetFullName().wc_str()).
                Categoricals({ IsShowingGroupLegends() ? foundGroupId->second : 0 }).
                Continuous({ static_cast<double>(score) }));
            }
        }

    // German Lix Gauge
    if ((GetReadabilityTests().is_test_included(ReadabilityMessages::LIX_GERMAN_CHILDRENS_LITERATURE()) ||
         GetReadabilityTests().is_test_included(ReadabilityMessages::LIX_GERMAN_TECHNICAL()) ) &&
         m_docs.size())
        {
        std::shared_ptr<LixGaugeGerman> lixGauge{ nullptr };
        Wisteria::Canvas* lixGaugeCanvas =
            dynamic_cast<Wisteria::Canvas*>(view->GetScoresView().
                FindWindowById(BaseProjectView::LIX_GAUGE_GERMAN_PAGE_ID));

        if (!lixGaugeCanvas)
            {
            lixGaugeCanvas = new Wisteria::Canvas(view->GetSplitter(),
                                                  BaseProjectView::LIX_GAUGE_GERMAN_PAGE_ID);
            lixGaugeCanvas->SetFixedObjectsGridSize(1, 1);

            lixGauge = std::make_shared<LixGaugeGerman>(lixGaugeCanvas,
                std::make_shared<Wisteria::Colors::Schemes::ColorScheme>(
                    *std::make_shared<Wisteria::Colors::Schemes::EarthTones>()));
            lixGauge->SetData(scoreDataset, scoresColumnName,
                IsShowingGroupLegends() ?
                    std::optional<const wxString>(groupColumnName) :
                    std::nullopt);
            lixGaugeCanvas->SetFixedObject(0, 0, lixGauge);
            lixGaugeCanvas->Hide();
            view->SetGermanLixGauge(lixGaugeCanvas);
            view->GetGermanLixGauge()->SetLabel(BaseProjectView::GetGermanLixGaugeLabel());
            view->GetGermanLixGauge()->SetName(BaseProjectView::GetGermanLixGaugeLabel());
            }
        else
            {
            lixGauge = std::dynamic_pointer_cast<LixGaugeGerman>
                (view->GetGermanLixGauge()->GetFixedObject(0, 0));
            assert(lixGauge);
            lixGauge->SetData(scoreDataset, scoresColumnName,
                IsShowingGroupLegends() ?
                    std::optional<const wxString>(groupColumnName) :
                    std::nullopt);
            }
        assert(lixGauge);
        lixGauge->UseEnglishLabels(IsUsingEnglishLabelsForGermanLix());
        UpdateGraphOptions(view->GetGermanLixGauge());

        if (IsShowingGroupLegends())
            {
            view->GetGermanLixGauge()->SetFixedObjectsGridSize(1, 2);
            view->GetGermanLixGauge()->SetFixedObject(0, 1,
                lixGauge->CreateLegend(Wisteria::Graphs::LegendOptions().PlacementHint(
                    Wisteria::LegendCanvasPlacementHint::RightOfGraph)));
            }
        else
            {
            lixGauge->SetColorScheme(
                std::make_shared<Colors::Schemes::ColorScheme>
                 (Colors::Schemes::ColorScheme{
                     ColorBrewer::GetColor(Colors::Color::CelestialBlue) }));
            }

        wxGCDC gdc(view->GetDocFrame());
        view->GetGermanLixGauge()->CalcAllSizes(gdc);
        }
    else
        {
        view->GetScoresView().RemoveWindowById(BaseProjectView::LIX_GAUGE_GERMAN_PAGE_ID);
        }
    }

//------------------------------------------------------------
void BatchProjectDoc::DisplayLixGauge()
    {
    BatchProjectView* view = dynamic_cast<BatchProjectView*>(GetFirstView());

    const wxString scoresColumnName{ _DT(L"SCORES") };
    const wxString groupColumnName{ _DT(L"GROUP") };

    auto scoreDataset = std::make_shared<Wisteria::Data::Dataset>();
    scoreDataset->AddContinuousColumn(scoresColumnName);
    scoreDataset->AddCategoricalColumn(groupColumnName, m_groupStringTable);
    scoreDataset->GetIdColumn().SetName(_DT(L"DOCS"));
    scoreDataset->Reserve(GetDocuments().size());

    for (auto pos = GetDocuments().cbegin();
        pos != GetDocuments().cend();
        ++pos)
        {
        if ((*pos)->LoadingOriginalTextSucceeded())
            {
            readability::lix_difficulty diffLevel;
            size_t gradeLevel{ 1 };
            const size_t score = readability::lix(diffLevel, gradeLevel, (*pos)->GetTotalWords(),
                                                  (*pos)->GetTotalHardLixRixWords(), (*pos)->GetTotalSentences() );
            auto foundGroupId =
                GetDocumentLabels().find((*pos)->GetOriginalDocumentDescription().wc_str());
            assert((!IsShowingGroupLegends() || foundGroupId != GetDocumentLabels().cend()) &&
                L"Could not find group label for Lix Gauge!");
            scoreDataset->AddRow(Data::RowInfo().
                Id(wxFileName((*pos)->GetOriginalDocumentFilePath()).GetFullName().wc_str()).
                Categoricals({ IsShowingGroupLegends() ? foundGroupId->second : 0 }).
                Continuous({ static_cast<double>(score) }));
            }
        }

    // Lix Gauge
    if (GetReadabilityTests().is_test_included(ReadabilityMessages::LIX()) && m_docs.size())
        {
        std::shared_ptr<LixGauge> lixGauge{ nullptr };
        Wisteria::Canvas* lixGaugeCanvas =
        dynamic_cast<Wisteria::Canvas*>(view->GetScoresView().
            FindWindowById(BaseProjectView::LIX_GAUGE_PAGE_ID));

        if (!lixGaugeCanvas)
            {
            lixGaugeCanvas = new Wisteria::Canvas(view->GetSplitter(),
                                                  BaseProjectView::LIX_GAUGE_PAGE_ID);
            lixGaugeCanvas->SetFixedObjectsGridSize(1, 1);

            lixGauge = std::make_shared<LixGauge>(lixGaugeCanvas,
                std::make_shared<Wisteria::Colors::Schemes::ColorScheme>(
                    *std::make_shared<Wisteria::Colors::Schemes::EarthTones>()));
            lixGauge->SetData(scoreDataset, scoresColumnName,
                IsShowingGroupLegends() ?
                    std::optional<const wxString>(groupColumnName) :
                    std::nullopt);

            lixGaugeCanvas->SetFixedObject(0, 0, lixGauge);
            lixGaugeCanvas->Hide();
            view->SetLixGauge(lixGaugeCanvas);
            view->GetLixGauge()->SetLabel(
                GetReadabilityTests().get_test_long_name(ReadabilityMessages::LIX()).c_str());
            view->GetLixGauge()->SetName(
                GetReadabilityTests().get_test_long_name(ReadabilityMessages::LIX()).c_str());
            }
        else
            {
            lixGauge = std::dynamic_pointer_cast<LixGauge>
                (view->GetLixGauge()->GetFixedObject(0, 0));
            assert(lixGauge);
            lixGauge->SetData(scoreDataset, scoresColumnName,
                IsShowingGroupLegends() ?
                    std::optional<const wxString>(groupColumnName) :
                    std::nullopt);
            }
        UpdateGraphOptions(view->GetLixGauge());

        if (IsShowingGroupLegends())
            {
            view->GetLixGauge()->SetFixedObjectsGridSize(1, 2);
            view->GetLixGauge()->SetFixedObject(0, 1,
                lixGauge->CreateLegend(Wisteria::Graphs::LegendOptions().PlacementHint(
                    Wisteria::LegendCanvasPlacementHint::RightOfGraph)));
            }
        else
            {
            lixGauge->SetColorScheme(
                std::make_shared<Colors::Schemes::ColorScheme>
                 (Colors::Schemes::ColorScheme{
                     ColorBrewer::GetColor(Colors::Color::CelestialBlue) }));
            }

        wxGCDC gdc(view->GetDocFrame());
        view->GetLixGauge()->CalcAllSizes(gdc);
        }
    else
        {
        view->GetScoresView().RemoveWindowById(BaseProjectView::LIX_GAUGE_PAGE_ID);
        }
    }

//------------------------------------------------------------
void BatchProjectDoc::DisplayReadabilityGraphs()
    {
    PROFILE();
    DisplayFleschChart();
    DisplayDB2Plot();
    DisplayCrawfordGraph();
    DisplayLixGauge();
    DisplayGermanLixGauge();

    const wxString groupColumnName{ _DT(L"GROUP") };
    // columns for Schwartz calculations
    const wxString totalWordsColumnName{ _DT(L"TOTALWORDS") };
    const wxString totalSyllablesNumeralsOneSyllableColumnName
        { _DT(L"TOTALSYLLABLESNUMERALSONESYLLABLE") };
    const wxString totalSentenceUnitsColumnName{ _DT(L"TOTALSENTENCEUNITS") };
    // columns for FRASE calculations
    const wxString totalSyllablesColumnName{ _DT(L"TOTALSYLLABLES") };
    const wxString totalSentencesColumnName{ _DT(L"TOTALSENTENCES") };
    // Fry
    const wxString totalSyllablesNumeralsFullySyllabizedColumnName
        { _DT(L"TOTALSYLLABLESNUMERALSFULLYSYLLABIZED") };
    // Raygor
    const wxString totalWordsLessNumerals{ _DT(L"TOTALWORDSLESSNUMERALS") };
    const wxString total6PlusCharWordsLessNumerals{ _DT(L"TOTAL6PLUSCHARWORDSLESSNUMERALS") };

    auto scoreDataset = std::make_shared<Wisteria::Data::Dataset>();

    scoreDataset->GetIdColumn().SetName(_DT(L"DOCS"));
    // do NOT change the ordering here
    scoreDataset->AddContinuousColumn(totalWordsColumnName);
    scoreDataset->AddContinuousColumn(totalSyllablesNumeralsOneSyllableColumnName);
    scoreDataset->AddContinuousColumn(totalSentenceUnitsColumnName);
    scoreDataset->AddContinuousColumn(totalSyllablesColumnName);
    scoreDataset->AddContinuousColumn(totalSentencesColumnName);
    scoreDataset->AddContinuousColumn(totalSyllablesNumeralsFullySyllabizedColumnName);
    scoreDataset->AddContinuousColumn(totalWordsLessNumerals);
    scoreDataset->AddContinuousColumn(total6PlusCharWordsLessNumerals);

    scoreDataset->AddCategoricalColumn(groupColumnName, m_groupStringTable);
    scoreDataset->Reserve(GetDocuments().size());

    for (const auto doc : GetDocuments())
        {
        if (doc->LoadingOriginalTextSucceeded())
            {
            auto foundGroupId =
                GetDocumentLabels().find(doc->GetOriginalDocumentDescription().wc_str());
            assert((!IsShowingGroupLegends() || foundGroupId != GetDocumentLabels().cend()) &&
                L"Could not find group label!");
            scoreDataset->AddRow(Data::RowInfo().
                Id(wxFileName(doc->GetOriginalDocumentFilePath()).GetFullName()).
                Categoricals(
                    { IsShowingGroupLegends() ? foundGroupId->second : 0 }).
                // do NOT change the ordering here
                Continuous(
                    {
                    doc->GetTotalWords(),
                    doc->GetTotalSyllablesNumeralsOneSyllable(),
                    doc->GetTotalSentenceUnits(),
                    doc->GetTotalSyllables(),
                    doc->GetTotalSentences(),
                    doc->GetTotalSyllablesNumeralsFullySyllabized(),
                    doc->GetTotalWords() - doc->GetTotalNumerals(),
                    doc->GetTotalSixPlusCharacterWordsIgnoringNumerals()
                    }));
            }
        }

    BatchProjectView* view = dynamic_cast<BatchProjectView*>(GetFirstView());

    // Fry graph
    Wisteria::Canvas* fryGraphCanvas =
        dynamic_cast<Wisteria::Canvas*>(view->GetScoresView().
            FindWindowById(BaseProjectView::FRY_PAGE_ID));
    if (GetReadabilityTests().is_test_included(ReadabilityMessages::FRY()) &&
        GetDocuments().size())
        {
        std::shared_ptr<FryGraph> fryGraph{ nullptr };
        if (!fryGraphCanvas)
            {
            fryGraphCanvas = new Wisteria::Canvas(view->GetSplitter(),
                                                  BaseProjectView::FRY_PAGE_ID);

            fryGraph = std::make_shared<FryGraph>(fryGraphCanvas,
                FryGraph::FryGraphType::Traditional);
            fryGraph->SetMessageCatalog(GetReadabilityMessageCatalogPtr());
            fryGraph->SetData(scoreDataset, totalWordsColumnName,
                totalSyllablesNumeralsFullySyllabizedColumnName,
                totalSentencesColumnName,
                IsShowingGroupLegends() ?
                    std::optional<const wxString>(groupColumnName) :
                    std::nullopt);

            fryGraphCanvas->SetFixedObjectsGridSize(1, 1);
            fryGraphCanvas->SetFixedObject(0, 0, fryGraph);
            fryGraphCanvas->Hide();
            view->SetFryGraph(fryGraphCanvas);
            view->GetFryGraph()->SetLabel(GetReadabilityTests().
                get_test_long_name(ReadabilityMessages::FRY()).c_str());
            view->GetFryGraph()->SetName(GetReadabilityTests().
                get_test_long_name(ReadabilityMessages::FRY()).c_str());
            }
        else
            {
            fryGraph = std::dynamic_pointer_cast<FryGraph>
                (view->GetFryGraph()->GetFixedObject(0, 0));
            assert(fryGraph);
            fryGraph->SetData(scoreDataset, totalWordsColumnName,
                totalSyllablesNumeralsFullySyllabizedColumnName,
                totalSentencesColumnName,
                IsShowingGroupLegends() ?
                    std::optional<const wxString>(groupColumnName) :
                    std::nullopt);
            }
        UpdateGraphOptions(view->GetFryGraph());
        fryGraph->SetInvalidAreaColor(GetInvalidAreaColor());

        // add legend if grouping
        if (IsShowingGroupLegends())
            {
            view->GetFryGraph()->SetFixedObjectsGridSize(1, 2);
            view->GetFryGraph()->SetFixedObject(0, 1,
                fryGraph->CreateLegend(Wisteria::Graphs::LegendOptions().PlacementHint(
                    Wisteria::LegendCanvasPlacementHint::RightOfGraph)));
            }
        // ...and if not grouping, use light blue for points
        else
            {
            fryGraph->SetColorScheme(
                std::make_shared<Colors::Schemes::ColorScheme>
                 (Colors::Schemes::ColorScheme{
                     ColorBrewer::GetColor(Colors::Color::CelestialBlue) }));
            }

        wxGCDC gdc(view->GetDocFrame());
        view->GetFryGraph()->CalcAllSizes(gdc);
        }
    else
        {
        view->GetScoresView().RemoveWindowById(BaseProjectView::FRY_PAGE_ID);
        view->SetFryGraph(nullptr);
        }

    // GPM Fry graph
    fryGraphCanvas =
        dynamic_cast<Wisteria::Canvas*>(view->GetScoresView().
            FindWindowById(BaseProjectView::GPM_FRY_PAGE_ID));
    if (GetReadabilityTests().is_test_included(ReadabilityMessages::GPM_FRY()) &&
        GetDocuments().size())
        {
        std::shared_ptr<FryGraph> gFryGraph{ nullptr };
        if (!fryGraphCanvas)
            {
            fryGraphCanvas = new Wisteria::Canvas(view->GetSplitter(),
                                                  BaseProjectView::GPM_FRY_PAGE_ID);

            gFryGraph = std::make_shared<FryGraph>(fryGraphCanvas,
                FryGraph::FryGraphType::GPM);
            gFryGraph->SetMessageCatalog(GetReadabilityMessageCatalogPtr());
            gFryGraph->SetData(scoreDataset, totalWordsColumnName,
                totalSyllablesNumeralsFullySyllabizedColumnName,
                totalSentencesColumnName,
                IsShowingGroupLegends() ?
                    std::optional<const wxString>(groupColumnName) :
                    std::nullopt);

            fryGraphCanvas->SetFixedObjectsGridSize(1, 1);
            fryGraphCanvas->SetFixedObject(0, 0, gFryGraph);
            fryGraphCanvas->Hide();
            view->SetGpmFryGraph(fryGraphCanvas);
            view->GetGpmFryGraph()->SetLabel(GetReadabilityTests().
                get_test_long_name(ReadabilityMessages::GPM_FRY()).c_str());
            view->GetGpmFryGraph()->SetName(GetReadabilityTests().
                get_test_long_name(ReadabilityMessages::GPM_FRY()).c_str());
            }
        else
            {
            gFryGraph = std::dynamic_pointer_cast<FryGraph>
                (view->GetGpmFryGraph()->GetFixedObject(0, 0));
            assert(gFryGraph);
            gFryGraph->SetData(scoreDataset, totalWordsColumnName,
                totalSyllablesNumeralsFullySyllabizedColumnName,
                totalSentencesColumnName,
                IsShowingGroupLegends() ?
                    std::optional<const wxString>(groupColumnName) :
                    std::nullopt);
            }
        UpdateGraphOptions(view->GetGpmFryGraph());
        gFryGraph->SetInvalidAreaColor(GetInvalidAreaColor());

        // add legend if grouping
        if (IsShowingGroupLegends())
            {
            view->GetGpmFryGraph()->SetFixedObjectsGridSize(1, 2);
            view->GetGpmFryGraph()->SetFixedObject(0, 1,
                gFryGraph->CreateLegend(Wisteria::Graphs::LegendOptions().PlacementHint(
                    Wisteria::LegendCanvasPlacementHint::RightOfGraph)));
            }
        // ...and if not grouping, use light blue for points
        else
            {
            gFryGraph->SetColorScheme(
                std::make_shared<Colors::Schemes::ColorScheme>
                 (Colors::Schemes::ColorScheme{
                     ColorBrewer::GetColor(Colors::Color::CelestialBlue) }));
            }

        wxGCDC gdc(view->GetDocFrame());
        view->GetGpmFryGraph()->CalcAllSizes(gdc);
        }
    else
        {
        view->GetScoresView().RemoveWindowById(BaseProjectView::GPM_FRY_PAGE_ID);
        view->SetGpmFryGraph(nullptr);
        }

    // Schwartz graph
    Wisteria::Canvas* schwartzGraphCanvas =
        dynamic_cast<Wisteria::Canvas*>(view->GetScoresView().
            FindWindowById(BaseProjectView::SCHWARTZ_PAGE_ID));
    if (GetReadabilityTests().is_test_included(ReadabilityMessages::SCHWARTZ()) &&
        GetDocuments().size())
        {
        std::shared_ptr<SchwartzGraph> schwartzGraph{ nullptr };
        if (!schwartzGraphCanvas)
            {
            schwartzGraphCanvas = new Wisteria::Canvas(view->GetSplitter(),
                                                       BaseProjectView::SCHWARTZ_PAGE_ID);

            schwartzGraph = std::make_shared<SchwartzGraph>(schwartzGraphCanvas);
            schwartzGraph->SetMessageCatalog(GetReadabilityMessageCatalogPtr());
            schwartzGraph->SetData(scoreDataset, totalWordsColumnName,
                totalSyllablesNumeralsOneSyllableColumnName,
                totalSentenceUnitsColumnName,
                IsShowingGroupLegends() ?
                    std::optional<const wxString>(groupColumnName) :
                    std::nullopt);

            schwartzGraphCanvas->SetFixedObjectsGridSize(1, 1);
            schwartzGraphCanvas->SetFixedObject(0, 0, schwartzGraph);
            schwartzGraphCanvas->Hide();
            view->SetSchwartzGraph(schwartzGraphCanvas);
            view->GetSchwartzGraph()->SetLabel(
                GetReadabilityTests().get_test_long_name(
                    ReadabilityMessages::SCHWARTZ()).c_str());
            view->GetSchwartzGraph()->SetName(
                GetReadabilityTests().get_test_long_name(
                    ReadabilityMessages::SCHWARTZ()).c_str());
            }
        else
            {
            schwartzGraph = std::dynamic_pointer_cast<SchwartzGraph>
                (view->GetSchwartzGraph()->GetFixedObject(0, 0));
            assert(schwartzGraph);
            schwartzGraph->SetData(scoreDataset, totalWordsColumnName,
                totalSyllablesNumeralsOneSyllableColumnName,
                totalSentenceUnitsColumnName,
                IsShowingGroupLegends() ?
                    std::optional<const wxString>(groupColumnName) :
                    std::nullopt);
            }
        UpdateGraphOptions(view->GetSchwartzGraph());

        schwartzGraph->SetInvalidAreaColor(GetInvalidAreaColor());

        // add legend if grouping
        if (IsShowingGroupLegends())
            {
            view->GetSchwartzGraph()->SetFixedObjectsGridSize(1, 2);
            view->GetSchwartzGraph()->SetFixedObject(0, 1,
                schwartzGraph->CreateLegend(Wisteria::Graphs::LegendOptions().PlacementHint(
                    Wisteria::LegendCanvasPlacementHint::RightOfGraph)));
            }
        // ...and if not grouping, use light blue for points
        else
            {
            schwartzGraph->SetColorScheme(
                std::make_shared<Colors::Schemes::ColorScheme>
                 (Colors::Schemes::ColorScheme{
                     ColorBrewer::GetColor(Colors::Color::CelestialBlue) }));
            }

        wxGCDC gdc(view->GetDocFrame());
        view->GetSchwartzGraph()->CalcAllSizes(gdc);
        }
    else
        {
        view->GetScoresView().RemoveWindowById(BaseProjectView::SCHWARTZ_PAGE_ID);
        view->SetSchwartzGraph(nullptr);
        }

    // FRASE graph
    Wisteria::Canvas* fraseGraphCanvas =
        dynamic_cast<Wisteria::Canvas*>(view->GetScoresView().
            FindWindowById(BaseProjectView::FRASE_PAGE_ID));
    if (GetReadabilityTests().is_test_included(ReadabilityMessages::FRASE()) &&
        GetDocuments().size())
        {
        std::shared_ptr<FraseGraph> fraseGraph{ nullptr };
        if (!fraseGraphCanvas)
            {
            fraseGraphCanvas = new Wisteria::Canvas(view->GetSplitter(),
                                                    BaseProjectView::FRASE_PAGE_ID);

            fraseGraph = std::make_shared<FraseGraph>(fraseGraphCanvas);
            fraseGraph->SetMessageCatalog(GetReadabilityMessageCatalogPtr());
            fraseGraph->SetData(scoreDataset, totalWordsColumnName,
                totalSyllablesColumnName,
                totalSentencesColumnName,
                IsShowingGroupLegends() ?
                    std::optional<const wxString>(groupColumnName) :
                    std::nullopt);

            fraseGraphCanvas->SetFixedObjectsGridSize(1, 1);
            fraseGraphCanvas->SetFixedObject(0, 0, fraseGraph);
            fraseGraphCanvas->Hide();
            view->SetFraseGraph(fraseGraphCanvas);
            view->GetFraseGraph()->SetLabel(
                GetReadabilityTests().get_test_long_name(
                    ReadabilityMessages::FRASE()).c_str());
            view->GetFraseGraph()->SetName(
                GetReadabilityTests().get_test_long_name(
                    ReadabilityMessages::FRASE()).c_str());
            }
        else
            {
            fraseGraph = std::dynamic_pointer_cast<FraseGraph>
                (view->GetFraseGraph()->GetFixedObject(0, 0));
            assert(fraseGraph);
            fraseGraph->SetData(scoreDataset, totalWordsColumnName,
                totalSyllablesColumnName,
                totalSentencesColumnName,
                IsShowingGroupLegends() ?
                    std::optional<const wxString>(groupColumnName) :
                    std::nullopt);
            }
        UpdateGraphOptions(view->GetFraseGraph());

        // add legend if grouping
        if (IsShowingGroupLegends())
            {
            view->GetFraseGraph()->SetFixedObjectsGridSize(1, 2);
            view->GetFraseGraph()->SetFixedObject(0, 1,
                fraseGraph->CreateLegend(Wisteria::Graphs::LegendOptions().PlacementHint(
                    Wisteria::LegendCanvasPlacementHint::RightOfGraph)));
            }
        // ...and if not grouping, use light blue for points
        else
            {
            fraseGraph->SetColorScheme(
                std::make_shared<Colors::Schemes::ColorScheme>
                 (Colors::Schemes::ColorScheme{
                     ColorBrewer::GetColor(Colors::Color::CelestialBlue) }));
            }

        wxGCDC gdc(view->GetDocFrame());
        view->GetFraseGraph()->CalcAllSizes(gdc);
        }
    else
        {
        view->GetScoresView().RemoveWindowById(BaseProjectView::FRASE_PAGE_ID);
        view->SetFraseGraph(nullptr);
        }

    // Raygor graph
    Wisteria::Canvas* raygorGraphCanvas =
        dynamic_cast<Wisteria::Canvas*>(view->GetScoresView().
            FindWindowById(BaseProjectView::RAYGOR_PAGE_ID));
    if (GetReadabilityTests().is_test_included(ReadabilityMessages::RAYGOR()) &&
         GetDocuments().size())
        {
        std::shared_ptr<RaygorGraph> raygorGraph{ nullptr };
        if (!raygorGraphCanvas)
            {
            raygorGraphCanvas = new Wisteria::Canvas(view->GetSplitter(),
                                                     BaseProjectView::RAYGOR_PAGE_ID);

            raygorGraphCanvas->SetFixedObjectsGridSize(1, 1);

            raygorGraph = std::make_shared<RaygorGraph>(raygorGraphCanvas);
            raygorGraph->SetMessageCatalog(GetReadabilityMessageCatalogPtr());
            raygorGraph->SetData(scoreDataset, totalWordsLessNumerals,
                total6PlusCharWordsLessNumerals,
                totalSentencesColumnName,
                IsShowingGroupLegends() ?
                    std::optional<const wxString>(groupColumnName) :
                    std::nullopt);

            raygorGraphCanvas->SetFixedObject(0, 0, raygorGraph);
            raygorGraphCanvas->Hide();
            view->SetRaygorGraph(raygorGraphCanvas);
            view->GetRaygorGraph()->SetLabel(GetReadabilityTests().
                get_test_long_name(ReadabilityMessages::RAYGOR()).c_str());
            view->GetRaygorGraph()->SetName(
                GetReadabilityTests().get_test_long_name(ReadabilityMessages::RAYGOR()).c_str());
            }
        else
            {
            raygorGraph = std::dynamic_pointer_cast<RaygorGraph>
                (view->GetRaygorGraph()->GetFixedObject(0, 0));
            assert(raygorGraph);
            raygorGraph->SetData(scoreDataset, totalWordsLessNumerals,
                total6PlusCharWordsLessNumerals,
                totalSentencesColumnName,
                IsShowingGroupLegends() ?
                    std::optional<const wxString>(groupColumnName) :
                    std::nullopt);
            }
        UpdateGraphOptions(view->GetRaygorGraph());

        raygorGraph->SetInvalidAreaColor(GetInvalidAreaColor());
        raygorGraph->SetRaygorStyle(GetRaygorStyle());

        // add legend if grouping
        if (IsShowingGroupLegends())
            {
            view->GetRaygorGraph()->SetFixedObjectsGridSize(1, 2);
            view->GetRaygorGraph()->SetFixedObject(0, 1,
                raygorGraph->CreateLegend(Wisteria::Graphs::LegendOptions().PlacementHint(
                    Wisteria::LegendCanvasPlacementHint::RightOfGraph)));
            }
        // ...and if not grouping, use light blue for points
        else
            {
            raygorGraph->SetColorScheme(
                std::make_shared<Colors::Schemes::ColorScheme>
                 (Colors::Schemes::ColorScheme{
                     ColorBrewer::GetColor(Colors::Color::CelestialBlue) }));
            }

        wxGCDC gdc(view->GetDocFrame());
        view->GetRaygorGraph()->CalcAllSizes(gdc);
        }
    else
        {
        view->GetScoresView().RemoveWindowById(BaseProjectView::RAYGOR_PAGE_ID);
        view->SetRaygorGraph(nullptr);
        }
    }

//------------------------------------------------------------
void BatchProjectDoc::DisplayBoxPlots()
    {
    PROFILE();
    BatchProjectView* view = dynamic_cast<BatchProjectView*>(GetFirstView());

    // standard tests
    for (auto& sTest : GetReadabilityTests().get_tests())
        {
        if (sTest.get_test().get_test_type() == readability::readability_test_type::grade_level ||
            sTest.get_test().get_test_type() == readability::readability_test_type::index_value_and_grade_level ||
            sTest.get_test().get_test_type() ==
                readability::readability_test_type::grade_level_and_predicted_cloze_score)
            {
            const wxString pageLabel =
                (sTest.get_test().get_test_type() == readability::readability_test_type::grade_level) ?
                wxString(sTest.get_test().get_short_name().c_str()) :
                BatchProjectView::FormatGradeLevelsLabel(sTest.get_test().get_short_name().c_str());
            Wisteria::Canvas* boxPlotCanvas =
                dynamic_cast<Wisteria::Canvas*>(view->GetBoxPlotView().FindWindowByIdAndLabel(
                    sTest.get_test().get_interface_id(), pageLabel));
            if (sTest.is_included() &&
                sTest.get_grade_point_collection()->GetContinuousColumn(GetScoreColumnName())->GetRowCount())
                {
                if (!boxPlotCanvas)
                    {
                    boxPlotCanvas = new Wisteria::Canvas(view->GetSplitter(), sTest.get_test().get_interface_id());
                    boxPlotCanvas->SetFixedObjectsGridSize(1, 1);
                    boxPlotCanvas->SetFixedObject(0, 0, std::make_shared<BoxPlot>(boxPlotCanvas));
                    boxPlotCanvas->Hide();
                    boxPlotCanvas->SetLabel(pageLabel);
                    boxPlotCanvas->SetName(pageLabel);
                    view->GetBoxPlotView().AddWindow(boxPlotCanvas);
                    }
                UpdateGraphOptions(boxPlotCanvas);

                auto boxPlot = std::dynamic_pointer_cast<BoxPlot>(boxPlotCanvas->GetFixedObject(0, 0));
                assert(boxPlot && "Invalid dynamic cast to box plot!");
                boxPlot->SetBrushScheme(
                    std::make_shared<Brushes::Schemes::BrushScheme>(
                        Colors::Schemes::ColorScheme({ GetGraphBoxColor() })));
                boxPlot->SetData(sTest.get_grade_point_collection(),
                    GetScoreColumnName(),
                    // if more documents than groups, then use grouping
                    (GetDocumentLabels().size() > 1 && GetDocuments().size() > GetDocumentLabels().size()) ?
                     std::optional<wxString>(GetGroupColumnName()) : std::nullopt);

                if (boxPlot->GetBoxCount() > 1)
                    {
                    boxPlotCanvas->SetFixedObjectsGridSize(1, 2);
                    boxPlotCanvas->SetFixedObject(0, 1,
                        boxPlot->CreateLegend(LegendOptions().PlacementHint(LegendCanvasPlacementHint::RightOfGraph)));
                    }

                boxPlot->GetTitle() = GraphItems::Label(
                    GraphItemInfo(sTest.get_test().get_long_name().c_str()).
                        DPIScaling(boxPlotCanvas->GetDPIScaleFactor()).
                        Scaling(boxPlotCanvas->GetScaling()).Pen(wxNullPen));

                boxPlot->SetShadowType(IsDisplayingDropShadows() ?
                    ShadowType::RightSideAndBottomShadow : ShadowType::NoShadow);
                boxPlot->SetOpacity(GetGraphBoxOpacity());
                boxPlot->SetBoxEffect(GetGraphBoxEffect());
                if (const auto convertedIcon = Wisteria::ReportEnumConvert::ConvertIcon(GetStippleShape());
                    convertedIcon)
                    { boxPlot->SetStippleShape(convertedIcon.value()); }
                boxPlot->SetStippleShapeColor(GetStippleShapeColor());
                boxPlot->ShowLabels(IsDisplayingBoxPlotLabels());
                boxPlot->ShowAllPoints(IsShowingAllBoxPlotPoints());

                // Adjust the axis range and load its labels,
                // and force the grade (Y) axis to show the full range of Kindergarten through Doctorate.
                boxPlot->GetLeftYAxis().SetRange(0, 19, 0, 1, 1);
                for (int i = 0; i < 20; ++i)
                    {
                    boxPlot->GetLeftYAxis().SetCustomLabel(i,
                        GraphItems::Label(GetReadabilityMessageCatalog().GetGradeScaleLongLabel(i)));
                    }
                boxPlot->GetLeftYAxis().SetLabelDisplay(Wisteria::AxisLabelDisplay::DisplayOnlyCustomLabels);
                wxGCDC gdc(view->GetDocFrame());
                boxPlotCanvas->CalcAllSizes(gdc);
                }
            else
                { view->GetBoxPlotView().RemoveWindowByIdAndLabel(sTest.get_test().get_interface_id(), pageLabel); }
            }
        // some tests can have grade levels AND cloze or index values, so don't use "else" here,
        // go through each logic gate to plot all of the test's results
        if (sTest.get_test().get_test_type() == readability::readability_test_type::index_value ||
            sTest.get_test().get_test_type() == readability::readability_test_type::index_value_and_grade_level)
            {
            const wxString pageLabel =
                (sTest.get_test().get_test_type() == readability::readability_test_type::index_value) ?
                wxString(sTest.get_test().get_short_name().c_str()) :
                BatchProjectView::FormatIndexValuesLabel(sTest.get_test().get_short_name().c_str());
            Wisteria::Canvas* boxPlotCanvas =
                dynamic_cast<Wisteria::Canvas*>(view->GetBoxPlotView().FindWindowByIdAndLabel(
                    sTest.get_test().get_interface_id(), pageLabel));
            if (sTest.is_included() &&
                sTest.get_index_point_collection()->GetContinuousColumn(GetScoreColumnName())->GetRowCount())
                {
                if (!boxPlotCanvas)
                    {
                    boxPlotCanvas = new Wisteria::Canvas(view->GetSplitter(), sTest.get_test().get_interface_id());
                    boxPlotCanvas->SetFixedObjectsGridSize(1,1);
                    boxPlotCanvas->SetFixedObject(0, 0, std::make_shared<BoxPlot>(boxPlotCanvas));
                    boxPlotCanvas->Hide();
                    boxPlotCanvas->SetLabel(pageLabel);
                    boxPlotCanvas->SetName(pageLabel);
                    view->GetBoxPlotView().AddWindow(boxPlotCanvas);
                    }
                UpdateGraphOptions(boxPlotCanvas);

                auto boxPlot = std::dynamic_pointer_cast<BoxPlot>(boxPlotCanvas->GetFixedObject(0, 0));
                assert(boxPlot && "Invalid dynamic cast to box plot!");
                boxPlot->SetBrushScheme(
                    std::make_shared<Brushes::Schemes::BrushScheme>(
                        Colors::Schemes::ColorScheme({ GetGraphBoxColor() })));
                boxPlot->SetData(sTest.get_index_point_collection(),
                    GetScoreColumnName(),
                    // if more documents than groups, then use grouping
                    (GetDocumentLabels().size() > 1 && GetDocuments().size() > GetDocumentLabels().size()) ?
                     std::optional<wxString>(GetGroupColumnName()) : std::nullopt);

                if (boxPlot->GetBoxCount() > 1)
                    {
                    boxPlotCanvas->SetFixedObjectsGridSize(1,2);
                    boxPlotCanvas->SetFixedObject(0, 1,
                        boxPlot->CreateLegend(LegendOptions().PlacementHint(LegendCanvasPlacementHint::RightOfGraph)));
                    }

                boxPlot->GetTitle() = GraphItems::Label(
                    GraphItemInfo(sTest.get_test().get_long_name().c_str()).
                                 DPIScaling(boxPlotCanvas->GetDPIScaleFactor()).
                                 Scaling(boxPlotCanvas->GetScaling()).Pen(wxNullPen));

                boxPlot->SetShadowType(IsDisplayingDropShadows() ?
                    ShadowType::RightSideAndBottomShadow : ShadowType::NoShadow);
                boxPlot->ShowLabels(IsDisplayingBoxPlotLabels());
                boxPlot->ShowAllPoints(IsShowingAllBoxPlotPoints());
                boxPlot->SetOpacity(GetGraphBoxOpacity());
                boxPlot->SetBoxEffect(GetGraphBoxEffect());
                if (const auto convertedIcon = Wisteria::ReportEnumConvert::ConvertIcon(GetStippleShape());
                    convertedIcon)
                    { boxPlot->SetStippleShape(convertedIcon.value()); }
                boxPlot->SetStippleShapeColor(GetStippleShapeColor());

                const auto [rangeStart, rangeEnd] = boxPlot->GetLeftYAxis().GetRange();
                // Set the ranges to fit the index range. Note that the calculated outlier ranges
                // may go outside of the standard test range, so use the calculated range if larger.
                if (sTest.get_test().get_id() == ReadabilityMessages::FLESCH().wc_str())
                    {
                    boxPlot->GetLeftYAxis().SetRange(
                        std::min<double>(0, rangeStart), std::max<double>(100,rangeEnd), 0, 10, 1); }
                else if (sTest.get_test().get_id() == ReadabilityMessages::EFLAW().wc_str())
                    {
                    boxPlot->GetLeftYAxis().SetRange(
                        std::min<double>(0, rangeStart), std::max<double>(30,rangeEnd), 0, 5, 1); }
                else if (sTest.get_test().get_id() == ReadabilityMessages::LIX().wc_str())
                    {
                    boxPlot->GetLeftYAxis().SetRange(
                        std::min<double>(0, rangeStart), std::max<double>(60,rangeEnd), 0, 10, 1); }
                else if (sTest.get_test().get_id() == ReadabilityMessages::RIX().wc_str())
                    {
                    boxPlot->GetLeftYAxis().SetRange(
                        std::min<double>(0, rangeStart), std::max<double>(8,rangeEnd), 1,
                            boxPlot->GetLeftYAxis().GetInterval()/*might be 1 or 2*/, 1);
                    }
                wxGCDC gdc(view->GetDocFrame());
                boxPlotCanvas->CalcAllSizes(gdc);
                }
            else
                { view->GetBoxPlotView().RemoveWindowByIdAndLabel(sTest.get_test().get_interface_id(), pageLabel); }
            }
        if (sTest.get_test().get_test_type() == readability::readability_test_type::predicted_cloze_score ||
            sTest.get_test().get_test_type() ==
                readability::readability_test_type::grade_level_and_predicted_cloze_score)
            {
            const wxString pageLabel =
                (sTest.get_test().get_test_type() == readability::readability_test_type::predicted_cloze_score) ?
                wxString(sTest.get_test().get_short_name().c_str()) :
                BatchProjectView::FormatClozeValuesLabel(sTest.get_test().get_short_name().c_str());
            Wisteria::Canvas* boxPlotCanvas =
                dynamic_cast<Wisteria::Canvas*>(view->GetBoxPlotView().FindWindowByIdAndLabel(
                    sTest.get_test().get_interface_id(), pageLabel));
            if (sTest.is_included() &&
                sTest.get_cloze_point_collection()->GetContinuousColumn(GetScoreColumnName())->GetRowCount())
                {
                if (!boxPlotCanvas)
                    {
                    boxPlotCanvas = new Wisteria::Canvas(view->GetSplitter(), sTest.get_test().get_interface_id());
                    boxPlotCanvas->SetFixedObjectsGridSize(1,1);
                    boxPlotCanvas->SetFixedObject(0,0,std::make_shared<BoxPlot>(boxPlotCanvas));
                    boxPlotCanvas->Hide();
                    boxPlotCanvas->SetLabel(pageLabel);
                    boxPlotCanvas->SetName(pageLabel);
                    view->GetBoxPlotView().AddWindow(boxPlotCanvas);
                    }
                UpdateGraphOptions(boxPlotCanvas);

                auto boxPlot = std::dynamic_pointer_cast<BoxPlot>(boxPlotCanvas->GetFixedObject(0, 0));
                assert(boxPlot && "Invalid dynamic cast to box plot!");
                boxPlot->SetBrushScheme(
                    std::make_shared<Brushes::Schemes::BrushScheme>(
                        Colors::Schemes::ColorScheme({ GetGraphBoxColor() })));
                boxPlot->SetData(sTest.get_cloze_point_collection(),
                    GetScoreColumnName(),
                    // if more documents than groups, then use grouping
                    (GetDocumentLabels().size() > 1 && GetDocuments().size() > GetDocumentLabels().size()) ?
                     std::optional<wxString>(GetGroupColumnName()) : std::nullopt);

                if (boxPlot->GetBoxCount() > 1)
                    {
                    boxPlotCanvas->SetFixedObjectsGridSize(1,2);
                    boxPlotCanvas->SetFixedObject(0, 1,
                        boxPlot->CreateLegend(LegendOptions().PlacementHint(LegendCanvasPlacementHint::RightOfGraph)));
                    }

                boxPlot->GetTitle() = GraphItems::Label(
                    GraphItemInfo(sTest.get_test().get_long_name().c_str()).
                        DPIScaling(boxPlotCanvas->GetDPIScaleFactor()).
                        Scaling(boxPlotCanvas->GetScaling()).Pen(wxNullPen));
                boxPlot->SetShadowType(
                    IsDisplayingDropShadows() ? ShadowType::RightSideAndBottomShadow : ShadowType::NoShadow);
                boxPlot->SetOpacity(GetGraphBoxOpacity());
                boxPlot->SetBoxEffect(GetGraphBoxEffect());
                if (const auto convertedIcon = Wisteria::ReportEnumConvert::ConvertIcon(GetStippleShape());
                    convertedIcon)
                    { boxPlot->SetStippleShape(convertedIcon.value()); }
                boxPlot->SetStippleShapeColor(GetStippleShapeColor());
                boxPlot->ShowLabels(IsDisplayingBoxPlotLabels());
                boxPlot->ShowAllPoints(IsShowingAllBoxPlotPoints());
                boxPlot->SetLabelPrecision(0);

                // adjust the range
                const auto [rangeStart, rangeEnd] = boxPlot->GetLeftYAxis().GetRange();
                boxPlot->GetLeftYAxis().SetRange(std::min<double>(0,rangeStart),
                                                 std::max<double>(100,rangeEnd), 0, 10, 1);
                wxGCDC gdc(view->GetDocFrame());
                boxPlotCanvas->CalcAllSizes(gdc);
                }
            else
                { view->GetBoxPlotView().RemoveWindowByIdAndLabel(sTest.get_test().get_interface_id(), pageLabel); }
            }
        }

    // custom tests
    for (auto testPos = GetCustTestsInUse().cbegin();
        testPos != GetCustTestsInUse().cend();
        ++testPos)
        {
        if (testPos->GetIterator()->get_test_type() == readability::readability_test_type::grade_level)
            {
            Wisteria::Canvas* boxPlotCanvas =
                dynamic_cast<Wisteria::Canvas*>(view->GetBoxPlotView().FindWindowById(
                    testPos->GetIterator()->get_interface_id()));
            auto& scoreDataset = m_customTestScores[(testPos-GetCustTestsInUse().begin())];
            if (scoreDataset->GetContinuousColumn(GetScoreColumnName())->GetRowCount())
                {
                if (!boxPlotCanvas)
                    {
                    boxPlotCanvas = new Wisteria::Canvas(view->GetSplitter(),
                                                         testPos->GetIterator()->get_interface_id());
                    boxPlotCanvas->SetFixedObjectsGridSize(1,1);
                    boxPlotCanvas->SetFixedObject(0,0,std::make_shared<BoxPlot>(boxPlotCanvas));
                    boxPlotCanvas->Hide();
                    boxPlotCanvas->SetLabel(testPos->GetIterator()->get_name().c_str());
                    boxPlotCanvas->SetName(testPos->GetIterator()->get_name().c_str());
                    view->GetBoxPlotView().AddWindow(boxPlotCanvas);
                    }
                UpdateGraphOptions(boxPlotCanvas);

                auto boxPlot = std::dynamic_pointer_cast<BoxPlot>(boxPlotCanvas->GetFixedObject(0, 0));
                assert(boxPlot && "Invalid dynamic cast to box plot!");
                boxPlot->SetBrushScheme(
                    std::make_shared<Brushes::Schemes::BrushScheme>(
                        Colors::Schemes::ColorScheme({ GetGraphBoxColor() })));
                boxPlot->SetData(scoreDataset,
                    GetScoreColumnName(),
                    // if more documents than groups, then use grouping
                    (GetDocumentLabels().size() > 1 && GetDocuments().size() > GetDocumentLabels().size()) ?
                     std::optional<wxString>(GetGroupColumnName()) : std::nullopt);

                if (boxPlot->GetBoxCount() > 1)
                    {
                    boxPlotCanvas->SetFixedObjectsGridSize(1,2);
                    boxPlotCanvas->SetFixedObject(0, 1,
                        boxPlot->CreateLegend(LegendOptions().PlacementHint(LegendCanvasPlacementHint::RightOfGraph)));
                    }

                boxPlot->GetTitle() = GraphItems::Label(
                    GraphItemInfo(testPos->GetIterator()->get_name().c_str()).
                        DPIScaling(boxPlotCanvas->GetDPIScaleFactor()).
                        Scaling(boxPlotCanvas->GetScaling()).Pen(wxNullPen));

                boxPlot->SetShadowType(
                    IsDisplayingDropShadows() ? ShadowType::RightSideAndBottomShadow : ShadowType::NoShadow);
                boxPlot->SetOpacity(GetGraphBoxOpacity());
                boxPlot->SetBoxEffect(GetGraphBoxEffect());
                if (const auto convertedIcon = Wisteria::ReportEnumConvert::ConvertIcon(GetStippleShape());
                    convertedIcon)
                    { boxPlot->SetStippleShape(convertedIcon.value()); }
                boxPlot->SetStippleShapeColor(GetStippleShapeColor());
                boxPlot->ShowLabels(IsDisplayingBoxPlotLabels());
                boxPlot->ShowAllPoints(IsShowingAllBoxPlotPoints());

                // Adjust the axis range and load its labels,
                // and force the grade (Y) axis to show the full range of
                // Kindergarten through Doctorate.
                boxPlot->GetLeftYAxis().SetRange(0, 19, 0, 1, 1);
                for (int i = 0; i < 20; ++i)
                    {
                    boxPlot->GetLeftYAxis().SetCustomLabel(i,
                        GraphItems::Label(GetReadabilityMessageCatalog().GetGradeScaleLongLabel(i)));
                    }
                boxPlot->GetLeftYAxis().SetLabelDisplay(Wisteria::AxisLabelDisplay::DisplayOnlyCustomLabels);
                wxGCDC gdc(view->GetDocFrame());
                boxPlotCanvas->CalcAllSizes(gdc);
                }
            else
                { view->GetBoxPlotView().RemoveWindowById(testPos->GetIterator()->get_interface_id()); }
            }
        else if (testPos->GetIterator()->get_test_type() == readability::readability_test_type::index_value ||
            testPos->GetIterator()->get_test_type() == readability::readability_test_type::predicted_cloze_score)
            {
            Wisteria::Canvas* boxPlotCanvas =
                dynamic_cast<Wisteria::Canvas*>(view->GetBoxPlotView().FindWindowById(
                    testPos->GetIterator()->get_interface_id()));
            auto& scoreDataset = m_customTestScores[(testPos-GetCustTestsInUse().begin())];
            if (scoreDataset->GetContinuousColumn(GetScoreColumnName())->GetRowCount())
                {
                if (!boxPlotCanvas)
                    {
                    boxPlotCanvas = new Wisteria::Canvas(view->GetSplitter(),
                                                         testPos->GetIterator()->get_interface_id());
                    boxPlotCanvas->SetFixedObjectsGridSize(1,1);
                    boxPlotCanvas->SetFixedObject(0,0,std::make_shared<BoxPlot>(boxPlotCanvas));
                    boxPlotCanvas->Hide();
                    boxPlotCanvas->SetLabel(testPos->GetIterator()->get_name().c_str());
                    boxPlotCanvas->SetName(testPos->GetIterator()->get_name().c_str());
                    view->GetBoxPlotView().AddWindow(boxPlotCanvas);
                    }
                UpdateGraphOptions(boxPlotCanvas);

                auto boxPlot = std::dynamic_pointer_cast<BoxPlot>(boxPlotCanvas->GetFixedObject(0, 0));
                assert(boxPlot && "Invalid dynamic cast to box plot!");
                boxPlot->SetData(scoreDataset,
                    GetScoreColumnName(),
                    // if more documents than groups, then use grouping
                    (GetDocumentLabels().size() > 1 && GetDocuments().size() > GetDocumentLabels().size()) ?
                    std::optional<wxString>(GetGroupColumnName()) : std::nullopt);
                boxPlot->SetBrushScheme(
                    std::make_shared<Brushes::Schemes::BrushScheme>(
                        Colors::Schemes::ColorScheme({ GetGraphBoxColor() })));

                if (boxPlot->GetBoxCount() > 1)
                    {
                    boxPlotCanvas->SetFixedObjectsGridSize(1, 2);
                    boxPlotCanvas->SetFixedObject(0, 1,
                        boxPlot->CreateLegend(
                            LegendOptions().PlacementHint(LegendCanvasPlacementHint::RightOfGraph)));
                    }

                boxPlot->GetTitle() = GraphItems::Label(
                    GraphItemInfo(testPos->GetIterator()->get_name().c_str()).
                        DPIScaling(boxPlotCanvas->GetDPIScaleFactor()).
                        Scaling(boxPlotCanvas->GetScaling()).Pen(wxNullPen));

                boxPlot->SetShadowType(
                    IsDisplayingDropShadows() ? ShadowType::RightSideAndBottomShadow : ShadowType::NoShadow);
                boxPlot->SetOpacity(GetGraphBoxOpacity());
                boxPlot->SetBoxEffect(GetGraphBoxEffect());
                if (const auto convertedIcon = Wisteria::ReportEnumConvert::ConvertIcon(GetStippleShape());
                    convertedIcon)
                    { boxPlot->SetStippleShape(convertedIcon.value()); }
                boxPlot->SetStippleShapeColor(GetStippleShapeColor());
                boxPlot->ShowLabels(IsDisplayingBoxPlotLabels());
                boxPlot->ShowAllPoints(IsShowingAllBoxPlotPoints());

                // adjust the axis range
                const auto [rangeStart, rangeEnd] = boxPlot->GetLeftYAxis().GetRange();
                if (testPos->GetIterator()->get_test_type() ==
                    readability::readability_test_type::predicted_cloze_score)
                    {
                    boxPlot->GetLeftYAxis().SetRange(std::min<double>(0,rangeStart),
                                                     std::max<double>(100,rangeEnd), 0, 10, 1);
                    }
                wxGCDC gdc(view->GetDocFrame());
                boxPlotCanvas->CalcAllSizes(gdc);
                }
            else
                { view->GetBoxPlotView().RemoveWindowById(testPos->GetIterator()->get_interface_id()); }
            }
        }
    }

//------------------------------------------------------------
void BatchProjectDoc::DisplayHistograms()
    {
    PROFILE();
    // First, remove any custom-test histograms that had their test removed from the project.
    BatchProjectView* view = dynamic_cast<BatchProjectView*>(GetFirstView());
    std::set<wxWindowID> validTestNames;
    for (auto rTests = GetReadabilityTests().get_tests().begin();
        rTests != GetReadabilityTests().get_tests().end();
        ++rTests)
        { validTestNames.insert(rTests->get_test().get_interface_id()); }
    for (std::vector<CustomReadabilityTestInterface>::const_iterator testPos = GetCustTestsInUse().cbegin();
        testPos != GetCustTestsInUse().cend();
        ++testPos)
        { validTestNames.insert(testPos->GetIterator()->get_interface_id()); }
    const long pageCount = static_cast<long>(view->GetHistogramsView().GetWindowCount());
    for (long i = pageCount-1; i >= 0; --i)
        {
        const wxWindowID currentId = view->GetHistogramsView().GetWindow(i)->GetId();
        if (validTestNames.find(currentId) == validTestNames.end())
            { view->GetHistogramsView().RemoveWindowById(currentId); }
        }

    // standard tests
    for (auto rTests = GetReadabilityTests().get_tests().begin();
        rTests != GetReadabilityTests().get_tests().end();
        ++rTests)
        {
        if (rTests->get_test().get_test_type() == readability::readability_test_type::grade_level)
            {
            DisplayHistogram(rTests->get_test().get_short_name().c_str(), rTests->get_test().get_interface_id(),
                rTests->get_test().get_long_name().c_str(), _(L"Grade Levels"),
                rTests->get_grade_point_collection(), rTests->is_included(), true, true);
            }
        else if (rTests->get_test().get_test_type() == readability::readability_test_type::index_value)
            {
            DisplayHistogram(rTests->get_test().get_short_name().c_str(), rTests->get_test().get_interface_id(),
                rTests->get_test().get_long_name().c_str(), _(L"Index Values"),
                rTests->get_index_point_collection(), rTests->is_included(), false, false);
            }
        else if (rTests->get_test().get_test_type() == readability::readability_test_type::predicted_cloze_score)
            {
            DisplayHistogram(rTests->get_test().get_short_name().c_str(), rTests->get_test().get_interface_id(),
                rTests->get_test().get_long_name().c_str(), _(L"Predicted Cloze Scores"),
                rTests->get_cloze_point_collection(), rTests->is_included(), false, true);
            }
        else if (rTests->get_test().get_test_type() == readability::readability_test_type::index_value_and_grade_level)
            {
            DisplayHistogram(
                BatchProjectView::FormatIndexValuesLabel(rTests->get_test().get_short_name().c_str()),
                rTests->get_test().get_interface_id(),
                BatchProjectView::FormatIndexValuesLabel(rTests->get_test().get_long_name().c_str()),
                _(L"Index Values"),
                rTests->get_index_point_collection(), rTests->is_included(), false, false);
            DisplayHistogram(
                BatchProjectView::FormatGradeLevelsLabel(rTests->get_test().get_short_name().c_str()),
                rTests->get_test().get_interface_id(),
                BatchProjectView::FormatGradeLevelsLabel(rTests->get_test().get_long_name().c_str()),
                _(L"Grade Levels"),
                rTests->get_grade_point_collection(), rTests->is_included(), true, true);
            }
        else if (rTests->get_test().get_test_type() ==
            readability::readability_test_type::grade_level_and_predicted_cloze_score)
            {
            DisplayHistogram(
                BatchProjectView::FormatGradeLevelsLabel(rTests->get_test().get_short_name().c_str()),
                rTests->get_test().get_interface_id(),
                BatchProjectView::FormatGradeLevelsLabel(rTests->get_test().get_long_name().c_str()),
                _(L"Grade Levels"),
                rTests->get_grade_point_collection(), rTests->is_included(), true, true);
            DisplayHistogram(
                BatchProjectView::FormatClozeValuesLabel(rTests->get_test().get_short_name().c_str()),
                rTests->get_test().get_interface_id(),
                BatchProjectView::FormatClozeValuesLabel(rTests->get_test().get_long_name().c_str()),
                _(L"Predicted Cloze Scores"),
                rTests->get_cloze_point_collection(), rTests->is_included(), false, true);
            }
        }
    // Custom word tests
    for (std::vector<CustomReadabilityTestInterface>::const_iterator testPos = GetCustTestsInUse().cbegin();
        testPos != GetCustTestsInUse().cend();
        ++testPos)
        {
        auto& scoreDataset = m_customTestScores[(testPos-GetCustTestsInUse().begin())];
        DisplayHistogram(testPos->GetTestName(), testPos->GetIterator()->get_interface_id(), testPos->GetTestName(),
            testPos->GetIterator()->get_test_type() ==
                readability::readability_test_type::grade_level ? _(L"Grade Levels") :
                testPos->GetIterator()->get_test_type() ==
                readability::readability_test_type::index_value ? _(L"Index Values") :
                _(L"Predicted Cloze Scores"),
            scoreDataset, true, testPos->GetIterator()->get_test_type() ==
                readability::readability_test_type::grade_level,
            (testPos->GetIterator()->get_test_type() == readability::readability_test_type::grade_level ||
             testPos->GetIterator()->get_test_type() == readability::readability_test_type::predicted_cloze_score));
        }
    }

//------------------------------------------------
void BatchProjectDoc::DisplayHistogram(const wxString& name, const wxWindowID Id,
                                       const wxString& topLabel, const wxString& bottomLabel,
                                       std::shared_ptr<const Wisteria::Data::Dataset> data,
                                       const bool includeTest,
                                       const bool isTestGradeLevel, const bool startAtOne)
    {
    BatchProjectView* view = dynamic_cast<BatchProjectView*>(GetFirstView());

    Wisteria::Canvas* canvas =
        dynamic_cast<Wisteria::Canvas*>(view->GetHistogramsView().FindWindowByIdAndLabel(Id, name));
    if (includeTest && data->GetRowCount())
        {
        if (!canvas)
            {
            canvas = new Wisteria::Canvas(view->GetSplitter(), Id);
            canvas->SetFixedObjectsGridSize(1,1);
            canvas->SetFixedObject(0,0,std::make_shared<Histogram>(canvas,
                IsShowingGroupLegends() ?
                std::make_shared<Wisteria::Brushes::Schemes::BrushScheme>(
                    *std::make_shared<Wisteria::Colors::Schemes::EarthTones>()) :
                std::make_shared<Wisteria::Brushes::Schemes::BrushScheme>(
                    *std::make_shared<Wisteria::Colors::Schemes::ColorScheme>
                    (Wisteria::Colors::Schemes::ColorScheme{ GetHistogramBarColor() }))) );
            canvas->Hide();
            canvas->SetLabel(name);
            canvas->SetName(name);
            view->GetHistogramsView().AddWindow(canvas);
            }
        UpdateGraphOptions(canvas);

        auto histogram = std::dynamic_pointer_cast<Histogram>(canvas->GetFixedObject(0, 0));
        assert(histogram && "Invalid histogram cast!");

        if (!IsShowingGroupLegends())
            {
            histogram->SetBrushScheme(
                std::make_shared<Wisteria::Brushes::Schemes::BrushScheme>(
                    *std::make_shared<Wisteria::Colors::Schemes::ColorScheme>(
                        Wisteria::Colors::Schemes::ColorScheme({ GetHistogramBarColor() }))));
            }

        histogram->SetData(data,
            GetScoreColumnName(),
            (IsShowingGroupLegends() ? std::optional<wxString>(GetGroupColumnName()) : std::nullopt),
            (isTestGradeLevel ? Histogram::BinningMethod::BinUniqueValues : GetHistogramBinningMethod()),
            GetHistogramRoundingMethod(),
            GetHistogramIntervalDisplay(), GetHistogramBinLabelDisplay(),
            true, (isTestGradeLevel ? 0 : startAtOne ? 1 : std::numeric_limits<double>::quiet_NaN()));

        if (IsShowingGroupLegends())
            {
            canvas->SetFixedObjectsGridSize(1, 2);
            canvas->SetFixedObject(0, 1,
                histogram->CreateLegend(
                    LegendOptions().PlacementHint(LegendCanvasPlacementHint::RightOfGraph).IncludeHeader(false)));
            }

        histogram->ClearProperties();
        histogram->GetBarAxis().ClearBrackets();
        histogram->GetTitle() = GraphItems::Label(GraphItemInfo(topLabel).DPIScaling(canvas->GetDPIScaleFactor()).
                                                 Scaling(canvas->GetScaling()).Pen(wxNullPen));
        histogram->GetScalingAxis().GetTitle().SetText(_(L"Number of Documents"));
        histogram->GetBarAxis().GetTitle() = GraphItems::Label(GraphItemInfo(bottomLabel).
                                                              DPIScaling(canvas->GetDPIScaleFactor()).
                                                              Scaling(canvas->GetScaling()).Pen(wxNullPen));
        histogram->SetShadowType(IsDisplayingDropShadows() ? ShadowType::RightSideShadow : ShadowType::NoShadow);
        histogram->SetBarEffect(GetHistogramBarEffect());
        if (const auto convertedIcon = Wisteria::ReportEnumConvert::ConvertIcon(GetStippleShape());
            convertedIcon)
            { histogram->SetStippleShape(convertedIcon.value()); }
        histogram->SetStippleShapeColor(GetStippleShapeColor());
        histogram->SetBarOpacity(GetHistogramBarOpacity());

        histogram->GetBarAxis().SetLabelLineLength(10);
        if (isTestGradeLevel)
            {
            histogram->AddProperty(_DT(L"ISGRADEPLOT"), true);
            if (histogram->GetBars().size())
                {
                for (int i = 0; i < 20; ++i)
                    {
                    const auto foundValidBar =
                        std::find_if(histogram->GetBars().cbegin(), histogram->GetBars().cend(),
                        [i](const auto& bar) noexcept
                        { return bar.GetAxisPosition() == i && bar.GetLength() > 0; });
                    histogram->GetBarAxis().SetCustomLabel(i,
                        // include the Kindergarten bin label just to show where everything starts,
                        // then hide any labels where the bins are empty
                        (foundValidBar == histogram->GetBars().cend() && i > 0) ?
                        GraphItems::Label{} :
                        GraphItems::Label(GetReadabilityMessageCatalog().GetGradeScaleLongLabel(i)));
                    }
                // if not too many bins, show the long grade labels on the X axis
                histogram->GetBarAxis().SetLabelDisplay(
                        (histogram->GetBinsWithValuesCount() <= 5) ?
                        AxisLabelDisplay::DisplayOnlyCustomLabels :
                        AxisLabelDisplay::DisplayValues);
                if (GetReadabilityMessageCatalog().GetGradeScale() ==
                        readability::grade_scale::k12_plus_united_states)
                    {
                    const auto lastGradeBar = histogram->GetBars().back().GetAxisPosition();
                    const auto addGradesBracket =
                        [&histogram, &lastGradeBar, this]
                        (const double startGrade, const double endGrade, const wxString& label)
                        {
                        if (startGrade > lastGradeBar)
                            { return; }
                        histogram->GetBarAxis().AddBracket(
                            Axis::AxisBracket(startGrade, std::min(endGrade, lastGradeBar),
                                startGrade + ((std::min(endGrade, lastGradeBar) - startGrade) / 2), label));
                        histogram->GetBarAxis().SetCustomLabel(startGrade,
                            GraphItems::Label(GetReadabilityMessageCatalog().GetGradeScaleLongLabel(startGrade)));
                        histogram->GetBarAxis().SetCustomLabel(endGrade,
                            GraphItems::Label(GetReadabilityMessageCatalog().GetGradeScaleLongLabel(endGrade)));
                        };

                    addGradesBracket(0.0, 4.0, _("Elementary School"));
                    addGradesBracket(5.0, 8.0, _("Middle School"));
                    addGradesBracket(9.0, 12.0, _("High School"));
                    addGradesBracket(13.0, 16.0, _("College"));
                    addGradesBracket(17.0, 18.0, _("Graduate School"));
                    addGradesBracket(19.0, 20.0, _("PhD"));
                    }
                }
            }
        wxGCDC gdc(view->GetDocFrame());
        canvas->CalcAllSizes(gdc);
        }
    else
        {
        // we are getting rid of this window (if it was included before)
        view->GetHistogramsView().RemoveWindowByIdAndLabel(Id, name);
        }
    }

//------------------------------------------------
bool BatchProjectDoc::RunProjectWizard(const wxString& path)
    {
    // Run through the project wizard
    ProjectWizardDlg* wizard =
        new ProjectWizardDlg(wxGetApp().GetParentingWindow(), ProjectType::BatchProject, path);
    if (wizard->ShowModal() != wxID_OK)
        {
        wizard->Destroy();
        return false;
        }

    SetProjectLanguage(wizard->GetLanguage());
    wxGetApp().GetAppOptions().SetProjectLanguage(wizard->GetLanguage());

    // get readability options that were selected
    if (wizard->IsDocumentTypeSelected() )
        {
        // general documents
        if (wizard->GetSelectedDocumentType() == readability::document_classification::general_document)
            {
            for (auto rTest = GetReadabilityTests().get_tests().begin();
                rTest != GetReadabilityTests().get_tests().end();
                ++rTest)
                {
                rTest->include(rTest->get_test().has_document_classification(
                        readability::document_classification::general_document) &&
                    rTest->get_test().has_language(GetProjectLanguage()));
                }
            for (CustomReadabilityTestCollection::const_iterator pos = m_custom_word_tests.cbegin();
                pos != m_custom_word_tests.cend();
                ++pos)
                {
                if (pos->has_document_classification(readability::document_classification::general_document))
                    { AddCustomReadabilityTest(wxString(pos->get_name().c_str())); }
                }
            }
        // technical manuals and documents
        else if (wizard->GetSelectedDocumentType() == readability::document_classification::technical_document )
            {
            for (auto rTest = GetReadabilityTests().get_tests().begin();
                rTest != GetReadabilityTests().get_tests().end();
                ++rTest)
                {
                rTest->include(rTest->get_test().has_document_classification(
                        readability::document_classification::technical_document) &&
                    rTest->get_test().has_language(GetProjectLanguage()));
                }
            for (CustomReadabilityTestCollection::const_iterator pos = m_custom_word_tests.begin();
                pos != m_custom_word_tests.end();
                ++pos)
                {
                if (pos->has_document_classification(readability::document_classification::technical_document))
                    { AddCustomReadabilityTest(wxString(pos->get_name().c_str())); }
                }
            }
        // short, terse forms
        else if (wizard->GetSelectedDocumentType() == readability::document_classification::nonnarrative_document)
            {
            // override how headers and lists are counted so that they are
            // always included if this is a form
            SetInvalidSentenceMethod(InvalidSentence::IncludeAsFullSentences);

            for (auto rTest = GetReadabilityTests().get_tests().begin();
                rTest != GetReadabilityTests().get_tests().end();
                ++rTest)
                {
                rTest->include(rTest->get_test().has_document_classification(
                        readability::document_classification::nonnarrative_document) &&
                    rTest->get_test().has_language(GetProjectLanguage()));
                }
            for (CustomReadabilityTestCollection::const_iterator pos = m_custom_word_tests.cbegin();
                pos != m_custom_word_tests.cend();
                ++pos)
                {
                if (pos->has_document_classification(readability::document_classification::nonnarrative_document))
                    { AddCustomReadabilityTest(wxString(pos->get_name().c_str())); }
                }
            }
        else if (wizard->GetSelectedDocumentType() == readability::document_classification::adult_literature_document)
            {
            for (auto rTest = GetReadabilityTests().get_tests().begin();
                rTest != GetReadabilityTests().get_tests().end();
                ++rTest)
                {
                rTest->include(rTest->get_test().has_document_classification(
                        readability::document_classification::adult_literature_document) &&
                    rTest->get_test().has_language(GetProjectLanguage()));
                }
            for (CustomReadabilityTestCollection::const_iterator pos = m_custom_word_tests.cbegin();
                pos != m_custom_word_tests.cend();
                ++pos)
                {
                if (pos->has_document_classification(readability::document_classification::adult_literature_document))
                    { AddCustomReadabilityTest(wxString(pos->get_name().c_str())); }
                }
            }
        else if (wizard->GetSelectedDocumentType() ==
            readability::document_classification::childrens_literature_document)
            {
            for (auto rTest = GetReadabilityTests().get_tests().begin();
                rTest != GetReadabilityTests().get_tests().end();
                ++rTest)
                {
                rTest->include(rTest->get_test().has_document_classification(
                        readability::document_classification::childrens_literature_document) &&
                    rTest->get_test().has_language(GetProjectLanguage()));
                }
            IncludeDolchSightWords((GetProjectLanguage() == readability::test_language::english_test));
            for (CustomReadabilityTestCollection::const_iterator pos = m_custom_word_tests.cbegin();
                pos != m_custom_word_tests.cend();
                ++pos)
                {
                if (pos->has_document_classification(
                    readability::document_classification::childrens_literature_document))
                    { AddCustomReadabilityTest(wxString(pos->get_name().c_str())); }
                }
            }
        wxGetApp().GetAppOptions().SetTestByDocumentType(wizard->GetSelectedDocumentType());
        }
    // user selected the program to use recommended tests by industry
    else if (wizard->IsIndustrySelected() )
        {
        if (wizard->GetSelectedIndustryType() ==
            readability::industry_classification::childrens_publishing_industry)
            {
            for (auto rTest = GetReadabilityTests().get_tests().begin();
                rTest != GetReadabilityTests().get_tests().end();
                ++rTest)
                {
                rTest->include(rTest->get_test().has_industry_classification(
                        readability::industry_classification::childrens_publishing_industry) &&
                    rTest->get_test().has_language(GetProjectLanguage()));
                }
            IncludeDolchSightWords((GetProjectLanguage() == readability::test_language::english_test));
            for (CustomReadabilityTestCollection::const_iterator pos = m_custom_word_tests.cbegin();
                pos != m_custom_word_tests.cend();
                ++pos)
                {
                if (pos->has_industry_classification(
                    readability::industry_classification::childrens_publishing_industry))
                    { AddCustomReadabilityTest(wxString(pos->get_name().c_str())); }
                }
            }
        else if (wizard->GetSelectedIndustryType() == readability::industry_classification::adult_publishing_industry)
            {
            for (auto rTest = GetReadabilityTests().get_tests().begin();
                rTest != GetReadabilityTests().get_tests().end();
                ++rTest)
                {
                rTest->include(rTest->get_test().has_industry_classification(
                        readability::industry_classification::adult_publishing_industry) &&
                    rTest->get_test().has_language(GetProjectLanguage()));
                }
            for (CustomReadabilityTestCollection::const_iterator pos = m_custom_word_tests.cbegin();
                pos != m_custom_word_tests.cend();
                ++pos)
                {
                if (pos->has_industry_classification(readability::industry_classification::adult_publishing_industry))
                    { AddCustomReadabilityTest(wxString(pos->get_name().c_str())); }
                }
            }
        else if (wizard->GetSelectedIndustryType() ==
            readability::industry_classification::sedondary_language_industry)
            {
            for (auto rTest = GetReadabilityTests().get_tests().begin();
                rTest != GetReadabilityTests().get_tests().end();
                ++rTest)
                {
                rTest->include(rTest->get_test().has_industry_classification(
                        readability::industry_classification::sedondary_language_industry) &&
                    rTest->get_test().has_language(GetProjectLanguage()));
                }
            IncludeDolchSightWords((GetProjectLanguage() == readability::test_language::english_test));
            for (CustomReadabilityTestCollection::const_iterator pos = m_custom_word_tests.cbegin();
                pos != m_custom_word_tests.cend();
                ++pos)
                {
                if (pos->has_industry_classification(
                    readability::industry_classification::sedondary_language_industry))
                    { AddCustomReadabilityTest(wxString(pos->get_name().c_str())); }
                }
            }
        else if (wizard->GetSelectedIndustryType() ==
            readability::industry_classification::childrens_healthcare_industry)
            {
            for (auto rTest = GetReadabilityTests().get_tests().begin();
                rTest != GetReadabilityTests().get_tests().end();
                ++rTest)
                {
                rTest->include(rTest->get_test().has_industry_classification(
                        readability::industry_classification::childrens_healthcare_industry) &&
                    rTest->get_test().has_language(GetProjectLanguage()));
                }
            for (CustomReadabilityTestCollection::const_iterator pos = m_custom_word_tests.cbegin();
                pos != m_custom_word_tests.cend();
                ++pos)
                {
                if (pos->has_industry_classification(
                    readability::industry_classification::childrens_healthcare_industry))
                    { AddCustomReadabilityTest(wxString(pos->get_name().c_str())); }
                }
            }
        else if (wizard->GetSelectedIndustryType() ==
            readability::industry_classification::adult_healthcare_industry)
            {
            for (auto rTest = GetReadabilityTests().get_tests().begin();
                rTest != GetReadabilityTests().get_tests().end();
                ++rTest)
                {
                rTest->include(rTest->get_test().has_industry_classification(
                        readability::industry_classification::adult_healthcare_industry) &&
                    rTest->get_test().has_language(GetProjectLanguage()));
                }
            for (CustomReadabilityTestCollection::const_iterator pos = m_custom_word_tests.cbegin();
                pos != m_custom_word_tests.cend();
                ++pos)
                {
                if (pos->has_industry_classification(
                    readability::industry_classification::adult_healthcare_industry))
                    { AddCustomReadabilityTest(wxString(pos->get_name().c_str())); }
                }
            }
        else if (wizard->GetSelectedIndustryType() ==
        readability::industry_classification::military_government_industry)
            {
            for (auto rTest = GetReadabilityTests().get_tests().begin();
                rTest != GetReadabilityTests().get_tests().end();
                ++rTest)
                {
                rTest->include(rTest->get_test().has_industry_classification(
                        readability::industry_classification::military_government_industry) &&
                    rTest->get_test().has_language(GetProjectLanguage()));
                }
            for (CustomReadabilityTestCollection::const_iterator pos = m_custom_word_tests.cbegin();
                pos != m_custom_word_tests.cend();
                ++pos)
                {
                if (pos->has_industry_classification(
                    readability::industry_classification::military_government_industry))
                    { AddCustomReadabilityTest(wxString(pos->get_name().c_str())); }
                }
            }
        else if (wizard->GetSelectedIndustryType() ==
        readability::industry_classification::broadcasting_industry)
            {
            for (auto rTest = GetReadabilityTests().get_tests().begin();
                rTest != GetReadabilityTests().get_tests().end();
                ++rTest)
                {
                rTest->include(rTest->get_test().has_industry_classification(
                        readability::industry_classification::broadcasting_industry) &&
                    rTest->get_test().has_language(GetProjectLanguage()));
                }
            for (CustomReadabilityTestCollection::const_iterator pos = m_custom_word_tests.cbegin();
                pos != m_custom_word_tests.cend();
                ++pos)
                {
                if (pos->has_industry_classification(readability::industry_classification::broadcasting_industry))
                    { AddCustomReadabilityTest(wxString(pos->get_name().c_str())); }
                }
            }
        wxGetApp().GetAppOptions().SetTestByIndustry(wizard->GetSelectedIndustryType());
        }
    // user manually selected the readability test to run
    else if (wizard->IsManualTestSelected() )
        {
        // manually selected standard tests
        SetReadabilityTests(wizard->GetReadabilityTestsInfo());
        for (auto rTest = GetReadabilityTests().get_tests().begin();
            rTest != GetReadabilityTests().get_tests().end();
            ++rTest)
            {
            // turn off any selected tests that don't belong to the project's language
            if (!rTest->get_test().has_language(GetProjectLanguage()))
                { rTest->include(false); }
            }
        wxGetApp().GetAppOptions().SetReadabilityTests(wizard->GetReadabilityTestsInfo());

        // Dolch
        if (wizard->GetLanguage() == readability::test_language::english_test)
            {
            IncludeDolchSightWords(wizard->IsDolchSelected());
            // Ignore whether this was checked or not if not English.
            // This way, if this project is non-English, then it won't affect
            // future English projects when they are being created.
            wxGetApp().GetAppOptions().SetDolch(IsIncludingDolchSightWords());
            }
        else
            { IncludeDolchSightWords(false); }
        // Custom tests. See what was selected, look it up in the global list of test, and add
        // its unique test ID to the options manager's list of included custom tests.
        wxGetApp().GetAppOptions().GetIncludedCustomTests().clear();
        wxArrayInt selectedTestIndices = wizard->GetSelectedCustomTests();
        for (size_t i = 0; i < selectedTestIndices.Count(); ++i)
            {
            CustomReadabilityTest selectedTest =
                m_custom_word_tests[selectedTestIndices.Item(i)];
            AddCustomReadabilityTest(selectedTest.get_name().c_str());
            wxGetApp().GetAppOptions().GetIncludedCustomTests().push_back(selectedTest.get_name().c_str());
            }
        }
    // user selected a test bundle
    else if (wizard->IsTestBundleSelected())
        {
        ApplyTestBundle(wizard->GetSelectedTestBundle());
        wxGetApp().GetAppOptions().SetSelectedTestBundle(wizard->GetSelectedTestBundle());
        }
    // set parsing options based on how the user defined the structure of the document
    IgnoreBlankLinesForParagraphsParser(wizard->IsSplitLinesSelected());
    IgnoreIndentingForParagraphsParser(wizard->IsCenteredTextSelected());
    if ((wizard->IsSplitLinesSelected() ||
         wizard->IsCenteredTextSelected()) &&
         !wizard->IsNewLinesAlwaysNewParagraphsSelected())
        {
        // also override paragraph ending logic if special format parsing is requested
        SetParagraphsParsingMethod(ParagraphParse::OnlySentenceTerminatedNewLinesAreParagraphs);
        }
    else if (wizard->IsNewLinesAlwaysNewParagraphsSelected())
        { SetParagraphsParsingMethod(ParagraphParse::EachNewLineIsAParagraph); }
    // DOC files are difficult to detect lists in, so if they are not explicitly specifying
    // "fitted to the page" analysis for this project (above), then override the global
    // option and set it to treat all newlines as the end of a paragraph.
    else
        {
        m_adjustParagraphParserForDocFiles = true;
        }

    if (wizard->IsNarrativeSelected())
        {
        SetInvalidSentenceMethod(InvalidSentence::ExcludeFromAnalysis);
        }
    else if (wizard->IsFragmentedTextSelected())
        {
        // override how headers and lists are counted so that they are
        // always included if this is a form
        SetInvalidSentenceMethod(InvalidSentence::IncludeAsFullSentences);
        for (CustomReadabilityTestCollection::const_iterator pos = m_custom_word_tests.cbegin();
            pos != m_custom_word_tests.cend();
            ++pos)
            {
            if (pos->has_document_classification(readability::document_classification::nonnarrative_document))
                { AddCustomReadabilityTest(wxString(pos->get_name().c_str())); }
            }
        }

    // grab the list of files and their (optional) descriptions
    GetSourceFilesInfo().clear();
    GetSourceFilesInfo().reserve(wizard->GetFileData()->GetItemCount());
    for (size_t i = 0; i < wizard->GetFileData()->GetItemCount(); ++i)
        {
        GetSourceFilesInfo().emplace_back(wizard->GetFileData()->GetItemText(i, 0),
                                          wizard->GetFileData()->GetItemText(i, 1));
        }
    // remove any duplicates
    std::sort(GetSourceFilesInfo().begin(), GetSourceFilesInfo().end());
    auto endOfUniquePos =
        std::unique(GetSourceFilesInfo().begin(), GetSourceFilesInfo().end());
    if (endOfUniquePos != GetSourceFilesInfo().end())
        { GetSourceFilesInfo().erase(endOfUniquePos, GetSourceFilesInfo().end()); }
    // if using random subsampling
    if (wizard->IsRandomSampling())
        {
        const size_t sampleSize = GetSourceFilesInfo().size() *
                                  safe_divide<double>(wizard->GetRandomSamplePercentage(), 100);
        assert(sampleSize < GetSourceFilesInfo().size() && "Invalid random sample size!");

        std::shuffle(GetSourceFilesInfo().begin(), GetSourceFilesInfo().end(),
                     wxGetApp().GetRandomNumberEngine());
        GetSourceFilesInfo().erase(GetSourceFilesInfo().begin() + sampleSize,
                                   GetSourceFilesInfo().end());
        assert(sampleSize == GetSourceFilesInfo().size() && "Invalid random sample size!");

        wxGetApp().GetAppOptions().SetBatchRandomSamplingSize(wizard->GetRandomSamplePercentage());
        }
    SetMinDocWordCountForBatch(wizard->GetMinDocWordCountForBatch());
    wxGetApp().GetAppOptions().SetMinDocWordCountForBatch(wizard->GetMinDocWordCountForBatch());

    wxGetApp().GetAppOptions().SetTestRecommendation(wizard->IsDocumentTypeSelected() ?
        TestRecommendation::BasedOnDocumentType :
        wizard->IsIndustrySelected() ? TestRecommendation::BasedOnIndustry :
        wizard->IsTestBundleSelected() ?
            TestRecommendation::UseBundle : TestRecommendation::ManuallySelectTests);
    wxGetApp().GetAppOptions().SaveOptionsFile();
    wizard->Destroy();
    return true;
    }

//-------------------------------------------------------
bool BatchProjectDoc::OnSaveDocument(const wxString& filename)
    {
    if (!IsSafeToUpdate())
        { return false; }

    if (!GetFilename().empty() &&
        GetFilename() != filename)
        {
        // must be coming from Save As, so make sure file isn't locked
        try
            {
            MemoryMappedFile sourceFile(filename);
            }
        catch (const MemoryMappedFileShareViolationException&)
            {
            LogMessage(_(L"File appears to be open by another application. Cannot save project."),
                _(L"Project Save"), wxOK|wxICON_EXCLAMATION);
            return false;
            }
        // don't care about the file being empty or whatever, just if it's locked
        catch (...)
            {}
        m_FileReadOnly = false;
        m_File.Close();
        }

    // if we opened earlier in read only mode then bail
    if (m_FileReadOnly)
        {
        LogMessage(_(L"Project file was opened as read only. Unable to save."),
                _(L"Project Save"), wxOK|wxICON_INFORMATION);
        return false;
        }

    if (!m_File.IsOpened() )
        {
        // if the file is already there and it is in use then fail
        if (!m_File.Open(filename, wxFile::write) )
            {
            m_FileReadOnly = true;
            LogMessage(_(L"File appears to be open by another application. Cannot save project."),
                _(L"Project Save"), wxOK|wxICON_EXCLAMATION);
            return false;
            }
        }

    SetFilename(filename, true);
    SetTitle(ParseTitleFromFileName(filename));

    /* Write the (zip file) project out to a temp file first, then swap it.
       This helps us from corrupting the original file if something goes wrong
       during the write process.*/
    wxTempFileOutputStream out(filename);
    wxZipOutputStream zip(out, 9/*Maximum compression*/);

    // settings.xml
    Wisteria::ZipCatalog::WriteText(zip, ProjectSettingsFileLabel(), FormatProjectSettings());

    // if storing indexed text, then include it
    if (GetDocumentStorageMethod() == TextStorage::EmbedText)
        {
        int counter{ 1 };
        wxProgressDialog progressDlg(wxString::Format(_(L"Saving \"%s\""), GetTitle()),
            wxString{}, static_cast<int>(m_docs.size()),
            nullptr, wxPD_AUTO_HIDE|wxPD_SMOOTH|wxPD_CAN_ABORT|wxPD_APP_MODAL);
        progressDlg.Centre();
        /* Use buffered output stream, NOT text output stream. Text output buffer
           messes around with the newlines in the text, whereas buffer streams preserve the text.*/
        for (std::vector<BaseProject*>::iterator pos = m_docs.begin();
            pos != m_docs.end();
            ++pos)
            {
            Wisteria::ZipCatalog::WriteText(zip, wxString::Format(L"Content%zu.txt", pos-m_docs.begin()),
                (*pos)->GetDocumentText());

            if (!progressDlg.Update(counter++))
                {
                zip.Close();
                m_File.Close();
                return false;
                }
            }
        }
    zip.Close();

    // close the project, replace it with the temp file, and (re)lock it
    m_File.Close();
    if (!out.Commit())
        {
        LogMessage(_(L"Unable to save project file. File may be locked by another process."),
                _(L"Project Error"), wxOK|wxICON_EXCLAMATION);
        return false;
        }
    if (!LockProjectFile())
        { return false; }

    Modify(false);
    SetDocumentSaved(true);
    return true;
    }

//-------------------------------------------------------
bool BatchProjectDoc::OnOpenDocument(const wxString& filename)
    {
    wxLogMessage(L"Opening project \"%s\"", filename);
    // make sure there aren't any projects getting updated before we start opening a new one.
    // opening a project may try to add new custom tests, which would cause a race condition with
    // the processing project
    wxList docs = wxGetApp().GetDocManager()->GetDocuments();
    for (size_t i = 0; i < docs.GetCount(); ++i)
        {
        const BaseProjectDoc* doc = dynamic_cast<BaseProjectDoc*>(docs.Item(i)->GetData());
        if (!doc->IsSafeToUpdate())
            { return false; }
        }
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureProfessionalCode()))
        {
        LogMessage(_(L"You are not currently licensed to create a new batch project."),
            _(L"Error"), wxOK|wxICON_ERROR);
        return false;
        }

    wxBusyCursor wait;

    // make sure the file exists first
    if (!wxFile::Exists(filename) )
        {
        LogMessage(
            wxString::Format(_(L"'%s': unable to open project file. File not found."), filename),
            _(L"Error"), wxOK|wxICON_ERROR);
        return false;
        }
    if (!OnSaveModified())
        { return false; }

    SetTitle(ParseTitleFromFileName(filename));
    SetFilename(filename, true);
    Modify(false);
    SetDocumentSaved(true);

    BaseProjectProcessingLock processingLock(this);

    MemoryMappedFile sourceFile;
    try
        {
        sourceFile.MapFile(GetFilename());
        const char* projectFileText = static_cast<char*>(sourceFile.GetStream());
        LoadProjectFile(projectFileText, sourceFile.GetMapSize());
        // unmap and lock the file while project is open
        sourceFile.UnmapFile();
        if (!LockProjectFile())
            { return false; }
        }
    catch (const MemoryMappedFileShareViolationException&)
        {
        /* Couldn't get a map of it (might be open in another process),
           so try to buffer it.*/
        if (m_File.Open(GetFilename(), wxFile::read) )
            {
            m_FileReadOnly = true;
            LogMessage(_(L"File appears to be open by another application. "
                         "Project file will be opened as read only."),
                       _(L"Project Open"), wxOK | wxICON_INFORMATION);
            }
        else
            {
            LogMessage(
                wxString::Format(_(L"'%s': unable to open project file."), GetFilename()),
                _(L"Project Open"), wxOK|wxICON_EXCLAMATION);
            return false;
            }
        auto projectFileText = std::make_unique<char[]>(m_File.Length()+1);

        m_File.Seek(0);
        const size_t readSize = m_File.Read(projectFileText.get(), m_File.Length());
        LoadProjectFile(projectFileText.get(), readSize);
        }
    catch (const MemoryMappedFileEmptyException&)
        {
        LogMessage(_(L"Invalid project file. File is empty."),
            _(L"Project Open"), wxOK|wxICON_EXCLAMATION);
        return false;
        }
    catch (const MemoryMappedInvalidFileType&)
        {
        LogMessage(_(L"Invalid file."),
            _(L"Project Open"), wxOK|wxICON_EXCLAMATION);
        return false;
        }
    catch (const MemoryMappedFileCloudFileError&)
        {
        LogMessage(_(L"Unable to open file from Cloud service."),
            _(L"Project Open"), wxOK|wxICON_EXCLAMATION);
        return false;
        }
    catch (const MemoryMappedFileException&)
        {
        // Couldn't get a map of the file for unknown reason, so buffer it.
        if (!LockProjectFile())
            { return false; }
        auto projectFileText = std::make_unique<char[]>(m_File.Length()+1);

        m_File.Seek(0);
        const size_t readSize = m_File.Read(projectFileText.get(), m_File.Length());
        LoadProjectFile(projectFileText.get(), readSize);
        }
    catch (...)
        {
        LogMessage(
            wxString::Format(_(L"'%s': unable to open project file."), GetFilename()),
            _(L"Project Open"), wxOK|wxICON_EXCLAMATION);
        return false;
        }

    wxProgressDialog progressDlg(
        wxString::Format(_(L"Opening \"%s\""), GetTitle()),
        wxString::Format(
            _(L"Analyzing %s documents..."),
            wxNumberFormatter::ToString(GetSourceFilesInfo().size(), 0,
                                        wxNumberFormatter::Style::Style_WithThousandsSep)),
        static_cast<int>(GetSourceFilesInfo().size() + 13), nullptr,
        wxPD_AUTO_HIDE | wxPD_SMOOTH | wxPD_ELAPSED_TIME | wxPD_CAN_ABORT | wxPD_APP_MODAL);
    progressDlg.Centre();
    int counter{ 1 };

    // If externally linking to the documents, then reset document collection.
    // Note that if the documents are embedded, then they would already be
    // initialized and loaded from the project file.
    if (GetDocumentStorageMethod() == TextStorage::NoEmbedText)
        { InitializeDocuments(); }

    if (!progressDlg.Update(counter++))
        { return false; }
    if (!LoadDocuments(progressDlg))
        { return false; }
    counter = progressDlg.GetValue();

    LoadGroupingLabelsFromDocumentsInfo();

    // prompt user about removing any failed documents.
    // If they request to leave them in, then load any warnings for all documents.
    if (!CheckForFailedDocuments())
        { LoadWarningsSection(); }

    if (!progressDlg.Update(counter++, _(L"Loading Dolch statistics...")))
        { return false; }
    LoadDolchSection();

    if (!progressDlg.Update(counter++, _(L"Loading difficult words...")))
        { return false; }
    LoadHardWordsSection();

    if (!progressDlg.Update(counter++, _(L"Loading graphs...")))
        { return false; }
    DisplayReadabilityGraphs();

    if (!progressDlg.Update(counter++, _(L"Loading scores...")))
        { return false; }
    LoadScoresSection();

    if (!progressDlg.Update(counter++, _(L"Loading summary statistics...")))
        { return false; }
    LoadSummaryStatsSection();

    if (!progressDlg.Update(counter++))
        { return false; }
    DisplayScores();

    if (!progressDlg.Update(counter++))
        { return false; }
    DisplayBoxPlots();

    if (!progressDlg.Update(counter++))
        { return false; }
    DisplayHistograms();

    if (!progressDlg.Update(counter++, _(L"Loading grammar information...")))
        { return false; }
    DisplayGrammar();

    if (!progressDlg.Update(counter++))
        { return false; }
    DisplayHardWords();
    DisplaySentencesBreakdown();
    DisplaySummaryStats();

    if (!progressDlg.Update(counter++))
        { return false; }
    DisplaySightWords();

    if (!progressDlg.Update(counter++))
        { return false; }
    DisplayWarnings();

    BatchProjectView* view = dynamic_cast<BatchProjectView*>(GetFirstView());
    view->UpdateSideBarIcons();
    view->UpdateRibbonState();
    view->Present();
    UpdateAllViews();

    view->GetSideBar()->SelectSubItem(
        view->GetSideBar()->FindSubItem(BatchProjectView::ID_SCORE_LIST_PAGE_ID));
    auto* scoresWindow = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
        view->GetScoresView().FindWindowById(BaseProjectView::ID_SCORE_LIST_PAGE_ID));
    if (scoresWindow != nullptr && scoresWindow->GetItemCount() > 0)
        {
        scoresWindow->Select(0);
        }

    return true;
    }

//-------------------------------------------------------
void BatchProjectDoc::LoadProjectFile(const char* projectFileText, const size_t textLength)
    {
    Wisteria::ZipCatalog cat(projectFileText, textLength);

    // open the project settings file
    std::wstring settingsFile = cat.ReadTextFile(ProjectSettingsFileLabel());
    if (!settingsFile.empty())
        {
        LoadSettingsFile(settingsFile.c_str());
        }
    else
        {
        LogMessage(_(L"Settings file could not be found in the project file. "
                      "Default settings will be used."),
                   wxGetApp().GetAppName(), wxOK | wxICON_INFORMATION);
        }

    InitializeDocuments();

    // if storing indexed text then read that in from the project file
    // and assign it to respective documents.
    if (GetDocumentStorageMethod() == TextStorage::EmbedText)
        {
        for (auto pos = m_docs.begin();
             pos != m_docs.end();
             ++pos)
            {
            (*pos)->SetDocumentText(
                cat.ReadTextFile(wxString::Format(_DT(L"Content%zu.txt"), pos - m_docs.begin())));
            }
        }
    }

//-------------------------------------------------------
void BatchProjectDoc::DisplayGrammar()
    {
    PROFILE();
    BatchProjectView* view = dynamic_cast<BatchProjectView*>(GetFirstView());
    assert(view);

    // Wording Errors
    Wisteria::UI::ListCtrlEx* listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
        view->GetGrammarView().FindWindowById(
            BaseProjectView::WORDING_ERRORS_LIST_PAGE_ID));
    if (GetGrammarInfo().IsWordingErrorsEnabled() &&m_wordingErrorData->GetItemCount())
        {
        if (!listView)
            {
            listView = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::WORDING_ERRORS_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetPhrasingErrorsTabLabel());
            listView->SetName(BaseProjectView::GetPhrasingErrorsTabLabel());
            listView->EnableGridLines();
            listView->EnableItemViewOnDblClick();
            listView->InsertColumn(0, _(L"Document"));
            listView->InsertColumn(1, _(L"Label"));
            listView->InsertColumn(2, _(L"Frequency"));
            listView->InsertColumn(3, BaseProjectView::GetPhrasingErrorsTabLabel());
            listView->InsertColumn(4, _(L"Suggestions"));
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            view->GetGrammarView().AddWindow(listView);
            }
        UpdateListOptions(listView);
        listView->SetColumnFilePathTruncationMode(0, GetFilePathTruncationMode());
        listView->SetVirtualDataProvider(m_wordingErrorData);
        listView->SetVirtualDataSize(m_wordingErrorData->GetItemCount());
        listView->SetColumnWidth(0, listView->EstimateColumnWidth(0));
        listView->SetColumnWidth(1, std::min(listView->EstimateColumnWidth(1), view->GetMaxColumnWidth()));
        listView->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);
        listView->SetColumnWidth(3, std::min(listView->EstimateColumnWidth(3), view->GetMaxColumnWidth()));
        listView->SetColumnWidth(4, std::min(listView->EstimateColumnWidth(4), view->GetMaxColumnWidth()));
        if (listView->GetSortedColumn() == -1)
            { listView->SortColumn(0, Wisteria::SortDirection::SortAscending); }
        else
            { listView->Resort(); }
        }
    else
        { view->GetGrammarView().RemoveWindowById(BaseProjectView::WORDING_ERRORS_LIST_PAGE_ID); }

    // Misspelled words
    listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
        view->GetGrammarView().FindWindowById(
            BaseProjectView::MISSPELLED_WORD_LIST_PAGE_ID));
    if (GetGrammarInfo().IsMisspellingsEnabled() && GetMisspelledWordData()->GetItemCount())
        {
        if (!listView)
            {
            listView = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::MISSPELLED_WORD_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetMisspellingsLabel());
            listView->SetName(BaseProjectView::GetMisspellingsLabel());
            listView->EnableGridLines();
            listView->InsertColumn(0, _(L"Document"));
            listView->InsertColumn(1, _(L"Label"));
            listView->InsertColumn(2, _(L"Frequency"));
            listView->InsertColumn(3, _(L"Unique Count"));
            listView->InsertColumn(4, BaseProjectView::GetMisspellingsLabel());
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            view->GetGrammarView().AddWindow(listView);
            }
        UpdateListOptions(listView);
        listView->SetColumnFilePathTruncationMode(0, GetFilePathTruncationMode());
        listView->SetVirtualDataProvider(GetMisspelledWordData());
        listView->SetVirtualDataSize(GetMisspelledWordData()->GetItemCount());
        listView->SetColumnWidth(0, listView->EstimateColumnWidth(0));
        listView->SetColumnWidth(1, std::min(listView->EstimateColumnWidth(1), view->GetMaxColumnWidth()));
        listView->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);
        listView->SetColumnWidth(3, wxLIST_AUTOSIZE_USEHEADER);
        listView->SetColumnWidth(4, std::min(listView->EstimateColumnWidth(4), view->GetMaxColumnWidth()));
        if (listView->GetSortedColumn() == -1)
            { listView->SortColumn(0, Wisteria::SortDirection::SortAscending); }
        else
            { listView->Resort(); }
        }
    else
        { view->GetGrammarView().RemoveWindowById(BaseProjectView::MISSPELLED_WORD_LIST_PAGE_ID); }

    // Repeated words
    listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
        view->GetGrammarView().FindWindowById(
            BaseProjectView::DUPLICATES_LIST_PAGE_ID));
    if (GetGrammarInfo().IsRepeatedWordsEnabled() && GetRepeatedWordData()->GetItemCount())
        {
        if (!listView)
            {
            listView = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::DUPLICATES_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetRepeatedWordsLabel());
            listView->SetName(BaseProjectView::GetRepeatedWordsLabel());
            listView->EnableGridLines();
            listView->EnableItemViewOnDblClick();
            listView->InsertColumn(0, _(L"Document"));
            listView->InsertColumn(1, _(L"Label"));
            listView->InsertColumn(2, _(L"Frequency"));
            listView->InsertColumn(3, _(L"Repeated Words"));
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            view->GetGrammarView().AddWindow(listView);
            }
        UpdateListOptions(listView);
        listView->SetColumnFilePathTruncationMode(0, GetFilePathTruncationMode());
        listView->SetVirtualDataProvider(GetRepeatedWordData());
        listView->SetVirtualDataSize(GetRepeatedWordData()->GetItemCount());
        listView->SetColumnWidth(0, listView->EstimateColumnWidth(0));
        listView->SetColumnWidth(1, std::min(listView->EstimateColumnWidth(1), view->GetMaxColumnWidth()));
        listView->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);
        listView->SetColumnWidth(3, std::min(listView->EstimateColumnWidth(3), view->GetMaxColumnWidth()));
        if (listView->GetSortedColumn() == -1)
            { listView->SortColumn(0, Wisteria::SortDirection::SortAscending); }
        else
            { listView->Resort(); }
        }
    else
        { view->GetGrammarView().RemoveWindowById(BaseProjectView::DUPLICATES_LIST_PAGE_ID); }

    // Incorrect articles
    listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
        view->GetGrammarView().FindWindowById(
            BaseProjectView::INCORRECT_ARTICLE_PAGE_ID));
    if (GetGrammarInfo().IsArticleMismatchesEnabled() && m_incorrectArticleData->GetItemCount())
        {
        if (!listView)
            {
            listView = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::INCORRECT_ARTICLE_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetArticleMismatchesLabel());
            listView->SetName(BaseProjectView::GetArticleMismatchesLabel());
            listView->EnableGridLines();
            listView->EnableItemViewOnDblClick();
            listView->InsertColumn(0, _(L"Document"));
            listView->InsertColumn(1, _(L"Label"));
            listView->InsertColumn(2, _(L"Frequency"));
            listView->InsertColumn(3, BaseProjectView::GetArticleMismatchesLabel());
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            view->GetGrammarView().AddWindow(listView);
            }
        UpdateListOptions(listView);
        listView->SetColumnFilePathTruncationMode(0, GetFilePathTruncationMode());
        listView->SetVirtualDataProvider(m_incorrectArticleData);
        listView->SetVirtualDataSize(m_incorrectArticleData->GetItemCount());
        listView->SetColumnWidth(0, listView->EstimateColumnWidth(0));
        listView->SetColumnWidth(1, std::min(listView->EstimateColumnWidth(1), view->GetMaxColumnWidth()));
        listView->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);
        listView->SetColumnWidth(3, std::min(listView->EstimateColumnWidth(3), view->GetMaxColumnWidth()));
        if (listView->GetSortedColumn() == -1)
            { listView->SortColumn(0, Wisteria::SortDirection::SortAscending); }
        else
            { listView->Resort(); }
        }
    else
        { view->GetGrammarView().RemoveWindowById(BaseProjectView::INCORRECT_ARTICLE_PAGE_ID); }

    // redundant phrases
    listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
        view->GetGrammarView().FindWindowById(
            BaseProjectView::REDUNDANT_PHRASE_LIST_PAGE_ID));
    if (GetGrammarInfo().IsRedundantPhrasesEnabled() && m_redundantPhraseData->GetItemCount())
        {
        if (!listView)
            {
            listView = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::REDUNDANT_PHRASE_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetRedundantPhrasesTabLabel());
            listView->SetName(BaseProjectView::GetRedundantPhrasesTabLabel());
            listView->EnableGridLines();
            listView->EnableItemViewOnDblClick();
            listView->InsertColumn(0, _(L"Document"));
            listView->InsertColumn(1, _(L"Label"));
            listView->InsertColumn(2, _(L"Frequency"));
            listView->InsertColumn(3, BaseProjectView::GetRedundantPhrasesTabLabel());
            listView->InsertColumn(4, _(L"Suggestions"));
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            view->GetGrammarView().AddWindow(listView);
            }
        UpdateListOptions(listView);
        listView->SetColumnFilePathTruncationMode(0, GetFilePathTruncationMode());
        listView->SetVirtualDataProvider(m_redundantPhraseData);
        listView->SetVirtualDataSize(m_redundantPhraseData->GetItemCount());
        listView->SetColumnWidth(0, listView->EstimateColumnWidth(0));
        listView->SetColumnWidth(1, std::min(listView->EstimateColumnWidth(1), view->GetMaxColumnWidth()));
        listView->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);
        listView->SetColumnWidth(3, std::min(listView->EstimateColumnWidth(3), view->GetMaxColumnWidth()));
        listView->SetColumnWidth(4, std::min(listView->EstimateColumnWidth(4), view->GetMaxColumnWidth()));
        if (listView->GetSortedColumn() == -1)
            { listView->SortColumn(0, Wisteria::SortDirection::SortAscending); }
        else
            { listView->Resort(); }
        }
    else
        { view->GetGrammarView().RemoveWindowById(BaseProjectView::REDUNDANT_PHRASE_LIST_PAGE_ID); }

    // overused words (by sentence)
    listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(view->GetGrammarView().FindWindowById(
            BaseProjectView::OVERUSED_WORDS_BY_SENTENCE_LIST_PAGE_ID));
    if (GetGrammarInfo().IsOverUsedWordsBySentenceEnabled() && m_overusedWordBySentenceData->GetItemCount())
        {
        if (!listView)
            {
            listView = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::OVERUSED_WORDS_BY_SENTENCE_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetOverusedWordsBySentenceLabel());
            listView->SetName(BaseProjectView::GetOverusedWordsBySentenceLabel());
            listView->EnableGridLines();
            listView->EnableItemViewOnDblClick();
            listView->InsertColumn(0, _(L"Document"));
            listView->InsertColumn(1, _(L"Label"));
            listView->InsertColumn(2, _(L"Frequency"));
            listView->InsertColumn(3, _(L"Overused Words"));
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            view->GetGrammarView().AddWindow(listView);
            }
        UpdateListOptions(listView);
        listView->SetColumnFilePathTruncationMode(0, GetFilePathTruncationMode());
        listView->SetVirtualDataProvider(m_overusedWordBySentenceData);
        listView->SetVirtualDataSize(m_overusedWordBySentenceData->GetItemCount());
        listView->SetColumnWidth(0, listView->EstimateColumnWidth(0));
        listView->SetColumnWidth(1, std::min(listView->EstimateColumnWidth(1), view->GetMaxColumnWidth()));
        listView->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);
        listView->SetColumnWidth(3, std::min(listView->EstimateColumnWidth(3), view->GetMaxColumnWidth()));
        if (listView->GetSortedColumn() == -1)
            { listView->SortColumn(0, Wisteria::SortDirection::SortAscending); }
        else
            { listView->Resort(); }
        }
    else
        { view->GetGrammarView().RemoveWindowById(BaseProjectView::OVERUSED_WORDS_BY_SENTENCE_LIST_PAGE_ID); }

    // wordy items
    listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
        view->GetGrammarView().FindWindowById(
            BaseProjectView::WORDY_PHRASES_LIST_PAGE_ID));
    if (GetGrammarInfo().IsWordyPhrasesEnabled() && m_wordyPhraseData->GetItemCount())
        {
        if (!listView)
            {
            listView = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::WORDY_PHRASES_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetWordyPhrasesTabLabel());
            listView->SetName(BaseProjectView::GetWordyPhrasesTabLabel());
            listView->EnableGridLines();
            listView->EnableItemViewOnDblClick();
            listView->InsertColumn(0, _(L"Document"));
            listView->InsertColumn(1, _(L"Label"));
            listView->InsertColumn(2, _(L"Frequency"));
            listView->InsertColumn(3, BaseProjectView::GetWordyPhrasesTabLabel());
            listView->InsertColumn(4, _(L"Suggestions"));
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            view->GetGrammarView().AddWindow(listView);
            }
        UpdateListOptions(listView);
        listView->SetColumnFilePathTruncationMode(0, GetFilePathTruncationMode());
        listView->SetVirtualDataProvider(m_wordyPhraseData);
        listView->SetVirtualDataSize(m_wordyPhraseData->GetItemCount());
        listView->SetColumnWidth(0, listView->EstimateColumnWidth(0));
        listView->SetColumnWidth(1, std::min(listView->EstimateColumnWidth(1), view->GetMaxColumnWidth()));
        listView->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);
        listView->SetColumnWidth(3, std::min(listView->EstimateColumnWidth(3), view->GetMaxColumnWidth()));
        listView->SetColumnWidth(4, std::min(listView->EstimateColumnWidth(4), view->GetMaxColumnWidth()));
        if (listView->GetSortedColumn() == -1)
            { listView->SortColumn(0, Wisteria::SortDirection::SortAscending); }
        else
            { listView->Resort(); }
        }
    else
        { view->GetGrammarView().RemoveWindowById(BaseProjectView::WORDY_PHRASES_LIST_PAGE_ID); }

    // cliches
    listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
        view->GetGrammarView().FindWindowById(
            BaseProjectView::CLICHES_LIST_PAGE_ID));
    if (GetGrammarInfo().IsClichesEnabled() && m_clichePhraseData->GetItemCount())
        {
        if (!listView)
            {
            listView = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::CLICHES_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetClichesTabLabel());
            listView->SetName(BaseProjectView::GetClichesTabLabel());
            listView->EnableGridLines();
            listView->EnableItemViewOnDblClick();
            listView->InsertColumn(0, _(L"Document"));
            listView->InsertColumn(1, _(L"Label"));
            listView->InsertColumn(2, _(L"Frequency"));
            listView->InsertColumn(3, BaseProjectView::GetClichesTabLabel());
            listView->InsertColumn(4, _(L"Explanations/Suggestions"));
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            view->GetGrammarView().AddWindow(listView);
            }
        UpdateListOptions(listView);
        listView->SetColumnFilePathTruncationMode(0, GetFilePathTruncationMode());
        listView->SetVirtualDataProvider(m_clichePhraseData);
        listView->SetVirtualDataSize(m_clichePhraseData->GetItemCount());
        listView->SetColumnWidth(0, listView->EstimateColumnWidth(0));
        listView->SetColumnWidth(1, std::min(listView->EstimateColumnWidth(1), view->GetMaxColumnWidth()));
        listView->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);
        listView->SetColumnWidth(3, std::min(listView->EstimateColumnWidth(3), view->GetMaxColumnWidth()));
        listView->SetColumnWidth(4, std::min(listView->EstimateColumnWidth(4), view->GetMaxColumnWidth()));
        if (listView->GetSortedColumn() == -1)
            { listView->SortColumn(0, Wisteria::SortDirection::SortAscending); }
        else
            { listView->Resort(); }
        }
    else
        { view->GetGrammarView().RemoveWindowById(BaseProjectView::CLICHES_LIST_PAGE_ID); }

    // Passive voice
    listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
        view->GetGrammarView().FindWindowById(
            BaseProjectView::PASSIVE_VOICE_PAGE_ID));
    if (GetGrammarInfo().IsPassiveVoiceEnabled() && m_passiveVoiceData->GetItemCount())
        {
        if (!listView)
            {
            listView = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::PASSIVE_VOICE_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetPassiveLabel());
            listView->SetName(BaseProjectView::GetPassiveLabel());
            listView->EnableGridLines();
            listView->EnableItemViewOnDblClick();
            listView->InsertColumn(0, _(L"Document"));
            listView->InsertColumn(1, _(L"Label"));
            listView->InsertColumn(2, _(L"Frequency"));
            listView->InsertColumn(3, BaseProjectView::GetPassiveLabel());
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            view->GetGrammarView().AddWindow(listView);
            }
        UpdateListOptions(listView);
        listView->SetColumnFilePathTruncationMode(0, GetFilePathTruncationMode());
        listView->SetVirtualDataProvider(m_passiveVoiceData);
        listView->SetVirtualDataSize(m_passiveVoiceData->GetItemCount());
        listView->SetColumnWidth(0, listView->EstimateColumnWidth(0));
        listView->SetColumnWidth(1, std::min(listView->EstimateColumnWidth(1), view->GetMaxColumnWidth()));
        listView->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);
        listView->SetColumnWidth(3, std::min(listView->EstimateColumnWidth(3), view->GetMaxColumnWidth()));
        if (listView->GetSortedColumn() == -1)
            { listView->SortColumn(0, Wisteria::SortDirection::SortAscending); }
        else
            { listView->Resort(); }
        }
    else
        { view->GetGrammarView().RemoveWindowById(BaseProjectView::PASSIVE_VOICE_PAGE_ID); }

    // sentences that begin with conjunctions
    listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(view->GetGrammarView().FindWindowById(
            BaseProjectView::SENTENCES_CONJUNCTION_START_LIST_PAGE_ID));
    if (GetGrammarInfo().IsConjunctionStartingSentencesEnabled() &&
        m_sentenceStartingWithConjunctionsData->GetItemCount())
        {
        if (!listView)
            {
            listView = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::SENTENCES_CONJUNCTION_START_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetSentenceStartingWithConjunctionsTabLabel());
            listView->SetName(BaseProjectView::GetSentenceStartingWithConjunctionsTabLabel());
            listView->EnableGridLines();
            listView->EnableItemViewOnDblClick();
            listView->InsertColumn(0, _(L"Document"));
            listView->InsertColumn(1, _(L"Label"));
            listView->InsertColumn(2, _(L"Frequency"));
            listView->InsertColumn(3, _(L"Conjunctions"));
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            view->GetGrammarView().AddWindow(listView);
            }
        UpdateListOptions(listView);
        listView->SetColumnFilePathTruncationMode(0, GetFilePathTruncationMode());
        listView->SetVirtualDataProvider(m_sentenceStartingWithConjunctionsData);
        listView->SetVirtualDataSize(m_sentenceStartingWithConjunctionsData->GetItemCount());
        listView->SetColumnWidth(0, listView->EstimateColumnWidth(0));
        listView->SetColumnWidth(1, std::min(listView->EstimateColumnWidth(1), view->GetMaxColumnWidth()));
        listView->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);
        listView->SetColumnWidth(3, std::min(listView->EstimateColumnWidth(3), view->GetMaxColumnWidth()));
        if (listView->GetSortedColumn() == -1)
            { listView->SortColumn(0, Wisteria::SortDirection::SortAscending); }
        else
            { listView->Resort(); }
        }
    else
        { view->GetGrammarView().RemoveWindowById(BaseProjectView::SENTENCES_CONJUNCTION_START_LIST_PAGE_ID); }

    // sentences that begin with lowercased words
    listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(view->GetGrammarView().FindWindowById(
            BaseProjectView::SENTENCES_LOWERCASE_START_LIST_PAGE_ID));
    if (GetGrammarInfo().IsLowercaseSentencesEnabled() && m_sentenceStartingWithLowercaseData->GetItemCount())
        {
        if (!listView)
            {
            listView = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::SENTENCES_LOWERCASE_START_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetSentenceStartingWithLowercaseTabLabel());
            listView->SetName(BaseProjectView::GetSentenceStartingWithLowercaseTabLabel());
            listView->EnableGridLines();
            listView->EnableItemViewOnDblClick();
            listView->InsertColumn(0, _(L"Document"));
            listView->InsertColumn(1, _(L"Label"));
            listView->InsertColumn(2, _(L"Frequency"));
            listView->InsertColumn(3, _(L"Starting Word"));
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            view->GetGrammarView().AddWindow(listView);
            }
        UpdateListOptions(listView);
        listView->SetColumnFilePathTruncationMode(0, GetFilePathTruncationMode());
        listView->SetVirtualDataProvider(m_sentenceStartingWithLowercaseData);
        listView->SetVirtualDataSize(m_sentenceStartingWithLowercaseData->GetItemCount());
        listView->SetColumnWidth(0, listView->EstimateColumnWidth(0));
        listView->SetColumnWidth(1, std::min(listView->EstimateColumnWidth(1), view->GetMaxColumnWidth()));
        listView->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);
        listView->SetColumnWidth(3, std::min(listView->EstimateColumnWidth(3), view->GetMaxColumnWidth()));
        if (listView->GetSortedColumn() == -1)
            { listView->SortColumn(0, Wisteria::SortDirection::SortAscending); }
        else
            { listView->Resort(); }
        }
    else
        { view->GetGrammarView().RemoveWindowById(BaseProjectView::SENTENCES_LOWERCASE_START_LIST_PAGE_ID); }
    }

//-------------------------------------------------------
void BatchProjectDoc::DisplaySummaryStats()
    {
    BatchProjectView* view = dynamic_cast<BatchProjectView*>(GetFirstView());
    // summary stats
    Wisteria::UI::ListCtrlEx* listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
        view->GetSummaryStatsView().FindWindowById(
            BaseProjectView::STATS_LIST_PAGE_ID));
    if (m_summaryStatsData->GetItemCount() &&
        GetStatisticsInfo().IsTableEnabled() &&
        GetStatisticsReportInfo().HasStatisticsEnabled())
        {
        if (!listView)
            {
            listView = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::STATS_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetSummaryStatisticsLabel());
            listView->SetName(BaseProjectView::GetSummaryStatisticsLabel());
            listView->EnableGridLines();
            listView->EnableItemViewOnDblClick();
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            view->GetSummaryStatsView().AddWindow(listView);
            }
        listView->DeleteAllColumns();
        for (size_t i = 0; i < m_summaryStatsColumnNames.size(); ++i)
            { listView->InsertColumn(i, m_summaryStatsColumnNames[i]); }

        UpdateListOptions(listView);
        listView->SetColumnFilePathTruncationMode(0, GetFilePathTruncationMode());
        listView->SetVirtualDataProvider(m_summaryStatsData);
        listView->SetVirtualDataSize(m_summaryStatsData->GetItemCount());
        listView->SetColumnWidth(0, listView->EstimateColumnWidth(0));
        listView->SetColumnWidth(1, std::min(listView->EstimateColumnWidth(1), view->GetMaxColumnWidth()));
        for (size_t i = 2; i < m_summaryStatsColumnNames.size(); ++i)
            { listView->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER); }

        if (listView->GetSortedColumn() == -1)
            { listView->SortColumn(0, Wisteria::SortDirection::SortAscending); }
        else
            { listView->Resort(); }
        }
    else
        { view->GetSummaryStatsView().RemoveWindowById(BaseProjectView::STATS_LIST_PAGE_ID); }
    }

//-------------------------------------------------------
void BatchProjectDoc::DisplaySentencesBreakdown()
    {
    BatchProjectView* view = dynamic_cast<BatchProjectView*>(GetFirstView());
    // long sentences
    Wisteria::UI::ListCtrlEx* listView =
        dynamic_cast<Wisteria::UI::ListCtrlEx*>(view->GetSentencesBreakdownView().FindWindowById(
            BaseProjectView::LONG_SENTENCES_LIST_PAGE_ID));
    if (m_overlyLongSentenceData->GetItemCount())
        {
        if (!listView)
            {
            listView = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::LONG_SENTENCES_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetLongSentencesLabel());
            listView->SetName(BaseProjectView::GetLongSentencesLabel());
            listView->EnableGridLines();
            listView->EnableItemViewOnDblClick();
            listView->InsertColumn(0, _(L"Document"));
            listView->InsertColumn(1, _(L"Label"));
            listView->InsertColumn(2, _(L"Overly-long Sentences"));
            listView->InsertColumn(3, _(L"Longest Sentence Length"));
            listView->InsertColumn(4, _(L"Longest Sentence"));
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            view->GetSentencesBreakdownView().AddWindow(listView);
            }
        UpdateListOptions(listView);
        listView->SetColumnFilePathTruncationMode(0, GetFilePathTruncationMode());
        listView->SetVirtualDataProvider(m_overlyLongSentenceData);
        listView->SetVirtualDataSize(m_overlyLongSentenceData->GetItemCount());
        listView->SetColumnWidth(0, listView->EstimateColumnWidth(0));
        listView->SetColumnWidth(1, std::min(listView->EstimateColumnWidth(1), view->GetMaxColumnWidth()));
        listView->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);
        listView->SetColumnWidth(3, wxLIST_AUTOSIZE_USEHEADER);
        listView->SetColumnWidth(4, listView->EstimateColumnWidth(3));
        if (listView->GetSortedColumn() == -1)
            { listView->SortColumn(0, Wisteria::SortDirection::SortAscending); }
        else
            { listView->Resort(); }
        }
    else
        { view->GetSentencesBreakdownView().RemoveWindowById(BaseProjectView::LONG_SENTENCES_LIST_PAGE_ID); }
    }

//-------------------------------------------------------
void BatchProjectDoc::DisplayHardWords()
    {
    PROFILE();
    BatchProjectView* view = dynamic_cast<BatchProjectView*>(GetFirstView());

    // Difficult words
    if (m_hardWordsData->GetItemCount())
        {
        Wisteria::UI::ListCtrlEx* listView =
            dynamic_cast<Wisteria::UI::ListCtrlEx*>(view->GetWordsBreakdownView().FindWindowById(
                BaseProjectView::ID_DIFFICULT_WORDS_LIST_PAGE_ID));
        if (!listView)
            {
            listView = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::ID_DIFFICULT_WORDS_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetDifficultWordsLabel());
            listView->SetName(BaseProjectView::GetDifficultWordsLabel());
            listView->EnableGridLines();
            listView->EnableItemViewOnDblClick();
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            view->GetWordsBreakdownView().InsertWindow(0, listView);
            }
        listView->DeleteAllColumns();
        long columnIndex = 0;
        listView->InsertColumn(columnIndex++, _(L"Document"));
        listView->SetColumnFilePathTruncationMode(0, GetFilePathTruncationMode());
        listView->InsertColumn(columnIndex++, _(L"Label"));
        listView->InsertColumn(columnIndex++, _(L"Total Words"));
        listView->InsertColumn(columnIndex++, _(L"% of complex (3+ syllable) words"));
        listView->InsertColumn(columnIndex++, _(L"Complex (3+ syllable) words"));
        listView->InsertColumn(columnIndex++, _(L"% of long (6+ characters) words"));
        listView->InsertColumn(columnIndex++, _(L"Long (6+ characters) words"));
        if (GetStatisticsReportInfo().IsExtendedWordsEnabled())
            {
            if (IsSmogLikeTestIncluded())
                {
                listView->InsertColumn(columnIndex++, _(L"% of SMOG hard words"));
                listView->InsertColumn(columnIndex++, _(L"SMOG hard words"));
                }
            if (GetReadabilityTests().is_test_included(ReadabilityMessages::GUNNING_FOG()) )
                {
                listView->InsertColumn(columnIndex++, _(L"% of Fog hard words"));
                listView->InsertColumn(columnIndex++, _(L"Fog hard words"));
                }
            if (IsDaleChallLikeTestIncluded())
                {
                listView->InsertColumn(columnIndex++, _(L"% of Dale-Chall unfamiliar words"));
                listView->InsertColumn(columnIndex++, _(L"Dale-Chall unfamiliar words"));
                }
            if (GetReadabilityTests().is_test_included(ReadabilityMessages::SPACHE()) )
                {
                listView->InsertColumn(columnIndex++, _(L"% of Spache unfamiliar words"));
                listView->InsertColumn(columnIndex++, _(L"Spache unfamiliar words"));
                }
            if (GetReadabilityTests().is_test_included(ReadabilityMessages::HARRIS_JACOBSON()) )
                {
                listView->InsertColumn(columnIndex++, _(L"% of HJ unfamiliar words"));
                listView->InsertColumn(columnIndex++, _(L"HJ unfamiliar words"));
                }
            for (std::vector<CustomReadabilityTestInterface>::const_iterator pos = GetCustTestsInUse().cbegin();
                pos != GetCustTestsInUse().cend();
                ++pos)
                {
                if (!pos->GetIterator()->is_using_familiar_words())
                    { continue; }
                listView->InsertColumn(columnIndex++,
                    wxString::Format(_(L"%% of %s unfamiliar words"), pos->GetIterator()->get_name().c_str()));
                listView->InsertColumn(columnIndex++,
                    wxString::Format(_(L"%s unfamiliar words"), pos->GetIterator()->get_name().c_str()));
                }
            }
        UpdateListOptions(listView);
        listView->SetVirtualDataProvider(m_hardWordsData);
        listView->SetVirtualDataSize(m_hardWordsData->GetItemCount());
        listView->SetColumnWidth(0, listView->EstimateColumnWidth(0));
        listView->SetColumnWidth(1, std::min(listView->EstimateColumnWidth(1), view->GetMaxColumnWidth()));
        for (long i = 2; i < listView->GetColumnCount(); ++i)
            { listView->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER); }
        if (listView->GetSortedColumn() == -1)
            { listView->SortColumn(0, Wisteria::SortDirection::SortAscending); }
        else
            { listView->Resort(); }
        }
    else
        { view->GetWordsBreakdownView().RemoveWindowById(BaseProjectView::ID_DIFFICULT_WORDS_LIST_PAGE_ID); }

    // All words
    if (GetAllWordsBatchData()->GetItemCount())
        {
        Wisteria::UI::ListCtrlEx* listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
            view->GetWordsBreakdownView().FindWindowById(
                BaseProjectView::ALL_WORDS_LIST_PAGE_ID));
        if (!listView)
            {
            listView = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::ALL_WORDS_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetAllWordsLabel());
            listView->SetName(BaseProjectView::GetAllWordsLabel());
            listView->EnableGridLines();
            listView->EnableItemViewOnDblClick();
            listView->InsertColumn(0, _(L"Word"));
            listView->InsertColumn(1, _(L"Frequency"));
            listView->InsertColumn(2, _(L"Document Count"));
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            view->GetWordsBreakdownView().AddWindow(listView);
            }
        UpdateListOptions(listView);
        listView->SetVirtualDataProvider(GetAllWordsBatchData());
        listView->SetVirtualDataSize(GetAllWordsBatchData()->GetItemCount());
        listView->SetColumnWidth(0, listView->EstimateColumnWidth(0));
        listView->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER);
        listView->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);
        }
    else
        { view->GetWordsBreakdownView().RemoveWindowById(BaseProjectView::ALL_WORDS_LIST_PAGE_ID); }

    // key words (uncommon words removed, remaining stemmed and combined)
    if (GetKeyWordsBatchData()->GetItemCount() &&
        // don't bother with condensed list if it has the same item count as the all words list
        // (that would mean that there was no condensing [stemming] that took place and that
        // these lists are the same).
        (GetKeyWordsBatchData()->GetItemCount() != GetAllWordsBatchData()->GetItemCount()))
        {
        Wisteria::UI::ListCtrlEx* listView =
            dynamic_cast<Wisteria::UI::ListCtrlEx*>(view->GetWordsBreakdownView().FindWindowById(
                BaseProjectView::ALL_WORDS_CONDENSED_LIST_PAGE_ID));
        if (!listView)
            {
            listView = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::ALL_WORDS_CONDENSED_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetKeyWordsLabel());
            listView->SetName(BaseProjectView::GetKeyWordsLabel());
            listView->EnableGridLines();
            listView->EnableItemViewOnDblClick();
            listView->InsertColumn(0, _(L"Word"));
            listView->InsertColumn(1, _(L"Frequency"));
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            view->GetWordsBreakdownView().AddWindow(listView);
            }
        UpdateListOptions(listView);
        listView->SetVirtualDataProvider(GetKeyWordsBatchData());
        listView->SetVirtualDataSize(GetKeyWordsBatchData()->GetItemCount());
        listView->Resort();
        listView->DistributeColumns();
        }
    else
        {
        // we are getting rid of this window (if nothing in it)
        view->GetWordsBreakdownView().RemoveWindowById(BaseProjectView::ALL_WORDS_CONDENSED_LIST_PAGE_ID);
        }

    // word cloud
    wxGCDC gdc(view->GetDocFrame());
    Wisteria::Canvas* wordCloudCanvas =
        dynamic_cast<Wisteria::Canvas*>(view->GetWordsBreakdownView().FindWindowById(
            BaseProjectView::WORD_CLOUD_PAGE_ID));
    if (m_keyWordsDataset->GetRowCount())
        {
        if (!wordCloudCanvas)
            {
            wordCloudCanvas = new Wisteria::Canvas(view->GetSplitter(),
                                                   BaseProjectView::WORD_CLOUD_PAGE_ID);
            wordCloudCanvas->SetFixedObjectsGridSize(1, 1);
            wordCloudCanvas->SetFixedObject(0, 0, std::make_shared<WordCloud>(wordCloudCanvas));
            wordCloudCanvas->Hide();
            wordCloudCanvas->SetLabel(BaseProjectView::GetWordCloudLabel());
            wordCloudCanvas->SetName(BaseProjectView::GetWordCloudLabel());
            wordCloudCanvas->SetPrinterSettings(*wxGetApp().GetPrintData());
            view->GetWordsBreakdownView().AddWindow(wordCloudCanvas);
            }
        UpdateGraphOptions(wordCloudCanvas);

        auto wordCloud = std::dynamic_pointer_cast<WordCloud>(wordCloudCanvas->GetFixedObject(0, 0));
        assert(wordCloud);
        // top 100 words, with a min frequency of 2
        wordCloud->SetData(m_keyWordsDataset,
                           GetWordsColumnName(), GetWordsCountsColumnName(), 2,
                           std::nullopt, 100);

        wordCloudCanvas->CalcAllSizes(gdc);
        }
    else
        {
        // we are getting rid of this window (if nothing in it)
        view->GetWordsBreakdownView().RemoveWindowById(BaseProjectView::WORD_CLOUD_PAGE_ID);
        }
    }

//-------------------------------------------------------
void BatchProjectDoc::DisplaySightWords()
    {
    PROFILE();
    BatchProjectView* view = dynamic_cast<BatchProjectView*>(GetFirstView());

    Wisteria::UI::ListCtrlEx* listView =
        dynamic_cast<Wisteria::UI::ListCtrlEx*>(view->GetDolchSightWordsView().FindWindowById(
            BaseProjectView::ID_DOLCH_COVERAGE_LIST_PAGE_ID));
    if (IsIncludingDolchSightWords() && m_dolchCompletionData->GetItemCount())
        {
        if (!listView)
            {
            listView = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::ID_DOLCH_COVERAGE_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetDolchCoverageTabLabel());
            listView->SetName(BaseProjectView::GetDolchCoverageTabLabel());
            listView->EnableGridLines();
            listView->EnableItemViewOnDblClick();
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            // add the columns
            listView->InsertColumn(listView->GetColumnCount(), _(L"Document"));
            listView->InsertColumn(listView->GetColumnCount(), _(L"Label"));
            listView->InsertColumn(listView->GetColumnCount(), _(L"Conjunctions Coverage"));
            listView->InsertColumn(listView->GetColumnCount(), _(L"Prepositions Coverage"));
            listView->InsertColumn(listView->GetColumnCount(), _(L"Pronouns Coverage"));
            listView->InsertColumn(listView->GetColumnCount(), _(L"Adverbs Coverage"));
            listView->InsertColumn(listView->GetColumnCount(), _(L"Adjectives Coverage"));
            listView->InsertColumn(listView->GetColumnCount(), _(L"Verbs Coverage"));
            listView->InsertColumn(listView->GetColumnCount(), _(L"Noun Coverage"));
            view->GetDolchSightWordsView().AddWindow(listView);
            }
        UpdateListOptions(listView);
        listView->SetColumnFilePathTruncationMode(0, GetFilePathTruncationMode());
        listView->SetVirtualDataProvider(m_dolchCompletionData);
        listView->SetVirtualDataSize(m_dolchCompletionData->GetItemCount());
        listView->SetColumnWidth(0, listView->EstimateColumnWidth(0));
        listView->SetColumnWidth(1, std::min(listView->EstimateColumnWidth(1), view->GetMaxColumnWidth()));
        for (long i = 2; i < listView->GetColumnCount(); ++i)
            { listView->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER); }
        if (listView->GetSortedColumn() == -1)
            { listView->SortColumn(0, Wisteria::SortDirection::SortAscending); }
        else
            { listView->Resort(); }
        }
    else
        { view->GetDolchSightWordsView().RemoveWindowById(BaseProjectView::ID_DOLCH_COVERAGE_LIST_PAGE_ID); }

    listView =
        dynamic_cast<Wisteria::UI::ListCtrlEx*>(view->GetDolchSightWordsView().FindWindowById(
            BaseProjectView::ID_DOLCH_WORDS_LIST_PAGE_ID));
    if (IsIncludingDolchSightWords() && m_dolchWordsBatchData->GetItemCount())
        {
        if (!listView)
            {
            listView = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::ID_DOLCH_WORDS_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetDolchWordTabLabel());
            listView->SetName(BaseProjectView::GetDolchWordTabLabel());
            listView->EnableGridLines();
            listView->EnableItemViewOnDblClick();
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            view->GetDolchSightWordsView().AddWindow(listView);
            }
        // insert the columns
        listView->DeleteAllColumns();
        listView->InsertColumn(listView->GetColumnCount(), _(L"Document"));
        listView->InsertColumn(listView->GetColumnCount(), _(L"Label"));
        listView->InsertColumn(listView->GetColumnCount(), _(L"% of Conjunctions"));
        if (GetStatisticsReportInfo().IsExtendedInformationEnabled())
            { listView->InsertColumn(listView->GetColumnCount(), _(L"Conjunctions")); }
        listView->InsertColumn(listView->GetColumnCount(), _(L"% of Prepositions"));
        if (GetStatisticsReportInfo().IsExtendedInformationEnabled())
            { listView->InsertColumn(listView->GetColumnCount(), _(L"Prepositions")); }
        listView->InsertColumn(listView->GetColumnCount(), _(L"% of Pronouns"));
        if (GetStatisticsReportInfo().IsExtendedInformationEnabled())
            { listView->InsertColumn(listView->GetColumnCount(), _(L"Pronouns")); }
        listView->InsertColumn(listView->GetColumnCount(), _(L"% of Adverbs"));
        if (GetStatisticsReportInfo().IsExtendedInformationEnabled())
            { listView->InsertColumn(listView->GetColumnCount(), _(L"Adverbs")); }
        listView->InsertColumn(listView->GetColumnCount(), _(L"% of Adjectives"));
        if (GetStatisticsReportInfo().IsExtendedInformationEnabled())
            { listView->InsertColumn(listView->GetColumnCount(), _(L"Adjectives")); }
        listView->InsertColumn(listView->GetColumnCount(), _(L"% of Verbs"));
        if (GetStatisticsReportInfo().IsExtendedInformationEnabled())
            { listView->InsertColumn(listView->GetColumnCount(), _(L"Verbs")); }
        listView->InsertColumn(listView->GetColumnCount(), _(L"% of Noun Words"));
        if (GetStatisticsReportInfo().IsExtendedInformationEnabled())
            { listView->InsertColumn(listView->GetColumnCount(), _(L"Nouns")); }

        UpdateListOptions(listView);
        listView->SetColumnFilePathTruncationMode(0, GetFilePathTruncationMode());
        listView->SetVirtualDataProvider(m_dolchWordsBatchData);
        listView->SetVirtualDataSize(m_dolchWordsBatchData->GetItemCount());
        listView->SetColumnWidth(0, listView->EstimateColumnWidth(0));
        listView->SetColumnWidth(1, std::min(listView->EstimateColumnWidth(1), view->GetMaxColumnWidth()));
        for (long i = 2; i < listView->GetColumnCount(); ++i)
            { listView->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER); }
        if (listView->GetSortedColumn() == -1)
            { listView->SortColumn(0, Wisteria::SortDirection::SortAscending); }
        else
            { listView->Resort(); }
        }
    else
        { view->GetDolchSightWordsView().RemoveWindowById(BaseProjectView::ID_DOLCH_WORDS_LIST_PAGE_ID); }

    listView =
        dynamic_cast<Wisteria::UI::ListCtrlEx*>(view->GetDolchSightWordsView().FindWindowById(
            BaseProjectView::ID_NON_DOLCH_WORDS_LIST_PAGE_ID));
    if (IsIncludingDolchSightWords() && m_NonDolchWordsData->GetItemCount())
        {
        if (!listView)
            {
            listView = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::ID_NON_DOLCH_WORDS_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetNonDolchWordTabLabel());
            listView->SetName(BaseProjectView::GetNonDolchWordTabLabel());
            listView->EnableGridLines();
            listView->EnableItemViewOnDblClick();
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            view->GetDolchSightWordsView().AddWindow(listView);
            }
        // insert the columns
        listView->DeleteAllColumns();
        listView->InsertColumn(listView->GetColumnCount(), _(L"Document"));
        listView->InsertColumn(listView->GetColumnCount(), _(L"Label"));
        listView->InsertColumn(listView->GetColumnCount(), _(L"% of Non-Dolch Words"));
        if (GetStatisticsReportInfo().IsExtendedInformationEnabled())
            { listView->InsertColumn(listView->GetColumnCount(), _(L"Non-Dolch Words")); }

        UpdateListOptions(listView);
        listView->SetColumnFilePathTruncationMode(0, GetFilePathTruncationMode());
        listView->SetVirtualDataProvider(m_NonDolchWordsData);
        listView->SetVirtualDataSize(m_NonDolchWordsData->GetItemCount());
        listView->SetColumnWidth(0, listView->EstimateColumnWidth(0));
        listView->SetColumnWidth(1, std::min(listView->EstimateColumnWidth(1), view->GetMaxColumnWidth()));
        for (long i = 2; i < listView->GetColumnCount(); ++i)
            { listView->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER); }
        if (listView->GetSortedColumn() == -1)
            { listView->SortColumn(0, Wisteria::SortDirection::SortAscending); }
        else
            { listView->Resort(); }
        }
    else
        { view->GetDolchSightWordsView().RemoveWindowById(BaseProjectView::ID_NON_DOLCH_WORDS_LIST_PAGE_ID); }
    }

//-------------------------------------------------------
void BatchProjectDoc::SetScoreStatsRow(
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> dataGrid, const wxString& rowName,
    const wxString& optionalDescription, const long rowNum, const std::vector<double>& data,
    const int decimalSize, const VarianceMethod varianceMethod, const bool allowCustomFormatting)
    {
    PROFILE();
    size_t currentColumn = 0;
    dataGrid->SetItemText(rowNum, currentColumn++, rowName);
    if (optionalDescription.length())
        { dataGrid->SetItemText(rowNum, currentColumn++, optionalDescription); }
    if (data.size())
        {
        constexpr int HIGHER_PRECISION = 3;
        std::vector<double> sortedData(data.begin(), data.end());
        std::sort(sortedData.begin(), sortedData.end());

        double minVal = *std::min_element(sortedData.begin(), sortedData.end());
        double maxVal = *std::max_element(sortedData.begin(), sortedData.end());
        std::set<double> modes = statistics::mode(sortedData, floor_value<double>() );
        const double rangeVal = maxVal-minVal;
        const double meansVal = statistics::mean(sortedData);
        double medianVal = statistics::median_presorted(sortedData);
        double Skewness = 0;
        double Kurtosis = 0;
        double stddev = 0;
        double Variance = 0;
        if (sortedData.size() >= 2)
            {
            stddev = statistics::standard_deviation(sortedData, varianceMethod == VarianceMethod::SampleVariance);
            Variance = statistics::variance(sortedData, varianceMethod == VarianceMethod::SampleVariance);
            }
        if (sortedData.size() >= 3)
            { Skewness = statistics::skewness(sortedData, varianceMethod == VarianceMethod::SampleVariance); }
        if (sortedData.size() >= 4)
            { Kurtosis = statistics::kurtosis(sortedData, varianceMethod == VarianceMethod::SampleVariance); }
        double lowerQuartile(0), upperQuartile(0);
        statistics::quartiles_presorted(sortedData, lowerQuartile, upperQuartile);

        dataGrid->SetItemValue(rowNum, currentColumn++, sortedData.size());
        dataGrid->SetItemValue(rowNum, currentColumn++, minVal,
            NumberFormatInfo(allowCustomFormatting ?
                NumberFormatInfo::NumberFormatType::CustomFormatting :
                NumberFormatInfo::NumberFormatType::StandardFormatting, decimalSize) );
        dataGrid->SetItemValue(rowNum, currentColumn++, maxVal,
            NumberFormatInfo(allowCustomFormatting ?
                NumberFormatInfo::NumberFormatType::CustomFormatting :
                NumberFormatInfo::NumberFormatType::StandardFormatting, decimalSize) );
        dataGrid->SetItemValue(rowNum, currentColumn++, rangeVal,
            NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, decimalSize) );

        if (modes.size() > 1)
            {
            wxString modeString;
            for (auto modesIter = modes.cbegin();
                 modesIter != modes.cend();
                 ++modesIter)
                {
                modeString += wxNumberFormatter::ToString(*modesIter, 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes) +
                    L"; ";
                }
            // chop off the last "; "
            if (modeString.length() > 2)
                { modeString.RemoveLast(2); }
            dataGrid->SetItemText(rowNum, currentColumn++, modeString,
                NumberFormatInfo(allowCustomFormatting ?
                    NumberFormatInfo::NumberFormatType::CustomFormatting :
                    NumberFormatInfo::NumberFormatType::StandardFormatting, 0),
                *modes.begin());
            }
        else if (modes.size() == 1)
            {
            dataGrid->SetItemValue(rowNum, currentColumn++, *modes.cbegin(),
                NumberFormatInfo(allowCustomFormatting ?
                    NumberFormatInfo::NumberFormatType::CustomFormatting :
                    NumberFormatInfo::NumberFormatType::StandardFormatting, 0));
            }
        else // shouldn't happen
            { dataGrid->SetItemText(rowNum, currentColumn++, wxString{}); }

        dataGrid->SetItemValue(rowNum, currentColumn++, meansVal,
            NumberFormatInfo(allowCustomFormatting ?
                NumberFormatInfo::NumberFormatType::CustomFormatting :
                NumberFormatInfo::NumberFormatType::StandardFormatting, decimalSize));
        dataGrid->SetItemValue(rowNum, currentColumn++, medianVal,
            NumberFormatInfo(allowCustomFormatting ?
                NumberFormatInfo::NumberFormatType::CustomFormatting :
                NumberFormatInfo::NumberFormatType::StandardFormatting, decimalSize));

        if (sortedData.size() < 3)
            {
            dataGrid->SetItemText(rowNum, currentColumn++,
                _(L"More than two values are required to calculate skewness."));
            }
        else
            {
            dataGrid->SetItemValue(rowNum, currentColumn++, Skewness,
                NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting,
                    std::max(decimalSize, HIGHER_PRECISION)));
            }

        if (sortedData.size() < 4)
            {
            dataGrid->SetItemText(rowNum, currentColumn++,
                _(L"More than three values are required to calculate Kurtosis."));
            }
        else
            {
            dataGrid->SetItemValue(rowNum, currentColumn++, Kurtosis,
                NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting,
                    std::max(decimalSize, HIGHER_PRECISION)));
            }

        if (sortedData.size() < 2)
            {
            dataGrid->SetItemText(rowNum, currentColumn++,
                _(L"More than one value is required to calculate std. dev."));
            dataGrid->SetItemText(rowNum, currentColumn++,
                _(L"More than one value is required to calculate variance."));
            }
        else
            {
            dataGrid->SetItemValue(rowNum, currentColumn++, stddev,
                NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting,
                    std::max(decimalSize, HIGHER_PRECISION)));
            dataGrid->SetItemValue(rowNum, currentColumn++, Variance,
                NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting,
                    std::max(decimalSize, HIGHER_PRECISION)));
            }
        dataGrid->SetItemValue(rowNum, currentColumn++, lowerQuartile,
            NumberFormatInfo(allowCustomFormatting ?
                NumberFormatInfo::NumberFormatType::CustomFormatting :
                NumberFormatInfo::NumberFormatType::StandardFormatting, decimalSize));
        dataGrid->SetItemValue(rowNum, currentColumn++, upperQuartile,
            NumberFormatInfo(allowCustomFormatting ?
                NumberFormatInfo::NumberFormatType::CustomFormatting :
                NumberFormatInfo::NumberFormatType::StandardFormatting, decimalSize));
        }
    else
        {
        // show a valid N of zero
        dataGrid->SetItemValue(rowNum, 1, 0);
        for (size_t i = 2; i <= CUMULATIVE_STATS_COUNT; ++i)
            { dataGrid->SetItemText(rowNum, i, _(L"N/A")); }
        }
    }

/// Removes a document from the collection (based on filepath).
//-------------------------------------------------------
void BatchProjectDoc::RemoveDocument(const wxString& docName)
    {
    std::optional<size_t> position{ std::nullopt };
    for (std::vector<BaseProject*>::iterator pos = m_docs.begin(); pos != m_docs.end(); ++pos)
        {
        if (CompareFilePaths((*pos)->GetOriginalDocumentFilePath(), docName) == 0)
            {
            wxDELETE(*pos);
            position = pos - m_docs.begin();
            m_docs.erase(pos);
            break;
            }
        }
    // if not found then don't bother looking for it in the file paths list
    if (!position.has_value())
        {
        return;
        }
    // Also remove the filepath from the list of file paths. These should already be synced up, so
    // we can remove it from the same position. If they are not synced up, then something is wrong,
    // so then we would re-sync everything to fix it.
    // cppcheck-suppress assertWithSideEffect
    assert(position.value() < GetSourceFilesInfo().size());
    // cppcheck-suppress assertWithSideEffect
    assert(CompareFilePaths(GetOriginalDocumentFilePath(position.value()), docName) == 0);
    if (position.value() < GetSourceFilesInfo().size() &&
        CompareFilePaths(GetOriginalDocumentFilePath(position.value()), docName) == 0)
        {
        GetSourceFilesInfo().erase(GetSourceFilesInfo().begin() + position.value());
        }
    // should never happen, this is a fail safe
    else
        {
        SyncFilePathsWithDocuments();
        }
    }

//-------------------------------------------------------
void BatchProjectDoc::SyncFilePathsWithDocuments()
    {
    GetSourceFilesInfo().clear();
    GetSourceFilesInfo().reserve(m_docs.size());
    for (std::vector<BaseProject*>::const_iterator pos = m_docs.cbegin(); pos != m_docs.cend();
         ++pos)
        {
        assert((*pos)->GetSourceFilesInfo().size());
        GetSourceFilesInfo().push_back((*pos)->GetSourceFilesInfo().at(0));
        }
    }
