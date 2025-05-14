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

#ifndef EDIT_TEXT_DIALOG_H
#define EDIT_TEXT_DIALOG_H

#include "../../Wisteria-Dataviz/src/ui/controls/formattedtextctrl.h"
#include "../../Wisteria-Dataviz/src/ui/ribbon/artmetro.h"
#include <wx/datetime.h>
#include <wx/dialog.h>
#include <wx/fdrepdlg.h>
#include <wx/ribbon/art.h>
#include <wx/ribbon/bar.h>
#include <wx/ribbon/buttonbar.h>
#include <wx/ribbon/gallery.h>
#include <wx/ribbon/toolbar.h>
#include <wx/valgen.h>
#include <wx/wx.h>

class BaseProjectDoc;

/// @brief Dialog for editing text from a project.
class EditTextDlg final : public wxDialog
    {
  public:
    /** @brief Constructor.
        @param parent The parent window.
        @param parentDoc The project that this dialog is linked to.
        @param value The text to edit.
        @param id The dialog's ID.
        @param caption The dialog's caption.
        @param description A description label to show beneath the text.
        @param pos The dialog's window position.
        @param size The dialog's size.
        @param style The dialog's style.*/
    EditTextDlg(wxWindow* parent, BaseProjectDoc* parentDoc, wxString value,
                wxWindowID id = wxID_ANY, const wxString& caption = _(L"Edit Text"),
                const wxString& description = wxString{}, const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxSize(600, 500),
                long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    /// @private
    EditTextDlg(const EditTextDlg& that) = delete;
    /// @private
    EditTextDlg& operator=(const EditTextDlg& that) = delete;

    /// @returns The edited text.
    [[nodiscard]]
    const wxString& GetValue() const noexcept
        {
        return m_value;
        }

    /// @brief Searches for and selects a string in the text control.
    /// @details Will search downward, case insensitively, and whole word.
    /// @param str The string to look for.
    /// @param replaceStr An optional replacement string.
    void SelectString(const wxString& str, const wxString& replaceStr = wxString{})
        {
        if (m_textEntry != nullptr)
            {
            m_textEntry->SetSelection(0, 0);
            m_textEntry->FindText(str, true, true, false);
            m_findData.SetFlags(wxFR_DOWN | wxFR_WHOLEWORD | wxFR_MATCHCASE);
            m_findData.SetFindString(str);
            m_findData.SetReplaceString(replaceStr);
            // if Replace dialog is opened, then recreate and reopen it with new content
            if (m_dlgReplace != nullptr)
                {
                m_dlgReplace->Destroy();
                m_dlgReplace = nullptr;
                wxCommandEvent dummyCmd;
                OnShowReplaceDialog(dummyCmd);
                }
            // ...or the same with the Find dialog
            else if (m_dlgFind != nullptr)
                {
                m_dlgFind->Destroy();
                m_dlgFind = nullptr;
                wxCommandEvent dummyCmd;
                OnShowFindDialog(dummyCmd);
                }
            }
        }

  private:
    /// Creation.
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& caption = wxString{},
                const wxString& description = wxString{}, const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE)
        {
        SetExtraStyle(GetExtraStyle() | wxWS_EX_BLOCK_EVENTS);
        wxDialog::Create(parent, id, caption, pos, size, style);
        SetMinSize(FromDIP(size));

        m_description = description;

        CreateControls();
        Centre();

        return true;
        }

    /// Creates the controls and sizers.
    void CreateControls();
    void OnEditButtons(wxRibbonButtonBarEvent& event);
    void OnLineSpaceSelected(wxCommandEvent& event);
    void OnParagraphSpaceSelected([[maybe_unused]] wxCommandEvent& event);
    void OnSaveButton([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnTextChanged([[maybe_unused]] wxCommandEvent& event);
    void OnClose([[maybe_unused]] wxCloseEvent& event);
    void OnShowFindDialog([[maybe_unused]] wxCommandEvent& event);
    void OnShowReplaceDialog([[maybe_unused]] wxCommandEvent& event);
    void OnFindDialog(wxFindDialogEvent& event);
    void OnOK([[maybe_unused]] wxCommandEvent& event);

    void Save();

    void EnableSaveButton(const bool enable = true);
    void UpdateButtons();

    wxString m_value;
    wxString m_description;
    wxMenu m_lineSpacingMenu;
    bool m_usingParaSpace{ false };
    wxTextAttrLineSpacing m_lineSpacing{ wxTEXT_ATTR_LINE_SPACING_NORMAL };

    BaseProjectDoc* m_parentDoc{ nullptr };
    Wisteria::UI::FormattedTextCtrl* m_textEntry{ nullptr };
    wxRibbonBar* m_ribbon{ nullptr };

    wxFindReplaceData m_findData{ wxFR_DOWN };

    wxFindReplaceDialog* m_dlgFind{ nullptr };
    wxFindReplaceDialog* m_dlgReplace{ nullptr };

    wxTextAttr m_style;
    };

#endif // EDIT_TEXT_DIALOG_H
