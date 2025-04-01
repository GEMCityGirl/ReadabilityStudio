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

#include "filepathresolver.h"
#include "../app/readability_app.h"

wxDECLARE_APP(ReadabilityApp);

//----------------------------------
wxString FilePathResolver::ResolvePath(
    const wxString& path, const bool attemptToConnect,
    std::initializer_list<wxString> pathsToSearch /*= std::initializer_list<wxString>{}*/)
    {
    // will set m_path directly, so we can ignore the return value here
    FilePathResolverBase::ResolvePath(path, pathsToSearch);
    // if failed, see if it's a web file by trying to connect to it
    if (attemptToConnect && path.length() && m_fileType == FilePathType::InvalidFileType)
        {
        // See if it is an URL, but make sure this remotely resembles a legit URL first.
        // More than 3 spaces is really odd for an URL and is more than likely bogus
        if (m_path.Freq(L' ') > 3)
            {
            m_fileType = FilePathType::InvalidFileType;
            return m_path;
            }
        wxString webPathToTest = m_path;

        webPathToTest.Trim(true);
        webPathToTest.Trim(false);

        webPathToTest.Replace(L"\\", L"/");
        // encode any spaces
        webPathToTest.Replace(L" ", L"%20");

        wxGetApp().GetWebHarvester().RequestResponse(L"https://www." + webPathToTest);

        // try to connect to it with https://www. prepended to it
        if (wxGetApp().GetWebHarvester().GetDownloader().GetLastStatus() != 200)
            {
            // if that didn't work, then try prepending just https://
            wxGetApp().GetWebHarvester().RequestResponse(L"https://" + webPathToTest);
            }

        if (wxGetApp().GetWebHarvester().GetDownloader().GetLastStatus() == 200)
            {
            // get redirect URL (if we got redirected)
            m_path = wxGetApp().GetWebHarvester().GetDownloader().GetLastUrl();
            m_fileType = FilePathType::HTTP;
            return m_path;
            }
        else
            {
            m_fileType = FilePathType::InvalidFileType;
            return m_path;
            }
        }

    return m_path;
    }
