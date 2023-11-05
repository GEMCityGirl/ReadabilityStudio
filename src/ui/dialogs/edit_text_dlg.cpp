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

    Bind(wxEVT_CHAR_HOOK,
        [this](wxKeyEvent& event)
        {
            if (event.ControlDown() && event.GetKeyCode() == L'S')
            {
                wxRibbonButtonBarEvent dummyEvt;
                EditTextDlg::OnSaveButton(dummyEvt);
            }
            else if (event.ControlDown() && event.GetKeyCode() == L'F')
            {
                wxFindDialogEvent dummyEvt;
                EditTextDlg::OnShowFindDialog(dummyEvt);
            }
            else if (event.ControlDown() && event.GetKeyCode() == L'H')
            {
                wxFindDialogEvent dummyEvt;
                EditTextDlg::OnShowReplaceDialog(dummyEvt);
            }
            event.Skip(true);
        }, wxID_ANY);

    Bind(wxEVT_FIND, &EditTextDlg::OnFindDialog, this);
    Bind(wxEVT_FIND_NEXT, &EditTextDlg::OnFindDialog, this);
    Bind(wxEVT_FIND_REPLACE, &EditTextDlg::OnFindDialog, this);
    Bind(wxEVT_FIND_REPLACE_ALL, &EditTextDlg::OnFindDialog, this);
    Bind(wxEVT_FIND_CLOSE, &EditTextDlg::OnFindDialog, this);

    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &EditTextDlg::OnSaveButton, this, wxID_SAVE);

    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &EditTextDlg::OnEditButtons, this, wxID_PASTE);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &EditTextDlg::OnEditButtons, this, wxID_CUT);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &EditTextDlg::OnEditButtons, this, wxID_COPY);

    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &EditTextDlg::OnEditButtons, this, wxID_UNDO);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &EditTextDlg::OnEditButtons, this, wxID_REDO);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &EditTextDlg::OnShowFindDialog, this, wxID_FIND);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &EditTextDlg::OnShowReplaceDialog, this, wxID_REPLACE);
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
            wxRibbonPanel* clipboardPage = new wxRibbonPanel(homePage, wxID_ANY, _(L"Clipboard"),
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

            buttonBar->AddButton(wxID_FIND, _(L"Find"),
                wxArtProvider::GetBitmap(wxART_FIND, wxART_BUTTON,
                    FromDIP(wxSize(32, 32))).ConvertToImage(),
                _(L"Search for text."));
            buttonBar->AddButton(wxID_REPLACE, _(L"Replace"),
                wxArtProvider::GetBitmap(wxART_FIND_AND_REPLACE, wxART_BUTTON,
                    FromDIP(wxSize(32, 32))).ConvertToImage(),
                _(L"Replace text."));

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
void EditTextDlg::OnFindDialog(wxFindDialogEvent& event)
    {
    if (event.GetEventType() == wxEVT_FIND || event.GetEventType() == wxEVT_FIND_NEXT)
        {
        auto foundPos = m_textEntry->FindText(event.GetFindString(),
            (event.GetFlags() & wxFR_DOWN), (event.GetFlags() & wxFR_WHOLEWORD), (event.GetFlags() & wxFR_MATCHCASE));
        if (foundPos != wxNOT_FOUND)
            {
            m_textEntry->SetSelection(foundPos, foundPos + event.GetFindString().length());
            }
        }
    else if (event.GetEventType() == wxEVT_FIND_REPLACE)
        {
        long from{ 0 }, to{ 0 };
        m_textEntry->GetSelection(&from, &to);
        // force search to start from beginning of selection
        m_textEntry->SetSelection(from, from);

        auto foundPos = m_textEntry->FindText(event.GetFindString(),
            (event.GetFlags() & wxFR_DOWN), (event.GetFlags() & wxFR_WHOLEWORD), (event.GetFlags() & wxFR_MATCHCASE));
        if (foundPos != wxNOT_FOUND)
            {
            // if what is being replaced matches what was already selected, then replace it
            if (from == foundPos && to == (foundPos + event.GetFindString().length()) )
                {
                m_textEntry->Replace(foundPos, foundPos + event.GetFindString().length(), event.GetReplaceString());
                m_textEntry->SetSelection(foundPos, foundPos + event.GetReplaceString().length());
                // ...then, find the next occurrance of string being replaced for the next replace button click
                foundPos = m_textEntry->FindText(event.GetFindString(),
                    (event.GetFlags() & wxFR_DOWN), (event.GetFlags() & wxFR_WHOLEWORD),
                    (event.GetFlags() & wxFR_MATCHCASE));
                if (foundPos != wxNOT_FOUND)
                    {
                    m_textEntry->SetSelection(foundPos, foundPos + event.GetFindString().length());
                    }
                }
            // ...otherwise, just select the next string being replaced so that user can see it in its
            // context and then decide on the next replace button click if they want to replace it
            else
                {
                m_textEntry->SetSelection(foundPos, foundPos + event.GetFindString().length());
                }
            }
        else
            { m_textEntry->SetSelection(from, to); }
        }
    else if (event.GetEventType() == wxEVT_FIND_REPLACE_ALL)
        {
        m_textEntry->SetSelection(0, 0);
        auto foundPos = m_textEntry->FindText(event.GetFindString(),
            (event.GetFlags() & wxFR_DOWN), (event.GetFlags() & wxFR_WHOLEWORD), (event.GetFlags() & wxFR_MATCHCASE));
        while (foundPos != wxNOT_FOUND)
            {
            m_textEntry->Replace(foundPos, foundPos + event.GetFindString().length(), event.GetReplaceString());
            m_textEntry->SetSelection(foundPos + event.GetReplaceString().length(),
                                      foundPos + event.GetReplaceString().length());
            // ...then, find the next occurrance of string being replaced for the next loop
            foundPos = m_textEntry->FindText(event.GetFindString(),
                (event.GetFlags() & wxFR_DOWN), (event.GetFlags() & wxFR_WHOLEWORD),
                (event.GetFlags() & wxFR_MATCHCASE));
            }
        }
    else if (event.GetEventType() == wxEVT_FIND_CLOSE)
        {
        if (m_dlgReplace)
            {
            m_dlgReplace->Destroy();
            m_dlgReplace = nullptr;
            }

        if (m_dlgFind)
            {
            m_dlgFind->Destroy();
            m_dlgFind = nullptr;
            }
        }
    }

//------------------------------------------------------
void EditTextDlg::OnShowReplaceDialog([[maybe_unused]] wxCommandEvent& event)
    {
    // get rid of Find dialog (if it was opened)
    if (m_dlgFind)
        {
        m_dlgFind->Destroy();
        m_dlgFind = nullptr;
        }
    if (m_dlgReplace == nullptr)
        {
        m_dlgReplace = new wxFindReplaceDialog(this, &m_findData, _(L"Replace"), wxFR_REPLACEDIALOG);
        }
    m_dlgReplace->Show(true);
    m_dlgReplace->SetFocus();
    }

//------------------------------------------------------
void EditTextDlg::OnShowFindDialog([[maybe_unused]] wxCommandEvent & event)
    {
    // get rid of Replace dialog (if it was opened)
    if (m_dlgReplace)
        {
        m_dlgReplace->Destroy();
        m_dlgReplace = nullptr;
        }
    if (m_dlgFind == nullptr)
        {
        m_dlgFind = new wxFindReplaceDialog(this, &m_findData, _(L"Find"));
        }
    m_dlgFind->Show(true);
    m_dlgFind->SetFocus();
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

    if (IsModal())
        { EndModal(wxID_CLOSE); }
    else
        { Show(false); }
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

//------------------------------------------------------
void EditTextDlg::OnTextChanged(wxCommandEvent& event)
    {
    if (m_textEntry)
        {
        EnableSaveButton(m_textEntry->IsModified());

        wxWindow* editButtonBarWindow = m_ribbon->FindWindow(EditTextDlg::ID_EDIT_RIBBON_BUTTON_BAR);
        if (editButtonBarWindow && editButtonBarWindow->IsKindOf(CLASSINFO(wxRibbonButtonBar)))
            {
            auto editButtonBar = dynamic_cast<wxRibbonButtonBar*>(editButtonBarWindow);
            assert(editButtonBar && L"Error casting ribbon bar!");
            if (editButtonBar)
                {
                editButtonBar->EnableButton(wxID_UNDO, m_textEntry->CanUndo());
                editButtonBar->EnableButton(wxID_REDO, m_textEntry->CanRedo());
                }
            }
        }
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
