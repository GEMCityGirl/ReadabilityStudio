///////////////////////////////////////////////////////////////////////////////
// Name:        lua_editor_dlg.cpp
// Author:      Blake Madden
// Copyright:   (c) 2005-2023 Blake Madden
// Licence:     3-Clause BSD licence
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "lua_editor_dlg.h"
#include "../../app/readability_app.h"
#include "../../Wisteria-Dataviz/src/ui/ribbon/artmetro.h"
#include "../../Wisteria-Dataviz/src/base/colorbrewer.h"
#include "../../Wisteria-Dataviz/src/import/html_extract_text.h"

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
    FlatTabArt() : wxAuiGenericTabArt(){}

    /// @private
    wxAuiTabArt* Clone() final
        { return new FlatTabArt(*this); }

    /// @private
    //-------------------------------------------------------
    void DrawBackground(wxDC& dc,
                        [[maybe_unused]] wxWindow* wnd,
                        const wxRect& rect) final
        {
        wxRect r;

        if (m_flags & wxAUI_NB_BOTTOM)
            { r = wxRect(rect.x, rect.y, rect.width+2, rect.height); }
        // TODO: else if (m_flags &wxAUI_NB_LEFT) {}
        // TODO: else if (m_flags &wxAUI_NB_RIGHT) {}
        else // for wxAUI_NB_TOP
            { r = wxRect(rect.x, rect.y, rect.width+2, rect.height-3); }

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
    static void ScaleBitmap(wxBitmap& bmp, double scale)
        {
        #if wxUSE_IMAGE && !defined(__WXGTK3__) && !defined(__WXMAC__)
            // scale to a close round number to improve quality
            scale = std::floor(scale + 0.25);
            if (scale > 1.0 && !(bmp.GetScaleFactor() > 1.0))
                {
                wxImage img = bmp.ConvertToImage();
                img.Rescale(bmp.GetWidth()*scale, bmp.GetHeight()*scale,
                    wxIMAGE_QUALITY_BOX_AVERAGE);
                bmp = wxBitmap(img);
                }
        #else
            wxUnusedVar(bmp);
            wxUnusedVar(scale);
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
            return text;

        size_t last_good_length = 0;
        for (size_t i = 0; i < text.length(); ++i)
            {
            wxString s = text.Left(i);
            s += L"...";

            dc.GetTextExtent(s, &x, &y);
            if (x > max_size)
                break;

            last_good_length = i;
            }

        wxString ret = text.Left(last_good_length);
        ret += L"...";
        return ret;
        }

    /// @private
    //-------------------------------------------------------
    void DrawTab(wxDC& dc,
                wxWindow* wnd,
                const wxAuiNotebookPage& page,
                const wxRect& in_rect,
                int close_button_state,
                wxRect* out_tab_rect,
                wxRect* out_button_rect,
                int* x_extent) final
        {
        if (!wnd)
            { return; }
        wxCoord normal_textx, normal_texty;
        wxCoord selected_textx, selected_texty;
        wxCoord texty;

        // if the caption is empty, measure some temporary text
        wxString caption = page.caption;
        if (caption.empty())
            caption = L"Xj";

        dc.SetFont(m_selectedFont);
        dc.GetTextExtent(caption, &selected_textx, &selected_texty);

        dc.SetFont(m_normalFont);
        dc.GetTextExtent(caption, &normal_textx, &normal_texty);

        // figure out the size of the tab
        const wxSize tab_size = GetTabSize(dc,
            wnd,
            page.caption,
            page.bitmap,
            page.active,
            close_button_state,
            x_extent);

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
            clip_width = (in_rect.x + in_rect.width) - tab_x;

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
        dc.DrawPolygon(WXSIZEOF(border_points), border_points);

        // there are two horizontal grey lines at the bottom of the tab control,
        // this gets rid of the top one of those lines in the tab control
        if (page.active)
            {
            if (m_flags & wxAUI_NB_BOTTOM)
                dc.SetPen(wxPen(m_baseColour.ChangeLightness(170)));
            // TODO: else if (m_flags &wxAUI_NB_LEFT) {}
            // TODO: else if (m_flags &wxAUI_NB_RIGHT) {}
            else // for wxAUI_NB_TOP
                dc.SetPen(m_baseColourPen);
            dc.DrawLine(border_points[0].x + 1,
                border_points[0].y,
                border_points[5].x,
                border_points[5].y);
            }

        int text_offset;
        int bitmap_offset = 0;
        if (page.bitmap.IsOk())
            {
            bitmap_offset = tab_x + wnd->FromDIP(8);

            // draw bitmap
            dc.DrawBitmap(page.bitmap.GetBitmapFor(wnd),
                bitmap_offset,
                drawn_tab_yoff + (drawn_tab_height / 2) -
                (page.bitmap.GetBitmapFor(wnd).GetScaledHeight() / 2),
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
            closeImg = Image::ChangeColor(closeImg, *wxBLACK,
                page.active ?
                ColorContrast::BlackOrWhiteContrast(ColorContrast::Shade(m_baseColour)) :
                ColorContrast::BlackOrWhiteContrast(m_baseColour));

            wxBitmap bmp{ closeImg };

            int offsetY = tab_y - 1;
            if (m_flags & wxAUI_NB_BOTTOM)
                offsetY = 1;

            wxRect rect(tab_x + tab_width - bmp.GetWidth() - wnd->FromDIP(1),
                offsetY + (tab_height / 2) - (bmp.GetHeight() / 2),
                bmp.GetWidth(),
                tab_height);

            IndentPressedBitmap(wnd->FromDIP(wxSize(1, 1)), &rect, close_button_state);
            dc.DrawBitmap(bmp, rect.x, rect.y, true);

            *out_button_rect = rect;
            close_button_width = bmp.GetWidth();
            }

        wxString draw_text = ChopText(dc,
            caption,
            tab_width - (text_offset - tab_x) - close_button_width);

        // draw tab text
        const wxColor font_color = page.active ?
            ColorContrast::BlackOrWhiteContrast(ColorContrast::Shade(m_baseColour)) :
            ColorContrast::BlackOrWhiteContrast(m_baseColour);
        dc.SetTextForeground(font_color);
        dc.DrawText(draw_text,
            text_offset,
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
                focusRectBitmap =
                wxRect(bitmap_offset,
                    drawn_tab_yoff + (drawn_tab_height / 2) -
                    (page.bitmap.GetBitmapFor(wnd).GetScaledHeight() / 2),
                    page.bitmap.GetBitmapFor(wnd).GetScaledWidth(),
                    page.bitmap.GetBitmapFor(wnd).GetScaledHeight());

            if (page.bitmap.IsOk() && draw_text.empty())
                focusRect = focusRectBitmap;
            else if (!page.bitmap.IsOk() && !draw_text.empty())
                focusRect = focusRectText;
            else if (page.bitmap.IsOk() && !draw_text.empty())
                focusRect = focusRectText.Union(focusRectBitmap);

            focusRect.Inflate(2, 2);

            wxRendererNative::Get().DrawFocusRect(wnd, dc, focusRect, 0);
            }

        *out_tab_rect = wxRect(tab_x, tab_y, tab_width, tab_height);

        dc.DestroyClippingRegion();
        }
    };

class ThemedToolbarArt : public wxAuiGenericToolBarArt
    {
public:
    void SetThemeColor(const wxColour& color)
        { m_baseColour = color; }
    };

//-------------------------------------------------------
LuaEditorDlg::LuaEditorDlg(wxWindow* parent, wxWindowID id /*= wxID_ANY*/,
        const wxString& caption /*= _(L"Lua Script")*/,
        const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/,
        long style /*= wxCAPTION|wxCLOSE_BOX|wxMINIMIZE_BOX|wxMAXIMIZE_BOX|wxRESIZE_BORDER*/) :
        wxFrame(parent, id, caption, pos, size, style),
        m_debugMessageWindow(nullptr)
    {
    m_mgr.SetManagedWindow(this);

    wxIcon ico;
    ico.CopyFromBitmap(wxGetApp().GetResourceManager().
            GetSVG(L"ribbon/lua.svg").GetBitmap(FromDIP(wxSize(16, 16))));
    SetIcon(ico);

    CreateControls();
    Centre();

    Bind(wxEVT_CLOSE_WINDOW, &LuaEditorDlg::OnClose, this);

    Bind(wxEVT_TOOL,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
        // hold down the SHIFT key to run "silently" (closes the window and runs it)
        if (wxGetMouseState().ShiftDown())
            {
            Show(false);
            wxGetApp().GetLuaRunner().RunLuaCode(
                dynamic_cast<CodeEditor*>(m_notebook->GetCurrentPage())->GetValue(),
                dynamic_cast<CodeEditor*>(m_notebook->GetCurrentPage())->GetScriptFilePath());
            Show();
            }
        else
            {
            // disable (and later reenable) the Run button while the script runs
            m_toolbar->EnableTool(XRCID("ID_RUN"), false);

            // run the script
            wxGetApp().GetLuaRunner().RunLuaCode(
                dynamic_cast<CodeEditor*>(m_notebook->GetCurrentPage())->GetValue(),
                dynamic_cast<CodeEditor*>(m_notebook->GetCurrentPage())->GetScriptFilePath());

            m_toolbar->EnableTool(XRCID("ID_RUN"), true);
            }
        },
        XRCID("ID_RUN"));

    Bind(wxEVT_AUINOTEBOOK_PAGE_CLOSE,
        [this]([[maybe_unused]] wxCommandEvent&)
            {
            auto codeEditor = dynamic_cast<CodeEditor*>(m_notebook->GetCurrentPage());
            if (codeEditor && codeEditor->GetModify())
                {
                if (wxMessageBox(_(L"Do you wish to save your unsaved changes?"),
                    _(L"Save Script"), wxYES_NO|wxICON_QUESTION) == wxYES)
                    { codeEditor->Save(); }
                }
            });

    Bind(wxEVT_TOOL,
        [this]([[maybe_unused]] wxCommandEvent&)
            {
            m_notebook->Freeze();
            m_notebook->AddPage(CreateLuaScript(m_notebook), _(L"(unnamed)"), true,
                wxGetApp().GetResourceManager().GetSVG(L"ribbon/lua.svg"));
            m_notebook->Thaw();
            },
        XRCID("ID_NEW"));

    Bind(wxEVT_TOOL,
        [this]([[maybe_unused]] wxCommandEvent&)
            {
            wxFileDialog dialogOpen
                    (this, _(L"Select Script to Open"),
                    wxEmptyString, wxEmptyString,
                    _(L"Lua Script (*.lua)|*.lua"),
                    wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_PREVIEW);
            if (dialogOpen.ShowModal() != wxID_OK)
                { return; }
            const wxString filePath = dialogOpen.GetPath();

            auto scriptCtrl = CreateLuaScript(m_notebook);
            if (scriptCtrl)
                {
                // Just the generic default page open and nothing in it? Then get rid of it
                // now that we are opening an existing script.
                if (m_notebook->GetPageCount() == 1)
                    {
                    auto codeEditor = dynamic_cast<CodeEditor*>(m_notebook->GetCurrentPage());
                    if (codeEditor && !codeEditor->GetModify() && codeEditor->GetScriptFilePath().empty())
                        { m_notebook->DeletePage(0); }
                    }
                scriptCtrl->LoadFile(filePath);
                scriptCtrl->SetScriptFilePath(filePath);

                m_notebook->AddPage(scriptCtrl, wxFileName(filePath).GetName(),
                    true, wxGetApp().GetResourceManager().GetSVG(L"ribbon/lua.svg"));
                }
            },
        XRCID("ID_OPEN"));

    Bind(wxEVT_TOOL,
        [this]([[maybe_unused]] wxCommandEvent&)
            {
            if (dynamic_cast<CodeEditor*>(m_notebook->GetCurrentPage())->Save())
                {
                m_notebook->SetPageText(m_notebook->GetSelection(),
                    wxFileName(
                        dynamic_cast<CodeEditor*>(m_notebook->GetCurrentPage())->
                            GetScriptFilePath()).GetName());
                }
            },
        XRCID("ID_SAVE"));

    Bind(wxEVT_TOOL,
        [this]([[maybe_unused]] wxCommandEvent&)
            {
            DebugClear();
            },
        XRCID("ID_CLEAR"));

    Bind(wxEVT_TOOL,
        [this]([[maybe_unused]] wxCommandEvent&)
            {
            const wxString helpPath =
                wxGetApp().FindResourceFile(L"ReadabilityStudioAPI/index.html");
            if (wxFile::Exists(helpPath))
                { wxLaunchDefaultBrowser(wxFileName::FileNameToURL(helpPath)); }
            },
        wxID_HELP);

    Bind(wxEVT_TOOL,
        [this]([[maybe_unused]] wxCommandEvent&)
            {
            const wxString helpPath = wxGetApp().FindResourceFile(L"LuaManual/contents.html");
            if (wxFile::Exists(helpPath))
                { wxLaunchDefaultBrowser(wxFileName::FileNameToURL(helpPath)); }
            },
        XRCID("LUA_REFERENCE"));

    Bind(wxEVT_SEARCH,
        [this](wxCommandEvent& evt)
            {
            auto codeEditor = dynamic_cast<CodeEditor*>(m_notebook->GetCurrentPage());
            if (codeEditor)
                { codeEditor->FindNext(evt.GetString()); }
            });
    }

//-------------------------------------------------------
void LuaEditorDlg::SetThemeColor(const wxColour& color)
    {
    m_mgr.GetArtProvider()->SetColour(wxAUI_DOCKART_BACKGROUND_COLOUR, color);

    ThemedToolbarArt* toolbarArt = new ThemedToolbarArt();
    toolbarArt->SetThemeColor(color);
    m_toolbar->SetArtProvider(toolbarArt);

    // notebook (and its children edit windows)
    FlatTabArt* notebookArt = new FlatTabArt();
    notebookArt->SetColour(color);
    m_notebook->SetArtProvider(notebookArt);

    for (size_t i = 0; i < m_notebook->GetPageCount(); ++i)
        {
        auto codeEditor = dynamic_cast<CodeEditor*>(m_notebook->GetPage(i));
        if (codeEditor)
            {
            codeEditor->SetThemeColor(
                m_mgr.GetArtProvider()->GetColour(wxAUI_DOCKART_BACKGROUND_COLOUR));
            }
        }

    const wxString htmlText = *(m_debugMessageWindow->GetParser()->GetSource());
    const auto debugReportBody = wxString::Format(
        L"<html>\n<body bgcolor=%s text=%s>",
            color.GetAsString(wxC2S_HTML_SYNTAX),
            ColorContrast::BlackOrWhiteContrast(color).GetAsString(wxC2S_HTML_SYNTAX)) +
        wxString(html_extract_text::get_body(htmlText.wc_str())) +
        L"\n</body>\n</html>";
    m_debugMessageWindow->SetPage(debugReportBody);

    m_mgr.Update();
    }

//-------------------------------------------------------
void LuaEditorDlg::DebugOutput(const wxString& str)
    {
    if (m_debugMessageWindow)
        {
        const wxColour bkColor =
            m_mgr.GetArtProvider()->GetColour(wxAUI_DOCKART_BACKGROUND_COLOUR);
        const wxString htmlText =
            wxString(html_extract_text::get_body(
                m_debugMessageWindow->GetParser()->GetSource()->wc_str())) +
                L"\n<br />" + str;
        const auto debugReportBody = wxString::Format(
            L"<html>\n<body bgcolor=%s text=%s>",
            bkColor.GetAsString(wxC2S_HTML_SYNTAX),
            ColorContrast::BlackOrWhiteContrast(bkColor).GetAsString(wxC2S_HTML_SYNTAX)) +
            htmlText +
            L"\n</body>\n</html>";
        m_debugMessageWindow->SetPage(debugReportBody);
        }
    }

//-------------------------------------------------------
void LuaEditorDlg::DebugClear()
    {
    if (m_debugMessageWindow)
        {
        const wxColour bkColor =
            m_mgr.GetArtProvider()->GetColour(wxAUI_DOCKART_BACKGROUND_COLOUR);
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
    auto codeEditor = new CodeEditor(parent, wxID_ANY);
    codeEditor->Show(false);
    codeEditor->SetLanguage(wxSTC_LEX_LUA);
    codeEditor->IncludeNumberMargin(true);
    codeEditor->IncludeFoldingMargin(true);
    codeEditor->SetDefaultHeader(L"dofile(Application.GetLuaConstantsPath())\n\n");
    codeEditor->SetText(codeEditor->GetDefaultHeader());
    codeEditor->SetModified(false);
    codeEditor->SetThemeColor(m_mgr.GetArtProvider()->GetColour(wxAUI_DOCKART_BACKGROUND_COLOUR));

    // import API info
        {
        std::vector<std::vector<std::wstring>> apiStrings;

        lily_of_the_valley::standard_delimited_character_column
            tabbedColumn(lily_of_the_valley::text_column_delimited_character_parser{ L'\t' }, 1);
        lily_of_the_valley::standard_delimited_character_column
            commaColumn(lily_of_the_valley::text_column_delimited_character_parser{ L',' },
                        std::nullopt);
        lily_of_the_valley::text_row<std::wstring> row(std::nullopt);
        row.treat_consecutive_delimitors_as_one(true); // skip consecutive commas
        row.add_column(tabbedColumn);
        row.add_column(commaColumn);
        row.allow_column_resizing(true);

        lily_of_the_valley::text_matrix<std::wstring> importer(&apiStrings);
            importer.add_row_definition(row);

        lily_of_the_valley::text_preview preview;

        // library/enum file
        wxString libraryText;
        wxString libFilePath = wxGetApp().FindResourceFile(L"RSClasses.api");
        if (Wisteria::TextStream::ReadFile(libFilePath, libraryText))
            {
            apiStrings.clear();

            // see how many lines are in the file and resize the container
            const size_t rowCount = preview(libraryText, L'\t', true, false);
            if (rowCount > 0)
                {
                apiStrings.resize(rowCount);
                importer.read(libraryText, rowCount, 3, true);

                for (auto& lib : apiStrings)
                    {
                    if (lib.size())
                        {
                        const auto libName = lib.front();
                        lib.erase(lib.begin(), lib.begin() + 1);
                        CodeEditor::NameList nl;
                        for (const auto& className : lib)
                            { nl.insert(className); }
                        codeEditor->AddClass(libName, nl);
                        }
                    }
                }
            }

        libFilePath = wxGetApp().FindResourceFile(L"RSLibraries.api");
        if (Wisteria::TextStream::ReadFile(libFilePath, libraryText))
            {
            apiStrings.clear();

            // see how many lines are in the file and resize the container
            const size_t rowCount = preview(libraryText, L'\t', true, false);
            if (rowCount > 0)
                {
                apiStrings.resize(rowCount);
                importer.read(libraryText, rowCount, 3, true);

                for (auto& lib : apiStrings)
                    {
                    if (lib.size())
                        {
                        const auto libName = lib.front();
                        lib.erase(lib.begin(), lib.begin() + 1);
                        CodeEditor::NameList nl;
                        for (const auto& lName : lib)
                            { nl.insert(lName); }
                        codeEditor->AddLibrary(libName, nl);
                        }
                    }
                }
            }
        }

    codeEditor->Finalize();
    codeEditor->Show();

    return codeEditor;
    }

//-------------------------------------------------------
void LuaEditorDlg::CreateControls()
    {
    m_toolbar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
        wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW | wxAUI_TB_VERTICAL);
    m_toolbar->SetToolBitmapSize(FromDIP(wxSize(16, 16)));
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
        _(L"Execute the script."));
    m_toolbar->AddSeparator();
    m_toolbar->AddTool(XRCID("ID_CLEAR"), _(L"Clear"),
        wxArtProvider::GetBitmapBundle(L"ID_CLEAR", wxART_BUTTON),
        _(L"Clear the log window."));
    m_toolbar->AddSeparator();
    m_toolbar->AddTool(wxID_HELP, _(L"Content"),
        wxArtProvider::GetBitmapBundle(wxART_HELP, wxART_BUTTON),
        _(L"View the documentation."));
    m_toolbar->AddTool(XRCID("LUA_REFERENCE"), _(L"Lua Reference"),
        wxArtProvider::GetBitmapBundle(wxART_HELP_BOOK, wxART_BUTTON),
        _(L"View the Lua Reference Manual."));
    m_toolbar->AddSeparator();
    m_toolbar->AddControl(new wxSearchCtrl(m_toolbar, wxID_ANY));

    m_toolbar->Realize();
    m_mgr.AddPane(m_toolbar, wxAuiPaneInfo().
        Name("auitoolbar").Caption(_(L"Tools")).
        ToolbarPane().Top().CloseButton(false).Fixed());

    m_notebook = new wxAuiNotebook(this, wxID_ANY,
        wxPoint(0, 0),
        FromDIP(wxSize(400, 200)),
        wxAUI_NB_TOP|wxAUI_NB_TAB_SPLIT|wxAUI_NB_TAB_MOVE|wxAUI_NB_SCROLL_BUTTONS|
        wxAUI_NB_CLOSE_ON_ALL_TABS|wxAUI_NB_MIDDLE_CLICK_CLOSE|
        wxAUI_NB_TAB_EXTERNAL_MOVE|wxNO_BORDER);

    m_notebook->AddPage(CreateLuaScript(m_notebook), _(L"(unnamed)"), true,
        wxGetApp().GetResourceManager().GetSVG(L"ribbon/lua.svg"));

    m_mgr.AddPane(m_notebook,
        wxAuiPaneInfo().Name(L"auinotebook").
        CenterPane().PaneBorder(false));

    m_debugMessageWindow = new wxHtmlWindow(this);
    m_mgr.AddPane(m_debugMessageWindow,
        wxAuiPaneInfo().Name(L"auidebug").
        Bottom().MinimizeButton(true).MaximizeButton(true).
        Caption(_(L"Debug Output")).FloatingSize(FromDIP(wxSize(800, 200))).
        BestSize(FromDIP(wxSize(800, 100))).
        PinButton(true).CloseButton(false));

    SetSize(FromDIP(wxSize(800, 700)));

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
            (wxMessageBox(_(L"Do you wish to save your unsaved changes?"),
                _(L"Save Script"), wxYES_NO | wxICON_QUESTION) == wxYES))
            {
            if (codeEditor->Save())
                {
                m_notebook->SetPageText(i, wxFileName(codeEditor->GetScriptFilePath()).GetName());
                }
            }
        }

    // this frame is meant to be modeless, so just hide it when closing and let
    // the parent app clean it up on app exit
    Hide();
    }
