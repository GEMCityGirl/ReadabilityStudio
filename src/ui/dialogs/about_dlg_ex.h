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

#include "../../../../Licensing/LicenseAdmin.h"
#include "../../Wisteria-Dataviz/src/ui/controls/listctrlex.h"
#include "../../Wisteria-Dataviz/src/ui/controls/sidebarbook.h"
#include <wx/bitmap.h>
#include <wx/dialog.h>
#include <wx/hyperlink.h>
#include <wx/image.h>
#include <wx/html/htmlwin.h>
#include <wx/notebook.h>
#include <wx/statline.h>
#include <wx/utils.h>
#include <wx/wx.h>

/// @brief Enhanced About dialog for an application.
class AboutDialogEx final : public wxDialog
    {
  public:
    /** @brief Constructor.
        @param parent The dialog's parent.
        @param logo The application's logo.
        @param AppVersion The application's version.
        @param copyright The copyright string.
        @param eula The end-user license agreement content.
        @param licenseAdmin The license administrator interface.
        @param id The dialog's ID.
        @param pos The dialog's position.
        @param size The dialog's size.
        @param style The dialog's style.*/
    AboutDialogEx(wxWindow* parent, const wxBitmap& logo, wxString appVersion,
                  wxString copyright, LicenseAdmin* licenseAdmin,
                  wxString eula, wxWindowID id = wxID_ANY,
                  const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                  long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    /// @private
    AboutDialogEx(const AboutDialogEx& that) = delete;
    /// @private
    AboutDialogEx& operator=(const AboutDialogEx& that) = delete;

    /// @returns The sidebar.
    [[nodiscard]]
    Wisteria::UI::SideBar* GetSideBar() noexcept
        {
        return m_sideBarBook->GetSideBar();
        }

  private:
    // page and button IDs
    constexpr static int ID_VERSION_PAGE = wxID_HIGHEST;
    constexpr static int ID_LICENSING_PAGE = wxID_HIGHEST + 1;
    constexpr static int ID_EULA_PAGE = wxID_HIGHEST + 2;
    constexpr static int ID_UPDATE_LICENSE = wxID_HIGHEST + 3;

    /// Creation.
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

    /// Creates the controls and sizers.
    void CreateControls();

    void OnUpdateLicense([[maybe_unused]] wxCommandEvent& event);
    void FillLicenseGrid();

    LicenseAdmin* m_licenseAdmin{ nullptr };
    Wisteria::UI::ListCtrlEx* m_licenseGrid{ nullptr };
    Wisteria::UI::SideBarBook* m_sideBarBook{ nullptr };

    wxBitmap m_logo;
    wxString m_appVersion;
    wxString m_serialNumber;
    wxString m_copyright;
    wxString m_eula;
    };

    /** @}*/

#endif //__ABOUT_DIALOG_EX_H__
