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
             wxString value,
             wxWindowID id /*= wxID_ANY*/,
             const wxString& caption /*= _(L"Edit Text")*/,
             const wxString& description /*= wxString{}*/,
             const wxPoint& pos /*= wxDefaultPosition*/,
             const wxSize& size /*= wxSize(600, 600)*/,
             long style /*= wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER*/) :
            m_value(std::move(value)),
            m_parentDoc(parentDoc)
    {
    SetBackgroundColour(wxGetApp().GetAppOptions().GetControlBackgroundColor());
    Create(parent, id, caption, description, pos, size, style);
    SetSize(m_parentDoc->GetFirstView()->GetFrame()->GetSize());
    Centre(wxCENTER_ON_SCREEN);

    Bind(wxEVT_BUTTON, &EditTextDlg::OnOK, this, wxID_OK);
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
        else if (event.GetKeyCode() == WXK_F3)
            {
            wxFindDialogEvent evt{ wxEVT_FIND_NEXT };
            evt.SetFindString(m_findData.GetFindString());
            evt.SetFlags(m_findData.GetFlags());
            EditTextDlg::OnFindDialog(evt);
            }
        event.Skip(true);
        }, wxID_ANY);

    Bind(wxEVT_FIND, &EditTextDlg::OnFindDialog, this);
    Bind(wxEVT_FIND_NEXT, &EditTextDlg::OnFindDialog, this);
    Bind(wxEVT_FIND_REPLACE, &EditTextDlg::OnFindDialog, this);
    Bind(wxEVT_FIND_REPLACE_ALL, &EditTextDlg::OnFindDialog, this);
    Bind(wxEVT_FIND_CLOSE, &EditTextDlg::OnFindDialog, this);

    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &EditTextDlg::OnSaveButton, this, wxID_SAVE);

    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED,
        [this](wxRibbonButtonBarEvent& event)
        {
        wxFontData data;
        data.SetInitialFont(m_style.GetFont());
        data.SetColour(m_style.GetTextColour());
        wxFontDialog dialog(this, data);
        if (dialog.ShowModal() == wxID_OK)
            {
            const bool wasModified = m_textEntry->IsModified();
            const bool isUndoEnabled = m_textEntry->CanUndo();
            m_style.SetFont(dialog.GetFontData().GetChosenFont());
            m_style.SetTextColour(dialog.GetFontData().GetColour());
            m_textEntry->SetStyle(0, m_textEntry->GetLastPosition(), m_style);
            // don't mark as modified when just changing the view's font
            m_textEntry->SetModified(wasModified);
            if (!isUndoEnabled)
                { m_textEntry->EmptyUndoBuffer(); }
            UpdateUndoButtons();
            }
        }, wxID_SELECT_FONT);

    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &EditTextDlg::OnEditButtons, this, wxID_PASTE);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &EditTextDlg::OnEditButtons, this, wxID_CUT);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &EditTextDlg::OnEditButtons, this, wxID_COPY);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &EditTextDlg::OnEditButtons, this, wxID_INDENT);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &EditTextDlg::OnEditButtons, this, wxID_JUSTIFY_LEFT);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &EditTextDlg::OnEditButtons, this, wxID_JUSTIFY_CENTER);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &EditTextDlg::OnEditButtons, this, wxID_JUSTIFY_RIGHT);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &EditTextDlg::OnEditButtons, this, wxID_JUSTIFY_FILL);
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED,
        [this](wxRibbonButtonBarEvent& event)
        {
        event.PopupMenu(&m_lineSpacingMenu);
        }, XRCID("ID_LINE_SPACING"));
    Bind(wxEVT_MENU, &EditTextDlg::OnLineSpaceSelected, this, XRCID("ID_LINE_SINGLE"));
    Bind(wxEVT_MENU, &EditTextDlg::OnLineSpaceSelected, this, XRCID("ID_LINE_ONE_AND_HALF"));
    Bind(wxEVT_MENU, &EditTextDlg::OnLineSpaceSelected, this, XRCID("ID_LINE_DOUBLE"));
    Bind(wxEVT_MENU, &EditTextDlg::OnParagraphSpaceSelected, this, XRCID("ID_ADD_PARAGRAPH_SPACE"));

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
        if (m_parentDoc && m_parentDoc->IsKindOf(CLASSINFO(ProjectDoc)))
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
        // paragraph
            {
            wxRibbonPanel* paragraphPage = new wxRibbonPanel(homePage, wxID_ANY, _(L"Paragraph"),
                wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
            wxRibbonButtonBar* buttonBar = new wxRibbonButtonBar(paragraphPage, ID_PARAGRAPH_RIBBON_BUTTON_BAR);

            buttonBar->AddButton(wxID_SELECT_FONT, _(L"Font"),
                wxArtProvider::GetBitmap(L"ID_FONT", wxART_BUTTON,
                    FromDIP(wxSize(32, 32))).ConvertToImage(),
                _(L"Changes the font showing the document."));

            buttonBar->AddToggleButton(wxID_INDENT, _(L"Indent"),
                wxArtProvider::GetBitmap(L"ID_PARAGRAPH_INDENT", wxART_BUTTON,
                    FromDIP(wxSize(32, 32))).ConvertToImage(),
                _(L"Indents the first line of each paragraph."));
            buttonBar->ToggleButton(wxID_INDENT, true);

            buttonBar->AddToggleButton(wxID_JUSTIFY_LEFT, _(L"Left"),
                wxArtProvider::GetBitmap(L"ID_ALIGN_LEFT", wxART_BUTTON,
                    FromDIP(wxSize(32, 32))).ConvertToImage(),
                _(L"Left aligns the text."));
            buttonBar->AddToggleButton(wxID_JUSTIFY_CENTER, _(L"Center"),
                wxArtProvider::GetBitmap(L"ID_ALIGN_CENTER", wxART_BUTTON,
                    FromDIP(wxSize(32, 32))).ConvertToImage(),
                _(L"Centers the text."));
            buttonBar->AddToggleButton(wxID_JUSTIFY_RIGHT, _(L"Right"),
                wxArtProvider::GetBitmap(L"ID_ALIGN_RIGHT", wxART_BUTTON,
                    FromDIP(wxSize(32, 32))).ConvertToImage(),
                _(L"Right aligns the text."));
            buttonBar->AddToggleButton(wxID_JUSTIFY_FILL, _(L"Justified"),
                wxArtProvider::GetBitmap(L"ID_ALIGN_JUSTIFIED", wxART_BUTTON,
                    FromDIP(wxSize(32, 32))).ConvertToImage(),
                _(L"Justifies the text."));
            buttonBar->ToggleButton(wxID_JUSTIFY_FILL, true);

            buttonBar->AddDropdownButton(XRCID("ID_LINE_SPACING"), _(L"Line Spacing"),
                wxArtProvider::GetBitmap(L"ID_LINE_SPACING", wxART_BUTTON,
                    FromDIP(wxSize(32, 32))).ConvertToImage(),
                _(L"Adjusts the spacing between lines."));

        #ifdef __WXMSW__
            m_lineSpacingMenu.Append(new wxMenuItem(&m_lineSpacingMenu, XRCID("ID_LINE_SINGLE"),
                _DT(L"1.0"), wxString{}, wxITEM_CHECK));
            m_lineSpacingMenu.Append(new wxMenuItem(&m_lineSpacingMenu, XRCID("ID_LINE_ONE_AND_HALF"),
                _DT(L"1.5"), wxString{}, wxITEM_CHECK));
            m_lineSpacingMenu.Append(new wxMenuItem(&m_lineSpacingMenu, XRCID("ID_LINE_DOUBLE"),
                _DT(L"2.0"), wxString{}, wxITEM_CHECK));
            m_lineSpacingMenu.Check(XRCID("ID_LINE_SINGLE"), true);
            m_lineSpacingMenu.AppendSeparator();
        #endif
            m_lineSpacingMenu.Append(new wxMenuItem(&m_lineSpacingMenu, XRCID("ID_ADD_PARAGRAPH_SPACE"),
                _(L"Add space after paragraphs"), wxString{}, wxITEM_CHECK));
            m_lineSpacingMenu.Check(XRCID("ID_ADD_PARAGRAPH_SPACE"), true);
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

            buttonBar->EnableButton(wxID_UNDO, false);
            buttonBar->EnableButton(wxID_REDO, false);
            }

        m_ribbon->SetArtProvider(new Wisteria::UI::RibbonMetroArtProvider);
        wxGetApp().UpdateRibbonTheme(m_ribbon);

        mainSizer->Add(m_ribbon, 0, wxEXPAND|wxALL, wxSizerFlags::GetDefaultBorder());
        m_ribbon->Realise();
        }

    // Note: you cannot use DDX (e.g., validators) with a text control if you want to use
    // wxTextAttr with it. Validators call either ChangeValue() or SetValue() and that
    // resets the control's wxTextAttr information. It seems that you can only use
    // AppendText() to preserve the default style information, so we need to manually
    // handle connecting the text control to m_value via Save() and our on OnOK().
    m_textEntry = new FormattedTextCtrl(this, wxID_ANY,
                                 wxDefaultPosition, wxDefaultSize,
                                 wxTE_AUTO_URL|wxTE_PROCESS_TAB);
    m_textEntry->SetMargins(10, 10);
    m_textEntry->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_TEXT_EDITOR_MENU"));
    if (m_parentDoc != nullptr)
        {
        m_style = wxTextAttr{ m_parentDoc->GetTextFontColor(), wxNullColour, m_parentDoc->GetTextViewFont() };
        m_style.SetLeftIndent(50, -40);
        m_style.SetParagraphSpacingAfter(40);
        m_style.SetAlignment(wxTextAttrAlignment::wxTEXT_ALIGNMENT_JUSTIFIED);
        m_textEntry->SetDefaultStyle(m_style);
        m_textEntry->EnableProofCheck(wxTextProofOptions::Default().
            Language((m_parentDoc->GetProjectLanguage() == readability::test_language::spanish_test) ?
                _DT("es") :
                (m_parentDoc->GetProjectLanguage() == readability::test_language::german_test) ?
                _DT("de") :
                _DT("en")).
            SpellCheck(true).GrammarCheck(true));
        }
    // must use AppendText to prevent text control's style from being wiped out
    m_textEntry->AppendText(m_value);
    m_textEntry->SetStyle(0, m_textEntry->GetLastPosition(), m_style);
    m_textEntry->SetModified(false);
    m_textEntry->EmptyUndoBuffer();
    EnableSaveButton(false);

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

    m_textEntry->SetFocus();
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
                // ...then, find the next occurrence of string being replaced for the next replace button click
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
            // ...then, find the next occurrence of string being replaced for the next loop
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
void EditTextDlg::OnOK(wxCommandEvent& event)
    {
    if (Validate() && TransferDataFromWindow() )
        {
        m_value = m_textEntry->GetValue();
        if (IsModal() )
            { EndModal(wxID_OK); }
        else
            {
            SetReturnCode(wxID_OK);
            Show(false);
            }
        }
    }

//------------------------------------------------------
void EditTextDlg::OnClose(wxCloseEvent& event)
    {
    if (m_textEntry && m_textEntry->IsModified() )
        {
        if (m_parentDoc && m_parentDoc->IsKindOf(CLASSINFO(ProjectDoc)) )
            {
            if (wxMessageBox(_(L"Do you wish to save your unsaved changes?"),
                    _(L"Save Changes"), wxYES_NO | wxICON_QUESTION) == wxYES)
                { Save(); }
            }
        else if (m_parentDoc && m_parentDoc->IsKindOf(CLASSINFO(BatchProjectDoc)))
            {
            assert(IsModal() && L"Text editor should be modal when called from a batch project!");
            if (wxMessageBox(_(L"Do you wish to save your unsaved changes?"),
                    _(L"Save Changes"), wxYES_NO | wxICON_QUESTION) == wxYES)
                {
                TransferDataFromWindow();
                m_value = m_textEntry->GetValue();
                EndModal(wxID_OK);
                return;
                }
            }
        }

    if (IsModal())
        { EndModal(wxID_CLOSE); }
    else
        {
        SetReturnCode(wxID_CLOSE);
        Show(false);
        }
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
void EditTextDlg::OnParagraphSpaceSelected(wxCommandEvent& event)
    {
    if (m_textEntry)
        {
        const bool wasModified = m_textEntry->IsModified();
        const bool isUndoEnabled = m_textEntry->CanUndo();

        // update menu
        const bool usingParaSpace = (m_style.GetParagraphSpacingAfter() > 0);
        m_lineSpacingMenu.Check(XRCID("ID_ADD_PARAGRAPH_SPACE"), !usingParaSpace);
        m_style.SetParagraphSpacingAfter(!usingParaSpace ? 40 : 0);

        m_textEntry->SetStyle(0, m_textEntry->GetLastPosition(), m_style);
        // don't mark as modified when just changing the view's font
        m_textEntry->SetModified(wasModified);
        if (!isUndoEnabled)
            { m_textEntry->EmptyUndoBuffer(); }
        UpdateUndoButtons();
        EnableSaveButton(wasModified);
        }
    }

//------------------------------------------------------
void EditTextDlg::OnLineSpaceSelected(wxCommandEvent& event)
    {
    if (m_textEntry)
        {
        const bool wasModified = m_textEntry->IsModified();
        const bool isUndoEnabled = m_textEntry->CanUndo();

        // update menu
        m_lineSpacingMenu.Check(XRCID("ID_LINE_SINGLE"), event.GetId() == XRCID("ID_LINE_SINGLE"));
        m_lineSpacingMenu.Check(XRCID("ID_LINE_ONE_AND_HALF"), event.GetId() == XRCID("ID_LINE_ONE_AND_HALF"));
        m_lineSpacingMenu.Check(XRCID("ID_LINE_DOUBLE"), event.GetId() == XRCID("ID_LINE_DOUBLE"));

        m_style.SetLineSpacing(event.GetId() == XRCID("ID_LINE_SINGLE") ?
            wxTEXT_ATTR_LINE_SPACING_NORMAL :
            event.GetId() == XRCID("ID_LINE_ONE_AND_HALF") ?
            wxTEXT_ATTR_LINE_SPACING_HALF :
            wxTEXT_ATTR_LINE_SPACING_TWICE);

        m_textEntry->SetStyle(0, m_textEntry->GetLastPosition(), m_style);
        // don't mark as modified when just changing the view's font
        m_textEntry->SetModified(wasModified);
        if (!isUndoEnabled)
            { m_textEntry->EmptyUndoBuffer(); }
        UpdateUndoButtons();
        EnableSaveButton(wasModified);
        }
    }

//------------------------------------------------------
void EditTextDlg::OnEditButtons(wxRibbonButtonBarEvent& event)
    {
    if (m_textEntry)
        {
        const bool wasModified = m_textEntry->IsModified();
        const bool isUndoEnabled = m_textEntry->CanUndo();

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
        else if (event.GetId() == wxID_INDENT)
            {
            if (m_style.GetLeftIndent() == 0)
                { m_style.SetLeftIndent(50, -40); }
            else
                { m_style.SetLeftIndent(0, 0); }
            m_textEntry->SetStyle(0, m_textEntry->GetLastPosition(), m_style);
            // don't mark as modified when just changing the view's font
            m_textEntry->SetModified(wasModified);
            if (!isUndoEnabled)
                { m_textEntry->EmptyUndoBuffer(); }
            UpdateUndoButtons();
            }
        else if (event.GetId() == wxID_JUSTIFY_LEFT ||
            event.GetId() == wxID_JUSTIFY_CENTER ||
            event.GetId() == wxID_JUSTIFY_RIGHT ||
            event.GetId() == wxID_JUSTIFY_FILL)
            {
            m_style.SetAlignment(
                event.GetId() == wxID_JUSTIFY_LEFT ?
                wxTextAttrAlignment::wxTEXT_ALIGNMENT_LEFT :
                event.GetId() == wxID_JUSTIFY_CENTER ?
                wxTextAttrAlignment::wxTEXT_ALIGNMENT_CENTER :
                event.GetId() == wxID_JUSTIFY_RIGHT ?
                wxTextAttrAlignment::wxTEXT_ALIGNMENT_RIGHT :
                wxTextAttrAlignment::wxTEXT_ALIGNMENT_JUSTIFIED);
            m_textEntry->SetStyle(0, m_textEntry->GetLastPosition(), m_style);
            UpdateIndentButtons();
            m_textEntry->SetModified(wasModified);
            if (!isUndoEnabled)
                { m_textEntry->EmptyUndoBuffer(); }
            UpdateUndoButtons();
            EnableSaveButton(wasModified);
            }
        }
    }

//------------------------------------------------------
void EditTextDlg::OnTextChanged(wxCommandEvent& event)
    {
    if (m_textEntry)
        {
        EnableSaveButton(m_textEntry->IsModified());
        UpdateUndoButtons();
        }
    }

//------------------------------------------------------
void EditTextDlg::Save()
    {
    if (!TransferDataFromWindow())
        { return; }

    m_value = m_textEntry->GetValue();

    // Only link the editor directly to a standard project.
    // Subprojects within batches will only get updated when this dialog
    // (which will be modal for batch projects) closes.
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

//------------------------------------------------------
void EditTextDlg::UpdateIndentButtons()
    {
    const wxWindowID buttonToEnable = (m_style.GetAlignment() == wxTEXT_ALIGNMENT_LEFT) ?
        wxID_JUSTIFY_LEFT :
        (m_style.GetAlignment() == wxTEXT_ALIGNMENT_CENTER) ?
        wxID_JUSTIFY_CENTER :
        (m_style.GetAlignment() == wxTEXT_ALIGNMENT_RIGHT) ?
        wxID_JUSTIFY_RIGHT : wxID_JUSTIFY_FILL;

    m_textEntry->SetStyle(0, m_textEntry->GetLastPosition(), m_style);
    wxWindow* paragraphButtonBarWindow = m_ribbon->FindWindow(EditTextDlg::ID_PARAGRAPH_RIBBON_BUTTON_BAR);
    if (paragraphButtonBarWindow && paragraphButtonBarWindow->IsKindOf(CLASSINFO(wxRibbonButtonBar)))
        {
        auto paragraphButtonBar = dynamic_cast<wxRibbonButtonBar*>(paragraphButtonBarWindow);
        assert(paragraphButtonBar && L"Error casting ribbon bar!");
        if (paragraphButtonBar)
            {
            paragraphButtonBar->ToggleButton(wxID_JUSTIFY_LEFT, false);
            paragraphButtonBar->ToggleButton(wxID_JUSTIFY_CENTER, false);
            paragraphButtonBar->ToggleButton(wxID_JUSTIFY_RIGHT, false);
            paragraphButtonBar->ToggleButton(wxID_JUSTIFY_FILL, false);
            paragraphButtonBar->ToggleButton(buttonToEnable, true);
            }
        }
    }

//------------------------------------------------------
void EditTextDlg::UpdateUndoButtons()
    {
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
