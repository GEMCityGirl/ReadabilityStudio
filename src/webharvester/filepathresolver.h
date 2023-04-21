/** @addtogroup Web Harvesting
    @brief Classes for the web scraping and downloading.
    @date 2005-2023
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __FILEPATH_RESOLVER__
#define __FILEPATH_RESOLVER__

#include <wx/wx.h>
#include <wx/file.h>
#include <wx/string.h>
#include <wx/filename.h>
#include <wx/regex.h>
#include <wx/filefn.h>
#include <wx/dir.h>
#include <wx/progdlg.h>
#include <wx/arrstr.h>
#include <wx/tokenzr.h>
#include <memory>
#include <string>
#include "webharvester.h"
#include "../Wisteria-Dataviz/src/util/fileutil.h"

/** @brief Class to determine which sort of filepath a string may resemble.
    @details This is useful for determining if a string is a file to a file or URL,
        and determining specifically which sort of path it is.*/
class FilePathResolver final : public FilePathResolverBase
    {
public:
    /// @brief Default constructor.
    FilePathResolver() = default;
    /** @brief Resolves a string to see if it is a file path.
        @param path The string to resolve to a file path.
        @param attemptToConnect Whether to attempt to connect the file (via the Internet)
            to see if it's an URL. This is final fallback when the filepath type
            can't be easily determined.
        @note Set @c attemptToConnect to @c false if performance is a concern.*/
    explicit FilePathResolver(const wxString& path, const bool attemptToConnect)
        { ResolvePath(path, attemptToConnect); }
    /** Resolves a string to see if it is a file path.
        @param path The string to resolve to a file path.
        @param attemptToConnect Whether to attempt to connect the file (via the Internet)
            to see if it's an URL. This is final fallback when the filepath type
            can't be easily determined.
        @note Set @c attemptToConnect to @c false if performance is a concern.*/
    wxString ResolvePath(const wxString& path, const bool attemptToConnect );
    };

/** @}*/

#endif // __FILEPATH_RESOLVER__
