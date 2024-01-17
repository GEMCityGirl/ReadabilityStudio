#include "license_dlg.h"

//-------------------------------------------------------------
bool LicenseDlg::Create(wxWindow* parent, wxWindowID id /*= wxID_ANY*/,
                              const wxString& caption /*= _("License Agreement")*/,
                              const wxPoint& pos /*= wxDefaultPosition*/,
                              const wxSize& size /*= wxDefaultSize*/,
                              long style /*= wxRESIZE_BORDER|wxCAPTION*/)
    {
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create(parent, id, caption, pos, size, style);

    CreateControls();

    Centre();
    return true;
    }

//-------------------------------------------------------------
void LicenseDlg::CreateControls()
    {
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->SetMinSize(FromDIP(wxSize(500, 600)));

    wxBoxSizer* controlsSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(controlsSizer, 1, wxEXPAND | wxALIGN_TOP | wxALL, 5);

    m_licenseWindow = new wxHtmlWindow(this);
    m_licenseWindow->SetWindowStyle(m_licenseWindow->GetWindowStyle() | wxBORDER_THEME);
    controlsSizer->Add(m_licenseWindow, 1, wxALL|wxEXPAND, 5);

    mainSizer->Add(CreateSeparatedButtonSizer(wxYES_NO), 0, wxEXPAND | wxALL, 5);
    SetEscapeId(wxID_NO);

    wxButton* okButton = dynamic_cast<wxButton*>(FindWindowById(wxID_YES));
    if (okButton)
        { okButton->SetLabel(_("I &agree")); }
    wxButton* cancelButton = dynamic_cast<wxButton*>(FindWindowById(wxID_NO));
    if (cancelButton)
        { cancelButton->SetLabel(_("I &do not agree")); }

    SetSizerAndFit(mainSizer);
    }
