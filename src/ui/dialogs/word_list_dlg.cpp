///////////////////////////////////////////////////////////////////////////////
// Name:        word_list_dlg.cpp
// Author:      Blake Madden
// Copyright:   (c) 2005-2023 Blake Madden
// Licence:     3-Clause BSD licence
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "word_list_dlg.h"
#include "../../Wisteria-Dataviz/src/ui/controls/searchpanel.h"
#include "../../Wisteria-Dataviz/src/ui/ribbon/artmetro.h"
#include "../../Wisteria-Dataviz/src/util/parentblocker.h"
#include "../../app/readability_app.h"
#include "../../projects/base_project.h"
#include "../../projects/base_project_doc.h"

wxDECLARE_APP(ReadabilityApp);

//------------------------------------------------------
WordListDlg::WordListDlg(wxWindow* parent, wxWindowID id /*= wxID_ANY*/,
                         const wxString& caption /*= _(L"Word Lists")*/,
                         const wxPoint& pos /*= wxDefaultPosition*/,
                         const wxSize& size /*= wxDefaultSize*/,
                         long style /*= wxCAPTION|wxCLOSE_BOX|wxRESIZE_BORDER*/)
    : wxDialog(parent, id, caption, pos, size, style)
    {
    CreateControls();
    Centre();

    Bind(wxEVT_CLOSE_WINDOW, &WordListDlg::OnClose, this);
    Bind(wxEVT_FIND, &WordListDlg::OnFind, this);
    Bind(wxEVT_FIND_NEXT, &WordListDlg::OnFind, this);
    Bind(wxEVT_BUTTON, &WordListDlg::OnNegative, this, wxID_NO);
    Bind(wxEVT_BUTTON, &WordListDlg::OnNegative, this, wxID_CANCEL);
    Bind(wxEVT_BUTTON, &WordListDlg::OnNegative, this, wxID_CLOSE);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &WordListDlg::OnRibbonButton, this, wxID_SAVE);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &WordListDlg::OnRibbonButton, this, wxID_PRINT);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &WordListDlg::OnRibbonButton, this, wxID_COPY);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &WordListDlg::OnRibbonButton, this, wxID_SELECTALL);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &WordListDlg::OnRibbonButton, this, XRCID("ID_LIST_SORT"));
    }

//------------------------------------------------------
void WordListDlg::SelectPage(const int pageId)
    {
    for (size_t i = 0; i < m_sideBar->GetPageCount(); ++i)
        {
        wxWindow* page = m_sideBar->GetPage(i);
        if (page && page->GetId() == pageId)
            {
            m_sideBar->SetSelection(i);
            }
        }
    }

//------------------------------------------------------
void WordListDlg::OnFind(wxFindDialogEvent& event)
    {
    Wisteria::UI::ListCtrlEx* listCtrl = GetActiveList();

    if (listCtrl)
        {
        ParentEventBlocker blocker(listCtrl);
        listCtrl->ProcessWindowEvent(event);
        listCtrl->SetFocus();
        }
    }

//------------------------------------------------------
void WordListDlg::OnRibbonButton(wxRibbonButtonBarEvent& event)
    {
    Wisteria::UI::ListCtrlEx* listCtrl = GetActiveList();

    if (listCtrl)
        {
        ParentEventBlocker blocker(listCtrl);
        listCtrl->ProcessWindowEvent(event);
        }
    }

//------------------------------------------------------
Wisteria::UI::ListCtrlEx* WordListDlg::GetActiveList()
    {
    wxWindow* listCtrl{ nullptr };
    const auto currentId{ m_sideBar->GetCurrentPage()->GetId() };
    if (currentId == DALE_CHALL_PAGE_ID)
        {
        listCtrl = m_sideBar->GetCurrentPage()->FindWindow(DALE_CHALL_LIST_ID);
        }
    else if (currentId == STOCKER_PAGE_ID)
        {
        listCtrl = m_sideBar->GetCurrentPage()->FindWindow(STOCKER_LIST_ID);
        }
    else if (currentId == SPACHE_PAGE_ID)
        {
        listCtrl = m_sideBar->GetCurrentPage()->FindWindow(SPACHE_LIST_ID);
        }
    else if (currentId == HARRIS_JACOBSON_PAGE_ID)
        {
        listCtrl = m_sideBar->GetCurrentPage()->FindWindow(HARRIS_JACOBSON_LIST_ID);
        }
    else if (currentId == DOLCH_PAGE_ID)
        {
        listCtrl = m_sideBar->GetCurrentPage()->FindWindow(DOLCH_LIST_ID);
        }

    assert(listCtrl && listCtrl->IsKindOf(CLASSINFO(Wisteria::UI::ListCtrlEx)) &&
           L"Invalid list control in word list dialog?!");
    return (listCtrl && listCtrl->IsKindOf(CLASSINFO(Wisteria::UI::ListCtrlEx))) ?
               dynamic_cast<Wisteria::UI::ListCtrlEx*>(listCtrl) :
               nullptr;
    }

//---------------------------------------------
void WordListDlg::OnNegative(wxCommandEvent& event)
    {
    if (IsModal())
        {
        EndModal(event.GetId());
        }
    else
        {
        Show(false);
        }
    }

//---------------------------------------------
void WordListDlg::AddSingleColumnPage(Wisteria::UI::SideBarBook* sideBar, const int id,
                                      const int listId, const wxString& label, const int imageId,
                                      std::shared_ptr<Wisteria::UI::ListCtrlExDataProvider> data,
                                      const word_list& wordList)
    {
    wxPanel* page = new wxPanel(sideBar, id, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);
    page->SetSizer(panelSizer);
    sideBar->AddPage(page, label, id, false, imageId);

    Wisteria::UI::ListCtrlEx* list =
        new Wisteria::UI::ListCtrlEx(page, listId, wxDefaultPosition, wxDefaultSize,
                       wxLC_VIRTUAL | wxLC_REPORT | wxLC_ALIGN_LEFT | wxLC_NO_HEADER);
    list->SetLabel(label);
    list->EnableGridLines();
    list->InsertColumn(0, _(L"Words"));
    list->SetVirtualDataProvider(data);
    list->SetVirtualDataSize(wordList.get_words().size(), 1);
    for (size_t i = 0; i < wordList.get_words().size(); ++i)
        {
        data->SetItemText(i, 0, wxString(wordList.get_words().at(i).c_str()));
        }
    list->DistributeColumns();
    BaseProjectDoc::UpdateListOptions(list);

    panelSizer->Add(list, wxSizerFlags{ 1 }.Expand());
    }

//---------------------------------------------
void WordListDlg::CreateControls()
    {
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->SetMinSize(FromDIP(wxSize(800, 600)));

    wxBoxSizer* searchSizer = new wxBoxSizer(wxHORIZONTAL);
    searchSizer->AddStretchSpacer(1);
    auto searcher = new Wisteria::UI::SearchPanel(this, wxID_ANY);
    searcher->SetBackgroundColour(GetBackgroundColour());
    searchSizer->Add(searcher);
    mainSizer->Add(searchSizer, 0, wxEXPAND);

        {
        wxRibbonBar* ribbon = new wxRibbonBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                              wxRIBBON_BAR_FLOW_HORIZONTAL);
        wxRibbonPage* homePage = new wxRibbonPage(ribbon, wxID_ANY, wxEmptyString);
            // export
            {
            wxRibbonPanel* exportPage =
                new wxRibbonPanel(homePage, wxID_ANY, _(L"Export"), wxNullBitmap, wxDefaultPosition,
                                  wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
            wxRibbonButtonBar* buttonBar = new wxRibbonButtonBar(exportPage);
            buttonBar->AddButton(
                wxID_SAVE, _(L"Save"),
                wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_BUTTON, FromDIP(wxSize(32, 32)))
                    .ConvertToImage(),
                _(L"Save the list."));
            buttonBar->AddButton(
                wxID_PRINT, _(L"Print"),
                wxArtProvider::GetBitmap(wxART_PRINT, wxART_BUTTON, FromDIP(wxSize(32, 32)))
                    .ConvertToImage(),
                _(L"Print the list."));
            }
            // edit
            {
            wxRibbonPanel* editPage =
                new wxRibbonPanel(homePage, wxID_ANY, _(L"Edit"), wxNullBitmap, wxDefaultPosition,
                                  wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
            wxRibbonButtonBar* buttonBar = new wxRibbonButtonBar(editPage);
            buttonBar->AddButton(
                wxID_COPY, _(L"Copy Selection"),
                wxArtProvider::GetBitmap(wxART_COPY, wxART_BUTTON, FromDIP(wxSize(32, 32)))
                    .ConvertToImage(),
                _(L"Copy the selected items."));
            buttonBar->AddButton(
                wxID_SELECTALL, _(L"Select All"),
                wxArtProvider::GetBitmap(L"ID_SELECT_ALL", wxART_BUTTON, FromDIP(wxSize(32, 32)))
                    .ConvertToImage(),
                _(L"Select the entire list."));
            buttonBar->AddButton(
                XRCID("ID_LIST_SORT"), _(L"Sort"),
                wxArtProvider::GetBitmap(L"ID_LIST_SORT", wxART_BUTTON, FromDIP(wxSize(32, 32)))
                    .ConvertToImage(),
                _(L"Sort the list."));
            }
        ribbon->SetArtProvider(new Wisteria::UI::RibbonMetroArtProvider);
        wxGetApp().UpdateRibbonTheme(ribbon);

        mainSizer->Add(ribbon, 0, wxEXPAND | wxALL, wxSizerFlags::GetDefaultBorder());
        ribbon->Realise();
        }

    m_sideBar = new Wisteria::UI::SideBarBook(this, wxID_ANY);
    wxGetApp().UpdateSideBarTheme(m_sideBar->GetSideBar());

    m_sideBar->SetIconSize(wxSize(32, 32));
    m_sideBar->GetImageList().push_back(
        wxGetApp().GetResourceManager().GetSVG(L"tests/dale-chall-test.svg"));
    m_sideBar->GetImageList().push_back(
        wxGetApp().GetResourceManager().GetSVG(L"tests/stocker.svg"));
    m_sideBar->GetImageList().push_back(
        wxGetApp().GetResourceManager().GetSVG(L"tests/harris-jacobson.svg"));
    m_sideBar->GetImageList().push_back(
        wxGetApp().GetResourceManager().GetSVG(L"tests/spache-test.svg"));
    m_sideBar->GetImageList().push_back(wxGetApp().GetResourceManager().GetSVG(L"tests/dolch.svg"));
    m_sideBar->GetSideBar()->Realize();
    mainSizer->Add(m_sideBar, wxSizerFlags{ 1 }.Expand().Border());

        // Dolch
        {
        wxPanel* page = new wxPanel(m_sideBar, DOLCH_PAGE_ID, wxDefaultPosition, wxDefaultSize,
                                    wxTAB_TRAVERSAL);
        wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);
        page->SetSizer(panelSizer);
        m_sideBar->AddPage(page, _(L"Dolch Sight Words"), DOLCH_PAGE_ID, true, 4);

        Wisteria::UI::ListCtrlEx* list =
            new Wisteria::UI::ListCtrlEx(page, DOLCH_LIST_ID, wxDefaultPosition, wxDefaultSize,
                                         wxLC_VIRTUAL | wxLC_REPORT | wxLC_ALIGN_LEFT);
        list->SetLabel(_(L"Dolch Sight Words"));
        list->EnableGridLines();
        list->InsertColumn(0, _(L"Dolch Words"));
        list->InsertColumn(1, _(L"Category"));
        list->SetVirtualDataProvider(m_DolchData);
        list->SetVirtualDataSize(BaseProject::m_dolch_word_list.get_list_size(), 2);
        size_t currentDolchItem = 0;
        for (const auto& DolchPos : BaseProject::m_dolch_word_list.get_words())
            {
            list->SetItemText(currentDolchItem, 0, wxString(DolchPos.get_word().c_str()));

            wxString classificationLabel;
            switch (DolchPos.get_type())
                {
            case readability::sight_word_type::conjunction:
                classificationLabel = _(L"Conjunction");
                break;
            case readability::sight_word_type::preposition:
                classificationLabel = _(L"Preposition");
                break;
            case readability::sight_word_type::pronoun:
                classificationLabel = _(L"Pronoun");
                break;
            case readability::sight_word_type::adverb:
                classificationLabel = _(L"Adverb");
                break;
            case readability::sight_word_type::adjective:
                classificationLabel = _(L"Adjective");
                break;
            case readability::sight_word_type::verb:
                classificationLabel = _(L"Verb");
                break;
            case readability::sight_word_type::noun:
                classificationLabel = _(L"Noun");
                break;
                };
            list->SetItemText(currentDolchItem++, 1, classificationLabel);
            }
        list->SetVirtualDataSize(currentDolchItem);
        list->DistributeColumns();
        BaseProjectDoc::UpdateListOptions(list);

        panelSizer->Add(list, wxSizerFlags{ 1 }.Expand());
        }

    AddSingleColumnPage(m_sideBar, HARRIS_JACOBSON_PAGE_ID, HARRIS_JACOBSON_LIST_ID,
                        _(L"Harris-Jacobson"), 2, m_HJData,
                        BaseProject::m_harris_jacobson_word_list);
    AddSingleColumnPage(m_sideBar, DALE_CHALL_PAGE_ID, DALE_CHALL_LIST_ID, _(L"New Dale-Chall"), 0,
                        m_DCData, BaseProject::m_dale_chall_word_list);
    AddSingleColumnPage(m_sideBar, SPACHE_PAGE_ID, SPACHE_LIST_ID, _(L"Spache Revised"), 3,
                        m_SpacheData, BaseProject::m_spache_word_list);
    AddSingleColumnPage(m_sideBar, STOCKER_PAGE_ID, STOCKER_LIST_ID,
                        _(L"Stocker's Catholic Supplement"), 1, m_StockerData,
                        BaseProject::m_stocker_catholic_word_list);

    mainSizer->Add(CreateButtonSizer(wxCLOSE), 0, wxEXPAND | wxALL,
                   wxSizerFlags::GetDefaultBorder());

    SetSizerAndFit(mainSizer);
    }

//-------------------------------------------------------
void WordListDlg::OnClose([[maybe_unused]] wxCloseEvent& event)
    {
    // search control locks up app if it has the focus here,
    // so remove the focus from it
    SetFocusIgnoringChildren();

    if (IsModal())
        {
        EndModal(wxID_CLOSE);
        }
    else
        {
        Hide();
        }
    }
