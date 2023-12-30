///////////////////////////////////////////////////////////////////////////////
// Name:        new_custom_test_simple_dlg.cpp
// Author:      Blake Madden
// Copyright:   (c) 2005-2023 Blake Madden
// Licence:     3-Clause BSD licence
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "new_custom_test_simple_dlg.h"
#include "../../app/readability_app.h"

wxDECLARE_APP(ReadabilityApp);

//-------------------------------------------------------------
void NewCustomWordTestSimpleDlg::OnOK([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();

    m_testName.Trim(true);
    m_testName.Trim(false);
    if (m_testName.empty())
        {
        wxMessageBox(_(L"Please select a valid test name."), _(L"Test Name"),
                     wxICON_EXCLAMATION | wxOK, this);
        return;
        }
    else if (m_wordListFilePath.empty() || !wxFileName::FileExists(m_wordListFilePath))
        {
        wxMessageBox(_(L"Please select a valid file path."), _(L"Invalid File"),
                     wxICON_EXCLAMATION | wxOK, this);
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

//-------------------------------------------------------------
void NewCustomWordTestSimpleDlg::OnBrowseForFileClick([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();
    wxFileDialog dialog(this, _(L"Select Word List File"),
                        m_wordListFilePath.length() ? wxString{} :
                                                      wxGetApp().GetAppOptions().GetWordListPath(),
                        m_wordListFilePath, _(L"Text files (*.txt)|*.txt"),
                        wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);
    if (dialog.ShowModal() != wxID_OK)
        {
        return;
        }

    m_wordListFilePath = dialog.GetPath();
    wxGetApp().GetAppOptions().SetWordListPath(wxFileName(m_wordListFilePath).GetPath());
    TransferDataToWindow();
    }

//-------------------------------------------------------------
void NewCustomWordTestSimpleDlg::CreateControls()
    {
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    wxStaticBoxSizer* testNameBoxSizer =
        new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _(L"Test name:")), wxVERTICAL);
    wxStaticBoxSizer* fileBrowseBoxSizer = new wxStaticBoxSizer(
        new wxStaticBox(this, wxID_ANY, _(L"Custom familiar word list:")), wxHORIZONTAL);
    mainSizer->Add(testNameBoxSizer, 0, wxEXPAND | wxALL, wxSizerFlags::GetDefaultBorder());
    mainSizer->Add(fileBrowseBoxSizer, 0, wxEXPAND | wxALL, wxSizerFlags::GetDefaultBorder());

    wxTextCtrl* testNameEdit =
        new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, FromDIP(wxSize(400, 25)),
                       wxBORDER_THEME, wxGenericValidator(&m_testName));
    testNameBoxSizer->Add(testNameEdit, 1, wxEXPAND);

    wxTextCtrl* filePathEdit =
        new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
                       wxSize(FromDIP(wxSize(400, 400)).GetWidth(), -1), wxBORDER_THEME,
                       wxGenericValidator(&m_wordListFilePath));
    filePathEdit->AutoCompleteFileNames();
    fileBrowseBoxSizer->Add(filePathEdit, 1, wxEXPAND);

    wxBitmapButton* fileBrowseButton =
        new wxBitmapButton(this, ID_FOLDER_BROWSE_BUTTON,
                           wxArtProvider::GetBitmapBundle(wxART_FILE_OPEN, wxART_BUTTON));
    fileBrowseBoxSizer->Add(fileBrowseButton, 0, wxALIGN_CENTER_VERTICAL);

    mainSizer->Add(CreateSeparatedButtonSizer(wxOK | wxCANCEL | wxHELP), 0, wxEXPAND | wxALL,
                   wxSizerFlags::GetDefaultBorder());

    SetSizerAndFit(mainSizer);

    testNameEdit->SetFocus();
    }
