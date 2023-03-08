#ifndef __WORD_LIST_DIALOG_H__
#define __WORD_LIST_DIALOG_H__

#include "../../../../SRC/Wisteria-Dataviz/src/ui/controls/sidebarbook.h"
#include "../../../../SRC/Wisteria-Dataviz/src/ui/controls/listctrlex.h"
#include "../../indexing/word_list.h"
#include <wx/ribbon/bar.h>
#include <wx/ribbon/buttonbar.h>
#include <wx/ribbon/gallery.h>
#include <wx/ribbon/toolbar.h>
#include <wx/ribbon/art.h>

class WordListDlg final : public wxDialog
    {
public:
    explicit WordListDlg(wxWindow* parent, wxWindowID id = wxID_ANY,
        const wxString& caption = _("Word Lists"), const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = wxCAPTION|wxCLOSE_BOX|wxRESIZE_BORDER);
    ~WordListDlg()
        {
        wxDELETE(m_DCData);
        wxDELETE(m_StockerData);
        wxDELETE(m_SpacheData);
        wxDELETE(m_HJData);
        wxDELETE(m_DolchData);
        }
    WordListDlg(const WordListDlg&) = delete;
    WordListDlg(WordListDlg&&) = delete;
    WordListDlg& operator=(const WordListDlg&) = delete;
    WordListDlg& operator=(WordListDlg&&) = delete;

    void SelectPage(const int pageId);
    constexpr static int DALE_CHALL_PAGE_ID = 1000;
    constexpr static int STOCKER_PAGE_ID = 1002;
    constexpr static int SPACHE_PAGE_ID = 1004;
    constexpr static int HARRIS_JACOBSON_PAGE_ID = 1006;
    constexpr static int DOLCH_PAGE_ID = 1008;
private:
    void CreateControls();
    [[nodiscard]] ListCtrlEx* GetActiveList();
    void OnFind(wxFindDialogEvent &event);
    void OnRibbonButton(wxRibbonButtonBarEvent& event);
    void OnNegative(wxCommandEvent& event);
    void OnClose([[maybe_unused]] wxCloseEvent& event);
    void AddSingleColumnPage(Wisteria::UI::SideBarBook* sideBar, const int id, const int listId,
                             const wxString& label, const int imageId,
                             ListCtrlExDataProvider* data, const word_list& wordList);

    constexpr static int DALE_CHALL_LIST_ID = 1001;
    constexpr static int STOCKER_LIST_ID = 1003;
    constexpr static int SPACHE_LIST_ID = 1005;
    constexpr static int HARRIS_JACOBSON_LIST_ID = 1007;
    constexpr static int DOLCH_LIST_ID = 1009;
    ListCtrlExDataProvider* m_DCData{ nullptr };
    ListCtrlExDataProvider* m_StockerData{ nullptr };
    ListCtrlExDataProvider* m_SpacheData{ nullptr };
    ListCtrlExDataProvider* m_HJData{ nullptr };
    ListCtrlExDataProvider* m_DolchData{ nullptr };
    Wisteria::UI::SideBarBook* m_sideBar{ nullptr };
    wxDECLARE_EVENT_TABLE();
    };

#endif //__WORD_LIST_DIALOG_H__
