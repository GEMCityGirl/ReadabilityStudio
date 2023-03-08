#include "word_list_dlg.h"
#include "../../projects/base_project.h"
#include "../../app/readability_app.h"
#include "../../../../SRC/Wisteria-Dataviz/src/ui/ribbon/artmetro.h"
#include "../../../../SRC/Wisteria-Dataviz/src/ui/controls/searchpanel.h"
#include "../../../../SRC/Wisteria-Dataviz/src/util/parentblocker.h"

wxDECLARE_APP(ReadabilityApp);

wxBEGIN_EVENT_TABLE(WordListDlg, wxDialog)
    EVT_CLOSE(WordListDlg::OnClose)
    EVT_FIND(wxID_ANY, WordListDlg::OnFind)
    EVT_FIND_NEXT(wxID_ANY, WordListDlg::OnFind)
    EVT_BUTTON(wxID_NO, WordListDlg::OnNegative)
    EVT_BUTTON(wxID_CANCEL, WordListDlg::OnNegative)
    EVT_BUTTON(wxID_CLOSE, WordListDlg::OnNegative)
    EVT_RIBBONBUTTONBAR_CLICKED(wxID_SAVE, WordListDlg::OnRibbonButton)
    EVT_RIBBONBUTTONBAR_CLICKED(wxID_PRINT, WordListDlg::OnRibbonButton)
    EVT_RIBBONBUTTONBAR_CLICKED(wxID_COPY, WordListDlg::OnRibbonButton)
    EVT_RIBBONBUTTONBAR_CLICKED(wxID_SELECTALL, WordListDlg::OnRibbonButton)
    EVT_RIBBONBUTTONBAR_CLICKED(XRCID("ID_LIST_SORT"), WordListDlg::OnRibbonButton)
wxEND_EVENT_TABLE()

WordListDlg::WordListDlg(wxWindow* parent, wxWindowID id /*= wxID_ANY*/,
        const wxString& caption /*= _("Word Lists")*/, const wxPoint& pos /*= wxDefaultPosition*/,
        const wxSize& size /*= wxDefaultSize*/, long style /*= wxCAPTION|wxCLOSE_BOX|wxRESIZE_BORDER*/) :
        wxDialog(parent, id, caption, pos, size, style),
        m_DCData(new ListCtrlExDataProvider),
        m_StockerData(new ListCtrlExDataProvider),
        m_SpacheData(new ListCtrlExDataProvider),
        m_HJData(new ListCtrlExDataProvider),
        m_DolchData(new ListCtrlExDataProvider),
        m_sideBar(nullptr)
        {
        SetBackgroundColour(wxGetApp().GetAppOptions().GetControlBackgroundColor());
        CreateControls();
        Centre();
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
void WordListDlg::OnFind(wxFindDialogEvent &event)
    {
    ListCtrlEx* listCtrl = GetActiveList();

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
    ListCtrlEx* listCtrl = GetActiveList();

    if (listCtrl)
        {
        ParentEventBlocker blocker(listCtrl);
        listCtrl->ProcessWindowEvent(event);
        }
    }

//------------------------------------------------------
ListCtrlEx* WordListDlg::GetActiveList()
    {
    wxWindow* listCtrl = nullptr;
    if (m_sideBar->GetCurrentPage()->GetId() == DALE_CHALL_PAGE_ID)
        { listCtrl = m_sideBar->GetCurrentPage()->FindWindow(DALE_CHALL_LIST_ID); }
    else if (m_sideBar->GetCurrentPage()->GetId() == STOCKER_PAGE_ID)
        { listCtrl = m_sideBar->GetCurrentPage()->FindWindow(STOCKER_LIST_ID); }
    else if (m_sideBar->GetCurrentPage()->GetId() == SPACHE_PAGE_ID)
        { listCtrl = m_sideBar->GetCurrentPage()->FindWindow(SPACHE_LIST_ID); }
    else if (m_sideBar->GetCurrentPage()->GetId() == HARRIS_JACOBSON_PAGE_ID)
        { listCtrl = m_sideBar->GetCurrentPage()->FindWindow(HARRIS_JACOBSON_LIST_ID); }
    else if (m_sideBar->GetCurrentPage()->GetId() == DOLCH_PAGE_ID)
        { listCtrl = m_sideBar->GetCurrentPage()->FindWindow(DOLCH_LIST_ID); }

    if (listCtrl && listCtrl->IsKindOf(CLASSINFO(ListCtrlEx)))
        { return dynamic_cast<ListCtrlEx*>(listCtrl); }
    else
        { return nullptr; }
    }

//---------------------------------------------
void WordListDlg::OnNegative(wxCommandEvent& event)
    {
    if (IsModal())
        { EndModal(event.GetId()); }
    else
        { Show(false); }
    }

//---------------------------------------------
void WordListDlg::AddSingleColumnPage(Wisteria::UI::SideBarBook* sideBar, const int id, const int listId,
        const wxString& label, const int imageId,
        ListCtrlExDataProvider* data, const word_list& wordList)
    {
    wxPanel* page = new wxPanel(sideBar, id, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);
    page->SetSizer(panelSizer);
    sideBar->AddPage(page, label, id, false, imageId);

    ListCtrlEx* list = new ListCtrlEx(page, listId, wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL|wxLC_REPORT|wxLC_ALIGN_LEFT|wxLC_NO_HEADER);
    list->SetLabel(label);
    list->EnableGridLines();
    list->InsertColumn(0, _("Words"));
    list->SetVirtualDataProvider(data);
    list->SetVirtualDataSize(wordList.get_words().size(), 1);
    for (size_t i = 0; i < wordList.get_words().size(); ++i)
        { data->SetItemText(i, 0, wxString(wordList.get_words().at(i).c_str())); }
    list->DistributeColumns();
    BaseProjectDoc::UpdateListOptions(list);

    panelSizer->Add(list, 1, wxEXPAND);
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
    mainSizer->Add(searchSizer,0,wxEXPAND);

        {
        wxRibbonBar* ribbon = new wxRibbonBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxRIBBON_BAR_FLOW_HORIZONTAL);
        wxRibbonPage* homePage = new wxRibbonPage(ribbon, wxID_ANY, wxEmptyString);
        //export
            {
            wxRibbonPanel* exportPage = new wxRibbonPanel(homePage, wxID_ANY, _("Export"), wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
            wxRibbonButtonBar* buttonBar = new wxRibbonButtonBar(exportPage);
            buttonBar->AddButton(wxID_SAVE, _("Save"),
                wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_BUTTON, FromDIP(wxSize(32,32))),
                _("Save the list."));
            buttonBar->AddButton(wxID_PRINT, _("Print"),
                wxArtProvider::GetBitmap(wxART_PRINT, wxART_BUTTON, FromDIP(wxSize(32,32))),
                _("Print the list."));
            }
        //edit
            {
            wxRibbonPanel* editPage = new wxRibbonPanel(homePage, wxID_ANY, _("Edit"), wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
            wxRibbonButtonBar* buttonBar = new wxRibbonButtonBar(editPage);
            buttonBar->AddButton(wxID_COPY, _("Copy Selection"),
                wxArtProvider::GetBitmap(wxART_COPY, wxART_BUTTON, FromDIP(wxSize(32,32))),
                _("Copy the selected items."));
            buttonBar->AddButton(wxID_SELECTALL, _("Select All"),
                wxArtProvider::GetBitmap(wxT("ID_SELECT_ALL"), wxART_BUTTON, FromDIP(wxSize(32,32))),
                _("Select the entire list."));
            buttonBar->AddButton(XRCID("ID_LIST_SORT"), _("Sort"), wxArtProvider::GetBitmap(wxT("ID_LIST_SORT"), wxART_BUTTON, FromDIP(wxSize(32,32))), _("Sort the list."));
            }
        ribbon->SetArtProvider(new Wisteria::UI::RibbonMetroArtProvider);
        wxGetApp().UpdateRibbonTheme(ribbon);

        mainSizer->Add(ribbon, 0, wxEXPAND|wxALL, wxSizerFlags::GetDefaultBorder());
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
    m_sideBar->GetImageList().push_back(
        wxGetApp().GetResourceManager().GetSVG(L"tests/dolch.svg"));
    m_sideBar->GetSideBar()->Realize();
    mainSizer->Add(m_sideBar, 1, wxEXPAND|wxALL, wxSizerFlags::GetDefaultBorder());

    // Dolch
        {
        wxPanel* page = new wxPanel(m_sideBar, DOLCH_PAGE_ID, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
        wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);
        page->SetSizer(panelSizer);
        m_sideBar->AddPage(page, _("Dolch Sight Words"), DOLCH_PAGE_ID, true, 4);

        ListCtrlEx* list = new ListCtrlEx(page, DOLCH_LIST_ID, wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL|wxLC_REPORT|wxLC_ALIGN_LEFT);
        list->SetLabel(_("Dolch Sight Words"));
        list->EnableGridLines();
        list->InsertColumn(0, _("Dolch Words"));
        list->InsertColumn(1, _("Category"));
        list->SetVirtualDataProvider(m_DolchData);
        list->SetVirtualDataSize(BaseProject::m_dolch_word_list.get_list_size(), 2);
        size_t currentDolchItem = 0;
        for (const auto& DolchPos : BaseProject::m_dolch_word_list.get_words())
            {
            list->SetItemText(currentDolchItem, 0,
                wxString(DolchPos.get_word().c_str()));

            wxString classificationLabel;
            switch (DolchPos.get_type())
                {
            case readability::sight_word_type::conjunction:
                classificationLabel = _("Conjunction");
                break;
            case readability::sight_word_type::preposition:
                classificationLabel = _("Preposition");
                break;
            case readability::sight_word_type::pronoun:
                classificationLabel = _("Pronoun");
                break;
            case readability::sight_word_type::adverb:
                classificationLabel = _("Adverb");
                break;
            case readability::sight_word_type::adjective:
                classificationLabel = _("Adjective");
                break;
            case readability::sight_word_type::verb:
                classificationLabel = _("Verb");
                break;
            case readability::sight_word_type::noun:
                classificationLabel = _("Noun");
                break;
                };
            list->SetItemText(currentDolchItem++, 1, classificationLabel);
            }
        list->SetVirtualDataSize(currentDolchItem);
        list->DistributeColumns();
        BaseProjectDoc::UpdateListOptions(list);

        panelSizer->Add(list, 1, wxEXPAND);
        }

    AddSingleColumnPage(m_sideBar, HARRIS_JACOBSON_PAGE_ID, HARRIS_JACOBSON_LIST_ID, _("Harris-Jacobson"), 2, m_HJData, BaseProject::m_harris_jacobson_word_list);
    AddSingleColumnPage(m_sideBar, DALE_CHALL_PAGE_ID, DALE_CHALL_LIST_ID, _("New Dale-Chall"), 0, m_DCData, BaseProject::m_dale_chall_word_list);
    AddSingleColumnPage(m_sideBar, SPACHE_PAGE_ID, SPACHE_LIST_ID, _("Spache Revised"), 3, m_SpacheData, BaseProject::m_spache_word_list);
    AddSingleColumnPage(m_sideBar, STOCKER_PAGE_ID, STOCKER_LIST_ID, _("Stocker's Catholic Supplement"), 1, m_StockerData, BaseProject::m_stocker_catholic_word_list);
    
    mainSizer->Add(CreateButtonSizer(wxCLOSE), 0, wxEXPAND|wxALL, wxSizerFlags::GetDefaultBorder());

    SetSizerAndFit(mainSizer);
    }

//-------------------------------------------------------
void WordListDlg::OnClose([[maybe_unused]] wxCloseEvent& event)
    {
    // search control locks up app if it has the focus here, so remove the focus from it
    SetFocusIgnoringChildren();

    if (IsModal())
        { EndModal(wxID_CLOSE); }
    else
        { Hide(); }
    }
