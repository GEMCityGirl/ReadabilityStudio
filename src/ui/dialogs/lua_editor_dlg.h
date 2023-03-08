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
#include "../../../../SRC/Wisteria-Dataviz/src/ui/controls/codeeditor.h"

class LuaEditorDlg final : public wxFrame
    {
public:
    explicit LuaEditorDlg(wxWindow* parent, wxWindowID id = wxID_ANY,
                 const wxString& caption = _("Lua Script"), const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxCAPTION|wxCLOSE_BOX|wxMINIMIZE_BOX|wxMAXIMIZE_BOX|wxRESIZE_BORDER);
    LuaEditorDlg(const LuaEditorDlg&) = delete;
    LuaEditorDlg(LuaEditorDlg&&) = delete;
    LuaEditorDlg& operator=(const LuaEditorDlg&) = delete;
    LuaEditorDlg& operator=(LuaEditorDlg&&) = delete;
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
    [[nodiscard]] Wisteria::UI::CodeEditor* CreateLuaScript(wxWindow* parent);

    wxAuiNotebook* m_notebook{ nullptr };
    wxAuiToolBar* m_toolbar{ nullptr };
    wxHtmlWindow* m_debugMessageWindow{ nullptr };
    wxAuiManager m_mgr;
    };

#endif //__LUA_EDITOR_DIALOG_H__
