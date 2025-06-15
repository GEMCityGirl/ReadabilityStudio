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
#include "../../OleanderStemmingLibrary/src/stemming.h"
#include "../../Wisteria-Dataviz/src/import/html_extract_text.h"
#include "../../lua/lua.h"
#include "../../tinyexpr-plusplus/tinyexpr.h"
#include "../../tinyxml2/tinyxml2.h"
#include <wx/generic/statbmpg.h>
#include <wx/stc/stc.h>
#include <wx/webrequest.h>
#include <wx/webview.h>

//------------------------------------------------------
AboutDialogEx::AboutDialogEx(wxWindow* parent, const wxBitmap& logo, wxString appVersion,
                             wxString copyright, wxString eula, wxString mlaCitation,
                             wxString apaCitation, wxString bibtexCitation, wxWindowID id,
                             const wxPoint& pos, const wxSize& size, long style)
    : m_logo(logo), m_appVersion(std::move(appVersion)), m_copyright(std::move(copyright)),
      m_eula(std::move(eula)), m_mlaCitation(std::move(mlaCitation)),
      m_apaCitation(std::move(apaCitation)), m_bibtexCitation(std::move(bibtexCitation))
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

    Bind(
        wxEVT_BUTTON,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            if (wxTheClipboard->Open())
                {
                if (!m_mlaCitation.empty())
                    {
                    // has italics in it
                    lily_of_the_valley::html_extract_text htmlExtract;
                    const wxString strippedText =
                        htmlExtract(m_mlaCitation.wc_str(), m_mlaCitation.length(), true, true);
                    wxTheClipboard->Clear();
                    wxDataObjectComposite* obj = new wxDataObjectComposite();
                    obj->Add(new wxTextDataObject(strippedText));
                    wxTheClipboard->SetData(obj);
                    }
                wxTheClipboard->Close();
                }
        },
        AboutDialogEx::ID_COPYMLA);

    Bind(
        wxEVT_BUTTON,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            if (wxTheClipboard->Open())
                {
                if (!m_apaCitation.empty())
                    {
                    wxTheClipboard->Clear();
                    wxDataObjectComposite* obj = new wxDataObjectComposite();
                    obj->Add(new wxTextDataObject(m_apaCitation));
                    wxTheClipboard->SetData(obj);
                    }
                wxTheClipboard->Close();
                }
        },
        AboutDialogEx::ID_COPYAPA);

    Bind(
        wxEVT_BUTTON,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            if (wxTheClipboard->Open())
                {
                if (!m_bibtexCitation.empty())
                    {
                    wxTheClipboard->Clear();
                    wxDataObjectComposite* obj = new wxDataObjectComposite();
                    obj->Add(new wxTextDataObject(m_bibtexCitation));
                    wxTheClipboard->SetData(obj);
                    }
                wxTheClipboard->Close();
                }
        },
        AboutDialogEx::ID_COPYBIBTEX);

    Bind(
        wxEVT_BUTTON,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            if (wxTheClipboard->Open())
                {
                if (!m_components.empty())
                    {
                    lily_of_the_valley::html_extract_text htmlExtract;
                    const wxString strippedText =
                        htmlExtract(m_components.wc_str(), m_components.length(), true, true);
                    wxTheClipboard->Clear();
                    wxDataObjectComposite* obj = new wxDataObjectComposite();
                    obj->Add(new wxTextDataObject(strippedText));
                    wxTheClipboard->SetData(obj);
                    }
                wxTheClipboard->Close();
                }
        },
        AboutDialogEx::ID_COPY_COMPONENTS);

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
        mainPanelSizer->AddSpacer(wxSizerFlags::GetDefaultBorder() * 2);
        mainPanelSizer->Add(new wxStaticText(mainPage, wxID_ANY, m_copyright),
                            wxSizerFlags{}.Left().Border(wxLEFT));
        mainPanelSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
        }

    // License page
    if (!m_eula.empty())
        {
        wxPanel* eulaPage = new wxPanel(m_sideBarBook);
        wxBoxSizer* mainPanelSizer = new wxBoxSizer(wxVERTICAL);
        eulaPage->SetSizer(mainPanelSizer);
        m_sideBarBook->AddPage(eulaPage, _(L"License"), ID_LICENSING_PAGE, false);

        wxTextCtrl* eulaWindow = new wxTextCtrl(
            eulaPage, wxID_ANY, wxString{}, wxDefaultPosition, wxDefaultSize,
            wxTE_MULTILINE | wxTE_RICH2 | wxTE_READONLY, wxGenericValidator(&m_eula));
        mainPanelSizer->Add(eulaWindow, wxSizerFlags{ 1 }.Expand().Border());
        }

        // components
        {
        wxPanel* componentsPage = new wxPanel(m_sideBarBook);
        wxBoxSizer* mainPanelSizer = new wxBoxSizer(wxVERTICAL);
        componentsPage->SetSizer(mainPanelSizer);
        m_sideBarBook->AddPage(componentsPage, _(L"Components"), ID_COMPONENTS, false);

        // Consistently format library info with the name and version number.
        const auto formatLibInfo = [](const auto& libInfo)
        {
            wxString copyright = libInfo.GetCopyright();
            copyright.Replace(L"\n", L"<br />");

            return wxString::Format(
                L"<span style='font-weight: bold;'>%s</span> - %s<br />%s<br />", libInfo.GetName(),
                (libInfo.AtLeast(0) ?
                     wxString::Format(_(L"version %s"), libInfo.GetNumericVersionString()) :
                     _(L"unversioned")),
                (libInfo.HasCopyright() ? copyright + L"<br />" : wxString{}));
        };

        // Get wxWidget's version, as well as its submodules, and sort them.
        std::vector<wxString> allLibInfo;
        allLibInfo.push_back(formatLibInfo(wxGetLibraryVersionInfo()));
        allLibInfo.push_back(formatLibInfo(Wisteria::GetLibraryVersionInfo()));
        allLibInfo.push_back(
            formatLibInfo(wxVersionInfo{ L"Lua", LUA_VERSION_MAJOR_N, LUA_VERSION_MINOR_N,
                                         LUA_VERSION_RELEASE_N, 0, wxString{}, LUA_COPYRIGHT }));
        allLibInfo.push_back(formatLibInfo(wxVersionInfo{
            L"Oleander Stemming Library", stemming::OLEANDER_STEM_MAJOR_VERSION,
            stemming::OLEANDER_STEM_MINOR_VERSION, stemming::OLEANDER_STEM_PATCH_VERSION,
            stemming::OLEANDER_STEM_TWEAK_VERSION, wxString{},
            stemming::OLEANDER_STEM_COPYRIGHT }));
        allLibInfo.push_back(formatLibInfo(
            wxVersionInfo{ L"TinyExpr++", TINYEXPR_CPP_MAJOR_VERSION, TINYEXPR_CPP_MINOR_VERSION,
                           TINYEXPR_CPP_PATCH_VERSION, TINYEXPR_CPP_TWEAK_VERSION, wxString{},
                           TINYEXPR_CPP_COPYRIGHT }));
        allLibInfo.push_back(formatLibInfo(wxVersionInfo{
            L"TinyXML2", TIXML2_MAJOR_VERSION, TIXML2_MINOR_VERSION, TIXML2_PATCH_VERSION }));
        allLibInfo.push_back(formatLibInfo(wxTIFFHandler::GetLibraryVersionInfo()));
        allLibInfo.push_back(formatLibInfo(wxJPEGHandler::GetLibraryVersionInfo()));
        allLibInfo.push_back(formatLibInfo(wxPNGHandler::GetLibraryVersionInfo()));
        allLibInfo.push_back(formatLibInfo(wxWEBPHandler::GetLibraryVersionInfo()));
        allLibInfo.push_back(formatLibInfo(wxRegEx::GetLibraryVersionInfo()));
        allLibInfo.push_back(formatLibInfo(wxXmlDocument::GetLibraryVersionInfo()));
        allLibInfo.push_back(formatLibInfo(wxStyledTextCtrl::GetLibraryVersionInfo()));
        allLibInfo.push_back(formatLibInfo(wxGetZlibVersionInfo()));
        // submodules without version information
        allLibInfo.push_back(formatLibInfo(wxVersionInfo{ L"NanoSVG", -1 }));
        allLibInfo.push_back(formatLibInfo(wxVersionInfo{ L"CRCpp", -1 }));
        allLibInfo.push_back(formatLibInfo(wxVersionInfo{ L"wxSimpleJSON", -1 }));
        allLibInfo.push_back(formatLibInfo(wxVersionInfo{ L"wxStartPage", -1 }));
        allLibInfo.push_back(formatLibInfo(wxVersionInfo{ L"easyexif", -1 }));
        allLibInfo.push_back(formatLibInfo(wxVersionInfo{ L"UTF8-CPP", -1 }));
        /// @todo uncomment if WebView is ever included
        // allLibInfo.push_back(formatLibInfo(wxWebView::GetBackendVersionInfo()));
        std::sort(allLibInfo.begin(), allLibInfo.end(),
                  [](const auto& lhv, const auto& rhv) { return wxStricmp(lhv, rhv) < 0; });

        // Format all the library strings into one.
        m_components = [&allLibInfo]()
        {
            wxString allStr;
            // most library strings will be ~30 characters long
            allStr.reserve(allLibInfo.size() * 50);
            for (const auto& lib : allLibInfo)
                {
                allStr.append(lib);
                }
            allStr.Trim();
            return allStr;
        }();

        mainPanelSizer->Add(new wxStaticText(componentsPage, wxID_ANY, _DT(L"Included libraries:")),
                            wxSizerFlags{}.Border(wxLEFT));
        wxBoxSizer* textRowSizer = new wxBoxSizer(wxHORIZONTAL);
        wxHtmlWindow* textWindow =
            new wxHtmlWindow(componentsPage, wxID_ANY, wxDefaultPosition, wxSize{ -1, FromDIP(75) },
                             wxHW_SCROLLBAR_AUTO | wxBORDER_THEME | wxHW_NO_SELECTION);
        textWindow->SetPage(m_components);
        textRowSizer->Add(textWindow, wxSizerFlags{ 1 }.Expand());
        textRowSizer->Add(new wxBitmapButton(componentsPage, ID_COPY_COMPONENTS,
                                             wxArtProvider::GetBitmap(wxART_COPY, wxART_BUTTON)));
        mainPanelSizer->Add(textRowSizer, wxSizerFlags{ 1 }.Expand().Border());
        }

    if (!m_mlaCitation.empty() && !m_apaCitation.empty() && !m_bibtexCitation.empty())
        {
        wxPanel* citationPage = new wxPanel(m_sideBarBook);
        wxBoxSizer* mainPanelSizer = new wxBoxSizer(wxVERTICAL);
        citationPage->SetSizer(mainPanelSizer);
        m_sideBarBook->AddPage(citationPage, _(L"Citation"), ID_CITATION, false);

        mainPanelSizer->Add(new wxStaticText(citationPage, wxID_ANY,
                                             wxString::Format(_(L"To cite %s in publications:"),
                                                              wxTheApp->GetAppName())),
                            wxSizerFlags{}.Border(wxLEFT));

        mainPanelSizer->AddSpacer(wxSizerFlags::GetDefaultBorder() * 2);

            {
            mainPanelSizer->Add(new wxStaticText(citationPage, wxID_ANY, _DT(L"MLA:")),
                                wxSizerFlags{}.Border(wxLEFT));
            wxBoxSizer* textRowSizer = new wxBoxSizer(wxHORIZONTAL);
            wxHtmlWindow* textWindow = new wxHtmlWindow(
                citationPage, wxID_ANY, wxDefaultPosition, wxSize{ -1, FromDIP(75) },
                wxHW_SCROLLBAR_AUTO | wxBORDER_THEME | wxHW_NO_SELECTION);
            textWindow->SetPage(m_mlaCitation);
            textRowSizer->Add(textWindow, wxSizerFlags{ 1 }.Expand());
            textRowSizer->Add(new wxBitmapButton(
                citationPage, ID_COPYMLA, wxArtProvider::GetBitmap(wxART_COPY, wxART_BUTTON)));
            mainPanelSizer->Add(textRowSizer, wxSizerFlags{}.Expand().Border());
            }

            {
            mainPanelSizer->Add(new wxStaticText(citationPage, wxID_ANY, _DT(L"APA:")),
                                wxSizerFlags{}.Border(wxLEFT));
            wxBoxSizer* textRowSizer = new wxBoxSizer(wxHORIZONTAL);
            wxHtmlWindow* textWindow = new wxHtmlWindow(
                citationPage, wxID_ANY, wxDefaultPosition, wxSize{ -1, FromDIP(75) },
                wxHW_SCROLLBAR_AUTO | wxBORDER_THEME | wxHW_NO_SELECTION);
            textWindow->SetPage(m_apaCitation);
            textRowSizer->Add(textWindow, wxSizerFlags{ 1 }.Expand());
            textRowSizer->Add(new wxBitmapButton(
                citationPage, ID_COPYAPA, wxArtProvider::GetBitmap(wxART_COPY, wxART_BUTTON)));
            mainPanelSizer->Add(textRowSizer, wxSizerFlags{}.Expand().Border());
            }

            {
            mainPanelSizer->Add(new wxStaticText(citationPage, wxID_ANY, _DT(L"BibTeX:")),
                                wxSizerFlags{}.Border(wxLEFT));
            wxBoxSizer* textRowSizer = new wxBoxSizer(wxHORIZONTAL);
            wxTextCtrl* textWindow = new wxTextCtrl(
                citationPage, wxID_ANY, wxString{}, wxDefaultPosition, wxSize{ -1, FromDIP(175) },
                wxTE_MULTILINE | wxTE_RICH2 | wxTE_READONLY, wxGenericValidator(&m_bibtexCitation));
            textWindow->SetFont(wxFontInfo{}.Family(wxFontFamily::wxFONTFAMILY_TELETYPE));
            textRowSizer->Add(textWindow, wxSizerFlags{ 1 }.Expand());
            textRowSizer->Add(new wxBitmapButton(
                citationPage, ID_COPYBIBTEX, wxArtProvider::GetBitmap(wxART_COPY, wxART_BUTTON)));
            mainPanelSizer->Add(textRowSizer, wxSizerFlags{ 1 }.Expand().Border());
            }
        }

    mainSizer->Add(CreateSeparatedButtonSizer(wxCLOSE), wxSizerFlags{}.Expand().Border());

    SetSizerAndFit(mainSizer);
    }
