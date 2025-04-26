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

#ifndef LUADEBUG_H
#define LUADEBUG_H

#include "lua.hpp"
#include <wx/wx.h>

// NOLINTBEGIN(readability-identifier-length)
// NOLINTBEGIN(readability-implicit-bool-conversion)

namespace LuaScripting
    {
    /** @brief Helper function to verify that the expected number
            of arguments where passed to a function.
        @param L The Lua state.
        @param minParemeterCount The minimum parameter count for the function.
        @param functionName The function's name (to display if an error occurs).
        @returns @c true if enough arguments where passed to the function.*/
    [[nodiscard]]
    bool VerifyParameterCount(lua_State* L, const int minParemeterCount,
                              const wxString& functionName);
    /** @brief Prints a message to the debug window.
        @param str The message to print.*/
    void DebugPrint(const wxString& str);

    /// @brief Helper function to load font attributes for a project.
    void LoadFontAttributes(lua_State* L, wxFont& font, wxColour& fontColor, bool calledFromObject);
    wxColour LoadColor(wxString colorStr);

    // quneiform-suppress-begin
    // clang-format off
    // NOTE: these must all be single-line for the build script to properly create new topics from these.
    int Print(lua_State* /*string message*/); // Prints a message to the script editor's debug window.
    int Clear(lua_State*); // Clears the log window.
    int /*string*/ GetScriptFolder(lua_State*); // Returns the folder path of the currently running script.
    // clang-format on
    // quneiform-suppress-end

    static const luaL_Reg DebugLib[] = { { "Print", Print },
                                         { "Clear", Clear },
                                         { "GetScriptFolder", GetScriptFolder },
                                         { nullptr, nullptr } };
    } // namespace LuaScripting

// NOLINTEND(readability-implicit-bool-conversion)
// NOLINTEND(readability-identifier-length)

#endif // LUADEBUG_H
