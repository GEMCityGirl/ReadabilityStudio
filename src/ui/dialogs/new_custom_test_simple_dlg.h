/** @addtogroup UI
    @brief Classes for the user interface.
    @date 2005-2023
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __NEW_CUSTOM_WORD_TEST_SIMPLE_DLG_H__
#define __NEW_CUSTOM_WORD_TEST_SIMPLE_DLG_H__

#include <wx/wx.h>
#include <wx/string.h>
#include <wx/statline.h>
#include <wx/valgen.h>
#include <wx/valtext.h>
#include <wx/filename.h>
#include "../../Wisteria-Dataviz/src/ui/dialogs/dialogwithhelp.h"

/// @brief Dialog to create a new custom word test.
/// @details This dialog will only prompt for the test name and path
///     to custom word list.
class NewCustomWordTestSimpleDlg final : public Wisteria::UI::DialogWithHelp
    {
public:
    /** @brief Constructor.
        @param parent The dialog's parent.
        @param id The dialog's ID.
        @param caption The dialog's caption.
        @param pos The dialog's position.
        @param size The dialog's size.
        @param style The dialog's style.*/
    explicit NewCustomWordTestSimpleDlg(wxWindow* parent, wxWindowID id = wxID_ANY,
        const wxString& caption = _("New Custom Test"),
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE|wxCLIP_CHILDREN|wxRESIZE_BORDER)
        { Create(parent, id, caption, pos, size, style); }
    /// @private
    NewCustomWordTestSimpleDlg(const NewCustomWordTestSimpleDlg& that) = delete;
    /// @private
    NewCustomWordTestSimpleDlg& operator=(const NewCustomWordTestSimpleDlg& that) = delete;

    /// @returns The test name.
    [[nodiscard]]
    wxString GetTestName() const
        { return m_testName; }
    /// @returns The word list's filepath.
    [[nodiscard]]
    wxString GetWordListFilePath() const
        { return m_wordListFilePath; }
private:
    static constexpr int ID_FOLDER_BROWSE_BUTTON = wxID_HIGHEST;

    /// Creation
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY,
                const wxString& caption = _("New Custom Test"),
                const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_DIALOG_STYLE|wxCLIP_CHILDREN|wxRESIZE_BORDER)
        {
        SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS|wxWS_EX_CONTEXTHELP);
        Wisteria::UI::DialogWithHelp::Create(parent, id, caption, pos, size, style);

        CreateControls();
        Centre();

        Bind(wxEVT_BUTTON, &NewCustomWordTestSimpleDlg::OnBrowseForFileClick, this,
            NewCustomWordTestSimpleDlg::ID_FOLDER_BROWSE_BUTTON);
        Bind(wxEVT_BUTTON, &NewCustomWordTestSimpleDlg::OnOK, this, wxID_OK);

        return true;
        }

    void CreateControls();
    void OnBrowseForFileClick([[maybe_unused]] wxCommandEvent& event);
    void OnOK([[maybe_unused]] wxCommandEvent& event);

    wxString m_testName;
    wxString m_wordListFilePath;
    };

/** @}*/

#endif //__NEW_CUSTOM_WORD_TEST_SIMPLE_DLG_H__
