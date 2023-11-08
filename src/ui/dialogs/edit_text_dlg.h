/** @addtogroup UI
    @brief Classes for the user interface.
    @date 2005-2023
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __EDIT_TEXT_DIALOG_H__
#define __EDIT_TEXT_DIALOG_H__

#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/valgen.h>
#include <wx/ribbon/bar.h>
#include <wx/ribbon/buttonbar.h>
#include <wx/ribbon/gallery.h>
#include <wx/ribbon/toolbar.h>
#include <wx/ribbon/art.h>
#include <wx/fdrepdlg.h>
#include "../../Wisteria-Dataviz/src/ui/controls/formattedtextctrl.h"
#include "../../Wisteria-Dataviz/src/ui/ribbon/artmetro.h"

class BaseProjectDoc;

/// @brief Dialog for editing text from a project.
class EditTextDlg final : public wxDialog
    {
public:
    /** @brief Constructor.
        @param parent The parent window.
        @param value The text to edit.
        @param id The dialog's ID.
        @param caption The dialog's caption.
        @param description A description label to show beneath the text.
        @param pos The dialog's window position.
        @param size The dialog's size.
        @param style The dialog's style.*/
    EditTextDlg(wxWindow* parent,
        BaseProjectDoc* parentDoc,
        wxString value,
        wxWindowID id = wxID_ANY,
        const wxString& caption = _(L"Edit Text"),
        const wxString& description = wxString{},
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxSize(600, 600),
        long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    /// @private
    EditTextDlg(const EditTextDlg& that) = delete;
    /// @private
    EditTextDlg& operator=(const EditTextDlg& that) = delete;
    /// @returns The edited text.
    [[nodiscard]]
    const wxString& GetValue() const noexcept
        { return m_value; }
    /// @brief Searches for and selects a string in the text control.
    /// @details Will search downward, case insensitively, and whole word.
    /// @param str The string to look for.
    void SelectString(const wxString& str)
        {
        if (m_textEntry != nullptr)
            {
            m_textEntry->SetSelection(0, 0);
            m_textEntry->FindText(str, true, true, false);
            m_findData.SetFlags(wxFR_DOWN|wxFR_WHOLEWORD|wxFR_MATCHCASE);
            m_findData.SetFindString(str);
            }
        }
private:
    /// Creation.
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY,
        const wxString& caption = wxString{},
        const wxString& description = wxString{},
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE)
        {
        SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
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
    void OnSaveButton(wxRibbonButtonBarEvent& event);
    void OnTextChanged(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnShowFindDialog([[maybe_unused]] wxCommandEvent& event);
    void OnShowReplaceDialog([[maybe_unused]] wxCommandEvent& event);
    void OnFindDialog(wxFindDialogEvent& event);
    void OnOK(wxCommandEvent& event);

    void Save();

    void EnableSaveButton(const bool enable = true);
    void UpdateIndentButtons();
    void UpdateUndoButtons();

    static constexpr auto ID_DOCUMENT_RIBBON_BUTTON_BAR = wxID_HIGHEST;
    static constexpr auto ID_CLIPBOARD_RIBBON_BUTTON_BAR = wxID_HIGHEST + 1;
    static constexpr auto ID_EDIT_RIBBON_BUTTON_BAR = wxID_HIGHEST + 2;
    static constexpr auto ID_PARAGRAPH_RIBBON_BUTTON_BAR = wxID_HIGHEST + 3;

    wxString m_value;
    wxString m_description;
    wxMenu m_lineSpacingMenu;

    BaseProjectDoc* m_parentDoc{ nullptr };
    FormattedTextCtrl* m_textEntry{ nullptr };
    wxRibbonBar* m_ribbon{ nullptr };

    wxFindReplaceData m_findData{ wxFR_DOWN };

    wxFindReplaceDialog* m_dlgFind{ nullptr };
    wxFindReplaceDialog* m_dlgReplace{ nullptr };

    wxTextAttr m_style;
    };

/** @}*/

#endif //__EDIT_TEXT_DIALOG_H__
