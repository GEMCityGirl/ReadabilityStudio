/** @addtogroup UI
    @brief Classes for the user interface.
    @date 2005-2023
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __FILTEREDTEXTEXPORTOPTIONS_DLG_H__
#define __FILTEREDTEXTEXPORTOPTIONS_DLG_H__

#include "../../Wisteria-Dataviz/src/ui/dialogs/dialogwithhelp.h"
#include <wx/statline.h>
#include <wx/valgen.h>
#include <wx/wx.h>

/// @brief Provides options for exporting filtered (e.g., romanized) text.
class FilteredTextExportOptionsDlg : public Wisteria::UI::DialogWithHelp
    {
  public:
    /** @brief Constructor.
        @param parent The parent window.
        @param id The dialog's ID.
        @param caption The dialog's caption.
        @param pos The dialog's window position.
        @param size The dialog's size.
        @param style The dialog's style.*/
    explicit FilteredTextExportOptionsDlg(
        wxWindow* parent, wxWindowID id = wxID_ANY,
        const wxString& caption = _(L"Additional Filtering Options"),
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE | wxCLIP_CHILDREN)
        {
        Create(parent, id, caption, pos, size, style);
        }

    /// @brief Constructor.
    FilteredTextExportOptionsDlg() = default;
    /// @private
    FilteredTextExportOptionsDlg(const FilteredTextExportOptionsDlg& that) = delete;
    /// @private
    FilteredTextExportOptionsDlg& operator=(const FilteredTextExportOptionsDlg& that) = delete;

    /** @brief Creation.
        @param parent The parent window.
        @param id The dialog's ID.
        @param caption The dialog's caption.
        @param pos The dialog's window position.
        @param size The dialog's size.
        @param style The dialog's style.*/
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY,
                const wxString& caption = _(L"Additional Filtering Options"),
                const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_DIALOG_STYLE | wxCLIP_CHILDREN)
        {
        SetExtraStyle(GetExtraStyle() | wxWS_EX_BLOCK_EVENTS | wxWS_EX_CONTEXTHELP);
        Wisteria::UI::DialogWithHelp::Create(parent, id, caption, pos, size, style);

        CreateControls();
        Centre();
        return true;
        }

    /// @returns @c true if romanization is checked.
    [[nodiscard]]
    bool IsReplacingCharacters() const noexcept
        {
        return m_replaceCharacters;
        }

    /// @returns @c true if removing ellipses is checked.
    [[nodiscard]]
    bool IsRemovingEllipses() const noexcept
        {
        return m_removeEllipses;
        }

    /// @returns @c true if removing bullets is checked.
    [[nodiscard]]
    bool IsRemovingBullets() const noexcept
        {
        return m_removeBullets;
        }

    /// @returns @c true if removing filepaths is checked.
    [[nodiscard]]
    bool IsRemovingFilePaths() const noexcept
        {
        return m_removeFilePaths;
        }

    /// @returns @c true if remove periods from abbreviations is checked.
    [[nodiscard]]
    bool IsStrippingAbbreviations() const noexcept
        {
        return m_stripAbbreviationPeriods;
        }

    /// @returns @c true if narrowing wide characters is checked.
    [[nodiscard]]
    bool IsNarrowingFullWidthCharacters() const noexcept
        {
        return m_narrowFullWidthCharacters;
        }

  private:
    void CreateControls();

    bool m_replaceCharacters{ true };
    bool m_removeEllipses{ true };
    bool m_removeBullets{ true };
    bool m_removeFilePaths{ true };
    bool m_stripAbbreviationPeriods{ true };
    bool m_narrowFullWidthCharacters{ true };
    };

    /** @}*/

#endif //__FILTEREDTEXTEXPORTOPTIONS_DLG_H__
