/** @addtogroup UI
    @brief Classes for the user interface.
    @date 2005-2023
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

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

/// @brief Dialog to display multiple word lists (read-only viewing).
class WordListDlg final : public wxDialog
    {
public:
    /** @brief Constructor.
        @param parent The dialog's parent.
        @param id The dialog's ID.
        @param caption The dialog's caption.
        @param pos The dialog's position.
        @param size The dialog's size.
        @param style The dialog's style.*/
    explicit WordListDlg(wxWindow* parent, wxWindowID id = wxID_ANY,
        const wxString& caption = _("Word Lists"), const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = wxCAPTION|wxCLOSE_BOX|wxRESIZE_BORDER);
    /// @private
    ~WordListDlg()
        {
        wxDELETE(m_DCData);
        wxDELETE(m_StockerData);
        wxDELETE(m_SpacheData);
        wxDELETE(m_HJData);
        wxDELETE(m_DolchData);
        }
    /// @private
    WordListDlg(const WordListDlg&) = delete;
    /// @private
    WordListDlg& operator=(const WordListDlg&) = delete;

    /// @brief Selects a word list page on the dialog.
    /// @param pageId The page (by ID) to select.
    void SelectPage(const int pageId);
    /// @brief The Dale-Chall page.
    constexpr static int DALE_CHALL_PAGE_ID = wxID_HIGHEST;
    /// @brief Stocker Catholic page.
    constexpr static int STOCKER_PAGE_ID = wxID_HIGHEST + 1;
    /// @brief The Spache page.
    constexpr static int SPACHE_PAGE_ID = wxID_HIGHEST + 2;
    /// @brief The Harris-Jacobson page.
    constexpr static int HARRIS_JACOBSON_PAGE_ID = wxID_HIGHEST + 3;
    /// @brief The Dolch page.
    constexpr static int DOLCH_PAGE_ID = wxID_HIGHEST + 4;
private:
    void CreateControls();
    [[nodiscard]]
    ListCtrlEx* GetActiveList();
    void OnFind(wxFindDialogEvent &event);
    void OnRibbonButton(wxRibbonButtonBarEvent& event);
    void OnNegative(wxCommandEvent& event);
    void OnClose([[maybe_unused]] wxCloseEvent& event);
    void AddSingleColumnPage(Wisteria::UI::SideBarBook* sideBar, const int id, const int listId,
                             const wxString& label, const int imageId,
                             ListCtrlExDataProvider* data, const word_list& wordList);

    constexpr static int DALE_CHALL_LIST_ID = wxID_HIGHEST + 5;
    constexpr static int STOCKER_LIST_ID = wxID_HIGHEST + 6;
    constexpr static int SPACHE_LIST_ID = wxID_HIGHEST + 7;
    constexpr static int HARRIS_JACOBSON_LIST_ID = wxID_HIGHEST + 8;
    constexpr static int DOLCH_LIST_ID = wxID_HIGHEST + 9;

    ListCtrlExDataProvider* m_DCData{ new ListCtrlExDataProvider };
    ListCtrlExDataProvider* m_StockerData{ new ListCtrlExDataProvider };
    ListCtrlExDataProvider* m_SpacheData{ new ListCtrlExDataProvider };
    ListCtrlExDataProvider* m_HJData{ new ListCtrlExDataProvider };
    ListCtrlExDataProvider* m_DolchData{ new ListCtrlExDataProvider };
    Wisteria::UI::SideBarBook* m_sideBar{ nullptr };
    };

/** @}*/

#endif //__WORD_LIST_DIALOG_H__
