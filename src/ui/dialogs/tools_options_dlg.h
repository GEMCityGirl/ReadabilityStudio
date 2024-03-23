/** @addtogroup UI
    @brief Classes for the user interface.
    @date 2005-2023
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __TOOLS_OPTIONS_DIALOG_H__
#define __TOOLS_OPTIONS_DIALOG_H__

#include "../../Wisteria-Dataviz/src/base/image.h"
#include "../../Wisteria-Dataviz/src/ui/controls/sidebarbook.h"
#include "../../Wisteria-Dataviz/src/util/backupvariable.h"
#include "../../Wisteria-Dataviz/src/util/warningmanager.h"
#include "../../projects/batch_project_view.h"
#include "../../projects/standard_project_view.h"
#include "../../readability/readability.h"
#include <limits>
#include <map>
#include <wx/aui/auibook.h>
#include <wx/fontdlg.h>
#include <wx/propgrid/advprops.h>
#include <wx/propgrid/manager.h>
#include <wx/propgrid/propgrid.h>
#include <wx/spinctrl.h>
#include <wx/statline.h>
#include <wx/tooltip.h>
#include <wx/treebook.h>
#include <wx/valgen.h>

class BaseProjectDoc;
class BatchProjectDoc;
class ProjectDoc;
class ReadabilityApp;

class ToolsOptionsDlg final : public wxDialog
    {
    friend ReadabilityApp;

  public:
    enum ToolSections
        {
        ProjectSection = 0x0010,
        GraphsSection = 0x0020,
        TextSection = 0x0040,
        ScoresSection = 0x0080,
        DocumentIndexing = 0x0100,
        Grammar = 0x0200,
        Statistics = 0x0400,
        WordsBreakdown = 0x0800,
        SentencesBreakdown = 0x1000,
        UNUSED1 = 0x2000,
        UNUSED2 = 0x4000,
        AllSections = (ProjectSection | GraphsSection | ScoresSection | TextSection |
        DocumentIndexing | Grammar | Statistics | WordsBreakdown | SentencesBreakdown)
        };

    /// Constructor.
    /// @param parent The parent of this dialog.
    /// @param project The project to connect this dialog to. If null, then this is
    ///     treating as a global settings dialog.
    /// @param sectionsToInclude The sections of options to include. This useful
    ///     for only showing a single section (e.g., graphs), thus making this
    ///     a simplified options editor.
    explicit ToolsOptionsDlg(wxWindow* parent, BaseProjectDoc* project = nullptr,
                             const ToolSections sectionsToInclude = AllSections);

    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& caption = _(L"Options"),
                const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

    /// @brief Selects the default page when opening the dialog.
    /// @param pageId The ID of the page to select.
    void SelectPage(const int pageId);

    // should be public so that clients can specify which page to select
    constexpr static int GENERAL_SETTINGS_PAGE = wxID_HIGHEST;
    constexpr static int PROJECT_SETTINGS_PAGE = wxID_HIGHEST + 1;

    constexpr static int DOCUMENT_DISPLAY_GENERAL_PAGE = wxID_HIGHEST + 2;
    constexpr static int DOCUMENT_DISPLAY_DOLCH_PAGE = wxID_HIGHEST + 3;

    constexpr static int SCORES_TEST_OPTIONS_PAGE = wxID_HIGHEST + 4;
    constexpr static int SCORES_DISPLAY_PAGE = wxID_HIGHEST + 5;

    constexpr static int ANALYSIS_INDEXING_PAGE = wxID_HIGHEST + 6;
    constexpr static int GRAMMAR_PAGE = wxID_HIGHEST + 7;
    constexpr static int ANALYSIS_STATISTICS_PAGE = wxID_HIGHEST + 8;
    constexpr static int WORDS_BREAKDOWN_PAGE = wxID_HIGHEST + 9;
    constexpr static int SENTENCES_BREAKDOWN_PAGE = wxID_HIGHEST + 10;

    constexpr static int GRAPH_GENERAL_PAGE = wxID_HIGHEST + 11;
    constexpr static int GRAPH_AXIS_PAGE = wxID_HIGHEST + 12;
    constexpr static int GRAPH_TITLES_PAGE = wxID_HIGHEST + 13;
    constexpr static int GRAPH_READABILITY_GRAPHS_PAGE = wxID_HIGHEST + 14;
    constexpr static int GRAPH_BAR_CHART_PAGE = wxID_HIGHEST + 15;
    constexpr static int GRAPH_HISTOGRAM_PAGE = wxID_HIGHEST + 16;
    constexpr static int GRAPH_BOX_PLOT_PAGE = wxID_HIGHEST + 17;

  private:
    constexpr static int ID_HIGHLIGHT_COLOR_BUTTON = wxID_HIGHEST + 18;
    constexpr static int ID_EXCLUDED_HIGHLIGHT_COLOR_BUTTON = wxID_HIGHEST + 19;
    constexpr static int ID_FONT_BUTTON = wxID_HIGHEST + 20;
    constexpr static int ID_DOCUMENT_STORAGE_RADIO_BOX = wxID_HIGHEST + 21;
    constexpr static int ID_FILE_BROWSE_BUTTON = wxID_HIGHEST + 22;
    constexpr static int ID_X_AXIS_FONT_BUTTON = wxID_HIGHEST + 23;
    constexpr static int ID_Y_AXIS_FONT_BUTTON = wxID_HIGHEST + 24;
    constexpr static int ID_GRAPH_TOP_TITLE_FONT_BUTTON = wxID_HIGHEST + 25;
    constexpr static int ID_GRAPH_BOTTOM_TITLE_FONT_BUTTON = wxID_HIGHEST + 26;
    constexpr static int ID_GRAPH_LEFT_TITLE_FONT_BUTTON = wxID_HIGHEST + 27;
    constexpr static int ID_GRAPH_RIGHT_TITLE_FONT_BUTTON = wxID_HIGHEST + 28;
    constexpr static int ID_PARAGRAPH_PARSE = wxID_HIGHEST + 29;
    constexpr static int ID_DUP_WORD_COLOR_BUTTON = wxID_HIGHEST + 30;
    constexpr static int ID_WORDY_PHRASE_COLOR_BUTTON = wxID_HIGHEST + 31;
    constexpr static int ID_DOLCH_CONJUNCTIONS_COLOR_BUTTON = wxID_HIGHEST + 32;
    constexpr static int ID_DOLCH_PREPOSITIONS_COLOR_BUTTON = wxID_HIGHEST + 33;
    constexpr static int ID_DOLCH_PRONOUNS_COLOR_BUTTON = wxID_HIGHEST + 34;
    constexpr static int ID_DOLCH_ADVERBS_COLOR_BUTTON = wxID_HIGHEST + 35;
    constexpr static int ID_DOLCH_ADJECTIVES_COLOR_BUTTON = wxID_HIGHEST + 36;
    constexpr static int ID_DOLCH_NOUN_COLOR_BUTTON = wxID_HIGHEST + 37;
    constexpr static int ID_EXPORT_SETTINGS_BUTTON = wxID_HIGHEST + 38;
    constexpr static int ID_LOAD_SETTINGS_BUTTON = wxID_HIGHEST + 39;
    constexpr static int ID_RESET_SETTINGS_BUTTON = wxID_HIGHEST + 40;
    constexpr static int ID_TEXT_EXCLUDE_METHOD = wxID_HIGHEST + 41;
    constexpr static int ID_SENTENCE_LONGER_THAN_BUTTON = wxID_HIGHEST + 42;
    constexpr static int ID_SENTENCE_OUTLIER_LENGTH_BUTTON = wxID_HIGHEST + 43;
    constexpr static int ID_IGNORE_BLANK_LINES_BUTTON = wxID_HIGHEST + 44;
    constexpr static int ID_IGNORE_INDENTING_BUTTON = wxID_HIGHEST + 45;
    constexpr static int ID_WARNING_MESSAGES_BUTTON = wxID_HIGHEST + 46;
    constexpr static int ID_ADD_FILE_BUTTON = wxID_HIGHEST + 47;
    constexpr static int ID_DELETE_FILE_BUTTON = wxID_HIGHEST + 48;
    constexpr static int ID_NUMBER_SYLLABIZE_METHOD = wxID_HIGHEST + 49;
    constexpr static int ID_DOLCH_VERBS_COLOR_BUTTON = wxID_HIGHEST + 50;
    constexpr static int ID_AGGRESSIVE_LIST_DEDUCTION_CHECKBOX = wxID_HIGHEST + 51;
    constexpr static int ID_EXCLUDED_PHRASES_FILE_EDIT_BUTTON = wxID_HIGHEST + 52;
    constexpr static int ID_EXCLUDE_COPYRIGHT_CHECKBOX = wxID_HIGHEST + 53;
    constexpr static int ID_EXCLUDE_CITATIONS_CHECKBOX = wxID_HIGHEST + 54;
    constexpr static int ID_EXCLUDE_FILE_ADDRESS_CHECKBOX = wxID_HIGHEST + 55;
    constexpr static int ID_EXCLUDE_NUMERALS_CHECKBOX = wxID_HIGHEST + 56;
    constexpr static int ID_EXCLUDE_PROPER_NOUNS_CHECKBOX = wxID_HIGHEST + 57;
    constexpr static int ID_INCOMPLETE_SENTENCE_VALID_VALUE_BOX = wxID_HIGHEST + 58;
    constexpr static int ID_APPLY_GRAPH_BACKGROUND_COLOR_FADE_CHECK_BOX = wxID_HIGHEST + 59;
    constexpr static int ID_GRAPH_BACKGROUND_OPACITY_SLIDER_LABEL = wxID_HIGHEST + 60;
    constexpr static int ID_GRAPH_BACKGROUND_OPACITY_SLIDER_PLUS_LABEL = wxID_HIGHEST + 61;
    constexpr static int ID_PLOT_COLOR_LABEL = wxID_HIGHEST + 62;
    constexpr static int ID_WATERMARK_LABEL = wxID_HIGHEST + 63;
    constexpr static int ID_WATERMARK_TEXT_BOX = wxID_HIGHEST + 64;
    constexpr static int ID_GRAPH_LOGO_LABEL = wxID_HIGHEST + 65;
    constexpr static int ID_BAR_CHART_EFFECTS_OPTIONS = wxID_HIGHEST + 66;
    constexpr static int ID_BAR_CHART_HORIZONTAL_OPTION = wxID_HIGHEST + 67;
    constexpr static int ID_BAR_CHART_VERTICAL_OPTION = wxID_HIGHEST + 68;
    constexpr static int ID_DOCUMENT_FONT_LABEL = wxID_HIGHEST + 69;
    constexpr static int ID_GRAPH_OPTIONS_PROPERTYGRID = wxID_HIGHEST + 70;
    constexpr static int ID_BARCHART_OPTIONS_PROPERTYGRID = wxID_HIGHEST + 71;
    constexpr static int ID_INCLUDE_FIRST_OCCURRENCE_EXCLUDE_CHECKBOX = wxID_HIGHEST + 72;
    constexpr static int ID_INCOMPLETE_SENTENCE_VALID_LABEL_START = wxID_HIGHEST + 73;
    constexpr static int ID_INCOMPLETE_SENTENCE_VALID_LABEL_END = wxID_HIGHEST + 74;
    constexpr static int ID_EXCLUSION_TAG_BLOCK_LABEL = wxID_HIGHEST + 75;
    constexpr static int ID_EXCLUSION_TAG_BLOCK_SELCTION = wxID_HIGHEST + 76;
    constexpr static int ID_ADDITIONAL_FILE_BROWSE_BUTTON = wxID_HIGHEST + 77;
    constexpr static int ID_ADD_FILES_BUTTON = wxID_HIGHEST + 78;
    constexpr static int ID_THEME_COMBO = wxID_HIGHEST + 79;
    constexpr static int ID_ADDITIONAL_FILE_FIELD = wxID_HIGHEST + 80;
    constexpr static int ID_SENTENCES_MUST_BE_CAP_BUTTON = wxID_HIGHEST + 81;
    constexpr static int ID_REALTIME_UPDATE_BUTTON = wxID_HIGHEST + 82;
    constexpr static int ID_DOCUMENT_DESCRIPTION_FIELD = wxID_HIGHEST + 83;
    constexpr static int ID_FILE_LIST = wxID_HIGHEST + 84;
    constexpr static int ID_DOCUMENT_PATH_FIELD = wxID_HIGHEST + 85;

    // button events
    void OnExportSettings([[maybe_unused]] wxCommandEvent& event);
    void OnImportSettings([[maybe_unused]] wxCommandEvent& event);
    void OnResetSettings([[maybe_unused]] wxCommandEvent& event);
    void OnParagraphParseChange([[maybe_unused]] wxCommandEvent& event);
    void OnHighlightColorSelect([[maybe_unused]] wxCommandEvent& event);
    void OnExcludedHighlightColorSelect([[maybe_unused]] wxCommandEvent& event);
    void OnDupWordHighlightColorSelect([[maybe_unused]] wxCommandEvent& event);
    void OnWordyPhraseHighlightColorSelect([[maybe_unused]] wxCommandEvent& event);
    void OnFontSelect(wxCommandEvent& event);
    void OnDocumentStorageRadioButtonClick([[maybe_unused]] wxCommandEvent& event);
    void OnFileBrowseButtonClick([[maybe_unused]] wxCommandEvent& event);
    void OnAdditionalDocumentFileBrowseButtonClick([[maybe_unused]] wxCommandEvent& event);
    void OnIncompleteSentencesChange([[maybe_unused]] wxCommandEvent& event);
    void OnExcludedPhrasesFileEditButtonClick([[maybe_unused]] wxCommandEvent& event);
    void OnExcludeNumeralsCheck(wxCommandEvent& event);
    // dolch color buttons
    void OnDolchConjunctionsHighlightColorSelect([[maybe_unused]] wxCommandEvent& event);
    void OnDolchPrepositionsHighlightColorSelect([[maybe_unused]] wxCommandEvent& event);
    void OnDolchPronounsHighlightColorSelect([[maybe_unused]] wxCommandEvent& event);
    void OnDolchAdverbsHighlightColorSelect([[maybe_unused]] wxCommandEvent& event);
    void OnDolchHighlightColorSelect(wxCommandEvent& event);
    void OnDolchNounHighlightColorSelect([[maybe_unused]] wxCommandEvent& event);
    // general events
    void OnWarningMessagesButtonClick([[maybe_unused]] wxCommandEvent& event);
    void OnHelp([[maybe_unused]] wxCommandEvent& event);
    void OnContextHelp([[maybe_unused]] wxHelpEvent& event);
    void OnOK([[maybe_unused]] wxCommandEvent& event);
    void OnAddFilesClick([[maybe_unused]] wxCommandEvent& event);
    void OnAddFileClick([[maybe_unused]] wxCommandEvent& event);
    void OnDeleteFileClick([[maybe_unused]] wxCommandEvent& event);
    void OnNumberSyllabizeChange([[maybe_unused]] wxCommandEvent& event);
    void OnExclusionBlockTagChange([[maybe_unused]] wxCommandEvent& event);

    // state functions
    [[nodiscard]]
    bool IsGeneralSettings() const noexcept;
    [[nodiscard]]
    bool IsStandardProjectSettings() const;
    [[nodiscard]]
    bool IsBatchProjectSettings() const;

    // saving functions
    bool ValidateOptions();
    void SaveOptions();
    void SaveProjectGraphOptions();
    void SaveTextWindowOptions();
    void SaveStatisticsOptions();
    [[nodiscard]]
    bool HaveOptionsChanged() const;
    [[nodiscard]]
    bool HaveDocumentOptionsChanged() const;
    [[nodiscard]]
    bool HaveGraphOptionsChanged() const;
    [[nodiscard]]
    bool HaveTextViewOptionsChanged() const noexcept;
    [[nodiscard]]
    bool HaveTestDisplayOptionsChanged() const;
    [[nodiscard]]
    bool HaveStatisticsOptionsChanged() const;
    [[nodiscard]]
    bool HaveWordsBreakdownOptionsChanged() const;
    [[nodiscard]]
    bool HaveSentencesBreakdownOptionsChanged() const;

    [[nodiscard]]
    ListCtrlEx* GetFileList() noexcept
        {
        return m_fileList;
        }

    /// Creates the controls and sizers
    void CreateControls();
    void CreateGraphSection();

    [[nodiscard]]
    wxString GetCustomTestsLabel() const
        {
        return _(L"Custom Tests");
        }

    [[nodiscard]]
    wxString GetInvalidRegionsColorLabel() const
        {
        return _(L"Invalid regions color");
        }

    [[nodiscard]]
    wxString GetRaygorStyleLabel() const
        {
        return _(L"Raygor style");
        }

    [[nodiscard]]
    wxString GetFleschChartConnectPointsLabel() const
        {
        return _(L"Connect points");
        }

    [[nodiscard]]
    wxString GetFleschSyllableRulerDocGroupsLabel() const
        {
        return _(L"Display grouped documents on syllable ruler");
        }

    [[nodiscard]]
    wxString GetUseEnglishLabelsForGermanLixLabel() const
        {
        return _(L"Use English translations for German Lix gauge");
        }

    [[nodiscard]]
    wxString GetColorLabel() const
        {
        return _(L"Color");
        }

    [[nodiscard]]
    wxString GetBackgroundColorLabel() const
        {
        return _DT(L"BGCOLOR");
        }

    [[nodiscard]]
    wxString GetEffectLabel() const
        {
        return _(L"Effect");
        }

    [[nodiscard]]
    wxString GetFitLabel() const
        {
        return _(L"Fit");
        }

    [[nodiscard]]
    wxString GetOpacityLabel() const
        {
        return _(L"Opacity");
        }

    [[nodiscard]]
    wxString GetImageFitLabel() const
        {
        return GetImageLabel() + L"." + GetFitLabel();
        }

    [[nodiscard]]
    wxString GetColorOpacityLabel() const
        {
        return GetColorLabel() + L"." + GetOpacityLabel();
        }

    [[nodiscard]]
    wxString GetImageOpacityLabel() const
        {
        return GetImageLabel() + L"." + GetOpacityLabel();
        }

    [[nodiscard]]
    wxString GetImageEffectLabel() const
        {
        return GetImageLabel() + L"." + GetEffectLabel();
        }

    [[nodiscard]]
    wxString GetLabelsOnBarsLabel() const
        {
        return _(L"Display labels on bars");
        }

    [[nodiscard]]
    wxString GetLabelsOnBoxesLabel() const
        {
        return _(L"Display box & whisker labels");
        }

    [[nodiscard]]
    wxString GetConnectBoxesLabel() const
        {
        return _(L"Connect middle points");
        }

    [[nodiscard]]
    wxString GetShowAllDataPointsLabel() const
        {
        return _(L"Show all data points");
        }

    [[nodiscard]]
    wxString GeOrientationLabel() const
        {
        return _(L"Orientation");
        }

    [[nodiscard]]
    wxString GetBinSortingLabel() const
        {
        return _(L"Sorting");
        }

    [[nodiscard]]
    wxString GetGradeLevelRoundingLabel() const
        {
        return _(L"Grade level/index value rounding");
        }

    [[nodiscard]]
    wxString GetIntervalDisplayLabel() const
        {
        return _(L"Interval display");
        }

    [[nodiscard]]
    wxString GetBinLabelsLabel() const
        {
        return _(L"Labels");
        }

    [[nodiscard]]
    wxString GetDisplayDropShadowsLabel() const
        {
        return _(L"Display drop shadows");
        }

    [[nodiscard]]
    wxString GetShowcaseComplexWordsLabel() const
        {
        return _(L"Showcase complex words");
        }

    [[nodiscard]]
    wxString GetStippleImageLabel() const
        {
        return _(L"Stipple image");
        }

    [[nodiscard]]
    wxString GetStippleShapeLabel() const
        {
        return _(L"Stipple shape");
        }

    [[nodiscard]]
    wxString GetStippleShapeColorLabel() const
        {
        return _(L"Stipple shape") + L"." + GetColorLabel();
        }

    [[nodiscard]]
    wxString GetCommonImageLabel() const
        {
        return _(L"Common image");
        }

    [[nodiscard]]
    wxString GetWatermarkLabel() const
        {
        return _(L"Watermark");
        }

    [[nodiscard]]
    wxString GetLogoImageLabel() const
        {
        return _(L"Logo image");
        }

    [[nodiscard]]
    wxString GetImageLabel() const
        {
        return _(L"Image");
        }

    [[nodiscard]]
    wxString GetApplyFadeLabel() const
        {
        return _(L"Fade");
        }

    [[nodiscard]]
    wxString GetBackgroundColorFadeLabel() const
        {
        return GetBackgroundColorLabel() + L"." + GetApplyFadeLabel();
        }

    [[nodiscard]]
    wxString GetBackgroundLabel() const
        {
        return _(L"Background");
        }

    [[nodiscard]]
    wxString GetIgnoreProperNounsLabel() const
        {
        return _(L"Ignore proper nouns");
        }

    [[nodiscard]]
    wxString GetIgnoreUppercasedWordsLabel() const
        {
        return _(L"Ignore UPPERCASED words");
        }

    [[nodiscard]]
    wxString GetIgnoreNumeralsLabel() const
        {
        return _(L"Ignore numerals");
        }

    [[nodiscard]]
    wxString GetIgnoreFileAddressesLabel() const
        {
        return _(L"Ignore Internet and file addresses");
        }

    [[nodiscard]]
    wxString GetIgnoreProgrammerCodeLabel() const
        {
        return _(L"Ignore programmer code");
        }

    [[nodiscard]]
    wxString GetAllowColloquialismsLabel() const
        {
        return _(L"Allow colloquialisms");
        }

    [[nodiscard]]
    wxString GetIgnoreSocialMediaLabel() const
        {
        return _(L"Ignore social media hashtags");
        }

    [[nodiscard]]
    wxString GetResultsLabel() const
        {
        return _(L"Results");
        }

    [[nodiscard]]
    wxString GetGrammarHighlightedReportLabel() const
        {
        return _(L"Highlighted Report");
        }

    [[nodiscard]]
    wxString GetSpellCheckerLabel() const
        {
        return _(L"Spell Checker");
        }

    [[nodiscard]]
    wxString GetDolchStatisticsReportLabel() const
        {
        return _(L"Dolch Report");
        }

    [[nodiscard]]
    wxString GetCoverageLabel() const
        {
        return _(L"Coverage");
        }

    [[nodiscard]]
    wxString GetDolchWordsLabel() const
        {
        return _DT(L"DOLCHWORDS");
        }

    [[nodiscard]]
    wxString GetDolchExplanationLabel() const
        {
        return _(L"Explanation");
        }

    [[nodiscard]]
    wxString GetStatisticsReportLabel() const
        {
        return _(L"Summary Report");
        }

    [[nodiscard]]
    wxString GetParagraphsLabel() const
        {
        return _(L"Paragraphs");
        }

    [[nodiscard]]
    wxString GetSentencesLabel() const
        {
        return _(L"Sentences");
        }

    [[nodiscard]]
    wxString GetWordsLabel() const
        {
        return _(L"Words");
        }

    [[nodiscard]]
    wxString GetExtendedWordsLabel() const
        {
        return _(L"Extended Words");
        }

    [[nodiscard]]
    wxString GetGrammarLabel() const
        {
        return _(L"Grammar");
        }

    [[nodiscard]]
    wxString GetNotesLabel() const
        {
        return _(L"Notes");
        }

    [[nodiscard]]
    wxString GetExtendedInformationLabel() const
        {
        return _(L"Extended Information");
        }

    [[nodiscard]]
    wxString GetFryLikeLabel() const
        {
        return _(L"Fry/GPM/Raygor/Schwartz");
        }

    [[nodiscard]]
    wxString GetFleschChartLabel() const
        {
        return _(L"Flesch Chart");
        }

    [[nodiscard]]
    wxString GetLixGaugeLabel() const
        {
        return _(L"Lix Gauge");
        }

    [[nodiscard]]
    wxString GetBarAppearanceLabel() const
        {
        return _(L"Bar Appearance");
        }

    [[nodiscard]]
    wxString GetBinningOptionsLabel() const
        {
        return _(L"Binning Options");
        }

    [[nodiscard]]
    wxString GetBinDisplayLabel() const
        {
        return _(L"Bin Display");
        }

    [[nodiscard]]
    wxString GetBoxAppearanceLabel() const
        {
        return _(L"Box Appearance");
        }

    [[nodiscard]]
    wxString GetBoxOptionsLabel() const
        {
        return _(L"Box Options");
        }

    [[nodiscard]]
    wxString GetGraphColorSchemeLabel() const
        {
        return _(L"Color Scheme");
        }

    [[nodiscard]]
    wxString GetGraphBackgroundLabel() const
        {
        return _(L"Background");
        }

    [[nodiscard]]
    wxString GetPlotAreaBackgroundLabel() const
        {
        return _(L"Plot Background");
        }

    [[nodiscard]]
    wxString GetWatermarksLogosLabel() const
        {
        return _(L"Watermarks & Logos");
        }

    [[nodiscard]]
    wxString GetEffectsLabel() const
        {
        return _(L"Effects");
        }

    [[nodiscard]]
    wxString GetGradeScaleLabel() const
        {
        return _(L"Grade scale");
        }

    [[nodiscard]]
    wxString GetGradeLabel() const
        {
        return _(L"Grade Display");
        }

    [[nodiscard]]
    wxString GetGradesLongFormatLabel() const
        {
        return _(L"Display scores in long format");
        }

    [[nodiscard]]
    wxString GetReadingAgeLabel() const
        {
        return _(L"Reading Age Display");
        }

    [[nodiscard]]
    wxString GetScoreResultsLabel() const
        {
        return _(L"Results");
        }

    [[nodiscard]]
    wxString GetIncludeScoreSummaryLabel() const
        {
        return _(L"Include test-summary report");
        }

    [[nodiscard]]
    wxString GetCalculationLabel() const
        {
        return _(L"Calculation");
        }

    [[nodiscard]]
    wxString GetCountIndependentClausesLabel() const
        {
        return _(L"Count independent clauses");
        }

    [[nodiscard]]
    wxString GetNumeralSyllabicationLabel() const
        {
        return _(L"Numeral syllabication");
        }

    [[nodiscard]]
    wxString GetFleschNumeralSyllabicationLabel() const
        {
        return _DT(L"FLESCHNUMSYL");
        }

    [[nodiscard]]
    wxString GetFleschKincaidNumeralSyllabicationLabel() const
        {
        return _DT(L"FLESCHKINCAIDNUMSYL");
        }

    [[nodiscard]]
    wxString GetIncludeStockerLabel() const
        {
        return _(L"Include Catholic Supplement");
        }

    [[nodiscard]]
    wxString GetProperNounsLabel() const
        {
        return _(L"Proper nouns");
        }

    [[nodiscard]]
    wxString GetTextExclusionLabel() const
        {
        return _(L"Text exclusion");
        }

    [[nodiscard]]
    wxString GetDCTextExclusionLabel() const
        {
        return _DT(L"DCTEXTEXCLUSION");
        }

    [[nodiscard]]
    wxString GetHJCTextExclusionLabel() const
        {
        return _DT(L"HJTEXTEXCLUSION");
        }

    [[nodiscard]]
    static wxString GetReadabilityScoresLabel()
        {
        return _(L"Readability Scores");
        }

    [[nodiscard]]
    static wxString GetGeneralSettingsLabel()
        {
        return _(L"General Settings");
        }

    [[nodiscard]]
    static wxString GetStatisticsLabel()
        {
        return _(L"Statistics");
        }

    [[nodiscard]]
    static wxString GetAnalysisLabel()
        {
        return _(L"Analysis");
        }

    [[nodiscard]]
    static wxString GetDolchSightWordsLabel()
        {
        return _(L"Dolch Sight Words");
        }

    [[nodiscard]]
    static wxString GetDocumentViewsLabel()
        {
        return _(L"Document Views");
        }

    [[nodiscard]]
    static wxString GetDifficultWordListsLabel()
        {
        return _(L"Difficult word lists");
        }

    [[nodiscard]]
    static wxString GetGraphsLabel()
        {
        return ProjectView::GetGraphsLabel();
        }

    [[nodiscard]]
    static wxString GetGeneralLabel()
        {
        return _(L"General");
        }

    [[nodiscard]]
    static wxString GetAxisSettingsLabel()
        {
        return _(L"Axes");
        }

    [[nodiscard]]
    static wxString GetTitlesLabel()
        {
        return _(L"Titles");
        }

    [[nodiscard]]
    static wxString GetReadabilityGraphLabel()
        {
        return _(L"Readability Graphs");
        }

    [[nodiscard]]
    static wxString GetBarChartLabel()
        {
        return _(L"Bar Charts");
        }

    [[nodiscard]]
    static wxString GetBoxPlotLabel()
        {
        return _(L"Box Plots");
        }

    [[nodiscard]]
    static wxString GetHistogramsLabel()
        {
        return _(L"Histograms");
        }

    [[nodiscard]]
    static wxString GetProjectSettingsLabel()
        {
        return _(L"Project Settings");
        }

    [[nodiscard]]
    static wxString GetSentencesWordsLabel()
        {
        return _(L"Sentences && Words");
        }

    [[nodiscard]]
    static wxString GetTestOptionsLabel()
        {
        return _(L"Test Options");
        }

    [[nodiscard]]
    static wxString GetScoreDisplayLabel()
        {
        return _(L"Scores");
        }

    /// Replaces various filepath shortcuts to the full pathway
    [[nodiscard]]
    wxString ExpandPath(wxString path) const;

    [[nodiscard]]
    ToolSections GetSectionsBeingShown() const noexcept
        {
        return m_sectionsBeingShown;
        }

    static bool IsPropertyAvailable(const wxPropertyGridPage* propGrid,
                                    const wxString& propertyName)
        {
        return (propGrid && propGrid->GetProperty(propertyName));
        }

    void CreateLabelHeader(wxWindow* parent, wxSizer* parentSizer, const wxString& title,
                           const bool addSidePadding = false) const;

    BaseProjectDoc* m_readabilityProjectDoc{ nullptr };
    wxRadioBox* m_docStorageRadioBox{ nullptr };
    wxCheckBox* m_realTimeUpdateCheckBox{ nullptr };
    wxCheckBox* m_ignoreCopyrightsCheckBox{ nullptr };
    wxCheckBox* m_ignoreCitationsCheckBox{ nullptr };
    wxCheckBox* m_ignoreFileAddressesCheckBox{ nullptr };
    wxCheckBox* m_ignoreNumeralsCheckBox{ nullptr };
    wxCheckBox* m_ignoreProperNounsCheckBox{ nullptr };
    wxCheckBox* m_includeExcludedPhraseFirstOccurrenceCheckBox{ nullptr };
    wxTextCtrl* m_excludedPhrasesPathFilePathEdit{ nullptr };
    wxBitmapButton* m_excludedPhrasesEditBrowseButton{ nullptr };
    wxCheckBox* m_aggressiveExclusionCheckBox{ nullptr };
    wxCheckBox* m_ignoreBlankLinesCheckBox{ nullptr };
    wxCheckBox* m_ignoreIndentingCheckBox{ nullptr };
    wxButton* m_highlightColorButton{ nullptr };
    wxButton* m_excludedHighlightColorButton{ nullptr };
    wxButton* m_duplicateWordHighlightColorButton{ nullptr };
    wxButton* m_wordyPhraseHighlightColorButton{ nullptr };
    wxButton* m_FontButton{ nullptr };
    wxButton* m_xAxisFontButton{ nullptr };
    wxButton* m_yAxisFontButton{ nullptr };
    wxButton* m_graphTopTitleFontButton{ nullptr };
    wxButton* m_graphBottomTitleFontButton{ nullptr };
    wxButton* m_graphLeftTitleFontButton{ nullptr };
    wxButton* m_graphRightTitleFontButton{ nullptr };
    Wisteria::UI::SideBarBook* m_sideBar{ nullptr };
    wxTextCtrl* m_stippleFilePathEdit{ nullptr };
    wxTextCtrl* m_filePathEdit{ nullptr };
    wxStaticText* m_includeIncompleteSentSizeincludeIncompleteLabel{ nullptr };
    wxStaticText* m_includeIncompleteSentSizeWordsLabel{ nullptr };
    wxBitmapButton* m_fileBrowseButton{ nullptr };
    wxStaticText* m_syllableLabel{ nullptr };
    wxChoice* m_syllableCombo{ nullptr };
    wxChoice* m_exclusionBlockTagsCombo{ nullptr };
    wxStaticText* m_exclusionBlockTagsLabel{ nullptr };
    // dolch buttons
    wxBitmapButton* m_DolchConjunctionsColorButton{ nullptr };
    wxBitmapButton* m_DolchPrepositionsColorButton{ nullptr };
    wxBitmapButton* m_DolchPronounsColorButton{ nullptr };
    wxBitmapButton* m_DolchAdverbsColorButton{ nullptr };
    wxBitmapButton* m_DolchAdjectivesColorButton{ nullptr };
    wxBitmapButton* m_DolchVerbsColorButton{ nullptr };
    wxBitmapButton* m_DolchNounColorButton{ nullptr };

    // property grid controls
    wxPropertyGridPage* m_readabilityTestsPropertyGrid{ nullptr };
    wxPropertyGridPage* m_gradeLevelPropertyGrid{ nullptr };
    wxPropertyGridPage* m_grammarPropertyGrid{ nullptr };
    wxPropertyGridPage* m_wordsBreakdownPropertyGrid{ nullptr };
    wxPropertyGridPage* m_sentencesBreakdownPropertyGrid{ nullptr };
    wxPropertyGridPage* m_statisticsPropertyGrid{ nullptr };
    wxPropertyGridPage* m_generalGraphPropertyGrid{ nullptr };
    wxPropertyGridPage* m_readabilityGraphPropertyGrid{ nullptr };
    wxPropertyGridPage* m_barChartPropertyGrid{ nullptr };
    wxPropertyGridPage* m_histogramPropertyGrid{ nullptr };
    wxPropertyGridPage* m_boxPlotsPropertyGrid{ nullptr };

    wxStaticText* m_readTestsSyllableLabel{ nullptr };
    wxStaticText* m_textExclusionLabel{ nullptr };

    wxBitmapButton* m_addFilesButton{ nullptr };
    wxBitmapButton* m_addFileButton{ nullptr };
    wxBitmapButton* m_deleteFileButton{ nullptr };
    ListCtrlEx* m_fileList{ nullptr };
    std::shared_ptr<ListCtrlExDataProvider> m_fileData
        { std::make_shared<ListCtrlExDataProvider>() };

    BackupVariable<wxString> m_userAgent;
    BackupVariable<bool> m_disablePeerVerify;

    // project settings
    BackupVariable<int> m_projectLanguage;
    BackupVariable<wxString> m_reviewer;
    BackupVariable<wxString> m_status;
    BackupVariable<wxString> m_description;
    BackupVariable<wxString> m_appendedDocumentFilePath;
    BackupVariable<bool> m_realTimeUpdate;

    // which options are we showing
    ToolSections m_sectionsBeingShown;

    // document storage/linking information
    BackupVariable<int> m_documentStorageMethod;
    BackupVariable<wxString> m_filePath;

    BackupVariable<int> m_textHighlightMethod;
    BackupVariable<wxColour> m_highlightedColor;
    BackupVariable<wxColour> m_excludedTextHighlightColor;
    BackupVariable<wxColour> m_duplicateWordHighlightColor;
    BackupVariable<wxColour> m_wordyPhraseHighlightColor;
    BackupVariable<wxFont> m_font;
    BackupVariable<wxColour> m_fontColor;

    // dolch highlighting
    BackupVariable<wxColour> m_dolchConjunctionsColor;
    BackupVariable<wxColour> m_dolchPrepositionsColor;
    BackupVariable<wxColour> m_dolchPronounsColor;
    BackupVariable<wxColour> m_dolchAdverbsColor;
    BackupVariable<wxColour> m_dolchAdjectivesColor;
    BackupVariable<wxColour> m_dolchVerbsColor;
    BackupVariable<wxColour> m_dolchNounColor;

    BackupVariable<bool> m_highlightDolchConjunctions;
    BackupVariable<bool> m_highlightDolchPrepositions;
    BackupVariable<bool> m_highlightDolchPronouns;
    BackupVariable<bool> m_highlightDolchAdverbs;
    BackupVariable<bool> m_highlightDolchAdjectives;
    BackupVariable<bool> m_highlightDolchVerbs;
    BackupVariable<bool> m_highlightDolchNouns;

    BackupVariable<bool> m_longSentencesNumberOfWords;
    BackupVariable<int> m_sentenceLength;
    BackupVariable<bool> m_longSentencesOutliers;

    BackupVariable<int> m_minDocWordCountForBatch;
    BackupVariable<int> m_filePathTruncationMode;

    BackupVariable<int> m_syllabicationMethod;
    BackupVariable<int> m_paragraphParsingMethod;
    BackupVariable<bool> m_ignoreBlankLinesForParagraphsParser;
    BackupVariable<bool> m_ignoreIndentingForParagraphsParser;
    BackupVariable<bool> m_sentenceStartMustBeUppercased;
    BackupVariable<bool> m_aggressiveExclusion;
    BackupVariable<bool> m_ignoreTrailingCopyrightNoticeParagraphs;
    BackupVariable<bool> m_ignoreTrailingCitations;
    BackupVariable<bool> m_ignoreFileAddresses;
    BackupVariable<bool> m_ignoreNumerals;
    BackupVariable<bool> m_ignoreProperNouns;
    BackupVariable<wxString> m_excludedPhrasesPath;
    bool m_excludedPhrasesEdited{ false };
    BackupVariable<bool> m_includeExcludedPhraseFirstOccurrence;
    BackupVariable<std::vector<std::pair<wchar_t, wchar_t>>> m_exclusionBlockTags;
    int m_exclusionBlockTagsOption{ 0 };
    BackupVariable<int> m_includeIncompleteSentencesIfLongerThan;
    BackupVariable<int> m_textExclusionMethod;

    // graph options
    BackupVariable<wxColour> m_xAxisFontColor;
    BackupVariable<wxFont> m_xAxisFont;
    BackupVariable<wxColour> m_yAxisFontColor;
    BackupVariable<wxFont> m_yAxisFont;
    BackupVariable<wxColour> m_topTitleFontColor;
    BackupVariable<wxFont> m_topTitleFont;
    BackupVariable<wxColour> m_bottomTitleFontColor;
    BackupVariable<wxFont> m_bottomTitleFont;
    BackupVariable<wxColour> m_leftTitleFontColor;
    BackupVariable<wxFont> m_leftTitleFont;
    BackupVariable<wxColour> m_rightTitleFontColor;
    BackupVariable<wxFont> m_rightTitleFont;

    wxDECLARE_CLASS(ToolsOptionsDlg);
    wxDECLARE_EVENT_TABLE();
    };

    /** @}*/

#endif //__TOOLS_OPTIONS_DIALOG_H__
