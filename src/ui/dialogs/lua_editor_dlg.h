/** @addtogroup UI
    @brief Classes for the user interface.
    @date 2005-2023
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __LUA_EDITOR_DIALOG_H__
#define __LUA_EDITOR_DIALOG_H__

#include <wx/dialog.h>
#include <wx/ribbon/bar.h>
#include <wx/ribbon/buttonbar.h>
#include <wx/ribbon/gallery.h>
#include <wx/ribbon/toolbar.h>
#include <wx/ribbon/art.h>
#include <wx/html/helpwnd.h>
#include <wx/splitter.h>
#include <wx/aui/aui.h>
#include <wx/srchctrl.h>
#include <wx/renderer.h>
#include <wx/fdrepdlg.h>
#include "../../Wisteria-Dataviz/src/ui/controls/codeeditor.h"

/// @brief Lua editor and runner dialog.
class LuaEditorDlg final : public wxFrame
    {
public:
    /** @brief Constructor.
        @param parent The dialog's parent.
        @param id The dialog's ID.
        @param caption The dialog's caption.
        @param pos The dialog's position.
        @param size The dialog's size.
        @param style The dialog's style.*/
    explicit LuaEditorDlg(wxWindow* parent, wxWindowID id = wxID_ANY,
                 const wxString& caption = _(L"Lua Script"), const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxCAPTION|wxCLOSE_BOX|wxMINIMIZE_BOX|wxMAXIMIZE_BOX|wxRESIZE_BORDER);
    /// @private
    LuaEditorDlg(const LuaEditorDlg&) = delete;
    /// @private
    LuaEditorDlg& operator=(const LuaEditorDlg&) = delete;
    /// @brief Sets the background color for the interface.
    ///     (Foreground colors will be adjusted to this color.)
    /// @param color The background color to use.
    void SetThemeColor(const wxColour& color);
    /// @brief Add a message to the debug output window.
    /// @param str The message to append.
    void DebugOutput(const wxString& str);
    /// @brief Clears the debug output window.
    void DebugClear();
private:
    void CreateControls();
    void OnClose([[maybe_unused]] wxCloseEvent& event);
    void OnSave([[maybe_unused]] wxCommandEvent& event);
    void OnShowFindDialog([[maybe_unused]] wxCommandEvent& event);
    void OnShowReplaceDialog([[maybe_unused]] wxCommandEvent& event);
    void OnFindDialog(wxFindDialogEvent& event);

    [[nodiscard]]
    Wisteria::UI::CodeEditor* CreateLuaScript(wxWindow* parent);

    wxAuiNotebook* m_notebook{ nullptr };
    wxAuiToolBar* m_toolbar{ nullptr };
    wxHtmlWindow* m_debugMessageWindow{ nullptr };
    wxAuiManager m_mgr;

    wxFindReplaceData m_findData{ wxFR_DOWN };

    wxFindReplaceDialog* m_dlgFind{ nullptr };
    wxFindReplaceDialog* m_dlgReplace{ nullptr };
    };

/** @}*/

#endif //__LUA_EDITOR_DIALOG_H__
