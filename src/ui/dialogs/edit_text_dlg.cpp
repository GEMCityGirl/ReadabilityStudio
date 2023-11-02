///////////////////////////////////////////////////////////////////////////////
// Name:        edit_text_dlg.cpp
// Author:      Blake Madden
// Copyright:   (c) 2005-2023 Blake Madden
// License:     3-Clause BSD license
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "edit_text_dlg.h"
#include "../../projects/base_project_doc.h"

void EditTextDlg::CreateControls()
    {
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    wxTextCtrl* textEntry = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                                           wxDefaultPosition, wxDefaultSize,
                                           wxTE_MULTILINE|wxTE_RICH2, wxGenericValidator(&m_value) );
    textEntry->EnableProofCheck(wxTextProofOptions::Default().
        Language((m_parentDoc->GetProjectLanguage() == readability::test_language::spanish_test) ?
                _DT("es") :
                (m_parentDoc->GetProjectLanguage() == readability::test_language::german_test) ?
                _DT("de") :
                _DT("en")).
            SpellCheck(true).GrammarCheck(true));
    mainSizer->Add(textEntry, 1, wxEXPAND);

    if (m_description.length())
        {
        mainSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
        wxStaticText* label = new wxStaticText(this,wxID_ANY, m_description);
        mainSizer->Add(label, wxSizerFlags().Border(wxLEFT, wxSizerFlags::GetDefaultBorder()));
        }

    mainSizer->Add(CreateSeparatedButtonSizer(wxOK|wxCANCEL), 0,
                   wxEXPAND|wxALL, wxSizerFlags::GetDefaultBorder());

    SetSizer(mainSizer);
    }
