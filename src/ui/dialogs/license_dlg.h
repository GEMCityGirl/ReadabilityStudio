#ifndef __WXLICENSE_DLG_H__
#define __WXLICENSE_DLG_H__

#include <wx/wx.h>
#include <wx/valgen.h>
#include <wx/html/htmlwin.h>

/// @brief Dialog to display an end-user license agreement.
class LicenseDlg : public wxDialog
    {
public:
    /// @brief Empty constructor.
    /// @sa Create().
    LicenseDlg() = default;
    /// Constructor.
    explicit LicenseDlg(wxWindow* parent, wxWindowID id = wxID_ANY,
                        const wxString& caption = _(L"License Agreement"),
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxSize(500, 600),
                        long style = wxRESIZE_BORDER | wxCAPTION)
        { Create(parent, id, caption, pos, size, style); }

    /** @brief Creation function.*/
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY,
                const wxString& caption = _(L"License Agreement"),
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = wxRESIZE_BORDER | wxCAPTION);

    /// @brief Sets the (formatted) text of the license agreement.
    /// @param content The EULA, formatted in HTML.
    void SetLicenseAggreement(const wxString& content)
        {
        m_licenseWindow->SetPage(content);
        }

private:

    void CreateControls();

    wxHtmlWindow* m_licenseWindow{ nullptr };
    };

#endif //__WXLICENSE_DLG_H__
