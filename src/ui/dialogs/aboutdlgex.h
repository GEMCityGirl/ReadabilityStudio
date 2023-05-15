/** @addtogroup UI
    @brief Classes for the user interface.
    @date 2005-2023
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __ABOUT_DIALOG_EX_H__
#define __ABOUT_DIALOG_EX_H__

#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/dialog.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/statline.h>
#include <wx/hyperlink.h>
#include <wx/utils.h>
#include "../../Wisteria-Dataviz/src/ui/controls/listctrlex.h"
#include "../../Wisteria-Dataviz/src/ui/controls/sidebarbook.h"
#include "../../../../SRC/Licensing/LicenseAdmin.h"

/// @brief Enhanced About dialog for an application.
class AboutDialogEx final : public wxDialog
    {
public:
    /** @brief Constructor.
        @param parent The dialog's parent.
        @param logo The application's logo.
        @param AppVersion The application's version.
        @param copyright The copyright string.
        @param licenseAdmin The license administator interface.
        @param id The dialog's ID.
        @param pos The dialog's position.
        @param size The dialog's size.
        @param style The dialog's style.*/
    AboutDialogEx(wxWindow* parent, const wxBitmap& logo,
                  const wxString& AppVersion,
                  const wxString& copyright,
                  LicenseAdmin* licenseAdmin,
                  wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);
    /// @private
    AboutDialogEx(const AboutDialogEx& that) = delete;
    /// @private
    AboutDialogEx& operator=(const AboutDialogEx& that) = delete;
    /// @returns The sidebar.
    [[nodiscard]]
    Wisteria::UI::SideBar* GetSideBar() noexcept
        { return m_sideBarBook->GetSideBar(); }
private:
    // page and button IDs
    static constexpr int ID_VERSION_PAGE = wxID_HIGHEST;
    static constexpr int ID_LICENSING_PAGE = wxID_HIGHEST + 1;
    static constexpr int ID_UPDATE_LICENSE = wxID_HIGHEST + 2;

    /// Creation.
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);

    /// Creates the controls and sizers.
    void CreateControls();

    void OnUpdateLicense([[maybe_unused]] wxCommandEvent& event);
    void FillLicenseGrid();

    LicenseAdmin* m_licenseAdmin{ nullptr };
    ListCtrlEx* m_licenseGrid{ nullptr };
    Wisteria::UI::SideBarBook* m_sideBarBook{ nullptr };

    wxBitmap m_logo;
    wxString m_appVersion;
    wxString m_serialNumber;
    wxString m_copyright;
    };

/** @}*/

#endif //__ABOUT_DIALOG_EX_H__
