/** @addtogroup Scripting
    @brief Classes for the scripting.
    @date 2005-2023
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __LUADEBUG_H__
#define __LUADEBUG_H__

#include "lua.hpp"
#include <wx/wx.h>

namespace LuaScripting
    {
    /** Helper function to verify that the expected number
        of arguments where passed to a function.
        @param L The Lua state.
        @param minParemeterCount The minimum parameter count for the function.
        @param functionName The function's name (to display if an error occurs).
        @returns @c true if enough arguments where passed to the function.*/
    [[nodiscard]]
    bool VerifyParameterCount(lua_State* L, const int minParemeterCount,
                              const wxString& functionName);
    /** @brief Prints a messase to the debug window.
        @param str The message to print.*/
    void DebugPrint(const wxString& str);

    int Print(lua_State*);
    int Clear(lua_State*);
    int GetScriptFolderPath(lua_State*);

    static const luaL_Reg DebugLib[] =
        {
            { "Print", Print },
            { "Clear", Clear },
            { "GetScriptFolderPath", GetScriptFolderPath },
            { nullptr, nullptr }
        };
    }

/** @}*/

#endif //__LUADEBUG_H__
