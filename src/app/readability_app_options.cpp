#include "readability_app_options.h"
#include "../Wisteria-Dataviz/src/base/colorbrewer.h"
#include "../Wisteria-Dataviz/src/import/html_encode.h"
#include "../Wisteria-Dataviz/src/math/mathematics.h"
#include "../ui/dialogs/custom_test_dlg.h"
#include "readability_app.h"

using namespace Wisteria;
using namespace Wisteria::Graphs;
using namespace Wisteria::UI;

wxDECLARE_APP(ReadabilityApp);

ReadabilityAppOptions::ReadabilityAppOptions()
    : m_textHighlight(TextHighlight::HighlightBackground),
      m_fontColor(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT))
    {
    SetFonts();
    SetColorsFromSystem();
    BaseProject::InitializeStandardReadabilityTests();
    BaseProject::ResetStandardReadabilityTests(m_readabilityTests);
    // set the warnings system
    WarningManager::AddWarning(WarningMessage(
        _DT(L"project-open-as-read-only"), _(L"Project file will be opened as read only."),
        wxString{}, _(L"Warn about projects being opened as read-only."), wxOK | wxICON_INFORMATION,
        false));
    WarningManager::AddWarning(WarningMessage(
        _DT(L"remove-test-from-project"), wxString{}, wxString{},
        _(L"Prompt when removing a test from a project."), wxICON_INFORMATION, false));
    WarningManager::AddWarning(WarningMessage(
        _DT(L"delete-document-from-batch"), wxString{}, wxString{},
        _(L"Prompt when removing a document from a batch project."), wxICON_INFORMATION, false));
    WarningManager::AddWarning(
        WarningMessage(_DT(L"document-no-words"),
                       _(L"The text that you are analyzing does not contain any valid words. "
                         "No calculations will be performed."),
                       _(L"Warning"), _(L"Warn about documents that do not contain any words."),
                       wxOK | wxICON_EXCLAMATION, false));
    WarningManager::AddWarning(
        WarningMessage(_DT(L"document-less-than-20-words"),
                       _(L"The text that you are analyzing is less than 20 words. "
                         "Most test results will not be meaningful with such a small sample."),
                       _(L"Warning"), _(L"Warn about documents containing less than 20 words."),
                       wxOK | wxICON_EXCLAMATION, false));
    WarningManager::AddWarning(WarningMessage(
        _DT(L"document-less-than-100-words"),
        _(L"The text that you are analyzing is less than 100 words. "
          "Factors, such as word and syllable counts, will be standardized for some tests."),
        _(L"Warning"), _(L"Warn about documents containing less than 100 words."),
        wxOK | wxICON_EXCLAMATION, false));
    WarningManager::AddWarning(
        WarningMessage(_DT(L"sentences-split-by-paragraph-breaks"), wxString{}, _(L"Warning"),
                       _(L"Warn about documents that contain sentences split by paragraph breaks."),
                       wxOK | wxICON_EXCLAMATION, false));
    WarningManager::AddWarning(
        WarningMessage(_DT(L"incomplete-sentences-valid-from-length"), wxString{}, _(L"Warning"),
                       _(L"Warn about documents that contain long incomplete sentences "
                         "that will be included in the analysis."),
                       wxOK | wxICON_EXCLAMATION, false));
    WarningManager::AddWarning(WarningMessage(
        _DT(L"high-count-sentences-being-ignored"),
        _(L"This document contains a large percentage of incomplete sentences that you "
          "have requested to ignore.\n\nDo you wish to change this option and "
          "include these items in the analysis?"),
        _(L"Warning"),
        _(L"Prompt if a document should switch to include sentences in the analysis."),
        wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, false));
    WarningManager::AddWarning(WarningMessage(
        _DT(L"ndc-proper-noun-conflict"),
        _(L"This test's proper-noun settings differ from the standard New Dale-Chall test.\n"
          "Do you wish to adjust this setting to match the standard test?"),
        _(L"Settings Conflict"),
        _(L"Prompt if a custom NDC test's proper noun settings "
          "differ from the standard NDC test."),
        wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, false));
    WarningManager::AddWarning(
        WarningMessage(_DT(L"new-dale-chall-text-exclusion-differs-note"), wxString{}, wxString{},
                       _(L"Prompt about New Dale-Chall using a different text "
                         "exclusion method from the system default."),
                       wxICON_INFORMATION, false));
    WarningManager::AddWarning(
        WarningMessage(_DT(L"harris-jacobson-text-exclusion-differs-note"), wxString{}, wxString{},
                       _(L"Prompt about Harris-Jacobson using a different text exclusion "
                         "method from the system default."),
                       wxICON_INFORMATION, false));
    WarningManager::AddWarning(WarningMessage(
        _DT(L"custom-test-numeral-settings-adjustment-required"),
        _(L"Harris-Jacobson requires numerals to be excluded from the overall word count.\n"
          "Numeral options for this test will be adjusted to take this into account."),
        _(L"Warning"), _(L"Warn when a custom test's numeral settings will be adjusted."),
        wxOK | wxICON_INFORMATION, false));
    WarningManager::AddWarning(WarningMessage(
        _DT(L"german-no-proper-noun-support"),
        _(L"Because German capitalizes all nouns, the program will be "
          "unable to detect proper nouns.\n"
          "Treatment of proper nouns as familiar words will be disabled for this test."),
        _(L"Warning"), _(L"Warn about German stemming not supporting proper noun detection."),
        wxOK | wxICON_INFORMATION, false));
    WarningManager::AddWarning(WarningMessage(
        _DT(L"histogram-unique-values-midpoints-required"),
        _(L"Note: sorting histogram bins by unique values requires midpoint interval display.\n"
          "Midpoint interval display will be enabled."),
        wxString{}, _(L"Warn about unique-value histograms requiring midpoint axis labels."),
        wxOK | wxICON_INFORMATION, true));
    WarningManager::AddWarning(WarningMessage(
        _DT(L"file-autosearch-from-project-directory"), wxString{}, _(L"File Not Found"),
        _(L"Prompt about auto-searching for missing files."), wxYES_NO | wxICON_QUESTION, false));
    WarningManager::AddWarning(WarningMessage(
        _DT(L"linked-document-is-embedded"),
        _(L"This document is embedded in the project.\n"
          "Do you wish to link to the original document instead?"),
        _(L"Link Document"), _(L"Prompt about re-linking to a document that has been embedded."),
        wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, false));
    WarningManager::AddWarning(WarningMessage(
        _DT(L"no-embedded-text"),
        _(L"No embedded text found in the project. Project will not be created."), _(L"Warning"),
        _(L"Prompt about failing to load a project that is missing its embedded text."),
        wxOK | wxICON_EXCLAMATION, false));
    WarningManager::AddWarning(
        WarningMessage(_DT(L"clear-type-turned-off"),
                       _(L"ClearType is currently turned off. Enabling this will make fonts appear "
                         "smoother and easier to read.\n\n"
                         "Do you wish to enable ClearType?"),
                       _(L"Warning"), _(L"Check if ClearType is turned on (Windows only)."),
                       wxYES_NO | wxICON_QUESTION, false));
    WarningManager::AddWarning(WarningMessage(
        _DT(L"note-export-from-save"),
        _(L"Any window can be exported by selecting \"Export\" from the \"Save\" button."),
        wxString{}, _(L"Prompt about how windows can be exported from the Save button."),
        wxICON_INFORMATION, true));
    WarningManager::AddWarning(WarningMessage(
        _DT(L"click-test-to-view"), _(L"Double click a test to view more information."), wxString{},
        _(L"Prompt about how double-clicking a test can show its help."), wxICON_INFORMATION,
        true));
    WarningManager::AddWarning(WarningMessage(
        _DT(L"bkimage-zoomin-noupscale"),
        _(L"When zooming, background images will not be stretched beyond their original sizes."),
        wxString{},
        _(L"Prompt about how background images will not be upscaled beyond their "
          "original size when zooming into a graph."),
        wxICON_INFORMATION, true));
    WarningManager::AddWarning(WarningMessage(
        _DT(L"note-project-properties"),
        _(L"Settings embedded in this project can be edited by "
          "clicking \"Home\" - \"Properties\"."),
        wxString{}, _(L"Prompt about how settings are embedded in projects and how to edit them."),
        wxICON_INFORMATION, true));
    WarningManager::AddWarning(WarningMessage(
        _DT(L"batch-goals"),
        _(L"Documents not passing the project's goals are shown in this window.\n"
          "The recommended min and max values for each goal are displayed,\n"
          "along with an icon indicating whether the document is passing these constraints."),
        wxString{}, _(L"Prompt about how the Goals window works in a batch project."),
        wxICON_INFORMATION, true));
    WarningManager::AddWarning(WarningMessage(
        _DT(L"set-app-exclusion-list-from-project"),
        _(L"Would you like to use this word exclusion list for all future projects?"),
        _(L"Set Global Word Exclusion List"),
        _(L"Prompt about whether to set the application's word exclusion list from a project."),
        wxYES_NO | wxICON_QUESTION, false));
    }

//------------------------------------------------
void ReadabilityAppOptions::SetFonts()
    {
    auto systemFont{ wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT) };
    systemFont.SetFaceName(Wisteria::GraphItems::Label::GetFirstAvailableWordProcessorFont());
    m_editorFont = systemFont.Larger().Larger();
    m_xAxisFont = systemFont;
    m_yAxisFont = systemFont;
    m_topTitleFont = systemFont;
    m_bottomTitleFont = systemFont;
    m_leftTitleFont = systemFont;
    m_rightTitleFont = systemFont;
    m_textViewFont = systemFont.Larger().Larger();
    // fix font issues in case the system is using a hidden font for its default (happens on macOS)
    Wisteria::GraphItems::Label::FixFont(m_editorFont);
    Wisteria::GraphItems::Label::FixFont(m_xAxisFont);
    Wisteria::GraphItems::Label::FixFont(m_yAxisFont);
    Wisteria::GraphItems::Label::FixFont(m_topTitleFont);
    Wisteria::GraphItems::Label::FixFont(m_bottomTitleFont);
    Wisteria::GraphItems::Label::FixFont(m_leftTitleFont);
    Wisteria::GraphItems::Label::FixFont(m_rightTitleFont);
    Wisteria::GraphItems::Label::FixFont(m_textViewFont);
    }

//------------------------------------------------
void ReadabilityAppOptions::SetColorsFromSystem()
    {
    m_themeName = _DT(L"System", DTExplanation::InternalKeyword);

    m_controlBackgroundColor = wxSystemSettings::GetColour(wxSystemColour::wxSYS_COLOUR_WINDOW);
    // Ribbon colors
    m_ribbonActiveTabColor = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
    m_ribbonInactiveTabColor = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
    // if active and inactive colors are close, then change the inactive color slightly
    if (std::abs(m_ribbonActiveTabColor.GetLuminance() - m_ribbonInactiveTabColor.GetLuminance()) <
        .05)
        {
        // if active is darker, then lighten inactive
        if (m_ribbonActiveTabColor.GetLuminance() < m_ribbonInactiveTabColor.GetLuminance())
            {
            m_ribbonInactiveTabColor = m_ribbonInactiveTabColor.ChangeLightness(105);
            }
        else
            {
            m_ribbonInactiveTabColor = m_ribbonInactiveTabColor.ChangeLightness(95);
            }
        }
    m_ribbonHoverColor = wxColour(253, 211, 155); // light orange color
    m_ribbonHoverFontColor = *wxBLACK;
    m_ribbonActiveFontColor = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
    m_ribbonInactiveFontColor = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);

    // Sidebar colors
    // if ugly Windows default gray, then override the system with prettier colors
    if (wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE) == wxColour(240, 240, 240))
        {
        m_sideBarBackgroundColor = wxColour(200, 211, 231); // Serenity
        m_sideBarParentColor = wxColour(180, 189, 207);     // slightly darker
        }
    else
        {
        m_sideBarBackgroundColor = m_ribbonInactiveTabColor;
        m_sideBarParentColor = m_ribbonActiveTabColor;
        }
    m_sideBarActiveColor = L"#FDB759"; // bright orange
    m_sideBarActiveFontColor = *wxBLACK;
    m_sideBarHoverColor = m_ribbonHoverColor;
    m_sideBarHoverFontColor = *wxBLACK;
    m_sideBarFontColor = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
    }

//------------------------------------------------
void ReadabilityAppOptions::ResetSettings()
    {
    SetFonts();

    // default exporting info
    BaseProjectDoc::SetExportTextViewExt(L"htm");
    BaseProjectDoc::SetExportListExt(L"htm");
    BaseProjectDoc::SetExportGraphExt(L"png");
    BaseProjectDoc::ExportHardWordLists(true);
    BaseProjectDoc::ExportSentencesBreakdown(true);
    BaseProjectDoc::ExportGraphs(true);
    BaseProjectDoc::ExportTestResults(true);
    BaseProjectDoc::ExportStatistics(true);
    BaseProjectDoc::ExportWordiness(true);
    BaseProjectDoc::ExportSightWords(true);
    BaseProjectDoc::ExportWarnings(true);
    BaseProjectDoc::ExportLists(true);
    BaseProjectDoc::ExportTextReports(true);

    m_appWindowMaximized = true;
    // theme settings
    SetColorsFromSystem();

    m_editorFontColor = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
    m_editorIndent = true;
    m_editorSpaceAfterNewlines = false;
    m_editorTextAlignment = wxTextAttrAlignment::wxTEXT_ALIGNMENT_JUSTIFIED;
    m_editorLineSpacing = wxTextAttrLineSpacing::wxTEXT_ATTR_LINE_SPACING_NORMAL;

    if (wxGetApp().GetLogFile() != nullptr)
        {
        wxGetApp().GetLogFile()->SetVerbose(false);
        }
    m_logAppendDailyLog = false;

    m_textHighlight = TextHighlight::HighlightBackground;
    m_dolchConjunctionsColor = wxColour(255, 255, 0);
    m_dolchPrepositionsColor = wxColour(0, 245, 255);
    m_dolchPronounsColor = wxColour(198, 226, 255);
    m_dolchAdverbsColor = wxColour(0, 250, 154);
    m_dolchAdjectivesColor = wxColour(221, 160, 221);
    m_dolchVerbColor = wxColour(254, 208, 112);
    m_dolchNounColor = wxColour(255, 182, 193);
    m_highlightDolchConjunctions = true;
    m_highlightDolchPrepositions = true;
    m_highlightDolchPronouns = true;
    m_highlightDolchAdverbs = true;
    m_highlightDolchAdjectives = true;
    m_highlightDolchVerbs = true;
    m_highlightDolchNouns = false;
    m_textHighlightColor = wxColour(152, 251, 152);
    m_excludedTextHighlightColor = wxColour(175, 175, 175);
    m_duplicateWordHighlightColor = wxColour(255, 128, 128);
    m_wordyPhraseHighlightColor = wxColour(0, 255, 255);
    m_fontColor = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
    m_textSource = TextSource::FromFile;
    m_batchGroupDefault = 2;
    m_longSentenceMethod = LongSentence::LongerThanSpecifiedLength;
    m_difficultSentenceLength = 22;
    m_numeralSyllabicationMethod = NumeralSyllabize::WholeWordIsOneSyllable;
    m_includeExcludedPhraseFirstOccurrence = false;
    m_paragraphsParsingMethod = ParagraphParse::OnlySentenceTerminatedNewLinesAreParagraphs;
    m_ignoreBlankLinesForParagraphsParser = false;
    m_ignoreIndentingForParagraphsParser = false;
    m_sentenceStartMustBeUppercased = false;
    m_aggressiveExclusion = false;
    m_ignoreTrailingCopyrightNoticeParagraphs = true;
    m_ignoreTrailingCitations = true;
    m_ignoreFileAddresses = false;
    m_ignoreNumerals = false;
    m_ignoreProperNouns = false;
    m_excludedPhrasesPath.clear();
    m_exclusionBlockTags.clear();
    m_invalidSentenceMethod = InvalidSentence::ExcludeFromAnalysis;
    m_includeIncompleteSentencesIfLongerThan = 15;
    m_spellcheck_ignore_proper_nouns = false;
    m_spellcheck_ignore_uppercased = true;
    m_spellcheck_ignore_numerals = true;
    m_spellcheck_ignore_file_addresses = true;
    m_spellcheck_ignore_programmer_code = false;
    m_allow_colloquialisms = true;
    for (auto rTest = GetReadabilityTests().get_tests().begin();
         rTest != GetReadabilityTests().get_tests().end(); ++rTest)
        {
        rTest->include(false);
        }
    m_includeDolchSightWords = false;
    m_testRecommendation = TestRecommendation::BasedOnDocumentType;
    m_testsByIndustry = readability::industry_classification::adult_publishing_industry;
    m_testsByDocumentType = readability::document_classification::adult_literature_document;
    GetReadabilityMessageCatalog().SetGradeScale(readability::grade_scale::k12_plus_united_states);
    GetReadabilityMessageCatalog().SetLongGradeScaleFormat(false);
    GetReadabilityMessageCatalog().SetReadingAgeDisplay(
        ReadabilityMessages::ReadingAgeDisplay::ReadingAgeAsARange);
    // document linking information
    m_documentStorageMethod = TextStorage::NoEmbedText;
    // theme
    m_themeName = _DT(L"System");
    // internet
    m_userAgent = _DT(L"Mozilla/5.0 (") + wxGetOsDescription() + _DT(L") WebKit/12.0 WebLion");
    wxGetApp().GetWebHarvester().SetUserAgent(m_userAgent);
    m_disablePeerVerify = false;
    m_useJsCookies = false;
    m_persistJsCookies = false;
    wxGetApp().GetWebHarvester().DisablePeerVerify(m_disablePeerVerify);
    wxGetApp().GetWebHarvester().UseJavaScriptCookies(m_useJsCookies);
    wxGetApp().GetWebHarvester().PersistJavaScriptCookies(m_persistJsCookies);
    // graph information
    m_boxPlotShowAllPoints = false;
    m_boxDisplayLabels = false;
    m_boxConnectMiddlePoints = true;
    m_barDisplayLabels = true;
    m_useGraphBackGroundColorLinearGradient = false;
    m_displayDropShadows = false;
    m_showcaseComplexWords = false;
    m_plotBackGroundImagePath.clear();
    m_graphColorSchemeName = _DT(L"campfire");
    m_watermark.clear();
    m_watermarkImg.clear();
    m_graphBackGroundColor = wxColour(255, 255, 255);
    m_plotBackGroundColor = wxColour(255, 255, 255);
    m_plotBackGroundImageOpacity = wxALPHA_OPAQUE;
    m_plotBackGroundColorOpacity = wxALPHA_TRANSPARENT;
    m_xAxisFontColor = wxColour(0, 0, 0);
    m_yAxisFontColor = wxColour(0, 0, 0);
    m_topTitleFontColor = wxColour(0, 0, 0);
    m_bottomTitleFontColor = wxColour(0, 0, 0);
    m_leftTitleFontColor = wxColour(0, 0, 0);
    m_rightTitleFontColor = wxColour(0, 0, 0);
    // honeydew
    m_graphInvalidAreaColor = wxColour(193, 205, 193);
    m_fleschChartConnectPoints = true;
    m_fleschChartSyllableRulerDocGroups = false;
    m_useEnglishLabelsGermanLix = false;
    m_histogramBinningMethod = Histogram::BinningMethod::BinByIntegerRange;
    m_histrogramBinLabelDisplayMethod = BinLabelDisplay::BinValue;
    m_histrogramRoundingMethod = RoundingMethod::RoundDown;
    m_histrogramIntervalDisplay = Histogram::IntervalDisplay::Cutpoints;
    // lavender
    m_histogramBarColor = wxColour(182, 164, 204);
    // rain color
    m_barChartBarColor = wxColour(107, 183, 196);
    m_histogramBarOpacity = wxALPHA_OPAQUE;
    m_histogramBarEffect = BoxEffect::Glassy;
    m_barChartOrientation = Wisteria::Orientation::Horizontal;
    m_graphBarOpacity = wxALPHA_OPAQUE;
    m_graphBarEffect = BoxEffect::Glassy;
    m_stippleShape = DONTTRANSLATE(L"book");
    m_stippleColor = wxColour{ L"#6082B6" };
    m_stippleImagePath.clear();
    m_commonImagePath.clear();
    m_graphBoxColor = wxColour(0, 128, 64);
    m_graphBoxOpacity = wxALPHA_OPAQUE;
    m_graphBoxEffect = BoxEffect::Glassy;
    m_plotBackgroundImageEffect = Wisteria::ImageEffect::NoEffect;
    m_plotBackgroundImageFit = Wisteria::ImageFit::Shrink;
    m_varianceMethod = VarianceMethod::PopulationVariance;
    GetStatisticsReportInfo().Reset();
    GetStatisticsInfo().Reset();
    GetGrammarInfo().EnableAll();
    GetWordsBreakdownInfo().EnableAll();
    GetSentencesBreakdownInfo().EnableAll();
    m_minDocWordCountForBatch = 50;
    m_randomSampleSizeForBatch = 15;
    m_filePathTruncationMode =
        ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode::OnlyShowFileNames;
    m_language = readability::test_language::english_test;
    // Keep reviewer to whatever it was before, don't reset that.
    // This isn't so much as setting, as an identifier for the user that they may be customized,
    // so leave it alone.
    m_realTimeUpdate = false;
    m_appendedDocumentFilePath.clear();
    // page setup
    m_paperId = wxPAPER_LETTER;
    m_paperOrientation = wxLANDSCAPE;
    // headers
    m_leftPrinterHeader.clear();
    m_centerPrinterHeader.clear();
    m_rightPrinterHeader.clear();
    // footers
    m_leftPrinterFooter.clear();
    m_centerPrinterFooter.clear();
    m_rightPrinterFooter.clear();
    // test inclusion options
    m_includeScoreSummaryReport = true;
    // test options
    m_dcTextExclusion = SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings;
    m_hjTextExclusion = SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings;
    m_dcProperNounCountingMethod = readability::proper_noun_counting_method::
        only_count_first_instance_of_proper_noun_as_unfamiliar;
    m_includeStockerCatholicDCSupplement = false;
    m_fogUseSentenceUnits = true;
    m_fleschNumeralSyllabizeMethod = FleschNumeralSyllabize::NumeralIsOneSyllable;
    m_fleschKincaidNumeralSyllabizeMethod =
        FleschKincaidNumeralSyllabize::FleschKincaidNumeralSoundOutEachDigit;
    m_raygorStyle = Wisteria::Graphs::RaygorStyle::BaldwinKaufman;
    // clear the colours
    m_customColours.clear();
    // reset the warning flags
    WarningManager::EnableWarnings();
    }

//------------------------------------------------
void ReadabilityAppOptions::LoadThemeNode(tinyxml2::XMLElement* appearanceNode)
    {
    if (appearanceNode != nullptr)
        {
        lily_of_the_valley::html_extract_text filter_html;

        int maximized = appearanceNode->ToElement()->IntAttribute(XML_WINDOW_MAXIMIZED.data(), 1);
        m_appWindowMaximized = int_to_bool(maximized);
        m_appWindowWidth = appearanceNode->ToElement()->IntAttribute(XML_WINDOW_WIDTH.data(), 800);
        m_appWindowHeight =
            appearanceNode->ToElement()->IntAttribute(XML_WINDOW_HEIGHT.data(), 700);
        // make sure the values make sense
        if (m_appWindowWidth < 1)
            {
            m_appWindowWidth = 800;
            }
        if (m_appWindowHeight < 1)
            {
            m_appWindowHeight = 700;
            }

        // macOS does its own theming, and WX aggressively applies it to its controls
// (dark theme effects the list and text controls). Because of this, the macOS
// version can't do custom ribbon/sidebar theming, it needs to use the system theming.
#ifndef __WXOSX__
        // window color
        auto windowColorNode =
            appearanceNode->FirstChildElement(XML_CONTROL_BACKGROUND_COLOR.data());
        if (windowColorNode)
            {
            int red = windowColorNode->ToElement()->IntAttribute(XmlFormat::GetRed().mb_str(), 255);
            int green =
                windowColorNode->ToElement()->IntAttribute(XmlFormat::GetGreen().mb_str(), 255);
            int blue =
                windowColorNode->ToElement()->IntAttribute(XmlFormat::GetBlue().mb_str(), 255);

            SetControlBackgroundColor(wxColour(red, green, blue));
            }
        else
            {
            SetControlBackgroundColor(
                wxSystemSettings::GetColour(wxSystemColour::wxSYS_COLOUR_WINDOW));
            }

        // ribbon
        auto ribbonActiveTabColorNode =
            appearanceNode->FirstChildElement(XML_RIBBON_ACTIVE_TAB_COLOR.data());
        if (ribbonActiveTabColorNode)
            {
            int red = ribbonActiveTabColorNode->ToElement()->IntAttribute(
                XmlFormat::GetRed().mb_str(), 255);
            int green = ribbonActiveTabColorNode->ToElement()->IntAttribute(
                XmlFormat::GetGreen().mb_str(), 255);
            int blue = ribbonActiveTabColorNode->ToElement()->IntAttribute(
                XmlFormat::GetBlue().mb_str(), 255);
            SetRibbonActiveTabColor(wxColour(red, green, blue));
            SetRibbonInactiveTabColor(
                Wisteria::Colors::ColorContrast::Shade(GetRibbonActiveTabColor()));
            }
        auto ribbonHoverColorNode =
            appearanceNode->FirstChildElement(XML_RIBBON_HOVER_COLOR.data());
        if (ribbonHoverColorNode)
            {
            int red =
                ribbonHoverColorNode->ToElement()->IntAttribute(XmlFormat::GetRed().mb_str(), 255);
            int green = ribbonHoverColorNode->ToElement()->IntAttribute(
                XmlFormat::GetGreen().mb_str(), 255);
            int blue =
                ribbonHoverColorNode->ToElement()->IntAttribute(XmlFormat::GetBlue().mb_str(), 255);
            SetRibbonHoverColor(wxColour(red, green, blue));
            }
        else
            {
            SetRibbonHoverColor(Wisteria::Colors::ColorContrast::Shade(GetRibbonActiveTabColor()));
            }
        SetRibbonActiveFontColor(
            Wisteria::Colors::ColorContrast::BlackOrWhiteContrast(GetRibbonActiveTabColor()));
        SetRibbonInactiveFontColor(
            Wisteria::Colors::ColorContrast::BlackOrWhiteContrast(GetRibbonInactiveTabColor()));
        SetRibbonHoverFontColor(
            Wisteria::Colors::ColorContrast::BlackOrWhiteContrast(GetRibbonHoverColor()));

        // sidebar
        auto sidebarBkColorNode =
            appearanceNode->FirstChildElement(XML_SIDEBAR_BACKGROUND_COLOR.data());
        if (sidebarBkColorNode)
            {
            int red =
                sidebarBkColorNode->ToElement()->IntAttribute(XmlFormat::GetRed().mb_str(), 255);
            int green =
                sidebarBkColorNode->ToElement()->IntAttribute(XmlFormat::GetGreen().mb_str(), 255);
            int blue =
                sidebarBkColorNode->ToElement()->IntAttribute(XmlFormat::GetBlue().mb_str(), 255);
            SetSideBarBackgroundColor(wxColour(red, green, blue));
            }
        auto sidebarActiveColorNode =
            appearanceNode->FirstChildElement(XML_SIDEBAR_ACTIVE_COLOR.data());
        if (sidebarActiveColorNode)
            {
            int red = sidebarActiveColorNode->ToElement()->IntAttribute(
                XmlFormat::GetRed().mb_str(), 255);
            int green = sidebarActiveColorNode->ToElement()->IntAttribute(
                XmlFormat::GetGreen().mb_str(), 255);
            int blue = sidebarActiveColorNode->ToElement()->IntAttribute(
                XmlFormat::GetBlue().mb_str(), 255);
            SetSideBarActiveColor(wxColour(red, green, blue));
            }
        else
            {
            SetSideBarActiveColor(
                Wisteria::Colors::ColorContrast::Shade(GetSideBarBackgroundColor()));
            }
        SetSideBarHoverColor(Wisteria::Colors::ColorContrast::Shade(GetSideBarActiveColor()));
        SetSideBarHoverFontColor(
            Wisteria::Colors::ColorContrast::BlackOrWhiteContrast(GetSideBarHoverColor()));

        auto sidebarParentColorNode =
            appearanceNode->FirstChildElement(XML_SIDEBAR_PARENT_COLOR.data());
        if (sidebarParentColorNode)
            {
            int red = sidebarParentColorNode->ToElement()->IntAttribute(
                XmlFormat::GetRed().mb_str(), 255);
            int green = sidebarParentColorNode->ToElement()->IntAttribute(
                XmlFormat::GetGreen().mb_str(), 255);
            int blue = sidebarParentColorNode->ToElement()->IntAttribute(
                XmlFormat::GetBlue().mb_str(), 255);
            SetSideBarParentColor(wxColour(red, green, blue));
            }
        else
            {
            SetSideBarParentColor(
                Wisteria::Colors::ColorContrast::Shade(GetSideBarBackgroundColor()));
            }
        SetSideBarFontColor(
            Wisteria::Colors::ColorContrast::BlackOrWhiteContrast(GetSideBarParentColor()));
        SetSideBarActiveFontColor(
            Wisteria::Colors::ColorContrast::BlackOrWhiteContrast(GetSideBarParentColor()));
#endif

        // see what theme is selected
        auto themeNode = appearanceNode->FirstChildElement(XML_THEME_NAME.data());
        if (themeNode)
            {
            const char* themeString = themeNode->ToElement()->Attribute(XML_VALUE.data());
            if (themeString)
                {
                const auto themeStr = Wisteria::TextStream::CharStreamToUnicode(
                    themeString, std::strlen(themeString));
                const wchar_t* convertedStr =
                    filter_html(themeStr.c_str(), themeStr.length(), true, false);
                if (convertedStr)
                    {
                    SetTheme(convertedStr);
                    // reset whatever was read in here if just using the system colors
                    if (GetTheme() == _DT(L"System"))
                        {
                        SetColorsFromSystem();
                        }
                    }
                }
            }
        }
    }

//------------------------------------------------
bool ReadabilityAppOptions::LoadThemeFile(const wxString& optionsFile)
    {
    if (!wxFile::Exists(optionsFile))
        {
        return false;
        }

    tinyxml2::XMLDocument doc;
    doc.LoadFile(optionsFile.mb_str());
    if (doc.Error())
        {
        wxMessageBox(wxString::Format(_(L"Unable to load theme file:\n%s"), doc.ErrorStr()),
                     _(L"Error"), wxOK | wxICON_ERROR);
        return false;
        }
    // see if it is a valid config file
    auto node = doc.FirstChildElement(XML_CONFIG_HEADER.data());
    if (!node)
        {
        wxMessageBox(_(L"Invalid configuration file. Project header section not found."),
                     _(L"Error"), wxOK | wxICON_ERROR);
        return false;
        }
    // read in the configurations
    auto configRootNode = node->FirstChildElement(XML_CONFIGURATIONS.data());
    if (!configRootNode)
        {
        wxMessageBox(_(L"Invalid configuration file. No configurations found."), _(L"Error"),
                     wxOK | wxICON_ERROR);
        return false;
        }

    auto appearanceNode = configRootNode->FirstChildElement(XML_APPEARANCE.data());
    LoadThemeNode(appearanceNode);
    return SaveOptionsFile();
    }

//------------------------------------------------
bool ReadabilityAppOptions::LoadOptionsFile(const wxString& optionsFile,
                                            const bool loadOnlyGeneralOptions /*= false*/,
                                            const bool writeChangesBackToThisFile /*= true*/)
    {
    if (writeChangesBackToThisFile)
        {
        // this is where we will save to later
        m_optionsFile = optionsFile;
        }

    if (!wxFile::Exists(optionsFile))
        {
        return false;
        }

    ResetSettings();

    m_appWindowWidth = wxSystemSettings::GetMetric(wxSYS_SCREEN_X) - 100;
    m_appWindowHeight = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y) - 100;

    lily_of_the_valley::html_extract_text filter_html;

    tinyxml2::XMLDocument doc;
    doc.LoadFile(optionsFile.mb_str());
    if (doc.Error())
        {
        // may appear while program is loading
        wxLogError(L"Unable to load configuration file:\n%s", doc.ErrorStr());
        return false;
        }
    // see if it is a valid config file
    auto node = doc.FirstChildElement(XML_CONFIG_HEADER.data());
    if (!node)
        {
        wxMessageBox(_(L"Invalid configuration file. Project header section not found."),
                     _(L"Error"), wxOK | wxICON_ERROR);
        return false;
        }
    // read in the configurations
    auto configRootNode = node->FirstChildElement(XML_CONFIGURATIONS.data());
    if (!configRootNode)
        {
        wxMessageBox(_(L"Invalid configuration file. No configurations found."), _(L"Error"),
                     wxOK | wxICON_ERROR);
        return false;
        }
    else
        {
        // general app information
        auto customColoursNode = configRootNode->FirstChildElement(XML_CUSTOM_COLORS.data());
        if (customColoursNode)
            {
            GetCustomColours().clear();
            for (int i = 0; i < m_maxCustomColors; ++i)
                {
                auto colourNode = customColoursNode->FirstChildElement(
                    wxString::Format(_DT(L"color%d"), i).mb_str());
                if (colourNode)
                    {
                    int red =
                        colourNode->ToElement()->IntAttribute(XmlFormat::GetRed().mb_str(), 255);
                    int green =
                        colourNode->ToElement()->IntAttribute(XmlFormat::GetBlue().mb_str(), 255);
                    int blue =
                        colourNode->ToElement()->IntAttribute(XmlFormat::GetGreen().mb_str(), 255);
                    GetCustomColours().push_back(wxColour(red, green, blue));
                    }
                else
                    {
                    break;
                    }
                }
            }

        // appearance of the program
        auto appearanceNode = configRootNode->FirstChildElement(XML_APPEARANCE.data());
        LoadThemeNode(appearanceNode);

        auto userAgentNode = configRootNode->FirstChildElement(XML_USER_AGENT.data());
        if (userAgentNode)
            {
            auto userAgent = userAgentNode->ToElement()->Attribute(XML_VALUE.data());
            if (userAgent)
                {
                const auto userAgentStr =
                    Wisteria::TextStream::CharStreamToUnicode(userAgent, std::strlen(userAgent));
                const wchar_t* convertedStr =
                    filter_html(userAgentStr.c_str(), userAgentStr.length(), true, false);
                if (convertedStr)
                    {
                    SetUserAgent(convertedStr);
                    wxGetApp().GetWebHarvester().SetUserAgent(convertedStr);
                    }
                }
            }

        auto downloadReplaceExistingNode =
            configRootNode->FirstChildElement(XML_DOWNLOAD_REPLACE_EXISTING.data());
        if (downloadReplaceExistingNode != nullptr)
            {
            wxGetApp().GetWebHarvester().ReplaceExistingFiles(
                downloadReplaceExistingNode->ToElement()->IntAttribute(
                    XML_VALUE.data(),
                    bool_to_int(wxGetApp().GetWebHarvester().IsReplacingExistingFiles())));
            }

        auto downloadWebFolderStructureNode =
            configRootNode->FirstChildElement(XML_DOWNLOAD_KEEP_FOLDER_STRUCTURE.data());
        if (downloadWebFolderStructureNode != nullptr)
            {
            wxGetApp().GetWebHarvester().KeepWebPathWhenDownloading(
                downloadWebFolderStructureNode->ToElement()->IntAttribute(
                    XML_VALUE.data(),
                    bool_to_int(wxGetApp().GetWebHarvester().IsKeepingWebPathWhenDownloading())));
            }

        auto downloadMinFileSizeNode =
            configRootNode->FirstChildElement(XML_DOWNLOAD_MIN_FILESIZE.data());
        if (downloadMinFileSizeNode != nullptr)
            {
            wxGetApp().GetWebHarvester().SetMinimumDownloadFileSizeInKilobytes(
                downloadMinFileSizeNode->ToElement()->IntAttribute(XML_VALUE.data(), 0));
            }

        auto useJsCookiesNode =
            configRootNode->FirstChildElement(XML_USE_JS_COOKIES.data());
        if (useJsCookiesNode != nullptr)
            {
            const int value = useJsCookiesNode->ToElement()->IntAttribute(XML_VALUE.data(), 0);
            m_useJsCookies = int_to_bool(value);
            wxGetApp().GetWebHarvester().UseJavaScriptCookies(m_useJsCookies);
            }

        auto persistCookiesNode = configRootNode->FirstChildElement(XML_PERSIST_COOKIES.data());
        if (persistCookiesNode != nullptr)
            {
            const int value = persistCookiesNode->ToElement()->IntAttribute(XML_VALUE.data(), 0);
            m_persistJsCookies = int_to_bool(value);
            wxGetApp().GetWebHarvester().PersistJavaScriptCookies(m_persistJsCookies);
            }

        auto disablePeerVerifyNode =
            configRootNode->FirstChildElement(XML_DISABLE_PEER_VERIFY.data());
        if (disablePeerVerifyNode != nullptr)
            {
            const int value = disablePeerVerifyNode->ToElement()->IntAttribute(XML_VALUE.data(), 0);
            m_disablePeerVerify = int_to_bool(value);
            wxGetApp().GetWebHarvester().DisablePeerVerify(m_disablePeerVerify);
            }

        auto filePathsNode = configRootNode->FirstChildElement(XML_FILE_OPEN_PATHS.data());
        if (filePathsNode)
            {
            auto wordlistPathNode =
                filePathsNode->FirstChildElement(XML_FILE_OPEN_WORDLIST_PATH.data());
            if (wordlistPathNode)
                {
                const char* wordlistPathString =
                    wordlistPathNode->ToElement()->Attribute(XML_VALUE.data());
                if (wordlistPathString)
                    {
                    const auto wordlistPathStr = Wisteria::TextStream::CharStreamToUnicode(
                        wordlistPathString, std::strlen(wordlistPathString));
                    const wchar_t* convertedStr =
                        filter_html(wordlistPathStr.c_str(), wordlistPathStr.length(), true, false);
                    if (convertedStr)
                        {
                        SetWordListPath(convertedStr);
                        }
                    }
                }
            auto projectPathNode =
                filePathsNode->FirstChildElement(XML_FILE_OPEN_PROJECT_PATH.data());
            if (projectPathNode)
                {
                const char* projectPathString =
                    projectPathNode->ToElement()->Attribute(XML_VALUE.data());
                if (projectPathString)
                    {
                    const auto projectPathStr = Wisteria::TextStream::CharStreamToUnicode(
                        projectPathString, std::strlen(projectPathString));
                    const wchar_t* convertedStr =
                        filter_html(projectPathStr.c_str(), projectPathStr.length(), true, false);
                    if (convertedStr)
                        {
                        SetProjectPath(convertedStr);
                        }
                    }
                }
            auto imagePathNode = filePathsNode->FirstChildElement(XML_FILE_OPEN_IMAGE_PATH.data());
            if (imagePathNode)
                {
                const char* imagePathString =
                    imagePathNode->ToElement()->Attribute(XML_VALUE.data());
                if (imagePathString)
                    {
                    const auto imagePathStr = Wisteria::TextStream::CharStreamToUnicode(
                        imagePathString, std::strlen(imagePathString));
                    const wchar_t* convertedStr =
                        filter_html(imagePathStr.c_str(), imagePathStr.length(), true, false);
                    if (convertedStr)
                        {
                        SetImagePath(convertedStr);
                        }
                    }
                }
            }
        // log report settings
        auto logSettingsNode = configRootNode->FirstChildElement(XML_LOG_SETTINGS.data());
        if (logSettingsNode != nullptr)
            {
            auto logVerboseNode = logSettingsNode->FirstChildElement(XML_LOG_VERBOSE.data());
            if (logVerboseNode != nullptr)
                {
                wxGetApp().GetLogFile()->SetVerbose(
                    int_to_bool(logVerboseNode->ToElement()->IntAttribute(XML_VALUE.data(), 1)));
                }
            auto logAppendNode = logSettingsNode->FirstChildElement(XML_LOG_APPEND_DAILY.data());
            if (logAppendNode != nullptr)
                {
                AppendDailyLog(
                    int_to_bool(logAppendNode->ToElement()->IntAttribute(XML_VALUE.data(), 1)));
                }
            }
        // printer settings
        auto printerSettingsNode = configRootNode->FirstChildElement(XML_PRINTER_SETTINGS.data());
        if (printerSettingsNode)
            {
            int value = 0;
            auto printerNode = printerSettingsNode->FirstChildElement(XML_PRINTER_ID.data());
            if (printerNode)
                {
                value = printerNode->ToElement()->IntAttribute(XML_VALUE.data(), value);
                SetPaperId(value);
                }
            value = 0;
            printerNode = printerSettingsNode->FirstChildElement(XML_PRINTER_ORIENTATION.data());
            if (printerNode)
                {
                value = printerNode->ToElement()->IntAttribute(XML_VALUE.data(), value);
                SetPaperOrientation(static_cast<wxPrintOrientation>(value));
                }
            printerNode = printerSettingsNode->FirstChildElement(XML_PRINTER_LEFT_HEADER.data());
            if (printerNode)
                {
                const char* printerString = printerNode->ToElement()->Attribute(XML_VALUE.data());
                if (printerString)
                    {
                    const auto printStr = Wisteria::TextStream::CharStreamToUnicode(
                        printerString, std::strlen(printerString));
                    const wchar_t* convertedStr =
                        filter_html(printStr.c_str(), printStr.length(), true, false);
                    if (convertedStr)
                        {
                        SetLeftPrinterHeader(convertedStr);
                        }
                    }
                }
            printerNode = printerSettingsNode->FirstChildElement(XML_PRINTER_CENTER_HEADER.data());
            if (printerNode)
                {
                const char* printerString = printerNode->ToElement()->Attribute(XML_VALUE.data());
                if (printerString)
                    {
                    const auto printStr = Wisteria::TextStream::CharStreamToUnicode(
                        printerString, std::strlen(printerString));
                    const wchar_t* convertedStr =
                        filter_html(printStr.c_str(), printStr.length(), true, false);
                    if (convertedStr)
                        {
                        SetCenterPrinterHeader(convertedStr);
                        }
                    }
                }
            printerNode = printerSettingsNode->FirstChildElement(XML_PRINTER_RIGHT_HEADER.data());
            if (printerNode)
                {
                const char* printerString = printerNode->ToElement()->Attribute(XML_VALUE.data());
                if (printerString)
                    {
                    const auto printStr = Wisteria::TextStream::CharStreamToUnicode(
                        printerString, std::strlen(printerString));
                    const wchar_t* convertedStr =
                        filter_html(printStr.c_str(), printStr.length(), true, false);
                    if (convertedStr)
                        {
                        SetRightPrinterHeader(convertedStr);
                        }
                    }
                }
            printerNode = printerSettingsNode->FirstChildElement(XML_PRINTER_LEFT_FOOTER.data());
            if (printerNode)
                {
                const char* printerString = printerNode->ToElement()->Attribute(XML_VALUE.data());
                if (printerString)
                    {
                    const auto printStr = Wisteria::TextStream::CharStreamToUnicode(
                        printerString, std::strlen(printerString));
                    const wchar_t* convertedStr =
                        filter_html(printStr.c_str(), printStr.length(), true, false);
                    if (convertedStr)
                        {
                        SetLeftPrinterFooter(convertedStr);
                        }
                    }
                }
            printerNode = printerSettingsNode->FirstChildElement(XML_PRINTER_CENTER_FOOTER.data());
            if (printerNode)
                {
                const char* printerString = printerNode->ToElement()->Attribute(XML_VALUE.data());
                if (printerString)
                    {
                    const auto printStr = Wisteria::TextStream::CharStreamToUnicode(
                        printerString, std::strlen(printerString));
                    const wchar_t* convertedStr =
                        filter_html(printStr.c_str(), printStr.length(), true, false);
                    if (convertedStr)
                        {
                        SetCenterPrinterFooter(convertedStr);
                        }
                    }
                }
            printerNode = printerSettingsNode->FirstChildElement(XML_PRINTER_RIGHT_FOOTER.data());
            if (printerNode)
                {
                const char* printerString = printerNode->ToElement()->Attribute(XML_VALUE.data());
                if (printerString)
                    {
                    const auto printStr = Wisteria::TextStream::CharStreamToUnicode(
                        printerString, std::strlen(printerString));
                    const wchar_t* convertedStr =
                        filter_html(printStr.c_str(), printStr.length(), true, false);
                    if (convertedStr)
                        {
                        SetRightPrinterFooter(convertedStr);
                        }
                    }
                }
            }
        // editor settings
        auto editorSettingsNode = configRootNode->FirstChildElement(XML_EDITOR.data());
        if (editorSettingsNode)
            {
            auto fontColorNode = editorSettingsNode->FirstChildElement(XML_EDITOR_FONTCOLOR.data());
            if (fontColorNode)
                {
                int red = fontColorNode->ToElement()->IntAttribute(XmlFormat::GetRed().mb_str(),
                                                                   m_editorFontColor.Red());
                int green = fontColorNode->ToElement()->IntAttribute(XmlFormat::GetGreen().mb_str(),
                                                                     m_editorFontColor.Green());
                int blue = fontColorNode->ToElement()->IntAttribute(XmlFormat::GetBlue().mb_str(),
                                                                    m_editorFontColor.Blue());
                m_editorFontColor.Set(red, green, blue);
                }
            // font
            auto fontNode = editorSettingsNode->FirstChildElement(XML_EDITOR_FONT.data());
            if (fontNode)
                {
                int pointSize = fontNode->ToElement()->IntAttribute(
                    XmlFormat::GetFontPointSize().mb_str(),
                    wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize());
                int style = fontNode->ToElement()->IntAttribute(XmlFormat::GetFontStyle().mb_str(),
                                                                wxFONTSTYLE_NORMAL);
                int weight = fontNode->ToElement()->IntAttribute(
                    XmlFormat::GetFontWeight().mb_str(), wxFONTWEIGHT_NORMAL);
                int underlined =
                    fontNode->ToElement()->IntAttribute(XmlFormat::GetFontUnderline().mb_str(), 0);
                // get the font point size
                m_editorFont.SetPointSize(
                    (pointSize > 0) ?
                        pointSize :
                        wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize());
                // get the font style
                m_editorFont.SetStyle(static_cast<wxFontStyle>(style));
                // get the font weight
                m_editorFont.SetWeight(static_cast<wxFontWeight>(weight));
                // get the font underlining
                m_editorFont.SetUnderlined(int_to_bool(underlined));
                // get the font facename
                const char* faceName =
                    fontNode->ToElement()->Attribute(XmlFormat::GetFontFaceName().mb_str());
                if (faceName)
                    {
                    const auto faceNameStr =
                        Wisteria::TextStream::CharStreamToUnicode(faceName, std::strlen(faceName));
                    const wchar_t* filteredText =
                        filter_html(faceNameStr.c_str(), faceNameStr.length(), true, false);
                    if (filteredText && wxFontEnumerator::IsValidFacename(filteredText))
                        {
                        m_editorFont.SetFaceName(wxString(filteredText));
                        }
                    }
                }
            auto indentNode = editorSettingsNode->FirstChildElement(XML_EDITOR_INDENT.data());
            if (indentNode)
                {
                IndentEditor(
                    int_to_bool(indentNode->ToElement()->IntAttribute(XML_VALUE.data(), 1)));
                }
            auto spaceAfterParagraphNode =
                editorSettingsNode->FirstChildElement(XML_EDITOR_SPACE_AFTER_PARAGRAPH.data());
            if (spaceAfterParagraphNode)
                {
                AddParagraphSpaceInEditor(int_to_bool(
                    spaceAfterParagraphNode->ToElement()->IntAttribute(XML_VALUE.data(), 1)));
                }
            auto textAlignNode =
                editorSettingsNode->FirstChildElement(XML_EDITOR_TEXT_ALIGNMENT.data());
            if (textAlignNode)
                {
                SetEditorTextAlignment(static_cast<wxTextAttrAlignment>(
                    textAlignNode->ToElement()->IntAttribute(XML_VALUE.data(), 1)));
                }
            auto lineSpacingnNode =
                editorSettingsNode->FirstChildElement(XML_EDITOR_LINE_SPACING.data());
            if (lineSpacingnNode)
                {
                SetEditorLineSpacing(static_cast<wxTextAttrLineSpacing>(
                    lineSpacingnNode->ToElement()->IntAttribute(XML_VALUE.data(), 1)));
                }
            }
        // just get the reviewer from project settings to be used for the start page
        auto projectSettingsForReview =
            configRootNode->FirstChildElement(XML_PROJECT_SETTINGS.data());
        if (projectSettingsForReview)
            {
            auto projectReviewer = projectSettingsForReview->FirstChildElement(XML_REVIEWER.data());
            if (projectReviewer)
                {
                const char* reviewerChars =
                    projectReviewer->ToElement()->Attribute(XML_VALUE.data());
                if (reviewerChars)
                    {
                    const auto reviewerStr = Wisteria::TextStream::CharStreamToUnicode(
                        reviewerChars, std::strlen(reviewerChars));
                    const wchar_t* convertedStr =
                        filter_html(reviewerStr.c_str(), reviewerStr.length(), true, false);
                    if (convertedStr)
                        {
                        SetReviewer(convertedStr);
                        }
                    }
                }
            }
        if (GetReviewer().empty())
            {
            SetReviewer(wxGetUserName());
            }

        // if only loading general info, then quit after reading this node
        if (loadOnlyGeneralOptions)
            {
            return true;
            }
        // warning settings
        auto warningSettingsNode =
            configRootNode->FirstChildElement(XML_WARNING_MESSAGE_SETTINGS.data());
        if (warningSettingsNode)
            {
            auto warningNode = warningSettingsNode->FirstChildElement(XML_WARNING_MESSAGE.data());
            while (warningNode)
                {
                const char* warningStringId = warningNode->ToElement()->Attribute(XML_VALUE.data());
                if (warningStringId)
                    {
                    auto id = Wisteria::TextStream::CharStreamToUnicode(
                        warningStringId, std::strlen(warningStringId));
                    std::vector<WarningMessage>::iterator warningIter =
                        WarningManager::GetWarning(id);
                    if (warningIter != WarningManager::GetWarnings().end())
                        {
                        int value = warningNode->ToElement()->IntAttribute(XML_DISPLAY.data(), 1);
                        warningIter->Show(int_to_bool(value));
                        value =
                            warningNode->ToElement()->IntAttribute(XML_PREVIOUS_RESPONSE.data(), 0);
                        warningIter->SetPreviousResponse(value);
                        }
                    }
                warningNode = warningNode->NextSiblingElement(XML_WARNING_MESSAGE.data());
                }
            }

        // export sections
        auto exportSettingsNode = configRootNode->FirstChildElement(XML_EXPORT.data());
        if (exportSettingsNode)
            {
            auto exportExtNode = exportSettingsNode->FirstChildElement(XML_EXPORT_LIST_EXT.data());
            if (exportExtNode)
                {
                const char* extString = exportExtNode->ToElement()->Attribute(XML_VALUE.data());
                if (extString)
                    {
                    BaseProjectDoc::SetExportListExt(Wisteria::TextStream::CharStreamToUnicode(
                        extString, std::strlen(extString)));
                    }
                }
            exportExtNode = exportSettingsNode->FirstChildElement(XML_EXPORT_TEXT_EXT.data());
            if (exportExtNode)
                {
                const char* extString = exportExtNode->ToElement()->Attribute(XML_VALUE.data());
                if (extString)
                    {
                    BaseProjectDoc::SetExportTextViewExt(Wisteria::TextStream::CharStreamToUnicode(
                        extString, std::strlen(extString)));
                    }
                }
            exportExtNode = exportSettingsNode->FirstChildElement(XML_EXPORT_GRAPH_EXT.data());
            if (exportExtNode)
                {
                const char* extString = exportExtNode->ToElement()->Attribute(XML_VALUE.data());
                if (extString)
                    {
                    BaseProjectDoc::SetExportGraphExt(Wisteria::TextStream::CharStreamToUnicode(
                        extString, std::strlen(extString)));
                    }
                }
            auto exportNode = exportSettingsNode->FirstChildElement(XML_EXPORT_LISTS.data());
            if (exportNode)
                {
                BaseProjectDoc::ExportHardWordLists(
                    int_to_bool(exportNode->ToElement()->IntAttribute(XML_VALUE.data(), 1)));
                }
            exportNode =
                exportSettingsNode->FirstChildElement(XML_EXPORT_SENTENCES_BREAKDOWN.data());
            if (exportNode)
                {
                BaseProjectDoc::ExportSentencesBreakdown(
                    int_to_bool(exportNode->ToElement()->IntAttribute(XML_VALUE.data(), 1)));
                }
            exportNode = exportSettingsNode->FirstChildElement(XML_EXPORT_GRAPHS.data());
            if (exportNode)
                {
                BaseProjectDoc::ExportGraphs(
                    int_to_bool(exportNode->ToElement()->IntAttribute(XML_VALUE.data(), 1)));
                }
            exportNode = exportSettingsNode->FirstChildElement(XML_EXPORT_TEST_RESULTS.data());
            if (exportNode)
                {
                BaseProjectDoc::ExportTestResults(
                    int_to_bool(exportNode->ToElement()->IntAttribute(XML_VALUE.data(), 1)));
                }
            exportNode = exportSettingsNode->FirstChildElement(XML_EXPORT_STATS.data());
            if (exportNode)
                {
                BaseProjectDoc::ExportStatistics(
                    int_to_bool(exportNode->ToElement()->IntAttribute(XML_VALUE.data(), 1)));
                }
            exportNode = exportSettingsNode->FirstChildElement(XML_EXPORT_GRAMMAR.data());
            if (exportNode)
                {
                BaseProjectDoc::ExportWordiness(
                    int_to_bool(exportNode->ToElement()->IntAttribute(XML_VALUE.data(), 1)));
                }
            exportNode = exportSettingsNode->FirstChildElement(XML_EXPORT_DOLCH_WORDS.data());
            if (exportNode)
                {
                BaseProjectDoc::ExportSightWords(
                    int_to_bool(exportNode->ToElement()->IntAttribute(XML_VALUE.data(), 1)));
                }
            exportNode = exportSettingsNode->FirstChildElement(XML_EXPORT_WARNINGS.data());
            if (exportNode)
                {
                BaseProjectDoc::ExportWarnings(
                    int_to_bool(exportNode->ToElement()->IntAttribute(XML_VALUE.data(), 1)));
                }
            }
        // New Project Settings section
        auto projectSettings = configRootNode->FirstChildElement(XML_PROJECT_SETTINGS.data());
        if (projectSettings)
            {
            // reviewer and status
            auto projectReviewer = projectSettings->FirstChildElement(XML_REVIEWER.data());
            if (projectReviewer)
                {
                const char* reviewerChars =
                    projectReviewer->ToElement()->Attribute(XML_VALUE.data());
                if (reviewerChars)
                    {
                    const auto reviewerStr = Wisteria::TextStream::CharStreamToUnicode(
                        reviewerChars, std::strlen(reviewerChars));
                    const wchar_t* convertedStr =
                        filter_html(reviewerStr.c_str(), reviewerStr.length(), true, false);
                    if (convertedStr)
                        {
                        SetReviewer(convertedStr);
                        }
                    }
                }
            auto realTimeRefresh = projectSettings->FirstChildElement(XML_REALTIME_UPDATE.data());
            if (realTimeRefresh)
                {
                const char* realTimeChars =
                    realTimeRefresh->ToElement()->Attribute(XML_VALUE.data());
                if (realTimeChars)
                    {
                    const auto realTimeStr = Wisteria::TextStream::CharStreamToUnicode(
                        realTimeChars, std::strlen(realTimeChars));
                    const wchar_t* convertedStr =
                        filter_html(realTimeStr.c_str(), realTimeStr.length(), true, false);
                    if (convertedStr)
                        {
                        UseRealTimeUpdate(convertedStr);
                        }
                    }
                }
            auto appendedDocPath = projectSettings->FirstChildElement(XML_APPENDED_DOC_PATH.data());
            if (appendedDocPath)
                {
                const char* appendedDocChars =
                    appendedDocPath->ToElement()->Attribute(XML_VALUE.data());
                if (appendedDocChars)
                    {
                    const auto appendedDocStr = Wisteria::TextStream::CharStreamToUnicode(
                        appendedDocChars, std::strlen(appendedDocChars));
                    const wchar_t* convertedStr =
                        filter_html(appendedDocStr.c_str(), appendedDocStr.length(), true, false);
                    if (convertedStr)
                        {
                        SetAppendedDocumentFilePath(convertedStr);
                        }
                    }
                }
            // document storage/linking
            auto docStorageNode =
                projectSettings->FirstChildElement(XML_DOCUMENT_STORAGE_METHOD.data());
            if (docStorageNode)
                {
                m_documentStorageMethod =
                    static_cast<TextStorage>(docStorageNode->ToElement()->IntAttribute(
                        XML_METHOD.data(), static_cast<int>(m_documentStorageMethod)));
                // verify that this is a sensical value
                if (m_documentStorageMethod != TextStorage::EmbedText &&
                    m_documentStorageMethod != TextStorage::NoEmbedText)
                    {
                    m_documentStorageMethod = TextStorage::NoEmbedText;
                    }
                }
            auto projectLang = projectSettings->FirstChildElement(XML_PROJECT_LANGUAGE.data());
            if (projectLang)
                {
                int value = projectLang->ToElement()->IntAttribute(XML_VALUE.data(),
                                                                   static_cast<int>(m_language));
                if (value < 0 || value >= static_cast<decltype(value)>(
                                              readability::test_language::TEST_LANGUAGE_COUNT))
                    {
                    value = static_cast<decltype(value)>(readability::test_language::english_test);
                    }
                else if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(
                             wxGetApp().FeatureLanguagePackCode()) &&
                         !wxGetApp().GetLicenseAdmin().IsFeatureEnabled(
                             wxGetApp().FeatureProfessionalCode()))
                    {
                    value = static_cast<decltype(value)>(readability::test_language::english_test);
                    }
                m_language = static_cast<readability::test_language>(value);
                }
            auto randSizeSizeNode =
                projectSettings->FirstChildElement(XML_RANDOM_SAMPLE_SIZE.data());
            if (randSizeSizeNode)
                {
                int value = randSizeSizeNode->ToElement()->IntAttribute(
                    XML_VALUE.data(), GetBatchRandomSamplingSize());
                // verify that this is a sensical value
                if (value < 1 || value > 100)
                    {
                    value = 25;
                    }
                SetBatchRandomSamplingSize(static_cast<size_t>(value));
                }
            auto minDocSizeNode =
                projectSettings->FirstChildElement(XML_MIN_DOC_SIZE_FOR_BATCH.data());
            if (minDocSizeNode)
                {
                int value = minDocSizeNode->ToElement()->IntAttribute(XML_VALUE.data(),
                                                                      GetMinDocWordCountForBatch());
                // verify that this is a sensical value
                if (value < 1)
                    {
                    value = 1;
                    }
                SetMinDocWordCountForBatch(static_cast<size_t>(value));
                }
            auto filePathTruncModeNode =
                projectSettings->FirstChildElement(XML_FILE_PATH_TRUNC_MODE.data());
            if (filePathTruncModeNode)
                {
                int value = filePathTruncModeNode->ToElement()->IntAttribute(
                    XML_VALUE.data(), static_cast<int>(GetFilePathTruncationMode()));
                // verify that this is a sensical value
                if (value < 0 || value >= static_cast<decltype(value)>(
                                              ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode::
                                                  COLUMN_FILE_PATHS_TRUNCATION_MODE_COUNT))
                    {
                    value = static_cast<decltype(value)>(
                        ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode::OnlyShowFileNames);
                    }
                SetFilePathTruncationMode(
                    static_cast<ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode>(value));
                }
            // sentences breakdown
            auto sentencesBreakdownNode =
                projectSettings->FirstChildElement(XML_SENTENCES_BREAKDOWN.data());
            if (sentencesBreakdownNode)
                {
                // which options are included
                auto sentencesBreakdownInfoNode =
                    sentencesBreakdownNode->FirstChildElement(XML_SENTENCES_BREAKDOWN_INFO.data());
                if (sentencesBreakdownInfoNode)
                    {
                    const char* InfoChars =
                        sentencesBreakdownInfoNode->ToElement()->Attribute(XML_VALUE.data());
                    if (InfoChars)
                        {
                        const auto InfoStr = Wisteria::TextStream::CharStreamToUnicode(
                            InfoChars, std::strlen(InfoChars));
                        const wchar_t* convertedStr =
                            filter_html(InfoStr.c_str(), InfoStr.length(), true, false);
                        if (convertedStr)
                            {
                            GetSentencesBreakdownInfo().Set(convertedStr);
                            }
                        }
                    }
                }
            // words breakdown
            auto wordsBreakdownNode =
                projectSettings->FirstChildElement(XML_WORDS_BREAKDOWN.data());
            if (wordsBreakdownNode)
                {
                // which options are included
                auto wordsBreakdownInfoNode =
                    wordsBreakdownNode->FirstChildElement(XML_WORDS_BREAKDOWN_INFO.data());
                if (wordsBreakdownInfoNode)
                    {
                    const char* InfoChars =
                        wordsBreakdownInfoNode->ToElement()->Attribute(XML_VALUE.data());
                    if (InfoChars)
                        {
                        const auto InfoStr = Wisteria::TextStream::CharStreamToUnicode(
                            InfoChars, std::strlen(InfoChars));
                        const wchar_t* convertedStr =
                            filter_html(InfoStr.c_str(), InfoStr.length(), true, false);
                        if (convertedStr)
                            {
                            GetWordsBreakdownInfo().Set(convertedStr);
                            }
                        }
                    }
                }
            // grammar
            auto grammarNode = projectSettings->FirstChildElement(XML_GRAMMAR.data());
            if (grammarNode)
                {
                // spell checking options
                auto spellCheckNode =
                    grammarNode->FirstChildElement(XML_SPELLCHECK_IGNORE_PROPER_NOUNS.data());
                if (spellCheckNode)
                    {
                    SpellCheckIgnoreProperNouns(
                        int_to_bool(spellCheckNode->ToElement()->IntAttribute(
                            XML_VALUE.data(), bool_to_int(SpellCheckIsIgnoringProperNouns()))));
                    }
                spellCheckNode =
                    grammarNode->FirstChildElement(XML_SPELLCHECK_IGNORE_UPPERCASED.data());
                if (spellCheckNode)
                    {
                    SpellCheckIgnoreUppercased(
                        int_to_bool(spellCheckNode->ToElement()->IntAttribute(
                            XML_VALUE.data(), bool_to_int(SpellCheckIsIgnoringUppercased()))));
                    }
                spellCheckNode =
                    grammarNode->FirstChildElement(XML_SPELLCHECK_IGNORE_NUMERALS.data());
                if (spellCheckNode)
                    {
                    SpellCheckIgnoreNumerals(int_to_bool(spellCheckNode->ToElement()->IntAttribute(
                        XML_VALUE.data(), bool_to_int(SpellCheckIsIgnoringNumerals()))));
                    }
                spellCheckNode =
                    grammarNode->FirstChildElement(XML_SPELLCHECK_IGNORE_FILE_ADDRESSES.data());
                if (spellCheckNode)
                    {
                    SpellCheckIgnoreFileAddresses(
                        int_to_bool(spellCheckNode->ToElement()->IntAttribute(
                            XML_VALUE.data(), bool_to_int(SpellCheckIsIgnoringFileAddresses()))));
                    }
                spellCheckNode =
                    grammarNode->FirstChildElement(XML_SPELLCHECK_IGNORE_PROGRAMMER_CODE.data());
                if (spellCheckNode)
                    {
                    SpellCheckIgnoreProgrammerCode(
                        int_to_bool(spellCheckNode->ToElement()->IntAttribute(
                            XML_VALUE.data(), bool_to_int(SpellCheckIsIgnoringProgrammerCode()))));
                    }
                spellCheckNode =
                    grammarNode->FirstChildElement(XML_SPELLCHECK_ALLOW_COLLOQUIALISMS.data());
                if (spellCheckNode)
                    {
                    SpellCheckAllowColloquialisms(
                        int_to_bool(spellCheckNode->ToElement()->IntAttribute(
                            XML_VALUE.data(), bool_to_int(SpellCheckIsAllowingColloquialisms()))));
                    }
                spellCheckNode =
                    grammarNode->FirstChildElement(XML_SPELLCHECK_IGNORE_SOCIAL_MEDIA_TAGS.data());
                if (spellCheckNode)
                    {
                    SpellCheckIgnoreSocialMediaTags(
                        int_to_bool(spellCheckNode->ToElement()->IntAttribute(
                            XML_VALUE.data(), bool_to_int(SpellCheckIsIgnoringSocialMediaTags()))));
                    }
                // which grammar options are included
                auto grammarInfoNode = grammarNode->FirstChildElement(XML_GRAMMAR_INFO.data());
                if (grammarInfoNode)
                    {
                    const char* grammarInfoChars =
                        grammarInfoNode->ToElement()->Attribute(XML_VALUE.data());
                    if (grammarInfoChars)
                        {
                        const auto grammarInfStr = Wisteria::TextStream::CharStreamToUnicode(
                            grammarInfoChars, std::strlen(grammarInfoChars));
                        const wchar_t* convertedStr =
                            filter_html(grammarInfStr.c_str(), grammarInfStr.length(), true, false);
                        if (convertedStr)
                            {
                            GetGrammarInfo().Set(convertedStr);
                            }
                        }
                    }
                }
            // wizard page defaults
            auto documentAnalysisNode =
                projectSettings->FirstChildElement(XML_DOCUMENT_ANALYSIS_LOGIC.data());
            if (documentAnalysisNode)
                {
                // determinant for what a long sentence is
                auto longSentenceNode =
                    documentAnalysisNode->FirstChildElement(XML_LONG_SENTENCE_METHOD.data());
                if (longSentenceNode)
                    {
                    m_longSentenceMethod =
                        static_cast<LongSentence>(longSentenceNode->ToElement()->IntAttribute(
                            XML_METHOD.data(), static_cast<int>(m_longSentenceMethod)));
                    // verify that this is a sensical value
                    if (m_longSentenceMethod != LongSentence::LongerThanSpecifiedLength &&
                        m_longSentenceMethod != LongSentence::OutlierLength)
                        {
                        m_longSentenceMethod = LongSentence::LongerThanSpecifiedLength;
                        }
                    }
                auto longSentenceLengthNode =
                    documentAnalysisNode->FirstChildElement(XML_LONG_SENTENCE_LENGTH.data());
                if (longSentenceLengthNode)
                    {
                    m_difficultSentenceLength = longSentenceLengthNode->ToElement()->IntAttribute(
                        XML_VALUE.data(), m_difficultSentenceLength);
                    // verify that this is a sensical value
                    if (m_difficultSentenceLength <= 0)
                        {
                        m_difficultSentenceLength = 22;
                        }
                    }
                // determinant for how to syllabize numerals
                auto numSyllNode = documentAnalysisNode->FirstChildElement(
                    XML_NUMERAL_SYLLABICATION_METHOD.data());
                if (numSyllNode)
                    {
                    m_numeralSyllabicationMethod =
                        static_cast<NumeralSyllabize>(numSyllNode->ToElement()->IntAttribute(
                            XML_METHOD.data(), static_cast<int>(m_numeralSyllabicationMethod)));
                    // verify that this is a sensical value
                    if (m_numeralSyllabicationMethod != NumeralSyllabize::WholeWordIsOneSyllable &&
                        m_numeralSyllabicationMethod != NumeralSyllabize::SoundOutEachDigit)
                        {
                        m_numeralSyllabicationMethod = NumeralSyllabize::WholeWordIsOneSyllable;
                        }
                    }
                // whether we should ignore blank lines when parsing paragraphs
                auto ignoreBlankLines = documentAnalysisNode->FirstChildElement(
                    XML_IGNORE_BLANK_LINES_FOR_PARAGRAPH_PARSING.data());
                if (ignoreBlankLines)
                    {
                    m_ignoreBlankLinesForParagraphsParser =
                        int_to_bool(ignoreBlankLines->ToElement()->IntAttribute(
                            XML_VALUE.data(), bool_to_int(m_ignoreBlankLinesForParagraphsParser)));
                    }
                // whether we should ignore indenting when parsing paragraphs
                auto ignoreIndents = documentAnalysisNode->FirstChildElement(
                    XML_IGNORE_INDENTING_FOR_PARAGRAPH_PARSING.data());
                if (ignoreIndents)
                    {
                    m_ignoreIndentingForParagraphsParser =
                        int_to_bool(ignoreIndents->ToElement()->IntAttribute(
                            XML_VALUE.data(), bool_to_int(m_ignoreIndentingForParagraphsParser)));
                    }
                // whether sentences must start capitalized
                auto sentenceStartMustBeUppercased = documentAnalysisNode->FirstChildElement(
                    XML_SENTENCES_MUST_START_CAPITALIZED.data());
                if (sentenceStartMustBeUppercased)
                    {
                    m_sentenceStartMustBeUppercased =
                        int_to_bool(sentenceStartMustBeUppercased->ToElement()->IntAttribute(
                            XML_VALUE.data(), bool_to_int(m_sentenceStartMustBeUppercased)));
                    }
                // file path to phrases to exclude from analysis
                wxString filePath;
                auto excludedPhrasesFilePath =
                    documentAnalysisNode->FirstChildElement(XML_EXCLUDED_PHRASES_PATH.data());
                if (excludedPhrasesFilePath)
                    {
                    const char* filePathChars =
                        excludedPhrasesFilePath->ToElement()->Attribute(XML_VALUE.data());
                    if (filePathChars)
                        {
                        const auto filePathStr = Wisteria::TextStream::CharStreamToUnicode(
                            filePathChars, std::strlen(filePathChars));
                        const wchar_t* convertedStr =
                            filter_html(filePathStr.c_str(), filePathStr.length(), true, false);
                        if (convertedStr)
                            {
                            SetExcludedPhrasesPath(convertedStr);
                            }
                        }
                    }
                // whether to include first occurrence of excluded phrases
                auto includeExcludedPhraseFirstOccurrence = documentAnalysisNode->FirstChildElement(
                    XML_EXCLUDED_PHRASES_INCLUDE_FIRST_OCCURRENCE.data());
                if (includeExcludedPhraseFirstOccurrence)
                    {
                    IncludeExcludedPhraseFirstOccurrence(
                        int_to_bool(includeExcludedPhraseFirstOccurrence->ToElement()->IntAttribute(
                            XML_VALUE.data(),
                            bool_to_int(IsIncludingExcludedPhraseFirstOccurrence()))));
                    }
                auto excludedBlockTags =
                    documentAnalysisNode->FirstChildElement(XML_EXCLUDE_BLOCK_TAGS.data());
                if (excludedBlockTags)
                    {
                    auto excludedBlockTagNode =
                        excludedBlockTags->FirstChildElement(XML_EXCLUDE_BLOCK_TAG.data());
                    while (excludedBlockTagNode)
                        {
                        const wxString blockTags =
                            TiXmlNodeToString(excludedBlockTagNode, XML_VALUE.data());
                        if (blockTags.length() >= 2)
                            {
                            m_exclusionBlockTags.push_back(
                                std::make_pair(blockTags[0], blockTags[1]));
                            }
                        excludedBlockTagNode =
                            excludedBlockTagNode->NextSiblingElement(XML_EXCLUDE_BLOCK_TAG.data());
                        }
                    }
                // whether to ignore proper nouns
                auto ignoreProperNouns =
                    documentAnalysisNode->FirstChildElement(XML_IGNORE_PROPER_NOUNS.data());
                if (ignoreProperNouns)
                    {
                    m_ignoreProperNouns = int_to_bool(ignoreProperNouns->ToElement()->IntAttribute(
                        XML_VALUE.data(), bool_to_int(m_ignoreProperNouns)));
                    }
                // whether to ignore numerals
                auto ignoreNumerals =
                    documentAnalysisNode->FirstChildElement(XML_IGNORE_NUMERALS.data());
                if (ignoreNumerals)
                    {
                    m_ignoreNumerals = int_to_bool(ignoreNumerals->ToElement()->IntAttribute(
                        XML_VALUE.data(), bool_to_int(m_ignoreNumerals)));
                    }
                // whether to ignore file address
                auto ignoreFileAddresses =
                    documentAnalysisNode->FirstChildElement(XML_IGNORE_FILE_ADDRESSES.data());
                if (ignoreFileAddresses)
                    {
                    m_ignoreFileAddresses =
                        int_to_bool(ignoreFileAddresses->ToElement()->IntAttribute(
                            XML_VALUE.data(), bool_to_int(m_ignoreFileAddresses)));
                    }
                // whether to ignore citations
                auto ignoreCitations =
                    documentAnalysisNode->FirstChildElement(XML_IGNORE_CITATIONS.data());
                if (ignoreCitations)
                    {
                    m_ignoreTrailingCitations =
                        int_to_bool(ignoreCitations->ToElement()->IntAttribute(
                            XML_VALUE.data(), bool_to_int(m_ignoreTrailingCitations)));
                    }
                // whether to aggressively exclude
                auto aggressivelyDeducingLists =
                    documentAnalysisNode->FirstChildElement(XML_AGGRESSIVE_EXCLUSION.data());
                if (aggressivelyDeducingLists)
                    {
                    m_aggressiveExclusion =
                        int_to_bool(aggressivelyDeducingLists->ToElement()->IntAttribute(
                            XML_VALUE.data(), bool_to_int(m_aggressiveExclusion)));
                    }
                // whether to ignore copyright notices
                auto ignoreCopyrightNotices =
                    documentAnalysisNode->FirstChildElement(XML_IGNORE_COPYRIGHT_NOTICES.data());
                if (ignoreCopyrightNotices)
                    {
                    m_ignoreTrailingCopyrightNoticeParagraphs =
                        int_to_bool(ignoreCopyrightNotices->ToElement()->IntAttribute(
                            XML_VALUE.data(),
                            bool_to_int(m_ignoreTrailingCopyrightNoticeParagraphs)));
                    }
                // determinant for how to text is parsed into paragraphs
                auto paraParsingNode =
                    documentAnalysisNode->FirstChildElement(XML_PARAGRAPH_PARSING_METHOD.data());
                if (paraParsingNode)
                    {
                    m_paragraphsParsingMethod =
                        static_cast<ParagraphParse>(paraParsingNode->ToElement()->IntAttribute(
                            XML_METHOD.data(), static_cast<int>(m_paragraphsParsingMethod)));
                    // verify that this is a sensical value
                    if (m_paragraphsParsingMethod !=
                            ParagraphParse::OnlySentenceTerminatedNewLinesAreParagraphs &&
                        m_paragraphsParsingMethod != ParagraphParse::EachNewLineIsAParagraph)
                        {
                        m_paragraphsParsingMethod =
                            ParagraphParse::OnlySentenceTerminatedNewLinesAreParagraphs;
                        }
                    }

                // determinant for how to handle headers, titles, bullet points, and lists
                auto invalidSentenceNode =
                    documentAnalysisNode->FirstChildElement(XML_INVALID_SENTENCE_METHOD.data());
                if (invalidSentenceNode)
                    {
                    m_invalidSentenceMethod =
                        static_cast<InvalidSentence>(invalidSentenceNode->ToElement()->IntAttribute(
                            XML_METHOD.data(), static_cast<int>(m_invalidSentenceMethod)));
                    if (static_cast<int>(m_invalidSentenceMethod) < 0 ||
                        static_cast<int>(m_invalidSentenceMethod) >=
                            static_cast<int>(InvalidSentence::INVALID_SENTENCE_METHOD_COUNT))
                        {
                        m_invalidSentenceMethod = InvalidSentence::ExcludeFromAnalysis;
                        }
                    }

                // Number of words that will make an incomplete sentence actually complete
                auto includeIncompleteSentencesIfLongerThanNode =
                    documentAnalysisNode->FirstChildElement(
                        XML_INCLUDE_INCOMPLETE_SENTENCES_LONGER_THAN.data());
                if (includeIncompleteSentencesIfLongerThanNode)
                    {
                    m_includeIncompleteSentencesIfLongerThan =
                        includeIncompleteSentencesIfLongerThanNode->ToElement()->IntAttribute(
                            XML_VALUE.data(), m_includeIncompleteSentencesIfLongerThan);
                    }
                }

            // test bundles
            auto testBundlesNode = projectSettings->FirstChildElement(XML_TEST_BUNDLES.data());
            if (testBundlesNode)
                {
                auto testBundleNode = testBundlesNode->FirstChildElement(XML_TEST_BUNDLE.data());
                while (testBundleNode)
                    {
                    // bundle name
                    auto bundleNameNode =
                        testBundleNode->FirstChildElement(XML_TEST_BUNDLE_NAME.data());
                    const wxString bundleName = TiXmlNodeToString(bundleNameNode, XML_VALUE.data());
                    if (bundleName.empty())
                        {
                        testBundleNode = testBundleNode->NextSiblingElement(XML_TEST_BUNDLE.data());
                        continue;
                        }
                    TestBundle bundle(bundleName.wc_str());
                    // bundle description
                    auto bundleDescriptionNode =
                        testBundleNode->FirstChildElement(XML_TEST_BUNDLE_DESCRIPTION.data());
                    if (bundleDescriptionNode)
                        {
                        bundle.SetDescription(
                            TiXmlNodeToString(bundleDescriptionNode, XML_VALUE.data()).wc_str());
                        }
                    // get the included tests
                    auto testNamesNode = testBundleNode->FirstChildElement(XML_TEST_NAMES.data());
                    if (!testNamesNode)
                        {
                        testBundleNode = testBundleNode->NextSiblingElement(XML_TEST_BUNDLE.data());
                        continue;
                        }
                    auto testNameNode = testNamesNode->FirstChildElement(XML_TEST_NAME.data());
                    while (testNameNode)
                        {
                        const wxString testName = TiXmlNodeToString(testNameNode, XML_VALUE.data());
                        if (testName.length())
                            {
                            const auto minGoal =
                                TiXmlNodeToDouble(testNameNode, XML_GOAL_MIN_VAL_GOAL.data());
                            const auto maxGoal =
                                TiXmlNodeToDouble(testNameNode, XML_GOAL_MAX_VAL_GOAL.data());
                            bundle.GetTestGoals().insert({ testName.wc_str(), minGoal, maxGoal });
                            }
                        testNameNode = testNameNode->NextSiblingElement(XML_TEST_NAME.data());
                        }
                    // get the included stats
                    auto statsNode =
                        testBundleNode->FirstChildElement(XML_BUNDLE_STATISTICS.data());
                    if (!statsNode)
                        {
                        testBundleNode = testBundleNode->NextSiblingElement(XML_TEST_BUNDLE.data());
                        continue;
                        }
                    auto statNode = statsNode->FirstChildElement(XML_BUNDLE_STATISTIC.data());
                    while (statNode)
                        {
                        const wxString statName = TiXmlNodeToString(statNode, XML_VALUE.data());
                        if (statName.length())
                            {
                            auto minGoal =
                                TiXmlNodeToDouble(statNode, XML_GOAL_MIN_VAL_GOAL.data());
                            auto maxGoal =
                                TiXmlNodeToDouble(statNode, XML_GOAL_MAX_VAL_GOAL.data());
                            bundle.GetStatGoals().insert({ statName.wc_str(), minGoal, maxGoal });
                            }
                        statNode = statNode->NextSiblingElement(XML_BUNDLE_STATISTIC.data());
                        }
                    BaseProject::m_testBundles.insert(bundle);
                    dynamic_cast<MainFrame*>(wxGetApp().GetMainFrame())
                        ->AddTestBundleToMenus(bundleName);

                    testBundleNode = testBundleNode->NextSiblingElement(XML_TEST_BUNDLE.data());
                    }
                }

            // custom tests
            auto customTestsNode = projectSettings->FirstChildElement(XML_CUSTOM_TESTS.data());
            if (customTestsNode)
                {
                auto customReadabilityTestNode =
                    customTestsNode->FirstChildElement(XML_CUSTOM_FAMILIAR_WORD_TEST.data());
                while (customReadabilityTestNode)
                    {
                    // test name
                    auto testNameNode =
                        customReadabilityTestNode->FirstChildElement(XML_TEST_NAME.data());
                    const char* testNameData =
                        testNameNode->ToElement()->Attribute(XML_VALUE.data());
                    if (!testNameData)
                        {
                        customReadabilityTestNode = customReadabilityTestNode->NextSiblingElement(
                            XML_CUSTOM_FAMILIAR_WORD_TEST.data());
                        continue;
                        }
                    auto testNameStr = Wisteria::TextStream::CharStreamToUnicode(
                        testNameData, std::strlen(testNameData));
                    const wchar_t* filteredText =
                        filter_html(testNameStr.c_str(), testNameStr.length(), true, false);
                    if (!filteredText)
                        {
                        customReadabilityTestNode = customReadabilityTestNode->NextSiblingElement(
                            XML_CUSTOM_FAMILIAR_WORD_TEST.data());
                        continue;
                        }
                    wxString testName(filteredText);
                    // file path
                    wxString filePath;
                    auto filePathNode = customReadabilityTestNode->FirstChildElement(
                        XML_FAMILIAR_WORD_FILE_PATH.data());
                    if (filePathNode)
                        {
                        const char* filePathData =
                            filePathNode->ToElement()->Attribute(XML_VALUE.data());
                        if (filePathData)
                            {
                            auto filePathStr = Wisteria::TextStream::CharStreamToUnicode(
                                filePathData, std::strlen(filePathData));
                            filteredText = filter_html(filePathStr.c_str(), filePathStr.length(),
                                                       true, false);
                            if (filteredText)
                                {
                                filePath = filteredText;
                                }
                            }
                        }
                    // test type
                    int testType = 0;
                    auto testTypeNode =
                        customReadabilityTestNode->FirstChildElement(XML_TEST_TYPE.data());
                    if (testTypeNode)
                        {
                        testType =
                            testTypeNode->ToElement()->IntAttribute(XML_VALUE.data(), testType);
                        }
                    if (testType < 0 ||
                        testType >=
                            static_cast<int>(readability::readability_test_type::TEST_TYPE_COUNT))
                        {
                        testType = 0;
                        }
                    // stemming type
                    int stemmingType = 0;
                    auto stemmingNode =
                        customReadabilityTestNode->FirstChildElement(XML_STEMMING_TYPE.data());
                    if (stemmingNode)
                        {
                        stemmingType = stemmingNode->ToElement()->IntAttribute(XML_VALUE.data(), 0);
                        }
                    if (stemmingType < 0 ||
                        stemmingType >=
                            static_cast<int>(stemming::stemming_type::STEMMING_TYPE_COUNT))
                        {
                        stemmingType = 0;
                        }
                    // formula type (for backward compatibility)
                    int formulaType = 0;
                    auto formulaNode =
                        customReadabilityTestNode->FirstChildElement(XML_TEST_FORMULA_TYPE.data());
                    if (formulaNode)
                        {
                        formulaType = formulaNode->ToElement()->IntAttribute(XML_VALUE.data(), 0);
                        }
                    if (formulaType != 0 && formulaType != 1)
                        {
                        formulaType = 0;
                        }
                    // the formula
                    wxString formula;
                    formulaNode =
                        customReadabilityTestNode->FirstChildElement(XML_TEST_FORMULA.data());
                    if (formulaNode)
                        {
                        const char* formulaData =
                            formulaNode->ToElement()->Attribute(XML_VALUE.data());
                        if (formulaData)
                            {
                            auto formulaStr = Wisteria::TextStream::CharStreamToUnicode(
                                formulaData, std::strlen(formulaData));
                            filteredText =
                                filter_html(formulaStr.c_str(), formulaStr.length(), true, false);
                            if (filteredText)
                                {
                                // need to format formula from U.S. format to current locale format
                                // for the parser to understand it
                                formula = FormulaFormat::FormatMathExpressionFromUS(filteredText);
                                }
                            }
                        }
                    string_util::remove_blank_lines(formula);
                    // formula string not in the file, so fall back to old formula type value
                    if (formula.empty())
                        {
                        if (formulaType == 1)
                            {
                            formula = ReadabilityFormulaParser::GetCustomSpacheSignature();
                            }
                        else
                            {
                            formula = ReadabilityFormulaParser::GetCustomNewDaleChallSignature();
                            }
                        }
                    // whether proper nouns and numbers should be included
                    int includeProperNouns = 1;
                    bool includeNumeric = false;
                    auto properNounNode = customReadabilityTestNode->FirstChildElement(
                        XML_INCLUDE_PROPER_NOUNS.data());
                    if (properNounNode)
                        {
                        includeProperNouns = properNounNode->ToElement()->IntAttribute(
                            XML_VALUE.data(), includeProperNouns);
                        }
                    auto numericNode =
                        customReadabilityTestNode->FirstChildElement(XML_INCLUDE_NUMERIC.data());
                    if (numericNode)
                        {
                        includeNumeric = int_to_bool(
                            numericNode->ToElement()->IntAttribute(XML_VALUE.data(), 0));
                        }
                    // whether DC and Spache lists should also be included with this test
                    bool includeCustomWordList = true /* best for backward compatibility */,
                         includeDCTest = false, includeSpacheTest = false, includeHJList = false,
                         includeStockerList = false, familiarWordsMustBeOnAllLists = false;
                    auto otherTestNode = customReadabilityTestNode->FirstChildElement(
                        XML_INCLUDE_CUSTOM_WORD_LIST.data());
                    if (otherTestNode)
                        {
                        includeCustomWordList = int_to_bool(
                            otherTestNode->ToElement()->IntAttribute(XML_VALUE.data(), 0));
                        }
                    otherTestNode =
                        customReadabilityTestNode->FirstChildElement(XML_INCLUDE_DC_LIST.data());
                    if (otherTestNode)
                        {
                        includeDCTest = int_to_bool(
                            otherTestNode->ToElement()->IntAttribute(XML_VALUE.data(), 0));
                        }
                    otherTestNode = customReadabilityTestNode->FirstChildElement(
                        XML_INCLUDE_SPACHE_LIST.data());
                    if (otherTestNode)
                        {
                        includeSpacheTest = int_to_bool(
                            otherTestNode->ToElement()->IntAttribute(XML_VALUE.data(), 0));
                        }
                    otherTestNode = customReadabilityTestNode->FirstChildElement(
                        XML_INCLUDE_HARRIS_JACOBSON_LIST.data());
                    if (otherTestNode)
                        {
                        includeHJList = int_to_bool(
                            otherTestNode->ToElement()->IntAttribute(XML_VALUE.data(), 0));
                        }
                    otherTestNode = customReadabilityTestNode->FirstChildElement(
                        XML_INCLUDE_STOCKER_LIST.data());
                    if (otherTestNode)
                        {
                        includeStockerList = int_to_bool(
                            otherTestNode->ToElement()->IntAttribute(XML_VALUE.data(), 0));
                        }
                    auto familiarWordsMustBeOnAllListsNode =
                        customReadabilityTestNode->FirstChildElement(
                            XML_FAMILIAR_WORDS_ALL_LISTS.data());
                    if (familiarWordsMustBeOnAllListsNode)
                        {
                        familiarWordsMustBeOnAllLists = int_to_bool(
                            familiarWordsMustBeOnAllListsNode->ToElement()->IntAttribute(
                                XML_VALUE.data(), 0));
                        }
                    // industry
                    bool industryChildrensPublishingSelected = false;
                    auto industryNode = customReadabilityTestNode->FirstChildElement(
                        XML_INDUSTRY_CHILDRENS_PUBLISHING.data());
                    if (industryNode)
                        {
                        industryChildrensPublishingSelected = int_to_bool(
                            industryNode->ToElement()->IntAttribute(XML_VALUE.data(), 0));
                        }
                    bool industryAdultPublishingSelected = false;
                    industryNode = customReadabilityTestNode->FirstChildElement(
                        XML_INDUSTRY_ADULTPUBLISHING.data());
                    if (industryNode)
                        {
                        industryAdultPublishingSelected = int_to_bool(
                            industryNode->ToElement()->IntAttribute(XML_VALUE.data(), 0));
                        }
                    bool industrySecondaryLanguageSelected = false;
                    industryNode = customReadabilityTestNode->FirstChildElement(
                        XML_INDUSTRY_SECONDARY_LANGUAGE.data());
                    if (industryNode)
                        {
                        industrySecondaryLanguageSelected = int_to_bool(
                            industryNode->ToElement()->IntAttribute(XML_VALUE.data(), 0));
                        }
                    bool industryChildrensHealthCareSelected = false;
                    industryNode = customReadabilityTestNode->FirstChildElement(
                        XML_INDUSTRY_CHILDRENS_HEALTHCARE.data());
                    if (industryNode)
                        {
                        industryChildrensHealthCareSelected = int_to_bool(
                            industryNode->ToElement()->IntAttribute(XML_VALUE.data(), 0));
                        }
                    bool industryAdultHealthCareSelected = false;
                    industryNode = customReadabilityTestNode->FirstChildElement(
                        XML_INDUSTRY_ADULT_HEALTHCARE.data());
                    if (industryNode)
                        {
                        industryAdultHealthCareSelected = int_to_bool(
                            industryNode->ToElement()->IntAttribute(XML_VALUE.data(), 0));
                        }
                    bool industryMilitaryGovernmentSelected = false;
                    industryNode = customReadabilityTestNode->FirstChildElement(
                        XML_INDUSTRY_MILITARY_GOVERNMENT.data());
                    if (industryNode)
                        {
                        industryMilitaryGovernmentSelected = int_to_bool(
                            industryNode->ToElement()->IntAttribute(XML_VALUE.data(), 0));
                        }
                    bool industryBroadcastingSelected = false;
                    industryNode = customReadabilityTestNode->FirstChildElement(
                        XML_INDUSTRY_BROADCASTING.data());
                    if (industryNode)
                        {
                        industryBroadcastingSelected =
                            int_to_bool(industryNode->ToElement()->IntAttribute(
                                XML_VALUE.data(), industryBroadcastingSelected));
                        }
                    // document
                    bool documentGeneralSelected = false;
                    auto documentNode =
                        customReadabilityTestNode->FirstChildElement(XML_DOCUMENT_GENERAL.data());
                    if (documentNode)
                        {
                        documentGeneralSelected =
                            int_to_bool(documentNode->ToElement()->IntAttribute(
                                XML_VALUE.data(), documentGeneralSelected));
                        }
                    bool documentTechSelected = false;
                    documentNode =
                        customReadabilityTestNode->FirstChildElement(XML_DOCUMENT_TECHNICAL.data());
                    if (documentNode)
                        {
                        documentTechSelected = int_to_bool(documentNode->ToElement()->IntAttribute(
                            XML_VALUE.data(), documentTechSelected));
                        }
                    bool documentFormSelected = false;
                    documentNode =
                        customReadabilityTestNode->FirstChildElement(XML_DOCUMENT_FORM.data());
                    if (documentNode)
                        {
                        documentFormSelected = int_to_bool(documentNode->ToElement()->IntAttribute(
                            XML_VALUE.data(), documentFormSelected));
                        }
                    bool documentYoungAdultSelected = false;
                    documentNode = customReadabilityTestNode->FirstChildElement(
                        XML_DOCUMENT_YOUNGADULT.data());
                    if (documentNode)
                        {
                        documentYoungAdultSelected =
                            int_to_bool(documentNode->ToElement()->IntAttribute(
                                XML_VALUE.data(), documentYoungAdultSelected));
                        }
                    bool documentChildrenSelected = false;
                    documentNode = customReadabilityTestNode->FirstChildElement(
                        XML_DOCUMENT_CHILDREN_LIT.data());
                    if (documentNode)
                        {
                        documentChildrenSelected =
                            int_to_bool(documentNode->ToElement()->IntAttribute(
                                XML_VALUE.data(), documentChildrenSelected));
                        }
                    CustomReadabilityTest cTest(
                        testName.wc_str(), formula.wc_str(),
                        static_cast<readability::readability_test_type>(testType),
                        filePath.wc_str(), static_cast<stemming::stemming_type>(stemmingType),
                        includeCustomWordList, includeDCTest, &BaseProject::m_dale_chall_word_list,
                        includeSpacheTest, &BaseProject::m_spache_word_list, includeHJList,
                        &BaseProject::m_harris_jacobson_word_list, includeStockerList,
                        &BaseProject::m_stocker_catholic_word_list, familiarWordsMustBeOnAllLists,
                        static_cast<readability::proper_noun_counting_method>(includeProperNouns),
                        includeNumeric, industryChildrensPublishingSelected,
                        industryAdultPublishingSelected, industrySecondaryLanguageSelected,
                        industryChildrensHealthCareSelected, industryAdultHealthCareSelected,
                        industryMilitaryGovernmentSelected, industryBroadcastingSelected,
                        documentGeneralSelected, documentTechSelected, documentFormSelected,
                        documentYoungAdultSelected, documentChildrenSelected);
                    BaseProjectDoc::AddGlobalCustomReadabilityTest(cTest);

                    customReadabilityTestNode = customReadabilityTestNode->NextSiblingElement(
                        XML_CUSTOM_FAMILIAR_WORD_TEST.data());
                    }
                }

            // graph options
            auto graphDefaultsNode = projectSettings->FirstChildElement(XML_GRAPH_SETTINGS.data());
            if (graphDefaultsNode)
                {
                // graph backgrounds
                auto imagePathNode = graphDefaultsNode->FirstChildElement(
                    XML_GRAPH_PLOT_BACKGROUND_IMAGE_PATH.data());
                if (imagePathNode)
                    {
                    const char* imagePath = imagePathNode->ToElement()->Attribute(XML_VALUE.data());
                    if (imagePath)
                        {
                        auto imagePathStr = Wisteria::TextStream::CharStreamToUnicode(
                            imagePath, std::strlen(imagePath));
                        const wchar_t* filteredText =
                            filter_html(imagePathStr.c_str(), imagePathStr.length(), true, false);
                        if (filteredText)
                            {
                            SetPlotBackGroundImagePath(wxString(filteredText));
                            }
                        }
                    }
                // color scheme
                auto colorSchemNode =
                    graphDefaultsNode->FirstChildElement(XML_GRAPH_COLOR_SCHEME.data());
                if (colorSchemNode)
                    {
                    const char* colorScheme =
                        colorSchemNode->ToElement()->Attribute(XML_VALUE.data());
                    if (colorScheme)
                        {
                        auto colorSchemeStr = Wisteria::TextStream::CharStreamToUnicode(
                            colorScheme, std::strlen(colorScheme));
                        const wchar_t* filteredText = filter_html(
                            colorSchemeStr.c_str(), colorSchemeStr.length(), true, false);
                        if (filteredText)
                            {
                            SetGraphColorScheme(wxString(filteredText));
                            }
                        }
                    }
                // graph background colors
                auto colorNode =
                    graphDefaultsNode->FirstChildElement(XML_GRAPH_BACKGROUND_COLOR.data());
                if (colorNode)
                    {
                    int red = colorNode->ToElement()->IntAttribute(XmlFormat::GetRed().mb_str(),
                                                                   GetBackGroundColor().Red());
                    int green = colorNode->ToElement()->IntAttribute(XmlFormat::GetGreen().mb_str(),
                                                                     GetBackGroundColor().Green());
                    int blue = colorNode->ToElement()->IntAttribute(XmlFormat::GetBlue().mb_str(),
                                                                    GetBackGroundColor().Blue());
                    SetBackGroundColor(wxColour(red, green, blue));
                    }
                auto backgroundImageEffectNode = graphDefaultsNode->FirstChildElement(
                    XML_GRAPH_PLOT_BACKGROUND_IMAGE_EFFECT.data());
                if (backgroundImageEffectNode)
                    {
                    int value = backgroundImageEffectNode->ToElement()->IntAttribute(
                        XML_VALUE.data(), static_cast<int>(GetPlotBackGroundImageEffect()));
                    if (value < 0 ||
                        value >= static_cast<decltype(value)>(ImageEffect::IMAGE_EFFECTS_COUNT))
                        {
                        value = static_cast<decltype(value)>(ImageEffect::NoEffect);
                        }
                    SetPlotBackGroundImageEffect(static_cast<ImageEffect>(value));
                    }
                auto backgroundImageFitNode = graphDefaultsNode->FirstChildElement(
                    XML_GRAPH_PLOT_BACKGROUND_IMAGE_FIT.data());
                if (backgroundImageFitNode)
                    {
                    int value = backgroundImageFitNode->ToElement()->IntAttribute(
                        XML_VALUE.data(), static_cast<int>(GetPlotBackGroundImageFit()));
                    if (value < 0 ||
                        value >= static_cast<decltype(value)>(ImageFit::IMAGE_FIT_COUNT))
                        {
                        value = static_cast<decltype(value)>(ImageFit::Shrink);
                        }
                    SetPlotBackGroundImageFit(static_cast<ImageFit>(value));
                    }
                colorNode =
                    graphDefaultsNode->FirstChildElement(XML_GRAPH_PLOT_BACKGROUND_COLOR.data());
                if (colorNode)
                    {
                    int red = colorNode->ToElement()->IntAttribute(XmlFormat::GetRed().mb_str(),
                                                                   GetPlotBackGroundColor().Red());
                    int green = colorNode->ToElement()->IntAttribute(
                        XmlFormat::GetGreen().mb_str(), GetPlotBackGroundColor().Green());
                    int blue = colorNode->ToElement()->IntAttribute(
                        XmlFormat::GetBlue().mb_str(), GetPlotBackGroundColor().Blue());
                    SetPlotBackGroundColor(wxColour(red, green, blue));
                    }
                auto opacityNode = graphDefaultsNode->FirstChildElement(
                    XML_GRAPH_PLOT_BACKGROUND_IMAGE_OPACITY.data());
                if (opacityNode)
                    {
                    SetPlotBackGroundImageOpacity(
                        static_cast<uint8_t>(opacityNode->ToElement()->IntAttribute(
                            XML_VALUE.data(), GetPlotBackGroundImageOpacity())));
                    }
                opacityNode = graphDefaultsNode->FirstChildElement(
                    XML_GRAPH_PLOT_BACKGROUND_COLOR_OPACITY.data());
                if (opacityNode)
                    {
                    SetPlotBackGroundColorOpacity(
                        static_cast<uint8_t>(opacityNode->ToElement()->IntAttribute(
                            XML_VALUE.data(), GetPlotBackGroundColorOpacity())));
                    }
                // linear gradient of backgrounds
                auto gradientNode = graphDefaultsNode->FirstChildElement(
                    XML_GRAPH_BACKGROUND_LINEAR_GRADIENT.data());
                if (gradientNode)
                    {
                    SetGraphBackGroundLinearGradient(
                        int_to_bool(gradientNode->ToElement()->IntAttribute(
                            XML_VALUE.data(), bool_to_int(GetGraphBackGroundLinearGradient()))));
                    }
                // stipple image
                auto stipplePathNode =
                    graphDefaultsNode->FirstChildElement(XML_GRAPH_STIPPLE_PATH.data());
                if (stipplePathNode)
                    {
                    const char* stipplePath =
                        stipplePathNode->ToElement()->Attribute(XML_VALUE.data());
                    if (stipplePath)
                        {
                        auto imagePathStr = Wisteria::TextStream::CharStreamToUnicode(
                            stipplePath, std::strlen(stipplePath));
                        const wchar_t* filteredText =
                            filter_html(imagePathStr.c_str(), imagePathStr.length(), true, false);
                        if (filteredText)
                            {
                            SetStippleImagePath(wxString(filteredText));
                            }
                        }
                    }
                // common image
                auto commonImagePathNode =
                    graphDefaultsNode->FirstChildElement(XML_GRAPH_COMMON_IMAGE_PATH.data());
                if (commonImagePathNode)
                    {
                    const char* commonImagePath =
                        commonImagePathNode->ToElement()->Attribute(XML_VALUE.data());
                    if (commonImagePath)
                        {
                        wxString imagePathStr = Wisteria::TextStream::CharStreamToUnicode(
                            commonImagePath, std::strlen(commonImagePath));
                        const wchar_t* filteredText =
                            filter_html(imagePathStr.wc_str(), imagePathStr.length(), true, false);
                        if (filteredText)
                            {
                            SetGraphCommonImagePath(wxString(filteredText));
                            }
                        }
                    }
                // stipple shape
                auto stippleShapeNode =
                    graphDefaultsNode->FirstChildElement(XML_GRAPH_STIPPLE_SHAPE.data());
                if (stippleShapeNode)
                    {
                    const char* stippleShape =
                        stippleShapeNode->ToElement()->Attribute(XML_VALUE.data());
                    if (stippleShape)
                        {
                        auto shapeStr = Wisteria::TextStream::CharStreamToUnicode(
                            stippleShape, std::strlen(stippleShape));
                        const wchar_t* filteredText =
                            filter_html(shapeStr.c_str(), shapeStr.length(), true, false);
                        if (filteredText)
                            {
                            SetStippleShape(wxString(filteredText));
                            }
                        }
                    }
                auto stippleColorNode =
                    graphDefaultsNode->FirstChildElement(XML_GRAPH_STIPPLE_COLOR.data());
                if (stippleColorNode)
                    {
                    int red = stippleColorNode->ToElement()->IntAttribute(
                        XmlFormat::GetRed().mb_str(), GetStippleShapeColor().Red());
                    int green = stippleColorNode->ToElement()->IntAttribute(
                        XmlFormat::GetGreen().mb_str(), GetStippleShapeColor().Green());
                    int blue = stippleColorNode->ToElement()->IntAttribute(
                        XmlFormat::GetBlue().mb_str(), GetStippleShapeColor().Blue());
                    SetStippleShapeColor(wxColour(red, green, blue));
                    }
                // whether drop shadows should be shown
                auto dropShadowNode =
                    graphDefaultsNode->FirstChildElement(XML_DISPLAY_DROP_SHADOW.data());
                if (dropShadowNode)
                    {
                    DisplayDropShadows(int_to_bool(dropShadowNode->ToElement()->IntAttribute(
                        XML_VALUE.data(), bool_to_int(IsDisplayingDropShadows()))));
                    }
                // whether to draw attention to the complex word groups in syllable graphs
                auto showcaseComplexWordsNode =
                    graphDefaultsNode->FirstChildElement(XML_SHOWCASE_COMPLEX_WORDS.data());
                if (showcaseComplexWordsNode)
                    {
                    ShowcaseComplexWords(
                        int_to_bool(showcaseComplexWordsNode->ToElement()->IntAttribute(
                            XML_VALUE.data(), bool_to_int(IsShowcasingComplexWords()))));
                    }
                // watermark
                auto watermarkNode =
                    graphDefaultsNode->FirstChildElement(XML_GRAPH_WATERMARK.data());
                if (watermarkNode)
                    {
                    const char* watermark = watermarkNode->ToElement()->Attribute(XML_VALUE.data());
                    if (watermark)
                        {
                        const auto waterMarkStr = Wisteria::TextStream::CharStreamToUnicode(
                            watermark, std::strlen(watermark));
                        const wchar_t* convertedStr =
                            filter_html(waterMarkStr.c_str(), waterMarkStr.length(), true, false);
                        if (convertedStr)
                            {
                            SetWatermark(convertedStr);
                            }
                        }
                    }
                watermarkNode = graphDefaultsNode->FirstChildElement(
                    XML_GRAPH_WATERMARK_LOGO_IMAGE_PATH.data());
                if (watermarkNode)
                    {
                    const char* watermark = watermarkNode->ToElement()->Attribute(XML_VALUE.data());
                    if (watermark)
                        {
                        const auto waterMarkStr = Wisteria::TextStream::CharStreamToUnicode(
                            watermark, std::strlen(watermark));
                        const wchar_t* convertedStr =
                            filter_html(waterMarkStr.c_str(), waterMarkStr.length(), true, false);
                        if (convertedStr)
                            {
                            SetWatermarkLogo(convertedStr);
                            }
                        }
                    }
                // histogram settings
                auto histogramNode =
                    graphDefaultsNode->FirstChildElement(XML_HISTOGRAM_SETTINGS.data());
                if (histogramNode)
                    {
                    auto catNode =
                        histogramNode->FirstChildElement(XML_GRAPH_BINNING_METHOD.data());
                    if (catNode)
                        {
                        int value = catNode->ToElement()->IntAttribute(
                            XML_VALUE.data(), static_cast<int>(GetHistorgramBinningMethod()));
                        if (value < 0 ||
                            value >= static_cast<decltype(value)>(
                                         Histogram::BinningMethod::BINNING_METHOD_COUNT))
                            {
                            value = static_cast<decltype(value)>(
                                Histogram::BinningMethod::BinByIntegerRange);
                            }
                        SetHistorgramBinningMethod(static_cast<Histogram::BinningMethod>(value));
                        }
                    auto roundNode =
                        histogramNode->FirstChildElement(XML_GRAPH_ROUNDING_METHOD.data());
                    if (roundNode)
                        {
                        int value = roundNode->ToElement()->IntAttribute(
                            XML_VALUE.data(),
                            static_cast<decltype(value)>(GetHistogramRoundingMethod()));
                        if (value < 0 || value >= static_cast<decltype(value)>(
                                                      RoundingMethod::ROUNDING_METHOD_COUNT))
                            {
                            value = static_cast<decltype(value)>(RoundingMethod::RoundDown);
                            }
                        SetHistogramRoundingMethod(static_cast<RoundingMethod>(value));
                        }
                    auto intervalNode =
                        histogramNode->FirstChildElement(XML_GRAPH_INTERVAL_DISPLAY.data());
                    if (intervalNode)
                        {
                        int value = intervalNode->ToElement()->IntAttribute(
                            XML_VALUE.data(),
                            static_cast<decltype(value)>(GetHistogramIntervalDisplay()));
                        if (value < 0 ||
                            value >= static_cast<decltype(value)>(
                                         Histogram::IntervalDisplay::INTERVAL_METHOD_COUNT))
                            {
                            value =
                                static_cast<decltype(value)>(Histogram::IntervalDisplay::Cutpoints);
                            }
                        SetHistogramIntervalDisplay(static_cast<Histogram::IntervalDisplay>(value));
                        }
                    auto catLabelNode =
                        histogramNode->FirstChildElement(XML_GRAPH_BINNING_LABEL_DISPLAY.data());
                    if (catLabelNode)
                        {
                        int value = catLabelNode->ToElement()->IntAttribute(
                            XML_VALUE.data(),
                            static_cast<decltype(value)>(GetHistrogramBinLabelDisplay()));
                        if (value < 0 || value >= static_cast<decltype(value)>(
                                                      BinLabelDisplay::BIN_LABEL_DISPLAY_COUNT))
                            {
                            value = static_cast<decltype(value)>(BinLabelDisplay::BinValue);
                            }
                        SetHistrogramBinLabelDisplay(static_cast<BinLabelDisplay>(value));
                        }
                    auto colorNodeHisto = histogramNode->FirstChildElement(XML_GRAPH_COLOR.data());
                    if (colorNodeHisto)
                        {
                        int red = colorNodeHisto->ToElement()->IntAttribute(
                            XmlFormat::GetRed().mb_str(), GetHistogramBarColor().Red());
                        int green = colorNodeHisto->ToElement()->IntAttribute(
                            XmlFormat::GetGreen().mb_str(), GetHistogramBarColor().Green());
                        int blue = colorNodeHisto->ToElement()->IntAttribute(
                            XmlFormat::GetBlue().mb_str(), GetHistogramBarColor().Blue());
                        SetHistogramBarColor(wxColour(red, green, blue));
                        }
                    auto opacityNodeHisto =
                        histogramNode->FirstChildElement(XML_GRAPH_OPACITY.data());
                    if (opacityNodeHisto)
                        {
                        SetHistogramBarOpacity(
                            static_cast<uint8_t>(opacityNodeHisto->ToElement()->IntAttribute(
                                XML_VALUE.data(), GetHistogramBarOpacity())));
                        }
                    auto barEffectNode = histogramNode->FirstChildElement(XML_BAR_EFFECT.data());
                    if (barEffectNode)
                        {
                        int value = barEffectNode->ToElement()->IntAttribute(
                            XML_VALUE.data(), static_cast<int>(GetHistogramBarEffect()));
                        if (value < 0 ||
                            value >= static_cast<decltype(value)>(BoxEffect::EFFECTS_COUNT))
                            {
                            value = static_cast<decltype(value)>(BoxEffect::Solid);
                            }
                        SetHistogramBarEffect(static_cast<BoxEffect>(value));
                        }
                    }
                // bar chart settings
                auto barChartNode =
                    graphDefaultsNode->FirstChildElement(XML_BAR_CHART_SETTINGS.data());
                if (barChartNode)
                    {
                    auto colorNodeBarChart =
                        barChartNode->FirstChildElement(XML_GRAPH_COLOR.data());
                    if (colorNodeBarChart)
                        {
                        int red = colorNodeBarChart->ToElement()->IntAttribute(
                            XmlFormat::GetRed().mb_str(), GetBarChartBarColor().Red());
                        int green = colorNodeBarChart->ToElement()->IntAttribute(
                            XmlFormat::GetGreen().mb_str(), GetBarChartBarColor().Green());
                        int blue = colorNodeBarChart->ToElement()->IntAttribute(
                            XmlFormat::GetBlue().mb_str(), GetBarChartBarColor().Blue());
                        SetBarChartBarColor(wxColour(red, green, blue));
                        }
                    auto orientationNode =
                        barChartNode->FirstChildElement(XML_BAR_ORIENTATION.data());
                    if (orientationNode)
                        {
                        int value = orientationNode->ToElement()->IntAttribute(
                            XML_VALUE.data(), static_cast<int>(GetBarChartOrientation()));
                        if (value < 0 || value >= static_cast<decltype(value)>(
                                                      Wisteria::Orientation::ORIENTATION_COUNT))
                            {
                            value = static_cast<decltype(value)>(Wisteria::Orientation::Vertical);
                            }
                        SetBarChartOrientation(static_cast<Wisteria::Orientation>(value));
                        }
                    auto opacityNodeBarChart =
                        barChartNode->FirstChildElement(XML_GRAPH_OPACITY.data());
                    if (opacityNodeBarChart)
                        {
                        SetGraphBarOpacity(
                            static_cast<uint8_t>(opacityNodeBarChart->ToElement()->IntAttribute(
                                XML_VALUE.data(), GetGraphBarOpacity())));
                        }
                    auto barEffectNode = barChartNode->FirstChildElement(XML_BAR_EFFECT.data());
                    if (barEffectNode)
                        {
                        int value = barEffectNode->ToElement()->IntAttribute(
                            XML_VALUE.data(), static_cast<int>(GetGraphBarEffect()));
                        if (value < 0 ||
                            value >= static_cast<decltype(value)>(BoxEffect::EFFECTS_COUNT))
                            {
                            value = static_cast<decltype(value)>(BoxEffect::Solid);
                            }
                        SetGraphBarEffect(static_cast<BoxEffect>(value));
                        }
                    auto barDisplayLabelNode =
                        barChartNode->FirstChildElement(XML_BAR_DISPLAY_LABELS.data());
                    if (barDisplayLabelNode)
                        {
                        DisplayBarChartLabels(
                            int_to_bool(barDisplayLabelNode->ToElement()->IntAttribute(
                                XML_VALUE.data(), bool_to_int(IsDisplayingBarChartLabels()))));
                        }
                    }
                // box plots settings
                auto boxPlotNode =
                    graphDefaultsNode->FirstChildElement(XML_BOX_PLOT_SETTINGS.data());
                if (boxPlotNode)
                    {
                    auto colorNodeBoxPlot = boxPlotNode->FirstChildElement(XML_GRAPH_COLOR.data());
                    if (colorNodeBoxPlot)
                        {
                        int red = colorNodeBoxPlot->ToElement()->IntAttribute(
                            XmlFormat::GetRed().mb_str(), GetGraphBoxColor().Red());
                        int green = colorNodeBoxPlot->ToElement()->IntAttribute(
                            XmlFormat::GetGreen().mb_str(), GetGraphBoxColor().Green());
                        int blue = colorNodeBoxPlot->ToElement()->IntAttribute(
                            XmlFormat::GetBlue().mb_str(), GetGraphBoxColor().Blue());
                        SetGraphBoxColor(wxColour(red, green, blue));
                        }
                    auto opacityNodeBoxPlot =
                        boxPlotNode->FirstChildElement(XML_GRAPH_OPACITY.data());
                    if (opacityNodeBoxPlot)
                        {
                        SetGraphBoxOpacity(
                            static_cast<uint8_t>(opacityNodeBoxPlot->ToElement()->IntAttribute(
                                XML_VALUE.data(), GetGraphBoxOpacity())));
                        }
                    auto boxEffectNode = boxPlotNode->FirstChildElement(XML_BOX_EFFECT.data());
                    if (boxEffectNode)
                        {
                        int value = boxEffectNode->ToElement()->IntAttribute(
                            XML_VALUE.data(), static_cast<int>(GetGraphBoxEffect()));
                        if (value < 0 ||
                            value >= static_cast<decltype(value)>(BoxEffect::EFFECTS_COUNT))
                            {
                            value = static_cast<decltype(value)>(BoxEffect::Solid);
                            }
                        SetGraphBoxEffect(static_cast<BoxEffect>(value));
                        }
                    auto boxPlotShowAllPointsNode =
                        boxPlotNode->FirstChildElement(XML_BOX_PLOT_SHOW_ALL_POINTS.data());
                    if (boxPlotShowAllPointsNode)
                        {
                        ShowAllBoxPlotPoints(
                            int_to_bool(boxPlotShowAllPointsNode->ToElement()->IntAttribute(
                                XML_VALUE.data(), bool_to_int(IsShowingAllBoxPlotPoints()))));
                        }
                    auto boxConnectMiddlePointsNode =
                        boxPlotNode->FirstChildElement(XML_BOX_CONNECT_MIDDLE_POINTS.data());
                    if (boxConnectMiddlePointsNode)
                        {
                        ConnectBoxPlotMiddlePoints(
                            int_to_bool(boxConnectMiddlePointsNode->ToElement()->IntAttribute(
                                XML_VALUE.data(), bool_to_int(IsConnectingBoxPlotMiddlePoints()))));
                        }
                    auto boxDisplayLabelNode =
                        boxPlotNode->FirstChildElement(XML_BOX_DISPLAY_LABELS.data());
                    if (boxDisplayLabelNode)
                        {
                        DisplayBoxPlotLabels(
                            int_to_bool(boxDisplayLabelNode->ToElement()->IntAttribute(
                                XML_VALUE.data(), bool_to_int(IsDisplayingBoxPlotLabels()))));
                        }
                    }
                // Readability graph options
                // Flesch chart
                auto fleschChartSettingsNode =
                    graphDefaultsNode->FirstChildElement(XML_FLESCH_CHART_SETTINGS.data());
                if (fleschChartSettingsNode)
                    {
                    auto connectionLine = fleschChartSettingsNode->FirstChildElement(
                        XML_INCLUDE_CONNECTION_LINE.data());
                    if (connectionLine)
                        {
                        ConnectFleschPoints(int_to_bool(connectionLine->ToElement()->IntAttribute(
                            XML_INCLUDE.data(), bool_to_int(IsConnectingFleschPoints()))));
                        }
                    auto rulerDocGroup = fleschChartSettingsNode->FirstChildElement(
                        XML_FLESCH_RULER_DOC_GROUPS.data());
                    if (rulerDocGroup)
                        {
                        IncludeFleschRulerDocGroups(
                            int_to_bool(rulerDocGroup->ToElement()->IntAttribute(
                                XML_VALUE.data(), bool_to_int(IsIncludingFleschRulerDocGroups()))));
                        }
                    }
                // Lix
                auto lixSettingsNode =
                    graphDefaultsNode->FirstChildElement(XML_LIX_SETTINGS.data());
                if (lixSettingsNode)
                    {
                    auto useEnglishLabels =
                        lixSettingsNode->FirstChildElement(XML_USE_ENGLISH_LABELS.data());
                    if (useEnglishLabels)
                        {
                        UseEnglishLabelsForGermanLix(
                            int_to_bool(useEnglishLabels->ToElement()->IntAttribute(
                                XML_INCLUDE.data(),
                                bool_to_int(IsUsingEnglishLabelsForGermanLix()))));
                        }
                    }
                // Fry/Raygor/GPM/Schwartz
                auto fryRaygorNode =
                    graphDefaultsNode->FirstChildElement(XML_FRY_RAYGOR_SETTINGS.data());
                if (fryRaygorNode)
                    {
                    // invalid area colors
                    auto colorNodeInvalidArea =
                        fryRaygorNode->FirstChildElement(XML_INVALID_AREA_COLOR.data());
                    if (colorNodeInvalidArea)
                        {
                        int red = colorNodeInvalidArea->ToElement()->IntAttribute(
                            XmlFormat::GetRed().mb_str(), GetInvalidAreaColor().Red());
                        int green = colorNodeInvalidArea->ToElement()->IntAttribute(
                            XmlFormat::GetGreen().mb_str(), GetInvalidAreaColor().Green());
                        int blue = colorNodeInvalidArea->ToElement()->IntAttribute(
                            XmlFormat::GetBlue().mb_str(), GetInvalidAreaColor().Blue());
                        SetInvalidAreaColor(wxColour(red, green, blue));
                        }
                    // Raygor appearance
                    auto raygorStyleNode =
                        fryRaygorNode->FirstChildElement(XML_RAYGOR_STYLE.data());
                    if (raygorStyleNode != nullptr)
                        {
                        int value = raygorStyleNode->ToElement()->IntAttribute(
                            XML_VALUE.data(), static_cast<int>(GetRaygorStyle()));
                        if (value < 0 ||
                            value >= static_cast<decltype(value)>(
                                         Wisteria::Graphs::RaygorStyle::RAYGOR_STYLE_COUNT))
                            {
                            value = static_cast<decltype(value)>(
                                Wisteria::Graphs::RaygorStyle::BaldwinKaufman);
                            }
                        SetRaygorStyle(static_cast<Wisteria::Graphs::RaygorStyle>(value));
                        }
                    }
                // axis options
                auto axisNode = graphDefaultsNode->FirstChildElement(XML_AXIS_SETTINGS.data());
                if (axisNode)
                    {
                    auto xAxisNode = axisNode->FirstChildElement(XML_X_AXIS.data());
                    if (xAxisNode)
                        {
                        // font color
                        auto colorNodeXAxis = xAxisNode->FirstChildElement(XML_FONT_COLOR.data());
                        if (colorNodeXAxis)
                            {
                            int red = colorNodeXAxis->ToElement()->IntAttribute(
                                XmlFormat::GetRed().mb_str(), GetXAxisFontColor().Red());
                            int green = colorNodeXAxis->ToElement()->IntAttribute(
                                XmlFormat::GetGreen().mb_str(), GetXAxisFontColor().Green());
                            int blue = colorNodeXAxis->ToElement()->IntAttribute(
                                XmlFormat::GetBlue().mb_str(), GetXAxisFontColor().Blue());
                            SetXAxisFontColor(wxColour(red, green, blue));
                            }
                        // font
                        auto fontNode = xAxisNode->FirstChildElement(XML_FONT.data());
                        if (fontNode)
                            {
                            int pointSize = fontNode->ToElement()->IntAttribute(
                                XmlFormat::GetFontPointSize().mb_str(),
                                wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize());
                            int style = fontNode->ToElement()->IntAttribute(
                                XmlFormat::GetFontStyle().mb_str(), wxFONTSTYLE_NORMAL);
                            int weight = fontNode->ToElement()->IntAttribute(
                                XmlFormat::GetFontWeight().mb_str(), wxFONTWEIGHT_NORMAL);
                            int underlined = fontNode->ToElement()->IntAttribute(
                                XmlFormat::GetFontUnderline().mb_str(), 0);
                            // get the font point size
                            m_xAxisFont.SetPointSize(
                                (pointSize > 0) ? pointSize :
                                                  wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT)
                                                      .GetPointSize());
                            // get the font style
                            m_xAxisFont.SetStyle(static_cast<wxFontStyle>(style));
                            // get the font weight
                            m_xAxisFont.SetWeight(static_cast<wxFontWeight>(weight));
                            // get the font underlining
                            m_xAxisFont.SetUnderlined(int_to_bool(underlined));
                            // get the font facename
                            const char* faceName = fontNode->ToElement()->Attribute(
                                XmlFormat::GetFontFaceName().mb_str());
                            if (faceName)
                                {
                                const auto faceNameStr = Wisteria::TextStream::CharStreamToUnicode(
                                    faceName, std::strlen(faceName));
                                const wchar_t* filteredText = filter_html(
                                    faceNameStr.c_str(), faceNameStr.length(), true, false);
                                if (filteredText && wxFontEnumerator::IsValidFacename(filteredText))
                                    {
                                    m_xAxisFont.SetFaceName(wxString(filteredText));
                                    }
                                }
                            }
                        }
                    auto yAxisNode = axisNode->FirstChildElement(XML_Y_AXIS.data());
                    if (yAxisNode)
                        {
                        // font color
                        auto colorNodeYAxis = yAxisNode->FirstChildElement(XML_FONT_COLOR.data());
                        if (colorNodeYAxis)
                            {
                            int red = colorNodeYAxis->ToElement()->IntAttribute(
                                XmlFormat::GetRed().mb_str(), GetYAxisFontColor().Red());
                            int green = colorNodeYAxis->ToElement()->IntAttribute(
                                XmlFormat::GetGreen().mb_str(), GetYAxisFontColor().Green());
                            int blue = colorNodeYAxis->ToElement()->IntAttribute(
                                XmlFormat::GetBlue().mb_str(), GetYAxisFontColor().Blue());
                            SetYAxisFontColor(wxColour(red, green, blue));
                            }
                        // font
                        auto fontNode = yAxisNode->FirstChildElement(XML_FONT.data());
                        if (fontNode)
                            {
                            int pointSize = fontNode->ToElement()->IntAttribute(
                                XmlFormat::GetFontPointSize().mb_str(),
                                wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize());
                            int style = fontNode->ToElement()->IntAttribute(
                                XmlFormat::GetFontStyle().mb_str(), wxFONTSTYLE_NORMAL);
                            int weight = fontNode->ToElement()->IntAttribute(
                                XmlFormat::GetFontWeight().mb_str(), wxFONTWEIGHT_NORMAL);
                            int underlined = fontNode->ToElement()->IntAttribute(
                                XmlFormat::GetFontUnderline().mb_str(), 0);
                            // get the font point size
                            m_yAxisFont.SetPointSize(
                                (pointSize > 0) ? pointSize :
                                                  wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT)
                                                      .GetPointSize());
                            // get the font style
                            m_yAxisFont.SetStyle(static_cast<wxFontStyle>(style));
                            // get the font weight
                            m_yAxisFont.SetWeight(static_cast<wxFontWeight>(weight));
                            // get the font underlining
                            m_yAxisFont.SetUnderlined(int_to_bool(underlined));
                            // get the font facename
                            const char* faceName = fontNode->ToElement()->Attribute(
                                XmlFormat::GetFontFaceName().mb_str());
                            if (faceName)
                                {
                                const auto faceNameStr =
                                    Wisteria::TextStream::CharStreamToUnicode(
                                        faceName, std::strlen(faceName));
                                const wchar_t* filteredText =
                                    filter_html(faceNameStr.c_str(), faceNameStr.length(), true, false);
                                if (filteredText && wxFontEnumerator::IsValidFacename(filteredText))
                                    {
                                    m_yAxisFont.SetFaceName(wxString(filteredText));
                                    }
                                }
                            }
                        }
                    }
                // title options
                auto titleNode = graphDefaultsNode->FirstChildElement(XML_TITLE_SETTINGS.data());
                if (titleNode)
                    {
                    auto topTitleNode = titleNode->FirstChildElement(XML_TOP_TITLE.data());
                    if (topTitleNode)
                        {
                        // font color
                        auto colorNodeTopTitle =
                            topTitleNode->FirstChildElement(XML_FONT_COLOR.data());
                        if (colorNodeTopTitle)
                            {
                            int red = colorNodeTopTitle->ToElement()->IntAttribute(
                                XmlFormat::GetRed().mb_str(), GetGraphTopTitleFontColor().Red());
                            int green = colorNodeTopTitle->ToElement()->IntAttribute(
                                XmlFormat::GetGreen().mb_str(),
                                GetGraphTopTitleFontColor().Green());
                            int blue = colorNodeTopTitle->ToElement()->IntAttribute(
                                XmlFormat::GetBlue().mb_str(), GetGraphTopTitleFontColor().Blue());
                            SetGraphTopTitleFontColor(wxColour(red, green, blue));
                            }
                        // font
                        auto fontNode = topTitleNode->FirstChildElement(XML_FONT.data());
                        if (fontNode)
                            {
                            int pointSize = fontNode->ToElement()->IntAttribute(
                                XmlFormat::GetFontPointSize().mb_str(),
                                wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize());
                            int style = fontNode->ToElement()->IntAttribute(
                                XmlFormat::GetFontStyle().mb_str(), wxFONTSTYLE_NORMAL);
                            int weight = fontNode->ToElement()->IntAttribute(
                                XmlFormat::GetFontWeight().mb_str(), wxFONTWEIGHT_NORMAL);
                            int underlined = fontNode->ToElement()->IntAttribute(
                                XmlFormat::GetFontUnderline().mb_str(), 0);
                            // get the font point size
                            m_topTitleFont.SetPointSize(
                                (pointSize > 0) ? pointSize :
                                                  wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT)
                                                      .GetPointSize());
                            // get the font style
                            m_topTitleFont.SetStyle(static_cast<wxFontStyle>(style));
                            // get the font weight
                            m_topTitleFont.SetWeight(static_cast<wxFontWeight>(weight));
                            // get the font underlining
                            m_topTitleFont.SetUnderlined(int_to_bool(underlined));
                            // get the font facename
                            const char* faceName = fontNode->ToElement()->Attribute(
                                XmlFormat::GetFontFaceName().mb_str());
                            if (faceName)
                                {
                                const auto faceNameStr =
                                    Wisteria::TextStream::CharStreamToUnicode(
                                        faceName, std::strlen(faceName));
                                const wchar_t* filteredText =
                                    filter_html(faceNameStr.c_str(), faceNameStr.length(), true, false);
                                if (filteredText && wxFontEnumerator::IsValidFacename(filteredText))
                                    {
                                    m_topTitleFont.SetFaceName(wxString(filteredText));
                                    }
                                }
                            }
                        }
                    auto bottomTitleNode = titleNode->FirstChildElement(XML_BOTTOM_TITLE.data());
                    if (bottomTitleNode)
                        {
                        // font color
                        auto colorNodeBottomTitle =
                            bottomTitleNode->FirstChildElement(XML_FONT_COLOR.data());
                        if (colorNodeBottomTitle)
                            {
                            int red = colorNodeBottomTitle->ToElement()->IntAttribute(
                                XmlFormat::GetRed().mb_str(), GetBottomTitleGraphFontColor().Red());
                            int green = colorNodeBottomTitle->ToElement()->IntAttribute(
                                XmlFormat::GetGreen().mb_str(),
                                GetBottomTitleGraphFontColor().Green());
                            int blue = colorNodeBottomTitle->ToElement()->IntAttribute(
                                XmlFormat::GetBlue().mb_str(),
                                GetBottomTitleGraphFontColor().Blue());
                            SetBottomTitleGraphFontColor(wxColour(red, green, blue));
                            }
                        // font
                        auto fontNode = bottomTitleNode->FirstChildElement(XML_FONT.data());
                        if (fontNode)
                            {
                            int pointSize = fontNode->ToElement()->IntAttribute(
                                XmlFormat::GetFontPointSize().mb_str(),
                                wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize());
                            int style = fontNode->ToElement()->IntAttribute(
                                XmlFormat::GetFontStyle().mb_str(), wxFONTSTYLE_NORMAL);
                            int weight = fontNode->ToElement()->IntAttribute(
                                XmlFormat::GetFontWeight().mb_str(), wxFONTWEIGHT_NORMAL);
                            int underlined = fontNode->ToElement()->IntAttribute(
                                XmlFormat::GetFontUnderline().mb_str(), 0);
                            // get the font point size
                            m_bottomTitleFont.SetPointSize(
                                (pointSize > 0) ? pointSize :
                                                  wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT)
                                                      .GetPointSize());
                            // get the font style
                            m_bottomTitleFont.SetStyle(static_cast<wxFontStyle>(style));
                            // get the font weight
                            m_bottomTitleFont.SetWeight(static_cast<wxFontWeight>(weight));
                            // get the font underlining
                            m_bottomTitleFont.SetUnderlined(int_to_bool(underlined));
                            // get the font facename
                            const char* faceName = fontNode->ToElement()->Attribute(
                                XmlFormat::GetFontFaceName().mb_str());
                            if (faceName)
                                {
                                const auto faceNameStr = Wisteria::TextStream::CharStreamToUnicode(
                                    faceName, std::strlen(faceName));
                                const wchar_t* filteredText = filter_html(
                                    faceNameStr.c_str(), faceNameStr.length(), true, false);
                                if (filteredText && wxFontEnumerator::IsValidFacename(filteredText))
                                    {
                                    m_bottomTitleFont.SetFaceName(wxString(filteredText));
                                    }
                                }
                            }
                        }
                    auto leftTitleNode = titleNode->FirstChildElement(XML_LEFT_TITLE.data());
                    if (leftTitleNode)
                        {
                        // font color
                        auto colorNodeLeftTitle =
                            leftTitleNode->FirstChildElement(XML_FONT_COLOR.data());
                        if (colorNodeLeftTitle)
                            {
                            int red = colorNodeLeftTitle->ToElement()->IntAttribute(
                                XmlFormat::GetRed().mb_str(), GetLeftTitleGraphFontColor().Red());
                            int green = colorNodeLeftTitle->ToElement()->IntAttribute(
                                XmlFormat::GetGreen().mb_str(),
                                GetLeftTitleGraphFontColor().Green());
                            int blue = colorNodeLeftTitle->ToElement()->IntAttribute(
                                XmlFormat::GetBlue().mb_str(), GetLeftTitleGraphFontColor().Blue());
                            SetLeftTitleGraphFontColor(wxColour(red, green, blue));
                            }
                        // font
                        auto fontNode = leftTitleNode->FirstChildElement(XML_FONT.data());
                        if (fontNode)
                            {
                            int pointSize = fontNode->ToElement()->IntAttribute(
                                XmlFormat::GetFontPointSize().mb_str(),
                                wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize());
                            int style = fontNode->ToElement()->IntAttribute(
                                XmlFormat::GetFontStyle().mb_str(), wxFONTSTYLE_NORMAL);
                            int weight = fontNode->ToElement()->IntAttribute(
                                XmlFormat::GetFontWeight().mb_str(), wxFONTWEIGHT_NORMAL);
                            int underlined = fontNode->ToElement()->IntAttribute(
                                XmlFormat::GetFontUnderline().mb_str(), 0);
                            // get the font point size
                            m_leftTitleFont.SetPointSize(
                                (pointSize > 0) ? pointSize :
                                                  wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT)
                                                      .GetPointSize());
                            // get the font style
                            m_leftTitleFont.SetStyle(static_cast<wxFontStyle>(style));
                            // get the font weight
                            m_leftTitleFont.SetWeight(static_cast<wxFontWeight>(weight));
                            // get the font underlining
                            m_leftTitleFont.SetUnderlined(int_to_bool(underlined));
                            // get the font facename
                            const char* faceName = fontNode->ToElement()->Attribute(
                                XmlFormat::GetFontFaceName().mb_str());
                            if (faceName)
                                {
                                const auto faceNameStr = Wisteria::TextStream::CharStreamToUnicode(
                                    faceName, std::strlen(faceName));
                                const wchar_t* filteredText = filter_html(
                                    faceNameStr.c_str(), faceNameStr.length(), true, false);
                                if (filteredText && wxFontEnumerator::IsValidFacename(filteredText))
                                    {
                                    m_leftTitleFont.SetFaceName(wxString(filteredText));
                                    }
                                }
                            }
                        }
                    auto rightTitleNode = titleNode->FirstChildElement(XML_RIGHT_TITLE.data());
                    if (rightTitleNode)
                        {
                        // font color
                        auto colorNodeRightTitle =
                            rightTitleNode->FirstChildElement(XML_FONT_COLOR.data());
                        if (colorNodeRightTitle)
                            {
                            int red = colorNodeRightTitle->ToElement()->IntAttribute(
                                XmlFormat::GetRed().mb_str(), GetRightTitleGraphFontColor().Red());
                            int green = colorNodeRightTitle->ToElement()->IntAttribute(
                                XmlFormat::GetGreen().mb_str(),
                                GetRightTitleGraphFontColor().Green());
                            int blue = colorNodeRightTitle->ToElement()->IntAttribute(
                                XmlFormat::GetBlue().mb_str(),
                                GetRightTitleGraphFontColor().Blue());
                            SetRightTitleGraphFontColor(wxColour(red, green, blue));
                            }
                        // font
                        auto fontNode = rightTitleNode->FirstChildElement(XML_FONT.data());
                        if (fontNode)
                            {
                            int pointSize = fontNode->ToElement()->IntAttribute(
                                XmlFormat::GetFontPointSize().mb_str(),
                                wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize());
                            int style = fontNode->ToElement()->IntAttribute(
                                XmlFormat::GetFontStyle().mb_str(), wxFONTSTYLE_NORMAL);
                            int weight = fontNode->ToElement()->IntAttribute(
                                XmlFormat::GetFontWeight().mb_str(), wxFONTWEIGHT_NORMAL);
                            int underlined = fontNode->ToElement()->IntAttribute(
                                XmlFormat::GetFontUnderline().mb_str(), 0);
                            // get the font point size
                            m_rightTitleFont.SetPointSize(
                                (pointSize > 0) ? pointSize :
                                                  wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT)
                                                      .GetPointSize());
                            // get the font style
                            m_rightTitleFont.SetStyle(static_cast<wxFontStyle>(style));
                            // get the font weight
                            m_rightTitleFont.SetWeight(static_cast<wxFontWeight>(weight));
                            // get the font underlining
                            m_rightTitleFont.SetUnderlined(int_to_bool(underlined));
                            // get the font facename
                            const char* faceName = fontNode->ToElement()->Attribute(
                                XmlFormat::GetFontFaceName().mb_str());
                            if (faceName)
                                {
                                const auto faceNameStr = Wisteria::TextStream::CharStreamToUnicode(
                                    faceName, std::strlen(faceName));
                                const wchar_t* filteredText = filter_html(
                                    faceNameStr.c_str(), faceNameStr.length(), true, false);
                                if (filteredText && wxFontEnumerator::IsValidFacename(filteredText))
                                    {
                                    m_rightTitleFont.SetFaceName(wxString(filteredText));
                                    }
                                }
                            }
                        }
                    }
                }
            // stats options
            auto statsDefaultsNode =
                projectSettings->FirstChildElement(XML_STATISTICS_SECTION.data());
            if (statsDefaultsNode)
                {
                // variance method
                auto varianceMethod =
                    statsDefaultsNode->FirstChildElement(XML_VARIANCE_METHOD.data());
                if (varianceMethod)
                    {
                    SetVarianceMethod(
                        static_cast<VarianceMethod>(varianceMethod->ToElement()->IntAttribute(
                            XML_METHOD.data(), static_cast<int>(GetVarianceMethod()))));
                    }
                // stats report
                auto statsNode =
                    statsDefaultsNode->FirstChildElement(XML_STATISTICS_RESULTS.data());
                if (statsNode)
                    {
                    const char* statsChars = statsNode->ToElement()->Attribute(XML_VALUE.data());
                    if (statsChars)
                        {
                        const auto statsStr = Wisteria::TextStream::CharStreamToUnicode(
                            statsChars, std::strlen(statsChars));
                        const wchar_t* convertedStr =
                            filter_html(statsStr.c_str(), statsStr.length(), true, false);
                        if (convertedStr)
                            {
                            GetStatisticsInfo().Set(convertedStr);
                            }
                        }
                    }
                // stats report
                auto statsReportNode =
                    statsDefaultsNode->FirstChildElement(XML_STATISTICS_REPORT.data());
                if (statsReportNode)
                    {
                    const char* statsReportChars =
                        statsReportNode->ToElement()->Attribute(XML_VALUE.data());
                    if (statsReportChars)
                        {
                        const auto statsReportStr = Wisteria::TextStream::CharStreamToUnicode(
                            statsReportChars, std::strlen(statsReportChars));
                        const wchar_t* convertedStr = filter_html(
                            statsReportStr.c_str(), statsReportStr.length(), true, false);
                        if (convertedStr)
                            {
                            GetStatisticsReportInfo().Set(convertedStr);
                            }
                        }
                    }
                }
            // wizard page defaults
            auto wizardPageDefaultsNode =
                projectSettings->FirstChildElement(XML_WIZARD_PAGES_SETTINGS.data());
            if (wizardPageDefaultsNode)
                {
                // batch grouping method
                auto batchGrouping =
                    wizardPageDefaultsNode->FirstChildElement(XML_WIZARD_BATCH_GROUP.data());
                if (batchGrouping != nullptr)
                    {
                    m_batchGroupDefault = batchGrouping->ToElement()->IntAttribute(
                            XML_METHOD.data(), m_batchGroupDefault);
                    }
                // Text Source
                auto textSource = wizardPageDefaultsNode->FirstChildElement(XML_TEXT_SOURCE.data());
                if (textSource)
                    {
                    m_textSource = static_cast<TextSource>(textSource->ToElement()->IntAttribute(
                        XML_METHOD.data(), static_cast<int>(m_textSource)));
                    if (m_textSource != TextSource::FromFile &&
                        m_textSource != TextSource::EnteredText)
                        {
                        m_textSource = TextSource::FromFile;
                        }
                    }
                // Test Recommendation
                auto testRecommendation =
                    wizardPageDefaultsNode->FirstChildElement(XML_TEST_RECOMMENDATION.data());
                if (testRecommendation)
                    {
                    m_testRecommendation = static_cast<TestRecommendation>(
                        testRecommendation->ToElement()->IntAttribute(
                            XML_METHOD.data(), static_cast<int>(m_testRecommendation)));
                    if (m_testRecommendation >= TestRecommendation::TEST_RECOMMENDATION_COUNT)
                        {
                        m_testRecommendation = TestRecommendation::BasedOnDocumentType;
                        }
                    }
                // Tests by Industry
                auto testsByIndustry =
                    wizardPageDefaultsNode->FirstChildElement(XML_TEST_BY_INDUSTRY.data());
                if (testsByIndustry)
                    {
                    m_testsByIndustry = static_cast<readability::industry_classification>(
                        testsByIndustry->ToElement()->IntAttribute(
                            XML_METHOD.data(), static_cast<int>(m_testsByIndustry)));
                    if (m_testsByIndustry ==
                        readability::industry_classification::INDUSTRY_CLASSIFICATION_COUNT)
                        {
                        m_testsByIndustry =
                            readability::industry_classification::adult_publishing_industry;
                        }
                    }
                // Tests By Document Type
                auto testsByDocumentType =
                    wizardPageDefaultsNode->FirstChildElement(XML_TEST_BY_DOCUMENT_TYPE.data());
                if (testsByDocumentType)
                    {
                    m_testsByDocumentType = static_cast<readability::document_classification>(
                        testsByDocumentType->ToElement()->IntAttribute(
                            XML_METHOD.data(), static_cast<int>(m_testsByDocumentType)));
                    if (m_testsByDocumentType ==
                        readability::document_classification::DOCUMENT_CLASSIFICATION_COUNT)
                        {
                        m_testsByDocumentType =
                            readability::document_classification::adult_literature_document;
                        }
                    }
                // tests by bundle
                auto bundleNode =
                    wizardPageDefaultsNode->FirstChildElement(XML_SELECTED_TEST_BUNDLE.data());
                if (bundleNode)
                    {
                    const char* bundleChars = bundleNode->ToElement()->Attribute(XML_VALUE.data());
                    if (bundleChars)
                        {
                        const auto bundleStr = Wisteria::TextStream::CharStreamToUnicode(
                            bundleChars, std::strlen(bundleChars));
                        const wchar_t* convertedStr =
                            filter_html(bundleStr.c_str(), bundleStr.length(), true, false);
                        if (convertedStr)
                            {
                            SetSelectedTestBundle(convertedStr);
                            }
                        }
                    }
                }

            // readability tests
            auto readabilityTestsNode =
                projectSettings->FirstChildElement(XML_READABILITY_TESTS_SECTION.data());
            if (readabilityTestsNode)
                {
                // grade scale display
                auto gradeScaleNode = readabilityTestsNode->FirstChildElement(
                    XML_READABILITY_TEST_GRADE_SCALE_DISPLAY.data());
                if (gradeScaleNode)
                    {
                    GetReadabilityMessageCatalog().SetGradeScale(
                        static_cast<readability::grade_scale>(
                            gradeScaleNode->ToElement()->IntAttribute(
                                XML_VALUE.data(),
                                static_cast<int>(GetReadabilityMessageCatalog().GetGradeScale()))));
                    }

                auto gradeScaleLongFormatNode = readabilityTestsNode->FirstChildElement(
                    XML_READABILITY_TEST_GRADE_SCALE_LONG_FORMAT.data());
                if (gradeScaleLongFormatNode)
                    {
                    GetReadabilityMessageCatalog().SetLongGradeScaleFormat(
                        int_to_bool(gradeScaleLongFormatNode->ToElement()->IntAttribute(
                            XML_VALUE.data(),
                            bool_to_int(
                                GetReadabilityMessageCatalog().IsUsingLongGradeScaleFormat()))));
                    }

                auto readingAgeFormatNode =
                    readabilityTestsNode->FirstChildElement(XML_READING_AGE_FORMAT.data());
                if (readingAgeFormatNode)
                    {
                    GetReadabilityMessageCatalog().SetReadingAgeDisplay(
                        static_cast<ReadabilityMessages::ReadingAgeDisplay>(
                            readingAgeFormatNode->ToElement()->IntAttribute(
                                XML_VALUE.data(),
                                static_cast<int>(
                                    GetReadabilityMessageCatalog().GetReadingAgeDisplay()))));
                    }

                auto includeScoreSummaryReportNode = readabilityTestsNode->FirstChildElement(
                    XML_INCLUDE_SCORES_SUMMARY_REPORT.data());
                if (includeScoreSummaryReportNode)
                    {
                    IncludeScoreSummaryReport(
                        int_to_bool(includeScoreSummaryReportNode->ToElement()->IntAttribute(
                            XML_VALUE.data(), bool_to_int(IsIncludingScoreSummaryReport()))));
                    }

                // test-specific options
                auto fleschKincaidOptionsNode =
                    readabilityTestsNode->FirstChildElement(XML_FLESCH_KINCAID_OPTIONS.data());
                if (fleschKincaidOptionsNode)
                    {
                    auto numeralMethod = fleschKincaidOptionsNode->FirstChildElement(
                        XML_NUMERAL_SYLLABICATION_METHOD.data());
                    if (numeralMethod)
                        {
                        SetFleschKincaidNumeralSyllabizeMethod(
                            static_cast<FleschKincaidNumeralSyllabize>(
                                numeralMethod->ToElement()->IntAttribute(
                                    XML_VALUE.data(),
                                    static_cast<int>(GetFleschKincaidNumeralSyllabizeMethod()))));
                        }
                    }

                auto fleschOptionsNode =
                    readabilityTestsNode->FirstChildElement(XML_FLESCH_OPTIONS.data());
                if (fleschOptionsNode)
                    {
                    auto numeralMethod = fleschOptionsNode->FirstChildElement(
                        XML_NUMERAL_SYLLABICATION_METHOD.data());
                    if (numeralMethod)
                        {
                        SetFleschNumeralSyllabizeMethod(static_cast<FleschNumeralSyllabize>(
                            numeralMethod->ToElement()->IntAttribute(
                                XML_VALUE.data(),
                                static_cast<int>(GetFleschNumeralSyllabizeMethod()))));
                        }
                    }

                auto fogOptionsNode =
                    readabilityTestsNode->FirstChildElement(XML_GUNNING_FOG_OPTIONS.data());
                if (fogOptionsNode)
                    {
                    auto useUnits =
                        fogOptionsNode->FirstChildElement(XML_USE_SENTENCE_UNITS.data());
                    if (useUnits)
                        {
                        SetFogUseSentenceUnits(int_to_bool(useUnits->ToElement()->IntAttribute(
                            XML_VALUE.data(), bool_to_int(FogUseSentenceUnits()))));
                        }
                    }

                auto hjOptionsNode =
                    readabilityTestsNode->FirstChildElement(XML_HARRIS_JACOBSON_OPTIONS.data());
                if (hjOptionsNode)
                    {
                    auto hjTextExclusion =
                        hjOptionsNode->FirstChildElement(XML_TEXT_EXCLUSION.data());
                    if (hjTextExclusion)
                        {
                        int value = hjTextExclusion->ToElement()->IntAttribute(
                            XML_VALUE.data(),
                            static_cast<int>(GetHarrisJacobsonTextExclusionMode()));
                        if (value < 0 ||
                            value >= static_cast<int>(SpecializedTestTextExclusion::
                                                          SPECIALIZED_TEST_TEXT_EXCLUSION_COUNT))
                            {
                            value = static_cast<int>(SpecializedTestTextExclusion::
                                                         ExcludeIncompleteSentencesExceptHeadings);
                            }
                        SetHarrisJacobsonTextExclusionMode(
                            static_cast<SpecializedTestTextExclusion>(value));
                        }
                    }

                auto dcOptionsNode =
                    readabilityTestsNode->FirstChildElement(XML_NEW_DALE_CHALL_OPTIONS.data());
                if (dcOptionsNode)
                    {
                    auto includeStockerListNode =
                        dcOptionsNode->FirstChildElement(XML_STOCKER_LIST.data());
                    if (includeStockerListNode)
                        {
                        IncludeStockerCatholicSupplement(
                            int_to_bool(includeStockerListNode->ToElement()->IntAttribute(
                                XML_VALUE.data(),
                                bool_to_int(IsIncludingStockerCatholicSupplement()))));
                        }
                    auto dcTextExclusion =
                        dcOptionsNode->FirstChildElement(XML_TEXT_EXCLUSION.data());
                    if (dcTextExclusion)
                        {
                        int value = dcTextExclusion->ToElement()->IntAttribute(
                            XML_VALUE.data(), static_cast<int>(GetDaleChallTextExclusionMode()));
                        if (value < 0 ||
                            value >= static_cast<int>(SpecializedTestTextExclusion::
                                                          SPECIALIZED_TEST_TEXT_EXCLUSION_COUNT))
                            {
                            value = static_cast<int>(SpecializedTestTextExclusion::
                                                         ExcludeIncompleteSentencesExceptHeadings);
                            }
                        SetDaleChallTextExclusionMode(
                            static_cast<SpecializedTestTextExclusion>(value));
                        }
                    auto dcProperNoun =
                        dcOptionsNode->FirstChildElement(XML_PROPER_NOUN_COUNTING_METHOD.data());
                    if (dcProperNoun)
                        {
                        int value = dcProperNoun->ToElement()->IntAttribute(
                            XML_VALUE.data(),
                            static_cast<int>(GetDaleChallProperNounCountingMethod()));
                        if (value < 0 ||
                            value >= static_cast<int>(readability::proper_noun_counting_method::
                                                          PROPERNOUNCOUNTINGMETHOD_COUNT))
                            {
                            value = static_cast<int>(
                                readability::proper_noun_counting_method::
                                    only_count_first_instance_of_proper_noun_as_unfamiliar);
                            }
                        SetDaleChallProperNounCountingMethod(
                            static_cast<readability::proper_noun_counting_method>(value));
                        }
                    }

                // read in the standard tests
                for (auto rTest = GetReadabilityTests().get_tests().begin();
                     rTest != GetReadabilityTests().get_tests().end(); ++rTest)
                    {
                    auto test = readabilityTestsNode->FirstChildElement(
                        wxString(rTest->get_test().get_id().c_str()).mb_str());
                    /* if attribute is not found then "includeValue" is set to zero for us,
                       so no need to check the return value here*/
                    if (test)
                        {
                        rTest->include(int_to_bool(test->ToElement()->IntAttribute(
                            XML_INCLUDE.data(), bool_to_int(rTest->is_included()))));
                        }
                    }

                auto test = readabilityTestsNode->FirstChildElement(XML_DOLCH_SUITE.data());
                if (test)
                    {
                    m_includeDolchSightWords = int_to_bool(test->ToElement()->IntAttribute(
                        XML_INCLUDE.data(), bool_to_int(m_includeDolchSightWords)));
                    }
                // custom test
                GetIncludedCustomTests().clear();
                test = readabilityTestsNode->FirstChildElement(XML_CUSTOM_TEST.data());
                while (test)
                    {
                    const char* testId = test->ToElement()->Attribute(XML_VALUE.data());
                    if (testId)
                        {
                        const auto testStr =
                            Wisteria::TextStream::CharStreamToUnicode(testId, std::strlen(testId));
                        const wchar_t* convertedStr =
                            filter_html(testStr.c_str(), testStr.length(), true, false);
                        if (convertedStr)
                            {
                            GetIncludedCustomTests().push_back(convertedStr);
                            }
                        }
                    test = test->NextSiblingElement(XML_CUSTOM_TEST.data());
                    }
                }
            // Text views
            auto textViewNode = projectSettings->FirstChildElement(XML_TEXT_VIEWS_SECTION.data());
            if (textViewNode)
                {
                // how highlighting is done
                auto highlightMethod = textViewNode->FirstChildElement(XML_HIGHLIGHT_METHOD.data());
                if (highlightMethod)
                    {
                    m_textHighlight =
                        static_cast<TextHighlight>(highlightMethod->ToElement()->IntAttribute(
                            XML_METHOD.data(), static_cast<int>(m_textHighlight)));
                    if (m_textHighlight != TextHighlight::HighlightBackground &&
                        m_textHighlight != TextHighlight::HighlightForeground)
                        {
                        m_textHighlight = TextHighlight::HighlightBackground;
                        }
                    }
                // the highlight color
                auto colorNode = textViewNode->FirstChildElement(XML_HIGHLIGHTCOLOR.data());
                if (colorNode)
                    {
                    int red = colorNode->ToElement()->IntAttribute(XmlFormat::GetRed().mb_str(),
                                                                   m_textHighlightColor.Red());
                    int green = colorNode->ToElement()->IntAttribute(XmlFormat::GetGreen().mb_str(),
                                                                     m_textHighlightColor.Green());
                    int blue = colorNode->ToElement()->IntAttribute(XmlFormat::GetBlue().mb_str(),
                                                                    m_textHighlightColor.Blue());
                    m_textHighlightColor.Set(red, green, blue);
                    }
                // the highlight color for dolch words
                auto dolchConjunctionsColorNode =
                    textViewNode->FirstChildElement(XML_DOLCH_CONJUNCTIONS_HIGHLIGHTCOLOR.data());
                if (dolchConjunctionsColorNode)
                    {
                    int red = dolchConjunctionsColorNode->ToElement()->IntAttribute(
                        XmlFormat::GetRed().mb_str(), GetDolchConjunctionsColor().Red());
                    int green = dolchConjunctionsColorNode->ToElement()->IntAttribute(
                        XmlFormat::GetGreen().mb_str(), GetDolchConjunctionsColor().Green());
                    int blue = dolchConjunctionsColorNode->ToElement()->IntAttribute(
                        XmlFormat::GetBlue().mb_str(), GetDolchConjunctionsColor().Blue());
                    SetDolchConjunctionsColor(wxColour(red, green, blue));
                    m_highlightDolchConjunctions =
                        int_to_bool(dolchConjunctionsColorNode->ToElement()->IntAttribute(
                            XML_INCLUDE.data(), bool_to_int(m_highlightDolchConjunctions)));
                    }
                auto dolchPrepositionsColorNode =
                    textViewNode->FirstChildElement(XML_DOLCH_PREPOSITIONS_HIGHLIGHTCOLOR.data());
                if (dolchPrepositionsColorNode)
                    {
                    int red = dolchPrepositionsColorNode->ToElement()->IntAttribute(
                        XmlFormat::GetRed().mb_str(), GetDolchPrepositionsColor().Red());
                    int green = dolchPrepositionsColorNode->ToElement()->IntAttribute(
                        XmlFormat::GetGreen().mb_str(), GetDolchPrepositionsColor().Green());
                    int blue = dolchPrepositionsColorNode->ToElement()->IntAttribute(
                        XmlFormat::GetBlue().mb_str(), GetDolchPrepositionsColor().Blue());
                    SetDolchPrepositionsColor(wxColour(red, green, blue));
                    m_highlightDolchPrepositions =
                        int_to_bool(dolchPrepositionsColorNode->ToElement()->IntAttribute(
                            XML_INCLUDE.data(), bool_to_int(m_highlightDolchPrepositions)));
                    }
                auto dolchPronounsColorNode =
                    textViewNode->FirstChildElement(XML_DOLCH_PRONOUNS_HIGHLIGHTCOLOR.data());
                if (dolchPronounsColorNode)
                    {
                    int red = dolchPronounsColorNode->ToElement()->IntAttribute(
                        XmlFormat::GetRed().mb_str(), GetDolchPronounsColor().Red());
                    int green = dolchPronounsColorNode->ToElement()->IntAttribute(
                        XmlFormat::GetGreen().mb_str(), GetDolchPronounsColor().Green());
                    int blue = dolchPronounsColorNode->ToElement()->IntAttribute(
                        XmlFormat::GetBlue().mb_str(), GetDolchPronounsColor().Blue());
                    SetDolchPronounsColor(wxColour(red, green, blue));
                    m_highlightDolchPronouns =
                        int_to_bool(dolchPronounsColorNode->ToElement()->IntAttribute(
                            XML_INCLUDE.data(), bool_to_int(m_highlightDolchPronouns)));
                    }
                auto dolchAdverbsColorNode =
                    textViewNode->FirstChildElement(XML_DOLCH_ADVERBS_HIGHLIGHTCOLOR.data());
                if (dolchAdverbsColorNode)
                    {
                    int red = dolchAdverbsColorNode->ToElement()->IntAttribute(
                        XmlFormat::GetRed().mb_str(), GetDolchAdverbsColor().Red());
                    int green = dolchAdverbsColorNode->ToElement()->IntAttribute(
                        XmlFormat::GetGreen().mb_str(), GetDolchAdverbsColor().Green());
                    int blue = dolchAdverbsColorNode->ToElement()->IntAttribute(
                        XmlFormat::GetBlue().mb_str(), GetDolchAdverbsColor().Blue());
                    SetDolchAdverbsColor(wxColour(red, green, blue));
                    m_highlightDolchAdverbs =
                        int_to_bool(dolchAdverbsColorNode->ToElement()->IntAttribute(
                            XML_INCLUDE.data(), bool_to_int(m_highlightDolchAdverbs)));
                    }
                auto dolchAdjectivesColorNode =
                    textViewNode->FirstChildElement(XML_DOLCH_ADJECTIVES_HIGHLIGHTCOLOR.data());
                if (dolchAdjectivesColorNode)
                    {
                    int red = dolchAdjectivesColorNode->ToElement()->IntAttribute(
                        XmlFormat::GetRed().mb_str(), GetDolchAdjectivesColor().Red());
                    int green = dolchAdjectivesColorNode->ToElement()->IntAttribute(
                        XmlFormat::GetGreen().mb_str(), GetDolchAdjectivesColor().Green());
                    int blue = dolchAdjectivesColorNode->ToElement()->IntAttribute(
                        XmlFormat::GetBlue().mb_str(), GetDolchAdjectivesColor().Blue());
                    SetDolchAdjectivesColor(wxColour(red, green, blue));
                    m_highlightDolchAdjectives =
                        int_to_bool(dolchAdjectivesColorNode->ToElement()->IntAttribute(
                            XML_INCLUDE.data(), bool_to_int(m_highlightDolchAdjectives)));
                    }
                auto dolchVerbColorNode =
                    textViewNode->FirstChildElement(XML_DOLCH_VERBS_HIGHLIGHTCOLOR.data());
                if (dolchVerbColorNode)
                    {
                    int red = dolchVerbColorNode->ToElement()->IntAttribute(
                        XmlFormat::GetRed().mb_str(), GetDolchVerbsColor().Red());
                    int green = dolchVerbColorNode->ToElement()->IntAttribute(
                        XmlFormat::GetGreen().mb_str(), GetDolchVerbsColor().Green());
                    int blue = dolchVerbColorNode->ToElement()->IntAttribute(
                        XmlFormat::GetBlue().mb_str(), GetDolchVerbsColor().Blue());
                    SetDolchVerbsColor(wxColour(red, green, blue));
                    m_highlightDolchVerbs =
                        int_to_bool(dolchVerbColorNode->ToElement()->IntAttribute(
                            XML_INCLUDE.data(), bool_to_int(m_highlightDolchVerbs)));
                    }
                auto dolchNounColorNode =
                    textViewNode->FirstChildElement(XML_DOLCH_NOUNS_HIGHLIGHTCOLOR.data());
                if (dolchNounColorNode)
                    {
                    int red = dolchNounColorNode->ToElement()->IntAttribute(
                        XmlFormat::GetRed().mb_str(), GetDolchNounColor().Red());
                    int green = dolchNounColorNode->ToElement()->IntAttribute(
                        XmlFormat::GetGreen().mb_str(), GetDolchNounColor().Green());
                    int blue = dolchNounColorNode->ToElement()->IntAttribute(
                        XmlFormat::GetBlue().mb_str(), GetDolchNounColor().Blue());
                    SetDolchNounColor(wxColour(red, green, blue));
                    m_highlightDolchNouns =
                        int_to_bool(dolchNounColorNode->ToElement()->IntAttribute(
                            XML_INCLUDE.data(), bool_to_int(m_highlightDolchNouns)));
                    }
                // the highlight color for wordy items
                auto wordyPhrasesColorNode =
                    textViewNode->FirstChildElement(XML_WORDY_PHRASE_HIGHLIGHTCOLOR.data());
                if (wordyPhrasesColorNode)
                    {
                    int red = wordyPhrasesColorNode->ToElement()->IntAttribute(
                        XmlFormat::GetRed().mb_str(), m_wordyPhraseHighlightColor.Red());
                    int green = wordyPhrasesColorNode->ToElement()->IntAttribute(
                        XmlFormat::GetGreen().mb_str(), m_wordyPhraseHighlightColor.Green());
                    int blue = wordyPhrasesColorNode->ToElement()->IntAttribute(
                        XmlFormat::GetBlue().mb_str(), m_wordyPhraseHighlightColor.Blue());
                    m_wordyPhraseHighlightColor.Set(red, green, blue);
                    }
                // the highlight color for repeated words
                auto dupWordColorNode =
                    textViewNode->FirstChildElement(XML_DUP_WORD_HIGHLIGHTCOLOR.data());
                if (dupWordColorNode)
                    {
                    int red = dupWordColorNode->ToElement()->IntAttribute(
                        XmlFormat::GetRed().mb_str(), m_duplicateWordHighlightColor.Red());
                    int green = dupWordColorNode->ToElement()->IntAttribute(
                        XmlFormat::GetGreen().mb_str(), m_duplicateWordHighlightColor.Green());
                    int blue = dupWordColorNode->ToElement()->IntAttribute(
                        XmlFormat::GetBlue().mb_str(), m_duplicateWordHighlightColor.Blue());
                    m_duplicateWordHighlightColor.Set(red, green, blue);
                    }
                // the highlight color for ignored sentences
                auto excludedColorNode =
                    textViewNode->FirstChildElement(XML_EXCLUDED_HIGHLIGHTCOLOR.data());
                if (excludedColorNode)
                    {
                    int red = excludedColorNode->ToElement()->IntAttribute(
                        XmlFormat::GetRed().mb_str(), m_excludedTextHighlightColor.Red());
                    int green = excludedColorNode->ToElement()->IntAttribute(
                        XmlFormat::GetGreen().mb_str(), m_excludedTextHighlightColor.Green());
                    int blue = excludedColorNode->ToElement()->IntAttribute(
                        XmlFormat::GetBlue().mb_str(), m_excludedTextHighlightColor.Blue());
                    m_excludedTextHighlightColor.Set(red, green, blue);
                    }
                // document display font information
                auto fontColorNode =
                    textViewNode->FirstChildElement(XML_DOCUMENT_DISPLAY_FONTCOLOR.data());
                if (fontColorNode)
                    {
                    int red = fontColorNode->ToElement()->IntAttribute(XmlFormat::GetRed().mb_str(),
                                                                       m_fontColor.Red());
                    int green = fontColorNode->ToElement()->IntAttribute(
                        XmlFormat::GetGreen().mb_str(), m_fontColor.Green());
                    int blue = fontColorNode->ToElement()->IntAttribute(
                        XmlFormat::GetBlue().mb_str(), m_fontColor.Blue());
                    m_fontColor.Set(red, green, blue);
                    }
                // font
                auto fontNode = textViewNode->FirstChildElement(XML_DOCUMENT_DISPLAY_FONT.data());
                if (fontNode)
                    {
                    int pointSize = fontNode->ToElement()->IntAttribute(
                        XmlFormat::GetFontPointSize().mb_str(),
                        wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize());
                    int style = fontNode->ToElement()->IntAttribute(
                        XmlFormat::GetFontStyle().mb_str(), wxFONTSTYLE_NORMAL);
                    int weight = fontNode->ToElement()->IntAttribute(
                        XmlFormat::GetFontWeight().mb_str(), wxFONTWEIGHT_NORMAL);
                    int underlined = fontNode->ToElement()->IntAttribute(
                        XmlFormat::GetFontUnderline().mb_str(), 0);
                    // get the font point size
                    m_textViewFont.SetPointSize(
                        (pointSize > 0) ?
                            pointSize :
                            wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize());
                    // get the font style
                    m_textViewFont.SetStyle(static_cast<wxFontStyle>(style));
                    // get the font weight
                    m_textViewFont.SetWeight(static_cast<wxFontWeight>(weight));
                    // get the font underlining
                    m_textViewFont.SetUnderlined(int_to_bool(underlined));
                    // get the font facename
                    const char* faceName =
                        fontNode->ToElement()->Attribute(XmlFormat::GetFontFaceName().mb_str());
                    if (faceName)
                        {
                        const auto faceNameStr = Wisteria::TextStream::CharStreamToUnicode(
                            faceName, std::strlen(faceName));
                        const wchar_t* filteredText =
                            filter_html(faceNameStr.c_str(), faceNameStr.length(), true, false);
                        if (filteredText && wxFontEnumerator::IsValidFacename(filteredText))
                            {
                            m_textViewFont.SetFaceName(wxString(filteredText));
                            }
                        }
                    }
                }
            }
        }

    // fix any issues with items loaded from this file
    Wisteria::GraphItems::Label::FixFont(m_editorFont);
    Wisteria::GraphItems::Label::FixFont(m_xAxisFont);
    Wisteria::GraphItems::Label::FixFont(m_yAxisFont);
    Wisteria::GraphItems::Label::FixFont(m_topTitleFont);
    Wisteria::GraphItems::Label::FixFont(m_bottomTitleFont);
    Wisteria::GraphItems::Label::FixFont(m_leftTitleFont);
    Wisteria::GraphItems::Label::FixFont(m_rightTitleFont);
    Wisteria::GraphItems::Label::FixFont(m_textViewFont);
    return true;
    }

//------------------------------------------------
bool ReadabilityAppOptions::SaveOptionsFile(const wxString& optionsFile /*= wxString{}*/)
    {
    assert(!m_optionsFile.empty());

    lily_of_the_valley::html_encode_text encode;

    tinyxml2::XMLDocument doc;
    doc.InsertEndChild(doc.NewDeclaration());

    auto root = doc.NewElement(XML_CONFIG_HEADER.data());
    root->SetAttribute(XML_VERSION.data(), wxGetApp().GetAppVersion().mb_str());

    auto configSection = doc.NewElement(XML_CONFIGURATIONS.data());

    auto customColours = doc.NewElement(XML_CUSTOM_COLORS.data());
    for (int i = 0; static_cast<size_t>(i) < GetCustomColours().size(); ++i)
        {
        auto customColor = doc.NewElement(wxString::Format(_DT(L"color%d"), i).mb_str());
        customColor->SetAttribute(XmlFormat::GetRed().mb_str(), GetCustomColours().at(i).Red());
        customColor->SetAttribute(XmlFormat::GetGreen().mb_str(), GetCustomColours().at(i).Green());
        customColor->SetAttribute(XmlFormat::GetBlue().mb_str(), GetCustomColours().at(i).Blue());
        customColours->InsertEndChild(customColor);
        }
    configSection->InsertEndChild(customColours);

    // general app information
    auto appearance = doc.NewElement(XML_APPEARANCE.data());
    appearance->SetAttribute(XML_WINDOW_MAXIMIZED.data(), bool_to_int(IsAppWindowMaximized()));
    appearance->SetAttribute(XML_WINDOW_WIDTH.data(), GetAppWindowWidth());
    appearance->SetAttribute(XML_WINDOW_HEIGHT.data(), GetAppWindowHeight());

    auto themeName = doc.NewElement(XML_THEME_NAME.data());
    const wxString themeNameEncoded = encode({ GetTheme().wc_str() }, false).c_str();
    themeName->SetAttribute(XML_VALUE.data(), themeNameEncoded.mb_str());
    appearance->InsertEndChild(themeName);

    auto controlBkColor = doc.NewElement(XML_CONTROL_BACKGROUND_COLOR.data());
    controlBkColor->SetAttribute(XmlFormat::GetRed().mb_str(), GetControlBackgroundColor().Red());
    controlBkColor->SetAttribute(XmlFormat::GetGreen().mb_str(),
                                 GetControlBackgroundColor().Green());
    controlBkColor->SetAttribute(XmlFormat::GetBlue().mb_str(), GetControlBackgroundColor().Blue());
    appearance->InsertEndChild(controlBkColor);

        // ribbon theming
        {
        auto activeTabColor = doc.NewElement(XML_RIBBON_ACTIVE_TAB_COLOR.data());
        activeTabColor->SetAttribute(XmlFormat::GetRed().mb_str(), GetRibbonActiveTabColor().Red());
        activeTabColor->SetAttribute(XmlFormat::GetGreen().mb_str(),
                                     GetRibbonActiveTabColor().Green());
        activeTabColor->SetAttribute(XmlFormat::GetBlue().mb_str(),
                                     GetRibbonActiveTabColor().Blue());
        appearance->InsertEndChild(activeTabColor);

        auto hoverColor = doc.NewElement(XML_RIBBON_HOVER_COLOR.data());
        hoverColor->SetAttribute(XmlFormat::GetRed().mb_str(), GetRibbonHoverColor().Red());
        hoverColor->SetAttribute(XmlFormat::GetGreen().mb_str(), GetRibbonHoverColor().Green());
        hoverColor->SetAttribute(XmlFormat::GetBlue().mb_str(), GetRibbonHoverColor().Blue());
        appearance->InsertEndChild(hoverColor);
        }
        // sidebar theming
        {
        auto sidebarBkColor = doc.NewElement(XML_SIDEBAR_BACKGROUND_COLOR.data());
        sidebarBkColor->SetAttribute(XmlFormat::GetRed().mb_str(),
                                     GetSideBarBackgroundColor().Red());
        sidebarBkColor->SetAttribute(XmlFormat::GetGreen().mb_str(),
                                     GetSideBarBackgroundColor().Green());
        sidebarBkColor->SetAttribute(XmlFormat::GetBlue().mb_str(),
                                     GetSideBarBackgroundColor().Blue());
        appearance->InsertEndChild(sidebarBkColor);

        auto sidebarActiveColor = doc.NewElement(XML_SIDEBAR_ACTIVE_COLOR.data());
        sidebarActiveColor->SetAttribute(XmlFormat::GetRed().mb_str(),
                                         GetSideBarActiveColor().Red());
        sidebarActiveColor->SetAttribute(XmlFormat::GetGreen().mb_str(),
                                         GetSideBarActiveColor().Green());
        sidebarActiveColor->SetAttribute(XmlFormat::GetBlue().mb_str(),
                                         GetSideBarActiveColor().Blue());
        appearance->InsertEndChild(sidebarActiveColor);

        auto sidebarParentColor = doc.NewElement(XML_SIDEBAR_PARENT_COLOR.data());
        sidebarParentColor->SetAttribute(XmlFormat::GetRed().mb_str(),
                                         GetSideBarParentColor().Red());
        sidebarParentColor->SetAttribute(XmlFormat::GetGreen().mb_str(),
                                         GetSideBarParentColor().Green());
        sidebarParentColor->SetAttribute(XmlFormat::GetBlue().mb_str(),
                                         GetSideBarParentColor().Blue());
        appearance->InsertEndChild(sidebarParentColor);
        }
    configSection->InsertEndChild(appearance);

    // just set this to true for backward compatibility
    auto licenseAccepted = doc.NewElement(XML_LICENSE_ACCEPTED.data());
    licenseAccepted->SetAttribute(XML_VALUE.data(), bool_to_int(true));
    configSection->InsertEndChild(licenseAccepted);

    auto userAgent = doc.NewElement(XML_USER_AGENT.data());
    userAgent->SetAttribute(XML_VALUE.data(),
                            wxString(encode({ GetUserAgent().wc_str() }, false).c_str()).mb_str());
    configSection->InsertEndChild(userAgent);

    auto downloadReplaceExistingNode = doc.NewElement(XML_DOWNLOAD_REPLACE_EXISTING.data());
    downloadReplaceExistingNode->SetAttribute(
        XML_VALUE.data(), bool_to_int(wxGetApp().GetWebHarvester().IsReplacingExistingFiles()));
    configSection->InsertEndChild(downloadReplaceExistingNode);

    auto downloadWebFolderStructureNode = doc.NewElement(XML_DOWNLOAD_KEEP_FOLDER_STRUCTURE.data());
    downloadWebFolderStructureNode->SetAttribute(
        XML_VALUE.data(),
        bool_to_int(wxGetApp().GetWebHarvester().IsKeepingWebPathWhenDownloading()));
    configSection->InsertEndChild(downloadWebFolderStructureNode);

    auto downloadMinSize = doc.NewElement(XML_DOWNLOAD_MIN_FILESIZE.data());
    downloadMinSize->SetAttribute(
        XML_VALUE.data(),
        wxGetApp().GetWebHarvester().GetMinimumDownloadFileSizeInKilobytes().value_or(5));
    configSection->InsertEndChild(downloadMinSize);

    auto jsCookies = doc.NewElement(XML_USE_JS_COOKIES.data());
    jsCookies->SetAttribute(XML_VALUE.data(), bool_to_int(IsUsingJavaScriptCookies()));
    configSection->InsertEndChild(jsCookies);

    auto persistCookies = doc.NewElement(XML_PERSIST_COOKIES.data());
    persistCookies->SetAttribute(XML_VALUE.data(), bool_to_int(IsPersistingJavaScriptCookies()));
    configSection->InsertEndChild(persistCookies);

    auto disablePv = doc.NewElement(XML_DISABLE_PEER_VERIFY.data());
    disablePv->SetAttribute(XML_VALUE.data(), bool_to_int(IsPeerVerifyDisabled()));
    configSection->InsertEndChild(disablePv);

    // last opened file locations
    auto filePaths = doc.NewElement(XML_FILE_OPEN_PATHS.data());
    // project path
    auto projectPath = doc.NewElement(XML_FILE_OPEN_PROJECT_PATH.data());
    projectPath->SetAttribute(
        XML_VALUE.data(), wxString(encode({ GetProjectPath().wc_str() }, false).c_str()).mb_str());
    filePaths->InsertEndChild(projectPath);
    // image path
    auto imagePath = doc.NewElement(XML_FILE_OPEN_IMAGE_PATH.data());
    imagePath->SetAttribute(XML_VALUE.data(),
                            wxString(encode({ GetImagePath().wc_str() }, false).c_str()).mb_str());
    filePaths->InsertEndChild(imagePath);
    // word list path
    auto wordlistPath = doc.NewElement(XML_FILE_OPEN_WORDLIST_PATH.data());
    wordlistPath->SetAttribute(
        XML_VALUE.data(), wxString(encode({ GetWordListPath().wc_str() }, false).c_str()).mb_str());
    filePaths->InsertEndChild(wordlistPath);

    configSection->InsertEndChild(filePaths);

    // warning settings
    auto warningSection = doc.NewElement(XML_WARNING_MESSAGE_SETTINGS.data());
    for (const auto& warning : WarningManager::GetWarnings())
        {
        auto warningItem = doc.NewElement(XML_WARNING_MESSAGE.data());
        warningItem->SetAttribute(XML_VALUE.data(), warning.GetId().mb_str());
        warningItem->SetAttribute(XML_DISPLAY.data(), bool_to_int(warning.ShouldBeShown()));
        warningItem->SetAttribute(XML_PREVIOUS_RESPONSE.data(), warning.GetPreviousResponse());
        warningSection->InsertEndChild(warningItem);
        }
    configSection->InsertEndChild(warningSection);

    // export settings
    auto exportSection = doc.NewElement(XML_EXPORT.data());
    // extensions
    auto listExt = doc.NewElement(XML_EXPORT_LIST_EXT.data());
    listExt->SetAttribute(XML_VALUE.data(), BaseProjectDoc::GetExportListExt().mb_str());
    exportSection->InsertEndChild(listExt);
    auto textExt = doc.NewElement(XML_EXPORT_TEXT_EXT.data());
    textExt->SetAttribute(XML_VALUE.data(), BaseProjectDoc::GetExportTextViewExt().mb_str());
    exportSection->InsertEndChild(textExt);
    auto graphExt = doc.NewElement(XML_EXPORT_GRAPH_EXT.data());
    graphExt->SetAttribute(XML_VALUE.data(), BaseProjectDoc::GetExportGraphExt().mb_str());
    exportSection->InsertEndChild(graphExt);
    // item inclusions
    auto includeLists = doc.NewElement(XML_EXPORT_LISTS.data());
    includeLists->SetAttribute(XML_VALUE.data(),
                               bool_to_int(BaseProjectDoc::IsExportingHardWordLists()));
    exportSection->InsertEndChild(includeLists);
    auto includeSentencesBreakdown = doc.NewElement(XML_EXPORT_SENTENCES_BREAKDOWN.data());
    includeSentencesBreakdown->SetAttribute(
        XML_VALUE.data(), bool_to_int(BaseProjectDoc::IsExportingSentencesBreakdown()));
    exportSection->InsertEndChild(includeSentencesBreakdown);
    auto includeGraphs = doc.NewElement(XML_EXPORT_GRAPHS.data());
    includeGraphs->SetAttribute(XML_VALUE.data(), bool_to_int(BaseProjectDoc::IsExportingGraphs()));
    exportSection->InsertEndChild(includeGraphs);
    auto includeTestResults = doc.NewElement(XML_EXPORT_TEST_RESULTS.data());
    includeTestResults->SetAttribute(XML_VALUE.data(),
                                     bool_to_int(BaseProjectDoc::IsExportingTestResults()));
    exportSection->InsertEndChild(includeTestResults);
    auto includeStats = doc.NewElement(XML_EXPORT_STATS.data());
    includeStats->SetAttribute(XML_VALUE.data(),
                               bool_to_int(BaseProjectDoc::IsExportingStatistics()));
    exportSection->InsertEndChild(includeStats);
    auto includeWordiness = doc.NewElement(XML_EXPORT_GRAMMAR.data());
    includeWordiness->SetAttribute(XML_VALUE.data(),
                                   bool_to_int(BaseProjectDoc::IsExportingWordiness()));
    exportSection->InsertEndChild(includeWordiness);
    auto includeSightWords = doc.NewElement(XML_EXPORT_DOLCH_WORDS.data());
    includeSightWords->SetAttribute(XML_VALUE.data(),
                                    bool_to_int(BaseProjectDoc::IsExportingSightWords()));
    exportSection->InsertEndChild(includeSightWords);
    auto includeWarnings = doc.NewElement(XML_EXPORT_WARNINGS.data());
    includeWarnings->SetAttribute(XML_VALUE.data(),
                                  bool_to_int(BaseProjectDoc::IsExportingWarnings()));
    exportSection->InsertEndChild(includeWarnings);

    configSection->InsertEndChild(exportSection);

    // log settings
    auto* logSection = doc.NewElement(XML_LOG_SETTINGS.data());
    auto* logVerbose = doc.NewElement(XML_LOG_VERBOSE.data());
    logVerbose->SetAttribute(XML_VALUE.data(), bool_to_int(wxGetApp().GetLogFile()->GetVerbose()));
    logSection->InsertEndChild(logVerbose);

    auto* logAppend = doc.NewElement(XML_LOG_APPEND_DAILY.data());
    logAppend->SetAttribute(XML_VALUE.data(), bool_to_int(IsAppendingDailyLog()));
    logSection->InsertEndChild(logAppend);

    configSection->InsertEndChild(logSection);

    // printer settings
    auto printerSection = doc.NewElement(XML_PRINTER_SETTINGS.data());
    // page setup
    auto paperId = doc.NewElement(XML_PRINTER_ID.data());
    paperId->SetAttribute(XML_VALUE.data(), GetPaperId());
    printerSection->InsertEndChild(paperId);
    auto paperOrientation = doc.NewElement(XML_PRINTER_ORIENTATION.data());
    paperOrientation->SetAttribute(XML_VALUE.data(), static_cast<int>(GetPaperOrientation()));
    printerSection->InsertEndChild(paperOrientation);
    // headers
    auto printerLeftHeader = doc.NewElement(XML_PRINTER_LEFT_HEADER.data());
    printerLeftHeader->SetAttribute(
        XML_VALUE.data(),
        wxString(encode({ GetLeftPrinterHeader().wc_str() }, false).c_str()).mb_str());
    printerSection->InsertEndChild(printerLeftHeader);
    auto printerCenterHeader = doc.NewElement(XML_PRINTER_CENTER_HEADER.data());
    printerCenterHeader->SetAttribute(
        XML_VALUE.data(),
        wxString(encode({ GetCenterPrinterHeader().wc_str() }, false).c_str()).mb_str());
    printerSection->InsertEndChild(printerCenterHeader);
    auto printerRightHeader = doc.NewElement(XML_PRINTER_RIGHT_HEADER.data());
    printerRightHeader->SetAttribute(
        XML_VALUE.data(),
        wxString(encode({ GetRightPrinterHeader().wc_str() }, false).c_str()).mb_str());
    printerSection->InsertEndChild(printerRightHeader);
    // footers
    auto printerLeftFooter = doc.NewElement(XML_PRINTER_LEFT_FOOTER.data());
    printerLeftFooter->SetAttribute(
        XML_VALUE.data(),
        wxString(encode({ GetLeftPrinterFooter().wc_str() }, false).c_str()).mb_str());
    printerSection->InsertEndChild(printerLeftFooter);
    auto printerCenterFooter = doc.NewElement(XML_PRINTER_CENTER_FOOTER.data());
    printerCenterFooter->SetAttribute(
        XML_VALUE.data(),
        wxString(encode({ GetCenterPrinterFooter().wc_str() }, false).c_str()).mb_str());
    printerSection->InsertEndChild(printerCenterFooter);
    auto printerRightFooter = doc.NewElement(XML_PRINTER_RIGHT_FOOTER.data());
    printerRightFooter->SetAttribute(
        XML_VALUE.data(),
        wxString(encode({ GetRightPrinterFooter().wc_str() }, false).c_str()).mb_str());
    printerSection->InsertEndChild(printerRightFooter);
    configSection->InsertEndChild(printerSection);

        // editor section
        {
        auto editorSection = doc.NewElement(XML_EDITOR.data());
        auto fontcolor = doc.NewElement(XML_EDITOR_FONTCOLOR.data());
        fontcolor->SetAttribute(XmlFormat::GetRed().mb_str(), m_editorFontColor.Red());
        fontcolor->SetAttribute(XmlFormat::GetGreen().mb_str(), m_editorFontColor.Green());
        fontcolor->SetAttribute(XmlFormat::GetBlue().mb_str(), m_editorFontColor.Blue());
        editorSection->InsertEndChild(fontcolor);

        auto font = doc.NewElement(XML_EDITOR_FONT.data());
        font->SetAttribute(XmlFormat::GetFontPointSize().mb_str(), m_editorFont.GetPointSize());
        font->SetAttribute(XmlFormat::GetFontStyle().mb_str(),
                           static_cast<int>(m_editorFont.GetStyle()));
        font->SetAttribute(XmlFormat::GetFontWeight().mb_str(),
                           static_cast<int>(m_editorFont.GetWeight()));
        font->SetAttribute(XmlFormat::GetFontUnderline().mb_str(),
                           bool_to_int(m_editorFont.GetUnderlined()));
        font->SetAttribute(
            XmlFormat::GetFontFaceName().mb_str(),
            wxString(encode({ m_editorFont.GetFaceName().wc_str() }, false).c_str()).mb_str());
        editorSection->InsertEndChild(font);

        auto indent = doc.NewElement(XML_EDITOR_INDENT.data());
        indent->SetAttribute(XML_VALUE.data(), bool_to_int(IsEditorIndenting()));
        editorSection->InsertEndChild(indent);

        auto spaceAfterParagraph = doc.NewElement(XML_EDITOR_SPACE_AFTER_PARAGRAPH.data());
        spaceAfterParagraph->SetAttribute(XML_VALUE.data(),
                                          bool_to_int(IsEditorShowSpaceAfterParagraph()));
        editorSection->InsertEndChild(spaceAfterParagraph);

        auto textAlignment = doc.NewElement(XML_EDITOR_TEXT_ALIGNMENT.data());
        textAlignment->SetAttribute(XML_VALUE.data(), static_cast<int>(GetEditorTextAlignment()));
        editorSection->InsertEndChild(textAlignment);

        auto lineSpacing = doc.NewElement(XML_EDITOR_LINE_SPACING.data());
        lineSpacing->SetAttribute(XML_VALUE.data(), static_cast<int>(GetEditorLineSpacing()));
        editorSection->InsertEndChild(lineSpacing);

        configSection->InsertEndChild(editorSection);
        }

    // New Project Settings
    auto projectSettings = doc.NewElement(XML_PROJECT_SETTINGS.data());

    // reviewer and status
    auto docReviewer = doc.NewElement(XML_REVIEWER.data());
    docReviewer->SetAttribute(XML_VALUE.data(),
                              wxString(encode({ GetReviewer().wc_str() }, false).c_str()).mb_str());
    projectSettings->InsertEndChild(docReviewer);

    auto realTimeRefresh = doc.NewElement(XML_REALTIME_UPDATE.data());
    realTimeRefresh->SetAttribute(XML_VALUE.data(), bool_to_int(IsRealTimeUpdating()));
    projectSettings->InsertEndChild(realTimeRefresh);

    // appended file
    auto appendedDocPath = doc.NewElement(XML_APPENDED_DOC_PATH.data());
    appendedDocPath->SetAttribute(
        XML_VALUE.data(),
        wxString(encode({ GetAppendedDocumentFilePath().wc_str() }, false).c_str()).mb_str());
    projectSettings->InsertEndChild(appendedDocPath);

    // document storage/linking
    auto docStorageMethod = doc.NewElement(XML_DOCUMENT_STORAGE_METHOD.data());
    docStorageMethod->SetAttribute(XML_METHOD.data(), static_cast<int>(m_documentStorageMethod));
    projectSettings->InsertEndChild(docStorageMethod);

    // Project language
    auto projectLang = doc.NewElement(XML_PROJECT_LANGUAGE.data());
    projectLang->SetAttribute(XML_VALUE.data(), static_cast<int>(GetProjectLanguage()));
    projectSettings->InsertEndChild(projectLang);

    // min doc size
    auto minDocSize = doc.NewElement(XML_MIN_DOC_SIZE_FOR_BATCH.data());
    minDocSize->SetAttribute(XML_VALUE.data(), static_cast<int>(GetMinDocWordCountForBatch()));
    projectSettings->InsertEndChild(minDocSize);

    // how file paths are shown in batch projects
    auto filePathTruncMode = doc.NewElement(XML_FILE_PATH_TRUNC_MODE.data());
    filePathTruncMode->SetAttribute(XML_VALUE.data(),
                                    static_cast<int>(GetFilePathTruncationMode()));
    projectSettings->InsertEndChild(filePathTruncMode);

    // random sampling size for batch projects
    auto randSampleSize = doc.NewElement(XML_RANDOM_SAMPLE_SIZE.data());
    randSampleSize->SetAttribute(XML_VALUE.data(), static_cast<int>(GetBatchRandomSamplingSize()));
    projectSettings->InsertEndChild(randSampleSize);

    // sentences breakdown
    auto sentencesBreakdownSection = doc.NewElement(XML_SENTENCES_BREAKDOWN.data());
    // which features are included
    auto sentencesBreakdownInfo = doc.NewElement(XML_SENTENCES_BREAKDOWN_INFO.data());
    sentencesBreakdownInfo->SetAttribute(XML_VALUE.data(),
                                         GetSentencesBreakdownInfo().ToString().mb_str());
    sentencesBreakdownSection->InsertEndChild(sentencesBreakdownInfo);
    projectSettings->InsertEndChild(sentencesBreakdownSection);

    // words breakdown
    auto wordsBreakdownSection = doc.NewElement(XML_WORDS_BREAKDOWN.data());
    // which features are included
    auto wordsBreakdownInfo = doc.NewElement(XML_WORDS_BREAKDOWN_INFO.data());
    wordsBreakdownInfo->SetAttribute(XML_VALUE.data(), GetWordsBreakdownInfo().ToString().mb_str());
    wordsBreakdownSection->InsertEndChild(wordsBreakdownInfo);
    projectSettings->InsertEndChild(wordsBreakdownSection);

    // grammar settings
    auto grammarSection = doc.NewElement(XML_GRAMMAR.data());

    auto spellCheckIgnoreProperNounsMethod =
        doc.NewElement(XML_SPELLCHECK_IGNORE_PROPER_NOUNS.data());
    spellCheckIgnoreProperNounsMethod->SetAttribute(XML_VALUE.data(),
                                                    bool_to_int(SpellCheckIsIgnoringProperNouns()));
    grammarSection->InsertEndChild(spellCheckIgnoreProperNounsMethod);

    auto spellCheckIgnoreUppercasedMethod = doc.NewElement(XML_SPELLCHECK_IGNORE_UPPERCASED.data());
    spellCheckIgnoreUppercasedMethod->SetAttribute(XML_VALUE.data(),
                                                   bool_to_int(SpellCheckIsIgnoringUppercased()));
    grammarSection->InsertEndChild(spellCheckIgnoreUppercasedMethod);

    auto spellCheckIgnoreNumeralsMethod = doc.NewElement(XML_SPELLCHECK_IGNORE_NUMERALS.data());
    spellCheckIgnoreNumeralsMethod->SetAttribute(XML_VALUE.data(),
                                                 bool_to_int(SpellCheckIsIgnoringNumerals()));
    grammarSection->InsertEndChild(spellCheckIgnoreNumeralsMethod);

    auto spellCheckIgnoreFileAddressMethod =
        doc.NewElement(XML_SPELLCHECK_IGNORE_FILE_ADDRESSES.data());
    spellCheckIgnoreFileAddressMethod->SetAttribute(
        XML_VALUE.data(), bool_to_int(SpellCheckIsIgnoringFileAddresses()));
    grammarSection->InsertEndChild(spellCheckIgnoreFileAddressMethod);

    auto spellCheckIgnoreProgrammerCodeMethod =
        doc.NewElement(XML_SPELLCHECK_IGNORE_PROGRAMMER_CODE.data());
    spellCheckIgnoreProgrammerCodeMethod->SetAttribute(
        XML_VALUE.data(), bool_to_int(SpellCheckIsIgnoringProgrammerCode()));
    grammarSection->InsertEndChild(spellCheckIgnoreProgrammerCodeMethod);

    auto spellCheckAllowColloquialismsMethod =
        doc.NewElement(XML_SPELLCHECK_ALLOW_COLLOQUIALISMS.data());
    spellCheckAllowColloquialismsMethod->SetAttribute(
        XML_VALUE.data(), bool_to_int(SpellCheckIsAllowingColloquialisms()));
    grammarSection->InsertEndChild(spellCheckAllowColloquialismsMethod);

    auto spellCheckIgnoreSocialMediaTagsMethod =
        doc.NewElement(XML_SPELLCHECK_IGNORE_SOCIAL_MEDIA_TAGS.data());
    spellCheckIgnoreSocialMediaTagsMethod->SetAttribute(
        XML_VALUE.data(), bool_to_int(SpellCheckIsIgnoringSocialMediaTags()));
    grammarSection->InsertEndChild(spellCheckIgnoreSocialMediaTagsMethod);

    // which grammar features are included
    auto grammarInfo = doc.NewElement(XML_GRAMMAR_INFO.data());
    grammarInfo->SetAttribute(XML_VALUE.data(), GetGrammarInfo().ToString().mb_str());
    grammarSection->InsertEndChild(grammarInfo);

    projectSettings->InsertEndChild(grammarSection);

    // document analysis settings
    auto documentAnalysisSection = doc.NewElement(XML_DOCUMENT_ANALYSIS_LOGIC.data());

    // sentence information
    auto sentenceMethod = doc.NewElement(XML_LONG_SENTENCE_METHOD.data());
    sentenceMethod->SetAttribute(XML_METHOD.data(), static_cast<int>(GetLongSentenceMethod()));
    documentAnalysisSection->InsertEndChild(sentenceMethod);

    auto sentenceLength = doc.NewElement(XML_LONG_SENTENCE_LENGTH.data());
    sentenceLength->SetAttribute(XML_VALUE.data(), GetDifficultSentenceLength());
    documentAnalysisSection->InsertEndChild(sentenceLength);

    // numeral syllabication
    auto numSyllMethod = doc.NewElement(XML_NUMERAL_SYLLABICATION_METHOD.data());
    numSyllMethod->SetAttribute(XML_METHOD.data(), static_cast<int>(m_numeralSyllabicationMethod));
    documentAnalysisSection->InsertEndChild(numSyllMethod);

    // whether to ignore blank lines when figuring out if an
    // incomplete sentences is end of paragraph
    auto ingnoreBlankLinesMethod =
        doc.NewElement(XML_IGNORE_BLANK_LINES_FOR_PARAGRAPH_PARSING.data());
    ingnoreBlankLinesMethod->SetAttribute(XML_VALUE.data(),
                                          bool_to_int(m_ignoreBlankLinesForParagraphsParser));
    documentAnalysisSection->InsertEndChild(ingnoreBlankLinesMethod);

    // whether we should ignore indenting when parsing paragraphs
    auto ingnoreIndentsMethod = doc.NewElement(XML_IGNORE_INDENTING_FOR_PARAGRAPH_PARSING.data());
    ingnoreIndentsMethod->SetAttribute(XML_VALUE.data(),
                                       bool_to_int(m_ignoreIndentingForParagraphsParser));
    documentAnalysisSection->InsertEndChild(ingnoreIndentsMethod);

    // whether sentences must start capitalized
    auto sentenceStartMustBeUppercased =
        doc.NewElement(XML_SENTENCES_MUST_START_CAPITALIZED.data());
    sentenceStartMustBeUppercased->SetAttribute(XML_VALUE.data(),
                                                bool_to_int(GetSentenceStartMustBeUppercased()));
    documentAnalysisSection->InsertEndChild(sentenceStartMustBeUppercased);

    // file path to phrases to exclude from analysis
    auto excludedPhrasesFilePath = doc.NewElement(XML_EXCLUDED_PHRASES_PATH.data());
    excludedPhrasesFilePath->SetAttribute(
        XML_VALUE.data(),
        wxString(encode({ GetExcludedPhrasesPath().wc_str() }, false).c_str()).mb_str());
    documentAnalysisSection->InsertEndChild(excludedPhrasesFilePath);

    // exclusion block tags
    auto excludeTagsSection = doc.NewElement(XML_EXCLUDE_BLOCK_TAGS.data());
    for (std::vector<std::pair<wchar_t, wchar_t>>::const_iterator currentExcludeTag =
             m_exclusionBlockTags.begin();
         currentExcludeTag != m_exclusionBlockTags.end(); ++currentExcludeTag)
        {
        auto excludeTags = doc.NewElement(XML_EXCLUDE_BLOCK_TAG.data());
        const wchar_t excludeTagsStr[3] = { currentExcludeTag->first, currentExcludeTag->second,
                                            0 };
        wxString excludeTagsEncoded = encode({ excludeTagsStr, 2 }, false).c_str();
        excludeTags->SetAttribute(XML_VALUE.data(), excludeTagsEncoded.mb_str());
        excludeTagsSection->InsertEndChild(excludeTags);
        }
    documentAnalysisSection->InsertEndChild(excludeTagsSection);

    // whether to include first occurrence of excluded phrases
    auto includeExcludedPhraseFirstOccurrence =
        doc.NewElement(XML_EXCLUDED_PHRASES_INCLUDE_FIRST_OCCURRENCE.data());
    includeExcludedPhraseFirstOccurrence->SetAttribute(
        XML_VALUE.data(), bool_to_int(IsIncludingExcludedPhraseFirstOccurrence()));
    documentAnalysisSection->InsertEndChild(includeExcludedPhraseFirstOccurrence);

    // whether to ignore Proper Nouns
    auto ignoreProperNouns = doc.NewElement(XML_IGNORE_PROPER_NOUNS.data());
    ignoreProperNouns->SetAttribute(XML_VALUE.data(), bool_to_int(IsIgnoringProperNouns()));
    documentAnalysisSection->InsertEndChild(ignoreProperNouns);

    // whether to ignore numerals
    auto ignoreNumerals = doc.NewElement(XML_IGNORE_NUMERALS.data());
    ignoreNumerals->SetAttribute(XML_VALUE.data(), bool_to_int(IsIgnoringNumerals()));
    documentAnalysisSection->InsertEndChild(ignoreNumerals);

    // whether to ignore file addresses
    auto ignoreFileAddress = doc.NewElement(XML_IGNORE_FILE_ADDRESSES.data());
    ignoreFileAddress->SetAttribute(XML_VALUE.data(), bool_to_int(IsIgnoringFileAddresses()));
    documentAnalysisSection->InsertEndChild(ignoreFileAddress);

    // whether to ignore citations
    auto ignoreCitations = doc.NewElement(XML_IGNORE_CITATIONS.data());
    ignoreCitations->SetAttribute(XML_VALUE.data(), bool_to_int(IsIgnoringTrailingCitations()));
    documentAnalysisSection->InsertEndChild(ignoreCitations);

    // whether to aggressively exclude
    auto aggressiveExclusion = doc.NewElement(XML_AGGRESSIVE_EXCLUSION.data());
    aggressiveExclusion->SetAttribute(XML_VALUE.data(), bool_to_int(IsExcludingAggressively()));
    documentAnalysisSection->InsertEndChild(aggressiveExclusion);

    // whether to ignore copyright notices
    auto ignoreCopyrightNotices = doc.NewElement(XML_IGNORE_COPYRIGHT_NOTICES.data());
    ignoreCopyrightNotices->SetAttribute(
        XML_VALUE.data(), bool_to_int(IsIgnoringTrailingCopyrightNoticeParagraphs()));
    documentAnalysisSection->InsertEndChild(ignoreCopyrightNotices);

    // paragraph parsing
    auto paraParsingMethod = doc.NewElement(XML_PARAGRAPH_PARSING_METHOD.data());
    paraParsingMethod->SetAttribute(XML_METHOD.data(), static_cast<int>(m_paragraphsParsingMethod));
    documentAnalysisSection->InsertEndChild(paraParsingMethod);

    // Number of words that will make an incomplete sentence actually complete
    auto includeIncompleteSentencesIfLongerThanValue =
        doc.NewElement(XML_INCLUDE_INCOMPLETE_SENTENCES_LONGER_THAN.data());
    includeIncompleteSentencesIfLongerThanValue->SetAttribute(
        XML_VALUE.data(), static_cast<int>(GetIncludeIncompleteSentencesIfLongerThanValue()));
    documentAnalysisSection->InsertEndChild(includeIncompleteSentencesIfLongerThanValue);

    // how to handle lists and header sentences
    auto invalidSentenceMethod = doc.NewElement(XML_INVALID_SENTENCE_METHOD.data());
    invalidSentenceMethod->SetAttribute(XML_METHOD.data(),
                                        static_cast<int>(GetInvalidSentenceMethod()));
    documentAnalysisSection->InsertEndChild(invalidSentenceMethod);
    projectSettings->InsertEndChild(documentAnalysisSection);

    // test bundles
    auto testBundlesSection = doc.NewElement(XML_TEST_BUNDLES.data());
    for (std::set<TestBundle>::const_iterator currentBundle = BaseProject::m_testBundles.begin();
         currentBundle != BaseProject::m_testBundles.end(); ++currentBundle)
        {
        // don't bother saving system bundles, those are dynamically added
        // by the program when it starts up
        if (currentBundle->IsLocked())
            {
            continue;
            }
        auto testBundle = doc.NewElement(XML_TEST_BUNDLE.data());
        // name
        auto testBundleName = doc.NewElement(XML_TEST_BUNDLE_NAME.data());
        wxString testBundleNameEncoded =
            encode({ currentBundle->GetName().c_str() }, false).c_str();
        testBundleName->SetAttribute(XML_VALUE.data(), testBundleNameEncoded.mb_str());
        testBundle->InsertEndChild(testBundleName);
        // description
        auto testBundleDescription = doc.NewElement(XML_TEST_BUNDLE_DESCRIPTION.data());
        wxString testBundleDescriptionEncoded =
            encode({ currentBundle->GetDescription().c_str() }, false).c_str();
        testBundleDescription->SetAttribute(XML_VALUE.data(),
                                            testBundleDescriptionEncoded.mb_str());
        testBundle->InsertEndChild(testBundleDescription);
        // included tests
        auto testNames = doc.NewElement(XML_TEST_NAMES.data());
        for (const auto& bundledTest : currentBundle->GetTestGoals())
            {
            auto testName = doc.NewElement(XML_TEST_NAME.data());
            wxString testNameEncoded = encode({ bundledTest.GetName().c_str() }, false).c_str();
            testName->SetAttribute(XML_VALUE.data(), testNameEncoded.mb_str());
            testName->SetAttribute(XML_GOAL_MIN_VAL_GOAL.data(),
                                   (std::isnan(bundledTest.GetMinGoal()) ?
                                        "" :
                                        wxString::FromCDouble(bundledTest.GetMinGoal()).mb_str()));
            testName->SetAttribute(XML_GOAL_MAX_VAL_GOAL.data(),
                                   (std::isnan(bundledTest.GetMaxGoal()) ?
                                        "" :
                                        wxString::FromCDouble(bundledTest.GetMaxGoal()).mb_str()));
            testNames->InsertEndChild(testName);
            }
        testBundle->InsertEndChild(testNames);
        // stats goals
        auto statGoals = doc.NewElement(XML_BUNDLE_STATISTICS.data());
        for (const auto& bundledStat : currentBundle->GetStatGoals())
            {
            auto statGoal = doc.NewElement(XML_BUNDLE_STATISTIC.data());
            wxString testNameEncoded = encode({ bundledStat.GetName().c_str() }, false).c_str();
            statGoal->SetAttribute(XML_VALUE.data(), testNameEncoded.mb_str());
            statGoal->SetAttribute(XML_GOAL_MIN_VAL_GOAL.data(),
                                   (std::isnan(bundledStat.GetMinGoal()) ?
                                        "" :
                                        wxString::FromCDouble(bundledStat.GetMinGoal()).mb_str()));
            statGoal->SetAttribute(XML_GOAL_MAX_VAL_GOAL.data(),
                                   (std::isnan(bundledStat.GetMaxGoal()) ?
                                        "" :
                                        wxString::FromCDouble(bundledStat.GetMaxGoal()).mb_str()));
            statGoals->InsertEndChild(statGoal);
            }
        testBundle->InsertEndChild(statGoals);

        testBundlesSection->InsertEndChild(testBundle);
        }
    projectSettings->InsertEndChild(testBundlesSection);

    // custom tests
    auto customTestsSection = doc.NewElement(XML_CUSTOM_TESTS.data());
    // add all global custom familiar word lists
    for (CustomReadabilityTestCollection::const_iterator pos =
             BaseProject::m_custom_word_tests.cbegin();
         pos != BaseProject::m_custom_word_tests.cend(); ++pos)
        {
        auto customFamWordTest = doc.NewElement(XML_CUSTOM_FAMILIAR_WORD_TEST.data());
        // name
        auto testName = doc.NewElement(XML_TEST_NAME.data());
        wxString testNameEncoded(pos->get_name().c_str());
        testNameEncoded = encode({ testNameEncoded.wc_str() }, false).c_str();
        testName->SetAttribute(XML_VALUE.data(), testNameEncoded.mb_str());
        customFamWordTest->InsertEndChild(testName);
        // file path
        auto filePath = doc.NewElement(XML_FAMILIAR_WORD_FILE_PATH.data());
        wxString filePathEncoded(pos->get_familiar_word_list_file_path().c_str());
        filePathEncoded = encode({ filePathEncoded.wc_str() }, false).c_str();
        filePath->SetAttribute(XML_VALUE.data(), filePathEncoded.mb_str());
        customFamWordTest->InsertEndChild(filePath);
        // formula (needs to be stored in U.S. format for portability)
        auto formula = doc.NewElement(XML_TEST_FORMULA.data());
        wxString formulaEncoded(
            FormulaFormat::FormatMathExpressionToUS(pos->get_formula().c_str()));
        formulaEncoded = encode({ formulaEncoded.wc_str() }, false).c_str();
        formula->SetAttribute(XML_VALUE.data(), formulaEncoded.mb_str());
        customFamWordTest->InsertEndChild(formula);
        // formula type (this is just needed for forward compatibility)
        const int formulaTypeValue =
            (wxString(pos->get_formula().c_str())
                 .CmpNoCase(ReadabilityFormulaParser::GetCustomSpacheSignature()) == 0) ?
                1 :
                0;
        auto formulaType = doc.NewElement(XML_TEST_FORMULA_TYPE.data());
        formulaType->SetAttribute(XML_VALUE.data(), formulaTypeValue);
        customFamWordTest->InsertEndChild(formulaType);
        // test type
        auto testType = doc.NewElement(XML_TEST_TYPE.data());
        testType->SetAttribute(XML_VALUE.data(), static_cast<int>(pos->get_test_type()));
        customFamWordTest->InsertEndChild(testType);
        // stemming type
        auto stemming = doc.NewElement(XML_STEMMING_TYPE.data());
        stemming->SetAttribute(XML_VALUE.data(), static_cast<int>(pos->get_stemming_type()));
        customFamWordTest->InsertEndChild(stemming);
        // include proper nouns
        auto includeProperNoun = doc.NewElement(XML_INCLUDE_PROPER_NOUNS.data());
        includeProperNoun->SetAttribute(XML_VALUE.data(),
                                        static_cast<int>(pos->get_proper_noun_method()));
        customFamWordTest->InsertEndChild(includeProperNoun);
        // include numeric
        auto includeNumeric = doc.NewElement(XML_INCLUDE_NUMERIC.data());
        includeNumeric->SetAttribute(XML_VALUE.data(),
                                     static_cast<int>(pos->is_including_numeric_as_familiar()));
        customFamWordTest->InsertEndChild(includeNumeric);
        // including custom list
        auto customWordList = doc.NewElement(XML_INCLUDE_CUSTOM_WORD_LIST.data());
        customWordList->SetAttribute(
            XML_VALUE.data(), static_cast<int>(pos->is_including_custom_familiar_word_list()));
        customFamWordTest->InsertEndChild(customWordList);
        // including DC list
        auto dcTest = doc.NewElement(XML_INCLUDE_DC_LIST.data());
        dcTest->SetAttribute(XML_VALUE.data(),
                             static_cast<int>(pos->is_including_dale_chall_list()));
        customFamWordTest->InsertEndChild(dcTest);
        // including Spache list
        auto spacheTest = doc.NewElement(XML_INCLUDE_SPACHE_LIST.data());
        spacheTest->SetAttribute(XML_VALUE.data(),
                                 static_cast<int>(pos->is_including_spache_list()));
        customFamWordTest->InsertEndChild(spacheTest);
        // including Stocker list
        auto stockerList = doc.NewElement(XML_INCLUDE_STOCKER_LIST.data());
        stockerList->SetAttribute(XML_VALUE.data(),
                                  static_cast<int>(pos->is_including_stocker_list()));
        customFamWordTest->InsertEndChild(stockerList);
        // including HJ list
        auto hjList = doc.NewElement(XML_INCLUDE_HARRIS_JACOBSON_LIST.data());
        hjList->SetAttribute(XML_VALUE.data(),
                             static_cast<int>(pos->is_including_harris_jacobson_list()));
        customFamWordTest->InsertEndChild(hjList);
        // whether familiar words have to be on each included list
        auto famAllLists = doc.NewElement(XML_FAMILIAR_WORDS_ALL_LISTS.data());
        famAllLists->SetAttribute(
            XML_VALUE.data(),
            static_cast<int>(pos->is_familiar_words_must_be_on_each_included_list()));
        customFamWordTest->InsertEndChild(famAllLists);
        // industry association
        auto industryChildrenPub = doc.NewElement(XML_INDUSTRY_CHILDRENS_PUBLISHING.data());
        industryChildrenPub->SetAttribute(
            XML_VALUE.data(),
            static_cast<int>(pos->has_industry_classification(
                readability::industry_classification::childrens_publishing_industry)));
        customFamWordTest->InsertEndChild(industryChildrenPub);

        auto industryAdultPublishing = doc.NewElement(XML_INDUSTRY_ADULTPUBLISHING.data());
        industryAdultPublishing->SetAttribute(
            XML_VALUE.data(),
            static_cast<int>(pos->has_industry_classification(
                readability::industry_classification::adult_publishing_industry)));
        customFamWordTest->InsertEndChild(industryAdultPublishing);

        auto industrySecondaryLanguage = doc.NewElement(XML_INDUSTRY_SECONDARY_LANGUAGE.data());
        industrySecondaryLanguage->SetAttribute(
            XML_VALUE.data(),
            static_cast<int>(pos->has_industry_classification(
                readability::industry_classification::sedondary_language_industry)));
        customFamWordTest->InsertEndChild(industrySecondaryLanguage);

        auto industryChildrensHealthCare = doc.NewElement(XML_INDUSTRY_CHILDRENS_HEALTHCARE.data());
        industryChildrensHealthCare->SetAttribute(
            XML_VALUE.data(),
            static_cast<int>(pos->has_industry_classification(
                readability::industry_classification::childrens_healthcare_industry)));
        customFamWordTest->InsertEndChild(industryChildrensHealthCare);

        auto industryAdultHealthCare = doc.NewElement(XML_INDUSTRY_ADULT_HEALTHCARE.data());
        industryAdultHealthCare->SetAttribute(
            XML_VALUE.data(),
            static_cast<int>(pos->has_industry_classification(
                readability::industry_classification::adult_healthcare_industry)));
        customFamWordTest->InsertEndChild(industryAdultHealthCare);

        auto industryMilitary = doc.NewElement(XML_INDUSTRY_MILITARY_GOVERNMENT.data());
        industryMilitary->SetAttribute(
            XML_VALUE.data(),
            static_cast<int>(pos->has_industry_classification(
                readability::industry_classification::military_government_industry)));
        customFamWordTest->InsertEndChild(industryMilitary);

        auto industryBroadCasting = doc.NewElement(XML_INDUSTRY_BROADCASTING.data());
        industryBroadCasting->SetAttribute(
            XML_VALUE.data(), static_cast<int>(pos->has_industry_classification(
                                  readability::industry_classification::broadcasting_industry)));
        customFamWordTest->InsertEndChild(industryBroadCasting);

        auto industryGeneralDocument = doc.NewElement(XML_DOCUMENT_GENERAL.data());
        industryGeneralDocument->SetAttribute(
            XML_VALUE.data(), static_cast<int>(pos->has_document_classification(
                                  readability::document_classification::general_document)));
        customFamWordTest->InsertEndChild(industryGeneralDocument);

        auto industryTechnicalDocument = doc.NewElement(XML_DOCUMENT_TECHNICAL.data());
        industryTechnicalDocument->SetAttribute(
            XML_VALUE.data(), static_cast<int>(pos->has_document_classification(
                                  readability::document_classification::technical_document)));
        customFamWordTest->InsertEndChild(industryTechnicalDocument);

        auto industryForm = doc.NewElement(XML_DOCUMENT_FORM.data());
        industryForm->SetAttribute(
            XML_VALUE.data(), static_cast<int>(pos->has_document_classification(
                                  readability::document_classification::nonnarrative_document)));
        customFamWordTest->InsertEndChild(industryForm);

        auto industryYoungAdultAndAdultLiterature = doc.NewElement(XML_DOCUMENT_YOUNGADULT.data());
        industryYoungAdultAndAdultLiterature->SetAttribute(
            XML_VALUE.data(),
            static_cast<int>(pos->has_document_classification(
                readability::document_classification::adult_literature_document)));
        customFamWordTest->InsertEndChild(industryYoungAdultAndAdultLiterature);

        auto industryChildrensLiterature = doc.NewElement(XML_DOCUMENT_CHILDREN_LIT.data());
        industryChildrensLiterature->SetAttribute(
            XML_VALUE.data(),
            static_cast<int>(pos->has_document_classification(
                readability::document_classification::childrens_literature_document)));
        customFamWordTest->InsertEndChild(industryChildrensLiterature);

        customTestsSection->InsertEndChild(customFamWordTest);
        }
    projectSettings->InsertEndChild(customTestsSection);

    // graph defaults
    auto graphDefaultsSection = doc.NewElement(XML_GRAPH_SETTINGS.data());
    // color scheme
    auto graphColorScheme = doc.NewElement(XML_GRAPH_COLOR_SCHEME.data());
    graphColorScheme->SetAttribute(
        XML_VALUE.data(),
        wxString(encode({ GetGraphColorScheme().wc_str() }, false).c_str()).mb_str());
    graphDefaultsSection->InsertEndChild(graphColorScheme);
    // background image
    auto graphBackgroundImage = doc.NewElement(XML_GRAPH_PLOT_BACKGROUND_IMAGE_PATH.data());
    graphBackgroundImage->SetAttribute(
        XML_VALUE.data(),
        wxString(encode({ GetPlotBackGroundImagePath().wc_str() }, false).c_str()).mb_str());
    graphDefaultsSection->InsertEndChild(graphBackgroundImage);

    auto backgroundImageEffect = doc.NewElement(XML_GRAPH_PLOT_BACKGROUND_IMAGE_EFFECT.data());
    backgroundImageEffect->SetAttribute(XML_VALUE.data(),
                                        static_cast<int>(GetPlotBackGroundImageEffect()));
    graphDefaultsSection->InsertEndChild(backgroundImageEffect);

    auto backgroundImageFit = doc.NewElement(XML_GRAPH_PLOT_BACKGROUND_IMAGE_FIT.data());
    backgroundImageFit->SetAttribute(XML_VALUE.data(),
                                     static_cast<int>(GetPlotBackGroundImageFit()));
    graphDefaultsSection->InsertEndChild(backgroundImageFit);
    // background colors
    if (GetBackGroundColor().IsOk())
        {
        auto backgroundColor = doc.NewElement(XML_GRAPH_BACKGROUND_COLOR.data());
        backgroundColor->SetAttribute(XmlFormat::GetRed().mb_str(), GetBackGroundColor().Red());
        backgroundColor->SetAttribute(XmlFormat::GetGreen().mb_str(), GetBackGroundColor().Green());
        backgroundColor->SetAttribute(XmlFormat::GetBlue().mb_str(), GetBackGroundColor().Blue());
        graphDefaultsSection->InsertEndChild(backgroundColor);
        }
    if (GetPlotBackGroundColor().IsOk())
        {
        auto plotBackgroundColor = doc.NewElement(XML_GRAPH_PLOT_BACKGROUND_COLOR.data());
        plotBackgroundColor->SetAttribute(XmlFormat::GetRed().mb_str(),
                                          GetPlotBackGroundColor().Red());
        plotBackgroundColor->SetAttribute(XmlFormat::GetGreen().mb_str(),
                                          GetPlotBackGroundColor().Green());
        plotBackgroundColor->SetAttribute(XmlFormat::GetBlue().mb_str(),
                                          GetPlotBackGroundColor().Blue());
        graphDefaultsSection->InsertEndChild(plotBackgroundColor);
        }
    // plot background image opacity
    auto graphBackgroundOpacity = doc.NewElement(XML_GRAPH_PLOT_BACKGROUND_IMAGE_OPACITY.data());
    graphBackgroundOpacity->SetAttribute(XML_VALUE.data(), GetPlotBackGroundImageOpacity());
    graphDefaultsSection->InsertEndChild(graphBackgroundOpacity);
    // plot background color opacity
    auto graphPlotBackgroundOpacity =
        doc.NewElement(XML_GRAPH_PLOT_BACKGROUND_COLOR_OPACITY.data());
    graphPlotBackgroundOpacity->SetAttribute(XML_VALUE.data(), GetPlotBackGroundColorOpacity());
    graphDefaultsSection->InsertEndChild(graphPlotBackgroundOpacity);
    // background linear gradient
    auto graphBackgroundGradient = doc.NewElement(XML_GRAPH_BACKGROUND_LINEAR_GRADIENT.data());
    graphBackgroundGradient->SetAttribute(XML_VALUE.data(),
                                          bool_to_int(GetGraphBackGroundLinearGradient()));
    graphDefaultsSection->InsertEndChild(graphBackgroundGradient);
    // stipple image path
    auto stipplePath = doc.NewElement(XML_GRAPH_STIPPLE_PATH.data());
    stipplePath->SetAttribute(
        XML_VALUE.data(),
        wxString(encode({ GetStippleImagePath().wc_str() }, false).c_str()).mb_str());
    graphDefaultsSection->InsertEndChild(stipplePath);
    // common image path
    auto commonImagePath = doc.NewElement(XML_GRAPH_COMMON_IMAGE_PATH.data());
    commonImagePath->SetAttribute(
        XML_VALUE.data(),
        wxString(encode({ GetGraphCommonImagePath().wc_str() }, false).c_str()).mb_str());
    graphDefaultsSection->InsertEndChild(commonImagePath);
    // stipple shape
    auto stippleShape = doc.NewElement(XML_GRAPH_STIPPLE_SHAPE.data());
    stippleShape->SetAttribute(
        XML_VALUE.data(), wxString(encode({ GetStippleShape().wc_str() }, false).c_str()).mb_str());
    graphDefaultsSection->InsertEndChild(stippleShape);

    if (GetStippleShapeColor().IsOk())
        {
        auto stippleShapeColor = doc.NewElement(XML_GRAPH_STIPPLE_COLOR.data());
        stippleShapeColor->SetAttribute(XmlFormat::GetRed().mb_str(), GetStippleShapeColor().Red());
        stippleShapeColor->SetAttribute(XmlFormat::GetGreen().mb_str(),
                                        GetStippleShapeColor().Green());
        stippleShapeColor->SetAttribute(XmlFormat::GetBlue().mb_str(),
                                        GetStippleShapeColor().Blue());
        graphDefaultsSection->InsertEndChild(stippleShapeColor);
        }

    // whether drop shadows should be shown
    auto graphDisplayDropShadow = doc.NewElement(XML_DISPLAY_DROP_SHADOW.data());
    graphDisplayDropShadow->SetAttribute(XML_VALUE.data(), bool_to_int(IsDisplayingDropShadows()));
    graphDefaultsSection->InsertEndChild(graphDisplayDropShadow);
    // whether to draw attention to the complex word groups in syllable graphs
    auto showcaseComplexWords = doc.NewElement(XML_SHOWCASE_COMPLEX_WORDS.data());
    showcaseComplexWords->SetAttribute(XML_VALUE.data(), bool_to_int(IsShowcasingComplexWords()));
    graphDefaultsSection->InsertEndChild(showcaseComplexWords);
    // watermark
    auto graphWatermarkLogo = doc.NewElement(XML_GRAPH_WATERMARK_LOGO_IMAGE_PATH.data());
    graphWatermarkLogo->SetAttribute(
        XML_VALUE.data(),
        wxString(encode({ GetWatermarkLogo().wc_str() }, false).c_str()).mb_str());
    graphDefaultsSection->InsertEndChild(graphWatermarkLogo);

    auto graphWatermark = doc.NewElement(XML_GRAPH_WATERMARK.data());
    graphWatermark->SetAttribute(
        XML_VALUE.data(), wxString(encode({ GetWatermark().wc_str() }, false).c_str()).mb_str());
    graphDefaultsSection->InsertEndChild(graphWatermark);

    // histogram settings
    auto histogramSettings = doc.NewElement(XML_HISTOGRAM_SETTINGS.data());
    // categorization method
    auto hCatMode = doc.NewElement(XML_GRAPH_BINNING_METHOD.data());
    hCatMode->SetAttribute(XML_VALUE.data(), static_cast<int>(GetHistorgramBinningMethod()));
    histogramSettings->InsertEndChild(hCatMode);
    // rounding method
    auto hRoundMode = doc.NewElement(XML_GRAPH_ROUNDING_METHOD.data());
    hRoundMode->SetAttribute(XML_VALUE.data(), static_cast<int>(GetHistogramRoundingMethod()));
    histogramSettings->InsertEndChild(hRoundMode);
    // interval display
    auto hIntervalDisplay = doc.NewElement(XML_GRAPH_INTERVAL_DISPLAY.data());
    hIntervalDisplay->SetAttribute(XML_VALUE.data(),
                                   static_cast<int>(GetHistogramIntervalDisplay()));
    histogramSettings->InsertEndChild(hIntervalDisplay);
    // categorization label display
    auto hCatLabelDisplay = doc.NewElement(XML_GRAPH_BINNING_LABEL_DISPLAY.data());
    hCatLabelDisplay->SetAttribute(XML_VALUE.data(),
                                   static_cast<int>(GetHistrogramBinLabelDisplay()));
    histogramSettings->InsertEndChild(hCatLabelDisplay);
    // bar color
    auto hbarColor = doc.NewElement(XML_GRAPH_COLOR.data());
    hbarColor->SetAttribute(XmlFormat::GetRed().mb_str(), GetHistogramBarColor().Red());
    hbarColor->SetAttribute(XmlFormat::GetGreen().mb_str(), GetHistogramBarColor().Green());
    hbarColor->SetAttribute(XmlFormat::GetBlue().mb_str(), GetHistogramBarColor().Blue());
    histogramSettings->InsertEndChild(hbarColor);
    // bar opacity
    auto hbarOpacity = doc.NewElement(XML_GRAPH_OPACITY.data());
    hbarOpacity->SetAttribute(XML_VALUE.data(), GetHistogramBarOpacity());
    histogramSettings->InsertEndChild(hbarOpacity);
    // bar effect
    auto hbarEffect = doc.NewElement(XML_BAR_EFFECT.data());
    hbarEffect->SetAttribute(XML_VALUE.data(), static_cast<int>(GetHistogramBarEffect()));
    histogramSettings->InsertEndChild(hbarEffect);

    graphDefaultsSection->InsertEndChild(histogramSettings);

    // bar chart settings
    auto barChartSettings = doc.NewElement(XML_BAR_CHART_SETTINGS.data());
    // bar color
    auto barChartbarColor = doc.NewElement(XML_GRAPH_COLOR.data());
    barChartbarColor->SetAttribute(XmlFormat::GetRed().mb_str(), GetBarChartBarColor().Red());
    barChartbarColor->SetAttribute(XmlFormat::GetGreen().mb_str(), GetBarChartBarColor().Green());
    barChartbarColor->SetAttribute(XmlFormat::GetBlue().mb_str(), GetBarChartBarColor().Blue());
    barChartSettings->InsertEndChild(barChartbarColor);
    // bar orientation
    auto barOrientation = doc.NewElement(XML_BAR_ORIENTATION.data());
    barOrientation->SetAttribute(XML_VALUE.data(), static_cast<int>(GetBarChartOrientation()));
    barChartSettings->InsertEndChild(barOrientation);
    // bar opacity
    auto barOpacity = doc.NewElement(XML_GRAPH_OPACITY.data());
    barOpacity->SetAttribute(XML_VALUE.data(), GetGraphBarOpacity());
    barChartSettings->InsertEndChild(barOpacity);
    // bar display labels
    auto barDisplayLabels = doc.NewElement(XML_BAR_DISPLAY_LABELS.data());
    barDisplayLabels->SetAttribute(XML_VALUE.data(),
                                   static_cast<int>(IsDisplayingBarChartLabels()));
    barChartSettings->InsertEndChild(barDisplayLabels);
    // bar effect
    auto barEffect = doc.NewElement(XML_BAR_EFFECT.data());
    barEffect->SetAttribute(XML_VALUE.data(), static_cast<int>(GetGraphBarEffect()));
    barChartSettings->InsertEndChild(barEffect);

    graphDefaultsSection->InsertEndChild(barChartSettings);

    // box plot settings
    auto boxPlotSettings = doc.NewElement(XML_BOX_PLOT_SETTINGS.data());
    // box color
    auto boxColor = doc.NewElement(XML_GRAPH_COLOR.data());
    boxColor->SetAttribute(XmlFormat::GetRed().mb_str(), GetGraphBoxColor().Red());
    boxColor->SetAttribute(XmlFormat::GetGreen().mb_str(), GetGraphBoxColor().Green());
    boxColor->SetAttribute(XmlFormat::GetBlue().mb_str(), GetGraphBoxColor().Blue());
    boxPlotSettings->InsertEndChild(boxColor);
    // box opacity
    auto boxOpacity = doc.NewElement(XML_GRAPH_OPACITY.data());
    boxOpacity->SetAttribute(XML_VALUE.data(), GetGraphBoxOpacity());
    boxPlotSettings->InsertEndChild(boxOpacity);
    // box effect
    auto boxEffect = doc.NewElement(XML_BOX_EFFECT.data());
    boxEffect->SetAttribute(XML_VALUE.data(), static_cast<int>(GetGraphBoxEffect()));
    boxPlotSettings->InsertEndChild(boxEffect);
    // box plot points
    auto boxPlotAllPoints = doc.NewElement(XML_BOX_PLOT_SHOW_ALL_POINTS.data());
    boxPlotAllPoints->SetAttribute(XML_VALUE.data(), static_cast<int>(IsShowingAllBoxPlotPoints()));
    boxPlotSettings->InsertEndChild(boxPlotAllPoints);
    // box connections
    auto boxConnectMiddlePoints = doc.NewElement(XML_BOX_CONNECT_MIDDLE_POINTS.data());
    boxConnectMiddlePoints->SetAttribute(XML_VALUE.data(),
                                         static_cast<int>(IsConnectingBoxPlotMiddlePoints()));
    boxPlotSettings->InsertEndChild(boxConnectMiddlePoints);
    // box display labels
    auto boxDisplayLabels = doc.NewElement(XML_BOX_DISPLAY_LABELS.data());
    boxDisplayLabels->SetAttribute(XML_VALUE.data(), static_cast<int>(IsDisplayingBoxPlotLabels()));
    boxPlotSettings->InsertEndChild(boxDisplayLabels);

    graphDefaultsSection->InsertEndChild(boxPlotSettings);

    // Readability graph settings
    // Lix
    auto lixSettings = doc.NewElement(XML_LIX_SETTINGS.data());
    auto useEnglishLabels = doc.NewElement(XML_USE_ENGLISH_LABELS.data());
    useEnglishLabels->SetAttribute(XML_INCLUDE.data(),
                                   bool_to_int(IsUsingEnglishLabelsForGermanLix()));
    lixSettings->InsertEndChild(useEnglishLabels);
    graphDefaultsSection->InsertEndChild(lixSettings);
    // Flesch chart
    auto fleschChartSettings = doc.NewElement(XML_FLESCH_CHART_SETTINGS.data());
    auto connectionLine = doc.NewElement(XML_INCLUDE_CONNECTION_LINE.data());
    connectionLine->SetAttribute(XML_INCLUDE.data(), bool_to_int(IsConnectingFleschPoints()));
    fleschChartSettings->InsertEndChild(connectionLine);

    auto rulerDocGroup = doc.NewElement(XML_FLESCH_RULER_DOC_GROUPS.data());
    rulerDocGroup->SetAttribute(XML_VALUE.data(), bool_to_int(IsIncludingFleschRulerDocGroups()));
    fleschChartSettings->InsertEndChild(rulerDocGroup);

    graphDefaultsSection->InsertEndChild(fleschChartSettings);
    // Fry/Raygor
    auto fryRaygor = doc.NewElement(XML_FRY_RAYGOR_SETTINGS.data());
    // invalid area colors
    auto invalidAreaColor = doc.NewElement(XML_INVALID_AREA_COLOR.data());
    invalidAreaColor->SetAttribute(XmlFormat::GetRed().mb_str(), GetInvalidAreaColor().Red());
    invalidAreaColor->SetAttribute(XmlFormat::GetGreen().mb_str(), GetInvalidAreaColor().Green());
    invalidAreaColor->SetAttribute(XmlFormat::GetBlue().mb_str(), GetInvalidAreaColor().Blue());
    fryRaygor->InsertEndChild(invalidAreaColor);

    auto raygorStyle = doc.NewElement(XML_RAYGOR_STYLE.data());
    raygorStyle->SetAttribute(XML_VALUE.data(), static_cast<int>(GetRaygorStyle()));
    fryRaygor->InsertEndChild(raygorStyle);

    graphDefaultsSection->InsertEndChild(fryRaygor);

    // x axis
    auto Axis = doc.NewElement(XML_AXIS_SETTINGS.data());
    auto xAxis = doc.NewElement(XML_X_AXIS.data());
    // x-axis font color
    auto xAxisFontColor = doc.NewElement(XML_FONT_COLOR.data());
    xAxisFontColor->SetAttribute(XmlFormat::GetRed().mb_str(), GetXAxisFontColor().Red());
    xAxisFontColor->SetAttribute(XmlFormat::GetGreen().mb_str(), GetXAxisFontColor().Green());
    xAxisFontColor->SetAttribute(XmlFormat::GetBlue().mb_str(), GetXAxisFontColor().Blue());
    // x-axis font
    auto xAxisFont = doc.NewElement(XML_FONT.data());
    xAxisFont->SetAttribute(XmlFormat::GetFontPointSize().mb_str(), GetXAxisFont().GetPointSize());
    xAxisFont->SetAttribute(XmlFormat::GetFontStyle().mb_str(),
                            static_cast<int>(GetXAxisFont().GetStyle()));
    xAxisFont->SetAttribute(XmlFormat::GetFontWeight().mb_str(),
                            static_cast<int>(GetXAxisFont().GetWeight()));
    xAxisFont->SetAttribute(XmlFormat::GetFontUnderline().mb_str(),
                            bool_to_int(GetXAxisFont().GetUnderlined()));
    xAxisFont->SetAttribute(
        XmlFormat::GetFontFaceName().mb_str(),
        wxString(encode({ GetXAxisFont().GetFaceName().wc_str() }, false).c_str()).mb_str());
    // put it all together
    xAxis->InsertEndChild(xAxisFontColor);
    xAxis->InsertEndChild(xAxisFont);
    Axis->InsertEndChild(xAxis);

    // y axis
    auto yAxis = doc.NewElement(XML_Y_AXIS.data());
    // y-axis font color
    auto yAxisFontColor = doc.NewElement(XML_FONT_COLOR.data());
    yAxisFontColor->SetAttribute(XmlFormat::GetRed().mb_str(), GetYAxisFontColor().Red());
    yAxisFontColor->SetAttribute(XmlFormat::GetGreen().mb_str(), GetYAxisFontColor().Green());
    yAxisFontColor->SetAttribute(XmlFormat::GetBlue().mb_str(), GetYAxisFontColor().Blue());
    // y-axis font
    auto yAxisFont = doc.NewElement(XML_FONT.data());
    yAxisFont->SetAttribute(XmlFormat::GetFontPointSize().mb_str(), GetYAxisFont().GetPointSize());
    yAxisFont->SetAttribute(XmlFormat::GetFontStyle().mb_str(),
                            static_cast<int>(GetYAxisFont().GetStyle()));
    yAxisFont->SetAttribute(XmlFormat::GetFontWeight().mb_str(),
                            static_cast<int>(GetYAxisFont().GetWeight()));
    yAxisFont->SetAttribute(XmlFormat::GetFontUnderline().mb_str(),
                            bool_to_int(GetYAxisFont().GetUnderlined()));
    yAxisFont->SetAttribute(
        XmlFormat::GetFontFaceName().mb_str(),
        wxString(encode({ GetYAxisFont().GetFaceName().wc_str() }, false).c_str()).mb_str());
    // put it all together
    yAxis->InsertEndChild(yAxisFontColor);
    yAxis->InsertEndChild(yAxisFont);
    Axis->InsertEndChild(yAxis);
    graphDefaultsSection->InsertEndChild(Axis);

    // title settings
    auto titleSettings = doc.NewElement(XML_TITLE_SETTINGS.data());
    auto topTitle = doc.NewElement(XML_TOP_TITLE.data());
    // top title font color
    auto topTitleFontColor = doc.NewElement(XML_FONT_COLOR.data());
    topTitleFontColor->SetAttribute(XmlFormat::GetRed().mb_str(),
                                    GetGraphTopTitleFontColor().Red());
    topTitleFontColor->SetAttribute(XmlFormat::GetGreen().mb_str(),
                                    GetGraphTopTitleFontColor().Green());
    topTitleFontColor->SetAttribute(XmlFormat::GetBlue().mb_str(),
                                    GetGraphTopTitleFontColor().Blue());
    // top title font
    auto topTitleFont = doc.NewElement(XML_FONT.data());
    topTitleFont->SetAttribute(XmlFormat::GetFontPointSize().mb_str(),
                               GetGraphTopTitleFont().GetPointSize());
    topTitleFont->SetAttribute(XmlFormat::GetFontStyle().mb_str(),
                               static_cast<int>(GetGraphTopTitleFont().GetStyle()));
    topTitleFont->SetAttribute(XmlFormat::GetFontWeight().mb_str(),
                               static_cast<int>(GetGraphTopTitleFont().GetWeight()));
    topTitleFont->SetAttribute(XmlFormat::GetFontUnderline().mb_str(),
                               bool_to_int(GetGraphTopTitleFont().GetUnderlined()));
    topTitleFont->SetAttribute(
        XmlFormat::GetFontFaceName().mb_str(),
        wxString(encode({ GetGraphTopTitleFont().GetFaceName().wc_str() }, false).c_str())
            .mb_str());
    // put it all together
    topTitle->InsertEndChild(topTitleFontColor);
    topTitle->InsertEndChild(topTitleFont);
    titleSettings->InsertEndChild(topTitle);

    auto bottomTitle = doc.NewElement(XML_BOTTOM_TITLE.data());
    // bottom title font color
    auto bottomTitleFontColor = doc.NewElement(XML_FONT_COLOR.data());
    bottomTitleFontColor->SetAttribute(XmlFormat::GetRed().mb_str(),
                                       GetBottomTitleGraphFontColor().Red());
    bottomTitleFontColor->SetAttribute(XmlFormat::GetGreen().mb_str(),
                                       GetBottomTitleGraphFontColor().Green());
    bottomTitleFontColor->SetAttribute(XmlFormat::GetBlue().mb_str(),
                                       GetBottomTitleGraphFontColor().Blue());
    // bottom title font
    auto bottomTitleFont = doc.NewElement(XML_FONT.data());
    bottomTitleFont->SetAttribute(XmlFormat::GetFontPointSize().mb_str(),
                                  GetBottomTitleGraphFont().GetPointSize());
    bottomTitleFont->SetAttribute(XmlFormat::GetFontStyle().mb_str(),
                                  static_cast<int>(GetBottomTitleGraphFont().GetStyle()));
    bottomTitleFont->SetAttribute(XmlFormat::GetFontWeight().mb_str(),
                                  static_cast<int>(GetBottomTitleGraphFont().GetWeight()));
    bottomTitleFont->SetAttribute(XmlFormat::GetFontUnderline().mb_str(),
                                  bool_to_int(GetBottomTitleGraphFont().GetUnderlined()));
    bottomTitleFont->SetAttribute(
        XmlFormat::GetFontFaceName().mb_str(),
        wxString(encode({ GetBottomTitleGraphFont().GetFaceName().wc_str() }, false).c_str())
            .mb_str());
    // put it all together
    bottomTitle->InsertEndChild(bottomTitleFontColor);
    bottomTitle->InsertEndChild(bottomTitleFont);
    titleSettings->InsertEndChild(bottomTitle);

    auto leftTitle = doc.NewElement(XML_LEFT_TITLE.data());
    // left title font color
    auto leftTitleFontColor = doc.NewElement(XML_FONT_COLOR.data());
    leftTitleFontColor->SetAttribute(XmlFormat::GetRed().mb_str(),
                                     GetLeftTitleGraphFontColor().Red());
    leftTitleFontColor->SetAttribute(XmlFormat::GetGreen().mb_str(),
                                     GetLeftTitleGraphFontColor().Green());
    leftTitleFontColor->SetAttribute(XmlFormat::GetBlue().mb_str(),
                                     GetLeftTitleGraphFontColor().Blue());
    // left title font
    auto leftTitleFont = doc.NewElement(XML_FONT.data());
    leftTitleFont->SetAttribute(XmlFormat::GetFontPointSize().mb_str(),
                                GetLeftTitleGraphFont().GetPointSize());
    leftTitleFont->SetAttribute(XmlFormat::GetFontStyle().mb_str(),
                                static_cast<int>(GetLeftTitleGraphFont().GetStyle()));
    leftTitleFont->SetAttribute(XmlFormat::GetFontWeight().mb_str(),
                                static_cast<int>(GetLeftTitleGraphFont().GetWeight()));
    leftTitleFont->SetAttribute(XmlFormat::GetFontUnderline().mb_str(),
                                bool_to_int(GetLeftTitleGraphFont().GetUnderlined()));
    leftTitleFont->SetAttribute(
        XmlFormat::GetFontFaceName().mb_str(),
        wxString(encode({ GetLeftTitleGraphFont().GetFaceName().wc_str() }, false).c_str())
            .mb_str());
    // put it all together
    leftTitle->InsertEndChild(leftTitleFontColor);
    leftTitle->InsertEndChild(leftTitleFont);
    titleSettings->InsertEndChild(leftTitle);

    auto rightTitle = doc.NewElement(XML_RIGHT_TITLE.data());
    // right title font color
    auto rightTitleFontColor = doc.NewElement(XML_FONT_COLOR.data());
    rightTitleFontColor->SetAttribute(XmlFormat::GetRed().mb_str(),
                                      GetRightTitleGraphFontColor().Red());
    rightTitleFontColor->SetAttribute(XmlFormat::GetGreen().mb_str(),
                                      GetRightTitleGraphFontColor().Green());
    rightTitleFontColor->SetAttribute(XmlFormat::GetBlue().mb_str(),
                                      GetRightTitleGraphFontColor().Blue());
    // right title font
    auto rightTitleFont = doc.NewElement(XML_FONT.data());
    rightTitleFont->SetAttribute(XmlFormat::GetFontPointSize().mb_str(),
                                 GetRightTitleGraphFont().GetPointSize());
    rightTitleFont->SetAttribute(XmlFormat::GetFontStyle().mb_str(),
                                 static_cast<int>(GetRightTitleGraphFont().GetStyle()));
    rightTitleFont->SetAttribute(XmlFormat::GetFontWeight().mb_str(),
                                 static_cast<int>(GetRightTitleGraphFont().GetWeight()));
    rightTitleFont->SetAttribute(XmlFormat::GetFontUnderline().mb_str(),
                                 bool_to_int(GetRightTitleGraphFont().GetUnderlined()));
    rightTitleFont->SetAttribute(
        XmlFormat::GetFontFaceName().mb_str(),
        wxString(encode({ GetRightTitleGraphFont().GetFaceName().wc_str() }, false).c_str())
            .mb_str());
    // put it all together
    rightTitle->InsertEndChild(rightTitleFontColor);
    rightTitle->InsertEndChild(rightTitleFont);
    titleSettings->InsertEndChild(rightTitle);
    graphDefaultsSection->InsertEndChild(titleSettings);

    projectSettings->InsertEndChild(graphDefaultsSection);

    // stats defaults
    auto statsDefaultsSection = doc.NewElement(XML_STATISTICS_SECTION.data());
    // variance method
    auto varianceMethod = doc.NewElement(XML_VARIANCE_METHOD.data());
    varianceMethod->SetAttribute(XML_METHOD.data(), static_cast<int>(GetVarianceMethod()));
    statsDefaultsSection->InsertEndChild(varianceMethod);
    // stats results
    auto statsResults = doc.NewElement(XML_STATISTICS_RESULTS.data());
    statsResults->SetAttribute(XML_VALUE.data(), GetStatisticsInfo().ToString().mb_str());
    statsDefaultsSection->InsertEndChild(statsResults);
    // stats report
    auto statsReport = doc.NewElement(XML_STATISTICS_REPORT.data());
    statsReport->SetAttribute(XML_VALUE.data(), GetStatisticsReportInfo().ToString().mb_str());
    statsDefaultsSection->InsertEndChild(statsReport);
    projectSettings->InsertEndChild(statsDefaultsSection);

    // wizard page defaults
    auto wizardPageDefaultsSection = doc.NewElement(XML_WIZARD_PAGES_SETTINGS.data());

    auto batchGrouping = doc.NewElement(XML_WIZARD_BATCH_GROUP.data());
    batchGrouping->SetAttribute(XML_METHOD.data(), m_batchGroupDefault);
    wizardPageDefaultsSection->InsertEndChild(batchGrouping);

    // Text Source
    auto textSource = doc.NewElement(XML_TEXT_SOURCE.data());
    textSource->SetAttribute(XML_METHOD.data(), static_cast<int>(m_textSource));
    wizardPageDefaultsSection->InsertEndChild(textSource);

    // Test Recommendation
    auto testRecommendation = doc.NewElement(XML_TEST_RECOMMENDATION.data());
    testRecommendation->SetAttribute(XML_METHOD.data(), static_cast<int>(m_testRecommendation));
    wizardPageDefaultsSection->InsertEndChild(testRecommendation);

    // Tests by Industry
    auto testsByIndustry = doc.NewElement(XML_TEST_BY_INDUSTRY.data());
    testsByIndustry->SetAttribute(XML_METHOD.data(), static_cast<int>(m_testsByIndustry));
    wizardPageDefaultsSection->InsertEndChild(testsByIndustry);

    // Tests By Document Type
    auto testsByDocumentType = doc.NewElement(XML_TEST_BY_DOCUMENT_TYPE.data());
    testsByDocumentType->SetAttribute(XML_METHOD.data(), static_cast<int>(m_testsByDocumentType));
    wizardPageDefaultsSection->InsertEndChild(testsByDocumentType);

    // tests by bundle
    auto testsByBundle = doc.NewElement(XML_SELECTED_TEST_BUNDLE.data());
    testsByBundle->SetAttribute(XML_VALUE.data(), GetSelectedTestBundle().mb_str());
    wizardPageDefaultsSection->InsertEndChild(testsByBundle);

    projectSettings->InsertEndChild(wizardPageDefaultsSection);

    // readability tests information
    auto readabilityTestSection = doc.NewElement(XML_READABILITY_TESTS_SECTION.data());

    // options inclusions
    auto isIncludingScoreSummary = doc.NewElement(XML_INCLUDE_SCORES_SUMMARY_REPORT.data());
    isIncludingScoreSummary->SetAttribute(XML_VALUE.data(),
                                          bool_to_int(IsIncludingScoreSummaryReport()));
    readabilityTestSection->InsertEndChild(isIncludingScoreSummary);

    // grade scale/reading age display
    auto readingAgeFormat = doc.NewElement(XML_READING_AGE_FORMAT.data());
    readingAgeFormat->SetAttribute(
        XML_VALUE.data(), static_cast<int>(GetReadabilityMessageCatalog().GetReadingAgeDisplay()));
    readabilityTestSection->InsertEndChild(readingAgeFormat);

    auto gradeScale = doc.NewElement(XML_READABILITY_TEST_GRADE_SCALE_DISPLAY.data());
    gradeScale->SetAttribute(XML_VALUE.data(),
                             static_cast<int>(GetReadabilityMessageCatalog().GetGradeScale()));
    readabilityTestSection->InsertEndChild(gradeScale);

    auto gradeScaleLongFormat = doc.NewElement(XML_READABILITY_TEST_GRADE_SCALE_LONG_FORMAT.data());
    gradeScaleLongFormat->SetAttribute(
        XML_VALUE.data(),
        bool_to_int(GetReadabilityMessageCatalog().IsUsingLongGradeScaleFormat()));
    readabilityTestSection->InsertEndChild(gradeScaleLongFormat);

    // test-specific options
    //----------------------
    // Flesch-Kincaid options
    auto fleschKincaidOptions = doc.NewElement(XML_FLESCH_KINCAID_OPTIONS.data());
    auto fleschKincaidNumeralMethod = doc.NewElement(XML_NUMERAL_SYLLABICATION_METHOD.data());
    fleschKincaidNumeralMethod->SetAttribute(
        XML_VALUE.data(), static_cast<int>(GetFleschKincaidNumeralSyllabizeMethod()));
    fleschKincaidOptions->InsertEndChild(fleschKincaidNumeralMethod);
    readabilityTestSection->InsertEndChild(fleschKincaidOptions);
    // Flesch options
    auto fleschOptions = doc.NewElement(XML_FLESCH_OPTIONS.data());
    auto fleschNumeralMethod = doc.NewElement(XML_NUMERAL_SYLLABICATION_METHOD.data());
    fleschNumeralMethod->SetAttribute(XML_VALUE.data(),
                                      static_cast<int>(GetFleschNumeralSyllabizeMethod()));
    fleschOptions->InsertEndChild(fleschNumeralMethod);
    readabilityTestSection->InsertEndChild(fleschOptions);
    // Gunning fog options
    auto fogOptions = doc.NewElement(XML_GUNNING_FOG_OPTIONS.data());
    auto useUnits = doc.NewElement(XML_USE_SENTENCE_UNITS.data());
    useUnits->SetAttribute(XML_VALUE.data(), bool_to_int(FogUseSentenceUnits()));
    fogOptions->InsertEndChild(useUnits);
    readabilityTestSection->InsertEndChild(fogOptions);
    // HJ options
    auto hjOptions = doc.NewElement(XML_HARRIS_JACOBSON_OPTIONS.data());
    // HJ text exclusion
    auto hjTextExclusionMode = doc.NewElement(XML_TEXT_EXCLUSION.data());
    hjTextExclusionMode->SetAttribute(XML_VALUE.data(),
                                      static_cast<int>(GetHarrisJacobsonTextExclusionMode()));
    hjOptions->InsertEndChild(hjTextExclusionMode);
    readabilityTestSection->InsertEndChild(hjOptions);
    // DC options
    auto dcOptions = doc.NewElement(XML_NEW_DALE_CHALL_OPTIONS.data());
    // stocker list
    auto stockerList = doc.NewElement(XML_STOCKER_LIST.data());
    stockerList->SetAttribute(XML_VALUE.data(),
                              bool_to_int(IsIncludingStockerCatholicSupplement()));
    dcOptions->InsertEndChild(stockerList);
    // dc text exclusion
    auto dcTextExclusionMode = doc.NewElement(XML_TEXT_EXCLUSION.data());
    dcTextExclusionMode->SetAttribute(XML_VALUE.data(),
                                      static_cast<int>(GetDaleChallTextExclusionMode()));
    dcOptions->InsertEndChild(dcTextExclusionMode);
    // proper nouns
    auto dcProperNouns = doc.NewElement(XML_PROPER_NOUN_COUNTING_METHOD.data());
    dcProperNouns->SetAttribute(XML_VALUE.data(),
                                static_cast<int>(GetDaleChallProperNounCountingMethod()));
    dcOptions->InsertEndChild(dcProperNouns);
    readabilityTestSection->InsertEndChild(dcOptions);

    for (auto rTest = GetReadabilityTests().get_tests().begin();
         rTest != GetReadabilityTests().get_tests().end(); ++rTest)
        {
        auto currentTest = doc.NewElement(wxString(rTest->get_test().get_id().c_str()).mb_str());
        currentTest->SetAttribute(XML_INCLUDE.data(), bool_to_int(rTest->is_included()));
        readabilityTestSection->InsertEndChild(currentTest);
        }

    // Dolch
    auto dolch = doc.NewElement(XML_DOLCH_SUITE.data());
    dolch->SetAttribute(XML_INCLUDE.data(), bool_to_int(IsDolchSelected()));
    readabilityTestSection->InsertEndChild(dolch);
    // all the custom tests being included
    for (size_t i = 0; i < GetIncludedCustomTests().size(); ++i)
        {
        auto customTest = doc.NewElement(XML_CUSTOM_TEST.data());
        customTest->SetAttribute(
            XML_VALUE.data(),
            wxString(encode({ GetIncludedCustomTests().at(i).wc_str() }, false).c_str()).mb_str());
        readabilityTestSection->InsertEndChild(customTest);
        }
    projectSettings->InsertEndChild(readabilityTestSection);

    // text views
    auto textViewsSection = doc.NewElement(XML_TEXT_VIEWS_SECTION.data());
    // how highlighting is done
    auto highlightMethod = doc.NewElement(XML_HIGHLIGHT_METHOD.data());
    highlightMethod->SetAttribute(XML_METHOD.data(), static_cast<int>(m_textHighlight));
    textViewsSection->InsertEndChild(highlightMethod);

    // highlighting information
    auto highlight = doc.NewElement(XML_HIGHLIGHTCOLOR.data());
    highlight->SetAttribute(XmlFormat::GetRed().mb_str(), m_textHighlightColor.Red());
    highlight->SetAttribute(XmlFormat::GetGreen().mb_str(), m_textHighlightColor.Green());
    highlight->SetAttribute(XmlFormat::GetBlue().mb_str(), m_textHighlightColor.Blue());
    textViewsSection->InsertEndChild(highlight);

    // highlight color for wordy items
    auto wordyPhrasesHighlight = doc.NewElement(XML_WORDY_PHRASE_HIGHLIGHTCOLOR.data());
    wordyPhrasesHighlight->SetAttribute(XmlFormat::GetRed().mb_str(),
                                        m_wordyPhraseHighlightColor.Red());
    wordyPhrasesHighlight->SetAttribute(XmlFormat::GetGreen().mb_str(),
                                        m_wordyPhraseHighlightColor.Green());
    wordyPhrasesHighlight->SetAttribute(XmlFormat::GetBlue().mb_str(),
                                        m_wordyPhraseHighlightColor.Blue());
    textViewsSection->InsertEndChild(wordyPhrasesHighlight);

    // highlight color for repeated words
    auto dupWordsHighlight = doc.NewElement(XML_DUP_WORD_HIGHLIGHTCOLOR.data());
    dupWordsHighlight->SetAttribute(XmlFormat::GetRed().mb_str(),
                                    m_duplicateWordHighlightColor.Red());
    dupWordsHighlight->SetAttribute(XmlFormat::GetGreen().mb_str(),
                                    m_duplicateWordHighlightColor.Green());
    dupWordsHighlight->SetAttribute(XmlFormat::GetBlue().mb_str(),
                                    m_duplicateWordHighlightColor.Blue());
    textViewsSection->InsertEndChild(dupWordsHighlight);

    // highlight color for dolch words
    auto dolchConjunctionsHighlight = doc.NewElement(XML_DOLCH_CONJUNCTIONS_HIGHLIGHTCOLOR.data());
    dolchConjunctionsHighlight->SetAttribute(XmlFormat::GetRed().mb_str(),
                                             GetDolchConjunctionsColor().Red());
    dolchConjunctionsHighlight->SetAttribute(XmlFormat::GetGreen().mb_str(),
                                             GetDolchConjunctionsColor().Green());
    dolchConjunctionsHighlight->SetAttribute(XmlFormat::GetBlue().mb_str(),
                                             GetDolchConjunctionsColor().Blue());
    dolchConjunctionsHighlight->SetAttribute(XML_INCLUDE.data(),
                                             bool_to_int(IsHighlightingDolchConjunctions()));
    textViewsSection->InsertEndChild(dolchConjunctionsHighlight);

    auto dolchPrepositionsHighlight = doc.NewElement(XML_DOLCH_PREPOSITIONS_HIGHLIGHTCOLOR.data());
    dolchPrepositionsHighlight->SetAttribute(XmlFormat::GetRed().mb_str(),
                                             GetDolchPrepositionsColor().Red());
    dolchPrepositionsHighlight->SetAttribute(XmlFormat::GetGreen().mb_str(),
                                             GetDolchPrepositionsColor().Green());
    dolchPrepositionsHighlight->SetAttribute(XmlFormat::GetBlue().mb_str(),
                                             GetDolchPrepositionsColor().Blue());
    dolchPrepositionsHighlight->SetAttribute(XML_INCLUDE.data(),
                                             bool_to_int(IsHighlightingDolchPrepositions()));
    textViewsSection->InsertEndChild(dolchPrepositionsHighlight);

    auto dolchPronounHighlight = doc.NewElement(XML_DOLCH_PRONOUNS_HIGHLIGHTCOLOR.data());
    dolchPronounHighlight->SetAttribute(XmlFormat::GetRed().mb_str(),
                                        GetDolchPronounsColor().Red());
    dolchPronounHighlight->SetAttribute(XmlFormat::GetGreen().mb_str(),
                                        GetDolchPronounsColor().Green());
    dolchPronounHighlight->SetAttribute(XmlFormat::GetBlue().mb_str(),
                                        GetDolchPronounsColor().Blue());
    dolchPronounHighlight->SetAttribute(XML_INCLUDE.data(),
                                        bool_to_int(IsHighlightingDolchPronouns()));
    textViewsSection->InsertEndChild(dolchPronounHighlight);

    auto dolchAdverbHighlight = doc.NewElement(XML_DOLCH_ADVERBS_HIGHLIGHTCOLOR.data());
    dolchAdverbHighlight->SetAttribute(XmlFormat::GetRed().mb_str(), GetDolchAdverbsColor().Red());
    dolchAdverbHighlight->SetAttribute(XmlFormat::GetGreen().mb_str(),
                                       GetDolchAdverbsColor().Green());
    dolchAdverbHighlight->SetAttribute(XmlFormat::GetBlue().mb_str(),
                                       GetDolchAdverbsColor().Blue());
    dolchAdverbHighlight->SetAttribute(XML_INCLUDE.data(),
                                       bool_to_int(IsHighlightingDolchAdverbs()));
    textViewsSection->InsertEndChild(dolchAdverbHighlight);

    auto dolchAdjectiveHighlight = doc.NewElement(XML_DOLCH_ADJECTIVES_HIGHLIGHTCOLOR.data());
    dolchAdjectiveHighlight->SetAttribute(XmlFormat::GetRed().mb_str(),
                                          GetDolchAdjectivesColor().Red());
    dolchAdjectiveHighlight->SetAttribute(XmlFormat::GetGreen().mb_str(),
                                          GetDolchAdjectivesColor().Green());
    dolchAdjectiveHighlight->SetAttribute(XmlFormat::GetBlue().mb_str(),
                                          GetDolchAdjectivesColor().Blue());
    dolchAdjectiveHighlight->SetAttribute(XML_INCLUDE.data(),
                                          bool_to_int(IsHighlightingDolchAdjectives()));
    textViewsSection->InsertEndChild(dolchAdjectiveHighlight);

    auto dolchVerbHighlight = doc.NewElement(XML_DOLCH_VERBS_HIGHLIGHTCOLOR.data());
    dolchVerbHighlight->SetAttribute(XmlFormat::GetRed().mb_str(), GetDolchVerbsColor().Red());
    dolchVerbHighlight->SetAttribute(XmlFormat::GetGreen().mb_str(), GetDolchVerbsColor().Green());
    dolchVerbHighlight->SetAttribute(XmlFormat::GetBlue().mb_str(), GetDolchVerbsColor().Blue());
    dolchVerbHighlight->SetAttribute(XML_INCLUDE.data(), bool_to_int(IsHighlightingDolchVerbs()));
    textViewsSection->InsertEndChild(dolchVerbHighlight);

    auto dolchNounHighlight = doc.NewElement(XML_DOLCH_NOUNS_HIGHLIGHTCOLOR.data());
    dolchNounHighlight->SetAttribute(XmlFormat::GetRed().mb_str(), GetDolchNounColor().Red());
    dolchNounHighlight->SetAttribute(XmlFormat::GetGreen().mb_str(), GetDolchNounColor().Green());
    dolchNounHighlight->SetAttribute(XmlFormat::GetBlue().mb_str(), GetDolchNounColor().Blue());
    dolchNounHighlight->SetAttribute(XML_INCLUDE.data(), bool_to_int(IsHighlightingDolchNouns()));
    textViewsSection->InsertEndChild(dolchNounHighlight);

    // highlight color for excluded text
    auto excludedHighlight = doc.NewElement(XML_EXCLUDED_HIGHLIGHTCOLOR.data());
    excludedHighlight->SetAttribute(XmlFormat::GetRed().mb_str(),
                                    m_excludedTextHighlightColor.Red());
    excludedHighlight->SetAttribute(XmlFormat::GetGreen().mb_str(),
                                    m_excludedTextHighlightColor.Green());
    excludedHighlight->SetAttribute(XmlFormat::GetBlue().mb_str(),
                                    m_excludedTextHighlightColor.Blue());
    textViewsSection->InsertEndChild(excludedHighlight);

    // document display font information
    auto fontcolor = doc.NewElement(XML_DOCUMENT_DISPLAY_FONTCOLOR.data());
    fontcolor->SetAttribute(XmlFormat::GetRed().mb_str(), m_fontColor.Red());
    fontcolor->SetAttribute(XmlFormat::GetGreen().mb_str(), m_fontColor.Green());
    fontcolor->SetAttribute(XmlFormat::GetBlue().mb_str(), m_fontColor.Blue());
    textViewsSection->InsertEndChild(fontcolor);

    auto font = doc.NewElement(XML_DOCUMENT_DISPLAY_FONT.data());
    font->SetAttribute(XmlFormat::GetFontPointSize().mb_str(), m_textViewFont.GetPointSize());
    font->SetAttribute(XmlFormat::GetFontStyle().mb_str(),
                       static_cast<int>(m_textViewFont.GetStyle()));
    font->SetAttribute(XmlFormat::GetFontWeight().mb_str(),
                       static_cast<int>(m_textViewFont.GetWeight()));
    font->SetAttribute(XmlFormat::GetFontUnderline().mb_str(),
                       bool_to_int(m_textViewFont.GetUnderlined()));
    font->SetAttribute(
        XmlFormat::GetFontFaceName().mb_str(),
        wxString(encode({ m_textViewFont.GetFaceName().wc_str() }, false).c_str()).mb_str());
    textViewsSection->InsertEndChild(font);

    projectSettings->InsertEndChild(textViewsSection);

    configSection->InsertEndChild(projectSettings);

    root->InsertEndChild(configSection);

    doc.InsertEndChild(root);
    doc.SaveFile(optionsFile.empty() ? m_optionsFile.mb_str() : optionsFile.mb_str());
    if (doc.Error())
        {
        wxMessageBox(wxString::Format(_(L"Unable to save configuration file:%s"), doc.ErrorStr()),
                     _(L"Error"), wxOK | wxICON_ERROR);
        return false;
        }
    return true;
    }

//--------------------------------------------
double ReadabilityAppOptions::TiXmlNodeToDouble(const tinyxml2::XMLNode* node,
                                                const wxString& tagToRead)
    {
    if (node == nullptr)
        {
        return std::numeric_limits<double>::quiet_NaN();
        }

    lily_of_the_valley::html_extract_text filter_html;

    const char* stringData = node->ToElement()->Attribute(tagToRead.mb_str());
    if (!stringData)
        {
        return std::numeric_limits<double>::quiet_NaN();
        }
    auto convertedStr =
        Wisteria::TextStream::CharStreamToUnicode(stringData, std::strlen(stringData));
    const wchar_t* filteredText =
        filter_html(convertedStr.c_str(), convertedStr.length(), true, false);
    if (!filteredText)
        {
        return std::numeric_limits<double>::quiet_NaN();
        }
    wxString dValueStr(filteredText);
    double dValue{ 0 };
    if (dValueStr.ToCDouble(&dValue))
        {
        return dValue;
        }
    else
        {
        return std::numeric_limits<double>::quiet_NaN();
        }
    }

//--------------------------------------------
wxString ReadabilityAppOptions::TiXmlNodeToString(const tinyxml2::XMLNode* node,
                                                  const wxString& tagToRead)
    {
    if (node == nullptr)
        {
        return wxString{};
        }

    lily_of_the_valley::html_extract_text filter_html;

    const char* stringData = node->ToElement()->Attribute(tagToRead.mb_str());
    if (!stringData)
        {
        return wxString{};
        }
    auto convertedStr =
        Wisteria::TextStream::CharStreamToUnicode(stringData, std::strlen(stringData));
    const wchar_t* filteredText =
        filter_html(convertedStr.c_str(), convertedStr.length(), true, false);
    if (!filteredText)
        {
        return wxString{};
        }
    return wxString(filteredText);
    }

//------------------------------------------------
wxString ReadabilityAppOptions::GetDocumentFilter()
    {
    const wxString TextFilter = _DT(L"*.txt");
    const wxString HtmlFiles = _DT(L"*.htm;*.html;*.xhtml;*.sgml;*.php;*.php3;*.php4;*.aspx;*.asp");
    const wxString WordFilter = _DT(L"*.doc;*.docx;*.docm;*.dot;*.wri");
    const wxString PowerPointFilter = _DT(L"*.pptx;*.pptm");
    const wxString OpenDocPrezFilter = _DT(L"*.odp;*.otp");
    const wxString OpenDocFilter = _DT(L"*.odt;*.ott");
    const wxString RtfFilter = _DT(L"*.rtf");
    const wxString MarkdownFilter = _DT(L"*.md;*.qmd;*.rmd");
    const wxString PsFilter = _DT(L"*.ps");
    const wxString IdlFilter = _DT(L"*.idl");
    const wxString CppFilter = _DT(L"*.cpp;*.c;*.h");
    const wxString AllDocumentsFilter = wxString::Format(
        _(L"Documents (%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s)|%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s"),
        TextFilter, HtmlFiles, WordFilter, PowerPointFilter, OpenDocPrezFilter, OpenDocFilter,
        RtfFilter, MarkdownFilter, PsFilter, IdlFilter, CppFilter, TextFilter, HtmlFiles,
        WordFilter, PowerPointFilter, OpenDocPrezFilter, OpenDocFilter, RtfFilter, MarkdownFilter,
        PsFilter, IdlFilter, CppFilter);
    return wxString::Format(
        _(L"%s|Text files (%s)|%s|HTML files (%s)|%s|Word files (%s)|%s|"
          "PowerPoint files (%s)|%s|OpenDocument Presentation files (%s)|%s|"
          "OpenDocument files (%s)|%s|Rich Text files (%s)|%s|"
          "Markdown files (%s)|%s|Postscript files (%s)|%s|"
          "Interface Definition Language files (%s)|%s|C++ source files (%s)|%s"),
        AllDocumentsFilter, TextFilter, TextFilter, HtmlFiles, HtmlFiles, WordFilter, WordFilter,
        PowerPointFilter, PowerPointFilter, OpenDocPrezFilter, OpenDocPrezFilter, OpenDocFilter,
        OpenDocFilter, RtfFilter, RtfFilter, MarkdownFilter, MarkdownFilter, PsFilter, PsFilter,
        IdlFilter, IdlFilter, CppFilter, CppFilter);
    }

//------------------------------------------------
void ReadabilityAppOptions::UpdateGraphOptions(Wisteria::Canvas* graphCanvas)
    {
    if (graphCanvas == nullptr)
        {
        return;
        }
    // load the graph images
    if (wxFile::Exists(GetPlotBackGroundImagePath()))
        {
        m_graphBackgroundImage =
            wxBitmapBundle(wxGetApp()
                               .GetResourceManager()
                               .GetBitmap(GetPlotBackGroundImagePath(), wxBITMAP_TYPE_ANY)
                               .ConvertToImage());
        }
    if (wxFile::Exists(GetWatermarkLogo()))
        {
        m_waterMarkImage = wxBitmapBundle(wxGetApp()
                                              .GetResourceManager()
                                              .GetBitmap(GetWatermarkLogo(), wxBITMAP_TYPE_ANY)
                                              .ConvertToImage());
        }
    if (wxFile::Exists(GetStippleImagePath()))
        {
        m_graphStippleImage =
            wxBitmapBundle(wxGetApp()
                               .GetResourceManager()
                               .GetBitmap(GetStippleImagePath(), wxBITMAP_TYPE_ANY)
                               .ConvertToImage());
        }

    graphCanvas->SetExportResources(wxGetApp().GetMainFrame()->GetHelpDirectory(),
                                    L"image-export.html");
    graphCanvas->SetPrinterSettings(*wxGetApp().GetPrintData());
    graphCanvas->SetLeftPrinterHeader(ReadabilityAppOptions::GetLeftPrinterHeader());
    graphCanvas->SetCenterPrinterHeader(ReadabilityAppOptions::GetCenterPrinterHeader());
    graphCanvas->SetRightPrinterHeader(ReadabilityAppOptions::GetRightPrinterHeader());
    graphCanvas->SetLeftPrinterFooter(ReadabilityAppOptions::GetLeftPrinterFooter());
    graphCanvas->SetCenterPrinterFooter(ReadabilityAppOptions::GetCenterPrinterFooter());
    graphCanvas->SetRightPrinterFooter(ReadabilityAppOptions::GetRightPrinterFooter());

    graphCanvas->SetBackgroundColor(GetBackGroundColor(), GetGraphBackGroundLinearGradient());
    graphCanvas->SetBackgroundImage(m_graphBackgroundImage, GetPlotBackGroundImageOpacity());

    auto plot = std::dynamic_pointer_cast<Graph2D>(graphCanvas->GetFixedObject(0, 0));
    plot->SetPlotBackgroundColor(Colors::ColorContrast::ChangeOpacity(
        GetPlotBackGroundColor(), GetPlotBackGroundColorOpacity()));

    plot->SetStippleBrush(m_graphStippleImage);
    plot->SetImageScheme(m_graphImageScheme);
    graphCanvas->SetWatermarkLogo(m_waterMarkImage, wxSize(100, 100));
    graphCanvas->SetWatermark(GetWatermark());
    plot->GetBottomXAxis().SetFont(GetXAxisFont());
    plot->GetBottomXAxis().SetFontColor(GetXAxisFontColor());
    plot->GetLeftYAxis().SetFont(GetYAxisFont());
    plot->GetLeftYAxis().SetFontColor(GetYAxisFontColor());
    for (size_t i = 0; i < plot->GetCustomAxes().size(); ++i)
        {
        plot->GetCustomAxes().at(i).SetFont(GetYAxisFont());
        plot->GetCustomAxes().at(i).SetFontColor(GetYAxisFontColor());
        }
    for (size_t i = 0; i < graphCanvas->GetTopTitles().size(); ++i)
        {
        graphCanvas->GetTopTitles().at(i).GetFont() = GetGraphTopTitleFont();
        graphCanvas->GetTopTitles().at(i).SetFontColor(GetGraphTopTitleFontColor());
        }
    for (size_t i = 0; i < graphCanvas->GetBottomTitles().size(); ++i)
        {
        graphCanvas->GetBottomTitles().at(i).GetFont() = GetBottomTitleGraphFont();
        graphCanvas->GetBottomTitles().at(i).SetFontColor(GetBottomTitleGraphFontColor());
        }
    for (size_t i = 0; i < graphCanvas->GetLeftTitles().size(); ++i)
        {
        graphCanvas->GetLeftTitles().at(i).GetFont() = GetLeftTitleGraphFont();
        graphCanvas->GetLeftTitles().at(i).SetFontColor(GetLeftTitleGraphFontColor());
        }
    for (size_t i = 0; i < graphCanvas->GetRightTitles().size(); ++i)
        {
        graphCanvas->GetRightTitles().at(i).GetFont() = GetRightTitleGraphFont();
        graphCanvas->GetRightTitles().at(i).SetFontColor(GetRightTitleGraphFontColor());
        }
    }
