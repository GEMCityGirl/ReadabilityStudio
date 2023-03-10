/** @addtogroup Scripting
    @brief Classes for the scripting.
    @date 2005-2023
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __LUAINTERFACE_H__
#define __LUAINTERFACE_H__

#include "luna.h"
#include <wx/wx.h>

/// Interface for running Lua code.
/// This manages the Lua session, registering our custom libraries, and keeps track of the
/// running script. Also prevents more than one script from running at a time.
class LuaInterpreter
    {
public:
    LuaInterpreter();
    ~LuaInterpreter();
    LuaInterpreter(const LuaInterpreter&) = delete;
    LuaInterpreter(LuaInterpreter&&) = delete;
    LuaInterpreter& operator=(const LuaInterpreter&) = delete;
    LuaInterpreter& operator=(LuaInterpreter&&) = delete;
    /** Runs a Lua script.
        @param filePath The script's file path. Code will be loaded from this file.*/
    void RunLuaFile(const wxString& filePath);
    /** Runs a block of Lua code.
        @param code The code to run.
        @param filePath The script's file path. This is only used for any calls to GetScriptPath()
                        from the script, it will not affect the code being ran. It can be left empty,
                        that will just cause any calls to GetScriptPath() to return empty as well.*/
    void RunLuaCode(const wxString& code, const wxString& filePath = wxEmptyString);
    /// @returns True if another block of Lua code is already running.
    [[nodiscard]] static bool IsRunning() noexcept
        { return m_isRunning; }
    /// @returns The file path of the currently running script (may be empty if RunLuaCode() was called with no defined file path).
    [[nodiscard]] static const wxString& GetScriptFilePath() noexcept
        { return m_scriptFilePath; }
    /// Sets the path of the currently running script.
    /// @param path The filepath of the currently running script.
    static void SetScriptFilePath(const wxString& path)
        { m_scriptFilePath = path; }
private:
    lua_State* m_L{ nullptr };
    static bool m_isRunning;
    static wxString m_scriptFilePath;
    };

/** @}*/

#endif //__LUAINTERFACE_H__
