///////////////////////////////////////////////////////////////////////////////
// Name:        lua_editor_dlg.cpp
// Author:      Blake Madden
// Copyright:   (c) 2005-2023 Blake Madden
// Licence:     3-Clause BSD licence
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "lua_editor_dlg.h"
#include "../../Wisteria-Dataviz/src/base/colorbrewer.h"
#include "../../Wisteria-Dataviz/src/import/html_extract_text.h"
#include "../../Wisteria-Dataviz/src/ui/ribbon/artmetro.h"
#include "../../app/readability_app.h"

wxDECLARE_APP(ReadabilityApp);

using namespace Wisteria::Colors;
using namespace Wisteria::GraphItems;
using namespace Wisteria::UI;
using namespace lily_of_the_valley;

/// @brief AUI tab art provider with a flat appearance.
/// @note When using, just call SetColour() to set the main color, and the active colors and text
///     will be automatically adjusted to contrast against that color.\n
///     Also, note that this provider is not system color aware;
///     it is the user's responsibility to set this provider's color to the system
///     (or custom) color.
class FlatTabArt : public wxAuiGenericTabArt
    {
  public:
    /// @private
    FlatTabArt() : wxAuiGenericTabArt() {}

    /// @private
    wxAuiTabArt* Clone() final { return new FlatTabArt(*this); }

    /// @private
    //-------------------------------------------------------
    void DrawBackground(wxDC& dc, [[maybe_unused]] wxWindow* wnd, const wxRect& rect) final
        {
        wxRect r;

        if (m_flags & wxAUI_NB_BOTTOM)
            {
            r = wxRect(rect.x, rect.y, rect.width + 2, rect.height);
            }
        // TODO: else if (m_flags &wxAUI_NB_LEFT) {}
        // TODO: else if (m_flags &wxAUI_NB_RIGHT) {}
        else // for wxAUI_NB_TOP
            {
            r = wxRect(rect.x, rect.y, rect.width + 2, rect.height - 3);
            }

        wxDCBrushChanger bch(dc, m_baseColourBrush);
        wxDCPenChanger pch(dc, m_baseColourPen);
        dc.DrawRectangle(r);
        }

    /// @private
    //-------------------------------------------------------
    static void IndentPressedBitmap(const wxSize& offset, wxRect* rect, int button_state) noexcept
        {
        if (button_state == wxAUI_BUTTON_STATE_PRESSED)
            {
            rect->x += offset.x;
            rect->y += offset.y;
            }
        }

    /// @private
    /// A utility function to scale a bitmap in place for use at the given scale factor.
    //-------------------------------------------------------
    static void ScaleBitmap([[maybe_unused]] wxBitmap& bmp, [[maybe_unused]] double scale)
        {
#if wxUSE_IMAGE && !defined(__WXGTK3__) && !defined(__WXOSX__)
        // scale to a close round number to improve quality
        scale = std::floor(scale + 0.25);
        if (scale > 1.0 && !(bmp.GetScaleFactor() > 1.0))
            {
            wxImage img = bmp.ConvertToImage();
            img.Rescale(bmp.GetWidth() * scale, bmp.GetHeight() * scale,
                        wxIMAGE_QUALITY_BOX_AVERAGE);
            bmp = wxBitmap(img);
            }
#endif // wxUSE_IMAGE
        }

    /// @private
    //-------------------------------------------------------
    static wxString ChopText(const wxDC& dc, const wxString& text, int max_size)
        {
        wxCoord x, y;

        // first check if the text fits with no problems
        dc.GetTextExtent(text, &x, &y);
        if (x <= max_size)
            {
            return text;
            }

        size_t last_good_length = 0;
        for (size_t i = 0; i < text.length(); ++i)
            {
            wxString s = text.Left(i);
            s += _(L"...");

            dc.GetTextExtent(s, &x, &y);
            if (x > max_size)
                {
                break;
                }

            last_good_length = i;
            }

        wxString ret = text.Left(last_good_length);
        ret += _(L"...");
        return ret;
        }

    /// @private
    //-------------------------------------------------------
    void DrawTab(wxDC& dc, wxWindow* wnd, const wxAuiNotebookPage& page, const wxRect& in_rect,
                 int close_button_state, wxRect* out_tab_rect, wxRect* out_button_rect,
                 int* x_extent) final
        {
        if (!wnd)
            {
            return;
            }
        wxCoord normal_textx, normal_texty;
        wxCoord selected_textx, selected_texty;
        wxCoord texty;

        // if the caption is empty, measure some temporary text
        wxString caption = page.caption;
        if (caption.empty())
            {
            caption = L"Xj";
            }

        dc.SetFont(m_selectedFont);
        dc.GetTextExtent(caption, &selected_textx, &selected_texty);

        dc.SetFont(m_normalFont);
        dc.GetTextExtent(caption, &normal_textx, &normal_texty);

        // figure out the size of the tab
        const wxSize tab_size = GetTabSize(dc, wnd, page.caption, page.bitmap, page.active,
                                           close_button_state, x_extent);

        const wxCoord tab_height = m_tabCtrlHeight - 3;
        const wxCoord tab_width = tab_size.x;
        const wxCoord tab_x = in_rect.x;
        const wxCoord tab_y = in_rect.y + in_rect.height - tab_height;

        caption = page.caption;

        // select pen, brush, and font for the tab to be drawn
        if (page.active)
            {
            dc.SetFont(m_selectedFont);
            texty = selected_texty;
            }
        else
            {
            dc.SetFont(m_normalFont);
            texty = normal_texty;
            }

        // create points that will make the tab outline
        int clip_width = tab_width;
        if (tab_x + clip_width > in_rect.x + in_rect.width)
            {
            clip_width = (in_rect.x + in_rect.width) - tab_x;
            }

        // since the above code above doesn't play well with WXDFB or WXCOCOA,
        // we'll just use a rectangle for the clipping region for now --
        dc.SetClippingRegion(tab_x, tab_y, clip_width + 1, tab_height - 3);

        wxPoint border_points[6];
        if (m_flags & wxAUI_NB_BOTTOM)
            {
            border_points[0] = wxPoint(tab_x, tab_y);
            border_points[1] = wxPoint(tab_x, tab_y + tab_height - 6);
            border_points[2] = wxPoint(tab_x + 2, tab_y + tab_height - 4);
            border_points[3] = wxPoint(tab_x + tab_width - 2, tab_y + tab_height - 4);
            border_points[4] = wxPoint(tab_x + tab_width, tab_y + tab_height - 6);
            border_points[5] = wxPoint(tab_x + tab_width, tab_y);
            }
        else // if (m_flags & wxAUI_NB_TOP) {}
            {
            border_points[0] = wxPoint(tab_x, tab_y + tab_height - 4);
            border_points[1] = wxPoint(tab_x, tab_y + 2);
            border_points[2] = wxPoint(tab_x + 2, tab_y);
            border_points[3] = wxPoint(tab_x + tab_width - 2, tab_y);
            border_points[4] = wxPoint(tab_x + tab_width, tab_y + 2);
            border_points[5] = wxPoint(tab_x + tab_width, tab_y + tab_height - 4);
            }
        // TODO: else if (m_flags &wxAUI_NB_LEFT) {}
        // TODO: else if (m_flags &wxAUI_NB_RIGHT) {}

        const int drawn_tab_yoff = border_points[1].y;
        const int drawn_tab_height = border_points[0].y - border_points[1].y;

        if (page.active)
            {
            // draw active tab
            // draw inactive tab
            const wxRect r(tab_x, tab_y, tab_width, tab_height);

            wxDCBrushChanger bch(dc, ColorContrast::Shade(m_baseColour));
            wxDCPenChanger pch(dc, m_baseColourPen);
            dc.DrawRectangle(r);
            }
        else
            {
            // draw inactive tab
            const wxRect r(tab_x, tab_y + 1, tab_width, tab_height - 3);

            wxDCBrushChanger bch(dc, m_baseColourBrush);
            wxDCPenChanger pch(dc, m_baseColourPen);
            dc.DrawRectangle(r);
            }

        // draw tab outline
        dc.SetPen(ColorContrast::BlackOrWhiteContrast(m_baseColour));
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.DrawPolygon(std::size(border_points), border_points);

        // there are two horizontal grey lines at the bottom of the tab control,
        // this gets rid of the top one of those lines in the tab control
        if (page.active)
            {
            if (m_flags & wxAUI_NB_BOTTOM)
                {
                dc.SetPen(wxPen(m_baseColour.ChangeLightness(170)));
                }
            // TODO: else if (m_flags &wxAUI_NB_LEFT) {}
            // TODO: else if (m_flags &wxAUI_NB_RIGHT) {}
            else // for wxAUI_NB_TOP
                {
                dc.SetPen(m_baseColourPen);
                }
            dc.DrawLine(border_points[0].x + 1, border_points[0].y, border_points[5].x,
                        border_points[5].y);
            }

        int text_offset;
        int bitmap_offset = 0;
        if (page.bitmap.IsOk())
            {
            bitmap_offset = tab_x + wnd->FromDIP(8);

            // draw bitmap
            dc.DrawBitmap(page.bitmap.GetBitmapFor(wnd), bitmap_offset,
                          drawn_tab_yoff + (drawn_tab_height / 2.0) -
                              (page.bitmap.GetBitmapFor(wnd).GetScaledHeight() / 2.0),
                          true);

            text_offset = bitmap_offset + page.bitmap.GetBitmapFor(wnd).GetScaledWidth();
            text_offset += wnd->FromDIP(3); // bitmap padding
            }
        else
            {
            text_offset = tab_x + wnd->FromDIP(8);
            }

        // draw close button if necessary
        int close_button_width{ 0 };
        if (close_button_state != wxAUI_BUTTON_STATE_HIDDEN)
            {
            auto closeImg = m_disabledCloseBmp.GetBitmapFor(wnd).ConvertToImage();
            closeImg = Image::ChangeColor(
                closeImg, *wxBLACK,
                page.active ?
                    ColorContrast::BlackOrWhiteContrast(ColorContrast::Shade(m_baseColour)) :
                    ColorContrast::BlackOrWhiteContrast(m_baseColour));

            wxBitmap bmp{ closeImg };

            int offsetY = tab_y - 1;
            if (m_flags & wxAUI_NB_BOTTOM)
                {
                offsetY = 1;
                }

            wxRect rect(tab_x + tab_width - bmp.GetWidth() - wnd->FromDIP(1),
                        offsetY + (tab_height / 2) - (bmp.GetHeight() / 2), bmp.GetWidth(),
                        tab_height);

            IndentPressedBitmap(wnd->FromDIP(wxSize{ 1, 1 }), &rect, close_button_state);
            dc.DrawBitmap(bmp, rect.x, rect.y, true);

            *out_button_rect = rect;
            close_button_width = bmp.GetWidth();
            }

        wxString draw_text =
            ChopText(dc, caption, tab_width - (text_offset - tab_x) - close_button_width);

        // draw tab text
        const wxColor font_color =
            page.active ? ColorContrast::BlackOrWhiteContrast(ColorContrast::Shade(m_baseColour)) :
                          ColorContrast::BlackOrWhiteContrast(m_baseColour);
        dc.SetTextForeground(font_color);
        dc.DrawText(draw_text, text_offset,
                    drawn_tab_yoff + (drawn_tab_height) / 2 - (texty / 2) - 1);

        // draw focus rectangle
        if (page.active && (wnd->FindFocus() == wnd))
            {
            wxRect focusRectText(text_offset,
                                 (drawn_tab_yoff + (drawn_tab_height) / 2 - (texty / 2) - 1),
                                 selected_textx, selected_texty);

            wxRect focusRect;
            wxRect focusRectBitmap;

            if (page.bitmap.IsOk())
                {
                focusRectBitmap = wxRect(bitmap_offset,
                                         drawn_tab_yoff + (drawn_tab_height / 2) -
                                             (page.bitmap.GetBitmapFor(wnd).GetScaledHeight() / 2),
                                         page.bitmap.GetBitmapFor(wnd).GetScaledWidth(),
                                         page.bitmap.GetBitmapFor(wnd).GetScaledHeight());
                }

            if (page.bitmap.IsOk() && draw_text.empty())
                {
                focusRect = focusRectBitmap;
                }
            else if (!page.bitmap.IsOk() && !draw_text.empty())
                {
                focusRect = focusRectText;
                }
            else if (page.bitmap.IsOk() && !draw_text.empty())
                {
                focusRect = focusRectText.Union(focusRectBitmap);
                }

            focusRect.Inflate(2, 2);

            wxRendererNative::Get().DrawFocusRect(wnd, dc, focusRect, 0);
            }

        *out_tab_rect = wxRect(tab_x, tab_y, tab_width, tab_height);

        dc.DestroyClippingRegion();
        }
    };

//-------------------------------------------------------
LuaEditorDlg::LuaEditorDlg(
    wxWindow* parent, wxWindowID id /*= wxID_ANY*/, const wxString& caption /*= _(L"Lua Script")*/,
    const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/,
    long style /*= wxCAPTION | wxCLOSE_BOX | wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxRESIZE_BORDER*/)
    : wxFrame(parent, id, caption, pos, size,
              ((parent == nullptr) ? style | wxDIALOG_NO_PARENT : style)),
      m_debugMessageWindow(nullptr)
    {
    m_mgr.SetManagedWindow(this);

    wxIcon ico;
    ico.CopyFromBitmap(wxGetApp()
                           .GetResourceManager()
                           .GetSVG(L"ribbon/lua.svg")
                           .GetBitmap(FromDIP(wxSize{ 32, 32 })));
    SetIcon(ico);

    CreateControls();
    Center();

    Bind(wxEVT_CLOSE_WINDOW, &LuaEditorDlg::OnClose, this);

    Bind(
        wxEVT_TOOL,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            if (m_notebook->GetPageCount() == 0)
                {
                return;
                }

            auto editor = dynamic_cast<CodeEditor*>(m_notebook->GetCurrentPage());
            editor->AnnotationClearAll();
            wxString errorMessage;

            // disable (and later re-enable) the Run button while the script runs
            m_toolbar->EnableTool(XRCID("ID_RUN"), false);
            m_toolbar->EnableTool(XRCID("ID_STOP"), true);

            // run the script
            wxGetApp().GetLuaRunner().RunLuaCode(
                // run either selected text or the whole script
                (editor->GetSelectionStart() == editor->GetSelectionEnd()) ?
                    editor->GetValue() :
                    editor->GetSelectedText(),
                editor->GetScriptFilePath(), errorMessage);

            m_toolbar->EnableTool(XRCID("ID_RUN"), true);
            m_toolbar->EnableTool(XRCID("ID_STOP"), false);

            if (errorMessage.length())
                {
                const int lineOffset = (editor->GetSelectionStart() == editor->GetSelectionEnd()) ?
                                           0 :
                                           editor->LineFromPosition(editor->GetSelectionStart());

                long lineNumber{ 0 };
                errorMessage.ToLong(&lineNumber);
                // make it zero indexed
                --lineNumber;
                // and offset if we are running a selection instead of the whole script
                lineNumber += lineOffset;

                // strip the number from the message
                if (const auto foundPos = errorMessage.find(L':'); foundPos != wxString::npos)
                    {
                    errorMessage.erase(0, foundPos + 1);
                    }

                wxMessageBox(wxString::Format(
                                // TRANSLATORS: placeholders are a line number and error message
                                // from a script
                                _(L"Line #%s: %s"), std::to_wstring(lineNumber + 1), errorMessage),
                            _(L"Script Error"), wxOK | wxICON_EXCLAMATION);
                if (lineNumber >= 0)
                    {
                    errorMessage.insert(0, _(L"Error:"));
                    // if scrolling up, then step back up an extra line so that we can see it
                    // after adding the annotation beneath it
                    editor->GotoLine((editor->GetFirstVisibleLine() < lineNumber) ? lineNumber :
                                                                                    lineNumber - 1);
                    editor->AnnotationSetText(lineNumber, errorMessage);
                    editor->AnnotationSetStyle(lineNumber, editor->ERROR_ANNOTATION_STYLE);

                    // Scintilla doesn't update the scroll width for annotations, even with
                    // scroll width tracking on, so do it manually.
                    const int width = editor->GetScrollWidth();

                    // Take into account the fact that the annotation is shown indented, with
                    // the same indent as the line it's attached to.
                    // Also, add 3; this is just a hack to account for the width of the box, there
                    // doesn't seem to be any way to get it directly from Scintilla.
                    const int indent = editor->GetLineIndentation(lineNumber) + FromDIP(3);

                    const int widthAnn = editor->TextWidth(editor->ERROR_ANNOTATION_STYLE,
                                                        errorMessage + wxString(indent, L' '));

                    if (widthAnn > width)
                        {
                        editor->SetScrollWidth(widthAnn);
                        }
                    }
                }
        },
        XRCID("ID_RUN"));

    Bind(
        wxEVT_TOOL,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            m_toolbar->EnableTool(XRCID("ID_RUN"), true);
            m_toolbar->EnableTool(XRCID("ID_STOP"), false);
            wxGetApp().GetLuaRunner().Quit();
        },
        XRCID("ID_STOP"));

    Bind(wxEVT_AUINOTEBOOK_PAGE_CLOSE,
         [this]([[maybe_unused]] wxCommandEvent&)
         {
             if (m_notebook->GetPageCount() == 0)
                 {
                 return;
                 }

             auto codeEditor = dynamic_cast<CodeEditor*>(m_notebook->GetCurrentPage());
             if (codeEditor && codeEditor->GetModify())
                 {
                 if (wxMessageBox(_(L"Do you wish to save your unsaved changes?"),
                                  _(L"Save Script"), wxYES_NO | wxICON_QUESTION) == wxYES)
                     {
                     codeEditor->Save();
                     }
                 }
         });

    Bind(
        wxEVT_TOOL,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            m_notebook->Freeze();
            m_notebook->AddPage(CreateLuaScript(m_notebook), _(L"(unnamed)"), true,
                                wxGetApp().GetResourceManager().GetSVG(L"ribbon/lua.svg"));
            m_notebook->Thaw();
        },
        XRCID("ID_NEW"));

    Bind(
        wxEVT_TOOL,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            wxFileDialog dialogOpen(this, _(L"Select Script to Open"), wxEmptyString, wxEmptyString,
                                    _(L"Lua Scripts (*.lua)|*.lua"),
                                    wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);
            if (dialogOpen.ShowModal() != wxID_OK)
                {
                return;
                }
            const wxString filePath = dialogOpen.GetPath();

            auto scriptCtrl = CreateLuaScript(m_notebook);
            if (scriptCtrl)
                {
                // Just the generic default page open and nothing in it? Then get rid of it
                // now that we are opening an existing script.
                if (m_notebook->GetPageCount() == 1)
                    {
                    auto codeEditor = dynamic_cast<CodeEditor*>(m_notebook->GetCurrentPage());
                    if (codeEditor && !codeEditor->GetModify() &&
                        codeEditor->GetScriptFilePath().empty())
                        {
                        m_notebook->DeletePage(0);
                        }
                    }
                scriptCtrl->LoadFile(filePath);
                scriptCtrl->SetScriptFilePath(filePath);

                m_notebook->AddPage(scriptCtrl, wxFileName(filePath).GetName(), true,
                                    wxGetApp().GetResourceManager().GetSVG(L"ribbon/lua.svg"));
                scriptCtrl->SetFocus();
                }
        },
        XRCID("ID_OPEN"));

    Bind(wxEVT_TOOL, &LuaEditorDlg::OnSave, this, XRCID("ID_SAVE"));

    Bind(wxEVT_TOOL, [this]([[maybe_unused]] wxCommandEvent&) { DebugClear(); }, XRCID("ID_CLEAR"));

    Bind(
        wxEVT_TOOL,
        []([[maybe_unused]] wxCommandEvent&)
        {
            const wxString manualPath = wxGetApp().GetMainFrameEx()->GetHelpDirectory() +
                                        wxFileName::GetPathSeparator() +
                                        _DT(L"readability-studio-manual.pdf");
            if (wxFile::Exists(manualPath))
                {
                wxLaunchDefaultApplication(manualPath);
                }
        },
        wxID_HELP);

    Bind(
        wxEVT_TOOL,
        []([[maybe_unused]] wxCommandEvent&)
        {
            const wxString helpPath = wxGetApp().GetMainFrameEx()->GetHelpDirectory() +
                                      _DT(L"/lua-5.4/doc/contents.html");
            if (wxFile::Exists(helpPath))
                {
                wxLaunchDefaultBrowser(wxFileName::FileNameToURL(helpPath));
                }
        },
        XRCID("LUA_REFERENCE"));

    Bind(wxEVT_SEARCH,
         [this](wxCommandEvent& evt)
         {
             auto codeEditor = dynamic_cast<CodeEditor*>(m_notebook->GetCurrentPage());
             if (codeEditor)
                 {
                 codeEditor->FindNext(evt.GetString());
                 }
         });

    Bind(wxEVT_TOOL, &LuaEditorDlg::OnShowFindDialog, this, wxID_FIND);
    Bind(wxEVT_TOOL, &LuaEditorDlg::OnShowReplaceDialog, this, wxID_REPLACE);
    Bind(wxEVT_FIND, &LuaEditorDlg::OnFindDialog, this);
    Bind(wxEVT_FIND_NEXT, &LuaEditorDlg::OnFindDialog, this);
    Bind(wxEVT_FIND_REPLACE, &LuaEditorDlg::OnFindDialog, this);
    Bind(wxEVT_FIND_REPLACE_ALL, &LuaEditorDlg::OnFindDialog, this);
    Bind(wxEVT_FIND_CLOSE, &LuaEditorDlg::OnFindDialog, this);

    Bind(
        wxEVT_CHAR_HOOK,
        [this](wxKeyEvent& event)
        {
            if (event.GetKeyCode() == WXK_F3)
                {
                wxFindDialogEvent evt{ wxEVT_FIND_NEXT };
                evt.SetFindString(m_findData.GetFindString());
                evt.SetFlags(m_findData.GetFlags());
                OnFindDialog(evt);
                }
            event.Skip(true);
        },
        wxID_ANY);

    wxAcceleratorEntry accelEntries[6];
    accelEntries[0].Set(wxACCEL_CMD, static_cast<int>(L'N'), XRCID("ID_NEW"));
    accelEntries[1].Set(wxACCEL_CMD, static_cast<int>(L'O'), XRCID("ID_OPEN"));
    accelEntries[2].Set(wxACCEL_CMD, static_cast<int>(L'S'), XRCID("ID_SAVE"));
    accelEntries[3].Set(wxACCEL_CMD, static_cast<int>(L'F'), wxID_FIND);
    accelEntries[4].Set(wxACCEL_CMD, static_cast<int>(L'H'), wxID_REPLACE);
    accelEntries[5].Set(wxACCEL_NORMAL, WXK_F5, XRCID("ID_RUN"));
    wxAcceleratorTable accelTable(std::size(accelEntries), accelEntries);
    SetAcceleratorTable(accelTable);
    }

//------------------------------------------------------
void LuaEditorDlg::OnSave([[maybe_unused]] wxCommandEvent& event)
    {
    if (m_notebook->GetPageCount() == 0)
        {
        return;
        }

    if (dynamic_cast<CodeEditor*>(m_notebook->GetCurrentPage())->Save())
        {
        m_notebook->SetPageText(
            m_notebook->GetSelection(),
            wxFileName(dynamic_cast<CodeEditor*>(m_notebook->GetCurrentPage())->GetScriptFilePath())
                .GetName());
        }
    }

//------------------------------------------------------
void LuaEditorDlg::OnShowReplaceDialog([[maybe_unused]] wxCommandEvent& event)
    {
    auto currentScript = dynamic_cast<CodeEditor*>(m_notebook->GetCurrentPage());
    if (currentScript != nullptr && m_findData.GetFindString().empty())
        {
        m_findData.SetFindString(currentScript->GetSelectedText());
        }

    // get rid of Find dialog (if it was opened)
    if (m_dlgFind)
        {
        m_dlgFind->Destroy();
        m_dlgFind = nullptr;
        }
    if (m_dlgReplace == nullptr)
        {
        m_dlgReplace =
            new wxFindReplaceDialog(this, &m_findData, _(L"Replace"), wxFR_REPLACEDIALOG);
        }
    m_dlgReplace->Show(true);
    m_dlgReplace->SetFocus();
    }

//------------------------------------------------------
void LuaEditorDlg::OnShowFindDialog([[maybe_unused]] wxCommandEvent& event)
    {
    auto currentScript = dynamic_cast<CodeEditor*>(m_notebook->GetCurrentPage());
    if (currentScript != nullptr && m_findData.GetFindString().empty())
        {
        m_findData.SetFindString(currentScript->GetSelectedText());
        }

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
void LuaEditorDlg::OnFindDialog(wxFindDialogEvent& event)
    {
    if (m_notebook->GetPageCount() == 0)
        {
        return;
        }

    auto currentScript = dynamic_cast<CodeEditor*>(m_notebook->GetCurrentPage());
    if (currentScript == nullptr)
        {
        return;
        }

    if (event.GetEventType() == wxEVT_FIND || event.GetEventType() == wxEVT_FIND_NEXT)
        {
        currentScript->OnFind(event);
        }
    else if (event.GetEventType() == wxEVT_FIND_REPLACE)
        {
        long from{ 0 }, to{ 0 };
        currentScript->GetSelection(&from, &to);
        // force search to start from beginning of selection
        currentScript->SetSelection(from, from);
        currentScript->SearchAnchor();

        auto foundPos = currentScript->FindNext(event.GetFindString(), event.GetFlags());
        if (foundPos != wxSTC_INVALID_POSITION)
            {
            // if what is being replaced matches what was already selected, then replace it
            if (from == foundPos &&
                to == static_cast<long>(foundPos + event.GetFindString().length()))
                {
                currentScript->Replace(foundPos, foundPos + event.GetFindString().length(),
                                       event.GetReplaceString());
                currentScript->SetSelection(foundPos, foundPos + event.GetReplaceString().length());
                currentScript->SearchAnchor();
                // ...then, find the next occurrence of string being replaced for the next replace
                // button click
                foundPos = currentScript->FindNext(event.GetFindString(), event.GetFlags());
                if (foundPos != wxSTC_INVALID_POSITION)
                    {
                    currentScript->SetSelection(foundPos,
                                                foundPos + event.GetFindString().length());
                    }
                }
            // ...otherwise, just select the next string being replaced so that user can see it in
            // its context and then decide on the next replace button click if they want to replace
            // it
            else
                {
                currentScript->SetSelection(foundPos, foundPos + event.GetFindString().length());
                currentScript->SearchAnchor();
                }
            }
        else
            {
            currentScript->SetSelection(from, to);
            currentScript->SearchAnchor();
            wxMessageBox(_(L"No further occurrences found."), _(L"Item Not Found"),
                         wxOK | wxICON_INFORMATION, this);
            }
        }
    else if (event.GetEventType() == wxEVT_FIND_REPLACE_ALL)
        {
        currentScript->SetSelection(0, 0);
        currentScript->SearchAnchor();
        auto foundPos = currentScript->FindNext(event.GetFindString(), event.GetFlags(), false);
        while (foundPos != wxSTC_INVALID_POSITION)
            {
            currentScript->Replace(foundPos, foundPos + event.GetFindString().length(),
                                   event.GetReplaceString());
            currentScript->SetSelection(foundPos + event.GetReplaceString().length(),
                                        foundPos + event.GetReplaceString().length());
            currentScript->SearchAnchor();
            // ...then, find the next occurrence of string being replaced for the next loop
            foundPos = currentScript->FindNext(event.GetFindString(), event.GetFlags(), false);
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

//-------------------------------------------------------
void LuaEditorDlg::SetThemeColor(const wxColour& color)
    {
    if (!color.IsOk())
        {
        return;
        }

    m_mgr.GetArtProvider()->SetColour(wxAUI_DOCKART_BACKGROUND_COLOUR, color);

    ThemedAuiToolbarArt* toolbarArt = new ThemedAuiToolbarArt;
    toolbarArt->SetThemeColor(color);
    m_toolbar->SetArtProvider(toolbarArt);

    // notebook (and its children edit windows)
    FlatTabArt* notebookArt = new FlatTabArt();
    notebookArt->SetColour(color);
    m_notebook->SetArtProvider(notebookArt);

    const wxString htmlText = *(m_debugMessageWindow->GetParser()->GetSource());
    const auto debugReportBody =
        wxString::Format(
            L"<html>\n<body bgcolor=%s text=%s>", color.GetAsString(wxC2S_HTML_SYNTAX),
            ColorContrast::BlackOrWhiteContrast(color).GetAsString(wxC2S_HTML_SYNTAX)) +
        wxString(html_extract_text::get_body(htmlText.wc_str())) + L"\n</body>\n</html>";
    m_debugMessageWindow->SetPage(debugReportBody);

    m_mgr.Update();
    }

//-------------------------------------------------------
void LuaEditorDlg::DebugOutput(const wxString& str)
    {
    if (m_debugMessageWindow)
        {
        const wxColour bkColor = m_mgr.GetArtProvider()->GetColour(wxAUI_DOCKART_BACKGROUND_COLOUR);
        const wxString htmlText = wxString(html_extract_text::get_body(
                                      m_debugMessageWindow->GetParser()->GetSource()->wc_str())) +
                                  L"\n<br />" + str;
        const auto debugReportBody =
            wxString::Format(
                L"<html>\n<body bgcolor=%s text=%s>", bkColor.GetAsString(wxC2S_HTML_SYNTAX),
                ColorContrast::BlackOrWhiteContrast(bkColor).GetAsString(wxC2S_HTML_SYNTAX)) +
            htmlText + L"\n</body>\n</html>";
        m_debugMessageWindow->SetPage(debugReportBody);
        }
    }

//-------------------------------------------------------
void LuaEditorDlg::DebugClear()
    {
    if (m_debugMessageWindow)
        {
        const wxColour bkColor = m_mgr.GetArtProvider()->GetColour(wxAUI_DOCKART_BACKGROUND_COLOUR);
        const auto debugReportBody = wxString::Format(
            L"<html>\n<body bgcolor=%s text=%s>\n</body>\n</html>",
            bkColor.GetAsString(wxC2S_HTML_SYNTAX),
            ColorContrast::BlackOrWhiteContrast(bkColor).GetAsString(wxC2S_HTML_SYNTAX));
        m_debugMessageWindow->SetPage(debugReportBody);
        }
    }

//-------------------------------------------------------
CodeEditor* LuaEditorDlg::CreateLuaScript(wxWindow* parent)
    {
    auto codeEditor = new CodeEditor(parent, wxSTC_LEX_LUA);
    codeEditor->Show(false);
    codeEditor->IncludeNumberMargin(true);
    codeEditor->IncludeFoldingMargin(true);
    codeEditor->SetDefaultHeader(wxString::Format(_(L"-- Imports %s specific enumerations"),
                                                  wxGetApp().GetAppDisplayName()) +
                                 L"\ndofile(Application.GetLuaConstantsPath())\n\n");
    codeEditor->SetText(codeEditor->GetDefaultHeader());
    codeEditor->SetModified(false);

        // import API info
        {
        std::vector<std::vector<std::wstring>> apiStrings;

        lily_of_the_valley::standard_delimited_character_column tabbedColumn(
            lily_of_the_valley::text_column_delimited_character_parser{ L'\t' }, 1);
        lily_of_the_valley::standard_delimited_character_column semiColonColumn(
            lily_of_the_valley::text_column_delimited_character_parser{ L';' }, std::nullopt);
        lily_of_the_valley::text_row<std::wstring> row(std::nullopt);
        row.treat_consecutive_delimitors_as_one(true); // skip consecutive semicolons
        row.add_column(tabbedColumn);
        row.add_column(semiColonColumn);
        row.allow_column_resizing(true);

        lily_of_the_valley::text_matrix<std::wstring> importer(&apiStrings);
        importer.add_row_definition(
            lily_of_the_valley::text_row<std::wstring>(1)); // skip warning in first line
        importer.add_row_definition(row);

        lily_of_the_valley::text_preview preview;

        // library/enum file
        wxString libraryText;
        wxString libFilePath = wxGetApp().FindResourceFile(L"rs-classes.api");
        if (Wisteria::TextStream::ReadFile(libFilePath, libraryText))
            {
            apiStrings.clear();

            // see how many lines are in the file and resize the container
            const size_t rowCount = preview(libraryText, L'\t', true, false);
            if (rowCount > 0)
                {
                apiStrings.resize(rowCount);
                importer.read(libraryText, rowCount, 2, true);

                for (auto& lib : apiStrings)
                    {
                    if (lib.size())
                        {
                        const std::wstring libName = lib.front();
                        lib.erase(lib.begin(), lib.begin() + 1);
                        CodeEditor::NameList nl;
                        for (const auto& className : lib)
                            {
                            nl.insert(className);
                            }
                        codeEditor->AddClass(libName, nl);
                        }
                    }
                }
            }

        apiStrings.clear();
        libFilePath = wxGetApp().FindResourceFile(L"rs-libraries.api");
        if (Wisteria::TextStream::ReadFile(libFilePath, libraryText))
            {
            // see how many lines are in the file and resize the container
            const size_t rowCount = preview(libraryText, L'\t', true, false);
            if (rowCount > 0)
                {
                apiStrings.resize(rowCount);
                importer.read(libraryText, rowCount, 2, true);

                for (auto& lib : apiStrings)
                    {
                    if (lib.size())
                        {
                        const std::wstring libName = lib.front();
                        lib.erase(lib.begin(), lib.begin() + 1);
                        CodeEditor::NameList nl;
                        for (const auto& lName : lib)
                            {
                            nl.insert(lName);
                            }
                        codeEditor->AddLibrary(libName, nl);
                        }
                    }
                }
            }

        apiStrings.clear();
        libFilePath = wxGetApp().FindResourceFile(L"rs-enums.api");
        if (Wisteria::TextStream::ReadFile(libFilePath, libraryText))
            {
            // see how many lines are in the file and resize the container
            const size_t rowCount = preview(libraryText, L'\t', true, false);
            if (rowCount > 0)
                {
                apiStrings.resize(rowCount);
                importer.read(libraryText, rowCount, 2, true);

                for (auto& lib : apiStrings)
                    {
                    if (lib.size())
                        {
                        const std::wstring libName = lib.front();
                        lib.erase(lib.begin(), lib.begin() + 1);
                        CodeEditor::NameList nl;
                        for (const auto& lName : lib)
                            {
                            nl.insert(lName);
                            }
                        codeEditor->AddLibrary(libName, nl);
                        }
                    }
                }
            }
        }

    codeEditor->Finalize();
    codeEditor->SetSelection(codeEditor->GetTextLength(), codeEditor->GetTextLength());
    codeEditor->Show();

    return codeEditor;
    }

//-------------------------------------------------------
void LuaEditorDlg::CreateControls()
    {
    m_toolbar = new wxAuiToolBar(this, wxID_ANY);
    m_toolbar->SetToolBitmapSize(FromDIP(wxSize{ 16, 16 }));
    // wxID_NEW and such trigger parent events
    m_toolbar->AddTool(XRCID("ID_NEW"), _(L"New"),
                       wxArtProvider::GetBitmapBundle(wxART_NEW, wxART_BUTTON),
                       _(L"Create a new script."));
    m_toolbar->AddTool(XRCID("ID_OPEN"), _(L"Open"),
                       wxArtProvider::GetBitmapBundle(wxART_FILE_OPEN, wxART_BUTTON),
                       _(L"Open a script."));
    m_toolbar->AddTool(XRCID("ID_SAVE"), _(L"Save"),
                       wxArtProvider::GetBitmapBundle(wxART_FILE_SAVE, wxART_BUTTON),
                       _(L"Save the script."));
    m_toolbar->AddTool(XRCID("ID_RUN"), _(L"Run"),
                       wxArtProvider::GetBitmapBundle(L"ID_RUN", wxART_BUTTON),
                       _(L"Execute the script (or selection)."));
    m_toolbar->AddTool(XRCID("ID_STOP"), _(L"Stop"),
                       wxArtProvider::GetBitmapBundle(wxART_QUIT, wxART_BUTTON),
                       _(L"Stop the currently running script."));
    m_toolbar->EnableTool(XRCID("ID_STOP"), false);
    m_toolbar->AddSeparator();
    m_toolbar->AddTool(wxID_FIND, _(L"Find"),
                       wxArtProvider::GetBitmapBundle(wxART_FIND, wxART_BUTTON), _(L"Find text."));
    m_toolbar->AddTool(wxID_REPLACE, _(L"Replace"),
                       wxArtProvider::GetBitmapBundle(wxART_FIND_AND_REPLACE, wxART_BUTTON),
                       _(L"Replace text."));
    m_toolbar->AddSeparator();
    m_toolbar->AddTool(XRCID("ID_CLEAR"), _(L"Clear"),
                       wxArtProvider::GetBitmapBundle(L"ID_CLEAR", wxART_BUTTON),
                       _(L"Clear the log window."));
    m_toolbar->AddSeparator();
    m_toolbar->AddTool(wxID_HELP, _(L"Content"),
                       wxArtProvider::GetBitmapBundle(wxART_HELP_BOOK, wxART_BUTTON),
                       _(L"View the documentation."));
    m_toolbar->AddTool(XRCID("LUA_REFERENCE"), _(L"Lua Reference"),
                       wxArtProvider::GetBitmapBundle(L"ID_E_HELP", wxART_BUTTON),
                       _(L"View the Lua Reference Manual."));

    m_toolbar->Realize();
    m_mgr.AddPane(m_toolbar, wxAuiPaneInfo()
                                 .Name(L"auitoolbar")
                                 .Caption(_(L"Tools"))
                                 .ToolbarPane()
                                 .Top()
                                 .CloseButton(false)
                                 .Fixed());

    m_notebook = new wxAuiNotebook(this, wxID_ANY, wxPoint{ 0, 0 }, FromDIP(wxSize{ 400, 200 }),
                                   wxAUI_NB_TOP | wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE |
                                       wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_CLOSE_ON_ALL_TABS |
                                       wxAUI_NB_MIDDLE_CLICK_CLOSE | wxAUI_NB_TAB_EXTERNAL_MOVE |
                                       wxNO_BORDER);

    m_notebook->AddPage(CreateLuaScript(m_notebook), _(L"(unnamed)"), true,
                        wxGetApp().GetResourceManager().GetSVG(L"ribbon/lua.svg"));

    m_mgr.AddPane(m_notebook, wxAuiPaneInfo().Name(L"auinotebook").CenterPane().PaneBorder(false));

    m_debugMessageWindow = new wxHtmlWindow(this);
    m_mgr.AddPane(m_debugMessageWindow, wxAuiPaneInfo()
                                            .Name(L"auidebug")
                                            .Bottom()
                                            .MinimizeButton(true)
                                            .MaximizeButton(true)
                                            .Caption(_(L"Debug Output"))
                                            .FloatingSize(FromDIP(wxSize{ 800, 200 }))
                                            .BestSize(FromDIP(wxSize{ 800, 100 }))
                                            .PinButton(true)
                                            .CloseButton(false));

    SetSize(FromDIP(wxSize{ 1200, 1200 }));

    m_mgr.Update();
    }

//-------------------------------------------------------
void LuaEditorDlg::OnClose([[maybe_unused]] wxCloseEvent& event)
    {
    // ask about any unsaved changes
    for (size_t i = 0; i < m_notebook->GetPageCount(); ++i)
        {
        auto codeEditor = dynamic_cast<CodeEditor*>(m_notebook->GetPage(i));
        if (codeEditor && codeEditor->GetModify() &&
            (wxMessageBox(_(L"Do you wish to save your unsaved changes?"), _(L"Save Script"),
                          wxYES_NO | wxICON_QUESTION) == wxYES))
            {
            if (codeEditor->Save())
                {
                m_notebook->SetPageText(i, wxFileName{ codeEditor->GetScriptFilePath() }.GetName());
                }
            }
        }

    // this frame is meant to be modeless, so just hide it when closing and let
    // the parent app clean it up on app exit
    HideAllWindows();
    }
