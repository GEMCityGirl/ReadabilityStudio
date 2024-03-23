/** @addtogroup UI
    @brief Classes for the user interface.
    @date 2005-2023
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __EDIT_WORD_LIST_DLG_H__
#define __EDIT_WORD_LIST_DLG_H__

#include "../../Wisteria-Dataviz/src/import/text_matrix.h"
#include "../../Wisteria-Dataviz/src/import/text_preview.h"
#include "../../Wisteria-Dataviz/src/ui/controls/listctrlex.h"
#include "../../Wisteria-Dataviz/src/ui/dialogs/dialogwithhelp.h"
#include "../../Wisteria-Dataviz/src/util/memorymappedfile.h"
#include "../../indexing/character_traits.h"
#include <algorithm>
#include <string>
#include <vector>
#include <wx/artprov.h>
#include <wx/bmpbuttn.h>
#include <wx/combobox.h>
#include <wx/intl.h>
#include <wx/tooltip.h>
#include <wx/wx.h>

/// @brief A dialog to edit a word (or phrase) list.
class EditWordListDlg final : public Wisteria::UI::DialogWithHelp
    {
    EditWordListDlg() = default;

  public:
    /// @brief The string type that they dialog storing data as.
    using OutputStringType = traits::case_insensitive_wstring_ex;

    /// @brief Constructor.
    /// @param parent The parent window.
    /// @param id The dialog's ID.
    /// @param caption The dialog's caption.
    /// @param pos The dialog's position.
    /// @param size The dialog's size.
    /// @param style The dialog's style.
    explicit EditWordListDlg(wxWindow* parent, wxWindowID id = wxID_ANY,
                             const wxString& caption = _(L"Edit List"),
                             const wxPoint& pos = wxDefaultPosition,
                             const wxSize& size = wxSize(1000, 1000),
                             long style = wxDEFAULT_DIALOG_STYLE | wxCLIP_CHILDREN |
                                          wxRESIZE_BORDER);


    /// @brief Sets the path to the file that is being edited.
    /// @param path The path of the file.
    void SetFilePath(const wxString& path)
        {
        m_wordListFilePath = path;
        TransferDataToWindow();
        }

    /// @brief Sets The default directory to search in.
    /// @param path The default directory.
    void SetDefaultDir(const wxString& path) { m_defaultDir = path; }

    /// @returns The path to the file that is being edited.
    [[nodiscard]]
    const wxString& GetFilePath() const noexcept
        {
        return m_wordListFilePath;
        }

    /// @brief Sets whether to parse and edit the lines of text as
    ///     full-lines (phrases) or tokenize everything into words.
    /// @param isPhraseFileMode @c true to import as lines of text.
    void SetPhraseFileMode(const bool isPhraseFileMode) noexcept
        {
        m_phraseFileMode = isPhraseFileMode;
        }

    /// @brief Saves the list.
    /// @param filePath The filepath to save to.
    /// @returns @c true if successful.
    bool Save(const wxString& filePath);
    /// @private
    constexpr static int ID_FILE_PATH_FIELD = wxID_HIGHEST;

  private:
    constexpr static int ID_BROWSE_FOR_FILE = wxID_HIGHEST + 1;
    constexpr static int ID_ADD_ITEM = wxID_HIGHEST + 2;
    constexpr static int ID_EDIT_ITEM = wxID_HIGHEST + 3;
    constexpr static int ID_DELETE_ITEM = wxID_HIGHEST + 4;
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY,
                const wxString& caption = _(L"Edit List"), const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxSize(600, 1000),
                long style = wxDEFAULT_DIALOG_STYLE | wxCLIP_CHILDREN | wxRESIZE_BORDER);
    void CreateControls();

    // user is selecting a different word list file
    void OnBrowseForFileClick([[maybe_unused]] wxCommandEvent& event);
    void OnFilePathChanged(wxCommandEvent& event);
    void OnOK([[maybe_unused]] wxCommandEvent& event);
    void OnClose([[maybe_unused]] wxCommandEvent& event);
    void OnAddItem([[maybe_unused]] wxCommandEvent& event);
    void OnEditItem([[maybe_unused]] wxCommandEvent& event);
    void OnDeleteItem([[maybe_unused]] wxCommandEvent& event);

    std::shared_ptr<ListCtrlExDataProvider> m_wordData{ std::make_shared<ListCtrlExDataProvider>() };
    ListCtrlEx* m_wordsList{ nullptr };
    wxTextCtrl* m_wordListFilePathCtrl{ nullptr };
    wxString m_wordListFilePath;
    wxString m_previousListFilePath;
    wxString m_defaultDir;
    bool m_phraseFileMode{ false };
    };

    /** @}*/

#endif //__EDIT_WORD_LIST_DLG_H__
