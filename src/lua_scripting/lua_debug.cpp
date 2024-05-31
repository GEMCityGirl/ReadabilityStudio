///////////////////////////////////////////////////////////////////////////////
// Name:        lua_debug.cpp
// Author:      Blake Madden
// Copyright:   (c) 2005-2023 Blake Madden
// Licence:     3-Clause BSD licence
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "lua_debug.h"
#include "../app/readability_app.h"

wxDECLARE_APP(ReadabilityApp);

namespace LuaScripting
    {
    //-------------------------------------------------------------
    bool VerifyParameterCount(lua_State* L, const int minParemeterCount,
                              const wxString& functionName)
        {
        assert(L);
        assert(minParemeterCount >= 0);
        if (lua_gettop(L) < minParemeterCount)
            {
            wxMessageBox(wxString::Format(
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
    int GetScriptFolderPath(lua_State* L)
        {
        if (wxGetApp().GetLuaRunner().GetScriptFilePath().empty())
            {
            DebugPrint(wxString::Format(_(L"%sError%s: call to %s returned empty because "
                                          "the script has not been saved yet."),
                                        L"<span style='color:red; font-weight:bold;'>", L"</span>",
                                        __func__));
            lua_pushstring(L, "");
            return 1;
            }
        lua_pushstring(L,
                       wxFileName(wxGetApp().GetLuaRunner().GetScriptFilePath()).GetPathWithSep());
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
