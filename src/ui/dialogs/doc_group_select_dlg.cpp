///////////////////////////////////////////////////////////////////////////////
// Name:        doc_group_select_dlg.cpp
// Author:      Blake Madden
// Copyright:   (c) 2005-2023 Blake Madden
// Licence:     3-Clause BSD licence
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "doc_group_select_dlg.h"

//----------------------------------------------------------
void DocGroupSelectDlg::CreateControls()
    {
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    wxArrayString choices = { _(L"Use documents' descriptions (will be loaded during import)"),
                              _(L"Use a grouping label"),
                              _(L"Use the last common folder between files") };

    wxRadioBox* radioBox = new wxRadioBox(
        this, wxID_ANY, _(L"Select how to label the document(s):"), wxDefaultPosition,
        wxDefaultSize, choices, 0, wxRA_SPECIFY_ROWS, wxGenericValidator(&m_selected));

    mainSizer->Add(radioBox, 0, wxALIGN_LEFT | wxALL, wxSizerFlags::GetDefaultBorder());

    // label box
    wxBoxSizer* labelSizer = new wxBoxSizer(wxHORIZONTAL);
    m_groupingLabelText = new wxStaticText(this, wxID_STATIC, _(L"Grouping label:"));
    m_groupingLabelEntry =
        new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                       wxBORDER_THEME, wxGenericValidator(&m_groupingLabel));
    labelSizer->Add(m_groupingLabelText, 0, wxALIGN_CENTER_VERTICAL);
    labelSizer->Add(m_groupingLabelEntry, 1, wxEXPAND | wxALL, wxSizerFlags::GetDefaultBorder());

    m_groupingLabelText->Enable(m_selected == 1);
    m_groupingLabelEntry->Enable(m_selected == 1);

    mainSizer->Add(labelSizer, 1, wxEXPAND | wxALL, wxSizerFlags::GetDefaultBorder());

    // don't show this again
    wxCheckBox* checkBox = new wxCheckBox(
        this, wxID_ANY, _(L"Do not show this again (always use document descriptions)"),
        wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_doNotShowThisAgain));
    mainSizer->Add(checkBox, 0, wxALL, wxSizerFlags::GetDefaultBorder());

    mainSizer->Add(CreateSeparatedButtonSizer(wxOK | wxCANCEL | wxHELP), 0, wxEXPAND | wxALL,
                   wxSizerFlags::GetDefaultBorder());

    SetSizerAndFit(mainSizer);

    Bind(wxEVT_RADIOBOX, &DocGroupSelectDlg::OnRadioBoxChange, this);
    Bind(wxEVT_BUTTON, &DocGroupSelectDlg::OnOK, this, wxID_OK);
    }

//----------------------------------------------------------
void DocGroupSelectDlg::OnRadioBoxChange([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();
    m_groupingLabelText->Enable(m_selected == 1);
    m_groupingLabelEntry->Enable(m_selected == 1);
    }

//-------------------------------------------------------------
void DocGroupSelectDlg::OnOK([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();

    // trim off whitespace off of label
    m_groupingLabel.Trim(true).Trim(false);

    // validate the label
    if (m_groupingLabel.empty() && GetSelection() == 1)
        {
        wxMessageBox(_(L"Please enter a grouping label."), _(L"Error"), wxOK | wxICON_EXCLAMATION);
        return;
        }

    if (IsModal())
        {
        EndModal(wxID_OK);
        }
    else
        {
        Show(false);
        }
    }
