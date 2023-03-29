///////////////////////////////////////////////////////////////////////////////
// Name:        filepathresolver.cpp
// Author:      Blake Madden
// Copyright:   (c) 2005-2023 Blake Madden
// Licence:     3-Clause BSD licence
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "filepathresolver.h"

//----------------------------------
wxString FilePathResolver::ResolvePath(const wxString& path, const bool attemptToConnect)
    {
    FilePathResolverBase::ResolvePath(path);
    // if failed, see if it's a web file by trying to connect to it
    if (attemptToConnect && m_fileType == FilePathType::InvalidFileType)
        {
        // See if it is an URL, but make sure this remotely resembles a legit URL first.
        // More than 3 spaces is really odd for an URL and is more than likely bogus
        if (m_path.Freq(L' ') > 3)
            {
            m_fileType = FilePathType::InvalidFileType;
            return m_path;
            }
        wxString webPathToTest = m_path;

        webPathToTest.Replace(L"\\", L"/");
        // encode any spaces
        webPathToTest.Replace(L" ", L"%20");

        struct WebHarvester::MemoryStruct chunk;
        CURL* curl_handle = curl_easy_init();

        curl_easy_setopt(curl_handle, CURLOPT_URL,
            static_cast<const char*>(wxString(L"https://www." + webPathToTest)));
        curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, 30);
        curl_easy_setopt(curl_handle, CURLOPT_AUTOREFERER, 1);
        curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, true);
        curl_easy_setopt(curl_handle, CURLOPT_PROXYAUTH, CURLAUTH_ANYSAFE);
        curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, static_cast<const char*>(WebHarvester::GetUserAgent()));
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WebHarvester::WriteMemoryCallback);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, reinterpret_cast<void*>(&chunk));

        // try to connect to it with https://www. prepended to it
        auto connectResult = curl_easy_perform(curl_handle);
        if (connectResult == CURLE_OK)
            { webPathToTest = wxString(L"https://www." + webPathToTest); }
        // if that didn't work, then try prepending just https://
        else
            {
            curl_easy_setopt(curl_handle, CURLOPT_URL,
                static_cast<const char*>(wxString(L"https://" + webPathToTest)));
            connectResult = curl_easy_perform(curl_handle);
            if (connectResult == CURLE_OK)
                { webPathToTest = wxString(L"https://" + webPathToTest); }
            }
        if (connectResult == CURLE_OK)
            {
            // get redirect URL (if we got redirected)
            char* url = nullptr;
            curl_easy_getinfo(curl_handle, CURLINFO_EFFECTIVE_URL, &url);
            if (url)
                { webPathToTest = wxString(url); }
            m_path = webPathToTest;
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




