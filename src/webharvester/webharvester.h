/** @addtogroup Web Harvesting
    @brief Classes for the web scraping and downloading.
    @date 2005-2023
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __WEBHARVESTER_H__
#define __WEBHARVESTER_H__

#include "../Wisteria-Dataviz/src/i18n-check/src/char_traits.h"
#include "../Wisteria-Dataviz/src/i18n-check/src/donttranslate.h"
#include "../Wisteria-Dataviz/src/import/html_extract_text.h"
#include "../Wisteria-Dataviz/src/util/downloadfile.h"
#include "../Wisteria-Dataviz/src/util/fileutil.h"
#include "../Wisteria-Dataviz/src/util/textstream.h"
#include <algorithm>
#include <functional>
#include <limits>
#include <optional>
#include <set>
#include <string_view>
#include <wx/dir.h>
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/filesys.h>
#include <wx/progdlg.h>
#include <wx/regex.h>
#include <wx/stream.h>
#include <wx/url.h>
#include <wx/utils.h>
#include <wx/wfstream.h>
#include <wx/wx.h>

/// @private
class wxStringLessWebPath
    {
  public:
    [[nodiscard]]
    bool
    operator()(const wxString& first, const wxString& second) const;
    };

/// @brief List of known web file extensions.
class WebPageExtension
    {
  public:
    /** @returns @c true if @c extension is a known web file extension.
        @param extension The file extension to review.*/
    [[nodiscard]]
    bool
    operator()(const wxString& extension) const
        {
        if (m_knownWebPageExtensions.find(extension.wc_str()) != m_knownWebPageExtensions.cend() ||
            m_knownWebPageExtensions.find(GetExtensionOrDomain(extension).wc_str()) !=
                m_knownWebPageExtensions.cend())
            {
            return true;
            }

        return false;
        }

    /** @returns @c true if @c extension is a dynamic webpage extension.
        @param extension The file extension to review.*/
    [[nodiscard]]
    bool IsDynamicExtension(const wxString& extension) const
        {
        if (m_knownDynamicExtensions.find(extension.wc_str()) != m_knownDynamicExtensions.cend() ||
            m_knownDynamicExtensions.find(GetExtensionOrDomain(extension).wc_str()) !=
                m_knownDynamicExtensions.cend())
            {
            return true;
            }

        return false;
        }

  private:
    std::set<string_util::case_insensitive_wstring> m_knownWebPageExtensions{
        _DT(L"asp"),  _DT(L"aspx"), _DT(L"ca"),    _DT(L"cfm"), _DT(L"cfml"), _DT(L"biz"),
        _DT(L"com"),  _DT(L"net"),  _DT(L"org"),   _DT(L"php"), _DT(L"php3"), _DT(L"php4"),
        _DT(L"html"), _DT(L"htm"),  _DT(L"xhtml"), _DT(L"sgml")
    };
    std::set<string_util::case_insensitive_wstring> m_knownDynamicExtensions{
        _DT(L"asp"), _DT(L"aspx"), _DT(L"php"), _DT(L"php3"), _DT(L"php4")
    };
    };

/// @brief List of known document or media file extensions that can be downloaded from a website.
/// @details These would be documents such as PDF or images, rather than HTML pages.
class NonWebPageFileExtension
    {
  public:
    /** @returns @c true if @c extension is a known file extension.
        @param extension The file extension to review.*/
    [[nodiscard]]
    inline bool
    operator()(std::wstring_view extension) const
        {
        return (m_nonWebPageFileExtensions.find(extension.data()) !=
                m_nonWebPageFileExtensions.cend());
        }

  private:
    std::set<string_util::case_insensitive_wstring> m_nonWebPageFileExtensions{
        // images
        _DT(L"ico"), _DT(L"jpg"), _DT(L"jpeg"), _DT(L"bmp"), _DT(L"gif"), _DT(L"png"), _DT(L"psd"),
        _DT(L"tif"), _DT(L"tiff"), _DT(L"wmf"), _DT(L"tga"), _DT(L"svg"),
        // style sheets
        _DT(L"css"),
        // documents
        _DT(L"doc"), _DT(L"docx"), _DT(L"ppt"), _DT(L"pptx"), _DT(L"xls"), _DT(L"xlsx"),
        _DT(L"csv"), _DT(L"rtf"), _DT(L"pdf"), _DT(L"ps"), _DT(L"txt"),
        // movies
        _DT(L"mov"), _DT(L"qt"), _DT(L"rv"), _DT(L"rm"), _DT(L"wmv"), _DT(L"mpg"), _DT(L"mpeg"),
        _DT(L"mpe"), _DT(L"avi"),
        // music
        _DT(L"mp3"), _DT(L"wav"), _DT(L"wma"), _DT(L"midi"), _DT(L"ra"), _DT(L"ram"),
        // programs
        _DT(L"exe"), _DT(L"jar"), _DT(L"swf"),
        // compressed files
        _DT(L"zip"), _DT(L"gzip"), _DT(L"tar"), _DT(L"bz2")
    };
    };

/// @brief List of known web script (e.g., JavaScript) extensions.
class ScriptFileExtension
    {
  public:
    /** @returns @c true if @c extension is a web script file extension.
        @param extension The file extension to review.*/
    [[nodiscard]]
    inline bool
    operator()(std::wstring_view extension) const
        {
        return (m_scriptFileExtensions.find(extension.data()) != m_scriptFileExtensions.cend());
        }

  private:
    std::set<string_util::case_insensitive_wstring> m_scriptFileExtensions{ _DT(L"js"),
                                                                            _DT(L"vbs") };
    };

/// @brief Interface for harvesting and (optionally) downloading web content from a base URL.
/// @details This is recommended to be a singleton object that connects to the application
///     or main frame. Be sure to call SetEventHandler() to connect the download events
///     for your parent event handler to the harvester. Also, call CancelPending() in the
///     parent event handler's close event.
class WebHarvester
    {
  public:
    /// @brief Domain restriction methods.
    enum class DomainRestriction
        {
        NotRestricted,             /*!< No restrictions.*/
        RestrictToDomain,          /*!< Restrict harvesting to the base URL's domain.*/
        RestrictToSubDomain,       /*!< Restrict harvesting to the base URL's subdomain.*/
        RestrictToSpecificDomains, /*!< Restrict harvesting to a list of user-defined domains.*/
        RestrictToExternalLinks,   /*!< Restrict harvesting to links outside of the base URL's
                                        domain.*/
        RestrictToFolder           /*!< Restrict harvesting to links within the base URL's folder.*/
        };

    /// @private
    WebHarvester()
        {
        m_downloader.SetUserAgent(GetUserAgent());
        m_downloader.DisablePeerVerify(IsPeerVerifyDisabled());
        }

    /// @private
    WebHarvester(const WebHarvester&) = delete;
    /// @private
    WebHarvester& operator=(const WebHarvester&) = delete;

    /// @brief Crawls the loaded URL.
    /// @returns @c false if crawl was cancelled.
    [[nodiscard]]
    bool CrawlLinks();

    /// @brief Downloads a file from the Internet.
    /// @param Url The link to download.
    /// @param fileExtension The (hint) file extension to download the file as. This is only
    ///     used if the webpage doesn't have a proper extension. If empty and
    ///     @c Url is empty, then the file extension will be determined by the MIME type.
    /// @returns The local file path of the file after downloading, or empty string upon failure.
    wxString DownloadFile(wxString& Url, const wxString& fileExtension = wxString{});
    /// @brief Download all the harvested links.
    /// @note This should be called after CrawlLinks().
    void DownloadFiles();

    /// @brief Cancels any pending download, read, or harvesting operation.
    void CancelPending() noexcept
        {
        m_isCancelled = true;
        m_downloader.CancelPending();
        }

    /** @brief Reads the content of a webpage into a buffer.
        @param[in,out] Url The webpage (may be altered if redirected).
        @param[out] webPageContent The content of the page.
        @param[out] contentType The MIME type (and possibly charset) of the page.
        @param[out] statusText Any possible information from the server
            (usually extended error information).
        @param[out] responseCode The response code when connecting to the page.
        @param acceptOnlyHtmlOrScriptFiles Whether only HTML or
            JS script files should be read.
        @returns Whether the file was successfully read.*/
    [[nodiscard]]
    bool ReadWebPage(wxString& Url, wxString& webPageContent, wxString& contentType,
                     wxString& statusText, int& responseCode,
                     const bool acceptOnlyHtmlOrScriptFiles = true);

    /// @brief Attempts to connect to @c url.
    /// @param Url The webpage to try to connect to.
    void RequestResponse(const wxString& url) { m_downloader.RequestResponse(url); }

    /// @returns The internal FileDownload object.
    [[nodiscard]]
    const FileDownload& GetDownloader() const noexcept
        {
        return m_downloader;
        }

    /** @brief Gets the content type of a webpage.
        @param[in,out] Url The webpage (may be altered if redirected).
        @param[out] responseCode The response code when connecting to the page.
        @returns The MIME type (and possibly charset) of the page's content type.*/
    wxString GetContentType(wxString& Url, int& responseCode);
    /// @returns The file type (possibly an extension) from a MIME type string.
    /// @param contentType The MIME type string.
    [[nodiscard]]
    static wxString GetFileTypeFromContentType(const wxString& contentType);

    /** @returns @c true if a link is pointing to an HTML page.
        @param[in,out] Url The link to test (may be redirected).
        @param[out] contentType The MIME type read from the page.
        @param[out] responseCode The response code from the page.*/
    [[nodiscard]]
    bool IsPageHtml(wxString& Url, wxString& contentType, int& responseCode);

    /// @brief Sets the depth level to crawl from the base URL.
    /// @param levels The number of levels to crawl.
    void SetDepthLevel(const size_t levels) noexcept { m_levelDepth = levels; }

    /// @returns The depth level to crawl from the base URL.
    [[nodiscard]]
    size_t GetDepthLevel() const noexcept
        {
        return m_levelDepth;
        }

    /// @brief Adds a file type to harvest and download (based on extension).
    /// @param fileExtension The file extension to download.
    /// @note You can pass in the extension,
    ///     or a full filepath and it will get the extension from that.
    void AddAllowableFileType(const wxString& fileExtension)
        {
        if (fileExtension.find(L'.') == std::wstring::npos)
            {
            m_fileExtensions.insert(fileExtension);
            }
        else
            {
            m_fileExtensions.insert(wxFileName(fileExtension).GetExt());
            }
        }

    /// @brief Removes any explicit file extensions that are restricting
    ///     to while harvesting and downloading.
    void ClearAllowableFileTypes() { m_fileExtensions.clear(); }

    /// @brief When downloading locally, keep the folder structure from the website.
    /// @param keep @c true to use the website's folder structure, @c false to download files in
    ///     a flat folder structure.
    /// @note This is recommended to prevent overwriting files with the same name.
    void KeepWebPathWhenDownloading(const bool keep = true) noexcept
        {
        m_keepWebPathWhenDownloading = keep;
        }

    /// @returns Whether the website's folder structure is being
    ///     mirrored when downloading files.
    [[nodiscard]]
    bool IsKeepingWebPathWhenDownloading() const noexcept
        {
        return m_keepWebPathWhenDownloading;
        }

    /// @brief Specifies whether all HTML content should be downloaded,
    ///     regardless of the file's extension.
    /// @param harvestAll @c true to download all HTML content.
    /// @note This is recommended if needing to download PHP response
    ///     pages with non-standard file extensions.
    void HarvestAllHtmlFiles(const bool harvestAll = true) noexcept
        {
        m_harvestAllHtml = harvestAll;
        }

    /// @brief Sets whether to build a list of broken links while crawling.
    /// @param search @c true to catalogue broken links.
    /// @warning Enabling this will degrade performance because it will
    ///     attempt to connect to each link.
    void SeachForBrokenLinks(const bool search = true) noexcept { m_searchForBrokenLinks = search; }

    /// @returns @c true if a list of broken links are being catalogued while harvesting.
    [[nodiscard]]
    bool IsSearchingForBrokenLinks() const noexcept
        {
        return m_searchForBrokenLinks;
        }

    /// @brief Sets the base URL to crawl.
    /// @param url The base URL.
    void SetUrl(const wxString& url)
        {
        m_url = url;
        html_utilities::html_url_format formatUrl(m_url.wc_str());
        m_domain = formatUrl.get_root_domain().c_str();
        m_fullDomain = formatUrl.get_root_full_domain().c_str();
        m_fullDomainFolderPath = formatUrl.get_directory_path().c_str();
        }

    /// @returns The base URL being crawled.
    [[nodiscard]]
    const wxString& GetUrl() const noexcept
        {
        return m_url;
        }

    /// @brief Sets the local folder to download files from the web.
    /// @param downloadDirectory The local download folder.
    void SetDownloadDirectory(const wxString& downloadDirectory)
        {
        m_downloadDirectory = downloadDirectory;
        }

    /// @returns The local folder where web content is being downloaded.
    [[nodiscard]]
    const wxString& GetDownloadDirectory() const noexcept
        {
        return m_downloadDirectory;
        }

    /// @returns @c true if files being downloaded will replace existing ones.
    /// @note If this returns @c false, then downloaded files will be renamed
    ///     if necessary to avoid overwriting existing files.
    [[nodiscard]]
    bool IsReplacingExistingFiles() const noexcept
        {
        return m_replaceExistingFiles;
        }

    /// @brief Specifies whether files being downloaded can overwrite
    ///     each other if they have the same path.
    /// @param replaceExistingFiles @c true to overwrite existing files.
    /// @note If this is set to @c false and a file with the same path
    ///     is about to be downloaded, the program will attempt to download
    ///     it with a different (but similar) name.
    void ReplaceExistingFiles(const bool replaceExistingFiles = true) noexcept
        {
        m_replaceExistingFiles = replaceExistingFiles;
        }

    /// @brief Sets whether to download files locally while crawling.
    /// @param downloadWhileCrawling @c true to download the web content.
    void DownloadFilesWhileCrawling(const bool downloadWhileCrawling = true) noexcept
        {
        m_downloadWhileCrawling = downloadWhileCrawling;
        }

    /// @returns Whether files are being downloaded locally while crawling.
    [[nodiscard]]
    bool IsDownloadingFilesWhileCrawling() const noexcept
        {
        return m_downloadWhileCrawling;
        }

    // Domain restriction
    //----------------------------------

    /** @brief Sets the domain restriction method.
        @param restriction The restriction method to use.*/
    void SetDomainRestriction(const DomainRestriction restriction) noexcept
        {
        m_domainRestriction = restriction;
        }

    /// @returns The domain-restriction method.
    [[nodiscard]]
    DomainRestriction GetDomainRestriction() const noexcept
        {
        return m_domainRestriction;
        }

    /// @brief Overrides the domain of the main webpage. Useful for only getting files
    ///     from an outside domain (or specific folder).
    /// @param domain The domain to restrict to.
    void SetRestrictedDomain(const wxString& domain)
        {
        html_utilities::html_url_format formatUrl(domain.wc_str());
        m_domain = formatUrl.get_root_domain().c_str();
        m_domainRestriction = DomainRestriction::RestrictToDomain;
        }

    /// @brief Resets the list of user-defined webpath restrictions.
    void ClearAllowableWebFolders() noexcept { m_allowableWebFolders.clear(); }

    /** @brief Adds a user-defined web path (domain/folder structure)
            to restrict harvesting to.
        @param domain A webpath to restrict harvesting to.*/
    void AddAllowableWebFolder(wxString domain)
        {
        if (domain.empty())
            {
            return;
            }
        // if a full webpage, then it should have an extension on it and
        // html_url_format will remove the webpage. But if there is no
        // extension (or its junk), then add a trailing '/' to prevent
        // the last folder from being removed.
        const wxString webExt = wxFileName(domain).GetExt();
        if ((webExt.empty() || webExt.length() > 4) && !domain.ends_with(L"/"))
            {
            domain.append(L"/");
            }
        html_utilities::html_url_format formatUrl(domain.wc_str());
        if (formatUrl.get_directory_path().length())
            {
            m_allowableWebFolders.emplace(formatUrl.get_directory_path().c_str());
            }
        }

    /// @returns The user-defined web paths (domains, folder structure)
    ///     that harvesting is constrained to.
    [[nodiscard]]
    wxArrayString GetAllowableWebFolders() const
        {
        wxArrayString domains;
        for (const auto& domain : m_allowableWebFolders)
            {
            domains.Add(domain.c_str());
            }
        return domains;
        }

    /// @brief Connect the downloader to a parent dialog or @c wxApp.
    /// @param handler The @c wxEvtHandler to connect the downloader to.
    void SetEventHandler(wxEvtHandler* handler) { m_downloader.SetAndBindEventHandler(handler); }

    /// @returns The list of harvested links.
    [[nodiscard]]
    const std::set<wxString, wxStringLessWebPath>& GetHarvestedLinks() const noexcept
        {
        return m_harvestedLinks;
        }

    /// @returns The list of files downloaded.
    /// @note DownloadFilesWhileCrawling() must be enabled.
    [[nodiscard]]
    const std::set<wxString>& GetDownloadedFilePaths() const noexcept
        {
        return m_downloadedFiles;
        }

    /// @returns A map of broken links and the respective pages they were found on.
    /// @note SeachForBrokenLinks() must be enabled.
    [[nodiscard]]
    const std::map<wxString, wxString>& GetBrokenLinks() const noexcept
        {
        return m_brokenLinks;
        }

    /// @returns The user agent sent to websites when crawling.
    /// @note If the user agent is empty, then one will be built from the OS description.
    [[nodiscard]]
    wxString GetUserAgent() const
        {
        /* May need to be set if not initialized.
           Needs to be initialized here because wxGetOsDescription()
           can't be called during global startup.
           Note that we call this a "WebLion."
           Using words like "harvester," "crawler," "scraper," and
           even "browser" will result in a forbidden response from some sites,
           so avoid using those words. */
        return (m_userAgent.empty() ?
                    _DT(L"Mozilla/5.0 (") + wxGetOsDescription() + _DT(L") WebKit/12.0 WebLion") :
                    m_userAgent);
        }

    /// @brief Sets the user agent sent to websites when crawling.
    /// @param agent The user agent string.
    void SetUserAgent(wxString agent)
        {
        m_userAgent = std::move(agent);
        m_downloader.SetUserAgent(GetUserAgent());
        }

    /** @brief Disable SSL certificate verification.
        @details This can be used to connect to self-signed servers or other
            invalid SSL connections.\n
            Disabling verification makes the communication insecure.
        @param disable @c true to disable SSL certificate verification.*/
    void DisablePeerVerify(const bool disable) noexcept
        {
        m_disablePeerVerify = disable;
        m_downloader.DisablePeerVerify(m_disablePeerVerify);
        }

    /// @returns Returns @c true if peer verification has been disabled.
    [[nodiscard]]
    bool IsPeerVerifyDisabled() const noexcept
        {
        return m_disablePeerVerify;
        }

    /// @brief Sets the minimum size that a file has to be to download it.
    /// @param size The minimum file size, in kilobytes.
    void SetMinimumDownloadFileSizeInKilobytes(const std::optional<uint32_t> size)
        {
        m_minFileDownloadSizeKilobytes = size;
        m_downloader.SetMinimumDownloadFileSizeInKilobytes(size);
        }

    /// @returns The minimum size a file must be to download. Will be `std::nullopt`
    ///     if size constraints are not being enforced.
    std::optional<uint32_t> GetMinimumDownloadFileSizeInKilobytes() const
        {
        return m_minFileDownloadSizeKilobytes;
        }

    /// @returns The character set, parsed from HTML's content type.
    /// @param contentType The content type section from a block of HTML to parse.
    [[nodiscard]]
    static wxString GetCharsetFromContentType(const wxString& contentType);

    /// @returns The character set, parsed from HTML's content.
    /// @param pageContent The full content of the HTML page.
    [[nodiscard]]
    static wxString GetCharsetFromPageContent(std::string_view pageContent);

  protected:
    /// @returns An URL with spaces encoded to '%20', '\/' converted to '/', and trimmed.
    /// @param url The URL to normalized.
    [[nodiscard]]
    wxString NormalizeUrl(const wxString& url)
        {
        wxString adjUrl{ url };
        adjUrl.Trim(true).Trim(false);
        // encode any spaces
        adjUrl.Replace(L" ", L"%20");
        adjUrl.Replace(L"\\/", L"/");

        return adjUrl;
        }
    [[nodiscard]]
    bool VerifyUrlDomainCriteria(const wxString& url);
    /** @brief If @c url meets all the criteria, adds it to the list of links
            that we are gathering for the client.\n
            If downloading while crawling, will also download the file.
        @param url The URL to review.
        @param fileExtension File extension information about the URL.
            This will be used if filtering file types to harvest (and download).
        @returns @c true if the provided URL will be included in the harvested results
            (and downloaded, if applicable).
        @warning File extension criteria must be handled by the caller because
            if the harvester is include all HTML pages then it will need to determine
            if an URL is HTML before passing it to this function.*/
    bool HarvestLink(wxString& url, const wxString& fileExtension);
    //----------------------------------
    bool CrawlLinks(wxString& url,
                    const html_utilities::hyperlink_parse::hyperlink_parse_method method);
    // cppcheck-suppress constParameter
    void CrawlLink(const wxString& currentLink, html_utilities::html_url_format& formatUrl,
                   const wxString& mainUrl, const bool isImage);

    [[nodiscard]]
    bool HasUrlAlreadyBeenHarvested(const wxString& url) const
        {
        return (m_harvestedLinks.find(url) != m_harvestedLinks.cend());
        }

    [[nodiscard]]
    bool HasUrlAlreadyBeenCrawled(const wxString& url) const
        {
        return (m_alreadyCrawledFiles.find(url) != m_alreadyCrawledFiles.cend());
        }

    [[nodiscard]]
    bool VerifyFileExtension(const wxString& fileExt) const
        {
        return (m_fileExtensions.find(fileExt) != m_fileExtensions.cend());
        }

  private:
    class wxStringLessNoCase
        {
      public:
        [[nodiscard]]
        inline bool
        operator()(const wxString& first, const wxString& second) const
            {
            return (first.CmpNoCase(second) < 0);
            }
        };

    size_t m_levelDepth{ 1 };
    wxString m_url;
    wxString m_userAgent;
    bool m_disablePeerVerify{ false };
    string_util::case_insensitive_wstring m_domain;
    string_util::case_insensitive_wstring m_fullDomain;
    string_util::case_insensitive_wstring m_fullDomainFolderPath;
    std::set<string_util::case_insensitive_wstring> m_allowableWebFolders;
    std::set<wxString, wxStringLessNoCase> m_fileExtensions;
    // cached state information
    std::set<wxString, wxStringLessWebPath> m_harvestedLinks;
    std::set<wxString> m_downloadedFiles;
    std::map<wxString, wxString> m_brokenLinks;
    std::set<wxString, wxStringLessWebPath> m_alreadyCrawledFiles;
    size_t m_currentLevel{ 0 };
    bool m_isCancelled{ false };

    wxString m_downloadDirectory;
    bool m_keepWebPathWhenDownloading{ true };
    NonWebPageFileExtension IsNonWebPageFileExtension;
    ScriptFileExtension IsScriptFileExtension;
    WebPageExtension IsWebPageExtension;
    // download criteria
    DomainRestriction m_domainRestriction{ DomainRestriction::RestrictToDomain };
    bool m_downloadWhileCrawling{ false };

    std::optional<uint32_t> m_minFileDownloadSizeKilobytes{ std::nullopt };
    bool m_replaceExistingFiles{ true };
    bool m_harvestAllHtml{ true };
    bool m_searchForBrokenLinks{ false };
    bool m_hideFileNamesWhileDownloading{ false };

    FileDownload m_downloader;
    // UI functionality
    wxProgressDialog* m_progressDlg{ nullptr };

    inline static const std::wstring_view HTML_CONTENT_TYPE{ _DT(L"text/html") };
    inline static const std::wstring_view JAVASCRIPT_CONTENT_TYPE{ _DT(
        L"application/x-javascript") };
    inline static const std::wstring_view VBSCRIPT_CONTENT_TYPE{ _DT(L"application/x-vbscript") };
    };

    /** @}*/

#endif // __WEBHARVESTER_H__
