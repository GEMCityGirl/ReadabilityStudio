/** @addtogroup UI
    @brief Classes for the user interface.
    @date 2005-2023
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __WXNEW_CUSTOM_TEST_SIMPLE_DLG_H__
#define __WXNEW_CUSTOM_TEST_SIMPLE_DLG_H__

#include <wx/wx.h>
#include <wx/string.h>
#include <wx/statline.h>
#include <wx/valgen.h>
#include <wx/valtext.h>
#include <wx/filename.h>
#include "../../../../SRC/Wisteria-Dataviz/src/ui/dialogs/dialogwithhelp.h"

class NewCustomTestSimpleDlg final : public Wisteria::UI::DialogWithHelp
    {
public:
    NewCustomTestSimpleDlg(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& caption = _("New Custom Test"),
                      const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                      long style = wxDEFAULT_DIALOG_STYLE|wxCLIP_CHILDREN|wxRESIZE_BORDER)
        { Create(parent, id, caption, pos, size, style); }

    /// Creation
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& caption = _("New Custom Test"),
                const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_DIALOG_STYLE|wxCLIP_CHILDREN|wxRESIZE_BORDER)
        {
        SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS|wxWS_EX_CONTEXTHELP);
        Wisteria::UI::DialogWithHelp::Create(parent, id, caption, pos, size, style);

        CreateControls();
        Centre();
        return true;
        }

    void CreateControls();
    [[nodiscard]] wxString GetTestName() const
        { return m_testName; }
    [[nodiscard]] wxString GetWordListFilePath() const
        { return m_wordListFilePath; }

    void OnBrowseForFileClick([[maybe_unused]] wxCommandEvent& event);
    void OnOK([[maybe_unused]] wxCommandEvent& event);

private:
    static constexpr int ID_FOLDER_BROWSE_BUTTON = 10001;
    wxString m_testName;
    wxString m_wordListFilePath;

    NewCustomTestSimpleDlg() noexcept {}
    wxDECLARE_NO_COPY_CLASS(NewCustomTestSimpleDlg);
    wxDECLARE_EVENT_TABLE();
    };

/** @}*/

#endif //__WXNEW_CUSTOM_TEST_SIMPLE_DLG_H__
