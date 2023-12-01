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
#include "../../webharvester/webharvester.h"
#include "../../Wisteria-Dataviz/src/ui/dialogs/dialogwithhelp.h"
#include "../../Wisteria-Dataviz/src/ui/controls/listctrlex.h"
#include "../../Wisteria-Dataviz/src/ui/controls/sidebarbook.h"

class WebHarvesterDlg final : public Wisteria::UI::DialogWithHelp
    {
public:
    /// @brief Constructor.
    WebHarvesterDlg(wxWindow* parent, const wxArrayString& urls, const int depthLevel,
             const wxString& fullDocFilter, const wxString& selectedDocFilter,
             const bool hideLocalDownloadOption,
             const bool downloadFilesLocally, const bool keepWebPathWhenDownloading,
             const wxString& downloadFolder,
             const int domainRestriction,
             const wxArrayString& domainStrings,
             wxWindowID id = wxID_ANY, const wxString& caption = _(L"Web Harvester"),
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER) :
        m_depthLevel(depthLevel), m_fullDocFilter(fullDocFilter),
        m_selectedDocFilter(selectedDocFilter),
        m_selectedDomainRestriction(domainRestriction),
        m_domains(domainStrings), m_hideLocalDownloadOption(hideLocalDownloadOption),
        m_downloadFilesLocally(downloadFilesLocally),
        m_keepWebPathWhenDownloading(keepWebPathWhenDownloading),
        m_urls(urls), m_downloadFolder(downloadFolder)
        { Create(parent, id, caption, pos, size, style); }
    /// @private
    ~WebHarvesterDlg()
        {
        wxDELETE(m_domainData);
        wxDELETE(m_urlData);
        }
    /// @private
    WebHarvesterDlg(WebHarvesterDlg&) = delete;
    /// @private
    WebHarvesterDlg& operator=(const WebHarvesterDlg&) = delete;
    [[nodiscard]]
    const wxArrayString& GetUrls() const noexcept
        { return m_urls; }
    [[nodiscard]]
    const wxString& GetSelectedDocFilter() const noexcept
        { return m_selectedDocFilter; }
    [[nodiscard]]
    int GetDepthLevel() const noexcept
        { return m_depthLevel; }
    [[nodiscard]]
    int GetDomainRestriction() const noexcept
        { return m_selectedDomainRestriction; }
    [[nodiscard]]
    const wxArrayString& GetRestrictedDomains() const noexcept
        { return m_domains; }
    void DownloadFilesLocally(const bool download) noexcept
        { m_downloadFilesLocally = download; }
    [[nodiscard]]
    bool IsDownloadFilesLocally() const noexcept
        { return m_downloadFilesLocally; }
    [[nodiscard]]
    bool IsRetainingWebsiteFolderStructure() const noexcept
        { return m_keepWebPathWhenDownloading; }
    [[nodiscard]]
    const wxString& GetDownloadFolder() const noexcept
        { return m_downloadFolder; }
    [[nodiscard]]
    const wxString& GetRawHtmlPage() const noexcept
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
    static constexpr int ID_DOWNLOAD_CHECKBOX = wxID_HIGHEST;
    static constexpr int ID_DOWNLOAD_FOLDER_BROWSE_BUTTON = wxID_HIGHEST + 1;
    static constexpr int ID_DOMAIN_COMBO = wxID_HIGHEST + 2;
    static constexpr int ID_ADD_URL_BUTTON = wxID_HIGHEST + 3;
    static constexpr int ID_DELETE_URL_BUTTON = wxID_HIGHEST + 4;
    static constexpr int ID_ADD_DOMAIN_BUTTON = wxID_HIGHEST + 5;
    static constexpr int ID_DELETE_DOMAIN_BUTTON = wxID_HIGHEST + 6;
    static constexpr int ID_HARVESTING_PAGE = wxID_HIGHEST + 7;
    static constexpr int ID_DOMAINS_PAGE = wxID_HIGHEST + 8;
    static constexpr int ID_DOWNLOAD_PAGE = wxID_HIGHEST + 9;

    [[nodiscard]]
    wxString GetUserSpecifiedDomainsLabel() const
        { return _(L"Restricted to user-defined domain(s)"); }
    /// Creation
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY,
                const wxString& caption = wxString{}, const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE);
    /// Creates the controls and sizers
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
    Wisteria::UI::SideBarBook* m_sideBarBook{ nullptr };
    wxComboBox* m_docFilterCombo{ nullptr };
    wxSpinCtrl* m_depthLevelCtrl{ nullptr };
    wxComboBox* m_domainCombo{ nullptr };
    ListCtrlEx* m_domainList{ nullptr };
    ListCtrlExDataProvider* m_domainData{ new ListCtrlExDataProvider };
    wxTextCtrl* m_localFolderEdit{ nullptr };
    wxStaticText* m_localFolderLabel{ nullptr };
    wxCheckBox* m_retainWebsiteFolderStuctureCheckBox{ nullptr };
    ListCtrlEx* m_urlList{ nullptr };
    wxBitmapButton* m_addDomainButton{ nullptr };
    wxBitmapButton* m_deleteDomainButton{ nullptr };

    wxArrayString m_urls;
    ListCtrlExDataProvider* m_urlData{ new ListCtrlExDataProvider };
    wxString m_rawHtmlPage;

    wxString m_downloadFolder;
    };

/** @}*/

#endif // __WEB_HARVESTER_DIALOG_H__
