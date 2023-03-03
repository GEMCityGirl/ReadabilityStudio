#ifndef __PROPGRID_WORDLIST_H__
#define __PROPGRID_WORDLIST_H__

#include <wx/wx.h>
#include <wx/propgrid/property.h>
#include <wx/propgrid/props.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/propgriddefs.h>
#include <wx/propgrid/manager.h>
#include <wx/propgrid/advprops.h>
#include "../dialogs/editwordlistdlg.h"

#define wxPG_HELP_PATH L"wxPG_HELP_PATH"
#define wxPG_TOPIC_PATH L"wxPG_TOPIC_PATH"
#define wxPG_PHRASE_MODE L"wxPG_PHRASE_MODE"

/// @brief Property grid feature for editing a word list.
class WXDLLIMPEXP_PROPGRID WordListProperty final : public wxLongStringProperty
    {
    wxDECLARE_DYNAMIC_CLASS(WordListProperty);
public:
    /** @brief Constructor.
        @param label The label to display in the property grid.
        @param name The property name.
        @param value The file path to the word list.*/
    WordListProperty(const wxString& label = wxPG_LABEL,
                     const wxString& name = wxPG_LABEL,
                     const wxString& value = wxString{});
    /// @private
    [[nodiscard]]
    wxValidator* DoGetValidator() const final;
    /// @private
    bool DoSetAttribute(const wxString& name, wxVariant& value) final;
    /// @private
    bool DisplayEditorDialog(wxPropertyGrid* pg, wxVariant& value) final;
private:
    wxString m_helpPath;
    wxString m_topicPath;
    bool m_phraseMode{ false };
    };

#endif //__PROPGRID_WORDLIST_H__
