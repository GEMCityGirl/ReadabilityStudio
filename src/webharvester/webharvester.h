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

#include <limits>
#include <set>
#include <functional>
#include <algorithm>
#include <wx/wx.h>
#include <wx/filesys.h>
#include <wx/url.h>
#include <wx/stream.h>
#include <wx/filename.h>
#include <wx/file.h>
#include <wx/wfstream.h>
#include <wx/dir.h>
#include <wx/utils.h>
#include <wx/progdlg.h>
#include "../../../../SRC/curl/include/curl/curl.h"
#include "../../../../SRC/curl/include/curl/easy.h"
#include "../Wisteria-Dataviz/src/import/html_extract_text.h"
#include "../Wisteria-Dataviz/src/i18n-check/src/char_traits.h"
#include "../Wisteria-Dataviz/src/util/textstream.h"
#include "../Wisteria-Dataviz/src/i18n-check/src/donttranslate.h"

class wxStringLessWebPath
    {
public:
    [[nodiscard]]
    bool operator()(const wxString& first, const wxString& second) const;
    };

/// @brief List of known web file extensions.
class WebPageExtension
    {
public:
    /** @returns @c true if @c extension is a known web file extension.
        @param extension The file extension to review.*/
    [[nodiscard]]
    inline bool operator()(const wchar_t* extension) const
        {
        // any sort of PHP page (even without the extension PHP) will follow this syntax
        if (std::wcschr(extension, wxT('?')) && std::wcschr(extension, wxT('=')) )
            { return true; }
        return (m_knownRegularFileExtensions.find(extension) != m_knownRegularFileExtensions.cend());
        }
private:
    std::set<string_util::case_insensitive_wstring> m_knownRegularFileExtensions
        { _DT(L"asp"), _DT(L"aspx"), _DT(L"ca"), _DT(L"cfm"),
          _DT(L"cfml"), _DT(L"biz"), _DT(L"com"), _DT(L"net"),
          _DT(L"org"), _DT(L"php"), _DT(L"php3"), _DT(L"php4"),
          _DT(L"html"), _DT(L"htm"), _DT(L"xhtml"), _DT(L"sgml") };
    };

/// @brief List of known file extensions that can be downloaded from a website.
class KnownRegularFileExtension
    {
public:
    /** @returns @c true if @c extension is a known file extension.
        @param extension The file extension to review.*/
    [[nodiscard]]
    inline bool operator()(const wchar_t* extension) const
        { return (m_knownRegularFileExtensions.find(extension) != m_knownRegularFileExtensions.cend()); }
private:
    std::set<string_util::case_insensitive_wstring> m_knownRegularFileExtensions
        { // images
          _DT(L"ico"), _DT(L"jpg"), _DT(L"jpeg"), _DT(L"bmp"),
          _DT(L"gif"), _DT(L"png"), _DT(L"psd"), _DT(L"tif"),
          _DT(L"tiff"), _DT(L"wmf"), _DT(L"tga"),
          // style sheets
          _DT(L"css"),
          // documents
          _DT(L"doc"), _DT(L"docx"), _DT(L"ppt"), _DT(L"pptx"),
          _DT(L"xls"), _DT(L"xlsx"), _DT(L"csv"), _DT(L"rtf"),
          _DT(L"pdf"), _DT(L"ps"), _DT(L"txt"),
          // movies
          _DT(L"mov"), _DT(L"qt"), _DT(L"rv"), _DT(L"rm"),
          _DT(L"wmv"), _DT(L"mpg"), _DT(L"mpeg"), _DT(L"mpe"),
          _DT(L"avi"),
          // music
          _DT(L"mp3"), _DT(L"wav"), _DT(L"wma"), _DT(L"midi"),
          _DT(L"ra"), _DT(L"ram"),
          // programs
          _DT(L"exe"), _DT(L"jar"), _DT(L"swf"),
          // compressed files
          _DT(L"zip"), _DT(L"gzip"), _DT(L"tar"), _DT(L"bz2") };
    };

/// @brief List of known web script (e.g., JavaScript) extensions.
class KnownScriptFileExtension
    {
public:
    /** @returns @c true if @c extension is a known web script file extension.
        @param extension The file extension to review.*/
    [[nodiscard]]
    inline bool operator()(const wchar_t* extension) const
        { return (m_knownFileExtensions.find(extension) != m_knownFileExtensions.cend()); }
private:
    std::set<string_util::case_insensitive_wstring> m_knownFileExtensions
        { _DT(L"js"), _DT(L"vbs") };
    };

/// @brief URL wrapper class that supports parsing a numeric sequence from the path.
class UrlWithNumericSequence
    {
public:
    /** @brief Constructor.
        @param path The URL to parse.
        @param referUrl The referring URL that @c path originated from.*/
    UrlWithNumericSequence(const wxString& path, const wxString& referUrl) :
        m_string(path), m_refererUrl(referUrl)
        { ParseSequenceNumber(); }
    /** @returns @c true if this URL is less than @c that.
        @param that The URL to compare against.*/
    [[nodiscard]]
    bool operator<(const UrlWithNumericSequence& that) const
        { return pathCmp(m_string, that.m_string); }
    /// @returns The base URL.
    [[nodiscard]]
    const wxString& GetPath() const noexcept
        { return m_string; }
    /// @returns The referring URL.
    [[nodiscard]]
    const wxString& GetReferUrl() const noexcept
        { return m_refererUrl; }
    /// @returns The number parsed from the URL.
    [[nodiscard]]
    long GetNumericValue() const noexcept
        { return m_numeric_value; }
    /// @returns The length of the numeric string in the URL.
    [[nodiscard]]
    size_t GetNumericWidth() const noexcept
        { return m_numeric_width; }
    /// @returns The section of the URL up to the number.
    [[nodiscard]]
    wxString GetPathPrefix() const
        { return m_string.substr(0, m_number_start); }
    /// @returns The section of the URL starting at the number.
    [[nodiscard]]
    wxString GetPathSuffix() const
        { return m_string.substr(m_number_end); }
 protected:
    void ParseSequenceNumber();
    void Reset() noexcept
        {
        m_numeric_value = wxNOT_FOUND;
        m_number_start = m_number_end = m_numeric_width = 0;
        }
    wxStringLessWebPath pathCmp;
    wxString m_string;
    wxString m_refererUrl;
    size_t m_number_start{ 0 };
    size_t m_number_end{ 0 };
    long m_numeric_value{ wxNOT_FOUND };
    size_t m_numeric_width{ 0 };
    };

/// @brief Interface for harvesting and (optionally) downloading links from a base URL.
class WebHarvester
    {
public:
    // Libcurl functions
    struct MemoryStruct
        {
        MemoryStruct() noexcept : memory(nullptr), size(0)
            {}
        ~MemoryStruct()
            { free(memory); }
        MemoryStruct(const MemoryStruct&) = delete;
        MemoryStruct(MemoryStruct&&) = delete;
        MemoryStruct& operator=(const MemoryStruct&) = delete;
        MemoryStruct& operator=(MemoryStruct&&) = delete;

        char* memory{ nullptr };
        size_t size{ 0 };
        };

    [[nodiscard]]
    static void* safe_realloc(void *ptr, size_t size)
        {
        /* There might be a realloc() out there that doesn't like reallocating
           null pointers, so we take care of it here */ 
        if (ptr)
            return realloc(ptr, size);
        else
            return malloc(size);
        }
    static size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data);

    /// Domain restriction methods.
    enum class DomainRestriction
        {
        NotRestricted,             /*!< No restrictions.*/
        RestrictToDomain,          /*!< Restrict harvesting to the base URL's domain.*/
        RestrictToSubDomain,       /*!< Restrict harvesting to the base URL's subdomain.*/
        RestrictToSpecificDomains, /*!< Restrict harvesting to a list of user-defined domains.*/
        RestrictToExternalLinks,   /*!< Restrict harvesting to links outside of the base URL's domain.*/
        RestrictToFolder           /*!< Restrict harvesting to links within the base URL's folder.*/
        };

    WebHarvester() = default;
    /// @brief Constructor.
    /// @param rootUrl The root URL to start the crawl from.
    explicit WebHarvester(const wxString& rootUrl) : m_url(rootUrl)
        {
        html_utilities::html_url_format formatUrl(rootUrl);
        m_domain = formatUrl.get_root_domain().c_str();
        m_fullDomain = formatUrl.get_root_full_domain().c_str();
        m_fullDomainFolderPath = formatUrl.get_directory_path().c_str();
        }
    /// @private
    WebHarvester(const WebHarvester&) = delete;
    /// @private
    WebHarvester& operator=(const WebHarvester&) = delete;

    /// @brief Crawls the loaded URL.
    /// @returns @c false is crawl was cancelled.
    [[nodiscard]]
    bool CrawlLinks();
    
    /// @brief Downloads a file from the Internet.
    /// @param Url The link to download.
    /// @param allowRedirect Set to true to follow the link if redirecting to another page.
    /// @param fileExtension The (hint) file extension to download the file as. This is only
    ///     used if the webpage doesn't have a proper extension. If empty and
    /// @c Url is empty, then the file extension will be determined by the MIME type.
    /// @returns The local file path of the file after downloading, or empty string upon failure.
    wxString DownloadFile(wxString& Url,
                          const bool allowRedirect = true,
                          const wxString& fileExtension = wxString{});
    /// @brief Download all of the harvested links.
    /// @note This should be called after CrawlLinks().
    void DownloadFiles();

    //Static help functions
    //----------------------------------
    /** @brief Creates a new file based on @c filePath, embedding a numeric
            sequence in it (making it unique).
            This is useful for saving a file and not overwriting one that
            already exists with the same name.
        @param filePath The original filepath
        @returns The new filepath that was created.*/
    [[nodiscard]]
    static wxString CreateNewFileName(const wxString& filePath);

    /** @brief Reads the content of a webpage into a buffer.
        @param[in,out] Url The webpage (may be altered if redirected).
        @param[out] webPageContent The content of the page.
        @param[out] contentType The MIME type of the page.
        @param[out] responseCode The response code when connecting to the page.
        @param acceptOnlyHtmlOrScriptFiles Whether only HTML or
            JS script files should be read.
        @param allowRedirect True if the page should still be read if
            redirected to a different page.
        @param userName The username to connect to the page (not currently implemented).
        @param passWord The password to connect to the page (not currently implemented).
        @returns Whether the file was successfully read.*/
    [[nodiscard]]
    static bool ReadWebPage(wxString& Url,
                            wxString& webPageContent,
                            wxString& contentType,
                            long& responseCode,
                            const bool acceptOnlyHtmlOrScriptFiles = true,
                            const bool allowRedirect = true,
                            // these are not used yet, so just hide them
                            [[maybe_unused]] const wxString& userName = wxString{},
                            [[maybe_unused]] const wxString& passWord = wxString{});

    /** @brief Gets the content type of a webpage.
        @param[in,out] Url The webpage (may be altered if redirected).
        @param[out] responseCode The response code when connecting to the page.
        @param userName The username to connect to the page (not currently implemented).
        @param passWord The password to connect to the page (not currently implemented).
        @returns The MIME string of the page's content type.*/
    static wxString GetContentType(wxString& Url, long& responseCode,
                                   // these are not used yet, so just hide them
                                   [[maybe_unused]] const wxString& userName = wxString{},
                                   [[maybe_unused]] const wxString& passWord = wxString{});
    /// @returns The file type (possible an extension) from a MIME type string.
    /// @param contentType The MIME type string.
    [[nodiscard]]
    static wxString GetFileTypeFromContentType(const wxString& contentType);
    /// @returns A human readable message connected to response code.
    /// @param responseCode The response code from a webpage connect attempt.
    [[nodiscard]]
    static wxString GetResponseMessage(const int responseCode);
    /** @returns @c true if a link is pointing to an HTML page.
        @param[in,out] Url The link to test (may be redirected).
        @param[out] contentType The MIME type read from the page.*/
    [[nodiscard]]
    bool IsPageHtml(wxString& Url, wxString& contentType);
    /// @brief Sets the depth level to crawl from the base URL.
    /// @param levels The number of levels to crawl.
    void SetDepthLevel(const size_t levels) noexcept
        { m_levelDepth = levels; }
    /// @returns The depth level to crawl from the base URL.
    [[nodiscard]]
    size_t GetDepthLevel() const noexcept
        { return m_levelDepth; }
    /// @brief Adds a file type to download (based on extension).
    /// @param fileExtension The file extension to download.
    /// @note You can pass in the extension,
    ///     or a full filepath and it will get the extension from that.
    void AddFileTypeToDownload(const wxString& fileExtension)
        {
        if (fileExtension.find(L'.') == std::wstring::npos)
            { m_fileExtensions.insert(fileExtension); }
        else
            { m_fileExtensions.insert(wxFileName(fileExtension).GetExt()); }
        }
    void EnableMissingSequentialFileSearcing(const bool enable = true) noexcept
        { m_searchForMissingSequentialFiles = enable; }
    /// @brief When downloading locally, keep the folder structure from the website.
    /// @param keep True to use the website's folder structure, false to download files in
    ///  a flat folder structure.
    /// @note This is recommended to prevent overwriting files with the same name.
    void KeepWebPathWhenDownloading(const bool keep = true) noexcept
        { m_keepWebPathWhenDownloading = keep; }
    /// @returns Whether the website's folder structure is being
    ///     mirrored when downloading files.
    [[nodiscard]]
    bool IsKeepingWebPathWhenDownloading() const noexcept
        { return m_keepWebPathWhenDownloading; }
    /// @brief Specifies whether all HTML content should be downloaded,
    ///     regardless of the file's extension.
    /// @param harvestAll True to download all HTML content.
    /// @note This is recommended if needing to download PHP response
    ///     pages with non-standard file extensions.
    void HarvestAllHtmlFiles(const bool harvestAll = true) noexcept
        { m_harvestAllHtml = harvestAll; }
    /// @brief Sets whether to build a list of broken links while crawling.
    /// @param search True to catalogue broken links.
    /// @warning Enable this will degrade performance because it will need to
    ///  attempt connecting to each link.
    void SeachForBrokenLinks(const bool search = true) noexcept
        { m_searchForBrokenLinks = search; }
    /// @returns @c true if a list of broken links are being catalogued while harvesting.
    [[nodiscard]]
    bool IsSearchingForBrokenLinks() const noexcept
        { return m_searchForBrokenLinks; }
    /// @brief Sets the base URL to crawl.
    /// @param url The base URL.
    void SetUrl(const wxString& url)
        { m_url = url; }
    /// @returns The base URL being crawled.
    [[nodiscard]]
    const wxString& GetUrl() const noexcept
        { return m_url; }
    /// @brief Sets the root folder to download files from the web.
    /// @param downloadDirectory The local download folder.
    void SetDownloadDirectory(const wxString& downloadDirectory)
        { m_downloadDirectory = downloadDirectory; }
    /// @returns The load folder where web content is being downloaded.
    [[nodiscard]]
    const wxString& GetDownloadDirectory() const noexcept
        { return m_downloadDirectory; }
    /// @brief Specifies whether files being downloaded can overwrite
    ///     each other if they have the same path.
    /// @param replaceExistingFiles True to overwrite existing files.
    /// @note If this is set to false and a file with the same path
    ///     is about to be downloaded, the program will attempt to download
    ///     it with a different (but similar) name.
    void ReplaceExistingFiles(const bool replaceExistingFiles = true) noexcept
        { m_replaceExistingFiles = replaceExistingFiles; }
    /// @brief Sets whether to download files locally while the crawl.
    /// @param downloadWhileCrawling True to download the web content.
    void DownloadFilesWhileCrawling(const bool downloadWhileCrawling = true) noexcept
        { m_downloadWhileCrawling = downloadWhileCrawling; }
    /// @returns Whether files are being downloaded locally during the crawl.
    [[nodiscard]]
    bool IsDownloadingFilesWhileCrawling() const noexcept
        { return m_downloadWhileCrawling; }

    // Download file size constraints
    //----------------------------------

    /// @brief Sets the minimum file size to download.
    /// @param size The minimum file size (in kbs.) a file must be to download it.
    void SetMinimumDownloadFileSizeInKilobytes(const wxFileOffset size) noexcept
        {
        m_minFileDownloadSize = size*KILOBYTE;
        m_restrictFileMinDownloadSize = true;
        }
    /// @returns The minimum file size (in kbs.) a file must be in order to download it.
    [[nodiscard]]
    wxFileOffset GetMinimumDownloadFileSizeInKilobytes() const noexcept
        { return m_minFileDownloadSize/KILOBYTE; }
    /// @brief Sets the maximum file size to download.
    /// @param size The maximum file size (in kbs.) a file can be in order to download it.
    /// @note By default, there is no maximum filesize constraint (only minimum).
    void SetMaximumDownloadFileSizeInKilobytes(const wxFileOffset size) noexcept
        {
        m_maxFileDownloadSize = size*KILOBYTE;
        m_restrictFileMaxDownloadSize = true;
        }

    // Domain restriction
    //----------------------------------

    /** @brief Sets the domain restriction method.
        @param restriction The restriction method to use.*/
    void SetDomainRestriction(const DomainRestriction restriction) noexcept
        { m_domainRestriction = restriction; }
    /// @returns The domain-restriction method.
    [[nodiscard]]
    DomainRestriction GetDomainRestriction() const noexcept
        { return m_domainRestriction; }
    /// @brief Overrides the domain of the main webpage. Useful for only getting files
    ///  from an outside domain (or specific folder).
    /// @param domain The domain to restrict to.
    void SetRestrictedDomain(const wxString& domain)
        {
        html_utilities::html_url_format formatUrl(domain.wc_str());
        m_domain = formatUrl.get_root_domain().c_str();
        m_domainRestriction = DomainRestriction::RestrictToDomain;
        }
    /// @brief Resets the list of user-defined webpath restrictions.
    void ClearAllowableWebFolders() noexcept
        { m_allowableWebFolders.clear(); }
    /** @brief Adds a user-defined web path (domain/folder structure)
            to restrict harvesting to.
        @param domain A webpath to restrict harvesting to.*/
    void AddAllowableWebFolder(wxString domain)
        {
        if (domain.empty())
            { return;  }
        // if a full webpage, then it should have an extension on it and
        // html_url_format will remove the webpage. But if there is no
        // extension (or its junk), then add a trailing '/' to prevent
        // the last folder from being removed.
        const wxString webExt = wxFileName(domain).GetExt();
        if ((webExt.empty() || webExt.length() > 4) && !domain.ends_with(L"/"))
            { domain.append(L"/"); }
        html_utilities::html_url_format formatUrl(domain.wc_str());
        if (formatUrl.get_directory_path().length())
            { m_allowableWebFolders.emplace(formatUrl.get_directory_path().c_str()); }
        }
    /// @returns The user-defined web paths (domains, folder structure)
    ///     that harvesting is constrained to.
    [[nodiscard]] 
    wxArrayString GetAllowableWebFolders() const
        {
        wxArrayString domains;
        for (const auto& domain : m_allowableWebFolders)
            { domains.Add(domain.c_str()); }
        return domains;
        }

    //Authentication info
    void SetCredentials(const wxString& userName, const wxString& passWord)
        {
        m_userName = userName;
        m_passWord = passWord;
        }

    /// @returns The list of harvested links.
    const std::set<UrlWithNumericSequence>& GetHarvestedLinks() const noexcept
        { return m_harvestedLinks; }
    /// @returns The list of files downloaded.
    /// @note DownloadFilesWhileCrawling() must be enabled.
    const std::set<wxString>& GetDownloadedFilePaths() const noexcept
        { return m_downloadedFiles; }
    /// @returns A map of broken links and the respective pages they were found on.
    /// @note SeachForBrokenLinks() must be enabled.
    const std::map<wxString, wxString>& GetBrokenLinks() const noexcept
        { return m_brokenLinks; }

    /// @returns The user agent sent to websites when crawling.
    /// @note If the user agent is empty, then one will be built from the OS description.
    [[nodiscard]]
    static wxString GetUserAgent()
        {
        // May need to be set if not initialized.
        // Needs to be initialized here because wxGetOsDescription()
        // can't be called during global startup.
        if (m_userAgent.empty())
            { SetUserAgent(_DT(L"Web-Harvester/") + wxGetOsDescription()); }
        return m_userAgent;
        }
    /// @brief Sets the user agent sent to websites when crawling.
    /// @param agent The user agent string.
    static void SetUserAgent(const wxString& agent)
        { m_userAgent = agent; }

    [[nodiscard]]
    static wxString GetCharsetFromContentType(const wxString& contentType);
    [[nodiscard]]
    static wxString GetCharsetFromPageContent(const char* pageContent, const size_t length);
protected:
    [[nodiscard]]
    inline constexpr static bool IsBadResponseCode(const long responseCode) noexcept
        {
        return (responseCode == 204 ||
            responseCode == 400 ||
            responseCode == 401 ||
            responseCode == 402 ||
            responseCode == 403 ||
            responseCode == 404 ||
            responseCode == 500 ||
            responseCode == 501 ||
            responseCode == 502 ||
            responseCode == 503);
        }
    [[nodiscard]]
    bool VerifyUrlDomainCriteria(const wxString& url);
    bool HarvestLink(wxString& url, const wxString& referringUrl,
                     const wxString& fileExtension = wxString{});
    //----------------------------------
    bool CrawlLinks(wxString& url, const html_utilities::hyperlink_parse::hyperlink_parse_method method);
    // cppcheck-suppress constParameter
    void CrawlLink(const wxString& currentLink, html_utilities::html_url_format& formatUrl,
                   const wxString& mainUrl, const bool isImage);
    //----------------------------------
    void SearchForMissingFiles();
    void DownloadSequentialRange(const wxString& prefix, const wxString& suffix,
                             const size_t numericWidth, const wxString& referUrl,
                             const std::set<long>& alreadyDownloadedNumbers,
                             std::set<UrlWithNumericSequence>& newFilesToDownload);
    [[nodiscard]]
    bool HasUrlAlreadyBeenHarvested(const wxString& url) const
        { return (m_harvestedLinks.find(UrlWithNumericSequence(url,url)) != m_harvestedLinks.cend()); }
    [[nodiscard]]
    bool HasUrlAlreadyBeenCrawled(const wxString& url) const
        { return (m_alreadyCrawledFiles.find(url) != m_alreadyCrawledFiles.cend()); }
    [[nodiscard]]
    bool ShouldFileBeHarvested(const wxString& fileExt) const
        { return (m_fileExtensions.find(fileExt) != m_fileExtensions.cend()); }
private:
    class wxStringLessNoCase
        {
    public:
        [[nodiscard]]
        inline bool operator()(const wxString& first, const wxString& second) const
            { return (first.CmpNoCase(second) < 0); }
        };
    size_t m_currentLevel{ 0 };
    size_t m_levelDepth{ 1 };
    wxString m_url;
    static wxString m_userAgent;
    string_util::case_insensitive_wstring m_domain;
    string_util::case_insensitive_wstring m_fullDomain;
    string_util::case_insensitive_wstring m_fullDomainFolderPath;
    std::set<string_util::case_insensitive_wstring> m_allowableWebFolders;
    std::set<UrlWithNumericSequence> m_harvestedLinks;
    std::set<wxString> m_downloadedFiles;
    std::map<wxString, wxString> m_brokenLinks;
    std::set<wxString, wxStringLessWebPath> m_alreadyCrawledFiles;
    std::set<wxString, wxStringLessNoCase> m_fileExtensions;
    wxString m_downloadDirectory;
    bool m_keepWebPathWhenDownloading{ true };
    KnownRegularFileExtension IsKnownRegularFileExtension;
    KnownScriptFileExtension IsKnownScriptFileExtension;
    WebPageExtension IsWebPageExtension;
    // download criteria
    DomainRestriction m_domainRestriction{ DomainRestriction::NotRestricted };
    bool m_searchForMissingSequentialFiles{ false };
    bool m_downloadWhileCrawling{ false };
    static constexpr size_t KILOBYTE = 1024;
    static constexpr size_t MEGABYTE = 1024*1024;
    wxFileOffset m_minFileDownloadSize{ KILOBYTE*5 };
    wxFileOffset m_maxFileDownloadSize{ MEGABYTE*5 };
    bool m_restrictFileMinDownloadSize{ false };
    bool m_restrictFileMaxDownloadSize{ false };
    bool m_replaceExistingFiles{ true };
    bool m_harvestAllHtml{ false };
    bool m_searchForBrokenLinks{ false };
    // authentication info
    wxString m_userName;
    wxString m_passWord;
    // UI functionality
    wxProgressDialog* m_progressDlg{ nullptr };
    bool m_hideFileNamesWhileDownloading{ false };
    bool m_isCancelled{ false };

    static const wxString HTML_CONTENT_TYPE;
    static const wxString JAVASCRIPT_CONTENT_TYPE;
    static const wxString VBSCRIPT_CONTENT_TYPE;
    };

/** @}*/

#endif // __WEBHARVESTER_H__
