///////////////////////////////////////////////////////////////////////////////
// Name:        tools_options_dlg.cpp
// Author:      Blake Madden
// Copyright:   (c) 2005-2023 Blake Madden
// Licence:     3-Clause BSD licence
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include <wx/wx.h>
#include <wx/cmndata.h>
#include <wx/colordlg.h>
#include <wx/bannerwindow.h>
#include "../../Wisteria-Dataviz/src/ui/dialogs/warningmessagesdlg.h"
#include "tools_options_dlg.h"
#include "editwordlistdlg.h"
#include "doc_group_select_dlg.h"
#include "../../app/readability_app_options.h"
#include "../../app/readability_app.h"
#include "../../projects/standard_project_doc.h"
#include "../../projects/batch_project_doc.h"
#include "../controls/wordlistproperty.h"

using namespace Wisteria;
using namespace Wisteria::Colors;
using namespace Wisteria::Graphs;
using namespace Wisteria::GraphItems;
using namespace Wisteria::UI;

wxDECLARE_APP(ReadabilityApp);

wxIMPLEMENT_CLASS(ToolsOptionsDlg, wxDialog)

wxBEGIN_EVENT_TABLE(ToolsOptionsDlg, wxDialog)
    EVT_BUTTON(wxID_OK, ToolsOptionsDlg::OnOK)
    EVT_BUTTON(ID_EXPORT_SETTINGS_BUTTON, ToolsOptionsDlg::OnExportSettings)
    EVT_BUTTON(ID_LOAD_SETTINGS_BUTTON, ToolsOptionsDlg::OnImportSettings)
    EVT_BUTTON(ID_RESET_SETTINGS_BUTTON, ToolsOptionsDlg::OnResetSettings)
    EVT_BUTTON(wxID_HELP, ToolsOptionsDlg::OnHelp)
    EVT_HELP(wxID_ANY, ToolsOptionsDlg::OnContextHelp)
    EVT_BUTTON(ID_HIGHLIGHT_COLOR_BUTTON, ToolsOptionsDlg::OnHighlightColorSelect)
    EVT_BUTTON(ID_EXCLUDED_HIGHLIGHT_COLOR_BUTTON, ToolsOptionsDlg::OnExcludedHighlightColorSelect)
    EVT_BUTTON(ID_DUP_WORD_COLOR_BUTTON, ToolsOptionsDlg::OnDupWordHighlightColorSelect)
    EVT_BUTTON(ID_WORDY_PHRASE_COLOR_BUTTON, ToolsOptionsDlg::OnWordyPhraseHighlightColorSelect)
    EVT_BUTTON(ID_FONT_BUTTON, ToolsOptionsDlg::OnFontSelect)
    EVT_BUTTON(ID_X_AXIS_FONT_BUTTON, ToolsOptionsDlg::OnFontSelect)
    EVT_BUTTON(ID_Y_AXIS_FONT_BUTTON, ToolsOptionsDlg::OnFontSelect)
    EVT_BUTTON(ID_GRAPH_TOP_TITLE_FONT_BUTTON, ToolsOptionsDlg::OnFontSelect)
    EVT_BUTTON(ID_GRAPH_BOTTOM_TITLE_FONT_BUTTON, ToolsOptionsDlg::OnFontSelect)
    EVT_BUTTON(ID_GRAPH_LEFT_TITLE_FONT_BUTTON, ToolsOptionsDlg::OnFontSelect)
    EVT_BUTTON(ID_GRAPH_RIGHT_TITLE_FONT_BUTTON, ToolsOptionsDlg::OnFontSelect)
    EVT_CHOICE(ID_TEXT_EXCLUDE_METHOD, ToolsOptionsDlg::OnIncompleteSentencesChange)
    EVT_RADIOBOX(ID_DOCUMENT_STORAGE_RADIO_BOX, ToolsOptionsDlg::OnDocumentStorageRadioButtonClick)
    EVT_CHOICE(ID_PARAGRAPH_PARSE, ToolsOptionsDlg::OnParagraphParseChange)
    EVT_CHOICE(ID_NUMBER_SYLLABIZE_METHOD, ToolsOptionsDlg::OnNumberSyllabizeChange)
    EVT_CHOICE(ID_EXCLUSION_TAG_BLOCK_SELCTION, ToolsOptionsDlg::OnExclusionBlockTagChange)
    EVT_BUTTON(ID_FILE_BROWSE_BUTTON, ToolsOptionsDlg::OnFileBrowseButtonClick)
    EVT_BUTTON(ID_ADDITIONAL_FILE_BROWSE_BUTTON, ToolsOptionsDlg::OnAdditionalDocumentFileBrowseButtonClick)
    EVT_BUTTON(ID_EXCLUDED_PHRASES_FILE_EDIT_BUTTON, ToolsOptionsDlg::OnExcludedPhrasesFileEditButtonClick)
    EVT_BUTTON(ToolsOptionsDlg::ID_WARNING_MESSAGES_BUTTON, ToolsOptionsDlg::OnWarningMessagesButtonClick)
    EVT_BUTTON(ID_DOLCH_CONJUNCTIONS_COLOR_BUTTON, ToolsOptionsDlg::OnDolchConjunctionsHighlightColorSelect)
    EVT_BUTTON(ID_DOLCH_PREPOSITIONS_COLOR_BUTTON, ToolsOptionsDlg::OnDolchPrepositionsHighlightColorSelect)
    EVT_BUTTON(ID_DOLCH_PRONOUNS_COLOR_BUTTON, ToolsOptionsDlg::OnDolchPronounsHighlightColorSelect)
    EVT_BUTTON(ID_DOLCH_ADVERBS_COLOR_BUTTON, ToolsOptionsDlg::OnDolchAdverbsHighlightColorSelect)
    EVT_BUTTON(ID_DOLCH_ADJECTIVES_COLOR_BUTTON, ToolsOptionsDlg::OnDolchHighlightColorSelect)
    EVT_BUTTON(ID_DOLCH_VERBS_COLOR_BUTTON, ToolsOptionsDlg::OnDolchHighlightColorSelect)
    EVT_BUTTON(ID_DOLCH_NOUN_COLOR_BUTTON, ToolsOptionsDlg::OnDolchNounHighlightColorSelect)
    EVT_BUTTON(ID_ADD_FILE_BUTTON, ToolsOptionsDlg::OnAddFileClick)
    EVT_BUTTON(ID_ADD_FILES_BUTTON, ToolsOptionsDlg::OnAddFilesClick)
    EVT_BUTTON(ID_DELETE_FILE_BUTTON, ToolsOptionsDlg::OnDeleteFileClick)
    EVT_CHECKBOX(ID_EXCLUDE_NUMERALS_CHECKBOX, ToolsOptionsDlg::OnExcludeNumeralsCheck)
wxEND_EVENT_TABLE()

void ToolsOptionsDlg::OnExcludeNumeralsCheck(wxCommandEvent& event)
    {
    if (m_syllableLabel)
        { m_syllableLabel->Enable(!event.IsChecked()); }
    if (m_syllableCombo)
        { m_syllableCombo->Enable(!event.IsChecked()); }
    if (m_readTestsSyllableLabel)
        { m_readTestsSyllableLabel->Enable(!event.IsChecked()); }
    if (IsPropertyAvailable(m_readabilityTestsPropertyGrid, GetFleschNumeralSyllabicationLabel()))
        { m_readabilityTestsPropertyGrid->EnableProperty(GetFleschNumeralSyllabicationLabel(), !event.IsChecked()); }
    if (IsPropertyAvailable(m_readabilityTestsPropertyGrid, GetFleschKincaidNumeralSyllabicationLabel()))
        {
        m_readabilityTestsPropertyGrid->EnableProperty(
            GetFleschKincaidNumeralSyllabicationLabel(), !event.IsChecked());
        }
    }

void ToolsOptionsDlg::OnExclusionBlockTagChange([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();
    m_exclusionBlockTags.get_value().clear();
    if (m_exclusionBlockTagsOption == 1)
        { m_exclusionBlockTags.get_value().push_back(std::make_pair(L'^',L'^')); }
    else if (m_exclusionBlockTagsOption == 2)
        { m_exclusionBlockTags.get_value().push_back(std::make_pair(L'<',L'>')); }
    else if (m_exclusionBlockTagsOption == 3)
        { m_exclusionBlockTags.get_value().push_back(std::make_pair(L'[',L']')); }
    else if (m_exclusionBlockTagsOption == 4)
        { m_exclusionBlockTags.get_value().push_back(std::make_pair(L'{',L'}')); }
    else if (m_exclusionBlockTagsOption == 5)
        { m_exclusionBlockTags.get_value().push_back(std::make_pair(L'(',L')')); }
    }

void ToolsOptionsDlg::OnNumberSyllabizeChange([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();
    if (m_readTestsSyllableLabel)
        {
        m_readTestsSyllableLabel->SetLabel(
            (m_syllabicationMethod == static_cast<int>(NumeralSyllabize::WholeWordIsOneSyllable)) ?
            _(L"* Numeral syllabication system default: numerals are one syllable.") :
            _(L"* Numeral syllabication system default: sound out each digit."));
        }
    }

void ToolsOptionsDlg::OnDeleteFileClick([[maybe_unused]] wxCommandEvent& event)
    {
    if (m_fileList)
        { m_fileList->DeleteSelectedItems(); }
    }

void ToolsOptionsDlg::OnAddFileClick([[maybe_unused]] wxCommandEvent& event)
    {
    if (m_fileList)
        { m_fileList->EditItem(m_fileList->AddRow(), 0); }
    }

void ToolsOptionsDlg::OnAddFilesClick([[maybe_unused]] wxCommandEvent& event)
    {
    wxFileDialog dialog
            (
            this,
            _(L"Add Document(s) to Project"),
            wxEmptyString,
            wxEmptyString,
            wxGetApp().GetAppOptions().GetDocumentFilter(),
            wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_PREVIEW|wxFD_MULTIPLE);

    if (dialog.ShowModal() != wxID_OK)
        { return; }

    wxArrayString files;
    dialog.GetPaths(files);

    if (files.GetCount() == 0)
        { return; }

    // see what sort of labeling should be used
    DocGroupSelectDlg selectLabelTypeDlg(this);
    if (selectLabelTypeDlg.ShowModal() != wxID_OK)
        { return; }

    const size_t currentFileCount = m_fileData->GetItemCount();
    m_fileData->SetSize(currentFileCount+files.GetCount(), 2);
    for (size_t i = 0; i < files.GetCount(); ++i)
        {
        m_fileData->SetItemText(currentFileCount+i, 0, files.Item(i));
        if (selectLabelTypeDlg.GetSelection() == 1)
            { m_fileData->SetItemText(currentFileCount+i, 1, selectLabelTypeDlg.GetGroupingLabel()); }
        }
    m_fileList->SetVirtualDataSize(m_fileData->GetItemCount());
    m_fileList->SetColumnWidth(0, m_fileList->GetClientSize().GetWidth()*.75);
    m_fileList->SetColumnWidth(1, m_fileList->GetClientSize().GetWidth()*.25);
    m_fileList->SetItemBeenEditedByUser(true);
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnIncompleteSentencesChange([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();
    if (m_syllableLabel)
        {
        m_syllableLabel->Enable(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::IncludeAsFullSentences)) ||
            !m_ignoreNumerals);
        }
    if (m_syllableCombo)
        {
        m_syllableCombo->Enable(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::IncludeAsFullSentences)) ||
            !m_ignoreNumerals);
        }
    if (m_readTestsSyllableLabel)
        {
        m_readTestsSyllableLabel->Enable(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::IncludeAsFullSentences)) ||
            !m_ignoreNumerals);
        }
    if (IsPropertyAvailable(m_readabilityTestsPropertyGrid, GetFleschNumeralSyllabicationLabel()))
        {
        m_readabilityTestsPropertyGrid->EnableProperty(GetFleschNumeralSyllabicationLabel(),
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::IncludeAsFullSentences)) ||
            !m_ignoreNumerals);
        }
    if (IsPropertyAvailable(m_readabilityTestsPropertyGrid, GetFleschKincaidNumeralSyllabicationLabel()))
        { m_readabilityTestsPropertyGrid->EnableProperty(GetFleschKincaidNumeralSyllabicationLabel(), (m_textExclusionMethod == static_cast<int>(InvalidSentence::IncludeAsFullSentences)) || !m_ignoreNumerals); }
    if (m_ignoreCopyrightsCheckBox)
        { m_ignoreCopyrightsCheckBox->Enable((m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) || (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings))); }
    if (m_ignoreCitationsCheckBox)
        { m_ignoreCitationsCheckBox->Enable((m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) || (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings))); }
    if (m_ignoreFileAddressesCheckBox)
        { m_ignoreFileAddressesCheckBox->Enable((m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) || (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings))); }
    if (m_ignoreNumeralsCheckBox)
        { m_ignoreNumeralsCheckBox->Enable((m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) || (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings))); }
    if (m_ignoreProperNounsCheckBox)
        { m_ignoreProperNounsCheckBox->Enable((m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) || (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings))); }
    if (m_includeExcludedPhraseFirstOccurrenceCheckBox)
        { m_includeExcludedPhraseFirstOccurrenceCheckBox->Enable((m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) || (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings))); }
    if (m_excludedPhrasesPathFilePathEdit)
        { m_excludedPhrasesPathFilePathEdit->Enable((m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) || (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings))); }
    if (m_excludedPhrasesEditBrowseButton)
        { m_excludedPhrasesEditBrowseButton->Enable((m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) || (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings))); }
    if (m_exclusionBlockTagsCombo)
        { m_exclusionBlockTagsCombo->Enable((m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) || (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings))); }
    if (m_exclusionBlockTagsLabel)
        { m_exclusionBlockTagsLabel->Enable((m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) || (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings))); }
    if (m_aggressiveExclusionCheckBox)
        { m_aggressiveExclusionCheckBox->Enable((m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) || (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings))); }
    if (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis))
        {
        if (m_textExclusionLabel)
            {
            m_textExclusionLabel->SetLabel(
                _(L"** Text exclusion system default: exclude all incomplete sentences."));
            }
        }
    else if (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings))
        {
        if (m_textExclusionLabel)
            {
            m_textExclusionLabel->SetLabel(
                _(L"** Text exclusion system default: exclude all incomplete sentences, except headings."));
            }
        }
    else if (m_textExclusionMethod == static_cast<int>(InvalidSentence::IncludeAsFullSentences))
        {
        if (m_textExclusionLabel)
            { m_textExclusionLabel->SetLabel(_(L"** Text exclusion system default: do not exclude any text.")); }
        }
    TransferDataToWindow();
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnParagraphParseChange([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();
    if (m_ignoreBlankLinesCheckBox)
        {
        m_ignoreBlankLinesCheckBox->Enable(
            static_cast<ParagraphParse>(m_paragraphParsingMethod.get_value()) ==
                ParagraphParse::OnlySentenceTerminatedNewLinesAreParagraphs);
        }
    if (m_ignoreIndentingCheckBox)
        {
        m_ignoreIndentingCheckBox->Enable(
            static_cast<ParagraphParse>(m_paragraphParsingMethod.get_value()) ==
            ParagraphParse::OnlySentenceTerminatedNewLinesAreParagraphs);
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnWarningMessagesButtonClick([[maybe_unused]] wxCommandEvent& event)
    {
    WarningMessagesDlg dlg(this);
    dlg.SetHelpTopic(wxGetApp().GetMainFrame()->GetHelpDirectory(), L"options-general-settings.html");
    dlg.ShowModal();
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnExcludedPhrasesFileEditButtonClick([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();
    EditWordListDlg editDlg(this,
        wxID_ANY, _(L"Edit Words/Phrases To Exclude"));
    editDlg.SetHelpTopic(wxGetApp().GetMainFrame()->GetHelpDirectory(), L"document-analysis.html");
    editDlg.SetPhraseFileMode(true);
    editDlg.SetFilePath(m_excludedPhrasesPath);
    if (editDlg.ShowModal() != wxID_OK)
        { return; }

    m_excludedPhrasesPath = editDlg.GetFilePath();
    m_excludedPhrasesEdited = true;
    TransferDataToWindow();
    SetFocus();
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnDocumentStorageRadioButtonClick([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();
    if (m_filePathEdit)
        { m_filePathEdit->Enable(m_documentStorageMethod == static_cast<int>(TextStorage::NoEmbedText)); }
    if (m_fileBrowseButton)
        { m_fileBrowseButton->Enable(m_documentStorageMethod == static_cast<int>(TextStorage::NoEmbedText)); }
    if (m_fileList && m_addFileButton && m_deleteFileButton && m_addFilesButton)
        {
        m_fileList->Enable(m_documentStorageMethod == static_cast<int>(TextStorage::NoEmbedText));
        m_addFileButton->Enable(m_documentStorageMethod == static_cast<int>(TextStorage::NoEmbedText));
        m_deleteFileButton->Enable(m_documentStorageMethod == static_cast<int>(TextStorage::NoEmbedText));
        m_addFilesButton->Enable(m_documentStorageMethod == static_cast<int>(TextStorage::NoEmbedText));
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnAdditionalDocumentFileBrowseButtonClick([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();
    wxFileName fn(m_appendedDocumentFilePath);
    wxFileDialog dialog
            (this,
            _(L"Select Document to Append"),
            fn.GetPath(),
            fn.GetFullName(),
            wxGetApp().GetAppOptions().GetDocumentFilter(),
            wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_PREVIEW);
    if (dialog.ShowModal() != wxID_OK)
        { return; }

    m_appendedDocumentFilePath = dialog.GetPath();
    TransferDataToWindow();
    SetFocus();
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnFileBrowseButtonClick([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();
    wxFileName fn(m_filePath);
    wxFileDialog dialog
            (this,
            _(L"Select Document to Analyze"),
            fn.GetPath(),
            fn.GetFullName(),
            wxGetApp().GetAppOptions().GetDocumentFilter(),
            wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_PREVIEW);
    if (dialog.ShowModal() != wxID_OK)
        { return; }

    m_filePath = dialog.GetPath();
    TransferDataToWindow();
    SetFocus();
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnFontSelect(wxCommandEvent& event)
    {
    wxFontData data;
    switch (event.GetId())
        {
    case ID_FONT_BUTTON:
        data.SetInitialFont(m_font);
        data.SetColour(m_fontColor);
        break;
    case ID_X_AXIS_FONT_BUTTON:
        data.SetInitialFont(m_xAxisFont);
        data.SetColour(m_xAxisFontColor);
        break;
    case ID_Y_AXIS_FONT_BUTTON:
        data.SetInitialFont(m_yAxisFont);
        data.SetColour(m_yAxisFontColor);
        break;
    case ID_GRAPH_TOP_TITLE_FONT_BUTTON:
        data.SetInitialFont(m_topTitleFont);
        data.SetColour(m_topTitleFontColor);
        break;
    case ID_GRAPH_BOTTOM_TITLE_FONT_BUTTON:
        data.SetInitialFont(m_bottomTitleFont);
        data.SetColour(m_bottomTitleFontColor);
        break;
    case ID_GRAPH_LEFT_TITLE_FONT_BUTTON:
        data.SetInitialFont(m_leftTitleFont);
        data.SetColour(m_leftTitleFontColor);
        break;
    case ID_GRAPH_RIGHT_TITLE_FONT_BUTTON:
        data.SetInitialFont(m_rightTitleFont);
        data.SetColour(m_rightTitleFontColor);
        break;
        };
    wxFontDialog dialog(this, data);
    if (dialog.ShowModal() == wxID_OK)
        {
        switch (event.GetId())
            {
        case ID_FONT_BUTTON:
            m_font = dialog.GetFontData().GetChosenFont();
            m_fontColor = dialog.GetFontData().GetColour();
            break;
        case ID_X_AXIS_FONT_BUTTON:
            m_xAxisFont = dialog.GetFontData().GetChosenFont();
            m_xAxisFontColor = dialog.GetFontData().GetColour();
            break;
        case ID_Y_AXIS_FONT_BUTTON:
            m_yAxisFont = dialog.GetFontData().GetChosenFont();
            m_yAxisFontColor = dialog.GetFontData().GetColour();
            break;
        case ID_GRAPH_TOP_TITLE_FONT_BUTTON:
            m_topTitleFont = dialog.GetFontData().GetChosenFont();
            m_topTitleFontColor = dialog.GetFontData().GetColour();
            break;
        case ID_GRAPH_BOTTOM_TITLE_FONT_BUTTON:
            m_bottomTitleFont = dialog.GetFontData().GetChosenFont();
            m_bottomTitleFontColor = dialog.GetFontData().GetColour();
            break;
        case ID_GRAPH_LEFT_TITLE_FONT_BUTTON:
            m_leftTitleFont = dialog.GetFontData().GetChosenFont();
            m_leftTitleFontColor = dialog.GetFontData().GetColour();
            break;
        case ID_GRAPH_RIGHT_TITLE_FONT_BUTTON:
            m_rightTitleFont = dialog.GetFontData().GetChosenFont();
            m_rightTitleFontColor = dialog.GetFontData().GetColour();
            break;
            };
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnHighlightColorSelect([[maybe_unused]] wxCommandEvent& event)
    {
    if (m_highlightColorButton == nullptr)
        { return; }
    wxColourData data;
    wxGetApp().GetAppOptions().CopyCustomColoursToColourData(data);
    data.SetChooseFull(true);
    data.SetColour(m_highlightedColor);

    wxColourDialog dialog(this, &data);
    if (dialog.ShowModal() == wxID_OK)
        {
        m_highlightedColor = dialog.GetColourData().GetColour();
        m_highlightColorButton->SetBitmapLabel(
            ResourceManager::CreateColorIcon(m_highlightedColor));
        wxGetApp().GetAppOptions().CopyColourDataToCustomColours(dialog.GetColourData());
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnExcludedHighlightColorSelect([[maybe_unused]] wxCommandEvent& event)
    {
    if (m_excludedHighlightColorButton == nullptr)
        { return; }
    wxColourData data;
    wxGetApp().GetAppOptions().CopyCustomColoursToColourData(data);
    data.SetChooseFull(true);
    data.SetColour(m_excludedTextHighlightColor);

    wxColourDialog dialog(this, &data);
    if (dialog.ShowModal() == wxID_OK)
        {
        m_excludedTextHighlightColor = dialog.GetColourData().GetColour();
        m_excludedHighlightColorButton->SetBitmapLabel(
            ResourceManager::CreateColorIcon(m_excludedTextHighlightColor));
        wxGetApp().GetAppOptions().CopyColourDataToCustomColours(dialog.GetColourData());
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnDupWordHighlightColorSelect([[maybe_unused]] wxCommandEvent& event)
    {
    if (m_duplicateWordHighlightColorButton == nullptr)
        { return; }
    wxColourData data;
    wxGetApp().GetAppOptions().CopyCustomColoursToColourData(data);
    data.SetChooseFull(true);
    data.SetColour(m_duplicateWordHighlightColor);

    wxColourDialog dialog(this, &data);
    if (dialog.ShowModal() == wxID_OK)
        {
        m_duplicateWordHighlightColor = dialog.GetColourData().GetColour();
        m_duplicateWordHighlightColorButton->SetBitmapLabel(
            ResourceManager::CreateColorIcon(m_duplicateWordHighlightColor));
        wxGetApp().GetAppOptions().CopyColourDataToCustomColours(dialog.GetColourData());
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnWordyPhraseHighlightColorSelect([[maybe_unused]] wxCommandEvent& event)
    {
    if (m_wordyPhraseHighlightColorButton == nullptr)
        { return; }
    wxColourData data;
    wxGetApp().GetAppOptions().CopyCustomColoursToColourData(data);
    data.SetChooseFull(true);
    data.SetColour(m_wordyPhraseHighlightColor);

    wxColourDialog dialog(this, &data);
    if (dialog.ShowModal() == wxID_OK)
        {
        m_wordyPhraseHighlightColor = dialog.GetColourData().GetColour();
        m_wordyPhraseHighlightColorButton->SetBitmapLabel(
            ResourceManager::CreateColorIcon(m_wordyPhraseHighlightColor));
        wxGetApp().GetAppOptions().CopyColourDataToCustomColours(dialog.GetColourData());
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnDolchConjunctionsHighlightColorSelect([[maybe_unused]] wxCommandEvent& event)
    {
    if (m_DolchConjunctionsColorButton == nullptr)
        { return; }
    wxColourData data;
    wxGetApp().GetAppOptions().CopyCustomColoursToColourData(data);
    data.SetChooseFull(true);
    data.SetColour(m_dolchConjunctionsColor);

    wxColourDialog dialog(this, &data);
    if (dialog.ShowModal() == wxID_OK)
        {
        m_dolchConjunctionsColor = dialog.GetColourData().GetColour();
        m_DolchConjunctionsColorButton->SetBitmapLabel(
            ResourceManager::CreateColorIcon(m_dolchConjunctionsColor));
        wxGetApp().GetAppOptions().CopyColourDataToCustomColours(dialog.GetColourData());
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnDolchPrepositionsHighlightColorSelect([[maybe_unused]] wxCommandEvent& event)
    {
    if (m_DolchPrepositionsColorButton == nullptr)
        { return; }
    wxColourData data;
    wxGetApp().GetAppOptions().CopyCustomColoursToColourData(data);
    data.SetChooseFull(true);
    data.SetColour(m_dolchPrepositionsColor);

    wxColourDialog dialog(this, &data);
    if (dialog.ShowModal() == wxID_OK)
        {
        m_dolchPrepositionsColor = dialog.GetColourData().GetColour();
        m_DolchPrepositionsColorButton->SetBitmapLabel(
            ResourceManager::CreateColorIcon(m_dolchPrepositionsColor));
        wxGetApp().GetAppOptions().CopyColourDataToCustomColours(dialog.GetColourData());
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnDolchPronounsHighlightColorSelect([[maybe_unused]] wxCommandEvent& event)
    {
    if (m_DolchPronounsColorButton == nullptr)
        { return; }
    wxColourData data;
    wxGetApp().GetAppOptions().CopyCustomColoursToColourData(data);
    data.SetChooseFull(true);
    data.SetColour(m_dolchPronounsColor);

    wxColourDialog dialog(this, &data);
    if (dialog.ShowModal() == wxID_OK)
        {
        m_dolchPronounsColor = dialog.GetColourData().GetColour();
        m_DolchPronounsColorButton->SetBitmapLabel(
            ResourceManager::CreateColorIcon(m_dolchPronounsColor));
        wxGetApp().GetAppOptions().CopyColourDataToCustomColours(dialog.GetColourData());
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnDolchAdverbsHighlightColorSelect([[maybe_unused]] wxCommandEvent& event)
    {
    if (m_DolchAdverbsColorButton == nullptr)
        { return; }
    wxColourData data;
    wxGetApp().GetAppOptions().CopyCustomColoursToColourData(data);
    data.SetChooseFull(true);
    data.SetColour(m_dolchAdverbsColor);

    wxColourDialog dialog(this, &data);
    if (dialog.ShowModal() == wxID_OK)
        {
        m_dolchAdverbsColor = dialog.GetColourData().GetColour();
        m_DolchAdverbsColorButton->SetBitmapLabel(
            ResourceManager::CreateColorIcon(m_dolchAdverbsColor));
        wxGetApp().GetAppOptions().CopyColourDataToCustomColours(dialog.GetColourData());
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnDolchHighlightColorSelect(wxCommandEvent& event)
    {
    if (event.GetId() == ID_DOLCH_ADJECTIVES_COLOR_BUTTON)
        {
        wxColourData data;
        wxGetApp().GetAppOptions().CopyCustomColoursToColourData(data);
        data.SetChooseFull(true);
        data.SetColour(m_dolchAdjectivesColor);

        wxColourDialog dialog(this, &data);
        if (dialog.ShowModal() == wxID_OK)
            {
            m_dolchAdjectivesColor = dialog.GetColourData().GetColour();
            m_DolchAdjectivesColorButton->SetBitmapLabel(
                ResourceManager::CreateColorIcon(m_dolchAdjectivesColor));
            wxGetApp().GetAppOptions().CopyColourDataToCustomColours(dialog.GetColourData());
            }
        }
    else if (event.GetId() == ID_DOLCH_VERBS_COLOR_BUTTON)
        {
        wxColourData data;
        wxGetApp().GetAppOptions().CopyCustomColoursToColourData(data);
        data.SetChooseFull(true);
        data.SetColour(m_dolchVerbsColor);

        wxColourDialog dialog(this, &data);
        if (dialog.ShowModal() == wxID_OK)
            {
            m_dolchVerbsColor = dialog.GetColourData().GetColour();
            m_DolchVerbsColorButton->SetBitmapLabel(
                ResourceManager::CreateColorIcon(m_dolchVerbsColor));
            wxGetApp().GetAppOptions().CopyColourDataToCustomColours(dialog.GetColourData());
            }
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnDolchNounHighlightColorSelect([[maybe_unused]] wxCommandEvent& event)
    {
    if (m_DolchNounColorButton == nullptr)
        { return; }
    wxColourData data;
    wxGetApp().GetAppOptions().CopyCustomColoursToColourData(data);
    data.SetChooseFull(true);
    data.SetColour(m_dolchNounColor);

    wxColourDialog dialog(this, &data);
    if (dialog.ShowModal() == wxID_OK)
        {
        m_dolchNounColor = dialog.GetColourData().GetColour();
        m_DolchNounColorButton->SetBitmapLabel(
            ResourceManager::CreateColorIcon(m_dolchNounColor));
        wxGetApp().GetAppOptions().CopyColourDataToCustomColours(dialog.GetColourData());
        }
    }

//-------------------------------------------------------------
ToolsOptionsDlg::ToolsOptionsDlg(wxWindow* parent, BaseProjectDoc* project /*= nullptr*/,
                                 const ToolSections sectionsToInclude /*= AllSections*/)  :
    m_readabilityProjectDoc(project),
    m_userAgent(wxGetApp().GetAppOptions().GetUserAgent()),
    // project settings
    m_projectLanguage(static_cast<int>(project ?
        project->GetProjectLanguage() : wxGetApp().GetAppOptions().GetProjectLanguage())),
    m_reviewer(project ? project->GetReviewer() : wxGetApp().GetAppOptions().GetReviewer()),
    m_status(project ? project->GetStatus() : wxGetApp().GetAppOptions().GetStatus()),
    m_description((project && project->GetSourceFilesInfo().size()) ?
        project->GetSourceFilesInfo().at(0).second : wxString()),
    m_appendedDocumentFilePath(project ?
        project->GetAppendedDocumentFilePath() : wxGetApp().GetAppOptions().GetAppendedDocumentFilePath()),
    // general program options needs to show everything
    m_sectionsBeingShown(project ? sectionsToInclude : AllSections),
    // document storage/linking information
    m_documentStorageMethod(project ?
        static_cast<int>(project->GetDocumentStorageMethod()) :
        static_cast<int>(wxGetApp().GetAppOptions().GetDocumentStorageMethod())),
    m_filePath(project ? project->GetOriginalDocumentFilePath() : wxString{}),
    // text view highlighting
    m_textHighlightMethod(project ?
        static_cast<int>(project->GetTextHighlightMethod()) :
        static_cast<int>(wxGetApp().GetAppOptions().GetTextHighlightMethod())),
    m_highlightedColor(project ?
        project->GetTextHighlightColor() :
        wxGetApp().GetAppOptions().GetTextHighlightColor() ),
    m_excludedTextHighlightColor(project ?
        project->GetExcludedTextHighlightColor() :
        wxGetApp().GetAppOptions().GetExcludedTextHighlightColor() ),
    m_duplicateWordHighlightColor(project ?
        project->GetDuplicateWordHighlightColor() :
        wxGetApp().GetAppOptions().GetDuplicateWordHighlightColor() ),
    m_wordyPhraseHighlightColor(project ?
        project->GetWordyPhraseHighlightColor() :
        wxGetApp().GetAppOptions().GetWordyPhraseHighlightColor() ),
    m_font(project ?
        project->GetTextViewFont() :
        wxGetApp().GetAppOptions().GetTextViewFont() ),
    m_fontColor(project ?
        project->GetTextFontColor() :
        wxGetApp().GetAppOptions().GetTextFontColor() ),
    // dolch
    m_dolchConjunctionsColor(project ?
        project->GetDolchConjunctionsColor() :
        wxGetApp().GetAppOptions().GetDolchConjunctionsColor()),
    m_dolchPrepositionsColor(project ?
        project->GetDolchPrepositionsColor() :
        wxGetApp().GetAppOptions().GetDolchPrepositionsColor()),
    m_dolchPronounsColor(project ?
        project->GetDolchPronounsColor() :
        wxGetApp().GetAppOptions().GetDolchPronounsColor()),
    m_dolchAdverbsColor(project ?
        project->GetDolchAdverbsColor() :
        wxGetApp().GetAppOptions().GetDolchAdverbsColor()),
    m_dolchAdjectivesColor(project ?
        project->GetDolchAdjectivesColor() :
        wxGetApp().GetAppOptions().GetDolchAdjectivesColor()),
    m_dolchVerbsColor(project ?
        project->GetDolchVerbsColor() :
        wxGetApp().GetAppOptions().GetDolchVerbsColor()),
    m_dolchNounColor(project ?
        project->GetDolchNounColor() :
        wxGetApp().GetAppOptions().GetDolchNounColor()),
    m_highlightDolchConjunctions(project ?
        project->IsHighlightingDolchConjunctions() :
        wxGetApp().GetAppOptions().IsHighlightingDolchConjunctions()),
    m_highlightDolchPrepositions(project ?
        project->IsHighlightingDolchPrepositions() :
        wxGetApp().GetAppOptions().IsHighlightingDolchPrepositions()),
    m_highlightDolchPronouns(project ?
        project->IsHighlightingDolchPronouns() :
        wxGetApp().GetAppOptions().IsHighlightingDolchPronouns()),
    m_highlightDolchAdverbs(project ?
        project->IsHighlightingDolchAdverbs() :
        wxGetApp().GetAppOptions().IsHighlightingDolchAdverbs()),
    m_highlightDolchAdjectives(project ?
        project->IsHighlightingDolchAdjectives() :
        wxGetApp().GetAppOptions().IsHighlightingDolchAdjectives()),
    m_highlightDolchVerbs(project ?
        project->IsHighlightingDolchVerbs() :
        wxGetApp().GetAppOptions().IsHighlightingDolchVerbs()),
    m_highlightDolchNouns(project ?
        project->IsHighlightingDolchNouns() :
        wxGetApp().GetAppOptions().IsHighlightingDolchNouns()),
    // long sentence method
    m_longSentencesNumberOfWords(project ?
    (project->GetLongSentenceMethod() == LongSentence::LongerThanSpecifiedLength) :
        (wxGetApp().GetAppOptions().GetLongSentenceMethod() == LongSentence::LongerThanSpecifiedLength)),
    m_sentenceLength(project ?
        project->GetDifficultSentenceLength() :
        wxGetApp().GetAppOptions().GetDifficultSentenceLength()),
    m_longSentencesOutliers(project ?
    (project->GetLongSentenceMethod() == LongSentence::OutlierLength) :
        (wxGetApp().GetAppOptions().GetLongSentenceMethod() == LongSentence::OutlierLength)),
    // batch project options
    m_minDocWordCountForBatch(project ?
        project->GetMinDocWordCountForBatch() : wxGetApp().GetAppOptions().GetMinDocWordCountForBatch()),
    m_filePathTruncationMode(project ?
        static_cast<int>(project->GetFilePathTruncationMode()) :
        static_cast<int>(wxGetApp().GetAppOptions().GetFilePathTruncationMode())),
    // number syllabizing
    m_syllabicationMethod(project ?
        static_cast<int>(project->GetNumeralSyllabicationMethod()) :
        static_cast<int>(wxGetApp().GetAppOptions().GetNumeralSyllabicationMethod())),
    // paragraph parsing
    m_paragraphParsingMethod(project ?
        static_cast<int>(project->GetParagraphsParsingMethod()) :
        static_cast<int>(wxGetApp().GetAppOptions().GetParagraphsParsingMethod())),
    m_ignoreBlankLinesForParagraphsParser(project ?
        project->GetIgnoreBlankLinesForParagraphsParser() :
        wxGetApp().GetAppOptions().GetIgnoreBlankLinesForParagraphsParser() ),
    m_ignoreIndentingForParagraphsParser(project ?
        project->GetIgnoreIndentingForParagraphsParser() :
        wxGetApp().GetAppOptions().GetIgnoreIndentingForParagraphsParser() ),
    m_sentenceStartMustBeUppercased(project ?
        project->GetSentenceStartMustBeUppercased() :
        wxGetApp().GetAppOptions().GetSentenceStartMustBeUppercased() ),
    m_aggressiveExclusion(project ?
        project->IsExcludingAggressively() :
        wxGetApp().GetAppOptions().IsExcludingAggressively() ),
    m_ignoreTrailingCopyrightNoticeParagraphs(project ?
        project->IsIgnoringTrailingCopyrightNoticeParagraphs() :
        wxGetApp().GetAppOptions().IsIgnoringTrailingCopyrightNoticeParagraphs() ),
    m_ignoreTrailingCitations(project ?
        project->IsIgnoringTrailingCitations() :
        wxGetApp().GetAppOptions().IsIgnoringTrailingCitations() ),
    m_ignoreFileAddresses(project ?
        project->IsIgnoringFileAddresses() :
        wxGetApp().GetAppOptions().IsIgnoringFileAddresses() ),
    m_ignoreNumerals(project ?
        project->IsIgnoringNumerals() :
        wxGetApp().GetAppOptions().IsIgnoringNumerals() ),
    m_ignoreProperNouns(project ?
        project->IsIgnoringProperNouns() :
        wxGetApp().GetAppOptions().IsIgnoringProperNouns() ),
    m_excludedPhrasesPath(project ?
        project->GetExcludedPhrasesPath() :
        wxGetApp().GetAppOptions().GetExcludedPhrasesPath() ),
    // used to track whether user edited this list from this dialog
    m_excludedPhrasesEdited(false),
    m_includeExcludedPhraseFirstOccurrence(project ?
        project->IsIncludingExcludedPhraseFirstOccurrence() :
        wxGetApp().GetAppOptions().IsIncludingExcludedPhraseFirstOccurrence() ),
    m_exclusionBlockTags(project ?
        project->GetExclusionBlockTags() :
        wxGetApp().GetAppOptions().GetExclusionBlockTags()),
    // header/list analysis
    m_includeIncompleteSentencesIfLongerThan(project ?
        project->GetIncludeIncompleteSentencesIfLongerThanValue() :
        wxGetApp().GetAppOptions().GetIncludeIncompleteSentencesIfLongerThanValue()),
    m_textExclusionMethod(project ?
        static_cast<int>(project->GetInvalidSentenceMethod()) :
        static_cast<int>(wxGetApp().GetAppOptions().GetInvalidSentenceMethod())),
    // title/font options
    m_xAxisFontColor(project ?
        project->GetXAxisFontColor() :
        wxGetApp().GetAppOptions().GetXAxisFontColor()),
    m_xAxisFont(project ?
        project->GetXAxisFont() :
        wxGetApp().GetAppOptions().GetXAxisFont()),
    m_yAxisFontColor(project ?
        project->GetYAxisFontColor() :
        wxGetApp().GetAppOptions().GetYAxisFontColor()),
    m_yAxisFont(project ? project->GetYAxisFont() :
        wxGetApp().GetAppOptions().GetYAxisFont()),
    m_topTitleFontColor(project ?
        project->GetTopTitleGraphFontColor() :
        wxGetApp().GetAppOptions().GetTopTitleGraphFontColor()),
    m_topTitleFont(project ?
        project->GetTopTitleGraphFont() :
        wxGetApp().GetAppOptions().GetTopTitleGraphFont()),
    m_bottomTitleFontColor(project ?
        project->GetBottomTitleGraphFontColor() :
        wxGetApp().GetAppOptions().GetBottomTitleGraphFontColor()),
    m_bottomTitleFont(project ?
        project->GetBottomTitleGraphFont() :
        wxGetApp().GetAppOptions().GetBottomTitleGraphFont()),
    m_leftTitleFontColor(project ?
        project->GetLeftTitleGraphFontColor() :
        wxGetApp().GetAppOptions().GetLeftTitleGraphFontColor()),
    m_leftTitleFont(project ?
        project->GetLeftTitleGraphFont() :
        wxGetApp().GetAppOptions().GetLeftTitleGraphFont()),
    m_rightTitleFontColor(project ?
        project->GetRightTitleGraphFontColor() :
        wxGetApp().GetAppOptions().GetRightTitleGraphFontColor()),
    m_rightTitleFont(project ?
        project->GetRightTitleGraphFont() :
        wxGetApp().GetAppOptions().GetRightTitleGraphFont())
    {
    wxString displayableProjectName = m_readabilityProjectDoc ?
        m_readabilityProjectDoc->GetTitle() : wxString{};
    if (displayableProjectName.length() >= 50)
        { displayableProjectName.Truncate(49).Append(static_cast<wchar_t>(8230)); }
    wxString title;
    if (IsGeneralSettings())
        { title = _(L"Options"); }
    else
        { title = wxString::Format(_(L"\"%s\" Properties"), displayableProjectName); }
    Create(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);
    }

//-------------------------------------------------------------
bool ToolsOptionsDlg::Create(wxWindow* parent, wxWindowID id, const wxString& caption,
                             const wxPoint& pos, const wxSize& size, long style)
    {
    SetExtraStyle(GetExtraStyle()|wxWS_EX_VALIDATE_RECURSIVELY);
    wxDialog::Create(parent, id, caption, pos, size, style);
    CreateControls();

    // DDX variables bound to controls
    TransferDataToWindow();

    // select the appropriate options if this is a project properties dialog
    if (GetSectionsBeingShown() == GraphsSection)
        { SelectPage(ToolsOptionsDlg::GRAPH_GENERAL_PAGE); }
    else if (GetSectionsBeingShown() == TextSection)
        { SelectPage(ToolsOptionsDlg::DOCUMENT_DISPLAY_GENERAL_PAGE); }
    else if (GetSectionsBeingShown() == ProjectSection)
        { SelectPage(ToolsOptionsDlg::PROJECT_SETTINGS_PAGE); }
    else if (GetSectionsBeingShown() == ScoresSection)
        { SelectPage(ToolsOptionsDlg::SCORES_TEST_OPTIONS_PAGE); }
    else if (GetSectionsBeingShown() == DocumentIndexing)
        { SelectPage(ToolsOptionsDlg::ANALYSIS_INDEXING_PAGE); }
    else if (GetSectionsBeingShown() == Grammar)
        { SelectPage(ToolsOptionsDlg::GRAMMAR_PAGE); }
    else if (GetSectionsBeingShown() == Statistics)
        { SelectPage(ToolsOptionsDlg::ANALYSIS_STATISTICS_PAGE); }
    else if (IsStandardProjectSettings())
        {
        const ProjectView* view = dynamic_cast<const ProjectView*>(m_readabilityProjectDoc->GetFirstView());
        const auto selectedID = view->GetSideBar()->GetSelectedFolderId();
        if (typeid(*view->GetActiveProjectWindow()) == typeid(Wisteria::Canvas))
            {
            const auto& graphType =
                typeid(*dynamic_cast<const Wisteria::Canvas*>(view->GetActiveProjectWindow())->GetFixedObject(0, 0));
            if (graphType == typeid(FleschChart) ||
                graphType == typeid(LixGauge) ||
                graphType == typeid(LixGaugeGerman) ||
                graphType == typeid(CrawfordGraph) ||
                graphType == typeid(DanielsonBryan2Plot) ||
                graphType == typeid(FraseGraph) ||
                graphType == typeid(FryGraph) ||
                graphType == typeid(RaygorGraph) ||
                graphType == typeid(SchwartzGraph) )
                { SelectPage(GRAPH_READABILITY_GRAPHS_PAGE); }
            else
                { SelectPage(GRAPH_GENERAL_PAGE); }
            }
        else if (!selectedID.has_value())
            { SelectPage(SCORES_DISPLAY_PAGE); }
        else if (selectedID == BaseProjectView::SIDEBAR_READABILITY_SCORES_SECTION_ID)
            { SelectPage(SCORES_DISPLAY_PAGE); }
        else if (selectedID == BaseProjectView::SIDEBAR_STATS_SUMMARY_SECTION_ID)
            { SelectPage(ANALYSIS_INDEXING_PAGE); }
        else if (selectedID == BaseProjectView::SIDEBAR_GRAMMAR_SECTION_ID)
            { SelectPage(GRAMMAR_PAGE); }
        else if (selectedID == BaseProjectView::SIDEBAR_DOLCH_SECTION_ID)
            { SelectPage(DOCUMENT_DISPLAY_DOLCH_PAGE); }
        else if (selectedID == BaseProjectView::SIDEBAR_WORDS_BREAKDOWN_SECTION_ID)
            { SelectPage(WORDS_BREAKDOWN_PAGE); }
        else if (selectedID == BaseProjectView::SIDEBAR_SENTENCES_BREAKDOWN_SECTION_ID)
            { SelectPage(SENTENCES_BREAKDOWN_PAGE); }
        else
            { SelectPage(ANALYSIS_INDEXING_PAGE); }
        }
    else if (IsBatchProjectSettings())
        {
        const BatchProjectView* view = dynamic_cast<const BatchProjectView*>(m_readabilityProjectDoc->GetFirstView());
        const auto selectedID = view->GetSideBar()->GetSelectedFolderId();
        if (typeid(*view->GetActiveProjectWindow()) == typeid(Wisteria::Canvas))
            {
            const auto& graphType =
                typeid(*dynamic_cast<const Wisteria::Canvas*>(view->GetActiveProjectWindow())->GetFixedObject(0, 0));
            if (graphType == typeid(FleschChart) ||
                graphType == typeid(LixGauge) ||
                graphType == typeid(LixGaugeGerman) ||
                graphType == typeid(CrawfordGraph) ||
                graphType == typeid(DanielsonBryan2Plot) ||
                graphType == typeid(FraseGraph) ||
                graphType == typeid(FryGraph) ||
                graphType == typeid(RaygorGraph) ||
                graphType == typeid(SchwartzGraph) )
                { SelectPage(GRAPH_READABILITY_GRAPHS_PAGE); }
            else
                { SelectPage(GRAPH_GENERAL_PAGE); }
            }
        else if (!selectedID.has_value())
            { SelectPage(SCORES_DISPLAY_PAGE); }
        else if (selectedID == BatchProjectView::SIDEBAR_READABILITY_SCORES_SECTION_ID)
            {
            if (m_projectLanguage == static_cast<int>(readability::test_language::english_test))
                { SelectPage(SCORES_TEST_OPTIONS_PAGE); }
            else
                { SelectPage(SCORES_DISPLAY_PAGE); }
            }
        else if (selectedID == BatchProjectView::SIDEBAR_HISTOGRAMS_SECTION_ID)
            { SelectPage(GRAPH_HISTOGRAM_PAGE); }
        else if (selectedID == BatchProjectView::SIDEBAR_BOXPLOTS_SECTION_ID)
            { SelectPage(GRAPH_BOX_PLOT_PAGE); }
        else if (selectedID == BatchProjectView::SIDEBAR_GRAMMAR_SECTION_ID)
            { SelectPage(GRAMMAR_PAGE); }
        else
            { SelectPage(ANALYSIS_INDEXING_PAGE); }
        }

    Center();

    return true;
    }

//-------------------------------------------------------------
bool ToolsOptionsDlg::HaveDocumentOptionsChanged() const
    {
    return m_longSentencesNumberOfWords.has_changed() ||
           m_longSentencesOutliers.has_changed() ||
           m_sentenceLength.has_changed() ||
           m_minDocWordCountForBatch.has_changed() ||
           m_syllabicationMethod.has_changed() ||
           m_paragraphParsingMethod.has_changed() ||
           m_ignoreBlankLinesForParagraphsParser.has_changed() ||
           m_ignoreIndentingForParagraphsParser.has_changed() ||
           m_sentenceStartMustBeUppercased.has_changed() ||
           m_aggressiveExclusion.has_changed() ||
           m_ignoreTrailingCopyrightNoticeParagraphs.has_changed() ||
           m_ignoreTrailingCitations.has_changed() ||
           m_ignoreFileAddresses.has_changed() ||
           m_ignoreNumerals.has_changed() ||
           m_ignoreProperNouns.has_changed() ||
           m_excludedPhrasesPath.has_changed() ||
           // simple boolean flag set if user had edited this list from this dialog
           m_excludedPhrasesEdited ||
           m_includeExcludedPhraseFirstOccurrence.has_changed() ||
           m_exclusionBlockTags.has_changed() ||
           m_textExclusionMethod.has_changed() ||
           m_includeIncompleteSentencesIfLongerThan.has_changed() ||
           m_appendedDocumentFilePath.has_changed() ||
           // test-specific options
           (IsPropertyAvailable(m_readabilityTestsPropertyGrid, GetDCTextExclusionLabel()) &&
               m_readabilityTestsPropertyGrid->IsPropertyModified(GetDCTextExclusionLabel())) ||
           (IsPropertyAvailable(m_readabilityTestsPropertyGrid, GetProperNounsLabel()) &&
               m_readabilityTestsPropertyGrid->IsPropertyModified(GetProperNounsLabel())) ||
           (IsPropertyAvailable(m_readabilityTestsPropertyGrid, GetIncludeStockerLabel()) &&
               m_readabilityTestsPropertyGrid->IsPropertyModified(GetIncludeStockerLabel())) ||
           (IsPropertyAvailable(m_readabilityTestsPropertyGrid, GetHJCTextExclusionLabel()) &&
               m_readabilityTestsPropertyGrid->IsPropertyModified(GetHJCTextExclusionLabel())) ||
           (IsPropertyAvailable(m_readabilityTestsPropertyGrid, GetCountIndependentClausesLabel()) &&
               m_readabilityTestsPropertyGrid->IsPropertyModified(GetCountIndependentClausesLabel())) ||
           (IsPropertyAvailable(m_readabilityTestsPropertyGrid, GetFleschNumeralSyllabicationLabel()) &&
               m_readabilityTestsPropertyGrid->IsPropertyModified(GetFleschNumeralSyllabicationLabel())) ||
           (IsPropertyAvailable(m_readabilityTestsPropertyGrid, GetFleschKincaidNumeralSyllabicationLabel()) &&
               m_readabilityTestsPropertyGrid->IsPropertyModified(GetFleschKincaidNumeralSyllabicationLabel())) ||
           m_projectLanguage.has_changed() ||
           m_documentStorageMethod.has_changed() ||
           m_filePath.has_changed() ||
           // grammar
           (IsPropertyAvailable(m_grammarPropertyGrid, GetIgnoreProperNounsLabel()) &&
               m_grammarPropertyGrid->IsPropertyModified(GetIgnoreProperNounsLabel())) ||
           (IsPropertyAvailable(m_grammarPropertyGrid, GetIgnoreUppercasedWordsLabel()) &&
               m_grammarPropertyGrid->IsPropertyModified(GetIgnoreUppercasedWordsLabel())) ||
           (IsPropertyAvailable(m_grammarPropertyGrid, GetIgnoreNumeralsLabel()) &&
               m_grammarPropertyGrid->IsPropertyModified(GetIgnoreNumeralsLabel())) ||
           (IsPropertyAvailable(m_grammarPropertyGrid, GetIgnoreFileAddressesLabel()) &&
               m_grammarPropertyGrid->IsPropertyModified(GetIgnoreFileAddressesLabel())) ||
           (IsPropertyAvailable(m_grammarPropertyGrid, GetIgnoreProgrammerCodeLabel()) &&
               m_grammarPropertyGrid->IsPropertyModified(GetIgnoreProgrammerCodeLabel())) ||
           (IsPropertyAvailable(m_grammarPropertyGrid, GetAllowColloquialismsLabel()) &&
               m_grammarPropertyGrid->IsPropertyModified(GetAllowColloquialismsLabel())) ||
           (IsPropertyAvailable(m_grammarPropertyGrid, GetIgnoreSocialMediaLabel()) &&
               m_grammarPropertyGrid->IsPropertyModified(GetIgnoreSocialMediaLabel())) ||
           (IsPropertyAvailable(m_grammarPropertyGrid, GetGrammarHighlightedReportLabel()) &&
               m_grammarPropertyGrid->IsPropertyModified(GetGrammarHighlightedReportLabel())) ||
           (IsPropertyAvailable(m_grammarPropertyGrid, BaseProjectView::GetMisspellingsLabel()) &&
               m_grammarPropertyGrid->IsPropertyModified(BaseProjectView::GetMisspellingsLabel())) ||
           (IsPropertyAvailable(m_grammarPropertyGrid, BaseProjectView::GetRepeatedWordsLabel()) &&
               m_grammarPropertyGrid->IsPropertyModified(BaseProjectView::GetRepeatedWordsLabel())) ||
           (IsPropertyAvailable(m_grammarPropertyGrid, BaseProjectView::GetArticleMismatchesLabel()) &&
               m_grammarPropertyGrid->IsPropertyModified(BaseProjectView::GetArticleMismatchesLabel())) ||
           (IsPropertyAvailable(m_grammarPropertyGrid, BaseProjectView::GetPhrasingErrorsTabLabel()) &&
               m_grammarPropertyGrid->IsPropertyModified(BaseProjectView::GetPhrasingErrorsTabLabel())) ||
           (IsPropertyAvailable(m_grammarPropertyGrid, BaseProjectView::GetRedundantPhrasesTabLabel()) &&
               m_grammarPropertyGrid->IsPropertyModified(BaseProjectView::GetRedundantPhrasesTabLabel())) ||
           (IsPropertyAvailable(m_grammarPropertyGrid, BaseProjectView::GetOverusedWordsBySentenceLabel()) &&
               m_grammarPropertyGrid->IsPropertyModified(BaseProjectView::GetOverusedWordsBySentenceLabel())) ||
           (IsPropertyAvailable(m_grammarPropertyGrid, BaseProjectView::GetWordyPhrasesTabLabel()) &&
               m_grammarPropertyGrid->IsPropertyModified(BaseProjectView::GetWordyPhrasesTabLabel())) ||
           (IsPropertyAvailable(m_grammarPropertyGrid, BaseProjectView::GetClichesTabLabel()) &&
               m_grammarPropertyGrid->IsPropertyModified(BaseProjectView::GetClichesTabLabel())) ||
           (IsPropertyAvailable(m_grammarPropertyGrid, BaseProjectView::GetPassiveLabel()) &&
               m_grammarPropertyGrid->IsPropertyModified(BaseProjectView::GetPassiveLabel())) ||
           (IsPropertyAvailable(m_grammarPropertyGrid,
               BaseProjectView::GetSentenceStartingWithConjunctionsTabLabel()) &&
               m_grammarPropertyGrid->IsPropertyModified(
                   BaseProjectView::GetSentenceStartingWithConjunctionsTabLabel())) ||
           (IsPropertyAvailable(m_grammarPropertyGrid,
               BaseProjectView::GetSentenceStartingWithLowercaseTabLabel()) &&
               m_grammarPropertyGrid->IsPropertyModified(
                   BaseProjectView::GetSentenceStartingWithLowercaseTabLabel())) ||
           (m_fileList && m_fileList->HasItemBeenEditedByUser());
    }

//-------------------------------------------------------------
bool ToolsOptionsDlg::HaveStatisticsOptionsChanged() const
    {
    return (IsPropertyAvailable(m_statisticsPropertyGrid, GetParagraphsLabel()) &&
        m_statisticsPropertyGrid->IsPropertyModified(GetParagraphsLabel())) ||
        (IsPropertyAvailable(m_statisticsPropertyGrid, GetSentencesLabel()) &&
            m_statisticsPropertyGrid->IsPropertyModified(GetSentencesLabel())) ||
        (IsPropertyAvailable(m_statisticsPropertyGrid, GetWordsLabel()) &&
            m_statisticsPropertyGrid->IsPropertyModified(GetWordsLabel())) ||
        (IsPropertyAvailable(m_statisticsPropertyGrid, GetExtendedWordsLabel()) &&
            m_statisticsPropertyGrid->IsPropertyModified(GetExtendedWordsLabel())) ||
        (IsPropertyAvailable(m_statisticsPropertyGrid, GetGrammarLabel()) &&
            m_statisticsPropertyGrid->IsPropertyModified(GetGrammarLabel())) ||
        (IsPropertyAvailable(m_statisticsPropertyGrid, GetNotesLabel()) &&
            m_statisticsPropertyGrid->IsPropertyModified(GetNotesLabel())) ||
        (IsPropertyAvailable(m_statisticsPropertyGrid, GetExtendedInformationLabel()) &&
            m_statisticsPropertyGrid->IsPropertyModified(GetExtendedInformationLabel())) ||
        (IsPropertyAvailable(m_statisticsPropertyGrid, GetCoverageLabel()) &&
            m_statisticsPropertyGrid->IsPropertyModified(GetCoverageLabel())) ||
        (IsPropertyAvailable(m_statisticsPropertyGrid, GetDolchWordsLabel()) &&
            m_statisticsPropertyGrid->IsPropertyModified(GetDolchWordsLabel())) ||
        (IsPropertyAvailable(m_statisticsPropertyGrid, GetDolchExplanationLabel()) &&
            m_statisticsPropertyGrid->IsPropertyModified(GetDolchExplanationLabel())) ||
        (IsPropertyAvailable(m_statisticsPropertyGrid, BaseProjectView::GetFormattedReportLabel()) &&
            m_statisticsPropertyGrid->IsPropertyModified(BaseProjectView::GetFormattedReportLabel())) ||
        (IsPropertyAvailable(m_statisticsPropertyGrid, BaseProjectView::GetTabularReportLabel()) &&
            m_statisticsPropertyGrid->IsPropertyModified(BaseProjectView::GetTabularReportLabel()));
    }

//-------------------------------------------------------------
bool ToolsOptionsDlg::HaveGraphOptionsChanged() const
    {
    return (IsPropertyAvailable(m_generalGraphPropertyGrid,GetDisplayDropShadowsLabel()) &&
        m_generalGraphPropertyGrid->IsPropertyModified(GetDisplayDropShadowsLabel())) ||
           // box plot options
           (IsPropertyAvailable(m_boxPlotsPropertyGrid,GetOpacityLabel()) &&
               m_boxPlotsPropertyGrid->IsPropertyModified(GetOpacityLabel())) ||
           (IsPropertyAvailable(m_boxPlotsPropertyGrid,GetLabelsOnBoxesLabel()) &&
               m_boxPlotsPropertyGrid->IsPropertyModified(GetLabelsOnBoxesLabel())) ||
           (IsPropertyAvailable(m_boxPlotsPropertyGrid,GetShowAllDataPointsLabel()) &&
               m_boxPlotsPropertyGrid->IsPropertyModified(GetShowAllDataPointsLabel())) ||
           (IsPropertyAvailable(m_boxPlotsPropertyGrid,GetConnectBoxesLabel()) &&
               m_boxPlotsPropertyGrid->IsPropertyModified(GetConnectBoxesLabel())) ||
           (IsPropertyAvailable(m_boxPlotsPropertyGrid,GetColorLabel()) &&
               m_boxPlotsPropertyGrid->IsPropertyModified(GetColorLabel())) ||
           (IsPropertyAvailable(m_boxPlotsPropertyGrid,GetEffectLabel()) &&
               m_boxPlotsPropertyGrid->IsPropertyModified(GetEffectLabel())) ||
           // general
           (IsPropertyAvailable(m_generalGraphPropertyGrid,GetWatermarkLabel()) &&
               m_generalGraphPropertyGrid->IsPropertyModified(GetWatermarkLabel())) ||
           (IsPropertyAvailable(m_generalGraphPropertyGrid,GetLogoImageLabel()) &&
               m_generalGraphPropertyGrid->IsPropertyModified(GetLogoImageLabel())) ||
           (IsPropertyAvailable(m_generalGraphPropertyGrid, GetBackgroundColorFadeLabel()) &&
               m_generalGraphPropertyGrid->IsPropertyModified(GetBackgroundColorFadeLabel())) ||
           (IsPropertyAvailable(m_generalGraphPropertyGrid,GetCustomImageBrushLabel()) &&
               m_generalGraphPropertyGrid->IsPropertyModified(GetCustomImageBrushLabel())) ||
           (IsPropertyAvailable(m_generalGraphPropertyGrid,GetBackgroundColorLabel()) &&
               m_generalGraphPropertyGrid->IsPropertyModified(GetBackgroundColorLabel())) ||
           (IsPropertyAvailable(m_generalGraphPropertyGrid,GetImageOpacityLabel()) &&
               m_generalGraphPropertyGrid->IsPropertyModified(GetImageOpacityLabel())) ||
           (IsPropertyAvailable(m_generalGraphPropertyGrid,GetColorLabel()) &&
               m_generalGraphPropertyGrid->IsPropertyModified(GetColorLabel())) ||
           (IsPropertyAvailable(m_generalGraphPropertyGrid,GetOpacityLabel()) &&
               m_generalGraphPropertyGrid->IsPropertyModified(GetOpacityLabel())) ||
           (IsPropertyAvailable(m_generalGraphPropertyGrid, GetImageLabel()) &&
               m_generalGraphPropertyGrid->IsPropertyModified(GetImageLabel())) ||
           (IsPropertyAvailable(m_generalGraphPropertyGrid, GetImageEffectLabel()) &&
               m_generalGraphPropertyGrid->IsPropertyModified(GetImageEffectLabel())) ||
           // barchart options
           (IsPropertyAvailable(m_barChartPropertyGrid,GetOpacityLabel()) &&
               m_barChartPropertyGrid->IsPropertyModified(GetOpacityLabel())) ||
           (IsPropertyAvailable(m_barChartPropertyGrid,GetColorLabel()) &&
               m_barChartPropertyGrid->IsPropertyModified(GetColorLabel())) ||
           (IsPropertyAvailable(m_barChartPropertyGrid,GeOrientationLabel()) &&
               m_barChartPropertyGrid->IsPropertyModified(GeOrientationLabel())) ||
           (IsPropertyAvailable(m_barChartPropertyGrid,GetEffectLabel()) &&
               m_barChartPropertyGrid->IsPropertyModified(GetEffectLabel())) ||
           (IsPropertyAvailable(m_barChartPropertyGrid,GetLabelsOnBarsLabel()) &&
               m_barChartPropertyGrid->IsPropertyModified(GetLabelsOnBarsLabel())) ||
           // histogram options
           (IsPropertyAvailable(m_histogramPropertyGrid,GetBinSortingLabel()) &&
               m_histogramPropertyGrid->IsPropertyModified(GetBinSortingLabel())) ||
           (IsPropertyAvailable(m_histogramPropertyGrid,GetGradeLevelRoundingLabel()) &&
               m_histogramPropertyGrid->IsPropertyModified(GetGradeLevelRoundingLabel())) ||
           (IsPropertyAvailable(m_histogramPropertyGrid,GetIntervalDisplayLabel()) &&
               m_histogramPropertyGrid->IsPropertyModified(GetIntervalDisplayLabel())) ||
           (IsPropertyAvailable(m_histogramPropertyGrid,GetBinLabelsLabel()) &&
               m_histogramPropertyGrid->IsPropertyModified(GetBinLabelsLabel())) ||
           (IsPropertyAvailable(m_histogramPropertyGrid,GetColorLabel()) &&
               m_histogramPropertyGrid->IsPropertyModified(GetColorLabel())) ||
           (IsPropertyAvailable(m_histogramPropertyGrid,GetOpacityLabel()) &&
               m_histogramPropertyGrid->IsPropertyModified(GetOpacityLabel())) ||
           (IsPropertyAvailable(m_histogramPropertyGrid,GetEffectLabel()) &&
               m_histogramPropertyGrid->IsPropertyModified(GetEffectLabel())) ||
           // title/font options
           m_xAxisFontColor.has_changed() ||
           m_xAxisFont.has_changed() ||
           m_yAxisFontColor.has_changed() ||
           m_yAxisFont.has_changed() ||
           m_topTitleFontColor.has_changed() ||
           m_topTitleFont.has_changed() ||
           m_bottomTitleFontColor.has_changed() ||
           m_bottomTitleFont.has_changed() ||
           m_leftTitleFontColor.has_changed() ||
           m_leftTitleFont.has_changed() ||
           m_rightTitleFontColor.has_changed() ||
           m_rightTitleFont.has_changed() ||
           (IsPropertyAvailable(m_readabilityGraphPropertyGrid,GetInvalidRegionsColorLabel()) &&
               m_readabilityGraphPropertyGrid->IsPropertyModified(GetInvalidRegionsColorLabel())) ||
           (IsPropertyAvailable(m_readabilityGraphPropertyGrid, GetRaygorStyleLabel()) &&
               m_readabilityGraphPropertyGrid->IsPropertyModified(GetRaygorStyleLabel())) ||
           (IsPropertyAvailable(m_readabilityGraphPropertyGrid,GetUseEnglishLabelsForGermanLixLabel()) &&
               m_readabilityGraphPropertyGrid->IsPropertyModified(GetUseEnglishLabelsForGermanLixLabel())) ||
           (IsPropertyAvailable(m_readabilityGraphPropertyGrid,GetFleschChartConnectPointsLabel()) &&
               m_readabilityGraphPropertyGrid->IsPropertyModified(GetFleschChartConnectPointsLabel())) ||
           (IsPropertyAvailable(m_readabilityGraphPropertyGrid,GetFleschSyllableRulerDocGroupsLabel()) &&
               m_readabilityGraphPropertyGrid->IsPropertyModified(GetFleschSyllableRulerDocGroupsLabel()));
    }

//-------------------------------------------------------------
bool ToolsOptionsDlg::HaveTextViewOptionsChanged() const noexcept
    {
    return m_textHighlightMethod.has_changed() ||
           m_highlightedColor.has_changed() ||
           m_excludedTextHighlightColor.has_changed() ||
           m_duplicateWordHighlightColor.has_changed() ||
           m_wordyPhraseHighlightColor.has_changed() ||
           m_font.has_changed() ||
           m_fontColor.has_changed() ||
           m_dolchConjunctionsColor.has_changed() ||
           m_dolchPrepositionsColor.has_changed() ||
           m_dolchPronounsColor.has_changed() ||
           m_dolchAdverbsColor.has_changed() ||
           m_dolchAdjectivesColor.has_changed() ||
           m_dolchVerbsColor.has_changed() ||
           m_dolchNounColor.has_changed() ||
           m_highlightDolchConjunctions.has_changed() ||
           m_highlightDolchPrepositions.has_changed() ||
           m_highlightDolchPronouns.has_changed() ||
           m_highlightDolchAdverbs.has_changed() ||
           m_highlightDolchAdjectives.has_changed() ||
           m_highlightDolchVerbs.has_changed() ||
           m_highlightDolchNouns.has_changed();
    }

//-------------------------------------------------------------
bool ToolsOptionsDlg::HaveTestDisplayOptionsChanged() const
    {
    return (IsPropertyAvailable(m_gradeLevelPropertyGrid,GetCalculationLabel()) &&
        m_gradeLevelPropertyGrid->IsPropertyModified(GetCalculationLabel())) ||
           (IsPropertyAvailable(m_gradeLevelPropertyGrid,GetGradeScaleLabel()) &&
               m_gradeLevelPropertyGrid->IsPropertyModified(GetGradeScaleLabel())) ||
           (IsPropertyAvailable(m_gradeLevelPropertyGrid,GetGradesLongFormatLabel()) &&
               m_gradeLevelPropertyGrid->IsPropertyModified(GetGradesLongFormatLabel())) ||
           (IsPropertyAvailable(m_gradeLevelPropertyGrid,GetIncludeScoreSummaryLabel()) &&
               m_gradeLevelPropertyGrid->IsPropertyModified(GetIncludeScoreSummaryLabel()));
    }

//-------------------------------------------------------------
bool ToolsOptionsDlg::HaveWordsBreakdownOptionsChanged() const
    {
    return (IsPropertyAvailable(m_wordsBreakdownPropertyGrid,
            BaseProjectView::GetWordCountsLabel()) && m_wordsBreakdownPropertyGrid->IsPropertyModified(
            BaseProjectView::GetWordCountsLabel())) ||
           (IsPropertyAvailable(m_wordsBreakdownPropertyGrid,
               BaseProjectView::GetSyllableCountsLabel()) &&
               m_wordsBreakdownPropertyGrid->IsPropertyModified(BaseProjectView::GetSyllableCountsLabel())) ||
           (IsPropertyAvailable(m_wordsBreakdownPropertyGrid,
               BaseProjectView::GetThreeSyllableWordsLabel()) &&
               m_wordsBreakdownPropertyGrid->IsPropertyModified(BaseProjectView::GetThreeSyllableWordsLabel())) ||
           (IsPropertyAvailable(m_wordsBreakdownPropertyGrid,BaseProjectView::GetSixCharWordsLabel()) &&
               m_wordsBreakdownPropertyGrid->IsPropertyModified(BaseProjectView::GetSixCharWordsLabel())) ||
           (IsPropertyAvailable(m_wordsBreakdownPropertyGrid,BaseProjectView::GetWordCloudLabel()) &&
               m_wordsBreakdownPropertyGrid->IsPropertyModified(BaseProjectView::GetWordCloudLabel())) ||
           (IsPropertyAvailable(m_wordsBreakdownPropertyGrid,BaseProjectView::GetDaleChallLabel()) &&
               m_wordsBreakdownPropertyGrid->IsPropertyModified(BaseProjectView::GetDaleChallLabel())) ||
           (IsPropertyAvailable(m_wordsBreakdownPropertyGrid,BaseProjectView::GetSpacheLabel()) &&
               m_wordsBreakdownPropertyGrid->IsPropertyModified(BaseProjectView::GetSpacheLabel())) ||
           (IsPropertyAvailable(m_wordsBreakdownPropertyGrid,BaseProjectView::GetHarrisJacobsonLabel()) &&
               m_wordsBreakdownPropertyGrid->IsPropertyModified(BaseProjectView::GetHarrisJacobsonLabel())) ||
           (IsPropertyAvailable(m_wordsBreakdownPropertyGrid,GetCustomTestsLabel()) &&
               m_wordsBreakdownPropertyGrid->IsPropertyModified(GetCustomTestsLabel())) ||
           (IsPropertyAvailable(m_wordsBreakdownPropertyGrid,BaseProjectView::GetAllWordsLabel()) &&
               m_wordsBreakdownPropertyGrid->IsPropertyModified(BaseProjectView::GetAllWordsLabel())) ||
           (IsPropertyAvailable(m_wordsBreakdownPropertyGrid,BaseProjectView::GetKeyWordsLabel()) &&
               m_wordsBreakdownPropertyGrid->IsPropertyModified(BaseProjectView::GetKeyWordsLabel()));
    }

//-------------------------------------------------------------
bool ToolsOptionsDlg::HaveSentencesBreakdownOptionsChanged() const
    {
    return (IsPropertyAvailable(m_sentencesBreakdownPropertyGrid,
            BaseProjectView::GetLongSentencesLabel()) &&
            m_sentencesBreakdownPropertyGrid->IsPropertyModified(
                BaseProjectView::GetLongSentencesLabel())) ||
           (IsPropertyAvailable(m_sentencesBreakdownPropertyGrid,
               BaseProjectView::GetSentenceLengthBoxPlotLabel()) &&
               m_sentencesBreakdownPropertyGrid->IsPropertyModified(
                   BaseProjectView::GetSentenceLengthBoxPlotLabel())) ||
           (IsPropertyAvailable(m_sentencesBreakdownPropertyGrid,
               BaseProjectView::GetSentenceLengthHistogramLabel()) &&
               m_sentencesBreakdownPropertyGrid->IsPropertyModified(
                   BaseProjectView::GetSentenceLengthHistogramLabel())) ||
           (IsPropertyAvailable(m_sentencesBreakdownPropertyGrid,
               BaseProjectView::GetSentenceLengthHeatmapLabel()) &&
               m_sentencesBreakdownPropertyGrid->IsPropertyModified(
                   BaseProjectView::GetSentenceLengthHeatmapLabel()));
    }

//-------------------------------------------------------------
bool ToolsOptionsDlg::HaveOptionsChanged() const
    {
    return // text formatting options
           HaveTextViewOptionsChanged() ||
           HaveTestDisplayOptionsChanged() ||
           HaveWordsBreakdownOptionsChanged() ||
           HaveSentencesBreakdownOptionsChanged() ||
           // file list display
           m_filePathTruncationMode.has_changed() ||
           m_reviewer.has_changed() ||
           m_status.has_changed() ||
           m_description.has_changed() ||
           HaveDocumentOptionsChanged() ||
           HaveStatisticsOptionsChanged() ||
           HaveGraphOptionsChanged();
    }

//-------------------------------------------------------------
bool ToolsOptionsDlg::ValidateOptions()
    {
    Validate();
    TransferDataFromWindow();
    // trim any spaces off of the file paths
    m_filePath = ExpandPath(m_filePath.get_value().Trim(true).Trim(false));
    m_excludedPhrasesPath = ExpandPath(m_excludedPhrasesPath.get_value().Trim(true).Trim(false));
    m_appendedDocumentFilePath = ExpandPath(m_appendedDocumentFilePath.get_value().Trim(true).Trim(false));
    TransferDataToWindow();
    if (IsBatchProjectSettings() && m_fileList && m_fileList->GetItemCount() == 0)
        {
        wxMessageBox(_(L"Project must contain at least one document."),
                wxGetApp().GetAppName(), wxOK|wxICON_EXCLAMATION);
        SelectPage(PROJECT_SETTINGS_PAGE);
        return false;
        }
    // if linking to external file then make sure it is valid and set flag to reload it
    if (m_filePath.has_changed() ||
        (m_documentStorageMethod.has_changed() &&
        m_documentStorageMethod == static_cast<int>(TextStorage::NoEmbedText) &&
        IsStandardProjectSettings()))
        {
        FilePathResolver resolvePath(m_filePath.get_value(), true);
        if (resolvePath.IsInvalidFile() ||
            (resolvePath.IsLocalOrNetworkFile() && !wxFile::Exists(m_filePath.get_value())) )
            {
            wxMessageBox(wxString::Format(_(L"\"%s\": file not found."), m_filePath.get_value() ),
                 wxGetApp().GetAppName(), wxOK|wxICON_EXCLAMATION);
            SelectPage(PROJECT_SETTINGS_PAGE);
            return false;
            }
        }
    if (m_appendedDocumentFilePath.has_changed() && m_appendedDocumentFilePath.get_value().length())
        {
        if (!wxFile::Exists(m_appendedDocumentFilePath.get_value()) )
            {
            wxMessageBox(wxString::Format(_(L"\"%s\": file not found."), m_appendedDocumentFilePath.get_value() ),
                 wxGetApp().GetAppName(), wxOK|wxICON_EXCLAMATION);
            SelectPage(PROJECT_SETTINGS_PAGE);
            return false;
            }
        }
    if (((IsPropertyAvailable(m_generalGraphPropertyGrid,GetCustomImageBrushLabel()) &&
        m_generalGraphPropertyGrid->GetPropertyValueAsString(GetCustomImageBrushLabel()).length()) ||
            (IsPropertyAvailable(m_barChartPropertyGrid,GetEffectLabel()) &&
                static_cast<BoxEffect>(m_barChartPropertyGrid->GetPropertyValueAsInt(GetEffectLabel())) ==
                    BoxEffect::Stipple) ||
            (IsPropertyAvailable(m_histogramPropertyGrid,GetEffectLabel()) &&
                static_cast<BoxEffect>(m_histogramPropertyGrid->GetPropertyValueAsInt(GetEffectLabel())) ==
                    BoxEffect::Stipple) ||
            (IsPropertyAvailable(m_boxPlotsPropertyGrid,GetEffectLabel()) &&
                static_cast<BoxEffect>(m_boxPlotsPropertyGrid->GetPropertyValueAsInt(GetEffectLabel())) ==
                    BoxEffect::Stipple)) &&
        !wxFile::Exists(m_generalGraphPropertyGrid->GetPropertyValueAsString(GetCustomImageBrushLabel())))
        {
        wxMessageBox(wxString::Format(
            _(L"\"%s\": stipple image file not found."),
                m_generalGraphPropertyGrid->GetPropertyValueAsString(GetCustomImageBrushLabel()) ),
            wxGetApp().GetAppName(), wxOK|wxICON_EXCLAMATION);
        wxFileDialog fd
            (this, _(L"Select Stipple Image"),
            wxGetApp().GetAppOptions().GetImagePath(), wxEmptyString,
            wxGetApp().GetAppOptions().IMAGE_LOAD_FILE_FILTER,
            wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_PREVIEW);
        if (fd.ShowModal() != wxID_OK)
            { return false; }
        wxGetApp().GetAppOptions().SetImagePath(wxFileName(fd.GetPath()).GetPath());
        m_generalGraphPropertyGrid->SetPropertyValue(GetCustomImageBrushLabel(), fd.GetPath());
        }
    if (IsPropertyAvailable(m_generalGraphPropertyGrid,GetImageLabel()) &&
        !m_generalGraphPropertyGrid->GetPropertyValueAsString(GetImageLabel()).empty() &&
        !wxFile::Exists(m_generalGraphPropertyGrid->GetPropertyValueAsString(GetImageLabel())) )
        {
        wxMessageBox(wxString::Format(
            _(L"\"%s\": graph background image file not found."),
                m_generalGraphPropertyGrid->GetPropertyValueAsString(GetImageLabel()) ),
            wxGetApp().GetAppName(), wxOK|wxICON_EXCLAMATION);
        wxFileDialog fd
            (this, _(L"Select Background Image"),
            wxGetApp().GetAppOptions().GetImagePath(), wxEmptyString,
            wxGetApp().GetAppOptions().IMAGE_LOAD_FILE_FILTER,
            wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_PREVIEW);
        if (fd.ShowModal() != wxID_OK)
            { return false; }
        wxGetApp().GetAppOptions().SetImagePath(wxFileName(fd.GetPath()).GetPath());
        m_generalGraphPropertyGrid->SetPropertyValue(GetImageLabel(), fd.GetPath());
        }
    if ((IsPropertyAvailable(m_generalGraphPropertyGrid,GetLogoImageLabel()) &&
        m_generalGraphPropertyGrid->GetPropertyValueAsString(GetLogoImageLabel()).length()) &&
        !wxFile::Exists(m_generalGraphPropertyGrid->GetPropertyValueAsString(GetLogoImageLabel())) )
        {
        wxMessageBox(wxString::Format(
            _(L"\"%s\": graph logo image file not found."),
                m_generalGraphPropertyGrid->GetPropertyValueAsString(GetLogoImageLabel()) ),
            wxGetApp().GetAppName(), wxOK|wxICON_EXCLAMATION);
        wxFileDialog fd
            (this, _(L"Select Logo Image"),
            wxGetApp().GetAppOptions().GetImagePath(), wxEmptyString,
            wxGetApp().GetAppOptions().IMAGE_LOAD_FILE_FILTER,
            wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_PREVIEW);
        if (fd.ShowModal() != wxID_OK)
            { return false; }
        wxGetApp().GetAppOptions().SetImagePath(wxFileName(fd.GetPath()).GetPath());
        m_generalGraphPropertyGrid->SetPropertyValue(GetLogoImageLabel(), fd.GetPath());
        }
    if (!IsStandardProjectSettings() &&
        IsPropertyAvailable(m_histogramPropertyGrid,GetBinSortingLabel()) &&
        IsPropertyAvailable(m_histogramPropertyGrid,GetIntervalDisplayLabel()) &&
        m_histogramPropertyGrid->GetPropertyValueAsInt(GetBinSortingLabel()) == 0 &&
        m_histogramPropertyGrid->GetPropertyValueAsInt(GetIntervalDisplayLabel()) != 1)
        {
        auto warningIter =
            wxGetApp().GetAppOptions().GetWarning(_DT(L"histogram-unique-values-midpoints-required"));
        if (warningIter != wxGetApp().GetAppOptions().GetWarnings().end() &&
            warningIter->ShouldBeShown())
            { wxMessageBox(warningIter->GetMessage(), wxGetApp().GetAppName(), warningIter->GetFlags(), this); }
        m_histogramPropertyGrid->SetPropertyValue(GetIntervalDisplayLabel(), 1);
        }
    if (m_excludedPhrasesPath.get_value().length() &&
        !wxFile::Exists(m_excludedPhrasesPath.get_value()) )
        {
        wxMessageBox(wxString::Format(
            _(L"\"%s\": excluded phrase file not found."), m_excludedPhrasesPath.get_value() ),
            wxGetApp().GetAppName(), wxOK|wxICON_EXCLAMATION);
        SelectPage(ANALYSIS_INDEXING_PAGE);
        return false;
        }
    return true;
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::SaveOptions()
    {
    if (GetSectionsBeingShown() == GraphsSection)
        {
        SaveProjectGraphOptions();
        return;
        }
    else if (GetSectionsBeingShown() == TextSection)
        {
        SaveTextWindowOptions();
        return;
        }
    else if (GetSectionsBeingShown() == Statistics)
        {
        SaveStatisticsOptions();
        return;
        }
    if (m_userAgent.has_changed())
        {
        wxGetApp().GetAppOptions().SetUserAgent(m_userAgent.get_value());
        wxGetApp().GetWebHarvester().SetUserAgent(m_userAgent.get_value());
        }
    if (m_readabilityProjectDoc && HaveOptionsChanged())
        {
        if (IsBatchProjectSettings() && HaveDocumentOptionsChanged())
            {
            // batch projects may need to do a full re-indexing, so just set this flag as a shortcut
            m_readabilityProjectDoc->RefreshRequired(ProjectRefresh::FullReindexing);
            // also update the filepaths if they were changed and we aren't embedding the documents
            if (m_fileList && m_fileList->HasItemBeenEditedByUser() &&
                m_documentStorageMethod == static_cast<int>(TextStorage::NoEmbedText))
                {
                m_readabilityProjectDoc->GetSourceFilesInfo().resize(m_fileList->GetItemCount());
                for (long i = 0; i < m_fileList->GetItemCount(); ++i)
                    {
                    m_readabilityProjectDoc->GetSourceFilesInfo()[i] =
                        comparable_first_pair(m_fileList->GetItemTextEx(i,0), m_fileList->GetItemTextEx(i, 1));
                    }
                }
            }
        else if (IsStandardProjectSettings() && HaveDocumentOptionsChanged())
            { m_readabilityProjectDoc->RefreshRequired(ProjectRefresh::FullReindexing); }

        if (IsPropertyAvailable(m_gradeLevelPropertyGrid,GetCalculationLabel()))
            {
            m_readabilityProjectDoc->GetReadabilityMessageCatalog().SetReadingAgeDisplay(
                static_cast<ReadabilityMessages::ReadingAgeDisplay>(
                    m_gradeLevelPropertyGrid->GetPropertyValueAsInt(GetCalculationLabel())));
            }
        if (IsPropertyAvailable(m_gradeLevelPropertyGrid,GetGradeScaleLabel()))
            {
            m_readabilityProjectDoc->GetReadabilityMessageCatalog().SetGradeScale(
                static_cast<readability::grade_scale>(m_gradeLevelPropertyGrid->GetPropertyValueAsInt(
                    GetGradeScaleLabel())));
            }
        if (IsPropertyAvailable(m_gradeLevelPropertyGrid,GetGradesLongFormatLabel()))
            {
            m_readabilityProjectDoc->GetReadabilityMessageCatalog().SetLongGradeScaleFormat(
                m_gradeLevelPropertyGrid->GetPropertyValueAsBool(GetGradesLongFormatLabel()));
            }
        if (IsPropertyAvailable(m_gradeLevelPropertyGrid,GetIncludeScoreSummaryLabel()))
            {
            m_readabilityProjectDoc->IncludeScoreSummaryReport(
                m_gradeLevelPropertyGrid->GetPropertyValueAsBool(GetIncludeScoreSummaryLabel()));
            }
        m_readabilityProjectDoc->SetLongSentenceMethod(
            m_longSentencesNumberOfWords ? LongSentence::LongerThanSpecifiedLength : LongSentence::OutlierLength);
        m_readabilityProjectDoc->SetDifficultSentenceLength(m_sentenceLength);
        m_readabilityProjectDoc->SetMinDocWordCountForBatch(m_minDocWordCountForBatch);
        if (IsPropertyAvailable(m_sentencesBreakdownPropertyGrid,BaseProjectView::GetLongSentencesLabel()))
            {
            m_readabilityProjectDoc->GetSentencesBreakdownInfo().EnableLongSentences(
                m_sentencesBreakdownPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetLongSentencesLabel()));
            }
        if (IsPropertyAvailable(m_sentencesBreakdownPropertyGrid,BaseProjectView::GetSentenceLengthBoxPlotLabel()))
            {
            m_readabilityProjectDoc->GetSentencesBreakdownInfo().EnableLengthsBoxPlot(
                m_sentencesBreakdownPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetSentenceLengthBoxPlotLabel()));
            }
        if (IsPropertyAvailable(m_sentencesBreakdownPropertyGrid,BaseProjectView::GetSentenceLengthHeatmapLabel()))
            {
            m_readabilityProjectDoc->GetSentencesBreakdownInfo().EnableLengthsHeatmap(
                m_sentencesBreakdownPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetSentenceLengthHeatmapLabel()));
            }
        if (IsPropertyAvailable(m_sentencesBreakdownPropertyGrid,BaseProjectView::GetSentenceLengthHistogramLabel()))
            {
            m_readabilityProjectDoc->GetSentencesBreakdownInfo().EnableLengthsHistogram(
                m_sentencesBreakdownPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetSentenceLengthHistogramLabel()));
            }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid,BaseProjectView::GetWordCountsLabel()))
            {
            m_readabilityProjectDoc->GetWordsBreakdownInfo().EnableWordBarchart(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetWordCountsLabel()));
            }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid, BaseProjectView::GetSyllableCountsLabel()))
            {
            m_readabilityProjectDoc->GetWordsBreakdownInfo().EnableSyllableHistogram(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetSyllableCountsLabel()));
            }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid, BaseProjectView::GetThreeSyllableWordsLabel()))
            {
            m_readabilityProjectDoc->GetWordsBreakdownInfo().Enable3PlusSyllables(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetThreeSyllableWordsLabel()));
            }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid, BaseProjectView::GetSixCharWordsLabel()))
            {
            m_readabilityProjectDoc->GetWordsBreakdownInfo().Enable6PlusCharacter(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetSixCharWordsLabel()));
            }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid, BaseProjectView::GetWordCloudLabel()))
            {
            m_readabilityProjectDoc->GetWordsBreakdownInfo().EnableWordCloud(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetWordCloudLabel()));
            }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid, BaseProjectView::GetDaleChallLabel()))
            {
            m_readabilityProjectDoc->GetWordsBreakdownInfo().EnableDCUnfamiliar(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetDaleChallLabel()));
            }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid, BaseProjectView::GetSpacheLabel()))
            {
            m_readabilityProjectDoc->GetWordsBreakdownInfo().EnableSpacheUnfamiliar(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetSpacheLabel()));
            }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid, BaseProjectView::GetHarrisJacobsonLabel()))
            {
            m_readabilityProjectDoc->GetWordsBreakdownInfo().EnableHarrisJacobsonUnfamiliar(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetHarrisJacobsonLabel()));
            }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid,GetCustomTestsLabel()))
            {
            m_readabilityProjectDoc->GetWordsBreakdownInfo().EnableCustomTestsUnfamiliar(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(GetCustomTestsLabel()));
            }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid, BaseProjectView::GetAllWordsLabel()))
            {
            m_readabilityProjectDoc->GetWordsBreakdownInfo().EnableAllWords(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetAllWordsLabel()));
            }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid, BaseProjectView::GetKeyWordsLabel()))
            {
            m_readabilityProjectDoc->GetWordsBreakdownInfo().EnableKeyWords(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetKeyWordsLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid,GetIgnoreProperNounsLabel()))
            {
            m_readabilityProjectDoc->SpellCheckIgnoreProperNouns(
                m_grammarPropertyGrid->GetPropertyValueAsBool(GetIgnoreProperNounsLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid,GetIgnoreUppercasedWordsLabel()))
            {
            m_readabilityProjectDoc->SpellCheckIgnoreUppercased(
                m_grammarPropertyGrid->GetPropertyValueAsBool(GetIgnoreUppercasedWordsLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid,GetIgnoreNumeralsLabel()))
            {
            m_readabilityProjectDoc->SpellCheckIgnoreNumerals(
                m_grammarPropertyGrid->GetPropertyValueAsBool(GetIgnoreNumeralsLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid,GetIgnoreFileAddressesLabel()))
            {
            m_readabilityProjectDoc->SpellCheckIgnoreFileAddresses(
                m_grammarPropertyGrid->GetPropertyValueAsBool(GetIgnoreFileAddressesLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid,GetIgnoreProgrammerCodeLabel()))
            {
            m_readabilityProjectDoc->SpellCheckIgnoreProgrammerCode(
                m_grammarPropertyGrid->GetPropertyValueAsBool(GetIgnoreProgrammerCodeLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid,GetAllowColloquialismsLabel()))
            {
            m_readabilityProjectDoc->SpellCheckAllowColloquialisms(
                m_grammarPropertyGrid->GetPropertyValueAsBool(GetAllowColloquialismsLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid,GetIgnoreSocialMediaLabel()))
            {
            m_readabilityProjectDoc->SpellCheckIgnoreSocialMediaTags(
                m_grammarPropertyGrid->GetPropertyValueAsBool(GetIgnoreSocialMediaLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid,GetGrammarHighlightedReportLabel()))
            {
            m_readabilityProjectDoc->GetGrammarInfo().EnableHighlightedReport(
                m_grammarPropertyGrid->GetPropertyValueAsBool(GetGrammarHighlightedReportLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, BaseProjectView::GetMisspellingsLabel()))
            {
            m_readabilityProjectDoc->GetGrammarInfo().EnableMisspellings(
                m_grammarPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetMisspellingsLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, BaseProjectView::GetRepeatedWordsLabel()))
            {
            m_readabilityProjectDoc->GetGrammarInfo().EnableRepeatedWords(
                m_grammarPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetRepeatedWordsLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, BaseProjectView::GetArticleMismatchesLabel()))
            {
            m_readabilityProjectDoc->GetGrammarInfo().EnableArticleMismatches(
                m_grammarPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetArticleMismatchesLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, BaseProjectView::GetPhrasingErrorsTabLabel()))
            {
            m_readabilityProjectDoc->GetGrammarInfo().EnableWordingErrors(
                m_grammarPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetPhrasingErrorsTabLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, BaseProjectView::GetRedundantPhrasesTabLabel()))
            {
            m_readabilityProjectDoc->GetGrammarInfo().EnableRedundantPhrases(
                m_grammarPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetRedundantPhrasesTabLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, BaseProjectView::GetOverusedWordsBySentenceLabel()))
            {
            m_readabilityProjectDoc->GetGrammarInfo().EnableOverUsedWordsBySentence(
                m_grammarPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetOverusedWordsBySentenceLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, BaseProjectView::GetWordyPhrasesTabLabel()))
            {
            m_readabilityProjectDoc->GetGrammarInfo().EnableWordyPhrases(
                m_grammarPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetWordyPhrasesTabLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, BaseProjectView::GetClichesTabLabel()))
            {
            m_readabilityProjectDoc->GetGrammarInfo().EnableCliches(
                m_grammarPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetClichesTabLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, BaseProjectView::GetPassiveLabel()))
            {
            m_readabilityProjectDoc->GetGrammarInfo().EnablePassiveVoice(
                m_grammarPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetPassiveLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, BaseProjectView::GetSentenceStartingWithConjunctionsTabLabel()))
            {
            m_readabilityProjectDoc->GetGrammarInfo().EnableConjunctionStartingSentences(
                m_grammarPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetSentenceStartingWithConjunctionsTabLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid,BaseProjectView::GetSentenceStartingWithLowercaseTabLabel()))
            {
            m_readabilityProjectDoc->GetGrammarInfo().EnableLowercaseSentences(
                m_grammarPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetSentenceStartingWithLowercaseTabLabel()));
            }
        m_readabilityProjectDoc->SetFilePathTruncationMode(
            static_cast<ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode>(m_filePathTruncationMode.get_value()));
        m_readabilityProjectDoc->SetNumeralSyllabicationMethod(
            static_cast<NumeralSyllabize>(m_syllabicationMethod.get_value()));
        m_readabilityProjectDoc->SetParagraphsParsingMethod(
            static_cast<ParagraphParse>(m_paragraphParsingMethod.get_value()));
        m_readabilityProjectDoc->SetIgnoreBlankLinesForParagraphsParser(m_ignoreBlankLinesForParagraphsParser);
        m_readabilityProjectDoc->SetIgnoreIndentingForParagraphsParser(m_ignoreIndentingForParagraphsParser);
        m_readabilityProjectDoc->SetSentenceStartMustBeUppercased(m_sentenceStartMustBeUppercased);
        m_readabilityProjectDoc->AggressiveExclusion(m_aggressiveExclusion);
        m_readabilityProjectDoc->IgnoreTrailingCopyrightNoticeParagraphs(m_ignoreTrailingCopyrightNoticeParagraphs);
        m_readabilityProjectDoc->IgnoreTrailingCitations(m_ignoreTrailingCitations);
        m_readabilityProjectDoc->IgnoreFileAddresses(m_ignoreFileAddresses);
        m_readabilityProjectDoc->IgnoreNumerals(m_ignoreNumerals);
        m_readabilityProjectDoc->IgnoreProperNouns(m_ignoreProperNouns);
        m_readabilityProjectDoc->SetExcludedPhrasesPath(m_excludedPhrasesPath);
        m_readabilityProjectDoc->IncludeExcludedPhraseFirstOccurrence(m_includeExcludedPhraseFirstOccurrence);
        m_readabilityProjectDoc->SetExclusionBlockTags(m_exclusionBlockTags);
        m_readabilityProjectDoc->SetIncludeIncompleteSentencesIfLongerThanValue(
            m_includeIncompleteSentencesIfLongerThan);
        m_readabilityProjectDoc->SetInvalidSentenceMethod(
            static_cast<InvalidSentence>(
                (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) ?
                    static_cast<int>(InvalidSentence::ExcludeFromAnalysis) :
                (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings)) ?
                    static_cast<int>(InvalidSentence::ExcludeExceptForHeadings) :
                    static_cast<int>(InvalidSentence::IncludeAsFullSentences)) );
        m_readabilityProjectDoc->SetProjectLanguage(
            static_cast<readability::test_language>(m_projectLanguage.get_value()));
        m_readabilityProjectDoc->SetReviewer(m_reviewer.get_value());
        m_readabilityProjectDoc->SetStatus(m_status.get_value());
        m_readabilityProjectDoc->SetAppendedDocumentFilePath(m_appendedDocumentFilePath.get_value());
        m_readabilityProjectDoc->SetDocumentStorageMethod(
            static_cast<TextStorage>(m_documentStorageMethod.get_value()));
        if (IsStandardProjectSettings())
            {
            m_readabilityProjectDoc->SetOriginalDocumentFilePath(m_filePath);
            m_readabilityProjectDoc->GetSourceFilesInfo().at(0).second = m_description;
            }

        // test-specific options
        if (IsPropertyAvailable(m_readabilityTestsPropertyGrid, GetDCTextExclusionLabel()))
            {
            m_readabilityProjectDoc->SetDaleChallTextExclusionMode(
                static_cast<SpecializedTestTextExclusion>(m_readabilityTestsPropertyGrid->GetPropertyValueAsInt(
                    GetDCTextExclusionLabel())));
            }
        if (IsPropertyAvailable(m_readabilityTestsPropertyGrid, GetProperNounsLabel()))
            {
            m_readabilityProjectDoc->SetDaleChallProperNounCountingMethod(
                static_cast<readability::proper_noun_counting_method>
                (m_readabilityTestsPropertyGrid->GetPropertyValueAsInt(GetProperNounsLabel())));
            }
        if (IsPropertyAvailable(m_readabilityTestsPropertyGrid, GetIncludeStockerLabel()))
            {
            m_readabilityProjectDoc->IncludeStockerCatholicSupplement(
                m_readabilityTestsPropertyGrid->GetPropertyValueAsBool(GetIncludeStockerLabel()));
            }
        if (IsPropertyAvailable(m_readabilityTestsPropertyGrid, GetHJCTextExclusionLabel()))
            {
            m_readabilityProjectDoc->SetHarrisJacobsonTextExclusionMode(
                static_cast<SpecializedTestTextExclusion>(
                    m_readabilityTestsPropertyGrid->GetPropertyValueAsInt(GetHJCTextExclusionLabel())));
            }
        if (IsPropertyAvailable(m_readabilityTestsPropertyGrid, GetCountIndependentClausesLabel()))
            {
            m_readabilityProjectDoc->SetFogUseSentenceUnits(
                m_readabilityTestsPropertyGrid->GetPropertyValueAsBool(GetCountIndependentClausesLabel()));
            }
        if (IsPropertyAvailable(m_readabilityTestsPropertyGrid, GetFleschNumeralSyllabicationLabel()))
            {
            m_readabilityProjectDoc->SetFleschNumeralSyllabizeMethod(
                static_cast<FleschNumeralSyllabize>(
                    m_readabilityTestsPropertyGrid->GetPropertyValueAsInt(GetFleschNumeralSyllabicationLabel())));
            }
        if (IsPropertyAvailable(m_readabilityTestsPropertyGrid, GetFleschKincaidNumeralSyllabicationLabel()))
            {
            m_readabilityProjectDoc->SetFleschKincaidNumeralSyllabizeMethod(
                static_cast<FleschKincaidNumeralSyllabize>(m_readabilityTestsPropertyGrid->GetPropertyValueAsInt(
                    GetFleschKincaidNumeralSyllabicationLabel())));
            }

        SaveTextWindowOptions();
        SaveProjectGraphOptions();
        SaveStatisticsOptions();

        m_readabilityProjectDoc->RefreshRequired(ProjectRefresh::Minimal);
        }
    else if (!m_readabilityProjectDoc && HaveOptionsChanged())
        {
        wxGetApp().GetAppOptions().SetTextHighlightMethod(
            static_cast<TextHighlight>(m_textHighlightMethod.get_value()));
        wxGetApp().GetAppOptions().SetTextHighlightColor(m_highlightedColor);
        wxGetApp().GetAppOptions().SetExcludedTextHighlightColor(m_excludedTextHighlightColor);
        wxGetApp().GetAppOptions().SetDuplicateWordHighlightColor(m_duplicateWordHighlightColor);
        wxGetApp().GetAppOptions().SetWordyPhraseHighlightColor(m_wordyPhraseHighlightColor);
        wxGetApp().GetAppOptions().SetTextViewFont(m_font);
        wxGetApp().GetAppOptions().SetTextFontColor(m_fontColor);
        wxGetApp().GetAppOptions().SetDolchConjunctionsColor(m_dolchConjunctionsColor);
        wxGetApp().GetAppOptions().SetDolchPrepositionsColor(m_dolchPrepositionsColor);
        wxGetApp().GetAppOptions().SetDolchPronounsColor(m_dolchPronounsColor);
        wxGetApp().GetAppOptions().SetDolchAdverbsColor(m_dolchAdverbsColor);
        wxGetApp().GetAppOptions().SetDolchAdjectivesColor(m_dolchAdjectivesColor);
        wxGetApp().GetAppOptions().SetDolchVerbsColor(m_dolchVerbsColor);
        wxGetApp().GetAppOptions().SetDolchNounColor(m_dolchNounColor);
        wxGetApp().GetAppOptions().HighlightDolchConjunctions(m_highlightDolchConjunctions);
        wxGetApp().GetAppOptions().HighlightDolchPrepositions(m_highlightDolchPrepositions);
        wxGetApp().GetAppOptions().HighlightDolchPronouns(m_highlightDolchPronouns);
        wxGetApp().GetAppOptions().HighlightDolchAdverbs(m_highlightDolchAdverbs);
        wxGetApp().GetAppOptions().HighlightDolchAdjectives(m_highlightDolchAdjectives);
        wxGetApp().GetAppOptions().HighlightDolchVerbs(m_highlightDolchVerbs);
        wxGetApp().GetAppOptions().HighlightDolchNouns(m_highlightDolchNouns);
        if (IsPropertyAvailable(m_gradeLevelPropertyGrid,GetCalculationLabel()))
            {
            wxGetApp().GetAppOptions().GetReadabilityMessageCatalog().SetReadingAgeDisplay(
                static_cast<ReadabilityMessages::ReadingAgeDisplay>(
                    m_gradeLevelPropertyGrid->GetPropertyValueAsInt(GetCalculationLabel())));
            }
        if (IsPropertyAvailable(m_gradeLevelPropertyGrid,GetGradeScaleLabel()))
            {
            wxGetApp().GetAppOptions().GetReadabilityMessageCatalog().SetGradeScale(
                static_cast<readability::grade_scale>(m_gradeLevelPropertyGrid->GetPropertyValueAsInt(
                    GetGradeScaleLabel())));
            }
        if (IsPropertyAvailable(m_gradeLevelPropertyGrid,GetGradesLongFormatLabel()))
            {
            wxGetApp().GetAppOptions().GetReadabilityMessageCatalog().SetLongGradeScaleFormat(
                m_gradeLevelPropertyGrid->GetPropertyValueAsBool(GetGradesLongFormatLabel()));
            }
        if (IsPropertyAvailable(m_gradeLevelPropertyGrid,GetIncludeScoreSummaryLabel()))
            {
            wxGetApp().GetAppOptions().IncludeScoreSummaryReport(
                m_gradeLevelPropertyGrid->GetPropertyValueAsBool(GetIncludeScoreSummaryLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid,GetParagraphsLabel()))
            {
            wxGetApp().GetAppOptions().GetStatisticsReportInfo().EnableParagraph(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(GetParagraphsLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid,GetSentencesLabel()))
            {
            wxGetApp().GetAppOptions().GetStatisticsReportInfo().EnableSentences(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(GetSentencesLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid,GetWordsLabel()))
            {
            wxGetApp().GetAppOptions().GetStatisticsReportInfo().EnableWords(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(GetWordsLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid,GetExtendedWordsLabel()))
            {
            wxGetApp().GetAppOptions().GetStatisticsReportInfo().EnableExtendedWords(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(GetExtendedWordsLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid,GetGrammarLabel()))
            {
            wxGetApp().GetAppOptions().GetStatisticsReportInfo().EnableGrammar(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(GetGrammarLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid,GetNotesLabel()))
            {
            wxGetApp().GetAppOptions().GetStatisticsReportInfo().EnableNotes(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(GetNotesLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid,GetExtendedInformationLabel()))
            {
            wxGetApp().GetAppOptions().GetStatisticsReportInfo().EnableExtendedInformation(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(GetExtendedInformationLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid,GetCoverageLabel()))
            {
            wxGetApp().GetAppOptions().GetStatisticsReportInfo().EnableDolchCoverage(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(GetCoverageLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid,GetDolchWordsLabel()))
            {
            wxGetApp().GetAppOptions().GetStatisticsReportInfo().EnableDolchWords(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(GetDolchWordsLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid,GetDolchExplanationLabel()))
            {
            wxGetApp().GetAppOptions().GetStatisticsReportInfo().EnableDolchExplanation(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(GetDolchExplanationLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid,BaseProjectView::GetFormattedReportLabel()))
            {
            wxGetApp().GetAppOptions().GetStatisticsInfo().EnableReport(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetFormattedReportLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid,BaseProjectView::GetTabularReportLabel()))
            {
            wxGetApp().GetAppOptions().GetStatisticsInfo().EnableTable(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetTabularReportLabel()));
            }
        wxGetApp().GetAppOptions().SetLongSentenceMethod(m_longSentencesNumberOfWords ?
            LongSentence::LongerThanSpecifiedLength : LongSentence::OutlierLength);
        wxGetApp().GetAppOptions().SetDifficultSentenceLength(m_sentenceLength);
        wxGetApp().GetAppOptions().SetMinDocWordCountForBatch(m_minDocWordCountForBatch);
        if (IsPropertyAvailable(m_grammarPropertyGrid,GetIgnoreProperNounsLabel()))
            {
            wxGetApp().GetAppOptions().SpellCheckIgnoreProperNouns(
                m_grammarPropertyGrid->GetPropertyValueAsBool(GetIgnoreProperNounsLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid,GetIgnoreUppercasedWordsLabel()))
            {
            wxGetApp().GetAppOptions().SpellCheckIgnoreUppercased(
                m_grammarPropertyGrid->GetPropertyValueAsBool(GetIgnoreUppercasedWordsLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid,GetIgnoreNumeralsLabel()))
            {
            wxGetApp().GetAppOptions().SpellCheckIgnoreNumerals(
                m_grammarPropertyGrid->GetPropertyValueAsBool(GetIgnoreNumeralsLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid,GetIgnoreFileAddressesLabel()))
            {
            wxGetApp().GetAppOptions().SpellCheckIgnoreFileAddresses(
                m_grammarPropertyGrid->GetPropertyValueAsBool(GetIgnoreFileAddressesLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid,GetIgnoreProgrammerCodeLabel()))
            {
            wxGetApp().GetAppOptions().SpellCheckIgnoreProgrammerCode(
                m_grammarPropertyGrid->GetPropertyValueAsBool(GetIgnoreProgrammerCodeLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid,GetAllowColloquialismsLabel()))
            {
            wxGetApp().GetAppOptions().SpellCheckAllowColloquialisms(
                m_grammarPropertyGrid->GetPropertyValueAsBool(GetAllowColloquialismsLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid,GetIgnoreSocialMediaLabel()))
            {
            wxGetApp().GetAppOptions().SpellCheckIgnoreSocialMediaTags(
                m_grammarPropertyGrid->GetPropertyValueAsBool(GetIgnoreSocialMediaLabel())); }
        if (IsPropertyAvailable(m_sentencesBreakdownPropertyGrid, BaseProjectView::GetLongSentencesLabel()))
            {
            wxGetApp().GetAppOptions().GetSentencesBreakdownInfo().EnableLongSentences(
                m_sentencesBreakdownPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetLongSentencesLabel()));
            }
        if (IsPropertyAvailable(m_sentencesBreakdownPropertyGrid, BaseProjectView::GetSentenceLengthBoxPlotLabel()))
            {
            wxGetApp().GetAppOptions().GetSentencesBreakdownInfo().EnableLengthsBoxPlot(
                m_sentencesBreakdownPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetSentenceLengthBoxPlotLabel()));
            }
        if (IsPropertyAvailable(m_sentencesBreakdownPropertyGrid, BaseProjectView::GetSentenceLengthHeatmapLabel()))
            {
            wxGetApp().GetAppOptions().GetSentencesBreakdownInfo().EnableLengthsHeatmap(
                m_sentencesBreakdownPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetSentenceLengthHeatmapLabel()));
            }
        if (IsPropertyAvailable(m_sentencesBreakdownPropertyGrid, BaseProjectView::GetSentenceLengthHistogramLabel()))
            {
            wxGetApp().GetAppOptions().GetSentencesBreakdownInfo().EnableLengthsHistogram(
                m_sentencesBreakdownPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetSentenceLengthHistogramLabel())); }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid, BaseProjectView::GetWordCountsLabel()))
            {
            wxGetApp().GetAppOptions().GetWordsBreakdownInfo().EnableWordBarchart(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetWordCountsLabel()));
            }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid, BaseProjectView::GetSyllableCountsLabel()))
            {
            wxGetApp().GetAppOptions().GetWordsBreakdownInfo().EnableSyllableHistogram(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetSyllableCountsLabel()));
            }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid, BaseProjectView::GetThreeSyllableWordsLabel()))
            {
            wxGetApp().GetAppOptions().GetWordsBreakdownInfo().Enable3PlusSyllables(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetThreeSyllableWordsLabel()));
            }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid, BaseProjectView::GetSixCharWordsLabel()))
            {
            wxGetApp().GetAppOptions().GetWordsBreakdownInfo().Enable6PlusCharacter(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetSixCharWordsLabel()));
            }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid, BaseProjectView::GetWordCloudLabel()))
            {
            wxGetApp().GetAppOptions().GetWordsBreakdownInfo().EnableWordCloud(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetWordCloudLabel()));
            }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid, BaseProjectView::GetDaleChallLabel()))
            {
            wxGetApp().GetAppOptions().GetWordsBreakdownInfo().EnableDCUnfamiliar(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetDaleChallLabel()));
            }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid, BaseProjectView::GetSpacheLabel()))
            {
            wxGetApp().GetAppOptions().GetWordsBreakdownInfo().EnableSpacheUnfamiliar(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetSpacheLabel()));
            }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid, BaseProjectView::GetHarrisJacobsonLabel()))
            {
            wxGetApp().GetAppOptions().GetWordsBreakdownInfo().EnableHarrisJacobsonUnfamiliar(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetHarrisJacobsonLabel()));
            }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid,GetCustomTestsLabel()))
            {
            wxGetApp().GetAppOptions().GetWordsBreakdownInfo().EnableCustomTestsUnfamiliar(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(GetCustomTestsLabel()));
            }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid, BaseProjectView::GetAllWordsLabel()))
            {
            wxGetApp().GetAppOptions().GetWordsBreakdownInfo().EnableAllWords(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetAllWordsLabel()));
            }
        if (IsPropertyAvailable(m_wordsBreakdownPropertyGrid, BaseProjectView::GetKeyWordsLabel()))
            {
            wxGetApp().GetAppOptions().GetWordsBreakdownInfo().EnableKeyWords(
                m_wordsBreakdownPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetKeyWordsLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid,GetGrammarHighlightedReportLabel()))
            {
            wxGetApp().GetAppOptions().GetGrammarInfo().EnableHighlightedReport(
                m_grammarPropertyGrid->GetPropertyValueAsBool(GetGrammarHighlightedReportLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, BaseProjectView::GetMisspellingsLabel()))
            {
            wxGetApp().GetAppOptions().GetGrammarInfo().EnableMisspellings(
                m_grammarPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetMisspellingsLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, BaseProjectView::GetRepeatedWordsLabel()))
            {
            wxGetApp().GetAppOptions().GetGrammarInfo().EnableRepeatedWords(
                m_grammarPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetRepeatedWordsLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, BaseProjectView::GetArticleMismatchesLabel()))
            {
            wxGetApp().GetAppOptions().GetGrammarInfo().EnableArticleMismatches(
                m_grammarPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetArticleMismatchesLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, BaseProjectView::GetPhrasingErrorsTabLabel()))
            {
            wxGetApp().GetAppOptions().GetGrammarInfo().EnableWordingErrors(
                m_grammarPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetPhrasingErrorsTabLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, BaseProjectView::GetRedundantPhrasesTabLabel()))
            {
            wxGetApp().GetAppOptions().GetGrammarInfo().EnableRedundantPhrases(
                m_grammarPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetRedundantPhrasesTabLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, BaseProjectView::GetOverusedWordsBySentenceLabel()))
            {
            wxGetApp().GetAppOptions().GetGrammarInfo().EnableOverUsedWordsBySentence(
                m_grammarPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetOverusedWordsBySentenceLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid,BaseProjectView::GetWordyPhrasesTabLabel()))
            {
            wxGetApp().GetAppOptions().GetGrammarInfo().EnableWordyPhrases(
                m_grammarPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetWordyPhrasesTabLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, BaseProjectView::GetClichesTabLabel()))
            {
            wxGetApp().GetAppOptions().GetGrammarInfo().EnableCliches(
                m_grammarPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetClichesTabLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, BaseProjectView::GetPassiveLabel()))
            {
            wxGetApp().GetAppOptions().GetGrammarInfo().EnablePassiveVoice(
                m_grammarPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetPassiveLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, BaseProjectView::GetSentenceStartingWithConjunctionsTabLabel()))
            {
            wxGetApp().GetAppOptions().GetGrammarInfo().EnableConjunctionStartingSentences(
                m_grammarPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetSentenceStartingWithConjunctionsTabLabel()));
            }
        if (IsPropertyAvailable(m_grammarPropertyGrid, BaseProjectView::GetSentenceStartingWithLowercaseTabLabel()))
            {
            wxGetApp().GetAppOptions().GetGrammarInfo().EnableLowercaseSentences(
                m_grammarPropertyGrid->GetPropertyValueAsBool(
                    BaseProjectView::GetSentenceStartingWithLowercaseTabLabel()));
            }
        wxGetApp().GetAppOptions().SetFilePathTruncationMode(
            static_cast<ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode>(m_filePathTruncationMode.get_value()));
        wxGetApp().GetAppOptions().SetNumeralSyllabicationMethod(
            static_cast<NumeralSyllabize>(m_syllabicationMethod.get_value()));
        wxGetApp().GetAppOptions().SetParagraphsParsingMethod(
            static_cast<ParagraphParse>(m_paragraphParsingMethod.get_value()));
        wxGetApp().GetAppOptions().SetIgnoreBlankLinesForParagraphsParser(m_ignoreBlankLinesForParagraphsParser);
        wxGetApp().GetAppOptions().SetIgnoreIndentingForParagraphsParser(m_ignoreIndentingForParagraphsParser);
        wxGetApp().GetAppOptions().SetSentenceStartMustBeUppercased(m_sentenceStartMustBeUppercased);
        wxGetApp().GetAppOptions().AggressiveExclusion(m_aggressiveExclusion);
        wxGetApp().GetAppOptions().IgnoreTrailingCopyrightNoticeParagraphs(m_ignoreTrailingCopyrightNoticeParagraphs);
        wxGetApp().GetAppOptions().IgnoreTrailingCitations(m_ignoreTrailingCitations);
        wxGetApp().GetAppOptions().IgnoreFileAddresses(m_ignoreFileAddresses);
        wxGetApp().GetAppOptions().IgnoreNumerals(m_ignoreNumerals);
        wxGetApp().GetAppOptions().IgnoreProperNouns(m_ignoreProperNouns);
        wxGetApp().GetAppOptions().IncludeExcludedPhraseFirstOccurrence(m_includeExcludedPhraseFirstOccurrence);
        wxGetApp().GetAppOptions().SetExcludedPhrasesPath(m_excludedPhrasesPath);
        wxGetApp().GetAppOptions().SetExclusionBlockTags(m_exclusionBlockTags);
        wxGetApp().GetAppOptions().SetIncludeIncompleteSentencesIfLongerThanValue(
            m_includeIncompleteSentencesIfLongerThan);
        wxGetApp().GetAppOptions().SetInvalidSentenceMethod(
            static_cast<InvalidSentence>(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) ?
                static_cast<int>(InvalidSentence::ExcludeFromAnalysis) :
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings)) ?
                static_cast<int>(InvalidSentence::ExcludeExceptForHeadings) :
                static_cast<int>(InvalidSentence::IncludeAsFullSentences)) );

        wxGetApp().GetAppOptions().SetProjectLanguage(
            static_cast<readability::test_language>(m_projectLanguage.get_value()));
        wxGetApp().GetAppOptions().SetReviewer(m_reviewer.get_value());
        wxGetApp().GetAppOptions().SetStatus(m_status.get_value());
        wxGetApp().GetAppOptions().SetAppendedDocumentFilePath(m_appendedDocumentFilePath.get_value());
        wxGetApp().GetAppOptions().SetDocumentStorageMethod(
            static_cast<TextStorage>(m_documentStorageMethod.get_value()));

        // test-specific options
        if (IsPropertyAvailable(m_readabilityTestsPropertyGrid, GetDCTextExclusionLabel()))
            {
            wxGetApp().GetAppOptions().SetDaleChallTextExclusionMode(
                static_cast<SpecializedTestTextExclusion>(
                    m_readabilityTestsPropertyGrid->GetPropertyValueAsInt(GetDCTextExclusionLabel())));
            }
        if (IsPropertyAvailable(m_readabilityTestsPropertyGrid, GetProperNounsLabel()))
            {
            wxGetApp().GetAppOptions().SetDaleChallProperNounCountingMethod(
                static_cast<readability::proper_noun_counting_method>(
                    m_readabilityTestsPropertyGrid->GetPropertyValueAsInt(GetProperNounsLabel())));
            }
        if (IsPropertyAvailable(m_readabilityTestsPropertyGrid, GetIncludeStockerLabel()))
            {
            wxGetApp().GetAppOptions().IncludeStockerCatholicSupplement(
                m_readabilityTestsPropertyGrid->GetPropertyValueAsBool(GetIncludeStockerLabel()));
            }
        if (IsPropertyAvailable(m_readabilityTestsPropertyGrid, GetHJCTextExclusionLabel()))
            {
            wxGetApp().GetAppOptions().SetHarrisJacobsonTextExclusionMode(
                static_cast<SpecializedTestTextExclusion>(
                    m_readabilityTestsPropertyGrid->GetPropertyValueAsInt(GetHJCTextExclusionLabel())));
            }
        if (IsPropertyAvailable(m_readabilityTestsPropertyGrid, GetCountIndependentClausesLabel()))
            { wxGetApp().GetAppOptions().SetFogUseSentenceUnits(
                m_readabilityTestsPropertyGrid->GetPropertyValueAsBool(GetCountIndependentClausesLabel())); }
        if (IsPropertyAvailable(m_readabilityTestsPropertyGrid, GetFleschNumeralSyllabicationLabel()))
            {
            wxGetApp().GetAppOptions().SetFleschNumeralSyllabizeMethod(
                static_cast<FleschNumeralSyllabize>(
                    m_readabilityTestsPropertyGrid->GetPropertyValueAsInt(GetFleschNumeralSyllabicationLabel())));
            }
        if (IsPropertyAvailable(m_readabilityTestsPropertyGrid, GetFleschKincaidNumeralSyllabicationLabel()))
            {
            wxGetApp().GetAppOptions().SetFleschKincaidNumeralSyllabizeMethod(
                static_cast<FleschKincaidNumeralSyllabize>(
                    m_readabilityTestsPropertyGrid->GetPropertyValueAsInt(
                        GetFleschKincaidNumeralSyllabicationLabel())));
            }

        if (IsPropertyAvailable(m_generalGraphPropertyGrid, GetBackgroundColorLabel()))
            {
            wxGetApp().GetAppOptions().SetBackGroundColor(
                wxAny(m_generalGraphPropertyGrid->GetProperty(
                    GetBackgroundColorLabel())->GetValue()).As<wxColour>());
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid, GetImageLabel()))
            {
            wxGetApp().GetAppOptions().SetBackGroundImagePath(
                m_generalGraphPropertyGrid->GetPropertyValueAsString(GetImageLabel()));
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid, GetImageEffectLabel()))
            {
            wxGetApp().GetAppOptions().SetBackGroundImageEffect(
                static_cast<ImageEffect>(m_generalGraphPropertyGrid->GetPropertyValueAsInt(GetImageEffectLabel())));
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid,GetWatermarkLabel()))
            {
            wxGetApp().GetAppOptions().SetWatermark(
                m_generalGraphPropertyGrid->GetPropertyValueAsString(GetWatermarkLabel()));
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid,GetLogoImageLabel()))
            {
            wxGetApp().GetAppOptions().SetWatermarkLogo(
                m_generalGraphPropertyGrid->GetPropertyValueAsString(GetLogoImageLabel()));
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid,GetImageOpacityLabel()))
            {
            wxGetApp().GetAppOptions().SetGraphBackGroundOpacity(
                static_cast<uint8_t>(m_generalGraphPropertyGrid->GetPropertyValueAsInt(GetImageOpacityLabel())));
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid, GetBackgroundColorFadeLabel()))
            {
            wxGetApp().GetAppOptions().SetGraphBackGroundLinearGradient(
                m_generalGraphPropertyGrid->GetPropertyValueAsBool(GetBackgroundColorFadeLabel()));
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid,GetColorLabel()))
            {
            wxGetApp().GetAppOptions().SetPlotBackGroundColor(
                wxAny(m_generalGraphPropertyGrid->GetProperty(GetColorLabel())->GetValue()).As<wxColour>());
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid,GetOpacityLabel()))
            {
            wxGetApp().GetAppOptions().SetGraphPlotBackGroundOpacity(
                static_cast<uint8_t>(m_generalGraphPropertyGrid->GetPropertyValueAsInt(GetOpacityLabel())));
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid,GetCustomImageBrushLabel()))
            {
            wxGetApp().GetAppOptions().SetGraphStippleImagePath(
                m_generalGraphPropertyGrid->GetPropertyValueAsString(GetCustomImageBrushLabel())); }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid,GetDisplayDropShadowsLabel()))
            {
            wxGetApp().GetAppOptions().DisplayDropShadows(
                m_generalGraphPropertyGrid->GetPropertyValueAsBool(GetDisplayDropShadowsLabel()));
            }

        if (IsPropertyAvailable(m_barChartPropertyGrid,GetColorLabel()))
            {
            wxGetApp().GetAppOptions().SetBarChartBarColor(
                wxAny(m_barChartPropertyGrid->GetProperty(GetColorLabel())->GetValue()).As<wxColour>());
            }
        if (IsPropertyAvailable(m_barChartPropertyGrid,GetOpacityLabel()))
            {
            wxGetApp().GetAppOptions().SetGraphBarOpacity(
                static_cast<uint8_t>(m_barChartPropertyGrid->GetPropertyValueAsInt(GetOpacityLabel())));
            }
        if (IsPropertyAvailable(m_barChartPropertyGrid,GeOrientationLabel()))
            {
            wxGetApp().GetAppOptions().SetBarChartOrientation(
                static_cast<Wisteria::Orientation>(
                    m_barChartPropertyGrid->GetPropertyValueAsInt(GeOrientationLabel())));
            }
        if (IsPropertyAvailable(m_barChartPropertyGrid,GetEffectLabel()))
            {
            wxGetApp().GetAppOptions().SetGraphBarEffect(
                static_cast<BoxEffect>(m_barChartPropertyGrid->GetPropertyValueAsInt(GetEffectLabel())));
            }
        if (IsPropertyAvailable(m_barChartPropertyGrid,GetLabelsOnBarsLabel()))
            {
            wxGetApp().GetAppOptions().DisplayBarLabels(
                m_barChartPropertyGrid->GetPropertyValueAsBool(GetLabelsOnBarsLabel()));
            }

        if (IsPropertyAvailable(m_histogramPropertyGrid,GetBinSortingLabel()))
            {
            wxGetApp().GetAppOptions().SetHistorgramBinningMethod(
                static_cast<Histogram::BinningMethod>(
                    m_histogramPropertyGrid->GetPropertyValueAsInt(GetBinSortingLabel())));
            }
        if (IsPropertyAvailable(m_histogramPropertyGrid,GetGradeLevelRoundingLabel()))
            {
            wxGetApp().GetAppOptions().SetHistogramRoundingMethod(
                static_cast<RoundingMethod>(
                    m_histogramPropertyGrid->GetPropertyValueAsInt(GetGradeLevelRoundingLabel())));
            }
        if (IsPropertyAvailable(m_histogramPropertyGrid,GetBinLabelsLabel()))
            {
            wxGetApp().GetAppOptions().SetHistrogramBinLabelDisplay(
                static_cast<BinLabelDisplay>(
                    m_histogramPropertyGrid->GetPropertyValueAsInt(GetBinLabelsLabel())));
            }
        if (IsPropertyAvailable(m_histogramPropertyGrid,GetIntervalDisplayLabel()))
            {
            wxGetApp().GetAppOptions().SetHistogramIntervalDisplay(
                static_cast<Histogram::IntervalDisplay>(
                    m_histogramPropertyGrid->GetPropertyValueAsInt(GetIntervalDisplayLabel())));
            }
        if (IsPropertyAvailable(m_histogramPropertyGrid,GetColorLabel()))
            {
            wxGetApp().GetAppOptions().SetHistogramBarColor(
                wxAny(m_histogramPropertyGrid->GetProperty(GetColorLabel())->GetValue()).As<wxColour>());
            }
        if (IsPropertyAvailable(m_histogramPropertyGrid,GetOpacityLabel()))
            {
            wxGetApp().GetAppOptions().SetHistogramBarOpacity(
                static_cast<uint8_t>(m_histogramPropertyGrid->GetPropertyValueAsInt(GetOpacityLabel())));
            }
        if (IsPropertyAvailable(m_histogramPropertyGrid,GetEffectLabel()))
            {
            wxGetApp().GetAppOptions().SetHistogramBarEffect(
                static_cast<BoxEffect>(m_histogramPropertyGrid->GetPropertyValueAsInt(GetEffectLabel())));
            }

        if (IsPropertyAvailable(m_boxPlotsPropertyGrid,GetColorLabel()))
            {
            wxGetApp().GetAppOptions().SetGraphBoxColor(
                wxAny(m_boxPlotsPropertyGrid->GetProperty(GetColorLabel())->GetValue()).As<wxColour>());
            }
        if (IsPropertyAvailable(m_boxPlotsPropertyGrid,GetOpacityLabel()))
            {
            wxGetApp().GetAppOptions().SetGraphBoxOpacity(
                static_cast<uint8_t>(m_boxPlotsPropertyGrid->GetPropertyValueAsInt(GetOpacityLabel())));
            }
        if (IsPropertyAvailable(m_boxPlotsPropertyGrid,GetEffectLabel()))
            {
            wxGetApp().GetAppOptions().SetGraphBoxEffect(
                static_cast<BoxEffect>(m_boxPlotsPropertyGrid->GetPropertyValueAsInt(GetEffectLabel())));
            }
        if (IsPropertyAvailable(m_boxPlotsPropertyGrid,GetLabelsOnBoxesLabel()))
            {
            wxGetApp().GetAppOptions().DisplayBoxPlotLabels(
                m_boxPlotsPropertyGrid->GetPropertyValueAsBool(GetLabelsOnBoxesLabel()));
            }
        if (IsPropertyAvailable(m_boxPlotsPropertyGrid,GetConnectBoxesLabel()))
            {
            wxGetApp().GetAppOptions().ConnectBoxPlotMiddlePoints(
                m_boxPlotsPropertyGrid->GetPropertyValueAsBool(GetConnectBoxesLabel()));
            }
        if (IsPropertyAvailable(m_boxPlotsPropertyGrid,GetShowAllDataPointsLabel()))
            {
            wxGetApp().GetAppOptions().ShowAllBoxPlotPoints(
                m_boxPlotsPropertyGrid->GetPropertyValueAsBool(GetShowAllDataPointsLabel()));
            }

        wxGetApp().GetAppOptions().SetXAxisFont(m_xAxisFont);
        wxGetApp().GetAppOptions().SetXAxisFontColor(m_xAxisFontColor);
        wxGetApp().GetAppOptions().SetYAxisFont(m_yAxisFont);
        wxGetApp().GetAppOptions().SetYAxisFontColor(m_yAxisFontColor);
        wxGetApp().GetAppOptions().SetTopTitleGraphFont(m_topTitleFont);
        wxGetApp().GetAppOptions().SetTopTitleGraphFontColor(m_topTitleFontColor);
        wxGetApp().GetAppOptions().SetBottomTitleGraphFont(m_bottomTitleFont);
        wxGetApp().GetAppOptions().SetBottomTitleGraphFontColor(m_bottomTitleFontColor);
        wxGetApp().GetAppOptions().SetLeftTitleGraphFont(m_leftTitleFont);
        wxGetApp().GetAppOptions().SetLeftTitleGraphFontColor(m_leftTitleFontColor);
        wxGetApp().GetAppOptions().SetRightTitleGraphFont(m_rightTitleFont);
        wxGetApp().GetAppOptions().SetRightTitleGraphFontColor(m_rightTitleFontColor);
        if (IsPropertyAvailable(m_readabilityGraphPropertyGrid, GetInvalidRegionsColorLabel()))
            {
            wxGetApp().GetAppOptions().SetInvalidAreaColor(
                wxAny(m_readabilityGraphPropertyGrid->GetProperty(
                    GetInvalidRegionsColorLabel())->GetValue()).As<wxColour>());
            }
        if (IsPropertyAvailable(m_readabilityGraphPropertyGrid, GetRaygorStyleLabel()))
            {
            wxGetApp().GetAppOptions().SetRaygorStyle(
                static_cast<Wisteria::Graphs::RaygorStyle>(
                    m_readabilityGraphPropertyGrid->GetPropertyValueAsInt(GetRaygorStyleLabel())) );
            }
        if (IsPropertyAvailable(m_readabilityGraphPropertyGrid,GetUseEnglishLabelsForGermanLixLabel()))
            {
            wxGetApp().GetAppOptions().UseEnglishLabelsForGermanLix(
                m_readabilityGraphPropertyGrid->GetPropertyValueAsBool(GetUseEnglishLabelsForGermanLixLabel()));
            }
        if (IsPropertyAvailable(m_readabilityGraphPropertyGrid,GetFleschChartConnectPointsLabel()))
            {
            wxGetApp().GetAppOptions().ConnectFleschPoints(
                m_readabilityGraphPropertyGrid->GetPropertyValueAsBool(GetFleschChartConnectPointsLabel()));
            }
        if (IsPropertyAvailable(m_readabilityGraphPropertyGrid,GetFleschSyllableRulerDocGroupsLabel()))
            {
            wxGetApp().GetAppOptions().IncludeFleschRulerDocGroups(
                m_readabilityGraphPropertyGrid->GetPropertyValueAsBool(GetFleschSyllableRulerDocGroupsLabel()));
            }
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::SaveStatisticsOptions()
    {
    if (m_readabilityProjectDoc && HaveOptionsChanged())
        {
        if (IsPropertyAvailable(m_statisticsPropertyGrid,GetParagraphsLabel()))
            {
            m_readabilityProjectDoc->GetStatisticsReportInfo().EnableParagraph(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(GetParagraphsLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid,GetSentencesLabel()))
            {
            m_readabilityProjectDoc->GetStatisticsReportInfo().EnableSentences(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(GetSentencesLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid,GetWordsLabel()))
            {
            m_readabilityProjectDoc->GetStatisticsReportInfo().EnableWords(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(GetWordsLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid,GetExtendedWordsLabel()))
            {
            m_readabilityProjectDoc->GetStatisticsReportInfo().EnableExtendedWords(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(GetExtendedWordsLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid,GetGrammarLabel()))
            {
            m_readabilityProjectDoc->GetStatisticsReportInfo().EnableGrammar(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(GetGrammarLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid,GetNotesLabel()))
            {
            m_readabilityProjectDoc->GetStatisticsReportInfo().EnableNotes(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(GetNotesLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid,GetExtendedInformationLabel()))
            {
            m_readabilityProjectDoc->GetStatisticsReportInfo().EnableExtendedInformation(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(GetExtendedInformationLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid,GetCoverageLabel()))
            {
            m_readabilityProjectDoc->GetStatisticsReportInfo().EnableDolchCoverage(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(GetCoverageLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid,GetDolchWordsLabel()))
            {
            m_readabilityProjectDoc->GetStatisticsReportInfo().EnableDolchWords
            (m_statisticsPropertyGrid->GetPropertyValueAsBool(GetDolchWordsLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid,GetDolchExplanationLabel()))
            {
            m_readabilityProjectDoc->GetStatisticsReportInfo().EnableDolchExplanation(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(GetDolchExplanationLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid,BaseProjectView::GetFormattedReportLabel()))
            {
            m_readabilityProjectDoc->GetStatisticsInfo().EnableReport(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetFormattedReportLabel()));
            }
        if (IsPropertyAvailable(m_statisticsPropertyGrid,BaseProjectView::GetTabularReportLabel()))
            {
            m_readabilityProjectDoc->GetStatisticsInfo().EnableTable(
                m_statisticsPropertyGrid->GetPropertyValueAsBool(BaseProjectView::GetTabularReportLabel()));
            }

        m_readabilityProjectDoc->RefreshRequired(ProjectRefresh::Minimal);
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::SaveTextWindowOptions()
    {
    if (m_readabilityProjectDoc && HaveOptionsChanged())
        {
        m_readabilityProjectDoc->SetTextHighlightMethod(static_cast<TextHighlight>(m_textHighlightMethod.get_value()));
        m_readabilityProjectDoc->SetTextHighlightColor(m_highlightedColor);
        m_readabilityProjectDoc->SetExcludedTextHighlightColor(m_excludedTextHighlightColor);
        m_readabilityProjectDoc->SetDuplicateWordHighlightColor(m_duplicateWordHighlightColor);
        m_readabilityProjectDoc->SetWordyPhraseHighlightColor(m_wordyPhraseHighlightColor);
        m_readabilityProjectDoc->SetTextViewFont(m_font);
        m_readabilityProjectDoc->SetTextFontColor(m_fontColor);
        m_readabilityProjectDoc->SetDolchConjunctionsColor(m_dolchConjunctionsColor);
        m_readabilityProjectDoc->SetDolchPrepositionsColor(m_dolchPrepositionsColor);
        m_readabilityProjectDoc->SetDolchPronounsColor(m_dolchPronounsColor);
        m_readabilityProjectDoc->SetDolchAdverbsColor(m_dolchAdverbsColor);
        m_readabilityProjectDoc->SetDolchAdjectivesColor(m_dolchAdjectivesColor);
        m_readabilityProjectDoc->SetDolchVerbsColor(m_dolchVerbsColor);
        m_readabilityProjectDoc->SetDolchNounColor(m_dolchNounColor);
        m_readabilityProjectDoc->HighlightDolchConjunctions(m_highlightDolchConjunctions);
        m_readabilityProjectDoc->HighlightDolchPrepositions(m_highlightDolchPrepositions);
        m_readabilityProjectDoc->HighlightDolchPronouns(m_highlightDolchPronouns);
        m_readabilityProjectDoc->HighlightDolchAdverbs(m_highlightDolchAdverbs);
        m_readabilityProjectDoc->HighlightDolchAdjectives(m_highlightDolchAdjectives);
        m_readabilityProjectDoc->HighlightDolchVerbs(m_highlightDolchVerbs);
        m_readabilityProjectDoc->HighlightDolchNouns(m_highlightDolchNouns);

        m_readabilityProjectDoc->RefreshRequired(ProjectRefresh::TextSection);
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::SaveProjectGraphOptions()
    {
    if (m_readabilityProjectDoc && HaveOptionsChanged())
        {
        if (IsPropertyAvailable(m_generalGraphPropertyGrid,GetBackgroundColorLabel()))
            {
            m_readabilityProjectDoc->SetBackGroundColor(
                wxAny(m_generalGraphPropertyGrid->GetProperty(GetBackgroundColorLabel())->GetValue()).As<wxColour>());
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid, GetImageLabel()))
            {
            m_readabilityProjectDoc->SetBackGroundImagePath(
                m_generalGraphPropertyGrid->GetPropertyValueAsString(GetImageLabel()));
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid, GetImageEffectLabel()))
            {
            m_readabilityProjectDoc->SetBackGroundImageEffect(
                static_cast<ImageEffect>(m_generalGraphPropertyGrid->GetPropertyValueAsInt(GetImageEffectLabel())));
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid,GetWatermarkLabel()))
            {
            m_readabilityProjectDoc->SetWatermark(
                m_generalGraphPropertyGrid->GetPropertyValueAsString(GetWatermarkLabel()));
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid,GetLogoImageLabel()))
            {
            m_readabilityProjectDoc->SetWatermarkLogoPath(
                m_generalGraphPropertyGrid->GetPropertyValueAsString(GetLogoImageLabel()));
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid,GetImageOpacityLabel()))
            {
            m_readabilityProjectDoc->SetGraphBackGroundOpacity(
                static_cast<uint8_t>(m_generalGraphPropertyGrid->GetPropertyValueAsInt(GetImageOpacityLabel())));
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid, GetBackgroundColorFadeLabel()))
            {
            m_readabilityProjectDoc->SetGraphBackGroundLinearGradient(
                m_generalGraphPropertyGrid->GetPropertyValueAsBool(GetBackgroundColorFadeLabel()));
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid,GetColorLabel()))
            {
            m_readabilityProjectDoc->SetPlotBackGroundColor(
                wxAny(m_generalGraphPropertyGrid->GetProperty(GetColorLabel())->GetValue()).As<wxColour>());
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid,GetOpacityLabel()))
            {
            m_readabilityProjectDoc->SetGraphPlotBackGroundOpacity(
                static_cast<uint8_t>(m_generalGraphPropertyGrid->GetPropertyValueAsInt(GetOpacityLabel())));
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid,GetCustomImageBrushLabel()))
            {
            m_readabilityProjectDoc->SetStippleImagePath(
                m_generalGraphPropertyGrid->GetPropertyValueAsString(GetCustomImageBrushLabel()));
            }
        if (IsPropertyAvailable(m_generalGraphPropertyGrid,GetDisplayDropShadowsLabel()))
            {
            m_readabilityProjectDoc->DisplayDropShadows(
                m_generalGraphPropertyGrid->GetPropertyValueAsBool(GetDisplayDropShadowsLabel()));
            }

        if (IsPropertyAvailable(m_barChartPropertyGrid,GetColorLabel()))
            {
            m_readabilityProjectDoc->SetBarChartBarColor(
                wxAny(m_barChartPropertyGrid->GetProperty(GetColorLabel())->GetValue()).As<wxColour>());
            }
        if (IsPropertyAvailable(m_barChartPropertyGrid,GeOrientationLabel()))
            {
            m_readabilityProjectDoc->SetBarChartOrientation(
                static_cast<Wisteria::Orientation>(
                    m_barChartPropertyGrid->GetPropertyValueAsInt(GeOrientationLabel())));
            }
        if (IsPropertyAvailable(m_barChartPropertyGrid,GetOpacityLabel()))
            {
            m_readabilityProjectDoc->SetGraphBarOpacity(
                static_cast<uint8_t>(m_barChartPropertyGrid->GetPropertyValueAsInt(GetOpacityLabel())));
            }
        if (IsPropertyAvailable(m_barChartPropertyGrid,GetEffectLabel()))
            {
            m_readabilityProjectDoc->SetGraphBarEffect(
                static_cast<BoxEffect>(m_barChartPropertyGrid->GetPropertyValueAsInt(GetEffectLabel())));
            }
        if (IsPropertyAvailable(m_barChartPropertyGrid,GetLabelsOnBarsLabel()))
            {
            m_readabilityProjectDoc->DisplayBarLabels(
                m_barChartPropertyGrid->GetPropertyValueAsBool(GetLabelsOnBarsLabel()));
            }

        if (IsPropertyAvailable(m_histogramPropertyGrid,GetBinSortingLabel()))
            {
            m_readabilityProjectDoc->SetHistorgramBinningMethod(
                static_cast<Histogram::BinningMethod>(
                    m_histogramPropertyGrid->GetPropertyValueAsInt(GetBinSortingLabel())));
            }
        if (IsPropertyAvailable(m_histogramPropertyGrid,GetGradeLevelRoundingLabel()))
            {
            m_readabilityProjectDoc->SetHistogramRoundingMethod(
                static_cast<RoundingMethod>(
                    m_histogramPropertyGrid->GetPropertyValueAsInt(GetGradeLevelRoundingLabel())));
            }
        if (IsPropertyAvailable(m_histogramPropertyGrid,GetIntervalDisplayLabel()))
            {
            m_readabilityProjectDoc->SetHistogramIntervalDisplay(
                static_cast<Histogram::IntervalDisplay>(
                    m_histogramPropertyGrid->GetPropertyValueAsInt(GetIntervalDisplayLabel())));
            }
        if (IsPropertyAvailable(m_histogramPropertyGrid,GetBinLabelsLabel()))
            { m_readabilityProjectDoc->SetHistrogramBinLabelDisplay(
                static_cast<BinLabelDisplay>(
                    m_histogramPropertyGrid->GetPropertyValueAsInt(GetBinLabelsLabel())));
            }
        if (IsPropertyAvailable(m_histogramPropertyGrid,GetColorLabel()))
            {
            m_readabilityProjectDoc->SetHistogramBarColor(
                wxAny(m_histogramPropertyGrid->GetProperty(GetColorLabel())->GetValue()).As<wxColour>());
            }
        if (IsPropertyAvailable(m_histogramPropertyGrid,GetOpacityLabel()))
            {
            m_readabilityProjectDoc->SetHistogramBarOpacity(
                static_cast<uint8_t>(m_histogramPropertyGrid->GetPropertyValueAsInt(GetOpacityLabel())));
            }
        if (IsPropertyAvailable(m_histogramPropertyGrid,GetEffectLabel()))
            {
            m_readabilityProjectDoc->SetHistogramBarEffect(
                static_cast<BoxEffect>(m_histogramPropertyGrid->GetPropertyValueAsInt(GetEffectLabel())));
            }

        if (IsPropertyAvailable(m_boxPlotsPropertyGrid,GetColorLabel()))
            {
            m_readabilityProjectDoc->SetGraphBoxColor(
                wxAny(m_boxPlotsPropertyGrid->GetProperty(GetColorLabel())->GetValue()).As<wxColour>());
            }
        if (IsPropertyAvailable(m_boxPlotsPropertyGrid,GetOpacityLabel()))
            {
            m_readabilityProjectDoc->SetGraphBoxOpacity(
                static_cast<uint8_t>(m_boxPlotsPropertyGrid->GetPropertyValueAsInt(GetOpacityLabel())));
            }
        if (IsPropertyAvailable(m_boxPlotsPropertyGrid,GetEffectLabel()))
            {
            m_readabilityProjectDoc->SetGraphBoxEffect(
                static_cast<BoxEffect>(m_boxPlotsPropertyGrid->GetPropertyValueAsInt(GetEffectLabel())));
            }
        if (IsPropertyAvailable(m_boxPlotsPropertyGrid,GetLabelsOnBoxesLabel()))
            {
            m_readabilityProjectDoc->DisplayBoxPlotLabels(
                m_boxPlotsPropertyGrid->GetPropertyValueAsBool(GetLabelsOnBoxesLabel()));
            }
        if (IsPropertyAvailable(m_boxPlotsPropertyGrid,GetConnectBoxesLabel()))
            {
            m_readabilityProjectDoc->ConnectBoxPlotMiddlePoints(
                m_boxPlotsPropertyGrid->GetPropertyValueAsBool(GetConnectBoxesLabel()));
            }
        if (IsPropertyAvailable(m_boxPlotsPropertyGrid,GetShowAllDataPointsLabel()))
            {
            m_readabilityProjectDoc->ShowAllBoxPlotPoints(
                m_boxPlotsPropertyGrid->GetPropertyValueAsBool(GetShowAllDataPointsLabel()));
            }

        m_readabilityProjectDoc->SetXAxisFont(m_xAxisFont);
        m_readabilityProjectDoc->SetXAxisFontColor(m_xAxisFontColor);
        m_readabilityProjectDoc->SetYAxisFont(m_yAxisFont);
        m_readabilityProjectDoc->SetYAxisFontColor(m_yAxisFontColor);
        m_readabilityProjectDoc->SetTopTitleGraphFont(m_topTitleFont);
        m_readabilityProjectDoc->SetTopTitleGraphFontColor(m_topTitleFontColor);
        m_readabilityProjectDoc->SetBottomTitleGraphFont(m_bottomTitleFont);
        m_readabilityProjectDoc->SetBottomTitleGraphFontColor(m_bottomTitleFontColor);
        m_readabilityProjectDoc->SetLeftTitleGraphFont(m_leftTitleFont);
        m_readabilityProjectDoc->SetLeftTitleGraphFontColor(m_leftTitleFontColor);
        m_readabilityProjectDoc->SetRightTitleGraphFont(m_rightTitleFont);
        m_readabilityProjectDoc->SetRightTitleGraphFontColor(m_rightTitleFontColor);
        if (IsPropertyAvailable(m_readabilityGraphPropertyGrid, GetInvalidRegionsColorLabel()))
            {
            m_readabilityProjectDoc->SetInvalidAreaColor(
                wxAny(m_readabilityGraphPropertyGrid->GetProperty(
                    GetInvalidRegionsColorLabel())->GetValue()).As<wxColour>());
            }
        if (IsPropertyAvailable(m_readabilityGraphPropertyGrid, GetRaygorStyleLabel()))
            {
            m_readabilityProjectDoc->SetRaygorStyle(
                static_cast<Wisteria::Graphs::RaygorStyle>(
                    m_readabilityGraphPropertyGrid->GetPropertyValueAsInt(GetRaygorStyleLabel())) );
            }
        if (IsPropertyAvailable(m_readabilityGraphPropertyGrid,GetUseEnglishLabelsForGermanLixLabel()))
            {
            m_readabilityProjectDoc->UseEnglishLabelsForGermanLix(
                m_readabilityGraphPropertyGrid->GetPropertyValueAsBool(GetUseEnglishLabelsForGermanLixLabel()));
            }
        if (IsPropertyAvailable(m_readabilityGraphPropertyGrid,GetFleschChartConnectPointsLabel()))
            {
            m_readabilityProjectDoc->ConnectFleschPoints(
                m_readabilityGraphPropertyGrid->GetPropertyValueAsBool(GetFleschChartConnectPointsLabel()));
            }
        if (IsPropertyAvailable(m_readabilityGraphPropertyGrid,GetFleschSyllableRulerDocGroupsLabel()))
            {
            m_readabilityProjectDoc->IncludeFleschRulerDocGroups(
                m_readabilityGraphPropertyGrid->GetPropertyValueAsBool(GetFleschSyllableRulerDocGroupsLabel()));
            }

        m_readabilityProjectDoc->RefreshRequired(ProjectRefresh::Minimal);
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnOK([[maybe_unused]] wxCommandEvent& event)
    {
    if (!ValidateOptions())
        { return; }

    SaveOptions();

    if (IsModal())
        { EndModal(wxID_OK); }
    else
        { Show(false); }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnExportSettings([[maybe_unused]] wxCommandEvent& event)
    {
    if (!ValidateOptions())
        { return; }

    if (HaveOptionsChanged())
        {
        if (wxMessageBox(
                _(L"Some of your settings have changed but have not been saved yet.\n"
                   "Do you wish to save these changes before exporting?"),
                _(L"Loading Settings"), wxYES_NO|wxYES_DEFAULT|wxICON_QUESTION) == wxYES)
            { SaveOptions(); }
        }
    wxFileDialog dialog(this,
                    _(L"Export Settings File"),
                    wxEmptyString,
                    L"Settings.xml",
                    _(L"Readability Studio Settings Files (*.xml)|*.xml"),
                    wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
    if (dialog.ShowModal() != wxID_OK)
        { return; }

    wxGetApp().GetAppOptions().SaveOptionsFile(dialog.GetPath());
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnImportSettings([[maybe_unused]] wxCommandEvent& event)
    {
    wxFileDialog dialog
            (
            this,
            _(L"Import Settings File"),
            wxEmptyString,
            L"Settings.xml",
            _(L"Readability Studio Settings Files (*.xml)|*.xml"),
            wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_PREVIEW);
    if (dialog.ShowModal() != wxID_OK)
        { return; }

    wxMessageBox(_(L"Settings from the selected file will now be applied."),
                _(L"Loading Settings"), wxOK|wxICON_INFORMATION);
    wxGetApp().GetAppOptions().LoadOptionsFile(dialog.GetPath(), false, false);
    Close();
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnResetSettings([[maybe_unused]] wxCommandEvent& event)
    {
    if (wxMessageBox(_(L"Do you wish to reset all of your program and user settings?"),
                _(L"Reset Settings"), wxYES_NO|wxYES_DEFAULT|wxICON_QUESTION) == wxYES)
        {
        wxGetApp().GetAppOptions().ResetSettings();
        Close();
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::CreateControls()
    {
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    if (GetSectionsBeingShown() == TextSection ||
        GetSectionsBeingShown() == GraphsSection ||
        GetSectionsBeingShown() == Statistics)
        { mainSizer->SetMinSize(FromDIP(wxSize(500, 600))); }
    else
        { mainSizer->SetMinSize(FromDIP(wxSize(650, 600))); }

    const int OPTION_INDENT_SIZE = wxSizerFlags::GetDefaultBorder()*3;

    m_sideBar = new SideBarBook(this, wxID_ANY);
    mainSizer->Add(m_sideBar, 1, wxEXPAND|wxALL, wxSizerFlags::GetDefaultBorder());

    wxGetApp().UpdateSideBarTheme(m_sideBar->GetSideBar());

    const auto fontImage = wxArtProvider::GetBitmapBundle(L"ID_FONT", wxART_BUTTON);
    const auto openImage = wxArtProvider::GetBitmapBundle(wxART_FILE_OPEN, wxART_BUTTON);

    if (GetSectionsBeingShown() == AllSections)
        {
        if (IsGeneralSettings())
            {
            wxPanel* generalSettingsPage =
                new wxPanel(m_sideBar, GENERAL_SETTINGS_PAGE, wxDefaultPosition,
                            wxDefaultSize, wxTAB_TRAVERSAL);
            wxBoxSizer* docpanelSizer = new wxBoxSizer(wxVERTICAL);
            generalSettingsPage->SetSizer(docpanelSizer);
            m_sideBar->AddPage(generalSettingsPage, GetGeneralSettingsLabel(),
                GENERAL_SETTINGS_PAGE, true, 10);

            CreateLabelHeader(generalSettingsPage, docpanelSizer, _(L"Settings:"), true);

            wxSizer* optionsSizer = new wxBoxSizer(wxVERTICAL);
            docpanelSizer->Add(optionsSizer, 0, wxEXPAND|wxLEFT, OPTION_INDENT_SIZE);

            wxButton* loadSettingsButton =
                new wxButton(generalSettingsPage, ID_LOAD_SETTINGS_BUTTON, _(L"Import..."));
            optionsSizer->Add(loadSettingsButton, 0, wxALIGN_LEFT|wxTOP|wxBOTTOM, wxSizerFlags::GetDefaultBorder());

            wxButton* exportSettingsButton =
                new wxButton(generalSettingsPage, ID_EXPORT_SETTINGS_BUTTON, _(L"Export..."));
            optionsSizer->Add(exportSettingsButton, 0, wxALIGN_LEFT|wxTOP|wxBOTTOM, wxSizerFlags::GetDefaultBorder());

            wxButton* resetSettingsButton =
                new wxButton(generalSettingsPage, ID_RESET_SETTINGS_BUTTON, _(L"Reset"));
            optionsSizer->Add(resetSettingsButton, 0, wxALIGN_LEFT|wxTOP|wxBOTTOM, wxSizerFlags::GetDefaultBorder());

        #ifndef __WXOSX__
            CreateLabelHeader(generalSettingsPage, docpanelSizer, _(L"Themes:"), true);

            optionsSizer = new wxBoxSizer(wxVERTICAL);
            docpanelSizer->Add(optionsSizer, 0, wxEXPAND|wxLEFT, OPTION_INDENT_SIZE);

            // load the list of themes
            wxDir dir;
            wxArrayString files;
            const wxString themesFolder = wxGetApp().FindResourceDirectory(_DT(L"Themes"));
            dir.GetAllFiles(themesFolder, &files, wxEmptyString, wxDIR_FILES);
            for (auto& theme : files)
                { theme = wxFileName(theme).GetName(); }
            files.Add(_(L"System")); // OK to translate
            files.Sort();

            auto themeCombo = new wxComboBox(generalSettingsPage, ID_THEME_COMBO, wxEmptyString,
                                    wxDefaultPosition, wxDefaultSize, 0, nullptr,
                                    wxCB_DROPDOWN|wxCB_READONLY);
            optionsSizer->Add(themeCombo, 0, wxALIGN_LEFT|wxTOP|wxBOTTOM, wxSizerFlags::GetDefaultBorder());
            for (const auto& theme : files)
                { themeCombo->Append(theme); }
            // if theme loaded is "System" or something not in the list of theme files
            // that we loaded, then choose system
            if (wxGetApp().GetAppOptions().GetTheme() == _DT(L"System") /*should not internally be translated*/ ||
                std::find(files.begin(), files.end(), wxGetApp().GetAppOptions().GetTheme()) == files.end())
                { themeCombo->SetStringSelection(_(L"System")); }
            else
                { themeCombo->SetStringSelection(wxGetApp().GetAppOptions().GetTheme()); }

            Bind(wxEVT_COMBOBOX,
                [this](wxCommandEvent& evt)
                    {
                    const wxString themePath = wxGetApp().FindResourceDirectory(_DT(L"Themes")) +
                            wxFileName::GetPathSeparator() + evt.GetString() + L".xml";
                    if (wxFileName::FileExists(themePath))
                        {
                        wxGetApp().GetAppOptions().SetTheme(evt.GetString());
                        wxGetApp().GetAppOptions().LoadThemeFile(themePath);
                        }
                    // either "System" or file is missing (or translated "System" is selected),
                    // so reset to system
                    else
                        {
                        wxGetApp().GetAppOptions().SetTheme(_DT(L"System"));
                        wxGetApp().GetAppOptions().SetColorsFromSystem();
                        }
                    wxGetApp().UpdateStartPageTheme();
                    wxGetApp().UpdateScriptEditorTheme();
                    wxGetApp().UpdateRibbonTheme();
                    wxGetApp().UpdateDocumentThemes();
                    wxGetApp().GetMainFrame()->Refresh();
                    wxGetApp().UpdateSideBarTheme(m_sideBar->GetSideBar());
                    Refresh();
                    },
                ID_THEME_COMBO);
        #endif

            CreateLabelHeader(generalSettingsPage, docpanelSizer, _(L"Internet:"), true);

            optionsSizer = new wxBoxSizer(wxVERTICAL);
            docpanelSizer->Add(optionsSizer, 0, wxEXPAND | wxLEFT, OPTION_INDENT_SIZE);

            wxBoxSizer* userAgentSizer = new wxBoxSizer(wxHORIZONTAL);
            optionsSizer->Add(userAgentSizer, wxSizerFlags().Expand());

            userAgentSizer->Add(
                new wxStaticText(generalSettingsPage, wxID_STATIC, _(L"User agent:")),
                0, wxALIGN_CENTER_VERTICAL);
            wxTextCtrl* userAgentEdit =
                new wxTextCtrl(generalSettingsPage, wxID_ANY, wxString{}, wxDefaultPosition,
                    wxDefaultSize, wxBORDER_THEME, wxGenericValidator(&m_userAgent));
            userAgentSizer->Add(userAgentEdit,
                wxSizerFlags(1).Expand().Border(wxLEFT, wxSizerFlags::GetDefaultBorder()));
            optionsSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

            CreateLabelHeader(generalSettingsPage, docpanelSizer, _(L"Warnings && Prompts:"), true);

            optionsSizer = new wxBoxSizer(wxVERTICAL);
            docpanelSizer->Add(optionsSizer, 0, wxEXPAND|wxLEFT, OPTION_INDENT_SIZE);

            wxButton* warningsButton =
                new wxButton(generalSettingsPage, ID_WARNING_MESSAGES_BUTTON, _(L"Customize..."));
            optionsSizer->Add(warningsButton, 0, wxALIGN_LEFT|wxTOP|wxBOTTOM, wxSizerFlags::GetDefaultBorder());
            }
        }

    // Project settings
    if (GetSectionsBeingShown() & ProjectSection)
        {
        wxPanel* projectSettingsPage =
            new wxPanel(m_sideBar, PROJECT_SETTINGS_PAGE, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
        wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);
        m_sideBar->AddPage(projectSettingsPage, GetProjectSettingsLabel(), PROJECT_SETTINGS_PAGE,
            // if the only section being shown, then show this page
            (GetSectionsBeingShown() == ProjectSection),
            11);

        // project properties
        CreateLabelHeader(projectSettingsPage, panelSizer, _(L"Project:"), true);

        wxFlexGridSizer* projectExtraInfoSizer = new wxFlexGridSizer(2, 5, 5);
        projectExtraInfoSizer->Add(
            new wxStaticText(projectSettingsPage, wxID_STATIC, _(L"Reviewer:")), 0, wxALIGN_CENTER_VERTICAL);
        wxTextCtrl* reviewerEdit =
            new wxTextCtrl(projectSettingsPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                           wxBORDER_THEME, wxGenericValidator(&m_reviewer) );
        projectExtraInfoSizer->Add(reviewerEdit, 1, wxEXPAND);

        if (!IsGeneralSettings())
            {
            projectExtraInfoSizer->Add(
                new wxStaticText(projectSettingsPage, wxID_STATIC, _(L"Status:")), 0, wxALIGN_CENTER_VERTICAL);
            wxTextCtrl* statusEdit =
                new wxTextCtrl(projectSettingsPage, wxID_ANY, wxEmptyString, wxDefaultPosition,
                               wxDefaultSize, wxBORDER_THEME, wxGenericValidator(&m_status) );
            projectExtraInfoSizer->Add(statusEdit, 1, wxEXPAND);
            }

        panelSizer->Add(projectExtraInfoSizer, 0, wxEXPAND|wxLEFT, OPTION_INDENT_SIZE);

        panelSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

        if ((IsGeneralSettings() &&
            wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureProfessionalCode())) ||
            IsBatchProjectSettings())
            {
            panelSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

            wxStaticBoxSizer* batchOptionsBox =
                new wxStaticBoxSizer(wxVERTICAL, projectSettingsPage, _(L"Batch options"));

            wxBoxSizer* minDocSizeBoxSizer = new wxBoxSizer(wxHORIZONTAL);
            batchOptionsBox->Add(minDocSizeBoxSizer, 0, wxEXPAND|wxLEFT, wxSizerFlags::GetDefaultBorder());

            wxStaticText* minDocSizeLabel =
                new wxStaticText(batchOptionsBox->GetStaticBox(), wxID_STATIC, _(L"Minimum document word count:"),
                    wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
            minDocSizeBoxSizer->Add(minDocSizeLabel, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT,
                                    wxSizerFlags::GetDefaultBorder());

            wxSpinCtrl* minDocWordCountForBatchSpinCtrl =
                new wxSpinCtrl(batchOptionsBox->GetStaticBox(), wxID_ANY,
                               std::to_wstring(m_minDocWordCountForBatch.get_value()),
                               wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1,
                               std::numeric_limits<int>::max(), 0);
            minDocWordCountForBatchSpinCtrl->SetValidator(wxGenericValidator(&m_minDocWordCountForBatch));
            minDocSizeBoxSizer->Add(minDocWordCountForBatchSpinCtrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0);

            wxBoxSizer* fileTruncSizer = new wxBoxSizer(wxHORIZONTAL);
            fileTruncSizer->Add(new wxStaticText(batchOptionsBox->GetStaticBox(),
                                                 wxID_STATIC, _(L"File path display:")), 0,
                                                 wxALIGN_CENTER_VERTICAL|wxRIGHT, wxSizerFlags::GetDefaultBorder());

            wxArrayString truncModes;
            truncModes.Add(_(L"Partially truncate the file path"));
            truncModes.Add(_(L"Show only the file name"));
            truncModes.Add(_(L"Show the full file path"));
            wxChoice* fileTruncCombo = new wxChoice(batchOptionsBox->GetStaticBox(), wxID_ANY,
                                                wxDefaultPosition, wxDefaultSize, truncModes,
                                                0, wxGenericValidator(&m_filePathTruncationMode));
            fileTruncSizer->Add(fileTruncCombo, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, wxSizerFlags::GetDefaultBorder());
            batchOptionsBox->AddSpacer(wxSizerFlags::GetDefaultBorder());
            batchOptionsBox->Add(fileTruncSizer, 0, wxEXPAND|wxLEFT, wxSizerFlags::GetDefaultBorder());

            panelSizer->Add(batchOptionsBox, 0, wxLEFT, OPTION_INDENT_SIZE);
            }

        panelSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

        CreateLabelHeader(projectSettingsPage, panelSizer, _(L"Document:"), true);

        if (wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureLanguagePackCode()) ||
            wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureProfessionalCode()))
            {
            wxArrayString languages;
            languages.Add(_(L"English"));
            languages.Add(_(L"Spanish"));
            languages.Add(_(L"German"));

            wxBoxSizer* langSizer = new wxBoxSizer(wxHORIZONTAL);
            wxStaticText* langLabel = new wxStaticText(projectSettingsPage, wxID_STATIC, _(L"Language:"),
                        wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
            langSizer->Add(langLabel, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT);
            langSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
            langSizer->Add(new wxChoice(projectSettingsPage, wxID_ANY,
                                        wxDefaultPosition, wxDefaultSize, languages, 0,
                                        wxGenericValidator(&m_projectLanguage)));
            panelSizer->Add(langSizer, 0, wxEXPAND|wxLEFT, OPTION_INDENT_SIZE);
            panelSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
            }

        // if Tools/Options dialog, batch project, or standard project that got its text from a file-based document
        if (IsGeneralSettings() ||
            IsBatchProjectSettings() ||
            (m_readabilityProjectDoc->GetTextSource() == TextSource::FromFile))
            {
            // document storage/linking section
            wxArrayString docLinking;
            docLinking.Add(_(L"&Embed the document's text into the project"));
            docLinking.Add(_(L"&Reload the document when opening project"));
            wxRadioBox* docStorageRadioBox =
                new wxRadioBox(projectSettingsPage, ID_DOCUMENT_STORAGE_RADIO_BOX, _(L"Linking and embedding"),
                               wxDefaultPosition, wxDefaultSize, docLinking, 0, wxRA_SPECIFY_ROWS,
                               wxGenericValidator(&m_documentStorageMethod) );
            panelSizer->Add(docStorageRadioBox, 0, wxLEFT, OPTION_INDENT_SIZE);
            panelSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
            }

        if (IsStandardProjectSettings() &&
            (m_readabilityProjectDoc->GetTextSource() == TextSource::FromFile))
            {
            wxBoxSizer* fileBrowseBoxSizer = new wxBoxSizer(wxHORIZONTAL);
            panelSizer->Add(fileBrowseBoxSizer, 0, wxEXPAND|wxLEFT, OPTION_INDENT_SIZE);

            m_filePathEdit = new wxTextCtrl(projectSettingsPage, wxID_ANY, wxEmptyString,
                                            wxDefaultPosition, wxDefaultSize, wxBORDER_THEME,
                                            wxGenericValidator(&m_filePath) );
            m_filePathEdit->AutoCompleteFileNames();
            fileBrowseBoxSizer->Add(m_filePathEdit, 1, wxEXPAND);

            m_fileBrowseButton = new wxBitmapButton(projectSettingsPage, ID_FILE_BROWSE_BUTTON, openImage);
            fileBrowseBoxSizer->Add(m_fileBrowseButton, 0, wxALIGN_CENTER_VERTICAL);
            // only enable these if document linking is selected
            m_filePathEdit->Enable(m_documentStorageMethod == static_cast<int>(TextStorage::NoEmbedText));
            m_fileBrowseButton->Enable(m_documentStorageMethod == static_cast<int>(TextStorage::NoEmbedText));

            panelSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

            wxBoxSizer* docDescriptionSizer = new wxBoxSizer(wxHORIZONTAL);
            panelSizer->Add(docDescriptionSizer, wxSizerFlags().Expand().Border(wxLEFT, OPTION_INDENT_SIZE));

            docDescriptionSizer->Add(
                new wxStaticText(projectSettingsPage, wxID_STATIC, _(L"Document description:")),
                                 0, wxALIGN_CENTER_VERTICAL);
            wxTextCtrl* descriptionEdit =
                new wxTextCtrl(projectSettingsPage, wxID_ANY, wxEmptyString, wxDefaultPosition,
                               wxDefaultSize, wxBORDER_THEME, wxGenericValidator(&m_description) );
            docDescriptionSizer->Add(descriptionEdit,
                                     wxSizerFlags(1).Expand().Border(wxLEFT, wxSizerFlags::GetDefaultBorder()));
            }
        else if (IsBatchProjectSettings())
            {
            wxBoxSizer* filesSizer = new wxBoxSizer(wxVERTICAL);
            wxBoxSizer* filesButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
            // add files (uses file browser)
            m_addFilesButton = new wxBitmapButton(projectSettingsPage, ID_ADD_FILES_BUTTON,
                wxArtProvider::GetBitmapBundle(L"ID_DOCUMENTS", wxART_BUTTON));
            m_addFilesButton->SetToolTip(_(L"Browse for document(s) to add"));
            m_addFilesButton->Enable(m_documentStorageMethod == static_cast<int>(TextStorage::NoEmbedText));
            filesButtonsSizer->Add(m_addFilesButton);

            // add row to file list
            m_addFileButton = new wxBitmapButton(projectSettingsPage, ID_ADD_FILE_BUTTON,
                wxArtProvider::GetBitmapBundle(L"ID_ADD", wxART_BUTTON));
            m_addFileButton->SetToolTip(_(L"Enter a document"));
            m_addFileButton->Enable(m_documentStorageMethod == static_cast<int>(TextStorage::NoEmbedText));
            filesButtonsSizer->Add(m_addFileButton);

            m_deleteFileButton = new wxBitmapButton(projectSettingsPage, ID_DELETE_FILE_BUTTON,
                wxArtProvider::GetBitmapBundle(wxART_DELETE, wxART_BUTTON));
            m_deleteFileButton->SetToolTip(_(L"Remove selected document"));
            m_deleteFileButton->Enable(m_documentStorageMethod == static_cast<int>(TextStorage::NoEmbedText));
            filesButtonsSizer->Add(m_deleteFileButton);
            filesSizer->Add(filesButtonsSizer, 0, wxALIGN_RIGHT);

            m_fileData->SetSize(m_readabilityProjectDoc->GetSourceFilesInfo().size(), 2);
            // fill the file info grid
            for (size_t i = 0; i < m_readabilityProjectDoc->GetSourceFilesInfo().size(); ++i)
                {
                m_fileData->SetItemText(i, 0, m_readabilityProjectDoc->GetSourceFilesInfo()[i].first);
                m_fileData->SetItemText(i, 1, m_readabilityProjectDoc->GetSourceFilesInfo()[i].second);
                }
            m_fileList = new ListCtrlEx(projectSettingsPage, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                        wxLC_VIRTUAL|wxLC_EDIT_LABELS|wxLC_REPORT|wxLC_ALIGN_LEFT);
            m_fileList->EnableGridLines();
            m_fileList->EnableItemDeletion();
            m_fileList->InsertColumn(0, _(L"Files"));
            m_fileList->InsertColumn(1, _(L"Labels"));
            m_fileList->SetColumnEditable(0);
            m_fileList->SetColumnEditable(1);
            m_fileList->SetVirtualDataProvider(m_fileData);
            m_fileList->SetVirtualDataSize(m_fileData->GetItemCount());
            m_fileList->Enable(m_documentStorageMethod == static_cast<int>(TextStorage::NoEmbedText));
            filesSizer->Add(m_fileList, 1, wxEXPAND|wxLEFT, OPTION_INDENT_SIZE);

            panelSizer->Add(filesSizer, 1, wxEXPAND);
            }

        panelSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

        // additional template file to append
            {
            CreateLabelHeader(projectSettingsPage, panelSizer,
                              _(L"Append Additional Document (e.g., policies, license agreements, addendums):"), true);

            wxBoxSizer* fileBrowseBoxSizer = new wxBoxSizer(wxHORIZONTAL);
            panelSizer->Add(fileBrowseBoxSizer, 0, wxEXPAND|wxLEFT, OPTION_INDENT_SIZE);

            wxTextCtrl* filePathEdit =
                new wxTextCtrl(projectSettingsPage, ID_ADDITIONAL_FILE_FIELD, wxEmptyString, wxDefaultPosition,
                               wxDefaultSize, wxBORDER_THEME, wxGenericValidator(&m_appendedDocumentFilePath) );
            filePathEdit->AutoCompleteFileNames();
            fileBrowseBoxSizer->Add(filePathEdit, 1, wxEXPAND);

            fileBrowseBoxSizer->Add(
                new wxBitmapButton(projectSettingsPage, ID_ADDITIONAL_FILE_BROWSE_BUTTON, openImage), 0,
                                   wxALIGN_CENTER_VERTICAL);
            }

        projectSettingsPage->SetSizer(panelSizer);
        }

    // Document parsing page
    if (GetSectionsBeingShown() & DocumentIndexing)
        {
        wxPanel* AnalysisIndexingPage =
            new wxPanel(m_sideBar, ANALYSIS_INDEXING_PAGE, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
        wxRadioButton* radioButton = nullptr;

        wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);
        AnalysisIndexingPage->SetSizer(panelSizer);
        m_sideBar->AddPage(AnalysisIndexingPage, _(L"Document Indexing"), ANALYSIS_INDEXING_PAGE,
            // if the only section being shown, then show this page
            (GetSectionsBeingShown() == DocumentIndexing),
            12);

        // long sentence section
        CreateLabelHeader(AnalysisIndexingPage, panelSizer, _(L"Consider Sentences Overly Long If:"), true);

        wxBoxSizer* optionsIndentSizer = new wxBoxSizer(wxVERTICAL);
        panelSizer->Add(optionsIndentSizer, 0, wxEXPAND|wxLEFT|wxRIGHT, OPTION_INDENT_SIZE);
        wxBoxSizer* longerThanWordSizer = new wxBoxSizer(wxHORIZONTAL);
        optionsIndentSizer->Add(longerThanWordSizer, 0, wxTOP|wxBOTTOM, wxSizerFlags::GetDefaultBorder());
        radioButton =
            new wxRadioButton(AnalysisIndexingPage, ID_SENTENCE_LONGER_THAN_BUTTON, _(L"&Longer than"),
                              wxDefaultPosition, wxDefaultSize, wxRB_GROUP,
                              wxGenericValidator(&m_longSentencesNumberOfWords));
        longerThanWordSizer->Add(radioButton, 0, wxALIGN_CENTER_VERTICAL, 0);
        // the spin control for the number of words
        wxSpinCtrl* wordsPerLongSentenceSpinCtrl =
            new wxSpinCtrl(AnalysisIndexingPage, wxID_ANY, std::to_wstring(m_sentenceLength.get_value()),
                           wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, std::numeric_limits<int>::max(), 0);
        wordsPerLongSentenceSpinCtrl->SetValidator(wxGenericValidator(&m_sentenceLength));
        longerThanWordSizer->Add(wordsPerLongSentenceSpinCtrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0);
        //"words" after it
        wxStaticText* wordsLabel =
            new wxStaticText(AnalysisIndexingPage, wxID_STATIC, _(L"words"), wxDefaultPosition, wxDefaultSize, 0);
        longerThanWordSizer->Add(wordsLabel, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT,
                                 wxSizerFlags::GetDefaultBorder());

        radioButton = new wxRadioButton(AnalysisIndexingPage, ID_SENTENCE_OUTLIER_LENGTH_BUTTON,
                                        _(L"Out&side sentence-length outlier range"), wxDefaultPosition,
                                        wxDefaultSize, 0, wxGenericValidator(&m_longSentencesOutliers));
        optionsIndentSizer->Add(radioButton, 0, wxEXPAND, wxSizerFlags::GetDefaultBorder());
        panelSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

        // sentence/paragraph deduction
        CreateLabelHeader(AnalysisIndexingPage, panelSizer, _(L"Sentence/Paragraph Deduction:"), true);
        optionsIndentSizer = new wxBoxSizer(wxVERTICAL);
        panelSizer->Add(optionsIndentSizer, 0, wxEXPAND|wxLEFT, OPTION_INDENT_SIZE);

        wxBoxSizer* lineEndsSizer = new wxBoxSizer(wxHORIZONTAL);
        lineEndsSizer->Add(
            new wxStaticText(AnalysisIndexingPage, wxID_STATIC, _(L"Line ends:")), 0,
                             wxALIGN_CENTER_VERTICAL|wxRIGHT, wxSizerFlags::GetDefaultBorder());

        wxArrayString paragraphParseOptions;
        paragraphParseOptions.Add(_(L"only begin a new paragraph if following a valid sentence"));
        paragraphParseOptions.Add(_(L"always begin a new paragraph"));
        wxChoice* paragraphParseCombo = new wxChoice(AnalysisIndexingPage, ID_PARAGRAPH_PARSE,
                                            wxDefaultPosition, wxDefaultSize, paragraphParseOptions,
                                            0, wxGenericValidator(&m_paragraphParsingMethod));
        lineEndsSizer->Add(paragraphParseCombo, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, wxSizerFlags::GetDefaultBorder());
        optionsIndentSizer->Add(lineEndsSizer, 0, wxEXPAND|wxBOTTOM, wxSizerFlags::GetDefaultBorder());

        m_ignoreBlankLinesCheckBox =
            new wxCheckBox(AnalysisIndexingPage, ID_IGNORE_BLANK_LINES_BUTTON, _(L"Ignore &blank lines"),
                           wxDefaultPosition, wxDefaultSize, 0,
                           wxGenericValidator(&m_ignoreBlankLinesForParagraphsParser) );
        m_ignoreBlankLinesCheckBox->Enable(
            static_cast<ParagraphParse>(m_paragraphParsingMethod.get_value()) ==
                ParagraphParse::OnlySentenceTerminatedNewLinesAreParagraphs);
        optionsIndentSizer->Add(m_ignoreBlankLinesCheckBox, 0, wxEXPAND|wxBOTTOM, wxSizerFlags::GetDefaultBorder());

        m_ignoreIndentingCheckBox =
            new wxCheckBox(AnalysisIndexingPage, ID_IGNORE_INDENTING_BUTTON, _(L"&Ignore indenting"),
                           wxDefaultPosition, wxDefaultSize, 0,
                           wxGenericValidator(&m_ignoreIndentingForParagraphsParser) );
        m_ignoreIndentingCheckBox->Enable(
            static_cast<ParagraphParse>(m_paragraphParsingMethod.get_value()) ==
                ParagraphParse::OnlySentenceTerminatedNewLinesAreParagraphs);
        optionsIndentSizer->Add(m_ignoreIndentingCheckBox, 0, wxEXPAND|wxBOTTOM, wxSizerFlags::GetDefaultBorder());

        wxCheckBox* sentenceStartMustBeUppercasedCheckBox =
            new wxCheckBox(AnalysisIndexingPage, wxID_ANY, _(L"Sentences must begin with capitalized words"),
                           wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_sentenceStartMustBeUppercased) );
        optionsIndentSizer->Add(sentenceStartMustBeUppercasedCheckBox, 0, wxEXPAND, wxSizerFlags::GetDefaultBorder());
        panelSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

        // incomplete sentence logic
        CreateLabelHeader(AnalysisIndexingPage, panelSizer, _(L"Text Exclusion:"), true);
        optionsIndentSizer = new wxBoxSizer(wxVERTICAL);
        panelSizer->Add(optionsIndentSizer, 0, wxEXPAND|wxLEFT, OPTION_INDENT_SIZE);

        wxBoxSizer* exclusionSizer = new wxBoxSizer(wxHORIZONTAL);
        exclusionSizer->Add(
            new wxStaticText(AnalysisIndexingPage, wxID_STATIC, _(L"Exclusion:")), 0,
                             wxALIGN_CENTER_VERTICAL|wxRIGHT, wxSizerFlags::GetDefaultBorder());

        wxArrayString exclusionOptions;
        exclusionOptions.Add(_(L"Exclude all incomplete sentences"));
        exclusionOptions.Add(_(L"Do not exclude any text"));
        exclusionOptions.Add(_(L"Exclude all incomplete sentences, except headings"));
        wxChoice* exclusionCombo = new wxChoice(AnalysisIndexingPage, ID_TEXT_EXCLUDE_METHOD,
                                            wxDefaultPosition, wxDefaultSize, exclusionOptions,
                                            0, wxGenericValidator(&m_textExclusionMethod));
        exclusionSizer->Add(exclusionCombo, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, wxSizerFlags::GetDefaultBorder());
        optionsIndentSizer->Add(exclusionSizer, 0, wxEXPAND|wxBOTTOM, wxSizerFlags::GetDefaultBorder());

        wxBoxSizer* includeIncompleteSentSizeSizer = new wxBoxSizer(wxHORIZONTAL);
        optionsIndentSizer->Add(includeIncompleteSentSizeSizer, 0, wxBOTTOM, wxSizerFlags::GetDefaultBorder());
        m_includeIncompleteSentSizeincludeIncompleteLabel =
            new wxStaticText(AnalysisIndexingPage, ID_INCOMPLETE_SENTENCE_VALID_LABEL_START,
                _(L"Include incomplete sentences containing more than"), wxDefaultPosition, wxDefaultSize, 0);
        includeIncompleteSentSizeSizer->Add(
            m_includeIncompleteSentSizeincludeIncompleteLabel, 0, wxALIGN_CENTER_VERTICAL);
        // the spin control for the number of words
        wxSpinCtrl* includeIncompleteSentencesIfLongerThanSpinCtrl =
            new wxSpinCtrl(AnalysisIndexingPage, ID_INCOMPLETE_SENTENCE_VALID_VALUE_BOX,
                std::to_wstring(m_includeIncompleteSentencesIfLongerThan.get_value()),
                wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, std::numeric_limits<int>::max(), 0);
        includeIncompleteSentencesIfLongerThanSpinCtrl->SetValidator(
            wxGenericValidator(&m_includeIncompleteSentencesIfLongerThan));
        includeIncompleteSentSizeSizer->Add(includeIncompleteSentencesIfLongerThanSpinCtrl, 0,
            wxLEFT|wxRIGHT, wxSizerFlags::GetDefaultBorder());
        //"words" after it
        m_includeIncompleteSentSizeWordsLabel =
            new wxStaticText(AnalysisIndexingPage, ID_INCOMPLETE_SENTENCE_VALID_LABEL_END, _(L"words"),
                wxDefaultPosition, wxDefaultSize, 0);
        includeIncompleteSentSizeSizer->Add(m_includeIncompleteSentSizeWordsLabel, 0, wxALIGN_CENTER_VERTICAL);

        wxFlexGridSizer* ignoreOptionsGrid =
            new wxFlexGridSizer(2, wxSize(wxSizerFlags::GetDefaultBorder(),wxSizerFlags::GetDefaultBorder()));
        optionsIndentSizer->Add(ignoreOptionsGrid,
            wxSizerFlags().Expand().Border(wxBOTTOM, wxSizerFlags::GetDefaultBorder()));

        m_aggressiveExclusionCheckBox =
            new wxCheckBox(AnalysisIndexingPage, ID_AGGRESSIVE_LIST_DEDUCTION_CHECKBOX, _(L"Aggressive exclusion"),
                wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_aggressiveExclusion) );
        m_aggressiveExclusionCheckBox->Enable(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) ||
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings)));
        ignoreOptionsGrid->Add(m_aggressiveExclusionCheckBox);

        m_ignoreFileAddressesCheckBox =
            new wxCheckBox(AnalysisIndexingPage, ID_EXCLUDE_FILE_ADDRESS_CHECKBOX,
                _(L"Also exclude Internet and file addresses"),
                wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_ignoreFileAddresses) );
        m_ignoreFileAddressesCheckBox->Enable(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) ||
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings)));
        ignoreOptionsGrid->Add(m_ignoreFileAddressesCheckBox);

        m_ignoreCopyrightsCheckBox = new wxCheckBox(AnalysisIndexingPage, ID_EXCLUDE_COPYRIGHT_CHECKBOX,
            _(L"Also exclude &copyright notices"),
                wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_ignoreTrailingCopyrightNoticeParagraphs) );
        m_ignoreCopyrightsCheckBox->Enable(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) ||
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings)));
        ignoreOptionsGrid->Add(m_ignoreCopyrightsCheckBox);

        m_ignoreNumeralsCheckBox =
            new wxCheckBox(AnalysisIndexingPage, ID_EXCLUDE_NUMERALS_CHECKBOX, _(L"Also exclude numerals"),
                wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_ignoreNumerals) );
        m_ignoreNumeralsCheckBox->Enable(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) ||
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings)));
        ignoreOptionsGrid->Add(m_ignoreNumeralsCheckBox);

        m_ignoreCitationsCheckBox =
            new wxCheckBox(AnalysisIndexingPage, ID_EXCLUDE_CITATIONS_CHECKBOX, _(L"Also exclude trailing citations"),
                wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_ignoreTrailingCitations) );
        m_ignoreCitationsCheckBox->Enable(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) ||
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings)));
        ignoreOptionsGrid->Add(m_ignoreCitationsCheckBox);

        m_ignoreProperNounsCheckBox =
            new wxCheckBox(AnalysisIndexingPage, ID_EXCLUDE_PROPER_NOUNS_CHECKBOX, _(L"Also exclude proper nouns"),
                wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_ignoreProperNouns) );
        m_ignoreProperNounsCheckBox->Enable(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) ||
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings)));
        ignoreOptionsGrid->Add(m_ignoreProperNounsCheckBox);

        // excluded phrases
        wxStaticBoxSizer* excludedPhrasesFileBrowseBoxSizer =
            new wxStaticBoxSizer(wxVERTICAL, AnalysisIndexingPage, _(L"Words && phrases to exclude:"));

        wxBoxSizer* excludePathSizer = new wxBoxSizer(wxHORIZONTAL);
        excludedPhrasesFileBrowseBoxSizer->Add(excludePathSizer, 1, wxEXPAND);

        m_excludedPhrasesPathFilePathEdit =
            new wxTextCtrl(excludedPhrasesFileBrowseBoxSizer->GetStaticBox(), wxID_ANY, wxEmptyString,
                           wxDefaultPosition, wxDefaultSize, wxBORDER_THEME,
                           wxGenericValidator(&m_excludedPhrasesPath) );
        m_excludedPhrasesPathFilePathEdit->AutoCompleteFileNames();
        excludePathSizer->Add(m_excludedPhrasesPathFilePathEdit, 1, wxEXPAND);

        m_excludedPhrasesEditBrowseButton = new wxBitmapButton(excludedPhrasesFileBrowseBoxSizer->GetStaticBox(),
            ID_EXCLUDED_PHRASES_FILE_EDIT_BUTTON,
            wxArtProvider::GetBitmapBundle(L"ID_EDIT", wxART_BUTTON));
        excludePathSizer->Add(m_excludedPhrasesEditBrowseButton, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT,
                              wxSizerFlags::GetDefaultBorder());

        m_includeExcludedPhraseFirstOccurrenceCheckBox =
            new wxCheckBox(excludedPhrasesFileBrowseBoxSizer->GetStaticBox(),
                ID_INCLUDE_FIRST_OCCURRENCE_EXCLUDE_CHECKBOX, _(L"Include first occurrence"),
                wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_includeExcludedPhraseFirstOccurrence) );
        excludedPhrasesFileBrowseBoxSizer->Add(m_includeExcludedPhraseFirstOccurrenceCheckBox, 0, wxEXPAND|wxTOP,
            wxSizerFlags::GetDefaultBorder());

        m_excludedPhrasesPathFilePathEdit->Enable(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) ||
             (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings)));
        m_excludedPhrasesEditBrowseButton->Enable(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) ||
             (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings)));
        m_includeExcludedPhraseFirstOccurrenceCheckBox->Enable(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) ||
             (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings)));

        optionsIndentSizer->Add(excludedPhrasesFileBrowseBoxSizer, 0, wxEXPAND, wxSizerFlags::GetDefaultBorder());
        panelSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

        // tag block exclusion
        if (m_exclusionBlockTags.get_value().size() == 0)
            { m_exclusionBlockTagsOption = 0; }
        else if (m_exclusionBlockTags.get_value().at(0) == std::make_pair(L'^', L'^'))
            { m_exclusionBlockTagsOption = 1; }
        else if (m_exclusionBlockTags.get_value().at(0) == std::make_pair(L'<', L'>'))
            { m_exclusionBlockTagsOption = 2; }
        else if (m_exclusionBlockTags.get_value().at(0) == std::make_pair(L'[', L']'))
            { m_exclusionBlockTagsOption = 3; }
        else if (m_exclusionBlockTags.get_value().at(0) == std::make_pair(L'{', L'}'))
            { m_exclusionBlockTagsOption = 4; }
        else if (m_exclusionBlockTags.get_value().at(0) == std::make_pair(L'(', L')'))
            { m_exclusionBlockTagsOption = 5; }
        else
            { m_exclusionBlockTagsOption = 0; }

        wxBoxSizer* exclusionBlockTagsSizer = new wxBoxSizer(wxHORIZONTAL);
        m_exclusionBlockTagsLabel =
            new wxStaticText(AnalysisIndexingPage, ID_EXCLUSION_TAG_BLOCK_LABEL, _(L"Exclude text between:"),
                             wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
        m_exclusionBlockTagsLabel->Enable(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) ||
             (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings)));
        exclusionBlockTagsSizer->Add(m_exclusionBlockTagsLabel, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT,
                                     wxSizerFlags::GetDefaultBorder());
        wxArrayString exclusionBlockTagsOptions;
        exclusionBlockTagsOptions.Add(_(L"Not enabled"));
        exclusionBlockTagsOptions.Add(_(L"^ and ^"));
        exclusionBlockTagsOptions.Add(_(L"< and >"));
        exclusionBlockTagsOptions.Add(_(L"[ and ]"));
        exclusionBlockTagsOptions.Add(_(L"{ and }"));
        exclusionBlockTagsOptions.Add(_(L"( and )"));
        m_exclusionBlockTagsCombo = new wxChoice(AnalysisIndexingPage, ID_EXCLUSION_TAG_BLOCK_SELCTION,
                                            wxDefaultPosition, wxDefaultSize, exclusionBlockTagsOptions,
                                            0, wxGenericValidator(&m_exclusionBlockTagsOption));
        m_exclusionBlockTagsCombo->Enable(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) ||
             (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeExceptForHeadings)));
        exclusionBlockTagsSizer->Add(m_exclusionBlockTagsCombo);
        optionsIndentSizer->Add(exclusionBlockTagsSizer, 0, wxEXPAND|wxTOP, wxSizerFlags::GetDefaultBorder());

        // syllabication
        CreateLabelHeader(AnalysisIndexingPage, panelSizer, _(L"Numerals:"), true);

        wxBoxSizer* syllableSizer = new wxBoxSizer(wxHORIZONTAL);
        m_syllableLabel = new wxStaticText(AnalysisIndexingPage, wxID_STATIC, _(L"Syllabication:"));
        syllableSizer->Add(m_syllableLabel, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, wxSizerFlags::GetDefaultBorder());

        wxArrayString syllableOptions;
        syllableOptions.Add(_(L"Numerals are one syllable"));
        syllableOptions.Add(_(L"Sound out each digit"));
        m_syllableCombo = new wxChoice(AnalysisIndexingPage, ID_NUMBER_SYLLABIZE_METHOD,
                                            wxDefaultPosition, wxDefaultSize, syllableOptions,
                                            0, wxGenericValidator(&m_syllabicationMethod));
        m_syllableCombo->Enable(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::IncludeAsFullSentences)) ||
             !m_ignoreNumerals);
        m_syllableLabel->Enable(
            (m_textExclusionMethod == static_cast<int>(InvalidSentence::IncludeAsFullSentences)) ||
             !m_ignoreNumerals);
        syllableSizer->Add(m_syllableCombo, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, wxSizerFlags::GetDefaultBorder());
        panelSizer->Add(syllableSizer, 0, wxEXPAND|wxLEFT, OPTION_INDENT_SIZE);
        }

    // readability settings
    if (GetSectionsBeingShown() & ScoresSection)
        {
        wxPanel* ScoreTestOptionsPage =
            new wxPanel(m_sideBar, SCORES_TEST_OPTIONS_PAGE, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
        m_sideBar->AddPage(ScoreTestOptionsPage, GetReadabilityScoresLabel(), SCORES_TEST_OPTIONS_PAGE,
            // if the only section being shown, then show this page
            (GetSectionsBeingShown() == ScoresSection),
            1);

        // test options tab
        if (IsGeneralSettings() ||
            m_projectLanguage == static_cast<int>(readability::test_language::english_test))
            {
            wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);
            ScoreTestOptionsPage->SetSizer(panelSizer);
            m_sideBar->AddSubPage(ScoreTestOptionsPage, GetTestOptionsLabel(), SCORES_TEST_OPTIONS_PAGE, false, 9);

            wxPropertyGridManager* pgMan =
                new wxPropertyGridManager(ScoreTestOptionsPage, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                          wxPG_BOLD_MODIFIED|wxPG_DESCRIPTION|wxPGMAN_DEFAULT_STYLE);
            m_readabilityTestsPropertyGrid = pgMan->AddPage();
            // Fog
            m_readabilityTestsPropertyGrid->Append(new wxPropertyCategory(_DT(L"Gunning Fog")) );
            m_readabilityTestsPropertyGrid->SetPropertyHelpString(
                _DT(L"Gunning Fog"),
                _(L"The options in this section customize how Gunning Fog related tests are calculated."));

            m_readabilityTestsPropertyGrid->Append(
                new wxBoolProperty(GetCountIndependentClausesLabel(),wxPG_LABEL,
                    (m_readabilityProjectDoc ?
                        m_readabilityProjectDoc->FogUseSentenceUnits() :
                        wxGetApp().GetAppOptions().FogUseSentenceUnits())));
            m_readabilityTestsPropertyGrid->SetPropertyAttribute(
                GetCountIndependentClausesLabel(), wxPG_BOOL_USE_CHECKBOX, true);
            m_readabilityTestsPropertyGrid->SetPropertyHelpString(
                GetCountIndependentClausesLabel(),
                _(L"Check this option to count independent clauses as separate sentences when calculating a "
                   "Gunning Fog score. Independent clauses are detected by dashes, colons, and semicolons "
                   "within a sentence."));
            // Flesch
            m_readabilityTestsPropertyGrid->Append(new wxPropertyCategory(_DT(L"Flesch")) );
            m_readabilityTestsPropertyGrid->SetPropertyHelpString(
                _DT(L"Flesch"),
                _(L"The options in this section customize how Flesch related tests are calculated."));

            wxPGChoices fleschNumeralMethods;
            fleschNumeralMethods.Add(_(L"Count numerals as one syllable"));
            fleschNumeralMethods.Add(_(L"System default*"));
            m_readabilityTestsPropertyGrid->Append(
                new wxEnumProperty(GetNumeralSyllabicationLabel(),
                    GetFleschNumeralSyllabicationLabel(), fleschNumeralMethods,
                (m_readabilityProjectDoc ?
                    static_cast<int>(m_readabilityProjectDoc->GetFleschNumeralSyllabizeMethod()) :
                    static_cast<int>(wxGetApp().GetAppOptions().GetFleschNumeralSyllabizeMethod()))) );
            m_readabilityTestsPropertyGrid->SetPropertyHelpString(
                GetFleschNumeralSyllabicationLabel(),
                _(L"Controls how numerals are syllabized for Flesch related tests."));
            m_readabilityTestsPropertyGrid->EnableProperty(
                GetFleschNumeralSyllabicationLabel(),
                (m_textExclusionMethod == static_cast<int>(InvalidSentence::IncludeAsFullSentences)) ||
                !m_ignoreNumerals);
            // Flesch-Kincaid
            m_readabilityTestsPropertyGrid->Append(new wxPropertyCategory(_DT(L"Flesch-Kincaid")) );
            m_readabilityTestsPropertyGrid->SetPropertyHelpString(
                _DT(L"Flesch-Kincaid"),
                _(L"The options in this section customize how Flesch-Kincaid is calculated."));

            wxPGChoices fleschKincaidNumeralMethods;
            fleschKincaidNumeralMethods.Add(_(L"Sound out each digit"));
            fleschKincaidNumeralMethods.Add(_(L"System default*"));
            m_readabilityTestsPropertyGrid->Append(
                new wxEnumProperty(GetNumeralSyllabicationLabel(),
                    GetFleschKincaidNumeralSyllabicationLabel(), fleschKincaidNumeralMethods,
                (m_readabilityProjectDoc ?
                    static_cast<int>(m_readabilityProjectDoc->GetFleschKincaidNumeralSyllabizeMethod()) :
                    static_cast<int>(wxGetApp().GetAppOptions().GetFleschKincaidNumeralSyllabizeMethod()))) );
            m_readabilityTestsPropertyGrid->SetPropertyHelpString(
                GetFleschKincaidNumeralSyllabicationLabel(),
                _(L"Controls how numerals are syllabized for Flesch-Kincaid."));
            m_readabilityTestsPropertyGrid->EnableProperty(
                GetFleschKincaidNumeralSyllabicationLabel(),
                (m_textExclusionMethod == static_cast<int>(InvalidSentence::IncludeAsFullSentences)) ||
                 !m_ignoreNumerals);
            // DC options
            m_readabilityTestsPropertyGrid->Append(new wxPropertyCategory(_DT(L"Dale-Chall")) );
            m_readabilityTestsPropertyGrid->SetPropertyHelpString(
                _DT(L"Dale-Chall"),
                _(L"The options in this section customize how Dale-Chall related tests are calculated."));

            m_readabilityTestsPropertyGrid->Append(
                new wxBoolProperty(GetIncludeStockerLabel(),wxPG_LABEL,
                    (m_readabilityProjectDoc ?
                        m_readabilityProjectDoc->IsIncludingStockerCatholicSupplement() :
                        wxGetApp().GetAppOptions().IsIncludingStockerCatholicSupplement())));
            m_readabilityTestsPropertyGrid->SetPropertyAttribute(
                GetIncludeStockerLabel(), wxPG_BOOL_USE_CHECKBOX, true);
            m_readabilityTestsPropertyGrid->SetPropertyHelpString(
                GetIncludeStockerLabel(),
                _(L"Check this option to include Stocker's supplementary word list for Catholic students."));

            wxPGChoices properNounMethods;
            properNounMethods.Add(_(L"Count as unfamiliar"));
            properNounMethods.Add(_(L"Count as familiar"));
            properNounMethods.Add(_(L"First occurrence of each is unfamiliar"));
            m_readabilityTestsPropertyGrid->Append(
                new wxEnumProperty(GetProperNounsLabel(), wxPG_LABEL, properNounMethods,
                (m_readabilityProjectDoc ?
                    static_cast<int>(m_readabilityProjectDoc->GetDaleChallProperNounCountingMethod()) :
                    static_cast<int>(wxGetApp().GetAppOptions().GetDaleChallProperNounCountingMethod()))) );
            m_readabilityTestsPropertyGrid->SetPropertyHelpString(
                GetProperNounsLabel(), _(L"Controls how proper nouns are treated by Dale-Chall related tests"));

            wxPGChoices textExclusionMethods;
            textExclusionMethods.Add(_(L"Exclude incomplete sentences, except headings"));
            textExclusionMethods.Add(_(L"System default**"));
            m_readabilityTestsPropertyGrid->Append(
                new wxEnumProperty(GetTextExclusionLabel(), GetDCTextExclusionLabel(), textExclusionMethods,
                (m_readabilityProjectDoc ?
                    static_cast<int>(m_readabilityProjectDoc->GetDaleChallTextExclusionMode()) :
                    static_cast<int>(wxGetApp().GetAppOptions().GetDaleChallTextExclusionMode()))) );
            m_readabilityTestsPropertyGrid->SetPropertyHelpString(GetDCTextExclusionLabel(),
                _(L"Controls how text is excluded in Dale-Chall related tests."));
            // HJ options
            m_readabilityTestsPropertyGrid->Append(new wxPropertyCategory(_DT(L"Harris-Jacobson")) );
            m_readabilityTestsPropertyGrid->SetPropertyHelpString(
                _DT(L"Harris-Jacobson"),
                _(L"The options in this section customize how Harris-Jacobson is calculated."));

            m_readabilityTestsPropertyGrid->Append(
                new wxEnumProperty(GetTextExclusionLabel(), GetHJCTextExclusionLabel(), textExclusionMethods,
                (m_readabilityProjectDoc ?
                    static_cast<int>(m_readabilityProjectDoc->GetHarrisJacobsonTextExclusionMode()) :
                    static_cast<int>(wxGetApp().GetAppOptions().GetHarrisJacobsonTextExclusionMode()))) );
            m_readabilityTestsPropertyGrid->SetPropertyHelpString(
                GetHJCTextExclusionLabel(), _(L"Controls how text is excluded in Harris-Jacobson."));

            pgMan->SelectProperty(_DT(L"Gunning Fog"));
            panelSizer->Add(pgMan, 1, wxEXPAND);

            const int ScaledNoteWidth = FromDIP(wxSize(400,400)).GetWidth();

            m_readTestsSyllableLabel = new wxStaticText(ScoreTestOptionsPage, wxID_STATIC,
                (m_syllabicationMethod == static_cast<int>(NumeralSyllabize::WholeWordIsOneSyllable)) ?
                _(L"* Numeral syllabication system default: numerals are one syllable.") :
                _(L"* Numeral syllabication system default: sound out each digit."),
                wxDefaultPosition, wxDefaultSize, 0);
            m_readTestsSyllableLabel->Wrap(ScaledNoteWidth);
            m_readTestsSyllableLabel->Enable(
                (m_textExclusionMethod == static_cast<int>(InvalidSentence::IncludeAsFullSentences)) ||
                 !m_ignoreNumerals);
            panelSizer->Add(m_readTestsSyllableLabel, 0, wxLEFT|wxRIGHT, wxSizerFlags::GetDefaultBorder());

            m_textExclusionLabel = new wxStaticText(ScoreTestOptionsPage, wxID_STATIC,
                (m_textExclusionMethod == static_cast<int>(InvalidSentence::IncludeAsFullSentences)) ?
                    _(L"** Text exclusion system default: do not exclude any text.") :
                (m_textExclusionMethod == static_cast<int>(InvalidSentence::ExcludeFromAnalysis)) ?
                    _(L"** Text exclusion system default: exclude all incomplete sentences.") :
                    _(L"** Text exclusion system default: exclude all incomplete sentences, except headings."),
                wxDefaultPosition, wxDefaultSize, 0);
            m_textExclusionLabel->Wrap(ScaledNoteWidth);
            panelSizer->Add(m_textExclusionLabel, 0, wxLEFT|wxRIGHT, wxSizerFlags::GetDefaultBorder());
            }

        // grade level tab
            {
            wxPanel* Panel = new wxPanel(m_sideBar, SCORES_DISPLAY_PAGE, wxDefaultPosition,
                                         wxDefaultSize, wxTAB_TRAVERSAL);
            wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);
            Panel->SetSizer(panelSizer);
            m_sideBar->AddSubPage(Panel, GetScoreDisplayLabel(), SCORES_DISPLAY_PAGE, false, 9);

            wxPropertyGridManager* pgMan =
                new wxPropertyGridManager(Panel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                          wxPG_BOLD_MODIFIED|wxPG_DESCRIPTION|wxPGMAN_DEFAULT_STYLE);
            m_gradeLevelPropertyGrid = pgMan->AddPage();

            // Results
            m_gradeLevelPropertyGrid->Append(new wxPropertyCategory(GetScoreResultsLabel()) );
            m_gradeLevelPropertyGrid->SetPropertyHelpString(
                GetScoreResultsLabel(),
                _(L"The options in this section customize which results to display in the Scores section."));

            m_gradeLevelPropertyGrid->Append(
                new wxBoolProperty(GetIncludeScoreSummaryLabel(),wxPG_LABEL,
                (m_readabilityProjectDoc ?
                    m_readabilityProjectDoc->IsIncludingScoreSummaryReport() :
                    wxGetApp().GetAppOptions().IsIncludingScoreSummaryReport())));
            m_gradeLevelPropertyGrid->SetPropertyAttribute(
                GetIncludeScoreSummaryLabel(), wxPG_BOOL_USE_CHECKBOX, true);
            m_gradeLevelPropertyGrid->SetPropertyHelpString(
                GetIncludeScoreSummaryLabel(),
                _(L"Check this option to include a summary report of the test scores in the results."));

            // grade display
            m_gradeLevelPropertyGrid->Append(new wxPropertyCategory(GetGradeLabel()) );
            m_gradeLevelPropertyGrid->SetPropertyHelpString(
                GetGradeLabel(),
                _(L"The options in this section customize how the grade scores are displayed."));

            wxPGChoices gradeLevelOption;
            gradeLevelOption.Add(
                _(L"K-12+ (United States of America)"),
                static_cast<int>(readability::grade_scale::k12_plus_united_states));
            gradeLevelOption.Add(
                _(L"K-12+ (Newfoundland and Labrador)"),
                static_cast<int>(readability::grade_scale::k12_plus_newfoundland_and_labrador));
            gradeLevelOption.Add(
                _(L"K-12+ (British Columbia/Yukon)"),
                static_cast<int>(readability::grade_scale::k12_plus_british_columbia));
            gradeLevelOption.Add(
                _(L"K-12+ (New Brunswick)"),
                static_cast<int>(readability::grade_scale::k12_plus_newbrunswick));
            gradeLevelOption.Add(
                _(L"K-12+ (Nova Scotia)"),
                static_cast<int>(readability::grade_scale::k12_plus_nova_scotia));
            gradeLevelOption.Add(
                _(L"K-12+ (Ontario)"),
                static_cast<int>(readability::grade_scale::k12_plus_ontario));
            gradeLevelOption.Add(
                _(L"K-12+ (Saskatchewan)"),
                static_cast<int>(readability::grade_scale::k12_plus_saskatchewan));
            gradeLevelOption.Add(
                _(L"K-12+ (Prince Edward Island)"),
                static_cast<int>(readability::grade_scale::k12_plus_prince_edward_island));
            gradeLevelOption.Add(
                _(L"K-12+ (Manitoba)"),
                static_cast<int>(readability::grade_scale::k12_plus_manitoba));
            gradeLevelOption.Add(
                _(L"K-12+ (Northwest Territories)"),
                static_cast<int>(readability::grade_scale::k12_plus_northwest_territories));
            gradeLevelOption.Add(
                _(L"K-12+ (Alberta)"),
                static_cast<int>(readability::grade_scale::k12_plus_alberta));
            gradeLevelOption.Add(
                _(L"K-12+ (Nunavut)"),
                static_cast<int>(readability::grade_scale::k12_plus_nunavut));
            gradeLevelOption.Add(
                _DT(L"Quebec"),
                static_cast<int>(readability::grade_scale::quebec));
            gradeLevelOption.Add(
                _(L"Key stages (England & Wales)"),
                static_cast<int>(readability::grade_scale::key_stages_england_wales));
            m_gradeLevelPropertyGrid->Append(new wxEnumProperty(GetGradeScaleLabel(), wxPG_LABEL, gradeLevelOption,
                (m_readabilityProjectDoc ?
                    static_cast<int>(m_readabilityProjectDoc->GetReadabilityMessageCatalog().GetGradeScale()) :
                    static_cast<int>(wxGetApp().GetAppOptions().GetReadabilityMessageCatalog().GetGradeScale()))) );
            m_gradeLevelPropertyGrid->SetPropertyHelpString(
                GetGradeScaleLabel(),
                _(L"Select from this list the grade scale that you wish to use."));

            m_gradeLevelPropertyGrid->Append(
                new wxBoolProperty(GetGradesLongFormatLabel(), wxPG_LABEL,
                (m_readabilityProjectDoc ?
                    m_readabilityProjectDoc->GetReadabilityMessageCatalog().IsUsingLongGradeScaleFormat() :
                    wxGetApp().GetAppOptions().GetReadabilityMessageCatalog().IsUsingLongGradeScaleFormat())));
            m_gradeLevelPropertyGrid->SetPropertyAttribute(GetGradesLongFormatLabel(), wxPG_BOOL_USE_CHECKBOX, true);
            m_gradeLevelPropertyGrid->SetPropertyHelpString(
                GetGradesLongFormatLabel(),
                _(L"Check this to display readability scores in long format "
                   "(e.g., \"2nd grade\") within the score grid."));

            m_gradeLevelPropertyGrid->Append(new wxPropertyCategory(GetReadingAgeLabel()) );
            m_gradeLevelPropertyGrid->SetPropertyHelpString(
                GetReadingAgeLabel(),
                _(L"The options in this section customize how reading ages are displayed."));
            // reading display
            wxPGChoices readingAgeDisplay;
            readingAgeDisplay.Add(_(L"School-year range"));
            readingAgeDisplay.Add(_(L"Round to semester"));
            m_gradeLevelPropertyGrid->Append(new wxEnumProperty(GetCalculationLabel(), wxPG_LABEL, readingAgeDisplay,
                (m_readabilityProjectDoc ?
                    static_cast<int>(m_readabilityProjectDoc->GetReadabilityMessageCatalog().GetReadingAgeDisplay()) :
                    static_cast<int>(wxGetApp().GetAppOptions().GetReadabilityMessageCatalog().
                        GetReadingAgeDisplay()))) );
            m_gradeLevelPropertyGrid->SetPropertyHelpString(
                GetCalculationLabel(),
                _(L"Selects how reading ages should be calculated from grade scores."));

            pgMan->SelectProperty(GetGradeLabel());

            panelSizer->Add(pgMan, 1, wxEXPAND);
            }
        }

    // Statistics page
    if (GetSectionsBeingShown() & Statistics)
        {
        wxPanel* Panel = new wxPanel(m_sideBar, ANALYSIS_STATISTICS_PAGE, wxDefaultPosition, wxDefaultSize,
                                     wxTAB_TRAVERSAL);
        wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);
        Panel->SetSizer(panelSizer);
        m_sideBar->AddPage(Panel, BaseProjectView::GetSummaryStatisticsLabel(), ANALYSIS_STATISTICS_PAGE,
            // if the only section being shown, then show this page
            (GetSectionsBeingShown() == Statistics),
            2);

        wxPropertyGridManager* pgMan =
            new wxPropertyGridManager(Panel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                      wxPG_BOLD_MODIFIED|wxPG_DESCRIPTION|wxPGMAN_DEFAULT_STYLE);
        m_statisticsPropertyGrid = pgMan->AddPage();
        // Results
        m_statisticsPropertyGrid->Append(new wxPropertyCategory(GetResultsLabel()) );
        m_statisticsPropertyGrid->SetPropertyHelpString(
            GetResultsLabel(),
            _(L"The options in this section customize which results to include in the Summary Statistics."));
        m_statisticsPropertyGrid->Append(
            new wxBoolProperty(BaseProjectView::GetFormattedReportLabel(), wxPG_LABEL,
            (m_readabilityProjectDoc ?
                m_readabilityProjectDoc->GetStatisticsInfo().IsReportEnabled() :
                wxGetApp().GetAppOptions().GetStatisticsInfo().IsReportEnabled())) );
        m_statisticsPropertyGrid->SetPropertyAttribute(
            BaseProjectView::GetFormattedReportLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_statisticsPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetFormattedReportLabel(),
            _(L"Check this to include the statistics report in the results."));
        m_statisticsPropertyGrid->Append(
            new wxBoolProperty(BaseProjectView::GetTabularReportLabel(), wxPG_LABEL,
            (m_readabilityProjectDoc ?
                m_readabilityProjectDoc->GetStatisticsInfo().IsTableEnabled() :
                wxGetApp().GetAppOptions().GetStatisticsInfo().IsTableEnabled())) );
        m_statisticsPropertyGrid->SetPropertyAttribute(
            BaseProjectView::GetTabularReportLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_statisticsPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetTabularReportLabel(),
            _(L"Check this to include the statistics (tabular format) in the results."));
        // Report
        m_statisticsPropertyGrid->Append(
            new wxPropertyCategory(GetStatisticsReportLabel()) );
        m_statisticsPropertyGrid->SetPropertyHelpString(
            GetStatisticsReportLabel(),
            _(L"The options in this section customize which statistics are included in the report and results."));
        m_statisticsPropertyGrid->Append(
            new wxBoolProperty(GetParagraphsLabel(), wxPG_LABEL,
            (m_readabilityProjectDoc ?
                m_readabilityProjectDoc->GetStatisticsReportInfo().IsParagraphEnabled() :
                wxGetApp().GetAppOptions().GetStatisticsReportInfo().IsParagraphEnabled())) );
        m_statisticsPropertyGrid->SetPropertyAttribute(GetParagraphsLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_statisticsPropertyGrid->SetPropertyHelpString(
            GetParagraphsLabel(),
            _(L"Check this to include the paragraph statistics in the report."));
        m_statisticsPropertyGrid->Append(
            new wxBoolProperty(GetSentencesLabel(), wxPG_LABEL,
            (m_readabilityProjectDoc ?
                m_readabilityProjectDoc->GetStatisticsReportInfo().IsSentencesEnabled() :
                wxGetApp().GetAppOptions().GetStatisticsReportInfo().IsSentencesEnabled())) );
        m_statisticsPropertyGrid->SetPropertyAttribute(GetSentencesLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_statisticsPropertyGrid->SetPropertyHelpString(
            GetSentencesLabel(),
            _(L"Check this to include the sentence statistics in the report."));
        m_statisticsPropertyGrid->Append(
            new wxBoolProperty(GetWordsLabel(), wxPG_LABEL,
            (m_readabilityProjectDoc ?
                m_readabilityProjectDoc->GetStatisticsReportInfo().IsWordsEnabled() :
                wxGetApp().GetAppOptions().GetStatisticsReportInfo().IsWordsEnabled())) );
        m_statisticsPropertyGrid->SetPropertyAttribute(GetWordsLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_statisticsPropertyGrid->SetPropertyHelpString(
            GetWordsLabel(),
            _(L"Check this to include the word statistics in the report."));
        m_statisticsPropertyGrid->Append(
            new wxBoolProperty(GetExtendedWordsLabel(), wxPG_LABEL,
            (m_readabilityProjectDoc ?
                m_readabilityProjectDoc->GetStatisticsReportInfo().IsExtendedWordsEnabled() :
                wxGetApp().GetAppOptions().GetStatisticsReportInfo().IsExtendedWordsEnabled())) );
        m_statisticsPropertyGrid->SetPropertyAttribute(GetExtendedWordsLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_statisticsPropertyGrid->SetPropertyHelpString(
            GetExtendedWordsLabel(),
            _(L"Check this to include extended word statistics (e.g., numerals) in the report."));
        m_statisticsPropertyGrid->Append(
            new wxBoolProperty(GetGrammarLabel(), wxPG_LABEL,
            (m_readabilityProjectDoc ?
                m_readabilityProjectDoc->GetStatisticsReportInfo().IsGrammarEnabled() :
                wxGetApp().GetAppOptions().GetStatisticsReportInfo().IsGrammarEnabled())) );
        m_statisticsPropertyGrid->SetPropertyAttribute(GetGrammarLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_statisticsPropertyGrid->SetPropertyHelpString(
            GetGrammarLabel(),
            _(L"Check this to include grammar statistics in the report."));
        m_statisticsPropertyGrid->Append(
            new wxBoolProperty(GetNotesLabel(), wxPG_LABEL,
            (m_readabilityProjectDoc ?
                m_readabilityProjectDoc->GetStatisticsReportInfo().IsNotesEnabled() :
                wxGetApp().GetAppOptions().GetStatisticsReportInfo().IsNotesEnabled())) );
        m_statisticsPropertyGrid->SetPropertyAttribute(GetNotesLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_statisticsPropertyGrid->SetPropertyHelpString(
            GetNotesLabel(),
            _(L"Check this to include the notes in the report."));
        m_statisticsPropertyGrid->Append(
            new wxBoolProperty(GetExtendedInformationLabel(), wxPG_LABEL,
            (m_readabilityProjectDoc ?
                m_readabilityProjectDoc->GetStatisticsReportInfo().IsExtendedInformationEnabled() :
                wxGetApp().GetAppOptions().GetStatisticsReportInfo().IsExtendedInformationEnabled())) );
        m_statisticsPropertyGrid->SetPropertyAttribute(GetExtendedInformationLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_statisticsPropertyGrid->SetPropertyHelpString(
            GetExtendedInformationLabel(),
            _(L"Check this to include more verbose statistics and notes in the results."));
        // Dolch report options
        if (m_projectLanguage == static_cast<int>(readability::test_language::english_test))
            {
            m_statisticsPropertyGrid->Append(new wxPropertyCategory(GetDolchStatisticsReportLabel()) );
            m_statisticsPropertyGrid->SetPropertyHelpString(
                GetDolchStatisticsReportLabel(),
                _(L"The options in this section customize which statistics are included in the Dolch report."));
            m_statisticsPropertyGrid->Append(
                new wxBoolProperty(GetCoverageLabel(), wxPG_LABEL,
                (m_readabilityProjectDoc ?
                    m_readabilityProjectDoc->GetStatisticsReportInfo().IsDolchCoverageEnabled() :
                    wxGetApp().GetAppOptions().GetStatisticsReportInfo().IsDolchCoverageEnabled())) );
            m_statisticsPropertyGrid->SetPropertyAttribute(GetCoverageLabel(), wxPG_BOOL_USE_CHECKBOX, true);
            m_statisticsPropertyGrid->SetPropertyHelpString(
                GetCoverageLabel(),
                _(L"Check this to include the word coverage statistics in the report."));
            m_statisticsPropertyGrid->Append(
                new wxBoolProperty(GetWordsLabel(), GetDolchWordsLabel(),
                (m_readabilityProjectDoc ?
                    m_readabilityProjectDoc->GetStatisticsReportInfo().IsDolchWordsEnabled() :
                    wxGetApp().GetAppOptions().GetStatisticsReportInfo().IsDolchWordsEnabled())) );
            m_statisticsPropertyGrid->SetPropertyAttribute(GetDolchWordsLabel(), wxPG_BOOL_USE_CHECKBOX, true);
            m_statisticsPropertyGrid->SetPropertyHelpString(
                GetDolchWordsLabel(),
                _(L"Check this to include the word statistics in the report."));
            m_statisticsPropertyGrid->Append(
                new wxBoolProperty(GetDolchExplanationLabel(), wxPG_LABEL,
                (m_readabilityProjectDoc ?
                    m_readabilityProjectDoc->GetStatisticsReportInfo().IsDolchExplanationEnabled() :
                    wxGetApp().GetAppOptions().GetStatisticsReportInfo().IsDolchExplanationEnabled())) );
            m_statisticsPropertyGrid->SetPropertyAttribute(GetDolchExplanationLabel(), wxPG_BOOL_USE_CHECKBOX, true);
            m_statisticsPropertyGrid->SetPropertyHelpString(
                GetDolchExplanationLabel(),
                _(L"Check this to include the Dolch explanation in the report."));
            }

        pgMan->SelectProperty(GetStatisticsReportLabel());

        panelSizer->Add(pgMan, 1, wxEXPAND);
        }

    // words breakdown page (these options only apply to general options and standard projects)
    if ((GetSectionsBeingShown() & WordsBreakdown) && !IsBatchProjectSettings())
        {
        wxPanel* Panel = new wxPanel(m_sideBar, WORDS_BREAKDOWN_PAGE, wxDefaultPosition, wxDefaultSize,
                                     wxTAB_TRAVERSAL);
        wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);
        Panel->SetSizer(panelSizer);
        m_sideBar->AddPage(Panel, BaseProjectView::GetWordsBreakdownLabel(), WORDS_BREAKDOWN_PAGE,
            // if the only section being shown, then show this page
            (GetSectionsBeingShown() == WordsBreakdown),
            13);

        wxPropertyGridManager* pgMan =
            new wxPropertyGridManager(Panel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                      wxPG_BOLD_MODIFIED|wxPG_DESCRIPTION|wxPGMAN_DEFAULT_STYLE);
        m_wordsBreakdownPropertyGrid = pgMan->AddPage();

        // which features to include
        m_wordsBreakdownPropertyGrid->Append(new wxPropertyCategory(GetResultsLabel()) );
        m_wordsBreakdownPropertyGrid->SetPropertyHelpString(
            GetResultsLabel(),
            _(L"The options on this page customize which results are included in the Words Breakdown section"));

        m_wordsBreakdownPropertyGrid->Append(
            new wxBoolProperty(BaseProjectView::GetWordCountsLabel(), wxPG_LABEL,
            (m_readabilityProjectDoc ?
                m_readabilityProjectDoc->GetWordsBreakdownInfo().IsWordBarchartEnabled() :
                wxGetApp().GetAppOptions().GetWordsBreakdownInfo().IsWordBarchartEnabled())) );
        m_wordsBreakdownPropertyGrid->SetPropertyAttribute(
            BaseProjectView::GetWordCountsLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_wordsBreakdownPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetWordCountsLabel(),
            _(L"Check this to include the word barchart in the results."));

        m_wordsBreakdownPropertyGrid->Append(
            new wxBoolProperty(BaseProjectView::GetSyllableCountsLabel(), wxPG_LABEL,
            (m_readabilityProjectDoc ?
                m_readabilityProjectDoc->GetWordsBreakdownInfo().IsSyllableHistogramEnabled() :
                wxGetApp().GetAppOptions().GetWordsBreakdownInfo().IsSyllableHistogramEnabled())) );
        m_wordsBreakdownPropertyGrid->SetPropertyAttribute(BaseProjectView::GetSyllableCountsLabel(),
                                                           wxPG_BOOL_USE_CHECKBOX, true);
        m_wordsBreakdownPropertyGrid->SetPropertyHelpString(BaseProjectView::GetSyllableCountsLabel(),
            _(L"Check this to include the syllable histogram in the results."));

        m_wordsBreakdownPropertyGrid->Append(
            new wxBoolProperty(BaseProjectView::GetThreeSyllableWordsLabel(), wxPG_LABEL,
            (m_readabilityProjectDoc ?
                m_readabilityProjectDoc->GetWordsBreakdownInfo().Is3PlusSyllablesEnabled() :
                wxGetApp().GetAppOptions().GetWordsBreakdownInfo().Is3PlusSyllablesEnabled())) );
        m_wordsBreakdownPropertyGrid->SetPropertyAttribute(BaseProjectView::GetThreeSyllableWordsLabel(),
                                                           wxPG_BOOL_USE_CHECKBOX, true);
        m_wordsBreakdownPropertyGrid->SetPropertyHelpString(BaseProjectView::GetThreeSyllableWordsLabel(),
            _(L"Check this to include the 3+ syllable words list in the results."));

        m_wordsBreakdownPropertyGrid->Append(
            new wxBoolProperty(BaseProjectView::GetSixCharWordsLabel(), wxPG_LABEL,
            (m_readabilityProjectDoc ?
                m_readabilityProjectDoc->GetWordsBreakdownInfo().Is6PlusCharacterEnabled() :
                wxGetApp().GetAppOptions().GetWordsBreakdownInfo().Is6PlusCharacterEnabled())) );
        m_wordsBreakdownPropertyGrid->SetPropertyAttribute(BaseProjectView::GetSixCharWordsLabel(),
                                                           wxPG_BOOL_USE_CHECKBOX, true);
        m_wordsBreakdownPropertyGrid->SetPropertyHelpString(BaseProjectView::GetSixCharWordsLabel(),
            _(L"Check this to include the 6+ character words in the results."));

        m_wordsBreakdownPropertyGrid->Append(
            new wxBoolProperty(BaseProjectView::GetWordCloudLabel(), wxPG_LABEL,
            (m_readabilityProjectDoc ?
                m_readabilityProjectDoc->GetWordsBreakdownInfo().IsWordCloudEnabled() :
                wxGetApp().GetAppOptions().GetWordsBreakdownInfo().IsWordCloudEnabled())) );
        m_wordsBreakdownPropertyGrid->SetPropertyAttribute(BaseProjectView::GetWordCloudLabel(),
                                                           wxPG_BOOL_USE_CHECKBOX, true);
        m_wordsBreakdownPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetWordCloudLabel(),
            _(L"Check this to include the key word cloud in the results."));

        m_wordsBreakdownPropertyGrid->Append(
            new wxBoolProperty(BaseProjectView::GetDaleChallLabel(), wxPG_LABEL,
            (m_readabilityProjectDoc ?
                m_readabilityProjectDoc->GetWordsBreakdownInfo().IsDCUnfamiliarEnabled() :
                wxGetApp().GetAppOptions().GetWordsBreakdownInfo().IsDCUnfamiliarEnabled())) );
        m_wordsBreakdownPropertyGrid->SetPropertyAttribute(
            BaseProjectView::GetDaleChallLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_wordsBreakdownPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetDaleChallLabel(),
            _(L"Check this to include the Dale-Chall unfamiliar words list in the results. "
               "(If a Dale-Chall test is included in the project)."));

        m_wordsBreakdownPropertyGrid->Append(
            new wxBoolProperty(BaseProjectView::GetSpacheLabel(), wxPG_LABEL,
            (m_readabilityProjectDoc ?
                m_readabilityProjectDoc->GetWordsBreakdownInfo().IsSpacheUnfamiliarEnabled() :
                wxGetApp().GetAppOptions().GetWordsBreakdownInfo().IsSpacheUnfamiliarEnabled())) );
        m_wordsBreakdownPropertyGrid->SetPropertyAttribute(
            BaseProjectView::GetSpacheLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_wordsBreakdownPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetSpacheLabel(),
            _(L"Check this to include the Spache unfamiliar words list in the results. "
               "(If Spache is included in the project)."));

        m_wordsBreakdownPropertyGrid->Append(
            new wxBoolProperty(BaseProjectView::GetHarrisJacobsonLabel(), wxPG_LABEL,
            (m_readabilityProjectDoc ?
                m_readabilityProjectDoc->GetWordsBreakdownInfo().IsHarrisJacobsonUnfamiliarEnabled() :
                wxGetApp().GetAppOptions().GetWordsBreakdownInfo().IsHarrisJacobsonUnfamiliarEnabled())) );
        m_wordsBreakdownPropertyGrid->SetPropertyAttribute(
            BaseProjectView::GetHarrisJacobsonLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_wordsBreakdownPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetHarrisJacobsonLabel(),
            _(L"Check this to include the Harris-Jacobson unfamiliar words list in the results. "
               "(If Harris-Jacobson is included in the project)."));

        m_wordsBreakdownPropertyGrid->Append(
            new wxBoolProperty(GetCustomTestsLabel(), wxPG_LABEL,
            (m_readabilityProjectDoc ?
                m_readabilityProjectDoc->GetWordsBreakdownInfo().IsCustomTestsUnfamiliarEnabled() :
                wxGetApp().GetAppOptions().GetWordsBreakdownInfo().IsCustomTestsUnfamiliarEnabled())) );
        m_wordsBreakdownPropertyGrid->SetPropertyAttribute(
            GetCustomTestsLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_wordsBreakdownPropertyGrid->SetPropertyHelpString(
            GetCustomTestsLabel(),
            _(L"Check this to include any custom tests' unfamiliar words lists in the results."));

        m_wordsBreakdownPropertyGrid->Append(
            new wxBoolProperty(BaseProjectView::GetAllWordsLabel(), wxPG_LABEL,
            (m_readabilityProjectDoc ?
                m_readabilityProjectDoc->GetWordsBreakdownInfo().IsAllWordsEnabled() :
                wxGetApp().GetAppOptions().GetWordsBreakdownInfo().IsAllWordsEnabled())) );
        m_wordsBreakdownPropertyGrid->SetPropertyAttribute(
            BaseProjectView::GetAllWordsLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_wordsBreakdownPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetAllWordsLabel(),
            _(L"Check this to include a list of all words in the results."));

        m_wordsBreakdownPropertyGrid->Append(
            new wxBoolProperty(BaseProjectView::GetKeyWordsLabel(), wxPG_LABEL,
            (m_readabilityProjectDoc ?
                m_readabilityProjectDoc->GetWordsBreakdownInfo().IsKeyWordsEnabled() :
                wxGetApp().GetAppOptions().GetWordsBreakdownInfo().IsKeyWordsEnabled())) );
        m_wordsBreakdownPropertyGrid->SetPropertyAttribute(
            BaseProjectView::GetKeyWordsLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_wordsBreakdownPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetKeyWordsLabel(),
            _(L"Check this to include a list of all words (in condensed form) in the results. "
               "Words with the same root (e.g., \"run\" and \"running\") will be combined into one row."));

        pgMan->SelectProperty(GetResultsLabel());
        pgMan->SetDescBoxHeight(FromDIP(wxSize(200,200)).GetHeight());

        panelSizer->Add(pgMan, 1, wxEXPAND);
        }

    // sentences breakdown page (these options only apply to general options and standard projects)
    if ((GetSectionsBeingShown() & SentencesBreakdown) && !IsBatchProjectSettings())
        {
        wxPanel* Panel = new wxPanel(m_sideBar, SENTENCES_BREAKDOWN_PAGE, wxDefaultPosition,
                                     wxDefaultSize, wxTAB_TRAVERSAL);
        wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);
        Panel->SetSizer(panelSizer);
        m_sideBar->AddPage(Panel, BaseProjectView::GetSentencesBreakdownLabel(), SENTENCES_BREAKDOWN_PAGE,
            // if the only section being shown, then show this page
            (GetSectionsBeingShown() == SentencesBreakdown),
            14);

        wxPropertyGridManager* pgMan =
            new wxPropertyGridManager(Panel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                      wxPG_BOLD_MODIFIED|wxPG_DESCRIPTION|wxPGMAN_DEFAULT_STYLE);
        m_sentencesBreakdownPropertyGrid = pgMan->AddPage();

        // which features to include
        m_sentencesBreakdownPropertyGrid->Append(new wxPropertyCategory(GetResultsLabel()) );
        m_sentencesBreakdownPropertyGrid->SetPropertyHelpString(
            GetResultsLabel(),
            _(L"The options on this page customize which results are included in the Sentences Breakdown section"));

        m_sentencesBreakdownPropertyGrid->Append(
            new wxBoolProperty(BaseProjectView::GetLongSentencesLabel(), wxPG_LABEL,
            (m_readabilityProjectDoc ?
                m_readabilityProjectDoc->GetSentencesBreakdownInfo().IsLongSentencesEnabled() :
                wxGetApp().GetAppOptions().GetSentencesBreakdownInfo().IsLongSentencesEnabled())) );
        m_sentencesBreakdownPropertyGrid->SetPropertyAttribute(
            BaseProjectView::GetLongSentencesLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_sentencesBreakdownPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetLongSentencesLabel(),
            _(L"Check this to include the overly-long sentences list in the results."));

        m_sentencesBreakdownPropertyGrid->Append(
            new wxBoolProperty(BaseProjectView::GetSentenceLengthBoxPlotLabel(), wxPG_LABEL,
            (m_readabilityProjectDoc ?
                m_readabilityProjectDoc->GetSentencesBreakdownInfo().IsLengthsBoxPlotEnabled() :
                wxGetApp().GetAppOptions().GetSentencesBreakdownInfo().IsLengthsBoxPlotEnabled())) );
        m_sentencesBreakdownPropertyGrid->SetPropertyAttribute(
            BaseProjectView::GetSentenceLengthBoxPlotLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_sentencesBreakdownPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetSentenceLengthBoxPlotLabel(),
            _(L"Check this to include the sentence-lengths box plot in the results."));

        m_sentencesBreakdownPropertyGrid->Append(
            new wxBoolProperty(BaseProjectView::GetSentenceLengthHistogramLabel(), wxPG_LABEL,
            (m_readabilityProjectDoc ?
                m_readabilityProjectDoc->GetSentencesBreakdownInfo().IsLengthsHistogramEnabled() :
                wxGetApp().GetAppOptions().GetSentencesBreakdownInfo().IsLengthsHistogramEnabled())) );
        m_sentencesBreakdownPropertyGrid->SetPropertyAttribute(
            BaseProjectView::GetSentenceLengthHistogramLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_sentencesBreakdownPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetSentenceLengthHistogramLabel(),
            _(L"Check this to include the sentence-lengths histogram in the results."));

        m_sentencesBreakdownPropertyGrid->Append(
            new wxBoolProperty(BaseProjectView::GetSentenceLengthHeatmapLabel(), wxPG_LABEL,
            (m_readabilityProjectDoc ?
                m_readabilityProjectDoc->GetSentencesBreakdownInfo().IsLengthsHeatmapEnabled() :
                wxGetApp().GetAppOptions().GetSentencesBreakdownInfo().IsLengthsHeatmapEnabled())) );
        m_sentencesBreakdownPropertyGrid->SetPropertyAttribute(
            BaseProjectView::GetSentenceLengthHeatmapLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_sentencesBreakdownPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetSentenceLengthHeatmapLabel(),
            _(L"Check this to include the sentence-lengths heatmap in the results."));

        pgMan->SelectProperty(GetResultsLabel());
        pgMan->SetDescBoxHeight(FromDIP(wxSize(200, 200)).GetHeight());

        panelSizer->Add(pgMan, 1, wxEXPAND);
        }

    // Grammar page
    if (GetSectionsBeingShown() & Grammar)
        {
        wxPanel* Panel = new wxPanel(m_sideBar, GRAMMAR_PAGE, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
        wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);
        Panel->SetSizer(panelSizer);
        m_sideBar->AddPage(Panel, BaseProjectView::GetGrammarLabel(), GRAMMAR_PAGE,
            // if the only section being shown, then show this page
            (GetSectionsBeingShown() == Grammar),
            4);

        wxPropertyGridManager* pgMan =
            new wxPropertyGridManager(Panel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                      wxPG_BOLD_MODIFIED|wxPG_DESCRIPTION|wxPGMAN_DEFAULT_STYLE);
        m_grammarPropertyGrid = pgMan->AddPage();

        // which grammar features to include
        m_grammarPropertyGrid->Append(new wxPropertyCategory(GetResultsLabel()) );
        m_grammarPropertyGrid->SetPropertyHelpString(
            GetResultsLabel(),
            _(L"The options in this section customize which features are included in the grammar analysis."));

        m_grammarPropertyGrid->Append(
            new wxBoolProperty(GetGrammarHighlightedReportLabel(), wxPG_LABEL,
            (m_readabilityProjectDoc ?
                m_readabilityProjectDoc->GetGrammarInfo().IsHighlightedReportEnabled() :
                wxGetApp().GetAppOptions().GetGrammarInfo().IsHighlightedReportEnabled())) );
        m_grammarPropertyGrid->SetPropertyAttribute(
            GetGrammarHighlightedReportLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_grammarPropertyGrid->SetPropertyHelpString(
            GetGrammarHighlightedReportLabel(),
            _(L"Check this to include the highlighted grammar report in the results."));

        m_grammarPropertyGrid->Append(
            new wxBoolProperty(BaseProjectView::GetMisspellingsLabel(), wxPG_LABEL,
            (m_readabilityProjectDoc ?
                m_readabilityProjectDoc->GetGrammarInfo().IsMisspellingsEnabled() :
                wxGetApp().GetAppOptions().GetGrammarInfo().IsMisspellingsEnabled())) );
        m_grammarPropertyGrid->SetPropertyAttribute(
            BaseProjectView::GetMisspellingsLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_grammarPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetMisspellingsLabel(),
            _(L"Check this to include misspellings in the results."));

        m_grammarPropertyGrid->Append(
            new wxBoolProperty(BaseProjectView::GetRepeatedWordsLabel(), wxPG_LABEL,
            (m_readabilityProjectDoc ?
                m_readabilityProjectDoc->GetGrammarInfo().IsRepeatedWordsEnabled() :
                wxGetApp().GetAppOptions().GetGrammarInfo().IsRepeatedWordsEnabled())) );
        m_grammarPropertyGrid->SetPropertyAttribute(
            BaseProjectView::GetRepeatedWordsLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_grammarPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetRepeatedWordsLabel(),
            _(L"Check this to include repeated words in the results."));

        m_grammarPropertyGrid->Append(
            new wxBoolProperty(BaseProjectView::GetArticleMismatchesLabel(), wxPG_LABEL,
            (m_readabilityProjectDoc ?
                m_readabilityProjectDoc->GetGrammarInfo().IsArticleMismatchesEnabled() :
                wxGetApp().GetAppOptions().GetGrammarInfo().IsArticleMismatchesEnabled())) );
        m_grammarPropertyGrid->SetPropertyAttribute(
            BaseProjectView::GetArticleMismatchesLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_grammarPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetArticleMismatchesLabel(),
            _(L"Check this to include article mismatches in the results."));

        m_grammarPropertyGrid->Append(
            new wxBoolProperty(BaseProjectView::GetPhrasingErrorsTabLabel(), wxPG_LABEL,
            (m_readabilityProjectDoc ?
                m_readabilityProjectDoc->GetGrammarInfo().IsWordingErrorsEnabled() :
                wxGetApp().GetAppOptions().GetGrammarInfo().IsWordingErrorsEnabled())) );
        m_grammarPropertyGrid->SetPropertyAttribute(
            BaseProjectView::GetPhrasingErrorsTabLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_grammarPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetPhrasingErrorsTabLabel(),
            _(L"Check this to include wording errors in the results."));

        m_grammarPropertyGrid->Append(
            new wxBoolProperty(BaseProjectView::GetRedundantPhrasesTabLabel(), wxPG_LABEL,
            (m_readabilityProjectDoc ?
                m_readabilityProjectDoc->GetGrammarInfo().IsRedundantPhrasesEnabled() :
                wxGetApp().GetAppOptions().GetGrammarInfo().IsRedundantPhrasesEnabled())) );
        m_grammarPropertyGrid->SetPropertyAttribute(
            BaseProjectView::GetRedundantPhrasesTabLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_grammarPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetRedundantPhrasesTabLabel(),
            _(L"Check this to include redundant phrases in the results."));

        m_grammarPropertyGrid->Append(
            new wxBoolProperty(BaseProjectView::GetOverusedWordsBySentenceLabel(), wxPG_LABEL,
            (m_readabilityProjectDoc ?
                m_readabilityProjectDoc->GetGrammarInfo().IsOverUsedWordsBySentenceEnabled() :
                wxGetApp().GetAppOptions().GetGrammarInfo().IsOverUsedWordsBySentenceEnabled())) );
        m_grammarPropertyGrid->SetPropertyAttribute(
            BaseProjectView::GetOverusedWordsBySentenceLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_grammarPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetOverusedWordsBySentenceLabel(),
            _(L"Check this to include overused words (by sentence) in the results."));

        m_grammarPropertyGrid->Append(
            new wxBoolProperty(BaseProjectView::GetWordyPhrasesTabLabel(), wxPG_LABEL,
            (m_readabilityProjectDoc ?
                m_readabilityProjectDoc->GetGrammarInfo().IsWordyPhrasesEnabled() :
                wxGetApp().GetAppOptions().GetGrammarInfo().IsWordyPhrasesEnabled())) );
        m_grammarPropertyGrid->SetPropertyAttribute(
            BaseProjectView::GetWordyPhrasesTabLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_grammarPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetWordyPhrasesTabLabel(), _(L"Check this to include wordy phrases in the results."));

        m_grammarPropertyGrid->Append(
            new wxBoolProperty(BaseProjectView::GetClichesTabLabel(), wxPG_LABEL,
            (m_readabilityProjectDoc ?
                m_readabilityProjectDoc->GetGrammarInfo().IsClichesEnabled() :
                wxGetApp().GetAppOptions().GetGrammarInfo().IsClichesEnabled())) );
        m_grammarPropertyGrid->SetPropertyAttribute(
            BaseProjectView::GetClichesTabLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_grammarPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetClichesTabLabel(), _(L"Check this to include clich\351s in the results."));

        m_grammarPropertyGrid->Append(
            new wxBoolProperty(BaseProjectView::GetPassiveLabel(), wxPG_LABEL,
            (m_readabilityProjectDoc ?
                m_readabilityProjectDoc->GetGrammarInfo().IsPassiveVoiceEnabled() :
                wxGetApp().GetAppOptions().GetGrammarInfo().IsPassiveVoiceEnabled())) );
        m_grammarPropertyGrid->SetPropertyAttribute(
            BaseProjectView::GetPassiveLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_grammarPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetPassiveLabel(), _(L"Check this to include passive voice in the results."));

        m_grammarPropertyGrid->Append(
            new wxBoolProperty(BaseProjectView::GetSentenceStartingWithConjunctionsTabLabel(), wxPG_LABEL,
            (m_readabilityProjectDoc ?
                m_readabilityProjectDoc->GetGrammarInfo().IsConjunctionStartingSentencesEnabled() :
                wxGetApp().GetAppOptions().GetGrammarInfo().IsConjunctionStartingSentencesEnabled())) );
        m_grammarPropertyGrid->SetPropertyAttribute(
            BaseProjectView::GetSentenceStartingWithConjunctionsTabLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_grammarPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetSentenceStartingWithConjunctionsTabLabel(),
            _(L"Check this to include conjunction-starting sentences in the results."));

        m_grammarPropertyGrid->Append(
            new wxBoolProperty(BaseProjectView::GetSentenceStartingWithLowercaseTabLabel(), wxPG_LABEL,
            (m_readabilityProjectDoc ?
                m_readabilityProjectDoc->GetGrammarInfo().IsLowercaseSentencesEnabled() :
                wxGetApp().GetAppOptions().GetGrammarInfo().IsLowercaseSentencesEnabled())) );
        m_grammarPropertyGrid->SetPropertyAttribute(
            BaseProjectView::GetSentenceStartingWithLowercaseTabLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_grammarPropertyGrid->SetPropertyHelpString(
            BaseProjectView::GetSentenceStartingWithLowercaseTabLabel(),
            _(L"Check this to include lowercased sentences in the results."));

        // spell checker section
        m_grammarPropertyGrid->Append(new wxPropertyCategory(GetSpellCheckerLabel()) );
        m_grammarPropertyGrid->SetPropertyHelpString(
            GetSpellCheckerLabel(),
            _(L"The options in this section select what the spell checker should ignore."));

        m_grammarPropertyGrid->Append(
            new wxBoolProperty(GetIgnoreProperNounsLabel(), wxPG_LABEL,
            (m_readabilityProjectDoc ?
                m_readabilityProjectDoc->SpellCheckIsIgnoringProperNouns() :
                wxGetApp().GetAppOptions().SpellCheckIsIgnoringProperNouns())));
        m_grammarPropertyGrid->SetPropertyAttribute(
            GetIgnoreProperNounsLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_grammarPropertyGrid->SetPropertyHelpString(
            GetIgnoreProperNounsLabel(),
            _(L"Check this to not spell check proper nouns. This is useful for excluding names "
               "and places that the spell checker may not recognize."));

        m_grammarPropertyGrid->Append(
            new wxBoolProperty(GetIgnoreUppercasedWordsLabel(), wxPG_LABEL,
            (m_readabilityProjectDoc ?
                m_readabilityProjectDoc->SpellCheckIsIgnoringUppercased() :
                wxGetApp().GetAppOptions().SpellCheckIsIgnoringUppercased())));
        m_grammarPropertyGrid->SetPropertyAttribute(
            GetIgnoreUppercasedWordsLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_grammarPropertyGrid->SetPropertyHelpString(
            GetIgnoreUppercasedWordsLabel(),
            _(L"Check this to not spell check uppercased words (e.g., \"FORTRAN\" or \"S.C.U.B.A.\")."));

        m_grammarPropertyGrid->Append(
            new wxBoolProperty(GetIgnoreNumeralsLabel(), wxPG_LABEL,
            (m_readabilityProjectDoc ?
                m_readabilityProjectDoc->SpellCheckIsIgnoringNumerals() :
                wxGetApp().GetAppOptions().SpellCheckIsIgnoringNumerals())));
        m_grammarPropertyGrid->SetPropertyAttribute(
            GetIgnoreNumeralsLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_grammarPropertyGrid->SetPropertyHelpString(
            GetIgnoreNumeralsLabel(),
            _(L"Check this to not spell check numerals. Numerals are words that consist of 50% (or more) numbers."));

        m_grammarPropertyGrid->Append(
            new wxBoolProperty(GetIgnoreFileAddressesLabel(), wxPG_LABEL,
            (m_readabilityProjectDoc ?
                m_readabilityProjectDoc->SpellCheckIsIgnoringFileAddresses() :
                wxGetApp().GetAppOptions().SpellCheckIsIgnoringFileAddresses())));
        m_grammarPropertyGrid->SetPropertyAttribute(
            GetIgnoreFileAddressesLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_grammarPropertyGrid->SetPropertyHelpString(
            GetIgnoreFileAddressesLabel(), _(L"Check this to not spell check file paths and website addresses."));

        m_grammarPropertyGrid->Append(
            new wxBoolProperty(GetIgnoreProgrammerCodeLabel(), wxPG_LABEL,
            (m_readabilityProjectDoc ?
                m_readabilityProjectDoc->SpellCheckIsIgnoringProgrammerCode() :
                wxGetApp().GetAppOptions().SpellCheckIsIgnoringProgrammerCode())));
        m_grammarPropertyGrid->SetPropertyAttribute(
            GetIgnoreProgrammerCodeLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_grammarPropertyGrid->SetPropertyHelpString(
            GetIgnoreProgrammerCodeLabel(),
            _(L"Check this to not spell check words with mixed upper and lower case words. "
               "This is useful for excluding programming syntax which may appear in computer-related documents."));

        m_grammarPropertyGrid->Append(
            new wxBoolProperty(GetAllowColloquialismsLabel(), wxPG_LABEL,
            (m_readabilityProjectDoc ?
                m_readabilityProjectDoc->SpellCheckIsAllowingColloquialisms() :
                wxGetApp().GetAppOptions().SpellCheckIsAllowingColloquialisms())));
        m_grammarPropertyGrid->SetPropertyAttribute(
            GetAllowColloquialismsLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_grammarPropertyGrid->SetPropertyHelpString(
            GetAllowColloquialismsLabel(),
            _(L"Check this to allow colloquialisms, such as \"ing\" "
               "at the end of a word being replaced with \"in'\"."));

        m_grammarPropertyGrid->Append(
            new wxBoolProperty(GetIgnoreSocialMediaLabel(), wxPG_LABEL,
            (m_readabilityProjectDoc ?
                m_readabilityProjectDoc->SpellCheckIsIgnoringSocialMediaTags() :
                wxGetApp().GetAppOptions().SpellCheckIsIgnoringSocialMediaTags())));
        m_grammarPropertyGrid->SetPropertyAttribute(
            GetIgnoreSocialMediaLabel(), wxPG_BOOL_USE_CHECKBOX, true);
        m_grammarPropertyGrid->SetPropertyHelpString(
            GetIgnoreSocialMediaLabel(), _(L"Check this to ignore hashtags, such as #ReadabilityFormulasRock."));

        pgMan->SelectProperty(GetSpellCheckerLabel());
        pgMan->SetDescBoxHeight(FromDIP(wxSize(200, 200)).GetHeight());

        panelSizer->Add(pgMan, 1, wxEXPAND);
        }

    // text window options page (these options only apply to general options and standard projects)
    if ((GetSectionsBeingShown() & TextSection) && !IsBatchProjectSettings())
        {
        wxPanel* Panel = new wxPanel(m_sideBar, DOCUMENT_DISPLAY_GENERAL_PAGE, wxDefaultPosition, wxDefaultSize,
                                     wxTAB_TRAVERSAL);

        m_sideBar->AddPage(Panel, BaseProjectView::GetHighlightedReportsLabel(), DOCUMENT_DISPLAY_GENERAL_PAGE,
            // if the only section being shown, then show this page
            (GetSectionsBeingShown() == TextSection),
            0);

        wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);
        Panel->SetSizer(panelSizer);
        m_sideBar->AddSubPage(Panel, GetGeneralLabel(), DOCUMENT_DISPLAY_GENERAL_PAGE, false, 9);

        CreateLabelHeader(Panel, panelSizer, _(L"Formatting"), true);

        // sizer associated with static box holding controls
        wxSizer* optionsSizer = new wxBoxSizer(wxVERTICAL);
        panelSizer->Add(optionsSizer, 0, wxLEFT, OPTION_INDENT_SIZE);

        // font
        m_FontButton = new wxButton(Panel, ID_FONT_BUTTON, _(L"Font"));
        m_FontButton->SetBitmap(fontImage);
        optionsSizer->Add(m_FontButton, 0, wxALIGN_TOP);

        optionsSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

        CreateLabelHeader(Panel, panelSizer, _(L"Highlighting"), true);

        wxBoxSizer* optionsIndentSizer = new wxBoxSizer(wxVERTICAL);
        panelSizer->Add(optionsIndentSizer, 0, wxLEFT, OPTION_INDENT_SIZE);

        // highlight color for excluded text
            {
            m_excludedHighlightColorButton =
                new wxButton(Panel, ID_EXCLUDED_HIGHLIGHT_COLOR_BUTTON, _(L"Excluded text color"));
            m_excludedHighlightColorButton->SetBitmap(ResourceManager::CreateColorIcon(m_excludedTextHighlightColor));
            optionsIndentSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
            optionsIndentSizer->Add(m_excludedHighlightColorButton, 0, wxEXPAND);
            }

        // highlight color
            {
            m_highlightColorButton =
                new wxButton(Panel, ID_HIGHLIGHT_COLOR_BUTTON, _(L"Difficult words and sentences color"));
            m_highlightColorButton->SetBitmap(ResourceManager::CreateColorIcon(m_highlightedColor));
            optionsIndentSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
            optionsIndentSizer->Add(m_highlightColorButton, 0, wxEXPAND);
            }

        // highlight color for grammar errors
            {
            m_duplicateWordHighlightColorButton =
                new wxButton(Panel, ID_DUP_WORD_COLOR_BUTTON, _(L"Grammar errors color"));
            m_duplicateWordHighlightColorButton->SetBitmap(
                ResourceManager::CreateColorIcon(m_duplicateWordHighlightColor));
            optionsIndentSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
            optionsIndentSizer->Add(m_duplicateWordHighlightColorButton, 0, wxEXPAND);
            }

        // highlight color for wordy items
            {
            m_wordyPhraseHighlightColorButton =
                new wxButton(Panel, ID_WORDY_PHRASE_COLOR_BUTTON, _(L"Wordy items color"));
            m_wordyPhraseHighlightColorButton->SetBitmap(
                ResourceManager::CreateColorIcon(m_wordyPhraseHighlightColor));
            optionsIndentSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
            optionsIndentSizer->Add(m_wordyPhraseHighlightColorButton, 0, wxEXPAND);
            }

        wxBoxSizer* highlightSizer = new wxBoxSizer(wxHORIZONTAL);
        highlightSizer->Add(
            new wxStaticText(Panel, wxID_STATIC, _(L"Highlight text by changing its:")), 0,
                             wxALIGN_CENTER_VERTICAL|wxRIGHT, wxSizerFlags::GetDefaultBorder());

        wxArrayString highlightOptions;
        highlightOptions.Add(_(L"Background color"));
        highlightOptions.Add(_(L"Font color"));
        wxChoice* highlightCombo = new wxChoice(Panel, ID_PARAGRAPH_PARSE,
                                            wxDefaultPosition, wxDefaultSize, highlightOptions,
                                            0, wxGenericValidator(&m_textHighlightMethod));
        highlightSizer->Add(highlightCombo, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, wxSizerFlags::GetDefaultBorder());
        panelSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
        panelSizer->Add(highlightSizer, wxSizerFlags().Border(wxLEFT, OPTION_INDENT_SIZE));

        // dolch tab
        if (IsGeneralSettings() || m_projectLanguage == static_cast<int>(readability::test_language::english_test))
            {
            Panel = new wxPanel(m_sideBar, DOCUMENT_DISPLAY_DOLCH_PAGE,
                                wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
            panelSizer = new wxBoxSizer(wxVERTICAL);
            Panel->SetSizer(panelSizer);
            m_sideBar->AddSubPage(Panel, GetDolchSightWordsLabel(), DOCUMENT_DISPLAY_DOLCH_PAGE, false, 9);

            CreateLabelHeader(Panel, panelSizer, _(L"Highlighting"), true);
            panelSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

            // sizer associated with static box holding controls
            optionsIndentSizer = new wxBoxSizer(wxVERTICAL);
            panelSizer->Add(optionsIndentSizer, 0, wxLEFT, OPTION_INDENT_SIZE);

            // Conjunctions color
                {
                wxBoxSizer* rowSizer = new wxBoxSizer(wxHORIZONTAL);
                optionsIndentSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
                optionsIndentSizer->Add(rowSizer, 0, wxEXPAND);

                m_DolchConjunctionsColorButton =
                    new wxBitmapButton(Panel, ID_DOLCH_CONJUNCTIONS_COLOR_BUTTON,
                        ResourceManager::CreateColorIcon(m_dolchConjunctionsColor));
                // add them to the sizer
                rowSizer->Add(new wxCheckBox(Panel, wxID_ANY, _(L"Dolch conjunctions color:"),
                        wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_highlightDolchConjunctions) ),
                    1, wxALIGN_CENTER_VERTICAL);
                rowSizer->Add(m_DolchConjunctionsColorButton,
                    wxSizerFlags(0).Align(wxRight).Border(wxLEFT, wxSizerFlags::GetDefaultBorder()));
                }
            // Prepositions color
                {
                wxBoxSizer* rowSizer = new wxBoxSizer(wxHORIZONTAL);
                optionsIndentSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
                optionsIndentSizer->Add(rowSizer, 0, wxEXPAND);

                m_DolchPrepositionsColorButton =
                    new wxBitmapButton(Panel, ID_DOLCH_PREPOSITIONS_COLOR_BUTTON,
                        ResourceManager::CreateColorIcon(m_dolchPrepositionsColor));
                // add them to the sizer
                rowSizer->Add(new wxCheckBox(Panel, wxID_ANY, _(L"Dolch prepositions color:"),
                        wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_highlightDolchPrepositions) ),
                    1, wxALIGN_CENTER_VERTICAL);
                rowSizer->Add(m_DolchPrepositionsColorButton,
                    wxSizerFlags(0).Align(wxRight).Border(wxLEFT, wxSizerFlags::GetDefaultBorder()));
                }
            // pronouns
                {
                wxBoxSizer* rowSizer = new wxBoxSizer(wxHORIZONTAL);
                optionsIndentSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
                optionsIndentSizer->Add(rowSizer, 0, wxEXPAND);

                m_DolchPronounsColorButton =
                    new wxBitmapButton(Panel, ID_DOLCH_PRONOUNS_COLOR_BUTTON,
                        ResourceManager::CreateColorIcon(m_dolchPronounsColor));
                // add them to the sizer
                rowSizer->Add(new wxCheckBox(Panel, wxID_ANY, _(L"Dolch pronouns color:"),
                    wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_highlightDolchPronouns) ),
                    1, wxALIGN_CENTER_VERTICAL);
                rowSizer->Add(m_DolchPronounsColorButton,
                    wxSizerFlags(0).Align(wxRight).Border(wxLEFT, wxSizerFlags::GetDefaultBorder()));
                }
            // adverbs
                {
                wxBoxSizer* rowSizer = new wxBoxSizer(wxHORIZONTAL);
                optionsIndentSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
                optionsIndentSizer->Add(rowSizer, 0, wxEXPAND);

                m_DolchAdverbsColorButton =
                    new wxBitmapButton(Panel, ID_DOLCH_ADVERBS_COLOR_BUTTON,
                        ResourceManager::CreateColorIcon(m_dolchAdverbsColor));
                // add them to the sizer
                rowSizer->Add(new wxCheckBox(Panel, wxID_ANY, _(L"Dolch adverbs color:"),
                    wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_highlightDolchAdverbs) ),
                    1, wxALIGN_CENTER_VERTICAL);
                rowSizer->Add(m_DolchAdverbsColorButton,
                    wxSizerFlags(0).Align(wxRight).Border(wxLEFT, wxSizerFlags::GetDefaultBorder()));
                }
            // adjectives
                {
                wxBoxSizer* rowSizer = new wxBoxSizer(wxHORIZONTAL);
                optionsIndentSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
                optionsIndentSizer->Add(rowSizer, 0, wxEXPAND);

                m_DolchAdjectivesColorButton =
                    new wxBitmapButton(Panel, ID_DOLCH_ADJECTIVES_COLOR_BUTTON,
                        ResourceManager::CreateColorIcon(m_dolchAdjectivesColor));
                // add them to the sizer
                rowSizer->Add(new wxCheckBox(Panel, wxID_ANY, _(L"Dolch adjectives color:"),
                    wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_highlightDolchAdjectives) ),
                    1, wxALIGN_CENTER_VERTICAL);
                rowSizer->Add(m_DolchAdjectivesColorButton,
                    wxSizerFlags(0).Align(wxRight).Border(wxLEFT, wxSizerFlags::GetDefaultBorder()));
                }
            // verbs
                {
                wxBoxSizer* rowSizer = new wxBoxSizer(wxHORIZONTAL);
                optionsIndentSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
                optionsIndentSizer->Add(rowSizer, 0, wxEXPAND);

                m_DolchVerbsColorButton =
                    new wxBitmapButton(Panel, ID_DOLCH_VERBS_COLOR_BUTTON,
                        ResourceManager::CreateColorIcon(m_dolchVerbsColor));
                // add them to the sizer
                rowSizer->Add(new wxCheckBox(Panel, wxID_ANY, _(L"Dolch verbs color:"),
                    wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_highlightDolchVerbs) ),
                    1, wxALIGN_CENTER_VERTICAL);
                rowSizer->Add(m_DolchVerbsColorButton,
                    wxSizerFlags(0).Align(wxRight).Border(wxLEFT, wxSizerFlags::GetDefaultBorder()));
                }
            // noun color
                {
                wxBoxSizer* rowSizer = new wxBoxSizer(wxHORIZONTAL);
                optionsIndentSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
                optionsIndentSizer->Add(rowSizer, 0, wxEXPAND);

                m_DolchNounColorButton =
                    new wxBitmapButton(Panel, ID_DOLCH_NOUN_COLOR_BUTTON,
                        ResourceManager::CreateColorIcon(m_dolchNounColor));
                // add them to the sizer
                rowSizer->Add(new wxCheckBox(Panel, wxID_ANY, _(L"Dolch nouns color:"),
                    wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_highlightDolchNouns) ),
                    1, wxALIGN_CENTER_VERTICAL);
                rowSizer->Add(m_DolchNounColorButton,
                    wxSizerFlags(0).Align(wxRight).Border(wxLEFT, wxSizerFlags::GetDefaultBorder()));
                }
            }
        }

    // graphs options page
    CreateGraphSection();

    mainSizer->Add(CreateButtonSizer(wxOK|wxCANCEL|wxHELP), 0, wxEXPAND|wxALL, wxSizerFlags::GetDefaultBorder());

    if (IsGeneralSettings())
        {
        auto banner = new wxBannerWindow(this, wxTOP);
        banner->SetText(wxString::Format(_(L"%s Options"), wxGetApp().GetAppDisplayName()),
            _(L"These options will only affect new projects.\n"
               "To change an existing project, click \"Properties\" on the Home tab."));
        banner->SetGradient(ColorBrewer::GetColor(Color::White), ColorBrewer::GetColor(Color::PastelGray));

        mainSizer->Insert(0, banner, wxSizerFlags().Expand());
        }
    else
        {
        wxString displayableProjectName = m_readabilityProjectDoc ?
            m_readabilityProjectDoc->GetTitle() : wxString{};
        if (displayableProjectName.length() >= 50)
            { displayableProjectName.Truncate(49).Append(static_cast<wchar_t>(8230)); }

        auto banner = new wxBannerWindow(this, wxTOP);
        banner->SetText(_(L"Project Properties"),
            wxString::Format(
                _(L"These options only affect the current project (\"%s\").\n"
                   "To change options for future projects, click \"Options\" on the Tools tab."),
                displayableProjectName));
        banner->SetGradient(ColorBrewer::GetColor(Color::White), ColorBrewer::GetColor(Color::PastelGray));

        mainSizer->Insert(0, banner, wxSizerFlags().Expand());
        }

    SetSizerAndFit(mainSizer);

    // need to fit these columns after everything else was resized
    if (IsBatchProjectSettings() && GetFileList())
        {
        GetFileList()->SetColumnWidth(0, GetFileList()->GetClientSize().GetWidth()*.75);
        GetFileList()->SetColumnWidth(1, GetFileList()->GetClientSize().GetWidth()*.25);
        }

    if (m_readabilityTestsPropertyGrid)
        { m_readabilityTestsPropertyGrid->FitColumns(); }
    if (m_gradeLevelPropertyGrid)
        { m_gradeLevelPropertyGrid->FitColumns(); }
    if (m_grammarPropertyGrid)
        { m_grammarPropertyGrid->FitColumns(); }
    if (m_wordsBreakdownPropertyGrid)
        { m_wordsBreakdownPropertyGrid->FitColumns(); }
    if (m_sentencesBreakdownPropertyGrid)
        { m_sentencesBreakdownPropertyGrid->FitColumns(); }
    if (m_statisticsPropertyGrid)
        { m_statisticsPropertyGrid->FitColumns(); }
    if (m_generalGraphPropertyGrid)
        { m_generalGraphPropertyGrid->FitColumns(); }
    if (m_readabilityGraphPropertyGrid)
        { m_readabilityGraphPropertyGrid->FitColumns(); }
    if (m_barChartPropertyGrid)
        { m_barChartPropertyGrid->FitColumns(); }
    if (m_histogramPropertyGrid)
        { m_histogramPropertyGrid->FitColumns(); }
    if (m_boxPlotsPropertyGrid)
        { m_boxPlotsPropertyGrid->FitColumns(); }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::CreateGraphSection()
    {
    if (GetSectionsBeingShown() & GraphsSection)
        {
        wxPanel* GraphGeneralPage =
            new wxPanel(m_sideBar, GRAPH_GENERAL_PAGE, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
        m_sideBar->AddPage(GraphGeneralPage, GetGraphsLabel(), GRAPH_GENERAL_PAGE,
            // if the only section being shown, then show this page
            (GetSectionsBeingShown() == GraphsSection),
            7);

        // General tab
        //-----------
            {
            wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);
            GraphGeneralPage->SetSizer(panelSizer);
            m_sideBar->AddSubPage(GraphGeneralPage, GetGeneralLabel(), GRAPH_GENERAL_PAGE, false, 9);

            wxPropertyGridManager* pgMan =
                new wxPropertyGridManager(GraphGeneralPage, ID_GRAPH_OPTIONS_PROPERTYGRID,
                                          wxDefaultPosition, wxDefaultSize,
                                          wxPG_BOLD_MODIFIED|wxPG_DESCRIPTION|wxPGMAN_DEFAULT_STYLE);
            m_generalGraphPropertyGrid = pgMan->AddPage();

            m_generalGraphPropertyGrid->Append(new wxPropertyCategory(GetGraphBackgroundLabel()) );
            m_generalGraphPropertyGrid->SetPropertyHelpString(
                GetGraphBackgroundLabel(),
                _(L"The options in this section customize the backgrounds of the graphs."));
            // color
            auto backgroundColorProp = m_generalGraphPropertyGrid->Append(
                new wxColourProperty(GetColorLabel(), GetBackgroundColorLabel(),
                    (m_readabilityProjectDoc ?
                        m_readabilityProjectDoc->GetBackGroundColor() :
                        wxGetApp().GetAppOptions().GetBackGroundColor())));
            backgroundColorProp->SetHelpString(
                _(L"Selects the color for the graphs' background. "
                   "Note that if you are displaying an image, then the image will be shown on top of this color."));
            // color fade
            auto colorFadeProp = backgroundColorProp->AppendChild(
                new wxBoolProperty(GetApplyFadeLabel(), wxPG_LABEL,
                (m_readabilityProjectDoc ?
                    m_readabilityProjectDoc->GetGraphBackGroundLinearGradient() :
                    wxGetApp().GetAppOptions().GetGraphBackGroundLinearGradient())));
            colorFadeProp->SetAttribute(wxPG_BOOL_USE_CHECKBOX, true);
            colorFadeProp->SetHelpString(
                _(L"Check this to apply a downward fade to the background color of your graphs. "
                   "The background of your graphs will fade (top-to-bottom) starting with the color "
                   "that you have selected into white."));
            // image
            wxImageFileProperty* backgroundImage =
                new wxImageFileProperty(GetImageLabel(), wxPG_LABEL,
                (m_readabilityProjectDoc ?
                    m_readabilityProjectDoc->GetBackGroundImagePath() :
                    wxGetApp().GetAppOptions().GetBackGroundImagePath()));
            backgroundImage->SetAttribute(wxPG_FILE_WILDCARD,wxGetApp().GetAppOptions().IMAGE_LOAD_FILE_FILTER);
            backgroundImage->SetAttribute(wxPG_DIALOG_TITLE, _(L"Select Background Image"));
            backgroundImage->SetAttribute(wxPG_ATTR_HINT, _(L"Select an image"));
            backgroundImage->SetHelpString(_(L"Selects the image for the graphs' background."));

            // image effect
            wxPGChoices imgEffects;
            imgEffects.Add(_(L"No effect"));
            imgEffects.Add(_(L"Grayscale"));
            imgEffects.Add(_(L"Blur horizontally"));
            imgEffects.Add(_(L"Blur vertically"));
            imgEffects.Add(_(L"Sepia"));
            imgEffects.Add(_(L"Frosted glass"));
            imgEffects.Add(_(L"Oil painting"));
            auto imgEffectProp = backgroundImage->AppendChild(
                new wxEnumProperty(GetEffectLabel(), wxPG_LABEL, imgEffects,
                    (m_readabilityProjectDoc ?
                        static_cast<int>(m_readabilityProjectDoc->GetBackGroundImageEffect()) :
                        static_cast<int>(wxGetApp().GetAppOptions().GetBackGroundImageEffect()))));
            imgEffectProp->SetHelpString(
                _(L"Applies an effect to the background image."));

            // image opacity
            auto imgOpacityProp = backgroundImage->AppendChild(
                new wxIntProperty(GetOpacityLabel(), wxPG_LABEL,
                (m_readabilityProjectDoc ?
                    m_readabilityProjectDoc->GetGraphBackGroundOpacity() :
                    wxGetApp().GetAppOptions().GetGraphBackGroundOpacity())) );
            imgOpacityProp->SetEditor(wxPGEditor_SpinCtrl);
            imgOpacityProp->SetAttribute(wxPG_ATTR_MIN, 0);
            imgOpacityProp->SetAttribute(wxPG_ATTR_MAX, 255);
            imgOpacityProp->SetHelpString(
                _(L"Sets the transparency of the background image. "
                   "A value of 255 will set the background to be fully opaque, whereas 0 will set "
                   "the background to be transparent."));

            m_generalGraphPropertyGrid->Append(backgroundImage);
            // set the default folder to the global image folder if no image provided
            if (m_generalGraphPropertyGrid->GetPropertyValueAsString(GetImageLabel()).empty())
                {
                backgroundImage->SetAttribute(wxPG_FILE_INITIAL_PATH, wxGetApp().GetAppOptions().GetImagePath());
                }

            // plot area
            m_generalGraphPropertyGrid->Append(new wxPropertyCategory(GetPlotAreaBackgroundLabel()) );
            m_generalGraphPropertyGrid->SetPropertyHelpString(
                GetPlotAreaBackgroundLabel(),
                _(L"The options in this section customize the plot area backgrounds of the graphs."));
            // color
            m_generalGraphPropertyGrid->Append(
                new wxColourProperty(GetColorLabel(), wxPG_LABEL,
                (m_readabilityProjectDoc ?
                    m_readabilityProjectDoc->GetPlotBackGroundColor() :
                    wxGetApp().GetAppOptions().GetPlotBackGroundColor())));
            m_generalGraphPropertyGrid->SetPropertyHelpString(
                GetColorLabel(), _(L"Selects the color for the plot area background of the graphs."));
            // opacity
            m_generalGraphPropertyGrid->Append(new wxIntProperty(GetOpacityLabel(), wxPG_LABEL,
                (m_readabilityProjectDoc ?
                    m_readabilityProjectDoc->GetGraphPlotBackGroundOpacity() :
                    wxGetApp().GetAppOptions().GetGraphPlotBackGroundOpacity())) );
            m_generalGraphPropertyGrid->SetPropertyEditor(GetOpacityLabel(), wxPGEditor_SpinCtrl);
            m_generalGraphPropertyGrid->SetPropertyAttribute(GetOpacityLabel(), wxPG_ATTR_MIN, 0);
            m_generalGraphPropertyGrid->SetPropertyAttribute(GetOpacityLabel(), wxPG_ATTR_MAX, 255);
            m_generalGraphPropertyGrid->SetPropertyHelpString(
                GetOpacityLabel(),
                _(L"Sets the transparency of the plot background. "
                   "A value of 255 will set the background to be fully opaque, whereas 0 will set the background "
                   "to be transparent."));

            m_generalGraphPropertyGrid->Append(new wxPropertyCategory(GetWatermarksLogosLabel()) );
            m_generalGraphPropertyGrid->SetPropertyHelpString(
                GetWatermarksLogosLabel(),
                _(L"The options in this section customize the watermarks and logo images of the graphs."));
            // watermark
            m_generalGraphPropertyGrid->Append(new wxStringProperty(GetWatermarkLabel(), wxPG_LABEL,
                (m_readabilityProjectDoc ?
                    m_readabilityProjectDoc->GetWatermark() :
                    wxGetApp().GetAppOptions().GetWatermark())));
            m_generalGraphPropertyGrid->SetPropertyAttribute(GetWatermarkLabel(), wxPG_ATTR_HINT, _(L"Enter text"));
            m_generalGraphPropertyGrid->SetPropertyHelpString(
                GetWatermarkLabel(),
                _(L"Enter a label to be stamped across your graphs into this field. "
                   "This label will be stamped diagonally, top left-hand corner to bottom right-hand corner."));
            // logo
            wxImageFileProperty* graphLogo = new wxImageFileProperty(GetLogoImageLabel(), wxPG_LABEL,
                (m_readabilityProjectDoc ?
                    m_readabilityProjectDoc->GetWatermarkLogoPath() :
                    wxGetApp().GetAppOptions().GetWatermarkLogo()));
            graphLogo->SetAttribute(wxPG_FILE_WILDCARD,wxGetApp().GetAppOptions().IMAGE_LOAD_FILE_FILTER);
            graphLogo->SetAttribute(wxPG_DIALOG_TITLE,_(L"Select Logo Image"));
            graphLogo->SetAttribute(wxPG_ATTR_HINT, _(L"Select an image"));
            m_generalGraphPropertyGrid->Append(graphLogo);
            // set the default folder to the global image folder if no image provided
            if (m_generalGraphPropertyGrid->GetPropertyValueAsString(GetLogoImageLabel()).empty())
                {
                graphLogo->SetAttribute(wxPG_FILE_INITIAL_PATH, wxGetApp().GetAppOptions().GetImagePath());
                }
            m_generalGraphPropertyGrid->SetPropertyHelpString(
                GetLogoImageLabel(),
                _(L"Enter the path to the image file used as your graphs' logo into this field. "
                   "The logo will be displayed in the lower right-hand corner of each graph."));

            m_generalGraphPropertyGrid->Append(new wxPropertyCategory(GetEffectsLabel()) );
            m_generalGraphPropertyGrid->SetPropertyHelpString(
                GetEffectsLabel(), _(L"The options in this section customize various visual effects of the graphs."));
            // stipple brush
            wxImageFileProperty* customBrushProp =
                new wxImageFileProperty(GetCustomImageBrushLabel(), wxPG_LABEL,
                (m_readabilityProjectDoc ?
                    m_readabilityProjectDoc->GetStippleImagePath() :
                    wxGetApp().GetAppOptions().GetGraphStippleImagePath()));
            customBrushProp->SetAttribute(wxPG_FILE_WILDCARD ,wxGetApp().GetAppOptions().IMAGE_LOAD_FILE_FILTER);
            customBrushProp->SetAttribute(wxPG_DIALOG_TITLE, _(L"Select Stipple Brush Image"));
            customBrushProp->SetAttribute(wxPG_ATTR_HINT, _(L"Select an image"));
            m_generalGraphPropertyGrid->Append(customBrushProp);
            m_generalGraphPropertyGrid->SetPropertyHelpString(
                GetCustomImageBrushLabel(),
                _(L"Enter into this field the file path to the image used for a stipple brush. "
                   "A stipple image can be used to draw stacked images across bars and boxes"));
            // set the default folder to the global image folder if no image provided
            if (m_generalGraphPropertyGrid->GetPropertyValueAsString(GetCustomImageBrushLabel()).empty())
                {
                customBrushProp->SetAttribute(wxPG_FILE_INITIAL_PATH, wxGetApp().GetAppOptions().GetImagePath());
                }
            // drop shadows
            m_generalGraphPropertyGrid->Append(
                new wxBoolProperty(GetDisplayDropShadowsLabel(), wxPG_LABEL,
                (m_readabilityProjectDoc ?
                    m_readabilityProjectDoc->IsDisplayingDropShadows() :
                    wxGetApp().GetAppOptions().IsDisplayingDropShadows())));
            m_generalGraphPropertyGrid->SetPropertyAttribute(
                GetDisplayDropShadowsLabel(), wxPG_BOOL_USE_CHECKBOX, true);
            m_generalGraphPropertyGrid->SetPropertyHelpString(
                GetDisplayDropShadowsLabel(),
                _(L"Check this to include shadows underneath items such as boxes, bars, and labels. "
                   "Unchecking this option will give your graphs a more \"flat\" look."));

            pgMan->SelectProperty(GetGraphBackgroundLabel());

            panelSizer->Add(pgMan, 1, wxEXPAND);
            }

        // Axes tab
        //-----------
            {
            wxPanel* Panel = new wxPanel(m_sideBar, GRAPH_AXIS_PAGE, wxDefaultPosition,
                                         wxDefaultSize, wxTAB_TRAVERSAL);
            wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);
            wxBoxSizer* optionsSizer = new wxBoxSizer(wxVERTICAL);
            Panel->SetSizer(panelSizer);
            m_sideBar->AddSubPage(Panel, GetAxisSettingsLabel(), GRAPH_AXIS_PAGE, false, 9);

            CreateLabelHeader(Panel, panelSizer, _(L"Fonts:"), true);
            m_xAxisFontButton = new wxButton(Panel, ID_X_AXIS_FONT_BUTTON, _(L"X axis"));
            m_xAxisFontButton->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/x-axis.svg"));
            optionsSizer->Add(m_xAxisFontButton, 0, wxLEFT|wxEXPAND, wxSizerFlags::GetDefaultBorder()*2);

            m_yAxisFontButton = new wxButton(Panel, ID_Y_AXIS_FONT_BUTTON, _(L"Y axis"));
            m_yAxisFontButton->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/y-axis.svg"));
            optionsSizer->Add(m_yAxisFontButton, 0, wxLEFT|wxEXPAND, wxSizerFlags::GetDefaultBorder()*2);

            panelSizer->Add(optionsSizer);
            }

        // Titles tab
        //-----------
            {
            wxPanel* Panel = new wxPanel(m_sideBar, GRAPH_TITLES_PAGE, wxDefaultPosition,
                                         wxDefaultSize, wxTAB_TRAVERSAL);
            wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);
            wxBoxSizer* optionsSizer = new wxBoxSizer(wxVERTICAL);
            Panel->SetSizer(panelSizer);
            m_sideBar->AddSubPage(Panel, GetTitlesLabel(), GRAPH_TITLES_PAGE, false, 9);

            CreateLabelHeader(Panel, panelSizer, _(L"Fonts:"), true);
            m_graphTopTitleFontButton = new wxButton(Panel, ID_GRAPH_TOP_TITLE_FONT_BUTTON, _(L"Top titles"));
            m_graphTopTitleFontButton->SetBitmap(
                wxGetApp().GetResourceManager().GetSVG(L"ribbon/top-titles.svg"));
            optionsSizer->Add(m_graphTopTitleFontButton, 0, wxLEFT|wxEXPAND, wxSizerFlags::GetDefaultBorder()*2);

            m_graphBottomTitleFontButton = new wxButton(Panel, ID_GRAPH_BOTTOM_TITLE_FONT_BUTTON, _(L"Bottom titles"));
            m_graphBottomTitleFontButton->SetBitmap(
                wxGetApp().GetResourceManager().GetSVG(L"ribbon/bottom-titles.svg"));
            optionsSizer->Add(m_graphBottomTitleFontButton, 0, wxLEFT|wxEXPAND, wxSizerFlags::GetDefaultBorder()*2);

            m_graphLeftTitleFontButton = new wxButton(Panel, ID_GRAPH_LEFT_TITLE_FONT_BUTTON, _(L"Left titles"));
            m_graphLeftTitleFontButton->SetBitmap(
                wxGetApp().GetResourceManager().GetSVG(L"ribbon/left-titles.svg"));
            optionsSizer->Add(m_graphLeftTitleFontButton, 0, wxLEFT|wxEXPAND, wxSizerFlags::GetDefaultBorder()*2);

            m_graphRightTitleFontButton = new wxButton(Panel, ID_GRAPH_RIGHT_TITLE_FONT_BUTTON, _(L"Right titles"));
            m_graphRightTitleFontButton->SetBitmap(
                wxGetApp().GetResourceManager().GetSVG(L"ribbon/right-titles.svg"));
            optionsSizer->Add(m_graphRightTitleFontButton, 0, wxLEFT|wxEXPAND, wxSizerFlags::GetDefaultBorder()*2);

            panelSizer->Add(optionsSizer);
            }

        // Readability Graphs tab
        //-----------
            {
            wxPanel* Panel = new wxPanel(m_sideBar, GRAPH_READABILITY_GRAPHS_PAGE, wxDefaultPosition,
                                         wxDefaultSize, wxTAB_TRAVERSAL);
            wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);
            Panel->SetSizer(panelSizer);
            m_sideBar->AddSubPage(Panel, GetReadabilityGraphLabel(), GRAPH_READABILITY_GRAPHS_PAGE, false, 9);

            // Fry-like graphs
            wxPropertyGridManager* pgMan =
                new wxPropertyGridManager(Panel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                          wxPG_BOLD_MODIFIED|wxPG_DESCRIPTION|wxPGMAN_DEFAULT_STYLE);
            m_readabilityGraphPropertyGrid = pgMan->AddPage();
            m_readabilityGraphPropertyGrid->Append(new wxPropertyCategory(GetFryLikeLabel()) );
            m_readabilityGraphPropertyGrid->SetPropertyHelpString(
                GetFryLikeLabel(),
                _(L"The options in this section customize the Fry, Gilliam-Pe\U000000F1a-Mountain, "
                   "Raygor, and Schwartz graphs."));
            m_readabilityGraphPropertyGrid->Append(
                new wxColourProperty(GetInvalidRegionsColorLabel(), wxPG_LABEL,
                (m_readabilityProjectDoc ?
                    m_readabilityProjectDoc->GetInvalidAreaColor() :
                    wxGetApp().GetAppOptions().GetInvalidAreaColor())));
            m_readabilityGraphPropertyGrid->SetPropertyHelpString(
                GetInvalidRegionsColorLabel(),
                _(L"Selects the color for the invalid sentence/word regions."));

            wxPGChoices raygorStyles;
            raygorStyles.Add(_(L"Original"));
            raygorStyles.Add(_(L"Baldwin-Kaufman"));
            raygorStyles.Add(_(L"Modern"));
            m_readabilityGraphPropertyGrid->Append(
                new wxEnumProperty(GetRaygorStyleLabel(), wxPG_LABEL, raygorStyles,
                (m_readabilityProjectDoc ?
                    static_cast<int>(m_readabilityProjectDoc->GetRaygorStyle()) :
                    static_cast<int>(wxGetApp().GetAppOptions().GetRaygorStyle()))) );
            m_readabilityGraphPropertyGrid->SetPropertyHelpString(
                GetRaygorStyleLabel(),
                _(L"Selects the layout style of the Raygor graph."));
            // flesch
            m_readabilityGraphPropertyGrid->Append(
                new wxPropertyCategory(GetFleschChartLabel()) );
            m_readabilityGraphPropertyGrid->SetPropertyHelpString(
                GetFleschChartLabel(),
                _(L"The options in this section customize the Flesch chart."));
            m_readabilityGraphPropertyGrid->Append(
                new wxBoolProperty(GetFleschChartConnectPointsLabel(), wxPG_LABEL,
                (m_readabilityProjectDoc ?
                    m_readabilityProjectDoc->IsConnectingFleschPoints() :
                    wxGetApp().GetAppOptions().IsConnectingFleschPoints())));
            m_readabilityGraphPropertyGrid->SetPropertyAttribute(
                GetFleschChartConnectPointsLabel(), wxPG_BOOL_USE_CHECKBOX, true);
            m_readabilityGraphPropertyGrid->SetPropertyHelpString(
                GetFleschChartConnectPointsLabel(),
                _(L"Check this to connect the factor and score points."));

            m_readabilityGraphPropertyGrid->Append(
                new wxBoolProperty(GetFleschSyllableRulerDocGroupsLabel(), wxPG_LABEL,
                (m_readabilityProjectDoc ?
                    m_readabilityProjectDoc->IsIncludingFleschRulerDocGroups() :
                    wxGetApp().GetAppOptions().IsIncludingFleschRulerDocGroups())));
            m_readabilityGraphPropertyGrid->SetPropertyAttribute(
                GetFleschSyllableRulerDocGroupsLabel(), wxPG_BOOL_USE_CHECKBOX, true);
            m_readabilityGraphPropertyGrid->SetPropertyHelpString(
                GetFleschSyllableRulerDocGroupsLabel(),
                _(L"Check this to display document names grouped along the syllable ruler."));

            // Lix gauge
            m_readabilityGraphPropertyGrid->Append(new wxPropertyCategory(GetLixGaugeLabel()) );
            m_readabilityGraphPropertyGrid->SetPropertyHelpString(
                GetLixGaugeLabel(), _(L"The options in this section customize the Lix Gauge."));
            m_readabilityGraphPropertyGrid->Append(
                new wxBoolProperty(GetUseEnglishLabelsForGermanLixLabel(), wxPG_LABEL,
                (m_readabilityProjectDoc ?
                    m_readabilityProjectDoc->IsUsingEnglishLabelsForGermanLix() :
                    wxGetApp().GetAppOptions().IsUsingEnglishLabelsForGermanLix())));
            m_readabilityGraphPropertyGrid->SetPropertyAttribute(
                GetUseEnglishLabelsForGermanLixLabel(), wxPG_BOOL_USE_CHECKBOX, true);
            m_readabilityGraphPropertyGrid->SetPropertyHelpString(
                GetUseEnglishLabelsForGermanLixLabel(),
                _(L"Check this to display the English translated bracket labels on the German Lix gauge."));

            pgMan->SelectProperty(GetFryLikeLabel());

            panelSizer->Add(pgMan, 1, wxEXPAND);
            }

        // Bar Chart tab
        //-----------
        if (!IsBatchProjectSettings())
            {
            wxPanel* Panel = new wxPanel(m_sideBar, GRAPH_BAR_CHART_PAGE, wxDefaultPosition,
                                         wxDefaultSize, wxTAB_TRAVERSAL);
            wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);
            Panel->SetSizer(panelSizer);
            m_sideBar->AddSubPage(Panel, GetBarChartLabel(), GRAPH_BAR_CHART_PAGE, false, 9);

            wxPropertyGridManager* pgMan =
                new wxPropertyGridManager(Panel, ID_BARCHART_OPTIONS_PROPERTYGRID, wxDefaultPosition, wxDefaultSize,
                                          wxPG_BOLD_MODIFIED|wxPG_DESCRIPTION|wxPGMAN_DEFAULT_STYLE);
            m_barChartPropertyGrid = pgMan->AddPage();
            m_barChartPropertyGrid->Append(new wxPropertyCategory(GetBarAppearanceLabel()) );
            m_barChartPropertyGrid->SetPropertyHelpString(
                GetBarAppearanceLabel(),
                _(L"The options in this section customize the display of the bars."));

            // color for bars
            m_barChartPropertyGrid->Append(
                new wxColourProperty(GetColorLabel(), wxPG_LABEL,
                (m_readabilityProjectDoc ?
                    m_readabilityProjectDoc->GetBarChartBarColor() :
                    wxGetApp().GetAppOptions().GetBarChartBarColor())));
            m_barChartPropertyGrid->SetPropertyHelpString(
                GetColorLabel(), _(L"Selects the color used for the bars."));
            // effects
            wxPGChoices barEffects;
            barEffects.Add(_(L"Solid"), wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-solid.svg"));
            barEffects.Add(_(L"Glass effect"), wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-glass.svg"));
            barEffects.Add(_(L"Color fade, bottom to top"),
                wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-bottom-to-top.svg"));
            barEffects.Add(_(L"Color fade, top to bottom"),
                wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-top-to-bottom.svg"));
            barEffects.Add(_(L"Stipple image"),
                wxGetApp().GetResourceManager().GetSVG(L"ribbon/brush.svg"));
            m_barChartPropertyGrid->Append(
                new wxEnumProperty(GetEffectLabel(), wxPG_LABEL, barEffects,
                (m_readabilityProjectDoc ?
                    static_cast<int>(m_readabilityProjectDoc->GetGraphBarEffect()) :
                    static_cast<int>(wxGetApp().GetAppOptions().GetGraphBarEffect()))) );
            m_barChartPropertyGrid->SetPropertyHelpString(
                GetEffectLabel(),
                _(L"Selects which effect to apply to the bars."));
            // bar opacity
            m_barChartPropertyGrid->Append(
                new wxIntProperty(GetOpacityLabel(), wxPG_LABEL,
                (m_readabilityProjectDoc ?
                    static_cast<int>(m_readabilityProjectDoc->GetGraphBarOpacity()) :
                    static_cast<int>(wxGetApp().GetAppOptions().GetGraphBarOpacity()))) );
            m_barChartPropertyGrid->SetPropertyEditor(GetOpacityLabel(), wxPGEditor_SpinCtrl);
            m_barChartPropertyGrid->SetPropertyAttribute(GetOpacityLabel(), wxPG_ATTR_MIN, 0);
            m_barChartPropertyGrid->SetPropertyAttribute(GetOpacityLabel(), wxPG_ATTR_MAX, 255);
            m_barChartPropertyGrid->SetPropertyHelpString(
                GetOpacityLabel(),
                _(L"Sets the transparency of the bars. A value of 255 will set the box to be fully opaque, "
                   "whereas 0 will set the bars to be transparent."));
            // orientation
            wxPGChoices orientation;
            orientation.Add(_(L"Horizontal"), wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-chart.svg"));
            orientation.Add(_(L"Vertical"), wxGetApp().GetResourceManager().GetSVG(L"ribbon/histogram.svg"));
            m_barChartPropertyGrid->Append(
                new wxEnumProperty(GeOrientationLabel(), wxPG_LABEL, orientation,
                (m_readabilityProjectDoc ?
                    static_cast<int>(m_readabilityProjectDoc->GetBarChartOrientation()) :
                    static_cast<int>(wxGetApp().GetAppOptions().GetBarChartOrientation()))) );
            m_barChartPropertyGrid->SetPropertyHelpString(
                GeOrientationLabel(),
                _(L"Selects whether to display the bars horizontally or vertically."));

            // labels
            m_barChartPropertyGrid->Append(
                new wxBoolProperty(GetLabelsOnBarsLabel(), wxPG_LABEL,
                (m_readabilityProjectDoc ?
                    m_readabilityProjectDoc->IsDisplayingBarLabels() :
                    wxGetApp().GetAppOptions().IsDisplayingBarLabels())));
            m_barChartPropertyGrid->SetPropertyAttribute(GetLabelsOnBarsLabel(), wxPG_BOOL_USE_CHECKBOX, true);
            m_barChartPropertyGrid->SetPropertyHelpString(
                GetLabelsOnBarsLabel(), _(L"Check this to display labels on the bars."));

            pgMan->SelectProperty(GetBarAppearanceLabel());

            panelSizer->Add(pgMan, 1, wxEXPAND);
            }

        // Histogram tab
        //-----------
            {
            wxPanel* Panel = new wxPanel(m_sideBar, GRAPH_HISTOGRAM_PAGE, wxDefaultPosition,
                                         wxDefaultSize, wxTAB_TRAVERSAL);
            wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);
            Panel->SetSizer(panelSizer);
            m_sideBar->AddSubPage(Panel, GetHistogramsLabel(), GRAPH_HISTOGRAM_PAGE, false, 9);

            wxPropertyGridManager* pgMan =
                new wxPropertyGridManager(Panel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                          wxPG_BOLD_MODIFIED|wxPG_DESCRIPTION|wxPGMAN_DEFAULT_STYLE);
            m_histogramPropertyGrid = pgMan->AddPage();
            m_histogramPropertyGrid->Append(new wxPropertyCategory(GetBarAppearanceLabel()) );
            m_histogramPropertyGrid->SetPropertyHelpString(
                GetBarAppearanceLabel(), _(L"The options in this section customize the display of the bars."));
            // color for bars
            m_histogramPropertyGrid->Append(
                new wxColourProperty(GetColorLabel(), wxPG_LABEL,
                (m_readabilityProjectDoc ?
                    m_readabilityProjectDoc->GetHistogramBarColor() :
                    wxGetApp().GetAppOptions().GetHistogramBarColor())));
            m_histogramPropertyGrid->SetPropertyHelpString(
                GetColorLabel(), _(L"Selects the color used for the bars."));
            // effects
            wxPGChoices histoBarEffects;
            histoBarEffects.Add(
                _(L"Solid"),
                wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-solid.svg"));
            histoBarEffects.Add(
                _(L"Glass effect"), wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-glass.svg"));
            histoBarEffects.Add(
                _(L"Color fade, bottom to top"),
                wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-bottom-to-top.svg"));
            histoBarEffects.Add(
                _(L"Color fade, top to bottom"),
                wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-top-to-bottom.svg"));
            histoBarEffects.Add(
                _(L"Stipple image"),
                wxGetApp().GetResourceManager().GetSVG(L"ribbon/brush.svg"));
            m_histogramPropertyGrid->Append(
                new wxEnumProperty(GetEffectLabel(), wxPG_LABEL, histoBarEffects,
                (m_readabilityProjectDoc ?
                    static_cast<int>(m_readabilityProjectDoc->GetHistogramBarEffect()) :
                    static_cast<int>(wxGetApp().GetAppOptions().GetHistogramBarEffect()))) );
            m_histogramPropertyGrid->SetPropertyHelpString(
                GetEffectLabel(), _(L"Selects which effect to apply to the bars."));
            // bar opacity
            m_histogramPropertyGrid->Append(
                new wxIntProperty(GetOpacityLabel(), wxPG_LABEL,
                (m_readabilityProjectDoc ?
                    m_readabilityProjectDoc->GetHistogramBarOpacity() :
                    wxGetApp().GetAppOptions().GetHistogramBarOpacity())) );
            m_histogramPropertyGrid->SetPropertyEditor(GetOpacityLabel(), wxPGEditor_SpinCtrl);
            m_histogramPropertyGrid->SetPropertyAttribute(GetOpacityLabel(), wxPG_ATTR_MIN, 0);
            m_histogramPropertyGrid->SetPropertyAttribute(GetOpacityLabel(), wxPG_ATTR_MAX, 255);
            m_histogramPropertyGrid->SetPropertyHelpString(
                GetOpacityLabel(),
                _(L"Sets the transparency of the bars. A value of 255 will set the box to be fully opaque, "
                   "whereas 0 will set the bars to be transparent."));

            m_histogramPropertyGrid->Append(new wxPropertyCategory(GetBinningOptionsLabel()) );
            m_histogramPropertyGrid->SetPropertyHelpString(
                GetBinningOptionsLabel(),
                _(L"The options in this section control how the data are categorized."));

            if (IsBatchProjectSettings() || IsGeneralSettings())
                {
                // sorting
                wxPGChoices sortChoices;
                wxArrayString categories;
                categories.Add(_(L"Create a bin for each unique value"));
                categories.Add(_(L"Sort by interval"));
                categories.Add(_(L"Sort by neat interval"));
                sortChoices.Add(categories);
                m_histogramPropertyGrid->Append(
                    new wxEnumProperty(GetBinSortingLabel(), wxPG_LABEL, sortChoices,
                    (m_readabilityProjectDoc ?
                        static_cast<int>(m_readabilityProjectDoc->GetHistorgramBinningMethod()) :
                        static_cast<int>(wxGetApp().GetAppOptions().GetHistorgramBinningMethod()))) );
                m_histogramPropertyGrid->SetPropertyHelpString(
                    GetBinSortingLabel(),
                    _(L"Bin sorting refers to how values (e.g., index and grade scores) are categorized "
                       "into separate classes. Each class (or bin) is displayed on a histogram as a bar. "
                       "The options listed here control how these bins are created and how your "
                       "data are sorted into them."));

                // rounding
                wxPGChoices roundingChoices;
                roundingChoices.Add(_(L"Round"));
                roundingChoices.Add(_(L"Round down"));
                roundingChoices.Add(_(L"Round up"));
                roundingChoices.Add(_(L"Do not round"));
                m_histogramPropertyGrid->Append(
                    new wxEnumProperty(GetGradeLevelRoundingLabel(), wxPG_LABEL, roundingChoices,
                    (m_readabilityProjectDoc ?
                        static_cast<int>(m_readabilityProjectDoc->GetHistogramRoundingMethod()) :
                        static_cast<int>(wxGetApp().GetAppOptions().GetHistogramRoundingMethod()))) );
                m_histogramPropertyGrid->SetPropertyHelpString(
                    GetGradeLevelRoundingLabel(),
                    _(L"This option controls how floating-point values are rounded when being sorted into bins."));
                }

            m_histogramPropertyGrid->Append(new wxPropertyCategory(GetBinDisplayLabel()) );
            m_histogramPropertyGrid->SetPropertyHelpString(
                GetBinDisplayLabel(),
                _(L"The options in this section control how the bars are labeled."));

            if (IsBatchProjectSettings() || IsGeneralSettings())
                {
                   wxPGChoices intervalTypes;
                intervalTypes.Add(_(L"Cutpoints"));
                intervalTypes.Add(_(L"Midpoints"));
                m_histogramPropertyGrid->Append(
                    new wxEnumProperty(GetIntervalDisplayLabel(), wxPG_LABEL, intervalTypes,
                    (m_readabilityProjectDoc ?
                        static_cast<int>(m_readabilityProjectDoc->GetHistogramIntervalDisplay()) :
                        static_cast<int>(wxGetApp().GetAppOptions().GetHistogramIntervalDisplay()))) );
                m_histogramPropertyGrid->SetPropertyHelpString(
                    GetIntervalDisplayLabel(),
                    _(L"Specifies how to display the bars' values range on the axis."));
                }

            wxPGChoices catLabelTypes;
            catLabelTypes.Add(_(L"Counts"));
            catLabelTypes.Add(_(L"Percentages"));
            catLabelTypes.Add(_(L"Counts & percentages"));
            catLabelTypes.Add(_(L"No labels"));
            m_histogramPropertyGrid->Append(
                new wxEnumProperty(GetBinLabelsLabel(), wxPG_LABEL, catLabelTypes,
                (m_readabilityProjectDoc ?
                    static_cast<int>(m_readabilityProjectDoc->GetHistrogramBinLabelDisplay()) :
                    static_cast<int>(wxGetApp().GetAppOptions().GetHistrogramBinLabelDisplay()))) );
            m_histogramPropertyGrid->SetPropertyHelpString(
                GetBinLabelsLabel(),
                _(L"Specifies what to display on the bars' labels."));

            pgMan->SelectProperty(GetBarAppearanceLabel());

            panelSizer->Add(pgMan, 1, wxEXPAND);
            }

        // Box Plot tab
        //-----------
            {
            wxPanel* Panel =
                new wxPanel(m_sideBar, GRAPH_BOX_PLOT_PAGE, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
            wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);
            Panel->SetSizer(panelSizer);
            m_sideBar->AddSubPage(Panel, GetBoxPlotLabel(), GRAPH_BOX_PLOT_PAGE, false, 9);

            wxPropertyGridManager* pgMan =
                new wxPropertyGridManager(Panel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                          wxPG_BOLD_MODIFIED|wxPG_DESCRIPTION|wxPGMAN_DEFAULT_STYLE);
            m_boxPlotsPropertyGrid = pgMan->AddPage();
            m_boxPlotsPropertyGrid->Append(new wxPropertyCategory(GetBoxAppearanceLabel()) );
            m_boxPlotsPropertyGrid->SetPropertyHelpString(GetBoxAppearanceLabel(),
                _(L"The options in this section customize the display of the box(es)."));
            // color for box
            m_boxPlotsPropertyGrid->Append(
                new wxColourProperty(GetColorLabel(), wxPG_LABEL,
                (m_readabilityProjectDoc ?
                    m_readabilityProjectDoc->GetGraphBoxColor() :
                    wxGetApp().GetAppOptions().GetGraphBoxColor())));
            m_boxPlotsPropertyGrid->SetPropertyHelpString(
                GetColorLabel(), _(L"Selects the color used for the boxes."));
            // effects
            wxPGChoices barEffects;
            barEffects.Add(_(L"Solid"),
                wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-solid.svg"));
            barEffects.Add(_(L"Glass effect"),
                wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-glass.svg"));
            barEffects.Add(_(L"Color fade, left to right"),
                wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-left-to-right.svg"));
            barEffects.Add(_(L"Color fade, right to left"),
                wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-right-to-left.svg"));
            barEffects.Add(_(L"Stipple image"),
                wxGetApp().GetResourceManager().GetSVG(L"ribbon/brush.svg"));
            m_boxPlotsPropertyGrid->Append(
                new wxEnumProperty(GetEffectLabel(), wxPG_LABEL, barEffects,
                (m_readabilityProjectDoc ?
                    static_cast<int>(m_readabilityProjectDoc->GetGraphBoxEffect()) :
                    static_cast<int>(wxGetApp().GetAppOptions().GetGraphBoxEffect()))) );
            m_boxPlotsPropertyGrid->SetPropertyHelpString(GetEffectLabel(),
                _(L"Selects which effect to apply to the boxes."));
            // box opacity
            m_boxPlotsPropertyGrid->Append(
                new wxIntProperty(GetOpacityLabel(), wxPG_LABEL,
                (m_readabilityProjectDoc ?
                    m_readabilityProjectDoc->GetGraphBoxOpacity() :
                    wxGetApp().GetAppOptions().GetGraphBoxOpacity())) );
            m_boxPlotsPropertyGrid->SetPropertyEditor(GetOpacityLabel(), wxPGEditor_SpinCtrl);
            m_boxPlotsPropertyGrid->SetPropertyAttribute(GetOpacityLabel(), wxPG_ATTR_MIN, 0);
            m_boxPlotsPropertyGrid->SetPropertyAttribute(GetOpacityLabel(), wxPG_ATTR_MAX, 255);
            m_boxPlotsPropertyGrid->SetPropertyHelpString(
                GetOpacityLabel(),
                _(L"Sets the transparency of the box. A value of 255 will set the box to be fully opaque, "
                   "whereas 0 will set the box to be transparent."));

            m_boxPlotsPropertyGrid->Append(new wxPropertyCategory(GetBoxOptionsLabel()) );
            m_boxPlotsPropertyGrid->SetPropertyHelpString(
                GetBoxOptionsLabel(),
                _(L"The options in this section customize the display of the labels and data."));

            m_boxPlotsPropertyGrid->Append(
                new wxBoolProperty(GetLabelsOnBoxesLabel(), wxPG_LABEL,
                (m_readabilityProjectDoc ?
                    m_readabilityProjectDoc->IsDisplayingBoxPlotLabels() :
                    wxGetApp().GetAppOptions().IsDisplayingBoxPlotLabels())));
            m_boxPlotsPropertyGrid->SetPropertyAttribute(GetLabelsOnBoxesLabel(), wxPG_BOOL_USE_CHECKBOX, true);
            m_boxPlotsPropertyGrid->SetPropertyHelpString(
                GetLabelsOnBoxesLabel(),
                _(L"Check this to display labels on the middle points, upper/lower control limits, and whiskers."));

            m_boxPlotsPropertyGrid->Append(
                new wxBoolProperty(GetConnectBoxesLabel(), wxPG_LABEL,
                (m_readabilityProjectDoc ?
                    m_readabilityProjectDoc->IsConnectingBoxPlotMiddlePoints() :
                    wxGetApp().GetAppOptions().IsConnectingBoxPlotMiddlePoints())));
            m_boxPlotsPropertyGrid->SetPropertyAttribute(GetConnectBoxesLabel(), wxPG_BOOL_USE_CHECKBOX, true);
            m_boxPlotsPropertyGrid->SetPropertyHelpString(
                GetConnectBoxesLabel(),
                _(L"Check this to display a line connecting the middle points of each box. "
                   "This only applies to plots with multiple boxes."));

            m_boxPlotsPropertyGrid->Append(
                new wxBoolProperty(GetShowAllDataPointsLabel(), wxPG_LABEL,
                (m_readabilityProjectDoc ?
                    m_readabilityProjectDoc->IsShowingAllBoxPlotPoints() :
                    wxGetApp().GetAppOptions().IsShowingAllBoxPlotPoints())));
            m_boxPlotsPropertyGrid->SetPropertyAttribute(GetShowAllDataPointsLabel(), wxPG_BOOL_USE_CHECKBOX, true);
            m_boxPlotsPropertyGrid->SetPropertyHelpString(
                GetShowAllDataPointsLabel(),
                _(L"Check this to display all data points on the box and whiskers. "
                   "If this is unchecked, then only outliers will be shown."));

            pgMan->SelectProperty(GetBoxAppearanceLabel());

            panelSizer->Add(pgMan, 1, wxEXPAND);
            }
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnContextHelp([[maybe_unused]] wxHelpEvent& event)
    {
    wxCommandEvent cmd;
    OnHelp(cmd);
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::OnHelp([[maybe_unused]] wxCommandEvent& event)
    {
    wxString topic;
    const wxWindow* page = m_sideBar->GetCurrentPage();
    if (page)
        {
        switch (page->GetId())
            {
        case GENERAL_SETTINGS_PAGE:
            topic = L"options-general-settings.html";
            break;
        case DOCUMENT_DISPLAY_GENERAL_PAGE:
            topic = L"options-highlighted-reports.html";
            break;
        case PROJECT_SETTINGS_PAGE:
            topic = L"project-settings.html";
            break;
        case DOCUMENT_DISPLAY_DOLCH_PAGE:
            topic = L"dolch-options.html";
            break;
        case SCORES_TEST_OPTIONS_PAGE:
            [[fallthrough]];
        case SCORES_DISPLAY_PAGE:
            topic = L"options-scores.html";
            break;
        case ANALYSIS_INDEXING_PAGE:
            topic = L"document-analysis.html";
            break;
        case GRAMMAR_PAGE:
            topic = L"options-grammar.html";
            break;
        case ANALYSIS_STATISTICS_PAGE:
            topic = L"options-statistics.html";
            break;
        case GRAPH_TITLES_PAGE:
            [[fallthrough]];
        case GRAPH_AXIS_PAGE:
            [[fallthrough]];
        case GRAPH_GENERAL_PAGE:
            topic = L"options-graphs.html";
            break;
        case GRAPH_READABILITY_GRAPHS_PAGE:
            topic = L"options-readability-graphs.html";
            break;
        case GRAPH_BAR_CHART_PAGE:
            topic = L"options-bar-charts.html";
            break;
        case GRAPH_HISTOGRAM_PAGE:
            topic = L"options-histograms.html";
            break;
        case GRAPH_BOX_PLOT_PAGE:
            topic = L"options-box-plots.html";
            break;
        case WORDS_BREAKDOWN_PAGE:
            topic = L"options-words-breakdown.html";
            break;
        case SENTENCES_BREAKDOWN_PAGE:
            topic = L"options-sentences-breakdown.html";
            break;
        default:
            topic = L"index.html";
            };
        wxLaunchDefaultBrowser(
            wxFileName::FileNameToURL(wxGetApp().GetMainFrame()->GetHelpDirectory() +
                                      wxFileName::GetPathSeparator() + topic));
        }
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::CreateLabelHeader(wxWindow* parent, wxSizer* parentSizer, const wxString& title,
                                        const bool addSidePadding /*= false*/) const
    {
    assert(parent && parentSizer);
    if (!parent || !parentSizer)
        { return; }
    // The title
    wxBoxSizer* titleSizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* titleText =
        new wxStaticText(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
    titleSizer->Add(titleText);
    titleSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
    // The static line
    titleSizer->Add(new wxStaticLine(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL),
                                     1, wxALIGN_CENTER_VERTICAL);
    if (addSidePadding)
        { parentSizer->Add(titleSizer, 0, wxEXPAND|wxLEFT|wxRIGHT, wxSizerFlags::GetDefaultBorder()); }
    else
        { parentSizer->Add(titleSizer, 0, wxEXPAND); }
    parentSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
    }

//-------------------------------------------------------------
void ToolsOptionsDlg::SelectPage(const int pageId)
    {
    for (size_t i = 0; i < m_sideBar->GetPageCount(); ++i)
        {
        const wxWindow* page = m_sideBar->GetPage(i);
        if (page && page->GetId() == pageId)
            {
            m_sideBar->SetSelection(i);
            }
        }
    }

//-------------------------------------------------------------
bool ToolsOptionsDlg::IsGeneralSettings() const noexcept
    { return !m_readabilityProjectDoc; }

//-------------------------------------------------------------
bool ToolsOptionsDlg::IsStandardProjectSettings() const
    { return (m_readabilityProjectDoc && m_readabilityProjectDoc->IsKindOf(CLASSINFO(ProjectDoc))); }

//-------------------------------------------------------------
bool ToolsOptionsDlg::IsBatchProjectSettings() const
    { return (m_readabilityProjectDoc && m_readabilityProjectDoc->IsKindOf(CLASSINFO(BatchProjectDoc))); }

//-------------------------------------------------------------
wxString ToolsOptionsDlg::ExpandPath(wxString path) const
    {
    if (path.starts_with(_DT(L"@[EXAMPLESDIR]")))
        {
        path.Replace(_DT(L"@[EXAMPLESDIR]"),
            wxGetApp().FindResourceDirectory(_DT(L"Examples")));
        }
    return path;
    }
