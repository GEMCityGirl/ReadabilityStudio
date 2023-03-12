///////////////////////////////////////////////////////////////////////////////
// Name:        lua_interface.cpp
// Author:      Blake Madden
// Copyright:   (c) 2005-2023 Blake Madden
// Licence:     3-Clause BSD licence
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "lua_interface.h"
#include "lua_application.h"
#include "lua_debug.h"
#include "lua_screenshot.h"
#include "lua_projects.h"
#include "../app/readability_app.h"

wxDECLARE_APP(ReadabilityApp);

bool LuaInterpreter::m_isRunning = false;
wxString LuaInterpreter::m_scriptFilePath;

//------------------------------------------------------
LuaInterpreter::LuaInterpreter()
    {
    m_L = luaL_newstate();
    luaL_openlibs(m_L);
    luaL_newlib(m_L, LuaScripting::ScreenshotLib);
    lua_setglobal(m_L, "ScreenshotLib");
    luaL_newlib(m_L, LuaScripting::ApplicationLib);
    lua_setglobal(m_L, "Application");
    luaL_newlib(m_L, LuaScripting::DebugLib);
    lua_setglobal(m_L, "Debug");
    Luna<LuaScripting::StandardProject>::Register(m_L);
    Luna<LuaScripting::BatchProject>::Register(m_L);
    }

//------------------------------------------------------
LuaInterpreter::~LuaInterpreter()
    {
    lua_gc(m_L, LUA_GCCOLLECT, 0);
    lua_close(m_L);
    }

//------------------------------------------------------
void LuaInterpreter::RunLuaFile(const wxString& filePath)
    {
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureProfessionalCode()))
        {
        wxMessageBox(_("Lua scripting is only available with Professional edition."),
                     _("Licensing Error"), wxOK|wxICON_EXCLAMATION);
        return;
        }
    else if (IsRunning())
        {
        wxMessageBox(_("Another Lua script is already running. "
                       "Please wait for the other script to finish."),
                     _("Lua Script"), wxOK|wxICON_INFORMATION);
        return;
        }
    m_isRunning = true;
    SetScriptFilePath(filePath);

    const wxDateTime startTime(wxDateTime::Now());
    if (luaL_dofile(m_L, filePath.utf8_str()) != 0)
        {
        // error message from Lua has cryptic section in front of it showing the first line of the
        // script and also just shows the line number without saying "line" in front of it,
        // so reformat this message to make it more readable.
        wxString errorMessage(luaL_checkstring(m_L, -1), wxConvUTF8);
        const auto EndOfErrorHeader = errorMessage.find(wxT("]:"));
        if (EndOfErrorHeader != wxNOT_FOUND)
            { errorMessage.erase(0,EndOfErrorHeader+2); }
        wxMessageBox(_("Line ") + errorMessage,
                     _("Script Error"), wxOK|wxICON_EXCLAMATION);
        LuaScripting::DebugPrint(
            _("<span style='color:red; font-weight:bold;'>Error</span>: Line ") + errorMessage);
        }
    const wxDateTime endTime(wxDateTime::Now());
    LuaScripting::DebugPrint(wxString::Format(
        _("Script ran for %s"), endTime.Subtract(startTime).Format()));

    m_isRunning = false;
    }

//------------------------------------------------------
void LuaInterpreter::RunLuaCode(const wxString& code, const wxString& filePath)
    {
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureProfessionalCode()))
        {
        wxMessageBox(_("Lua scripting is only available with Professional edition."),
                     _("Licensing Error"), wxOK|wxICON_EXCLAMATION);
        return;
        }
    else if (IsRunning())
        {
        wxMessageBox(_("Another Lua script is already running. "
                       "Please wait for the other script to finish."),
                     _("Lua Script"), wxOK|wxICON_INFORMATION);
        return;
        }
    m_isRunning = true;
    SetScriptFilePath(filePath);

    const wxDateTime startTime(wxDateTime::Now());
    if (luaL_dostring(m_L, code.utf8_str()) != 0)
        {
        // error message from Lua has cryptic section in front of it showing the first line of the
        // script and also just shows the line number without saying "line" in front of it,
        // so reformat this message to make it more readable.
        wxString errorMessage(luaL_checkstring(m_L, -1), wxConvUTF8);
        const auto EndOfErrorHeader = errorMessage.find(wxT("]:"));
        if (EndOfErrorHeader != wxNOT_FOUND)
            { errorMessage.erase(0,EndOfErrorHeader+2); }
        wxMessageBox(_("Line ") + errorMessage,
                     _("Script Error"), wxOK|wxICON_EXCLAMATION);
        LuaScripting::DebugPrint(
            _("<span style='color:red; font-weight:bold;'>Error</span>: Line ") + errorMessage);
        }
    const wxDateTime endTime(wxDateTime::Now());
    LuaScripting::DebugPrint(
        wxString::Format(_("Script ran for %s"), endTime.Subtract(startTime).Format()));

    m_isRunning = false;
    }
