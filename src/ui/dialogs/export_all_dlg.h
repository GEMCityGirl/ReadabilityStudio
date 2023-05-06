#ifndef __EXPORTALL_DLG_H__
#define __EXPORTALL_DLG_H__

#include <wx/wx.h>
#include <wx/string.h>
#include <wx/valgen.h>
#include <wx/combobox.h>
#include <wx/tooltip.h>
#include "../../Wisteria-Dataviz/src/ui/dialogs/imageexportdlg.h"
#include "../../Wisteria-Dataviz/src/ui/dialogs/dialogwithhelp.h"

class BaseProjectDoc;
class BatchProjectDoc;
class ProjectDoc;

/// @brief Dialog for exporting all results.
class ExportAllDlg final : public Wisteria::UI::DialogWithHelp
    {
public:
    /// @brief Constructor.
    ExportAllDlg(wxWindow* parent, BaseProjectDoc* doc,
                 const bool fileMode,
                 wxWindowID id = wxID_ANY, const wxString& caption = _(L"Export Options"),
                 const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                 long style = wxDEFAULT_DIALOG_STYLE|wxCLIP_CHILDREN|wxRESIZE_BORDER);
    /// @private
    ExportAllDlg() = delete;
    /// @private
    ExportAllDlg(const ExportAllDlg& that) = delete;
    /// @private
    ExportAllDlg& operator=(const ExportAllDlg&) = delete;

    /// @returns The export folder path.
    [[nodiscard]]
    const wxString& GetFolderPath() const noexcept
        { return m_folderPath; }
    /// @returns The file path of where the file(s) was saved.
    [[nodiscard]]
    const wxString& GetFilePath() const noexcept
        { return m_filePath; }
    /// @returns The selected file extension for exported text windows.
    [[nodiscard]]
    const wxString& GetExportTextViewExt() const noexcept
        { return m_textViewExt; }
     /// @returns The selected file extension for exported list windows.
    [[nodiscard]]
    const wxString& GetExportListExt() const noexcept
        { return m_listExt; }
     /// @returns The selected file extension for exported graphs.
    [[nodiscard]]
    const wxString& GetExportGraphExt() const noexcept
        { return m_graphExt; }
    /// @returns The selected export options.
    [[nodiscard]]
    const Wisteria::UI::ImageExportOptions& GetImageExportOptions() const noexcept
        { return m_imageExportOptions; }
    /// @private
    [[nodiscard]]
    Wisteria::UI::ImageExportOptions& GetImageExportOptions() noexcept
        { return m_imageExportOptions; }

    /// @returns @c true if exporting hard-word lists.
    [[nodiscard]]
    bool IsExportingHardWordLists() const noexcept
        { return m_exportHardWordLists; }
    /// @returns @c true if exporting the sentences section.
    [[nodiscard]]
    bool IsExportingSentencesBreakdown() const noexcept
        { return m_exportSentencesBreakdown; }
    /// @returns @c true if exporting graphs.
    [[nodiscard]]
    bool IsExportingGraphs() const noexcept
        { return m_exportGraphs; }
    /// @returns @c true if exporting test results.
    [[nodiscard]]
    bool IsExportingTestResults() const noexcept
        { return m_exportTestResults; }
    /// @returns @c true if exporting the statistics section.
    [[nodiscard]]
    bool IsExportingStatistics() const noexcept
        { return m_exportStatistics; }
    /// @returns @c true if exporting the grammar section.
    [[nodiscard]]
    bool IsExportingGrammar() const noexcept
        { return m_exportWordiness; }
    /// @returns @c true if exporting the Dolch section.
    [[nodiscard]]
    bool IsExportingSightWords() const noexcept
        { return m_exportSightWords; }
    /// @returns @c true if exporting the warnings section.
    [[nodiscard]]
    bool IsExportingWarnings() const noexcept
        { return m_exportWarnings; }
    /// @returns @c true if exporting lists (from any section).
    [[nodiscard]]
    bool IsExportingLists() const noexcept
        { return m_exportingLists; }
    /// @returns @c true if exporting text windows (from any section).
    [[nodiscard]]
    bool IsExportingTextReports() const noexcept
        { return m_exportingTextReports; }
private:
    static constexpr int ID_FOLDER_BROWSE_BUTTON = wxID_HIGHEST;
    static constexpr int ID_IMAGE_OPTIONS_BUTTON = wxID_HIGHEST + 1;
    static constexpr int ID_INCLUDE_LIST_CHECKBOX = wxID_HIGHEST + 2;
    static constexpr int ID_INCLUDE_TEXT_CHECKBOX = wxID_HIGHEST + 3;
    static constexpr int ID_TEXT_TYPE_LABEL = wxID_HIGHEST + 4;
    static constexpr int ID_TEXT_TYPE_COMBO = wxID_HIGHEST + 5;
    static constexpr int ID_LIST_TYPE_LABEL = wxID_HIGHEST + 6;
    static constexpr int ID_LIST_TYPE_COMBO = wxID_HIGHEST + 7;

    /// Creation
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& caption = _(L"Export Options"),
                const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_DIALOG_STYLE|wxCLIP_CHILDREN|wxRESIZE_BORDER)
        {
        SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS|wxWS_EX_CONTEXTHELP);
        Wisteria::UI::DialogWithHelp::Create( parent, id, caption, pos, size, style );

        CreateControls();
        GetSizer()->Fit(this);
        GetSizer()->SetSizeHints(this);
        Centre();
        return true;
        }

    void CreateControls();
    void OnOK([[maybe_unused]] wxCommandEvent& event);
    void OnFolderBrowseButtonClick([[maybe_unused]] wxCommandEvent& event);
    void OnImageOptionsButtonClick([[maybe_unused]] wxCommandEvent& event);
    void OnIncludeListsTextWindowsCheck([[maybe_unused]] wxCommandEvent& event);

    [[nodiscard]]
    bool IsStandardProject() const;
    [[nodiscard]]
    bool IsBatchProject() const;

    BaseProjectDoc* m_readabilityProjectDoc{ nullptr };
    wxBitmapButton* m_folderBrowseButton{ nullptr };
    wxComboBox* m_textViewCombo{ nullptr };
    wxComboBox* m_listCombo{ nullptr };
    wxComboBox* m_graphCombo{ nullptr };
    wxString m_folderPath;
    wxString m_filePath;
    wxString m_textViewExt;
    wxString m_listExt;
    wxString m_graphExt;

    Wisteria::UI::ImageExportOptions m_imageExportOptions;

    bool m_exportHardWordLists{ false };
    bool m_exportSentencesBreakdown{ false };
    bool m_exportGraphs{ false };
    bool m_exportTestResults{ false };
    bool m_exportStatistics{ false };
    bool m_exportWordiness{ false };
    bool m_exportSightWords{ false };
    bool m_exportWarnings{ false };
    bool m_exportingLists{ false };
    bool m_exportingTextReports{ false };

    bool m_fileMode{ true };
    };

#endif //__EXPORTALL_DLG_H__
