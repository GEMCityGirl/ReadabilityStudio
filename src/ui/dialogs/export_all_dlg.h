#ifndef __WXEXPORTALL_DLG_H__
#define __WXEXPORTALL_DLG_H__

#include <wx/wx.h>
#include <wx/string.h>
#include <wx/valgen.h>
#include <wx/combobox.h>
#include <wx/tooltip.h>
#include "../../../../SRC/Wisteria-Dataviz/src/ui/dialogs/imageexportdlg.h"
#include "../../../../SRC/Wisteria-Dataviz/src/ui/dialogs/dialogwithhelp.h"

class BaseProjectDoc;
class BatchProjectDoc;
class ProjectDoc;

class ExportAllDlg: public Wisteria::UI::DialogWithHelp
    {
public:
    /// Constructors
    ExportAllDlg(wxWindow* parent, BaseProjectDoc* doc,
                 const bool fileMode,
                 wxWindowID id = wxID_ANY, const wxString& caption = _("Export Options"),
                 const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                 long style = wxDEFAULT_DIALOG_STYLE|wxCLIP_CHILDREN|wxRESIZE_BORDER);
    ExportAllDlg() = delete;

    /// Creation
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& caption = _("Export Options"),
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

    [[nodiscard]] const wxString& GetFolderPath() const noexcept
        { return m_folderPath; }
    /// @returns The file path of where the file(s) was saved.
    [[nodiscard]] const wxString& GetFilePath() const noexcept
        { return m_filePath; }
    [[nodiscard]] const wxString& GetExportTextViewExt() const noexcept
        { return m_textViewExt; }
    [[nodiscard]] const wxString& GetExportListExt() const noexcept
        { return m_listExt; }
    [[nodiscard]] const wxString& GetExportGraphExt() const noexcept
        { return m_graphExt; }
    [[nodiscard]] const Wisteria::UI::ImageExportOptions& GetImageExportOptions() const noexcept
        { return m_imageExportOptions; }
    [[nodiscard]] Wisteria::UI::ImageExportOptions& GetImageExportOptions() noexcept
        { return m_imageExportOptions; }

    [[nodiscard]] bool IsExportingHardWordLists() const noexcept
        { return m_exportHardWordLists; }
    [[nodiscard]] bool IsExportingSentencesBreakdown() const noexcept
        { return m_exportSentencesBreakdown; }
    [[nodiscard]] bool IsExportingGraphs() const noexcept
        { return m_exportGraphs; }
    [[nodiscard]] bool IsExportingTestResults() const noexcept
        { return m_exportTestResults; }
    [[nodiscard]] bool IsExportingStatistics() const noexcept
        { return m_exportStatistics; }
    [[nodiscard]] bool IsExportingWordiness() const noexcept
        { return m_exportWordiness; }
    [[nodiscard]] bool IsExportingSightWords() const noexcept
        { return m_exportSightWords; }
    [[nodiscard]] bool IsExportingWarnings() const noexcept
        { return m_exportWarnings; }
    [[nodiscard]] bool IsExportingLists() const noexcept
        { return m_exportingLists; }
    [[nodiscard]] bool IsExportingTextReports() const noexcept
        { return m_exportingTextReports; }

    [[nodiscard]] bool IsStandardProject() const;
    [[nodiscard]] bool IsBatchProject() const;
private:
    static constexpr int ID_FOLDER_BROWSE_BUTTON = 1001;
    static constexpr int ID_IMAGE_OPTIONS_BUTTON = 1002;
    static constexpr int ID_INCLUDE_LIST_CHECKBOX = 1003;
    static constexpr int ID_INCLUDE_TEXT_CHECKBOX = 1008;
    static constexpr int ID_TEXT_TYPE_LABEL = 1004;
    static constexpr int ID_TEXT_TYPE_COMBO = 1005;
    static constexpr int ID_LIST_TYPE_LABEL = 1006;
    static constexpr int ID_LIST_TYPE_COMBO = 1007;
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

    wxDECLARE_NO_COPY_CLASS(ExportAllDlg);
    wxDECLARE_EVENT_TABLE();
    };

#endif //__WXEXPORTALL_DLG_H__
