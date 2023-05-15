/** @addtogroup UI
    @brief Classes for the user interface.
    @date 2005-2023
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __FILTEREDTEXTPREVIEWDLG_DIALOG_H__
#define __FILTEREDTEXTPREVIEWDLG_DIALOG_H__

#include <wx/wx.h>
#include <wx/valgen.h>
#include <wx/collpane.h>
#include "../../Wisteria-Dataviz/src/ui/dialogs/dialogwithhelp.h"
#include "../../app/optionenums.h"

/// @brief Dialog for previewing filtered (i.e., romanized) text.
class FilteredTextPreviewDlg final : public Wisteria::UI::DialogWithHelp
    {
public:
    /** @brief Constructor.
        @param parent The parent window.
        @param textExclusionMethod The exclusion method.
        @param excludeCopyrightNotices Whether to remove copyright notices.
        @param excludeCitations Whether to remove citations.
        @param replaceCharacters Whether to romanize.
        @param removeEllipses Whether to remove ellipses.
        @param removeBullets Whether to remove bullets.
        @param removeFilePaths Whether to remove filepaths.
        @param stripAbbreviationPeriods Whether to remove abbreviations' periods.
        @param id The dialog's ID.
        @param caption The dialog's caption.
        @param pos The dialog's window position.
        @param size The dialog's size.
        @param style The dialog's style.*/
    FilteredTextPreviewDlg(wxWindow* parent,
             const InvalidSentence textExclusionMethod,
             const bool excludeCopyrightNotices,
             const bool excludeCitations,
             const bool replaceCharacters,
             const bool removeEllipses,
             const bool removeBullets,
             const bool removeFilePaths,
             const bool stripAbbreviationPeriods,
             wxWindowID id = wxID_ANY,
             const wxString& caption = _(L"Preview Filtered Document"),
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)
        {
        SetExclusionInfo(textExclusionMethod, excludeCopyrightNotices, excludeCitations,
                         replaceCharacters, removeEllipses, removeBullets,
                         removeFilePaths, stripAbbreviationPeriods);
        Create(parent, id, caption, pos, size, style);
        }
    /// @private
    FilteredTextPreviewDlg(const FilteredTextPreviewDlg& that) = delete;
    /// @private
    FilteredTextPreviewDlg& operator=(const FilteredTextPreviewDlg& that) = delete;
    /** @brief Sets the text for the preview window.
        @param text The preview text to use.
        @todo Use move semantics.*/
    void SetFilteredValue(const wxString& text)
        {
        m_filteredValue = text;
        TransferDataToWindow();
        }
    /// @brief Expands the details window beneath the preview.
    /// @param show @c true to expand it, @c false to collapse it.
    void ShowDetails(const bool show = true)
        {
        if (m_collpane)
            {
            if (show)
                { m_collpane->Expand(); }
            else
                { m_collpane->Collapse(); }
            }
        }
private:
    /// @brief Creation.
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY,
                const wxString& caption = wxEmptyString, const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE)
        {
        SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
        DialogWithHelp::Create(parent, id, caption, pos, size, style );

        CreateControls();
        Centre();
        m_collpane->Collapse();

        return true;
        }

    /// @brief Creates the controls and sizers.
    void CreateControls();

    void SetExclusionInfo(const InvalidSentence textExclusionMethod,
         const bool excludeCopyrightNotices,
         const bool excludeCitations,
         const bool replaceCharacters,
         const bool removeEllipses,
         const bool removeBullets,
         const bool removeFilePaths,
         const bool stripAbbreviationPeriods);

    wxCollapsiblePane* m_collpane{ nullptr };

    wxString m_infoLabel;
    wxString m_filteredValue;
    };

/** @}*/

#endif //__FILTEREDTEXTPREVIEWDLG_DIALOG_H__
