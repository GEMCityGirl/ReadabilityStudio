/** @addtogroup UI
    @brief Classes for the user interface.
    @date 2005-2023
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __PROJECTWIZARD_H__
#define __PROJECTWIZARD_H__

#include <wx/choicdlg.h>
#include "../../../../SRC/wxTools/wxWebHarvester.h"
#include "../../../../SRC/wxTools/wxWebHarvesterDlg.h"
#include "../../../../SRC/Wisteria-Dataviz/src/ui/dialogs/excelpreviewdlg.h"
#include "../../../../SRC/Wisteria-Dataviz/src/ui/dialogs/getdirdlg.h"
#include "../../../../SRC/Wisteria-Dataviz/src/ui/dialogs/archivedlg.h"
#include "../../../../SRC/Wisteria-Dataviz/src/ui/controls/listctrlex.h"
#include "../../../../SRC/Wisteria-Dataviz/src/ui/controls/thumbnail.h"
#include "../../../../SRC/Wisteria-Dataviz/src/ui/controls/sidebarbook.h"
#include "../../../../SRC/Licensing/LicenseAdmin.h"
#include "doc_group_select_dlg.h"
#include "../../app/readability_app.h"

enum class ProjectType
    {
    StandardProject,
    BatchProject
    };

class ProjectWizardDlg final : public wxDialog
    {
public:
    ProjectWizardDlg(wxWindow* parent, const ProjectType projectType,
                     const wxString& path = wxString{}, wxWindowID id = wxID_ANY,
                     const wxString& caption = _("New Project Wizard"),
                     const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                     long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);
    /// @private
    ProjectWizardDlg(const ProjectWizardDlg&) = delete;
    /// @private
    ProjectWizardDlg& operator=(const ProjectWizardDlg&) = delete;
    /// @private
    ~ProjectWizardDlg()
        { wxDELETE(m_fileData); }
    /// Creates the controls and sizers.
    void CreateControls();
    [[nodiscard]] readability::test_language GetLanguage() const;
    //batch document entry
    [[nodiscard]] const ListCtrlExDataProvider* GetFileData() const noexcept
        { return m_fileData; }
    [[nodiscard]] bool IsRandomSampling() const
        { return m_isRandomSampling->IsChecked(); }
    [[nodiscard]] int GetRandomSamplePercentage() const
        { return m_randPercentageCtrl->GetValue(); }

    [[nodiscard]] static wxString GetLastSelectedFolder()
        { return m_lastSelectedFolder; }
    static void SetLastSelectedFolder(const wxString& folder)
        { m_lastSelectedFolder = folder; }

    [[nodiscard]] ListCtrlEx* GetFileList() noexcept
        { return m_fileList; }
    //document entry
    [[nodiscard]] bool IsTextFromFileSelected() const noexcept
        { return m_fromFileSelected; }
    [[nodiscard]] bool IsManualTextEntrySelected() const noexcept
        { return m_manualSelected; }
    void SetTextFromFileSelected()
        {
        TransferDataFromWindow();
        m_fromFileSelected = true;
        m_textEntryEdit->Disable();
        m_filePathEdit->Enable();
        m_fileBrowseButton->Enable();
        TransferDataToWindow();
        }
    void SetManualTextEntrySelected()
        {
        TransferDataFromWindow();
        m_manualSelected = true;
        m_textEntryEdit->Enable();
        m_filePathEdit->Disable();
        m_fileBrowseButton->Disable();
        TransferDataToWindow();
        }
    [[nodiscard]] wxString GetFilePath() const
        { return m_filePath; }
    void SetFilePath(const wxString& path)
        {
        m_filePath = path;
        TransferDataToWindow();
        }
    [[nodiscard]] wxString GetEnteredText() const
        { return m_enteredText; }
    void SetEnteredText(const wxString& text)
        {
        m_enteredText = text;
        TransferDataToWindow();
        }
    //document structure
    [[nodiscard]] bool IsNarrativeSelected() const noexcept
        { return m_narrativeSelected; }
    [[nodiscard]] bool IsFragmentedTextSelected() const noexcept
        { return m_fragmentedTextSelected; }
    [[nodiscard]] bool IsSplitLinesSelected() const noexcept
        { return m_splitLinesSelected; }
    [[nodiscard]] bool IsCenteredTextSelected() const noexcept
        { return m_centeredText; }
    [[nodiscard]] bool IsNewLinesAlwaysNewParagraphsSelected() const noexcept
        { return m_newLinesAlwaysNewParagraphs; }
    void SetNarrativeSelected() noexcept
        {
        m_narrativeSelected = true;
        m_fragmentedTextSelected = false;
        }
    void SetFragmentedTextSelected() noexcept
        {
        m_narrativeSelected = false;
        m_fragmentedTextSelected = true;
        }
    void SetSplitLinesSelected(const bool setVal) noexcept
        { m_splitLinesSelected = setVal; }
    void SetCenteredTextSelected(const bool setVal) noexcept
        { m_centeredText = setVal; }
    void SetNewLinesAlwaysNewParagraphsSelected(const bool setVal) noexcept
        { m_newLinesAlwaysNewParagraphs = setVal; }
    //test selection method
    void SetTestSelectionMethod(const int method)
        {
        m_testSelectionMethod = method;
        wxASSERT(is_within(std::make_pair(0,static_cast<int>(TestRecommendation::TEST_RECOMMENDATION_COUNT)-1), m_testSelectionMethod));
        TransferDataToWindow();
        UpdateTestSelectionMethodUI();
        }
    [[nodiscard]] bool IsDocumentTypeSelected() const noexcept
        { return m_testSelectionMethod == 0; }
    [[nodiscard]] bool IsIndustrySelected() const noexcept
        { return m_testSelectionMethod == 1; }
    [[nodiscard]] bool IsManualTestSelected() const noexcept
        { return m_testSelectionMethod == 2; }
    [[nodiscard]] bool IsTestBundleSelected() const noexcept
        { return m_testSelectionMethod == 3; }
    //document type
    [[nodiscard]] readability::document_classification GetSelectedDocumentType() const noexcept
        { return static_cast<readability::document_classification>(m_selectedDocType); }
    void SelectDocumentType(const readability::document_classification docType)
        {
        m_selectedDocType = static_cast<decltype(m_selectedDocType)>(docType);
        TransferDataToWindow();
        }
    //industry type
    [[nodiscard]]
    readability::industry_classification GetSelectedIndustryType() const noexcept
        { return static_cast<readability::industry_classification>(m_selectedIndustryType); }
    void SelectIndustryType(const readability::industry_classification industryType)
        {
        m_selectedIndustryType = static_cast<decltype(m_selectedIndustryType)>(industryType);
        TransferDataToWindow();
        }
    //tests
    [[nodiscard]]
    readability::readability_test_collection<>& GetReadabilityTestsInfo()
        {
        for (size_t i = 0; i < m_testsCheckListBox->GetCount(); ++i)
            { m_readabilityTests.include_test(m_testsCheckListBox->GetString(i), m_testsCheckListBox->IsChecked(i)); }
        return m_readabilityTests;
        }
    [[nodiscard]]
    bool IsDolchSelected() const
        { return (GetLanguage() == readability::test_language::english_test) && m_includeDolchSightWords; }
    [[nodiscard]]
    const wxArrayInt GetSelectedCustomTests() const noexcept
        { return m_selectedCustomTests; }
    // test bundles
    [[nodiscard]]
    wxString GetSelectedTestBundle() const
        { return m_testsBundlesRadioBox->GetString(m_selectedBundle); }
    void SetSelectedTestBundle(const wxString& bundleName)
        {
        const auto foundPos = m_testsBundlesRadioBox->FindString(bundleName);
        if (foundPos != wxNOT_FOUND)
            { m_selectedBundle = foundPos; }
        TransferDataToWindow();
        }
    void SelectPage(const size_t page)
        { m_sideBarBook->SetSelection(page); }
    [[nodiscard]]
    size_t GetSelectedPage() const noexcept
        { return m_sideBarBook->GetSelection(); }

    // just used for screenshots
    void SelectStandardTestManually(const wxString& test)
        {
        auto found = m_testsCheckListBox->FindString(test);
        if (found != wxNOT_FOUND)
            { m_selectedTests.push_back(found); }
        TransferDataToWindow();
        }
private:
    [[nodiscard]] ProjectType GetProjectType() const noexcept
        { return m_projectType; }
    void LoadSpreadsheet(wxString excelPath = wxString{});
    void LoadArchive(wxString archivePath = wxString{});
    void UpdateTestSelectionMethodUI();
    void UpdateTestsUI();
    /// @warning: do not change these IDs, they are used in the screenshot script
    static const int ID_FILE_BROWSE_BUTTON = 10001;
    static const int ID_FROM_FILE_BUTTON = 10002;
    static const int ID_MANUALLY_ENTERED_TEXT_BUTTON = 10003;
    static const int NARRATIVE_WITH_ILLUSTRATIONS_LINK_ID = 10004;
    static const int FRAGMENTED_LINK_ID = 10005;
    static const int CENTERED_TEXT_LINK_ID = 10006;
    static const int TEST_SELECT_METHOD_BUTTON = 10007;
    static const int LANGUAGE_BUTTON = 10008;
    static const int ID_BATCH_FOLDER_BROWSE_BUTTON = 10009;
    static const int ID_BATCH_FILE_BROWSE_BUTTON = 10010;
    static const int ID_RANDOM_SAMPLE_CHECK = 10011;
    static const int ID_RANDOM_SAMPLE_LABEL = 10012;
    static const int ID_WEB_PAGES_BROWSE_BUTTON = 10013;
    static const int ID_ARCHIVE_FILE_BROWSE_BUTTON = 10014;
    static const int ID_SPREADSHEET_FILE_BROWSE_BUTTON = 10015;
    static const int NEWLINES_ALWAYS_NEW_PARAGRAPH_LINK_ID = 10016;
    static const int ID_WEB_PAGE_BROWSE_BUTTON = 10017;
    static const int ID_ADD_FILE_BUTTON = 10018;
    static const int ID_DELETE_FILE_BUTTON = 10019;
    static const int ID_HARD_RETURN_CHECKBOX = 10020;
    static const int ID_HARD_RETURN_LABEL = 10021;
    static const int ID_NONNARRATIVE_RADIO_BUTTON = 10022;
    static const int ID_NONNARRATIVE_LABEL = 10023;
    static const int ID_NARRATIVE_RADIO_BUTTON = 10024;
    static const int ID_NARRATIVE_LABEL = 10025;
    static const int ID_SENTENCES_SPLIT_RADIO_BUTTON = 10026;
    static const int ID_SENTENCES_SPLIT_LABEL = 10027;

    void OnHelp([[maybe_unused]] wxCommandEvent& event);
    void OnContextHelp([[maybe_unused]] wxHelpEvent& event);
    void OnFileBrowseButtonClick([[maybe_unused]] wxCommandEvent& event);
    void OnSourceRadioChange(wxCommandEvent& event);
    void OnTestSelectionMethodChanged([[maybe_unused]] wxCommandEvent& event);
    void OnLanguageChanged([[maybe_unused]] wxCommandEvent& event);
    void OnButtonClick(wxCommandEvent& link);
    void OnOK([[maybe_unused]] wxCommandEvent& event);
    void OnNavigate(wxCommandEvent& event);
    void OnPageChange(wxBookCtrlEvent& event);
    void OnAddFolderButtonClick([[maybe_unused]] wxCommandEvent& event);
    void OnAddFileButtonClick([[maybe_unused]] wxCommandEvent& event);
    void OnAddWebPagesButtonClick([[maybe_unused]] wxCommandEvent& event);
    void OnAddWebPageButtonClick([[maybe_unused]] wxCommandEvent& event);
    void OnAddArchiveFileButtonClick([[maybe_unused]] wxCommandEvent& event);
    void OnAddSpreadsheetFileButtonClick([[maybe_unused]] wxCommandEvent& event);
    void OnRandomSampleCheck([[maybe_unused]] wxCommandEvent& event);
    void OnAddToListClick([[maybe_unused]] wxCommandEvent& event);
    void OnDeleteFromListClick([[maybe_unused]] wxCommandEvent& event);

    //batch document entry
    ListCtrlEx* m_fileList{ nullptr };
    ListCtrlExDataProvider* m_fileData{ nullptr };
    wxCheckBox* m_isRandomSampling{ nullptr };
    wxSpinCtrl* m_randPercentageCtrl{ nullptr };
    static wxString m_lastSelectedFolder;

    //text/document entry
    wxTextCtrl* m_filePathEdit{ nullptr };
    wxBitmapButton* m_fileBrowseButton{ nullptr };
    wxString m_filePath;
    bool m_fromFileSelected{ true };
    bool m_manualSelected{ false };

    wxTextCtrl* m_textEntryEdit{ nullptr };
    wxString m_enteredText;

    //document structure
    bool m_narrativeSelected{ false };
    bool m_splitLinesSelected{ false };
    bool m_fragmentedTextSelected{ false };
    bool m_centeredText{ false };
    bool m_newLinesAlwaysNewParagraphs{ false };

    //test selection method
    int m_testSelectionMethod{ 0 };

    wxBoxSizer* m_testTypesSizer{ nullptr };

    //document types
    wxRadioBox* m_docTypeRadioBox{ nullptr };
    int m_selectedDocType{ 0 };

    //industry types
    wxRadioBox* m_industryTypeRadioBox{ nullptr };
    int m_selectedIndustryType{ 0 };

    //tests
    wxFlexGridSizer* m_testsSizer{ nullptr };

    wxRadioBox* m_testsBundlesRadioBox{ nullptr };
    int m_selectedBundle{ 0 };

    bool m_includeDolchSightWords{ false };
    readability::readability_test_collection<> m_readabilityTests;
    wxArrayInt m_selectedTests;
    wxCheckListBox* m_testsCheckListBox{ nullptr };
    wxArrayInt m_selectedCustomTests;
    wxCheckListBox* m_customTestsCheckListBox{ nullptr };
    wxCheckBox* m_DolchCheckBox{ nullptr };

    ProjectType m_projectType{ ProjectType::StandardProject };
    Wisteria::UI::SideBarBook* m_sideBarBook{ nullptr };
    int m_selectedLang{ 0 };

    wxDECLARE_EVENT_TABLE();
    };

/** @}*/

#endif //__PROJECTWIZARD_H__
