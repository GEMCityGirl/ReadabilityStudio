///////////////////////////////////////////////////////////////////////////////
// Name:        aboutdlg.cpp
// Author:      Blake Madden
// Copyright:   (c) 2005-2023 Blake Madden
// Licence:     3-Clause BSD licence
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "aboutdlgex.h"
#include <wx/generic/statbmpg.h>

//------------------------------------------------------
void AboutDialogEx::OnUpdateLicense([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();

    if (!m_licenseAdmin->LoadSerialNumber(m_serialNumber))
        {
        wxMessageBox(_(L"Unable to load serial number."),
            _(L"Error"), wxOK|wxICON_EXCLAMATION, this);
        return;
        }
    // save the licensing info now that we have it and have validated it
    if (!m_licenseAdmin->SaveLicenseFile(m_licenseAdmin->GetLicenseFilePath()))
        {
        wxMessageBox(
            _(L"Unable to save license file. "
              "You must have Administrator privileges to update your license file."),
            _(L"Error"), wxOK|wxICON_EXCLAMATION, this);
        return;
        }
    // let's try it again from the top
    if (!m_licenseAdmin->LoadLicenseFile(m_licenseAdmin->GetLicenseFilePath(),
                                         wxTheApp->GetAppName()))
        {
        wxMessageBox(
            _(L"Unable to open updated license file. Please close and reopen the application."),
            _(L"Error"), wxOK|wxICON_EXCLAMATION, this);
        return;
        }
    FillLicenseGrid();
    wxMessageBox(
            _(L"License Update Complete. Please close and reopen the application for all "
              "changes to take effect."),
            _(L"License Update"), wxOK|wxICON_INFORMATION, this);
    wxLogMessage(L"License Updated.");
    }

//------------------------------------------------------
void AboutDialogEx::FillLicenseGrid()
    {
    if (!m_licenseGrid)
        { return; }
    wxWindowUpdateLocker noUpdates(m_licenseGrid);
    m_licenseGrid->DeleteAllItems();
    // add the feature info to the grid
    for (size_t i = 0; i < m_licenseAdmin->GetFeatureCount(); ++i)
        {
        const LicenseFeature feature = m_licenseAdmin->GetFeatureInfo(i);
        /* if the signature isn't valid then the license file has been tampered with,
           so just ignore this feature*/
        if (!feature.IsSignatureValid() )
            { continue; }
        long insertedIndex = m_licenseGrid->InsertItem(m_licenseGrid->GetItemCount(),
            (feature.GetDescription() != wxEmptyString) ?
                feature.GetDescription() : feature.GetFeatureCode());
        if (!feature.IsEnabled() )
            {
            m_licenseGrid->SetItem(insertedIndex, 1, _(L"Not enabled"));
            m_licenseGrid->SetItem(insertedIndex, 2, _(L"N/A"));
            }
        else if (feature.IsPermanent() )
            {
            m_licenseGrid->SetItem(insertedIndex, 1, _(L"Never expires"));
            m_licenseGrid->SetItem(insertedIndex, 2, _(L"N/A"));
            }
        else if (feature.IsExpirationDateValid())
            {
            m_licenseGrid->SetItem(insertedIndex, 1,
                feature.GetExpirationDateInfo().GetDateFormattedString());
            if (feature.IsExpired())
                { m_licenseGrid->SetItem(insertedIndex, 2, _(L"Expired")); }
            else
                {
                m_licenseGrid->SetItem(insertedIndex, 2,
                    std::to_wstring(feature.GetExpirationDateInfo().
                                    GetDaysRemainingUntilExpiration()));
                }
            }
        // this should not happen unless license file was tampered with
        else
            {
            m_licenseGrid->SetItem(insertedIndex, 1, _(L"Error loading date"));
            m_licenseGrid->SetItem(insertedIndex, 2, _(L"Disabled"));
            }
        }
    }

//------------------------------------------------------
AboutDialogEx::AboutDialogEx(wxWindow* parent, const wxBitmap& logo,
                   const wxString& AppVersion, const wxString& copyright,
                   LicenseAdmin* licenseAdmin, wxWindowID id,
                   const wxPoint& pos, const wxSize& size, long style) :
                   m_licenseAdmin(licenseAdmin),
                   m_logo(logo), m_appVersion(AppVersion), m_copyright(copyright)
    {
    m_serialNumber = m_licenseAdmin->GetSerialNumber();
    Create(parent, id, pos, size, style);
    Bind(wxEVT_BUTTON, &AboutDialogEx::OnUpdateLicense, this, AboutDialogEx::ID_UPDATE_LICENSE);
    }

//------------------------------------------------------
bool AboutDialogEx::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos,
                           const wxSize& size, long style)
    {
    wxDialog::Create(parent, id, wxString::Format(_(L"About %s"),
                     wxTheApp->GetAppName()), pos, size, style);

    CreateControls();
    Centre();

    return true;
    }

//------------------------------------------------------
void AboutDialogEx::CreateControls()
    {
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    m_sideBarBook = new Wisteria::UI::SideBarBook(this, wxID_ANY);
    mainSizer->Add(m_sideBarBook, 1, wxEXPAND|wxALL, wxSizerFlags::GetDefaultBorder());

    // version info page
        {
        wxPanel* mainPage =
            new wxPanel(m_sideBarBook, ID_VERSION_PAGE,
                        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
        wxBoxSizer* mainPanelSizer = new wxBoxSizer(wxVERTICAL);
        mainPage->SetSizer(mainPanelSizer);
        m_sideBarBook->AddPage(mainPage, _(L"Product Info"), ID_VERSION_PAGE, true);

        if (m_logo.IsOk())
            {
            wxGenericStaticBitmap* logoBox =
                new wxGenericStaticBitmap(mainPage, wxID_ANY, m_logo,
                    wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER);
            mainPanelSizer->Add(logoBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL,
                wxSizerFlags::GetDefaultBorder());
            }

        auto productInfoGrid = new wxFlexGridSizer(2, wxSize(wxSizerFlags::GetDefaultBorder(), 0));
        productInfoGrid->Add(new wxStaticText(mainPage, wxID_ANY, _(L"Version:")));
        productInfoGrid->Add(new wxStaticText(mainPage, wxID_ANY, m_appVersion));
    #ifndef NDEBUG
        productInfoGrid->Add(new wxStaticText(mainPage, wxID_ANY, _(L"Build:")));
        productInfoGrid->Add(new wxStaticText(mainPage, wxID_ANY, _(L"Debug")));
    #endif
        productInfoGrid->Add(new wxStaticText(mainPage, wxID_ANY, _(L"Built on:")));
        wxDateTime buildDate;
        buildDate.ParseDate(__DATE__);
        productInfoGrid->Add(new wxStaticText(mainPage, wxID_ANY, buildDate.Format(L"%B %d, %G")));
        productInfoGrid->Add(new wxStaticText(mainPage, wxID_ANY, _(L"Platform:")));
        productInfoGrid->Add(new wxStaticText(mainPage, wxID_ANY, wxGetOsDescription()));

        // put it all together
        mainPanelSizer->Add(new wxStaticText(mainPage, wxID_ANY, wxTheApp->GetAppName()), 0,
                                             wxALIGN_LEFT|wxLEFT, wxSizerFlags::GetDefaultBorder());
        mainPanelSizer->Add(productInfoGrid, 0, wxALIGN_LEFT|wxLEFT, wxSizerFlags::GetDefaultBorder());
        mainPanelSizer->Add(new wxStaticText(mainPage, wxID_ANY, m_copyright), 0,
                                             wxALIGN_LEFT|wxLEFT, wxSizerFlags::GetDefaultBorder());
        }

    // licensing page
        {
        wxPanel* mainPage = new wxPanel(m_sideBarBook, ID_LICENSING_PAGE, wxDefaultPosition,
                                        wxDefaultSize, wxTAB_TRAVERSAL);
        wxBoxSizer* mainPanelSizer = new wxBoxSizer(wxVERTICAL);
        mainPage->SetSizer(mainPanelSizer);
        m_sideBarBook->AddPage(mainPage, _(L"Licensing"), ID_LICENSING_PAGE, false);

        wxBoxSizer* updateSerialNumberSizer = new wxBoxSizer(wxHORIZONTAL);
        updateSerialNumberSizer->Add(new wxStaticText(mainPage, wxID_ANY, _(L"Serial #:")), 0,
            wxALIGN_LEFT | wxALIGN_CENTRE_VERTICAL | wxRIGHT, wxSizerFlags::GetDefaultBorder());
        updateSerialNumberSizer->Add(new wxTextCtrl(mainPage, wxID_ANY, wxEmptyString,
            wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_serialNumber)), 1, wxEXPAND);
        updateSerialNumberSizer->Add(new wxButton(mainPage, ID_UPDATE_LICENSE, _(L"Update")));
        mainPanelSizer->Add(updateSerialNumberSizer, wxSizerFlags().Expand().Border(wxLEFT|wxRIGHT));

        // user and license info
        auto userInfoGrid = new wxFlexGridSizer(2, wxSize(wxSizerFlags::GetDefaultBorder(),0));
        if (m_licenseAdmin->GetUserName().length())
            {
            userInfoGrid->Add(new wxStaticText(mainPage, wxID_ANY, _(L"User Name:")));
            userInfoGrid->Add(new wxStaticText(mainPage, wxID_ANY, m_licenseAdmin->GetUserName()));
            }
        if (m_licenseAdmin->GetUserOrganization().length())
            {
            userInfoGrid->Add(new wxStaticText(mainPage, wxID_ANY, _(L"Company:")));
            userInfoGrid->Add(new wxStaticText(mainPage, wxID_ANY,
                                    m_licenseAdmin->GetUserOrganization()));
            }
        userInfoGrid->Add(new wxStaticText(mainPage, wxID_ANY, _(L"User ID:")));
        userInfoGrid->Add(new wxStaticText(mainPage, wxID_ANY, m_licenseAdmin->GetPublicKey()));
        userInfoGrid->Add(new wxStaticText(mainPage, wxID_ANY, _(L"License:")));
        userInfoGrid->Add(new wxStaticText(mainPage, wxID_ANY,
                                    m_licenseAdmin->GetProductDescription()));
        mainPanelSizer->Add(userInfoGrid, wxSizerFlags().Border(wxALL).Align(wxALIGN_LEFT));

        // license grid
        m_licenseGrid = new ListCtrlEx(mainPage, wxID_ANY, wxDefaultPosition,
            wxDefaultSize, wxLC_SINGLE_SEL | wxLC_REPORT | wxBORDER_SUNKEN);
        m_licenseGrid->EnableGridLines();
        m_licenseGrid->InsertColumn(0, _(L"Product Feature"), wxLIST_FORMAT_LEFT,
                                    wxLIST_AUTOSIZE_USEHEADER);
        m_licenseGrid->InsertColumn(1, _(L"Expiration Date"), wxLIST_FORMAT_LEFT,
                                    wxLIST_AUTOSIZE_USEHEADER);
        m_licenseGrid->InsertColumn(2, _(L"Days Remaining"), wxLIST_FORMAT_LEFT,
                                    wxLIST_AUTOSIZE_USEHEADER);
        FillLicenseGrid();
        mainPanelSizer->Add(m_licenseGrid, 1, wxALIGN_LEFT | wxEXPAND | wxALL,
                            wxSizerFlags::GetDefaultBorder());
        m_licenseGrid->SetColumnWidth(0, m_licenseGrid->EstimateColumnWidth(0));
        m_licenseGrid->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER);
        m_licenseGrid->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);
        }

    mainSizer->Add(CreateSeparatedButtonSizer(wxCLOSE), 0, wxALL|wxEXPAND,
                   wxSizerFlags::GetDefaultBorder());

    SetSizerAndFit(mainSizer);
    }
