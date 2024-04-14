///////////////////////////////////////////////////////////////////////////////
// Name:        webharvester.cpp
// Author:      Blake Madden
// Copyright:   (c) 2005-2023 Blake Madden
// Licence:     3-Clause BSD licence
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "webharvester.h"
#include "filepathresolver.h"

//----------------------------------
bool wxStringLessWebPath::operator()(const wxString& first, const wxString& second) const
    {
    FilePathResolver resolver;
    wxString firstPath = resolver.ResolvePath(first, true);
    wxString secondPath = resolver.ResolvePath(second, true);
    if (firstPath.length() > 0 && firstPath[firstPath.length() - 1] == L'/')
        {
        firstPath.RemoveLast();
        }
    if (secondPath.length() > 0 && secondPath[secondPath.length() - 1] == L'/')
        {
        secondPath.RemoveLast();
        }
    return (firstPath.CmpNoCase(secondPath) < 0);
    }

//----------------------------------
wxString WebHarvester::DownloadFile(wxString& Url, const wxString& fileExtension /*= wxString{}*/)
    {
    if (Url.empty())
        {
        return wxString{};
        }

    // strip off bookmark (if there is one)
    const auto bookMarkIndex = Url.find(L'#', true);
    if (bookMarkIndex != wxString::npos)
        {
        Url.Truncate(bookMarkIndex);
        }
    Url.Trim(true).Trim(false);
    // encode any spaces
    Url.Replace(L" ", L"%20");

    // remove "https" (and the like) from the file path so that when
    // we build a mirrored local folder structure, we don't have a
    // folder named "https."
    wxString urlLocalFileFriendlyName = Url;
    const wxRegEx re(L"^(http|https|ftp|ftps|gopher|file)://", wxRE_ICASE | wxRE_EXTENDED);
    re.ReplaceFirst(&urlLocalFileFriendlyName, wxString{});
    // in case of an url like www.company.com/events/
    if (urlLocalFileFriendlyName.length() > 0 &&
        urlLocalFileFriendlyName[urlLocalFileFriendlyName.length() - 1] == L'/')
        {
        // chop off the trailing '/'
        urlLocalFileFriendlyName.RemoveLast();
        // add an extension to the url, unless it is the main page
        if (!html_utilities::html_url_format::is_url_top_level_domain(
                urlLocalFileFriendlyName.wc_str()))
            {
            urlLocalFileFriendlyName.Append(L".htm");
            }
        }

    // first create the folder to save the file
    wxString downloadPath = m_downloadDirectory;
    if (downloadPath.length() > 0 &&
        downloadPath[downloadPath.length() - 1] != wxFileName::GetPathSeparator())
        {
        downloadPath += wxFileName::GetPathSeparator();
        }
    // "mirror" the webpage's path on the local system
    if (m_keepWebPathWhenDownloading)
        {
        html_utilities::html_url_format formatUrl(urlLocalFileFriendlyName.wc_str());
        wxString webDirPath = formatUrl.get_directory_path().c_str();

        // convert the path to something acceptable for the local system
#ifdef __WXMSW__
        webDirPath.Replace(L"/", L"\\");
#endif
        webDirPath = StripIllegalFileCharacters(webDirPath);
        if (webDirPath.length() > 0 &&
            webDirPath[webDirPath.length() - 1] != wxFileName::GetPathSeparator())
            {
            webDirPath += wxFileName::GetPathSeparator();
            }
        downloadPath += webDirPath;
        }

    const wxString downloadPathFolder = downloadPath;

    // and see where it will be downloaded locally
    // (make sure file name is legal for the local file system)
    wxString fileName = wxFileName(urlLocalFileFriendlyName).GetFullName();
    if (fileName.empty())
        {
        return wxString{};
        }
    downloadPath = downloadPath + StripIllegalFileCharacters(fileName);
    /* Check the extension on the file we are downloading. It might not have one, might be
       junk (because it is a PHP query), or it might be a domain that wouldn't make sense for
       local file types. If so, append the file extension "hint" onto it
       (or determine it from the MIME type).*/
    const wxString webFileExt = wxFileName(downloadPath).GetExt();
    if (webFileExt.empty() || webFileExt.length() > 4)
        {
        int rCode{ 200 };
        const wxString downloadExt = StripIllegalFileCharacters(
            fileExtension.length() ? fileExtension :
                                     GetFileTypeFromContentType(GetContentType(Url, rCode)));
        // If we needed to connect to the page to get its MIME type, then check the response
        // code while we are at it. Bail early if we got a bad response (or timed out).
        if (fileExtension.empty() && QueueDownload::IsBadResponseCode(rCode))
            {
            wxLogVerbose(L"'%s': bad response from web page; unable to download", Url);
            return wxString{};
            }
        downloadPath += L'.' + downloadExt;
        }
    else if (html_utilities::html_url_format::is_url_top_level_domain(Url.wc_str()))
        {
        downloadPath += L".html";
        }

    if (!m_replaceExistingFiles && wxFileName::FileExists(downloadPath))
        {
        // if the file already exists and we aren't overwriting,
        // then create a different name for it
        downloadPath = CreateNewFileName(downloadPath);
        if (downloadPath.empty())
            {
            return wxString{};
            }
        }

    wxYield();
    if (m_progressDlg)
        {
        wxStringTokenizer tkz(Url, L"\n\r", wxTOKEN_STRTOK);
        const wxString urlLabel = tkz.GetNextToken();
        if (!m_progressDlg->Pulse(m_hideFileNamesWhileDownloading ?
                                      _(L"Downloading...") :
                                      wxString::Format(_(L"Downloading \"%s\""), urlLabel)))
            {
            m_isCancelled = true;
            return wxString{};
            }
        }

    wxLogVerbose(L"Preparing to download '%s'", Url);
    // create the target folder
    if (!wxFileName::DirExists(downloadPathFolder))
        {
        wxFileName::Mkdir(downloadPathFolder, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
        }

    // now download the file locally
    if (m_downloader.Download(Url, downloadPath))
        {
        m_downloadedFiles.insert(downloadPath);
        }
    else
        {
        const int responseCode = m_downloader.GetLastStatus();

        // check the response code
        if (QueueDownload::IsBadResponseCode(responseCode))
            {
            wxLogWarning(L"%s: unable to connect to page, error code #%i (%s).", Url, responseCode,
                         QueueDownload::GetResponseMessage(responseCode));
            }
        wxLogWarning(L"Unable to download to '%s'", downloadPath);
        downloadPath.clear();
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
            {
            fileExt = fileExt.substr(0, semiColon);
            }
        }
    return fileExt;
    }

//----------------------------------
wxString WebHarvester::GetContentType(wxString& Url, int& responseCode)
    {
    responseCode = 404;

    if (Url.empty())
        {
        return wxString{};
        }
    // encode any spaces
    Url.Trim(true).Trim(false);
    Url.Replace(L" ", L"%20");

    wxLogVerbose(L"Preparing to get content type from '%s'", Url);
    m_downloader.RequestResponse(Url);

    responseCode = m_downloader.GetLastStatus();

    return m_downloader.GetLastContentType();
    }

//----------------------------------
bool WebHarvester::ReadWebPage(wxString& Url, wxString& webPageContent, wxString& contentType,
                               wxString& statusText, int& responseCode,
                               const bool acceptOnlyHtmlOrScriptFiles /*= true*/)
    {
    webPageContent.clear();
    contentType.clear();
    statusText.clear();
    responseCode = 404;

    if (Url.empty())
        {
        return false;
        }

    // strip off bookmark (if there is one)
    const auto bookMarkIndex = Url.find(L'#', true);
    if (bookMarkIndex != wxString::npos)
        {
        Url.Truncate(bookMarkIndex);
        }
    Url.Trim(true).Trim(false);
    // encode any spaces
    Url.Replace(L" ", L"%20");

    wxLogVerbose(L"Preparing to read %s", Url);
    if (!m_downloader.Read(Url))
        {
        responseCode = m_downloader.GetLastStatus();
        statusText = m_downloader.GetLastStatusText();
        wxLogWarning(L"%s: Unable to connect to page, error code #%i (%s).", Url, responseCode,
                     QueueDownload::GetResponseMessage(responseCode));
        return false;
        }

    responseCode = m_downloader.GetLastStatus();
    statusText = m_downloader.GetLastStatusText();
    if (QueueDownload::IsBadResponseCode(responseCode))
        {
        wxLogWarning(L"%s: Unable to connect to page, error code #%i (%s).", Url, responseCode,
                     QueueDownload::GetResponseMessage(responseCode));
        return false;
        }
    else if (m_downloader.GetLastRead().size() > 0)
        {
        contentType = m_downloader.GetLastContentType();
        if (contentType.empty())
            {
            contentType = L"text/html; charset=utf-8";
            }

        // first make sure it is really a webpage
        if (acceptOnlyHtmlOrScriptFiles && contentType.length() &&
            string_util::strnicmp(contentType.wc_str(), HTML_CONTENT_TYPE.data(),
                                  HTML_CONTENT_TYPE.length()) != 0 &&
            string_util::strnicmp(contentType.wc_str(), JAVASCRIPT_CONTENT_TYPE.data(),
                                  JAVASCRIPT_CONTENT_TYPE.length()) != 0 &&
            string_util::strnicmp(contentType.wc_str(), VBSCRIPT_CONTENT_TYPE.data(),
                                  VBSCRIPT_CONTENT_TYPE.length()) != 0)
            {
            return false;
            }

        // get redirect URL (if we got redirected)
        Url = m_downloader.GetLastUrl();
        /* Convert from the file's charset to the application's charset.
           Try to get it from the response header first because that is more
           accurate when the file is really UTF-8 but the designer put something like
           8859-1 in the meta section. If that fails, then read the meta section.*/
        wxString charSet = GetCharsetFromContentType(contentType);
        if (charSet.empty())
            {
            charSet = GetCharsetFromPageContent(
                { &m_downloader.GetLastRead()[0], m_downloader.GetLastRead().size() });
            }
        // Watch out for embedded NULLs in stream (happens with poorly formatted HTML).
        // In this situation, we need to split the stream into valid chunks, convert them,
        // and then piece them back together.
        if (string_util::strnlen(&m_downloader.GetLastRead()[0],
                                 m_downloader.GetLastRead().size()) <
            m_downloader.GetLastRead().size())
            {
            wxLogWarning(L"Embedded null terminator(s) encountered in page.");
            webPageContent = Wisteria::TextStream::CharStreamWithEmbeddedNullsToUnicode(
                &m_downloader.GetLastRead()[0], m_downloader.GetLastRead().size(), charSet);
            }
        else
            {
            webPageContent = Wisteria::TextStream::CharStreamToUnicode(
                &m_downloader.GetLastRead()[0], m_downloader.GetLastRead().size(), charSet);
            }
        }
    else
        {
        responseCode = 204;
        wxLogWarning(L"'%s': connection successful, but no content was read.", Url);
        return false;
        }

    return true;
    }

//----------------------------------
bool WebHarvester::IsPageHtml(wxString& Url, wxString& contentType, int& responseCode)
    {
    contentType.clear();
    if (Url.empty())
        {
        return false;
        }

    contentType = GetContentType(Url, responseCode);
    if (contentType.empty())
        {
        return false;
        }
    return string_util::strnicmp(contentType.wc_str(), HTML_CONTENT_TYPE.data(),
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
            {
            break;
            }
        wxString currentPage{ link };
        DownloadFile(currentPage);
        }
    }

//----------------------------------
bool WebHarvester::CrawlLinks()
    {
    m_hideFileNamesWhileDownloading = wxGetMouseState().ShiftDown();
    wxStringTokenizer tkz(m_url, L"\n\r", wxTOKEN_STRTOK);
    const wxString urlLabel =
        m_hideFileNamesWhileDownloading ? wxString{ L"..." } : L" \"" + tkz.GetNextToken() + L"\"";
    m_progressDlg =
        new wxProgressDialog(_(L"Web Harvester"), wxString::Format(_(L"Harvesting %s"), urlLabel),
                             5, nullptr, wxPD_SMOOTH | wxPD_ELAPSED_TIME | wxPD_CAN_ABORT);
    m_progressDlg->Centre();
    m_progressDlg->Raise();

    // reset state information
    m_isCancelled = false;
    m_currentLevel = 0;

    m_harvestedLinks.clear();
    m_downloadedFiles.clear();
    m_brokenLinks.clear();
    m_alreadyCrawledFiles.clear();
    CrawlLinks(m_url, html_utilities::hyperlink_parse::hyperlink_parse_method::html);

    // Now check the original URL to see if it is a file that should be downloaded
    int responseCode{ 200 };
    const wxString fnExt = GetExtensionOrDomain(m_url);
    const wxString fileExt =
        fnExt.length() ? fnExt : GetFileTypeFromContentType(GetContentType(m_url, responseCode));
    wxString contentType;
    // Add the link to files to harvest/download if it matches our criteria
    if ((m_harvestAllHtml && IsPageHtml(m_url, contentType, responseCode)) ||
        VerifyFileExtension(fileExt))
        {
        HarvestLink(m_url, fileExt);
        }
    m_progressDlg->Pulse();

    m_progressDlg->Destroy();
    m_progressDlg = nullptr;

    wxLogVerbose(L"Crawling '%s' complete.", m_url);

    return !m_isCancelled;
    }

//----------------------------------
bool WebHarvester::CrawlLinks(wxString& url,
                              const html_utilities::hyperlink_parse::hyperlink_parse_method method)
    {
    if (m_isCancelled || url.empty() || HasUrlAlreadyBeenCrawled(url))
        {
        return false;
        }

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
        int responseCode{ 0 };
        if (!ReadWebPage(url, fileText, contentType, statusText, responseCode, true))
            {
            --m_currentLevel;
            // don't bother trying to crawl this later if it failed
            m_alreadyCrawledFiles.insert(url);
            return false;
            }
        }
    else if (resolve.IsLocalOrNetworkFile())
        {
        Wisteria::TextStream::ReadFile(url, fileText);
        }
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
    if (HasUrlAlreadyBeenCrawled(url))
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
        if (!m_progressDlg->Pulse(m_hideFileNamesWhileDownloading ?
                                      _(L"Harvesting...") :
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
    html_utilities::html_url_format formatUrl(url.wc_str());
    while (true)
        {
        // gather its hyperlinks
        const wchar_t* currentLink = getHyperLinks();
        if (currentLink != nullptr)
            {
            CrawlLink(wxString(currentLink, getHyperLinks.get_current_hyperlink_length()),
                      formatUrl, url,
                      // if an image (can only determine this if using the HTML parser)
                      (getHyperLinks.get_parse_method() ==
                       html_utilities::hyperlink_parse::hyperlink_parse_method::html) ?
                          getHyperLinks.get_html_parser().is_current_link_an_image() :
                          false);
            }
        else
            {
            break;
            }
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
                             html_utilities::html_url_format& formatUrl, const wxString& mainUrl,
                             const bool isImage)
    {
    if (m_isCancelled)
        {
        return;
        }

    const wxString urlLabel = currentLink;
    if (!m_progressDlg->Pulse(m_hideFileNamesWhileDownloading ?
                                  _(L"Crawling...") :
                                  wxString::Format(_(L"Crawling \"%s\""), urlLabel)))
        {
        m_isCancelled = true;
        --m_currentLevel;
        return;
        }

    // skip "mailto" anchors, telephone numbers, placeholders,
    // and any bookmarks on the same page
    const wxRegEx anchorsToSkip{ L"[[:space:]]*(mailto[:]|tel[:]|#|"
                                 "javascript[:][[:space:]]*void).*" };

    if (anchorsToSkip.Matches(currentLink))
        {
        return;
        }

    wxString fullUrl;
    fullUrl.assign(formatUrl(currentLink.wc_str(), isImage));
    if (fullUrl.empty())
        {
        return;
        }
    if ((m_currentLevel > GetDepthLevel() || HasUrlAlreadyBeenCrawled(fullUrl)) &&
        HasUrlAlreadyBeenHarvested(fullUrl))
        {
        return;
        }
    // this should usually be turned off for performance,
    // but it's a nice way to get a list of broken links from a site
    if (IsSearchingForBrokenLinks())
        {
        int responseCode{ 200 };
        GetContentType(fullUrl, responseCode);
        if (responseCode == 404)
            {
            m_brokenLinks.emplace(std::make_pair(fullUrl, mainUrl));
            }
        if (QueueDownload::IsBadResponseCode(responseCode))
            {
            wxLogVerbose(L"'%s': bad response from web page; unable to crawl page", fullUrl);
            return;
            }
        }

    // first make sure that if we are domain restricted,
    // then don't bother with it if it's from another domain
    if (!VerifyUrlDomainCriteria(fullUrl))
        {
        return;
        }

    wxString fileExt = GetExtensionOrDomain(fullUrl);
    // try to determine the file type if there is no extension
    if (fileExt.empty())
        {
        const size_t lastSlash = fullUrl.rfind(L'/');
        const size_t queryPos =
            (lastSlash != wxString::npos) ? fullUrl.find(L'?', lastSlash) : wxString::npos;

        // Any sort of page with a query, then treat as PHP
        if (queryPos != std::wstring_view::npos)
            {
            fileExt = L"php";
            }
        else
            {
            int responseCode{ 200 };
            fileExt = GetFileTypeFromContentType(GetContentType(fullUrl, responseCode));
            if (QueueDownload::IsBadResponseCode(responseCode))
                {
                wxLogVerbose(L"'%s': bad response from web page; unable to crawl page", fullUrl);
                return;
                }
            }
        }

    /* See if the page is HTML so that we know whether to crawl it or not. Sometimes
       the file extension on a page is different from its actual content, so if we are
       verifying the content, then also connect to it and read its mime type. Otherwise,
       go off of its extension and if that doesn't work then read its mime type.*/
    wxString contentType;
    bool pageIsHtml = false;

    if (IsKnownRegularFileExtension(fileExt.wc_str()) ||
        IsKnownScriptFileExtension(fileExt.wc_str()))
        {
        pageIsHtml = false;
        }
    else if (IsWebPageExtension(fileExt))
        {
        pageIsHtml = true;
        }
    else if (html_utilities::html_url_format::is_url_top_level_domain(fullUrl.wc_str()))
        {
        pageIsHtml = true;
        }
    else
        {
        int responseCode{ 200 };
        pageIsHtml = IsPageHtml(fullUrl, contentType, responseCode);
        if (QueueDownload::IsBadResponseCode(responseCode))
            {
            wxLogVerbose(L"'%s': bad response from web page; unable to crawl page", fullUrl);
            return;
            }
        }

    html_utilities::html_url_format formatCurrentUrl(fullUrl.wc_str());

    // First, crawl the page (if applicable)
    ///////////////////////////////////////
    // Javascript/VBScript files are crawled differently, so check that first
    if (IsKnownScriptFileExtension(fileExt.wc_str()))
        {
        CrawlLinks(fullUrl, html_utilities::hyperlink_parse::hyperlink_parse_method::script);
        if (VerifyFileExtension(fileExt))
            {
            HarvestLink(fullUrl, fileExt);
            }
        return;
        }
    else if (IsKnownRegularFileExtension(fileExt.wc_str()))
        {
        // Some JPG links are actually HTML pages being used as a gallery of sorts
        // for a JPG, so treat it as such if the MIME type indicates that.
        if (fileExt.CmpNoCase(L"jpg") == 0 || fileExt.CmpNoCase(L"jpeg") == 0)
            {
            int responseCode{ 200 };
            const wxString actualFileType =
                GetFileTypeFromContentType(GetContentType(fullUrl, responseCode));
            if (QueueDownload::IsBadResponseCode(responseCode))
                {
                wxLogVerbose(L"'%s': bad response from web page; unable to crawl page", fullUrl);
                return;
                }
            if (actualFileType.CmpNoCase(L"html") == 0)
                {
                pageIsHtml = true;
                fileExt = actualFileType;
                wxLogVerbose(L"'%s': JPG is really HTML; will be crawled as such.", fullUrl);
                }
            }

        // sometimes pages with a certain file extension are really HTML pages,
        // so crawl if necessary
        if (pageIsHtml)
            {
            CrawlLinks(fullUrl, html_utilities::hyperlink_parse::hyperlink_parse_method::html);
            }
        // add the link to files to harvest/download if it matches our criteria
        if ((m_harvestAllHtml && pageIsHtml) || VerifyFileExtension(fileExt))
            {
            HarvestLink(fullUrl, fileExt);
            }
        }
    else
        {
        /* If not a known "regular" file extension (e.g., PDF) or a webpage extension (e.g., HTML),
           then figure out its type. If a webpage, then crawl it or see if it is a type of file that
           we want to download.*/
        if (IsWebPageExtension(fileExt) ||
            html_utilities::html_url_format::is_url_top_level_domain(fullUrl.wc_str()))
            {
            CrawlLinks(fullUrl, html_utilities::hyperlink_parse::hyperlink_parse_method::html);
            if (m_harvestAllHtml || VerifyFileExtension(fileExt))
                {
                HarvestLink(fullUrl, fileExt);
                }
            return;
            }
        else
            {
            // if it's a PHP query, then see if it is really a page needing to be crawled
            if (formatCurrentUrl.has_query())
                {
                if (pageIsHtml)
                    {
                    CrawlLinks(fullUrl,
                               html_utilities::hyperlink_parse::hyperlink_parse_method::html);
                    }
                // need to get this page's type and download it if it meets the criteria
                fileExt = GetFileTypeFromContentType(contentType);
                if (VerifyFileExtension(fileExt) || (pageIsHtml && m_harvestAllHtml))
                    {
                    // need to override its extension too because the url has a different
                    // file extension on it due to it being a PHP query
                    HarvestLink(fullUrl, fileExt);
                    }
                return;
                }
            // otherwise, an HTML page with an unknown extension
            else if (pageIsHtml)
                {
                CrawlLinks(fullUrl, html_utilities::hyperlink_parse::hyperlink_parse_method::html);
                if (m_harvestAllHtml || VerifyFileExtension(fileExt))
                    {
                    HarvestLink(fullUrl, fileExt);
                    }
                return;
                }
            // ...finally, not a webpage and an unknown extension.
            // Just figure out its real type and see if we should download it
            else if (contentType.length())
                {
                fileExt = GetFileTypeFromContentType(contentType);
                if (VerifyFileExtension(fileExt))
                    {
                    HarvestLink(fullUrl, fileExt);
                    }
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
        {
        return true;
        }
    // this URL should already be fully expanded, so just initialize the formatter with it
    // as the root URL and then set it as the main URL.
    html_utilities::html_url_format formatUrl(url.wc_str());
    formatUrl(url.wc_str(), false);
    if (m_domainRestriction == DomainRestriction::RestrictToDomain)
        {
        if (m_domain != formatUrl.get_domain().c_str())
            {
            return false;
            }
        }
    else if (m_domainRestriction == DomainRestriction::RestrictToSubDomain)
        {
        if (m_fullDomain != formatUrl.get_full_domain().c_str())
            {
            return false;
            }
        }
    else if (m_domainRestriction == DomainRestriction::RestrictToExternalLinks)
        {
        if (m_domain == formatUrl.get_domain().c_str() ||
            m_fullDomain == formatUrl.get_full_domain().c_str())
            {
            return false;
            }
        }
    else if (m_domainRestriction == DomainRestriction::RestrictToSpecificDomains)
        {
        if (m_allowableWebFolders.find(formatUrl.get_directory_path().c_str()) ==
            m_allowableWebFolders.end())
            {
            return false;
            }
        }
    else if (m_domainRestriction == DomainRestriction::RestrictToFolder)
        {
        if (m_fullDomainFolderPath != formatUrl.get_directory_path().c_str())
            {
            return false;
            }
        }
    return true;
    }

//----------------------------------
bool WebHarvester::HarvestLink(wxString& url, const wxString& fileExtension)
    {
    if (m_isCancelled || url.empty() || !VerifyUrlDomainCriteria(url))
        {
        return false;
        }
    else if (HasUrlAlreadyBeenHarvested(url))
        {
        return true;
        }

    // Verify that the file is HTTP/HTTPS.
    FilePathResolver resolve(url, false);
    if (!resolve.IsHTTPFile() && !resolve.IsHTTPSFile())
        {
        return false;
        }
    m_harvestedLinks.insert(url);
    if (IsDownloadingFilesWhileCrawling())
        {
        DownloadFile(url, fileExtension);
        }
    return true;
    }

//----------------------------------
wxString WebHarvester::GetCharsetFromContentType(const wxString& contentType)
    {
    const auto index = contentType.Lower().find(L"charset=");
    const auto semicolon = contentType.find(L";");
    if (index != wxString::npos)
        {
        wxString charSet = contentType.substr(index + 8);
        charSet.Replace(L"\"", L"");
        charSet.Replace(L"\'", L"");
        charSet.Trim(false);
        charSet.Trim(true);
        return charSet;
        }
    else if (semicolon != wxString::npos)
        {
        wxString charSet = contentType.substr(semicolon + 1);
        charSet.Replace(L"\"", L"");
        charSet.Replace(L"\'", L"");
        charSet.Trim(false);
        charSet.Trim(true);
        return charSet;
        }
    else
        {
        return wxLocale::GetSystemEncodingName();
        }
    }

//----------------------------------
wxString WebHarvester::GetCharsetFromPageContent(std::string_view pageContent)
    {
    std::string charSet = lily_of_the_valley::html_extract_text::parse_charset(
        pageContent.data(), pageContent.length());
    if (charSet.empty())
        {
        return wxLocale::GetSystemEncodingName();
        }
    else
        {
        // The HTML 5 specification requires that documents advertised as
        // ISO-8859-1 actually be parsed with the Windows-1252 encoding.
        if (string_util::stricmp(charSet.c_str(), "iso-8859-1") == 0)
            {
            charSet = "windows-1252";
            }
        return wxString(charSet.c_str(), wxConvLibc);
        }
    }
