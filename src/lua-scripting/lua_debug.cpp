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

#include "lua_debug.h"
#include "../Wisteria-Dataviz/src/base/reportbuilder.h"
#include "../app/readability_app.h"

// NOLINTBEGIN(readability-identifier-length)
// NOLINTBEGIN(readability-implicit-bool-conversion)

wxDECLARE_APP(ReadabilityApp);

namespace LuaScripting
    {
    //-------------------------------------------------------------
    wxColour LoadColor(wxString colorStr)
        {
        wxColour color{ colorStr.MakeLower() };
        if (!color.IsOk())
            {
            auto foundColor = Wisteria::ReportBuilder::GetColorMap().find(colorStr.wc_str());
            if (foundColor != Wisteria::ReportBuilder::GetColorMap().cend())
                {
                color = Wisteria::Colors::ColorBrewer::GetColor(foundColor->second);
                }
            }
        return color;
        }

    //-------------------------------------------------------------
    void LoadFontAttributes(lua_State* L, wxFont& font, wxColour& fontColor, bool calledFromObject)
        {
        int paramIndex = (calledFromObject ? 2 : 1);
        // name, point size, weight, color (as a string)
        wxString fontName{ luaL_checkstring(L, paramIndex++), wxConvUTF8 };
        if (fontName.CmpNoCase(_DT(L"MONOSPACE")) == 0)
            {
            fontName = Wisteria::GraphItems::Label::GetFirstAvailableMonospaceFont();
            }
        else if (fontName.CmpNoCase(_DT(L"CURSIVE")) == 0)
            {
            fontName = Wisteria::GraphItems::Label::GetFirstAvailableCursiveFont();
            }
        if (!fontName.empty())
            {
            font.SetFaceName(fontName);
            }

        if (lua_gettop(L) >= paramIndex)
            {
            const double pointSize{ luaL_checknumber(L, paramIndex++) };
            if (pointSize > 0)
                {
                font.SetFractionalPointSize(pointSize);
                }
            }

        if (lua_gettop(L) >= paramIndex)
            {
            const int fontWeight{ static_cast<int>(luaL_checkinteger(L, paramIndex++)) };
            if (fontWeight > 0)
                {
                font.SetWeight(static_cast<wxFontWeight>(fontWeight));
                }
            }

        if (lua_gettop(L) >= paramIndex)
            {
            fontColor = LoadColor(wxString{ luaL_checkstring(L, paramIndex++), wxConvUTF8 });
            }
        }

    //-------------------------------------------------------------
    bool VerifyParameterCount(lua_State* L, const int minParemeterCount,
                              const wxString& functionName)
        {
        assert(L);
        assert(minParemeterCount >= 0);
        if (lua_gettop(L) < minParemeterCount)
            {
            wxMessageBox(wxString::Format(
                             // TRANSLATORS: %s is a function name that failed from a script
                             _(L"%s: Invalid number of arguments.\n\n%d expected, %d provided."),
                             functionName, minParemeterCount, lua_gettop(L)),
                         _(L"Script Error"), wxOK | wxICON_EXCLAMATION);
            return false;
            }
        else
            {
            return true;
            }
        }

    //-------------------------------------------------------------
    void DebugPrint(const wxString& str)
        {
        wxGetApp().GetMainFrameEx()->GetLuaEditor()->DebugOutput(str);
        wxGetApp().Yield();
        }

    //-------------------------------------------------------------
    int GetScriptFolder(lua_State* L)
        {
        if (wxGetApp().GetLuaRunner().GetScriptFilePath().empty())
            {
            DebugPrint(wxString::Format( // TRANSLATORS: %s are highlighting tags and
                                         // should stay wrapped around "Error"
                _(L"%sError%s: call to %s returned empty because "
                  "the script has not been saved yet."),
                L"<span style='color:red; font-weight:bold;'>", L"</span>", __func__));
            lua_pushstring(L, "");
            return 1;
            }
        lua_pushstring(
            L, wxFileName{ wxGetApp().GetLuaRunner().GetScriptFilePath() }.GetPathWithSep());
        return 1;
        }

    //-------------------------------------------------------------
    int Print(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        DebugPrint(wxString(luaL_checkstring(L, 1), wxConvUTF8));
        return 0;
        }

    //-------------------------------------------------------------
    int Clear([[maybe_unused]] lua_State* L)
        {
        wxGetApp().GetMainFrameEx()->GetLuaEditor()->DebugClear();
        wxGetApp().Yield();
        return 0;
        }
    } // namespace LuaScripting

// NOLINTEND(readability-implicit-bool-conversion)
// NOLINTEND(readability-identifier-length)
