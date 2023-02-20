///////////////////////////////////////////////////////////////////////////////
// Name:        filtered_text_preview_dlg.cpp
// Author:      Blake Madden
// Copyright:   (c) 2005-2023 Blake Madden
// Licence:     3-Clause BSD licence
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "filtered_text_preview_dlg.h"

void FilteredTextPreviewDlg::CreateControls()
    {
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer* previewWindowSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(previewWindowSizer, 1, wxEXPAND|wxALL, wxSizerFlags::GetDefaultBorder());

    previewWindowSizer->Add(new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
                            FromDIP(wxSize(500,300)), wxTE_MULTILINE|wxTE_RICH2|wxTE_READONLY,
                            wxGenericValidator(&m_filteredValue) ), 1, wxEXPAND);

    previewWindowSizer->AddSpacer(wxSizerFlags::GetDefaultBorder() * 2);

    m_collpane = new wxCollapsiblePane(this, wxID_ANY, _("Details:"));
    previewWindowSizer->Add(m_collpane, 0, wxEXPAND);

    wxBoxSizer* filteredOutSizer = new wxBoxSizer(wxVERTICAL);
    filteredOutSizer->Add(new wxStaticText(m_collpane->GetPane(),
                          wxID_STATIC, _("The following are being filtered:")) );
    wxStaticText* infoLabel = new wxStaticText(m_collpane->GetPane(), wxID_STATIC, m_infoLabel);
    filteredOutSizer->Add(infoLabel, 0, wxEXPAND);
    m_collpane->GetPane()->SetSizer(filteredOutSizer);
    m_collpane->Expand();
    filteredOutSizer->SetSizeHints(m_collpane->GetPane());

    mainSizer->Add(CreateSeparatedButtonSizer(wxOK|wxCANCEL|wxHELP), 0,
                   wxEXPAND|wxALL, wxSizerFlags::GetDefaultBorder());

    SetSizerAndFit(mainSizer);
    }

void FilteredTextPreviewDlg::SetExclusionInfo(const InvalidSentence textExclusionMethod,
         const bool excludeCopyrightNotices,
         const bool excludeCitations,
         const bool replaceCharacters,
         const bool removeEllipses,
         const bool removeBullets,
         const bool removeFilePaths,
         const bool stripAbbreviationPeriods)
    {
    m_infoLabel.clear();
    if (textExclusionMethod != InvalidSentence::IncludeAsFullSentences)
        {
        if (textExclusionMethod == InvalidSentence::ExcludeFromAnalysis)
            { m_infoLabel += L"\n\t" + _("All incomplete sentences"); }
        else if (textExclusionMethod == InvalidSentence::ExcludeExceptForHeadings)
            { m_infoLabel += L"\n\t" + _("All incomplete sentences, except headings"); }
        if (excludeCopyrightNotices)
            { m_infoLabel += L"\n\t" + _("Trailing copyright notices"); }
        if (excludeCitations)
            { m_infoLabel += L"\n\t" + _("Trailing citations"); }
        }
    if (replaceCharacters)
        { m_infoLabel += L"\n\t" + _("Special characters"); }
    if (removeEllipses)
        { m_infoLabel += L"\n\t" + _("Ellipses"); }
    if (removeBullets)
        { m_infoLabel += L"\n\t" + _("Bullets and list-item numbering"); }
    if (removeFilePaths)
        { m_infoLabel += L"\n\t" + _("Internet and file addresses"); }
    if (stripAbbreviationPeriods)
        { m_infoLabel += L"\n\t" + _("Trailing periods from abbreviations and acronyms"); }
    }
