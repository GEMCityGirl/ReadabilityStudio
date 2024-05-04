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

#include "../../Wisteria-Dataviz/src/ui/controls/listctrlex.h"
#include "../../Wisteria-Dataviz/src/ui/controls/sidebarbook.h"
#include "../../Wisteria-Dataviz/src/ui/dialogs/dialogwithhelp.h"
#include "../../webharvester/webharvester.h"
#include <wx/arrstr.h>
#include <wx/bmpbuttn.h>
#include <wx/collpane.h>
#include <wx/combobox.h>
#include <wx/dialog.h>
#include <wx/spinctrl.h>
#include <wx/tooltip.h>
#include <wx/valgen.h>
#include <wx/wx.h>

class WebHarvesterDlg final : public Wisteria::UI::DialogWithHelp
    {
  public:
    /// @brief Constructor.
    /// @note Call UpdateFromHarvesterSettings() to fill in the remaining settings
    ///     from an existing WebHarvester object.
    WebHarvesterDlg(wxWindow* parent, wxArrayString urls, const wxString& fullDocFilter,
                    const wxString& selectedDocFilter, const bool hideLocalDownloadOption,
                    wxWindowID id = wxID_ANY, const wxString& caption = _(L"Web Harvester"),
                    const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                    long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
        : m_fullDocFilter(fullDocFilter), m_selectedDocFilter(selectedDocFilter),
          m_hideLocalDownloadOption(hideLocalDownloadOption), m_urls(std::move(urls))
        {
        Create(parent, id, caption, pos, size, style);
        }

    /// @private
    WebHarvesterDlg(WebHarvesterDlg&) = delete;
    /// @private
    WebHarvesterDlg& operator=(const WebHarvesterDlg&) = delete;

    [[nodiscard]]
    const wxArrayString& GetUrls() const noexcept
        {
        return m_urls;
        }

    [[nodiscard]]
    const wxString& GetSelectedDocFilter() const noexcept
        {
        return m_selectedDocFilter;
        }

    [[nodiscard]]
    int GetDepthLevel() const noexcept
        {
        return m_depthLevel;
        }

    [[nodiscard]]
    wxString GetUserAgent() const
        {
        return m_userAgent;
        }

    /// @returns Returns @c true if peer verification has been disabled.
    [[nodiscard]]
    bool IsPeerVerifyDisabled() const noexcept
        {
        return m_disablePeerVerify;
        }

    /// @returns Whether cookies should be extracted from JS code and sent back to the
    ///     server when connected to them.
    [[nodiscard]]
    bool IsUsingJavaScriptCookies() const noexcept
        {
        return m_useJsCookies;
        }

    [[nodiscard]]
    int GetDomainRestriction() const noexcept
        {
        return m_selectedDomainRestriction;
        }

    [[nodiscard]]
    const wxArrayString& GetRestrictedDomains() const noexcept
        {
        return m_domains;
        }

    void DownloadFilesLocally(const bool download) noexcept
        {
        m_downloadFilesLocally = download;
        TransferDataToWindow();
        }

    [[nodiscard]]
    bool IsDownloadFilesLocally() const noexcept
        {
        return m_downloadFilesLocally;
        }

    [[nodiscard]]
    bool IsRetainingWebsiteFolderStructure() const noexcept
        {
        return m_keepWebPathWhenDownloading;
        }

    [[nodiscard]]
    bool IsReplacingExistingFiles() const noexcept
        {
        return m_replaceExistingFiles;
        }

    [[nodiscard]]
    const wxString& GetDownloadFolder() const noexcept
        {
        return m_downloadFolder;
        }

    void SetMinimumDownloadFileSizeInKilobytes(const int minKbs) noexcept
        {
        m_minFileSizeInKiloBytes = minKbs;
        TransferDataToWindow();
        }

    [[nodiscard]]
    int GetMinimumDownloadFileSizeInKilobytes() const noexcept
        {
        return m_minFileSizeInKiloBytes;
        }

    /// Updates the dialog from a harvester's settings.
    void UpdateFromHarvesterSettings(const WebHarvester& harvester);
    /// Sets a harvest object to use the settings from this dialog.
    void UpdateHarvesterSettings(WebHarvester& harvester);

  private:
    constexpr static int ID_DOWNLOAD_CHECKBOX = wxID_HIGHEST;
    constexpr static int ID_DOWNLOAD_FOLDER_BROWSE_BUTTON = wxID_HIGHEST + 1;
    constexpr static int ID_DOMAIN_COMBO = wxID_HIGHEST + 2;
    constexpr static int ID_ADD_URL_BUTTON = wxID_HIGHEST + 3;
    constexpr static int ID_DELETE_URL_BUTTON = wxID_HIGHEST + 4;
    constexpr static int ID_ADD_DOMAIN_BUTTON = wxID_HIGHEST + 5;
    constexpr static int ID_DELETE_DOMAIN_BUTTON = wxID_HIGHEST + 6;
    constexpr static int ID_HARVESTING_PAGE = wxID_HIGHEST + 7;
    constexpr static int ID_DOMAINS_PAGE = wxID_HIGHEST + 8;
    constexpr static int ID_DOWNLOAD_PAGE = wxID_HIGHEST + 9;
    constexpr static int ID_LOAD_URLS_BUTTON = wxID_HIGHEST + 10;

    void OnOK([[maybe_unused]] wxCommandEvent& event);
    void OnDownloadCheck([[maybe_unused]] wxCommandEvent& event);
    void OnFolderBrowseButtonClick([[maybe_unused]] wxCommandEvent& event);
    void OnDomainComboSelect([[maybe_unused]] wxCommandEvent& event);
    void OnAddUrlClick([[maybe_unused]] wxCommandEvent& event);
    void OnDeleteUrlClick([[maybe_unused]] wxCommandEvent& event);
    void OnAddDomainClick([[maybe_unused]] wxCommandEvent& event);
    void OnLoadUrlsClick([[maybe_unused]] wxCommandEvent& event);
    void OnDeleteDomainClick([[maybe_unused]] wxCommandEvent& event);

    [[nodiscard]]
    wxString GetUserSpecifiedDomainsLabel() const
        {
        return _(L"Restricted to user-defined domain(s)");
        }

    /// Creation
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& caption = wxString{},
                const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_DIALOG_STYLE);
    /// Creates the controls and sizers
    void CreateControls();
    int m_depthLevel{ 1 };
    wxString m_userAgent;
    bool m_disablePeerVerify{ false };
    bool m_useJsCookies{ false };
    wxString m_fullDocFilter;
    wxString m_selectedDocFilter;
    int m_selectedDomainRestriction{ 0 };
    wxArrayString m_domains;
    bool m_hideLocalDownloadOption{ false };
    bool m_downloadFilesLocally{ true };
    bool m_keepWebPathWhenDownloading{ true };
    bool m_replaceExistingFiles{ true };
    bool m_logBrokenLinks{ false };
    int m_minFileSizeInKiloBytes{ 5 };
    Wisteria::UI::SideBarBook* m_sideBarBook{ nullptr };
    wxComboBox* m_docFilterCombo{ nullptr };
    wxSpinCtrl* m_depthLevelCtrl{ nullptr };
    wxComboBox* m_domainCombo{ nullptr };
    Wisteria::UI::ListCtrlEx* m_domainList{ nullptr };
    std::shared_ptr<Wisteria::UI::ListCtrlExDataProvider> m_domainData{
        std::make_shared<Wisteria::UI::ListCtrlExDataProvider>()
    };
    wxTextCtrl* m_localFolderEdit{ nullptr };
    wxStaticText* m_localFolderLabel{ nullptr };
    wxCheckBox* m_retainWebsiteFolderStuctureCheckBox{ nullptr };
    wxCheckBox* m_replaceExistingFilesCheckBox{ nullptr };
    Wisteria::UI::ListCtrlEx* m_urlList{ nullptr };
    wxBitmapButton* m_addDomainButton{ nullptr };
    wxBitmapButton* m_deleteDomainButton{ nullptr };
    wxStaticText* m_minFileSizeLabel{ nullptr };
    wxSpinCtrl* m_minFileSizeCtrl{ nullptr };
    wxBitmapButton* m_folderBrowseButton{ nullptr };

    wxArrayString m_urls;
    std::shared_ptr<Wisteria::UI::ListCtrlExDataProvider> m_urlData{
        std::make_shared<Wisteria::UI::ListCtrlExDataProvider>()
    };

    wxString m_downloadFolder;
    };

    /** @}*/

#endif // __WEB_HARVESTER_DIALOG_H__
