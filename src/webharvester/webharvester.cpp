///////////////////////////////////////////////////////////////////////////////
// Name:        webharvester.cpp
// Author:      Blake Madden
// Copyright:   (c) 2005-2023 Blake Madden
// Licence:     3-Clause BSD licence
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "webharvester.h"
#include "filepathresolver.h"

const wxString WebHarvester::HTML_CONTENT_TYPE = _DT(L"text/html");
const wxString WebHarvester::JAVASCRIPT_CONTENT_TYPE = _DT(L"application/x-javascript");
const wxString WebHarvester::VBSCRIPT_CONTENT_TYPE = _DT(L"application/x-vbscript");

//----------------------------------
bool wxStringLessWebPath::operator()(const wxString& first, const wxString& second) const
    {
    FilePathResolver resolver;
    wxString firstPath = resolver.ResolvePath(first, true);
    wxString secondPath = resolver.ResolvePath(second, true);
    if (firstPath.length() > 0 && firstPath[firstPath.length()-1] == L'/')
        { firstPath.RemoveLast(); }
    if (secondPath.length() > 0 && secondPath[secondPath.length()-1] == L'/')
        { secondPath.RemoveLast(); }
    return (firstPath.CmpNoCase(secondPath) < 0);
    }

//----------------------------------
void UrlWithNumericSequence::ParseSequenceNumber()
    {
    const auto lastSlash = m_string.find(L'/', true);
    if (lastSlash == wxString::npos)
        {
        // no slash? this is a bad url
        Reset();
        return;
        }
    m_number_start = std::wcscspn(m_string.wc_str() + lastSlash, L"0123456789") + lastSlash;
    // no number was found, so don't bother
    if (m_number_start == m_string.length())
        {
        Reset();
        return;
        }
    wchar_t* end = nullptr;
    m_numeric_value = ::wxStrtol(m_string.wc_str() + m_number_start, &end, 10);
    m_numeric_width = end - (m_string.wc_str() + m_number_start);
    // no number was found (this should not happen at this point)
    if (m_numeric_width == 0)
        {
        Reset();
        return;
        }

    m_number_end = m_number_start+m_numeric_width;
    }

//----------------------------------
void WebHarvester::SearchForMissingFiles()
    {
    // Go through the links and see if there are any numerically
    // sequenced files that weren't listed and add them to the download queue.
    // They may or may not exist, but this is a nice way to download files that
    // may have not been listed on the webpages.
    if (m_harvestedLinks.size() < 2)
        { return; }

    std::set<UrlWithNumericSequence> newFilesToDownload;

    wxString currentPrefix = m_harvestedLinks.begin()->GetPathPrefix();
    size_t numericWidth = m_harvestedLinks.begin()->GetNumericWidth();
    std::set<long> alreadyDownloadedNumbers;
    if (m_harvestedLinks.begin()->GetNumericValue() != wxNOT_FOUND)
        { alreadyDownloadedNumbers.insert(m_harvestedLinks.begin()->GetNumericValue()); }

    std::set<UrlWithNumericSequence>::const_iterator previousPos;
    std::set<UrlWithNumericSequence>::const_iterator pos = m_harvestedLinks.cbegin();
    ++pos; // already have the first one, so skip it
    for (; pos != m_harvestedLinks.end(); ++pos)
        {
        if (currentPrefix.CmpNoCase(pos->GetPathPrefix()) == 0)
            {
            // if still on the same file name then add the number to the list
            if (pos->GetNumericValue() != wxNOT_FOUND)
                { alreadyDownloadedNumbers.insert(pos->GetNumericValue()); }
            continue;
            }
        else
            {
            previousPos = pos; --previousPos;
            // the file sequence has ended, so see what needs downloading
            DownloadSequentialRange(currentPrefix, previousPos->GetPathSuffix(),
                                    numericWidth, previousPos->GetReferUrl(),
                                    alreadyDownloadedNumbers, newFilesToDownload);
            // finally, reset for the next loop around
            currentPrefix = pos->GetPathPrefix();
            numericWidth = pos->GetNumericWidth();
            alreadyDownloadedNumbers.clear();
            }
        }
    // download the last sequence set now
    --pos;
    DownloadSequentialRange(currentPrefix, pos->GetPathSuffix(),
                                    numericWidth, pos->GetReferUrl(),
                                    alreadyDownloadedNumbers, newFilesToDownload);

    m_harvestedLinks.insert(newFilesToDownload.cbegin(), newFilesToDownload.cend());
    }

//----------------------------------
void WebHarvester::DownloadSequentialRange(const wxString& prefix, const wxString& suffix,
                             const size_t numericWidth, const wxString& referUrl,
                             const std::set<long>& alreadyDownloadedNumbers,
                             std::set<UrlWithNumericSequence>& newFilesToDownload)
    {
    wxString newLink;
    // at least two numbers needed to figure out the range
    if (alreadyDownloadedNumbers.size() >= 2)
        {
        long rangeStart = *(alreadyDownloadedNumbers.begin());
        long rangeEnd = *(--alreadyDownloadedNumbers.end());
        for (long i = rangeStart+1; i < rangeEnd-1; ++i)
            {
            // if a number in the sequence that was not found
            // on the page then format it and add to list
            if (alreadyDownloadedNumbers.find(i) == alreadyDownloadedNumbers.end())
                {
                wxString formatString = wxString::Format(L"%%0%uu", numericWidth);
                newLink = prefix;
                newLink += wxString::Format(formatString, i);
                newLink += suffix;
                if (IsDownloadingFilesWhileCrawling() && DownloadFile(newLink).length())
                    { newFilesToDownload.emplace(UrlWithNumericSequence(newLink, referUrl)); }
                }
            }
        // if we are downloading right now then go an extra step and try to
        // download files beyond the listed range
        if (IsDownloadingFilesWhileCrawling())
            {
            long i = 0;
            // first download any files starting at index 0 if the first listed
            // file was less than 10 but not 0
            if (rangeStart < 10 && rangeStart > 0)
                {
                while (i < rangeStart)
                    {
                    wxString formatString = wxString::Format(L"%%0%uu", numericWidth);
                    newLink = prefix;
                    newLink += wxString::Format(formatString, i);
                    newLink += suffix;
                    if (DownloadFile(newLink).length())
                        { newFilesToDownload.emplace(UrlWithNumericSequence(newLink, referUrl)); }
                    ++i;
                    }
                }
            // now try to download any files after the listed range
            i = rangeEnd + 1;
            // 1,000 is an arbitrary sentinel value--sometimes non-existent pages return HTTP 200
            while (i < 1'000)
                {
                wxString formatString = wxString::Format(L"%%0%uu", numericWidth);
                newLink = prefix;
                newLink += wxString::Format(formatString, i);
                newLink += suffix;
                if (DownloadFile(newLink).length())
                    {
                    ++i;
                    newFilesToDownload.emplace(UrlWithNumericSequence(newLink, referUrl));
                    }
                else
                    { break; }
                }
            }
        }
    }

//----------------------------------
wxString WebHarvester::DownloadFile(wxString& Url,
                                    const wxString& fileExtension /*= wxString{}*/)
    {
    if (Url.empty() )
        { return wxString{}; }

    // strip off bookmark (if there is one)
    const auto bookMarkIndex = Url.find(L'#', true);
    if (bookMarkIndex != wxString::npos)
        { Url.Truncate(bookMarkIndex); }
    Url.Trim(true).Trim(false);
    // encode any spaces
    Url.Replace(L" ", L"%20");

    // remove "https" (and the like) from the file path so that when
    // we build a mirrored local folder structure, we don't have a
    // folder named "https."
    wxString urlLocalFileFriendlyName = Url;
    const wxRegEx re(L"^(http|https|ftp|ftps|gopher|file)://", wxRE_ICASE|wxRE_EXTENDED);
    re.ReplaceFirst(&urlLocalFileFriendlyName, wxString{});
    // in case of an url like www.company.com/events/
    if (urlLocalFileFriendlyName.length() > 0 &&
        urlLocalFileFriendlyName[urlLocalFileFriendlyName.length()-1] == L'/')
        {
        // chop off the trailing '/'
        urlLocalFileFriendlyName.RemoveLast();
        // add an extension to the url, unless it is the main page
        if (!html_utilities::html_url_format::is_url_top_level_domain(urlLocalFileFriendlyName))
            { urlLocalFileFriendlyName.Append(L".htm"); }
        }

    // first create the folder to save the file
    wxString downloadPath = m_downloadDirectory;
    if (downloadPath.length() > 0 &&
        downloadPath[downloadPath.length()-1] != wxFileName::GetPathSeparator())
        { downloadPath += wxFileName::GetPathSeparator(); }
    // "mirror" the webpage's path on the local system
    if (m_keepWebPathWhenDownloading)
        {
        html_utilities::html_url_format formatUrl(urlLocalFileFriendlyName);
        wxString webDirPath = formatUrl.get_directory_path().c_str();

        // convert the path to something acceptable for the local system
    #ifdef __WXMSW__
        webDirPath.Replace(L"/", L"\\");
    #endif
        webDirPath = StripIllegalFileCharacters(webDirPath);
        if (webDirPath.length() > 0 &&
            webDirPath[webDirPath.length()-1] != wxFileName::GetPathSeparator())
            { webDirPath += wxFileName::GetPathSeparator(); }
        downloadPath += webDirPath;
        }

    const wxString downloadPathFolder = downloadPath;

    // and see where it will be downloaded locally
    // (make sure file name is legal for the local file system)
    wxString fileName = wxFileName(urlLocalFileFriendlyName).GetFullName();
    if (fileName.empty())
        { return wxString{}; }
    downloadPath = downloadPath + StripIllegalFileCharacters(fileName);
    /* Check the extension on the file we are downloading. It might not have one, might be
       junk (because it is a PHP query), or it might be a domain that wouldn't make sense for
       local file types. If so, append the file extension "hint" onto it
       (or determine it from the MIME type).*/
    const wxString webFileExt = wxFileName(downloadPath).GetExt();
    if (webFileExt.empty() || webFileExt.length() > 4)
        {
        long rCode{ 0 };
        const wxString downloadExt = StripIllegalFileCharacters(
            fileExtension.length() ? fileExtension :
            GetFileTypeFromContentType(GetContentType(Url, rCode)));
        downloadPath += L'.' + downloadExt;
        }
    else if (html_utilities::html_url_format::is_url_top_level_domain(Url))
        { downloadPath += L".html"; }

    if (!m_replaceExistingFiles && wxFileName::FileExists(downloadPath))
        {
        // if the file already exists and we aren't overwriting,
        // then create a different name for it
        downloadPath = CreateNewFileName(downloadPath);
        if (downloadPath.empty())
            { return wxString{}; }
        }

    wxYield();
    if (m_progressDlg)
        {
        wxStringTokenizer tkz(Url, L"\n\r", wxTOKEN_STRTOK);
        const wxString urlLabel = tkz.GetNextToken();
        if (!m_progressDlg->Pulse(m_hideFileNamesWhileDownloading ? _(L"Downloading...") :
            wxString::Format(_(L"Downloading \"%s\""), urlLabel)))
            {
            m_isCancelled = true;
            return wxString{};
            }
        }

    m_downloader.RequestResponse(Url);
    auto responseCode = m_downloader.GetLastStatus();

    // check the response code
    if (IsBadResponseCode(responseCode))
        {
        wxLogWarning(L"%s: Unable to connect to page, error code #%i.", Url, responseCode);
        downloadPath.clear();
        }
    // otherwise, if file is OK then download it
    else
        {
        // create the target folder
        if (!wxFileName::DirExists(downloadPathFolder) )
            { wxFileName::Mkdir(downloadPathFolder, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL); }

        // now download the file locally
        if (m_downloader.Download(Url, downloadPath))
            { m_downloadedFiles.insert(downloadPath); }
        else
            {
            wxLogWarning(L"Unable to write to %s", downloadPath);
            downloadPath.clear();
            }
        }

    return downloadPath;
    }

//----------------------------------
wxString WebHarvester::GetFileTypeFromContentType(const wxString& contentType)
    {
    wxString fileExt;
    auto slash = contentType.find(L'/');
    if (slash != wxString::npos)
        {
        fileExt = contentType.substr(++slash);
        // in case there is charset info at the end, then chop that off
        const auto semiColon = fileExt.find(L';');
        if (semiColon != wxString::npos)
            { fileExt = fileExt.substr(0, semiColon); }
        }
    return fileExt;
    }

//----------------------------------
wxString WebHarvester::GetContentType(wxString& Url, long& responseCode)
    {
    responseCode = 404;

    if (Url.empty() )
        { return wxString{}; }
    // encode any spaces
    Url.Trim(true).Trim(false);
    Url.Replace(L" ", L"%20");

    m_downloader.RequestResponse(Url);

    responseCode = m_downloader.GetLastStatus();

    return m_downloader.GetLastContentType();
    }

//---------------------------------------------------
wxString WebHarvester::CreateNewFileName(const wxString& filePath)
    {
    wxString dir, name, ext;
    // False positive with cppcheck when using --library=wxwidgets,
    // as this version of SplitPath returns void.
    // cppcheck-suppress ignoredReturnValue
    wxFileName::SplitPath(filePath, &dir, &name, &ext);
    wxString newFilePath;
    for (size_t i = 0; i < 1'000; ++i)
        {
        newFilePath = wxString::Format(L"%s%c%s%04u.%s",
                                       dir, wxFileName::GetPathSeparator(),
                                       name, i, ext);
        if (!wxFileName::FileExists(newFilePath))
            {
            // create the file as we will use it later
            wxFile file(newFilePath, wxFile::write);
            if (!file.IsOpened())
                { continue; }
            return newFilePath;
            }
        }

    return wxString{};
    }

//----------------------------------
wxString WebHarvester::GetResponseMessage(const int responseCode)
    {
    if (responseCode < 300)
        { return _(L"Connection successful."); }
    switch (responseCode)
        {
    case 301:
        return _(L"Page has moved.");
        break;
    case 302:
        return _(L"Page was found, but under a different URL.");
        break;
    case 204:
        return _(L"Page not responding.");
        break;
    case 400:
        return _(L"Bad request.");
        break;
    case 401:
        return _(L"Unauthorized.");
        break;
    case 402:
        return _(L"Payment Required.");
        break;
    case 403:
        return _(L"Forbidden.");
        break;
    case 404:
        return _(L"Page not found.");
        break;
    case 500:
        return _(L"Internal Error.");
        break;
    case 501:
        return _(L"Not implemented.");
        break;
    case 502:
        return _(L"Service temporarily overloaded.");
        break;
    case 503:
        return _(L"Gateway timeout.");
        break;
    default:
        return _(L"Unable to connect to the Internet.");
        };
    }

//----------------------------------
bool WebHarvester::ReadWebPage(wxString& Url,
                               wxString& webPageContent,
                               wxString& contentType,
                               wxString& statusText,
                               long& responseCode,
                               const bool acceptOnlyHtmlOrScriptFiles /*= true*/)
    {
    webPageContent.clear();
    contentType.clear();
    statusText.clear();
    responseCode = 404;

    if (Url.empty() )
        { return false; }

    // strip off bookmark (if there is one)
    const auto bookMarkIndex = Url.find(L'#', true);
    if (bookMarkIndex != wxString::npos)
        { Url.Truncate(bookMarkIndex); }
    Url.Trim(true).Trim(false);
    // encode any spaces
    Url.Replace(L" ", L"%20");

    if (!m_downloader.Read(Url))
        {
        responseCode = m_downloader.GetLastStatus();
        statusText = m_downloader.GetLastStatusText();
        wxLogWarning(L"%s: Unable to connect to page, error code #%i.", Url, responseCode);
        return false;
        }

    responseCode = m_downloader.GetLastStatus();
    statusText = m_downloader.GetLastStatusText();
    if (IsBadResponseCode(responseCode))
        {
        wxLogWarning(L"%s: Unable to connect to page, error code #%i.", Url, responseCode);
        return false;
        }
    else if (m_downloader.GetLastRead().size())
        {
        contentType = m_downloader.GetLastContentType();
        if (contentType.empty())
            { contentType = L"text/html; charset=utf-8"; }

        // first make sure it is really a webpage
        if (acceptOnlyHtmlOrScriptFiles &&
            contentType.length() &&
            string_util::strnicmp(contentType.wc_str(), HTML_CONTENT_TYPE.wc_str(),
                                  HTML_CONTENT_TYPE.length()) != 0 &&
            string_util::strnicmp(contentType.wc_str(), JAVASCRIPT_CONTENT_TYPE.wc_str(),
                                  JAVASCRIPT_CONTENT_TYPE.length()) != 0 &&
            string_util::strnicmp(contentType.wc_str(), VBSCRIPT_CONTENT_TYPE.wc_str(),
                                  VBSCRIPT_CONTENT_TYPE.length()) != 0)
            { return false; }

        // get redirect URL (if we got redirected)
        Url = m_downloader.GetLastUrl();
        /* Convert from the file's charset to the application's charset.
           Try to get it from the response header first because that is more
           accurate when the file is really UTF8 but the designer put something like
           8859-1 in the meta section. If that fails, then read the meta section.*/
        wxString charSet = GetCharsetFromContentType(contentType);
        if (charSet.empty())
            {
            charSet = GetCharsetFromPageContent(
                &m_downloader.GetLastRead()[0], m_downloader.GetLastRead().size());
            }
        // Watch out for embedded NULLs in stream (happens with poorly formatted HTML).
        // In this situation, we need to split the stream into valid chunks, convert them,
        // and then piece them back together.
        if (string_util::strnlen(&m_downloader.GetLastRead()[0], m_downloader.GetLastRead().size()) <
            m_downloader.GetLastRead().size())
            {
            wxLogWarning(L"Embedded null terminator(s) encountered in page.");
            webPageContent =
                Wisteria::TextStream::CharStreamWithEmbeddedNullsToUnicode(
                    &m_downloader.GetLastRead()[0], m_downloader.GetLastRead().size(), charSet);
            }
        else
            {
            webPageContent =
                Wisteria::TextStream::CharStreamToUnicode(
                    &m_downloader.GetLastRead()[0], m_downloader.GetLastRead().size(), charSet);
            }
        }

    return true;
    }

//----------------------------------
bool WebHarvester::IsPageHtml(wxString& Url, wxString& contentType)
    {
    contentType.clear();
    if (Url.empty() )
        { return false; }

    long responseCode{ 0 };
    contentType = GetContentType(Url, responseCode);
    if (contentType.empty())
        { return false; }
    return string_util::strnicmp(contentType.wc_str(), HTML_CONTENT_TYPE.wc_str(),
                                 HTML_CONTENT_TYPE.length()) == 0;
    }

//----------------------------------
void WebHarvester::DownloadFiles()
    {
    m_downloadedFiles.clear();
    // go through the harvested links and download the files
    // matching the ones that we want to download
    for (const auto& link : m_harvestedLinks)
        {
        if (m_isCancelled)
            { break; }
        wxString currentPage = link.GetPath();
        DownloadFile(currentPage);
        }
    }

//----------------------------------
bool WebHarvester::CrawlLinks()
    {
    m_hideFileNamesWhileDownloading = wxGetMouseState().ShiftDown();
    wxStringTokenizer tkz(m_url, L"\n\r", wxTOKEN_STRTOK);
    const wxString urlLabel = m_hideFileNamesWhileDownloading ?
        wxString{ L"..." } : L" \"" + tkz.GetNextToken() + L"\"";
    m_progressDlg = new wxProgressDialog(_(L"Web Harvester"),
        wxString::Format(_(L"Harvesting %s"), urlLabel), 5, nullptr,
        wxPD_SMOOTH|wxPD_ELAPSED_TIME|wxPD_CAN_ABORT);
    m_progressDlg->Centre();
    m_progressDlg->Raise();

    m_isCancelled = false;
    m_currentLevel = 0;
    m_downloadedFiles.clear();
    m_alreadyCrawledFiles.clear();
    m_brokenLinks.clear();
    CrawlLinks(m_url, html_utilities::hyperlink_parse::hyperlink_parse_method::html);

    // Now check the original URL to see if it is a file that should be downloaded
    long rCode{ 0 };
    const wxString fnExt = wxFileName(m_url).GetExt();
    const wxString fileExt = fnExt.length() ? fnExt :
        GetFileTypeFromContentType(GetContentType(m_url, rCode));
    wxString contentType;
    // Add the link to files to download if it matches our criteria
    if (!HasUrlAlreadyBeenHarvested(m_url) &&
        ((m_harvestAllHtml && IsPageHtml(m_url, contentType)) ||
        ShouldFileBeHarvested(fileExt)) )
        { HarvestLink(m_url, m_url, fileExt); }
    m_progressDlg->Pulse();

    // Search for excluded links now
    if (m_searchForMissingSequentialFiles)
        { SearchForMissingFiles(); }
    m_progressDlg->Destroy(); m_progressDlg = nullptr;

    return !m_isCancelled;
    }

//----------------------------------
bool WebHarvester::CrawlLinks(wxString& url,
    const html_utilities::hyperlink_parse::hyperlink_parse_method method)
    {
    if (m_isCancelled || url.empty() || HasUrlAlreadyBeenCrawled(url))
        { return false; }

    ++m_currentLevel;
    if (m_currentLevel > GetDepthLevel())
        {
        --m_currentLevel;
        return false;
        }

    wxString fileText;
    FilePathResolver resolve(url, true);
    if (resolve.IsHTTPFile() || resolve.IsHTTPSFile())
        {
        // read in the page
        wxString contentType;
        wxString statusText;
        long responseCode{ 0 };
        if (!ReadWebPage(url, fileText, contentType, statusText, responseCode, true) )
            {
            --m_currentLevel;
            return false;
            }
        }
    else if (resolve.IsLocalOrNetworkFile())
        { Wisteria::TextStream::ReadFile(url, fileText); }
    // if raw HTML text was passed in, then parse that instead
    else if (string_util::stristr(url.wc_str(), L"<html") )
        { fileText = url; }
    else
        {
        --m_currentLevel;
        return false;
        }

    // Just in case the url was redirected, make sure it isn't one that we have already crawled
    // or isn't a different domain than what we are allowing.
    if (!VerifyUrlDomainCriteria(url))
        {
        // prevent crawling it later if it doesn't meet our criteria
        m_alreadyCrawledFiles.insert(url);
        --m_currentLevel;
        return false;
        }
    if (HasUrlAlreadyBeenCrawled(url) )
        {
        --m_currentLevel;
        return false;
        }

    // it's ready to be crawled now, so marked it as crawled for later
    m_alreadyCrawledFiles.insert(url);

    wxYield();
    if (m_progressDlg)
        {
        wxStringTokenizer tkz(url, L"\n\r", wxTOKEN_STRTOK);
        const wxString urlLabel = tkz.GetNextToken();
        if (!m_progressDlg->Pulse(m_hideFileNamesWhileDownloading ? _(L"Harvesting...") :
            wxString::Format(_(L"Harvesting \"%s\""), urlLabel)))
            {
            m_isCancelled = true;
            --m_currentLevel;
            return false;
            }
        }

    html_utilities::hyperlink_parse getHyperLinks(fileText.wc_str(), fileText.length(), method);
    if (getHyperLinks.get_parse_method() ==
            html_utilities::hyperlink_parse::hyperlink_parse_method::html &&
        getHyperLinks.get_html_parser().get_base_url())
        {
        url.assign(getHyperLinks.get_html_parser().get_base_url(),
                   getHyperLinks.get_html_parser().get_base_url_length());
        }

    // NOTE: if a 302 or 300 are encountered, then the url may be different now,
    // as well as a base url in the head
    html_utilities::html_url_format formatUrl(url);
    while (true)
        {
        // gather its hyperlinks
        const auto* currentLink = getHyperLinks();
        if (currentLink)
            { CrawlLink(wxString(currentLink,getHyperLinks.get_current_hyperlink_length()),
                        formatUrl, url,
                        // if an image (can only determine this if using the HTML parser)
                        (getHyperLinks.get_parse_method() ==
                            html_utilities::hyperlink_parse::hyperlink_parse_method::html) ?
                            getHyperLinks.get_html_parser().is_current_link_an_image() : false); }
        else
            { break; }
        if (m_isCancelled)
            {
            --m_currentLevel;
            return false;
            }
        }
    --m_currentLevel;

    return true;
    }

//----------------------------------
void WebHarvester::CrawlLink(const wxString& currentLink,
                             // cppcheck-suppress constParameter
                             html_utilities::html_url_format& formatUrl,
                             const wxString& mainUrl, const bool isImage)
    {
    if (m_isCancelled)
        { return; }

    const wxString urlLabel = currentLink;
    if (!m_progressDlg->Pulse(m_hideFileNamesWhileDownloading ? _(L"Crawling...") :
        wxString::Format(_(L"Crawling \"%s\""), urlLabel)))
        {
        m_isCancelled = true;
        --m_currentLevel;
        return;
        }

    // skip "mailto" anchors
    if (currentLink.length() >= 7 &&
        string_util::strnicmp(currentLink.wc_str(), _DT(L"mailto:"), 7) == 0)
        { return; }
    // and telephone numbers
    if (currentLink.length() >= 4 &&
        string_util::strnicmp(currentLink.wc_str(), _DT(L"tel:"), 4) == 0)
        { return; }
    // ...or any bookmarks on the same page
    else if (currentLink.length() >= 1 &&
        currentLink[0] == L'#')
        { return; }

    wxString fullUrl;
    fullUrl.assign(formatUrl(currentLink, currentLink.length(), isImage));
    if (fullUrl.empty())
        { return; }
    if ((m_currentLevel > GetDepthLevel() || HasUrlAlreadyBeenCrawled(fullUrl)) &&
        HasUrlAlreadyBeenHarvested(fullUrl))
        { return; }
    // this should usually be turned off for performance,
    // but it's a nice way to get a list of broken links from a site
    if (IsSearchingForBrokenLinks())
        {
        long responseCode{ 0 };
        GetContentType(fullUrl, responseCode);
        if (responseCode == 404)
            { m_brokenLinks.emplace(std::make_pair(fullUrl, mainUrl)); }
        }

    // first make sure that if we are domain restricted,
    // then don't bother with it if it's from another domain
    if (!VerifyUrlDomainCriteria(fullUrl))
        { return; }

    wxString fileExt = wxFileName(fullUrl).GetExt();
    // try to determine the file type if there is no extension
    if (fileExt.empty())
        {
        // any sort of PHP page (even without the extension PHP) will follow this syntax
        if (std::wcschr(fullUrl, L'?') && std::wcschr(fullUrl, L'='))
            { fileExt = L"php"; }
        else
            {
            long rCode{ 0 };
            fileExt = GetFileTypeFromContentType(GetContentType(fullUrl, rCode));
            }
        }

    /* See if the page is HTML so that we know whether to crawl it or not. Sometimes
       the file extension on a page is different from its actual content, so if we are
       verifying the content, then also connect to it and read its mime type. Otherwise,
       go off of its extension and if that doesn't work then read its mime type.*/
    wxString contentType;
    bool pageIsHtml = false;

    if (IsKnownRegularFileExtension(fileExt) || IsKnownScriptFileExtension(fileExt))
        { pageIsHtml = false; }
    else if (IsWebPageExtension(fileExt))
        { pageIsHtml = true; }
    else if (html_utilities::html_url_format::is_url_top_level_domain(fullUrl))
        { pageIsHtml = true; }
    else
        { pageIsHtml = IsPageHtml(fullUrl, contentType); }

    html_utilities::html_url_format formatCurrentUrl(fullUrl);

    // First, crawl the page (if applicable)
    ///////////////////////////////////////
    // Javascript/VBScript files are crawled differently, so check that first
    if (IsKnownScriptFileExtension(fileExt))
        {
        CrawlLinks(fullUrl, html_utilities::hyperlink_parse::hyperlink_parse_method::script);
        if (ShouldFileBeHarvested(fileExt))
            { HarvestLink(fullUrl, mainUrl, fileExt); }
        return;
        }
    else if (IsKnownRegularFileExtension(fileExt))
        {
        // sometimes pages with a certain file extension are really HTML pages, so crawl if necessary
        if (pageIsHtml)
            { CrawlLinks(fullUrl, html_utilities::hyperlink_parse::hyperlink_parse_method::html); }
        // add the link to files to download later if it matches our criteria            
        if ((m_harvestAllHtml && pageIsHtml) ||
            ShouldFileBeHarvested(fileExt) )
            { HarvestLink(fullUrl, mainUrl, fileExt); }
        }
    else
        {
        /* If not a known "regular" file extension (e.g., PDF) or a webpage extension (e.g., HTML), then
           figure out its type. If a webpage, then crawl it or see if it is a type of file that we want
           to download.*/
        if (IsWebPageExtension(fileExt) ||
            html_utilities::html_url_format::is_url_top_level_domain(fullUrl))
            {
            CrawlLinks(fullUrl, html_utilities::hyperlink_parse::hyperlink_parse_method::html);
            if (m_harvestAllHtml || ShouldFileBeHarvested(fileExt))
                { HarvestLink(fullUrl, mainUrl, fileExt); }
            return;
            }
        else
            {
            // if it's a PHP query, then see if it is really a page needing to be crawled
            if (formatCurrentUrl.has_query() )
                {
                if (pageIsHtml)
                    { CrawlLinks(fullUrl, html_utilities::hyperlink_parse::hyperlink_parse_method::html); }
                // need to get this page's type and download it if it meets the criteria
                fileExt = GetFileTypeFromContentType(contentType);
                if (ShouldFileBeHarvested(fileExt) || (pageIsHtml && m_harvestAllHtml))
                    {
                    // need to override its extension too because the url has a different
                    // file extension on it due to it being a PHP query
                    HarvestLink(fullUrl, mainUrl, fileExt);
                    }
                return;
                }
            // otherwise, an HTML page with an unknown extension
            else if (pageIsHtml)
                {
                CrawlLinks(fullUrl, html_utilities::hyperlink_parse::hyperlink_parse_method::html);
                if (m_harvestAllHtml || ShouldFileBeHarvested(fileExt))
                    { HarvestLink(fullUrl, mainUrl, fileExt); }
                return;
                }
            // ...finally, not a webpage and an unknown extension.
            // Just figure out its real type and see if we should download it
            else if (contentType.length())
                {
                fileExt = GetFileTypeFromContentType(contentType);
                if (ShouldFileBeHarvested(fileExt))
                    { HarvestLink(fullUrl, mainUrl, fileExt); }
                return;
                }
            }
        }
    }

//----------------------------------
bool WebHarvester::VerifyUrlDomainCriteria(const wxString& url)
    {
    // the root url should always pass this test, even if its own domain is not
    // in the list of allowable domains
    if (url.CmpNoCase(GetUrl()) == 0)
        { return true; }
    // this URL should already be fully expanded, so just initialize the formatter with it
    // as the root URL and then set it as the main URL.
    html_utilities::html_url_format formatUrl(url);
    formatUrl(url, url.length());
    if (m_domainRestriction == DomainRestriction::RestrictToDomain)
        {
        if (m_domain != formatUrl.get_domain().c_str())
            { return false; }
        }
    else if (m_domainRestriction == DomainRestriction::RestrictToSubDomain)
        {
        if (m_fullDomain != formatUrl.get_full_domain().c_str())
            { return false; }
        }
    else if (m_domainRestriction == DomainRestriction::RestrictToExternalLinks)
        {
        if (m_domain == formatUrl.get_domain().c_str() ||
            m_fullDomain == formatUrl.get_full_domain().c_str() )
            { return false; }
        }
    else if (m_domainRestriction == DomainRestriction::RestrictToSpecificDomains)
        {
        if (m_allowableWebFolders.find(formatUrl.get_directory_path().c_str()) ==
            m_allowableWebFolders.end())
            { return false; }
        }
    else if (m_domainRestriction == DomainRestriction::RestrictToFolder)
        {
        if (m_fullDomainFolderPath != formatUrl.get_directory_path().c_str())
            { return false; }
        }
    return true;
    }

//----------------------------------
bool WebHarvester::HarvestLink(wxString& url, const wxString& referringUrl,
                               const wxString& fileExtension /*= wxString{}*/)
    {
    if (m_isCancelled || url.empty() || HasUrlAlreadyBeenHarvested(url) ||
        !VerifyUrlDomainCriteria(url))
        { return false; }

    // Verify that the file is HTTP/HTTPS.
    FilePathResolver resolve(url, false);
    if (!resolve.IsHTTPFile() && !resolve.IsHTTPSFile())
        { return false; }
    m_harvestedLinks.emplace(UrlWithNumericSequence(url, referringUrl));
    if (IsDownloadingFilesWhileCrawling())
        { DownloadFile(url, fileExtension); }
    return true;
    }

//----------------------------------
wxString WebHarvester::GetCharsetFromContentType(const wxString& contentType)
    {
    const auto index = contentType.Lower().find(L"charset=");
    const auto semicolon = contentType.find(L";");
    if (index != wxString::npos)
        {
        wxString charSet = contentType.substr(index+8);
        charSet.Replace(L"\"", L"");
        charSet.Replace(L"\'", L"");
        charSet.Trim(false); charSet.Trim(true);
        return charSet;
        }
    else if (semicolon != wxString::npos)
        {
        wxString charSet = contentType.substr(semicolon+1);
        charSet.Replace(L"\"", L"");
        charSet.Replace(L"\'", L"");
        charSet.Trim(false); charSet.Trim(true);
        return charSet;
        }
    else
        { return wxLocale::GetSystemEncodingName(); }
    }

//----------------------------------
wxString WebHarvester::GetCharsetFromPageContent(const char* pageContent, const size_t length)
    {
    std::string charSet = lily_of_the_valley::html_extract_text::parse_charset(pageContent, length);
    if (charSet.empty())
        { return wxLocale::GetSystemEncodingName(); }
    else
        {
        // The HTML 5 specification requires that documents advertised as
        // ISO-8859-1 actually be parsed with the Windows-1252 encoding.
        if (string_util::stricmp(charSet.c_str(), "iso-8859-1") == 0)
            { charSet = "windows-1252"; }
        return wxString(charSet.c_str(), wxConvLibc);
        }
    }
