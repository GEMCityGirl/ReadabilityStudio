///////////////////////////////////////////////////////////////////////////////
// Name:        edit_text_dlg.cpp
// Author:      Blake Madden
// Copyright:   (c) 2005-2023 Blake Madden
// License:     3-Clause BSD license
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "edit_text_dlg.h"
#include "../../projects/base_project_doc.h"
#include "../../projects/standard_project_doc.h"
#include "../../projects/batch_project_doc.h"
#include "../../app/readability_app.h"

wxDECLARE_APP(ReadabilityApp);

//------------------------------------------------------
EditTextDlg::EditTextDlg(wxWindow* parent,
             BaseProjectDoc* parentDoc,
             wxWindowID id /*= wxID_ANY*/,
             const wxString& caption /*= _(L"Edit Text")*/,
             const wxString& description /*= wxString{}*/,
             const wxPoint& pos /*= wxDefaultPosition*/,
             const wxSize& size /*= wxSize(600, 600)*/,
             long style /*= wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER*/) : m_parentDoc(parentDoc)
    {
    SetBackgroundColour(wxGetApp().GetAppOptions().GetControlBackgroundColor());
    Create(parent, id, caption, description, pos, size, style);
    SetSize(m_parentDoc->GetFirstView()->GetFrame()->GetSize());
    Centre(wxCENTER_ON_SCREEN);

    Bind(wxEVT_CLOSE_WINDOW, &EditTextDlg::OnClose, this);
    Bind(wxEVT_TEXT, &EditTextDlg::OnTextChanged, this);

    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &EditTextDlg::OnSaveButton, this, wxID_SAVE);

    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &EditTextDlg::OnEditButtons, this, wxID_PASTE);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &EditTextDlg::OnEditButtons, this, wxID_CUT);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &EditTextDlg::OnEditButtons, this, wxID_COPY);

    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &EditTextDlg::OnEditButtons, this, wxID_UNDO);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &EditTextDlg::OnEditButtons, this, wxID_REDO);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &EditTextDlg::OnEditButtons, this, wxID_SELECTALL);
    }

//------------------------------------------------------
void EditTextDlg::CreateControls()
    {
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer* searchSizer = new wxBoxSizer(wxHORIZONTAL);
    searchSizer->AddStretchSpacer(1);
    mainSizer->Add(searchSizer, 0, wxEXPAND);

        {
        m_ribbon =
            new wxRibbonBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                            wxRIBBON_BAR_FLOW_HORIZONTAL);
        wxRibbonPage* homePage = new wxRibbonPage(m_ribbon, wxID_ANY, wxString{});
        // export
            {
            wxRibbonPanel* exportPage =
                new wxRibbonPanel(homePage, wxID_ANY, _DT(L" "),
                    wxNullBitmap, wxDefaultPosition, wxDefaultSize,
                    wxRIBBON_PANEL_NO_AUTO_MINIMISE);
            wxRibbonButtonBar* buttonBar = new wxRibbonButtonBar(exportPage, ID_DOCUMENT_RIBBON_BUTTON_BAR);
            buttonBar->AddButton(wxID_SAVE, _(L"Save"),
                wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_BUTTON,
                    FromDIP(wxSize(32, 32))).ConvertToImage(),
                _(L"Save the document."));
            }
        // clipboard
            {
            wxRibbonPanel* clipboardPage = new wxRibbonPanel(homePage, ID_CLIPBOARD_RIBBON_BUTTON_BAR, _(L"Clipboard"),
                wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
            wxRibbonButtonBar* buttonBar = new wxRibbonButtonBar(clipboardPage, ID_CLIPBOARD_RIBBON_BUTTON_BAR);
            buttonBar->AddButton(wxID_PASTE, _(L"Paste"),
                wxArtProvider::GetBitmap(wxART_PASTE, wxART_BUTTON,
                    FromDIP(wxSize(32, 32))).ConvertToImage(),
                _(L"Pastes the clipboard's content into the document."));
            buttonBar->AddButton(wxID_CUT, _(L"Cut"),
                wxArtProvider::GetBitmap(wxART_CUT, wxART_BUTTON,
                    FromDIP(wxSize(32, 32))).ConvertToImage(),
                _(L"Cuts the selection."));
            buttonBar->AddButton(wxID_COPY, _(L"Copy"),
                wxArtProvider::GetBitmap(wxART_COPY, wxART_BUTTON,
                    FromDIP(wxSize(32, 32))).ConvertToImage(),
                _(L"Copy the selection."));
            }
        // edit
            {
            wxRibbonPanel* editPage = new wxRibbonPanel(homePage, wxID_ANY, _(L"Edit"),
                wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
            wxRibbonButtonBar* buttonBar = new wxRibbonButtonBar(editPage, ID_EDIT_RIBBON_BUTTON_BAR);
            buttonBar->AddButton(wxID_UNDO, _(L"Undo"),
                wxArtProvider::GetBitmap(wxART_UNDO, wxART_BUTTON,
                    FromDIP(wxSize(32, 32))).ConvertToImage(),
                _(L"Undoes the last operation."));
            buttonBar->AddButton(wxID_REDO, _(L"Redo"),
                wxArtProvider::GetBitmap(wxART_REDO, wxART_BUTTON,
                    FromDIP(wxSize(32, 32))).ConvertToImage(),
                _(L"Repeats the last operation."));
            buttonBar->AddButton(wxID_SELECTALL, _(L"Select All"),
                wxArtProvider::GetBitmap(L"ID_SELECT_ALL", wxART_BUTTON,
                    FromDIP(wxSize(32, 32))).ConvertToImage(),
                _(L"Select all text."));
            }

        m_ribbon->SetArtProvider(new Wisteria::UI::RibbonMetroArtProvider);
        wxGetApp().UpdateRibbonTheme(m_ribbon);

        mainSizer->Add(m_ribbon, 0, wxEXPAND|wxALL, wxSizerFlags::GetDefaultBorder());
        m_ribbon->Realise();
        }

    m_textEntry = new FormattedTextCtrl(this, wxID_ANY,
                                 wxDefaultPosition, wxDefaultSize,
                                 wxTE_AUTO_URL|wxTE_PROCESS_TAB,
                                 wxGenericValidator(&m_value) );
    m_textEntry->SetFont(m_parentDoc->GetTextViewFont());
    m_textEntry->SetMargins(10, 10);
    m_textEntry->EnableProofCheck(wxTextProofOptions::Default().
        Language((m_parentDoc->GetProjectLanguage() == readability::test_language::spanish_test) ?
                _DT("es") :
                (m_parentDoc->GetProjectLanguage() == readability::test_language::german_test) ?
                _DT("de") :
                _DT("en")).
            SpellCheck(true).GrammarCheck(true));
    mainSizer->Add(m_textEntry, 1, wxEXPAND);

    if (m_description.length())
        {
        mainSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
        wxStaticText* label = new wxStaticText(this,wxID_ANY, m_description);
        mainSizer->Add(label, wxSizerFlags().Border(wxLEFT, wxSizerFlags::GetDefaultBorder()));
        }

    // batch uses this in modal mode
    if (m_parentDoc && m_parentDoc->IsKindOf(CLASSINFO(BatchProjectDoc)))
        {
        mainSizer->Add(CreateSeparatedButtonSizer(wxOK|wxCANCEL), 0,
                       wxEXPAND|wxALL, wxSizerFlags::GetDefaultBorder());
        }

    SetSizer(mainSizer);
    }

//------------------------------------------------------
void EditTextDlg::OnClose(wxCloseEvent& event)
    {
    if (m_textEntry && m_textEntry->IsModified())
        {
        if (wxMessageBox(_(L"Do you wish to save your unsaved changes?"),
                _(L"Save Changes"), wxYES_NO | wxICON_QUESTION) == wxYES)
            { Save(); }
        }
    event.Skip();
    }

//------------------------------------------------------
void EditTextDlg::OnSaveButton(wxRibbonButtonBarEvent& event)
    {
    Save();

    // mark the text control as not being dirty, so that if we close now
    // it won't need to ask about wanting to save
    if (m_textEntry)
        { m_textEntry->DiscardEdits(); }
    EnableSaveButton(false);
    }

//------------------------------------------------------
void EditTextDlg::OnEditButtons(wxRibbonButtonBarEvent& event)
    {
    if (m_textEntry)
        {
        if (event.GetId() == wxID_SELECTALL)
            { m_textEntry->SelectAll(); }
        else if (event.GetId() == wxID_UNDO)
            { m_textEntry->Undo(); }
        else if (event.GetId() == wxID_REDO)
            { m_textEntry->Redo(); }
        else if (event.GetId() == wxID_COPY)
            { m_textEntry->Copy(); }
        else if (event.GetId() == wxID_CUT)
            { m_textEntry->Cut(); }
        else if (event.GetId() == wxID_PASTE)
            { m_textEntry->Paste(); }
        }
    }

void EditTextDlg::OnTextChanged(wxCommandEvent& event)
    {
    EnableSaveButton(true);
    }

//------------------------------------------------------
void EditTextDlg::Save()
    {
    if (!TransferDataFromWindow())
        { return; }

    if (m_parentDoc && m_parentDoc->IsKindOf(CLASSINFO(ProjectDoc)))
        {
        auto projectDoc = dynamic_cast<ProjectDoc*>(m_parentDoc);
        assert(projectDoc && L"Bad cast to standard project!");
        if (projectDoc)
            {
            projectDoc->SetDocumentText(m_value);
            projectDoc->Modify(true);
            projectDoc->RefreshRequired(ProjectRefresh::FullReindexing);
            projectDoc->RefreshProject();
            projectDoc->Save();
            }
        }
    }

//------------------------------------------------------
void EditTextDlg::EnableSaveButton(const bool enable /*= true*/)
    {
    wxWindow* saveButtonBarWindow = m_ribbon->FindWindow(EditTextDlg::ID_DOCUMENT_RIBBON_BUTTON_BAR);
    if (saveButtonBarWindow && saveButtonBarWindow->IsKindOf(CLASSINFO(wxRibbonButtonBar)))
        {
        auto saveButtonBar = dynamic_cast<wxRibbonButtonBar*>(saveButtonBarWindow);
        assert(saveButtonBar && L"Error casting ribbon bar!");
        if (saveButtonBar)
            { saveButtonBar->EnableButton(wxID_SAVE, enable); }
        }
    }
