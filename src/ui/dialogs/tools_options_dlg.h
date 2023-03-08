#ifndef __TOOLS_OPTIONS_DIALOG_H__
#define __TOOLS_OPTIONS_DIALOG_H__

#include <limits>
#include <map>
#include <wx/valgen.h>
#include <wx/fontdlg.h>
#include <wx/treebook.h>
#include <wx/statline.h>
#include <wx/spinctrl.h>
#include <wx/tooltip.h>
#include <wx/aui/auibook.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/manager.h>
#include <wx/propgrid/advprops.h>
#include "../../readability/readability.h"
#include "../../../../SRC/Wisteria-Dataviz/src/base/image.h"
#include "../../../../SRC/Wisteria-Dataviz/src/util/warningmanager.h"
#include "../../../../SRC/Wisteria-Dataviz/src/util/backupvariable.h"
#include "../../../../SRC/Wisteria-Dataviz/src/ui/controls/sidebarbook.h"
#include "../../projects/standard_project_view.h"
#include "../../projects/batch_project_view.h"

class BaseProjectDoc;
class BatchProjectDoc;
class ProjectDoc;

class ToolsOptionsDlg final : public wxDialog
    {
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
        Unused1 = 0x2000,
        Unused2 = 0x4000,
        AllSections = (ProjectSection|GraphsSection|ScoresSection|TextSection|DocumentIndexing|Grammar|Statistics|WordsBreakdown|SentencesBreakdown)
        };
    /// Constructors
    explicit ToolsOptionsDlg(wxWindow* parent, BaseProjectDoc* project = nullptr, const ToolSections sectionsToInclude = AllSections);
    ~ToolsOptionsDlg()
        { wxDELETE(m_fileData); }
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& caption = _("Options"),
               const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
               long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);
    [[nodiscard]]
    ListCtrlEx* GetFileList() noexcept
        { return m_fileList; }
    ///Creates the controls and sizers
    void CreateControls();
    bool ValidateOptions();
    void SaveOptions();
    void SaveProjectGraphOptions();
    void SaveTextWindowOptions();
    void SaveStatisticsOptions();
    //button events
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
    //dolch color buttons
    void OnDolchConjunctionsHighlightColorSelect([[maybe_unused]] wxCommandEvent& event);
    void OnDolchPrepositionsHighlightColorSelect([[maybe_unused]] wxCommandEvent& event);
    void OnDolchPronounsHighlightColorSelect([[maybe_unused]] wxCommandEvent& event);
    void OnDolchAdverbsHighlightColorSelect([[maybe_unused]] wxCommandEvent& event);
    void OnDolchHighlightColorSelect(wxCommandEvent& event);
    void OnDolchNounHighlightColorSelect([[maybe_unused]] wxCommandEvent& event);
    //general events
    void OnWarningMessagesButtonClick([[maybe_unused]] wxCommandEvent& event);
    void OnHelp([[maybe_unused]] wxCommandEvent& event);
    void OnContextHelp([[maybe_unused]] wxHelpEvent& event);
    void OnOK([[maybe_unused]] wxCommandEvent& event);
    [[nodiscard]] bool HaveOptionsChanged() const;
    [[nodiscard]] bool HaveDocumentOptionsChanged() const;
    [[nodiscard]] bool HaveGraphOptionsChanged() const;
    [[nodiscard]] bool HaveTextViewOptionsChanged() const noexcept;
    [[nodiscard]] bool HaveTestDisplayOptionsChanged() const;
    [[nodiscard]] bool HaveStatisticsOptionsChanged() const;
    [[nodiscard]] bool HaveWordsBreakdownOptionsChanged() const;
    [[nodiscard]] bool HaveSentencesBreakdownOptionsChanged() const;

    void SelectPage(const int pageId);

    [[nodiscard]] bool IsGeneralSettings() const noexcept;
    [[nodiscard]] bool IsStandardProjectSettings() const;
    [[nodiscard]] bool IsBatchProjectSettings() const;

    [[nodiscard]] static wxString GetReadabilityScoresLabel()
        { return _("Readability Scores"); }
    [[nodiscard]] static wxString GetGeneralSettingsLabel()
        { return _("General Settings"); }
    [[nodiscard]] static wxString GetStatisticsLabel()
        { return _("Statistics"); }
    [[nodiscard]] static wxString GetAnalysisLabel()
        { return _("Analysis"); }
    [[nodiscard]] static wxString GetDolchSightWordsLabel()
        { return _("Dolch Sight Words"); }
    [[nodiscard]] static wxString GetDocumentViewsLabel()
        { return _("Document Views"); }
    [[nodiscard]] static wxString GetDifficultWordListsLabel()
        { return _("Difficult word lists"); }
    [[nodiscard]] static wxString GetGraphsLabel()
        { return ProjectView::GetGraphsLabel(); }
    [[nodiscard]] static wxString GetGeneralLabel()
        { return _("General"); }
    [[nodiscard]] static wxString GetAxisSettingsLabel()
        { return _("Axes"); }
    [[nodiscard]] static wxString GetTitlesLabel()
        { return _("Titles"); }
    [[nodiscard]] static wxString GetReadabilityGraphLabel()
        { return _("Readability Graphs"); }
    [[nodiscard]] static wxString GetBarChartLabel()
        { return _("Bar Charts"); }
    [[nodiscard]] static wxString GetBoxPlotLabel()
        { return _("Box Plots"); }
    [[nodiscard]] static wxString GetHistogramsLabel()
        { return _("Histograms"); }
    [[nodiscard]] static wxString GetProjectSettingsLabel()
        { return _("Project Settings"); }
    [[nodiscard]] static wxString GetSentencesWordsLabel()
        { return _("Sentences && Words"); }
    [[nodiscard]] static wxString GetTestOptionsLabel()
        { return _("Test Options"); }
    [[nodiscard]] static wxString GetScoreDisplayLabel()
        { return _("Scores"); }

    void OnAddFilesClick([[maybe_unused]] wxCommandEvent& event);
    void OnAddFileClick([[maybe_unused]] wxCommandEvent& event);
    void OnDeleteFileClick([[maybe_unused]] wxCommandEvent& event);
    void OnNumberSyllabizeChange([[maybe_unused]] wxCommandEvent& event);
    void OnExclusionBlockTagChange([[maybe_unused]] wxCommandEvent& event);
    const static int GENERAL_SETTINGS_PAGE = 1000;
    const static int PROJECT_SETTINGS_PAGE = 2000;

    const static int DOCUMENT_DISPLAY_GENERAL_PAGE = 3000;
    const static int DOCUMENT_DISPLAY_DOLCH_PAGE = 3002;

    const static int SCORES_TEST_OPTIONS_PAGE = 4000;
    const static int SCORES_DISPLAY_PAGE = 4001;

    const static int ANALYSIS_INDEXING_PAGE = 5000;
    const static int GRAMMAR_PAGE = 5001;
    const static int ANALYSIS_STATISTICS_PAGE = 5002;
    const static int WORDS_BREAKDOWN_PAGE = 5003;
    const static int SENTENCES_BREAKDOWN_PAGE = 5004;

    const static int GRAPH_GENERAL_PAGE = 6000;
    const static int GRAPH_AXIS_PAGE = 6001;
    const static int GRAPH_TITLES_PAGE = 6002;
    const static int GRAPH_READABILITY_GRAPHS_PAGE = 6003;
    const static int GRAPH_BAR_CHART_PAGE = 6004;
    const static int GRAPH_HISTOGRAM_PAGE = 6005;
    const static int GRAPH_BOX_PLOT_PAGE = 6006;
private:
    const static int ID_HIGHLIGHT_COLOR_BUTTON = 1001;
    const static int ID_EXCLUDED_HIGHLIGHT_COLOR_BUTTON = 1002;
    const static int ID_FONT_BUTTON = 1003;
    const static int ID_DOCUMENT_STORAGE_RADIO_BOX = 1004;
    const static int ID_FILE_BROWSE_BUTTON = 1006;
    const static int ID_X_AXIS_FONT_BUTTON = 1011;
    const static int ID_Y_AXIS_FONT_BUTTON = 1012;
    const static int ID_GRAPH_TOP_TITLE_FONT_BUTTON = 1013;
    const static int ID_GRAPH_BOTTOM_TITLE_FONT_BUTTON = 1014;
    const static int ID_GRAPH_LEFT_TITLE_FONT_BUTTON = 1015;
    const static int ID_GRAPH_RIGHT_TITLE_FONT_BUTTON = 1016;
    const static int ID_PARAGRAPH_PARSE = 1019;
    const static int ID_DUP_WORD_COLOR_BUTTON = 1023;
    const static int ID_WORDY_PHRASE_COLOR_BUTTON = 1024;
    const static int ID_DOLCH_CONJUNCTIONS_COLOR_BUTTON = 1025;
    const static int ID_DOLCH_PREPOSITIONS_COLOR_BUTTON = 1026;
    const static int ID_DOLCH_PRONOUNS_COLOR_BUTTON = 1027;
    const static int ID_DOLCH_ADVERBS_COLOR_BUTTON = 1028;
    const static int ID_DOLCH_ADJECTIVES_COLOR_BUTTON = 1029;
    const static int ID_DOLCH_NOUN_COLOR_BUTTON = 1030;
    const static int ID_EXPORT_SETTINGS_BUTTON = 1031;
    const static int ID_LOAD_SETTINGS_BUTTON = 1032;
    const static int ID_RESET_SETTINGS_BUTTON = 1033;
    const static int ID_TEXT_EXCLUDE_METHOD = 1036;
    const static int ID_EXCLUDE_INCOMPLETE_SENTENCES_RADIO_BUTTON = 1039;
    const static int ID_SENTENCE_LONGER_THAN_BUTTON = 1040;
    const static int ID_SENTENCE_OUTLIER_LENGTH_BUTTON = 1041;
    const static int ID_IGNORE_BLANK_LINES_BUTTON = 1042;
    const static int ID_IGNORE_INDENTING_BUTTON = 1043;
    const static int ID_WARNING_MESSAGES_BUTTON = 1044;
    const static int ID_ADD_FILE_BUTTON = 1045;
    const static int ID_DELETE_FILE_BUTTON = 1046;
    const static int ID_NUMBER_SYLLABIZE_METHOD = 1048;
    const static int ID_DOLCH_VERBS_COLOR_BUTTON = 1050;
    const static int ID_AGGRESSIVE_LIST_DEDUCTION_CHECKBOX = 1051;
    const static int ID_EXCLUDED_PHRASES_FILE_EDIT_BUTTON = 1052;
    const static int ID_EXCLUDE_COPYRIGHT_CHECKBOX = 1054;
    const static int ID_EXCLUDE_CITATIONS_CHECKBOX = 1055;
    const static int ID_EXCLUDE_FILE_ADDRESS_CHECKBOX = 1056;
    const static int ID_EXCLUDE_NUMERALS_CHECKBOX = 1057;
    const static int ID_EXCLUDE_PROPER_NOUNS_CHECKBOX = 1058;
    const static int ID_INCOMPLETE_SENTENCE_VALID_VALUE_BOX = 1059;
    const static int ID_APPLY_GRAPH_BACKGROUND_COLOR_FADE_CHECK_BOX = 1060;
    const static int ID_GRAPH_BACKGROUND_OPACITY_SLIDER_LABEL = 1061;
    const static int ID_GRAPH_BACKGROUND_OPACITY_SLIDER_PLUS_LABEL = 1062;
    const static int ID_PLOT_COLOR_LABEL = 1063;
    const static int ID_WATERMARK_LABEL = 1065;
    const static int ID_WATERMARK_TEXT_BOX = 1066;
    const static int ID_GRAPH_LOGO_LABEL = 1067;
    const static int ID_BAR_CHART_EFFECTS_OPTIONS = 1068;
    const static int ID_BAR_CHART_HORIZONTAL_OPTION = 1069;
    const static int ID_BAR_CHART_VERTICAL_OPTION = 1070;
    const static int ID_DOCUMENT_FONT_LABEL = 1071;
    const static int ID_GRAPH_OPTIONS_PROPERTYGRID = 1073;
    const static int ID_BARCHART_OPTIONS_PROPERTYGRID = 1074;
    const static int ID_INCLUDE_FIRST_OCCURRENCE_EXCLUDE_CHECKBOX = 1075;
    const static int ID_INCOMPLETE_SENTENCE_VALID_LABEL_START = 1076;
    const static int ID_INCOMPLETE_SENTENCE_VALID_LABEL_END = 1077;
    const static int ID_EXCLUSION_TAG_BLOCK_LABEL = 1078;
    const static int ID_EXCLUSION_TAG_BLOCK_SELCTION = 1079;
    const static int ID_ADDITIONAL_FILE_BROWSE_BUTTON = 1080;
    const static int ID_ADD_FILES_BUTTON = 1081;
    const static int ID_THEME_COMBO = 1082;
    const static int ID_ADDITIONAL_FILE_FIELD = 1083;

    [[nodiscard]] wxString GetCustomTestsLabel() const
        { return _("Custom Tests"); }
    [[nodiscard]] wxString GetInvalidRegionsColorLabel() const
        { return _("Invalid regions color"); }
    [[nodiscard]] wxString GetFleshChartConnectPointsLabel() const
        { return _("Connect points"); }
    [[nodiscard]] wxString GetUseEnglishLabelsForGermanLixLabel() const
        { return _("Use English translations for German Lix gauge"); }
    [[nodiscard]] wxString GetColorLabel() const
        { return _("Color"); }
    [[nodiscard]] wxString GetBackgroundColorLabel() const
        { return _DT(L"BGCOLOR"); }
    [[nodiscard]] wxString GetEffectLabel() const
        { return _("Effect"); }
    [[nodiscard]] wxString GetOpacityLabel() const
        { return _("Opacity"); }
    [[nodiscard]] wxString GetImageOpacityLabel() const
        { return _("Image opacity"); }
    [[nodiscard]] wxString GetLabelsOnBarsLabel() const
        { return _("Display labels on bars"); }
    [[nodiscard]] wxString GetLabelsOnBoxesLabel() const
        { return _("Display box & whisker labels"); }
    [[nodiscard]] wxString GetConnectBoxesLabel() const
        { return _("Connect middle points"); }
    [[nodiscard]] wxString GetShowAllDataPointsLabel() const
        { return _("Show all data points"); }
    [[nodiscard]] wxString GeOrientationLabel() const
        { return _("Orientation"); }
    [[nodiscard]] wxString GetBinSortingLabel() const
        { return _("Sorting"); }
    [[nodiscard]] wxString GetGradeLevelRoundingLabel() const
        { return _("Grade level/index value rounding"); }
    [[nodiscard]] wxString GetIntervalDisplayLabel() const
        { return _("Interval display"); }
    [[nodiscard]] wxString GetBinLabelsLabel() const
        { return _("Labels"); }
    [[nodiscard]] wxString GetDisplayDropShadowsLabel() const
        { return _("Display drop shadows"); }
    [[nodiscard]] wxString GetCustomImageBrushLabel() const
        { return _("Custom image brush"); }
    [[nodiscard]] wxString GetWatermarkLabel() const
        { return _("Watermark"); }
    [[nodiscard]] wxString GetLogoImageLabel() const
        { return _("Logo image"); }
    [[nodiscard]] wxString GetImageLabel() const
        { return _("Image"); }
    [[nodiscard]] wxString GetApplyFadeLabel() const
        { return _("Apply color fade"); }
    [[nodiscard]] wxString GetBackgroundLabel() const
        { return _("Background"); }
    [[nodiscard]] wxString GetIgnoreProperNounsLabel() const
        { return _("Ignore proper nouns"); }
    [[nodiscard]] wxString GetIgnoreUppercasedWordsLabel() const
        { return _("Ignore UPPERCASED words"); }
    [[nodiscard]] wxString GetIgnoreNumeralsLabel() const
        { return _("Ignore numerals"); }
    [[nodiscard]] wxString GetIgnoreFileAddressesLabel() const
        { return _("Ignore Internet and file addresses"); }
    [[nodiscard]] wxString GetIgnoreProgrammerCodeLabel() const
        { return _("Ignore programmer code"); }
    [[nodiscard]] wxString GetAllowColloquialismsLabel() const
        { return _("Allow colloquialisms"); }
    [[nodiscard]] wxString GetIgnoreSocialMediaLabel() const
        { return _("Ignore social media hashtags"); }
    [[nodiscard]] wxString GetResultsLabel() const
        { return _("Results"); }
    [[nodiscard]] wxString GetGrammarHighlightedReportLabel() const
        { return _("Highlighted Report"); }
    [[nodiscard]] wxString GetSpellCheckerLabel() const
        { return _("Spell Checker"); }
    [[nodiscard]] wxString GetDolchStatisticsReportLabel() const
        { return _("Dolch Report"); }
    [[nodiscard]] wxString GetCoverageLabel() const
        { return _("Coverage"); }
    [[nodiscard]] wxString GetDolchWordsLabel() const
        { return _DT(L"DOLCHWORDS"); }
    [[nodiscard]] wxString GetDolchExplanationLabel() const
        { return _("Explanation"); }
    [[nodiscard]] wxString GetStatisticsReportLabel() const
        { return _("Summary Report"); }
    [[nodiscard]] wxString GetParagraphsLabel() const
        { return _("Paragraphs"); }
    [[nodiscard]] wxString GetSentencesLabel() const
        { return _("Sentences"); }
    [[nodiscard]] wxString GetWordsLabel() const
        { return _("Words"); }
    [[nodiscard]] wxString GetExtendedWordsLabel() const
        { return _("Extended Words"); }
    [[nodiscard]] wxString GetGrammarLabel() const
        { return _("Grammar"); }
    [[nodiscard]] wxString GetNotesLabel() const
        { return _("Notes"); }
    [[nodiscard]] wxString GetExtendedInformationLabel() const
        { return _("Extended Information"); }
    [[nodiscard]] wxString GetFryLikeLabel() const
        { return _("Fry/GPM/Raygor/Schwartz"); }
    [[nodiscard]] wxString GetFleschChartLabel() const
        { return _("Flesch Chart"); }
    [[nodiscard]] wxString GetLixGaugeLabel() const
        { return _("Lix Gauge"); }
    [[nodiscard]] wxString GetBarAppearanceLabel() const
        { return _("Bar Appearance"); }
    [[nodiscard]] wxString GetBinningOptionsLabel() const
        { return _("Binning Options"); }
    [[nodiscard]] wxString GetBinDisplayLabel() const
        { return _("Bin Display"); }
    [[nodiscard]] wxString GetBoxAppearanceLabel() const
        { return _("Box Appearance"); }
    [[nodiscard]] wxString GetBoxOptionsLabel() const
        { return _("Box Options"); }
    [[nodiscard]] wxString GetGraphBackgroundLabel() const
        { return _("Background"); }
    [[nodiscard]] wxString GetPlotAreaBackgroundLabel() const
        { return _("Plot Area Background"); }
    [[nodiscard]] wxString GetWatermarksLogosLabel() const
        { return _("Watermarks & Logos"); }
    [[nodiscard]] wxString GetEffectsLabel() const
        { return _("Effects"); }
    [[nodiscard]] wxString GetGradeScaleLabel() const
        { return _("Grade scale"); }
    [[nodiscard]] wxString GetGradeLabel() const
        { return _("Grade Display"); }
    [[nodiscard]] wxString GetGradesLongFormatLabel() const
        { return _("Display scores in long format"); }
    [[nodiscard]] wxString GetReadingAgeLabel() const
        { return _("Reading Age Display"); }
    [[nodiscard]] wxString GetScoreResultsLabel() const
        { return _("Results"); }
    [[nodiscard]] wxString GetIncludeScoreSummaryLabel() const
        { return _("Include test-summary report"); }
    [[nodiscard]] wxString GetCalculationLabel() const
        { return _("Calculation"); }
    [[nodiscard]] wxString GetCountIndependentClausesLabel() const
        { return _("Count independent clauses"); }
    [[nodiscard]] wxString GetNumeralSyllabicationLabel() const
        { return _("Numeral syllabication"); }
    [[nodiscard]] wxString GetFleschNumeralSyllabicationLabel() const
        { return _DT(L"FLESCHNUMSYL"); }
    [[nodiscard]] wxString GetFleschKincaidNumeralSyllabicationLabel() const
        { return _DT(L"FLESCHKINCAIDNUMSYL"); }
    [[nodiscard]] wxString GetIncludeStockerLabel() const
        { return _("Include Catholic Supplement"); }
    [[nodiscard]] wxString GetProperNounsLabel() const
        { return _("Proper nouns"); }
    [[nodiscard]] wxString GetTextExclusionLabel() const
        { return _("Text exclusion"); }
    [[nodiscard]] wxString GetDCTextExclusionLabel() const
        { return _DT(L"DCTEXTEXCLUSION"); }
    [[nodiscard]] wxString GetHJCTextExclusionLabel() const
        { return _DT(L"HJTEXTEXCLUSION"); }

    /// Replaces various filepath shortcuts to the full pathway
    [[nodiscard]] wxString ExpandPath(wxString path) const;

    [[nodiscard]] ToolSections GetSectionsBeingShown() const noexcept
        { return m_sectionsBeingShown; }

    static bool IsPropertyAvailable(const wxPropertyGridPage* propGrid, const wxString& propertyName)
        { return (propGrid && propGrid->GetProperty(propertyName)); }

    void CreateLabelHeader(wxWindow* parent, wxSizer* parentSizer, const wxString& title, const bool addSidePadding = false) const;

    BaseProjectDoc* m_readabilityProjectDoc { nullptr };
    wxCheckBox* m_ignoreCopyrightsCheckBox { nullptr };
    wxCheckBox* m_ignoreCitationsCheckBox { nullptr };
    wxCheckBox* m_ignoreFileAddressesCheckBox { nullptr };
    wxCheckBox* m_ignoreNumeralsCheckBox { nullptr };
    wxCheckBox* m_ignoreProperNounsCheckBox { nullptr };
    wxCheckBox* m_includeExcludedPhraseFirstOccurrenceCheckBox { nullptr };
    wxTextCtrl* m_excludedPhrasesPathFilePathEdit { nullptr };
    wxBitmapButton* m_excludedPhrasesEditBrowseButton { nullptr };
    wxCheckBox* m_aggressiveExclusionCheckBox { nullptr };
    wxCheckBox* m_ignoreBlankLinesCheckBox { nullptr };
    wxCheckBox* m_ignoreIndentingCheckBox { nullptr };
    wxButton* m_highlightColorButton { nullptr };
    wxButton* m_excludedHighlightColorButton { nullptr };
    wxButton* m_duplicateWordHighlightColorButton { nullptr };
    wxButton* m_wordyPhraseHighlightColorButton { nullptr };
    wxButton* m_FontButton { nullptr };
    wxButton* m_xAxisFontButton { nullptr };
    wxButton* m_yAxisFontButton { nullptr };
    wxButton* m_graphTopTitleFontButton { nullptr };
    wxButton* m_graphBottomTitleFontButton { nullptr };
    wxButton* m_graphLeftTitleFontButton { nullptr };
    wxButton* m_graphRightTitleFontButton { nullptr };
    Wisteria::UI::SideBarBook* m_sideBar { nullptr };
    wxTextCtrl* m_stippleFilePathEdit { nullptr };
    wxTextCtrl* m_filePathEdit { nullptr };
    wxStaticText* m_includeIncompleteSentSizeincludeIncompleteLabel { nullptr };
    wxStaticText* m_includeIncompleteSentSizeWordsLabel { nullptr };
    wxBitmapButton* m_fileBrowseButton { nullptr };
    wxStaticText* m_syllableLabel { nullptr };
    wxChoice* m_syllableCombo { nullptr };
    wxChoice* m_exclusionBlockTagsCombo { nullptr };
    wxStaticText* m_exclusionBlockTagsLabel { nullptr };
    //dolch buttons
    wxBitmapButton* m_DolchConjunctionsColorButton { nullptr };
    wxBitmapButton* m_DolchPrepositionsColorButton { nullptr };
    wxBitmapButton* m_DolchPronounsColorButton { nullptr };
    wxBitmapButton* m_DolchAdverbsColorButton { nullptr };
    wxBitmapButton* m_DolchAdjectivesColorButton { nullptr };
    wxBitmapButton* m_DolchVerbsColorButton { nullptr };
    wxBitmapButton* m_DolchNounColorButton { nullptr };

    //property grid controls
    wxPropertyGridPage* m_readabilityTestsPropertyGrid { nullptr };
    wxPropertyGridPage* m_gradeLevelPropertyGrid { nullptr };
    wxPropertyGridPage* m_grammarPropertyGrid { nullptr };
    wxPropertyGridPage* m_wordsBreakdownPropertyGrid { nullptr };
    wxPropertyGridPage* m_sentencesBreakdownPropertyGrid { nullptr };
    wxPropertyGridPage* m_statisticsPropertyGrid { nullptr };
    wxPropertyGridPage* m_generalGraphPropertyGrid { nullptr };
    wxPropertyGridPage* m_readabilityGraphPropertyGrid { nullptr };
    wxPropertyGridPage* m_barChartPropertyGrid { nullptr };
    wxPropertyGridPage* m_histogramPropertyGrid { nullptr };
    wxPropertyGridPage* m_boxPlotsPropertyGrid { nullptr };

    wxStaticText* m_readTestsSyllableLabel { nullptr };
    wxStaticText* m_textExclusionLabel { nullptr };

    wxBitmapButton* m_addFilesButton { nullptr };
    wxBitmapButton* m_addFileButton { nullptr };
    wxBitmapButton* m_deleteFileButton { nullptr };
    ListCtrlEx* m_fileList { nullptr };
    ListCtrlExDataProvider* m_fileData { nullptr };

    //project language
    BackupVariable<int> m_projectLanguage;
    BackupVariable<wxString> m_reviewer;
    BackupVariable<wxString> m_status;
    BackupVariable<wxString> m_description;
    BackupVariable<wxString> m_appendedDocumentFilePath;

    //which options are we showing
    ToolSections m_sectionsBeingShown;

    //document storage/linking information
    BackupVariable<int> m_documentStorageMethod;
    BackupVariable<wxString> m_filePath;

    BackupVariable<int> m_textHighlightMethod;
    BackupVariable<wxColour> m_highlightedColor;
    BackupVariable<wxColour> m_excludedTextHighlightColor;
    BackupVariable<wxColour> m_duplicateWordHighlightColor;
    BackupVariable<wxColour> m_wordyPhraseHighlightColor;
    BackupVariable<wxFont> m_font;
    BackupVariable<wxColour> m_fontColor;

    //dolch highlighting
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
    BackupVariable<bool> m_includeExcludedPhraseFirstOccurrence;
    bool m_excludedPhrasesEdited { false };
    BackupVariable<std::vector<std::pair<wchar_t,wchar_t>>> m_exclusionBlockTags;
    int m_exclusionBlockTagsOption { 0 };
    BackupVariable<int> m_includeIncompleteSentencesIfLongerThan;
    BackupVariable<int> m_textExclusionMethod;

    //graph options
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

#endif //__TOOLS_OPTIONS_DIALOG_H__
