///////////////////////////////////////////////////////////////////////////////
// Name:        projectdoc.cpp
// Author:      Blake Madden
// Copyright:   (c) 2005-2023 Blake Madden
// Licence:     3-Clause BSD licence
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "standard_project_doc.h"
#include "standard_project_view.h"
#include "../results_format/project_report_format.h"
#include "../ui/dialogs/project_wizard_dlg.h"
#include "../results_format/word_collectiont_text_formatting.h"
#include "../readability/readability.h"
#include "../Wisteria-Dataviz/src/import/rtf_encode.h"
#include "../Wisteria-Dataviz/src/import/html_encode.h"
#include "../Wisteria-Dataviz/src/graphs/heatmap.h"
#include "../Wisteria-Dataviz/src/graphs/wordcloud.h"
#include "../Wisteria-Dataviz/src/ui/dialogs/listdlg.h"

using namespace Wisteria;
using namespace Wisteria::Graphs;
using namespace Wisteria::GraphItems;
using namespace Wisteria::Colors;
using namespace Wisteria::GraphItems;
using namespace Wisteria::UI;

wxDECLARE_APP(ReadabilityApp);

wxIMPLEMENT_DYNAMIC_CLASS(ProjectDoc, wxDocument)

//-------------------------------------------------------
void ProjectDoc::ShowQueuedMessages()
    {
    BaseProjectView* view = dynamic_cast<BaseProjectView*>(GetFirstView());
    for (std::vector<WarningMessage>::const_iterator queuedMsgIter = GetQueuedMessages().begin();
        queuedMsgIter != GetQueuedMessages().end();
        ++queuedMsgIter)
        { view->ShowInfoMessage(*queuedMsgIter); }
    }

//-------------------------------------------------------
void ProjectDoc::RemoveMisspellings([[maybe_unused]] const wxArrayString& misspellingsToRemove)
    {
    RefreshRequired(ProjectRefresh::FullReindexing);
    RefreshProject();
    }

//-------------------------------------------------------
void ProjectDoc::ExcludeAllCustomTestsTests()
    {
    ProjectView* view = dynamic_cast<ProjectView*>(GetFirstView());
    for (std::vector<CustomReadabilityTestInterface>::const_iterator pos = GetCustTestsInUse().begin();
        pos != GetCustTestsInUse().end();
        ++pos)
        {
        while (view->GetWordsBreakdownView().RemoveWindowById(pos->GetIterator()->get_interface_id()) )
            {}
        }
    BaseProject::ExcludeAllCustomTestsTests();
    }

//-------------------------------------------------------
std::vector<CustomReadabilityTestInterface>::iterator ProjectDoc::RemoveCustomReadabilityTest(
    const wxString& testName, const int Id)
    {
    ProjectView* view = dynamic_cast<ProjectView*>(GetFirstView());

    // remove any views that are related to this test
    // (text window and word list window)
    while (view->GetWordsBreakdownView().RemoveWindowById(Id))
        {}
    // remove the test
    const auto iter = BaseProject::RemoveCustomReadabilityTest(testName, Id);
    // remove the test score
    const auto location = view->GetReadabilityScoresList()->GetResultsListCtrl()->FindEx(testName);
    if (location != wxNOT_FOUND)
        { view->GetReadabilityScoresList()->GetResultsListCtrl()->DeleteItem(location); }

    DisplayStatistics();
    view->UpdateSideBarIcons();

    return iter;
    }

//------------------------------------------------
void ProjectDoc::LoadManuallyEnteredText()
    {
    SetOriginalDocumentFilePath(wxString{});
    try
        {
        LoadDocument();
        }
    catch (...)
        {
        LogMessage(_(L"An unknown error occurred while analyzing the text. Unable to create project."),
            _(L"Error"), wxOK|wxICON_EXCLAMATION);
        }
    }

//------------------------------------------------
void ProjectDoc::RefreshStatisticsReports()
    {
    if (!IsSafeToUpdate())
        { return; }

    // if refresh is not necessary then return
    if (IsRefreshRequired() == false)
        { return; }

    ProjectView* view = dynamic_cast<ProjectView*>(GetFirstView());
    const auto selectedItem = view->GetSideBar()->GetSelectedFolderId();

    wxWindowUpdateLocker noUpdates(GetDocumentWindow());
    BaseProjectProcessingLock processingLock(this);
    DisplayStatistics();

    view->UpdateSideBarIcons();
    view->Present();
    UpdateAllViews();

    auto selectedIndex = view->GetSideBar()->FindFolder(selectedItem);
    if (!selectedIndex.has_value())
        { selectedIndex = view->GetSideBar()->FindFolder(BaseProjectView::SIDEBAR_READABILITY_SCORES_SECTION_ID); }
    view->GetSideBar()->SelectFolder(selectedIndex, false);

    GetDocumentWindow()->Refresh();
    ResetRefreshRequired();
    Modify(true);
    }

//------------------------------------------------
void ProjectDoc::RefreshGraphs()
    {
    if (!IsSafeToUpdate())
        { return; }

    // if refresh is not necessary then return
    if (IsRefreshRequired() == false)
        { return; }

    wxWindowUpdateLocker noUpdates(GetDocumentWindow());
    BaseProjectProcessingLock processingLock(this);
    DisplayReadabilityScores(false);
    DisplayReadabilityGraphs();
    DisplayWordCharts();
    DisplaySentenceCharts();

    GetDocumentWindow()->Refresh();
    ResetRefreshRequired();
    Modify(true);
    }

//------------------------------------------------
void ProjectDoc::RefreshProject()
    {
    if (!IsSafeToUpdate())
        { return; }

    // if refresh is not necessary then return
    if (IsRefreshRequired() == false)
        { return; }

    wxBusyCursor bc;

    // reload the excluded phrases
    LoadExcludePhrases();

    // load appended template file (if there is one)
    LoadAppendedDocument();

    ProjectView* view = dynamic_cast<ProjectView*>(GetFirstView());
    const auto selectedItem = view->GetSideBar()->GetSelectedSubItemId();

    // If the original text is gone (there won't be anything to analyze),
    // or if just cosmetic changes (e.g., graph options), then don't re-index, just do a simple refresh.
    if (LoadingOriginalTextSucceeded() == false ||
        !IsDocumentReindexingRequired())
        {
        wxWindowUpdateLocker noUpdates(GetDocumentWindow());
        BaseProjectProcessingLock processingLock(this);
        DisplayReadabilityScores(false);
        DisplayStatistics();
        DisplayReadabilityGraphs();
        DisplayWordsBreakdown();
        DisplaySentencesBreakdown();
        if (IsTextSectionRefreshRequired())
            { DisplayHighlightedText(GetTextHighlightColor(), GetTextViewFont()); }
        else
            { UpdateHighlightedTextWindows(); }

        view->UpdateSideBarIcons();
        view->Present();
        UpdateAllViews();

        if (!view->GetSideBar()->SelectSubItemById(selectedItem, true, false))
            { view->GetSideBar()->SelectFolder(0, true, false); }

        GetDocumentWindow()->Refresh();
        ResetRefreshRequired();
        Modify(true);
        return;
        }
    BaseProjectProcessingLock processingLock(this);
    wxWindowUpdateLocker noUpdates(GetDocumentWindow());

    // reload the document
    if (GetDocumentStorageMethod() == TextStorage::NoEmbedText)
        {
        if (!LoadExternalDocument() )
            {
            // if the loading failed then reset everything and bail
            ResetRefreshRequired();
            return;
            }
        }
    // if embedded, then reload our embedded content
    else
        {
        try
            {
            LoadDocument();
            }
        catch (...)
            {
            LogMessage(_(L"An unknown error occurred while analyzing the document. Unable to create project."),
                _(L"Error"), wxOK|wxICON_EXCLAMATION);
            return;
            }
        }

    /* if they set this to exclude headers and such make sure we actually have some
       valid sentences to work with*/
    bool invalidSentenceMethodSwitched = false;
    if ((GetInvalidSentenceMethod() == InvalidSentence::ExcludeFromAnalysis ||
         GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings) &&
        GetWords()->get_complete_sentence_count() == 0)
        {
        LogMessage(
            _(L"You have requested to ignore incomplete sentences, but there are no other valid "
               "sentences in the text. Incomplete sentences will need to be included in the analysis."),
            _(L"Warning"), wxOK|wxICON_EXCLAMATION);
        SetInvalidSentenceMethod(InvalidSentence::IncludeAsFullSentences);
        invalidSentenceMethodSwitched = true;
        }

    if (GetInvalidSentenceMethod() == InvalidSentence::ExcludeFromAnalysis ||
        GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings)
        { CalculateStatisticsIgnoringInvalidSentences(); }
    else if (GetInvalidSentenceMethod() == InvalidSentence::IncludeAsFullSentences)
        { CalculateStatistics(); }
    CalculateGraphData();

    if (GetTotalWords() == 0)
        {
        LogMessage(
            (GetTextSource() == TextSource::FromFile) ?
                _(L"No words were found in the original document. Project cannot be recalculated.") :
                _(L"No valid words were entered. Project cannot be recalculated."),
            _(L"Import Error"), wxOK|wxICON_INFORMATION);
        GetDocumentWindow()->Refresh();

        if (invalidSentenceMethodSwitched)
            { SetInvalidSentenceMethod(InvalidSentence::ExcludeFromAnalysis); }
        ResetRefreshRequired();
        return;
        }

    /* NOTE: do not use a progress bar with APP_MODAL because that calls
       disable on the text view windows. On macOS, disabling/re-enabling
       text controls appears to reset their font color (which we are customizing
       in DisplayHighlightedText().*/
    wxBusyInfo bi(wxBusyInfoFlags().Text(_(L"Reloading project...")));

    LoadHardWords();

    DisplayStatistics();
    DisplayReadabilityScores(false);
    DisplayReadabilityGraphs();
    DisplayWordsBreakdown();
    DisplayHighlightedText(GetTextHighlightColor(), GetTextViewFont());
    DisplaySentencesBreakdown();
    DisplayGrammar();
    DisplaySightWords();

    DeleteUniqueWordMap();

    Modify(true);

    view->UpdateSideBarIcons();
    view->Present();
    UpdateAllViews();

    // See if the view that was originally selected is gone.
    // If so then select the scores section.
    if (!view->GetSideBar()->SelectSubItemById(selectedItem, true, false))
        { view->GetSideBar()->SelectFolder(0, true, false); }

    GetDocumentWindow()->Refresh();

    ResetRefreshRequired();

    ShowQueuedMessages();
    }

//------------------------------------------------
bool ProjectDoc::LoadProjectFile(const char* projectFileText, const size_t textLength)
    {
    Wisteria::ZipCatalog cat(projectFileText, textLength);

    // open the project settings file
    wxString settingsFile = cat.ReadTextFile(ProjectSettingsFileLabel());
    if (!settingsFile.empty())
        { LoadSettingsFile(settingsFile); }
    else
        {
        LogMessage(_(L"Settings file could not be found in the project file. Default settings will be used."),
                wxGetApp().GetAppName(), wxOK|wxICON_INFORMATION);
        }

    // open the project meta file
    wxString metaFile = cat.ReadTextFile(ProjectMetaFileLabel());
    if (!metaFile.empty())
        { LoadMetaFile(metaFile); }
    else
        {
        LogMessage(_(L"Meta information file could not be found in the project file."),
                wxGetApp().GetAppName(), wxOK|wxICON_INFORMATION);
        }

    // load the embedded text
    const wxString contentFile = cat.ReadTextFile(ProjectContentFileLabel());
    // if the document's content is just embedded and shouldn't be reloaded
    if (GetDocumentStorageMethod() == TextStorage::EmbedText)
        {
        if (!contentFile.empty())
            {
            SetDocumentText(contentFile);
            try
                { LoadDocument(); }
            catch (...)
                {
                LogMessage(_(L"An unknown error occurred while analyzing the document. Unable to create project."),
                    _(L"Error"), wxOK|wxICON_EXCLAMATION);
                return false;
                }
            }
        else
            {
            /* If text was manually entered before but not embedded for some reason, then it's lost.
               This should not happen unless you manually tamper with the project file. */
            if (GetTextSource() == TextSource::EnteredText)
                {
                if (WarningManager::HasWarning(_DT(L"no-embedded-text")))
                    {
                    WarningMessage warningMsg = *WarningManager::GetWarning(_DT(L"no-embedded-text"));
                    warningMsg.SetMessage(
                        _(L"Manually entered text could not be found in the project file.\n"
                           "Only previously calculated scores and statistics will be displayed."));
                    LogMessage(warningMsg);
                    }
                return false;
                }
            // ...otherwise, external file was supposed to be embedded but internal copy of the text couldn't be found.
            // Try to reload it.
            else
                {
                if (LoadExternalDocument())
                    {
                    LogMessage(
                        _(L"The document's content could not be found in the project file. "
                           "Original document will be reloaded."),
                        _(L"Warning"), wxOK|wxICON_INFORMATION);
                    return true;
                    }
                // Should not happen. File was supposed to be embedded, but wasn't in the project file
                // and external file can't be found either.
                else
                    {
                    LogMessage(
                        _(L"Document content could not be found in the project file and external document "
                           "could not be located.\nOnly previously calculated scores and "
                           "statistics will be displayed."),
                        _(L"Warning"), wxOK|wxICON_EXCLAMATION);
                    return false;
                    }
                }
            }
        }
    // project is set to always reload the file
    else
        {
        if (GetTextSource() == TextSource::FromFile)
            {
            if (LoadExternalDocument() )
                { return true; }
            else
                {
                LogMessage(
                    _(L"External document could not be located.\n"
                       "Only previously calculated scores and statistics will be displayed."),
                    _(L"Warning"), wxOK|wxICON_EXCLAMATION);
                return false;
                }
            }
        /* This should not happen because the entered text flag overrides the storage flag to force it to embed,
           but just for the sake of being verbose...*/
        if (GetTextSource() == TextSource::EnteredText)
            {
            LogMessage(
                _(L"Manually entered text was not embedded previously.\n"
                   "Only previously calculated scores and statistics will be displayed."),
                _(L"Warning"), wxOK|wxICON_EXCLAMATION);
            return false;
            }
        }

    return true;
    }

//------------------------------------------------
void ProjectDoc::LoadMetaFile(const wchar_t* settingsFileText)
    {
    wxString currentStartTag;
    wxString currentEndTag;

    // read in the statistics
    currentStartTag.clear();
    currentStartTag.append(L"<").append(wxGetApp().GetAppOptions().XML_STATISTICS);
    currentEndTag.clear();
    const wchar_t* statsSection = std::wcsstr(settingsFileText, currentStartTag);
    if (!statsSection)
        {
        LogMessage(wxString::Format(_(L"Warning: \"%s\" section not found in project file."), currentStartTag),
                _(L"Error"), wxOK|wxICON_ERROR);
        return;
        }
    const wchar_t* statsSectionEnd = std::wcsstr(statsSection, L">");
    if (statsSectionEnd &&
        (statsSection < statsSectionEnd) )
        {
        // size of the text string
        SetTextSize(XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_TEXT_SIZE));
        // unique words
        SetTotalUniqueWords(XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_UNIQUE_WORDS));
        // unique monosyllabic words
        SetTotalUniqueMonoSyllablicWords(XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_UNIQUE_MONOSYLLABLIC_WORDS));
        // unique 6 Chars+ Words
        SetTotalUnique6CharsPlusWords(XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_UNIQUE_SIXCHAR_PLUS_WORDS));
        // unique 3+ Syllable Words
        SetTotalUnique3PlusSyllableWords(XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_UNIQUE_THREE_SYLLABLE_PLUS_WORDS));
        // unique DC Hard Words
        SetTotalUniqueDCHardWords(XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_UNIQUE_DC_HARD_WORDS));
        // unique Spache Hard Words
        SetTotalUniqueHardWordsSpache(XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_UNIQUE_SPACHE_HARD_WORDS));
        // unique Harris Jacobson Hard Words
        SetTotalUniqueHarrisJacobsonHardWords(XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_UNIQUE_HARRIS_JACOBSON_HARD_WORDS));
        // unique Mini Words
        SetTotalUniqueMiniWords(XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_UNIQUE_MINI_WORDS));
        // unique Hard Fog Words
        SetTotalUniqueHardFogWords(XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_UNIQUE_FOG_HARD_WORDS));
        // unique Hard Words (numerals fully syllabized)
        SetUnique3PlusSyllabicWordsNumeralsFullySyllabized(
            XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
                wxGetApp().GetAppOptions().XML_UNIQUE_SMOG_WORDS_WORDS));
        // total Words
        m_totalWords = XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_TOTAL_WORDS);
        // total Sentences
        m_totalSentences = XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_TOTAL_SENTENCES);
        // total sentence units
        m_totalSentenceUnits = XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_TOTAL_SENTENCE_UNITS);
        // total Paragraphs
        m_totalParagraphs = XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_TOTAL_PARAGRAPH);
        // total Syllables
        m_totalSyllables = XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_TOTAL_SYLLABLE);
        // total Characters
        m_totalCharacters = XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_TOTAL_CHARACTERS);
        // total characters and punctuation (this includes punctuation that is part of the words and between them)
        m_totalCharactersPlusPunctuation = XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_TOTAL_CHARACTERS_PLUS_PUNCTUATION);
        // total Mono Syllabic
        m_totalMonoSyllabic = XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_TOTAL_MONOSYLLABIC_WORDS);
        // total 3plus Syllable Words
        m_total3plusSyllableWords = XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_TOTAL_THREE_PLUSSYLLABLE_WORDS);
        // total Hard Words Spache
        m_totalHardWordsSpache = XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_TOTAL_SPACHE_HARD_WORDS);
        // total Hard Words Harris-Jacobson
        m_totalHardWordsHarrisJacobson = XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_TOTAL_HARRIS_JACOBSON_HARD_WORDS);
        // total Hard Words Dale Chall
        m_totalHardWordsDaleChall = XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_TOTAL_DALECHALL_HARD_WORDS);
        // total Hard Words Fog
        m_totalHardWordsFog = XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_TOTAL_FOG_HARDWORDS);
        // total Hard Words (numerals fully syllabized)
        m_total3PlusSyllabicWordsNumeralsFullySyllabized =
            XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
                wxGetApp().GetAppOptions().XML_TOTAL_SMOG_HARD_WORDS);
        // total Hard Words Sol
        m_totalHardWordsSol = XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_TOTAL_SOL_HARD_WORDS);
        // total Hard Lix/Rix words
        m_totalHardWordsLixRix = XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_TOTAL_LIX_RIX_HARD_WORDS);
        // total Syllables Ignoring Numerals
        m_totalSyllablesIgnoringNumerals = XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_TOTAL_SYLLABLES_IGNORING_NUMERALS);
        // Number of syllables, numerals just one syllable
        m_totalSyllablesNumeralsOneSyllable = XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_TOTAL_SYLLABLES_NUMERALS_ONE_SYLLABLE);
        // Number of syllables, numerals fully syllabized
        m_totalSyllablesNumeralsFullySyllabized = XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_TOTAL_SYLLABLES_NUMERALS_FULLY_SYLLABIZED);
        // total Syllables Ignoring Numerals
        m_totalSyllablesIgnoringNumeralsAndProperNouns =
            XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
                wxGetApp().GetAppOptions().XML_TOTAL_SYLLABLES_IGNORING_NUMERALS_AND_PROPER_NOUNS);
        // total Long Words
        m_totalLongWords = XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_TOTAL_LONG_WORDS);
        m_totalSixPlusCharacterWordsIgnoringNumerals = XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_TOTAL_LONG_WORDS_IGNORING_NUMERALS);
        // total MiniWords
        m_totalMiniWords = XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_TOTAL_MINIWORDS);
        // total Numerals
        m_totalNumerals = XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_TOTAL_NUMERALS);
        // total proper nouns
        m_totalProperNouns = XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_TOTAL_PROPER_NOUNS);
        // total Overly Long Sentences
        m_totalOverlyLongSentences = XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_TOTAL_OVERLY_LONG_SENTENCES);
        // supplementary stats
        SetTotalWordsFromCompleteSentencesAndHeaders(
            XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
                wxGetApp().GetAppOptions().XML_TOTAL_WORDS_FROM_COMPLETE_SENTENCES_AND_HEADERS));
        SetTotalSentencesFromCompleteSentencesAndHeaders(
            XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
                wxGetApp().GetAppOptions().XML_TOTAL_SENTENCES_FROM_COMPLETE_SENTENCES_AND_HEADERS));
        SetTotalNumeralsFromCompleteSentencesAndHeaders(
            XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
                wxGetApp().GetAppOptions().XML_TOTAL_NUMERALS_FROM_COMPLETE_SENTENCES_AND_HEADERS));
        SetTotalCharactersFromCompleteSentencesAndHeaders(
            XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
                wxGetApp().GetAppOptions().XML_TOTAL_CHARACTERS_FROM_COMPLETE_SENTENCES_AND_HEADERS));
        // special types of sentences
        m_totalInterrogativeSentences = XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_TOTAL_INTERROGATIVE_SENTENCES);
        m_totalExclamatorySentences = XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_TOTAL_EXCLAMATORY_SENTENCES);
        // longest sentence
        m_longestSentence = XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_LONGEST_SENTENCES);
        // longest sentence index
        m_longestSentenceIndex = XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_LONGEST_SENTENCE_INDEX);
        // difficult Sentence Length
        SetDifficultSentenceLength(static_cast<int>(XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_DIFFICULT_SENTENCE_LENGTH)));
        // grammar stats
        SetMisspelledWordCount(XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_MISSPELLING_COUNT));
        SetDuplicateWordCount(XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_DUPLICATE_WORD_COUNT));
        SetWordyPhraseCount(XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_WORDY_PHRASE_COUNT));
        SetClicheCount(XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_CLICHE_COUNT));
        SetRedundantPhraseCount(XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_REDUNDANT_PHRASE_COUNT));
        SetWordingErrorCount(XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_WORDING_ERROR_COUNT));
        SetSentenceStartingWithConjunctionsCount(XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_SENTENCE_CONJUNCTION_START_COUNT));
        SetSentenceStartingWithLowercaseCount(XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_SENTENCE_LOWERCASE_START_COUNT));
        SetMismatchedArticleCount(
            XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
                wxGetApp().GetAppOptions().XML_ARTICLE_MISMATCH_COUNT));
        SetPassiveVoicesCount(XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_PASSIVE_VOICE_COUNT));
        SetOverusedWordsBySentenceCount(XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_OVERUSED_WORDS_BY_SENTENCE_COUNT));
        // Dolch stats
        m_dolchConjunctionCounts.first = XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_DOLCH_UNIQUE_CONJUNCTIONS_COUNT);
        m_dolchConjunctionCounts.second = XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_DOLCH_TOTAL_CONJUNCTIONS_COUNT);
        m_dolchPrepositionCounts.first = XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_DOLCH_UNIQUE_PREPOSITIONS_COUNT);
        m_dolchPrepositionCounts.second = XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_DOLCH_TOTAL_PREPOSITIONS_COUNT);
        m_dolchPronounCounts.first = XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_DOLCH_UNIQUE_PRONOUNS_COUNT);
        m_dolchPronounCounts.second = XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_DOLCH_TOTAL_PRONOUNS_COUNT);
        m_dolchAdverbCounts.first = XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_DOLCH_UNIQUE_ADVERBS_COUNT);
        m_dolchAdverbCounts.second = XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_DOLCH_TOTAL_ADVERBS_COUNT);
        m_dolchAdjectiveCounts.first = XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_DOLCH_UNIQUE_ADJECTIVES_COUNT);
        m_dolchAdjectiveCounts.second = XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_DOLCH_TOTAL_ADJECTIVES_COUNT);
        m_dolchVerbCounts.first = XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_DOLCH_UNIQUE_VERBS_COUNT);
        m_dolchVerbCounts.second = XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_DOLCH_TOTAL_VERBS_COUNT);
        m_dolchNounCounts.first = XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_DOLCH_UNIQUE_NOUNS_COUNT);
        m_dolchNounCounts.second = XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_DOLCH_TOTAL_NOUNS_COUNT);
        SetUnusedDolchConjunctions(XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_DOLCH_UNUSED_CONJUNCTIONS_COUNT));
        SetUnusedDolchPrepositions(XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_DOLCH_UNUSED_PREPOSITIONS_COUNT));
        SetUnusedDolchPronouns(XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_DOLCH_UNUSED_PRONOUNS_COUNT));
        SetUnusedDolchAdverbs(XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_DOLCH_UNUSED_ADVERBS_COUNT));
        SetUnusedDolchAdjectives(XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_DOLCH_UNUSED_ADJECTIVES_COUNT));
        SetUnusedDolchVerbs(XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_DOLCH_UNUSED_VERBS_COUNT));
        SetUnusedDolchNouns(XmlFormat::GetAttributeLongValue(statsSection, statsSectionEnd,
            wxGetApp().GetAppOptions().XML_DOLCH_UNUSED_NOUNS_COUNT));
        }
    else
        {
        LogMessage(wxString::Format(_(L"Warning: \"%s\" section not found in project file."), currentStartTag),
                _(L"Error"), wxOK|wxICON_ERROR);
        }

    // read in any custom tests (these may or may not be in a project's meta data)
    currentStartTag.clear();
    currentStartTag.append(L"<").append(wxGetApp().GetAppOptions().XML_CUSTOM_TESTS);
    const wchar_t* customTestSection = std::wcsstr(settingsFileText, currentStartTag.wc_str() );
    if (!customTestSection)
        { return; }
    currentEndTag.clear();
    currentEndTag.append(L"</").append(wxGetApp().GetAppOptions().XML_CUSTOM_TESTS).append(L">");
    const wchar_t* customTestSectionEnd = std::wcsstr(customTestSection, currentEndTag);
    const wchar_t* customTest = customTestSection;
    currentStartTag.clear();
    currentStartTag.append(L"<").append(wxGetApp().GetAppOptions().XML_CUSTOM_FAMILIAR_WORD_TEST);
    if (customTestSectionEnd &&
        (customTestSection < customTestSectionEnd) )
        {
        while (customTest)
            {
            customTest = std::wcsstr(customTest, currentStartTag.wc_str() );
            if (!customTest || customTest > customTestSectionEnd)
                { break; }
            const wchar_t* customTestEnd = std::wcsstr(customTest, L"/>");
            if (!customTestEnd || customTestEnd > customTestSectionEnd)
                { break; }
            // read in the values
            wxString textName = XmlFormat::GetAttributeString(customTest, customTestEnd,
                wxGetApp().GetAppOptions().XML_TEST_NAME);
            const long unfamiliarCount = XmlFormat::GetAttributeLongValue(customTest, customTestEnd,
                wxGetApp().GetAppOptions().XML_UNFAMILIAR_WORD_COUNT);
            const long uniqueUnfamiliarCount = XmlFormat::GetAttributeLongValue(customTest, customTestEnd,
                wxGetApp().GetAppOptions().XML_UNIQUE_UNFAMILIAR_WORD_COUNT);
            if (HasCustomTest(textName))
                {
                GetCustomTest(textName)->SetUnfamiliarWordCount(unfamiliarCount);
                GetCustomTest(textName)->SetUniqueUnfamiliarWordCount(uniqueUnfamiliarCount);
                }
            customTest = customTestEnd;
            }
        }
    }

//------------------------------------------------
bool ProjectDoc::OnOpenDocument(const wxString& filename)
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

    // make sure the file exists first
    if (!wxFile::Exists(filename) )
        {
        LogMessage(
            wxString::Format(_(L"'%s': unable to open project file. File not found."), filename),
            _(L"Error"), wxOK|wxICON_EXCLAMATION);
        return false;
        }
    if (!OnSaveModified())
        { return false; }

    SetTitle(ParseTitleFromFileName(filename));
    SetFilename(filename, true);

    BaseProjectProcessingLock processingLock(this);

    MemoryMappedFile sourceFile;
    try
        {
        sourceFile.MapFile(GetFilename());
        const char* projectFileText = static_cast<char*>(sourceFile.GetStream());
        SetLoadingOriginalTextSucceeded(LoadProjectFile(projectFileText, sourceFile.GetMapSize()));
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
            if (WarningManager::HasWarning(_DT(L"project-open-as-read-only")))
                { LogMessage(*WarningManager::GetWarning(_DT(L"project-open-as-read-only"))); }
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
        SetLoadingOriginalTextSucceeded(LoadProjectFile(projectFileText.get(), readSize));
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
        /* Couldn't get a map of the file for unknown reason, so buffer it.*/
        if (!LockProjectFile())
            { return false; }
        auto projectFileText = std::make_unique<char[]>(m_File.Length()+1);

        m_File.Seek(0);
        const size_t readSize = m_File.Read(projectFileText.get(), m_File.Length());
        SetLoadingOriginalTextSucceeded(LoadProjectFile(projectFileText.get(), readSize));
        }
    catch (...)
        {
        LogMessage(
            wxString::Format(_(L"'%s': unable to open project file."), GetFilename()),
            _(L"Project Open"), wxOK|wxICON_EXCLAMATION);
        return false;
        }

    wxBusyCursor bc;

    /* If the indexed text was found in the project or external file
       then reanalyze everything. This should be the normal scenario and
       would only fail if the text was not embedded and the 
       external file could not be found (if applicable).*/
    if (LoadingOriginalTextSucceeded())
        {
        /* if they set this to exclude headers and such, make sure we actually have some
           valid sentences to work with*/
        if ((GetInvalidSentenceMethod() == InvalidSentence::ExcludeFromAnalysis ||
            GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings) &&
            GetWords()->get_complete_sentence_count() == 0)
            {
            LogMessage(_(L"You have requested to ignore incomplete sentences, but there are no other "
                          "valid sentences in the text. Incomplete sentences will need to be included "
                          "in the analysis."),
                       _(L"Warning"), wxOK|wxICON_EXCLAMATION);
            SetInvalidSentenceMethod(InvalidSentence::IncludeAsFullSentences);
            }

        if (GetInvalidSentenceMethod() == InvalidSentence::ExcludeFromAnalysis  ||
            GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings)
            { CalculateStatisticsIgnoringInvalidSentences(); }
        else if (GetInvalidSentenceMethod() == InvalidSentence::IncludeAsFullSentences)
            { CalculateStatistics(); }
        CalculateGraphData();

        if (GetTotalWords() == 0)
            {
            LogMessage(_(L"No words were found in this file."),
                _(L"Error"), wxOK|wxICON_ERROR);
            return false;
            }

        wxBusyInfo bi(wxBusyInfoFlags().Text(_(L"Loading project...")));

        LoadHardWords();

        DisplayStatistics();
        DisplayReadabilityScores(false);
        DisplayReadabilityGraphs();
        DisplayWordsBreakdown();
        DisplayHighlightedText(GetTextHighlightColor(), GetTextViewFont());
        DisplaySentencesBreakdown();
        DisplayGrammar();
        DisplaySightWords();

        DeleteUniqueWordMap();
        }
    else
        {
        /* Don't bother calling CalculateStatistics(), just use the values
           loaded from the settings file in the project*/
        if (GetTotalWords() == 0)
            {
            LogMessage(_(L"No words were found in this file."),
                _(L"Error"), wxOK|wxICON_ERROR);
            return false;
            }

        wxBusyInfo bi(wxBusyInfoFlags().Text(_(L"Loading project...")));

        /* Don't bother calling LoadHardWords() if the original text was lost*/
        /* Don't bother calling DisplayHighlightedText() if the original text was lost*/
        /* Don't bother deleting m_word_frequency_map if the original text was lost*/
        DisplayReadabilityScores(false);
        DisplayStatistics();
        DisplayReadabilityGraphs();
        /* Don't bother calling DisplayWordsBreakdown() if the original text was lost*/
        }

    /* Don't call "Modify(false)" because if the external document could not be found when
       loading the project and user searched for the document then the modified flag is set*/
    SetDocumentSaved(true);

    ProjectView* view = dynamic_cast<ProjectView*>(GetFirstView());
    view->UpdateSideBarIcons();
    view->Present();
    UpdateAllViews();

    const auto selectedIndex = view->GetSideBar()->FindFolder(BaseProjectView::SIDEBAR_READABILITY_SCORES_SECTION_ID);
    view->GetSideBar()->SelectFolder(selectedIndex.value_or(0), true);

    ShowQueuedMessages();

    if (WarningManager::HasWarning(_DT(L"note-project-properties")))
        { view->ShowInfoMessage(*WarningManager::GetWarning(_DT(L"note-project-properties"))); }

    return true;
    }

//------------------------------------------------
bool ProjectDoc::RunProjectWizard(const wxString& path)
    {
    // make sure document content is cleared
    FreeDocumentText();

    // Run through the project wizard
    ProjectWizardDlg* wizard = new ProjectWizardDlg(wxGetApp().GetMainFrame(), ProjectType::StandardProject, path);
    if (wizard->ShowModal() != wxID_OK)
        {
        wizard->Destroy();
        return false;
        }

    SetProjectLanguage(wizard->GetLanguage());
    wxGetApp().GetAppOptions().SetProjectLanguage(wizard->GetLanguage());

    if (wizard->IsTextFromFileSelected() )
        {
        SetTitle(ParseTitleFromFileName(wizard->GetFilePath()));

        SetOriginalDocumentFilePath(wizard->GetFilePath());
        SetTextSource(TextSource::FromFile);
        }
    else if (wizard->IsManualTextEntrySelected() )
        {
        SetTitle(_(L"Untitled"));
        SetDocumentText(wizard->GetEnteredText());
        SetTextSource(TextSource::EnteredText);
        }

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
                rTest->include(
                    rTest->get_test().has_document_classification(
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
        else if (wizard->GetSelectedDocumentType() == readability::document_classification::technical_document)
            {
            for (auto rTest = GetReadabilityTests().get_tests().begin();
                rTest != GetReadabilityTests().get_tests().end();
                ++rTest)
                {
                rTest->include(
                    rTest->get_test().has_document_classification(
                        readability::document_classification::technical_document) &&
                    rTest->get_test().has_language(GetProjectLanguage()));
                }
            for (CustomReadabilityTestCollection::const_iterator pos = m_custom_word_tests.cbegin();
                pos != m_custom_word_tests.cend();
                ++pos)
                {
                if (pos->has_document_classification(readability::document_classification::technical_document))
                    { AddCustomReadabilityTest(wxString(pos->get_name().c_str())); }
                }
            }
        // short, terse forms
        else if (wizard->GetSelectedDocumentType() == readability::document_classification::nonnarrative_document)
            {
            // override how headers and lists are counted so that they are always included if this is a form
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
        if (wizard->GetSelectedIndustryType() == readability::industry_classification::childrens_publishing_industry)
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
                if (pos->has_industry_classification(readability::industry_classification::adult_healthcare_industry))
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
        else if (wizard->GetSelectedIndustryType() == readability::industry_classification::broadcasting_industry)
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
            wxGetApp().GetAppOptions().GetIncludedCustomTests().emplace_back(selectedTest.get_name().c_str());
            }
        }
    // user selected a test bundle
    else if (wizard->IsTestBundleSelected())
        {
        ApplyTestBundle(wizard->GetSelectedTestBundle());
        wxGetApp().GetAppOptions().SetSelectedTestBundle(wizard->GetSelectedTestBundle());
        }
    // set parsing options based on how the user defined the structure of the document
    SetIgnoreBlankLinesForParagraphsParser(wizard->IsSplitLinesSelected());
    SetIgnoreIndentingForParagraphsParser(wizard->IsCenteredTextSelected());
    if ((wizard->IsSplitLinesSelected() ||
         wizard->IsCenteredTextSelected()) &&
         !wizard->IsNewLinesAlwaysNewParagraphsSelected())
        {
        // also override paragraph ending logic if special format parsing is requested
        SetParagraphsParsingMethod(ParagraphParse::OnlySentenceTerminatedNewLinesAreParagraphs);
        }
    else if (wizard->IsNewLinesAlwaysNewParagraphsSelected())
        { SetParagraphsParsingMethod(ParagraphParse::EachNewLineIsAParagraph); }
    // pre-2007 Microsoft Word files (*.doc) are difficult to detect lists in, so if we are
    // not explicitly specifying "fitted to the page" analysis for this project (above), then
    // override the global option and set it to treat all newlines as the end of a paragraph.
    else if (wizard->IsTextFromFileSelected() &&
        wxFileName(wizard->GetFilePath()).GetExt().CmpNoCase(_DT(L"doc")) == 0)
        {
        SetParagraphsParsingMethod(ParagraphParse::EachNewLineIsAParagraph);
        }

    if (wizard->IsNarrativeSelected())
        {
        SetInvalidSentenceMethod(InvalidSentence::ExcludeFromAnalysis);
        }
    else if (wizard->IsFragmentedTextSelected())
        {
        // override how headers and lists are counted so that they are always included if this is a form
        SetInvalidSentenceMethod(InvalidSentence::IncludeAsFullSentences);
        for (CustomReadabilityTestCollection::const_iterator pos = m_custom_word_tests.begin();
            pos != m_custom_word_tests.end();
            ++pos)
            {
            if (pos->has_document_classification(readability::document_classification::nonnarrative_document))
                { AddCustomReadabilityTest(wxString(pos->get_name().c_str())); }
            }
        }

    wxGetApp().GetAppOptions().SetTextSource(wizard->IsTextFromFileSelected() ?
        TextSource::FromFile : TextSource::EnteredText);
    wxGetApp().GetAppOptions().SetTestRecommendation(wizard->IsDocumentTypeSelected() ?
        TestRecommendation::BasedOnDocumentType :
        wizard->IsIndustrySelected() ? TestRecommendation::BasedOnIndustry :
        wizard->IsTestBundleSelected() ? TestRecommendation::UseBundle : TestRecommendation::ManuallySelectTests);
    wxGetApp().GetAppOptions().SaveOptionsFile();
    wizard->Destroy();
    return true;
    }

//-------------------------------------------------------
void ProjectDoc::DisplayReadabilityScores(const bool setFocus)
    {
    ProjectView* view = dynamic_cast<ProjectView*>(GetFirstView());
    if (view)
        { view->GetReadabilityScoresList()->Clear(); }

    for (auto rTest = GetReadabilityTests().get_tests().begin();
        rTest != GetReadabilityTests().get_tests().end();
        ++rTest)
        {
        if (rTest->is_included())
            { AddStandardReadabilityTest(rTest->get_test().get_id().c_str(), false); }
        }

    if (IsIncludingDolchSightWords())
        {
        /* NOTE, do not call AddDolchSightWords here because that will require a project refresh,
           just add a message to the score window.
           We don't actually show anything in the score window (Dolch isn't really a test),
           but point user in the right direction.*/
        SetReadabilityTestResult(ReadabilityMessages::GetDolchLabel(),
                ReadabilityMessages::GetDolchLabel(),
                L"<tr><td>" + wxString(_(L"Refer to the <a href=\"#Dolch\">Dolch Sight Words</a> section.")) +
                L"</td></tr>",
                std::make_pair(std::numeric_limits<double>::quiet_NaN(), wxString{}),
                wxString{},
                std::numeric_limits<double>::quiet_NaN(),
                std::numeric_limits<double>::quiet_NaN(), false);
        }
    AddCustomReadabilityTests();
    if (view)
        {
        UpdateExplanationListOptions(view->GetReadabilityScoresList());
        view->UpdateStatistics();
        view->GetReadabilityScoresList()->UpdateExplanationDisplay();
        view->GetReadabilityScoresList()->GetResultsListCtrl()->SortColumn(0, Wisteria::SortDirection::SortAscending);
        view->GetReadabilityScoresList()->FitWindows();
        if (setFocus)
            {
            view->GetSideBar()->SelectFolder(
                view->GetSideBar()->FindFolder(BaseProjectView::SIDEBAR_READABILITY_SCORES_SECTION_ID));
            }
        // update, add, or remove the scores summary report
        if (IsIncludingScoreSummaryReport())
            {
            wxString text = view->GetReadabilityScoresList()->GetExplanationsText();
            if (text.empty())
                { text = _(L"No tests are currently in the project."); }
            HtmlTableWindow* scoresReport =
                dynamic_cast<HtmlTableWindow*>(view->GetReadabilityResultsView().FindWindowById(
                    BaseProjectView::READABILITY_SCORES_SUMMARY_REPORT_PAGE_ID));
            if (!scoresReport)
                {
                scoresReport = new HtmlTableWindow(view->GetSplitter(),
                                                   BaseProjectView::READABILITY_SCORES_SUMMARY_REPORT_PAGE_ID);
                scoresReport->Hide();
                scoresReport->SetName(_(L"Summary Report"));
                scoresReport->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_HTML_MENU") );
                scoresReport->SetPrinterSettings(wxGetApp().GetPrintData());
                scoresReport->SetLeftPrinterHeader(wxGetApp().GetAppOptions().GetLeftPrinterHeader());
                scoresReport->SetCenterPrinterHeader(wxGetApp().GetAppOptions().GetCenterPrinterHeader());
                scoresReport->SetRightPrinterHeader(wxGetApp().GetAppOptions().GetRightPrinterHeader());
                scoresReport->SetLeftPrinterFooter(wxGetApp().GetAppOptions().GetLeftPrinterFooter());
                scoresReport->SetCenterPrinterFooter(wxGetApp().GetAppOptions().GetCenterPrinterFooter());
                scoresReport->SetRightPrinterFooter(wxGetApp().GetAppOptions().GetRightPrinterFooter());
                view->GetReadabilityResultsView().InsertWindow(1, scoresReport);
                }
            scoresReport->SetPage(
                ProjectReportFormat::FormatHtmlReportStart(
                    wxGetApp().GetAppOptions().GetControlBackgroundColor(),
                    ColorContrast::BlackOrWhiteContrast(wxGetApp().GetAppOptions().GetControlBackgroundColor())) +
                text +
                ProjectReportFormat::FormatHtmlReportEnd());
            }
        else
            {
            // we are getting rid of this window (if nothing in it)
            view->GetReadabilityResultsView().RemoveWindowById(
                BaseProjectView::READABILITY_SCORES_SUMMARY_REPORT_PAGE_ID);
            }

        // add/remove the goals
        if (GetTestGoals().size() || GetStatGoals().size())
            {
            ListCtrlEx* goalsList =
                dynamic_cast<ListCtrlEx*>(view->GetReadabilityResultsView().FindWindowById(
                    BaseProjectView::READABILITY_GOALS_PAGE_ID));
            if (!goalsList)
                {
                goalsList = new ListCtrlEx(view->GetSplitter(),
                    BaseProjectView::READABILITY_GOALS_PAGE_ID, wxDefaultPosition, wxDefaultSize,
                    wxLC_REPORT|wxBORDER_SUNKEN);
                goalsList->Hide();
                goalsList->SetLabel(_(L"Goals"));
                goalsList->SetName(_(L"Goals"));
                goalsList->InsertColumn(0, _(L"Test/Statistic"));
                goalsList->InsertColumn(1, _(L"Minimum Recommended Value"));
                goalsList->InsertColumn(2, _(L"Maximum Recommended Value"));
                goalsList->SetSortable(true);
                goalsList->EnableItemViewOnDblClick();
                goalsList->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
                UpdateListOptions(goalsList);
                view->GetReadabilityResultsView().InsertWindow(1, goalsList);
                }
            goalsList->DeleteAllItems();

            // subroutine to add a goal into our results list
            const auto insertGoalIntoList = [goalsList](const TestGoal& goal, const wxString& goalName)
                {
                const auto insertedItem = goalsList->InsertItem(0,
                    wxString::Format(L"%s %s",
                        (goal.GetPassFailFlags().all() ?
                            BaseProjectView::GetCheckmarkEmoji() : BaseProjectView::GetWarningEmoji()),
                        goalName));
                if (!std::isnan(goal.GetMinGoal()))
                    {
                    goalsList->SetItemText(insertedItem, 1,
                        wxString::Format(L"%s %s",
                            (goal.GetPassFailFlags()[0] ?
                                BaseProjectView::GetCheckmarkEmoji() : BaseProjectView::GetWarningEmoji()),
                            wxNumberFormatter::ToString(goal.GetMinGoal(), 1,
                                wxNumberFormatter::Style::Style_NoTrailingZeroes)));
                    }
                // no goal means that it "passed"
                else
                    { goalsList->SetItemText(insertedItem, 1, BaseProjectView::GetCheckmarkEmoji()); }
                if (!std::isnan(goal.GetMaxGoal()))
                    {
                    goalsList->SetItemText(insertedItem, 2,
                        wxString::Format(L"%s %s",
                        (goal.GetPassFailFlags()[1] ?
                            BaseProjectView::GetCheckmarkEmoji() : BaseProjectView::GetWarningEmoji()),
                            wxNumberFormatter::ToString(goal.GetMaxGoal(), 1,
                                wxNumberFormatter::Style::Style_NoTrailingZeroes)));
                    }
                else
                    { goalsList->SetItemText(insertedItem, 2, BaseProjectView::GetCheckmarkEmoji()); }
                };

            // test goals
            for (const auto& goal : GetTestGoals())
                {
                if (!goal.HasGoals())
                    { continue; }
                const auto [sTest, found] = GetReadabilityTests().find_test(goal.GetName().c_str());
                const wxString testName = found ? sTest->get_test().get_long_name().c_str() : goal.GetName().c_str();

                insertGoalIntoList(goal, testName);
                }
            // stats goals
            for (const auto& goal : GetStatGoals())
                {
                if (!goal.HasGoals())
                    { continue; }
                const auto statGoalLabel = GetStatGoalLabels().find({ goal.GetName(), goal.GetName() });
                const wxString goalName = (statGoalLabel != GetStatGoalLabels().cend()) ?
                    statGoalLabel->first.second.c_str() : goal.GetName().c_str();

                insertGoalIntoList(goal, goalName);
                }
            goalsList->DistributeColumns();
            goalsList->SetColumnWidth(0, goalsList->FromDIP(wxSize(200, 200).GetWidth()));
            }
        else
            {
            // we are getting rid of this window (if nothing in it)
            view->GetReadabilityResultsView().RemoveWindowById(BaseProjectView::READABILITY_GOALS_PAGE_ID);
            }
        }
    }

//-------------------------------------------------------
bool ProjectDoc::OnCreate(const wxString& path, long flags)
    {
    // see if anything is even licensed first
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureEnglishReadabilityTestsCode()))
        {
        LogMessage(_(L"You are not currently licensed to create a new project."),
            _(L"Error"), wxOK|wxICON_ERROR);
        return false;
        }
    if (flags & wxDOC_NEW)
        {
        const wxString exampleFolder = wxGetApp().FindResourceDirectory(_DT(L"Examples"));
        if (exampleFolder.empty() || !wxFileName::DirExists(exampleFolder))
            { wxLogWarning(L"Unable to find examples folder:\n%s", exampleFolder); }
        FilePathResolver resolvePath(path, false);
        // If a file path to a document (e.g., an RTF file) that is NOT from the Examples folder,
        // then bypass the wizard and just use the system defaults.
        // This is useful for scripting, where you need to create a new project from a filepath
        // and add tests and whatnot afterwards. In this case, you don't want an interactive
        // wizard appearing.
        if (!resolvePath.IsInvalidFile() &&
            wxFileName(path).GetPath().CmpNoCase(exampleFolder) != 0)
            {
            SetOriginalDocumentFilePath(path);
            SetTextSource(TextSource::FromFile);
            return wxDocument::OnCreate(wxString{}, flags);
            }
        // otherwise, use the wizard if raw text (or no text, or examples file path) was passed in
        else
            {
            if (!RunProjectWizard(path) )
                { return false; }
            }
        }
    return wxDocument::OnCreate(path, flags);
    }

//-------------------------------------------------------
bool ProjectDoc::OnNewDocument()
    {
    if (!wxDocument::OnNewDocument() )
        { return false; }

    wxBusyCursor bc;

    BaseProjectProcessingLock processingLock(this);

    LoadExcludePhrases();

    // load appended template file (if there is one)
    LoadAppendedDocument();

    // load the document
    if (GetTextSource() == TextSource::FromFile)
        {
        // there is embedded text (that may have been passed from a batch project), so load that here.
        if (GetDocumentStorageMethod() == TextStorage::EmbedText && GetDocumentText().length())
            { LoadDocument(); }
        else if (!LoadExternalDocument() )
            { return false; }
        SetTitle(ParseTitleFromFileName(GetOriginalDocumentFilePath()));
        SetFilename(ParseTitleFromFileName(GetOriginalDocumentFilePath()), true);
        }
    else if (GetTextSource() == TextSource::EnteredText)
        {
        LoadManuallyEnteredText();
        /* need to set storage method to embed if using manually entered text;
           otherwise, the text will get lost*/
        SetDocumentStorageMethod(TextStorage::EmbedText);
        }

    if (GetWords() == nullptr)
        {
        LogMessage(_(L"An unknown error occurred while loading the document."),
                    _(L"Error"), wxOK|wxICON_ERROR);
        return false;
        }

    /* See if there is an inordinate amount of titles/headers/bullets, and if they
       are asking to ignore these then make sure they understand that a large part of the
       document will be ignored.
       Note that we don't bother with this check with webpages because they normally
       contain lists for things like menus that we would indeed want to ignore.*/
    FilePathResolver resolvePath(GetOriginalDocumentFilePath(), true);
    if (GetWords()->get_sentence_count() > 0 &&
        !resolvePath.IsWebFile())
        {
        /* if document is nothing but valid sentences then it is OK.*/
        if (GetWords()->get_sentence_count() == GetWords()->get_complete_sentence_count())
            {
            // NOOP
            }
        else
            {
            const double numberOfInvalidSentencesPercentage =
                safe_divide<double>((GetWords()->get_sentence_count() - GetWords()->get_complete_sentence_count()),
                GetWords()->get_sentence_count());
            if (numberOfInvalidSentencesPercentage > 0.60f &&
                (GetInvalidSentenceMethod() == InvalidSentence::ExcludeFromAnalysis ||
                 GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings))
                {
                auto warningIter = wxGetApp().GetAppOptions().GetWarning(_DT(L"high-count-sentences-being-ignored"));
                if (warningIter != wxGetApp().GetAppOptions().GetWarnings().end() &&
                    warningIter->ShouldBeShown())
                    {
                    wxRichMessageDialog msg(wxGetApp().GetMainFrame(), warningIter->GetMessage(),
                                                    warningIter->GetTitle(), warningIter->GetFlags());
                    msg.SetEscapeId(wxID_NO);
                    msg.ShowCheckBox(_(L"Remember my answer"));
                    msg.SetYesNoLabels(_(L"Include incomplete sentences"),
                                       _(L"Continue excluding incomplete sentences"));
                    const int dlgResponse = msg.ShowModal();
                    if (warningIter != wxGetApp().GetAppOptions().GetWarnings().end() &&
                        msg.IsCheckBoxChecked())
                        {
                        warningIter->Show(false);
                        warningIter->SetPreviousResponse(dlgResponse);
                        }
                    if (dlgResponse == wxID_YES)
                        { SetInvalidSentenceMethod(InvalidSentence::IncludeAsFullSentences); }
                    }
                else if (warningIter->GetPreviousResponse() == wxID_YES)
                    { SetInvalidSentenceMethod(InvalidSentence::IncludeAsFullSentences); }
                }
            }
        }

    if (GetInvalidSentenceMethod() == InvalidSentence::ExcludeFromAnalysis ||
        GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings)
        { CalculateStatisticsIgnoringInvalidSentences(); }
    else if (GetInvalidSentenceMethod() == InvalidSentence::IncludeAsFullSentences)
        { CalculateStatistics(); }
    CalculateGraphData();
    if (GetTotalWords() == 0)
        {
        LogMessage(
            (GetTextSource() == TextSource::FromFile) ?
                _(L"No words were found in this file.") :
                _(L"No valid words were entered."),
            _(L"Import Error"), wxOK|wxICON_INFORMATION);
        return false;
        }

    wxBusyInfo bi(wxBusyInfoFlags().Text(_(L"Loading project...")));

    // load the images now
    SetBackGroundImagePath(GetBackGroundImagePath());
    SetStippleImagePath(GetStippleImagePath());
    SetWatermarkLogoPath(GetWatermarkLogoPath());

    LoadHardWords();

    DisplayStatistics();
    DisplayReadabilityScores(false);
    DisplayReadabilityGraphs();
    DisplayWordsBreakdown();
    DisplayHighlightedText(GetTextHighlightColor(), GetTextViewFont());
    DisplaySentencesBreakdown();
    DisplayGrammar();
    DisplaySightWords();

    DeleteUniqueWordMap();

    Modify(true);

    ProjectView* view = dynamic_cast<ProjectView*>(GetFirstView());
    view->UpdateSideBarIcons();
    view->Present();
    UpdateAllViews();

    const auto selectedIndex = view->GetSideBar()->FindFolder(BaseProjectView::SIDEBAR_READABILITY_SCORES_SECTION_ID);
    view->GetSideBar()->SelectFolder(selectedIndex.value_or(0), true);

    if (GetTotalWords() < 20)
        {
        if (WarningManager::HasWarning(_DT(L"document-less-than-20-words")))
            { LogMessage(*WarningManager::GetWarning(_DT(L"document-less-than-20-words")), true); }
        }
    else if (GetTotalWords() < 100)
        {
        if (WarningManager::HasWarning(_DT(L"document-less-than-100-words")))
            { LogMessage(*WarningManager::GetWarning(_DT(L"document-less-than-100-words")), true); }
        }

    // Check for sentences that got broken up by paragraph breaks and warn if there are lot of them,
    // this indicates a messed up file.
    // We do this here so not to interrupt the creation of the project too much.
    size_t paragraphBrokenSentences = 0;
    for (std::vector<size_t>::const_iterator pos = GetWords()->get_lowercase_beginning_sentences().begin();
        pos != GetWords()->get_lowercase_beginning_sentences().end();
        ++pos)
        {
        // if there is a complete, 3-word or more sentence starting with a lowercased letter
        // following an incomplete sentence (that would not be a list item or header),
        // then this might be a sentence accidentally split by two lines (e.g., a paragraph break)
        if (*pos > 0 &&
            GetWords()->get_sentences()[*pos].get_word_count() > 3 &&
            GetWords()->get_sentences()[(*pos)].get_type() == grammar::sentence_paragraph_type::complete &&
            GetWords()->get_sentences()[(*pos)-1].get_type() == grammar::sentence_paragraph_type::incomplete)
            { ++paragraphBrokenSentences; }
        }
    if (paragraphBrokenSentences >= 5)
        {
        if (WarningManager::HasWarning(_DT(L"sentences-split-by-paragraph-breaks")))
            {
            WarningMessage warningMsg = *WarningManager::GetWarning(
                _DT(L"sentences-split-by-paragraph-breaks"));
            warningMsg.SetMessage(wxString::Format(
                _(L"This document contains at least %zu sentences that appear to be split by paragraph breaks. "
                   "This may lead to incorrect results.\n"
                   "Please review your document to verify that this is intentional."), paragraphBrokenSentences));
            LogMessage(warningMsg, true);
            }
        }

    // Go through the sentences and see if any are not complete but considered valid because of their length.
    // If any are found, then mention it to the user.
    size_t sentencesMissingEndingPunctionsConsideredCompleteBecauseOfLength = 0;
    std::vector<punctuation::punctuation_mark>::const_iterator punctPos = GetWords()->get_punctuation().begin();
    wxArrayString longIncompleteSentences;
    for (std::vector<grammar::sentence_info>::const_iterator sentPos = GetWords()->get_sentences().begin();
         sentPos != GetWords()->get_sentences().end();
         ++sentPos)
        {
        // note our special case for list item lines ending with semicolons will be ignored here
        if (sentPos->is_valid() && !sentPos->ends_with_valid_punctuation() &&
            sentPos->get_ending_punctuation() != common_lang_constants::SEMICOLON &&
            sentPos->get_word_count() > GetIncludeIncompleteSentencesIfLongerThanValue())
            {
            ++sentencesMissingEndingPunctionsConsideredCompleteBecauseOfLength;
            longIncompleteSentences.Add(ProjectReportFormat::FormatSentence(this, *sentPos, punctPos,
                GetWords()->get_punctuation().end()));
            }
        }
    if (sentencesMissingEndingPunctionsConsideredCompleteBecauseOfLength > 0)
        {
        auto warningIter = wxGetApp().GetAppOptions().GetWarning(_DT(L"incomplete-sentences-valid-from-length"));
        if (warningIter != wxGetApp().GetAppOptions().GetWarnings().end() &&
            warningIter->ShouldBeShown())
            {
            ListDlg listDlg(wxGetApp().GetMainFrame(), longIncompleteSentences, false,
                wxGetApp().GetAppOptions().GetRibbonActiveTabColor(),
                wxGetApp().GetAppOptions().GetRibbonHoverColor(),
                wxGetApp().GetAppOptions().GetRibbonActiveFontColor(),
                LD_CLOSE_BUTTON|LD_DONT_SHOW_AGAIN, wxID_ANY,
                warningIter->GetTitle(), wxString::Format(
                _(L"This document contains %zu incomplete sentence(s) longer than %zu words which will be "
                   "included in the analysis.\n\nTo change this, increase the "
                   "\"Include incomplete sentences containing more than...\" option under "
                   "Project Properties->Document Indexing."),
                sentencesMissingEndingPunctionsConsideredCompleteBecauseOfLength,
                GetIncludeIncompleteSentencesIfLongerThanValue()));
            const int dlgResponse = listDlg.ShowModal();
            // save the checkbox status
            if (listDlg.IsCheckBoxChecked())
                {
                warningIter->Show(false);
                warningIter->SetPreviousResponse(dlgResponse);
                }
            }
        }

    ShowQueuedMessages();

    return true;
    }

//-------------------------------------------------------
void ProjectDoc::DisplayWordsBreakdown()
    {
    PROFILE();
    DisplayWordCharts();

    ProjectView* view = dynamic_cast<ProjectView*>(GetFirstView());

    // place the word lists beneath the last graph in the Words Breakdown section
    int lastGraphPosition{ wxNOT_FOUND };
    auto lastGraphWindow =
        view->GetWordsBreakdownView().RFindWindowByType(CLASSINFO(Wisteria::Canvas));
    if (lastGraphWindow)
        {
        lastGraphPosition =
            view->GetWordsBreakdownView().FindWindowPositionById(lastGraphWindow->GetId());
        }

    // complex words (3+ syllable)
    ListCtrlEx* listView =
        dynamic_cast<ListCtrlEx*>(view->GetWordsBreakdownView().FindWindowById(
            BaseProjectView::HARD_WORDS_LIST_PAGE_ID));
    // data will be null if call to LoadHardWords() failed
    // (will happen if document was missing or other project failure).
    if (GetWordsBreakdownInfo().Is3PlusSyllablesEnabled() &&
        GetTotalUnique3PlusSyllableWords() > 0 && Get3SyllablePlusData())
        {
        if (listView)
            {
            listView->SetVirtualDataSize(GetTotalUnique3PlusSyllableWords());
            listView->Resort();
            listView->DistributeColumns();
            }
        else
            {
            listView = new ListCtrlEx(view->GetSplitter(),
                BaseProjectView::HARD_WORDS_LIST_PAGE_ID, wxDefaultPosition, wxDefaultSize,
                wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetThreeSyllableListWordsLabel());
            listView->SetName(BaseProjectView::GetThreeSyllableListWordsLabel());
            listView->EnableGridLines();
            listView->InsertColumn(0, _(L"Word"));
            listView->InsertColumn(1, _(L"Syllable Count"));
            listView->InsertColumn(2, _(L"Frequency"));
            listView->InsertColumn(3, _(L"Suggestion"));
            listView->SetVirtualDataProvider(Get3SyllablePlusData());
            listView->SetVirtualDataSize(GetTotalUnique3PlusSyllableWords());
            listView->DistributeColumns();
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            UpdateListOptions(listView);
            // sort by syllable count, then frequency, then words (highest to lowest)
            std::vector<std::pair<size_t,Wisteria::SortDirection>> columnsToSort;
            columnsToSort.push_back(
                std::pair<size_t,Wisteria::SortDirection>(1, Wisteria::SortDirection::SortDescending));
            columnsToSort.push_back(
                std::pair<size_t,Wisteria::SortDirection>(2, Wisteria::SortDirection::SortDescending));
            columnsToSort.push_back(
                std::pair<size_t,Wisteria::SortDirection>(0, Wisteria::SortDirection::SortAscending));
            listView->SortColumns(columnsToSort);

            if (view->GetWordsBreakdownView().GetWindowCount() == 0)
                { view->GetWordsBreakdownView().AddWindow(listView); }
            else
                { view->GetWordsBreakdownView().InsertWindow(++lastGraphPosition, listView); }
            }
        }
    else
        {
        // we are getting rid of this window (if it was included before)
        view->GetWordsBreakdownView().RemoveWindowById(BaseProjectView::HARD_WORDS_LIST_PAGE_ID);
        }

    // long words (6+ characters)
    listView = dynamic_cast<ListCtrlEx*>(view->GetWordsBreakdownView().FindWindowById(
        BaseProjectView::LONG_WORDS_LIST_PAGE_ID));
    if (GetWordsBreakdownInfo().Is6PlusCharacterEnabled() &&
        GetTotalUnique6CharsPlusWords() > 0 && Get6CharacterPlusData())
        {
        if (listView)
            {
            listView->SetVirtualDataSize(GetTotalUnique6CharsPlusWords());
            listView->Resort();
            listView->DistributeColumns();
            }
        else
            {
            listView = new ListCtrlEx(view->GetSplitter(),
                BaseProjectView::LONG_WORDS_LIST_PAGE_ID, wxDefaultPosition, wxDefaultSize,
                wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetSixCharWordsListLabel());
            listView->SetName(BaseProjectView::GetSixCharWordsListLabel());
            listView->EnableGridLines();
            listView->InsertColumn(0, _(L"Word"));
            listView->InsertColumn(1, _(L"Character Count"));
            listView->InsertColumn(2, _(L"Frequency"));
            listView->InsertColumn(3, _(L"Suggestion"));
            listView->SetVirtualDataProvider(Get6CharacterPlusData());
            listView->SetVirtualDataSize(GetTotalUnique6CharsPlusWords());
            listView->DistributeColumns();
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            UpdateListOptions(listView);
            // sort by character count, then frequency, then word (highest to lowest)
            std::vector<std::pair<size_t,Wisteria::SortDirection>> columnsToSort;
            columnsToSort.push_back(
                std::pair<size_t,Wisteria::SortDirection>(1, Wisteria::SortDirection::SortDescending));
            columnsToSort.push_back(
                std::pair<size_t,Wisteria::SortDirection>(2, Wisteria::SortDirection::SortDescending));
            columnsToSort.push_back(
                std::pair<size_t,Wisteria::SortDirection>(0, Wisteria::SortDirection::SortAscending));
            listView->SortColumns(columnsToSort);

            if (view->GetWordsBreakdownView().GetWindowCount() == 0)
                { view->GetWordsBreakdownView().AddWindow(listView); }
            else
                { view->GetWordsBreakdownView().InsertWindow(++lastGraphPosition, listView); }
            }
        }
    else
        {
        // we are getting rid of this window (if it was included before)
        view->GetWordsBreakdownView().RemoveWindowById(BaseProjectView::LONG_WORDS_LIST_PAGE_ID);
        }

    // hard words (DC)
    listView = dynamic_cast<ListCtrlEx*>(view->GetWordsBreakdownView().FindWindowById(
        BaseProjectView::DC_WORDS_LIST_PAGE_ID));
    if (GetWordsBreakdownInfo().IsDCUnfamiliarEnabled() &&
        IsDaleChallLikeTestIncluded() && GetTotalUniqueDCHardWords() > 0 && GetDaleChallHardWordData())
        {
        if (listView)
            {
            listView->SetVirtualDataSize(GetTotalUniqueDCHardWords());
            listView->Resort();
            listView->DistributeColumns();
            }
        else
            {
            listView = new ListCtrlEx(view->GetSplitter(),
                BaseProjectView::DC_WORDS_LIST_PAGE_ID, wxDefaultPosition, wxDefaultSize,
                wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetDaleChallLabel());
            listView->SetName(wxString::Format(_(L"%s (Unfamiliar) List"), BaseProjectView::GetDaleChallLabel()) );
            listView->EnableGridLines();
            listView->InsertColumn(0, _(L"Unfamiliar Word"));
            listView->InsertColumn(1, _(L"Frequency"));
            listView->InsertColumn(2, _(L"Suggestion"));
            listView->SetVirtualDataProvider(GetDaleChallHardWordData());
            listView->SetVirtualDataSize(GetTotalUniqueDCHardWords());
            listView->DistributeColumns();
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            UpdateListOptions(listView);

            if (view->GetWordsBreakdownView().GetWindowCount() == 0)
                { view->GetWordsBreakdownView().AddWindow(listView); }
            else
                { view->GetWordsBreakdownView().InsertWindow(++lastGraphPosition, listView); }
            }
        }
    else
        {
        // we are getting rid of this window (if nothing in it)
        view->GetWordsBreakdownView().RemoveWindowById(BaseProjectView::DC_WORDS_LIST_PAGE_ID);
        }

    // hard words (Spache)
    listView = dynamic_cast<ListCtrlEx*>(view->GetWordsBreakdownView().FindWindowById(
        BaseProjectView::SPACHE_WORDS_LIST_PAGE_ID));
    if (GetWordsBreakdownInfo().IsSpacheUnfamiliarEnabled() &&
        GetReadabilityTests().is_test_included(ReadabilityMessages::SPACHE()) &&
        GetTotalUniqueHardWordsSpache() > 0 && GetSpacheHardWordData())
        {
        if (listView)
            {
            listView->SetVirtualDataSize(GetTotalUniqueHardWordsSpache());
            listView->Resort();
            listView->DistributeColumns();
            }
        else
            {
            listView = new ListCtrlEx(view->GetSplitter(),
                BaseProjectView::SPACHE_WORDS_LIST_PAGE_ID, wxDefaultPosition, wxDefaultSize,
                wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetSpacheLabel());
            listView->SetName(wxString::Format(_(L"%s (Unfamiliar) List"), BaseProjectView::GetSpacheLabel()) );
            listView->EnableGridLines();
            listView->InsertColumn(0, _(L"Unfamiliar Word"));
            listView->InsertColumn(1, _(L"Frequency"));
            listView->InsertColumn(2, _(L"Suggestion"));
            listView->SetVirtualDataProvider(GetSpacheHardWordData());
            listView->SetVirtualDataSize(GetTotalUniqueHardWordsSpache());
            listView->DistributeColumns();
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            UpdateListOptions(listView);

            if (view->GetWordsBreakdownView().GetWindowCount() == 0)
                { view->GetWordsBreakdownView().AddWindow(listView); }
            else
                { view->GetWordsBreakdownView().InsertWindow(++lastGraphPosition, listView); }
            }
        }
    else
        {
        // we are getting rid of this window (if nothing in it)
        view->GetWordsBreakdownView().RemoveWindowById(BaseProjectView::SPACHE_WORDS_LIST_PAGE_ID);
        }

    // hard words (Harris-Jacobson)
    listView = dynamic_cast<ListCtrlEx*>(view->GetWordsBreakdownView().FindWindowById(
        BaseProjectView::HARRIS_JACOBSON_WORDS_LIST_PAGE_ID));
    if (GetWordsBreakdownInfo().IsHarrisJacobsonUnfamiliarEnabled() &&
        GetReadabilityTests().is_test_included(ReadabilityMessages::HARRIS_JACOBSON()) &&
        GetTotalUniqueHarrisJacobsonHardWords() > 0 && GetHarrisJacobsonHardWordDataData())
        {
        if (listView)
            {
            listView->SetVirtualDataSize(GetTotalUniqueHarrisJacobsonHardWords());
            listView->Resort();
            listView->DistributeColumns();
            }
        else
            {
            listView = new ListCtrlEx(view->GetSplitter(),
                BaseProjectView::HARRIS_JACOBSON_WORDS_LIST_PAGE_ID, wxDefaultPosition, wxDefaultSize,
                wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetHarrisJacobsonLabel());
            listView->SetName(wxString::Format(_(L"%s (Unfamiliar) List"),
                BaseProjectView::GetHarrisJacobsonLabel()) );
            listView->EnableGridLines();
            listView->InsertColumn(0, _(L"Unfamiliar Word"));
            listView->InsertColumn(1, _(L"Frequency"));
            listView->InsertColumn(2, _(L"Suggestion"));
            listView->SetVirtualDataProvider(GetHarrisJacobsonHardWordDataData());
            listView->SetVirtualDataSize(GetTotalUniqueHarrisJacobsonHardWords());
            listView->DistributeColumns();
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            UpdateListOptions(listView);

            if (view->GetWordsBreakdownView().GetWindowCount() == 0)
                { view->GetWordsBreakdownView().AddWindow(listView); }
            else
                { view->GetWordsBreakdownView().InsertWindow(++lastGraphPosition, listView); }
            }
        }
    else
        {
        // we are getting rid of this window (if nothing in it)
        view->GetWordsBreakdownView().RemoveWindowById(BaseProjectView::HARRIS_JACOBSON_WORDS_LIST_PAGE_ID);
        }

    // custom hard words
    for (std::vector<CustomReadabilityTestInterface>::iterator pos = GetCustTestsInUse().begin();
        pos != GetCustTestsInUse().end();
        ++pos)
        {
        listView = dynamic_cast<ListCtrlEx*>(view->GetWordsBreakdownView().FindWindowById(
            pos->GetIterator()->get_interface_id(), CLASSINFO(ListCtrlEx)));
        if (GetWordsBreakdownInfo().IsCustomTestsUnfamiliarEnabled() &&
            pos->GetIterator()->is_using_familiar_words() &&
            pos->GetUniqueUnfamiliarWordCount() > 0 &&
            pos->GetListViewData())
            {
            if (listView)
                {
                listView->SetVirtualDataProvider(pos->GetListViewData());
                listView->SetVirtualDataSize(pos->GetUniqueUnfamiliarWordCount());
                listView->Resort();
                listView->DistributeColumns();
                }
            else
                {
                listView = new ListCtrlEx(view->GetSplitter(), pos->GetIterator()->get_interface_id(),
                    wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
                listView->Hide();
                listView->SetLabel(pos->GetIterator()->get_name().c_str());
                listView->SetName(wxString(pos->GetIterator()->get_name().c_str()) + _(L" (Unfamiliar) List"));
                listView->EnableGridLines();
                listView->InsertColumn(0, _(L"Unfamiliar Word"));
                listView->InsertColumn(1, _(L"Frequency"));
                listView->SetVirtualDataProvider(pos->GetListViewData());
                listView->SetVirtualDataSize(pos->GetUniqueUnfamiliarWordCount());
                listView->DistributeColumns();
                listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
                UpdateListOptions(listView);

                if (view->GetWordsBreakdownView().GetWindowCount() == 0)
                    { view->GetWordsBreakdownView().AddWindow(listView); }
                else
                    { view->GetWordsBreakdownView().InsertWindow(++lastGraphPosition, listView); }
                }
            }
        else
            {
            // we are getting rid of this window (if it was included before)
            view->GetWordsBreakdownView().RemoveWindowById(pos->GetIterator()->get_interface_id());
            }
        }

    // all words
    listView = dynamic_cast<ListCtrlEx*>(view->GetWordsBreakdownView().FindWindowById(
        BaseProjectView::ALL_WORDS_LIST_PAGE_ID));
    if (GetWordsBreakdownInfo().IsAllWordsEnabled() &&
        GetTotalWords() > 0 && GetAllWordsBaseData())
        {
        if (listView)
            {
            listView->SetVirtualDataSize(GetAllWordsBaseData()->GetItemCount());
            listView->Resort();
            listView->DistributeColumns();

            }
        else
            {
            listView = new ListCtrlEx(view->GetSplitter(), BaseProjectView::ALL_WORDS_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetAllWordsLabel());
            listView->SetName(BaseProjectView::GetAllWordsLabel());
            listView->EnableGridLines();
            listView->InsertColumn(0, _(L"Word"));
            listView->InsertColumn(1, _(L"Frequency"));
            listView->InsertColumn(2, _(L"Syllable Count"));
            listView->InsertColumn(3, _(L"Character Count"));
            listView->SetVirtualDataProvider(GetAllWordsBaseData());
            listView->SetVirtualDataSize(GetAllWordsBaseData()->GetItemCount());
            listView->DistributeColumns();
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            UpdateListOptions(listView);

            if (view->GetWordsBreakdownView().GetWindowCount() == 0)
                { view->GetWordsBreakdownView().AddWindow(listView); }
            else
                { view->GetWordsBreakdownView().InsertWindow(++lastGraphPosition, listView); }
            }
        }
    else
        {
        // we are getting rid of this window (if nothing in it)
        view->GetWordsBreakdownView().RemoveWindowById(BaseProjectView::ALL_WORDS_LIST_PAGE_ID);
        }

    // key words list
    listView = dynamic_cast<ListCtrlEx*>(view->GetWordsBreakdownView().FindWindowById(
        BaseProjectView::ALL_WORDS_CONDENSED_LIST_PAGE_ID));
    if (GetWordsBreakdownInfo().IsKeyWordsEnabled() &&
        GetTotalWords() > 0 && GetKeyWordsBaseData() &&
        // don't bother with condensed list if it has the same item count as the all words list
        // (that would mean that there was no condensing [stemming] that took place and that these lists are the same).
        (GetKeyWordsBaseData()->GetItemCount() != GetAllWordsBaseData()->GetItemCount()))
        {
        if (listView)
            {
            listView->SetVirtualDataSize(GetKeyWordsBaseData()->GetItemCount());
            listView->Resort();
            listView->DistributeColumns();
            }
        else
            {
            listView = new ListCtrlEx(view->GetSplitter(),
                BaseProjectView::ALL_WORDS_CONDENSED_LIST_PAGE_ID, wxDefaultPosition, wxDefaultSize,
                wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetKeyWordsLabel());
            listView->SetName(BaseProjectView::GetKeyWordsLabel());
            listView->EnableGridLines();
            listView->InsertColumn(0, _(L"Word"));
            listView->InsertColumn(1, _(L"Frequency"));
            listView->SetVirtualDataProvider(GetKeyWordsBaseData());
            listView->SetVirtualDataSize(GetKeyWordsBaseData()->GetItemCount());
            listView->DistributeColumns();
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            UpdateListOptions(listView);
            if (view->GetWordsBreakdownView().GetWindowCount() == 0)
                { view->GetWordsBreakdownView().AddWindow(listView); }
            else
                { view->GetWordsBreakdownView().InsertWindow(++lastGraphPosition, listView); }
            }
        }
    else
        {
        // we are getting rid of this window (if nothing in it)
        view->GetWordsBreakdownView().RemoveWindowById(BaseProjectView::ALL_WORDS_CONDENSED_LIST_PAGE_ID);
        }

#ifndef NDEBUG
    // display it
    listView = dynamic_cast<ListCtrlEx*>(view->GetWordsBreakdownView().FindWindowById(
        BaseProjectView::PROPER_NOUNS_LIST_PAGE_ID));
    if (GetWordsBreakdownInfo().IsProperNounsEnabled() &&
        GetProperNounsData() && GetProperNounsData()->GetItemCount())
        {
        if (listView)
            {
            listView->SetVirtualDataSize(GetProperNounsData()->GetItemCount());
            listView->Resort();
            listView->DistributeColumns();
            }
        else
            {
            listView = new ListCtrlEx(view->GetSplitter(),
                BaseProjectView::PROPER_NOUNS_LIST_PAGE_ID, wxDefaultPosition, wxDefaultSize,
                wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetProperNounsLabel());
            listView->SetName(BaseProjectView::GetProperNounsLabel());
            listView->EnableGridLines();
            listView->InsertColumn(0, _(L"Proper Noun"));
            listView->InsertColumn(1, _(L"Frequency"));
            listView->InsertColumn(2, _(L"Personal"));
            listView->SetVirtualDataProvider(GetProperNounsData());
            listView->SetVirtualDataSize(GetProperNounsData()->GetItemCount());
            listView->DistributeColumns();
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            UpdateListOptions(listView);

            if (view->GetWordsBreakdownView().GetWindowCount() == 0)
                { view->GetWordsBreakdownView().AddWindow(listView); }
            else
                { view->GetWordsBreakdownView().InsertWindow(++lastGraphPosition, listView); }
            }
        }
    else
        {
        // we are getting rid of this window (if nothing in it)
        view->GetWordsBreakdownView().RemoveWindowById(BaseProjectView::PROPER_NOUNS_LIST_PAGE_ID);
        }

    // contractions
    listView = dynamic_cast<ListCtrlEx*>(view->GetWordsBreakdownView().FindWindowById(
        BaseProjectView::CONTRACTIONS_LIST_PAGE_ID));
    if (GetWordsBreakdownInfo().IsContractionsEnabled() &&
        GetContractionsData() && GetContractionsData()->GetItemCount())
        {
        if (listView)
            {
            listView->SetVirtualDataSize(GetContractionsData()->GetItemCount());
            listView->Resort();
            listView->DistributeColumns();
            }
        else
            {
            listView = new ListCtrlEx(view->GetSplitter(), BaseProjectView::CONTRACTIONS_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetContractionsLabel());
            listView->SetName(BaseProjectView::GetContractionsLabel());
            listView->EnableGridLines();
            listView->InsertColumn(0, _(L"Contraction"));
            listView->InsertColumn(1, _(L"Frequency"));
            listView->SetVirtualDataProvider(GetContractionsData());
            listView->SetVirtualDataSize(GetContractionsData()->GetItemCount());
            listView->DistributeColumns();
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            UpdateListOptions(listView);
            
            if (view->GetWordsBreakdownView().GetWindowCount() == 0)
                { view->GetWordsBreakdownView().AddWindow(listView); }
            else
                { view->GetWordsBreakdownView().InsertWindow(++lastGraphPosition, listView); }
            }
        }
    else
        {
        // we are getting rid of this window (if nothing in it)
        view->GetWordsBreakdownView().RemoveWindowById(BaseProjectView::CONTRACTIONS_LIST_PAGE_ID);
        }
#endif
    }

//-------------------------------------------------------
void ProjectDoc::DisplaySentenceCharts()
    {
    PROFILE();
    ProjectView* view = dynamic_cast<ProjectView*>(GetFirstView());
    wxGCDC gdc(view->GetDocFrame());

    // box plot of sentence lengths
    if (GetSentencesBreakdownInfo().IsLengthsBoxPlotEnabled() &&
        m_sentenceWordLengths->GetRowCount())
        {
        Wisteria::Canvas* sentenceBoxPlotCanvas =
            dynamic_cast<Wisteria::Canvas*>(view->GetSentencesBreakdownView().FindWindowById(
                BaseProjectView::SENTENCE_BOX_PLOT_PAGE_ID));
        if (!sentenceBoxPlotCanvas)
            {
            sentenceBoxPlotCanvas = new Wisteria::Canvas(view->GetSplitter(),
                BaseProjectView::SENTENCE_BOX_PLOT_PAGE_ID);
            sentenceBoxPlotCanvas->SetFixedObjectsGridSize(1, 1);
            sentenceBoxPlotCanvas->SetFixedObject(0, 0, std::make_shared<BoxPlot>(sentenceBoxPlotCanvas));
            sentenceBoxPlotCanvas->Hide();
            sentenceBoxPlotCanvas->SetLabel(BaseProjectView::GetSentenceLengthBoxPlotLabel());
            sentenceBoxPlotCanvas->SetName(BaseProjectView::GetSentenceLengthBoxPlotLabel());
            sentenceBoxPlotCanvas->GetTopTitles().push_back(
                GraphItems::Label(GraphItemInfo(_(L"Sentence-lengths Spread")).Scaling(
                    sentenceBoxPlotCanvas->GetScaling()).Pen(wxNullPen)));
            sentenceBoxPlotCanvas->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_GRAPH_MENU") );
            sentenceBoxPlotCanvas->SetPrinterSettings(*wxGetApp().GetPrintData());
            view->GetSentencesBreakdownView().AddWindow(sentenceBoxPlotCanvas);
            }
        auto sentenceBoxPlot =
            std::dynamic_pointer_cast<BoxPlot>(sentenceBoxPlotCanvas->GetFixedObject(0, 0));
        sentenceBoxPlot->SetBrushScheme(
                    std::make_shared<Brushes::Schemes::BrushScheme>(
                        Colors::Schemes::ColorScheme({ GetGraphBoxColor() })));
        sentenceBoxPlot->SetData(m_sentenceWordLengths, GetSentenceWordCountsColumnName(),
            std::nullopt);

        UpdateGraphOptions(sentenceBoxPlotCanvas);

        sentenceBoxPlot->SetShadowType(IsDisplayingDropShadows() ?
            ShadowType::RightSideAndBottomShadow : ShadowType::NoShadow);
        sentenceBoxPlot->ShowLabels(IsDisplayingBoxPlotLabels());
        sentenceBoxPlot->ShowAllPoints(IsShowingAllBoxPlotPoints());
        sentenceBoxPlot->SetOpacity(GetGraphBoxOpacity());
        sentenceBoxPlot->SetBoxEffect(GetGraphBoxEffect());

        sentenceBoxPlotCanvas->CalcAllSizes(gdc);
        }
    else
        { view->GetSentencesBreakdownView().RemoveWindowById(BaseProjectView::SENTENCE_BOX_PLOT_PAGE_ID); }

    // histogram of sentence lengths
    if (GetSentencesBreakdownInfo().IsLengthsHistogramEnabled() &&
        m_sentenceWordLengths->GetRowCount())
        {
        Wisteria::Canvas* sentenceHistogramCanvas =
            dynamic_cast<Wisteria::Canvas*>(view->GetSentencesBreakdownView().FindWindowById(
                BaseProjectView::SENTENCE_HISTOGRAM_PAGE_ID));
        if (!sentenceHistogramCanvas)
            {
            sentenceHistogramCanvas = new Wisteria::Canvas(view->GetSplitter(),
                BaseProjectView::SENTENCE_HISTOGRAM_PAGE_ID);
            sentenceHistogramCanvas->SetFixedObjectsGridSize(1,1);
            sentenceHistogramCanvas->SetFixedObject(0, 0,
                std::make_shared<Histogram>(sentenceHistogramCanvas,
                    std::make_shared<Wisteria::Brushes::Schemes::BrushScheme>(
                        *std::make_shared<Wisteria::Colors::Schemes::ColorScheme>(
                        Wisteria::Colors::Schemes::ColorScheme{ GetHistogramBarColor() }))) );
            sentenceHistogramCanvas->Hide();
            sentenceHistogramCanvas->SetLabel(BaseProjectView::GetSentenceLengthHistogramLabel());
            sentenceHistogramCanvas->SetName(BaseProjectView::GetSentenceLengthHistogramLabel());
            sentenceHistogramCanvas->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_GRAPH_MENU") );
            sentenceHistogramCanvas->SetPrinterSettings(*wxGetApp().GetPrintData());
            view->GetSentencesBreakdownView().AddWindow(sentenceHistogramCanvas);
            }
        UpdateGraphOptions(sentenceHistogramCanvas);

        auto sentenceHistogram =
            std::dynamic_pointer_cast<Histogram>(sentenceHistogramCanvas->GetFixedObject(0, 0));
        assert(sentenceHistogram);

        sentenceHistogram->GetTitle().SetText(_(L"Sentence-lengths Distribution"));
        sentenceHistogram->GetTitle().SetRelativeAlignment(RelativeAlignment::Centered);
        sentenceHistogram->SetSortable(false);
        sentenceHistogram->SetShadowType(IsDisplayingDropShadows() ?
            ShadowType::RightSideShadow : ShadowType::NoShadow);
        sentenceHistogram->SetBarEffect(GetHistogramBarEffect());
        sentenceHistogram->SetBarOpacity(GetHistogramBarOpacity());
        sentenceHistogram->SetBrushScheme(
            std::make_shared<Wisteria::Brushes::Schemes::BrushScheme>(
                *std::make_shared<Wisteria::Colors::Schemes::ColorScheme>(
                    Wisteria::Colors::Schemes::ColorScheme({ GetHistogramBarColor() }))));
        sentenceHistogram->SetData(m_sentenceWordLengths, GetSentenceWordCountsColumnName(),
            std::nullopt,
            Histogram::BinningMethod::BinUniqueValues,
            RoundingMethod::NoRounding,
            Histogram::IntervalDisplay::Midpoints,
            IsDisplayingBarLabels() ?
                GetHistrogramBinLabelDisplay() : BinLabelDisplay::NoDisplay,
            true, 1, std::make_pair(std::nullopt, 8));
        sentenceHistogram->GetLeftYAxis().GetTitle().SetText(_(L"Number of Sentences"));
        sentenceHistogram->GetBottomXAxis().GetTitle().SetText(_(L"Number of Words per Sentence"));
        sentenceHistogramCanvas->CalcAllSizes(gdc);
        }
    else
        {
        // we are getting rid of this window (if nothing in it)
        view->GetSentencesBreakdownView().RemoveWindowById(BaseProjectView::SENTENCE_HISTOGRAM_PAGE_ID);
        }

    // heatmap of sentence lengths
    const auto paragraphCount = (GetInvalidSentenceMethod() == InvalidSentence::IncludeAsFullSentences) ?
        GetWords()->get_paragraph_count() : GetWords()->get_valid_paragraph_count();
    if (GetSentencesBreakdownInfo().IsLengthsHeatmapEnabled() && m_sentenceWordLengths->GetRowCount())
        {
        Wisteria::Canvas* sentenceHeatmapCanvas =
            dynamic_cast<Wisteria::Canvas*>(view->GetSentencesBreakdownView().FindWindowById(
                BaseProjectView::SENTENCE_HEATMAP_PAGE_ID));
        if (!sentenceHeatmapCanvas)
            {
            sentenceHeatmapCanvas = new Wisteria::Canvas(view->GetSplitter(),
                BaseProjectView::SENTENCE_HEATMAP_PAGE_ID);
            sentenceHeatmapCanvas->SetFixedObjectsGridSize(1, 2);
            sentenceHeatmapCanvas->SetFixedObject(0, 0, std::make_shared<HeatMap>(sentenceHeatmapCanvas));
            sentenceHeatmapCanvas->Hide();
            sentenceHeatmapCanvas->SetLabel(BaseProjectView::GetSentenceLengthHeatmapLabel());
            sentenceHeatmapCanvas->SetName(BaseProjectView::GetSentenceLengthHeatmapLabel());

            sentenceHeatmapCanvas->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_GRAPH_MENU") );
            sentenceHeatmapCanvas->SetPrinterSettings(*wxGetApp().GetPrintData());
            view->GetSentencesBreakdownView().AddWindow(sentenceHeatmapCanvas);
            }
        UpdateGraphOptions(sentenceHeatmapCanvas);

        auto heatMap = std::dynamic_pointer_cast<HeatMap>(sentenceHeatmapCanvas->GetFixedObject(0, 0));
        heatMap->SetCanvasMargins(5, 5, 5, 5);
        heatMap->SetData(m_sentenceWordLengths,
            GetSentenceWordCountsColumnName(),
            ((paragraphCount <= 500) ? std::optional<wxString>(GetGroupColumnName()) : std::nullopt),
            (paragraphCount <= 20 ? 1 :
             paragraphCount <= 50 ? 2 :
             paragraphCount <= 75 ? 3 :
             paragraphCount <= 100 ? 4 : 5));

        heatMap->GetCaption().SetText((paragraphCount <= 500) ?
            _(L"Paragraph order is displayed top-to-bottom.\nSentence order is displayed left-to-right "
               "within each paragraph.\nLonger sentences (relative to the document) are darker; "
               "shorter ones are lighter.") :
            _(L"Sentence order is displayed left-to-right, top-to-bottom.\nLonger sentences "
               "(relative to the document) are darker; shorter ones are lighter."));
        heatMap->GetTitle().SetRelativeAlignment(Wisteria::RelativeAlignment::Centered);
        heatMap->GetTitle().SetText(_(L"Sentence-lengths Heatmap"));
        heatMap->SetLeftPadding(5);
        heatMap->SetGroupHeaderPrefix(_(L"Paragraphs"));

        auto legend = heatMap->CreateLegend(
            LegendOptions().PlacementHint(LegendCanvasPlacementHint::RightOfGraph).IncludeHeader(true));
        legend->SetLine(0, _(L"Sentence Word Counts"));
        legend->SetCanvasWidthProportion(sentenceHeatmapCanvas->CalcMinWidthProportion(legend));
        sentenceHeatmapCanvas->SetFixedObject(0, 1, legend);

        sentenceHeatmapCanvas->CalcAllSizes(gdc);
        }
    else
        { view->GetSentencesBreakdownView().RemoveWindowById(BaseProjectView::SENTENCE_HEATMAP_PAGE_ID); }
    }

//-------------------------------------------------------
void ProjectDoc::DisplayWordCharts()
    {
    PROFILE();
    ProjectView* view = dynamic_cast<ProjectView*>(GetFirstView());
    wxGCDC gdc(view->GetDocFrame());

    // word bar chart
    Wisteria::Canvas* wordBarChartCanvas =
        dynamic_cast<Wisteria::Canvas*>(view->GetWordsBreakdownView().FindWindowById(
            BaseProjectView::WORD_BREAKDOWN_PAGE_ID));
    if (GetWordsBreakdownInfo().IsWordBarchartEnabled())
        {
        if (!wordBarChartCanvas)
            {
            wordBarChartCanvas = new Wisteria::Canvas(view->GetSplitter(),
                                                      BaseProjectView::WORD_BREAKDOWN_PAGE_ID);
            wordBarChartCanvas->SetFixedObjectsGridSize(1, 1);
            wordBarChartCanvas->SetFixedObject(0, 0, std::make_shared<BarChart>(wordBarChartCanvas));
            wordBarChartCanvas->Hide();
            wordBarChartCanvas->SetLabel(BaseProjectView::GetWordCountsLabel());
            wordBarChartCanvas->SetName(BaseProjectView::GetWordCountsLabel());
            wordBarChartCanvas->GetTopTitles().push_back(GraphItems::Label(
                GraphItemInfo(_(L"Word Totals (by Category)")).
                Scaling(wordBarChartCanvas->GetScaling()).Pen(wxNullPen)));
            wordBarChartCanvas->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_GRAPH_MENU") );
            wordBarChartCanvas->SetPrinterSettings(*wxGetApp().GetPrintData());
            std::dynamic_pointer_cast<BarChart>(wordBarChartCanvas->GetFixedObject(0, 0))->SetSortable(true);
            std::dynamic_pointer_cast<BarChart>(
                wordBarChartCanvas->GetFixedObject(0, 0))->SetSortDirection(Wisteria::SortDirection::NoSort);
            view->GetWordsBreakdownView().InsertWindow(0, wordBarChartCanvas);
            }
        UpdateGraphOptions(wordBarChartCanvas);

        auto wordBarChart = std::dynamic_pointer_cast<BarChart>(wordBarChartCanvas->GetFixedObject(0, 0));
        assert(wordBarChart);

        wordBarChart->ClearBars();
        wordBarChart->SetBarOrientation(static_cast<Wisteria::Orientation>(GetBarChartOrientation()));
        wordBarChart->GetBarAxis().SetLabelDisplay(Wisteria::AxisLabelDisplay::DisplayOnlyCustomLabels);
        wordBarChart->GetScalingAxis().SetLabelDisplay(Wisteria::AxisLabelDisplay::DisplayCustomLabelsOrValues);
        wordBarChart->IncludeSpacesBetweenBars(true);
        wordBarChart->SetShadowType(IsDisplayingDropShadows() ?
            ShadowType::RightSideShadow : ShadowType::NoShadow);
        wordBarChart->SetBinLabelDisplay(IsDisplayingBarLabels() ?
            BinLabelDisplay::BinValue : BinLabelDisplay::NoDisplay);
        wordBarChart->GetRightYAxis().Show(false);
        wordBarChart->GetTopXAxis().Show(false);
        wordBarChart->GetScalingAxis().GetGridlinePen() = wxNullPen;
        // Do not change the order of these bars, brackets are built based on this order (see below)
        size_t currentBar{ 0 };
        // go through the custom (familiar word) tests
        for(const auto& cText : GetCustTestsInUse())
            {
            if (!cText.GetIterator()->is_using_familiar_words())
                { continue; }
            wordBarChart->AddBar(BarChart::Bar(++currentBar,
                {
                    { BarChart::BarBlock(BarChart::BarBlockInfo(static_cast<double>(cText.GetUnfamiliarWordCount())).
                        Brush(GetBarChartBarColor())) }
                },
                wxNumberFormatter::ToString(cText.GetUnfamiliarWordCount(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|wxNumberFormatter::Style::Style_WithThousandsSep),
                GraphItems::Label((cText.GetIterator()->get_name().c_str()) + _(L" (unfamiliar)")),
                                                  GetGraphBarEffect(), GetGraphBarOpacity()));
            }

        if (IsDaleChallLikeTestIncluded())
            {
            wordBarChart->AddBar(BarChart::Bar(++currentBar,
                { { BarChart::BarBlock(BarChart::BarBlockInfo(static_cast<double>(GetTotalHardWordsDaleChall())).
                    Brush(GetBarChartBarColor())) } },
                wxNumberFormatter::ToString(GetTotalHardWordsDaleChall(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|wxNumberFormatter::Style::Style_WithThousandsSep),
                GraphItems::Label(_(L"DC (unfamiliar)")),
                GetGraphBarEffect(), GetGraphBarOpacity()) );
            }
        if (GetReadabilityTests().is_test_included(ReadabilityMessages::SPACHE()))
            {
            wordBarChart->AddBar(BarChart::Bar(++currentBar,
                { { BarChart::BarBlock(BarChart::BarBlockInfo(static_cast<double>(GetTotalHardWordsSpache())).
                    Brush(GetBarChartBarColor())) } },
                wxNumberFormatter::ToString(GetTotalHardWordsSpache(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|wxNumberFormatter::Style::Style_WithThousandsSep),
                GraphItems::Label(_(L"Spache (unfamiliar)")),
                GetGraphBarEffect(), GetGraphBarOpacity()) );
            }
        if (GetReadabilityTests().is_test_included(ReadabilityMessages::HARRIS_JACOBSON()))
            {
            wordBarChart->AddBar(BarChart::Bar(++currentBar,
                { { BarChart::BarBlock(BarChart::BarBlockInfo(static_cast<double>(GetTotalHardWordsHarrisJacobson())).
                    Brush(GetBarChartBarColor())) } },
                wxNumberFormatter::ToString(GetTotalHardWordsHarrisJacobson(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|wxNumberFormatter::Style::Style_WithThousandsSep),
                GraphItems::Label(_(L"HJ (unfamiliar)")),
                GetGraphBarEffect(), GetGraphBarOpacity()) );
            }
        wordBarChart->AddBar(BarChart::Bar(++currentBar,
            { { BarChart::BarBlock(BarChart::BarBlockInfo(static_cast<double>(GetTotal3PlusSyllabicWords())).
                Brush(GetBarChartBarColor())) } },
            wxNumberFormatter::ToString(GetTotal3PlusSyllabicWords(), 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|wxNumberFormatter::Style::Style_WithThousandsSep),
            GraphItems::Label(_(L"3+ syllables")),
            GetGraphBarEffect(), GetGraphBarOpacity()) );
        wordBarChart->AddBar(BarChart::Bar(++currentBar,
            { { BarChart::BarBlock(BarChart::BarBlockInfo(static_cast<double>(GetTotalLongWords())).
                Brush(GetBarChartBarColor())) } },
            wxNumberFormatter::ToString(GetTotalLongWords(), 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|wxNumberFormatter::Style::Style_WithThousandsSep),
            GraphItems::Label(_(L"6+ characters")),
            GetGraphBarEffect(), GetGraphBarOpacity()) );
        wordBarChart->AddBar(BarChart::Bar(++currentBar,
            { { BarChart::BarBlock(BarChart::BarBlockInfo(static_cast<double>(GetTotalMonoSyllabicWords())).
                Brush(GetBarChartBarColor())) } },
            wxNumberFormatter::ToString(GetTotalMonoSyllabicWords(), 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes | wxNumberFormatter::Style::Style_WithThousandsSep),
            GraphItems::Label(_(L"Monosyllabic")),
            GetGraphBarEffect(), GetGraphBarOpacity()));
        if (GetReadabilityTests().is_test_included(ReadabilityMessages::EFLAW()))
            {
            wordBarChart->AddBar(BarChart::Bar(++currentBar,
                { { BarChart::BarBlock(BarChart::BarBlockInfo(static_cast<double>(GetTotalMiniWords())).
                    Brush(GetBarChartBarColor())) } },
                wxNumberFormatter::ToString(GetTotalMiniWords(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|wxNumberFormatter::Style::Style_WithThousandsSep),
                GraphItems::Label(_(L"EFLAW miniwords")),
                GetGraphBarEffect(), GetGraphBarOpacity()) );
            }
        // all the words
        wordBarChart->AddBar(BarChart::Bar(++currentBar,
            { { BarChart::BarBlock(BarChart::BarBlockInfo(static_cast<double>(GetTotalWords())).
                Brush(GetBarChartBarColor())) } },
            wxNumberFormatter::ToString(GetTotalWords(), 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|wxNumberFormatter::Style::Style_WithThousandsSep),
            GraphItems::Label(_(L"Total Words")),
            GetGraphBarEffect(), GetGraphBarOpacity()) );

        // add brackets around the factors of the categories
        auto threeSyllableBarPos = wordBarChart->GetBarAxis().FindCustomLabelPosition(_(L"3+ syllables"));
        auto monoSyllableBarPos = wordBarChart->GetBarAxis().FindCustomLabelPosition(_(L"Monosyllabic"));
        auto eflawBarPos = wordBarChart->GetBarAxis().FindCustomLabelPosition(_(L"EFLAW miniwords"));

        if (threeSyllableBarPos && threeSyllableBarPos.value() > 1)
            {
            wordBarChart->GetBarAxis().AddBracket(
                Axis::AxisBracket(1, threeSyllableBarPos.value(),
                    ((threeSyllableBarPos.value() - 1) * math_constants::half) + 1,
                    _("Familiarity")));
            }

        assert(threeSyllableBarPos && monoSyllableBarPos && L"Can't find bars in word barchart!");
        if (threeSyllableBarPos && monoSyllableBarPos)
            {
            wordBarChart->GetBarAxis().AddBracket(
                Axis::AxisBracket(threeSyllableBarPos.value(), eflawBarPos.value_or(monoSyllableBarPos.value()),
                ((eflawBarPos.value_or(monoSyllableBarPos.value()) - threeSyllableBarPos.value())
                    * math_constants::half) +
                        threeSyllableBarPos.value(),
                _("Complexity or\nLength")));
            }

        for (auto& bracket : wordBarChart->GetBarAxis().GetBrackets())
            {
            bracket.SetPerpendicularLabelConnectionLinesAlignment(AxisLabelAlignment::AlignWithBoundary);
            }

        // update the bar labels
        wordBarChart->SetBinLabelDisplay(IsDisplayingBarLabels() ?
            BinLabelDisplay::BinValue : BinLabelDisplay::NoDisplay);

        // won't sort unless this is a refresh and user sorted it previously
        wordBarChart->SortBars(BarChart::BarSortComparison::SortByBarLength, wordBarChart->GetSortDirection());

        wordBarChartCanvas->CalcAllSizes(gdc);
        }
    else
        {
        // we are getting rid of this window (if nothing in it)
        view->GetWordsBreakdownView().RemoveWindowById(BaseProjectView::WORD_BREAKDOWN_PAGE_ID);
        }

    // syllable histogram
    Wisteria::Canvas* histoCanvas =
        dynamic_cast<Wisteria::Canvas*>(view->GetWordsBreakdownView().
            FindWindowById(BaseProjectView::SYLLABLE_HISTOGRAM_PAGE_ID));
    if (GetWordsBreakdownInfo().IsSyllableHistogramEnabled())
        {
        if (!histoCanvas)
            {
            histoCanvas = new Wisteria::Canvas(view->GetSplitter(),
                                               BaseProjectView::SYLLABLE_HISTOGRAM_PAGE_ID);
            histoCanvas->SetFixedObjectsGridSize(1, 1);
            histoCanvas->SetFixedObject(0, 0,
                std::make_shared<Histogram>(histoCanvas,
                    std::make_shared<Wisteria::Brushes::Schemes::BrushScheme>(
                        *std::make_shared<Wisteria::Colors::Schemes::ColorScheme>(
                        Wisteria::Colors::Schemes::ColorScheme({ GetHistogramBarColor() }))) ) );
            histoCanvas->Hide();
            histoCanvas->SetLabel(BaseProjectView::GetSyllableCountsLabel());
            histoCanvas->SetName(BaseProjectView::GetSyllableCountsLabel());
            histoCanvas->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_GRAPH_MENU") );
            histoCanvas->SetPrinterSettings(*wxGetApp().GetPrintData());
            const auto wordChartPosition =
                view->GetWordsBreakdownView().FindWindowPositionById(BaseProjectView::WORD_BREAKDOWN_PAGE_ID);
            view->GetWordsBreakdownView().InsertWindow((wordChartPosition != wxNOT_FOUND) ?
                wordChartPosition+1 : 0, histoCanvas);
            }
        UpdateGraphOptions(histoCanvas);

        auto syllableHistogram = std::dynamic_pointer_cast<Histogram>(histoCanvas->GetFixedObject(0, 0));
        assert(syllableHistogram);

        syllableHistogram->GetTitle().SetText(_(L"Words (by Syllable Count)"));
        syllableHistogram->GetTitle().SetRelativeAlignment(RelativeAlignment::Centered);
        syllableHistogram->SetSortable(false);
        syllableHistogram->SetShadowType(IsDisplayingDropShadows() ?
            ShadowType::RightSideShadow : ShadowType::NoShadow);
        syllableHistogram->SetBarEffect(GetHistogramBarEffect());
        syllableHistogram->SetBarOpacity(GetHistogramBarOpacity());
        syllableHistogram->SetBrushScheme(
            std::make_shared<Wisteria::Brushes::Schemes::BrushScheme>(
                *std::make_shared<Wisteria::Colors::Schemes::ColorScheme>(
                Wisteria::Colors::Schemes::ColorScheme({ GetHistogramBarColor() }))));
        syllableHistogram->SetData(m_syllableCounts, GetSyllableCountsColumnName(),
            std::nullopt,
            Histogram::BinningMethod::BinUniqueValues,
            RoundingMethod::NoRounding,
            Histogram::IntervalDisplay::Midpoints,
            IsDisplayingBarLabels() ?
                GetHistrogramBinLabelDisplay() : BinLabelDisplay::NoDisplay,
            true, 1);
        syllableHistogram->GetLeftYAxis().GetTitle().SetText(_(L"Number of Words"));
        syllableHistogram->GetBottomXAxis().GetTitle().SetText(_(L"Number of Syllables per Word"));
        syllableHistogram->GetRightYAxis().Show(false);
        syllableHistogram->GetTopXAxis().Show(false);
        syllableHistogram->GetScalingAxis().GetGridlinePen() = wxNullPen;
        histoCanvas->CalcAllSizes(gdc);
        }
    else
        {
        // we are getting rid of this window (if nothing in it)
        view->GetWordsBreakdownView().RemoveWindowById(BaseProjectView::SYLLABLE_HISTOGRAM_PAGE_ID);
        }

    // word cloud
    Wisteria::Canvas* wordCloudCanvas =
        dynamic_cast<Wisteria::Canvas*>(view->GetWordsBreakdownView().FindWindowById(
            BaseProjectView::WORD_CLOUD_PAGE_ID));
    if (GetWordsBreakdownInfo().IsWordCloudEnabled() &&
        m_keyWordsDataset->GetRowCount())
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
            wordCloudCanvas->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_GRAPH_MENU") );
            wordCloudCanvas->SetPrinterSettings(*wxGetApp().GetPrintData());

            // place beneath bar charts (if included)
            int otherGraphPosition{ wxNOT_FOUND };
            auto otherGraphWindow =
                view->GetWordsBreakdownView().RFindWindowByType(CLASSINFO(Wisteria::Canvas));
            if (otherGraphWindow)
                {
                otherGraphPosition =
                    view->GetWordsBreakdownView().FindWindowPositionById(otherGraphWindow->GetId());
                }
            view->GetWordsBreakdownView().InsertWindow(
                (otherGraphPosition != wxNOT_FOUND) ?
                    otherGraphPosition + 1 :
                    0,
                wordCloudCanvas);
            }
        UpdateGraphOptions(wordCloudCanvas);

        auto wordCloud = std::dynamic_pointer_cast<WordCloud>(wordCloudCanvas->GetFixedObject(0, 0));
        assert(wordCloud);
        // top 100 words, with a min frequency of 2 (unless less than 100 words, then include everything)
        wordCloud->SetData(m_keyWordsDataset,
                           GetWordsColumnName(), GetWordsCountsColumnName(),
                           (m_keyWordsDataset->GetRowCount() < 100 ? 1 : 2),
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
void ProjectDoc::AddCrawfordGraph(const bool setFocus)
    {
    const double gradeValue =
        readability::crawford(GetTotalWords(), GetTotalSyllables(), GetTotalSentences());
    const double syllablesPer100Words =
        GetTotalSyllables()*(safe_divide<double>(100, GetTotalWords()));

    const wxString scoresColumnName{ _DT(L"SCORES") };
    const wxString syllablesColumnName{ _DT(L"SYLLABLES") };

    auto scoreDataset = std::make_shared<Wisteria::Data::Dataset>();
    scoreDataset->AddContinuousColumn(scoresColumnName);
    scoreDataset->AddContinuousColumn(syllablesColumnName);

    scoreDataset->AddRow(Data::RowInfo().
                    Continuous({ gradeValue, syllablesPer100Words }));

    // Crawford graph
    ProjectView* view = dynamic_cast<ProjectView*>(GetFirstView());
    wxGCDC gdc(view->GetDocFrame());

    Wisteria::Canvas* crawfordGraphView =
        dynamic_cast<Wisteria::Canvas*>(view->GetReadabilityResultsView().
            FindWindowById(BaseProjectView::CRAWFORD_GRAPH_PAGE_ID));
    if (crawfordGraphView)
        {
        auto crawfordGraph =
            std::dynamic_pointer_cast<CrawfordGraph>(crawfordGraphView->GetFixedObject(0, 0));
        crawfordGraph->SetData(scoreDataset, scoresColumnName, syllablesColumnName);
        crawfordGraphView->CalcAllSizes(gdc);
        }
    else
        {
        crawfordGraphView = new Wisteria::Canvas(view->GetSplitter(),
                                                 BaseProjectView::CRAWFORD_GRAPH_PAGE_ID);
        crawfordGraphView->SetFixedObjectsGridSize(1, 1);

        crawfordGraphView->Hide();
        crawfordGraphView->SetLabel(BaseProjectView::GetCrawfordGraphLabel());
        crawfordGraphView->SetName(BaseProjectView::GetCrawfordGraphLabel());
        crawfordGraphView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_GRAPH_MENU") );
        crawfordGraphView->SetPrinterSettings(*wxGetApp().GetPrintData());

        auto crawfordGraph =
            std::make_shared<CrawfordGraph>(crawfordGraphView,
                std::make_shared<Colors::Schemes::ColorScheme>
                 (Colors::Schemes::ColorScheme{
                     ColorBrewer::GetColor(Colors::Color::CelestialBlue) }));

        crawfordGraph->SetData(scoreDataset, scoresColumnName, syllablesColumnName);
        crawfordGraphView->SetFixedObject(0, 0, crawfordGraph);

        crawfordGraphView->CalcAllSizes(gdc);
        view->GetReadabilityResultsView().AddWindow(crawfordGraphView);
        }
    UpdateGraphOptions(crawfordGraphView);

    // if they asked to set focus to the score, then select the graph
    if (setFocus)
        {
        view->UpdateSideBarIcons();
        view->GetSideBar()->SelectSubItem(
            view->GetSideBar()->FindSubItem(BaseProjectView::CRAWFORD_GRAPH_PAGE_ID));
        }
    }

//-------------------------------------------------------
void ProjectDoc::AddDB2Plot(const bool setFocus)
    {
    const size_t score = readability::danielson_bryan_2(GetTotalWords(),
                                                        GetTotalCharactersPlusPunctuation(),
                                                        GetTotalSentences());

    const wxString scoresColumnName{ _DT(L"SCORES") };

    auto scoreDataset = std::make_shared<Wisteria::Data::Dataset>();
    scoreDataset->AddContinuousColumn(scoresColumnName);

    scoreDataset->AddRow(Data::RowInfo().
                    Continuous({ static_cast<double>(score) }));

    // DB2
    ProjectView* view = dynamic_cast<ProjectView*>(GetFirstView());
    wxGCDC gdc(view->GetDocFrame());

    Wisteria::Canvas* db2PlotView =
        dynamic_cast<Wisteria::Canvas*>(view->GetReadabilityResultsView().
            FindWindowById(BaseProjectView::DB2_PAGE_ID));
    if (db2PlotView)
        {
        auto db2Plot =
            std::dynamic_pointer_cast<DanielsonBryan2Plot>(db2PlotView->GetFixedObject(0, 0));
        db2Plot->SetData(scoreDataset, scoresColumnName);
        db2PlotView->CalcAllSizes(gdc);
        }
    else
        {
        db2PlotView = new Wisteria::Canvas(view->GetSplitter(),
                                           BaseProjectView::DB2_PAGE_ID);
        db2PlotView->SetFixedObjectsGridSize(1,1);

        db2PlotView->Hide();
        db2PlotView->SetLabel(BaseProjectView::GetDB2Label());
        db2PlotView->SetName(BaseProjectView::GetDB2Label());
        db2PlotView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_GRAPH_MENU") );
        db2PlotView->SetPrinterSettings(*wxGetApp().GetPrintData());

        auto db2Plot =
            std::make_shared<DanielsonBryan2Plot>(db2PlotView,
                std::make_shared<Colors::Schemes::ColorScheme>
                 (Colors::Schemes::ColorScheme{
                     ColorBrewer::GetColor(Colors::Color::CelestialBlue) }));

        db2Plot->SetData(scoreDataset, scoresColumnName);
        db2PlotView->SetFixedObject(0, 0, db2Plot);
        db2PlotView->CalcAllSizes(gdc);
        view->GetReadabilityResultsView().AddWindow(db2PlotView);
        }
    UpdateGraphOptions(db2PlotView);

    // if they asked to set focus to the score then select the graph
    if (setFocus)
        {
        view->UpdateSideBarIcons();
        view->GetSideBar()->SelectSubItem(
            view->GetSideBar()->FindSubItem(BaseProjectView::DB2_PAGE_ID));
        }
    }

//-------------------------------------------------------
void ProjectDoc::AddLixGermanGuage(const bool setFocus)
    {
    readability::german_lix_difficulty diffLevel;
    const size_t score = readability::german_lix(diffLevel, GetTotalWords(),
                                                 GetTotalHardLixRixWords(), GetTotalSentences() );

    const wxString scoresColumnName{ _DT(L"SCORES") };

    auto scoreDataset = std::make_shared<Wisteria::Data::Dataset>();
    scoreDataset->AddContinuousColumn(scoresColumnName);

    scoreDataset->AddRow(Data::RowInfo().
                    Continuous({ static_cast<double>(score) }));

    // Lix Gauge (German)
    ProjectView* view = dynamic_cast<ProjectView*>(GetFirstView());
    wxGCDC gdc(view->GetDocFrame());

    Wisteria::Canvas* lixGaugeView =
        dynamic_cast<Wisteria::Canvas*>(view->GetReadabilityResultsView().
            FindWindowById(BaseProjectView::LIX_GAUGE_GERMAN_PAGE_ID));
    if (lixGaugeView)
        {
        auto lixGauge = std::dynamic_pointer_cast<LixGaugeGerman>
            (lixGaugeView->GetFixedObject(0, 0));
        lixGauge->SetData(scoreDataset, scoresColumnName);
        lixGaugeView->CalcAllSizes(gdc);
        }
    else
        {
        lixGaugeView = new Wisteria::Canvas(view->GetSplitter(),
                                            BaseProjectView::LIX_GAUGE_GERMAN_PAGE_ID);
        lixGaugeView->SetFixedObjectsGridSize(1, 1);

        lixGaugeView->Hide();
        lixGaugeView->SetLabel(BaseProjectView::GetGermanLixGaugeLabel());
        lixGaugeView->SetName(BaseProjectView::GetGermanLixGaugeLabel());
        lixGaugeView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_GRAPH_MENU") );
        lixGaugeView->SetPrinterSettings(*wxGetApp().GetPrintData());

        auto lixGauge =
            std::make_shared<LixGaugeGerman>(lixGaugeView,
                std::make_shared<Colors::Schemes::ColorScheme>
                 (Colors::Schemes::ColorScheme{
                     ColorBrewer::GetColor(Colors::Color::CelestialBlue) }));

        lixGauge->SetData(scoreDataset, scoresColumnName);
        lixGaugeView->SetFixedObject(0, 0, lixGauge);
        lixGaugeView->CalcAllSizes(gdc);
        view->GetReadabilityResultsView().AddWindow(lixGaugeView);
        }
    UpdateGraphOptions(lixGaugeView);

    // in case this option changed
    std::dynamic_pointer_cast<LixGaugeGerman>(lixGaugeView->GetFixedObject(0, 0))->
        UseEnglishLabels(IsUsingEnglishLabelsForGermanLix());
    lixGaugeView->CalcAllSizes(gdc);

    // if they asked to set focus to the score then select the graph
    if (setFocus)
        {
        view->UpdateSideBarIcons();
        view->GetSideBar()->SelectSubItem(
            view->GetSideBar()->FindSubItem(BaseProjectView::LIX_GAUGE_GERMAN_PAGE_ID));
        }
    }

//-------------------------------------------------------
void ProjectDoc::AddLixGauge(const bool setFocus)
    {
    readability::lix_difficulty diffLevel;
    size_t gradeLevel{ 1 };
    const size_t score = readability::lix(diffLevel, gradeLevel, GetTotalWords(),
                                          GetTotalHardLixRixWords(), GetTotalSentences() );

    const wxString scoresColumnName{ _DT(L"SCORES") };

    auto scoreDataset = std::make_shared<Wisteria::Data::Dataset>();
    scoreDataset->AddContinuousColumn(scoresColumnName);

    scoreDataset->AddRow(Data::RowInfo().
                    Continuous({ static_cast<double>(score) }));

    // Lix Gauge
    ProjectView* view = dynamic_cast<ProjectView*>(GetFirstView());
    wxGCDC gdc(view->GetDocFrame());

    Wisteria::Canvas* lixGaugeView =
        dynamic_cast<Wisteria::Canvas*>(view->GetReadabilityResultsView().
            FindWindowById(BaseProjectView::LIX_GAUGE_PAGE_ID));
    if (lixGaugeView)
        {
        auto lixGauge = std::dynamic_pointer_cast<LixGauge>(lixGaugeView->GetFixedObject(0, 0));
        lixGauge->SetData(scoreDataset, scoresColumnName);
        lixGaugeView->CalcAllSizes(gdc);
        }
    else
        {
        lixGaugeView = new Wisteria::Canvas(view->GetSplitter(),
                                            BaseProjectView::LIX_GAUGE_PAGE_ID);
        lixGaugeView->SetFixedObjectsGridSize(1, 1);

        lixGaugeView->Hide();
        lixGaugeView->SetLabel(BaseProjectView::GetLixGaugeLabel());
        lixGaugeView->SetName(BaseProjectView::GetLixGaugeLabel());
        lixGaugeView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_GRAPH_MENU") );
        lixGaugeView->SetPrinterSettings(*wxGetApp().GetPrintData());

        auto lixGauge =
            std::make_shared<LixGauge>(lixGaugeView,
                std::make_shared<Colors::Schemes::ColorScheme>
                 (Colors::Schemes::ColorScheme{
                     ColorBrewer::GetColor(Colors::Color::CelestialBlue) }));

        lixGauge->SetData(scoreDataset, scoresColumnName);
        lixGaugeView->SetFixedObject(0, 0, lixGauge);
        lixGaugeView->CalcAllSizes(gdc);
        view->GetReadabilityResultsView().AddWindow(lixGaugeView);
        }
    UpdateGraphOptions(lixGaugeView);

    // if they asked to set focus to the score then select the graph
    if (setFocus)
        {
        view->UpdateSideBarIcons();
        view->GetSideBar()->SelectSubItem(
            view->GetSideBar()->FindSubItem(BaseProjectView::LIX_GAUGE_PAGE_ID));
        }
    }

//-------------------------------------------------------
void ProjectDoc::AddFleschChart(const bool setFocus)
    {
    const double ASL = safe_divide<double>(GetTotalWords(), GetTotalSentences());
    const double ASW = safe_divide<double>(
        (GetFleschNumeralSyllabizeMethod() == FleschNumeralSyllabize::NumeralIsOneSyllable) ?
            GetTotalSyllablesNumeralsOneSyllable() : GetTotalSyllables(),
        GetTotalWords());
    readability::flesch_difficulty diffLevel;
    const size_t score = readability::flesch_reading_ease(GetTotalWords(),
        (GetFleschNumeralSyllabizeMethod() == FleschNumeralSyllabize::NumeralIsOneSyllable) ?
            GetTotalSyllablesNumeralsOneSyllable() : GetTotalSyllables(),
        GetTotalSentences(),
        diffLevel);

    const wxString wordsColumnName{ _DT(L"WORDS") };
    const wxString scoresColumnName{ _DT(L"SCORES") };
    const wxString syllablesColumnName{ _DT(L"SYLLABLES") };

    auto scoreDataset = std::make_shared<Wisteria::Data::Dataset>();
    scoreDataset->AddContinuousColumn(wordsColumnName);
    scoreDataset->AddContinuousColumn(scoresColumnName);
    scoreDataset->AddContinuousColumn(syllablesColumnName);

    scoreDataset->AddRow(Data::RowInfo().
                    Continuous({ ASL, static_cast<double>(score), ASW }));

    // Flesch chart
    ProjectView* view = dynamic_cast<ProjectView*>(GetFirstView());
    wxGCDC gdc(view->GetDocFrame());

    Wisteria::Canvas* fleschChartCanvas =
        dynamic_cast<Wisteria::Canvas*>(view->GetReadabilityResultsView().
            FindWindowById(BaseProjectView::FLESCH_CHART_PAGE_ID));
    if (fleschChartCanvas)
        {
        auto fleschChart =
            std::dynamic_pointer_cast<FleschChart>(fleschChartCanvas->GetFixedObject(0, 0));
        fleschChart->SetData(scoreDataset, wordsColumnName, scoresColumnName, syllablesColumnName);
        fleschChartCanvas->CalcAllSizes(gdc);
        }
    else
        {
        fleschChartCanvas = new Wisteria::Canvas(view->GetSplitter(),
                                                 BaseProjectView::FLESCH_CHART_PAGE_ID);
        fleschChartCanvas->SetFixedObjectsGridSize(1, 1);

        fleschChartCanvas->Hide();
        fleschChartCanvas->SetLabel(BaseProjectView::GetFleschChartLabel());
        fleschChartCanvas->SetName(BaseProjectView::GetFleschChartLabel());
        fleschChartCanvas->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_GRAPH_MENU") );
        fleschChartCanvas->SetPrinterSettings(*wxGetApp().GetPrintData());

        auto fleschChart =
            std::make_shared<FleschChart>(fleschChartCanvas,
                std::make_shared<Colors::Schemes::ColorScheme>
                 (Colors::Schemes::ColorScheme{
                     ColorBrewer::GetColor(Colors::Color::CelestialBlue) }));
        assert(fleschChart);
        fleschChart->SetData(scoreDataset, wordsColumnName, scoresColumnName, syllablesColumnName);
        fleschChartCanvas->SetFixedObject(0, 0, fleschChart);

        fleschChartCanvas->CalcAllSizes(gdc);
        view->GetReadabilityResultsView().AddWindow(fleschChartCanvas);
        }
    UpdateGraphOptions(fleschChartCanvas);

    std::dynamic_pointer_cast<FleschChart>(fleschChartCanvas->GetFixedObject(0, 0))->
        ShowConnectionLine(IsConnectingFleschPoints());
    fleschChartCanvas->CalcAllSizes(gdc);

    // if they asked to set focus to the score then select the graph
    if (setFocus)
        {
        view->UpdateSideBarIcons();
        view->GetSideBar()->SelectSubItem(
            view->GetSideBar()->FindSubItem(BaseProjectView::FLESCH_CHART_PAGE_ID));
        }
    }

//-------------------------------------------------------
bool ProjectDoc::AddSchwartzTest(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureLanguagePackCode()) &&
        !wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureProfessionalCode()))
        { return false; }

    const wxString CURRENT_TEST_KEY = ReadabilityMessages::SCHWARTZ();

    const wxString wordsColumnName{ _DT(L"WORDS") };
    const wxString syllablesColumnName{ _DT(L"SYLLABLES") };
    const wxString sencentesColumnName{ _DT(L"SENTENCES") };

    auto scoreDataset = std::make_shared<Wisteria::Data::Dataset>();
    scoreDataset->AddContinuousColumn(wordsColumnName);
    scoreDataset->AddContinuousColumn(syllablesColumnName);
    scoreDataset->AddContinuousColumn(sencentesColumnName);

    scoreDataset->AddRow(Data::RowInfo().
                    Continuous(
                        {
                        GetTotalWords(),
                        GetTotalSyllablesNumeralsOneSyllable(),
                        GetTotalSentenceUnits()
                        }));

    if (GetTotalWords() == 0)
        {
        LogMessage(
            wxString::Format(_(L"Unable to calculate %s: at least one word "
                               "must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    if (GetTotalSentenceUnits() == 0)
        {
        LogMessage(
            wxString::Format(_(L"Unable to calculate %s: at least one sentence "
                               "must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    // Schwartz graph
    ProjectView* view = dynamic_cast<ProjectView*>(GetFirstView());
    wxGCDC gdc(view->GetDocFrame());

    Wisteria::Canvas* schwartzGraphView =
        dynamic_cast<Wisteria::Canvas*>(view->GetReadabilityResultsView().
            FindWindowById(BaseProjectView::SCHWARTZ_PAGE_ID));
    if (schwartzGraphView)
        {
        auto schwartzGraph =
            std::dynamic_pointer_cast<SchwartzGraph>(schwartzGraphView->GetFixedObject(0, 0));
        assert(schwartzGraph);
        schwartzGraph->SetData(scoreDataset, wordsColumnName, syllablesColumnName,
                               sencentesColumnName);
        schwartzGraphView->CalcAllSizes(gdc);
        }
    else
        {
        schwartzGraphView = new Wisteria::Canvas(view->GetSplitter(),
                                                 BaseProjectView::SCHWARTZ_PAGE_ID);
        schwartzGraphView->SetFixedObjectsGridSize(1, 1);

        schwartzGraphView->Hide();
        schwartzGraphView->SetLabel(BaseProjectView::GetSchwartzLabel());
        schwartzGraphView->SetName(BaseProjectView::GetSchwartzLabel());
        schwartzGraphView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_GRAPH_MENU") );
        schwartzGraphView->SetPrinterSettings(*wxGetApp().GetPrintData());

        auto schwartzGraph =
            std::make_shared<SchwartzGraph>(schwartzGraphView,
                std::make_shared<Colors::Schemes::ColorScheme>
                 (Colors::Schemes::ColorScheme{
                     ColorBrewer::GetColor(Colors::Color::CelestialBlue) }));
        schwartzGraph->SetMessageCatalog(GetReadabilityMessageCatalogPtr());
        schwartzGraph->SetData(scoreDataset, wordsColumnName, syllablesColumnName,
                               sencentesColumnName);

        schwartzGraphView->SetFixedObject(0, 0, schwartzGraph);
        schwartzGraphView->CalcAllSizes(gdc);
        view->GetReadabilityResultsView().AddWindow(schwartzGraphView);
        }
    UpdateGraphOptions(schwartzGraphView);

    auto schwartzGraph =
        std::dynamic_pointer_cast<SchwartzGraph>(schwartzGraphView->GetFixedObject(0, 0));
    assert(schwartzGraph);

    schwartzGraph->SetInvalidAreaColor(GetInvalidAreaColor());
    schwartzGraphView->CalcAllSizes(gdc);

    const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
    if (!VerifyTestBeforeAdding(theTest))
        { return false; }

    if (schwartzGraph->GetScores().front().IsScoreInvalid())
        {
        const wxString description = L"<tr><td>" +
            wxString(_(L"Text is too difficult to be plotted.")) + L"</td></tr>";

        SetReadabilityTestResult(CURRENT_TEST_KEY,
            theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(std::numeric_limits<double>::quiet_NaN(), wxString{}),
            wxString{},
            std::numeric_limits<double>::quiet_NaN(), // will be shown as an empty string
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    else
        {
        const wxString INFLUENCE_DESCRIPTION =
            schwartzGraph->GetScores().front().IsWordsHard() ?
            _(L"3+ syllable words in the text primarily influenced this score.") :
            _(L"The sentence lengths in the text primarily influenced this score.");
        const auto gradeValues = schwartzGraph->GetScores().front().GetScoreRange();

        // if a range between different grades, then it needs to be displayed differently
        wxString displayableGradeLevel, description;
        if (gradeValues.first != gradeValues.second)
            {
            displayableGradeLevel = wxString::Format(L"%zu-%zu", gradeValues.first, gradeValues.second);
            description = ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().
                GetGradeScaleDescription(gradeValues.first, gradeValues.second) + L"<br /><br />" +
                INFLUENCE_DESCRIPTION,
                theTest.first->get_test());
            }
        else
            {
            displayableGradeLevel = wxString::Format(L"%zu", gradeValues.first);
            if (gradeValues.first == 8)
                { displayableGradeLevel = wxString::Format(_(L"(Advanced) %s+"), displayableGradeLevel); }
            description = ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().
                GetGradeScaleDescription(gradeValues.first) + L"<br /><br />" +
                INFLUENCE_DESCRIPTION,
                theTest.first->get_test());
            }

        SetReadabilityTestResult(CURRENT_TEST_KEY,
            theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(
                safe_divide<double>(gradeValues.first+gradeValues.second, 2), displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(gradeValues.first, gradeValues.second,
                GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);

    // if they asked to set focus to the score then select the graph
    if (setFocus)
        {
        view->UpdateSideBarIcons();
        view->GetSideBar()->SelectSubItem(
            view->GetSideBar()->FindSubItem(BaseProjectView::SCHWARTZ_PAGE_ID));
        }

    return true;
    }

//-------------------------------------------------------
bool ProjectDoc::AddFraseTest(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureLanguagePackCode()) &&
        !wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureProfessionalCode()))
        { return false; }

    const wxString CURRENT_TEST_KEY = ReadabilityMessages::FRASE();

    const wxString wordsColumnName{ _DT(L"WORDS") };
    const wxString syllablesColumnName{ _DT(L"SYLLABLES") };
    const wxString sencentesColumnName{ _DT(L"SENTENCES") };

    auto scoreDataset = std::make_shared<Wisteria::Data::Dataset>();
    scoreDataset->AddContinuousColumn(wordsColumnName);
    scoreDataset->AddContinuousColumn(syllablesColumnName);
    scoreDataset->AddContinuousColumn(sencentesColumnName);

    scoreDataset->AddRow(Data::RowInfo().
                    Continuous(
                        {
                        GetTotalWords(),
                        GetTotalSyllables(),
                        GetTotalSentences()
                        }));

    if (GetTotalWords() == 0)
        {
        LogMessage(
            wxString::Format(_(L"Unable to calculate %s: at least one word must "
                               "be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    if (GetTotalSentences() == 0)
        {
        LogMessage(
            wxString::Format(_(L"Unable to calculate %s: at least one sentence must "
                               "be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    // FRASE graph
    ProjectView* view = dynamic_cast<ProjectView*>(GetFirstView());
    wxGCDC gdc(view->GetDocFrame());

    Wisteria::Canvas* fraseGraphView =
        dynamic_cast<Wisteria::Canvas*>(view->GetReadabilityResultsView().
            FindWindowById(BaseProjectView::FRASE_PAGE_ID));
    if (fraseGraphView)
        {
        auto fraseGraph =
            std::dynamic_pointer_cast<FraseGraph>(fraseGraphView->GetFixedObject(0, 0));
        assert(fraseGraph);
        fraseGraph->SetData(scoreDataset, wordsColumnName, syllablesColumnName,
                            sencentesColumnName);
        }
    else
        {
        fraseGraphView = new Wisteria::Canvas(view->GetSplitter(),
                                              BaseProjectView::FRASE_PAGE_ID);

        fraseGraphView->SetFixedObjectsGridSize(1, 1);

        fraseGraphView->Hide();
        fraseGraphView->SetLabel(BaseProjectView::GetFraseLabel());
        fraseGraphView->SetName(BaseProjectView::GetFraseLabel());
        fraseGraphView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_GRAPH_MENU") );
        fraseGraphView->SetPrinterSettings(*wxGetApp().GetPrintData());

        auto fraseGraph = std::make_shared<FraseGraph>(fraseGraphView,
            std::make_shared<Colors::Schemes::ColorScheme>
                 (Colors::Schemes::ColorScheme{
                     ColorBrewer::GetColor(Colors::Color::CelestialBlue) }));
        fraseGraph->SetMessageCatalog(GetReadabilityMessageCatalogPtr());
        fraseGraph->SetData(scoreDataset, wordsColumnName, syllablesColumnName,
                            sencentesColumnName);

        fraseGraphView->SetFixedObject(0, 0, fraseGraph);
        view->GetReadabilityResultsView().AddWindow(fraseGraphView);
        }
    UpdateGraphOptions(fraseGraphView);

    fraseGraphView->CalcAllSizes(gdc);

    const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
    if (!VerifyTestBeforeAdding(theTest))
        { return false; }

    auto fraseGraph = std::dynamic_pointer_cast<FraseGraph>(fraseGraphView->GetFixedObject(0, 0));
    assert(fraseGraph);
    if (fraseGraph->GetScores().front().IsScoreInvalid())
        {
        const wxString description = L"<tr><td>" +
            wxString(_(L"Text is too difficult to be plotted.")) + L"</td></tr>";

        SetReadabilityTestResult(CURRENT_TEST_KEY,
            theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(std::numeric_limits<double>::quiet_NaN(), wxString{}),
            wxString{},
            std::numeric_limits<double>::quiet_NaN()/*Will be shown as an empty string*/,
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    else
        {
        const wxString INFLUENCE_DESCRIPTION =
            fraseGraph->GetScores().front().IsWordsHard() ?
            _(L"3+ syllable words in the text primarily influenced this score.") :
            _(L"The sentence lengths in the text primarily influenced this score.");
        const size_t val = fraseGraph->GetScores().front().GetScore();

        const wxString description = ProjectReportFormat::FormatTestResult(
                ReadabilityMessages::GetFraseDescription(val) + L"<br /><br />" +
                INFLUENCE_DESCRIPTION,
                theTest.first->get_test());

        SetReadabilityTestResult(CURRENT_TEST_KEY,
            theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(std::numeric_limits<double>::quiet_NaN(), wxString{}),
            wxString{}, val,
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);

    // if they asked to set focus to the score then select the graph
    if (setFocus)
        {
        view->UpdateSideBarIcons();
        view->GetSideBar()->SelectSubItem(
            view->GetSideBar()->FindSubItem(BaseProjectView::FRASE_PAGE_ID));
        }

    return true;
    }

//-------------------------------------------------------
void ProjectDoc::DisplayReadabilityGraphs()
    {
    try
        {
        ProjectView* view = dynamic_cast<ProjectView*>(GetFirstView());
        wxGCDC gdc(view->GetDocFrame());

        // remove Fry graph if test is not included (Note that this chart is added by AddFryTest, not here).
        if (!GetReadabilityTests().is_test_included(ReadabilityMessages::FRY()))
            { view->GetReadabilityResultsView().RemoveWindowById(BaseProjectView::FRY_PAGE_ID); }
        // remove GPM (Spanish) Fry graph if test is not included
        // (Note that this chart is added by AddGilliamPenaMountainFryTest, not here).
        if (!GetReadabilityTests().is_test_included(ReadabilityMessages::GPM_FRY()))
            { view->GetReadabilityResultsView().RemoveWindowById(BaseProjectView::GPM_FRY_PAGE_ID); }
        // remove FRASE graph if test is not included (Note that this chart is added by AddFraseTest, not here).
        if (!GetReadabilityTests().is_test_included(ReadabilityMessages::FRASE()))
            { view->GetReadabilityResultsView().RemoveWindowById(BaseProjectView::FRASE_PAGE_ID); }
        // remove Schwartz graph if test is not included (Note that this chart is added by AddSchwartzTest, not here).
        if (!GetReadabilityTests().is_test_included(ReadabilityMessages::SCHWARTZ()))
            { view->GetReadabilityResultsView().RemoveWindowById(BaseProjectView::SCHWARTZ_PAGE_ID); }
        // remove Flesch Chart graph if test is not included (Note that this chart is added by AddFleschTest, not here).
        if (!GetReadabilityTests().is_test_included(ReadabilityMessages::FLESCH()))
            { view->GetReadabilityResultsView().RemoveWindowById(BaseProjectView::FLESCH_CHART_PAGE_ID); }
        // remove DB2 graph if test is not included (Note that this chart is added by AddDB2, not here).
        if (!GetReadabilityTests().is_test_included(ReadabilityMessages::DANIELSON_BRYAN_2()))
            { view->GetReadabilityResultsView().RemoveWindowById(BaseProjectView::DB2_PAGE_ID); }
        // remove Lix Gauge if test is not included (Note that this chart is added by AddLixTest, not here).
        if (!GetReadabilityTests().is_test_included(ReadabilityMessages::LIX()))
            { view->GetReadabilityResultsView().RemoveWindowById(BaseProjectView::LIX_GAUGE_PAGE_ID); }
        // remove German Lix Gauge if neither German Lix test is included
        // (Note that this chart is added by AddLixGermanXXX, not here).
        if (!GetReadabilityTests().is_test_included(ReadabilityMessages::LIX_GERMAN_CHILDRENS_LITERATURE()) &&
            !GetReadabilityTests().is_test_included(ReadabilityMessages::LIX_GERMAN_TECHNICAL()))
            { view->GetReadabilityResultsView().RemoveWindowById(BaseProjectView::LIX_GAUGE_GERMAN_PAGE_ID); }
        // remove Crawford graph if test is not included (Note that this chart is added by AddCrawfordTest, not here).
        if (!GetReadabilityTests().is_test_included(ReadabilityMessages::CRAWFORD()))
            { view->GetReadabilityResultsView().RemoveWindowById(BaseProjectView::CRAWFORD_GRAPH_PAGE_ID); }
        // remove Raygor graph if test is not included (Note that this chart is added by AddRaygorTest, not here).
        if (!GetReadabilityTests().is_test_included(ReadabilityMessages::RAYGOR()))
            { view->GetReadabilityResultsView().RemoveWindowById(BaseProjectView::RAYGOR_PAGE_ID); }
        // need to sort the list of scores after adding the graph tests to it
        view->GetReadabilityScoresList()->GetResultsListCtrl()->SortColumn(0, Wisteria::SortDirection::SortAscending);

        // Dolch graphs
        if (IsIncludingDolchSightWords())
            {
            Wisteria::Canvas* coverageBarChartCanvas =
                dynamic_cast<Wisteria::Canvas*>(view->GetDolchSightWordsView().FindWindowById(
                    BaseProjectView::DOLCH_COVERAGE_CHART_PAGE_ID));
            if (!coverageBarChartCanvas)
                {
                coverageBarChartCanvas = new Wisteria::Canvas(view->GetSplitter(),
                    BaseProjectView::DOLCH_COVERAGE_CHART_PAGE_ID);
                coverageBarChartCanvas->SetFixedObjectsGridSize(1, 1);
                coverageBarChartCanvas->SetFixedObject(0, 0, std::make_shared<BarChart>(coverageBarChartCanvas));
                coverageBarChartCanvas->Hide();
                coverageBarChartCanvas->SetLabel(BaseProjectView::GetCoverageChartTabLabel());
                coverageBarChartCanvas->SetName(BaseProjectView::GetCoverageChartTabLabel());
                coverageBarChartCanvas->GetTopTitles().push_back(
                    GraphItems::Label(
                        GraphItemInfo(_(L"Dolch Word Coverage (%)")).
                        Scaling(coverageBarChartCanvas->GetScaling()).Pen(wxNullPen)));
                coverageBarChartCanvas->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_GRAPH_MENU") );
                coverageBarChartCanvas->SetPrinterSettings(*wxGetApp().GetPrintData());
                std::dynamic_pointer_cast<BarChart>(coverageBarChartCanvas->GetFixedObject(0, 0))->SetSortable(true);
                view->GetDolchSightWordsView().AddWindow(coverageBarChartCanvas);
                }
            UpdateGraphOptions(coverageBarChartCanvas);

            auto coverageBarChart = std::dynamic_pointer_cast<BarChart>(coverageBarChartCanvas->GetFixedObject(0, 0));
            assert(coverageBarChart);

            coverageBarChart->ClearBars();
            coverageBarChart->SetBarOrientation(static_cast<Wisteria::Orientation>(GetBarChartOrientation()));
            coverageBarChart->GetBarAxis().SetLabelDisplay(Wisteria::AxisLabelDisplay::DisplayOnlyCustomLabels);
            coverageBarChart->GetScalingAxis().SetLabelDisplay(Wisteria::AxisLabelDisplay::DisplayCustomLabelsOrValues);
            coverageBarChart->GetScalingAxis().SetRange(0, 100, 0, 10, 2);
            coverageBarChart->SetShadowType(IsDisplayingDropShadows() ?
                ShadowType::RightSideShadow : ShadowType::NoShadow);
            coverageBarChart->IncludeSpacesBetweenBars(true);
            coverageBarChart->GetRightYAxis().Show(false);
            coverageBarChart->GetTopXAxis().Show(false);
            coverageBarChart->GetScalingAxis().GetGridlinePen() = wxNullPen;

            size_t currentBar = 0;
            const double dolchNounPercentage =
                safe_divide<double>((ProjectReportFormat::MAX_DOLCH_NOUNS-GetUnusedDolchNouns()),
                    ProjectReportFormat::MAX_DOLCH_NOUNS) * 100;
            coverageBarChart->AddBar(BarChart::Bar(++currentBar,
                {
                    { BarChart::BarBlock(BarChart::BarBlockInfo(dolchNounPercentage).Brush(GetDolchNounColor())) }
                },
                wxNumberFormatter::ToString(dolchNounPercentage, 1,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) +
                L"%", GraphItems::Label(_(L"Nouns")),
                GetGraphBarEffect(), GetGraphBarOpacity()), false);

            const double dolchVerbsPercentage =
                safe_divide<double>((ProjectReportFormat::MAX_DOLCH_VERBS - GetUnusedDolchVerbs()),
                    ProjectReportFormat::MAX_DOLCH_VERBS)*100;
            coverageBarChart->AddBar(BarChart::Bar(++currentBar,
                {
                    { BarChart::BarBlock(BarChart::BarBlockInfo(dolchVerbsPercentage).Brush(GetDolchVerbsColor())) }
                },
                wxNumberFormatter::ToString(dolchVerbsPercentage, 1,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) +
                L"%", GraphItems::Label(_(L"Verbs")),
                GetGraphBarEffect(), GetGraphBarOpacity()), false);

            const double dolchAdjectivesPercentage =
                safe_divide<double>((ProjectReportFormat::MAX_DOLCH_ADJECTIVE_WORDS - GetUnusedDolchAdjectives()),
                    ProjectReportFormat::MAX_DOLCH_ADJECTIVE_WORDS)*100;
            coverageBarChart->AddBar(BarChart::Bar(++currentBar,
                { { BarChart::BarBlock(BarChart::BarBlockInfo(dolchAdjectivesPercentage).
                    Brush(GetDolchAdjectivesColor())) } },
                wxNumberFormatter::ToString(dolchAdjectivesPercentage, 1,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) +
                L"%", GraphItems::Label(_(L"Adjectives")),
                GetGraphBarEffect(), GetGraphBarOpacity()), false);

            const double dolchAdverbsPercentage =
                safe_divide<double>((ProjectReportFormat::MAX_DOLCH_ADVERB_WORDS-GetUnusedDolchAdverbs()),
                    ProjectReportFormat::MAX_DOLCH_ADVERB_WORDS)*100;
            coverageBarChart->AddBar(BarChart::Bar(++currentBar,
                {
                    { BarChart::BarBlock(
                        BarChart::BarBlockInfo(dolchAdverbsPercentage).Brush(GetDolchAdverbsColor())) }
                },
                wxNumberFormatter::ToString(dolchAdverbsPercentage, 1,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) +
                L"%", GraphItems::Label(_(L"Adverbs")),
                GetGraphBarEffect(), GetGraphBarOpacity()), false);

            const double dolchPronounsPercentage =
                safe_divide<double>((ProjectReportFormat::MAX_DOLCH_PRONOUN_WORDS-GetUnusedDolchPronouns()),
                    ProjectReportFormat::MAX_DOLCH_PRONOUN_WORDS)*100;
            coverageBarChart->AddBar(BarChart::Bar(++currentBar,
                {
                    { BarChart::BarBlock(
                        BarChart::BarBlockInfo(dolchPronounsPercentage).Brush(GetDolchPronounsColor())) }
                },
                wxNumberFormatter::ToString(dolchPronounsPercentage, 1,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|wxNumberFormatter::Style::Style_WithThousandsSep) +
                L"%", GraphItems::Label(_(L"Pronouns")),
                GetGraphBarEffect(), GetGraphBarOpacity()), false);

            const double dolchPrepositionsPercentage =
                safe_divide<double>(
                    (ProjectReportFormat::MAX_DOLCH_PREPOSITION_WORDS-GetUnusedDolchPrepositions()),
                     ProjectReportFormat::MAX_DOLCH_PREPOSITION_WORDS)*100;
            coverageBarChart->AddBar(BarChart::Bar(++currentBar,
                {
                    { BarChart::BarBlock(
                        BarChart::BarBlockInfo(dolchPrepositionsPercentage).Brush(GetDolchPrepositionsColor())) }
                },
                wxNumberFormatter::ToString(dolchPrepositionsPercentage, 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) + L"%", GraphItems::Label(_(L"Prepositions")),
                GetGraphBarEffect(), GetGraphBarOpacity()), false);

            const double dolchConjunctionsPercentage =
                safe_divide<double>(
                    (ProjectReportFormat::MAX_DOLCH_CONJUNCTION_WORDS-GetUnusedDolchConjunctions()),
                     ProjectReportFormat::MAX_DOLCH_CONJUNCTION_WORDS)*100;
            coverageBarChart->AddBar(BarChart::Bar(++currentBar,
                {
                    { BarChart::BarBlock(
                        BarChart::BarBlockInfo(dolchConjunctionsPercentage).Brush(GetDolchConjunctionsColor())) }
                },
                wxNumberFormatter::ToString(dolchConjunctionsPercentage, 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) +
                L"%", GraphItems::Label(_(L"Conjunctions")),
                GetGraphBarEffect(), GetGraphBarOpacity()), false);

            // update the bar labels
            coverageBarChart->SetBinLabelDisplay(IsDisplayingBarLabels() ?
                BinLabelDisplay::BinValue : BinLabelDisplay::NoDisplay);

            coverageBarChart->SortBars(
                BarChart::BarSortComparison::SortByBarLength, coverageBarChart->GetSortDirection());

            coverageBarChartCanvas->CalcAllSizes(gdc);

            // Dolch words breakdown chart
            const size_t totalDolchWords = m_dolchConjunctionCounts.second +
                m_dolchPrepositionCounts.second + m_dolchPronounCounts.second +
                m_dolchAdverbCounts.second + m_dolchAdjectiveCounts.second +
                m_dolchVerbCounts.second + m_dolchNounCounts.second;

            Wisteria::Canvas* wordBarChartCanvas =
                dynamic_cast<Wisteria::Canvas*>(view->GetDolchSightWordsView().FindWindowById(
                    BaseProjectView::DOLCH_BREAKDOWN_PAGE_ID));
            if (!wordBarChartCanvas)
                {
                wordBarChartCanvas =
                    new Wisteria::Canvas(view->GetSplitter(), BaseProjectView::DOLCH_BREAKDOWN_PAGE_ID);
                wordBarChartCanvas->SetFixedObjectsGridSize(1, 1);
                wordBarChartCanvas->SetFixedObject(0, 0, std::make_shared<BarChart>(wordBarChartCanvas));
                wordBarChartCanvas->Hide();
                wordBarChartCanvas->SetLabel(BaseProjectView::GetWordCountsLabel());
                wordBarChartCanvas->SetName(BaseProjectView::GetWordCountsLabel());
                wordBarChartCanvas->GetTopTitles().push_back(
                    GraphItems::Label(
                        GraphItemInfo(_(L"Dolch Word Breakdown")).
                        Scaling(wordBarChartCanvas->GetScaling()).Pen(wxNullPen)));
                wordBarChartCanvas->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_GRAPH_MENU") );
                wordBarChartCanvas->SetPrinterSettings(*wxGetApp().GetPrintData());
                std::dynamic_pointer_cast<BarChart>(wordBarChartCanvas->GetFixedObject(0, 0))->SetSortable(true);
                view->GetDolchSightWordsView().AddWindow(wordBarChartCanvas);
                }
            UpdateGraphOptions(wordBarChartCanvas);
            auto dolchBarChart = std::dynamic_pointer_cast<BarChart>(wordBarChartCanvas->GetFixedObject(0, 0));
            assert(dolchBarChart);

            dolchBarChart->ClearBars();
            dolchBarChart->SetBarOrientation(static_cast<Wisteria::Orientation>(GetBarChartOrientation()));
            dolchBarChart->GetBarAxis().SetLabelDisplay(Wisteria::AxisLabelDisplay::DisplayOnlyCustomLabels);
            dolchBarChart->GetScalingAxis().SetLabelDisplay(Wisteria::AxisLabelDisplay::DisplayCustomLabelsOrValues);
            dolchBarChart->SetShadowType(IsDisplayingDropShadows() ?
                ShadowType::RightSideShadow : ShadowType::NoShadow);
            dolchBarChart->IncludeSpacesBetweenBars(true);
            dolchBarChart->SetBinLabelDisplay(IsDisplayingBarLabels() ?
                BinLabelDisplay::BinValue : BinLabelDisplay::NoDisplay);
            dolchBarChart->GetRightYAxis().Show(false);
            dolchBarChart->GetTopXAxis().Show(false);
            dolchBarChart->GetScalingAxis().GetGridlinePen() = wxNullPen;

            currentBar = 0;

            dolchBarChart->AddBar(BarChart::Bar(++currentBar,
                {
                    { BarChart::BarBlock(
                        BarChart::BarBlockInfo(static_cast<double>(GetDolchNounCounts().second)).
                        Brush(GetDolchNounColor())) }
                },
                wxNumberFormatter::ToString(GetDolchNounCounts().second, 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep), GraphItems::Label(_(L"Nouns")),
                GetGraphBarEffect(), GetGraphBarOpacity()) );

            dolchBarChart->AddBar(BarChart::Bar(++currentBar,
                {
                    { BarChart::BarBlock(
                        BarChart::BarBlockInfo(static_cast<double>(GetDolchVerbsCounts().second)).
                        Brush(GetDolchVerbsColor())) }
                },
                wxNumberFormatter::ToString(GetDolchVerbsCounts().second, 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep), GraphItems::Label(_(L"Verbs")),
                GetGraphBarEffect(), GetGraphBarOpacity()) );

            dolchBarChart->AddBar(BarChart::Bar(++currentBar,
                {
                    { BarChart::BarBlock(
                       BarChart::BarBlockInfo(static_cast<double>(GetDolchAdjectiveCounts().second)).
                       Brush(GetDolchAdjectivesColor())) }
                },
                wxNumberFormatter::ToString(GetDolchAdjectiveCounts().second, 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep), GraphItems::Label(_(L"Adjectives")),
                GetGraphBarEffect(), GetGraphBarOpacity()) );

            dolchBarChart->AddBar(BarChart::Bar(++currentBar,
                {
                    { BarChart::BarBlock(
                       BarChart::BarBlockInfo(static_cast<double>(GetDolchAdverbCounts().second)).
                       Brush(GetDolchAdverbsColor())) }
                },
                wxNumberFormatter::ToString(GetDolchAdverbCounts().second, 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep), GraphItems::Label(_(L"Adverbs")),
                GetGraphBarEffect(), GetGraphBarOpacity()) );

            dolchBarChart->AddBar(BarChart::Bar(++currentBar,
                {
                    { BarChart::BarBlock(
                       BarChart::BarBlockInfo(static_cast<double>(GetDolchPronounCounts().second)).
                       Brush(GetDolchPronounsColor())) }
                },
                wxNumberFormatter::ToString(GetDolchPronounCounts().second, 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep), GraphItems::Label(_(L"Pronouns")),
                GetGraphBarEffect(), GetGraphBarOpacity()) );

            dolchBarChart->AddBar(BarChart::Bar(++currentBar,
                {
                    { BarChart::BarBlock(
                        BarChart::BarBlockInfo(static_cast<double>(GetDolchPrepositionWordCounts().second)).
                            Brush(GetDolchPrepositionsColor())) }
                },
                wxNumberFormatter::ToString(GetDolchPrepositionWordCounts().second, 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep), GraphItems::Label(_(L"Prepositions")),
                GetGraphBarEffect(), GetGraphBarOpacity()) );

            dolchBarChart->AddBar(BarChart::Bar(++currentBar,
                {
                    { BarChart::BarBlock(
                        BarChart::BarBlockInfo(static_cast<double>(GetDolchConjunctionCounts().second)).
                        Brush(GetDolchConjunctionsColor())) }
                },
                wxNumberFormatter::ToString(GetDolchConjunctionCounts().second, 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep), GraphItems::Label(_(L"Conjunctions")),
                GetGraphBarEffect(), GetGraphBarOpacity()) );

            dolchBarChart->AddBar(BarChart::Bar(++currentBar,
                {
                    { BarChart::BarBlock(
                        BarChart::BarBlockInfo(static_cast<double>(GetTotalWords() - totalDolchWords)).
                        Brush(GetBarChartBarColor())) }
                },
                wxNumberFormatter::ToString((GetTotalWords()-totalDolchWords), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep), GraphItems::Label(_(L"Non-Dolch Words")),
                GetGraphBarEffect(), GetGraphBarOpacity()) );

            dolchBarChart->AddBar(BarChart::Bar(++currentBar,
                {
                    { BarChart::BarBlock(
                        BarChart::BarBlockInfo(static_cast<double>(totalDolchWords)).
                            Brush(GetBarChartBarColor())) }
                },
                wxNumberFormatter::ToString(totalDolchWords, 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep), GraphItems::Label(_(L"Total Dolch Words")),
                GetGraphBarEffect(), GetGraphBarOpacity()) );

            // all the words
            dolchBarChart->AddBar(BarChart::Bar(++currentBar,
                {
                    { BarChart::BarBlock(
                        BarChart::BarBlockInfo(static_cast<double>(GetTotalWords())).
                        Brush(GetBarChartBarColor())) }
                },
                wxNumberFormatter::ToString(GetTotalWords(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep), GraphItems::Label(_(L"Total Words")),
                GetGraphBarEffect(), GetGraphBarOpacity()) );

            // update the bar labels
            dolchBarChart->SetBinLabelDisplay(IsDisplayingBarLabels() ?
                BinLabelDisplay::BinValue : BinLabelDisplay::NoDisplay);

            dolchBarChart->SortBars(BarChart::BarSortComparison::SortByBarLength, dolchBarChart->GetSortDirection());

            wordBarChartCanvas->CalcAllSizes(gdc);
            }
        else
            {
            view->GetDolchSightWordsView().RemoveWindowById(BaseProjectView::DOLCH_COVERAGE_CHART_PAGE_ID);
            view->GetDolchSightWordsView().RemoveWindowById(BaseProjectView::DOLCH_BREAKDOWN_PAGE_ID);
            }
        }
    catch (...)
        {
        LogMessage(_(L"An unknown error occurred. Unable to create graphs."),
            _(L"Error"), wxOK|wxICON_EXCLAMATION);
        }
    }

//-------------------------------------------------------
void ProjectDoc::DisplaySentencesBreakdown()
    {
    DisplayOverlyLongSentences();
    DisplaySentenceCharts();
    }

/// load the general stats view
//-------------------------------------------------------
void ProjectDoc::DisplayStatistics()
    {
    ProjectView* view = dynamic_cast<ProjectView*>(GetFirstView());
    assert(view);

    if (GetStatisticsInfo().IsReportEnabled())
        {
        HtmlTableWindow* summaryReportWindow =
            dynamic_cast<HtmlTableWindow*>(view->GetSummaryView().FindWindowById(
                BaseProjectView::STATS_REPORT_PAGE_ID));
        if (summaryReportWindow == nullptr)
            {
            summaryReportWindow = new HtmlTableWindow(view->GetSplitter(), BaseProjectView::STATS_REPORT_PAGE_ID);
            summaryReportWindow->Hide();
            summaryReportWindow->SetLabel(BaseProjectView::GetFormattedReportLabel());
            summaryReportWindow->SetName(BaseProjectView::GetFormattedReportLabel());
            summaryReportWindow->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_HTML_MENU") );
            summaryReportWindow->SetPrinterSettings(wxGetApp().GetPrintData());
            summaryReportWindow->SetLeftPrinterHeader(wxGetApp().GetAppOptions().GetLeftPrinterHeader());
            summaryReportWindow->SetCenterPrinterHeader(wxGetApp().GetAppOptions().GetCenterPrinterHeader());
            summaryReportWindow->SetRightPrinterHeader(wxGetApp().GetAppOptions().GetRightPrinterHeader());
            summaryReportWindow->SetLeftPrinterFooter(wxGetApp().GetAppOptions().GetLeftPrinterFooter());
            summaryReportWindow->SetCenterPrinterFooter(wxGetApp().GetAppOptions().GetCenterPrinterFooter());
            summaryReportWindow->SetRightPrinterFooter(wxGetApp().GetAppOptions().GetRightPrinterFooter());
            }

        wxString formattedStats = ProjectReportFormat::FormatHtmlReportStart(
                wxGetApp().GetAppOptions().GetControlBackgroundColor(),
                ((wxGetApp().GetAppOptions().GetControlBackgroundColor().
                    GetLuminance() < .5f) ? *wxWHITE : *wxBLACK)) +
            ProjectReportFormat::FormatStatisticsInfo(this,
                GetStatisticsReportInfo(),
                wxSystemSettings::GetColour(wxSYS_COLOUR_HOTLIGHT),
                view->GetSummaryStatisticsListData()) +
            ProjectReportFormat::FormatHtmlReportEnd();
        // if document failed to be loaded and we are just showing the basic stats,
        // then remove the links to the various windows that won't be shown
        if (!LoadingOriginalTextSucceeded())
            {
            std::wstring strippedStatsText{ formattedStats };
            lily_of_the_valley::html_format::strip_hyperlinks(strippedStatsText);
            formattedStats = strippedStatsText;
            }

        summaryReportWindow->SetPage(formattedStats);
        view->GetSummaryView().InsertWindow(0, summaryReportWindow);
        }
    else
        { view->GetSummaryView().RemoveWindowById(BaseProjectView::STATS_REPORT_PAGE_ID); }

    if (GetStatisticsInfo().IsTableEnabled())
        {
        ListCtrlEx* tabularStatsList = dynamic_cast<ListCtrlEx*>(view->GetSummaryView().FindWindowById(
            BaseProjectView::STATS_LIST_PAGE_ID));
        if (tabularStatsList == nullptr)
            {
            tabularStatsList = new ListCtrlEx(view->GetSplitter(), BaseProjectView::STATS_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            tabularStatsList->Hide();
            tabularStatsList->SetLabel(BaseProjectView::GetTabularReportLabel());
            tabularStatsList->SetName(BaseProjectView::GetTabularReportLabel());
            tabularStatsList->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            UpdateListOptions(tabularStatsList);
            tabularStatsList->EnableGridLines();
            tabularStatsList->EnableItemViewOnDblClick();
            tabularStatsList->InsertColumn(0, _(L"Statistic"));
            tabularStatsList->InsertColumn(1, _(L"Value"));
            tabularStatsList->InsertColumn(2, _(L"Percentage"));
            tabularStatsList->SetVirtualDataProvider(view->GetSummaryStatisticsListData());
            }
        tabularStatsList->SetVirtualDataSize(view->GetSummaryStatisticsListData()->GetItemCount(),
            view->GetSummaryStatisticsListData()->GetColumnCount());
        tabularStatsList->SetColumnWidth(0, tabularStatsList->EstimateColumnWidth(0));
        tabularStatsList->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER);
        tabularStatsList->SetColumnWidth(2, tabularStatsList->EstimateColumnWidth(2));
        tabularStatsList->Resort();
        tabularStatsList->DistributeColumns();
        const auto buddyWindowPosition = view->GetSummaryView().FindWindowPositionById(
            BaseProjectView::STATS_REPORT_PAGE_ID);
        view->GetSummaryView().InsertWindow((buddyWindowPosition != wxNOT_FOUND) ?
            buddyWindowPosition+1 : 0, tabularStatsList);
        }
    else
        {
        // we are getting rid of this window (if it was included before)
        view->GetSummaryView().RemoveWindowById(BaseProjectView::STATS_LIST_PAGE_ID);
        }

    if (IsIncludingDolchSightWords())
        {
        HtmlTableWindow* sumWindow =
            dynamic_cast<HtmlTableWindow*>(view->GetDolchSightWordsView().FindWindowById(
                BaseProjectView::DOLCH_STATS_PAGE_ID));
        if (!sumWindow)
            {
            sumWindow = new HtmlTableWindow(view->GetSplitter(), BaseProjectView::DOLCH_STATS_PAGE_ID);
            sumWindow->Hide();
            sumWindow->SetLabel(_(L"Summary"));
            sumWindow->SetName(_(L"Dolch Summary"));
            sumWindow->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_HTML_MENU") );
            sumWindow->SetPrinterSettings(wxGetApp().GetPrintData());
            sumWindow->SetLeftPrinterHeader(wxGetApp().GetAppOptions().GetLeftPrinterHeader());
            sumWindow->SetCenterPrinterHeader(wxGetApp().GetAppOptions().GetCenterPrinterHeader());
            sumWindow->SetRightPrinterHeader(wxGetApp().GetAppOptions().GetRightPrinterHeader());
            sumWindow->SetLeftPrinterFooter(wxGetApp().GetAppOptions().GetLeftPrinterFooter());
            sumWindow->SetCenterPrinterFooter(wxGetApp().GetAppOptions().GetCenterPrinterFooter());
            sumWindow->SetRightPrinterFooter(wxGetApp().GetAppOptions().GetRightPrinterFooter());

            view->GetDolchSightWordsView().AddWindow(sumWindow);
            }
        assert(sumWindow);
        sumWindow->SetPage(
            ProjectReportFormat::FormatHtmlReportStart(
                wxGetApp().GetAppOptions().GetControlBackgroundColor(),
                ColorContrast::BlackOrWhiteContrast(wxGetApp().GetAppOptions().GetControlBackgroundColor()),
                _(L"Dolch Summary")) +
                ProjectReportFormat::FormatDolchStatisticsInfo(this,
                    GetStatisticsReportInfo(), true,
                    wxSystemSettings::GetColour(wxSYS_COLOUR_HOTLIGHT), nullptr) +
            ProjectReportFormat::FormatHtmlReportEnd());
        }
    else
        { view->GetDolchSightWordsView().RemoveWindowById(BaseProjectView::DOLCH_STATS_PAGE_ID); }
    }

//-------------------------------------------------------
bool ProjectDoc::AddGilliamPenaMountainFryTest(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureLanguagePackCode()) &&
        !wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureProfessionalCode()))
        { return false; }

    const wxString CURRENT_TEST_KEY = ReadabilityMessages::GPM_FRY();

     const wxString wordsColumnName{ _DT(L"WORDS") };
    const wxString syllablesColumnName{ _DT(L"SYLLABLES") };
    const wxString sencentesColumnName{ _DT(L"SENTENCES") };

    auto scoreDataset = std::make_shared<Wisteria::Data::Dataset>();
    scoreDataset->AddContinuousColumn(wordsColumnName);
    scoreDataset->AddContinuousColumn(syllablesColumnName);
    scoreDataset->AddContinuousColumn(sencentesColumnName);

    scoreDataset->AddRow(Data::RowInfo().
                    Continuous(
                        {
                        GetTotalWords(),
                        GetTotalSyllablesNumeralsFullySyllabized(),
                        GetTotalSentences()
                        }));

    if (GetTotalWords() == 0)
        {
        LogMessage(wxString::Format(
            _(L"Unable to calculate %s: at least one word must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    if (GetTotalSentences() == 0)
        {
        LogMessage(wxString::Format(
            _(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    // GPM (Fry) graph
    ProjectView* view = dynamic_cast<ProjectView*>(GetFirstView());
    wxGCDC gdc(view->GetDocFrame());

    Wisteria::Canvas* fryGraphView =
        dynamic_cast<Wisteria::Canvas*>(view->GetReadabilityResultsView().
            FindWindowById(BaseProjectView::GPM_FRY_PAGE_ID));
    if (fryGraphView)
        {
        auto gFryGraph =
            std::dynamic_pointer_cast<FryGraph>(fryGraphView->GetFixedObject(0, 0));
        assert(gFryGraph);
        gFryGraph->SetData(scoreDataset, wordsColumnName, syllablesColumnName,
                            sencentesColumnName);
        fryGraphView->CalcAllSizes(gdc);
        }
    else
        {
        fryGraphView = new Wisteria::Canvas(view->GetSplitter(),
                                            BaseProjectView::GPM_FRY_PAGE_ID);

        fryGraphView->SetFixedObjectsGridSize(1, 1);
        fryGraphView->Hide();
        fryGraphView->SetLabel(BaseProjectView::GetGilliamPenanMountainFryLabel());
        fryGraphView->SetName(BaseProjectView::GetGilliamPenanMountainFryLabel());
        fryGraphView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_GRAPH_MENU") );
        fryGraphView->SetPrinterSettings(*wxGetApp().GetPrintData());

        auto gFryGraph = std::make_shared<FryGraph>(fryGraphView,
            FryGraph::FryGraphType::GPM,
            std::make_shared<Colors::Schemes::ColorScheme>
                 (Colors::Schemes::ColorScheme{
                     ColorBrewer::GetColor(Colors::Color::CelestialBlue) }));
        gFryGraph->SetMessageCatalog(GetReadabilityMessageCatalogPtr());
        gFryGraph->SetData(scoreDataset, wordsColumnName, syllablesColumnName,
                           sencentesColumnName);

        fryGraphView->SetFixedObject(0, 0, gFryGraph);

        view->GetReadabilityResultsView().AddWindow(fryGraphView);
        }
    UpdateGraphOptions(fryGraphView);

    auto gFryGraph =
        std::dynamic_pointer_cast<FryGraph>(fryGraphView->GetFixedObject(0, 0));
    gFryGraph->SetInvalidAreaColor(GetInvalidAreaColor());
    fryGraphView->CalcAllSizes(gdc);

    const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
    if (!VerifyTestBeforeAdding(theTest))
        { return false; }

    if (gFryGraph->GetScores().front().IsScoreInvalid())
        {
        const wxString description = L"<tr><td>" +
            _(L"Text is too difficult to be plotted.") +
            L"</td></tr>";

        SetReadabilityTestResult(CURRENT_TEST_KEY,
            theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(
                std::numeric_limits<double>::quiet_NaN(), BaseProjectView::GetFailedLabel()),
            BaseProjectView::GetFailedLabel(),
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    else if (gFryGraph->GetScores().front().IsScoreOutOfGradeRange())
        {
        wxString TOO_DIFFICULT_DESCRIPTION =
            _(L"Text is too difficult to be classified to a specific grade level because "
              "it contains too many ");
        TOO_DIFFICULT_DESCRIPTION += gFryGraph->GetScores().front().IsWordsHard() ?
                                    _(L"high syllable words.") :
                                    _(L"long sentences.");
        const wxString description = L"<tr><td>" + TOO_DIFFICULT_DESCRIPTION + L"</td></tr>";

        SetReadabilityTestResult(CURRENT_TEST_KEY,
            theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(
                std::numeric_limits<double>::quiet_NaN(), BaseProjectView::GetFailedLabel()),
            BaseProjectView::GetFailedLabel(),
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    else
        {
        const wxString INFLUENCE_DESCRIPTION =
            gFryGraph->GetScores().front().IsWordsHard() ?
            _(L"3+ syllable words in the text primarily influenced this grade level score.") :
            _(L"The sentence lengths in the text primarily influenced this grade level score.");
        const size_t gradeValue = gFryGraph->GetScores().front().GetScore();

        const wxString description = ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue) +
                L"<br /><br />" +
                INFLUENCE_DESCRIPTION,
                theTest.first->get_test(),
            ((GetTotalNumerals() > 0) ?
                ReadabilityMessages::GetNumeralAreFullySyllabizedNote() :
                wxString{}));

        wxString displayableGradeLevel =
            wxNumberFormatter::ToString(gradeValue, 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 17)
            { displayableGradeLevel += L"+"; }

        SetReadabilityTestResult(CURRENT_TEST_KEY,
            theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(gradeValue,
                GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);

    // if they asked to set focus to the score then select the graph
    if (setFocus)
        {
        view->UpdateSideBarIcons();
        view->GetSideBar()->SelectSubItem(
            view->GetSideBar()->FindSubItem(BaseProjectView::GPM_FRY_PAGE_ID));
        }

    return true;
    }

//-------------------------------------------------------
bool ProjectDoc::AddFryTest(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureEnglishReadabilityTestsCode()))
        { return false; }

    const wxString CURRENT_TEST_KEY = ReadabilityMessages::FRY();

    const wxString wordsColumnName{ _DT(L"WORDS") };
    const wxString syllablesColumnName{ _DT(L"SYLLABLES") };
    const wxString sencentesColumnName{ _DT(L"SENTENCES") };

    auto scoreDataset = std::make_shared<Wisteria::Data::Dataset>();
    scoreDataset->AddContinuousColumn(wordsColumnName);
    scoreDataset->AddContinuousColumn(syllablesColumnName);
    scoreDataset->AddContinuousColumn(sencentesColumnName);

    scoreDataset->AddRow(Data::RowInfo().
                    Continuous(
                        {
                        GetTotalWords(),
                        GetTotalSyllablesNumeralsFullySyllabized(),
                        GetTotalSentences()
                        }));

    if (GetTotalWords() == 0)
        {
        LogMessage(wxString::Format(
            _(L"Unable to calculate %s: at least one word must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    if (GetTotalSentences() == 0)
        {
        LogMessage(wxString::Format(
            _(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    // Fry graph
    ProjectView* view = dynamic_cast<ProjectView*>(GetFirstView());
    wxGCDC gdc(view->GetDocFrame());

    Wisteria::Canvas* fryGraphView =
        dynamic_cast<Wisteria::Canvas*>(view->GetReadabilityResultsView().
            FindWindowById(BaseProjectView::FRY_PAGE_ID));
    if (fryGraphView)
        {
        auto fryGraph =
            std::dynamic_pointer_cast<FryGraph>(fryGraphView->GetFixedObject(0, 0));
        assert(fryGraph);
        fryGraph->SetData(scoreDataset, wordsColumnName, syllablesColumnName,
                            sencentesColumnName);
        fryGraphView->CalcAllSizes(gdc);
        }
    else
        {
        fryGraphView = new Wisteria::Canvas(view->GetSplitter(),
                                            BaseProjectView::FRY_PAGE_ID);

        fryGraphView->SetFixedObjectsGridSize(1, 1);

        fryGraphView->Hide();
        fryGraphView->SetLabel(BaseProjectView::GetFryLabel());
        fryGraphView->SetName(BaseProjectView::GetFryLabel());
        fryGraphView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_GRAPH_MENU") );
        fryGraphView->SetPrinterSettings(*wxGetApp().GetPrintData());

        auto fryGraph = std::make_shared<FryGraph>(fryGraphView,
            FryGraph::FryGraphType::Traditional,
            std::make_shared<Colors::Schemes::ColorScheme>
                 (Colors::Schemes::ColorScheme{
                     ColorBrewer::GetColor(Colors::Color::CelestialBlue) }));
        fryGraph->SetMessageCatalog(GetReadabilityMessageCatalogPtr());
        fryGraph->SetData(scoreDataset, wordsColumnName, syllablesColumnName,
                            sencentesColumnName);

        fryGraphView->SetFixedObject(0, 0, fryGraph);
        view->GetReadabilityResultsView().AddWindow(fryGraphView);
        }
    UpdateGraphOptions(fryGraphView);

    auto fryGraph =
            std::dynamic_pointer_cast<FryGraph>(fryGraphView->GetFixedObject(0, 0));

    fryGraph->SetInvalidAreaColor(GetInvalidAreaColor());
    fryGraphView->CalcAllSizes(gdc);

    const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
    if (!VerifyTestBeforeAdding(theTest))
        { return false; }

    if (fryGraph->GetScores().front().IsScoreInvalid())
        {
        wxString description = L"<tr><td>" +
            _(L"Text is too difficult to be plotted.") +
           L"</td></tr>";

        SetReadabilityTestResult(CURRENT_TEST_KEY,
            theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(
                std::numeric_limits<double>::quiet_NaN(), BaseProjectView::GetFailedLabel()),
            BaseProjectView::GetFailedLabel(),
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    else if (fryGraph->GetScores().front().IsScoreOutOfGradeRange())
        {
        wxString TOO_DIFFICULT_DESCRIPTION =
            _(L"Text is too difficult to be classified to a specific grade level because "
              "it contains too many ");
        TOO_DIFFICULT_DESCRIPTION += fryGraph->GetScores().front().IsWordsHard() ?
                                    _(L"high syllable words.") :
                                    _(L"long sentences.");
        wxString description = L"<tr><td>" + TOO_DIFFICULT_DESCRIPTION + L"</td></tr>";

        SetReadabilityTestResult(CURRENT_TEST_KEY,
            theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(
                std::numeric_limits<double>::quiet_NaN(), BaseProjectView::GetFailedLabel()),
            BaseProjectView::GetFailedLabel(),
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    else
        {
        const wxString INFLUENCE_DESCRIPTION =
            fryGraph->GetScores().front().IsWordsHard() ?
            _(L"3+ syllable words in the text primarily influenced this grade level score.") :
            _(L"The sentence lengths in the text primarily influenced this grade level score.");
        const size_t gradeValue = fryGraph->GetScores().front().GetScore();

        const wxString description =
            ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue) + L"<br />" +
                INFLUENCE_DESCRIPTION,
                theTest.first->get_test(),
                ((GetTotalNumerals() > 0) ?
                    ReadabilityMessages::GetNumeralAreFullySyllabizedNote() : wxString{}));

        wxString displayableGradeLevel =
            wxNumberFormatter::ToString(gradeValue, 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 17)
            { displayableGradeLevel += L"+"; }

        SetReadabilityTestResult(CURRENT_TEST_KEY,
            theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(gradeValue,
                GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);

    // if they asked to set focus to the score then select the graph
    if (setFocus)
        {
        view->UpdateSideBarIcons();
        view->GetSideBar()->SelectSubItem(
            view->GetSideBar()->FindSubItem(BaseProjectView::FRY_PAGE_ID));
        }

    return true;
    }

//-------------------------------------------------------
bool ProjectDoc::AddRaygorTest(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureEnglishReadabilityTestsCode()))
        { return false; }

    const wxString CURRENT_TEST_KEY = ReadabilityMessages::RAYGOR();

    const wxString wordsColumnName{ _DT(L"WORDS") };
    const wxString sixCharWordsColumnName{ _DT(L"6CHARWORDS") };
    const wxString sencentesColumnName{ _DT(L"SENTENCES") };

    auto scoreDataset = std::make_shared<Wisteria::Data::Dataset>();
    scoreDataset->AddContinuousColumn(wordsColumnName);
    scoreDataset->AddContinuousColumn(sixCharWordsColumnName);
    scoreDataset->AddContinuousColumn(sencentesColumnName);

    scoreDataset->AddRow(Data::RowInfo().
                    Continuous(
                        {
                        GetTotalWords() - GetTotalNumerals(),
                        GetTotalSixPlusCharacterWordsIgnoringNumerals(),
                        GetTotalSentences()
                        }));

    if (!GetTotalWords())
        {
        LogMessage(wxString::Format(
            _(L"Unable to calculate %s: at least one word must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    if (!GetTotalSentences() )
        {
        LogMessage(wxString::Format(
            _(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    // Raygor graph
    ProjectView* view = dynamic_cast<ProjectView*>(GetFirstView());
    wxGCDC gdc(view->GetDocFrame());

    Wisteria::Canvas* raygorGraphView =
        dynamic_cast<Wisteria::Canvas*>(view->GetReadabilityResultsView().
            FindWindowById(BaseProjectView::RAYGOR_PAGE_ID));
    if (raygorGraphView)
        {
        auto raygorGraph =
            std::dynamic_pointer_cast<RaygorGraph>(raygorGraphView->GetFixedObject(0, 0));
        assert(raygorGraph);
        raygorGraph->SetData(scoreDataset, wordsColumnName, sixCharWordsColumnName,
                            sencentesColumnName);
        }
    else
        {
        raygorGraphView = new Wisteria::Canvas(view->GetSplitter(),
                                               BaseProjectView::RAYGOR_PAGE_ID);

        raygorGraphView->SetFixedObjectsGridSize(1, 1);

        raygorGraphView->Hide();
        raygorGraphView->SetLabel(BaseProjectView::GetRaygorLabel());
        raygorGraphView->SetName(BaseProjectView::GetRaygorLabel());
        raygorGraphView->AssignContextMenu(
            wxXmlResource::Get()->LoadMenu(L"IDM_GRAPH_MENU") );
        raygorGraphView->SetPrinterSettings(*wxGetApp().GetPrintData());

        auto raygorGraph = std::make_shared<RaygorGraph>(raygorGraphView,
            std::make_shared<Colors::Schemes::ColorScheme>
                 (Colors::Schemes::ColorScheme{
                     ColorBrewer::GetColor(Colors::Color::CelestialBlue) }));
        raygorGraph->SetMessageCatalog(GetReadabilityMessageCatalogPtr());
        raygorGraph->SetData(scoreDataset, wordsColumnName, sixCharWordsColumnName,
                            sencentesColumnName);

        raygorGraphView->SetFixedObject(0, 0, raygorGraph);

        view->GetReadabilityResultsView().AddWindow(raygorGraphView);
        }
    UpdateGraphOptions(raygorGraphView);

    auto raygorGraph =
        std::dynamic_pointer_cast<RaygorGraph>(raygorGraphView->GetFixedObject(0, 0));
    raygorGraph->SetInvalidAreaColor(GetInvalidAreaColor());
    raygorGraph->SetRaygorStyle(GetRaygorStyle());
    raygorGraphView->CalcAllSizes(gdc);

    const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
    if (!VerifyTestBeforeAdding(theTest))
        { return false; }

    if (raygorGraph->GetScores().at(0).IsScoreInvalid())
        {
        SetReadabilityTestResult(CURRENT_TEST_KEY,
            theTest.first->get_test().get_long_name().c_str(),
            L"<tr><td>" + _(L"Text is too difficult to be plotted.") + L"</td></tr>",
            std::make_pair(
                std::numeric_limits<double>::quiet_NaN(), BaseProjectView::GetFailedLabel()),
            BaseProjectView::GetFailedLabel(),
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    else if (raygorGraph->GetScores().at(0).IsScoreOutOfGradeRange())
        {
        wxString TOO_DIFFICULT_DESCRIPTION =
            _(L"Text is too difficult to be classified to a specific grade level "
              "because it contains too many ");
        TOO_DIFFICULT_DESCRIPTION += raygorGraph->GetScores().at(0).IsWordsHard() ?
                                    _(L"6+ character words.") :
                                    _(L"long sentences.");
        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(),
            L"<tr><td>" + TOO_DIFFICULT_DESCRIPTION + L"</td></tr>",
            std::make_pair(std::numeric_limits<double>::quiet_NaN(), BaseProjectView::GetFailedLabel()),
            BaseProjectView::GetFailedLabel(),
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    else
        {
        const wxString INFLUENCE_DESCRIPTION =
            raygorGraph->GetScores().at(0).IsWordsHard() ?
            _(L"6+ character words in the text primarily influenced this grade level score.") :
            _(L"The sentence lengths in the text primarily influenced this grade level score.");
        const size_t gradeValue = raygorGraph->GetScores().at(0).GetScore();

        wxString displayableGradeLevel =
            wxNumberFormatter::ToString(gradeValue, 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 17)
            { displayableGradeLevel += L"+"; }
        const wxString description = ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue) +
                L"<br /><br />" +
                INFLUENCE_DESCRIPTION,
                theTest.first->get_test(),
                ReadabilityMessages::GetPunctuationIgnoredNote());

        SetReadabilityTestResult(CURRENT_TEST_KEY,
            theTest.first->get_test().get_long_name().c_str(),
            description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(gradeValue,
                GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);

    // if they asked to set focus to the score then select the graph
    if (setFocus)
        {
        view->UpdateSideBarIcons();
        view->GetSideBar()->SelectSubItem(
            view->GetSideBar()->FindSubItem(BaseProjectView::RAYGOR_PAGE_ID));
        }

    return true;
    }

bool ProjectDoc::AddDolchSightWords()
    {
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureEnglishReadabilityTestsCode()))
        { return false; }
    if (!GetTotalWords())
        {
        LogMessage(_(L"Unable to calculate Dolch words: at least one word must be present in document."),
            _(L"Error"), wxOK|wxICON_ERROR);
        IncludeDolchSightWords(false);
        return false;
        }
    if (!GetTotalSentences() )
        {
        LogMessage(_(L"Unable to calculate Dolch words: at least one valid sentence must be present in document."),
            _(L"Error"), wxOK|wxICON_ERROR);
        IncludeDolchSightWords(false);
        return false;
        }

    IncludeDolchSightWords(true);
    RefreshRequired(ProjectRefresh::FullReindexing);
    RefreshProject();

    ProjectView* view = dynamic_cast<ProjectView*>(GetFirstView());
    view->GetSideBar()->SelectFolder(view->GetSideBar()->FindFolder(BaseProjectView::SIDEBAR_DOLCH_SECTION_ID));

    return true;
    }

//-------------------------------------------------------
void ProjectDoc::SetReadabilityTestResult(const wxString& testId,
                                    const wxString& testName,
                                    const wxString& description,
                                    const std::pair<double, wxString>& USGradeLevel,
                                    const wxString& readerAge,
                                    const double indexScore,
                                    const double clozeScore,
                                    const bool setFocus)
    {
    BaseProject::SetReadabilityTestResult(testId, testName, description, USGradeLevel, readerAge,
                                          indexScore, clozeScore, setFocus);

    ProjectView* view = dynamic_cast<ProjectView*>(GetFirstView());
    assert(view && "Invalid view when adding test!");
    if (!view)
        { return; }
    // format the explanation window
    const wxString explanationString = wxString::Format(
        L"<table class='minipage' style='width:100%%;'><tr>\n"
         "<th colspan='2' style='background:%s; text-align:left'>"
         "<span style='color:%s;'>%s</span></th></tr>\n%s\n</table>",
        ProjectReportFormat::GetReportHeaderColor().GetAsString(wxC2S_HTML_SYNTAX),
        ProjectReportFormat::GetReportHeaderFontColor().GetAsString(wxC2S_HTML_SYNTAX),
            testName, description);

    wxWindowUpdateLocker noUpdates(view->GetReadabilityScoresList());
    long location = view->GetReadabilityScoresList()->GetResultsListCtrl()->FindEx(testName);
    if (location == wxNOT_FOUND)
        {
        location = view->GetReadabilityScoresList()->GetResultsListCtrl()->AddRow(testName);
        }
    view->GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(location, 1, USGradeLevel.second,
        NumberFormatInfo(NumberFormatInfo::NumberFormatType::CustomFormatting, 1));
    view->GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(location, 2, readerAge);
    if (std::isnan(indexScore))
        { view->GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(location, 3, wxString{}); }
    else
        {
        view->GetReadabilityScoresList()->GetDataProvider()->SetItemValue(
            location, 3, round_decimal_place(indexScore, 10),
            NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 1));
        }
    if (std::isnan(clozeScore))
        { view->GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(location, 4, wxString{}); }
    else
        {
        view->GetReadabilityScoresList()->GetDataProvider()->SetItemValue(
            location, 4, round_to_integer(clozeScore));
        }
    view->GetReadabilityScoresList()->GetExplanations()[testName] = explanationString;

    // select item and select scores window in the main project view
    location = view->GetReadabilityScoresList()->GetResultsListCtrl()->FindEx(testName);
    view->GetReadabilityScoresList()->GetResultsListCtrl()->Select(location);
    // don't call Focus() because that will call EnsureVisible and cause interface problems, just set the state
    view->GetReadabilityScoresList()->GetResultsListCtrl()->SetItemState(location,
        wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED);
    view->GetReadabilityScoresList()->GetResultsListCtrl()->SetColumnWidth(0,
        view->GetReadabilityScoresList()->GetResultsListCtrl()->EstimateColumnWidth(0));
    view->GetReadabilityScoresList()->GetResultsListCtrl()->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER);
    view->GetReadabilityScoresList()->GetResultsListCtrl()->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);
    view->GetReadabilityScoresList()->GetResultsListCtrl()->SetColumnWidth(3, wxLIST_AUTOSIZE_USEHEADER);
    view->GetReadabilityScoresList()->GetResultsListCtrl()->SetSize(
        view->GetReadabilityScoresList()->GetResultsListCtrl()->GetSize());

    if (setFocus)
        {
        // focus the project section
        view->GetSideBar()->SelectFolder(
            view->GetSideBar()->FindFolder(BaseProjectView::SIDEBAR_READABILITY_SCORES_SECTION_ID));
        }
    }

//-------------------------------------------------------
void ProjectDoc::UpdateHighlightedTextWindows()
    {
    ProjectView* view = dynamic_cast<ProjectView*>(GetFirstView());
    // DC
    if (GetWordsBreakdownInfo().IsDCUnfamiliarEnabled() && IsDaleChallLikeTestIncluded() )
        {
        const auto buddyWindowPosition = view->GetWordsBreakdownView().FindWindowPositionById(
            BaseProjectView::DC_WORDS_LIST_PAGE_ID);
        view->GetWordsBreakdownView().InsertWindow((buddyWindowPosition != wxNOT_FOUND) ?
            buddyWindowPosition+1 : 0, m_dcTextWindow);
        }
    else
        { view->GetWordsBreakdownView().RemoveWindowById(BaseProjectView::DC_WORDS_TEXT_PAGE_ID); }
    // Spache
    if (GetWordsBreakdownInfo().IsSpacheUnfamiliarEnabled() && GetReadabilityTests().is_test_included(
        ReadabilityMessages::SPACHE()))
        {
        const auto buddyWindowPosition = view->GetWordsBreakdownView().FindWindowPositionById(
            BaseProjectView::SPACHE_WORDS_LIST_PAGE_ID);
        view->GetWordsBreakdownView().InsertWindow((buddyWindowPosition != wxNOT_FOUND) ?
            buddyWindowPosition+1 : 0, m_spacheTextWindow);
        }
    else
        { view->GetWordsBreakdownView().RemoveWindowById(BaseProjectView::SPACHE_WORDS_TEXT_PAGE_ID); }
    // HJ
    if (GetWordsBreakdownInfo().IsHarrisJacobsonUnfamiliarEnabled() &&
        GetReadabilityTests().is_test_included(ReadabilityMessages::HARRIS_JACOBSON()))
        {
        const auto buddyWindowPosition = view->GetWordsBreakdownView().FindWindowPositionById(
            BaseProjectView::HARRIS_JACOBSON_WORDS_LIST_PAGE_ID);
        view->GetWordsBreakdownView().InsertWindow((buddyWindowPosition != wxNOT_FOUND) ?
            buddyWindowPosition+1 : 0, m_hjTextWindow);
        }
    else
        { view->GetWordsBreakdownView().RemoveWindowById(BaseProjectView::HARRIS_JACOBSON_WORDS_TEXT_PAGE_ID); }
    }

//-------------------------------------------------------
std::pair<wxString, wxString> ProjectDoc::FormatRtfHeaderFont(
    const wxFont& textViewFont,
    const size_t mainFontColorIndex)
    {
    wxString mainFontHeader = wxString::Format(L"\\viewkind4\\uc1\\pard\\cf%zu", mainFontColorIndex);
    wxString endSection = L"\\par\\cf0";
    endSection += L"\\cf0";
    if (textViewFont.GetWeight() == wxFONTWEIGHT_BOLD)
        {
        endSection += L"\\b0";
        mainFontHeader += L"\\b";
        }
    if (textViewFont.GetStyle() == wxFONTSTYLE_ITALIC)
        {
        endSection += L"\\i0";
        mainFontHeader += L"\\i";
        }
    if (textViewFont.GetUnderlined())
        {
        endSection += L"\\ulnone";
        mainFontHeader += _DT(L"\\ul");
        }
    // "fs" command is in half points
    mainFontHeader += wxString::Format(L"\\f0\\fs%d ", textViewFont.GetPointSize()*2);

    endSection += L"\\par}";

    return std::make_pair(mainFontHeader, endSection);
    }

//-------------------------------------------------------
ProjectDoc::HighlighterColors ProjectDoc::BuildReportColors(
    const wxColour& highlightColor,
    const wxColour& backgroundColor)
    {
    HighlighterColors highlighterColors;

    ColorContrast colorContrast(backgroundColor);
    highlighterColors.highlightColor = (GetTextHighlightMethod() == TextHighlight::HighlightForeground) ?
        colorContrast.Contrast(highlightColor) : highlightColor;

    highlighterColors.errorHighlightColor =
        (GetTextHighlightMethod() == TextHighlight::HighlightForeground) ?
        colorContrast.Contrast(GetDuplicateWordHighlightColor()) : GetDuplicateWordHighlightColor();
    highlighterColors.styleHighlightColor =
        (GetTextHighlightMethod() == TextHighlight::HighlightForeground) ?
        colorContrast.Contrast(GetWordyPhraseHighlightColor()) : GetWordyPhraseHighlightColor();
    highlighterColors.excludedTextHighlightColor =
        (GetTextHighlightMethod() == TextHighlight::HighlightForeground) ?
        colorContrast.Contrast(GetExcludedTextHighlightColor()) : GetExcludedTextHighlightColor();
    highlighterColors.dolchConjunctionsTextHighlightColor =
        (GetTextHighlightMethod() == TextHighlight::HighlightForeground) ?
        colorContrast.Contrast(GetDolchConjunctionsColor()) : GetDolchConjunctionsColor();
    highlighterColors.dolchPrepositionsTextHighlightColor =
        (GetTextHighlightMethod() == TextHighlight::HighlightForeground) ?
        colorContrast.Contrast(GetDolchPrepositionsColor()) : GetDolchPrepositionsColor();
    highlighterColors.dolchPronounsTextHighlightColor =
        (GetTextHighlightMethod() == TextHighlight::HighlightForeground) ?
        colorContrast.Contrast(GetDolchPronounsColor()) : GetDolchPronounsColor();
    highlighterColors.dolchAdverbsTextHighlightColor =
        (GetTextHighlightMethod() == TextHighlight::HighlightForeground) ?
        colorContrast.Contrast(GetDolchAdverbsColor()) : GetDolchAdverbsColor();
    highlighterColors.dolchAdjectivesTextHighlightColor =
        (GetTextHighlightMethod() == TextHighlight::HighlightForeground) ?
        colorContrast.Contrast(GetDolchAdjectivesColor()) : GetDolchAdjectivesColor();
    highlighterColors.dolchVerbsTextHighlightColor =
        (GetTextHighlightMethod() == TextHighlight::HighlightForeground) ?
        colorContrast.Contrast(GetDolchVerbsColor()) : GetDolchVerbsColor();
    highlighterColors.dolchNounTextHighlightColor =
        (GetTextHighlightMethod() == TextHighlight::HighlightForeground) ?
        colorContrast.Contrast(GetDolchNounColor()) : GetDolchNounColor();

    return highlighterColors;
    }

//-------------------------------------------------------
std::tuple<wxString, wxString, wxString> ProjectDoc::BuildColorTable(
    const wxFont& textViewFont,
    const HighlighterColors& highlighterColors,
    const wxColour& backgroundColor)
    {
    // black is at position 1,
    // white is added to the end (position 13)
    wxString colorTable = wxString::Format(
        L"{\\colortbl; "
        "\\red0\\green0\\blue0;"
        "\\red%u\\green%u\\blue%u;\\red%u\\green%u\\blue%u;\\red%u\\green%u\\blue%u;\\red%u\\green%u\\blue%u;"
        "\\red%u\\green%u\\blue%u;\\red%u\\green%u\\blue%u;\\red%u\\green%u\\blue%u;\\red%u\\green%u\\blue%u;",
        highlighterColors.highlightColor.Red(),
            highlighterColors.highlightColor.Green(),
            highlighterColors.highlightColor.Blue(),
        highlighterColors.errorHighlightColor.Red(),
            highlighterColors.errorHighlightColor.Green(),
            highlighterColors.errorHighlightColor.Blue(),
        highlighterColors.styleHighlightColor.Red(),
            highlighterColors.styleHighlightColor.Green(),
            highlighterColors.styleHighlightColor.Blue(),
        highlighterColors.excludedTextHighlightColor.Red(),
            highlighterColors.excludedTextHighlightColor.Green(),
            highlighterColors.excludedTextHighlightColor.Blue(),
        highlighterColors.dolchConjunctionsTextHighlightColor.Red(),
            highlighterColors.dolchConjunctionsTextHighlightColor.Green(),
            highlighterColors.dolchConjunctionsTextHighlightColor.Blue(),
        highlighterColors.dolchPrepositionsTextHighlightColor.Red(),
            highlighterColors.dolchPrepositionsTextHighlightColor.Green(),
            highlighterColors.dolchPrepositionsTextHighlightColor.Blue(),
        highlighterColors.dolchPronounsTextHighlightColor.Red(),
            highlighterColors.dolchPronounsTextHighlightColor.Green(),
            highlighterColors.dolchPronounsTextHighlightColor.Blue(),
        highlighterColors.dolchAdverbsTextHighlightColor.Red(),
            highlighterColors.dolchAdverbsTextHighlightColor.Green(),
            highlighterColors.dolchAdverbsTextHighlightColor.Blue());
    colorTable += wxString::Format(
        L"\\red%u\\green%u\\blue%u;\\red%u"
        "\\green%u\\blue%u;\\red%u\\green%u\\blue%u;"
        "\\red255\\green255\\blue255;}",
        highlighterColors.dolchAdjectivesTextHighlightColor.Red(),
            highlighterColors.dolchAdjectivesTextHighlightColor.Green(),
            highlighterColors.dolchAdjectivesTextHighlightColor.Blue(),
        highlighterColors.dolchVerbsTextHighlightColor.Red(),
            highlighterColors.dolchVerbsTextHighlightColor.Green(),
            highlighterColors.dolchVerbsTextHighlightColor.Blue(),
        highlighterColors.dolchNounTextHighlightColor.Red(),
            highlighterColors.dolchNounTextHighlightColor.Green(),
            highlighterColors.dolchNounTextHighlightColor.Blue());

    const auto [mainFontHeader, ending] =
        FormatRtfHeaderFont(textViewFont, ((backgroundColor.GetLuminance() < .5f) ? 13 : 1));

    return std::make_tuple(colorTable, mainFontHeader, ending);
    }

//-------------------------------------------------------
ProjectDoc::HighlighterTags ProjectDoc::BuildHighlighterTags(
    [[maybe_unused]] const wxColour& highlightColor,
    [[maybe_unused]] const HighlighterColors& highlighterColors)
    {
    HighlighterTags highlighterTags;

#if defined (__WXMSW__) || defined (__WXOSX__)
    // When highlighting backgrounds, set the text colors to contrast against it.
    // Note that even though the text window might be dark (with white font), the highlighting
    // might be light and in that case we will want a dark font for that word.
    // (13 and white, 1 is black [refer to color table below].)
    const int highlightedTextColorIndexBGMode = (highlightColor.GetLuminance() < .5f) ? 13 : 1;
    const int errorTextColorIndexBGMode = (GetDuplicateWordHighlightColor().GetLuminance() < .5f) ? 13 : 1;
    const int styleTextColorIndexBGMode = (GetWordyPhraseHighlightColor().GetLuminance() < .5f) ? 13 : 1;
    const int excludedTextColorIndexBGMode = (GetExcludedTextHighlightColor().GetLuminance() < .5f) ? 13 : 1;
    const int dolchConjunctionsTextColorIndexBGMode = (GetDolchConjunctionsColor().GetLuminance() < .5f) ? 13 : 1;
    const int dolchPrepositionsTextColorIndexBGMode = (GetDolchPrepositionsColor().GetLuminance() < .5f) ? 13 : 1;
    const int dolchPronounsTextColorIndexBGMode = (GetDolchPronounsColor().GetLuminance() < .5f) ? 13 : 1;
    const int dolchAdverbsTextColorIndexBGMode = (GetDolchAdverbsColor().GetLuminance() < .5f) ? 13 : 1;
    const int dolchAdjectivesTextColorIndexBGMode = (GetDolchAdjectivesColor().GetLuminance() < .5f) ? 13 : 1;
    const int dolchVerbsTextColorIndexBGMode = (GetDolchVerbsColor().GetLuminance() < .5f) ? 13 : 1;
    const int dolchNounTextColorIndexBGMode = (GetDolchNounColor().GetLuminance() < .5f) ? 13 : 1;
#elif defined(__WXGTK__)
    const wxColour highlightedTextColor = (highlightColor.GetLuminance() < .5f) ? *wxWHITE : *wxBLACK;
    const wxColour errorTextColor = (GetDuplicateWordHighlightColor().GetLuminance() < .5f) ? *wxWHITE : *wxBLACK;
    const wxColour styleTextColor = (GetWordyPhraseHighlightColor().GetLuminance() < .5f) ? *wxWHITE : *wxBLACK;
    const wxColour excludedTextColor = (GetExcludedTextHighlightColor().GetLuminance() < .5f) ? *wxWHITE : *wxBLACK;
    const wxColour dolchConjunctionsTextColor =
        (GetDolchConjunctionsColor().GetLuminance() < .5f) ? *wxWHITE : *wxBLACK;
    const wxColour dolchPrepositionsTextColor =
        (GetDolchPrepositionsColor().GetLuminance() < .5f) ? *wxWHITE : *wxBLACK;
    const wxColour dolchPronounsTextColor = (GetDolchPronounsColor().GetLuminance() < .5f) ? *wxWHITE : *wxBLACK;
    const wxColour dolchAdverbsTextColor = (GetDolchAdverbsColor().GetLuminance() < .5f) ? *wxWHITE : *wxBLACK;
    const wxColour dolchAdjectivesTextColor = (GetDolchAdjectivesColor().GetLuminance() < .5f) ? *wxWHITE : *wxBLACK;
    const wxColour dolchVerbsTextColor = (GetDolchVerbsColor().GetLuminance() < .5f) ? *wxWHITE : *wxBLACK;
    const wxColour dolchNounTextColor = (GetDolchNounColor().GetLuminance() < .5f) ? *wxWHITE : *wxBLACK;
#endif

#ifdef __WXMSW__
    highlighterTags.HIGHLIGHT_BEGIN = (GetTextHighlightMethod() == TextHighlight::HighlightBackground) ?
        wxString::Format(L"{\\highlight2\\cf%d ", highlightedTextColorIndexBGMode) :
        wxString{ L"{\\cf2 " };
    highlighterTags.ERROR_HIGHLIGHT_BEGIN = (GetTextHighlightMethod() == TextHighlight::HighlightBackground) ?
        wxString::Format(L"{\\highlight3\\cf%d ", errorTextColorIndexBGMode) :
        wxString{ L"{\\cf3 " };
    highlighterTags.PHRASE_HIGHLIGHT_BEGIN = (GetTextHighlightMethod() == TextHighlight::HighlightBackground) ?
        wxString::Format(L"{\\highlight4\\cf%d ", styleTextColorIndexBGMode) :
        wxString{ L"{\\cf4 " };
    highlighterTags.IGNORE_HIGHLIGHT_BEGIN = (GetTextHighlightMethod() == TextHighlight::HighlightBackground) ?
        wxString::Format(L"{\\highlight5\\cf%d ", excludedTextColorIndexBGMode) :
        wxString{ L"{\\cf5 " };
    // Dolch highlighting
    highlighterTags.DOLCH_CONJUNCTION_BEGIN = IsHighlightingDolchConjunctions() ?
        (GetTextHighlightMethod() == TextHighlight::HighlightBackground) ?
        wxString::Format(L"{\\highlight6\\cf%d ", dolchConjunctionsTextColorIndexBGMode) :
        wxString{ L"{\\cf6 " } :
        wxString{};
    highlighterTags.DOLCH_PREPOSITIONS_BEGIN = IsHighlightingDolchPrepositions() ?
        (GetTextHighlightMethod() == TextHighlight::HighlightBackground) ?
        wxString::Format(L"{\\highlight7\\cf%d ", dolchPrepositionsTextColorIndexBGMode) :
        wxString{ L"{\\cf7 " } :
        wxString{};
    highlighterTags.DOLCH_PRONOUN_BEGIN = IsHighlightingDolchPronouns() ?
        (GetTextHighlightMethod() == TextHighlight::HighlightBackground) ?
        wxString::Format(L"{\\highlight8\\cf%d ", dolchPronounsTextColorIndexBGMode) :
        wxString{ L"{\\cf8 " } :
        wxString{};
    highlighterTags.DOLCH_ADVERB_BEGIN = IsHighlightingDolchAdverbs() ?
        (GetTextHighlightMethod() == TextHighlight::HighlightBackground) ?
        wxString::Format(L"{\\highlight9\\cf%d ", dolchAdverbsTextColorIndexBGMode) :
        wxString{ L"{\\cf9 " } :
        wxString{};
    highlighterTags.DOLCH_ADJECTIVE_BEGIN = IsHighlightingDolchAdjectives() ?
        (GetTextHighlightMethod() == TextHighlight::HighlightBackground) ?
        wxString::Format(L"{\\highlight10\\cf%d ", dolchAdjectivesTextColorIndexBGMode) :
        wxString{ L"{\\cf10 " } :
        wxString{};
    highlighterTags.DOLCH_VERB_BEGIN = IsHighlightingDolchVerbs() ?
        (GetTextHighlightMethod() == TextHighlight::HighlightBackground) ?
        wxString::Format(L"{\\highlight11\\cf%d ", dolchVerbsTextColorIndexBGMode) :
        wxString{ L"{\\cf11 " } :
        wxString{};
    highlighterTags.DOLCH_NOUN_BEGIN = IsHighlightingDolchNouns() ?
        (GetTextHighlightMethod() == TextHighlight::HighlightBackground) ?
        wxString::Format(L"{\\highlight12\\cf%d ", dolchNounTextColorIndexBGMode) :
        wxString{ L"{\\cf12 " } :
        wxString{};
    // terminator
    highlighterTags.HIGHLIGHT_END = L"}";

    // these are used for the legend lines because they are always set to use background highlighting
    highlighterTags.HIGHLIGHT_BEGIN_LEGEND = L"{\\highlight2 ";
    highlighterTags.HIGHLIGHT_END_LEGEND = L"}";
    highlighterTags.DUPLICATE_HIGHLIGHT_BEGIN_LEGEND = L"{\\highlight3 ";
    highlighterTags.PHRASE_HIGHLIGHT_BEGIN_LEGEND = L"{\\highlight4 ";
    highlighterTags.IGNORE_HIGHLIGHT_BEGIN_LEGEND = L"{\\highlight5 ";
    // dolch highlighting
    highlighterTags.DOLCH_CONJUNCTION_BEGIN_LEGEND = IsHighlightingDolchConjunctions() ?
        L"{\\highlight6 " : L"{";
    highlighterTags.DOLCH_PREPOSITIONS_BEGIN_LEGEND = IsHighlightingDolchPrepositions() ?
        L"{\\highlight7 " : L"{";
    highlighterTags.DOLCH_PRONOUN_BEGIN_LEGEND = IsHighlightingDolchPronouns() ?
        L"{\\highlight8 " : L"{";
    highlighterTags.DOLCH_ADVERB_BEGIN_LEGEND = IsHighlightingDolchAdverbs() ?
        L"{\\highlight9 " : L"{";
    highlighterTags.DOLCH_ADJECTIVE_BEGIN_LEGEND = IsHighlightingDolchAdjectives() ?
        L"{\\highlight10 " : L"{";
    highlighterTags.DOLCH_VERB_BEGIN_LEGEND = IsHighlightingDolchVerbs() ?
        L"{\\highlight11 " : L"{";
    highlighterTags.DOLCH_NOUN_BEGIN_LEGEND = IsHighlightingDolchNouns() ?
        L"{\\highlight12 " : L"{";
#elif defined(__WXOSX__)
    highlighterTags.HIGHLIGHT_BEGIN = (GetTextHighlightMethod() == TextHighlight::HighlightBackground) ?
        wxString::Format(L"{\\cb2\\cf%d ", highlightedTextColorIndexBGMode) :
        L"{\\cf2 ";
    highlighterTags.ERROR_HIGHLIGHT_BEGIN = (GetTextHighlightMethod() == TextHighlight::HighlightBackground) ?
        wxString::Format(L"{\\cb3\\cf%d ", errorTextColorIndexBGMode) :
        L"{\\cf3 ";
    highlighterTags.PHRASE_HIGHLIGHT_BEGIN = (GetTextHighlightMethod() == TextHighlight::HighlightBackground) ?
        wxString::Format(L"{\\cb4\\cf%d ", styleTextColorIndexBGMode) :
        L"{\\cf4 ";
    highlighterTags.IGNORE_HIGHLIGHT_BEGIN = (GetTextHighlightMethod() == TextHighlight::HighlightBackground) ?
        wxString::Format(L"{\\cb5\\cf%d ", excludedTextColorIndexBGMode) :
        L"{\\cf5 ";
    // dolch highlighting
    highlighterTags.DOLCH_CONJUNCTION_BEGIN = IsHighlightingDolchConjunctions() ?
        (GetTextHighlightMethod() == TextHighlight::HighlightBackground) ?
        wxString::Format(L"{\\cb6\\cf%d ", dolchConjunctionsTextColorIndexBGMode) :
        L"{\\cf6 " :
        wxString{};
    highlighterTags.DOLCH_PREPOSITIONS_BEGIN = IsHighlightingDolchPrepositions() ?
        (GetTextHighlightMethod() == TextHighlight::HighlightBackground) ?
        wxString::Format(L"{\\cb7\\cf%d ", dolchPrepositionsTextColorIndexBGMode) :
        L"{\\cf7 " :
        wxString{};
    highlighterTags.DOLCH_PRONOUN_BEGIN = IsHighlightingDolchPronouns() ?
        (GetTextHighlightMethod() == TextHighlight::HighlightBackground) ?
        wxString::Format(L"{\\cb8\\cf%d ", dolchPronounsTextColorIndexBGMode) :
        L"{\\cf8 " :
        wxString{};
    highlighterTags.DOLCH_ADVERB_BEGIN = IsHighlightingDolchAdverbs() ?
        (GetTextHighlightMethod() == TextHighlight::HighlightBackground) ?
        wxString::Format(L"{\\cb9\\cf%d ", dolchAdverbsTextColorIndexBGMode) :
        L"{\\cf9 " :
        wxString{};
    highlighterTags.DOLCH_ADJECTIVE_BEGIN = IsHighlightingDolchAdjectives() ?
        (GetTextHighlightMethod() == TextHighlight::HighlightBackground) ?
        wxString::Format(L"{\\cb10\\cf%d ", dolchAdjectivesTextColorIndexBGMode) :
        L"{\\cf10 " :
        wxString{};
    highlighterTags.DOLCH_VERB_BEGIN = IsHighlightingDolchNouns() ?
        (GetTextHighlightMethod() == TextHighlight::HighlightBackground) ?
        wxString::Format(L"{\\cb11\\cf%d ", dolchVerbsTextColorIndexBGMode) :
        L"{\\cf11 " :
        wxString{};
    highlighterTags.DOLCH_NOUN_BEGIN = IsHighlightingDolchNouns() ?
        (GetTextHighlightMethod() == TextHighlight::HighlightBackground) ?
        wxString::Format(L"{\\cb12\\cf%d ", dolchNounTextColorIndexBGMode) :
        L"{\\cf12 " :
        wxString{};
    // terminator
    highlighterTags.HIGHLIGHT_END = L"}";

    // these are used for the legend lines because they are always set to use background highlighting
    highlighterTags.HIGHLIGHT_BEGIN_LEGEND = L"{\\cb2 ";
    highlighterTags.HIGHLIGHT_END_LEGEND = L"}";
    highlighterTags.DUPLICATE_HIGHLIGHT_BEGIN_LEGEND = L"{\\cb3 ";
    highlighterTags.PHRASE_HIGHLIGHT_BEGIN_LEGEND = L"{\\cb4 ";
    highlighterTags.IGNORE_HIGHLIGHT_BEGIN_LEGEND = L"{\\cb5 ";
    // dolch highlighting
    highlighterTags.DOLCH_CONJUNCTION_BEGIN_LEGEND = IsHighlightingDolchConjunctions() ?
        L"{\\cb6 " : L"{";
    highlighterTags.DOLCH_PREPOSITIONS_BEGIN_LEGEND = IsHighlightingDolchPrepositions() ?
        L"{\\cb7 " : L"{";
    highlighterTags.DOLCH_PRONOUN_BEGIN_LEGEND = IsHighlightingDolchPronouns() ?
        L"{\\cb8 " : L"{";
    highlighterTags.DOLCH_ADVERB_BEGIN_LEGEND = IsHighlightingDolchAdverbs() ?
        L"{\\cb9 " : L"{";
    highlighterTags.DOLCH_ADJECTIVE_BEGIN_LEGEND = IsHighlightingDolchAdjectives() ?
        L"{\\cb10 " : L"{";
    highlighterTags.DOLCH_VERB_BEGIN_LEGEND = IsHighlightingDolchVerbs() ?
        L"{\\cb11 " : L"{";
    highlighterTags.DOLCH_NOUN_BEGIN_LEGEND = IsHighlightingDolchNouns() ?
        L"{\\cb12 " : L"{";
#elif defined(__WXGTK__)
    highlighterTags.HIGHLIGHT_END = L"</span>";
    highlighterTags.BOLD_BEGIN = L"<b>";
    highlighterTags.BOLD_END = L"</b>";
    highlighterTags.TAB_SYMBOL = L"    ";
    highlighterTags.CRLF = L"\r\n";
    highlighterTags.HIGHLIGHT_END_LEGEND = L"</span>";
    highlighterTags.HIGHLIGHT_BEGIN = (GetTextHighlightMethod() == TextHighlight::HighlightBackground) ?
        wxString::Format(L"<span background=\"%s\" foreground=\"%s\">",
            highlighterColors.highlightColor.GetAsString(wxC2S_HTML_SYNTAX),
            highlightedTextColor.GetAsString(wxC2S_HTML_SYNTAX)) :
        wxString::Format(L"<span foreground=\"%s\" weight=\"heavy\">",
            highlighterColors.highlightColor.GetAsString(wxC2S_HTML_SYNTAX));
    highlighterTags.ERROR_HIGHLIGHT_BEGIN = (GetTextHighlightMethod() == TextHighlight::HighlightBackground) ?
        wxString::Format(L"<span background=\"%s\" foreground=\"%s\">",
            highlighterColors.errorHighlightColor.GetAsString(wxC2S_HTML_SYNTAX),
            errorTextColor.GetAsString(wxC2S_HTML_SYNTAX)) :
        wxString::Format(L"<span foreground=\"%s\">",
            highlighterColors.errorHighlightColor.GetAsString(wxC2S_HTML_SYNTAX));
    highlighterTags.PHRASE_HIGHLIGHT_BEGIN = (GetTextHighlightMethod() == TextHighlight::HighlightBackground) ?
        wxString::Format(L"<span background=\"%s\" foreground=\"%s\">",
            highlighterColors.styleHighlightColor.GetAsString(wxC2S_HTML_SYNTAX),
            styleTextColor.GetAsString(wxC2S_HTML_SYNTAX)) :
        wxString::Format(L"<span foreground=\"%s\">",
            highlighterColors.styleHighlightColor.GetAsString(wxC2S_HTML_SYNTAX));
    highlighterTags.IGNORE_HIGHLIGHT_BEGIN = (GetTextHighlightMethod() == TextHighlight::HighlightBackground) ?
        wxString::Format(L"<span background=\"%s\"  foreground=\"%s\" strikethrough=\"true\">",
            highlighterColors.excludedTextHighlightColor.GetAsString(wxC2S_HTML_SYNTAX),
            excludedTextColor.GetAsString(wxC2S_HTML_SYNTAX)) :
        wxString::Format(L"<span foreground=\"%s\" strikethrough=\"true\">",
            highlighterColors.excludedTextHighlightColor.GetAsString(wxC2S_HTML_SYNTAX));
    highlighterTags.DOLCH_CONJUNCTION_BEGIN = IsHighlightingDolchConjunctions() ?
        (GetTextHighlightMethod() == TextHighlight::HighlightBackground) ?
        wxString::Format(L"<span background=\"%s\" foreground=\"%s\">",
            highlighterColors.dolchConjunctionsTextHighlightColor.GetAsString(wxC2S_HTML_SYNTAX),
            dolchConjunctionsTextColor.GetAsString(wxC2S_HTML_SYNTAX)) :
        wxString::Format(L"<span foreground=\"%s\" weight=\"heavy\">",
            highlighterColors.dolchConjunctionsTextHighlightColor.GetAsString(wxC2S_HTML_SYNTAX)) :
        wxString{};
    highlighterTags.DOLCH_PREPOSITIONS_BEGIN = IsHighlightingDolchPrepositions() ?
        (GetTextHighlightMethod() == TextHighlight::HighlightBackground) ?
        wxString::Format(L"<span background=\"%s\" foreground=\"%s\">",
            highlighterColors.dolchPrepositionsTextHighlightColor.GetAsString(wxC2S_HTML_SYNTAX),
            dolchPrepositionsTextColor.GetAsString(wxC2S_HTML_SYNTAX)) :
        wxString::Format(L"<span foreground=\"%s\" weight=\"heavy\">",
            highlighterColors.dolchPrepositionsTextHighlightColor.GetAsString(wxC2S_HTML_SYNTAX)) :
        wxString{};
    highlighterTags.DOLCH_PRONOUN_BEGIN = IsHighlightingDolchPronouns() ?
        (GetTextHighlightMethod() == TextHighlight::HighlightBackground) ?
        wxString::Format(L"<span background=\"%s\" foreground=\"%s\">",
            highlighterColors.dolchPronounsTextHighlightColor.GetAsString(wxC2S_HTML_SYNTAX),
            dolchPronounsTextColor.GetAsString(wxC2S_HTML_SYNTAX)) :
        wxString::Format(L"<span foreground=\"%s\" weight=\"heavy\">",
            highlighterColors.dolchPronounsTextHighlightColor.GetAsString(wxC2S_HTML_SYNTAX)) :
        wxString{};
    highlighterTags.DOLCH_ADVERB_BEGIN = IsHighlightingDolchAdverbs() ?
        (GetTextHighlightMethod() == TextHighlight::HighlightBackground) ?
        wxString::Format(L"<span background=\"%s\" foreground=\"%s\">",
            highlighterColors.dolchAdverbsTextHighlightColor.GetAsString(wxC2S_HTML_SYNTAX),
            dolchAdverbsTextColor.GetAsString(wxC2S_HTML_SYNTAX)) :
        wxString::Format(L"<span foreground=\"%s\" weight=\"heavy\">",
            highlighterColors.dolchAdverbsTextHighlightColor.GetAsString(wxC2S_HTML_SYNTAX)) :
        wxString{};
    highlighterTags.DOLCH_ADJECTIVE_BEGIN = IsHighlightingDolchAdjectives() ?
        (GetTextHighlightMethod() == TextHighlight::HighlightBackground) ?
        wxString::Format(L"<span background=\"%s\" foreground=\"%s\">",
            highlighterColors.dolchAdjectivesTextHighlightColor.GetAsString(wxC2S_HTML_SYNTAX),
            dolchAdjectivesTextColor.GetAsString(wxC2S_HTML_SYNTAX)) :
        wxString::Format(L"<span foreground=\"%s\" weight=\"heavy\">",
            highlighterColors.dolchAdjectivesTextHighlightColor.GetAsString(wxC2S_HTML_SYNTAX)) :
        wxString{};
    highlighterTags.DOLCH_VERB_BEGIN = IsHighlightingDolchVerbs() ?
        (GetTextHighlightMethod() == TextHighlight::HighlightBackground) ?
        wxString::Format(L"<span background=\"%s\" foreground=\"%s\">",
            highlighterColors.dolchVerbsTextHighlightColor.GetAsString(wxC2S_HTML_SYNTAX),
            dolchVerbsTextColor.GetAsString(wxC2S_HTML_SYNTAX)) :
        wxString::Format(L"<span foreground=\"%s\" weight=\"heavy\">",
            highlighterColors.dolchVerbsTextHighlightColor.GetAsString(wxC2S_HTML_SYNTAX)) :
        wxString{};
    highlighterTags.DOLCH_NOUN_BEGIN = IsHighlightingDolchNouns() ?
        (GetTextHighlightMethod() == TextHighlight::HighlightBackground) ?
        wxString::Format(L"<span background=\"%s\" foreground=\"%s\">",
            highlighterColors.dolchNounTextHighlightColor.GetAsString(wxC2S_HTML_SYNTAX),
            dolchNounTextColor.GetAsString(wxC2S_HTML_SYNTAX)) :
        wxString::Format(L"<span foreground=\"%s\" weight=\"heavy\">",
            highlighterColors.dolchNounTextHighlightColor.GetAsString(wxC2S_HTML_SYNTAX)) :
        wxString{};

    // these are used for the legend lines because they are always set to use background highlighting
    highlighterTags.HIGHLIGHT_BEGIN_LEGEND =
        wxString::Format(L"<span background=\"%s\">",
            highlighterColors.highlightColor.GetAsString(wxC2S_HTML_SYNTAX));

    highlighterTags.DUPLICATE_HIGHLIGHT_BEGIN_LEGEND =
        wxString::Format(L"<span background=\"%s\">",
            highlighterColors.errorHighlightColor.GetAsString(wxC2S_HTML_SYNTAX));
    highlighterTags.PHRASE_HIGHLIGHT_BEGIN_LEGEND =
        wxString::Format(L"<span background=\"%s\">",
            highlighterColors.styleHighlightColor.GetAsString(wxC2S_HTML_SYNTAX));
    highlighterTags.IGNORE_HIGHLIGHT_BEGIN_LEGEND =
        wxString::Format(L"<span background=\"%s\" strikethrough=\"true\">",
            highlighterColors.excludedTextHighlightColor.GetAsString(wxC2S_HTML_SYNTAX));
    // dolch highlighting
    highlighterTags.DOLCH_CONJUNCTION_BEGIN_LEGEND = IsHighlightingDolchConjunctions() ?
        wxString::Format(L"<span background=\"%s\">",
            highlighterColors.dolchConjunctionsTextHighlightColor.GetAsString(wxC2S_HTML_SYNTAX)) :
        wxString{};
    highlighterTags.DOLCH_PREPOSITIONS_BEGIN_LEGEND = IsHighlightingDolchPrepositions() ?
        wxString::Format(L"<span background=\"%s\">",
            highlighterColors.dolchPrepositionsTextHighlightColor.GetAsString(wxC2S_HTML_SYNTAX)) :
        wxString{};
    highlighterTags.DOLCH_PRONOUN_BEGIN_LEGEND = IsHighlightingDolchPronouns() ?
        wxString::Format(L"<span background=\"%s\">",
            highlighterColors.dolchPronounsTextHighlightColor.GetAsString(wxC2S_HTML_SYNTAX)) :
        wxString{};
    highlighterTags.DOLCH_ADVERB_BEGIN_LEGEND = IsHighlightingDolchAdverbs() ?
        wxString::Format(L"<span background=\"%s\">",
            highlighterColors.dolchAdverbsTextHighlightColor.GetAsString(wxC2S_HTML_SYNTAX)) :
        wxString{};
    highlighterTags.DOLCH_ADJECTIVE_BEGIN_LEGEND = IsHighlightingDolchAdjectives() ?
        wxString::Format(L"<span background=\"%s\">",
            highlighterColors.dolchAdjectivesTextHighlightColor.GetAsString(wxC2S_HTML_SYNTAX)) :
        wxString{};
    highlighterTags.DOLCH_VERB_BEGIN_LEGEND = IsHighlightingDolchVerbs() ?
        wxString::Format(L"<span background=\"%s\">",
            highlighterColors.dolchVerbsTextHighlightColor.GetAsString(wxC2S_HTML_SYNTAX)) :
        wxString{};
    highlighterTags.DOLCH_NOUN_BEGIN_LEGEND = IsHighlightingDolchNouns() ?
        wxString::Format(L"<span background=\"%s\">",
            highlighterColors.dolchNounTextHighlightColor.GetAsString(wxC2S_HTML_SYNTAX)) :
        wxString{};
#endif

 #if defined (__WXMSW__) || defined (__WXOSX__)
    highlighterTags.BOLD_BEGIN = L"{\\b";
    highlighterTags.BOLD_END = L"}";
    highlighterTags.TAB_SYMBOL = L"    ";
    highlighterTags.CRLF = L"\\par\n";
#endif

    return highlighterTags;
    }

//-------------------------------------------------------
std::pair<ProjectDoc::TextLegendLines, size_t> ProjectDoc::BuildLegendLines(const HighlighterTags& highlighterTags)
    {
    [[maybe_unused]] lily_of_the_valley::rtf_encode_text rtfEncode;
    TextLegendLines legendLines;

    wxString currentLegendLabel{ _(L"Excluded text") };
    legendLines.ignoredSentencesLegendLine = wxString::Format(L"%s    %s   %s  %s%s",
            highlighterTags.CRLF,
            highlighterTags.IGNORE_HIGHLIGHT_BEGIN_LEGEND, highlighterTags.HIGHLIGHT_END_LEGEND,
        #ifdef __WXGTK__
            lily_of_the_valley::html_encode_text::simple_encode(
                { currentLegendLabel.wc_str(), currentLegendLabel.length() }).c_str(),
        #else
            rtfEncode({ currentLegendLabel.wc_str(), currentLegendLabel.length() }).c_str(),
        #endif
        highlighterTags.CRLF);
    currentLegendLabel = _(L"3+ syllable words");
    legendLines.hardWordsLegendLine = wxString::Format(L"%s    %s   %s  %s%s",
            highlighterTags.CRLF,
            highlighterTags.HIGHLIGHT_BEGIN_LEGEND, highlighterTags.HIGHLIGHT_END_LEGEND,
        #ifdef __WXGTK__
            lily_of_the_valley::html_encode_text::simple_encode(
                { currentLegendLabel.wc_str(), currentLegendLabel.length() }).c_str(),
        #else
            rtfEncode({ currentLegendLabel.wc_str(), currentLegendLabel.length() }).c_str(),
        #endif
            highlighterTags.CRLF);
    currentLegendLabel = _(L"6+ character words");
    legendLines.longWordsLegendLine = wxString::Format(L"%s    %s   %s  %s%s",
            highlighterTags.CRLF,
            highlighterTags.HIGHLIGHT_BEGIN_LEGEND, highlighterTags.HIGHLIGHT_END_LEGEND,
        #ifdef __WXGTK__
            lily_of_the_valley::html_encode_text::simple_encode(
                { currentLegendLabel.wc_str(), currentLegendLabel.length() }).c_str(),
        #else
            rtfEncode({ currentLegendLabel.wc_str(), currentLegendLabel.length() }).c_str(),
        #endif
            highlighterTags.CRLF);
    currentLegendLabel = _(L"Unfamiliar New Dale-Chall words");
    legendLines.unfamiliarDCWordsLegendLine = wxString::Format(L"%s    %s   %s  %s%s",
            highlighterTags.CRLF,
            highlighterTags.HIGHLIGHT_BEGIN_LEGEND, highlighterTags.HIGHLIGHT_END_LEGEND,
        #ifdef __WXGTK__
            lily_of_the_valley::html_encode_text::simple_encode(
                { currentLegendLabel.wc_str(), currentLegendLabel.length() }).c_str(),
        #else
            rtfEncode({ currentLegendLabel.wc_str(), currentLegendLabel.length() }).c_str(),
        #endif
            highlighterTags.CRLF);
    currentLegendLabel = _(L"Unfamiliar Spache Revised words");
    legendLines.unfamiliarSpacheWordsLegendLine = wxString::Format(L"%s    %s   %s  %s%s",
            highlighterTags.CRLF,
            highlighterTags.HIGHLIGHT_BEGIN_LEGEND, highlighterTags.HIGHLIGHT_END_LEGEND,
        #ifdef __WXGTK__
            lily_of_the_valley::html_encode_text::simple_encode(
                { currentLegendLabel.wc_str(), currentLegendLabel.length() }).c_str(),
        #else
            rtfEncode({ currentLegendLabel.wc_str(), currentLegendLabel.length() }).c_str(),
        #endif
            highlighterTags.CRLF);
    currentLegendLabel = _(L"Unfamiliar Harris-Jacobson words");
    legendLines.unfamiliarHarrisJacobsonWordsLegendLine = wxString::Format(L"%s    %s   %s  %s%s",
            highlighterTags.CRLF,
            highlighterTags.HIGHLIGHT_BEGIN_LEGEND, highlighterTags.HIGHLIGHT_END_LEGEND,
        #ifdef __WXGTK__
            lily_of_the_valley::html_encode_text::simple_encode(
                { currentLegendLabel.wc_str(), currentLegendLabel.length() }).c_str(),
        #else
            rtfEncode({ currentLegendLabel.wc_str(), currentLegendLabel.length() }).c_str(),
        #endif
            highlighterTags.CRLF);
    currentLegendLabel = _(L"Overly-long sentences");
    legendLines.longSentencesLegendLine = wxString::Format(L"%s    %s   %s  %s%s",
            highlighterTags.CRLF,
            highlighterTags.HIGHLIGHT_BEGIN_LEGEND, highlighterTags.HIGHLIGHT_END_LEGEND,
        #ifdef __WXGTK__
            lily_of_the_valley::html_encode_text::simple_encode(
                { currentLegendLabel.wc_str(), currentLegendLabel.length() }).c_str(),
        #else
            rtfEncode({ currentLegendLabel.wc_str(), currentLegendLabel.length() }).c_str(),
        #endif
            highlighterTags.CRLF);
    currentLegendLabel =
        (GetProjectLanguage() == readability::test_language::english_test) ?
        _(L"Errors (Repeated words, wording errors, mismatched articles, and misspellings)") :
        _(L"Errors (Repeated words)");
    legendLines.grammarIssuesLegendLine = wxString::Format(L"%s    %s   %s  %s%s",
            highlighterTags.CRLF,
            highlighterTags.DUPLICATE_HIGHLIGHT_BEGIN_LEGEND, highlighterTags.HIGHLIGHT_END_LEGEND,
        #ifdef __WXGTK__
            lily_of_the_valley::html_encode_text::simple_encode(
                { currentLegendLabel.wc_str(), currentLegendLabel.length() }).c_str(),
        #else
            rtfEncode({ currentLegendLabel.wc_str(), currentLegendLabel.length()}).c_str(),
        #endif
            highlighterTags.CRLF);
    currentLegendLabel =
        _(L"Style (Wordy items, redundant phrases, passive voice, and clich\351s)");
    legendLines.writingStyleLegendLine = (GetProjectLanguage() == readability::test_language::english_test) ?
        wxString::Format(L"%s    %s   %s  %s%s",
            highlighterTags.CRLF,
            highlighterTags.PHRASE_HIGHLIGHT_BEGIN_LEGEND, highlighterTags.HIGHLIGHT_END_LEGEND,
        #ifdef __WXGTK__
            lily_of_the_valley::html_encode_text::simple_encode(
                { currentLegendLabel.wc_str(), currentLegendLabel.length() }).c_str(),
        #else
            rtfEncode({ currentLegendLabel.wc_str(), currentLegendLabel.length() }).c_str(),
        #endif
            highlighterTags.CRLF) :
        wxString{};
    currentLegendLabel = _(L"Dolch conjunctions");
    legendLines.dolch1WordsLegendLine = IsHighlightingDolchConjunctions() ?
        wxString::Format(L"%s    %s   %s  %s%s",
                highlighterTags.CRLF,
                highlighterTags.DOLCH_CONJUNCTION_BEGIN_LEGEND, highlighterTags.HIGHLIGHT_END_LEGEND,
            #ifdef __WXGTK__
                lily_of_the_valley::html_encode_text::simple_encode(
                    { currentLegendLabel.wc_str(), currentLegendLabel.length() }).c_str(),
            #else
                rtfEncode({ currentLegendLabel.wc_str(), currentLegendLabel.length() }).c_str(),
            #endif
                highlighterTags.CRLF) :
        wxString{};
    currentLegendLabel = _(L"Dolch prepositions");
    legendLines.dolch2WordsLegendLine = IsHighlightingDolchPrepositions() ?
        wxString::Format(L"%s    %s   %s  %s%s",
                highlighterTags.CRLF,
                highlighterTags.DOLCH_PREPOSITIONS_BEGIN_LEGEND, highlighterTags.HIGHLIGHT_END_LEGEND,
            #ifdef __WXGTK__
                lily_of_the_valley::html_encode_text::simple_encode(
                    { currentLegendLabel.wc_str(), currentLegendLabel.length() }).c_str(),
            #else
                rtfEncode({ currentLegendLabel.wc_str(), currentLegendLabel.length() }).c_str(),
            #endif
                highlighterTags.CRLF) :
        wxString{};
    currentLegendLabel = _(L"Dolch pronouns");
    legendLines.dolch3WordsLegendLine = IsHighlightingDolchPronouns() ?
        wxString::Format(L"%s    %s   %s  %s%s",
                highlighterTags.CRLF,
                highlighterTags.DOLCH_PRONOUN_BEGIN_LEGEND, highlighterTags.HIGHLIGHT_END_LEGEND,
            #ifdef __WXGTK__
                lily_of_the_valley::html_encode_text::simple_encode(
                    { currentLegendLabel.wc_str(), currentLegendLabel.length() }).c_str(),
            #else
                rtfEncode({ currentLegendLabel.wc_str(), currentLegendLabel.length() }).c_str(),
            #endif
                highlighterTags.CRLF) :
        wxString{};
    currentLegendLabel = _(L"Dolch adverbs");
    legendLines.dolch4WordsLegendLine = IsHighlightingDolchAdverbs() ?
        wxString::Format(L"%s    %s   %s  %s%s",
                highlighterTags.CRLF,
                highlighterTags.DOLCH_ADVERB_BEGIN_LEGEND, highlighterTags.HIGHLIGHT_END_LEGEND,
            #ifdef __WXGTK__
                lily_of_the_valley::html_encode_text::simple_encode(
                    { currentLegendLabel.wc_str(), currentLegendLabel.length() }).c_str(),
            #else
                rtfEncode({ currentLegendLabel.wc_str(), currentLegendLabel.length() }).c_str(),
            #endif
                highlighterTags.CRLF) :
        wxString{};
    currentLegendLabel = _(L"Dolch adjectives");
    legendLines.dolch5WordsLegendLine = IsHighlightingDolchAdjectives() ?
        wxString::Format(L"%s    %s   %s  %s%s",
                highlighterTags.CRLF,
                highlighterTags.DOLCH_ADJECTIVE_BEGIN_LEGEND, highlighterTags.HIGHLIGHT_END_LEGEND,
            #ifdef __WXGTK__
                lily_of_the_valley::html_encode_text::simple_encode(
                    { currentLegendLabel.wc_str(), currentLegendLabel.length() }).c_str(),
            #else
                rtfEncode({ currentLegendLabel.wc_str(), currentLegendLabel.length() }).c_str(),
            #endif
                highlighterTags.CRLF) :
        wxString{};
    currentLegendLabel = _(L"Dolch verbs");
    legendLines.dolchVerbsLegendLine = IsHighlightingDolchVerbs() ?
        wxString::Format(L"%s    %s   %s  %s%s",
                highlighterTags.CRLF,
                highlighterTags.DOLCH_VERB_BEGIN_LEGEND, highlighterTags.HIGHLIGHT_END_LEGEND,
            #ifdef __WXGTK__
                lily_of_the_valley::html_encode_text::simple_encode(
                    { currentLegendLabel.wc_str(), currentLegendLabel.length() }).c_str(),
            #else
                rtfEncode({ currentLegendLabel.wc_str(), currentLegendLabel.length() }).c_str(),
            #endif
                highlighterTags.CRLF) :
        wxString{};
    currentLegendLabel = _(L"Dolch nouns");
    legendLines.dolchNounsLegendLine = IsHighlightingDolchNouns() ?
        wxString::Format(L"%s    %s   %s  %s%s",
                highlighterTags.CRLF,
                highlighterTags.DOLCH_NOUN_BEGIN_LEGEND, highlighterTags.HIGHLIGHT_END_LEGEND,
            #ifdef __WXGTK__
                lily_of_the_valley::html_encode_text::simple_encode(
                    { currentLegendLabel.wc_str(), currentLegendLabel.length() }).c_str(),
            #else
                rtfEncode({ currentLegendLabel.wc_str(), currentLegendLabel.length() }).c_str(),
            #endif
                highlighterTags.CRLF) :
        wxString{};
    currentLegendLabel = _(L"Non-Dolch words");
    legendLines.nonDolchWordsLegendLine = wxString::Format(L"%s    %s   %s  %s%s",
            highlighterTags.CRLF,
            highlighterTags.HIGHLIGHT_BEGIN_LEGEND, highlighterTags.HIGHLIGHT_END_LEGEND,
        #ifdef __WXGTK__
            lily_of_the_valley::html_encode_text::simple_encode(
                { currentLegendLabel.wc_str(), currentLegendLabel.length() }).c_str(),
        #else
            rtfEncode({ currentLegendLabel.wc_str(), currentLegendLabel.length() }).c_str(),
        #endif
            highlighterTags.CRLF);

    return std::make_pair(legendLines,
        (legendLines.ignoredSentencesLegendLine.length() + legendLines.hardWordsLegendLine.length() +
        legendLines.longWordsLegendLine.length() + legendLines.unfamiliarDCWordsLegendLine.length() +
        legendLines.unfamiliarHarrisJacobsonWordsLegendLine.length() +
        legendLines.unfamiliarSpacheWordsLegendLine.length() + legendLines.longSentencesLegendLine.length() +
        legendLines.grammarIssuesLegendLine.length() + legendLines.writingStyleLegendLine.length() +
        legendLines.dolch1WordsLegendLine.length() +
        legendLines.dolch2WordsLegendLine.length() + legendLines.dolch3WordsLegendLine.length() +
        legendLines.dolch4WordsLegendLine.length() +
        legendLines.dolch5WordsLegendLine.length() + legendLines.dolchVerbsLegendLine.length() +
        legendLines.dolchNounsLegendLine.length() +
        legendLines.nonDolchWordsLegendLine.length() +
        1000/*little extra padding for anything added around the legend lines*/) );
    }

//-------------------------------------------------------
wxString ProjectDoc::BuildLegendLine(
    const HighlighterTags& highlighterTags, const wxString& legendStr)
    {
    [[maybe_unused]] lily_of_the_valley::rtf_encode_text rtfEncode;

    return wxString::Format(L"%s    %s   %s  %s%s",
        highlighterTags.CRLF,
        highlighterTags.HIGHLIGHT_BEGIN_LEGEND, highlighterTags.HIGHLIGHT_END_LEGEND,
#ifdef __WXGTK__
        lily_of_the_valley::html_encode_text::simple_encode(
            { legendStr.wc_str(), legendStr.length() }).c_str(),
#else
        rtfEncode({ legendStr.wc_str(), legendStr.length() }).c_str(),
#endif
        highlighterTags.CRLF);
    }

//-------------------------------------------------------
wxString ProjectDoc::BuildLegend(const wxString& legendLine,
    const ProjectDoc::TextLegendLines& legendLines,
    const wxFont& textViewFont)
    {
#if defined (__WXMSW__) || defined (__WXOSX__)
    return wxString::Format(L" \\pard\\fs%u%s%s \\fs%u\\par\n",
        (textViewFont.GetPointSize() - 2) * 2,
        legendLines.ignoredSentencesLegendLine,
        legendLine,
        textViewFont.GetPointSize() * 2);
#else
    return wxString::Format(L"<span size=\"%u\">%s%s</span>\n",
        (textViewFont.GetPointSize() - 2) * 2,
        legendLines.ignoredSentencesLegendLine,
        legendLine);
#endif
    }

//-------------------------------------------------------
ProjectDoc::TextLegends ProjectDoc::BuildLegends(const ProjectDoc::TextLegendLines& legendLines,
    const wxFont& textViewFont)
    {
    TextLegends textLegends;
#if defined (__WXMSW__) || defined (__WXOSX__)
    textLegends.plaintTextWindowLegend = wxString::Format(L" \\pard\\fs%u%s \\fs%u\\par\n",
        (textViewFont.GetPointSize() - 2) * 2,
        legendLines.ignoredSentencesLegendLine,
        textViewFont.GetPointSize() * 2);
    textLegends.hardWordsLegend = wxString::Format(L" \\pard\\fs%u%s%s \\fs%u\\par\n",
        (textViewFont.GetPointSize() - 2) * 2,
        legendLines.ignoredSentencesLegendLine,
        legendLines.hardWordsLegendLine,
        textViewFont.GetPointSize() * 2);
    textLegends.longWordsLegend = wxString::Format(L" \\pard\\fs%u%s%s \\fs%u\\par\n",
        (textViewFont.GetPointSize() - 2) * 2,
        legendLines.ignoredSentencesLegendLine,
        legendLines.longWordsLegendLine,
        textViewFont.GetPointSize() * 2);
    textLegends.unfamiliarDCWordsLegend = wxString::Format(L" \\pard\\fs%u%s%s \\fs%u\\par\n",
        (textViewFont.GetPointSize() - 2) * 2,
        legendLines.ignoredSentencesLegendLine,
        legendLines.unfamiliarDCWordsLegendLine,
        textViewFont.GetPointSize() * 2);
    textLegends.unfamiliarSpacheWordsLegend = wxString::Format(L" \\pard\\fs%u%s%s \\fs%u\\par\n",
        (textViewFont.GetPointSize() - 2) * 2,
        legendLines.ignoredSentencesLegendLine,
        legendLines.unfamiliarSpacheWordsLegendLine,
        textViewFont.GetPointSize() * 2);
    textLegends.unfamiliarHarrisJacobsonWordsLegend = wxString::Format(L" \\pard\\fs%u%s%s \\fs%u\\par\n",
        (textViewFont.GetPointSize() - 2) * 2,
        legendLines.ignoredSentencesLegendLine,
        legendLines.unfamiliarHarrisJacobsonWordsLegendLine,
        textViewFont.GetPointSize() * 2);
    textLegends.dolchWindowLegend = wxString::Format(L" \\pard\\fs%u%s%s%s%s%s%s%s%s \\fs%u\\par\n",
        (textViewFont.GetPointSize() - 2) * 2,
        legendLines.ignoredSentencesLegendLine,
        legendLines.dolch1WordsLegendLine,
        legendLines.dolch2WordsLegendLine,
        legendLines.dolch3WordsLegendLine,
        legendLines.dolch4WordsLegendLine,
        legendLines.dolch5WordsLegendLine,
        legendLines.dolchVerbsLegendLine,
        legendLines.dolchNounsLegendLine,
        textViewFont.GetPointSize() * 2);
    textLegends.nonDolchWordsLegend = wxString::Format(L" \\pard\\fs%u%s%s \\fs%u\\par\n",
        (textViewFont.GetPointSize() - 2) * 2,
        legendLines.ignoredSentencesLegendLine,
        legendLines.nonDolchWordsLegendLine,
        textViewFont.GetPointSize() * 2);
    textLegends.wordinessWindowLegend = wxString::Format(L" \\pard\\fs%u%s%s%s%s \\fs%u\\par\n",
        (textViewFont.GetPointSize() - 2) * 2,
        legendLines.ignoredSentencesLegendLine,
        legendLines.longSentencesLegendLine,
        legendLines.grammarIssuesLegendLine,
        legendLines.writingStyleLegendLine,
        textViewFont.GetPointSize() * 2);

#elif defined(__WXGTK__)
    // note that font "size" in Pango is 1024th of a point
    textLegends.plaintTextWindowLegend = wxString::Format(L"<span size=\"%u\">%s</span>\n",
        ((textViewFont.GetPointSize() - 2) * 1024),
        legendLines.ignoredSentencesLegendLine);
    textLegends.hardWordsLegend = wxString::Format(L"<span size=\"%u\">%s%s</span>\n",
        ((textViewFont.GetPointSize() - 2) * 1024),
        legendLines.ignoredSentencesLegendLine,
        legendLines.hardWordsLegendLine);
    textLegends.longWordsLegend = wxString::Format(L"<span size=\"%u\">%s%s</span>\n",
        ((textViewFont.GetPointSize() - 2) * 1024),
        legendLines.ignoredSentencesLegendLine,
        legendLines.longWordsLegendLine);
    textLegends.unfamiliarDCWordsLegend = wxString::Format(L"<span size=\"%u\">%s%s</span>\n",
        ((textViewFont.GetPointSize() - 2) * 1024),
        legendLines.ignoredSentencesLegendLine,
        legendLines.unfamiliarDCWordsLegendLine);
    textLegends.unfamiliarSpacheWordsLegend = wxString::Format(L"<span size=\"%u\">%s%s</span>\n",
        ((textViewFont.GetPointSize() - 2) * 1024),
        legendLines.ignoredSentencesLegendLine,
        legendLines.unfamiliarSpacheWordsLegendLine);
    textLegends.unfamiliarHarrisJacobsonWordsLegend = wxString::Format(L"<span size=\"%u\">%s%s</span>\n",
        ((textViewFont.GetPointSize() - 2) * 1024),
        legendLines.ignoredSentencesLegendLine,
        legendLines.unfamiliarHarrisJacobsonWordsLegendLine);
    textLegends.dolchWindowLegend = wxString::Format(L"<span size=\"%u\">%s%s%s%s%s%s%s%s</span>\n",
        ((textViewFont.GetPointSize() - 2) * 1024),
        legendLines.ignoredSentencesLegendLine,
        legendLines.dolch1WordsLegendLine,
        legendLines.dolch2WordsLegendLine,
        legendLines.dolch3WordsLegendLine,
        legendLines.dolch4WordsLegendLine,
        legendLines.dolch5WordsLegendLine,
        legendLines.dolchVerbsLegendLine,
        legendLines.dolchNounsLegendLine);
    textLegends.nonDolchWordsLegend = wxString::Format(L"<span size=\"%u\">%s%s</span>\n",
        ((textViewFont.GetPointSize() - 2) * 1024),
        legendLines.ignoredSentencesLegendLine,
        legendLines.nonDolchWordsLegendLine);
    textLegends.wordinessWindowLegend = wxString::Format(L"<span size=\"%u\">%s%s%s%s</span>\n",
        ((textViewFont.GetPointSize() - 2) * 1024),
        legendLines.ignoredSentencesLegendLine,
        legendLines.longSentencesLegendLine,
        legendLines.grammarIssuesLegendLine,
        legendLines.writingStyleLegendLine);
#endif
    return textLegends;
    }

//-------------------------------------------------------
ProjectDoc::TextHeader ProjectDoc::BuildHeader(
    const wxColour& backgroundColor,
    [[maybe_unused]] const HighlighterColors& highlighterColors,
    const wxFont& textViewFont)
    {
    TextHeader textHeaders;
#if defined (__WXMSW__) || defined (__WXOSX__)
    // other formatting
    wxString fontFamily;
    switch (textViewFont.GetFamily())
        {
        case wxFONTFAMILY_DEFAULT:
            fontFamily = L"fnil";
            break;
        case wxFONTFAMILY_DECORATIVE:
            fontFamily = L"fdecor";
            break;
        case wxFONTFAMILY_ROMAN:
            fontFamily = L"froman";
            break;
        case wxFONTFAMILY_SCRIPT:
            fontFamily = L"fscript";
            break;
        case wxFONTFAMILY_SWISS:
            fontFamily = L"fswiss";
            break;
        case wxFONTFAMILY_MODERN:
            fontFamily = L"fmodern";
            break;
        case wxFONTFAMILY_TELETYPE:
            fontFamily = L"ftech";
            break;
        default:
            fontFamily = L"fnil";
        };

    const wxString headerSection =
        wxString::Format(L"{\\rtf1\\ansi\\ansicpg1252\\deff0\\deflang1033{\\fonttbl{\\f0\\%s\\fcharset0 %s;}}",
            fontFamily, textViewFont.GetFaceName());

    const auto [colorTableThemed, mainFontHeaderThemed, ending] =
        BuildColorTable(textViewFont, highlighterColors, backgroundColor);
    textHeaders.endSection = ending;
    textHeaders.colorTable = colorTableThemed;
    textHeaders.mainFontHeader = mainFontHeaderThemed;
    textHeaders.header = headerSection + colorTableThemed + mainFontHeaderThemed;

#elif defined(__WXGTK__)
    textHeaders.endSection = L"</span>";

    textHeaders.header =
        wxString::Format(
            L"<span background=\"%s\" foreground=\"%s\" face=\"%s\" "
            "size=\"%u\" style=\"%s\" weight=\"%s\" underline=\"%s\">\n",
            backgroundColor.GetAsString(wxC2S_HTML_SYNTAX),
            ((backgroundColor.GetLuminance() < .5f) ? L"white" : L"black"),
            textViewFont.GetFaceName(),
            // "size" in Pango is 1024th of a point
            textViewFont.GetPointSize() * 1024,
            (textViewFont.GetStyle() == wxFONTSTYLE_ITALIC) ? _DT(L"italic") : _DT(L"normal"),
            (textViewFont.GetWeight() == wxFONTWEIGHT_BOLD) ? _DT(L"bold") : _DT(L"normal"),
            textViewFont.GetUnderlined() ? _DT(L"single") : _DT(L"none"));;
#endif
    return textHeaders;
    }

//-------------------------------------------------------
void ProjectDoc::DisplayHighlightedText(const wxColour& highlightColor, const wxFont& textViewFont)
    {
    PROFILE();
    if (GetWords() == nullptr)
        { return; }

    try
        {
        ProjectView* view = dynamic_cast<ProjectView*>(GetFirstView());

        // build the general highlighters
        const HighlighterColors highlighterColorsThemed =
            BuildReportColors(highlightColor, GetTextReportBackgroundColor());
        const HighlighterTags highlighterTagsThemed = BuildHighlighterTags(highlightColor, highlighterColorsThemed);

        const HighlighterColors highlighterColorsPaperWhite =
            BuildReportColors(highlightColor, *wxWHITE);
        const HighlighterTags highlighterTagsPaperWhite =
            BuildHighlighterTags(highlightColor, highlighterColorsPaperWhite);

        // build the legends
        const auto [legendLinesThemed, maxLegendSizeThemed] = BuildLegendLines(highlighterTagsThemed);
        const TextLegends textLegendsThemed = BuildLegends(legendLinesThemed, textViewFont);

        const auto [legendLinesPaperWhite, maxLegendSizePaperWhite] = BuildLegendLines(highlighterTagsPaperWhite);
        const TextLegends textLegendsPaperWhite = BuildLegends(legendLinesPaperWhite, textViewFont);

        // build the headers
        const TextHeader textHeaderThemed =
            BuildHeader(GetTextReportBackgroundColor(), highlighterColorsThemed, textViewFont);
        const TextHeader textHeaderPaperWhite = BuildHeader(*wxWHITE, highlighterColorsPaperWhite, textViewFont);

        // specialized highlighters
        SyllableCountGreaterEqualWithHighlighting<word_case_insensitive_no_stem>
            is3PlusSyllablesThemed(3,
                (GetNumeralSyllabicationMethod() == NumeralSyllabize::WholeWordIsOneSyllable),
                highlighterTagsThemed.HIGHLIGHT_BEGIN, highlighterTagsThemed.HIGHLIGHT_END);
        WordLengthGreaterEqualsWithHighlighting<word_case_insensitive_no_stem>
            is6PlusCharsThemed(6, highlighterTagsThemed.HIGHLIGHT_BEGIN, highlighterTagsThemed.HIGHLIGHT_END);
        IsNotFamiliarWordWithHighlighting<word_case_insensitive_no_stem,
                         const word_list,
                         stemming::no_op_stem<word_case_insensitive_no_stem>>
                            isNotDCWordThemed(IsIncludingStockerCatholicSupplement() ?
                                &BaseProject::m_dale_chall_plus_stocker_catholic_word_list :
                                &BaseProject::m_dale_chall_word_list,
                                highlighterTagsThemed.HIGHLIGHT_BEGIN, highlighterTagsThemed.HIGHLIGHT_END,
                            readability::proper_noun_counting_method(GetDaleChallProperNounCountingMethod()));
        IsNotFamiliarWordWithHighlighting<word_case_insensitive_no_stem,
                         const word_list,
                         stemming::no_op_stem<word_case_insensitive_no_stem>>
                            isNotSpacheWordThemed(&m_spache_word_list,
                                highlighterTagsThemed.HIGHLIGHT_BEGIN, highlighterTagsThemed.HIGHLIGHT_END,
                            readability::proper_noun_counting_method::all_proper_nouns_are_familiar);
        IsNotFamiliarWordExcludeNumeralsWithHighlighting<word_case_insensitive_no_stem,
                         const word_list,
                         stemming::no_op_stem<word_case_insensitive_no_stem>>
                            isNotHJWordThemed(&m_harris_jacobson_word_list,
                            highlighterTagsThemed.HIGHLIGHT_BEGIN, highlighterTagsThemed.HIGHLIGHT_END,
                            highlighterTagsThemed.IGNORE_HIGHLIGHT_BEGIN, highlighterTagsThemed.HIGHLIGHT_END,
                            readability::proper_noun_counting_method::all_proper_nouns_are_familiar);
        IsDolchWordWithLevelHighlighting<word_case_insensitive_no_stem>
                            isDolchWordThemed(&m_dolch_word_list,
                                highlighterTagsThemed.DOLCH_CONJUNCTION_BEGIN,
                                    highlighterTagsThemed.DOLCH_PREPOSITIONS_BEGIN,
                                highlighterTagsThemed.DOLCH_PRONOUN_BEGIN,
                                    highlighterTagsThemed.DOLCH_ADVERB_BEGIN,
                                highlighterTagsThemed.DOLCH_ADJECTIVE_BEGIN,
                                    highlighterTagsThemed.DOLCH_VERB_BEGIN,
                                highlighterTagsThemed.DOLCH_NOUN_BEGIN, highlighterTagsThemed.HIGHLIGHT_END);
        IsNotDolchWordWithLevelHighlighting<word_case_insensitive_no_stem>
                            isNotDolchWordThemed(&m_dolch_word_list,
                                highlighterTagsThemed.HIGHLIGHT_BEGIN, highlighterTagsThemed.HIGHLIGHT_END);

        SyllableCountGreaterEqualWithHighlighting<word_case_insensitive_no_stem>
            is3PlusSyllablesPaperWhite(3,
                (GetNumeralSyllabicationMethod() == NumeralSyllabize::WholeWordIsOneSyllable),
                highlighterTagsPaperWhite.HIGHLIGHT_BEGIN, highlighterTagsPaperWhite.HIGHLIGHT_END);
        WordLengthGreaterEqualsWithHighlighting<word_case_insensitive_no_stem>
            is6PlusCharsPaperWhite(6, highlighterTagsPaperWhite.HIGHLIGHT_BEGIN,
                highlighterTagsPaperWhite.HIGHLIGHT_END);
        IsNotFamiliarWordWithHighlighting<word_case_insensitive_no_stem,
            const word_list,
            stemming::no_op_stem<word_case_insensitive_no_stem>>
            isNotDCWordPaperWhite(IsIncludingStockerCatholicSupplement() ?
                &BaseProject::m_dale_chall_plus_stocker_catholic_word_list :
                &BaseProject::m_dale_chall_word_list,
                highlighterTagsPaperWhite.HIGHLIGHT_BEGIN, highlighterTagsPaperWhite.HIGHLIGHT_END,
                readability::proper_noun_counting_method(GetDaleChallProperNounCountingMethod()));
        IsNotFamiliarWordWithHighlighting<word_case_insensitive_no_stem,
            const word_list,
            stemming::no_op_stem<word_case_insensitive_no_stem>>
            isNotSpacheWordPaperWhite(&m_spache_word_list,
                highlighterTagsPaperWhite.HIGHLIGHT_BEGIN, highlighterTagsPaperWhite.HIGHLIGHT_END,
                readability::proper_noun_counting_method::all_proper_nouns_are_familiar);
        IsNotFamiliarWordExcludeNumeralsWithHighlighting<word_case_insensitive_no_stem,
            const word_list,
            stemming::no_op_stem<word_case_insensitive_no_stem>>
            isNotHJWordPaperWhite(&m_harris_jacobson_word_list,
                highlighterTagsPaperWhite.HIGHLIGHT_BEGIN, highlighterTagsPaperWhite.HIGHLIGHT_END,
                highlighterTagsPaperWhite.IGNORE_HIGHLIGHT_BEGIN, highlighterTagsPaperWhite.HIGHLIGHT_END,
                readability::proper_noun_counting_method::all_proper_nouns_are_familiar);
        IsDolchWordWithLevelHighlighting<word_case_insensitive_no_stem>
            isDolchWordPaperWhite(&m_dolch_word_list,
                highlighterTagsPaperWhite.DOLCH_CONJUNCTION_BEGIN,
                highlighterTagsPaperWhite.DOLCH_PREPOSITIONS_BEGIN,
                highlighterTagsPaperWhite.DOLCH_PRONOUN_BEGIN,
                highlighterTagsPaperWhite.DOLCH_ADVERB_BEGIN,
                highlighterTagsPaperWhite.DOLCH_ADJECTIVE_BEGIN,
                highlighterTagsPaperWhite.DOLCH_VERB_BEGIN,
                highlighterTagsPaperWhite.DOLCH_NOUN_BEGIN, highlighterTagsPaperWhite.HIGHLIGHT_END);
        IsNotDolchWordWithLevelHighlighting<word_case_insensitive_no_stem>
            isNotDolchWordPaperWhite(&m_dolch_word_list,
                highlighterTagsPaperWhite.HIGHLIGHT_BEGIN, highlighterTagsPaperWhite.HIGHLIGHT_END);

        // get the full count of all chars (including ignored ones) and their punctuation
        const size_t fullCharWithPunctCount = std::accumulate(GetWords()->get_words().begin(),
                    GetWords()->get_words().end(), static_cast<size_t>(0),
                    add_word_size<word_case_insensitive_no_stem>() );
        size_t paragraphLeadingLines = 0;
        for (const auto& para : GetWords()->get_paragraphs())
            { paragraphLeadingLines += para.get_leading_end_of_line_count(); }
        /* Set the size of the buffers for the worst case scenario that every word is highlighted.
           Note that for encoding, the worst case scenario is a char to be expanded to 7 chars. In RTF,
           a unicode value will be "\u0000?", so assume the worst that every character and punctuation
           may be expanded into that.*/
        // some characters have to be encoded for RTF or Pango
        const size_t textBufferLength = fullCharWithPunctCount*7 +
            textHeaderThemed.header.length() + textHeaderThemed.colorTable.length() +
            textHeaderThemed.mainFontHeader.length() +
            // two spaces after each sentence, and ending punctuations
            (GetWords()->get_sentence_count()*3) +
            (GetWords()->get_paragraph_count() *
                (highlighterTagsThemed.CRLF.length() + highlighterTagsThemed.TAB_SYMBOL.length())) +
            (paragraphLeadingLines*(highlighterTagsThemed.CRLF.length() +
                highlighterTagsThemed.TAB_SYMBOL.length())) +
            // some punctuation has to be escaped or encoded for RTF or Pango
            (GetWords()->get_punctuation().size()*7) +
            // gets the spaces between each word too
            (GetWords()->get_word_count() *
                (highlighterTagsThemed.HIGHLIGHT_BEGIN.length() +
                    highlighterTagsThemed.HIGHLIGHT_END.length()+2)) +
            /* 3 spaces, 2 parenthesis, and 3 spots for the number */
            (GetWords()->get_sentence_count() *
                (highlighterTagsThemed.BOLD_BEGIN.length() + highlighterTagsThemed.BOLD_END.length()+8)) +
            textHeaderThemed.endSection.length() + maxLegendSizeThemed + 1;

        auto docText = std::make_unique<wchar_t[]>(textBufferLength+1);

        const bool textBeingExcluded =
            (GetInvalidSentenceMethod() == InvalidSentence::ExcludeFromAnalysis ||
             GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings);

        const auto setFormattedTextAndRestoreInsertionPoint = [](FormattedTextCtrl* textWindow, const wchar_t* formattedText)
            {
            const auto cursorPos = textWindow->GetInsertionPoint();
            textWindow->SetFormattedText(formattedText);
            textWindow->SetInsertionPoint(cursorPos);
            textWindow->ShowPosition(cursorPos);
            };

        // creates and loads text into a formatted text window
        const auto buildTextWindow = [this, view, textBeingExcluded, textBufferLength,
                                      &textHeaderThemed, &textHeaderPaperWhite,
                                      &highlighterTagsThemed, &highlighterTagsPaperWhite,
                                      &docText, &setFormattedTextAndRestoreInsertionPoint]
                                     (FormattedTextCtrl* textWindow, const int ID, const wxString& label,
                                      auto& highlighter, const wxString& legend)
            {
            if (!textWindow)
                {
                textWindow =
                    new FormattedTextCtrl(view->GetSplitter(), ID, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
                textWindow->Hide();
                textWindow->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_TEXT_MENU"));
                textWindow->SetLabel(label);
                textWindow->SetName(label);
                }
            UpdateTextWindowOptions(textWindow);
            [[maybe_unused]] const size_t textLength = FormatWordCollectionHighlightedWords(GetWords(),
                                        highlighter,
                                        docText.get(), textBufferLength,
                                        textHeaderThemed.header, textHeaderThemed.endSection, legend,
                                        highlighterTagsThemed.IGNORE_HIGHLIGHT_BEGIN,
                                        highlighterTagsThemed.HIGHLIGHT_END,
                                        highlighterTagsThemed.TAB_SYMBOL, highlighterTagsThemed.CRLF,
                                        textBeingExcluded,
                                        GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings,
                                        textBeingExcluded,
                                    #ifdef __WXGTK__
                                        false
                                    #else
                                        true
                                    #endif
                                        );
#ifndef __WXGTK__
            // not necessary on Linux and causes an assert
            textWindow->SetMaxLength(static_cast<unsigned long>(textLength));
#endif
            setFormattedTextAndRestoreInsertionPoint(textWindow, docText.get());

#ifdef DEBUG_EXPERIMENTAL_CODE
            const auto tempFilePath = wxFileName::CreateTempFileName(label);
            wxFile textWindowDump(tempFilePath, wxFile::OpenMode::write);
            if (textWindowDump.IsOpened())
                {
                textWindowDump.Write(docText.get());
                wxLogDebug(L"Text view written to: %s", tempFilePath);
                }
#endif

            highlighter.Reset();

            FormatWordCollectionHighlightedWords(GetWords(),
                                    highlighter,
                                    docText.get(), textBufferLength,
                                    textHeaderPaperWhite.header, textHeaderPaperWhite.endSection, legend,
                                    highlighterTagsPaperWhite.IGNORE_HIGHLIGHT_BEGIN,
                                    highlighterTagsPaperWhite.HIGHLIGHT_END,
                                    highlighterTagsPaperWhite.TAB_SYMBOL, highlighterTagsPaperWhite.CRLF,
                                    textBeingExcluded,
                                    GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings,
                                    textBeingExcluded,
                                #ifdef __WXGTK__
                                    false
                                #else
                                    true
                                #endif
                                    );
            textWindow->SetUnthemedFormattedText(docText.get());

#ifdef DEBUG_EXPERIMENTAL_CODE
            const auto tempFilePathPaper = wxFileName::CreateTempFileName(label + L" Paper White");
            wxFile textWindowDumpPaper(tempFilePathPaper, wxFile::OpenMode::write);
            if (textWindowDumpPaper.IsOpened())
                {
                textWindowDumpPaper.Write(docText.get());
                wxLogDebug(L"Text view written to: %s", tempFilePathPaper);
                }
#endif

            return textWindow;
            };

        // 3+ syllable highlighted words
            {
            FormattedTextCtrl* textWindow =
                dynamic_cast<FormattedTextCtrl*>(view->GetWordsBreakdownView().FindWindowById(
                    BaseProjectView::HARD_WORDS_TEXT_PAGE_ID));
            // always included for any language
                {
                textWindow = buildTextWindow(textWindow, BaseProjectView::HARD_WORDS_TEXT_PAGE_ID,
                    BaseProjectView::GetThreeSyllableReportWordsLabel(), is3PlusSyllablesThemed,
                        textLegendsThemed.hardWordsLegend);
                }
            if (GetWordsBreakdownInfo().Is3PlusSyllablesEnabled() && GetTotalUnique3PlusSyllableWords() > 0)
                {
                const auto buddyWindowPosition =
                    view->GetWordsBreakdownView().FindWindowPositionById(BaseProjectView::HARD_WORDS_LIST_PAGE_ID);
                view->GetWordsBreakdownView().InsertWindow(
                    (buddyWindowPosition != wxNOT_FOUND) ? buddyWindowPosition+1 : 0, textWindow);
                }
            else
                { view->GetWordsBreakdownView().RemoveWindowById(BaseProjectView::HARD_WORDS_TEXT_PAGE_ID); }
            }

        // 6+ character highlighted words
            {
            FormattedTextCtrl* textWindow =
                dynamic_cast<FormattedTextCtrl*>(view->GetWordsBreakdownView().FindWindowById(
                    BaseProjectView::LONG_WORDS_TEXT_PAGE_ID));
            // always included for any language
                {
                textWindow = buildTextWindow(textWindow, BaseProjectView::LONG_WORDS_TEXT_PAGE_ID,
                    BaseProjectView::GetSixCharWordsReportLabel(),
                        is6PlusCharsThemed, textLegendsThemed.longWordsLegend);
                }
            if (GetWordsBreakdownInfo().Is6PlusCharacterEnabled() && GetTotalUnique6CharsPlusWords() > 0)
                {
                const auto buddyWindowPosition =
                    view->GetWordsBreakdownView().FindWindowPositionById(BaseProjectView::LONG_WORDS_LIST_PAGE_ID);
                view->GetWordsBreakdownView().InsertWindow(
                    (buddyWindowPosition != wxNOT_FOUND) ? buddyWindowPosition+1 : 0, textWindow);
                }
            else
                { view->GetWordsBreakdownView().RemoveWindowById(BaseProjectView::LONG_WORDS_TEXT_PAGE_ID); }
            }

        // DC highlighted words
        m_dcTextWindow = dynamic_cast<FormattedTextCtrl*>(view->GetWordsBreakdownView().FindWindowById(
            BaseProjectView::DC_WORDS_TEXT_PAGE_ID));
        // We will always format this for an English project, even if test is not included.
        // That way, if the test is included later we just have to show it.
        if (GetProjectLanguage() == readability::test_language::english_test)
            {
            if (!m_dcTextWindow)
                {
                m_dcTextWindow = new FormattedTextCtrl(view->GetSplitter(),
                    BaseProjectView::DC_WORDS_TEXT_PAGE_ID, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
                m_dcTextWindow->Hide();
                m_dcTextWindow->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_TEXT_MENU"));
                m_dcTextWindow->SetLabel(_(L"Dale-Chall (Unfamiliar) Report"));
                m_dcTextWindow->SetName(_(L"Dale-Chall (Unfamiliar) Report"));
                }
            UpdateTextWindowOptions(m_dcTextWindow);
            [[maybe_unused]] size_t textLength{ 0 };
            if (GetDaleChallTextExclusionMode() ==
                SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings)
                {
                textLength = FormatWordCollectionHighlightedWords(GetWords(),
                                        isNotDCWordThemed,
                                        docText.get(), textBufferLength,
                                        textHeaderThemed.header, textHeaderThemed.endSection,
                                        textLegendsThemed.unfamiliarDCWordsLegend,
                                        highlighterTagsThemed.IGNORE_HIGHLIGHT_BEGIN,
                                        highlighterTagsThemed.HIGHLIGHT_END,
                                        highlighterTagsThemed.TAB_SYMBOL, highlighterTagsThemed.CRLF,
                                        // forcibly exclude lists but include headers, invalid words will also be valid
                                        true,
                                        true,
                                        false,
                                    #ifdef __WXGTK__
                                        false
                                    #else
                                        true
                                    #endif
                                        );
                }
            else
                {
                textLength = FormatWordCollectionHighlightedWords(GetWords(),
                                        isNotDCWordThemed,
                                        docText.get(), textBufferLength,
                                        textHeaderThemed.header, textHeaderThemed.endSection,
                                        textLegendsThemed.unfamiliarDCWordsLegend,
                                        highlighterTagsThemed.IGNORE_HIGHLIGHT_BEGIN,
                                        highlighterTagsThemed.HIGHLIGHT_END,
                                        highlighterTagsThemed.TAB_SYMBOL, highlighterTagsThemed.CRLF,
                                        textBeingExcluded,
                                        GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings,
                                        textBeingExcluded,
                                    #ifdef __WXGTK__
                                        false
                                    #else
                                        true
                                    #endif
                                        );
                }
#ifndef __WXGTK__
            m_dcTextWindow->SetMaxLength(static_cast<unsigned long>(textLength));
#endif
            setFormattedTextAndRestoreInsertionPoint(m_dcTextWindow, docText.get());

            isNotDCWordThemed.Reset();
            isNotDCWordPaperWhite.Reset();

            if (GetDaleChallTextExclusionMode() ==
                SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings)
                {
                FormatWordCollectionHighlightedWords(GetWords(),
                                        isNotDCWordPaperWhite,
                                        docText.get(), textBufferLength,
                                        textHeaderPaperWhite.header, textHeaderPaperWhite.endSection,
                                        textLegendsPaperWhite.unfamiliarDCWordsLegend,
                                        highlighterTagsPaperWhite.IGNORE_HIGHLIGHT_BEGIN,
                                        highlighterTagsPaperWhite.HIGHLIGHT_END,
                                        highlighterTagsPaperWhite.TAB_SYMBOL, highlighterTagsPaperWhite.CRLF,
                                        // forcibly exclude lists but include headers, invalid words will also be valid
                                        true,
                                        true,
                                        false,
                                    #ifdef __WXGTK__
                                        false
                                    #else
                                        true
                                    #endif
                                        );
                }
            else
                {
                FormatWordCollectionHighlightedWords(GetWords(),
                                        isNotDCWordPaperWhite,
                                        docText.get(), textBufferLength,
                                        textHeaderPaperWhite.header, textHeaderPaperWhite.endSection,
                                        textLegendsPaperWhite.unfamiliarDCWordsLegend,
                                        highlighterTagsPaperWhite.IGNORE_HIGHLIGHT_BEGIN,
                                        highlighterTagsPaperWhite.HIGHLIGHT_END,
                                        highlighterTagsPaperWhite.TAB_SYMBOL, highlighterTagsPaperWhite.CRLF,
                                        textBeingExcluded,
                                        GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings,
                                        textBeingExcluded,
                                    #ifdef __WXGTK__
                                        false
                                    #else
                                        true
                                    #endif
                                        );
                }
            m_dcTextWindow->SetUnthemedFormattedText(docText.get());
            }
        if (GetWordsBreakdownInfo().IsDCUnfamiliarEnabled() && IsDaleChallLikeTestIncluded() )
            {
            const auto buddyWindowPosition = view->GetWordsBreakdownView().FindWindowPositionById(
                BaseProjectView::DC_WORDS_LIST_PAGE_ID);
            view->GetWordsBreakdownView().InsertWindow((buddyWindowPosition != wxNOT_FOUND) ?
                buddyWindowPosition+1 : 0, m_dcTextWindow);
            }
        else
            { view->GetWordsBreakdownView().RemoveWindowById(BaseProjectView::DC_WORDS_TEXT_PAGE_ID); }

        // Spache highlighted words
        m_spacheTextWindow = dynamic_cast<FormattedTextCtrl*>(view->GetWordsBreakdownView().FindWindowById(
            BaseProjectView::SPACHE_WORDS_TEXT_PAGE_ID));
        // We will always format this for an English project, even if test is not included.
        // That way, if the test is included later we just have to show it.
        if (GetProjectLanguage() == readability::test_language::english_test)
            {
            m_spacheTextWindow = buildTextWindow(m_spacheTextWindow, BaseProjectView::SPACHE_WORDS_TEXT_PAGE_ID,
                _(L"Spache (Unfamiliar) Report"), isNotSpacheWordThemed, textLegendsThemed.unfamiliarSpacheWordsLegend);
            }
        if (GetWordsBreakdownInfo().IsSpacheUnfamiliarEnabled() &&
            GetReadabilityTests().is_test_included(ReadabilityMessages::SPACHE()) )
            {
            const auto buddyWindowPosition = view->GetWordsBreakdownView().FindWindowPositionById(
                BaseProjectView::SPACHE_WORDS_LIST_PAGE_ID);
            view->GetWordsBreakdownView().InsertWindow((buddyWindowPosition != wxNOT_FOUND) ?
                buddyWindowPosition+1 : 0, m_spacheTextWindow);
            }
        else
            { view->GetWordsBreakdownView().RemoveWindowById(BaseProjectView::SPACHE_WORDS_TEXT_PAGE_ID); }

        // HJ highlighted words
        m_hjTextWindow = dynamic_cast<FormattedTextCtrl*>(view->GetWordsBreakdownView().FindWindowById(
            BaseProjectView::HARRIS_JACOBSON_WORDS_TEXT_PAGE_ID));
        // We will always format this for an English project, even if test is not included.
        // That way, if the test is included later we just have to show it.
        if (GetProjectLanguage() == readability::test_language::english_test)
            {
            if (!m_hjTextWindow)
                {
                m_hjTextWindow = new FormattedTextCtrl(view->GetSplitter(),
                    BaseProjectView::HARRIS_JACOBSON_WORDS_TEXT_PAGE_ID, wxDefaultPosition,
                    wxDefaultSize, wxTE_READONLY);
                m_hjTextWindow->Hide();
                m_hjTextWindow->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_TEXT_MENU"));
                m_hjTextWindow->SetLabel(_(L"Harris-Jacobson (Unfamiliar) Report"));
                m_hjTextWindow->SetName(_(L"Harris-Jacobson (Unfamiliar) Report"));
                }
            UpdateTextWindowOptions(m_hjTextWindow);
            [[maybe_unused]] size_t textLength = 0;
            if (GetHarrisJacobsonTextExclusionMode() ==
                SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings)
                {
                textLength = FormatWordCollectionHighlightedWords(GetWords(),
                                    isNotHJWordThemed,
                                    docText.get(), textBufferLength,
                                    textHeaderThemed.header, textHeaderThemed.endSection,
                                    textLegendsThemed.unfamiliarHarrisJacobsonWordsLegend,
                                    highlighterTagsThemed.IGNORE_HIGHLIGHT_BEGIN, highlighterTagsThemed.HIGHLIGHT_END,
                                    highlighterTagsThemed.TAB_SYMBOL, highlighterTagsThemed.CRLF,
                                    // HJ explicitly states what to exclude, so always show what it is
                                    // excluding in this window
                                    true,
                                    true,
                                    false,
                                #ifdef __WXGTK__
                                    false
                                #else
                                    true
                                #endif
                                    );
                }
            else
                {
                textLength = FormatWordCollectionHighlightedWords(GetWords(),
                                    isNotHJWordThemed,
                                    docText.get(), textBufferLength,
                                    textHeaderThemed.header, textHeaderThemed.endSection,
                                    textLegendsThemed.unfamiliarHarrisJacobsonWordsLegend,
                                    highlighterTagsThemed.IGNORE_HIGHLIGHT_BEGIN, highlighterTagsThemed.HIGHLIGHT_END,
                                    highlighterTagsThemed.TAB_SYMBOL, highlighterTagsThemed.CRLF,
                                    textBeingExcluded,
                                    GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings,
                                    textBeingExcluded,
                                #ifdef __WXGTK__
                                    false
                                #else
                                    true
                                #endif
                                    );
                }
#ifndef __WXGTK__
            m_hjTextWindow->SetMaxLength(static_cast<unsigned long>(textLength));
#endif
            setFormattedTextAndRestoreInsertionPoint(m_hjTextWindow, docText.get());

            isNotHJWordThemed.Reset();
            isNotHJWordPaperWhite.Reset();

            if (GetHarrisJacobsonTextExclusionMode() ==
                SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings)
                {
                FormatWordCollectionHighlightedWords(GetWords(),
                                    isNotHJWordPaperWhite,
                                    docText.get(), textBufferLength,
                                    textHeaderPaperWhite.header, textHeaderPaperWhite.endSection,
                                    textLegendsPaperWhite.unfamiliarHarrisJacobsonWordsLegend,
                                    highlighterTagsPaperWhite.IGNORE_HIGHLIGHT_BEGIN,
                                    highlighterTagsPaperWhite.HIGHLIGHT_END,
                                    highlighterTagsPaperWhite.TAB_SYMBOL, highlighterTagsPaperWhite.CRLF,
                                    // HJ explicitly states what to exclude, so always show what it
                                    // is excluding in this window
                                    true,
                                    true,
                                    false,
                                #ifdef __WXGTK__
                                    false
                                #else
                                    true
                                #endif
                                    );
                }
            else
                {
                FormatWordCollectionHighlightedWords(GetWords(),
                                    isNotHJWordPaperWhite,
                                    docText.get(), textBufferLength,
                                    textHeaderPaperWhite.header, textHeaderPaperWhite.endSection,
                                    textLegendsPaperWhite.unfamiliarHarrisJacobsonWordsLegend,
                                    highlighterTagsPaperWhite.IGNORE_HIGHLIGHT_BEGIN,
                                    highlighterTagsPaperWhite.HIGHLIGHT_END,
                                    highlighterTagsPaperWhite.TAB_SYMBOL, highlighterTagsPaperWhite.CRLF,
                                    textBeingExcluded,
                                    GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings,
                                    textBeingExcluded,
                                #ifdef __WXGTK__
                                    false
                                #else
                                    true
                                #endif
                                    );
                }
            m_hjTextWindow->SetUnthemedFormattedText(docText.get());
            }
        if (GetWordsBreakdownInfo().IsHarrisJacobsonUnfamiliarEnabled() &&
            GetReadabilityTests().is_test_included(ReadabilityMessages::HARRIS_JACOBSON()) )
            {
            const auto buddyWindowPosition = view->GetWordsBreakdownView().FindWindowPositionById(
                BaseProjectView::HARRIS_JACOBSON_WORDS_LIST_PAGE_ID);
            view->GetWordsBreakdownView().InsertWindow((buddyWindowPosition != wxNOT_FOUND) ?
                buddyWindowPosition+1 : 0, m_hjTextWindow);
            }
        else
            { view->GetWordsBreakdownView().RemoveWindowById(BaseProjectView::HARRIS_JACOBSON_WORDS_TEXT_PAGE_ID); }

        // go through the custom readability tests
        for(std::vector<CustomReadabilityTestInterface>::iterator pos = GetCustTestsInUse().begin();
            pos != GetCustTestsInUse().end();
            ++pos)
            {
            const wxString windowLabel(
                wxString::Format( _(L"%s (Unfamiliar) Report"), pos->GetIterator()->get_name().c_str()));
            FormattedTextCtrl* textWindow =
                dynamic_cast<FormattedTextCtrl*>(view->GetWordsBreakdownView().FindWindowById(
                    pos->GetIterator()->get_interface_id(), CLASSINFO(FormattedTextCtrl)) );

            if (pos->GetIterator()->is_using_familiar_words())
                {
                if (!textWindow)
                    {
                    textWindow =
                        new FormattedTextCtrl(view->GetSplitter(), pos->GetIterator()->get_interface_id(),
                                              wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
                    textWindow->Hide();
                    textWindow->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_TEXT_MENU"));
                    textWindow->SetLabel(windowLabel);
                    textWindow->SetName(windowLabel);
                    // find respective list and add it beneath that
                    auto buddyWindowPosition =
                        view->GetWordsBreakdownView().FindWindowPositionById(
                            pos->GetIterator()->get_interface_id(), CLASSINFO(ListCtrlEx));
                    view->GetWordsBreakdownView().InsertWindow(
                        (buddyWindowPosition != wxNOT_FOUND) ? buddyWindowPosition+1 : 0, textWindow);
                    }
                UpdateTextWindowOptions(textWindow);

                const wxString unfamiliarWordsLegendLineThemed =
                    BuildLegendLine(highlighterTagsThemed,
                        wxString::Format(_(L"Unfamiliar %s words"), pos->GetIterator()->get_name().c_str()) );
                const wxString unfamiliarWordsLegendThemed =
                    BuildLegend(unfamiliarWordsLegendLineThemed, legendLinesThemed, textViewFont);

                IsNotCustomFamiliarWordWithHighlighting<std::vector<CustomReadabilityTestInterface>::iterator>
                    notCustomWordThemed(pos,
                        highlighterTagsThemed.HIGHLIGHT_BEGIN, highlighterTagsThemed.HIGHLIGHT_END);
                IsNotCustomFamiliarWordExcludeNumeralsWithHighlighting<
                    std::vector<CustomReadabilityTestInterface>::iterator>
                    notCustomWordExcludeNumberalsThemed(pos,
                        highlighterTagsThemed.HIGHLIGHT_BEGIN, highlighterTagsThemed.HIGHLIGHT_END,
                        highlighterTagsThemed.IGNORE_HIGHLIGHT_BEGIN, highlighterTagsThemed.HIGHLIGHT_END);

                IsNotCustomFamiliarWordWithHighlighting<std::vector<CustomReadabilityTestInterface>::iterator>
                    notCustomWordPaperWhite(pos,
                        highlighterTagsPaperWhite.HIGHLIGHT_BEGIN, highlighterTagsPaperWhite.HIGHLIGHT_END);
                IsNotCustomFamiliarWordExcludeNumeralsWithHighlighting<
                    std::vector<CustomReadabilityTestInterface>::iterator>
                    notCustomWordExcludeNumberalsPaperWhite(pos,
                        highlighterTagsPaperWhite.HIGHLIGHT_BEGIN, highlighterTagsPaperWhite.HIGHLIGHT_END,
                        highlighterTagsPaperWhite.IGNORE_HIGHLIGHT_BEGIN,
                        highlighterTagsPaperWhite.HIGHLIGHT_END);

                // clear cached first instances of proper nouns
                notCustomWordThemed.Reset();
                notCustomWordExcludeNumberalsThemed.Reset();

                [[maybe_unused]] size_t textLength = 0;
                // special text exclusion logic is used for Custom HJ and DC tests
                if ((pos->IsHarrisJacobsonFormula() &&
                     GetHarrisJacobsonTextExclusionMode() ==
                        SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ||
                    (pos->IsDaleChallFormula() &&
                     GetDaleChallTextExclusionMode() ==
                        SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings))
                    {
                    if (pos->IsHarrisJacobsonFormula())
                        {
                        textLength = FormatWordCollectionHighlightedWords(GetWords(),
                                notCustomWordExcludeNumberalsThemed,
                                docText.get(), textBufferLength,
                                textHeaderThemed.header, textHeaderThemed.endSection,
                                unfamiliarWordsLegendThemed,
                                highlighterTagsThemed.IGNORE_HIGHLIGHT_BEGIN, highlighterTagsThemed.HIGHLIGHT_END,
                                highlighterTagsThemed.TAB_SYMBOL, highlighterTagsThemed.CRLF,
                                // forcibly exclude lists but include headers, invalid words will also be valid
                                true,
                                true,
                                false,
                            #ifdef __WXGTK__
                                false
                            #else
                                true
                            #endif
                                );
                        }
                    else
                        {
                        textLength = FormatWordCollectionHighlightedWords(GetWords(),
                                notCustomWordThemed,
                                docText.get(), textBufferLength,
                                textHeaderThemed.header, textHeaderThemed.endSection,
                                unfamiliarWordsLegendThemed,
                                highlighterTagsThemed.IGNORE_HIGHLIGHT_BEGIN, highlighterTagsThemed.HIGHLIGHT_END,
                                highlighterTagsThemed.TAB_SYMBOL, highlighterTagsThemed.CRLF,
                                // forcibly exclude lists but include headers, invalid words will also be valid
                                true,
                                true,
                                false,
                            #ifdef __WXGTK__
                                false
                            #else
                                true
                            #endif
                                );
                        }
                    }
                else
                    {
                    if (pos->IsHarrisJacobsonFormula())
                        {
                        textLength = FormatWordCollectionHighlightedWords(GetWords(),
                                notCustomWordExcludeNumberalsThemed,
                                docText.get(), textBufferLength,
                                textHeaderThemed.header, textHeaderThemed.endSection,
                                unfamiliarWordsLegendThemed,
                                highlighterTagsThemed.IGNORE_HIGHLIGHT_BEGIN, highlighterTagsThemed.HIGHLIGHT_END,
                                highlighterTagsThemed.TAB_SYMBOL, highlighterTagsThemed.CRLF,
                                textBeingExcluded,
                                GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings,
                                textBeingExcluded,
                            #ifdef __WXGTK__
                                false
                            #else
                                true
                            #endif
                                );
                        }
                    else
                        {
                        textLength = FormatWordCollectionHighlightedWords(GetWords(),
                                notCustomWordThemed,
                                docText.get(), textBufferLength,
                                textHeaderThemed.header, textHeaderThemed.endSection,
                                unfamiliarWordsLegendThemed,
                                highlighterTagsThemed.IGNORE_HIGHLIGHT_BEGIN, highlighterTagsThemed.HIGHLIGHT_END,
                                highlighterTagsThemed.TAB_SYMBOL, highlighterTagsThemed.CRLF,
                                textBeingExcluded,
                                GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings,
                                textBeingExcluded,
                            #ifdef __WXGTK__
                                false
                            #else
                                true
                            #endif
                                );
                        }
                    }
#ifndef __WXGTK__
                textWindow->SetMaxLength(static_cast<unsigned long>(textLength));
#endif
                setFormattedTextAndRestoreInsertionPoint(textWindow, docText.get());

                notCustomWordThemed.Reset();
                notCustomWordExcludeNumberalsThemed.Reset();

                notCustomWordPaperWhite.Reset();
                notCustomWordExcludeNumberalsPaperWhite.Reset();

                const wxString unfamiliarWordsLegendLinePaperWhite =
                    BuildLegendLine(highlighterTagsPaperWhite,
                        wxString::Format(_(L"Unfamiliar %s words"), pos->GetIterator()->get_name().c_str()));
                const wxString unfamiliarWordsLegendPaperWhite =
                    BuildLegend(unfamiliarWordsLegendLinePaperWhite, legendLinesPaperWhite, textViewFont);

                if ((pos->IsHarrisJacobsonFormula() &&
                     GetHarrisJacobsonTextExclusionMode() ==
                        SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ||
                    (pos->IsDaleChallFormula() &&
                     GetDaleChallTextExclusionMode() ==
                        SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings))
                    {
                    if (pos->IsHarrisJacobsonFormula())
                        {
                        FormatWordCollectionHighlightedWords(GetWords(),
                                notCustomWordExcludeNumberalsPaperWhite,
                                docText.get(), textBufferLength,
                                textHeaderPaperWhite.header, textHeaderPaperWhite.endSection,
                                unfamiliarWordsLegendPaperWhite,
                                highlighterTagsPaperWhite.IGNORE_HIGHLIGHT_BEGIN,
                                highlighterTagsPaperWhite.HIGHLIGHT_END,
                                highlighterTagsPaperWhite.TAB_SYMBOL, highlighterTagsPaperWhite.CRLF,
                                // forcibly exclude lists but include headers, invalid words will also be valid
                                true,
                                true,
                                false,
                            #ifdef __WXGTK__
                                false
                            #else
                                true
                            #endif
                                );
                        }
                    else
                        {
                        FormatWordCollectionHighlightedWords(GetWords(),
                                notCustomWordPaperWhite,
                                docText.get(), textBufferLength,
                                textHeaderPaperWhite.header, textHeaderPaperWhite.endSection,
                                unfamiliarWordsLegendPaperWhite,
                                highlighterTagsPaperWhite.IGNORE_HIGHLIGHT_BEGIN,
                                highlighterTagsPaperWhite.HIGHLIGHT_END,
                                highlighterTagsPaperWhite.TAB_SYMBOL, highlighterTagsPaperWhite.CRLF,
                                // forcibly exclude lists but include headers, invalid words will also be valid
                                true,
                                true,
                                false,
                            #ifdef __WXGTK__
                                false
                            #else
                                true
                            #endif
                                );
                        }
                    }
                else
                    {
                    if (pos->IsHarrisJacobsonFormula())
                        {
                        FormatWordCollectionHighlightedWords(GetWords(),
                                notCustomWordExcludeNumberalsPaperWhite,
                                docText.get(), textBufferLength,
                                textHeaderPaperWhite.header, textHeaderPaperWhite.endSection,
                                unfamiliarWordsLegendPaperWhite,
                                highlighterTagsPaperWhite.IGNORE_HIGHLIGHT_BEGIN,
                                highlighterTagsPaperWhite.HIGHLIGHT_END,
                                highlighterTagsPaperWhite.TAB_SYMBOL, highlighterTagsPaperWhite.CRLF,
                                textBeingExcluded,
                                GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings,
                                textBeingExcluded,
                            #ifdef __WXGTK__
                                false
                            #else
                                true
                            #endif
                                );
                        }
                    else
                        {
                        FormatWordCollectionHighlightedWords(GetWords(),
                                notCustomWordPaperWhite,
                                docText.get(), textBufferLength,
                                textHeaderPaperWhite.header, textHeaderPaperWhite.endSection,
                                unfamiliarWordsLegendPaperWhite,
                                highlighterTagsPaperWhite.IGNORE_HIGHLIGHT_BEGIN,
                                highlighterTagsPaperWhite.HIGHLIGHT_END,
                                highlighterTagsPaperWhite.TAB_SYMBOL, highlighterTagsPaperWhite.CRLF,
                                textBeingExcluded,
                                GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings,
                                textBeingExcluded,
                            #ifdef __WXGTK__
                                false
                            #else
                                true
                            #endif
                                );
                        }
                    }
                textWindow->SetUnthemedFormattedText(docText.get());
                }
            else
                { view->GetWordsBreakdownView().RemoveWindowById(pos->GetIterator()->get_interface_id()); }
            }

        // grammar issues highlighted
        if (GetGrammarInfo().IsHighlightedReportEnabled())
            {
            // display this in the Grammar section
            FormattedTextCtrl* textWindow = dynamic_cast<FormattedTextCtrl*>(view->GetGrammarView().FindWindowById(
                BaseProjectView::LONG_SENTENCES_AND_WORDINESS_TEXT_PAGE_ID));
            if (!textWindow)
                {
                textWindow = new FormattedTextCtrl(view->GetSplitter(),
                    BaseProjectView::LONG_SENTENCES_AND_WORDINESS_TEXT_PAGE_ID, wxDefaultPosition,
                    wxDefaultSize, wxTE_READONLY);
                textWindow->Hide();
                textWindow->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_TEXT_MENU"));
                textWindow->SetLabel(_(L"Highlighted Report"));
                textWindow->SetName(_(L"Highlighted Report"));
                view->GetGrammarView().InsertWindow(0, textWindow);
                }
            UpdateTextWindowOptions(textWindow);
            [[maybe_unused]] const size_t textLength = FormatWordCollectionHighlightedGrammarIssues(GetWords(),
                    GetDifficultSentenceLength(),
                    docText.get(), textBufferLength,
                    textHeaderThemed.header, textHeaderThemed.endSection,
                    textLegendsThemed.wordinessWindowLegend,
                    highlighterTagsThemed.HIGHLIGHT_BEGIN, highlighterTagsThemed.HIGHLIGHT_END,
                    highlighterTagsThemed.ERROR_HIGHLIGHT_BEGIN,
                    highlighterTagsThemed.PHRASE_HIGHLIGHT_BEGIN,
                    highlighterTagsThemed.IGNORE_HIGHLIGHT_BEGIN,
                    // if default style is bold, then don't use bold tags internally
                    // because that will mess up the RTF
                    (textViewFont.GetWeight() == wxFONTWEIGHT_BOLD) ?
                        wxString{} : highlighterTagsThemed.BOLD_BEGIN,
                    (textViewFont.GetWeight() == wxFONTWEIGHT_BOLD) ?
                        wxString{} : highlighterTagsThemed.BOLD_END,
                    highlighterTagsThemed.TAB_SYMBOL, highlighterTagsThemed.CRLF,
                    textBeingExcluded,
                    textBeingExcluded,
                #ifdef __WXGTK__
                    false
                #else
                    true
                #endif
                    );
#ifndef __WXGTK__
            textWindow->SetMaxLength(static_cast<unsigned long>(textLength));
#endif
            setFormattedTextAndRestoreInsertionPoint(textWindow, docText.get());

#ifdef DEBUG_EXPERIMENTAL_CODE
            const auto tempFilePath = wxFileName::CreateTempFileName(_(L"Highlighted Report"));
            wxFile textWindowDump(tempFilePath, wxFile::OpenMode::write);
            if (textWindowDump.IsOpened())
                {
                textWindowDump.Write(docText.get());
                wxLogDebug(L"Text view written to: %s", tempFilePath);
                }
#endif

            FormatWordCollectionHighlightedGrammarIssues(GetWords(),
                                        GetDifficultSentenceLength(),
                                        docText.get(), textBufferLength,
                                        textHeaderPaperWhite.header, textHeaderPaperWhite.endSection,
                                        textLegendsPaperWhite.wordinessWindowLegend,
                                        highlighterTagsPaperWhite.HIGHLIGHT_BEGIN,
                                        highlighterTagsPaperWhite.HIGHLIGHT_END,
                                        highlighterTagsPaperWhite.ERROR_HIGHLIGHT_BEGIN,
                                        highlighterTagsPaperWhite.PHRASE_HIGHLIGHT_BEGIN,
                                        highlighterTagsPaperWhite.IGNORE_HIGHLIGHT_BEGIN,
                                        // if default style is bold, then don't use bold tags internally because
                                        // that will mess up the RTF
                                        (textViewFont.GetWeight() == wxFONTWEIGHT_BOLD) ?
                                            wxString{} : highlighterTagsThemed.BOLD_BEGIN,
                                        (textViewFont.GetWeight() == wxFONTWEIGHT_BOLD) ?
                                            wxString{} : highlighterTagsThemed.BOLD_END,
                                        highlighterTagsPaperWhite.TAB_SYMBOL, highlighterTagsPaperWhite.CRLF,
                                        textBeingExcluded,
                                        textBeingExcluded,
                                    #ifdef __WXGTK__
                                        false
                                    #else
                                        true
                                    #endif
                                        );
            textWindow->SetUnthemedFormattedText(docText.get());
            }
        else
            { view->GetGrammarView().RemoveWindowById(BaseProjectView::LONG_SENTENCES_AND_WORDINESS_TEXT_PAGE_ID); }

        // Dolch sight words
        if (IsIncludingDolchSightWords())
            {
            FormattedTextCtrl* textWindow =
                dynamic_cast<FormattedTextCtrl*>(view->GetDolchSightWordsView().FindWindowById(
                    BaseProjectView::DOLCH_WORDS_TEXT_PAGE_ID));
            textWindow = buildTextWindow(textWindow, BaseProjectView::DOLCH_WORDS_TEXT_PAGE_ID,
                _(L"Highlighted Dolch Words"), isDolchWordThemed, textLegendsThemed.dolchWindowLegend);
            view->GetDolchSightWordsView().AddWindow(textWindow);
            }
        else
            { view->GetDolchSightWordsView().RemoveWindowById(BaseProjectView::DOLCH_WORDS_TEXT_PAGE_ID); }

        if (IsIncludingDolchSightWords())
            {
            FormattedTextCtrl* textWindow =
                dynamic_cast<FormattedTextCtrl*>(view->GetDolchSightWordsView().FindWindowById(
                    BaseProjectView::NON_DOLCH_WORDS_TEXT_PAGE_ID));
            textWindow = buildTextWindow(textWindow, BaseProjectView::NON_DOLCH_WORDS_TEXT_PAGE_ID,
                _(L"Highlighted Non-Dolch Words"), isNotDolchWordThemed, textLegendsThemed.nonDolchWordsLegend);
            view->GetDolchSightWordsView().AddWindow(textWindow);
            }
        else
            { view->GetDolchSightWordsView().RemoveWindowById(BaseProjectView::NON_DOLCH_WORDS_TEXT_PAGE_ID); }
        }
    catch (...)
        {
        wxMessageBox(
            _(L"An internal error occurred while formatting the highlighted text. "
               "Please contact the software vendor."), _(L"Error"),
            wxICON_EXCLAMATION|wxOK);
        }
    }

//-------------------------------------------------------
bool ProjectDoc::OnSaveDocument(const wxString& filename)
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
            LogMessage(_(L"Project appears to be open by another application. Cannot save project."),
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
        // If the file is already there and it is in use then fail.
        // otherwise, may be a new project needing to be created. Either way, we need to
        // truncate the file (and maybe create it), so open it for writing.
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

    // Write the meta data information, such as statistics
    //---------------------------------------------------
    wxString metaFileText = wxString{}; wxString sectionText = wxString{};
    metaFileText.append(L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<").
        append(wxGetApp().GetAppOptions().XML_OFFICE_DOCUMENT_META_HEADER).
        append(L">\n\t<").append(wxGetApp().GetAppOptions().XML_OFFICE_META).append(L">\n");
    // statistics section
    metaFileText.append(L"\t\t<").append(wxGetApp().GetAppOptions().XML_STATISTICS);
    // text size
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_TEXT_SIZE, GetTextSize());
    metaFileText += sectionText;
    // unique words
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_UNIQUE_WORDS, GetTotalUniqueWords());
    metaFileText += sectionText;
    // unique monosyllabic words
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_UNIQUE_MONOSYLLABLIC_WORDS, GetTotalUniqueMonoSyllablicWords());
    metaFileText += sectionText;
    // unique 6 Chars+ Words
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_UNIQUE_SIXCHAR_PLUS_WORDS, GetTotalUnique6CharsPlusWords());
    metaFileText += sectionText;
    // unique 3+ Syllable Words
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_UNIQUE_THREE_SYLLABLE_PLUS_WORDS, GetTotalUnique3PlusSyllableWords());
    metaFileText += sectionText;
    // unique DC Hard Words
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_UNIQUE_DC_HARD_WORDS, GetTotalUniqueDCHardWords());
    metaFileText += sectionText;
    // unique Spache Hard Words
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_UNIQUE_SPACHE_HARD_WORDS, GetTotalUniqueHardWordsSpache());
    metaFileText += sectionText;
    // unique Harris-Jacobson Hard Words
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_UNIQUE_HARRIS_JACOBSON_HARD_WORDS, GetTotalUniqueHarrisJacobsonHardWords());
    metaFileText += sectionText;
    // unique Mini Words
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_UNIQUE_MINI_WORDS, GetTotalUniqueMiniWords());
    metaFileText += sectionText;
    // unique Hard Fog Words
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_UNIQUE_FOG_HARD_WORDS, GetTotalUniqueHardFogWords());
    metaFileText += sectionText;
    // unique Hard Words (numerals fully syllabized)
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_UNIQUE_SMOG_WORDS_WORDS, GetUnique3PlusSyllabicWordsNumeralsFullySyllabized());
    metaFileText += sectionText;
    // total Words
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_TOTAL_WORDS, GetTotalWords());
    metaFileText += sectionText;
    // total Sentences
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_TOTAL_SENTENCES, GetTotalSentences());
    metaFileText += sectionText;
    // total Sentence units
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_TOTAL_SENTENCE_UNITS, GetTotalSentenceUnits());
    metaFileText += sectionText;
    // total Paragraphs
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_TOTAL_PARAGRAPH, GetTotalParagraphs());
    metaFileText += sectionText;
    // total Syllables
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_TOTAL_SYLLABLE, GetTotalSyllables());
    metaFileText += sectionText;
    // total Characters
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_TOTAL_CHARACTERS, GetTotalCharacters());
    metaFileText += sectionText;
    // total characters + punctuation
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_TOTAL_CHARACTERS_PLUS_PUNCTUATION, GetTotalCharactersPlusPunctuation());
    metaFileText += sectionText;
    // total Mono Syllabic
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_TOTAL_MONOSYLLABIC_WORDS, GetTotalMonoSyllabicWords());
    metaFileText += sectionText;
    // total 3+ Syllable Words
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_TOTAL_THREE_PLUSSYLLABLE_WORDS, GetTotal3PlusSyllabicWords());
    metaFileText += sectionText;
    // total Hard Words Spache
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_TOTAL_SPACHE_HARD_WORDS, GetTotalHardWordsSpache());
    metaFileText += sectionText;
    // total Hard Words Harris-Jacobson
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_TOTAL_HARRIS_JACOBSON_HARD_WORDS, GetTotalHardWordsHarrisJacobson());
    metaFileText += sectionText;
    // total Hard Words Dale Chall
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_TOTAL_DALECHALL_HARD_WORDS, GetTotalHardWordsDaleChall());
    metaFileText += sectionText;
    // total Hard Words Fog
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_TOTAL_FOG_HARDWORDS, GetTotalHardWordsFog());
    metaFileText += sectionText;
    // total Hard Words (numerals fully syllabized)
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_TOTAL_SMOG_HARD_WORDS, GetTotal3PlusSyllabicWordsNumeralsFullySyllabized());
    metaFileText += sectionText;
    // total Hard Words SOL
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_TOTAL_SOL_HARD_WORDS, GetTotalHardWordsSol());
    metaFileText += sectionText;
    // total Hard Lix/Rix words
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_TOTAL_LIX_RIX_HARD_WORDS, GetTotalHardLixRixWords());
    metaFileText += sectionText;
    // total Syllables Ignoring Numerals
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_TOTAL_SYLLABLES_IGNORING_NUMERALS, m_totalSyllablesIgnoringNumerals);
    metaFileText += sectionText;
    // Number of syllables, numerals just one syllable
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_TOTAL_SYLLABLES_NUMERALS_ONE_SYLLABLE,
        GetTotalSyllablesNumeralsOneSyllable());
    metaFileText += sectionText;
    // Number of syllables, numerals fully syllabized
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_TOTAL_SYLLABLES_NUMERALS_FULLY_SYLLABIZED,
        GetTotalSyllablesNumeralsFullySyllabized());
    metaFileText += sectionText;
    // total Syllables Ignoring Numerals and proper nouns
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_TOTAL_SYLLABLES_IGNORING_NUMERALS_AND_PROPER_NOUNS,
        GetTotalSyllablesIgnoringNumeralsAndProperNouns());
    metaFileText += sectionText;
    // total Long Words
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_TOTAL_LONG_WORDS, GetTotalLongWords());
    metaFileText += sectionText;
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_TOTAL_LONG_WORDS_IGNORING_NUMERALS,
        GetTotalSixPlusCharacterWordsIgnoringNumerals());
    metaFileText += sectionText;
    // total MiniWords
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_TOTAL_MINIWORDS, GetTotalMiniWords());
    metaFileText += sectionText;
    // total Numerals
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_TOTAL_NUMERALS, GetTotalNumerals());
    metaFileText += sectionText;
    // total proper nouns
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_TOTAL_PROPER_NOUNS, GetTotalProperNouns());
    metaFileText += sectionText;
    // total Overly Long Sentences
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_TOTAL_OVERLY_LONG_SENTENCES, GetTotalOverlyLongSentences());
    metaFileText += sectionText;
    // supplementary stats
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_TOTAL_WORDS_FROM_COMPLETE_SENTENCES_AND_HEADERS,
        GetTotalWordsFromCompleteSentencesAndHeaders());
    metaFileText += sectionText;
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_TOTAL_SENTENCES_FROM_COMPLETE_SENTENCES_AND_HEADERS,
        GetTotalSentencesFromCompleteSentencesAndHeaders());
    metaFileText += sectionText;
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_TOTAL_NUMERALS_FROM_COMPLETE_SENTENCES_AND_HEADERS,
        GetTotalNumeralsFromCompleteSentencesAndHeaders());
    metaFileText += sectionText;
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_TOTAL_CHARACTERS_FROM_COMPLETE_SENTENCES_AND_HEADERS,
        GetTotalCharactersFromCompleteSentencesAndHeaders());
    metaFileText += sectionText;
    // special sentences
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_TOTAL_INTERROGATIVE_SENTENCES, GetTotalInterrogativeSentences());
    metaFileText += sectionText;
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_TOTAL_EXCLAMATORY_SENTENCES, GetTotalExclamatorySentences());
    metaFileText += sectionText;
    // Longest Sentence
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_LONGEST_SENTENCES, GetLongestSentence());
    metaFileText += sectionText;
    // longest sentence index
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_LONGEST_SENTENCE_INDEX, GetLongestSentenceIndex());
    metaFileText += sectionText;
    // difficult Sentence Length
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_DIFFICULT_SENTENCE_LENGTH, GetDifficultSentenceLength());
    metaFileText += sectionText;
    // grammar info
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_MISSPELLING_COUNT, GetMisspelledWordCount());
    metaFileText += sectionText;
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_DUPLICATE_WORD_COUNT, GetDuplicateWordCount());
    metaFileText += sectionText;
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_WORDY_PHRASE_COUNT, GetWordyPhraseCount());
    metaFileText += sectionText;
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_REDUNDANT_PHRASE_COUNT, GetRedundantPhraseCount());
    metaFileText += sectionText;
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_WORDING_ERROR_COUNT, GetWordingErrorCount());
    metaFileText += sectionText;
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_CLICHE_COUNT, GetClicheCount());
    metaFileText += sectionText;
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_SENTENCE_CONJUNCTION_START_COUNT, GetSentenceStartingWithConjunctionsCount());
    metaFileText += sectionText;
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_SENTENCE_LOWERCASE_START_COUNT, GetSentenceStartingWithLowercaseCount());
    metaFileText += sectionText;
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_ARTICLE_MISMATCH_COUNT, GetMismatchedArticleCount());
    metaFileText += sectionText;
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_PASSIVE_VOICE_COUNT, GetPassiveVoicesCount());
    metaFileText += sectionText;
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_OVERUSED_WORDS_BY_SENTENCE_COUNT, GetOverusedWordsBySentenceCount());
    metaFileText += sectionText;
    // Dolch stats
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_DOLCH_UNIQUE_CONJUNCTIONS_COUNT, GetDolchConjunctionCounts().first);
    metaFileText += sectionText;
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_DOLCH_TOTAL_CONJUNCTIONS_COUNT, GetDolchConjunctionCounts().second);
    metaFileText += sectionText;
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_DOLCH_UNIQUE_PREPOSITIONS_COUNT, GetDolchPrepositionWordCounts().first);
    metaFileText += sectionText;
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_DOLCH_TOTAL_PREPOSITIONS_COUNT, GetDolchPrepositionWordCounts().second);
    metaFileText += sectionText;
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_DOLCH_UNIQUE_PRONOUNS_COUNT, GetDolchPronounCounts().first);
    metaFileText += sectionText;
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_DOLCH_TOTAL_PRONOUNS_COUNT, GetDolchPronounCounts().second);
    metaFileText += sectionText;
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_DOLCH_UNIQUE_ADVERBS_COUNT, GetDolchAdverbCounts().first);
    metaFileText += sectionText;
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_DOLCH_TOTAL_ADVERBS_COUNT, GetDolchAdverbCounts().second);
    metaFileText += sectionText;
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_DOLCH_UNIQUE_ADJECTIVES_COUNT, GetDolchAdjectiveCounts().first);
    metaFileText += sectionText;
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_DOLCH_TOTAL_ADJECTIVES_COUNT, GetDolchAdjectiveCounts().second);
    metaFileText += sectionText;
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_DOLCH_UNIQUE_NOUNS_COUNT, GetDolchNounCounts().first);
    metaFileText += sectionText;
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_DOLCH_TOTAL_NOUNS_COUNT, GetDolchNounCounts().second);
    metaFileText += sectionText;
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_DOLCH_UNUSED_CONJUNCTIONS_COUNT, GetUnusedDolchConjunctions());
    metaFileText += sectionText;
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_DOLCH_UNUSED_PREPOSITIONS_COUNT, GetUnusedDolchPrepositions());
    metaFileText += sectionText;
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_DOLCH_UNUSED_PRONOUNS_COUNT, GetUnusedDolchPronouns());
    metaFileText += sectionText;
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_DOLCH_UNUSED_ADVERBS_COUNT, GetUnusedDolchAdverbs());
    metaFileText += sectionText;
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_DOLCH_UNUSED_ADJECTIVES_COUNT, GetUnusedDolchAdjectives());
    metaFileText += sectionText;
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_DOLCH_UNUSED_VERBS_COUNT, GetUnusedDolchVerbs());
    metaFileText += sectionText;
    XmlFormat::FormatAttribute(sectionText,
        wxGetApp().GetAppOptions().XML_DOLCH_UNUSED_NOUNS_COUNT, GetUnusedDolchNouns());
    metaFileText += sectionText;

    metaFileText.append(L" />\n").append(L"\t</").append(wxGetApp().GetAppOptions().XML_OFFICE_META).append(L">\n");

    // custom tests settings
    metaFileText.append(L"\t<").append(wxGetApp().GetAppOptions().XML_CUSTOM_TESTS).append(L">\n");
    for (auto pos = GetCustTestsInUse().cbegin();
         pos != GetCustTestsInUse().cend();
         ++pos)
        {
        if (!pos->GetIterator()->is_using_familiar_words())
            { continue; }
        metaFileText.append(L"\t\t<").append(wxGetApp().GetAppOptions().XML_CUSTOM_FAMILIAR_WORD_TEST);
        XmlFormat::FormatAttribute(sectionText,
            wxGetApp().GetAppOptions().XML_TEST_NAME, wxString(pos->GetIterator()->get_name().c_str()));
        metaFileText += sectionText;
        XmlFormat::FormatAttribute(sectionText,
            wxGetApp().GetAppOptions().XML_UNFAMILIAR_WORD_COUNT, pos->GetUnfamiliarWordCount());
        metaFileText += sectionText;
        XmlFormat::FormatAttribute(sectionText,
            wxGetApp().GetAppOptions().XML_UNIQUE_UNFAMILIAR_WORD_COUNT, pos->GetUniqueUnfamiliarWordCount());
        metaFileText += sectionText;
        metaFileText.append(L" />\n");
        }
    metaFileText.append(L"\t</").append(wxGetApp().GetAppOptions().XML_CUSTOM_TESTS).append(L">\n");

    metaFileText.append(L"</").append(wxGetApp().GetAppOptions().XML_OFFICE_DOCUMENT_META).append(L">");

    /* Write the (zip file) project out to a temp file first, then swap it.
       This helps us from corrupting the original file if something goes wrong
       during the write process.*/
    wxTempFileOutputStream out(filename);
    wxZipOutputStream zip(out, 9/*Maximum compression*/);

    // settings.xml
    Wisteria::ZipCatalog::WriteText(zip, ProjectSettingsFileLabel(), FormatProjectSettings());

    // meta.xml
    Wisteria::ZipCatalog::WriteText(zip, ProjectMetaFileLabel(), metaFileText);

    /* If storing indexed text then include it.
       Note that if text was manually entered then it is always embedded;
       otherwise, it would be lost.*/
    if (GetDocumentStorageMethod() == TextStorage::EmbedText)
        {
        Wisteria::ZipCatalog::WriteText(zip, ProjectContentFileLabel(), GetDocumentText());
        if (m_graphBackgroundImage.IsOk())
            {
            const wxFileName fn(GetBackGroundImagePath());
            wxString ext{ fn.GetExt() };
            zip.PutNextEntry(fn.GetFullName());
            m_graphBackgroundImage.GetBitmap(m_graphBackgroundImage.GetDefaultSize()).ConvertToImage().
                SaveFile(zip, Image::GetImageFileTypeFromExtension(ext));
            }
        if (m_graphStippleImage.IsOk())
            {
            const wxFileName fn(GetStippleImagePath());
            wxString ext{ fn.GetExt() };
            zip.PutNextEntry(fn.GetFullName());
            m_graphStippleImage.GetBitmap(m_graphStippleImage.GetDefaultSize()).ConvertToImage().
                SaveFile(zip, Image::GetImageFileTypeFromExtension(ext));
            }
        if (m_waterMarkImage.IsOk())
            {
            const wxFileName fn(GetWatermarkLogoPath());
            wxString ext{ fn.GetExt() };
            zip.PutNextEntry(fn.GetFullName());
            m_waterMarkImage.GetBitmap(m_waterMarkImage.GetDefaultSize()).ConvertToImage().
                SaveFile(zip, Image::GetImageFileTypeFromExtension(ext));
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
void ProjectDoc::DisplayOverlyLongSentences()
    {
    PROFILE();
    ProjectView* view = dynamic_cast<ProjectView*>(GetFirstView());

    if (!GetWords())
        {
        view->GetSentencesBreakdownView().RemoveWindowById(BaseProjectView::LONG_SENTENCES_LIST_PAGE_ID);
        return;
        }

    // list the overly long sentences
    m_overlyLongSentenceData->DeleteAllItems();
    m_overlyLongSentenceData->SetSize(GetWords()->get_sentence_count(), 3);
    size_t longSenteceCount = 0;
    const grammar::sentence_length_greater_than sentenceGreater(GetDifficultSentenceLength());
    const grammar::complete_sentence_length_greater_than completeSentenceGreater(GetDifficultSentenceLength());
    wxString currentSentence;
    // punctuation markers
    std::vector<punctuation::punctuation_mark>::const_iterator punctPos = GetWords()->get_punctuation().begin();
    for (std::vector<grammar::sentence_info>::const_iterator pos = GetWords()->get_sentences().begin();
        pos != GetWords()->get_sentences().end();
        ++pos)
        {
        if ((GetInvalidSentenceMethod() == InvalidSentence::ExcludeFromAnalysis && completeSentenceGreater(*pos)) ||
            (GetInvalidSentenceMethod() == InvalidSentence::IncludeAsFullSentences && sentenceGreater(*pos)) ||
            (GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings && completeSentenceGreater(*pos)))
            {
            currentSentence =
                ProjectReportFormat::FormatSentence(this, *pos, punctPos, GetWords()->get_punctuation().end());

            m_overlyLongSentenceData->SetItemText(longSenteceCount, 0, currentSentence);
            if (GetInvalidSentenceMethod() == InvalidSentence::IncludeAsFullSentences)
                { m_overlyLongSentenceData->SetItemValue(longSenteceCount, 1, pos->get_word_count()); }
            else
                { m_overlyLongSentenceData->SetItemValue(longSenteceCount, 1, pos->get_valid_word_count()); }
            m_overlyLongSentenceData->SetItemValue(longSenteceCount++, 2,
                // add 1 to make it one-indexed
                (pos - GetWords()->get_sentences().begin())+1,
                NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting,0,true));
            }
        }
    m_overlyLongSentenceData->SetSize(longSenteceCount);

    // long sentences
    if (GetSentencesBreakdownInfo().IsLongSentencesEnabled() && m_overlyLongSentenceData->GetItemCount())
        {
        ListCtrlEx* listView =
            dynamic_cast<ListCtrlEx*>(view->GetSentencesBreakdownView().FindWindowById(
                BaseProjectView::LONG_SENTENCES_LIST_PAGE_ID));
        if (listView)
            {
            listView->SetVirtualDataProvider(m_overlyLongSentenceData);
            listView->SetVirtualDataSize(m_overlyLongSentenceData->GetItemCount());
            listView->DistributeColumns();
            listView->Resort();
            }
        else
            {
            listView = new ListCtrlEx(view->GetSplitter(), BaseProjectView::LONG_SENTENCES_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetLongSentencesLabel());
            listView->SetName(BaseProjectView::GetLongSentencesLabel());
            listView->EnableGridLines();
            listView->InsertColumn(0, _(L"Sentence"));
            listView->InsertColumn(1, _(L"Word Count"));
            listView->InsertColumn(2, _(L"Sentence #"));
            listView->SetVirtualDataProvider(m_overlyLongSentenceData);
            listView->SetVirtualDataSize(m_overlyLongSentenceData->GetItemCount());
            listView->DistributeColumns();
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            UpdateListOptions(listView);
            // sort by length (high to low), then by index (low to high, order of appearance in the document)
            std::vector<std::pair<size_t,Wisteria::SortDirection>> columnsToSort;
            columnsToSort.push_back(
                std::pair<size_t,Wisteria::SortDirection>(1, Wisteria::SortDirection::SortDescending));
            columnsToSort.push_back(
                std::pair<size_t,Wisteria::SortDirection>(2, Wisteria::SortDirection::SortAscending));
            listView->SortColumns(columnsToSort);
            view->GetSentencesBreakdownView().AddWindow(listView);
            }
        }
    else
        {
        // we are getting rid of this window (if it was included before)
        view->GetSentencesBreakdownView().RemoveWindowById(BaseProjectView::LONG_SENTENCES_LIST_PAGE_ID);
        }
    }

//-------------------------------------------------------
void ProjectDoc::DisplayGrammar()
    {
    PROFILE();
    ProjectView* view = dynamic_cast<ProjectView*>(GetFirstView());

    // misspelled words
    frequency_set<traits::case_insensitive_wstring_ex> misspelledWords;
    const auto& misspelledWordIndices = GetWords()->get_misspelled_words();
    for (size_t i = 0; i < misspelledWordIndices.size(); ++i)
        { misspelledWords.insert(GetWords()->get_word(misspelledWordIndices[i]).c_str()); }
    m_misspelledWordData->DeleteAllItems();
    m_misspelledWordData->SetSize(misspelledWords.get_data().size(), 2);
    size_t uniqueMisspellingCount = 0;
    for (auto mIter = misspelledWords.get_data().cbegin();
        mIter != misspelledWords.get_data().cend();
        ++mIter)
        {
        m_misspelledWordData->SetItemText(uniqueMisspellingCount, 0, mIter->first.c_str());
        m_misspelledWordData->SetItemValue(uniqueMisspellingCount++, 1, mIter->second);
        }
    // totally redundant, but fixes mysterious painting issue when project is refreshed
    m_misspelledWordData->SetSize(misspelledWords.get_data().size());
    ListCtrlEx* listView = dynamic_cast<ListCtrlEx*>(view->GetGrammarView().FindWindowById(
        BaseProjectView::MISSPELLED_WORD_LIST_PAGE_ID));
    if (GetGrammarInfo().IsMisspellingsEnabled() && m_misspelledWordData->GetItemCount())
        {
        if (listView)
            {
            listView->SetVirtualDataProvider(m_misspelledWordData);
            listView->SetVirtualDataSize(m_misspelledWordData->GetItemCount());
            listView->DistributeColumns();
            listView->Resort();
            }
        else
            {
            listView = new ListCtrlEx(view->GetSplitter(), BaseProjectView::MISSPELLED_WORD_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetMisspellingsLabel());
            listView->SetName(BaseProjectView::GetMisspellingsLabel());
            listView->EnableGridLines();
            listView->InsertColumn(0, _(L"Misspelling"));
            listView->InsertColumn(1, _(L"Frequency"));
            listView->SetVirtualDataProvider(m_misspelledWordData);
            listView->SetVirtualDataSize(m_misspelledWordData->GetItemCount());
            listView->DistributeColumns();
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            UpdateListOptions(listView);
            listView->SortColumn(0, Wisteria::SortDirection::SortAscending);
            view->GetGrammarView().AddWindow(listView);
            }
        }
    else
        {
        // we are getting rid of this window (if it was included before)
        view->GetGrammarView().RemoveWindowById(BaseProjectView::MISSPELLED_WORD_LIST_PAGE_ID);
        }

    // repeated words
    const auto& dupWordIndices = GetWords()->get_duplicate_word_indices();
    m_dupWordData->DeleteAllItems();
    m_dupWordData->SetSize(dupWordIndices.size(), 2);
    for (size_t i = 0; i < dupWordIndices.size(); ++i)
        {
        const word_case_insensitive_no_stem& dupWord = GetWords()->get_word(dupWordIndices[i]);
        m_dupWordData->SetItemText(i, 0, wxString::Format(L"%s %s", dupWord.c_str(), dupWord.c_str()));
        m_dupWordData->SetItemValue(i, 1, dupWord.get_sentence_index() + 1,
            // make it one-indexed
            NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting,0,true));
        }
    listView = dynamic_cast<ListCtrlEx*>(view->GetGrammarView().FindWindowById(
        BaseProjectView::DUPLICATES_LIST_PAGE_ID));
    if (GetGrammarInfo().IsRepeatedWordsEnabled() && m_dupWordData->GetItemCount())
        {
        if (listView)
            {
            listView->SetVirtualDataProvider(m_dupWordData);
            listView->SetVirtualDataSize(m_dupWordData->GetItemCount());
            listView->DistributeColumns();
            listView->Resort();
            }
        else
            {
            listView = new ListCtrlEx(view->GetSplitter(), BaseProjectView::DUPLICATES_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetRepeatedWordsLabel());
            listView->SetName(BaseProjectView::GetRepeatedWordsLabel());
            listView->EnableGridLines();
            listView->InsertColumn(0, _(L"Words that Appear Twice Adjacently"));
            listView->InsertColumn(1, _(L"Sentence #"));
            listView->SetVirtualDataProvider(m_dupWordData);
            listView->SetVirtualDataSize(m_dupWordData->GetItemCount());
            listView->DistributeColumns();
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            UpdateListOptions(listView);
            view->GetGrammarView().AddWindow(listView);
            }
        }
    else
        {
        // we are getting rid of this window (if it was included before)
        view->GetGrammarView().RemoveWindowById(BaseProjectView::DUPLICATES_LIST_PAGE_ID);
        }

    // Mismatched articles
    frequency_set<traits::case_insensitive_wstring_ex> articleMismatchesWords;
    const auto& incorectArticleIndices = GetWords()->get_incorrect_article_indices();
    for (size_t i = 0; i < incorectArticleIndices.size(); ++i)
        { articleMismatchesWords.insert(GetWords()->get_word(incorectArticleIndices[i]).c_str() +
            traits::case_insensitive_wstring_ex(L" ") + GetWords()->get_word(incorectArticleIndices[i]+1).c_str()); }
    m_incorrectArticleData->DeleteAllItems();
    m_incorrectArticleData->SetSize(articleMismatchesWords.get_data().size(), 2);
    size_t uniqueIncorrectArticleCount = 0;
    for (auto mIter = articleMismatchesWords.get_data().cbegin();
        mIter != articleMismatchesWords.get_data().cend();
        ++mIter)
        {
        m_incorrectArticleData->SetItemText(uniqueIncorrectArticleCount, 0, mIter->first.c_str());
        m_incorrectArticleData->SetItemValue(uniqueIncorrectArticleCount++, 1, mIter->second);
        }
    listView = dynamic_cast<ListCtrlEx*>(view->GetGrammarView().FindWindowById(
        BaseProjectView::INCORRECT_ARTICLE_PAGE_ID));
    if (GetGrammarInfo().IsArticleMismatchesEnabled() && m_incorrectArticleData->GetItemCount())
        {
        if (listView)
            {
            listView->SetVirtualDataProvider(m_incorrectArticleData);
            listView->SetVirtualDataSize(m_incorrectArticleData->GetItemCount());
            listView->DistributeColumns();
            listView->Resort();
            }
        else
            {
            listView = new ListCtrlEx(view->GetSplitter(), BaseProjectView::INCORRECT_ARTICLE_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetArticleMismatchesLabel());
            listView->SetName(BaseProjectView::GetArticleMismatchesLabel());
            listView->EnableGridLines();
            listView->InsertColumn(0, _(L"Article Mismatch"));
            listView->InsertColumn(1, _(L"Frequency"));
            listView->SetVirtualDataProvider(m_incorrectArticleData);
            listView->SetVirtualDataSize(m_incorrectArticleData->GetItemCount());
            listView->DistributeColumns();
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            UpdateListOptions(listView);
            listView->SortColumn(0, Wisteria::SortDirection::SortAscending);
            view->GetGrammarView().AddWindow(listView);
            }
        }
    else
        {
        // we are getting rid of this window (if it was included before)
        view->GetGrammarView().RemoveWindowById(BaseProjectView::INCORRECT_ARTICLE_PAGE_ID);
        }

    // wordy items and cliches
    const auto& wordyIndices = GetWords()->get_known_phrase_indices();
    const auto& wordyPhrases =
        GetWords()->get_known_phrases().get_phrases();
    m_wordyPhraseData->DeleteAllItems();
    m_wordyPhraseData->SetSize(GetWords()->get_known_phrase_indices().size(), 3);
    m_redundantPhraseData->DeleteAllItems();
    m_redundantPhraseData->SetSize(GetWords()->get_known_phrase_indices().size(), 3);
    m_wordingErrorData->DeleteAllItems();
    m_wordingErrorData->SetSize(GetWords()->get_known_phrase_indices().size(), 3);
    m_clichePhraseData->DeleteAllItems();
    m_clichePhraseData->SetSize(GetWords()->get_known_phrase_indices().size(), 3);
    size_t wordyPhraseCount(0), redundantPhraseCount(0), wordingErrorCount(0), clicheCount(0);
    for (size_t i = 0; i < wordyIndices.size(); ++i)
        {
        if (wordyPhrases[wordyIndices[i].second].first.get_type() == grammar::phrase_type::phrase_cliche)
            {
            m_clichePhraseData->SetItemText(
                clicheCount, 0, wordyPhrases[wordyIndices[i].second].first.to_string().c_str());
            m_clichePhraseData->SetItemText(
                clicheCount, 1, wordyPhrases[wordyIndices[i].second].second.c_str());
            m_clichePhraseData->SetItemValue(clicheCount++, 2,
                // make 1-based index
                GetWords()->get_words()[wordyIndices[i].first].get_sentence_index()+1,
                NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting,0,true));
            }
        else if (wordyPhrases[wordyIndices[i].second].first.get_type() == grammar::phrase_type::phrase_redundant)
            {
            m_redundantPhraseData->SetItemText(
                redundantPhraseCount, 0, wordyPhrases[wordyIndices[i].second].first.to_string().c_str());
            m_redundantPhraseData->SetItemText(
                redundantPhraseCount, 1, wordyPhrases[wordyIndices[i].second].second.c_str());
            m_redundantPhraseData->SetItemValue(redundantPhraseCount++, 2,
                // make 1-based index
                GetWords()->get_words()[wordyIndices[i].first].get_sentence_index()+1,
                NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting,0,true));
            }
        else if (wordyPhrases[wordyIndices[i].second].first.get_type() == grammar::phrase_type::phrase_error)
            {
            m_wordingErrorData->SetItemText(
                wordingErrorCount, 0, wordyPhrases[wordyIndices[i].second].first.to_string().c_str());
            m_wordingErrorData->SetItemText(
                wordingErrorCount, 1, wordyPhrases[wordyIndices[i].second].second.c_str());
            m_wordingErrorData->SetItemValue(wordingErrorCount++, 2,
                // make 1-based index
                GetWords()->get_words()[wordyIndices[i].first].get_sentence_index()+1,
                NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting,0,true));
            }
        else
            {
            m_wordyPhraseData->SetItemText(
                wordyPhraseCount, 0, wordyPhrases[wordyIndices[i].second].first.to_string().c_str());
            m_wordyPhraseData->SetItemText(
                wordyPhraseCount, 1, wordyPhrases[wordyIndices[i].second].second.c_str());
            m_wordyPhraseData->SetItemValue(wordyPhraseCount++, 2,
                // make 1-based index
                GetWords()->get_words()[wordyIndices[i].first].get_sentence_index()+1,
                NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting,0,true));
            }
        }
    m_wordyPhraseData->SetSize(wordyPhraseCount);
    m_redundantPhraseData->SetSize(redundantPhraseCount);
    m_wordingErrorData->SetSize(wordingErrorCount);
    m_clichePhraseData->SetSize(clicheCount);

    // Wording errors
    listView = dynamic_cast<ListCtrlEx*>(view->GetGrammarView().FindWindowById(
        BaseProjectView::WORDING_ERRORS_LIST_PAGE_ID));
    if (GetGrammarInfo().IsWordingErrorsEnabled() && m_wordingErrorData->GetItemCount())
        {
        if (listView)
            {
            listView->SetVirtualDataProvider(m_wordingErrorData);
            listView->SetVirtualDataSize(m_wordingErrorData->GetItemCount());
            listView->DistributeColumns();
            listView->Resort();
            }
        else
            {
            listView = new ListCtrlEx(view->GetSplitter(), BaseProjectView::WORDING_ERRORS_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetPhrasingErrorsTabLabel());
            listView->SetName(BaseProjectView::GetPhrasingErrorsTabLabel());
            listView->EnableGridLines();
            listView->InsertColumn(0, BaseProjectView::GetPhrasingErrorsTabLabel());
            listView->InsertColumn(1, _(L"Suggestion"));
            listView->InsertColumn(2, _(L"Sentence #"));
            listView->SetVirtualDataProvider(m_wordingErrorData);
            listView->SetVirtualDataSize(m_wordingErrorData->GetItemCount());
            listView->DistributeColumns();
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            UpdateListOptions(listView);
            view->GetGrammarView().AddWindow(listView);
            }
        }
    else
        {
        // we are getting rid of this window (if it was included before)
        view->GetGrammarView().RemoveWindowById(BaseProjectView::WORDING_ERRORS_LIST_PAGE_ID);
        }

    // redundant phrases
    listView = dynamic_cast<ListCtrlEx*>(view->GetGrammarView().FindWindowById(
        BaseProjectView::REDUNDANT_PHRASE_LIST_PAGE_ID));
    if (GetGrammarInfo().IsRedundantPhrasesEnabled() && m_redundantPhraseData->GetItemCount())
        {
        if (listView)
            {
            listView->SetVirtualDataProvider(m_redundantPhraseData);
            listView->SetVirtualDataSize(m_redundantPhraseData->GetItemCount());
            listView->DistributeColumns();
            listView->Resort();
            }
        else
            {
            listView = new ListCtrlEx(view->GetSplitter(), BaseProjectView::REDUNDANT_PHRASE_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetRedundantPhrasesTabLabel());
            listView->SetName(BaseProjectView::GetRedundantPhrasesTabLabel());
            listView->EnableGridLines();
            listView->InsertColumn(0, BaseProjectView::GetRedundantPhrasesTabLabel());
            listView->InsertColumn(1, _(L"Suggestion"));
            listView->InsertColumn(2, _(L"Sentence #"));
            listView->SetVirtualDataProvider(m_redundantPhraseData);
            listView->SetVirtualDataSize(m_redundantPhraseData->GetItemCount());
            listView->DistributeColumns();
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            UpdateListOptions(listView);
            view->GetGrammarView().AddWindow(listView);
            }
        }
    else
        {
        // we are getting rid of this window (if it was included before)
        view->GetGrammarView().RemoveWindowById(BaseProjectView::REDUNDANT_PHRASE_LIST_PAGE_ID);
        }

    // overused words (by sentence)
    GetOverusedWordsBySentenceData()->SetSize(GetWords()->get_overused_words_by_sentence().size(), 4);
    // reset punctuation marker
    auto punctPos = GetWords()->get_punctuation().begin();
    size_t previousSentencePos = 0;
    auto previousPunctPos = GetWords()->get_punctuation().cbegin();
    for (auto overUsedWordsListsIter = GetWords()->get_overused_words_by_sentence().begin();
        overUsedWordsListsIter != GetWords()->get_overused_words_by_sentence().end();
        ++overUsedWordsListsIter)
        {
        // Format the full sentence (to show context)
        // Note that some sentences may have multiple issues and are listed more than once,
        // so we need to reset the punctuation make to the start of the sentence if it's the same sentence again.
        if (previousSentencePos == overUsedWordsListsIter->first)
            { punctPos = previousPunctPos; }
        previousPunctPos = punctPos;
        previousSentencePos = overUsedWordsListsIter->first;
        wxString currentSentence = ProjectReportFormat::FormatSentence(this,
            GetWords()->get_sentences()[overUsedWordsListsIter->first],
            punctPos, GetWords()->get_punctuation().end());
        // format the list of like words
        wxString theWords;
        for (std::set<size_t>::const_iterator overusedWordsIter = overUsedWordsListsIter->second.begin();
            overusedWordsIter != overUsedWordsListsIter->second.end();
            ++overusedWordsIter)
            { theWords.append(GetWords()->get_word((*overusedWordsIter)).c_str()).append(L"; "); }
        theWords.Trim();
        theWords.RemoveLast();
        GetOverusedWordsBySentenceData()->SetItemText(
            overUsedWordsListsIter-GetWords()->get_overused_words_by_sentence().begin(), 0,
            theWords);
        GetOverusedWordsBySentenceData()->SetItemText(
            overUsedWordsListsIter-GetWords()->get_overused_words_by_sentence().begin(), 1,
            currentSentence);
        GetOverusedWordsBySentenceData()->SetItemValue(
            overUsedWordsListsIter-GetWords()->get_overused_words_by_sentence().begin(), 2,
            // Use word count (not valid word count) since this is for grammar analysis.
            // This relates more to what a reader is seeing, not a readability analysis
            GetWords()->get_sentences()[overUsedWordsListsIter->first].get_word_count());
        GetOverusedWordsBySentenceData()->SetItemValue(
            overUsedWordsListsIter-GetWords()->get_overused_words_by_sentence().begin(), 3,
            (overUsedWordsListsIter->first) + 1,
            NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting,0,true));
        }
    listView = dynamic_cast<ListCtrlEx*>(view->GetGrammarView().FindWindowById(
        BaseProjectView::OVERUSED_WORDS_BY_SENTENCE_LIST_PAGE_ID));
    if (GetGrammarInfo().IsOverUsedWordsBySentenceEnabled() &&
        GetOverusedWordsBySentenceData() && GetOverusedWordsBySentenceData()->GetItemCount())
        {
        if (listView)
            {
            listView->SetVirtualDataSize(GetOverusedWordsBySentenceData()->GetItemCount());
            listView->Resort();
            listView->DistributeColumns();
            }
        else
            {
            listView = new ListCtrlEx(view->GetSplitter(), BaseProjectView::OVERUSED_WORDS_BY_SENTENCE_LIST_PAGE_ID,
                                      wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetOverusedWordsBySentenceLabel());
            listView->SetName(BaseProjectView::GetOverusedWordsBySentenceLabel());
            listView->EnableGridLines();
            listView->InsertColumn(0, _(L"Repeated Word"));
            listView->InsertColumn(1, _(L"Sentence"));
            listView->InsertColumn(2, _(L"Sentence Length"));
            listView->InsertColumn(3, _(L"Sentence #"));
            listView->SetVirtualDataProvider(GetOverusedWordsBySentenceData());
            listView->SetVirtualDataSize(GetOverusedWordsBySentenceData()->GetItemCount());
            listView->DistributeColumns();
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            UpdateListOptions(listView);
            // sort by sentence length, then words (lowest to highest)
            std::vector<std::pair<size_t,Wisteria::SortDirection>> columnsToSort;
            columnsToSort.push_back(std::pair<size_t, Wisteria::SortDirection>(2,
                Wisteria::SortDirection::SortAscending));
            columnsToSort.push_back(std::pair<size_t, Wisteria::SortDirection>(0,
                Wisteria::SortDirection::SortAscending));
            listView->SortColumns(columnsToSort);

            view->GetGrammarView().AddWindow(listView);
            }
        }
    else
        {
        // we are getting rid of this window (if nothing in it)
        view->GetGrammarView().RemoveWindowById(BaseProjectView::OVERUSED_WORDS_BY_SENTENCE_LIST_PAGE_ID);
        }

    // Wordy items
    listView = dynamic_cast<ListCtrlEx*>(view->GetGrammarView().FindWindowById(
        BaseProjectView::WORDY_PHRASES_LIST_PAGE_ID));
    if (GetGrammarInfo().IsWordyPhrasesEnabled() && m_wordyPhraseData->GetItemCount())
        {
        if (listView)
            {
            listView->SetVirtualDataProvider(m_wordyPhraseData);
            listView->SetVirtualDataSize(m_wordyPhraseData->GetItemCount());
            listView->DistributeColumns();
            listView->Resort();
            }
        else
            {
            listView = new ListCtrlEx(view->GetSplitter(), BaseProjectView::WORDY_PHRASES_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetWordyPhrasesTabLabel());
            listView->SetName(BaseProjectView::GetWordyPhrasesTabLabel());
            listView->EnableGridLines();
            listView->InsertColumn(0, BaseProjectView::GetWordyPhrasesTabLabel());
            listView->InsertColumn(1, _(L"Suggestion"));
            listView->InsertColumn(2, _(L"Sentence #"));
            listView->SetVirtualDataProvider(m_wordyPhraseData);
            listView->SetVirtualDataSize(m_wordyPhraseData->GetItemCount());
            listView->DistributeColumns();
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            UpdateListOptions(listView);
            view->GetGrammarView().AddWindow(listView);
            }
        }
    else
        {
        // we are getting rid of this window (if it was included before)
        view->GetGrammarView().RemoveWindowById(BaseProjectView::WORDY_PHRASES_LIST_PAGE_ID);
        }

    // Cliches
    listView = dynamic_cast<ListCtrlEx*>(view->GetGrammarView().FindWindowById(
        BaseProjectView::CLICHES_LIST_PAGE_ID));
    if (GetGrammarInfo().IsClichesEnabled() && m_clichePhraseData->GetItemCount())
        {
        if (listView)
            {
            listView->SetVirtualDataProvider(m_clichePhraseData);
            listView->SetVirtualDataSize(m_clichePhraseData->GetItemCount());
            listView->DistributeColumns();
            listView->Resort();
            }
        else
            {
            listView = new ListCtrlEx(view->GetSplitter(), BaseProjectView::CLICHES_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetClichesTabLabel());
            listView->SetName(BaseProjectView::GetClichesTabLabel());
            listView->EnableGridLines();
            listView->InsertColumn(0, BaseProjectView::GetClichesTabLabel());
            listView->InsertColumn(1, _(L"Explanation/Suggestion"));
            listView->InsertColumn(2, _(L"Sentence #"));
            listView->SetVirtualDataProvider(m_clichePhraseData);
            listView->SetVirtualDataSize(m_clichePhraseData->GetItemCount());
            listView->DistributeColumns();
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            UpdateListOptions(listView);
            view->GetGrammarView().AddWindow(listView);
            }
        }
    else
        {
        // we are getting rid of this window (if it was included before)
        view->GetGrammarView().RemoveWindowById(BaseProjectView::CLICHES_LIST_PAGE_ID);
        }

    // Passive voice
    frequency_set<traits::case_insensitive_wstring_ex> passiveVoicePhrases;
    const auto& passiveVoiceIndices = GetWords()->get_passive_voice_indices();
    for (size_t i = 0; i < passiveVoiceIndices.size(); ++i)
        {
        traits::case_insensitive_wstring_ex currentPassivePhrase;
        for (size_t wordCounter = 0; wordCounter < passiveVoiceIndices[i].second; ++wordCounter)
            {
            currentPassivePhrase += (wordCounter == passiveVoiceIndices[i].second-1) ?
                traits::case_insensitive_wstring_ex(
                    GetWords()->get_word(passiveVoiceIndices[i].first + wordCounter)) :
                traits::case_insensitive_wstring_ex(
                    GetWords()->get_word(passiveVoiceIndices[i].first + wordCounter) + L' ');
            }
        passiveVoicePhrases.insert(currentPassivePhrase);
        }
    m_passiveVoiceData->DeleteAllItems();
    m_passiveVoiceData->SetSize(passiveVoicePhrases.get_data().size(), 2);
    size_t uniquePassiveVoiceCount = 0;
    for (auto mIter = passiveVoicePhrases.get_data().cbegin();
        mIter != passiveVoicePhrases.get_data().cend();
        ++mIter)
        {
        m_passiveVoiceData->SetItemText(uniquePassiveVoiceCount, 0, mIter->first.c_str());
        m_passiveVoiceData->SetItemValue(uniquePassiveVoiceCount++, 1, mIter->second);
        }
    listView = dynamic_cast<ListCtrlEx*>(view->GetGrammarView().FindWindowById(
        BaseProjectView::PASSIVE_VOICE_PAGE_ID));
    if (GetGrammarInfo().IsPassiveVoiceEnabled() && m_passiveVoiceData->GetItemCount())
        {
        if (listView)
            {
            listView->SetVirtualDataProvider(m_passiveVoiceData);
            listView->SetVirtualDataSize(m_passiveVoiceData->GetItemCount());
            listView->DistributeColumns();
            listView->Resort();
            }
        else
            {
            listView = new ListCtrlEx(view->GetSplitter(), BaseProjectView::PASSIVE_VOICE_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetPassiveLabel());
            listView->SetName(BaseProjectView::GetPassiveLabel());
            listView->EnableGridLines();
            listView->InsertColumn(0, BaseProjectView::GetPassiveLabel());
            listView->InsertColumn(1, _(L"Frequency"));
            listView->SetVirtualDataProvider(m_passiveVoiceData);
            listView->SetVirtualDataSize(m_passiveVoiceData->GetItemCount());
            listView->DistributeColumns();
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            UpdateListOptions(listView);
            listView->SortColumn(0, Wisteria::SortDirection::SortAscending);
            view->GetGrammarView().AddWindow(listView);
            }
        }
    else
        {
        // we are getting rid of this window (if it was included before)
        view->GetGrammarView().RemoveWindowById(BaseProjectView::PASSIVE_VOICE_PAGE_ID);
        }

    // sentences that begin with conjunctions
    m_sentenceStartingWithConjunctionsData->DeleteAllItems();
    m_sentenceStartingWithConjunctionsData->SetSize(GetWords()->get_sentence_count(), 2);
    size_t sentenceStartingWithConjunctionsCount = 0;
    // reset punctuation marker
    punctPos = GetWords()->get_punctuation().begin();
    wxString currentSentence;
    for (std::vector<size_t>::const_iterator pos = GetWords()->get_conjunction_beginning_sentences().begin();
        pos != GetWords()->get_conjunction_beginning_sentences().end();
        ++pos)
        {
        currentSentence = ProjectReportFormat::FormatSentence(this, GetWords()->get_sentences()[*pos],
            punctPos, GetWords()->get_punctuation().end());

        m_sentenceStartingWithConjunctionsData->SetItemText(sentenceStartingWithConjunctionsCount, 0, currentSentence);
        m_sentenceStartingWithConjunctionsData->SetItemValue(sentenceStartingWithConjunctionsCount++, 1,
            // add 1 to make it one-indexed
            (*pos)+1, NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting,0,true));
        }
    m_sentenceStartingWithConjunctionsData->SetSize(sentenceStartingWithConjunctionsCount);
    // display it
    listView = dynamic_cast<ListCtrlEx*>(view->GetGrammarView().FindWindowById(
        BaseProjectView::SENTENCES_CONJUNCTION_START_LIST_PAGE_ID));
    if (GetGrammarInfo().IsConjunctionStartingSentencesEnabled() &&
        m_sentenceStartingWithConjunctionsData->GetItemCount())
        {
        if (listView)
            {
            listView->SetVirtualDataProvider(m_sentenceStartingWithConjunctionsData);
            listView->SetVirtualDataSize(m_sentenceStartingWithConjunctionsData->GetItemCount());
            listView->DistributeColumns();
            listView->Resort();
            }
        else
            {
            listView = new ListCtrlEx(view->GetSplitter(), BaseProjectView::SENTENCES_CONJUNCTION_START_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetSentenceStartingWithConjunctionsTabLabel());
            listView->SetName(BaseProjectView::GetSentenceStartingWithConjunctionsTabLabel());
            listView->EnableGridLines();
            listView->InsertColumn(0, _(L"Sentence"));
            listView->InsertColumn(1, _(L"Sentence #"));
            listView->SetVirtualDataProvider(m_sentenceStartingWithConjunctionsData);
            listView->SetVirtualDataSize(m_sentenceStartingWithConjunctionsData->GetItemCount());
            listView->DistributeColumns();
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            UpdateListOptions(listView);
            view->GetGrammarView().AddWindow(listView);
            }
        }
    else
        {
        // we are getting rid of this window (if it was included before)
        view->GetGrammarView().RemoveWindowById(BaseProjectView::SENTENCES_CONJUNCTION_START_LIST_PAGE_ID);
        }

    // sentences that begin with lowercased words
    m_sentenceStartingWithLowercaseData->DeleteAllItems();
    m_sentenceStartingWithLowercaseData->SetSize(GetWords()->get_sentence_count(), 2);
    size_t sentenceStartingWithLowercaseCount = 0;
    // reset punctuation marker
    punctPos = GetWords()->get_punctuation().begin();
    for (std::vector<size_t>::const_iterator pos = GetWords()->get_lowercase_beginning_sentences().begin();
        pos != GetWords()->get_lowercase_beginning_sentences().end();
        ++pos)
        {
        currentSentence = ProjectReportFormat::FormatSentence(this, GetWords()->get_sentences()[*pos],
            punctPos, GetWords()->get_punctuation().end());

        m_sentenceStartingWithLowercaseData->SetItemText(sentenceStartingWithLowercaseCount, 0, currentSentence);
        m_sentenceStartingWithLowercaseData->SetItemValue(sentenceStartingWithLowercaseCount++, 1,
           // add 1 to make it one-indexed
           (*pos)+1, NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 0, true));
        }
    m_sentenceStartingWithLowercaseData->SetSize(sentenceStartingWithLowercaseCount);
    // display it
    listView = dynamic_cast<ListCtrlEx*>(view->GetGrammarView().FindWindowById(
        BaseProjectView::SENTENCES_LOWERCASE_START_LIST_PAGE_ID));
    if (GetGrammarInfo().IsLowercaseSentencesEnabled() && m_sentenceStartingWithLowercaseData->GetItemCount())
        {
        if (listView)
            {
            listView->SetVirtualDataProvider(m_sentenceStartingWithLowercaseData);
            listView->SetVirtualDataSize(m_sentenceStartingWithLowercaseData->GetItemCount());
            listView->DistributeColumns();
            listView->Resort();
            }
        else
            {
            listView = new ListCtrlEx(view->GetSplitter(), BaseProjectView::SENTENCES_LOWERCASE_START_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetSentenceStartingWithLowercaseTabLabel());
            listView->SetName(BaseProjectView::GetSentenceStartingWithLowercaseTabLabel());
            listView->EnableGridLines();
            listView->InsertColumn(0, _(L"Sentence"));
            listView->InsertColumn(1, _(L"Sentence #"));
            listView->SetVirtualDataProvider(m_sentenceStartingWithLowercaseData);
            listView->SetVirtualDataSize(m_sentenceStartingWithLowercaseData->GetItemCount());
            listView->DistributeColumns();
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            UpdateListOptions(listView);
            view->GetGrammarView().AddWindow(listView);
            }
        }
    else
        {
        // we are getting rid of this window (if it was included before)
        view->GetGrammarView().RemoveWindowById(BaseProjectView::SENTENCES_LOWERCASE_START_LIST_PAGE_ID);
        }
    }

//-------------------------------------------------------
void ProjectDoc::DisplaySightWords()
    {
    PROFILE();
    ProjectView* view = dynamic_cast<ProjectView*>(GetFirstView());
    assert(view && "Error getting view when displaying sight words!");
    if (!view)
        { return; }

    // Dolch words
    ListCtrlEx* listView = dynamic_cast<ListCtrlEx*>(view->GetDolchSightWordsView().FindWindowById(
        BaseProjectView::DOLCH_WORDS_LIST_PAGE_ID));
    if (IsIncludingDolchSightWords() && GetDolchWordData()->GetItemCount())
        {
        if (listView)
            {
            listView->SetVirtualDataSize(GetDolchWordData()->GetItemCount());
            listView->DistributeColumns();
            listView->Resort();
            }
        else
            {
            listView = new ListCtrlEx(view->GetSplitter(), BaseProjectView::DOLCH_WORDS_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetDolchWordTabLabel());
            listView->SetName(BaseProjectView::GetDolchWordTabLabel());
            listView->EnableGridLines();
            listView->InsertColumn(0, BaseProjectView::GetDolchWordTabLabel());
            listView->InsertColumn(1, _(L"Frequency"));
            listView->InsertColumn(2, _(L"Category"));
            listView->SetVirtualDataProvider(GetDolchWordData());
            listView->SetVirtualDataSize(GetDolchWordData()->GetItemCount());
            listView->DistributeColumns();
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            UpdateListOptions(listView);
            view->GetDolchSightWordsView().AddWindow(listView);
            }
        }
    else
        { view->GetDolchSightWordsView().RemoveWindowById(BaseProjectView::DOLCH_WORDS_LIST_PAGE_ID); }

    // non-Dolch words
    listView = dynamic_cast<ListCtrlEx*>(view->GetDolchSightWordsView().FindWindowById(
        BaseProjectView::NON_DOLCH_WORDS_LIST_PAGE_ID));
    if (IsIncludingDolchSightWords() && GetNonDolchWordData()->GetItemCount())
        {
        if (listView)
            {
            listView->SetVirtualDataSize(GetNonDolchWordData()->GetItemCount());
            listView->DistributeColumns();
            listView->Resort();
            }
        else
            {
            listView = new ListCtrlEx(view->GetSplitter(), BaseProjectView::NON_DOLCH_WORDS_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetNonDolchWordTabLabel());
            listView->SetName(BaseProjectView::GetNonDolchWordTabLabel());
            listView->EnableGridLines();
            listView->InsertColumn(0, BaseProjectView::GetNonDolchWordTabLabel());
            listView->InsertColumn(1, _(L"Frequency"));
            listView->SetVirtualDataProvider(GetNonDolchWordData());
            listView->SetVirtualDataSize(GetNonDolchWordData()->GetItemCount());
            listView->DistributeColumns();
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            UpdateListOptions(listView);
            view->GetDolchSightWordsView().AddWindow(listView);
            }
        }
    else
        { view->GetDolchSightWordsView().RemoveWindowById(BaseProjectView::NON_DOLCH_WORDS_LIST_PAGE_ID); }

    // unused Dolch words
    listView = dynamic_cast<ListCtrlEx*>(view->GetDolchSightWordsView().FindWindowById(
        BaseProjectView::UNUSED_DOLCH_WORDS_LIST_PAGE_ID));
    if (IsIncludingDolchSightWords() && GetUnusedDolchWordData()->GetItemCount())
        {
        if (listView)
            {
            listView->SetVirtualDataSize(GetUnusedDolchWordData()->GetItemCount());
            listView->DistributeColumns();
            listView->Resort();
            }
        else
            {
            listView = new ListCtrlEx(view->GetSplitter(), BaseProjectView::UNUSED_DOLCH_WORDS_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL|wxLC_REPORT|wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetUnusedDolchWordTabLabel());
            listView->SetName(BaseProjectView::GetUnusedDolchWordTabLabel());
            listView->EnableGridLines();
            listView->InsertColumn(0, BaseProjectView::GetUnusedDolchWordTabLabel());
            listView->InsertColumn(1, _(L"Category"));
            listView->SetVirtualDataProvider(GetUnusedDolchWordData());
            listView->SetVirtualDataSize(GetUnusedDolchWordData()->GetItemCount());
            listView->DistributeColumns();
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU") );
            UpdateListOptions(listView);
            view->GetDolchSightWordsView().AddWindow(listView);
            }
        }
    else
        { view->GetDolchSightWordsView().RemoveWindowById(BaseProjectView::UNUSED_DOLCH_WORDS_LIST_PAGE_ID); }
    }

//-------------------------------------------------------
void ProjectDoc::CalculateGraphData()
    {
    // syllable histogram
    m_syllableCounts->Clear();
    m_syllableCounts->AddContinuousColumn(GetSyllableCountsColumnName());
    m_syllableCounts->AddCategoricalColumn(GetGroupColumnName());
    m_syllableCounts->Reserve(GetTotalWords());
    for (auto wordPos = GetWords()->get_words().cbegin();
        wordPos != GetWords()->get_words().cend();
        ++wordPos)
        {
        if (GetInvalidSentenceMethod() == InvalidSentence::ExcludeFromAnalysis ||
            GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings)
            {
            if (wordPos->is_valid())
                {
                if (wordPos->is_numeric() &&
                    GetNumeralSyllabicationMethod() == NumeralSyllabize::WholeWordIsOneSyllable)
                    {
                    m_syllableCounts->AddRow(Data::RowInfo().
                        Continuous({ 1 }).Categoricals({ 0 }));
                    }
                else
                    {
                    m_syllableCounts->AddRow(Data::RowInfo().
                        Continuous({ static_cast<double>(wordPos->get_syllable_count()) }).
                        Categoricals({ 0 }).Id(wordPos->c_str()));
                    }
                }
            }
        else
            {
            if (wordPos->is_numeric() &&
                GetNumeralSyllabicationMethod() == NumeralSyllabize::WholeWordIsOneSyllable)
                {
                m_syllableCounts->AddRow(Data::RowInfo().
                    Continuous({ 1 }).
                    Categoricals({ 0 }));
                }
            else
                {
                m_syllableCounts->AddRow(Data::RowInfo().
                    Continuous({ static_cast<double>(wordPos->get_syllable_count()) } ).
                    Categoricals({ 0 }).Id(wordPos->c_str()));
                }
            }
        }
    // load the sentence word counts for the box plot/line chart
    m_sentenceWordLengths->Clear();
    m_sentenceWordLengths->AddContinuousColumn(GetSentenceWordCountsColumnName());
    m_sentenceWordLengths->AddContinuousColumn(GetSentenceIndicesColumnName());
    m_sentenceWordLengths->AddCategoricalColumn(GetGroupColumnName());
    m_sentenceWordLengths->Reserve(GetTotalSentences());
    if (GetInvalidSentenceMethod() == InvalidSentence::ExcludeFromAnalysis ||
        GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings)
        {
        for (auto paragraphIter = GetWords()->get_paragraphs().cbegin();
            paragraphIter != GetWords()->get_paragraphs().cend();
            ++paragraphIter)
            {
            if (paragraphIter->is_valid())
                {
                for (size_t sentenceIndex = paragraphIter->get_first_sentence_index();
                    sentenceIndex <= paragraphIter->get_last_sentence_index();
                    ++sentenceIndex)
                    {
                    if (GetWords()->get_sentences()[sentenceIndex].is_valid())
                        {
                        m_sentenceWordLengths->AddRow(Data::RowInfo().
                            Continuous({
                                static_cast<double>(GetWords()->get_sentences()[sentenceIndex].get_valid_word_count()),
                                static_cast<double>(sentenceIndex) }).
                            Categoricals({ static_cast<Data::GroupIdType>(
                                std::distance(GetWords()->get_paragraphs().cbegin(), paragraphIter)) }));
                        }
                    }
                }
            }
        }
    else
        {
        for (auto paragraphIter = GetWords()->get_paragraphs().cbegin();
             paragraphIter != GetWords()->get_paragraphs().cend();
             ++paragraphIter)
            {
            for (size_t sentenceIndex = paragraphIter->get_first_sentence_index();
                 sentenceIndex <= paragraphIter->get_last_sentence_index();
                 ++sentenceIndex)
                {
                m_sentenceWordLengths->AddRow(Data::RowInfo().
                    Continuous({ static_cast<double>(GetWords()->get_sentences()[sentenceIndex].get_word_count()),
                               static_cast<double>(sentenceIndex) }).
                    Categoricals({ static_cast<Data::GroupIdType>(
                        std::distance(GetWords()->get_paragraphs().cbegin(), paragraphIter)) }));
                }
            }
        }

    // calculate the outliers (using the box plot method)
    auto wordCountColumn = m_sentenceWordLengths->GetContinuousColumn(GetSentenceWordCountsColumnName());
    auto sentenceIndicesColumn = m_sentenceWordLengths->GetContinuousColumn(GetSentenceIndicesColumnName());
    statistics::find_outliers outlierInfo(wordCountColumn->GetValues());
    for (auto labelsPos = wordCountColumn->GetValues().cbegin();
         labelsPos != wordCountColumn->GetValues().cend();
         ++labelsPos)
        {
        const size_t sentenceIndex = sentenceIndicesColumn->GetValue(labelsPos - wordCountColumn->GetValues().cbegin());
        if (sentenceIndex < GetWords()->get_sentences().size())
            {
#ifndef NDEBUG
            [[maybe_unused]] const auto debugWordCount =
                (GetInvalidSentenceMethod() == InvalidSentence::IncludeAsFullSentences) ?
                 GetWords()->get_sentences()[sentenceIndex].get_word_count() :
                 GetWords()->get_sentences()[sentenceIndex].get_valid_word_count();
            assert(debugWordCount == *labelsPos);
#endif
            if (*labelsPos > outlierInfo.get_upper_outlier_boundary() ||
                *labelsPos < outlierInfo.get_lower_outlier_boundary())
                {
                auto punctPos = GetWords()->get_punctuation().begin();
                wxString currentSentence = wxString::Format(_(L"Sentence #%s (%s)"),
                    wxNumberFormatter::ToString(sentenceIndex+1/*add 1 to make it one-indexed*/,0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep),
                    ProjectReportFormat::FormatSentence(this, GetWords()->get_sentences()[sentenceIndex],
                        punctPos, GetWords()->get_punctuation().end()) );
                if (currentSentence.length() >= 100)
                    { currentSentence.Truncate(99).Append(wchar_t{ 8230 }); }
                m_sentenceWordLengths->GetIdColumn().
                    SetValue(labelsPos - wordCountColumn->GetValues().cbegin(), currentSentence.wc_str());
                }
            else
                {
                m_sentenceWordLengths->GetIdColumn().SetValue(labelsPos - wordCountColumn->GetValues().cbegin(),
                    wxString::Format(_(L"Sentence #%s"),
                        wxNumberFormatter::ToString(sentenceIndex+1/*add 1 to make it one-indexed*/,0,
                            wxNumberFormatter::Style::Style_NoTrailingZeroes|
                            wxNumberFormatter::Style::Style_WithThousandsSep)).wc_str() );
                }
            }
        else
            {
            wxFAIL_MSG(wxString::Format(
                L"Invalid sentence index (%i out of %i) when updating box plot labels!",
                sentenceIndex, GetWords()->get_sentences().size()));
            }
        }
    }
