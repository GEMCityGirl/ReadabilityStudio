/** @addtogroup UI
    @brief Classes for the user interface.
    @date 2005-2023
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

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

/// @brief Test bundle editing dialog.
class TestBundleDlg final : public Wisteria::UI::DialogWithHelp
    {
public:
    /// @brief Constructor.
    /// @param parent The dialog's parent.
    /// @param testBundle The test bundle to edit.
    TestBundleDlg(wxWindow* parent, TestBundle& testBundle);
    /// @private
    TestBundleDlg() = delete;
    /// @private
    TestBundleDlg(const TestBundleDlg&) = delete;
    /// @private
    TestBundleDlg& operator=(const TestBundleDlg&) = delete;
    /// @brief Sets the bundle's name.
    /// @param name The bundle name to use.
    void SetTestBundleName(const wxString& name)
        {
        m_bundleName = name;
        TransferDataToWindow();
        }
    /// @brief Selects the page with the given page ID.
    /// @param pageId The window ID of the page to select.
    void SelectPage(const wxWindowID pageId);
private:
    void CreateControls();
    void OnOK([[maybe_unused]] wxCommandEvent& event);

    // page IDs
    static constexpr int ID_GENERAL_PAGE = wxID_HIGHEST;
    static constexpr int ID_STANDARD_TEST_PAGE = wxID_HIGHEST + 1;
    static constexpr int ID_VOCAB_PAGE = wxID_HIGHEST + 2;
    static constexpr int ID_CUSTOM_TEST_PAGE = wxID_HIGHEST + 3;
    static constexpr int ID_GOALS_PAGE = wxID_HIGHEST + 4;
    static constexpr int ID_ADD_TEST_GOALS_BUTTON = wxID_HIGHEST + 5;
    static constexpr int ID_DELETE_TEST_GOALS_BUTTON = wxID_HIGHEST + 6;
    static constexpr int ID_ADD_STAT_GOALS_BUTTON = wxID_HIGHEST + 7;
    static constexpr int ID_DELETE_STAT_GOALS_BUTTON = wxID_HIGHEST + 8;

    wxString m_bundleName;
    wxString m_descriptionName;
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


/** @}*/

#endif //__TEST_BUNDLE_DLG_H__
