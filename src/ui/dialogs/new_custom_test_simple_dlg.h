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

#ifndef __NEW_CUSTOM_WORD_TEST_SIMPLE_DLG_H__
#define __NEW_CUSTOM_WORD_TEST_SIMPLE_DLG_H__

#include "../../Wisteria-Dataviz/src/ui/dialogs/dialogwithhelp.h"
#include <wx/filename.h>
#include <wx/statline.h>
#include <wx/string.h>
#include <wx/valgen.h>
#include <wx/valtext.h>
#include <wx/wx.h>

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
                                        const wxPoint& pos = wxDefaultPosition,
                                        const wxSize& size = wxDefaultSize,
                                        long style = wxDEFAULT_DIALOG_STYLE | wxCLIP_CHILDREN |
                                                     wxRESIZE_BORDER)
        {
        Create(parent, id, caption, pos, size, style);
        }

    /// @private
    NewCustomWordTestSimpleDlg(const NewCustomWordTestSimpleDlg& that) = delete;
    /// @private
    NewCustomWordTestSimpleDlg& operator=(const NewCustomWordTestSimpleDlg& that) = delete;

    /// @returns The test name.
    [[nodiscard]]
    wxString GetTestName() const
        {
        return m_testName;
        }

    /// @returns The word list's filepath.
    [[nodiscard]]
    wxString GetWordListFilePath() const
        {
        return m_wordListFilePath;
        }

  private:
    constexpr static int ID_FOLDER_BROWSE_BUTTON = wxID_HIGHEST;

    /// Creation
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY,
                const wxString& caption = _("New Custom Test"),
                const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_DIALOG_STYLE | wxCLIP_CHILDREN | wxRESIZE_BORDER)
        {
        SetExtraStyle(GetExtraStyle() | wxWS_EX_BLOCK_EVENTS | wxWS_EX_CONTEXTHELP);
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

#endif //__NEW_CUSTOM_WORD_TEST_SIMPLE_DLG_H__
