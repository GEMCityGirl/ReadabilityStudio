#include "export_all_dlg.h"
#include "../../app/readability_app.h"
#include "../../projects/standard_project_doc.h"
#include "../../projects/batch_project_doc.h"
#include "../../projects/standard_project_view.h"
#include "../../projects/batch_project_view.h"

wxDECLARE_APP(ReadabilityApp);

using namespace Wisteria::GraphItems;

ExportAllDlg::ExportAllDlg(wxWindow* parent, BaseProjectDoc* doc,
                  const bool fileMode,
                  wxWindowID id /*= wxID_ANY*/, const wxString& caption /*= _(L"Export Options")*/,
                  const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/,
                  long style /*= wxDEFAULT_DIALOG_STYLE|wxCLIP_CHILDREN*/) :
        m_readabilityProjectDoc(doc), m_fileMode(fileMode)
    {
    wxASSERT_MSG(doc, L"NULL document passed to export all dialog!");
    wxString exportDir = doc->GetExportFolder();
    if (exportDir.empty())
        {
        // if export path not specified yet then try the path of the project (if saved already)
        wxFileName fn(doc->GetFilename());
        if (!fn.GetPath().empty())
            { exportDir = fn.GetPath(); }
        // then try the documents folder
        else
            { exportDir = wxStandardPaths::Get().GetDocumentsDir(); }
        }
    m_folderPath = exportDir;

    wxString exportFile = doc->GetExportFile();
    if (exportFile.empty())
        {
        // if export path not specified yet, then try the path of the project (if saved already)
        wxFileName fn(doc->GetFilename());
        if (!fn.GetPath().empty())
            { exportFile = fn.GetPathWithSep() + doc->GetTitle() + L".htm"; }
        }
    m_filePath = exportFile;

    m_listExt = doc->GetExportListExt();
    m_textViewExt = doc->GetExportTextViewExt();
    m_graphExt = doc->GetExportGraphExt();
    m_exportHardWordLists = doc->IsExportingHardWordLists();
    m_exportSentencesBreakdown = doc->IsExportingSentencesBreakdown();
    m_exportGraphs = doc->IsExportingGraphs();
    m_exportTestResults = doc->IsExportingTestResults();
    m_exportStatistics = doc->IsExportingStatistics();
    m_exportWordiness = doc->IsExportingWordiness();
    m_exportSightWords = doc->IsExportingSightWords();
    m_exportWarnings = doc->IsExportingWarnings();
    m_exportingLists = doc->IsExportingLists();
    m_exportingTextReports = doc->IsExportingTextReports();
    m_imageExportOptions.m_imageSize = doc->GetImageExportOptions().m_imageSize;
    Create(parent, id, caption, pos, size, style);

    Bind(wxEVT_BUTTON, &ExportAllDlg::OnFolderBrowseButtonClick, this, ID_FOLDER_BROWSE_BUTTON);
    Bind(wxEVT_BUTTON, &ExportAllDlg::OnImageOptionsButtonClick, this, ID_IMAGE_OPTIONS_BUTTON);
    Bind(wxEVT_BUTTON, &ExportAllDlg::OnOK, this, wxID_OK);
    Bind(wxEVT_CHECKBOX, &ExportAllDlg::OnIncludeListsTextWindowsCheck,
        this, ID_INCLUDE_LIST_CHECKBOX);
    Bind(wxEVT_CHECKBOX, &ExportAllDlg::OnIncludeListsTextWindowsCheck,
        this, ID_INCLUDE_TEXT_CHECKBOX);
    }

//---------------------------------------------
void ExportAllDlg::OnOK([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();
    if (m_folderPath.empty())
        {
        wxMessageBox(_(L"Export folder not specified."), 
            _(L"Error"), wxOK|wxICON_EXCLAMATION);
        return;
        }
    if (IsStandardProject() && !m_exportHardWordLists &&
        !m_exportSentencesBreakdown && !m_exportGraphs &&
        !m_exportTestResults && !m_exportStatistics && !m_exportWordiness &&
        !m_exportSightWords)
        {
        wxMessageBox(_(L"You must select at least one item to be saved."), 
            _(L"Error"), wxOK|wxICON_EXCLAMATION);
        return;
        }
    if (IsBatchProject() && !m_exportHardWordLists &&
        !m_exportSentencesBreakdown && !m_exportGraphs &&
        !m_exportTestResults && !m_exportStatistics && !m_exportWordiness &&
        !m_exportSightWords && !m_exportWarnings)
        {
        wxMessageBox(_(L"You must select at least one item to be saved."), 
            _(L"Error"), wxOK|wxICON_EXCLAMATION);
        return;
        }
    if (m_textViewCombo)
        { m_textViewExt = m_textViewCombo->GetValue(); }
    if (m_listCombo)
        { m_listExt = m_listCombo->GetValue(); }
    if (m_graphCombo)
        { m_graphExt = m_graphCombo->GetValue(); }
    
    if (IsModal())
        { EndModal(wxID_OK); }
    else
        { Show(false); }
    }

//-------------------------------------------------------------
void ExportAllDlg::OnIncludeListsTextWindowsCheck([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();

    auto exportControl = FindWindow(ID_LIST_TYPE_LABEL);
    if (exportControl)
        { exportControl->Enable(m_exportingLists); }
    exportControl = FindWindow(ID_LIST_TYPE_COMBO);
    if (exportControl)
        { exportControl->Enable(m_exportingLists); }
    exportControl = FindWindow(ID_TEXT_TYPE_LABEL);
    if (exportControl)
        { exportControl->Enable(m_exportingTextReports); }
    exportControl = FindWindow(ID_TEXT_TYPE_COMBO);
    if (exportControl)
        { exportControl->Enable(m_exportingTextReports); }

    TransferDataToWindow();
    }

//-------------------------------------------------------------
void ExportAllDlg::OnImageOptionsButtonClick([[maybe_unused]] wxCommandEvent& event)
    {
    wxASSERT_MSG(m_graphCombo, L"Graph file combobox not initialized!");

    wxString ext{(m_graphCombo ? m_graphCombo->GetValue() : L"png")};
    Wisteria::UI::ImageExportDlg optDlg(this,
        Image::GetImageFileTypeFromExtension(ext),
        wxNullBitmap,
        m_imageExportOptions);
    optDlg.SetHelpTopic(
        wxGetApp().GetMainFrame()->GetHelpDirectory(),
        L"image-export.html");
    if (optDlg.ShowModal() == wxID_OK)
        { m_imageExportOptions = optDlg.GetOptions(); }
    }

//-------------------------------------------------------------
void ExportAllDlg::OnFolderBrowseButtonClick([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();

    if (m_fileMode)
        {
        wxFileDialog fdialog(this,
                    _(L"Save As"),
                    wxString{},
                    m_filePath.length() ?
                        m_filePath :
                        m_readabilityProjectDoc->GetTitle() + L".htm",
                    _(L"HTML Files (*.htm;*.html)|*.htm;*.html"),
                    wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
        if (fdialog.ShowModal() != wxID_OK)
            { return; }
        m_filePath = fdialog.GetPath();
        }
    else
        {
        wxDirDialog dirDlg(this, _(L"Choose a directory"), m_folderPath);
        if (dirDlg.ShowModal() != wxID_OK)
            { return; }
        m_folderPath = dirDlg.GetPath();
        }
    TransferDataToWindow();
    SetFocus();
    }

//-------------------------------------------------------------
void ExportAllDlg::CreateControls()
    {
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

    wxBoxSizer* itemsBoxSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(itemsBoxSizer, 0, wxEXPAND|wxALIGN_TOP|wxALL, wxSizerFlags::GetDefaultBorder());

    // export path
    wxStaticBox* pathBox = new wxStaticBox(this, wxID_ANY,
        m_fileMode ? _(L"File to export to:") : _(L"Folder to export to:"));
    wxStaticBoxSizer* pathBoxBoxSizer = new wxStaticBoxSizer(pathBox, wxVERTICAL);
    itemsBoxSizer->Add(pathBoxBoxSizer, 1, wxEXPAND);

    wxBoxSizer* folderBrowseBoxSizer = new wxBoxSizer(wxHORIZONTAL);
    pathBoxBoxSizer->Add(folderBrowseBoxSizer, 0, wxEXPAND|wxALL, wxSizerFlags::GetDefaultBorder());

    wxTextCtrl* folderPathEdit =
        new wxTextCtrl(this, wxID_ANY, wxString{}, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME,
                       m_fileMode ?
                           wxGenericValidator(&m_filePath) : wxGenericValidator(&m_folderPath) );
    folderBrowseBoxSizer->Add(folderPathEdit, 1, wxEXPAND);

    m_folderBrowseButton = new wxBitmapButton(this, ID_FOLDER_BROWSE_BUTTON,
        wxArtProvider::GetBitmapBundle(wxART_FILE_OPEN, wxART_BUTTON));
    folderBrowseBoxSizer->Add(m_folderBrowseButton);

    // inclusion options
    wxStaticBox* inclusionSectionBox =
        new wxStaticBox(this, wxID_ANY, _(L"Sections to export:"));
    wxStaticBoxSizer* inclusionSectionBoxSizer =
        new wxStaticBoxSizer(inclusionSectionBox, wxVERTICAL);
    itemsBoxSizer->Add(inclusionSectionBoxSizer);
    if (IsStandardProject())
        {
        const ProjectView* view =
            dynamic_cast<const ProjectView*>(m_readabilityProjectDoc->GetFirstView());

        wxCheckBox* testResultsCheck = new wxCheckBox(this, wxID_ANY, _(L"Test scores"),
            wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_exportTestResults));
        inclusionSectionBoxSizer->Add(testResultsCheck, 0, wxEXPAND|wxALL,
                                      wxSizerFlags::GetDefaultBorder());

        wxCheckBox* statisticsCheck = new wxCheckBox(this, wxID_ANY, _(L"Summary statistics"),
            wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_exportStatistics));
        inclusionSectionBoxSizer->Add(statisticsCheck, 0, wxEXPAND|wxALL,
                                      wxSizerFlags::GetDefaultBorder());

        if (view->GetWordsBreakdownView().GetWindowCount() > 0)
            {
            wxCheckBox* listCheck = new wxCheckBox(this, wxID_ANY, _(L"Words breakdown"),
                wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_exportHardWordLists));
            inclusionSectionBoxSizer->Add(listCheck, 0, wxEXPAND|wxALL,
                                          wxSizerFlags::GetDefaultBorder());
            }

        if (view->GetSentencesBreakdownView().GetWindowCount() > 0)
            {
            wxCheckBox* listCheck = new wxCheckBox(this, wxID_ANY, _(L"Sentences breakdown"),
                wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_exportSentencesBreakdown));
            inclusionSectionBoxSizer->Add(listCheck, 0, wxEXPAND|wxALL,
                                          wxSizerFlags::GetDefaultBorder());
            }

        if (view->GetGrammarView().GetWindowCount() > 0)
            {
            wxCheckBox* wordinessCheck = new wxCheckBox(this, wxID_ANY, _(L"Grammar section"),
                wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_exportWordiness));
            inclusionSectionBoxSizer->Add(wordinessCheck, 0, wxEXPAND|wxALL,
                                          wxSizerFlags::GetDefaultBorder());
            }

        if (view->GetDolchSightWordsView().GetWindowCount() > 0)
            {
            wxCheckBox* sightWordsCheck = new wxCheckBox(this, wxID_ANY, _(L"Dolch sight words section"),
                wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_exportSightWords));
            inclusionSectionBoxSizer->Add(sightWordsCheck, 0, wxEXPAND|wxALL,
                                          wxSizerFlags::GetDefaultBorder());
            }
        }
    else
        {
        const BatchProjectView* view =
            dynamic_cast<const BatchProjectView*>(m_readabilityProjectDoc->GetFirstView());

        wxCheckBox* testResultsCheck = new wxCheckBox(this, wxID_ANY, _(L"Test scores"),
            wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_exportTestResults));
        inclusionSectionBoxSizer->Add(testResultsCheck, 0, wxEXPAND|wxALL,
                                      wxSizerFlags::GetDefaultBorder());

        wxCheckBox* graphCheck = new wxCheckBox(this, wxID_ANY, _(L"Histograms/box plots"),
            wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_exportGraphs));
        inclusionSectionBoxSizer->Add(graphCheck, 0, wxEXPAND|wxALL,
                                      wxSizerFlags::GetDefaultBorder());

        if (view->GetWordsBreakdownView().GetWindowCount() > 0)
            {
            wxCheckBox* listCheck = new wxCheckBox(this, wxID_ANY, _(L"Words breakdown"),
                wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_exportHardWordLists));
            inclusionSectionBoxSizer->Add(listCheck, 0, wxEXPAND|wxALL,
                                          wxSizerFlags::GetDefaultBorder());
            }

        if (view->GetSentencesBreakdownView().GetWindowCount() > 0)
            {
            wxCheckBox* listCheck = new wxCheckBox(this, wxID_ANY, _(L"Sentences breakdown"),
                wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_exportSentencesBreakdown));
            inclusionSectionBoxSizer->Add(listCheck, 0, wxEXPAND|wxALL,
                                          wxSizerFlags::GetDefaultBorder());
            }

        if (view->GetGrammarView().GetWindowCount() > 0)
            {
            wxCheckBox* grammarCheck = new wxCheckBox(this, wxID_ANY, _(L"Grammar section"),
                wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_exportWordiness));
            inclusionSectionBoxSizer->Add(grammarCheck, 0, wxEXPAND|wxALL,
                                          wxSizerFlags::GetDefaultBorder());
            }

        if (view->GetDolchSightWordsView().GetWindowCount() > 0)
            {
            wxCheckBox* sightWordsCheck = new wxCheckBox(this, wxID_ANY, _(L"Dolch sight word section"),
                wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_exportSightWords));
            inclusionSectionBoxSizer->Add(sightWordsCheck, 0, wxEXPAND|wxALL,
                                          wxSizerFlags::GetDefaultBorder());
            }

        if (view->GetWarningsView()->GetItemCount() > 0)
            {
            wxCheckBox* warningsCheck = new wxCheckBox(this, wxID_ANY, _(L"Warnings section"),
                wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_exportWarnings));
            inclusionSectionBoxSizer->Add(warningsCheck, 0, wxEXPAND|wxALL,
                                          wxSizerFlags::GetDefaultBorder());
            }
        }

    // the extension values
    wxStaticBox* exportTypeBox =
        new wxStaticBox(this, wxID_ANY, _(L"Export file types:"));
    wxStaticBoxSizer* exportTypeStaticBoxSizer = new wxStaticBoxSizer(exportTypeBox, wxVERTICAL);
    itemsBoxSizer->Add(exportTypeStaticBoxSizer);

    auto exportTypeBoxSizer = new wxFlexGridSizer(3, 5, 5);
    exportTypeStaticBoxSizer->Add(exportTypeBoxSizer);

    if (!m_fileMode)
        {
        wxStaticText* listLabel =
            new wxStaticText(this, ID_LIST_TYPE_LABEL, _(L"Export lists as:"),
            wxDefaultPosition, wxDefaultSize, 0);
        listLabel->Enable(m_exportingLists);
        exportTypeBoxSizer->Add(listLabel, 0, wxALIGN_CENTER_VERTICAL|wxALL,
                                wxSizerFlags::GetDefaultBorder());

        m_listCombo = new wxComboBox(this, ID_LIST_TYPE_COMBO, wxString{},
                                     wxDefaultPosition, wxDefaultSize, 0, nullptr,
                                     wxCB_DROPDOWN|wxCB_READONLY);
        m_listCombo->Append(L"htm");
        m_listCombo->Append(L"txt");
        m_listCombo->SetStringSelection(m_listExt);
        m_listCombo->Enable(m_exportingLists);
        exportTypeBoxSizer->Add(m_listCombo, 0, wxALIGN_LEFT|wxALL,
                                wxSizerFlags::GetDefaultBorder());
        exportTypeBoxSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

        // text view section
        if (IsStandardProject())
            {
            wxStaticText* textViewLabel =
                new wxStaticText(this, ID_TEXT_TYPE_LABEL, _(L"Export text reports as:"),
                wxDefaultPosition, wxDefaultSize, 0);
            textViewLabel->Enable(m_exportingTextReports);
            exportTypeBoxSizer->Add(textViewLabel, 0, wxALIGN_CENTER_VERTICAL|wxALL,
                                    wxSizerFlags::GetDefaultBorder());

            m_textViewCombo = new wxComboBox(this, ID_TEXT_TYPE_COMBO, wxString{},
                                            wxDefaultPosition, wxDefaultSize, 0, nullptr,
                                            wxCB_DROPDOWN|wxCB_READONLY);
            m_textViewCombo->Append(L"htm");
            m_textViewCombo->Append(L"rtf");
            m_textViewCombo->SetStringSelection(m_textViewExt);
            m_textViewCombo->Enable(m_exportingTextReports);
            exportTypeBoxSizer->Add(m_textViewCombo, 0, wxALIGN_LEFT|wxALL,
                                    wxSizerFlags::GetDefaultBorder());
            exportTypeBoxSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
            }
        }

    // graph view section
    wxStaticText* graphLabel = new wxStaticText(this, wxID_STATIC, _(L"Export graphs as:"),
        wxDefaultPosition, wxDefaultSize, 0);
    exportTypeBoxSizer->Add(graphLabel, 0, wxALIGN_CENTER_VERTICAL|wxALL,
                            wxSizerFlags::GetDefaultBorder());

    m_graphCombo = new wxComboBox(this, wxID_ANY, wxString{},
                                  wxDefaultPosition, wxDefaultSize, 0, nullptr,
                                  wxCB_DROPDOWN|wxCB_READONLY);
    m_graphCombo->Append(L"png");
    m_graphCombo->Append(L"jpg");
    m_graphCombo->Append(L"bmp");
    m_graphCombo->Append(L"tif");
    m_graphCombo->Append(L"tga");
    m_graphCombo->Append(L"gif");
    m_graphCombo->Append(L"svg");
    m_graphCombo->SetStringSelection(m_graphExt);
    exportTypeBoxSizer->Add(m_graphCombo, 0, wxALIGN_CENTER_VERTICAL|wxALL,
                            wxSizerFlags::GetDefaultBorder());

    auto imageButton = new wxButton(this, ID_IMAGE_OPTIONS_BUTTON, _(L"Image options"));
    imageButton->SetBitmap(
        wxGetApp().GetResourceManager().GetSVG(L"ribbon/boxplot.svg"));
    exportTypeBoxSizer->Add(imageButton, 0, wxALIGN_CENTER_VERTICAL|wxALL,
                            wxSizerFlags::GetDefaultBorder());

    if (IsStandardProject())
        {
        mainSizer->Add(
            new wxCheckBox(this, ID_INCLUDE_LIST_CHECKBOX, _(L"Include lists"),
                wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_exportingLists)),
            0, wxEXPAND|wxALL, wxSizerFlags::GetDefaultBorder());

        mainSizer->Add(
            new wxCheckBox(this, ID_INCLUDE_TEXT_CHECKBOX, _(L"Include text reports"),
                wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_exportingTextReports)),
            0, wxEXPAND | wxALL, wxSizerFlags::GetDefaultBorder());
        }

    mainSizer->Add(CreateSeparatedButtonSizer(wxOK|wxCANCEL|wxHELP), 0,
                   wxEXPAND|wxALL, wxSizerFlags::GetDefaultBorder());
    }

//---------------------------------------------
bool ExportAllDlg::IsStandardProject() const
    {
    return (m_readabilityProjectDoc &&
            m_readabilityProjectDoc->IsKindOf(CLASSINFO(ProjectDoc)));
    }

//---------------------------------------------
bool ExportAllDlg::IsBatchProject() const
    {
    return (m_readabilityProjectDoc &&
            m_readabilityProjectDoc->IsKindOf(CLASSINFO(BatchProjectDoc)));
    }
