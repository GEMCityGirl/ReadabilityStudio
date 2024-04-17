///////////////////////////////////////////////////////////////////////////////
// Name:        project_wizard_dlg.cpp
// Author:      Blake Madden
// Copyright:   (c) 2005-2023 Blake Madden
// Licence:     3-Clause BSD licence
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "project_wizard_dlg.h"

wxDECLARE_APP(ReadabilityApp);

using namespace Wisteria;
using namespace Wisteria::GraphItems;
using namespace Wisteria::UI;

wxString ProjectWizardDlg::m_lastSelectedFolder;

class Banner : public wxWindow
    {
  public:
    Banner(wxWindow* parent, wxWindowID id, const wxBitmapBundle& logo, const wxString& label)
        : wxWindow(parent, id, wxDefaultPosition, wxSize(300, 50), wxFULL_REPAINT_ON_RESIZE),
          m_logo(logo), m_label(label)
        {
        SetMinSize(FromDIP(wxSize(300, 50)));
        SetBackgroundStyle(wxBG_STYLE_CUSTOM);
        Bind(wxEVT_PAINT, &Banner::OnPaint, this);
        }

    void OnPaint([[maybe_unused]] wxPaintEvent& event)
        {
        wxAutoBufferedPaintDC adc(this);
        adc.Clear();
        wxGCDC dc(adc);

        wxCoord textWidth, textHeight;
        dc.GetTextExtent(m_label, &textWidth, &textHeight);

        wxBitmap logo = m_logo.GetBitmap(FromDIP(wxSize(32, 32))).ConvertToImage();

        const wxCoord leftBorder =
            (GetClientSize().GetWidth() / 2) - ((logo.GetWidth() / 2) + (textWidth / 2) + 3);

        dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
        dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
        dc.SetBackground(*wxTRANSPARENT_BRUSH);
        dc.Clear();
        dc.DrawBitmap(logo, leftBorder, (GetClientSize().GetHeight() / 2) - (logo.GetHeight() / 2));
        dc.DrawText(m_label, leftBorder + logo.GetWidth() + 6,
                    (GetClientSize().GetHeight() / 2) - (textHeight / 2));
        dc.DrawLine((GetClientSize().GetWidth() / 10), GetClientSize().GetHeight() - 1,
                    GetClientSize().GetWidth() - (GetClientSize().GetWidth() / 10),
                    GetClientSize().GetHeight() - 1);
        }

    void SetLogo(const wxBitmapBundle& logo) { m_logo = logo; }

    void SetLabel(const wxString& label) final { m_label = label; }

  private:
    wxBitmapBundle m_logo;
    wxString m_label;
    };

//-------------------------------------------------------------
ProjectWizardDlg::ProjectWizardDlg(wxWindow* parent, const ProjectType projectType,
    const wxString& path /*= wxString{}*/, wxWindowID id /*= wxID_ANY*/,
    const wxString& caption /*= _(L"New Project Wizard")*/,
    const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/,
    long style /*= wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER*/,
    ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode fileTruncMode /*=
        ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode::NoTruncation*/) :
    m_minDocWordCountForBatch(wxGetApp().GetAppOptions().GetMinDocWordCountForBatch()),
    m_fromFileSelected(wxGetApp().GetAppOptions().GetTextSource() == TextSource::FromFile),
    m_manualSelected(wxGetApp().GetAppOptions().GetTextSource() == TextSource::EnteredText),
    m_testSelectionMethod(static_cast<int>(wxGetApp().GetAppOptions().GetTestRecommendation())),
    m_selectedDocType(static_cast<int>(wxGetApp().GetAppOptions().GetTestByDocumentType())),
    m_selectedIndustryType(static_cast<int>(wxGetApp().GetAppOptions().GetTestByIndustry())),
    m_includeDolchSightWords(wxGetApp().GetAppOptions().IsDolchSelected()),
    m_readabilityTests(wxGetApp().GetAppOptions().GetReadabilityTests()),
    m_projectType(projectType),
    m_selectedLang(static_cast<int>(wxGetApp().GetAppOptions().GetProjectLanguage())),
    m_fileListTruncationMode(fileTruncMode)
    {
    SetExtraStyle(GetExtraStyle() | wxWS_EX_VALIDATE_RECURSIVELY | wxWS_EX_CONTEXTHELP);
    wxDialog::Create(parent, id, caption, pos, size, style);
    // determine whether a file path or raw text was passed in
    if (path.length())
        {
        FilePathResolver resolvePath(path, false);
        if (resolvePath.IsInvalidFile())
            {
            m_fromFileSelected = false;
            m_manualSelected = true;
            m_enteredText = path;
            }
        else
            {
            m_fromFileSelected = true;
            m_manualSelected = false;
            m_filePath = path;
            }
        }

    CreateControls();
    Centre();

    if (wxGetApp().GetAppOptions().GetInvalidSentenceMethod() ==
        InvalidSentence::IncludeAsFullSentences)
        {
        SetFragmentedTextSelected();
        }
    else
        {
        SetNarrativeSelected();
        }
    SetSplitLinesSelected(wxGetApp().GetAppOptions().GetIgnoreBlankLinesForParagraphsParser());
    // ignoring indenting doesn't make sense if each line should start a new paragraph
    SetCenteredTextSelected(wxGetApp().GetAppOptions().GetParagraphsParsingMethod() !=
                                ParagraphParse::EachNewLineIsAParagraph &&
                            wxGetApp().GetAppOptions().GetIgnoreIndentingForParagraphsParser());
    SetNewLinesAlwaysNewParagraphsSelected(
        wxGetApp().GetAppOptions().GetParagraphsParsingMethod() ==
        ParagraphParse::EachNewLineIsAParagraph);

    Bind(wxEVT_BUTTON, &ProjectWizardDlg::OnGroupClick, this, ProjectWizardDlg::ID_GROUP_BUTTON);
    Bind(wxEVT_BUTTON, &ProjectWizardDlg::OnHelp, this, wxID_HELP);
    Bind(wxEVT_BUTTON, &ProjectWizardDlg::OnNavigate, this, wxID_FORWARD);
    Bind(wxEVT_BUTTON, &ProjectWizardDlg::OnNavigate, this, wxID_BACKWARD);
    Bind(wxEVT_BUTTON, &ProjectWizardDlg::OnOK, this, wxID_OK);
    Bind(wxEVT_BUTTON, &ProjectWizardDlg::OnFileBrowseButtonClick, this,
         ProjectWizardDlg::ID_FILE_BROWSE_BUTTON);
    Bind(wxEVT_BUTTON, &ProjectWizardDlg::OnButtonClick, this,
         ProjectWizardDlg::NARRATIVE_WITH_ILLUSTRATIONS_LINK_ID);
    Bind(wxEVT_BUTTON, &ProjectWizardDlg::OnButtonClick, this,
         ProjectWizardDlg::FRAGMENTED_LINK_ID);
    Bind(wxEVT_BUTTON, &ProjectWizardDlg::OnButtonClick, this,
         ProjectWizardDlg::CENTERED_TEXT_LINK_ID);
    Bind(wxEVT_BUTTON, &ProjectWizardDlg::OnAddFolderButtonClick, this,
         ProjectWizardDlg::ID_BATCH_FOLDER_BROWSE_BUTTON);
    Bind(wxEVT_BUTTON, &ProjectWizardDlg::OnAddFileButtonClick, this,
         ProjectWizardDlg::ID_BATCH_FILE_BROWSE_BUTTON);
    Bind(wxEVT_BUTTON, &ProjectWizardDlg::OnAddWebPagesButtonClick, this,
         ProjectWizardDlg::ID_WEB_PAGES_BROWSE_BUTTON);
    Bind(wxEVT_BUTTON, &ProjectWizardDlg::OnAddWebPageButtonClick, this,
         ProjectWizardDlg::ID_WEB_PAGE_BROWSE_BUTTON);
    Bind(wxEVT_BUTTON, &ProjectWizardDlg::OnAddArchiveFileButtonClick, this,
         ProjectWizardDlg::ID_ARCHIVE_FILE_BROWSE_BUTTON);
    Bind(wxEVT_BUTTON, &ProjectWizardDlg::OnAddSpreadsheetFileButtonClick, this,
         ProjectWizardDlg::ID_SPREADSHEET_FILE_BROWSE_BUTTON);
    Bind(wxEVT_BUTTON, &ProjectWizardDlg::OnAddToListClick, this,
         ProjectWizardDlg::ID_ADD_FILE_BUTTON);
    Bind(wxEVT_BUTTON, &ProjectWizardDlg::OnDeleteFromListClick, this,
         ProjectWizardDlg::ID_DELETE_FILE_BUTTON);

    Bind(wxEVT_CHECKBOX, &ProjectWizardDlg::OnRandomSampleCheck, this,
         ProjectWizardDlg::ID_RANDOM_SAMPLE_CHECK);
    Bind(wxEVT_CHOICE, &ProjectWizardDlg::OnLanguageChanged, this,
         ProjectWizardDlg::LANGUAGE_BUTTON);
    Bind(wxEVT_RADIOBUTTON, &ProjectWizardDlg::OnSourceRadioChange, this,
         ProjectWizardDlg::ID_FROM_FILE_BUTTON);
    Bind(wxEVT_RADIOBUTTON, &ProjectWizardDlg::OnSourceRadioChange, this,
         ProjectWizardDlg::ID_MANUALLY_ENTERED_TEXT_BUTTON);
    Bind(wxEVT_RADIOBOX, &ProjectWizardDlg::OnTestSelectionMethodChanged, this,
         ProjectWizardDlg::TEST_SELECT_METHOD_BUTTON);

    Bind(wxEVT_SIDEBARBOOK_PAGE_CHANGED, &ProjectWizardDlg::OnPageChange, this);

    Bind(wxEVT_HELP, &ProjectWizardDlg::OnContextHelp, this);

    Bind(
        wxEVT_CHAR_HOOK,
        [this](wxKeyEvent& event)
        {
            if (event.ControlDown() && event.GetKeyCode() == L'G')
                {
                wxRibbonButtonBarEvent dummyEvt;
                OnGroupClick(dummyEvt);
                }
            else
                {
                event.Skip(true);
                }
        },
        wxID_ANY);
    }

//-------------------------------------------------------------
void ProjectWizardDlg::CreateControls()
    {
    const int ScaledNoteWidth = FromDIP(wxSize(500, 500)).GetWidth();

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    m_sideBarBook = new SideBarBook(this, wxID_ANY);
    wxGetApp().UpdateSideBarTheme(m_sideBarBook->GetSideBar());
    mainSizer->Add(m_sideBarBook, 1, wxEXPAND | wxALL, wxSizerFlags::GetDefaultBorder());

    m_sideBarBook->GetImageList().push_back(wxArtProvider::GetBitmapBundle(L"ID_DOCUMENT"));
    m_sideBarBook->GetImageList().push_back(
        wxGetApp().GetResourceManager().GetSVG(L"ribbon/document-structure.svg"));
    m_sideBarBook->GetImageList().push_back(
        wxGetApp().GetResourceManager().GetSVG(L"tests/flesch-test.svg"));

    // document page
    if (GetProjectType() == ProjectType::StandardProject)
        {
        wxPanel* page =
            new wxPanel(m_sideBarBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
        wxBoxSizer* pageSizer = new wxBoxSizer(wxVERTICAL);
        page->SetSizer(pageSizer);
        m_sideBarBook->AddPage(page, _(L"Document"), wxID_ANY, true, 0);

        // The options
        wxBoxSizer* optionsSizer = new wxBoxSizer(wxVERTICAL);

        Banner* banner = new Banner(
            page, wxID_ANY,
            wxArtProvider::GetBitmap(L"ID_DOCUMENT", wxART_BUTTON, FromDIP(wxSize(32, 32))),
            _(L"Select Document"));
        optionsSizer->Add(banner, 0, wxEXPAND | wxBOTTOM, wxSizerFlags::GetDefaultBorder());

        // select the language
        if (wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureLanguagePackCode()) ||
            wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureProfessionalCode()))
            {
            // do not sort, connected to an enumeration
            wxArrayString langs;
            langs.Add(_(L"English"));
            langs.Add(_(L"Spanish"));
            langs.Add(_(L"German"));
            wxBoxSizer* langSizer = new wxBoxSizer(wxHORIZONTAL);
            langSizer->Add(new wxStaticText(page, wxID_STATIC, _(L"Document language:")), 0,
                           wxRIGHT | wxALIGN_CENTRE, wxSizerFlags::GetDefaultBorder());
            langSizer->Add(new wxChoice(page, LANGUAGE_BUTTON, wxDefaultPosition, wxDefaultSize,
                                        langs, 0, wxGenericValidator(&m_selectedLang)),
                           0, wxALIGN_LEFT | wxALL, wxSizerFlags::GetDefaultBorder());
            optionsSizer->Add(langSizer);
            }

        // file path
        optionsSizer->Add(new wxRadioButton(
            page, ID_FROM_FILE_BUTTON, _(L"&Read text from a file or webpage:"), wxDefaultPosition,
            wxDefaultSize, wxRB_GROUP, wxGenericValidator(&m_fromFileSelected)));
        optionsSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

        wxBoxSizer* fileBrowseBoxSizer = new wxBoxSizer(wxHORIZONTAL);
        optionsSizer->Add(fileBrowseBoxSizer, wxSizerFlags().Expand().Border(
                                                  wxLEFT, wxSizerFlags::GetDefaultBorder() * 3));

        m_filePathEdit =
            new wxTextCtrl(page, wxID_ANY, wxString{}, wxDefaultPosition, wxDefaultSize,
                           wxTE_RICH2 | wxBORDER_THEME, wxGenericValidator(&m_filePath));
        m_filePathEdit->AutoCompleteFileNames();
        fileBrowseBoxSizer->Add(m_filePathEdit, 1, wxEXPAND);

        m_fileBrowseButton =
            new wxBitmapButton(page, ID_FILE_BROWSE_BUTTON,
                               wxArtProvider::GetBitmapBundle(wxART_FILE_OPEN, wxART_BUTTON));
        m_fileBrowseButton->SetHelpText(
            _(L"Click this button to browse for the file that you want to analyze."));
        fileBrowseBoxSizer->Add(m_fileBrowseButton, 0, wxRIGHT, wxSizerFlags::GetDefaultBorder());
        optionsSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

        // Manual text entry
        optionsSizer->Add(new wxRadioButton(
            page, ID_MANUALLY_ENTERED_TEXT_BUTTON, _(L"Manually enter text:"), wxDefaultPosition,
            wxDefaultSize, 0, wxGenericValidator(&m_manualSelected)));

        m_textEntryEdit =
            new wxTextCtrl(page, wxID_ANY, wxString{}, wxDefaultPosition, wxDefaultSize,
                           wxTE_MULTILINE | wxTE_RICH2, wxGenericValidator(&m_enteredText));
        optionsSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
        wxBoxSizer* editBoxSizer = new wxBoxSizer(wxHORIZONTAL);
        editBoxSizer->Add(m_textEntryEdit, wxSizerFlags().Proportion(1).Expand().Border(
                                               wxLEFT, wxSizerFlags::GetDefaultBorder() * 3));
        optionsSizer->Add(editBoxSizer, 1, wxEXPAND | wxRIGHT, wxSizerFlags::GetDefaultBorder());

        if (m_fromFileSelected)
            {
            m_textEntryEdit->Disable();
            }
        else
            {
            m_filePathEdit->Disable();
            m_fileBrowseButton->Disable();
            }

        if (wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureLanguagePackCode()) ||
            wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureProfessionalCode()))
            {
            wxStaticText* noteLabel = new wxStaticText(
                page, wxID_STATIC,
                _(L"The selected language will affect syllable counting and determine which "
                  "tests and grammar features will be made available."));
            optionsSizer->Add(noteLabel, 0, wxALIGN_LEFT | wxALL, wxSizerFlags::GetDefaultBorder());
            }

        pageSizer->Add(optionsSizer, 1, wxEXPAND | wxALL, wxSizerFlags::GetDefaultBorder());
        }
    else // Batch project
        {
        wxPanel* page =
            new wxPanel(m_sideBarBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
        wxBoxSizer* pageSizer = new wxBoxSizer(wxVERTICAL);
        page->SetSizer(pageSizer);
        m_sideBarBook->AddPage(page, _(L"Documents"), wxID_ANY, true, 0);

        // The options
        wxBoxSizer* optionsSizer = new wxBoxSizer(wxVERTICAL);

        Banner* banner = new Banner(
            page, wxID_ANY,
            wxArtProvider::GetBitmap(L"ID_DOCUMENT", wxART_BUTTON, FromDIP(wxSize(32, 32))),
            _(L"Select Documents"));
        optionsSizer->Add(banner, 0, wxEXPAND | wxBOTTOM, wxSizerFlags::GetDefaultBorder());

        // select the language
        if (wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureLanguagePackCode()) ||
            wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureProfessionalCode()))
            {
            wxArrayString langs;
            langs.Add(_(L"English"));
            langs.Add(_(L"Spanish"));
            langs.Add(_(L"German"));
            wxBoxSizer* langSizer = new wxBoxSizer(wxHORIZONTAL);
            langSizer->Add(new wxStaticText(page, wxID_STATIC, _(L"Documents' language:")), 0,
                           wxRIGHT | wxALIGN_CENTRE, wxSizerFlags::GetDefaultBorder());
            langSizer->Add(new wxChoice(page, LANGUAGE_BUTTON, wxDefaultPosition, wxDefaultSize,
                                        langs, 0, wxGenericValidator(&m_selectedLang)),
                           0, wxALIGN_LEFT | wxALL, wxSizerFlags::GetDefaultBorder());
            optionsSizer->Add(langSizer);
            }

        auto buttonsSizer = new wxGridSizer(
            4, wxSize(wxSizerFlags::GetDefaultBorder(), wxSizerFlags::GetDefaultBorder()));

        wxButton* button = new wxButton(page, ID_BATCH_FOLDER_BROWSE_BUTTON, _(L"&Add folder..."));
        button->SetBitmap(wxArtProvider::GetBitmapBundle(wxART_FILE_OPEN, wxART_BUTTON));
        buttonsSizer->Add(button, wxSizerFlags().Align(wxALIGN_LEFT).Expand());

        button = new wxButton(page, ID_BATCH_FILE_BROWSE_BUTTON, _(L"&Add file(s)..."));
        button->SetBitmap(wxArtProvider::GetBitmapBundle(L"ID_DOCUMENTS", wxART_BUTTON));
        buttonsSizer->Add(button, wxSizerFlags().Align(wxALIGN_LEFT).Expand());

        button = new wxButton(page, ID_ARCHIVE_FILE_BROWSE_BUTTON, _(L"&Add archive..."));
        button->SetBitmap(wxArtProvider::GetBitmapBundle(L"ID_ARCHIVE", wxART_BUTTON));
        buttonsSizer->Add(button, wxSizerFlags().Align(wxALIGN_LEFT).Expand());

        button = new wxButton(page, ID_SPREADSHEET_FILE_BROWSE_BUTTON, _(L"&Add spreadsheet..."));
        button->SetBitmap(wxArtProvider::GetBitmapBundle(L"ID_SPREADSHEET", wxART_BUTTON));
        buttonsSizer->Add(button, wxSizerFlags().Align(wxALIGN_LEFT).Expand());

        button = new wxButton(page, ID_WEB_PAGES_BROWSE_BUTTON, _(L"&Add web pages..."));
        button->SetBitmap(wxArtProvider::GetBitmapBundle(L"ID_WEB_EXPORT", wxART_BUTTON));
        buttonsSizer->Add(button, wxSizerFlags().Align(wxALIGN_LEFT).Expand());

        button = new wxButton(page, ID_WEB_PAGE_BROWSE_BUTTON, _(L"&Add web page..."));
        button->SetBitmap(wxArtProvider::GetBitmapBundle(L"ID_WEB_EXPORT", wxART_BUTTON));
        buttonsSizer->Add(button, wxSizerFlags().Align(wxALIGN_LEFT).Expand());

        optionsSizer->Add(buttonsSizer);

        // add and remove buttons for file grid
        wxBoxSizer* filesButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
        auto addFileButton = new wxBitmapButton(
            page, ID_ADD_FILE_BUTTON, wxArtProvider::GetBitmapBundle(L"ID_ADD", wxART_BUTTON));
        addFileButton->SetToolTip(_(L"Add a document"));
        filesButtonsSizer->Add(addFileButton);

        auto deleteFileButton =
            new wxBitmapButton(page, ID_DELETE_FILE_BUTTON,
                               wxArtProvider::GetBitmapBundle(wxART_DELETE, wxART_BUTTON));
        deleteFileButton->SetToolTip(_(L"Remove selected document"));
        filesButtonsSizer->Add(deleteFileButton);

        auto groupButton = new wxBitmapButton(
            page, ID_GROUP_BUTTON, wxArtProvider::GetBitmapBundle(L"ID_GROUP", wxART_BUTTON));
        groupButton->SetToolTip(_(L"Group selected documents"));
        filesButtonsSizer->Add(groupButton);

        optionsSizer->Add(filesButtonsSizer, 0, wxALIGN_RIGHT);

        if (GetFilePath().length())
            {
            FilePathResolver rp(GetFilePath(), false);
            // if page is created with a default folder or file then add it to the list
            if (wxFileName::DirExists(GetFilePath()))
                {
                wxBusyCursor wait;
                wxArrayString files;
                wxDir::GetAllFiles(GetFilePath(), &files, wxString{}, wxDIR_FILES | wxDIR_DIRS);
                files =
                    FilterFiles(files, wxGetApp().GetAppOptions().ALL_DOCUMENTS_WILDCARD.data());

                m_fileData->SetSize(files.GetCount(), 2);
                m_fileData->SetValues(files);
                }
            else if (FilePathResolver::IsSpreadsheet(GetFilePath()) &&
                     wxFileName::FileExists(GetFilePath()))
                {
                LoadSpreadsheet(GetFilePath());
                }
            else if (FilePathResolver::IsArchive(GetFilePath()) &&
                     wxFileName::FileExists(GetFilePath()))
                {
                LoadArchive(GetFilePath());
                }
            else
                {
                m_fileData->SetSize(1, 2);
                m_fileData->SetItemText(0, 0, GetFilePath());
                }
            }
        else
            {
            m_fileData->SetSize(0, 2);
            }
        m_fileList =
            new ListCtrlEx(page, wxID_ANY, wxDefaultPosition, FromDIP(wxSize(200, 150)),
                           wxLC_VIRTUAL | wxLC_EDIT_LABELS | wxLC_REPORT | wxLC_ALIGN_LEFT);
        m_fileList->EnableGridLines();
        m_fileList->EnableItemDeletion();
        m_fileList->InsertColumn(0, _(L"Files"));
        m_fileList->InsertColumn(1, _(L"Labels"));
        m_fileList->SetColumnEditable(0);
        m_fileList->SetColumnEditable(1);
        m_fileList->SetColumnFilePathTruncationMode(0, m_fileListTruncationMode);
        m_fileList->SetVirtualDataProvider(m_fileData);
        m_fileList->SetVirtualDataSize(m_fileData->GetItemCount());
        optionsSizer->Add(m_fileList, 2, wxALIGN_LEFT | wxEXPAND | wxALL,
                          wxSizerFlags::GetDefaultBorder());

        // min word count
        wxBoxSizer* minDocSizeBoxSizer = new wxBoxSizer(wxHORIZONTAL);

        wxStaticText* minDocSizeLabel =
            new wxStaticText(page, wxID_STATIC, _(L"Minimum document word count:"),
                             wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
        minDocSizeBoxSizer->Add(minDocSizeLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT,
                                wxSizerFlags::GetDefaultBorder());

        wxSpinCtrl* minDocWordCountForBatchSpinCtrl = new wxSpinCtrl(
            page, wxID_ANY, std::to_wstring(m_minDocWordCountForBatch), wxDefaultPosition,
            wxDefaultSize, wxSP_ARROW_KEYS, 1, std::numeric_limits<int>::max(), 0);
        minDocWordCountForBatchSpinCtrl->SetValidator(
            wxGenericValidator(&m_minDocWordCountForBatch));
        minDocSizeBoxSizer->Add(minDocWordCountForBatchSpinCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL,
                                0);

        // random sampling
        wxBoxSizer* randomOptionsSizer = new wxBoxSizer(wxHORIZONTAL);
        m_isRandomSampling = new wxCheckBox(page, ID_RANDOM_SAMPLE_CHECK, _(L"Randomly sample "));
        m_isRandomSampling->SetValue(false);

        m_randPercentageCtrl = new wxSpinCtrl(
            page, ID_RANDOM_SAMPLE_SPIN,
            std::to_wstring(wxGetApp().GetAppOptions().GetBatchRandomSamplingSize()));
        m_randPercentageCtrl->SetRange(1, 100);
        m_randPercentageCtrl->Enable(false);

        randomOptionsSizer->Add(m_isRandomSampling, 0, wxALIGN_LEFT | wxEXPAND, 0);
        randomOptionsSizer->Add(m_randPercentageCtrl, 0, wxALIGN_LEFT | wxEXPAND, 0);
        randomOptionsSizer->Add(
            new wxStaticText(page, ID_RANDOM_SAMPLE_LABEL, _(L"% of the documents.")), 0,
            wxALIGN_CENTER_VERTICAL, 0);

        pageSizer->Add(optionsSizer, 1, wxEXPAND | wxALL, wxSizerFlags::GetDefaultBorder());
        pageSizer->Add(minDocSizeBoxSizer, 0, wxALIGN_LEFT | wxEXPAND | wxLEFT,
                       wxSizerFlags::GetDefaultBorder());
        pageSizer->Add(randomOptionsSizer, 0, wxALIGN_LEFT | wxEXPAND | wxALL,
                       wxSizerFlags::GetDefaultBorder());
        }
        // document structure
        {
        wxFont imageFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
        imageFont.MakeBold().MakeLarger().MakeLarger();
        Wisteria::GraphItems::Label imageLabel(GraphItemInfo()
                                                   .DPIScaling(GetDPIScaleFactor())
                                                   .Pen(wxPen(wxColour(L"#BCE8F1"), 2))
                                                   .Font(imageFont)
                                                   .Padding(4, 4, 4, 4));
        imageLabel.SetFontBackgroundColor(wxColour(L"#D9EDF7"));
        imageLabel.SetFontColor(wxColour(L"#31708F"));
        imageLabel.SetTextAlignment(Wisteria::TextAlignment::FlushLeft);
        imageLabel.SetAnchoring(Wisteria::Anchoring::TopRightCorner);

        if (wxGetApp().GetAppOptions().GetInvalidSentenceMethod() ==
            InvalidSentence::IncludeAsFullSentences)
            {
            SetFragmentedTextSelected();
            }
        else
            {
            SetNarrativeSelected();
            }
        SetSplitLinesSelected(wxGetApp().GetAppOptions().GetIgnoreBlankLinesForParagraphsParser());
        SetCenteredTextSelected(wxGetApp().GetAppOptions().GetIgnoreIndentingForParagraphsParser());

        wxPanel* page =
            new wxPanel(m_sideBarBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
        wxBoxSizer* pageSizer = new wxBoxSizer(wxVERTICAL);
        page->SetSizer(pageSizer);
        m_sideBarBook->AddPage(page, _(L"Document Structure"), wxID_ANY, false, 1);

        // the options
        wxBoxSizer* optionsSizer = new wxBoxSizer(wxVERTICAL);

        Banner* banner =
            new Banner(page, wxID_ANY,
                       wxGetApp().GetResourceManager().GetSVG(L"ribbon/document-structure.svg"),
                       _(L"Specify Document Structure"));
        optionsSizer->Add(banner, 0, wxEXPAND | wxBOTTOM, wxSizerFlags::GetDefaultBorder());

        wxStaticBoxSizer* docTypeSizer = new wxStaticBoxSizer(wxVERTICAL, page, _(L"Composition"));
        wxStaticBoxSizer* docLayoutSizer = new wxStaticBoxSizer(wxVERTICAL, page, _(L"Layout"));
        optionsSizer->Add(docTypeSizer, 0, wxEXPAND | wxLEFT | wxRIGHT,
                          wxSizerFlags::GetDefaultBorder());
        optionsSizer->Add(docLayoutSizer, 0, wxEXPAND | wxLEFT | wxRIGHT,
                          wxSizerFlags::GetDefaultBorder());

            // narrative text
            {
            wxBoxSizer* narrativeSizer = new wxBoxSizer(wxHORIZONTAL);
            wxBoxSizer* narrativeLablesSizer = new wxBoxSizer(wxVERTICAL);
            wxRadioButton* narrativeRadioButton =
                new wxRadioButton(docTypeSizer->GetStaticBox(), ID_NARRATIVE_RADIO_BUTTON,
                                  _(L"&Narrative text"), wxDefaultPosition, wxDefaultSize,
                                  wxRB_GROUP, wxGenericValidator(&m_narrativeSelected));
            narrativeLablesSizer->Add(narrativeRadioButton);
            wxStaticText* noteLabel = new wxStaticText(
                docTypeSizer->GetStaticBox(), ID_NARRATIVE_LABEL,
                _(L"Document contains flowing sentences and paragraphs. "
                  "Items such as headers and list items are not part of the narrative text and "
                  "should be ignored."),
                wxDefaultPosition, wxDefaultSize, 0);
            noteLabel->Wrap(ScaledNoteWidth);
            narrativeLablesSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
            narrativeLablesSizer->Add(noteLabel, 1, wxLEFT, wxSizerFlags::GetDefaultBorder() * 3);
            narrativeLablesSizer->SetMinSize(ScaledNoteWidth, -1);
            narrativeSizer->Add(narrativeLablesSizer, 0, wxALIGN_CENTER);

            narrativeSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
            wxBitmap previewImage = wxGetApp().GetScaledImage(
                L"wizard/Narrative.png", wxBITMAP_TYPE_PNG,
                wxSize(wxSystemSettings::GetMetric(wxSystemMetric::wxSYS_SCREEN_X) * .75,
                       wxSystemSettings::GetMetric(wxSystemMetric::wxSYS_SCREEN_Y) * .75));
            wxMemoryDC memDc(previewImage);
            memDc.SetFont(imageFont);
            // draw the label
            imageLabel.SetText(
                _(L"Narrative text is analyzed, while headers such as these are ignored."));
            imageLabel.SplitTextToFitBoundingBox(
                memDc, wxSize(memDc.GetSize().GetWidth() * .70f, memDc.GetSize().GetHeight()));
            imageLabel.SetAnchorPoint(
                wxPoint(memDc.GetSize().GetWidth() - wxSizerFlags::GetDefaultBorder(),
                        wxSizerFlags::GetDefaultBorder()));
            imageLabel.Draw(memDc);
            memDc.SelectObject(wxNullBitmap);
            narrativeSizer->AddStretchSpacer();
            narrativeSizer->Add(new Thumbnail(docTypeSizer->GetStaticBox(), previewImage));
            docTypeSizer->Add(narrativeSizer, 0, wxALL | wxEXPAND,
                              wxSizerFlags::GetDefaultBorder());
            }
            // non-narrative text
            {
            wxBoxSizer* sparseSizer = new wxBoxSizer(wxHORIZONTAL);
            wxBoxSizer* sparseLablesSizer = new wxBoxSizer(wxVERTICAL);
            wxRadioButton* sparseRadioButton =
                new wxRadioButton(docTypeSizer->GetStaticBox(), ID_NONNARRATIVE_RADIO_BUTTON,
                                  _(L"Non-narrative, &fragmented text"), wxDefaultPosition,
                                  wxDefaultSize, 0, wxGenericValidator(&m_fragmentedTextSelected));
            sparseLablesSizer->Add(sparseRadioButton);
            wxStaticText* noteLabel = new wxStaticText(
                docTypeSizer->GetStaticBox(), ID_NONNARRATIVE_LABEL,
                _(L"Instead of the standard sentence and paragraph structure, the document mostly "
                  "consists of list items and terse sentence fragments. "
                  "NOTE: this option will disable text exclusion."),
                wxDefaultPosition, wxDefaultSize, 0);
            noteLabel->Wrap(ScaledNoteWidth);
            wxBoxSizer* noteSizer = new wxBoxSizer(wxHORIZONTAL);
            noteSizer->Add(noteLabel, 1, wxLEFT, wxSizerFlags::GetDefaultBorder() * 3);

            wxButton* moreInfoButton =
                new wxButton(docTypeSizer->GetStaticBox(), FRAGMENTED_LINK_ID, wxString{},
                             wxDefaultPosition, wxDefaultSize, wxBU_NOTEXT | wxBORDER_NONE);
            moreInfoButton->SetBitmap(
                wxGetApp().GetResourceManager().GetSVG(L"ribbon/help-button.svg"));
            noteSizer->Add(moreInfoButton,
                           wxSizerFlags().Border(wxLEFT, wxSizerFlags::GetDefaultBorder()));
            noteSizer->SetMinSize(ScaledNoteWidth, -1);

            sparseLablesSizer->Add(noteSizer);

            sparseSizer->Add(sparseLablesSizer, 0, wxALIGN_CENTER);

            sparseSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
            wxBitmap previewImage = wxGetApp().GetScaledImage(
                L"wizard/Sparse.png", wxBITMAP_TYPE_PNG,
                wxSize(wxSystemSettings::GetMetric(wxSystemMetric::wxSYS_SCREEN_X) * .75,
                       wxSystemSettings::GetMetric(wxSystemMetric::wxSYS_SCREEN_Y) * .75));
            wxMemoryDC memDc(previewImage);
            // draw the label
            imageLabel.SetText(
                _(L"Document contains few sentences and mostly consists of terse blocks of text."));
            imageLabel.SplitTextToFitBoundingBox(
                memDc, wxSize(memDc.GetSize().GetWidth() * .60f, memDc.GetSize().GetHeight()));
            imageLabel.SetAnchorPoint(
                wxPoint(memDc.GetSize().GetWidth() - wxSizerFlags::GetDefaultBorder(),
                        wxSizerFlags::GetDefaultBorder()));
            imageLabel.Draw(memDc);
            memDc.SelectObject(wxNullBitmap);
            sparseSizer->AddStretchSpacer();
            sparseSizer->Add(new Thumbnail(docTypeSizer->GetStaticBox(), previewImage));
            docTypeSizer->Add(sparseSizer, 0, wxALL | wxEXPAND, wxSizerFlags::GetDefaultBorder());
            }
            // text with "broken lines"
            {
            wxBoxSizer* narrativeSizer = new wxBoxSizer(wxHORIZONTAL);
            wxBoxSizer* narrativeLablesSizer = new wxBoxSizer(wxVERTICAL);
            wxCheckBox* narrativeButton = new wxCheckBox(
                docLayoutSizer->GetStaticBox(), ID_SENTENCES_SPLIT_RADIO_BUTTON,
                _(L"&Sentences are split by illustrations or extra spacing"), wxDefaultPosition,
                wxDefaultSize, wxCHK_2STATE, wxGenericValidator(&m_splitLinesSelected));
            narrativeLablesSizer->Add(narrativeButton);
            narrativeLablesSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

            wxStaticText* noteLabel = new wxStaticText(
                docLayoutSizer->GetStaticBox(), ID_SENTENCES_SPLIT_LABEL,
                _(L"The document's sentences may be wrapped around illustrations or contain "
                  "empty lines between them. This is common for children's picture books."),
                wxDefaultPosition, wxDefaultSize, 0);
            noteLabel->Wrap(ScaledNoteWidth);
            wxBoxSizer* noteSizer = new wxBoxSizer(wxHORIZONTAL);
            noteSizer->Add(noteLabel, 1, wxLEFT, wxSizerFlags::GetDefaultBorder() * 3);
            noteSizer->SetMinSize(ScaledNoteWidth, -1);

            wxButton* moreInfoButton = new wxButton(
                docLayoutSizer->GetStaticBox(), NARRATIVE_WITH_ILLUSTRATIONS_LINK_ID, wxString{},
                wxDefaultPosition, wxDefaultSize, wxBU_NOTEXT | wxBORDER_NONE);
            moreInfoButton->SetBitmap(
                wxGetApp().GetResourceManager().GetSVG(L"ribbon/help-button.svg"));
            noteSizer->Add(moreInfoButton,
                           wxSizerFlags().Border(wxLEFT, wxSizerFlags::GetDefaultBorder()));

            narrativeLablesSizer->Add(noteSizer);

            narrativeSizer->Add(narrativeLablesSizer, 0, wxALIGN_CENTER);

            narrativeSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
            wxBitmap previewImage = wxGetApp().GetScaledImage(
                L"wizard/NarrativeIllustrated.png", wxBITMAP_TYPE_PNG,
                wxSize(wxSystemSettings::GetMetric(wxSystemMetric::wxSYS_SCREEN_X) * .75,
                       wxSystemSettings::GetMetric(wxSystemMetric::wxSYS_SCREEN_Y) * .75));
            wxMemoryDC memDc(previewImage);
            // draw the label
            imageLabel.SetText(
                _(L"Sentences split into fragments by illustrations will be chained together."));
            imageLabel.SplitTextToFitBoundingBox(
                memDc, wxSize(memDc.GetSize().GetWidth() * .33f, memDc.GetSize().GetHeight()));
            imageLabel.SetAnchorPoint(
                wxPoint(memDc.GetSize().GetWidth() - wxSizerFlags::GetDefaultBorder(),
                        wxSizerFlags::GetDefaultBorder()));
            imageLabel.Draw(memDc);
            memDc.SelectObject(wxNullBitmap);
            narrativeSizer->AddStretchSpacer();
            narrativeSizer->Add(new Thumbnail(docLayoutSizer->GetStaticBox(), previewImage));
            narrativeSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

            previewImage = wxGetApp().GetScaledImage(
                L"wizard/NarrativeWithLines.png", wxBITMAP_TYPE_PNG,
                wxSize(wxSystemSettings::GetMetric(wxSystemMetric::wxSYS_SCREEN_X) * .75,
                       wxSystemSettings::GetMetric(wxSystemMetric::wxSYS_SCREEN_Y) * .75));
            wxMemoryDC memDc2(previewImage);
            // draw the label
            imageLabel.SetText(
                _(L"Split sentences with extra lines between them will be chained together."));
            imageLabel.SplitTextToFitBoundingBox(
                memDc2, wxSize(memDc2.GetSize().GetWidth() * .50f, memDc2.GetSize().GetHeight()));
            imageLabel.SetAnchorPoint(
                wxPoint(memDc2.GetSize().GetWidth() - wxSizerFlags::GetDefaultBorder(),
                        wxSizerFlags::GetDefaultBorder()));
            imageLabel.Draw(memDc2);
            memDc2.SelectObject(wxNullBitmap);
            narrativeSizer->Add(new Thumbnail(docLayoutSizer->GetStaticBox(), previewImage));
            docLayoutSizer->Add(narrativeSizer, 0, wxALL | wxEXPAND,
                                wxSizerFlags::GetDefaultBorder());
            }
            // centered text
            {
            wxBoxSizer* centeredSizer = new wxBoxSizer(wxHORIZONTAL);
            wxBoxSizer* centeredLablesSizer = new wxBoxSizer(wxVERTICAL);
            wxCheckBox* centeredButton =
                new wxCheckBox(docLayoutSizer->GetStaticBox(), wxID_ANY,
                               _(L"Centered/left-aligned text"), wxDefaultPosition, wxDefaultSize,
                               wxCHK_2STATE, wxGenericValidator(&m_centeredText));
            centeredLablesSizer->Add(centeredButton);
            wxStaticText* noteLabel = new wxStaticText(
                docLayoutSizer->GetStaticBox(), wxID_STATIC,
                _(L"Text is indented to be centered or left-aligned on the page. "
                  "Selecting this option will instruct the program to ignore indenting when "
                  "deducing where paragraphs begin and end."),
                wxDefaultPosition, wxDefaultSize, 0);
            noteLabel->Wrap(ScaledNoteWidth);
            wxBoxSizer* noteSizer = new wxBoxSizer(wxHORIZONTAL);
            noteSizer->Add(noteLabel, 1, wxLEFT, wxSizerFlags::GetDefaultBorder() * 3);

            wxButton* moreInfoButton =
                new wxButton(docLayoutSizer->GetStaticBox(), CENTERED_TEXT_LINK_ID, wxString{},
                             wxDefaultPosition, wxDefaultSize, wxBU_NOTEXT | wxBORDER_NONE);
            moreInfoButton->SetBitmap(
                wxGetApp().GetResourceManager().GetSVG(L"ribbon/help-button.svg"));
            noteSizer->Add(moreInfoButton, 0, wxLEFT, wxSizerFlags::GetDefaultBorder() * 3);
            noteSizer->SetMinSize(ScaledNoteWidth, -1);

            centeredLablesSizer->Add(noteSizer, 0, wxALIGN_CENTER);

            centeredSizer->Add(centeredLablesSizer, 0, wxALIGN_CENTER);

            wxBitmap previewImage = wxGetApp().GetScaledImage(
                L"wizard/CenteredText.png", wxBITMAP_TYPE_PNG,
                wxSize(wxSystemSettings::GetMetric(wxSystemMetric::wxSYS_SCREEN_X) * .75,
                       wxSystemSettings::GetMetric(wxSystemMetric::wxSYS_SCREEN_Y) * .75));
            wxMemoryDC memDc(previewImage);
            // draw the label
            imageLabel.SetText(_(L"Text is left-aligned to be centered on the page."));
            imageLabel.SetAnchorPoint(
                wxPoint(memDc.GetSize().GetWidth() - wxSizerFlags::GetDefaultBorder(),
                        wxSizerFlags::GetDefaultBorder()));
            imageLabel.Draw(memDc);
            memDc.SelectObject(wxNullBitmap);
            centeredSizer->AddStretchSpacer();
            centeredSizer->Add(new Thumbnail(docLayoutSizer->GetStaticBox(), previewImage));
            docLayoutSizer->Add(centeredSizer, 0, wxALL | wxEXPAND,
                                wxSizerFlags::GetDefaultBorder());
            }
            // new lines are always new paragraphs (overrides center text option above)
            {
            wxBoxSizer* wrappedSizer = new wxBoxSizer(wxHORIZONTAL);
            wxBoxSizer* wrappedLablesSizer = new wxBoxSizer(wxVERTICAL);
            wxCheckBox* wrappedButton = new wxCheckBox(
                docLayoutSizer->GetStaticBox(), ID_HARD_RETURN_CHECKBOX,
                _(L"Line ends (i.e., hard returns) mark the start of a new paragraph"),
                wxDefaultPosition, wxDefaultSize, wxCHK_2STATE,
                wxGenericValidator(&m_newLinesAlwaysNewParagraphs));
            wrappedLablesSizer->Add(wrappedButton);
            wxStaticText* noteLabel = new wxStaticText(
                docLayoutSizer->GetStaticBox(), ID_HARD_RETURN_LABEL,
                _(L"Hard returns in the text always force the start of a new paragraph. "
                  "Selecting this option will instruct the program to treat all line ends as the "
                  "end of the "
                  "sentence and paragraph, regardless of whether it ends with a period."),
                wxDefaultPosition, wxDefaultSize, 0);
            noteLabel->Wrap(ScaledNoteWidth);
            wrappedLablesSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
            wrappedLablesSizer->Add(noteLabel, 1, wxLEFT, wxSizerFlags::GetDefaultBorder() * 3);
            wrappedLablesSizer->SetMinSize(ScaledNoteWidth, -1);
            wrappedSizer->Add(wrappedLablesSizer, 0, wxALIGN_CENTER);

            wrappedSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

            wxBitmap previewImage = wxGetApp().GetScaledImage(
                L"wizard/HardReturns.png", wxBITMAP_TYPE_PNG,
                wxSize(wxSystemSettings::GetMetric(wxSystemMetric::wxSYS_SCREEN_X) * .75,
                       wxSystemSettings::GetMetric(wxSystemMetric::wxSYS_SCREEN_Y) * .75));
            wxMemoryDC memDc(previewImage);
            memDc.SetFont(imageFont);
            // draw the labels
            imageLabel.SetText(_(L"Hard returns separate these lines into different paragraphs. "
                                 "This prevents them from being combined into one sentence."));
            imageLabel.SplitTextToFitBoundingBox(
                memDc, wxSize(memDc.GetSize().GetWidth() * .55f, memDc.GetSize().GetHeight()));
            imageLabel.SetAnchorPoint(
                wxPoint(memDc.GetSize().GetWidth() - wxSizerFlags::GetDefaultBorder(),
                        wxSizerFlags::GetDefaultBorder() * 10));
            imageLabel.Draw(memDc);
            memDc.SelectObject(wxNullBitmap);
            wrappedSizer->AddStretchSpacer();
            wrappedSizer->Add(new Thumbnail(docLayoutSizer->GetStaticBox(), previewImage));
            wrappedSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

            previewImage = wxGetApp().GetScaledImage(
                L"wizard/LineEndsAreNewParagraphs.png", wxBITMAP_TYPE_PNG,
                wxSize(wxSystemSettings::GetMetric(wxSystemMetric::wxSYS_SCREEN_X) * .75,
                       wxSystemSettings::GetMetric(wxSystemMetric::wxSYS_SCREEN_Y) * .75));
            wxMemoryDC memDc2(previewImage);
            memDc2.SetFont(imageFont);
            // draw the labels
            imageLabel.SetText(
                _(L"Line ends always force the start of a new sentence and paragraph."));
            imageLabel.SetAnchorPoint(
                wxPoint(memDc2.GetSize().GetWidth() - wxSizerFlags::GetDefaultBorder(),
                        wxSizerFlags::GetDefaultBorder()));
            imageLabel.Draw(memDc2);
            memDc2.SelectObject(wxNullBitmap);
            wrappedSizer->Add(new Thumbnail(docLayoutSizer->GetStaticBox(), previewImage));
            docLayoutSizer->Add(wrappedSizer, 0, wxALL | wxEXPAND,
                                wxSizerFlags::GetDefaultBorder());
            }
        pageSizer->Add(optionsSizer, 1, wxEXPAND | wxALL, wxSizerFlags::GetDefaultBorder());
        }
        // test selection
        {
        wxPanel* page =
            new wxPanel(m_sideBarBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
        wxBoxSizer* pageSizer = new wxBoxSizer(wxVERTICAL);
        page->SetSizer(pageSizer);
        m_sideBarBook->AddPage(page, _(L"Test Selection"), wxID_ANY, false, 2);

        // The options
        wxBoxSizer* optionsSizer = new wxBoxSizer(wxVERTICAL);

        Banner* banner = new Banner(
            page, wxID_ANY, wxGetApp().GetResourceManager().GetSVG(L"tests/flesch-test.svg"),
            _(L"Select Readability Tests"));
        optionsSizer->Add(banner, 0, wxEXPAND | wxBOTTOM, wxSizerFlags::GetDefaultBorder());

        wxArrayString options;
        options.Add(_(L"Recommend tests based on &document type"));
        options.Add(_(L"Recommend tests based on &industry or field"));
        options.Add(_(L"&Manually select tests"));
        options.Add(_(L"Use a test &bundle"));
        optionsSizer->Add(new wxRadioBox(page, TEST_SELECT_METHOD_BUTTON,
                                         _(L"Choose how to select tests:"), wxDefaultPosition,
                                         wxDefaultSize, options, 0, wxRA_SPECIFY_ROWS,
                                         wxGenericValidator(&m_testSelectionMethod)),
                          0, wxLEFT, wxSizerFlags::GetDefaultBorder());

        m_testTypesSizer = new wxBoxSizer(wxVERTICAL);
        // document types
        wxArrayString docTypes;
        docTypes.Add(_(L"&General document (textbook, report, correspondence)"));
        docTypes.Add(_(L"&Technical document or form (application, r\351sum\351/CV, instructions, "
                       "manual, detailed report)"));
        docTypes.Add(_(L"Non-narrative &form with fragmented text (brochure, menu, quiz). "
                       "NOTE: this option will disable text exclusion."));
        docTypes.Add(_(L"&Literature (young adult and adult)"));
        docTypes.Add(_(L"&Children's literature"));
        m_docTypeRadioBox = new wxRadioBox(
            page, wxID_ANY, _(L"Select the type of document that you are analyzing:"),
            wxDefaultPosition, wxDefaultSize, docTypes, 0, wxRA_SPECIFY_ROWS,
            wxGenericValidator(&m_selectedDocType));
        m_testTypesSizer->Add(m_docTypeRadioBox, 0, wxLEFT, wxSizerFlags::GetDefaultBorder());

        // industry type
        wxArrayString industryTypes;
        industryTypes.Add(_(L"Children's &publishing (literature, textbooks, magazines)"));
        industryTypes.Add(_(L"&Young adult and adult publishing (literature, textbooks, "
                            "magazine/newspaper articles)"));
        industryTypes.Add(_(L"&Children's healthcare"));
        industryTypes.Add(_(L"&Adult healthcare"));
        industryTypes.Add(_(L"&Military and government"));
        industryTypes.Add(_(L"&Second Language (ESL) Education"));
        industryTypes.Add(_(L"&Broadcasting"));
        m_industryTypeRadioBox = new wxRadioBox(
            page, wxID_ANY, _(L"Select the type of industry that this document belongs to:"),
            wxDefaultPosition, wxDefaultSize, industryTypes, 0, wxRA_SPECIFY_ROWS,
            wxGenericValidator(&m_selectedIndustryType));
        m_testTypesSizer->Add(m_industryTypeRadioBox, 0, wxLEFT, wxSizerFlags::GetDefaultBorder());

        // standard tests
        m_testsCheckListBox =
            new wxCheckListBox(page, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, nullptr,
                               wxLB_EXTENDED | wxLB_NEEDED_SB | wxLB_HSCROLL | wxBORDER_THEME,
                               wxGenericValidator(&m_selectedTests));
        m_testsSizer = new wxFlexGridSizer(3, 2, wxSizerFlags::GetDefaultBorder(),
                                           wxSizerFlags::GetDefaultBorder());
        m_testsSizer->Add(new wxStaticText(page, wxID_STATIC, _(L"Standard tests:")));
        m_testsSizer->Add(new wxStaticText(
            page, wxID_STATIC,
            BaseProject::m_custom_word_tests.size() ? _(L"Custom tests:") : wxString{}));
        m_testsSizer->AddGrowableRow(1, 1);
        m_testsSizer->Add(m_testsCheckListBox, 0, wxEXPAND);
        // custom test
        if (BaseProject::m_custom_word_tests.size())
            {
            wxArrayString customTestNames;
            for (const auto& customTest : BaseProject::m_custom_word_tests)
                {
                customTestNames.Add(customTest.get_name().c_str());
                }
            // go through the list of test IDs that were checked the last time this wizard was used
            for (size_t i = 0; i < wxGetApp().GetAppOptions().GetIncludedCustomTests().size(); ++i)
                {
                // find the test in the global list of tests, searching by test id
                CustomReadabilityTestCollection::const_iterator testIter =
                    std::find(BaseProject::m_custom_word_tests.begin(),
                              BaseProject::m_custom_word_tests.end(),
                              wxGetApp().GetAppOptions().GetIncludedCustomTests().at(i).c_str());
                // if the test was found, then check it in the list
                if (testIter != BaseProject::m_custom_word_tests.end())
                    {
                    m_selectedCustomTests.push_back(testIter -
                                                    BaseProject::m_custom_word_tests.begin());
                    }
                }
            m_customTestsCheckListBox = new wxCheckListBox(
                page, wxID_ANY, wxDefaultPosition, wxSize(FromDIP(wxSize(250, 250)).GetWidth(), -1),
                customTestNames, wxLB_EXTENDED | wxLB_NEEDED_SB | wxLB_HSCROLL | wxBORDER_THEME,
                wxGenericValidator(&m_selectedCustomTests));
            m_testsSizer->Add(m_customTestsCheckListBox, 0, wxEXPAND);
            }
        else
            {
            m_testsSizer->Add(new wxStaticText(page, wxID_STATIC, wxString{}));
            }
            // Dolch option
            {
            m_DolchCheckBox =
                new wxCheckBox(page, wxID_ANY, _(L"Dolch Sight Words Suite"), wxDefaultPosition,
                               wxDefaultSize, 0, wxGenericValidator(&m_includeDolchSightWords));
            m_testsSizer->Add(m_DolchCheckBox);
            }
        m_testTypesSizer->Add(m_testsSizer, 1, wxLEFT | wxEXPAND, wxSizerFlags::GetDefaultBorder());

        // test bundles
        wxArrayString testBundles;
        for (const auto& bundle : BaseProject::m_testBundles)
            {
            testBundles.Add(bundle.GetName().c_str());
            }
        m_testsBundlesRadioBox =
            new wxRadioBox(page, wxID_ANY, _(L"Select the test bundle to apply:"),
                           wxDefaultPosition, wxDefaultSize, testBundles, 0, wxRA_SPECIFY_ROWS,
                           wxGenericValidator(&m_selectedBundle));
        SetSelectedTestBundle(wxGetApp().GetAppOptions().GetSelectedTestBundle());
        m_testTypesSizer->Add(m_testsBundlesRadioBox, 0, wxLEFT, wxSizerFlags::GetDefaultBorder());

        TransferDataToWindow();
        UpdateTestsUI();
        UpdateTestSelectionMethodUI();
        optionsSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
        optionsSizer->Add(m_testTypesSizer, 1, wxEXPAND);

        pageSizer->Add(optionsSizer, 1, wxEXPAND | wxALL, wxSizerFlags::GetDefaultBorder());
        }
    wxBoxSizer* buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonsSizer->AddStretchSpacer();
    wxButton* backButton = new wxButton(this, wxID_BACKWARD, _(L"< Back"));
    buttonsSizer->Add(backButton, 0, wxEXPAND);
    buttonsSizer->Add(new wxButton(this, wxID_FORWARD, _(L"Forward >")), 0, wxEXPAND);
    buttonsSizer->Add(new wxButton(this, wxID_OK, _(L"Finish")), 0, wxEXPAND);
    buttonsSizer->AddSpacer(wxSizerFlags::GetDefaultBorder() * 2);
    wxButton* cancelButton = new wxButton(this, wxID_CANCEL);
    buttonsSizer->Add(cancelButton, 0, wxEXPAND);
    wxButton* helpButton = new wxButton(this, wxID_HELP);
    buttonsSizer->Add(helpButton, 0, wxEXPAND);

    backButton->Enable(false);

    mainSizer->Add(buttonsSizer, 0, wxEXPAND | wxALL, wxSizerFlags::GetDefaultBorder());
    SetSizerAndFit(mainSizer);

    if (GetFileList())
        {
        m_fileList->SetColumnWidth(0, m_fileList->GetClientSize().GetWidth() * .75);
        m_fileList->SetColumnWidth(1, m_fileList->GetClientSize().GetWidth() * .25);
        }
    }

//-------------------------------------------------------------
void ProjectWizardDlg::OnDeleteFromListClick([[maybe_unused]] wxCommandEvent& event)
    {
    if (m_fileList)
        {
        m_fileList->DeleteSelectedItems();
        }
    }

//-------------------------------------------------------------
void ProjectWizardDlg::OnAddToListClick([[maybe_unused]] wxCommandEvent& event)
    {
    if (m_fileList)
        {
        m_fileList->EditItem(m_fileList->AddRow(), 0);
        }
    }

//-------------------------------------------------------------
void ProjectWizardDlg::OnGroupClick([[maybe_unused]] wxCommandEvent& event)
    {
    if (m_fileList)
        {
        const auto getCommonFolder = [this]()
        {
            auto selectedItem = m_fileList->GetFirstSelected();
            if (selectedItem != wxNOT_FOUND)
                {
                wxString lastPath{ m_fileList->GetItemTextFormatted(selectedItem, 0) };
                wxString currentCommonFolder;
                wxString lastCommonFolder;
                while (selectedItem != wxNOT_FOUND)
                    {
                    selectedItem = m_fileList->GetNextSelected(selectedItem);
                    if (selectedItem == wxNOT_FOUND)
                        {
                        break;
                        }
                    const auto [currentCommonFolder, folderPos] = GetCommonFolder(
                        lastPath, m_fileList->GetItemTextFormatted(selectedItem, 0));
                    if (currentCommonFolder.empty())
                        {
                        return wxString{};
                        }
                    if (lastCommonFolder.length() &&
                        currentCommonFolder.CmpNoCase(lastCommonFolder) != 0)
                        {
                        return wxString{};
                        }
                    lastPath = m_fileList->GetItemTextFormatted(selectedItem, 0);
                    lastCommonFolder = currentCommonFolder;
                    }
                return currentCommonFolder;
                }
            else
                {
                return wxString{};
                }
        };

        auto firstSelected = m_fileList->GetFirstSelected();
        if (firstSelected != wxNOT_FOUND)
            {
            // use either the first group (if already specified), or find the common folder
            // from selected items and use that as the default group
            wxString currentGroup{ m_fileList->GetItemTextFormatted(firstSelected, 1) };
            // cppcheck-suppress knownConditionTrueFalse
            if (currentGroup.empty())
                {
                currentGroup = getCommonFolder();
                }
            wxTextEntryDialog dlg(this, _(L"Enter a group label for the selected documents"),
                                  _(L"Group Label"), currentGroup);
            if (dlg.ShowModal() == wxID_OK)
                {
                wxWindowUpdateLocker noUpdates(m_fileList);
                while (firstSelected != wxNOT_FOUND)
                    {
                    m_fileList->SetItemText(firstSelected, 1, dlg.GetValue());
                    firstSelected = m_fileList->GetNextSelected(firstSelected);
                    }
                }
            }
        }
    }

//-------------------------------------------------------------
void ProjectWizardDlg::UpdateTestsUI()
    {
    TransferDataFromWindow();
    m_readabilityTests = wxGetApp().GetAppOptions().GetReadabilityTests();
    wxArrayString testNames;
    m_selectedTests.clear();
    for (const auto& rTest : m_readabilityTests.get_tests())
        {
        if (rTest.get_test().has_language(GetLanguage()))
            {
            testNames.push_back(rTest.get_test().get_long_name().c_str());
            // if the test is included, add it to the list of checked items
            // in this list to reflect that
            if (rTest.is_included())
                {
                m_selectedTests.push_back(testNames.size() - 1);
                }
            }
        }
    m_testsCheckListBox->Clear();
    m_testsCheckListBox->InsertItems(testNames, 0);
    m_DolchCheckBox->Enable(GetLanguage() == readability::test_language::english_test);
    TransferDataToWindow();
    }

//-------------------------------------------------------------
void ProjectWizardDlg::LoadArchive(wxString archivePath /*= wxString{}*/)
    {
    ArchiveDlg dlg(this, wxGetApp().GetAppOptions().GetDocumentFilter());
    dlg.SetPath(archivePath);
    dlg.SetSelectedFileFilter(wxGetApp().GetLastSelectedDocFilter());
    dlg.SetHelpTopic(wxGetApp().GetMainFrame()->GetHelpDirectory(),
                     L"batch-project-import-features.html");
    if (dlg.ShowModal() != wxID_OK)
        {
        return;
        }

    // see what sort of labeling should be used
    wxString groupLabel;
    bool groupByLastCommonFolder{ false };
    auto warningIter = WarningManager::GetWarning(_DT(L"prompt-for-batch-label"));
    if (warningIter != WarningManager::GetWarnings().end() && warningIter->ShouldBeShown())
        {
        DocGroupSelectDlg selectLabelTypeDlg(this);
        selectLabelTypeDlg.SetHelpTopic(wxGetApp().GetMainFrame()->GetHelpDirectory(),
                                        L"batch-project-import-features.html");
        if (selectLabelTypeDlg.ShowModal() != wxID_OK)
            {
            return;
            }
        if (selectLabelTypeDlg.GetSelection() == 1)
            {
            groupLabel = selectLabelTypeDlg.GetGroupingLabel();
            }
        else if (selectLabelTypeDlg.GetSelection() == 2)
            {
            groupByLastCommonFolder = true;
            }
        if (selectLabelTypeDlg.IsNotShowingAgain())
            {
            warningIter->Show(false);
            }
        }

    wxWindowDisabler disableAll;
    wxBusyInfo wait(_(L"Retrieving files..."));
    wxGetApp().SetLastSelectedDocFilter(dlg.GetSelectedFileFilter());

    Wisteria::ZipCatalog archive(dlg.GetPath());
    wxArrayString files = FilterFiles(archive.GetPaths(),
                                      ExtractExtensionsFromFileFilter(dlg.GetSelectedFileFilter()));
    files.Sort();

    const size_t currentFileCount = m_fileData->GetItemCount();
    m_fileData->SetSize(currentFileCount + files.GetCount(), 2);
    for (size_t i = 0; i < files.GetCount(); ++i)
        {
        m_fileData->SetItemText(currentFileCount + i, 0, dlg.GetPath() + L"#" + files[i]);
        if (groupLabel.length())
            {
            m_fileData->SetItemText(currentFileCount + i, 1, groupLabel);
            }
        }

    if (groupByLastCommonFolder && files.size())
        {
        LoadGroupFromLastCommonFolder(currentFileCount, files);
        }

    if (m_fileList)
        {
        m_fileList->SetVirtualDataSize(m_fileData->GetItemCount());
        m_fileList->SetColumnWidth(0, m_fileList->GetClientSize().GetWidth() * .75);
        m_fileList->SetColumnWidth(1, m_fileList->GetClientSize().GetWidth() * .25);
        }
    }

//-------------------------------------------------------------
readability::test_language ProjectWizardDlg::GetLanguage() const
    {
    return (wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureLanguagePackCode()) ||
            wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureProfessionalCode())) ?
               static_cast<readability::test_language>(m_selectedLang) :
               readability::test_language::english_test;
    }

//-------------------------------------------------------------
void ProjectWizardDlg::LoadSpreadsheet(wxString excelPath /*= wxString{}*/)
    {
    if (excelPath.empty())
        {
        wxFileDialog dlg(this, _(L"Select Spreadsheet to Analyze"), wxString{}, wxString{},
                         _(L"Excel Files (*.xlsx)|*.xlsx"),
                         wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);

        if (dlg.ShowModal() != wxID_OK)
            {
            return;
            }
        excelPath = dlg.GetPath();
        }

    // only a provided group label makes sense here since cells don't have document
    // descriptions and there aren't any folders to pull a group from
    wxString groupLabel;
    auto warningIter = WarningManager::GetWarning(_DT(L"prompt-for-batch-label"));
    if (warningIter != WarningManager::GetWarnings().end() && warningIter->ShouldBeShown())
        {
        wxTextEntryDialog dlg(this, _(L"Enter a group label for the selected documents"),
                              _(L"Group Label"));
        if (dlg.ShowModal() == wxID_OK)
            {
            groupLabel = dlg.GetValue();
            }
        }

    Wisteria::ZipCatalog archive(excelPath);
    if (archive.Find(L"xl/workbook.xml") == nullptr)
        {
        wxMessageBox(
            _(L"Unable to open Excel document, file is either password-protected or corrupt."),
            wxGetApp().GetAppDisplayName(), wxICON_EXCLAMATION | wxOK);
        return;
        }
    lily_of_the_valley::xlsx_extract_text excelExtract{ false };
        {
        wxWindowDisabler disableAll;
        wxBusyInfo wait(_(L"Loading Excel file..."));
        const wxString workbookFileText = archive.ReadTextFile(L"xl/workbook.xml");
        excelExtract.read_worksheet_names(workbookFileText.wc_str(), workbookFileText.length());
        // read the string table
        const wxString sharedStrings = archive.ReadTextFile(L"xl/sharedStrings.xml");
        if (sharedStrings.length())
            {
            excelExtract.read_shared_strings(sharedStrings.wc_str(), sharedStrings.length());
            }
        }
    // name of worksheets and list of cells with text in them
    std::vector<std::pair<std::wstring, std::vector<std::wstring>>> workSheets;
    wxArrayString worksheets;
    wxArrayInt workSheetSelections;
    for (size_t i = 0; i < excelExtract.get_worksheet_names().size(); ++i)
        {
        worksheets.push_back(excelExtract.get_worksheet_names().at(i).c_str());
        workSheetSelections.push_back(i);
        }
    // only ask for which worksheets to select if there is more than one in the workbook
    if (worksheets.size() > 1)
        {
        wxMultiChoiceDialog chooseWorksheetsDlg(this, _(L"Select the worksheet(s) to import:"),
                                                _(L"Excel Import"), worksheets);
        chooseWorksheetsDlg.SetSelections(workSheetSelections);
        if (chooseWorksheetsDlg.ShowModal() != wxID_OK)
            {
            return;
            }
        workSheetSelections = chooseWorksheetsDlg.GetSelections();
        }
    for (size_t i = 0; i < workSheetSelections.size(); ++i)
        {
        lily_of_the_valley::xlsx_extract_text::worksheet wrk;
            {
            wxWindowDisabler disableAll;
            wxBusyInfo wait(_(L"Loading worksheet..."));
            const wxString sheetFile = archive.ReadTextFile(
                wxString::Format(L"xl/worksheets/sheet%d.xml", workSheetSelections.Item(i) + 1));
            if (sheetFile.length())
                {
                excelExtract(sheetFile.wc_str(), sheetFile.length(), wrk);
                }
            else
                {
                return;
                }
            }

        ExcelPreviewDlg excelPreview(
            this, &wrk, &excelExtract, wxID_ANY,
            wxString::Format(
                _(L"\"%s\" Preview"),
                excelExtract.get_worksheet_names().at(workSheetSelections.Item(i)).c_str()));
        excelPreview.SetHelpTopic(wxGetApp().GetMainFrame()->GetHelpDirectory(),
                                  L"batch-project-import-features.html");
        if (excelPreview.ShowModal() == wxID_OK)
            {
            // filter just the rows that were requested by setting unselected cells to empty
            if (excelPreview.IsImportingOnlySelectedCells())
                {
                for (size_t rowPos = 0; rowPos < wrk.size(); ++rowPos)
                    {
                    for (size_t colPos = 0; colPos < wrk[rowPos].size(); ++colPos)
                        {
                        if (!excelPreview.IsCellSelected(wxGridCellCoords(rowPos, colPos)))
                            {
                            wrk[rowPos].operator[](colPos).set_value(std::wstring());
                            }
                        }
                    }
                }
#ifndef NDEBUG
            if (!lily_of_the_valley::xlsx_extract_text::verify_sheet(wrk).first)
                {
                wxFAIL_MSG(
                    wxString(L"Excel worksheet cell's out of order. First incorrect cell: ") +
                    lily_of_the_valley::xlsx_extract_text::verify_sheet(wrk).second.c_str());
                }
            // verify that the filtering looks OK when debugging
            ExcelPreviewDlg excelPreviewFilterDEBUG(
                this, &wrk, &excelExtract, wxID_ANY,
                wxString(_DT(L"DEBUG CHECK ")) +
                    excelExtract.get_worksheet_names().at(workSheetSelections.Item(i)).c_str());
            excelPreviewFilterDEBUG.ShowModal();
#endif
            workSheets.push_back(std::pair<std::wstring, std::vector<std::wstring>>(
                excelExtract.get_worksheet_names().at(workSheetSelections.Item(i)),
                std::vector<std::wstring>()));
            lily_of_the_valley::xlsx_extract_text::get_text_cell_names(
                wrk, workSheets[workSheets.size() - 1].second);
            }
        }

    if (workSheets.size())
        {
        wxWindowDisabler disableAll;
        wxBusyInfo wait(_(L"Updating file list..."));
        // list the cells in the grid
        const size_t currentFileCount = m_fileData->GetItemCount();
        size_t cellCount = 0;
        for (size_t i = 0; i < workSheets.size(); ++i)
            {
            cellCount += workSheets[i].second.size();
            }
        m_fileData->SetSize(currentFileCount + cellCount, 2);

        size_t cellCounter = 0;
        for (size_t i = 0; i < workSheets.size(); ++i)
            {
            const wxString fullPath = excelPath + L"#" + workSheets[i].first.c_str() + L"#";
            for (std::vector<std::wstring>::const_iterator cellPos = workSheets[i].second.begin();
                 cellPos != workSheets[i].second.end(); ++cellPos, ++cellCounter)
                {
                m_fileData->SetItemText(currentFileCount + cellCounter, 0,
                                        fullPath + cellPos->c_str());
                if (groupLabel.length())
                    {
                    m_fileData->SetItemText(currentFileCount + cellCounter, 1, groupLabel);
                    }
                }
            }
        if (m_fileList)
            {
            m_fileList->SetVirtualDataSize(m_fileData->GetItemCount());
            m_fileList->SetColumnWidth(0, m_fileList->GetClientSize().GetWidth() * .75);
            m_fileList->SetColumnWidth(1, m_fileList->GetClientSize().GetWidth() * .25);
            }
        }
    }

//-------------------------------------------------------------
void ProjectWizardDlg::UpdateTestSelectionMethodUI()
    {
    TransferDataFromWindow();
    if (IsDocumentTypeSelected())
        {
        m_testTypesSizer->Show(m_docTypeRadioBox, true, true);
        m_testTypesSizer->Hide(m_industryTypeRadioBox, true);
        m_testTypesSizer->Hide(m_testsSizer, true);
        m_testTypesSizer->Hide(m_testsBundlesRadioBox, true);
        }
    else if (IsIndustrySelected())
        {
        m_testTypesSizer->Show(m_industryTypeRadioBox, true, true);
        m_testTypesSizer->Hide(m_docTypeRadioBox, true);
        m_testTypesSizer->Hide(m_testsSizer, true);
        m_testTypesSizer->Hide(m_testsBundlesRadioBox, true);
        }
    else if (IsManualTestSelected())
        {
        m_testTypesSizer->Hide(m_docTypeRadioBox, true);
        m_testTypesSizer->Hide(m_industryTypeRadioBox, true);
        m_testTypesSizer->Show(m_testsSizer, true, true);
        m_testTypesSizer->Hide(m_testsBundlesRadioBox, true);
        }
    else if (IsTestBundleSelected())
        {
        m_testTypesSizer->Hide(m_docTypeRadioBox, true);
        m_testTypesSizer->Hide(m_industryTypeRadioBox, true);
        m_testTypesSizer->Hide(m_testsSizer, true);
        m_testTypesSizer->Show(m_testsBundlesRadioBox, true, true);
        }
    m_testTypesSizer->Layout();
    }

//-------------------------------------------------------------
void ProjectWizardDlg::OnPageChange(wxBookCtrlEvent& event)
    {
    if (event.GetSelection() == 0)
        {
        if (wxWindow::FindWindow(wxID_BACKWARD))
            {
            wxWindow::FindWindow(wxID_BACKWARD)->Enable(false);
            }
        if (wxWindow::FindWindow(wxID_FORWARD))
            {
            wxWindow::FindWindow(wxID_FORWARD)->Enable(true);
            }
        }
    else if (static_cast<size_t>(event.GetSelection()) == m_sideBarBook->GetPageCount() - 1)
        {
        if (wxWindow::FindWindow(wxID_FORWARD))
            {
            wxWindow::FindWindow(wxID_FORWARD)->Enable(false);
            }
        if (wxWindow::FindWindow(wxID_BACKWARD))
            {
            wxWindow::FindWindow(wxID_BACKWARD)->Enable(true);
            }
        }
    else
        {
        if (wxWindow::FindWindow(wxID_BACKWARD))
            {
            wxWindow::FindWindow(wxID_BACKWARD)->Enable(true);
            }
        if (wxWindow::FindWindow(wxID_FORWARD))
            {
            wxWindow::FindWindow(wxID_FORWARD)->Enable(true);
            }
        }
    }

//-------------------------------------------------------------
void ProjectWizardDlg::OnNavigate(wxCommandEvent& event)
    {
    if (event.GetId() == wxID_FORWARD)
        {
        if (static_cast<size_t>(m_sideBarBook->GetSelection()) < m_sideBarBook->GetPageCount() - 1)
            {
            m_sideBarBook->SetSelection(m_sideBarBook->GetSelection() + 1);
            }
        if (static_cast<size_t>(m_sideBarBook->GetSelection()) == m_sideBarBook->GetPageCount() - 1)
            {
            wxWindow::FindWindow(wxID_FORWARD)->Enable(false);
            }
        wxWindow::FindWindow(wxID_BACKWARD)->Enable(true);
        }
    else
        {
        if (m_sideBarBook->GetSelection() > 0)
            {
            m_sideBarBook->SetSelection(m_sideBarBook->GetSelection() - 1);
            }
        if (m_sideBarBook->GetSelection() == 0)
            {
            wxWindow::FindWindow(wxID_BACKWARD)->Enable(false);
            }
        wxWindow::FindWindow(wxID_FORWARD)->Enable(true);
        }
    }

//-------------------------------------------------------------
void ProjectWizardDlg::OnOK([[maybe_unused]] wxCommandEvent& event)
    {
    Validate();
    TransferDataFromWindow();

    if (GetProjectType() == ProjectType::StandardProject)
        {
        if (IsTextFromFileSelected())
            {
            FilePathResolver resolvePath(GetFilePath(), true);
            if (resolvePath.IsInvalidFile() || (resolvePath.IsLocalOrNetworkFile() &&
                                                !wxFile::Exists(resolvePath.GetResolvedPath())))
                {
                // open the file dialog to give the user one last chance to select file
                wxCommandEvent fileButtonEvent(wxEVT_COMMAND_BUTTON_CLICKED, ID_FILE_BROWSE_BUTTON);
                GetEventHandler()->ProcessEvent(fileButtonEvent);
                // if user hit Cancel then don't proceed and tell them what to do
                if (GetFilePath().length() == 0)
                    {
                    m_sideBarBook->SetSelection(0);
                    wxMessageBox(_(L"Please enter a file to be analyzed before continuing."),
                                 _(L"Invalid Input"), wxICON_EXCLAMATION | wxOK, this);
                    return;
                    }
                // if file doesn't exist
                else if (!wxFile::Exists(GetFilePath()))
                    {
                    m_sideBarBook->SetSelection(0);
                    wxMessageBox(_(L"File not found. Please enter a valid file to be analyzed "
                                   "before continuing."),
                                 _(L"Invalid Input"), wxICON_EXCLAMATION | wxOK, this);
                    return;
                    }
                }
            }
        }
    else
        {
        if (GetFileData()->GetItemCount() == 0)
            {
            // open the file dialog to give the user one last chance to select file
            wxCommandEvent fileButtonEvent(wxEVT_COMMAND_BUTTON_CLICKED,
                                           ID_BATCH_FOLDER_BROWSE_BUTTON);
            GetEventHandler()->ProcessEvent(fileButtonEvent);
            // if user hit Cancel then don't proceed and tell them what to do
            if (GetFileData()->GetItemCount() == 0)
                {
                wxMessageBox(_(L"Please select files to be analyzed before continuing."),
                             _(L"Invalid Input"), wxICON_EXCLAMATION | wxOK, this);
                return;
                }
            }
        }

    // if user indicates that the document is non-narrative text that should be fully analyzed, then
    // non-narrative form on the document type page should be selected too
    if (IsFragmentedTextSelected() && IsDocumentTypeSelected() &&
        GetSelectedDocumentType() != readability::document_classification::nonnarrative_document)
        {
        wxMessageBox(
            _(L"Because the document composition was set to non-narrative text, the document type "
              "will also be adjusted to non-narrative."),
            wxGetApp().GetAppDisplayName(), wxICON_INFORMATION | wxOK, this);
        SelectDocumentType(readability::document_classification::nonnarrative_document);
        }
    // and the inverse as well, if the tests for non-narrative form are selected,
    // then set the composition to non-narrative.
    if (IsDocumentTypeSelected() &&
        GetSelectedDocumentType() == readability::document_classification::nonnarrative_document &&
        !IsFragmentedTextSelected())
        {
        wxMessageBox(
            _(L"Because the test selection is set to non-narrative, the document composition will "
              "also be set to non-narrative."),
            wxGetApp().GetAppDisplayName(), wxICON_INFORMATION | wxOK, this);
        SetFragmentedTextSelected();
        }
    if (IsCenteredTextSelected() && IsNewLinesAlwaysNewParagraphsSelected())
        {
        SelectPage(1);
        wxMessageBox(
            _(L"If text is centered, then new lines should not force the start of a new paragraph. "
              "Please unselect one of these options."),
            wxGetApp().GetAppDisplayName(), wxICON_INFORMATION | wxOK, this);
        return;
        }
    if (IsSplitLinesSelected() && IsNewLinesAlwaysNewParagraphsSelected())
        {
        SelectPage(1);
        wxMessageBox(_(L"If sentences are split, then new lines should not force the start of a "
                       "new paragraph. Please unselect one of these options."),
                     wxGetApp().GetAppDisplayName(), wxICON_INFORMATION | wxOK, this);
        return;
        }

    if (IsDocumentTypeSelected())
        {
        bool hasIncludedStandardTest = false;
        for (const auto& rTest : m_readabilityTests.get_tests())
            {
            if (rTest.get_test().has_document_classification(GetSelectedDocumentType()) &&
                rTest.get_test().has_language(GetLanguage()))
                {
                hasIncludedStandardTest = true;
                break;
                }
            }
        if (!hasIncludedStandardTest)
            {
            wxMessageBox(_(L"Note that there are no tests associated with this document type for "
                           "the selected language."),
                         wxGetApp().GetAppDisplayName(), wxICON_INFORMATION | wxOK);
            }
        }
    else if (IsIndustrySelected())
        {
        bool hasIncludedStandardTest = false;
        for (const auto& rTest : m_readabilityTests.get_tests())
            {
            if (rTest.get_test().has_industry_classification(GetSelectedIndustryType()) &&
                rTest.get_test().has_language(GetLanguage()))
                {
                hasIncludedStandardTest = true;
                break;
                }
            }
        if (!hasIncludedStandardTest)
            {
            wxMessageBox(_(L"Note that there are no tests associated with this "
                           "industry for the selected language."),
                         wxGetApp().GetAppDisplayName(), wxICON_INFORMATION | wxOK);
            }
        }

    if (IsModal())
        {
        EndModal(wxID_OK);
        }
    else
        {
        Show(false);
        }
    }

//-------------------------------------------------------------
void ProjectWizardDlg::OnTestSelectionMethodChanged([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();
    UpdateTestSelectionMethodUI();
    }

//-------------------------------------------------------------
void ProjectWizardDlg::OnButtonClick(wxCommandEvent& link)
    {
    wxString helpTopic;
    if (link.GetId() == NARRATIVE_WITH_ILLUSTRATIONS_LINK_ID)
        {
        helpTopic = L"line-chaining.html";
        }
    else if (link.GetId() == FRAGMENTED_LINK_ID)
        {
        helpTopic = L"framented-text.html";
        }
    else if (link.GetId() == CENTERED_TEXT_LINK_ID)
        {
        helpTopic = L"controlling-how-sentences-are-deduced.html";
        }
    wxLaunchDefaultBrowser(wxFileName::FileNameToURL(wxGetApp().GetMainFrame()->GetHelpDirectory() +
                                                     wxFileName::GetPathSeparator() + helpTopic));
    }

//-------------------------------------------------------------
void ProjectWizardDlg::OnLanguageChanged([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();
    UpdateTestsUI();
    }

//-------------------------------------------------------------
void ProjectWizardDlg::OnSourceRadioChange(wxCommandEvent& event)
    {
    m_textEntryEdit->Enable((event.GetId() == ID_MANUALLY_ENTERED_TEXT_BUTTON));
    m_filePathEdit->Enable((event.GetId() == ID_FROM_FILE_BUTTON));
    m_fileBrowseButton->Enable((event.GetId() == ID_FROM_FILE_BUTTON));
    }

//-------------------------------------------------------------
void ProjectWizardDlg::OnFileBrowseButtonClick([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();
    wxFileDialog dialog(this, _(L"Select Document to Analyze"), wxString{}, wxString{},
                        wxGetApp().GetAppOptions().GetDocumentFilter(),
                        wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);

    if (dialog.ShowModal() != wxID_OK)
        {
        return;
        }

    m_filePath = dialog.GetPath();
    TransferDataToWindow();
    SetFocus();
    }

//-------------------------------------------------------------
void ProjectWizardDlg::OnRandomSampleCheck([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();
    if (m_randPercentageCtrl && m_isRandomSampling)
        {
        m_randPercentageCtrl->Enable(m_isRandomSampling->IsChecked());
        }
    }

//-------------------------------------------------------------
void ProjectWizardDlg::OnAddWebPageButtonClick([[maybe_unused]] wxCommandEvent& event)
    {
    wxTextEntryDialog textDlg(this, _(L"Enter a web page to analyze:"), _(L"Enter Web Page"));
    if (textDlg.ShowModal() == wxID_OK && textDlg.GetValue().length())
        {
        FilePathResolver resolver(textDlg.GetValue(), false);
        if (!resolver.IsWebFile())
            {
            wxMessageBox(
                wxString::Format(_(L"\"%s\" does not appear to be a valid web page.\n"
                                   "Be sure to include the 'www', 'http', or 'https' prefix."),
                                 resolver.GetResolvedPath()),
                wxGetApp().GetAppDisplayName(), wxICON_WARNING | wxOK);
            return;
            }
        // see what sort of labeling should be used
        wxString groupLabel;
        auto warningIter = WarningManager::GetWarning(_DT(L"prompt-for-batch-label"));
        if (warningIter != WarningManager::GetWarnings().end() && warningIter->ShouldBeShown())
            {
            DocGroupSelectDlg selectLabelTypeDlg(this);
            selectLabelTypeDlg.SetHelpTopic(wxGetApp().GetMainFrame()->GetHelpDirectory(),
                                            L"batch-project-import-features.html");
            if (selectLabelTypeDlg.ShowModal() != wxID_OK)
                {
                return;
                }
            if (selectLabelTypeDlg.GetSelection() == 1)
                {
                groupLabel = selectLabelTypeDlg.GetGroupingLabel();
                }
            if (selectLabelTypeDlg.IsNotShowingAgain())
                {
                warningIter->Show(false);
                }
            }

        const size_t currentFileCount = m_fileData->GetItemCount();
        m_fileData->SetSize(currentFileCount + 1, 2);
        m_fileData->SetItemText(currentFileCount, 0, resolver.GetResolvedPath());
        if (groupLabel.length())
            {
            m_fileData->SetItemText(currentFileCount, 1, groupLabel);
            }
        m_fileList->SetVirtualDataSize(m_fileData->GetItemCount());
        m_fileList->SetColumnWidth(0, m_fileList->GetClientSize().GetWidth() * .75);
        m_fileList->SetColumnWidth(1, m_fileList->GetClientSize().GetWidth() * .25);
        }
    }

//-------------------------------------------------------------
void ProjectWizardDlg::OnAddWebPagesButtonClick([[maybe_unused]] wxCommandEvent& event)
    {
    WebHarvesterDlg webHarvestDlg(this, wxGetApp().GetLastSelectedWebPages(),
                                  wxGetApp().GetAppOptions().GetDocumentFilter(),
                                  wxGetApp().GetLastSelectedDocFilter(), false);
    webHarvestDlg.UpdateFromHarvesterSettings(wxGetApp().GetWebHarvester());
    webHarvestDlg.SetHelpTopic(wxGetApp().GetMainFrame()->GetHelpDirectory(),
                               L"web-harvester.html");
    if (webHarvestDlg.ShowModal() != wxID_OK)
        {
        return;
        }

    // see what sort of labeling should be used
    wxString groupLabel;
    bool groupByLastCommonFolder{ false };
    auto warningIter = WarningManager::GetWarning(_DT(L"prompt-for-batch-label"));
    if (warningIter != WarningManager::GetWarnings().end() && warningIter->ShouldBeShown())
        {
        DocGroupSelectDlg selectLabelTypeDlg(this);
        selectLabelTypeDlg.SetHelpTopic(wxGetApp().GetMainFrame()->GetHelpDirectory(),
                                        L"batch-project-import-features.html");
        if (selectLabelTypeDlg.ShowModal() != wxID_OK)
            {
            return;
            }
        if (selectLabelTypeDlg.GetSelection() == 1)
            {
            groupLabel = selectLabelTypeDlg.GetGroupingLabel();
            }
        else if (selectLabelTypeDlg.GetSelection() == 2)
            {
            groupByLastCommonFolder = true;
            }
        if (selectLabelTypeDlg.IsNotShowingAgain())
            {
            warningIter->Show(false);
            }
        }

    wxGetApp().SetLastSelectedWebPages(webHarvestDlg.GetUrls());
    wxGetApp().SetLastSelectedDocFilter(webHarvestDlg.GetSelectedDocFilter());
    webHarvestDlg.UpdateHarvesterSettings(wxGetApp().GetWebHarvester());

    size_t totalFileCount{ 0 };
    wxArrayString files;

    for (size_t urlCounter = 0; urlCounter < webHarvestDlg.GetUrls().GetCount(); ++urlCounter)
        {
        FilePathResolver resolver(webHarvestDlg.GetUrls().Item(urlCounter), false);
        wxGetApp().GetWebHarvester().SetUrl(resolver.GetResolvedPath());

        // if cancelled, we still will want what was harvested up to that point,
        // so it's OK to ignore the user response here
        [[maybe_unused]] auto crawlResult = wxGetApp().GetWebHarvester().CrawlLinks();

        // add the new links to the list
        const size_t currentFileCount = m_fileData->GetItemCount();
        totalFileCount += currentFileCount;
        size_t i = 0;
        if (webHarvestDlg.IsDownloadFilesLocally())
            {
            m_fileData->SetSize(
                currentFileCount + wxGetApp().GetWebHarvester().GetDownloadedFilePaths().size(), 2);
            for (const auto& path : wxGetApp().GetWebHarvester().GetDownloadedFilePaths())
                {
                m_fileData->SetItemText(currentFileCount + i, 0, path);
                files.push_back(path);
                if (groupLabel.length())
                    {
                    m_fileData->SetItemText(currentFileCount + i, 1, groupLabel);
                    }
                // if they chose to use the documents' descriptions as the labels,
                // then those are loaded on import
                ++i;
                }
            }
        else
            {
            m_fileData->SetSize(
                currentFileCount + wxGetApp().GetWebHarvester().GetHarvestedLinks().size(), 2);
            for (const auto& path : wxGetApp().GetWebHarvester().GetHarvestedLinks())
                {
                m_fileData->SetItemText(currentFileCount + i, 0, path);
                files.push_back(path);
                if (groupLabel.length())
                    {
                    m_fileData->SetItemText(currentFileCount + i, 1, groupLabel);
                    }
                ++i;
                }
            }
        }

    if (groupByLastCommonFolder && files.size())
        {
        LoadGroupFromLastCommonFolder(totalFileCount, files);
        }

    m_fileList->SetVirtualDataSize(m_fileData->GetItemCount());
    m_fileList->SetColumnWidth(0, m_fileList->GetClientSize().GetWidth() * .75);
    m_fileList->SetColumnWidth(1, m_fileList->GetClientSize().GetWidth() * .25);
    }

//-------------------------------------------------------------
void ProjectWizardDlg::OnAddFolderButtonClick([[maybe_unused]] wxCommandEvent& event)
    {
    GetDirFilterDialog dirDlg(this, wxGetApp().GetAppOptions().GetDocumentFilter());
    dirDlg.SetSelectedFileFilter(wxGetApp().GetLastSelectedDocFilter());
    dirDlg.SetPath(GetLastSelectedFolder());
    dirDlg.SetHelpTopic(wxGetApp().GetMainFrame()->GetHelpDirectory(),
                        L"batch-project-import-features.html");
    if (dirDlg.ShowModal() != wxID_OK || dirDlg.GetPath().empty())
        {
        return;
        }
    // get the list of files
    wxArrayString files;
        {
        wxWindowDisabler disableAll;
        wxBusyInfo wait(_(L"Retrieving files..."));
        SetLastSelectedFolder(dirDlg.GetPath());
        wxGetApp().SetLastSelectedDocFilter(dirDlg.GetSelectedFileFilter());
        wxDir::GetAllFiles(dirDlg.GetPath(), &files, wxString{},
                           dirDlg.IsRecursive() ? wxDIR_FILES | wxDIR_DIRS : wxDIR_FILES);
        files = FilterFiles(files, ExtractExtensionsFromFileFilter(dirDlg.GetSelectedFileFilter()));
        }
    files.Sort();

    // see what sort of labeling should be used
    wxString groupLabel;
    bool groupByLastCommonFolder{ false };
    auto warningIter = WarningManager::GetWarning(_DT(L"prompt-for-batch-label"));
    if (warningIter != WarningManager::GetWarnings().end() && warningIter->ShouldBeShown())
        {
        DocGroupSelectDlg selectLabelTypeDlg(this);
        selectLabelTypeDlg.SetHelpTopic(wxGetApp().GetMainFrame()->GetHelpDirectory(),
                                        L"batch-project-import-features.html");
        if (selectLabelTypeDlg.ShowModal() != wxID_OK)
            {
            return;
            }
        if (selectLabelTypeDlg.GetSelection() == 1)
            {
            groupLabel = selectLabelTypeDlg.GetGroupingLabel();
            }
        else if (selectLabelTypeDlg.GetSelection() == 2)
            {
            groupByLastCommonFolder = true;
            }
        if (selectLabelTypeDlg.IsNotShowingAgain())
            {
            warningIter->Show(false);
            }
        }

    wxWindowUpdateLocker noUpdates(m_fileList);
    const size_t currentFileCount = m_fileData->GetItemCount();
    m_fileData->SetSize(currentFileCount + files.GetCount(), 2);
    for (size_t i = 0; i < files.GetCount(); ++i)
        {
        m_fileData->SetItemText(currentFileCount + i, 0, files.Item(i));
        if (groupLabel.length())
            {
            m_fileData->SetItemText(currentFileCount + i, 1, groupLabel);
            }
        // if they chose to use the documents' descriptions as the labels,
        // then those are loaded on import later
        }

    if (groupByLastCommonFolder && files.size())
        {
        LoadGroupFromLastCommonFolder(currentFileCount, files);
        }

    m_fileList->SetVirtualDataSize(m_fileData->GetItemCount());
    m_fileList->SetColumnWidth(0, m_fileList->GetClientSize().GetWidth() * .75);
    m_fileList->SetColumnWidth(1, m_fileList->GetClientSize().GetWidth() * .25);
    }

//-------------------------------------------------------------
void ProjectWizardDlg::LoadGroupFromLastCommonFolder(const size_t currentFileCount,
                                                     const wxArrayString& files)
    {
    std::pair<wxString, size_t> commonFolder{ wxString{}, wxString::npos };
    size_t i{ 0 };
    for (; i < files.GetCount() - 1; ++i)
        {
        // if last item had a common folder group match, then...
        if (commonFolder.first.length() > 0)
            {
            const auto lastMatch{ commonFolder };
            commonFolder = GetCommonFolder(files[i], files[i + 1]);
            // ...update if the match between the current item and the next one
            // is a longer path
            if (commonFolder.first.length() > 0 && lastMatch.second <= commonFolder.second)
                {
                m_fileData->SetItemText(currentFileCount + i, 1, commonFolder.first);
                m_fileData->SetItemText(currentFileCount + i + 1, 1, commonFolder.first);
                }
            continue;
            }
        commonFolder = GetCommonFolder(files[i], files[i + 1]);
        if (commonFolder.first.length() > 0)
            {
            m_fileData->SetItemText(currentFileCount + i, 1, commonFolder.first);
            m_fileData->SetItemText(currentFileCount + i + 1, 1, commonFolder.first);
            }
        }
    }

//-------------------------------------------------------------
void ProjectWizardDlg::OnAddSpreadsheetFileButtonClick([[maybe_unused]] wxCommandEvent& event)
    {
    LoadSpreadsheet();
    }

//-------------------------------------------------------------
void ProjectWizardDlg::OnAddArchiveFileButtonClick([[maybe_unused]] wxCommandEvent& event)
    {
    LoadArchive();
    }

//-------------------------------------------------------------
void ProjectWizardDlg::OnAddFileButtonClick([[maybe_unused]] wxCommandEvent& event)
    {
    wxFileDialog dialog(this, _(L"Select Document(s) to Analyze"), wxString{}, wxString{},
                        wxGetApp().GetAppOptions().GetDocumentFilter(),
                        wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW | wxFD_MULTIPLE);

    if (dialog.ShowModal() != wxID_OK)
        {
        return;
        }

    wxArrayString files;
    dialog.GetPaths(files);

    if (files.GetCount() == 0)
        {
        return;
        }

    // see what sort of labeling should be used
    wxString groupLabel;
    bool groupByLastCommonFolder{ false };
    auto warningIter = WarningManager::GetWarning(_DT(L"prompt-for-batch-label"));
    if (warningIter != WarningManager::GetWarnings().end() && warningIter->ShouldBeShown())
        {
        DocGroupSelectDlg selectLabelTypeDlg(this);
        selectLabelTypeDlg.SetHelpTopic(wxGetApp().GetMainFrame()->GetHelpDirectory(),
                                        L"batch-project-import-features.html");
        if (selectLabelTypeDlg.ShowModal() != wxID_OK)
            {
            return;
            }
        if (selectLabelTypeDlg.GetSelection() == 1)
            {
            groupLabel = selectLabelTypeDlg.GetGroupingLabel();
            }
        else if (selectLabelTypeDlg.GetSelection() == 2)
            {
            groupByLastCommonFolder = true;
            }
        if (selectLabelTypeDlg.IsNotShowingAgain())
            {
            warningIter->Show(false);
            }
        }

    // set the default name of the project to the last folder of the file selected here.
    const wxArrayString folders = wxFileName(wxFileName(files[0]).GetPathWithSep()).GetDirs();
    SetLastSelectedFolder(folders.size() ? folders.back() : wxString{});

    const size_t currentFileCount = m_fileData->GetItemCount();
    m_fileData->SetSize(currentFileCount + files.GetCount(), 2);
    for (size_t i = 0; i < files.GetCount(); ++i)
        {
        m_fileData->SetItemText(currentFileCount + i, 0, files.Item(i));
        if (groupLabel.length())
            {
            m_fileData->SetItemText(currentFileCount + i, 1, groupLabel);
            }
        // if they chose to use the documents' descriptions as the labels,
        // then those are loaded on import
        }

    if (groupByLastCommonFolder && files.size())
        {
        LoadGroupFromLastCommonFolder(currentFileCount, files);
        }

    m_fileList->SetVirtualDataSize(m_fileData->GetItemCount());
    m_fileList->SetColumnWidth(0, m_fileList->GetClientSize().GetWidth() * .75);
    m_fileList->SetColumnWidth(1, m_fileList->GetClientSize().GetWidth() * .25);
    }

//-------------------------------------------------------------
void ProjectWizardDlg::OnContextHelp([[maybe_unused]] wxHelpEvent& event)
    {
    wxCommandEvent cmd;
    OnHelp(cmd);
    }

//-------------------------------------------------------------
void ProjectWizardDlg::OnHelp([[maybe_unused]] wxCommandEvent& event)
    {
    const wxString helpProjectPath = wxGetApp().GetMainFrame()->GetHelpDirectory();
    wxString helpTopic = (GetProjectType() == ProjectType::StandardProject) ?
                             L"creating-standard-project.html" :
                             L"creating-batch-project.html";
    wxLaunchDefaultBrowser(
        wxFileName::FileNameToURL(helpProjectPath + wxFileName::GetPathSeparator() + helpTopic));
    }
