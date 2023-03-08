#ifndef __TEST_BUNDLE_DLG_H__
#define __TEST_BUNDLE_DLG_H__

#include <wx/wx.h>
#include <wx/gbsizer.h>
#include "../../projects/base_project.h"
#include "../../readability/readability_test.h"
#include "../../../../SRC/Wisteria-Dataviz/src/ui/dialogs/dialogwithhelp.h"
#include "../../../../SRC/Wisteria-Dataviz/src/ui/controls/listctrlex.h"
#include "../../../../SRC/Wisteria-Dataviz/src/ui/controls/sidebarbook.h"

class TestBundle;

class TestBundleDlg final : public Wisteria::UI::DialogWithHelp
    {
public:
    TestBundleDlg() = delete;
    TestBundleDlg(wxWindow* parent, TestBundle& testBundle);
    TestBundleDlg(const TestBundleDlg&) = delete;
    TestBundleDlg(TestBundleDlg&&) = delete;
    TestBundleDlg& operator=(const TestBundleDlg&) = delete;
    TestBundleDlg& operator=(TestBundleDlg&&) = delete;
    void SetTestBundleName(const wxString& name)
        {
        m_bundleName = name;
        TransferDataToWindow();
        }
    /// Selects the page with the given page ID.
    /// @param pageId The window ID of the page to select.
    void SelectPage(const wxWindowID pageId);

    // page IDs
    static constexpr int ID_GENERAL_PAGE = 20001;
    static constexpr int ID_STANDARD_TEST_PAGE = 20002;
    static constexpr int ID_VOCAB_PAGE = 20003;
    static constexpr int ID_CUSTOM_TEST_PAGE = 20004;
    static constexpr int ID_GOALS_PAGE = 20005;
    static constexpr int ID_ADD_TEST_GOALS_BUTTON = 30001;
    static constexpr int ID_DELETE_TEST_GOALS_BUTTON = 30002;
    static constexpr int ID_ADD_STAT_GOALS_BUTTON = 30003;
    static constexpr int ID_DELETE_STAT_GOALS_BUTTON = 30004;
private:
    void CreateControls();
    void OnOK([[maybe_unused]] wxCommandEvent& event);

    wxString m_bundleName;
    bool m_includeDolchSightWords{ false };
    TestBundle& m_testBundle;
    readability::readability_test_collection<> m_standardTests;
    CustomReadabilityTestCollection m_custom_tests;
    wxTextCtrl* m_bundleNameCtrl{ nullptr };
    wxTextCtrl* m_bundleDescriptionCtrl{ nullptr };
    ListCtrlEx* m_testGoalsListCtrl{ nullptr };
    ListCtrlEx* m_statGoalsListCtrl{ nullptr };
    Wisteria::UI::SideBarBook* m_sideBarBook{ nullptr };
    };

#endif //__TEST_BUNDLE_DLG_H__
