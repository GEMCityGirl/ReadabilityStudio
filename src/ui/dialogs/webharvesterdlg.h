/** @addtogroup UI
    @brief Classes for the user interface.
    @date 2005-2023
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __WEB_HARVESTER_DIALOG_H__
#define __WEB_HARVESTER_DIALOG_H__

#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/valgen.h>
#include <wx/spinctrl.h>
#include <wx/combobox.h>
#include <wx/arrstr.h>
#include <wx/collpane.h>
#include <wx/tooltip.h>
#include <wx/bmpbuttn.h>
#include "webharvester.h"
#include "../../../../SRC/Wisteria-Dataviz/src/ui/dialogs/dialogwithhelp.h"
#include "../../../../SRC/Wisteria-Dataviz/src/ui/controls/listctrlex.h"
#include "../../../../SRC/Wisteria-Dataviz/src/ui/controls/sidebarbook.h"

class WebHarvesterDlg final : public Wisteria::UI::DialogWithHelp
    {
public:
    /// @brief Constructor.
    WebHarvesterDlg(wxWindow* parent, const wxArrayString& urls, const int depthLevel,
             const wxString& fullDocFilter, const wxString& selectedDocFilter,
             const bool hideLocalDownloadOption,
             const bool downloadFilesLocally, const bool keepWebPathWhenDownloading,
             const wxString& downloadFolder, const wxString& userAgent,
             const int domainRestriction,
             const wxArrayString& domainStrings, const int minFileSizeInKiloBytes,
             wxWindowID id = wxID_ANY, const wxString& caption = _("Web Harvester"),
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER) :
        m_depthLevel(depthLevel), m_fullDocFilter(fullDocFilter), m_selectedDocFilter(selectedDocFilter),
        m_selectedDomainRestriction(domainRestriction),
        m_domains(domainStrings), m_hideLocalDownloadOption(hideLocalDownloadOption),
        m_downloadFilesLocally(downloadFilesLocally), m_keepWebPathWhenDownloading(keepWebPathWhenDownloading),
        m_minFileSizeInKiloBytes(minFileSizeInKiloBytes),
        m_sideBarBook(nullptr),
        m_docFilterCombo(nullptr), m_depthLevelCtrl(nullptr), m_domainCombo(nullptr), m_domainList(nullptr),
        m_minFileSizeCtrl(nullptr), m_domainData(new ListCtrlExDataProvider), m_localFolderEdit(nullptr),
        m_localFolderLabel(nullptr), m_minFileSizeLabel(nullptr), m_retainWebsiteFolderStuctureCheckBox(nullptr),
        m_verifyPageMimeTypeCheckBox(nullptr),
        m_urlList(nullptr), m_addDomainButton(nullptr), m_deleteDomainButton(nullptr),
        m_downloadFolder(downloadFolder), m_userAgent(userAgent), m_urls(urls),
        m_urlData(new ListCtrlExDataProvider)
        { Create(parent, id, caption, pos, size, style); }
    /// @brief Destructor.
    ~WebHarvesterDlg()
        {
        wxDELETE(m_domainData);
        wxDELETE(m_urlData);
        }
    WebHarvesterDlg(WebHarvesterDlg&) = delete;
    WebHarvesterDlg& operator=(const WebHarvesterDlg&) = delete;
    [[nodiscard]] const wxArrayString& GetUrls() const noexcept
        { return m_urls; }
    [[nodiscard]] const wxString& GetSelectedDocFilter() const noexcept
        { return m_selectedDocFilter; }
    [[nodiscard]] int GetDepthLevel() const noexcept
        { return m_depthLevel; }
    [[nodiscard]] int GetDomainRestriction() const noexcept
        { return m_selectedDomainRestriction; }
    [[nodiscard]] const wxArrayString& GetRestrictedDomains() const noexcept
        { return m_domains; }
    void DownloadFilesLocally(const bool download) noexcept
        { m_downloadFilesLocally = download; }
    [[nodiscard]] bool IsDownloadFilesLocally() const noexcept
        { return m_downloadFilesLocally; }
    [[nodiscard]] bool IsRetainingWebsiteFolderStructure() const noexcept
        { return m_keepWebPathWhenDownloading; }
    [[nodiscard]] size_t GetMinimumDownloadFileSizeInKilobytes() const noexcept
        { return m_minFileSizeInKiloBytes; }
    [[nodiscard]] const wxString& GetDownloadFolder() const noexcept
        { return m_downloadFolder; }
    [[nodiscard]] const wxString& GetUserAgent() const noexcept
        { return m_userAgent; }
    [[nodiscard]] const wxString& GetRawHtmlPage() const noexcept
        { return m_rawHtmlPage; }
    /// Updates the dialog from a harvester's settings,
    void UpdateFromHarvesterSettings(const WebHarvester& harvester);
    /// Sets a harvest object to use the settings from this dialog.
    void UpdateHarvesterSettings(WebHarvester& harvester);
    void OnOK([[maybe_unused]] wxCommandEvent& event);
    void OnDownloadCheck([[maybe_unused]] wxCommandEvent& event);
    void OnFolderBrowseButtonClick([[maybe_unused]] wxCommandEvent& event);
    void OnDomainComboSelect([[maybe_unused]] wxCommandEvent& event);
    void OnAddUrlClick([[maybe_unused]] wxCommandEvent& event);
    void OnDeleteUrlClick([[maybe_unused]] wxCommandEvent& event);
    void OnAddDomainClick([[maybe_unused]] wxCommandEvent& event);
    void OnDeleteDomainClick([[maybe_unused]] wxCommandEvent& event);
private:
    static constexpr int ID_DOWNLOAD_CHECKBOX = 10001;
    static constexpr int ID_DOWNLOAD_FOLDER_BROWSE_BUTTON = 10002;
    static constexpr int ID_DOMAIN_COMBO = 10003;
    static constexpr int ID_ADD_URL_BUTTON = 10004;
    static constexpr int ID_DELETE_URL_BUTTON = 10005;
    static constexpr int ID_ADD_DOMAIN_BUTTON = 10006;
    static constexpr int ID_DELETE_DOMAIN_BUTTON = 10007;
    static constexpr int ID_HARVESTING_PAGE = 10008;
    static constexpr int ID_DOMAINS_PAGE = 10009;
    static constexpr int ID_DOWNLOAD_PAGE = 10010;

    [[nodiscard]] wxString GetUserSpecifiedDomainsLabel() const
        { return _("Restricted to user-defined domain(s)"); }
    ///Creation
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& caption = wxEmptyString, const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE);
    ///Creates the controls and sizers
    void CreateControls();
    int m_depthLevel{ 1 };
    wxString m_fullDocFilter;
    wxString m_selectedDocFilter;
    int m_selectedDomainRestriction{ 0 };
    wxArrayString m_domains;
    bool m_hideLocalDownloadOption{ false };
    bool m_downloadFilesLocally{ true };
    bool m_keepWebPathWhenDownloading{ true };
    bool m_logBrokenLinks{ false };
    size_t m_minFileSizeInKiloBytes{ 5 };
    Wisteria::UI::SideBarBook* m_sideBarBook{ nullptr };
    wxComboBox* m_docFilterCombo{ nullptr };
    wxSpinCtrl* m_depthLevelCtrl{ nullptr };
    wxComboBox* m_domainCombo{ nullptr };
    ListCtrlEx* m_domainList{ nullptr };
    wxSpinCtrl* m_minFileSizeCtrl{ nullptr };
    ListCtrlExDataProvider* m_domainData{ nullptr };
    wxTextCtrl* m_localFolderEdit{ nullptr };
    wxStaticText* m_localFolderLabel{ nullptr };
    wxStaticText* m_minFileSizeLabel{ nullptr };
    wxCheckBox* m_retainWebsiteFolderStuctureCheckBox{ nullptr };
    wxCheckBox* m_verifyPageMimeTypeCheckBox{ nullptr };
    ListCtrlEx* m_urlList{ nullptr };
    wxBitmapButton* m_addDomainButton{ nullptr };
    wxBitmapButton* m_deleteDomainButton{ nullptr };

    wxArrayString m_urls;
    ListCtrlExDataProvider* m_urlData{ nullptr };
    wxString m_rawHtmlPage;

    wxString m_downloadFolder;
    wxString m_userAgent;

    wxDECLARE_EVENT_TABLE();
    };

#endif // __WEB_HARVESTER_DIALOG_H__
