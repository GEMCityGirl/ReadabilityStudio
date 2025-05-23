/********************************************************************************
 * Copyright (c) 2005-2025 Blake Madden
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * https://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   Blake Madden - initial implementation
 ********************************************************************************/

#include "about_dlg_ex.h"
#include <wx/generic/statbmpg.h>

//------------------------------------------------------
AboutDialogEx::AboutDialogEx(wxWindow* parent, const wxBitmap& logo, wxString appVersion,
                             wxString copyright, wxString eula, wxWindowID id, const wxPoint& pos,
                             const wxSize& size, long style)
    : m_logo(logo), m_appVersion(std::move(appVersion)), m_copyright(std::move(copyright)),
      m_eula(std::move(eula))
    {
    Create(parent, id, pos, size, style);
    }

//------------------------------------------------------
bool AboutDialogEx::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
                           long style)
    {
    wxDialog::Create(parent, id,
                     wxString::Format(/* TRANSLATORS: %s is the application name */ _(L"About %s"),
                                      wxTheApp->GetAppName()),
                     pos, size, style);

    CreateControls();
    Centre();

    return true;
    }

//------------------------------------------------------
void AboutDialogEx::CreateControls()
    {
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    m_sideBarBook = new Wisteria::UI::SideBarBook(this, wxID_ANY);
    mainSizer->Add(m_sideBarBook, wxSizerFlags{ 1 }.Expand().Border());

        // version info page
        {
        wxPanel* mainPage = new wxPanel(m_sideBarBook);
        wxBoxSizer* mainPanelSizer = new wxBoxSizer(wxVERTICAL);
        mainPage->SetSizer(mainPanelSizer);
        m_sideBarBook->AddPage(mainPage, _(L"Product Info"), ID_VERSION_PAGE, true);

        if (m_logo.IsOk())
            {
            wxGenericStaticBitmap* logoBox = new wxGenericStaticBitmap(
                mainPage, wxID_ANY, m_logo, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER);
            mainPanelSizer->Add(logoBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL,
                                wxSizerFlags::GetDefaultBorder());
            }

        auto productInfoGrid = new wxFlexGridSizer(2, wxSize(wxSizerFlags::GetDefaultBorder(), 0));
        productInfoGrid->Add(new wxStaticText(mainPage, wxID_ANY, _(L"Version:")));
        productInfoGrid->Add(new wxStaticText(mainPage, wxID_ANY, m_appVersion));
#ifndef NDEBUG
        productInfoGrid->Add(new wxStaticText(
            mainPage, wxID_ANY,
            // TRANSLATORS: Compiled version of the program (e.g., DEBUG or RELEASE)
            _(L"Build:")));
        productInfoGrid->Add(new wxStaticText(mainPage, wxID_ANY, _(L"DEBUG")));
#endif
        productInfoGrid->Add(new wxStaticText(mainPage, wxID_ANY, _(L"Built on:")));
        wxDateTime buildDate;
        buildDate.ParseDate(__DATE__);
        productInfoGrid->Add(new wxStaticText(mainPage, wxID_ANY, buildDate.Format(L"%B %d, %G")));
        productInfoGrid->Add(new wxStaticText(mainPage, wxID_ANY,
                                              // TRANSLATORS: Operating System
                                              _(L"Platform:")));
        productInfoGrid->Add(
            new wxStaticText(mainPage, wxID_ANY,
#ifdef __WXGTK__
                             wxGetOsDescription() + L" (" +
                                 wxPlatformInfo::Get().GetLinuxDistributionInfo().Description + L")"
#else
                             wxGetOsDescription()
#endif
                             ));

        // put it all together
        mainPanelSizer->Add(new wxStaticText(mainPage, wxID_ANY, wxTheApp->GetAppName()),
                            wxSizerFlags{}.Left().Border(wxLEFT));
        mainPanelSizer->Add(productInfoGrid, wxSizerFlags{}.Left().Border(wxLEFT));
        mainPanelSizer->Add(new wxStaticText(mainPage, wxID_ANY, m_copyright),
                            wxSizerFlags{}.Left().Border(wxLEFT));
        mainPanelSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
        }

    // License page
    if (m_eula.length() > 0)
        {
        wxPanel* eulaPage = new wxPanel(m_sideBarBook);
        wxBoxSizer* mainPanelSizer = new wxBoxSizer(wxVERTICAL);
        eulaPage->SetSizer(mainPanelSizer);
        m_sideBarBook->AddPage(eulaPage, _(L"License"), ID_EULA_PAGE, false);

        wxTextCtrl* eulaWindow =
            new wxTextCtrl(eulaPage, wxID_ANY, wxString{}, wxDefaultPosition, wxDefaultSize,
                           wxTE_MULTILINE | wxTE_RICH2, wxGenericValidator(&m_eula));
        mainPanelSizer->Add(eulaWindow, wxSizerFlags{ 1 }.Expand().Border());
        }

    mainSizer->Add(CreateSeparatedButtonSizer(wxCLOSE), wxSizerFlags{}.Expand().Border());

    SetSizerAndFit(mainSizer);
    }
