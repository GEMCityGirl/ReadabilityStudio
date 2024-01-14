#include <wx/colordlg.h>
#include <wx/numdlg.h>
#include <wx/choicdlg.h>
#include "base_project_view.h"
#include "batch_project_doc.h"
#include "standard_project_doc.h"
#include "../app/readability_app.h"
#include "../ui/dialogs/test_bundle_dlg.h"
#include "../ui/dialogs/tools_options_dlg.h"
#include "../ui/dialogs/edit_word_list_dlg.h"
#include "../Wisteria-Dataviz/src/ui/dialogs/opacitydlg.h"
#include "../Wisteria-Dataviz/src/ui/ribbon/artmetro.h"

wxDECLARE_APP(ReadabilityApp);

wxIMPLEMENT_DYNAMIC_CLASS(BaseProjectView, wxView);

IdRange BaseProjectView::m_customTestSidebarIdRange(SIDEBAR_CUSTOM_TESTS_START_ID, 1000);

using namespace Wisteria;
using namespace Wisteria::Graphs;
using namespace Wisteria::GraphItems;
using namespace Wisteria::UI;

wxBEGIN_EVENT_TABLE(BaseProjectView, wxView)
    EVT_RIBBONBUTTONBAR_CLICKED(XRCID("ID_DOCUMENT_REFRESH"), BaseProjectView::OnDocumentRefresh)
    EVT_MENU(XRCID("ID_DOCUMENT_REFRESH"), BaseProjectView::OnDocumentRefreshMenu)
    EVT_RIBBONBUTTONBAR_CLICKED(wxID_PROPERTIES, BaseProjectView::OnProjectSettings)
    EVT_RIBBONBUTTONBAR_CLICKED(XRCID("ID_EDIT_STATS_REPORT"), BaseProjectView::OnEditStatsReportButton)
    EVT_MENU(XRCID("ID_LS_LONGER_THAN"), BaseProjectView::OnLongSentencesOptions)
    EVT_MENU(XRCID("ID_LS_OUTLIER_RANGE"), BaseProjectView::OnLongSentencesOptions)
    EVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED(XRCID("ID_SENTENCE_LENGTHS"), BaseProjectView::OnLongSentences)
    EVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED(XRCID("ID_NUMERAL_SYLLABICATION"), BaseProjectView::OnNumeralSyllabication)
    EVT_MENU(XRCID("ID_NUMSYL_ONE"), BaseProjectView::OnNumeralSyllabicationOptions)
    EVT_MENU(XRCID("ID_NUMSYL_EACH_DIGIT"), BaseProjectView::OnNumeralSyllabicationOptions)
    EVT_MENU(XRCID("ID_TE_ALL_INCOMPLETE"), BaseProjectView::OnTextExclusionOptions)
    EVT_MENU(XRCID("ID_TE_NO_EXCLUDE"), BaseProjectView::OnTextExclusionOptions)
    EVT_MENU(XRCID("ID_TE_ALL_INCOMPLETE_EXCEPT_HEADERS"), BaseProjectView::OnTextExclusionOptions)
    EVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED(XRCID("ID_TEXT_EXCLUSION"), BaseProjectView::OnTextExclusion)
    EVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED(XRCID("ID_EXCLUSION_TAGS"), BaseProjectView::OnExclusionTags)
    EVT_MENU(XRCID("ID_EXCLUSION_TAGS_NOT_ENABLED"), BaseProjectView::OnExclusionTagsOptions)
    EVT_MENU(XRCID("ID_EXCLUSION_TAGS_CAROTS"), BaseProjectView::OnExclusionTagsOptions)
    EVT_MENU(XRCID("ID_EXCLUSION_TAGS_ANGLES"), BaseProjectView::OnExclusionTagsOptions)
    EVT_MENU(XRCID("ID_EXCLUSION_TAGS_BRACES"), BaseProjectView::OnExclusionTagsOptions)
    EVT_MENU(XRCID("ID_EXCLUSION_TAGS_CURLIES"), BaseProjectView::OnExclusionTagsOptions)
    EVT_MENU(XRCID("ID_EXCLUSION_TAGS_PARANS"), BaseProjectView::OnExclusionTagsOptions)
    EVT_MENU(XRCID("ID_LE_ONLY_AFTER_VALID_SENTENCE"), BaseProjectView::OnLineEndOptions)
    EVT_MENU(XRCID("ID_LE_ALWAYS_NEW_PARAGRAPH"), BaseProjectView::OnLineEndOptions)
    EVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED(XRCID("ID_LINE_ENDS"), BaseProjectView::OnLineEnds)
    EVT_RIBBONBUTTONBAR_CLICKED(XRCID("ID_IGNORE_BLANK_LINES"), BaseProjectView::OnIgnoreBlankLines)
    EVT_RIBBONBUTTONBAR_CLICKED(XRCID("ID_IGNORE_INDENTING"), BaseProjectView::OnIgnoreIdenting)
    EVT_RIBBONBUTTONBAR_CLICKED(XRCID("ID_SENTENCES_CAPITALIZED"), BaseProjectView::OnStrictCapitalization)
    EVT_RIBBONBUTTONBAR_CLICKED(XRCID("ID_EXCLUDE_AGGRESSIVELY"), BaseProjectView::OnAggressivelyExclude)
    EVT_RIBBONBUTTONBAR_CLICKED(XRCID("ID_EXCLUDE_COPYRIGHT_NOTICES"), BaseProjectView::OnIgnoreCopyrights)
    EVT_RIBBONBUTTONBAR_CLICKED(XRCID("ID_EXCLUDE_TRAILING_CITATIONS"), BaseProjectView::OnIgnoreCitations)
    EVT_RIBBONBUTTONBAR_CLICKED(XRCID("ID_EXCLUDE_FILE_ADDRESSES"), BaseProjectView::OnIgnoreFileAddresses)
    EVT_RIBBONBUTTONBAR_CLICKED(XRCID("ID_EXCLUDE_NUMERALS"), BaseProjectView::OnIgnoreNumerals)
    EVT_RIBBONBUTTONBAR_CLICKED(XRCID("ID_EXCLUDE_PROPER_NOUNS"), BaseProjectView::OnIgnoreProperNouns)
    EVT_RIBBONBUTTONBAR_CLICKED(XRCID("ID_EXCLUDE_WORD_LIST"), BaseProjectView::OnExcludeWordsList)
    EVT_RIBBONBUTTONBAR_CLICKED(XRCID("ID_INCOMPLETE_THRESHOLD"), BaseProjectView::OnIncompleteThreshold)
    EVT_MENU(XRCID("ID_EDIT_DICTIONARY"), BaseProjectView::OnEditDictionary)
    EVT_MENU(XRCID("ID_EDIT_ENGLISH_DICTIONARY"), BaseProjectView::OnEditEnglishDictionary)
    EVT_RIBBONBUTTONBAR_CLICKED(XRCID("ID_DROP_SHADOW"), BaseProjectView::OnDropShadow)
    EVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED(wxID_ZOOM_IN, BaseProjectView::OnZoomButton)
    EVT_RIBBONBUTTONBAR_CLICKED(XRCID("ID_EDIT_WATERMARK"), BaseProjectView::OnGraphWatermark)
    EVT_RIBBONBUTTONBAR_CLICKED(XRCID("ID_EDIT_LOGO"), BaseProjectView::OnGraphLogo)
    EVT_RIBBONBUTTONBAR_CLICKED(XRCID("ID_USE_ENGLISH_LABELS"), BaseProjectView::OnEnglishLabels)
    EVT_RIBBONBUTTONBAR_CLICKED(XRCID("ID_INVALID_REGION_COLOR"), BaseProjectView::OnInvalidRegionColor)
    EVT_RIBBONBUTTONBAR_CLICKED(XRCID("ID_FLESCH_DISPLAY_LINES"), BaseProjectView::OnFleschConnectLinesButton)
    EVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED(XRCID("ID_EDIT_BAR_ORIENTATION"), BaseProjectView::OnBarOrientationButton)
    EVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED(XRCID("ID_EDIT_BAR_STYLE"), BaseProjectView::OnBarStyleButton)
    EVT_RIBBONBUTTONBAR_CLICKED(XRCID("ID_EDIT_BAR_LABELS"), BaseProjectView::OnBarLabelsButton)
    EVT_MENU(XRCID("ID_EDIT_BAR_COLOR"), BaseProjectView::OnEditGraphColor)
    EVT_MENU(XRCID("ID_EDIT_BAR_OPACITY"), BaseProjectView::OnEditGraphOpacity)
    EVT_MENU(XRCID("ID_BAR_HORIZONTAL"), BaseProjectView::OnBarOrientationSelected)
    EVT_MENU(XRCID("ID_BAR_VERTICAL"), BaseProjectView::OnBarOrientationSelected)
    EVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED(XRCID("ID_EDIT_HISTOBAR_LABELS"), BaseProjectView::OnHistoBarsLabelsButton)
    EVT_MENU(XRCID("ID_HISTOBAR_LABELS_PERCENTAGE"), BaseProjectView::OnHistoBarLabelSelected)
    EVT_MENU(XRCID("ID_HISTOBAR_LABELS_COUNT"), BaseProjectView::OnHistoBarLabelSelected)
    EVT_MENU(XRCID("ID_HISTOBAR_LABELS_COUNT_AND_PERCENT"), BaseProjectView::OnHistoBarLabelSelected)
    EVT_MENU(XRCID("ID_HISTOBAR_NO_LABELS"), BaseProjectView::OnHistoBarLabelSelected)
    EVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED(XRCID("ID_EDIT_HISTOGRAM_BAR_STYLE"), BaseProjectView::OnHistoBarStyleButton)
    EVT_MENU(XRCID("ID_EDIT_HISTOGRAM_BAR_COLOR"), BaseProjectView::OnEditGraphColor)
    EVT_MENU(XRCID("ID_EDIT_HISTOBAR_OPACITY"), BaseProjectView::OnEditGraphOpacity)
    EVT_RIBBONBUTTONBAR_CLICKED(XRCID("ID_BOX_PLOT_DISPLAY_ALL_POINTS"), BaseProjectView::OnBoxPlotShowAllPointsButton)
    EVT_RIBBONBUTTONBAR_CLICKED(XRCID("ID_BOX_PLOT_DISPLAY_LABELS"), BaseProjectView::OnBoxPlotShowLabelsButton)
    EVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED(XRCID("ID_EDIT_BOX_STYLE"), BaseProjectView::OnBoxStyleButton)
    EVT_MENU(XRCID("ID_EDIT_BOX_COLOR"), BaseProjectView::OnEditGraphColor)
    EVT_MENU(XRCID("ID_EDIT_BOX_OPACITY"), BaseProjectView::OnEditGraphOpacity)
    EVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED(
        XRCID("ID_EDIT_GRAPH_BACKGROUND"), BaseProjectView::OnEditGraphBackgroundButton)
    EVT_MENU(XRCID("ID_EDIT_PLOT_BKIMAGE_OPACITY"), BaseProjectView::OnEditGraphOpacity)
    EVT_MENU(XRCID("ID_EDIT_PLOT_BKCOLOR"), BaseProjectView::OnEditGraphColor)
    EVT_MENU(XRCID("ID_EDIT_PLOT_BKCOLOR_OPACITY"), BaseProjectView::OnEditGraphOpacity)
    EVT_MENU(XRCID("ID_EDIT_GRAPH_BKCOLOR"), BaseProjectView::OnEditGraphColor)
    EVT_MENU(XRCID("ID_EDIT_PLOT_BKIMAGE"), BaseProjectView::OnEditPlotBackgroundImage)
    EVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED(XRCID("ID_EDIT_GRAPH_FONTS"), BaseProjectView::OnEditGraphFontsButton)
    EVT_MENU(XRCID("ID_EDIT_Y_AXIS_FONT"), BaseProjectView::OnEditGraphFont)
    EVT_MENU(XRCID("ID_EDIT_X_AXIS_FONT"), BaseProjectView::OnEditGraphFont)
    EVT_MENU(XRCID("ID_EDIT_TOP_TITLES_FONT"), BaseProjectView::OnEditGraphFont)
    EVT_MENU(XRCID("ID_EDIT_BOTTOM_TITLES_FONT"), BaseProjectView::OnEditGraphFont)
    EVT_MENU(XRCID("ID_EDIT_LEFT_TITLES_FONT"), BaseProjectView::OnEditGraphFont)
    EVT_MENU(XRCID("ID_EDIT_RIGHT_TITLES_FONT"), BaseProjectView::OnEditGraphFont)
    EVT_RIBBONBUTTONBAR_CLICKED(XRCID("ID_EDIT_DICTIONARY"), BaseProjectView::OnEditDictionaryButton)
    EVT_MENU(XRCID("ID_EDIT_DICTIONARY_PROJECT_SETTINGS"), BaseProjectView::OnEditDictionaryProjectSettings)
    EVT_MENU(XRCID("ID_ADD_CUSTOM_TEST_BUNDLE"), BaseProjectView::OnAddCustomTestBundle)
    EVT_MENU(XRCID("ID_EDIT_CUSTOM_TEST_BUNDLE"), BaseProjectView::OnEditCustomTestBundle)
    EVT_MENU(XRCID("ID_REMOVE_CUSTOM_TEST_BUNDLE"), BaseProjectView::OnRemoveCustomTestBundle)
    EVT_MENU(XRCID("ID_ADD_CHILDRENS_PUBLISHING_TEST_BUNDLE"), BaseProjectView::OnTestBundle)
    EVT_MENU(XRCID("ID_ADD_ADULT_PUBLISHING_TEST_BUNDLE"), BaseProjectView::OnTestBundle)
    EVT_MENU(XRCID("ID_ADD_CHILDRENS_HEALTHCARE_TEST_BUNDLE"), BaseProjectView::OnTestBundle)
    EVT_MENU(XRCID("ID_ADD_ADULT_HEALTHCARE_TEST_BUNDLE"), BaseProjectView::OnTestBundle)
    EVT_MENU(XRCID("ID_ADD_MILITARY_COVERNMENT_TEST_BUNDLE"), BaseProjectView::OnTestBundle)
    EVT_MENU(XRCID("ID_ADD_SECOND_LANGUAGE_TEST_BUNDLE"), BaseProjectView::OnTestBundle)
    EVT_MENU(XRCID("ID_ADD_BROADCASTING_TEST_BUNDLE"), BaseProjectView::OnTestBundle)
    EVT_MENU(XRCID("ID_GENERAL_DOCUMENT_TEST_BUNDLE"), BaseProjectView::OnTestBundle)
    EVT_MENU(XRCID("ID_TECHNICAL_DOCUMENT_TEST_BUNDLE"), BaseProjectView::OnTestBundle)
    EVT_MENU(XRCID("ID_NONNARRATIVE_DOCUMENT_TEST_BUNDLE"), BaseProjectView::OnTestBundle)
    EVT_MENU(XRCID("ID_ADULT_LITERATURE_DOCUMENT_TEST_BUNDLE"), BaseProjectView::OnTestBundle)
    EVT_MENU(XRCID("ID_CHILDRENS_LITERATURE_DOCUMENT_TEST_BUNDLE"), BaseProjectView::OnTestBundle)
    EVT_MENU(XRCID("ID_ADD_CUSTOM_TEST"), BaseProjectView::OnAddCustomTest)
    EVT_MENU(XRCID("ID_ADD_CUSTOM_NEW_DALE_CHALL_TEST"), BaseProjectView::OnAddCustomTest)
    EVT_MENU(XRCID("ID_ADD_CUSTOM_SPACHE_TEST"), BaseProjectView::OnAddCustomTest)
    EVT_MENU(XRCID("ID_ADD_CUSTOM_HARRIS_JACOBSON_TEST"), BaseProjectView::OnAddCustomTest)
    EVT_MENU(XRCID("ID_EDIT_CUSTOM_TEST"), BaseProjectView::OnEditCustomTest)
    EVT_MENU(XRCID("ID_REMOVE_CUSTOM_TEST"), BaseProjectView::OnRemoveCustomTest)
    EVT_MENU(wxID_FIND, BaseProjectView::OnFindMenu)
    EVT_MENU(XRCID("ID_FIND_NEXT"), BaseProjectView::OnFindNext)
    EVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED(XRCID("ID_WORD_LISTS"), BaseProjectView::OnWordListDropdown)
    EVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED(XRCID("ID_BLANK_GRAPHS"), BaseProjectView::OnBlankGraphDropdown)
    EVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED(wxID_OPEN, BaseProjectView::OnOpenDropdown)
    EVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED(XRCID("ID_SAVE_OPTIONS"), BaseProjectView::OnSaveDropdown)
    EVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED(wxID_COPY, BaseProjectView::OnCopyDropdown)
    EVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED(XRCID("ID_EDIT_DICTIONARY"), BaseProjectView::OnDictionaryDropdown)
    EVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED(XRCID("ID_GRAPH_SORT"), BaseProjectView::OnGraphSortDropdown)
    EVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED(wxID_PRINT, BaseProjectView::OnPrintDropdown)
    EVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED(
        XRCID("ID_PRIMARY_AGE_TESTS_BUTTON"), BaseProjectView::OnPrimaryAgeTestsDropdown)
    EVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED(
        XRCID("ID_SECONDARY_AGE_TESTS_BUTTON"), BaseProjectView::OnSecondaryAgeTestsDropdown)
    EVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED(
        XRCID("ID_ADULT_TESTS_BUTTON"), BaseProjectView::OnAdultTestsDropdown)
    EVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED(
        XRCID("ID_SECOND_LANGUAGE_TESTS_BUTTON"), BaseProjectView::OnSecondLanguageTestsDropdown)
    EVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED(XRCID("ID_CUSTOM_TESTS"), BaseProjectView::OnCustomTestsDropdown)
    EVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED(XRCID("ID_TEST_BUNDLES"), BaseProjectView::OnTestBundlesDropdown)
    EVT_RIBBONBAR_TAB_LEFT_DCLICK(wxID_ANY, BaseProjectView::OnDClickRibbonBar)
    EVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED(XRCID("ID_GRADE_SCALES"), BaseProjectView::OnGradeScaleDropdown)
    EVT_RIBBONBAR_PAGE_CHANGED(wxID_ANY, BaseProjectView::OnClickRibbonBar)
wxEND_EVENT_TABLE()

//---------------------------------------------------
BaseProjectView::BaseProjectView()
    {
    Bind(wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
            {
            m_sidebarShown = !m_sidebarShown;
            ShowSideBar(m_sidebarShown);
            },
        XRCID("ID_SHOW_SIDEBAR"));

    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED,
        [this]([[maybe_unused]] wxCommandEvent&)
            {
            m_sidebarShown = !m_sidebarShown;
            ShowSideBar(m_sidebarShown);
            },
        XRCID("ID_SHOW_SIDEBAR"));

    Bind(wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
            {
            GetRibbon()->ShowPanels(
                GetRibbon()->GetDisplayMode() == wxRIBBON_BAR_MINIMIZED ?
                wxRIBBON_BAR_EXPANDED : wxRIBBON_BAR_MINIMIZED);
            },
        XRCID("ID_TOGGLE_RIBBON"));

    // menu & ribbon button events
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &BaseProjectView::OnEditGraphColorScheme, this,
        XRCID("ID_EDIT_GRAPH_COLOR_SCHEME"));

    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &BaseProjectView::OnShowcaseComplexWords, this,
        XRCID("ID_EDIT_GRAPH_SHOWCASE_COMPLEX_WORDS"));

    Bind(wxEVT_MENU, &BaseProjectView::OnBarStyleSelected, this,
        XRCID("ID_BAR_STYLE_SOLID"));
    Bind(wxEVT_MENU, &BaseProjectView::OnBarStyleSelected, this,
        XRCID("ID_BAR_STYLE_GLASS"));
    Bind(wxEVT_MENU, &BaseProjectView::OnBarStyleSelected, this,
        XRCID("ID_BAR_STYLE_BTOT"));
    Bind(wxEVT_MENU, &BaseProjectView::OnBarStyleSelected, this,
        XRCID("ID_BAR_STYLE_TTOB"));
    Bind(wxEVT_MENU, &BaseProjectView::OnBarStyleSelected, this,
        XRCID("ID_BAR_STYLE_STIPPLE_IMAGE"));
    Bind(wxEVT_MENU, &BaseProjectView::OnBarStyleSelected, this,
        XRCID("ID_BAR_STYLE_STIPPLE_SHAPE"));
    Bind(wxEVT_MENU, &BaseProjectView::OnBarStyleSelected, this,
        XRCID("ID_BAR_STYLE_WATERCOLOR"));
    Bind(wxEVT_MENU, &BaseProjectView::OnBarStyleSelected, this,
        XRCID("ID_BAR_STYLE_THICK_WATERCOLOR"));
    Bind(wxEVT_MENU, &BaseProjectView::OnBarStyleSelected, this,
        XRCID("ID_BAR_STYLE_COMMON_IMAGE"));

    Bind(wxEVT_MENU, &BaseProjectView::OnHistoBarStyleSelected, this,
        XRCID("ID_HISTOGRAM_BAR_STYLE_SOLID"));
    Bind(wxEVT_MENU, &BaseProjectView::OnHistoBarStyleSelected, this,
        XRCID("ID_HISTOGRAM_BAR_STYLE_GLASS"));
    Bind(wxEVT_MENU, &BaseProjectView::OnHistoBarStyleSelected, this,
        XRCID("ID_HISTOGRAM_BAR_STYLE_BTOT"));
    Bind(wxEVT_MENU, &BaseProjectView::OnHistoBarStyleSelected, this,
        XRCID("ID_HISTOGRAM_BAR_STYLE_TTOB"));
    Bind(wxEVT_MENU, &BaseProjectView::OnHistoBarStyleSelected, this,
        XRCID("ID_HISTOGRAM_BAR_STYLE_STIPPLE_IMAGE"));
    Bind(wxEVT_MENU, &BaseProjectView::OnHistoBarStyleSelected, this,
        XRCID("ID_HISTOGRAM_BAR_STYLE_STIPPLE_SHAPE"));
    Bind(wxEVT_MENU, &BaseProjectView::OnHistoBarStyleSelected, this,
        XRCID("ID_HISTOGRAM_BAR_STYLE_WATERCOLOR"));
    Bind(wxEVT_MENU, &BaseProjectView::OnHistoBarStyleSelected, this,
        XRCID("ID_HISTOGRAM_BAR_STYLE_THICK_WATERCOLOR"));
    Bind(wxEVT_MENU, &BaseProjectView::OnHistoBarStyleSelected, this,
        XRCID("ID_HISTOGRAM_BAR_STYLE_COMMON_IMAGE"));

    Bind(wxEVT_MENU, &BaseProjectView::OnBoxStyleSelected, this,
        XRCID("ID_BOX_STYLE_SOLID"));
    Bind(wxEVT_MENU, &BaseProjectView::OnBoxStyleSelected, this,
        XRCID("ID_BOX_STYLE_GLASS"));
    Bind(wxEVT_MENU, &BaseProjectView::OnBoxStyleSelected, this,
        XRCID("ID_BOX_STYLE_LTOR"));
    Bind(wxEVT_MENU, &BaseProjectView::OnBoxStyleSelected, this,
        XRCID("ID_BOX_STYLE_RTOL"));
    Bind(wxEVT_MENU, &BaseProjectView::OnBoxStyleSelected, this,
        XRCID("ID_BOX_STYLE_STIPPLE_IMAGE"));
    Bind(wxEVT_MENU, &BaseProjectView::OnBoxStyleSelected, this,
        XRCID("ID_BOX_STYLE_STIPPLE_SHAPE"));
    Bind(wxEVT_MENU, &BaseProjectView::OnBoxStyleSelected, this,
        XRCID("ID_BOX_STYLE_WATERCOLOR"));
    Bind(wxEVT_MENU, &BaseProjectView::OnBoxStyleSelected, this,
        XRCID("ID_BOX_STYLE_THICK_WATERCOLOR"));
    Bind(wxEVT_MENU, &BaseProjectView::OnBoxStyleSelected, this,
        XRCID("ID_BOX_STYLE_COMMON_IMAGE"));

    Bind(wxEVT_MENU, &BaseProjectView::OnBoxSelectStippleBrush, this,
        XRCID("ID_BOX_SELECT_STIPPLE_IMAGE"));
    Bind(wxEVT_MENU, &BaseProjectView::OnBoxSelectStippleShape, this,
        XRCID("ID_BOX_BAR_SELECT_STIPPLE_SHAPE"));
    Bind(wxEVT_MENU, &BaseProjectView::OnBoxSelectCommonImage, this,
        XRCID("ID_BOX_SELECT_COMMON_IMAGE"));

    Bind(wxEVT_MENU, &BaseProjectView::OnHistoBarSelectStippleBrush, this,
        XRCID("ID_HISTOGRAM_BAR_SELECT_BRUSH"));
    Bind(wxEVT_MENU, &BaseProjectView::OnHistoBarSelectStippleShape, this,
        XRCID("ID_HISTOGRAM_BAR_SELECT_STIPPLE_SHAPE"));
    Bind(wxEVT_MENU, &BaseProjectView::OnHistoBarSelectCommonImage, this,
        XRCID("ID_HISTOGRAM_BAR_SELECT_COMMON_IMAGE"));

    Bind(wxEVT_MENU, &BaseProjectView::OnBarSelectStippleBrush, this,
        XRCID("ID_BAR_SELECT_STIPPLE_IMAGE"));
    Bind(wxEVT_MENU, &BaseProjectView::OnBarSelectStippleShape, this,
        XRCID("ID_BAR_SELECT_STIPPLE_SHAPE"));
    Bind(wxEVT_MENU, &BaseProjectView::OnBarSelectCommonImage, this,
        XRCID("ID_BAR_SELECT_COMMON_IMAGE"));

    Bind(wxEVT_MENU, &BaseProjectView::OnGraphColorFade, this,
        XRCID("ID_GRAPH_BKCOLOR_FADE"));

    Bind(wxEVT_MENU, &BaseProjectView::OnEditPlotBackgroundImageEffect, this,
        XRCID("ID_PLOT_BKIMAGE_EFFECT_NO_EFFECT"));
    Bind(wxEVT_MENU, &BaseProjectView::OnEditPlotBackgroundImageEffect, this,
        XRCID("ID_PLOT_BKIMAGE_EFFECT_GRAYSCALE"));
    Bind(wxEVT_MENU, &BaseProjectView::OnEditPlotBackgroundImageEffect, this,
        XRCID("ID_PLOT_BKIMAGE_EFFECT_BLUR_HORIZONTALLY"));
    Bind(wxEVT_MENU, &BaseProjectView::OnEditPlotBackgroundImageEffect, this,
        XRCID("ID_PLOT_BKIMAGE_EFFECT_BLUR_VERTICALLY"));
    Bind(wxEVT_MENU, &BaseProjectView::OnEditPlotBackgroundImageEffect, this,
        XRCID("ID_PLOT_BKIMAGE_EFFECT_SEPIA"));
    Bind(wxEVT_MENU, &BaseProjectView::OnEditPlotBackgroundImageEffect, this,
        XRCID("ID_PLOT_BKIMAGE_EFFECT_FROSTED_GLASS"));
    Bind(wxEVT_MENU, &BaseProjectView::OnEditPlotBackgroundImageEffect, this,
        XRCID("ID_PLOT_BKIMAGE_EFFECT_OIL_PAINTING"));

    Bind(wxEVT_MENU, &BaseProjectView::OnEditPlotBackgroundImageFit, this,
        XRCID("ID_PLOT_BKIMAGE_FIT_CROP_AND_CENTER"));
    Bind(wxEVT_MENU, &BaseProjectView::OnEditPlotBackgroundImageFit, this,
        XRCID("ID_PLOT_BKIMAGE_FIT_SHRINK"));

    Bind(wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
            {
            BaseProjectDoc* doc = dynamic_cast<BaseProjectDoc*>(GetDocument());
            assert(doc && L"Failed to get document!");
            if (!doc)
                { return; }

            doc->SetPlotBackGroundImagePath(wxString{});
            doc->RefreshRequired(ProjectRefresh::Minimal);
            doc->RefreshGraphs();
            },
        XRCID("ID_EDIT_PLOT_BKIMAGE_REMOVE"));

    // Raygor style
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED,
        &BaseProjectView::OnEditGraphRaygorStyleButton, this,
        XRCID("ID_EDIT_GRAPH_RAYGOR_STYLE"));
    Bind(wxEVT_MENU,
        &BaseProjectView::OnRaygorStyleSelected, this,
        XRCID("ID_EDIT_GRAPH_RAYGOR_ORIGINAL"));
    Bind(wxEVT_MENU,
        &BaseProjectView::OnRaygorStyleSelected, this,
        XRCID("ID_EDIT_GRAPH_RAYGOR_BALDWIN_KAUFMAN"));
    Bind(wxEVT_MENU,
        &BaseProjectView::OnRaygorStyleSelected, this,
        XRCID("ID_EDIT_GRAPH_RAYGOR_MODERN"));
    }

//---------------------------------------------------
void BaseProjectView::OnEditStatsReportButton([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    BaseProjectDoc* theProject = dynamic_cast<BaseProjectDoc*>(GetDocument());
    ToolsOptionsDlg optionsDlg(GetDocFrame(), theProject, ToolsOptionsDlg::Statistics);
    optionsDlg.SelectPage(ToolsOptionsDlg::ANALYSIS_STATISTICS_PAGE);
    if (optionsDlg.ShowModal() == wxID_OK)
        { theProject->RefreshStatisticsReports(); }
    }

//---------------------------------------------------
void BaseProjectView::OnBarOrientationSelected(wxCommandEvent& event)
    {
    if (event.GetId() == XRCID("ID_BAR_HORIZONTAL"))
        { dynamic_cast<BaseProjectDoc*>(GetDocument())->SetBarChartOrientation(Wisteria::Orientation::Horizontal); }
    else if (event.GetId() == XRCID("ID_BAR_VERTICAL"))
        { dynamic_cast<BaseProjectDoc*>(GetDocument())->SetBarChartOrientation(Wisteria::Orientation::Vertical); }

    dynamic_cast<BaseProjectDoc*>(GetDocument())->RefreshRequired(ProjectRefresh::Minimal);
    dynamic_cast<BaseProjectDoc*>(GetDocument())->RefreshGraphs();
    }

//---------------------------------------------------
void BaseProjectView::OnBarStyleSelected(wxCommandEvent& event)
    {
    auto baseDoc{ dynamic_cast<BaseProjectDoc*>(GetDocument()) };
    assert(baseDoc && "Invalid document!");

    if (event.GetId() == XRCID("ID_BAR_STYLE_SOLID"))
        { baseDoc->SetGraphBarEffect(BoxEffect::Solid); }
    else if (event.GetId() == XRCID("ID_BAR_STYLE_GLASS"))
        { baseDoc->SetGraphBarEffect(BoxEffect::Glassy); }
    else if (event.GetId() == XRCID("ID_BAR_STYLE_BTOT"))
        { baseDoc->SetGraphBarEffect(BoxEffect::FadeFromBottomToTop); }
    else if (event.GetId() == XRCID("ID_BAR_STYLE_TTOB"))
        { baseDoc->SetGraphBarEffect(BoxEffect::FadeFromTopToBottom); }
    else if (event.GetId() == XRCID("ID_BAR_STYLE_STIPPLE_IMAGE"))
        {
        if (!wxFile::Exists(baseDoc->GetStippleImagePath()))
            {
            wxFileDialog fd
                (GetDocFrame(), _(L"Select Stipple Image"),
                wxGetApp().GetAppOptions().GetImagePath(),
                wxString{}, wxGetApp().GetAppOptions().GetImageFileFilter(),
                wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_PREVIEW);
            if (fd.ShowModal() != wxID_OK)
                { return; }
            wxGetApp().GetAppOptions().SetImagePath(wxFileName(fd.GetPath()).GetPath());
            baseDoc->SetStippleImagePath(fd.GetPath());
            }
        baseDoc->SetGraphBarEffect(BoxEffect::StippleImage);
        }
    else if (event.GetId() == XRCID("ID_BAR_STYLE_STIPPLE_SHAPE"))
        { baseDoc->SetGraphBarEffect(BoxEffect::StippleShape); }
    else if (event.GetId() == XRCID("ID_BAR_STYLE_WATERCOLOR"))
        { baseDoc->SetGraphBarEffect(BoxEffect::WaterColor); }
    else if (event.GetId() == XRCID("ID_BAR_STYLE_THICK_WATERCOLOR"))
        { baseDoc->SetGraphBarEffect(BoxEffect::ThickWaterColor); }
    else if (event.GetId() == XRCID("ID_BAR_STYLE_COMMON_IMAGE"))
        {
        if (!wxFile::Exists(baseDoc->GetGraphCommonImagePath()))
            {
            wxFileDialog fd
                (GetDocFrame(), _(L"Select Common Image"),
                wxGetApp().GetAppOptions().GetImagePath(), wxString{},
                wxGetApp().GetAppOptions().GetImageFileFilter(),
                wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_PREVIEW);
            if (fd.ShowModal() != wxID_OK)
                { return; }
            wxGetApp().GetAppOptions().SetImagePath(wxFileName(fd.GetPath()).GetPath());
            baseDoc->SetGraphCommonImagePath(fd.GetPath());
            }
        baseDoc->SetGraphBarEffect(BoxEffect::CommonImage);
        }
    baseDoc->RefreshRequired(ProjectRefresh::Minimal);
    baseDoc->RefreshGraphs();
    }

//---------------------------------------------------
void BaseProjectView::OnHistoBarStyleSelected(wxCommandEvent& event)
    {
    auto baseDoc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    assert(baseDoc && L"Failed to get document!");
    if (!baseDoc)
        { return; }

    if (event.GetId() == XRCID("ID_HISTOGRAM_BAR_STYLE_SOLID"))
        { baseDoc->SetHistogramBarEffect(BoxEffect::Solid); }
    else if (event.GetId() == XRCID("ID_HISTOGRAM_BAR_STYLE_GLASS"))
        { baseDoc->SetHistogramBarEffect(BoxEffect::Glassy); }
    else if (event.GetId() == XRCID("ID_HISTOGRAM_BAR_STYLE_BTOT"))
        { baseDoc->SetHistogramBarEffect(BoxEffect::FadeFromBottomToTop); }
    else if (event.GetId() == XRCID("ID_HISTOGRAM_BAR_STYLE_TTOB"))
        { baseDoc->SetHistogramBarEffect(BoxEffect::FadeFromTopToBottom); }
    else if (event.GetId() == XRCID("ID_HISTOGRAM_BAR_STYLE_STIPPLE_IMAGE"))
        {
        if (!wxFile::Exists(baseDoc->GetStippleImagePath()))
            {
            wxFileDialog fd
                (GetDocFrame(), _(L"Select Stipple Image"),
                wxGetApp().GetAppOptions().GetImagePath(), wxString{},
                wxGetApp().GetAppOptions().GetImageFileFilter(),
                wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_PREVIEW);
            if (fd.ShowModal() != wxID_OK)
                { return; }
            wxGetApp().GetAppOptions().SetImagePath(wxFileName(fd.GetPath()).GetPath());
            baseDoc->SetStippleImagePath(fd.GetPath());
            }
        baseDoc->SetHistogramBarEffect(BoxEffect::StippleImage);
        }
    else if (event.GetId() == XRCID("ID_HISTOGRAM_BAR_STYLE_STIPPLE_SHAPE"))
        { dynamic_cast<BaseProjectDoc*>(GetDocument())->SetHistogramBarEffect(BoxEffect::StippleShape); }
    else if (event.GetId() == XRCID("ID_HISTOGRAM_BAR_STYLE_WATERCOLOR"))
        { dynamic_cast<BaseProjectDoc*>(GetDocument())->SetHistogramBarEffect(BoxEffect::WaterColor); }
    else if (event.GetId() == XRCID("ID_HISTOGRAM_BAR_STYLE_THICK_WATERCOLOR"))
        { dynamic_cast<BaseProjectDoc*>(GetDocument())->SetHistogramBarEffect(BoxEffect::ThickWaterColor); }
    else if (event.GetId() == XRCID("ID_HISTOGRAM_BAR_STYLE_COMMON_IMAGE"))
        {
        if (!wxFile::Exists(baseDoc->GetGraphCommonImagePath()))
            {
            wxFileDialog fd
                (GetDocFrame(), _(L"Select Common Image"),
                wxGetApp().GetAppOptions().GetImagePath(), wxString{},
                wxGetApp().GetAppOptions().GetImageFileFilter(),
                wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_PREVIEW);
            if (fd.ShowModal() != wxID_OK)
                { return; }
            wxGetApp().GetAppOptions().SetImagePath(wxFileName(fd.GetPath()).GetPath());
            baseDoc->SetGraphCommonImagePath(fd.GetPath());
            }
        dynamic_cast<BaseProjectDoc*>(GetDocument())->SetHistogramBarEffect(BoxEffect::CommonImage);
        }
    baseDoc->RefreshRequired(ProjectRefresh::Minimal);
    baseDoc->RefreshGraphs();
    }

//---------------------------------------------------
void BaseProjectView::OnHistoBarSelectStippleBrush([[maybe_unused]] wxCommandEvent& event)
    {
    auto baseDoc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    assert(baseDoc && L"Failed to get document!");
    if (!baseDoc)
        { return; }

    wxFileDialog fd
        (GetDocFrame(), _(L"Select Stipple Image"),
        wxGetApp().GetAppOptions().GetImagePath(), wxString{},
        wxGetApp().GetAppOptions().GetImageFileFilter(),
        wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_PREVIEW);
    if (fd.ShowModal() != wxID_OK)
        { return; }
    wxGetApp().GetAppOptions().SetImagePath(wxFileName(fd.GetPath()).GetPath());
    baseDoc->SetStippleImagePath(fd.GetPath());

    baseDoc->SetHistogramBarEffect(BoxEffect::StippleImage);

    baseDoc->RefreshRequired(ProjectRefresh::Minimal);
    baseDoc->RefreshGraphs();
    }

//---------------------------------------------------
void BaseProjectView::OnHistoBarSelectStippleShape([[maybe_unused]] wxCommandEvent& event)
    {
    auto baseDoc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    assert(baseDoc && L"Failed to get document!");
    if (!baseDoc)
        { return; }

    wxArrayString choices;
    for (const auto& shape : wxGetApp().GetShapeMap())
        { choices.push_back(shape.first); }

    wxSingleChoiceDialog shapesDlg(GetDocFrame(),
        _("Select a shape to draw across your bars:"), _("Select Stipple Shape"), choices);
    shapesDlg.SetSize({ GetDocFrame()->FromDIP(200), GetDocFrame()->FromDIP(400) });
    shapesDlg.Center();
    if (shapesDlg.ShowModal() != wxID_OK)
        { return; }

    const auto foundShape = wxGetApp().GetShapeMap().find(shapesDlg.GetStringSelection());
    if (foundShape != wxGetApp().GetShapeMap().cend())
        { baseDoc->SetStippleShape(foundShape->second); }

    baseDoc->SetHistogramBarEffect(BoxEffect::StippleShape);

    baseDoc->RefreshRequired(ProjectRefresh::Minimal);
    baseDoc->RefreshGraphs();
    }

//---------------------------------------------------
void BaseProjectView::OnBarSelectStippleShape([[maybe_unused]] wxCommandEvent& event)
    {
    auto baseDoc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    assert(baseDoc && L"Failed to get document!");
    if (!baseDoc)
        { return; }

    wxArrayString choices;
    for (const auto& shape : wxGetApp().GetShapeMap())
        { choices.push_back(shape.first); }

    wxSingleChoiceDialog shapesDlg(GetDocFrame(),
        _("Select a shape to draw across your bars:"), _("Select Stipple Shape"), choices);
    shapesDlg.SetSize({ GetDocFrame()->FromDIP(200), GetDocFrame()->FromDIP(400) });
    shapesDlg.Center();
    if (shapesDlg.ShowModal() != wxID_OK)
        { return; }

    const auto foundShape = wxGetApp().GetShapeMap().find(shapesDlg.GetStringSelection());
    if (foundShape != wxGetApp().GetShapeMap().cend())
        { baseDoc->SetStippleShape(foundShape->second); }

    baseDoc->SetGraphBarEffect(BoxEffect::StippleShape);

    baseDoc->RefreshRequired(ProjectRefresh::Minimal);
    baseDoc->RefreshGraphs();
    }

//---------------------------------------------------
void BaseProjectView::OnBoxSelectStippleShape([[maybe_unused]] wxCommandEvent& event)
    {
    auto baseDoc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    assert(baseDoc && L"Failed to get document!");
    if (!baseDoc)
        { return; }

    wxArrayString choices;
    for (const auto& shape : wxGetApp().GetShapeMap())
        { choices.push_back(shape.first); }

    wxSingleChoiceDialog shapesDlg(GetDocFrame(),
        _("Select a shape to draw across your boxes:"), _("Select Stipple Shape"), choices);
    shapesDlg.SetSize({ GetDocFrame()->FromDIP(200), GetDocFrame()->FromDIP(400) });
    shapesDlg.Center();
    if (shapesDlg.ShowModal() != wxID_OK)
        { return; }

    const auto foundShape = wxGetApp().GetShapeMap().find(shapesDlg.GetStringSelection());
    if (foundShape != wxGetApp().GetShapeMap().cend())
        { baseDoc->SetStippleShape(foundShape->second); }

    baseDoc->SetGraphBoxEffect(BoxEffect::StippleShape);

    baseDoc->RefreshRequired(ProjectRefresh::Minimal);
    baseDoc->RefreshGraphs();
    }

//---------------------------------------------------
void BaseProjectView::OnHistoBarSelectCommonImage([[maybe_unused]] wxCommandEvent& event)
    {
    auto baseDoc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    assert(baseDoc && L"Failed to get document!");
    if (!baseDoc)
        { return; }

    wxFileDialog fd
        (GetDocFrame(), _(L"Select Common Image"),
        wxGetApp().GetAppOptions().GetImagePath(), wxString{},
        wxGetApp().GetAppOptions().GetImageFileFilter(),
        wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_PREVIEW);
    if (fd.ShowModal() != wxID_OK)
        { return; }
    wxGetApp().GetAppOptions().SetImagePath(wxFileName(fd.GetPath()).GetPath());
    baseDoc->SetGraphCommonImagePath(fd.GetPath());

    baseDoc->SetHistogramBarEffect(BoxEffect::CommonImage);

    baseDoc->RefreshRequired(ProjectRefresh::Minimal);
    baseDoc->RefreshGraphs();
    }

//---------------------------------------------------
void BaseProjectView::OnBoxSelectCommonImage([[maybe_unused]] wxCommandEvent& event)
    {
    auto baseDoc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    assert(baseDoc && L"Failed to get document!");
    if (!baseDoc)
        { return; }

    wxFileDialog fd
        (GetDocFrame(), _(L"Select Common Image"),
        wxGetApp().GetAppOptions().GetImagePath(), wxString{},
        wxGetApp().GetAppOptions().GetImageFileFilter(),
        wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_PREVIEW);
    if (fd.ShowModal() != wxID_OK)
        { return; }
    wxGetApp().GetAppOptions().SetImagePath(wxFileName(fd.GetPath()).GetPath());
    baseDoc->SetGraphCommonImagePath(fd.GetPath());

    baseDoc->SetGraphBoxEffect(BoxEffect::CommonImage);

    baseDoc->RefreshRequired(ProjectRefresh::Minimal);
    baseDoc->RefreshGraphs();
    }

//---------------------------------------------------
void BaseProjectView::OnBoxSelectStippleBrush([[maybe_unused]] wxCommandEvent& event)
    {
    auto baseDoc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    assert(baseDoc && L"Failed to get document!");
    if (!baseDoc)
        { return; }

    wxFileDialog fd
        (GetDocFrame(), _(L"Select Stipple Image"),
        wxGetApp().GetAppOptions().GetImagePath(), wxString{},
        wxGetApp().GetAppOptions().GetImageFileFilter(),
        wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_PREVIEW);
    if (fd.ShowModal() != wxID_OK)
        { return; }
    wxGetApp().GetAppOptions().SetImagePath(wxFileName(fd.GetPath()).GetPath());
    baseDoc->SetStippleImagePath(fd.GetPath());

    baseDoc->SetGraphBoxEffect(BoxEffect::StippleImage);

    baseDoc->RefreshRequired(ProjectRefresh::Minimal);
    baseDoc->RefreshGraphs();
    }

//---------------------------------------------------
void BaseProjectView::OnBarSelectStippleBrush([[maybe_unused]] wxCommandEvent& event)
    {
    auto baseDoc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    assert(baseDoc && L"Failed to get document!");
    if (!baseDoc)
        { return; }

    wxFileDialog fd
        (GetDocFrame(), _(L"Select Stipple Image"),
        wxGetApp().GetAppOptions().GetImagePath(), wxString{},
        wxGetApp().GetAppOptions().GetImageFileFilter(),
        wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_PREVIEW);
    if (fd.ShowModal() != wxID_OK)
        { return; }
    wxGetApp().GetAppOptions().SetImagePath(wxFileName(fd.GetPath()).GetPath());
    baseDoc->SetStippleImagePath(fd.GetPath());

    baseDoc->SetGraphBarEffect(BoxEffect::StippleImage);

    baseDoc->RefreshRequired(ProjectRefresh::Minimal);
    baseDoc->RefreshGraphs();
    }

//---------------------------------------------------
void BaseProjectView::OnBarSelectCommonImage([[maybe_unused]] wxCommandEvent& event)
    {
    auto baseDoc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    assert(baseDoc && L"Failed to get document!");
    if (!baseDoc)
        { return; }

    wxFileDialog fd
        (GetDocFrame(), _(L"Select Common Image"),
        wxGetApp().GetAppOptions().GetImagePath(), wxString{},
        wxGetApp().GetAppOptions().GetImageFileFilter(),
        wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_PREVIEW);
    if (fd.ShowModal() != wxID_OK)
        { return; }
    wxGetApp().GetAppOptions().SetImagePath(wxFileName(fd.GetPath()).GetPath());
    baseDoc->SetGraphCommonImagePath(fd.GetPath());

    baseDoc->SetGraphBarEffect(BoxEffect::CommonImage);

    baseDoc->RefreshRequired(ProjectRefresh::Minimal);
    baseDoc->RefreshGraphs();
    }

//---------------------------------------------------
void BaseProjectView::OnBoxStyleSelected(wxCommandEvent& event)
    {
    auto baseDoc{ dynamic_cast<BaseProjectDoc*>(GetDocument()) };
    assert(baseDoc && "Invalid document!");
    if (event.GetId() == XRCID("ID_BOX_STYLE_SOLID"))
        { baseDoc->SetGraphBoxEffect(BoxEffect::Solid); }
    else if (event.GetId() == XRCID("ID_BOX_STYLE_GLASS"))
        { baseDoc->SetGraphBoxEffect(BoxEffect::Glassy); }
    else if (event.GetId() == XRCID("ID_BOX_STYLE_LTOR"))
        { baseDoc->SetGraphBoxEffect(BoxEffect::FadeFromLeftToRight); }
    else if (event.GetId() == XRCID("ID_BOX_STYLE_RTOL"))
        { baseDoc->SetGraphBoxEffect(BoxEffect::FadeFromRightToLeft); }
    else if (event.GetId() == XRCID("ID_BOX_STYLE_STIPPLE_IMAGE"))
        {
        if (!wxFile::Exists(baseDoc->GetStippleImagePath()))
            {
            wxFileDialog fd
                (GetDocFrame(), _(L"Select Stipple Image"),
                wxGetApp().GetAppOptions().GetImagePath(), wxString{},
                wxGetApp().GetAppOptions().GetImageFileFilter(),
                wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_PREVIEW);
            if (fd.ShowModal() != wxID_OK)
                { return; }
            wxGetApp().GetAppOptions().SetImagePath(wxFileName(fd.GetPath()).GetPath());
            baseDoc->SetStippleImagePath(fd.GetPath());
            }
        baseDoc->SetGraphBoxEffect(BoxEffect::StippleImage);
        }
    else if (event.GetId() == XRCID("ID_BOX_STYLE_STIPPLE_SHAPE"))
        { baseDoc->SetGraphBoxEffect(BoxEffect::StippleShape); }
    else if (event.GetId() == XRCID("ID_BOX_STYLE_WATERCOLOR"))
        { baseDoc->SetGraphBoxEffect(BoxEffect::WaterColor); }
    else if (event.GetId() == XRCID("ID_BOX_STYLE_THICK_WATERCOLOR"))
        { baseDoc->SetGraphBoxEffect(BoxEffect::ThickWaterColor); }
    else if (event.GetId() == XRCID("ID_BOX_STYLE_COMMON_IMAGE"))
        {
        if (!wxFile::Exists(baseDoc->GetGraphCommonImagePath()))
            {
            wxFileDialog fd
                (GetDocFrame(), _(L"Select Common Image"),
                wxGetApp().GetAppOptions().GetImagePath(), wxString{},
                wxGetApp().GetAppOptions().GetImageFileFilter(),
                wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_PREVIEW);
            if (fd.ShowModal() != wxID_OK)
                { return; }
            wxGetApp().GetAppOptions().SetImagePath(wxFileName(fd.GetPath()).GetPath());
            baseDoc->SetGraphCommonImagePath(fd.GetPath());
            }
        baseDoc->SetGraphBoxEffect(BoxEffect::CommonImage);
        }
    baseDoc->RefreshRequired(ProjectRefresh::Minimal);
    baseDoc->RefreshGraphs();
    }

//---------------------------------------------------
void BaseProjectView::OnBoxPlotShowAllPointsButton([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    dynamic_cast<BaseProjectDoc*>(GetDocument())->ShowAllBoxPlotPoints(
        !dynamic_cast<BaseProjectDoc*>(GetDocument())->IsShowingAllBoxPlotPoints());
    dynamic_cast<BaseProjectDoc*>(GetDocument())->RefreshRequired(ProjectRefresh::Minimal);
    dynamic_cast<BaseProjectDoc*>(GetDocument())->RefreshGraphs();
    }

//---------------------------------------------------
void BaseProjectView::OnBoxPlotShowLabelsButton([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    dynamic_cast<BaseProjectDoc*>(GetDocument())->DisplayBoxPlotLabels(
        !dynamic_cast<BaseProjectDoc*>(GetDocument())->IsDisplayingBoxPlotLabels());
    dynamic_cast<BaseProjectDoc*>(GetDocument())->RefreshRequired(ProjectRefresh::Minimal);
    dynamic_cast<BaseProjectDoc*>(GetDocument())->RefreshGraphs();
    }

//---------------------------------------------------
void BaseProjectView::OnBarLabelsButton([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    auto doc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    assert(doc && L"Failed to get document!");
    if (!doc)
        { return; }

    doc->DisplayBarChartLabels(!doc->IsDisplayingBarChartLabels());
    doc->RefreshRequired(ProjectRefresh::Minimal);
    doc->RefreshGraphs();
    }

//---------------------------------------------------
void BaseProjectView::OnFleschConnectLinesButton([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    dynamic_cast<BaseProjectDoc*>(GetDocument())->ConnectFleschPoints(
        !dynamic_cast<BaseProjectDoc*>(GetDocument())->IsConnectingFleschPoints());
    dynamic_cast<BaseProjectDoc*>(GetDocument())->RefreshRequired(ProjectRefresh::Minimal);
    dynamic_cast<BaseProjectDoc*>(GetDocument())->RefreshGraphs();
    }

//---------------------------------------------------
void BaseProjectView::OnEnglishLabels([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    auto doc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    assert(doc && L"Failed to get document!");
    if (!doc)
        { return; }

    doc->UseEnglishLabelsForGermanLix(!doc->IsUsingEnglishLabelsForGermanLix());
    doc->RefreshRequired(ProjectRefresh::Minimal);
    doc->RefreshGraphs();
    }

//---------------------------------------------------
void BaseProjectView::OnGraphLogo([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    BaseProjectDoc* doc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    assert(doc && L"Failed to get document!");
    if (!doc)
        { return; }

    wxFileDialog fd(GetDocFrame(), _(L"Select Logo Image"),
        doc->GetWatermarkLogoPath().length() ? wxString{} : wxGetApp().GetAppOptions().GetImagePath(),
        doc->GetWatermarkLogoPath(),
        wxGetApp().GetAppOptions().GetImageFileFilter(),
        wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_PREVIEW);
    if (fd.ShowModal() == wxID_OK)
        {
        wxGetApp().GetAppOptions().SetImagePath(wxFileName(fd.GetPath()).GetPath());
        doc->SetWatermarkLogoPath(fd.GetPath());
        doc->RefreshRequired(ProjectRefresh::Minimal);
        doc->RefreshGraphs();
        }
    }

//---------------------------------------------------
void BaseProjectView::OnGraphWatermark([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    wxTextEntryDialog textDlg(GetDocFrame(),
        _(L"Enter watermark:\n\n(Note that the tags @DATETIME@, @DATE@, and @TIME@ can be used\n"
           "to dynamically expand into the current date and time.)"),
        _(L"Watermark"),
        dynamic_cast<BaseProjectDoc*>(GetDocument())->GetWatermark(), wxTextEntryDialogStyle|wxTE_MULTILINE);
    if (textDlg.ShowModal() == wxID_OK)
        {
        dynamic_cast<BaseProjectDoc*>(GetDocument())->SetWatermark(textDlg.GetValue());
        dynamic_cast<BaseProjectDoc*>(GetDocument())->RefreshRequired(ProjectRefresh::Minimal);
        dynamic_cast<BaseProjectDoc*>(GetDocument())->RefreshGraphs();
        }
    }

//---------------------------------------------------
void BaseProjectView::OnDropShadow([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    dynamic_cast<BaseProjectDoc*>(GetDocument())->DisplayDropShadows(
        !dynamic_cast<BaseProjectDoc*>(GetDocument())->IsDisplayingDropShadows());
    dynamic_cast<BaseProjectDoc*>(GetDocument())->RefreshRequired(ProjectRefresh::Minimal);
    dynamic_cast<BaseProjectDoc*>(GetDocument())->RefreshGraphs();
    }

//---------------------------------------------------
void BaseProjectView::OnShowcaseComplexWords([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    BaseProjectDoc* doc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    assert(doc && L"Failed to get document!");
    if (!doc)
        { return; }

    doc->ShowcaseComplexWords(!doc->IsShowcasingComplexWords());
    doc->RefreshRequired(ProjectRefresh::Minimal);
    doc->RefreshGraphs();
    }

//---------------------------------------------------
void BaseProjectView::OnInvalidRegionColor(wxRibbonButtonBarEvent& event)
    {
    wxCommandEvent cmd(wxEVT_NULL, event.GetId());
    OnEditGraphColor(cmd);
    }

//---------------------------------------------------
void BaseProjectView::OnRaygorStyleSelected([[maybe_unused]] wxCommandEvent& event)
    {
    if (event.GetId() == XRCID("ID_EDIT_GRAPH_RAYGOR_ORIGINAL"))
        {
        m_raygorStyleMenu.Check(XRCID("ID_EDIT_GRAPH_RAYGOR_ORIGINAL"), true);
        m_raygorStyleMenu.Check(XRCID("ID_EDIT_GRAPH_RAYGOR_BALDWIN_KAUFMAN"), false);
        m_raygorStyleMenu.Check(XRCID("ID_EDIT_GRAPH_RAYGOR_MODERN"), false);
        dynamic_cast<BaseProjectDoc*>(GetDocument())->SetRaygorStyle(
            Wisteria::Graphs::RaygorStyle::Original);
        }
    else if (event.GetId() == XRCID("ID_EDIT_GRAPH_RAYGOR_BALDWIN_KAUFMAN"))
        {
        m_raygorStyleMenu.Check(XRCID("ID_EDIT_GRAPH_RAYGOR_ORIGINAL"), false);
        m_raygorStyleMenu.Check(XRCID("ID_EDIT_GRAPH_RAYGOR_BALDWIN_KAUFMAN"), true);
        m_raygorStyleMenu.Check(XRCID("ID_EDIT_GRAPH_RAYGOR_MODERN"), false);
        dynamic_cast<BaseProjectDoc*>(GetDocument())->SetRaygorStyle(
            Wisteria::Graphs::RaygorStyle::BaldwinKaufman);
        }
    else if (event.GetId() == XRCID("ID_EDIT_GRAPH_RAYGOR_MODERN"))
        {
        m_raygorStyleMenu.Check(XRCID("ID_EDIT_GRAPH_RAYGOR_ORIGINAL"), false);
        m_raygorStyleMenu.Check(XRCID("ID_EDIT_GRAPH_RAYGOR_BALDWIN_KAUFMAN"), false);
        m_raygorStyleMenu.Check(XRCID("ID_EDIT_GRAPH_RAYGOR_MODERN"), true);
        dynamic_cast<BaseProjectDoc*>(GetDocument())->SetRaygorStyle(
            Wisteria::Graphs::RaygorStyle::Modern);
        }
    dynamic_cast<BaseProjectDoc*>(GetDocument())->RefreshRequired(ProjectRefresh::Minimal);
    dynamic_cast<BaseProjectDoc*>(GetDocument())->RefreshGraphs();
    }

//---------------------------------------------------
void BaseProjectView::OnGraphColorFade([[maybe_unused]] wxCommandEvent& event)
    {
    dynamic_cast<BaseProjectDoc*>(GetDocument())->SetGraphBackGroundLinearGradient(
        !dynamic_cast<BaseProjectDoc*>(GetDocument())->GetGraphBackGroundLinearGradient());
    wxMenuItem* fadeOption = m_graphBackgroundMenu.FindItem(XRCID("ID_GRAPH_BKCOLOR_FADE"));
    if (fadeOption)
        { fadeOption->Check(dynamic_cast<BaseProjectDoc*>(GetDocument())->GetGraphBackGroundLinearGradient()); }
    dynamic_cast<BaseProjectDoc*>(GetDocument())->RefreshRequired(ProjectRefresh::Minimal);
    dynamic_cast<BaseProjectDoc*>(GetDocument())->RefreshGraphs();
    }

//---------------------------------------------------
void BaseProjectView::OnEditPlotBackgroundImageFit(wxCommandEvent& event)
    {
    auto doc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    // uncheck all the options
    if (auto tempMenuItem = m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_FIT_CROP_AND_CENTER"));
        tempMenuItem != nullptr)
        { tempMenuItem->Check(false); }
    if (auto tempMenuItem = m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_FIT_SHRINK"));
        tempMenuItem != nullptr)
        { tempMenuItem->Check(false); }
    wxMenuItem* menuItem{ nullptr };
    if (event.GetId() == XRCID("ID_PLOT_BKIMAGE_FIT_CROP_AND_CENTER"))
        {
        doc->SetPlotBackGroundImageFit(ImageFit::CropAndCenter);
        menuItem = m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_FIT_CROP_AND_CENTER"));
        }
    else if (event.GetId() == XRCID("ID_PLOT_BKIMAGE_FIT_SHRINK"))
        {
        doc->SetPlotBackGroundImageFit(ImageFit::Shrink);
        menuItem = m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_FIT_SHRINK"));
        }

    if (menuItem)
        { menuItem->Check(true); }
    doc->RefreshRequired(ProjectRefresh::Minimal);
    doc->RefreshGraphs();
    }

//---------------------------------------------------
void BaseProjectView::OnEditPlotBackgroundImageEffect(wxCommandEvent& event)
    {
    auto doc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    // uncheck all the options
    if (auto tempMenuItem = m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_EFFECT_NO_EFFECT"));
        tempMenuItem != nullptr)
        { tempMenuItem->Check(false); }
    if (auto tempMenuItem = m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_EFFECT_GRAYSCALE"));
        tempMenuItem != nullptr)
        { tempMenuItem->Check(false); }
    if (auto tempMenuItem = m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_EFFECT_BLUR_HORIZONTALLY"));
        tempMenuItem != nullptr)
        { tempMenuItem->Check(false); }
    if (auto tempMenuItem = m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_EFFECT_BLUR_VERTICALLY"));
        tempMenuItem != nullptr)
        { tempMenuItem->Check(false); }
    if (auto tempMenuItem = m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_EFFECT_SEPIA"));
        tempMenuItem != nullptr)
        { tempMenuItem->Check(false); }
    if (auto tempMenuItem = m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_EFFECT_FROSTED_GLASS"));
        tempMenuItem != nullptr)
        { tempMenuItem->Check(false); }
    if (auto tempMenuItem = m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_EFFECT_OIL_PAINTING"));
        tempMenuItem != nullptr)
        { tempMenuItem->Check(false); }
    wxMenuItem* menuItem{ nullptr };
    if (event.GetId() == XRCID("ID_PLOT_BKIMAGE_EFFECT_NO_EFFECT"))
        {
        doc->SetPlotBackGroundImageEffect(ImageEffect::NoEffect);
        menuItem = m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_EFFECT_NO_EFFECT"));
        }
    else if (event.GetId() == XRCID("ID_PLOT_BKIMAGE_EFFECT_GRAYSCALE"))
        {
        doc->SetPlotBackGroundImageEffect(ImageEffect::Grayscale);
        menuItem = m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_EFFECT_GRAYSCALE"));
        }
    else if (event.GetId() == XRCID("ID_PLOT_BKIMAGE_EFFECT_BLUR_HORIZONTALLY"))
        {
        doc->SetPlotBackGroundImageEffect(ImageEffect::BlurHorizontal);
        menuItem = m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_EFFECT_BLUR_HORIZONTALLY"));
        }
    else if (event.GetId() == XRCID("ID_PLOT_BKIMAGE_EFFECT_BLUR_VERTICALLY"))
        {
        doc->SetPlotBackGroundImageEffect(ImageEffect::BlurVertical);
        menuItem = m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_EFFECT_BLUR_VERTICALLY"));
        }
    else if (event.GetId() == XRCID("ID_PLOT_BKIMAGE_EFFECT_SEPIA"))
        {
        doc->SetPlotBackGroundImageEffect(ImageEffect::Sepia);
        menuItem = m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_EFFECT_SEPIA"));
        }
    else if (event.GetId() == XRCID("ID_PLOT_BKIMAGE_EFFECT_FROSTED_GLASS"))
        {
        doc->SetPlotBackGroundImageEffect(ImageEffect::FrostedGlass);
        menuItem = m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_EFFECT_FROSTED_GLASS"));
        }
    else if (event.GetId() == XRCID("ID_PLOT_BKIMAGE_EFFECT_OIL_PAINTING"))
        {
        doc->SetPlotBackGroundImageEffect(ImageEffect::OilPainting);
        menuItem = m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_EFFECT_OIL_PAINTING"));
        }

    if (menuItem)
        { menuItem->Check(true); }
    doc->RefreshRequired(ProjectRefresh::Minimal);
    doc->RefreshGraphs();
    }

//---------------------------------------------------
void BaseProjectView::OnEditPlotBackgroundImage([[maybe_unused]] wxCommandEvent& event)
    {
    BaseProjectDoc* doc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    assert(doc && L"Failed to get document!");
    if (!doc)
        { return; }

    wxFileDialog fd(GetDocFrame(), _(L"Select Plot Background Image"),
            doc->GetPlotBackGroundImagePath().length() ? wxString{} : wxGetApp().GetAppOptions().GetImagePath(),
            doc->GetPlotBackGroundImagePath(),
            wxGetApp().GetAppOptions().GetImageFileFilter(),
            wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_PREVIEW);
    if (fd.ShowModal() != wxID_OK)
        { return; }
    wxGetApp().GetAppOptions().SetImagePath(wxFileName(fd.GetPath()).GetPath());
    doc->SetPlotBackGroundImagePath(fd.GetPath());
    doc->RefreshRequired(ProjectRefresh::Minimal);
    doc->RefreshGraphs();
    }

//---------------------------------------------------
void BaseProjectView::OnEditGraphFontsButton(wxRibbonButtonBarEvent& event)
    { event.PopupMenu(&m_graphFontsMenu); }

//---------------------------------------------------
void BaseProjectView::OnEditGraphFont(wxCommandEvent& event)
    {
    wxFontData data;
    if (event.GetId() == XRCID("ID_EDIT_X_AXIS_FONT"))
        {
        data.SetInitialFont(dynamic_cast<BaseProjectDoc*>(GetDocument())->GetXAxisFont());
        data.SetColour(dynamic_cast<BaseProjectDoc*>(GetDocument())->GetXAxisFontColor());
        }
    else if (event.GetId() == XRCID("ID_EDIT_Y_AXIS_FONT"))
        {
        data.SetInitialFont(dynamic_cast<BaseProjectDoc*>(GetDocument())->GetYAxisFont());
        data.SetColour(dynamic_cast<BaseProjectDoc*>(GetDocument())->GetYAxisFontColor());
        }
    else if (event.GetId() == XRCID("ID_EDIT_TOP_TITLES_FONT"))
        {
        data.SetInitialFont(dynamic_cast<BaseProjectDoc*>(GetDocument())->GetGraphTopTitleFont());
        data.SetColour(dynamic_cast<BaseProjectDoc*>(GetDocument())->GetGraphTopTitleFontColor());
        }
    else if (event.GetId() == XRCID("ID_EDIT_BOTTOM_TITLES_FONT"))
        {
        data.SetInitialFont(dynamic_cast<BaseProjectDoc*>(GetDocument())->GetBottomTitleGraphFont());
        data.SetColour(dynamic_cast<BaseProjectDoc*>(GetDocument())->GetBottomTitleGraphFontColor());
        }
    else if (event.GetId() == XRCID("ID_EDIT_LEFT_TITLES_FONT"))
        {
        data.SetInitialFont(dynamic_cast<BaseProjectDoc*>(GetDocument())->GetLeftTitleGraphFont());
        data.SetColour(dynamic_cast<BaseProjectDoc*>(GetDocument())->GetLeftTitleGraphFontColor());
        }
    else if (event.GetId() == XRCID("ID_EDIT_RIGHT_TITLES_FONT"))
        {
        data.SetInitialFont(dynamic_cast<BaseProjectDoc*>(GetDocument())->GetRightTitleGraphFont());
        data.SetColour(dynamic_cast<BaseProjectDoc*>(GetDocument())->GetRightTitleGraphFontColor());
        }

    wxFontDialog dialog(GetDocFrame(), data);
    if (dialog.ShowModal() == wxID_OK)
        {
        if (event.GetId() == XRCID("ID_EDIT_X_AXIS_FONT"))
            {
            dynamic_cast<BaseProjectDoc*>(GetDocument())->SetXAxisFont(
                dialog.GetFontData().GetChosenFont());
            dynamic_cast<BaseProjectDoc*>(GetDocument())->SetXAxisFontColor(
                dialog.GetFontData().GetColour());
            }
        else if (event.GetId() == XRCID("ID_EDIT_Y_AXIS_FONT"))
            {
            dynamic_cast<BaseProjectDoc*>(GetDocument())->SetYAxisFont(
                dialog.GetFontData().GetChosenFont());
            dynamic_cast<BaseProjectDoc*>(GetDocument())->SetYAxisFontColor(
                dialog.GetFontData().GetColour());
            }
        else if (event.GetId() == XRCID("ID_EDIT_TOP_TITLES_FONT"))
            {
            dynamic_cast<BaseProjectDoc*>(GetDocument())->SetGraphTopTitleFont(
                dialog.GetFontData().GetChosenFont());
            dynamic_cast<BaseProjectDoc*>(GetDocument())->SetGraphTopTitleFontColor(
                dialog.GetFontData().GetColour());
            }
        else if (event.GetId() == XRCID("ID_EDIT_BOTTOM_TITLES_FONT"))
            {
            dynamic_cast<BaseProjectDoc*>(GetDocument())->SetBottomTitleGraphFont(
                dialog.GetFontData().GetChosenFont());
            dynamic_cast<BaseProjectDoc*>(GetDocument())->SetBottomTitleGraphFontColor(
                dialog.GetFontData().GetColour());
            }
        else if (event.GetId() == XRCID("ID_EDIT_LEFT_TITLES_FONT"))
            {
            dynamic_cast<BaseProjectDoc*>(GetDocument())->SetLeftTitleGraphFont(
                dialog.GetFontData().GetChosenFont());
            dynamic_cast<BaseProjectDoc*>(GetDocument())->SetLeftTitleGraphFontColor(
                dialog.GetFontData().GetColour());
            }
        else if (event.GetId() == XRCID("ID_EDIT_RIGHT_TITLES_FONT"))
            {
            dynamic_cast<BaseProjectDoc*>(GetDocument())->SetRightTitleGraphFont(
                dialog.GetFontData().GetChosenFont());
            dynamic_cast<BaseProjectDoc*>(GetDocument())->SetRightTitleGraphFontColor(
                dialog.GetFontData().GetColour());
            }
        dynamic_cast<BaseProjectDoc*>(GetDocument())->RefreshRequired(ProjectRefresh::Minimal);
        dynamic_cast<BaseProjectDoc*>(GetDocument())->RefreshGraphs();
        }
    }

//---------------------------------------------------
void BaseProjectView::OnEditGraphOpacity(wxCommandEvent& event)
    {
    wxBitmap bmp(wxGetApp().GetMainFrame()->FromDIP(wxSize(300, 300)));
    uint8_t opacity = wxALPHA_OPAQUE;

    const auto fillSquare = [](wxBitmap& theBmp, wxColour opacityColor)
        {
        wxMemoryDC memDC(theBmp);
        memDC.SetBrush(wxBrush(opacityColor));
        memDC.SetPen(*wxBLACK_PEN);
        memDC.Clear();
        memDC.DrawRectangle(0, 0, theBmp.GetWidth(), theBmp.GetHeight());
        memDC.SelectObject(wxNullBitmap);
        };

    BaseProjectDoc* doc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    assert(doc && L"Failed to get document!");
    if (!doc)
        { return; }

    if (event.GetId() == XRCID("ID_EDIT_PLOT_BKIMAGE_OPACITY"))
        {
        auto img = Wisteria::GraphItems::Image::LoadFile(doc->GetPlotBackGroundImagePath());
        if (!img.IsOk())
            {
            wxFileDialog fd(GetDocFrame(), _(L"Select Plot Background Image"),
                    doc->GetPlotBackGroundImagePath().length() ?
                        wxString{} :
                        wxGetApp().GetAppOptions().GetImagePath(),
                    doc->GetPlotBackGroundImagePath(),
                    wxGetApp().GetAppOptions().GetImageFileFilter(),
                    wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_PREVIEW);
            if (fd.ShowModal() != wxID_OK)
                { return; }
            wxGetApp().GetAppOptions().SetImagePath(wxFileName(fd.GetPath()).GetPath());
            doc->SetPlotBackGroundImagePath(fd.GetPath());
            img = Wisteria::GraphItems::Image::LoadFile(doc->GetPlotBackGroundImagePath());
            if (!img.IsOk())
                { return; }
            else
                { bmp = img; }
            }
        else
            { bmp = img; }
        opacity = doc->GetPlotBackGroundImageOpacity();
        }
    else if (event.GetId() == XRCID("ID_EDIT_BOX_OPACITY"))
        {
        fillSquare(bmp, doc->GetGraphBoxColor());
        opacity = doc->GetGraphBoxOpacity();
        }
    else if (event.GetId() == XRCID("ID_EDIT_BAR_OPACITY"))
        {
        fillSquare(bmp, doc->GetBarChartBarColor());
        opacity = doc->GetGraphBarOpacity();
        }
    else if (event.GetId() == XRCID("ID_EDIT_HISTOBAR_OPACITY"))
        {
        fillSquare(bmp, doc->GetHistogramBarColor());
        opacity = doc->GetHistogramBarOpacity();
        }
    else if (event.GetId() == XRCID("ID_EDIT_PLOT_BKCOLOR_OPACITY"))
        {
        fillSquare(bmp, doc->GetPlotBackGroundColor());
        opacity = doc->GetPlotBackGroundColorOpacity();
        }

    Wisteria::UI::OpacityDlg dlg(GetDocFrame(), opacity, bmp);
    if (dlg.ShowModal())
        {
        if (event.GetId() == XRCID("ID_EDIT_PLOT_BKIMAGE_OPACITY"))
            { doc->SetPlotBackGroundImageOpacity(dlg.GetOpacity()); }
        else if (event.GetId() == XRCID("ID_EDIT_BOX_OPACITY"))
            { doc->SetGraphBoxOpacity(dlg.GetOpacity()); }
        else if (event.GetId() == XRCID("ID_EDIT_BAR_OPACITY"))
            { doc->SetGraphBarOpacity(dlg.GetOpacity()); }
        else if (event.GetId() == XRCID("ID_EDIT_HISTOBAR_OPACITY"))
            { doc->SetHistogramBarOpacity(dlg.GetOpacity()); }
        else if (event.GetId() == XRCID("ID_EDIT_PLOT_BKCOLOR_OPACITY"))
            { doc->SetPlotBackGroundColorOpacity(dlg.GetOpacity()); }
        doc->RefreshRequired(ProjectRefresh::Minimal);
        doc->RefreshGraphs();
        }
    }

//---------------------------------------------------
void BaseProjectView::OnEditGraphColorScheme([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    auto baseDoc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    assert(baseDoc && L"Failed to get document!");
    if (!baseDoc)
        { return; }

    wxArrayString choices;
    for (const auto& colorScheme : wxGetApp().GetGraphColorSchemeMap())
        { choices.push_back(colorScheme.first); }

    wxSingleChoiceDialog colorSchemeDlg(GetDocFrame(),
        _("Select a color scheme:"), _("Select Color Scheme"), choices);
    colorSchemeDlg.SetSize({ GetDocFrame()->FromDIP(200), GetDocFrame()->FromDIP(400) });
    colorSchemeDlg.Center();
    if (colorSchemeDlg.ShowModal() != wxID_OK)
        { return; }

    const auto foundColorScheme = wxGetApp().GetGraphColorSchemeMap().find(colorSchemeDlg.GetStringSelection());
    if (foundColorScheme != wxGetApp().GetGraphColorSchemeMap().cend())
        { baseDoc->SetGraphColorScheme(foundColorScheme->second); }

    baseDoc->RefreshRequired(ProjectRefresh::Minimal);
    baseDoc->RefreshGraphs();
    }

//---------------------------------------------------
void BaseProjectView::OnEditGraphColor(wxCommandEvent& event)
    {
    wxColourData data;
    wxGetApp().GetAppOptions().CopyCustomColoursToColourData(data);
    data.SetChooseFull(true);
    auto doc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    assert(doc && L"Invalid document when editing graph colors!");
    if (event.GetId() == XRCID("ID_EDIT_GRAPH_BKCOLOR"))
        { data.SetColour(doc->GetBackGroundColor()); }
    else if (event.GetId() == XRCID("ID_EDIT_BOX_COLOR"))
        { data.SetColour(doc->GetGraphBoxColor()); }
    else if (event.GetId() == XRCID("ID_EDIT_HISTOGRAM_BAR_COLOR"))
        { data.SetColour(doc->GetHistogramBarColor()); }
    else if (event.GetId() == XRCID("ID_EDIT_BAR_COLOR"))
        { data.SetColour(doc->GetBarChartBarColor()); }
    else if (event.GetId() == XRCID("ID_INVALID_REGION_COLOR"))
        { data.SetColour(doc->GetInvalidAreaColor()); }
    else if (event.GetId() == XRCID("ID_EDIT_PLOT_BKCOLOR"))
        { data.SetColour(doc->GetPlotBackGroundColor()); }

    wxColourDialog dialog(GetDocFrame(), &data);
    if (dialog.ShowModal() == wxID_OK)
        {
        wxGetApp().GetAppOptions().CopyColourDataToCustomColours(dialog.GetColourData());
        const auto color = dialog.GetColourData().GetColour();
        if (event.GetId() == XRCID("ID_EDIT_GRAPH_BKCOLOR"))
            { doc->SetBackGroundColor(color); }
        else if (event.GetId() == XRCID("ID_EDIT_BOX_COLOR"))
            { doc->SetGraphBoxColor(color); }
        else if (event.GetId() == XRCID("ID_EDIT_HISTOGRAM_BAR_COLOR"))
            { doc->SetHistogramBarColor(color); }
        else if (event.GetId() == XRCID("ID_EDIT_BAR_COLOR"))
            { doc->SetBarChartBarColor(color); }
        else if (event.GetId() == XRCID("ID_INVALID_REGION_COLOR"))
            { doc->SetInvalidAreaColor(color); }
        else if (event.GetId() == XRCID("ID_EDIT_PLOT_BKCOLOR"))
            { doc->SetPlotBackGroundColor(color); }

        doc->RefreshRequired(ProjectRefresh::Minimal);
        doc->RefreshGraphs();
        }
    }

//---------------------------------------------------
void BaseProjectView::OnEditGraphRaygorStyleButton(wxRibbonButtonBarEvent& event)
    { event.PopupMenu(&m_raygorStyleMenu); }

//---------------------------------------------------
void BaseProjectView::OnEditGraphBackgroundButton(wxRibbonButtonBarEvent& event)
    { event.PopupMenu(&m_graphBackgroundMenu); }

//---------------------------------------------------
void BaseProjectView::OnBoxStyleButton(wxRibbonButtonBarEvent& event)
    { event.PopupMenu(&m_boxStyleMenu); }

//---------------------------------------------------
void BaseProjectView::OnHistoBarLabelSelected(wxCommandEvent& event)
    {
    for (size_t i = 0; i < m_histobarLabelsMenu.GetMenuItemCount(); ++i)
        { m_histobarLabelsMenu.FindItemByPosition(i)->Check(false); }
    if (event.GetId() == XRCID("ID_HISTOBAR_LABELS_PERCENTAGE"))
        {
        dynamic_cast<BaseProjectDoc*>(GetDocument())->SetHistrogramBinLabelDisplay(BinLabelDisplay::BinPercentage);
        wxMenuItem* item = m_histobarLabelsMenu.FindItem(XRCID("ID_HISTOBAR_LABELS_PERCENTAGE"));
        if (item)
            { item->Check(true); }
        }
    else if (event.GetId() == XRCID("ID_HISTOBAR_LABELS_COUNT"))
        {
        dynamic_cast<BaseProjectDoc*>(GetDocument())->SetHistrogramBinLabelDisplay(BinLabelDisplay::BinValue);
        wxMenuItem* item = m_histobarLabelsMenu.FindItem(XRCID("ID_HISTOBAR_LABELS_COUNT"));
        if (item)
            { item->Check(true); }
        }
    else if (event.GetId() == XRCID("ID_HISTOBAR_LABELS_COUNT_AND_PERCENT"))
        {
        dynamic_cast<BaseProjectDoc*>(GetDocument())->SetHistrogramBinLabelDisplay(
            BinLabelDisplay::BinValueAndPercentage);
        wxMenuItem* item = m_histobarLabelsMenu.FindItem(XRCID("ID_HISTOBAR_LABELS_COUNT_AND_PERCENT"));
        if (item)
            { item->Check(true); }
        }
    else if (event.GetId() == XRCID("ID_HISTOBAR_NO_LABELS"))
        {
        dynamic_cast<BaseProjectDoc*>(GetDocument())->SetHistrogramBinLabelDisplay(BinLabelDisplay::NoDisplay);
        wxMenuItem* item = m_histobarLabelsMenu.FindItem(XRCID("ID_HISTOBAR_NO_LABELS"));
        if (item)
            { item->Check(true); }
        }
    dynamic_cast<BaseProjectDoc*>(GetDocument())->RefreshRequired(ProjectRefresh::Minimal);
    dynamic_cast<BaseProjectDoc*>(GetDocument())->RefreshGraphs();
    }

//---------------------------------------------------
void BaseProjectView::OnHistoBarStyleButton(wxRibbonButtonBarEvent& event)
    { event.PopupMenu(&m_histoBarStyleMenu); }

//---------------------------------------------------
void BaseProjectView::OnBarStyleButton(wxRibbonButtonBarEvent& event)
    { event.PopupMenu(&m_barStyleMenu); }

//---------------------------------------------------
void BaseProjectView::OnBarOrientationButton(wxRibbonButtonBarEvent& event)
    { event.PopupMenu(&m_barOrientationMenu); }

//---------------------------------------------------
void BaseProjectView::OnZoomButton(wxRibbonButtonBarEvent& event)
    { event.PopupMenu(&m_zoomMenu); }

//---------------------------------------------------
void BaseProjectView::OnHistoBarsLabelsButton(wxRibbonButtonBarEvent& event)
    { event.PopupMenu(&m_histobarLabelsMenu); }

//---------------------------------------------------
void BaseProjectView::OnDClickRibbonBar([[maybe_unused]] wxRibbonBarEvent& event)
    { GetRibbon()->ShowPanels(!GetRibbon()->ArePanelsShown()); }

//---------------------------------------------------
void BaseProjectView::OnClickRibbonBar([[maybe_unused]] wxRibbonBarEvent& event)
    { GetRibbon()->ShowPanels(); }

//---------------------------------------------------
void BaseProjectView::OnExcludeWordsList([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    BaseProjectDoc* doc = dynamic_cast<BaseProjectDoc*>(GetDocument());

    EditWordListDlg editDlg(GetDocFrame(),
        wxID_ANY, _(L"Edit Words/Phrases To Exclude"));
    editDlg.SetHelpTopic(wxGetApp().GetMainFrame()->GetHelpDirectory(), L"document-analysis.html");
    editDlg.SetPhraseFileMode(true);
    editDlg.SetFilePath(doc->GetExcludedPhrasesPath());
    if (editDlg.ShowModal() != wxID_OK)
        { return; }

    doc->SetExcludedPhrasesPath(editDlg.GetFilePath());
    doc->RefreshRequired(ProjectRefresh::FullReindexing);
    doc->RefreshProject();
    }

//---------------------------------------------------
void BaseProjectView::OnIncompleteThreshold([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    BaseProjectDoc* doc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    const long len = wxGetNumberFromUser(
        _(L"Include incomplete sentences containing more than:"),
        wxString{}, _(L"Incomplete Sentence Threshold"),
        doc->GetIncludeIncompleteSentencesIfLongerThanValue(), 0, std::numeric_limits<int>::max());
    if (len == -1)
        { return; }
    doc->SetIncludeIncompleteSentencesIfLongerThanValue(len);
    doc->RefreshRequired(ProjectRefresh::FullReindexing);
    doc->RefreshProject();
    }

//---------------------------------------------------
void BaseProjectView::OnNumeralSyllabication(wxRibbonButtonBarEvent& event)
    { event.PopupMenu(&m_numeralSyllabicationMenu); }

//---------------------------------------------------
void BaseProjectView::OnNumeralSyllabicationOptions(wxCommandEvent& event)
    {
    BaseProjectDoc* doc = dynamic_cast<BaseProjectDoc*>(GetDocument());

    doc->SetNumeralSyllabicationMethod((event.GetId() == XRCID("ID_NUMSYL_ONE")) ?
        NumeralSyllabize::WholeWordIsOneSyllable : NumeralSyllabize::SoundOutEachDigit);

    for (size_t i = 0; i < m_numeralSyllabicationMenu.GetMenuItemCount(); ++i)
        { m_numeralSyllabicationMenu.FindItemByPosition(i)->Check(false); }
    wxMenuItem* item = m_numeralSyllabicationMenu.FindItem(
        (doc->GetNumeralSyllabicationMethod() == NumeralSyllabize::WholeWordIsOneSyllable) ?
        XRCID("ID_NUMSYL_ONE") :
        XRCID("ID_NUMSYL_EACH_DIGIT"));
    if (item)
        { item->Check(true); }

    doc->RefreshRequired(ProjectRefresh::FullReindexing);
    doc->RefreshProject();
    }

//---------------------------------------------------
void BaseProjectView::OnLongSentences(wxRibbonButtonBarEvent& event)
    { event.PopupMenu(&m_longSentencesMenu); }

//---------------------------------------------------
void BaseProjectView::OnLongSentencesOptions(wxCommandEvent& event)
    {
    BaseProjectDoc* doc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    if (event.GetId() == XRCID("ID_LS_LONGER_THAN"))
        {
        const long len = wxGetNumberFromUser(
            _(L"Consider sentences overly long if longer than:"), wxString{}, _(L"Long Sentences"),
            doc->GetDifficultSentenceLength(), 0, std::numeric_limits<int>::max());
        if (len == -1)
            { return; }
        doc->SetDifficultSentenceLength(len);
        }
    doc->SetLongSentenceMethod((event.GetId() == XRCID("ID_LS_LONGER_THAN")) ?
        LongSentence::LongerThanSpecifiedLength : LongSentence::OutlierLength);

    for (size_t i = 0; i < m_longSentencesMenu.GetMenuItemCount(); ++i)
        { m_longSentencesMenu.FindItemByPosition(i)->Check(false); }
    wxMenuItem* item = m_longSentencesMenu.FindItem(
        (doc->GetLongSentenceMethod() == LongSentence::LongerThanSpecifiedLength) ?
        XRCID("ID_LS_LONGER_THAN") :
        XRCID("ID_LS_OUTLIER_RANGE"));
    if (item)
        { item->Check(true); }

    doc->RefreshRequired(ProjectRefresh::FullReindexing);
    doc->RefreshProject();
    }

//---------------------------------------------------
void BaseProjectView::OnTextExclusionOptions(wxCommandEvent& event)
    {
    BaseProjectDoc* doc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    doc->SetInvalidSentenceMethod((event.GetId() == XRCID("ID_TE_ALL_INCOMPLETE")) ?
        InvalidSentence::ExcludeFromAnalysis : (event.GetId() == XRCID("ID_TE_NO_EXCLUDE")) ?
        InvalidSentence::IncludeAsFullSentences : InvalidSentence::ExcludeExceptForHeadings);

    for (size_t i = 0; i < m_textExclusionMenu.GetMenuItemCount(); ++i)
        { m_textExclusionMenu.FindItemByPosition(i)->Check(false); }
    wxMenuItem* item = m_textExclusionMenu.FindItem(
        (doc->GetInvalidSentenceMethod() == InvalidSentence::ExcludeFromAnalysis) ?
        XRCID("ID_TE_ALL_INCOMPLETE") :
        (doc->GetInvalidSentenceMethod() == InvalidSentence::IncludeAsFullSentences) ? XRCID("ID_TE_NO_EXCLUDE") :
        XRCID("ID_TE_ALL_INCOMPLETE_EXCEPT_HEADERS"));
    if (item)
        { item->Check(true); }

    if (GetRibbon())
        {
        wxWindow* exclusionButtonBar = GetRibbon()->FindWindow(MainFrame::ID_TEXT_EXCLUSION_RIBBON_BUTTON_BAR);
        if (exclusionButtonBar && exclusionButtonBar->IsKindOf(CLASSINFO(wxRibbonButtonBar)))
            {
            dynamic_cast<wxRibbonButtonBar*>(exclusionButtonBar)->EnableButton(
                XRCID("ID_EXCLUDE_AGGRESSIVELY"),
                (doc->GetInvalidSentenceMethod() != InvalidSentence::IncludeAsFullSentences));
            dynamic_cast<wxRibbonButtonBar*>(exclusionButtonBar)->EnableButton(
                XRCID("ID_EXCLUDE_COPYRIGHT_NOTICES"),
                (doc->GetInvalidSentenceMethod() != InvalidSentence::IncludeAsFullSentences));
            dynamic_cast<wxRibbonButtonBar*>(exclusionButtonBar)->EnableButton(
                XRCID("ID_EXCLUDE_TRAILING_CITATIONS"),
                (doc->GetInvalidSentenceMethod() != InvalidSentence::IncludeAsFullSentences));
            dynamic_cast<wxRibbonButtonBar*>(exclusionButtonBar)->EnableButton(
                XRCID("ID_EXCLUDE_FILE_ADDRESSES"),
                (doc->GetInvalidSentenceMethod() != InvalidSentence::IncludeAsFullSentences));
            dynamic_cast<wxRibbonButtonBar*>(exclusionButtonBar)->EnableButton(
                XRCID("ID_EXCLUDE_NUMERALS"),
                (doc->GetInvalidSentenceMethod() != InvalidSentence::IncludeAsFullSentences));
            dynamic_cast<wxRibbonButtonBar*>(exclusionButtonBar)->EnableButton(
                XRCID("ID_EXCLUDE_PROPER_NOUNS"),
                (doc->GetInvalidSentenceMethod() != InvalidSentence::IncludeAsFullSentences));
            dynamic_cast<wxRibbonButtonBar*>(exclusionButtonBar)->EnableButton(
                XRCID("ID_EXCLUDE_WORD_LIST"),
                (doc->GetInvalidSentenceMethod() != InvalidSentence::IncludeAsFullSentences));
            }
        wxWindow* numeralButtonBar = GetRibbon()->FindWindow(MainFrame::ID_NUMERALS_RIBBON_BUTTON_BAR);
        if (numeralButtonBar && numeralButtonBar->IsKindOf(CLASSINFO(wxRibbonButtonBar)))
            {
            dynamic_cast<wxRibbonButtonBar*>(numeralButtonBar)->EnableButton(
                XRCID("ID_NUMERAL_SYLLABICATION"),
                (doc->GetInvalidSentenceMethod() == InvalidSentence::IncludeAsFullSentences) ?
                    true : !doc->IsIgnoringNumerals());
            }
        }
    doc->RefreshRequired(ProjectRefresh::FullReindexing);
    doc->RefreshProject();
    }

//---------------------------------------------------
void BaseProjectView::OnExclusionTags(wxRibbonButtonBarEvent& event)
    { event.PopupMenu(&m_exclusionTagsMenu); }

//---------------------------------------------------
void BaseProjectView::OnExclusionTagsOptions(wxCommandEvent& event)
    {
    BaseProjectDoc* doc = dynamic_cast<BaseProjectDoc*>(GetDocument());

    doc->GetExclusionBlockTags().clear();
    if (event.GetId() == XRCID("ID_EXCLUSION_TAGS_CAROTS"))
        { doc->GetExclusionBlockTags().push_back(std::make_pair(L'^',L'^')); }
    else if (event.GetId() == XRCID("ID_EXCLUSION_TAGS_ANGLES"))
        { doc->GetExclusionBlockTags().push_back(std::make_pair(L'<',L'>')); }
    else if (event.GetId() == XRCID("ID_EXCLUSION_TAGS_BRACES"))
        { doc->GetExclusionBlockTags().push_back(std::make_pair(L'[',L']')); }
    else if (event.GetId() == XRCID("ID_EXCLUSION_TAGS_CURLIES"))
        { doc->GetExclusionBlockTags().push_back(std::make_pair(L'{',L'}')); }
    else if (event.GetId() == XRCID("ID_EXCLUSION_TAGS_PARANS"))
        { doc->GetExclusionBlockTags().push_back(std::make_pair(L'(',L')')); }

    for (size_t i = 0; i < m_exclusionTagsMenu.GetMenuItemCount(); ++i)
        { m_exclusionTagsMenu.FindItemByPosition(i)->Check(false); }
    wxMenuItem* item = m_exclusionTagsMenu.FindItem((doc->GetExclusionBlockTags().size() == 0) ?
        XRCID("ID_EXCLUSION_TAGS_NOT_ENABLED") :
        (doc->GetExclusionBlockTags().at(0) == std::make_pair(L'^', L'^')) ? XRCID("ID_EXCLUSION_TAGS_CAROTS") :
            (doc->GetExclusionBlockTags().at(0) == std::make_pair(L'<', L'>')) ? XRCID("ID_EXCLUSION_TAGS_ANGLES") :
            (doc->GetExclusionBlockTags().at(0) == std::make_pair(L'[', L']')) ? XRCID("ID_EXCLUSION_TAGS_BRACES") :
            (doc->GetExclusionBlockTags().at(0) == std::make_pair(L'{', L'}')) ? XRCID("ID_EXCLUSION_TAGS_CURLIES") :
            (doc->GetExclusionBlockTags().at(0) == std::make_pair(L'(', L')')) ? XRCID("ID_EXCLUSION_TAGS_PARANS") :
        XRCID("ID_EXCLUSION_TAGS_NOT_ENABLED"));
    if (item)
        { item->Check(true); }

    doc->RefreshRequired(ProjectRefresh::FullReindexing);
    doc->RefreshProject();
    }

//---------------------------------------------------
void BaseProjectView::OnTextExclusion(wxRibbonButtonBarEvent& event)
    { event.PopupMenu(&m_textExclusionMenu); }

//---------------------------------------------------
void BaseProjectView::OnLineEnds(wxRibbonButtonBarEvent& event)
    { event.PopupMenu(&m_lineEndsMenu); }

//---------------------------------------------------
void BaseProjectView::OnLineEndOptions(wxCommandEvent& event)
    {
    BaseProjectDoc* doc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    doc->SetParagraphsParsingMethod((event.GetId() == XRCID("ID_LE_ONLY_AFTER_VALID_SENTENCE")) ?
        ParagraphParse::OnlySentenceTerminatedNewLinesAreParagraphs : ParagraphParse::EachNewLineIsAParagraph);

    for (size_t i = 0; i < m_lineEndsMenu.GetMenuItemCount(); ++i)
        { m_lineEndsMenu.FindItemByPosition(i)->Check(false); }
    wxMenuItem* item = m_lineEndsMenu.FindItem(
        (doc->GetParagraphsParsingMethod() == ParagraphParse::OnlySentenceTerminatedNewLinesAreParagraphs) ?
        XRCID("ID_LE_ONLY_AFTER_VALID_SENTENCE") : XRCID("ID_LE_ALWAYS_NEW_PARAGRAPH"));
    if (item)
        { item->Check(true); }

    if (GetRibbon())
        {
        wxWindow* deductionButtonBar =
            GetRibbon()->FindWindow(MainFrame::ID_PARAGRAPH_DEDUCTION_RIBBON_BUTTON_BAR);
        if (deductionButtonBar && deductionButtonBar->IsKindOf(CLASSINFO(wxRibbonButtonBar)))
            {
            dynamic_cast<wxRibbonButtonBar*>(deductionButtonBar)->EnableButton(
                XRCID("ID_IGNORE_BLANK_LINES"),
                    (doc->GetParagraphsParsingMethod() ==
                        ParagraphParse::OnlySentenceTerminatedNewLinesAreParagraphs));
            dynamic_cast<wxRibbonButtonBar*>(deductionButtonBar)->EnableButton(
                XRCID("ID_IGNORE_INDENTING"),
                    (doc->GetParagraphsParsingMethod() ==
                        ParagraphParse::OnlySentenceTerminatedNewLinesAreParagraphs));
            }
        }
    doc->RefreshRequired(ProjectRefresh::FullReindexing);
    doc->RefreshProject();
    }

//---------------------------------------------------
void BaseProjectView::OnIgnoreBlankLines([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    BaseProjectDoc* theProject = dynamic_cast<BaseProjectDoc*>(GetDocument());
    theProject->SetIgnoreBlankLinesForParagraphsParser(!theProject->GetIgnoreBlankLinesForParagraphsParser());
    theProject->RefreshRequired(ProjectRefresh::FullReindexing);
    theProject->RefreshProject();
    }

//---------------------------------------------------
void BaseProjectView::OnIgnoreIdenting([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    BaseProjectDoc* theProject = dynamic_cast<BaseProjectDoc*>(GetDocument());
    theProject->SetIgnoreIndentingForParagraphsParser(!theProject->GetIgnoreIndentingForParagraphsParser());
    theProject->RefreshRequired(ProjectRefresh::FullReindexing);
    theProject->RefreshProject();
    }

//---------------------------------------------------
void BaseProjectView::OnStrictCapitalization([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    BaseProjectDoc* theProject = dynamic_cast<BaseProjectDoc*>(GetDocument());
    theProject->SetSentenceStartMustBeUppercased(!theProject->GetSentenceStartMustBeUppercased());
    theProject->RefreshRequired(ProjectRefresh::FullReindexing);
    theProject->RefreshProject();
    }

//---------------------------------------------------
void BaseProjectView::OnAggressivelyExclude([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    BaseProjectDoc* theProject = dynamic_cast<BaseProjectDoc*>(GetDocument());
    theProject->AggressiveExclusion(!theProject->IsExcludingAggressively());
    theProject->RefreshRequired(ProjectRefresh::FullReindexing);
    theProject->RefreshProject();
    }

//---------------------------------------------------
void BaseProjectView::OnIgnoreCopyrights([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    BaseProjectDoc* theProject = dynamic_cast<BaseProjectDoc*>(GetDocument());
    theProject->IgnoreTrailingCopyrightNoticeParagraphs(!theProject->IsIgnoringTrailingCopyrightNoticeParagraphs());
    theProject->RefreshRequired(ProjectRefresh::FullReindexing);
    theProject->RefreshProject();
    }

//---------------------------------------------------
void BaseProjectView::OnIgnoreCitations([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    BaseProjectDoc* theProject = dynamic_cast<BaseProjectDoc*>(GetDocument());
    theProject->IgnoreTrailingCitations(!theProject->IsIgnoringTrailingCitations());
    theProject->RefreshRequired(ProjectRefresh::FullReindexing);
    theProject->RefreshProject();
    }

//---------------------------------------------------
void BaseProjectView::OnIgnoreFileAddresses([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    BaseProjectDoc* theProject = dynamic_cast<BaseProjectDoc*>(GetDocument());
    theProject->IgnoreFileAddresses(!theProject->IsIgnoringFileAddresses());
    theProject->RefreshRequired(ProjectRefresh::FullReindexing);
    theProject->RefreshProject();
    }

//---------------------------------------------------
void BaseProjectView::OnIgnoreNumerals([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    BaseProjectDoc* theProject = dynamic_cast<BaseProjectDoc*>(GetDocument());
    theProject->IgnoreNumerals(!theProject->IsIgnoringNumerals());
    if (GetRibbon())
        {
        wxWindow* numeralButtonBar = GetRibbon()->FindWindow(MainFrame::ID_NUMERALS_RIBBON_BUTTON_BAR);
        if (numeralButtonBar && numeralButtonBar->IsKindOf(CLASSINFO(wxRibbonButtonBar)))
            {
            dynamic_cast<wxRibbonButtonBar*>(numeralButtonBar)->EnableButton(
                XRCID("ID_NUMERAL_SYLLABICATION"), !theProject->IsIgnoringNumerals());
            }
        }
    theProject->RefreshRequired(ProjectRefresh::FullReindexing);
    theProject->RefreshProject();
    }

//---------------------------------------------------
void BaseProjectView::OnIgnoreProperNouns([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    BaseProjectDoc* theProject = dynamic_cast<BaseProjectDoc*>(GetDocument());
    theProject->IgnoreProperNouns(!theProject->IsIgnoringProperNouns());
    theProject->RefreshRequired(ProjectRefresh::FullReindexing);
    theProject->RefreshProject();
    }

//---------------------------------------------------
void BaseProjectView::OnEditDictionaryButton([[maybe_unused]] wxRibbonButtonBarEvent& event)
    { wxGetApp().EditDictionary(dynamic_cast<BaseProjectDoc*>(GetDocument())->GetProjectLanguage()); }

//---------------------------------------------------
void BaseProjectView::OnEditEnglishDictionary([[maybe_unused]] wxCommandEvent& event)
    { wxGetApp().EditDictionary(readability::test_language::english_test); }

//---------------------------------------------------
void BaseProjectView::OnEditDictionaryProjectSettings([[maybe_unused]] wxCommandEvent& event)
    {
    BaseProjectDoc* theProject = dynamic_cast<BaseProjectDoc*>(GetDocument());
    ToolsOptionsDlg optionsDlg(GetDocFrame(), theProject, ToolsOptionsDlg::Grammar);
    optionsDlg.SelectPage(ToolsOptionsDlg::GRAMMAR_PAGE);
    if (optionsDlg.ShowModal() == wxID_OK)
        { theProject->RefreshProject(); }
    }

//---------------------------------------------------
void BaseProjectView::OnEditDictionary([[maybe_unused]] wxCommandEvent& event)
    { wxGetApp().EditDictionary(dynamic_cast<BaseProjectDoc*>(GetDocument())->GetProjectLanguage()); }

//-------------------------------------------------------
void BaseProjectView::OnAddCustomTestBundle(wxCommandEvent& event)
    { wxGetApp().GetMainFrameEx()->OnAddCustomTestBundle(event); }

//-------------------------------------------------------
void BaseProjectView::OnEditCustomTestBundle(wxCommandEvent& event)
    { wxGetApp().GetMainFrameEx()->OnEditCustomTestBundle(event); }

//-------------------------------------------------------
void BaseProjectView::OnRemoveCustomTestBundle(wxCommandEvent& event)
    { wxGetApp().GetMainFrameEx()->OnRemoveCustomTestBundle(event); }

//-------------------------------------------------------
void BaseProjectView::OnCustomTestBundle(wxCommandEvent& event)
    {
    const int menuId = event.GetId();
    std::map<int, wxString>::const_iterator pos = MainFrame::GetTestBundleMenuIds().find(menuId);
    if (pos == MainFrame::GetTestBundleMenuIds().end())
        {
        wxMessageBox(_(L"Unable to find test bundle: internal error, please contact software vendor."),
            _(L"Error"), wxOK|wxICON_ERROR);
        return;
        }
    // Add the tests & goals to the project
    if (GetDocument() )
        {
        auto doc = dynamic_cast<BaseProjectDoc*>(GetDocument());
        assert(doc);
        if (doc && doc->IsSafeToUpdate())
            {
            const bool hadDolchSightWords = doc->IsIncludingDolchSightWords();
            doc->ApplyTestBundle(pos->second);
            if (doc->IsKindOf(CLASSINFO(ProjectDoc)))
                {
                // if Dolch was added or removed then refresh
                if ((!hadDolchSightWords && doc->IsIncludingDolchSightWords()) ||
                    (hadDolchSightWords && !doc->IsIncludingDolchSightWords()) )
                    { doc->RefreshRequired(ProjectRefresh::FullReindexing); }
                // refresh
                doc->RefreshRequired(ProjectRefresh::Minimal);
                doc->RefreshProject();
                }
            else if (doc->IsKindOf(CLASSINFO(BatchProjectDoc)))
                {
                doc->RefreshRequired(ProjectRefresh::Minimal);
                doc->RefreshProject();
                }
            doc->Modify(true);
            }
        }
    }

//-------------------------------------------------------
void BaseProjectView::OnTestBundle(wxCommandEvent& event)
    {
    // see what sort of bundle this is
    bool basedOnIndustry = true;
    readability::industry_classification selectedIndustry =
        readability::industry_classification::adult_publishing_industry;
    readability::document_classification selectedDocument =
        readability::document_classification::adult_literature_document;
    if (event.GetId() == XRCID("ID_ADD_CHILDRENS_PUBLISHING_TEST_BUNDLE"))
        {
        selectedIndustry = readability::industry_classification::childrens_publishing_industry;
        basedOnIndustry = true;
        }
    else if (event.GetId() == XRCID("ID_ADD_ADULT_PUBLISHING_TEST_BUNDLE"))
        {
        selectedIndustry = readability::industry_classification::adult_publishing_industry;
        basedOnIndustry = true;
        }
    else if (event.GetId() == XRCID("ID_ADD_CHILDRENS_HEALTHCARE_TEST_BUNDLE"))
        {
        selectedIndustry = readability::industry_classification::childrens_healthcare_industry;
        basedOnIndustry = true;
        }
    else if (event.GetId() == XRCID("ID_ADD_ADULT_HEALTHCARE_TEST_BUNDLE"))
        {
        selectedIndustry = readability::industry_classification::adult_healthcare_industry;
        basedOnIndustry = true;
        }
    else if (event.GetId() == XRCID("ID_ADD_MILITARY_COVERNMENT_TEST_BUNDLE"))
        {
        selectedIndustry = readability::industry_classification::military_government_industry;
        basedOnIndustry = true;
        }
    else if (event.GetId() == XRCID("ID_ADD_SECOND_LANGUAGE_TEST_BUNDLE"))
        {
        selectedIndustry = readability::industry_classification::sedondary_language_industry;
        basedOnIndustry = true;
        }
    else if (event.GetId() == XRCID("ID_ADD_BROADCASTING_TEST_BUNDLE"))
        {
        selectedIndustry = readability::industry_classification::broadcasting_industry;
        basedOnIndustry = true;
        }
    else if (event.GetId() == XRCID("ID_GENERAL_DOCUMENT_TEST_BUNDLE"))
        {
        selectedDocument = readability::document_classification::general_document;
        basedOnIndustry = false;
        }
    else if (event.GetId() == XRCID("ID_TECHNICAL_DOCUMENT_TEST_BUNDLE"))
        {
        selectedDocument = readability::document_classification::technical_document;
        basedOnIndustry = false;
        }
    else if (event.GetId() == XRCID("ID_NONNARRATIVE_DOCUMENT_TEST_BUNDLE"))
        {
        selectedDocument = readability::document_classification::nonnarrative_document;
        basedOnIndustry = false;
        }
    else if (event.GetId() == XRCID("ID_ADULT_LITERATURE_DOCUMENT_TEST_BUNDLE"))
        {
        selectedDocument = readability::document_classification::adult_literature_document;
        basedOnIndustry = false;
        }
    else if (event.GetId() == XRCID("ID_CHILDRENS_LITERATURE_DOCUMENT_TEST_BUNDLE"))
        {
        selectedDocument = readability::document_classification::childrens_literature_document;
        basedOnIndustry = false;
        }

    // Standard project
    if (GetDocument() && GetDocument()->IsKindOf(CLASSINFO(ProjectDoc)) )
        {
        ProjectDoc* doc = dynamic_cast<ProjectDoc*>(GetDocument());
        if (doc && doc->IsSafeToUpdate())
            {
            const bool hadDolchSightWords = doc->IsIncludingDolchSightWords();
            doc->ExcludeAllTests();
            // standard tests
            for (auto rTest = doc->GetReadabilityTests().get_tests().begin();
                rTest != doc->GetReadabilityTests().get_tests().end();
                ++rTest)
                {
                const bool matchesClassification = basedOnIndustry ?
                    rTest->get_test().has_industry_classification(selectedIndustry) :
                    rTest->get_test().has_document_classification(selectedDocument);
                rTest->include(matchesClassification &&
                               rTest->get_test().has_language(doc->GetProjectLanguage()) );
                }
            // custom tests
            for (CustomReadabilityTestCollection::const_iterator pos = doc->m_custom_word_tests.begin();
                pos != doc->m_custom_word_tests.end();
                ++pos)
                {
                if ((basedOnIndustry && pos->has_industry_classification(selectedIndustry)) ||
                    pos->has_document_classification(selectedDocument))
                    {
                    doc->RefreshRequired(ProjectRefresh::FullReindexing);
                    doc->AddCustomReadabilityTest(wxString(pos->get_name().c_str()));
                    }
                }
            // see if Dolch section should be added
            if (((basedOnIndustry &&
                selectedIndustry == readability::industry_classification::childrens_publishing_industry) ||
                (basedOnIndustry &&
                 selectedIndustry == readability::industry_classification::sedondary_language_industry) ||
                (!basedOnIndustry &&
                 selectedDocument == readability::document_classification::childrens_literature_document)) &&
                (doc->GetProjectLanguage() == readability::test_language::english_test))
                { doc->IncludeDolchSightWords(); }
            // if Dolch was added or removed then refresh
            if ((!hadDolchSightWords && doc->IsIncludingDolchSightWords()) ||
                (hadDolchSightWords && !doc->IsIncludingDolchSightWords()) )
                { doc->RefreshRequired(ProjectRefresh::FullReindexing); }
            doc->RefreshRequired(ProjectRefresh::Minimal);
            doc->RefreshProject();
            }
        }
    // Batch project
    if (GetDocument() && GetDocument()->IsKindOf(CLASSINFO(BatchProjectDoc)) )
        {
        BatchProjectDoc* doc = dynamic_cast<BatchProjectDoc*>(GetDocument());
        if (doc && doc->IsSafeToUpdate())
            {
            doc->ExcludeAllTests();
            // standard tests
            for (auto rTest = doc->GetReadabilityTests().get_tests().begin();
                rTest != doc->GetReadabilityTests().get_tests().end();
                ++rTest)
                {
                const bool matchesClassification = basedOnIndustry ?
                    rTest->get_test().has_industry_classification(selectedIndustry) :
                    rTest->get_test().has_document_classification(selectedDocument);
                rTest->include(matchesClassification && rTest->get_test().has_language(doc->GetProjectLanguage()) );
                }
            // custom tests
            for (CustomReadabilityTestCollection::const_iterator pos = doc->m_custom_word_tests.begin();
                pos != doc->m_custom_word_tests.end();
                ++pos)
                {
                if ((basedOnIndustry && pos->has_industry_classification(selectedIndustry)) ||
                    pos->has_document_classification(selectedDocument))
                    {
                    doc->RefreshRequired(ProjectRefresh::FullReindexing);
                    doc->AddCustomReadabilityTest(wxString(pos->get_name().c_str()));
                    }
                }
            // see if Dolch section should be added...
            if (((basedOnIndustry &&
                  selectedIndustry == readability::industry_classification::childrens_publishing_industry) ||
                (basedOnIndustry &&
                 selectedIndustry == readability::industry_classification::sedondary_language_industry) ||
                (!basedOnIndustry &&
                 selectedDocument == readability::document_classification::childrens_literature_document)) &&
                (doc->GetProjectLanguage() == readability::test_language::english_test))
                { doc->IncludeDolchSightWords(); }
            // ...or removed
            else if (doc->IsIncludingDolchSightWords())
                { doc->IncludeDolchSightWords(false); }
            // need to do a full refresh (except document re-indexing) for a batch project
            // because some many sections in it involve test scores.
            doc->RefreshRequired(ProjectRefresh::Minimal);
            doc->RefreshProject();
            }
        }
    }

//-------------------------------------------------------
void BaseProjectView::OnCopyDropdown(wxRibbonButtonBarEvent& evt)
    { evt.PopupMenu(&m_copyMenu); }

//-------------------------------------------------------
void BaseProjectView::OnWordListDropdown(wxRibbonButtonBarEvent& evt)
    { evt.PopupMenu(&m_wordListMenu); }

//-------------------------------------------------------
void BaseProjectView::OnBlankGraphDropdown(wxRibbonButtonBarEvent& evt)
    { evt.PopupMenu(&m_blankGraphMenu); }

//-------------------------------------------------------
void BaseProjectView::OnDictionaryDropdown(wxRibbonButtonBarEvent& evt)
    {
    wxMenu menu;
    menu.Append(XRCID("ID_EDIT_ENGLISH_DICTIONARY"), _(L"Edit Custom Dictionary..."));
    menu.Append(XRCID("ID_ADD_ITEM_TO_DICTIONARY"), _(L"Add Selected Words"));
    menu.Append(XRCID("ID_EDIT_DICTIONARY_PROJECT_SETTINGS"), _(L"Settings (Current Project)..."));
    menu.Append(XRCID("ID_EDIT_DICTIONARY_SETTINGS"), _(L"Settings (Globally)..."));
    evt.PopupMenu(&menu);
    }

//-------------------------------------------------------
void BaseProjectView::OnPrimaryAgeTestsDropdown(wxRibbonButtonBarEvent& evt)
    {
    if (m_primaryAgeTestsMenu.GetMenuItemCount() == 0)
        {
        wxMessageBox(_(L"No tests are available in this category."),
            wxGetApp().GetAppDisplayName(), wxOK|wxICON_INFORMATION);
        }
    else
        { evt.PopupMenu(&m_primaryAgeTestsMenu); }
    }

//-------------------------------------------------------
void BaseProjectView::OnSecondaryAgeTestsDropdown(wxRibbonButtonBarEvent& evt)
    {
    if (m_secondaryAgeTestsMenu.GetMenuItemCount() == 0)
        {
        wxMessageBox(_(L"No tests are available in this category."),
            wxGetApp().GetAppDisplayName(), wxOK|wxICON_INFORMATION);
        }
    else
        { evt.PopupMenu(&m_secondaryAgeTestsMenu); }
    }

//-------------------------------------------------------
void BaseProjectView::OnAdultTestsDropdown(wxRibbonButtonBarEvent& evt)
    {
    if (m_adultTestsMenu.GetMenuItemCount() == 0)
        {
        wxMessageBox(_(L"No tests are available in this category."),
            wxGetApp().GetAppDisplayName(), wxOK|wxICON_INFORMATION);
        }
    else
        { evt.PopupMenu(&m_adultTestsMenu); }
    }

//-------------------------------------------------------
void BaseProjectView::OnSecondLanguageTestsDropdown(wxRibbonButtonBarEvent& evt)
    {
    if (m_secondLanguageTestsMenu.GetMenuItemCount() == 0)
        {
        wxMessageBox(_(L"No tests are available in this category."),
            wxGetApp().GetAppDisplayName(), wxOK|wxICON_INFORMATION);
        }
    else
        { evt.PopupMenu(&m_secondLanguageTestsMenu); }
    }

//-------------------------------------------------------
void BaseProjectView::OnCustomTestsDropdown(wxRibbonButtonBarEvent& evt)
    { evt.PopupMenu(&m_customTestsMenu); }

//-------------------------------------------------------
void BaseProjectView::OnOpenDropdown(wxRibbonButtonBarEvent& evt)
    { evt.PopupMenu(&m_fileOpenMenu); }

//-------------------------------------------------------
void BaseProjectView::OnTestBundlesDropdown(wxRibbonButtonBarEvent& evt)
    { evt.PopupMenu(&m_testsBundleMenu); }

//-------------------------------------------------------
void BaseProjectView::OnSaveDropdown(wxRibbonButtonBarEvent& evt)
    { evt.PopupMenu(&m_exportMenu); }

//-------------------------------------------------------
void BaseProjectView::OnPrintDropdown(wxRibbonButtonBarEvent& evt)
    { evt.PopupMenu(&m_printMenu); }

//-------------------------------------------------------
void BaseProjectView::OnExampleDropdown(wxRibbonButtonBarEvent& evt)
    { evt.PopupMenu(&m_exampleMenu); }

//-------------------------------------------------------
void BaseProjectView::OnGraphSortDropdown(wxRibbonButtonBarEvent& evt)
    { evt.PopupMenu(&m_graphSortMenu); }

//-------------------------------------------------------
void BaseProjectView::OnGradeScaleDropdown(wxRibbonButtonBarEvent& evt)
    { evt.PopupMenu(&m_gradeScaleMenu); }

//-------------------------------------------------------
void BaseProjectView::OnRemoveCustomTest(wxCommandEvent& event)
    { wxGetApp().GetMainFrameEx()->OnRemoveCustomTest(event); }

//-------------------------------------------------------
void BaseProjectView::OnAddCustomTest(wxCommandEvent& event)
    { wxGetApp().GetMainFrameEx()->OnAddCustomTest(event); }

//-------------------------------------------------------
void BaseProjectView::OnEditCustomTest(wxCommandEvent& event)
    { wxGetApp().GetMainFrameEx()->OnEditCustomTest(event); }

//-------------------------------------------------------
void BaseProjectView::OnCustomTest(wxCommandEvent& event)
    {
    const int menuId = event.GetId();
    std::map<int, wxString>::const_iterator pos = MainFrame::GetCustomTestMenuIds().find(menuId);
    if (pos == MainFrame::GetCustomTestMenuIds().end())
        {
        wxMessageBox(_(L"Unable to find custom test: internal error, please contact software vendor."),
            _(L"Error"), wxOK|wxICON_ERROR);
        return;
        }
    BaseProjectDoc* doc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    if (doc && doc->IsSafeToUpdate())
        {
        doc->AddCustomReadabilityTest(pos->second);
        // projects will need to do a full re-indexing
        doc->RefreshRequired(ProjectRefresh::FullReindexing);
        doc->RefreshProject();
        }
    }

//---------------------------------------------------
void BaseProjectView::OnFindMenu([[maybe_unused]] wxCommandEvent& event)
    { GetSearchPanel()->Activate(); }

//---------------------------------------------------
void BaseProjectView::OnFindNext([[maybe_unused]] wxCommandEvent& event)
    {
    wxCommandEvent cmd(wxEVT_NULL);
    GetSearchPanel()->OnSearch(cmd);
    }

//-------------------------------------------------------
wxDocChildFrame* BaseProjectView::CreateChildFrame(wxDocument* doc, wxView* view)
    {
    const wxSize windowSize(std::max(wxGetApp().GetMainFrame()->GetClientSize().GetWidth(), 800),
                            std::max(wxGetApp().GetMainFrame()->GetClientSize().GetHeight(), 600) );

    wxFileName fn(doc->GetFilename());
    wxString title = fn.GetName().length() ? fn.GetName() : wxFileName::StripExtension(doc->GetTitle());
    wxDocChildFrame* subframe = new wxDocChildFrame(doc, view, wxGetApp().GetMainFrame(), wxID_ANY,
                                      title, wxDefaultPosition,
                                      windowSize,
                                      wxDEFAULT_FRAME_STYLE);
    subframe->Show(false);
    if (wxGetApp().GetMainFrame()->IsMaximized())
        {
        subframe->Maximize();
        subframe->SetSize(subframe->GetSize());
        }

    wxIcon appIcon;
    appIcon.CopyFromBitmap(
        wxGetApp().GetResourceManager().
            GetSVG(L"ribbon/app-logo.svg").GetBitmap(GetActiveProjectWindow()->FromDIP(wxSize(32, 32))));
    subframe->SetIcon(appIcon);
    subframe->CenterOnScreen();

    Bind(wxEVT_MENU, &BaseProjectView::OnCustomTest, this,
        wxGetApp().GetMainFrameEx()->CUSTOM_TEST_RANGE.GetFirstId(),
        wxGetApp().GetMainFrameEx()->CUSTOM_TEST_RANGE.GetLastId());
    Bind(wxEVT_MENU, &BaseProjectView::OnCustomTestBundle, this,
        wxGetApp().GetMainFrameEx()->TEST_BUNDLE_RANGE.GetFirstId(),
        wxGetApp().GetMainFrameEx()->TEST_BUNDLE_RANGE.GetLastId());

    Bind(wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
            {
            wxRibbonButtonBarEvent event;
            OnProjectSettings(event);
            },
        wxID_PROPERTIES);

    // bar chart sorting menu
    wxMenuItem* item = new wxMenuItem(&m_graphSortMenu,
        XRCID("ID_SORT_ASCENDING"), _(L"Sort Ascending"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-sort-ascending.svg"));
    m_graphSortMenu.Append(item);

    item = new wxMenuItem(&m_graphSortMenu,
        XRCID("ID_SORT_DESCENDING"), _(L"Sort Descending"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-sort-descending.svg"));
    m_graphSortMenu.Append(item);

    // graph background menu
    // color submenu
    auto graphBackgroundColorSubMenu = new wxMenu{};
    item = new wxMenuItem(graphBackgroundColorSubMenu,
        XRCID("ID_EDIT_GRAPH_BKCOLOR"), _(L"Color..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/color-wheel.svg"));
    graphBackgroundColorSubMenu->Append(item);

    graphBackgroundColorSubMenu->Append(
        new wxMenuItem(graphBackgroundColorSubMenu,
        XRCID("ID_GRAPH_BKCOLOR_FADE"), _(L"Fade"), wxString{}, wxITEM_CHECK));

    m_graphBackgroundMenu.AppendSubMenu(graphBackgroundColorSubMenu, _(L"Background Color"));

    m_graphBackgroundMenu.AppendSeparator();

    // plot color submenu
    auto plotColorSubMenu = new wxMenu{};
    item = new wxMenuItem(plotColorSubMenu,
        XRCID("ID_EDIT_PLOT_BKCOLOR"), _(L"Color..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/color-wheel.svg"));
    plotColorSubMenu->Append(item);

    item = new wxMenuItem(plotColorSubMenu,
        XRCID("ID_EDIT_PLOT_BKCOLOR_OPACITY"), _(L"Opacity..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/opacity.svg"));
    plotColorSubMenu->Append(item);

    m_graphBackgroundMenu.AppendSubMenu(plotColorSubMenu, _(L"Plot Color"));

    // image submenu
    auto graphBackgroundImageSubMenu = new wxMenu{};
    item = new wxMenuItem(graphBackgroundImageSubMenu,
        XRCID("ID_EDIT_PLOT_BKIMAGE"), _(L"Image..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/image.svg"));
    graphBackgroundImageSubMenu->Append(item);

    graphBackgroundImageSubMenu->Append(
        new wxMenuItem(graphBackgroundImageSubMenu,
        XRCID("ID_EDIT_PLOT_BKIMAGE_OPACITY"), _(L"Opacity...")));

    auto graphBackgroundImageEffectSubMenu = new wxMenu{};

    graphBackgroundImageEffectSubMenu->Append(
        new wxMenuItem(graphBackgroundImageEffectSubMenu,
            XRCID("ID_PLOT_BKIMAGE_EFFECT_NO_EFFECT"), _(L"No Effect"), wxString{}, wxITEM_CHECK));
    graphBackgroundImageEffectSubMenu->Append(
        new wxMenuItem(graphBackgroundImageEffectSubMenu,
            XRCID("ID_PLOT_BKIMAGE_EFFECT_GRAYSCALE"), _(L"Grayscale"), wxString{}, wxITEM_CHECK));
    graphBackgroundImageEffectSubMenu->Append(
        new wxMenuItem(graphBackgroundImageEffectSubMenu,
            XRCID("ID_PLOT_BKIMAGE_EFFECT_BLUR_HORIZONTALLY"), _(L"Blur Horizontally"), wxString{}, wxITEM_CHECK));
    graphBackgroundImageEffectSubMenu->Append(
        new wxMenuItem(graphBackgroundImageEffectSubMenu,
            XRCID("ID_PLOT_BKIMAGE_EFFECT_BLUR_VERTICALLY"), _(L"Blur Vertically"), wxString{}, wxITEM_CHECK));
    graphBackgroundImageEffectSubMenu->Append(
        new wxMenuItem(graphBackgroundImageEffectSubMenu,
            XRCID("ID_PLOT_BKIMAGE_EFFECT_SEPIA"), _(L"Sepia"), wxString{}, wxITEM_CHECK));
    graphBackgroundImageEffectSubMenu->Append(
        new wxMenuItem(graphBackgroundImageEffectSubMenu,
            XRCID("ID_PLOT_BKIMAGE_EFFECT_FROSTED_GLASS"), _(L"Frosted Glass"), wxString{}, wxITEM_CHECK));
    graphBackgroundImageEffectSubMenu->Append(
        new wxMenuItem(graphBackgroundImageEffectSubMenu,
            XRCID("ID_PLOT_BKIMAGE_EFFECT_OIL_PAINTING"), _(L"Oil Painting"), wxString{}, wxITEM_CHECK));

    graphBackgroundImageSubMenu->AppendSubMenu(graphBackgroundImageEffectSubMenu, _(L"Effects"));

    auto graphBackgroundImageFitSubMenu = new wxMenu{};

    graphBackgroundImageFitSubMenu->Append(
        new wxMenuItem(graphBackgroundImageFitSubMenu,
            XRCID("ID_PLOT_BKIMAGE_FIT_CROP_AND_CENTER"), _(L"Crop && Center"), wxString{}, wxITEM_CHECK));
    graphBackgroundImageFitSubMenu->Append(
        new wxMenuItem(graphBackgroundImageFitSubMenu,
            XRCID("ID_PLOT_BKIMAGE_FIT_SHRINK"), _(L"Shrink to Fit"), wxString{}, wxITEM_CHECK));

    graphBackgroundImageSubMenu->AppendSubMenu(graphBackgroundImageFitSubMenu, _(L"Fit"));

    item = new wxMenuItem(graphBackgroundImageSubMenu,
        XRCID("ID_EDIT_PLOT_BKIMAGE_REMOVE"), _(L"Remove Image"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/delete.svg"));
    graphBackgroundImageSubMenu->Append(item);

    m_graphBackgroundMenu.AppendSubMenu(graphBackgroundImageSubMenu, _(L"Plot Image"));

    // Raygor styles
    m_raygorStyleMenu.Append(
        XRCID("ID_EDIT_GRAPH_RAYGOR_ORIGINAL"), _(L"Original"))->SetCheckable(true);
    m_raygorStyleMenu.Append(
        XRCID("ID_EDIT_GRAPH_RAYGOR_BALDWIN_KAUFMAN"), _DT(L"Baldwin-Kaufman"))->SetCheckable(true);
    m_raygorStyleMenu.Append(
        XRCID("ID_EDIT_GRAPH_RAYGOR_MODERN"), _(L"Modern"))->SetCheckable(true);

    // histogram bar labels
    m_histobarLabelsMenu.Append(new wxMenuItem(&m_histobarLabelsMenu,
        XRCID("ID_HISTOBAR_LABELS_COUNT"), _(L"Counts"), wxString{}, wxITEM_CHECK));
    m_histobarLabelsMenu.Append(new wxMenuItem(&m_histobarLabelsMenu,
        XRCID("ID_HISTOBAR_LABELS_PERCENTAGE"), _(L"Percentages"), wxString{}, wxITEM_CHECK));
    m_histobarLabelsMenu.Append(new wxMenuItem(&m_histobarLabelsMenu,
        XRCID("ID_HISTOBAR_LABELS_COUNT_AND_PERCENT"), _(L"Counts && Percentages"), wxString{}, wxITEM_CHECK));
    m_histobarLabelsMenu.Append(new wxMenuItem(&m_histobarLabelsMenu,
        XRCID("ID_HISTOBAR_NO_LABELS"), _(L"No labels"), wxString{}, wxITEM_CHECK));

    // zoom
    item = new wxMenuItem(&m_zoomMenu, wxID_ZOOM_IN, _(L"Zoom In"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/zoom-in.svg"));
    m_zoomMenu.Append(item);

    item = new wxMenuItem(&m_zoomMenu, wxID_ZOOM_OUT, _(L"Zoom Out"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/zoom-out.svg"));
    m_zoomMenu.Append(item);

    item = new wxMenuItem(&m_zoomMenu, wxID_ZOOM_FIT, _(L"Reset Zoom"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/zoom-to-fit.svg"));
    m_zoomMenu.Append(item);

    // bar orientation
    item = new wxMenuItem(&m_barOrientationMenu, XRCID("ID_BAR_HORIZONTAL"), _(L"Horizontal"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-chart.svg"));
    m_barOrientationMenu.Append(item);

    item = new wxMenuItem(&m_barOrientationMenu, XRCID("ID_BAR_VERTICAL"), _(L"Vertical"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/histogram.svg"));
    m_barOrientationMenu.Append(item);

    // bar style menu
    item = new wxMenuItem(&m_barStyleMenu, XRCID("ID_EDIT_BAR_COLOR"), _(L"Color..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/color-wheel.svg"));
    m_barStyleMenu.Append(item);

    item = new wxMenuItem(&m_barStyleMenu, XRCID("ID_EDIT_BAR_OPACITY"), _(L"Opacity..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-top-to-bottom.svg"));
    m_barStyleMenu.Append(item);

    item = new wxMenuItem(&m_barStyleMenu,
        XRCID("ID_BAR_SELECT_STIPPLE_IMAGE"), _(L"Select Stipple Image..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/image.svg"));
    m_barStyleMenu.Append(item);

    item = new wxMenuItem(&m_barStyleMenu,
        XRCID("ID_BAR_SELECT_STIPPLE_SHAPE"), _(L"Select Stipple Shape..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/apple.svg"));
    m_barStyleMenu.Append(item);

    item = new wxMenuItem(&m_barStyleMenu,
        XRCID("ID_BAR_SELECT_COMMON_IMAGE"), _(L"Select Common Image..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/image.svg"));
    m_barStyleMenu.Append(item);

    m_barStyleMenu.AppendSeparator();

    item = new wxMenuItem(&m_barStyleMenu, XRCID("ID_BAR_STYLE_SOLID"), _(L"Solid"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-solid.svg"));
    m_barStyleMenu.Append(item);

    item = new wxMenuItem(&m_barStyleMenu, XRCID("ID_BAR_STYLE_GLASS"), _(L"Glass Effect"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-glass.svg"));
    m_barStyleMenu.Append(item);

    item = new wxMenuItem(&m_barStyleMenu, XRCID("ID_BAR_STYLE_BTOT"), _(L"Color fade, Bottom-to-top"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-bottom-to-top.svg"));
    m_barStyleMenu.Append(item);

    item = new wxMenuItem(&m_barStyleMenu, XRCID("ID_BAR_STYLE_TTOB"), _(L"Color Fade, Top-to-bottom"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-top-to-bottom.svg"));
    m_barStyleMenu.Append(item);

    item = new wxMenuItem(&m_barStyleMenu, XRCID("ID_BAR_STYLE_STIPPLE_IMAGE"), _(L"Stipple Image"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/image.svg"));
    m_barStyleMenu.Append(item);

    item = new wxMenuItem(&m_barStyleMenu, XRCID("ID_BAR_STYLE_STIPPLE_SHAPE"), _(L"Stipple Shape"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/apple.svg"));
    m_barStyleMenu.Append(item);

    item = new wxMenuItem(&m_barStyleMenu, XRCID("ID_BAR_STYLE_WATERCOLOR"), _(L"Watercolor"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/brush.svg"));
    m_barStyleMenu.Append(item);

    item = new wxMenuItem(&m_barStyleMenu, XRCID("ID_BAR_STYLE_THICK_WATERCOLOR"), _(L"Thick Watercolor"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/brush.svg"));
    m_barStyleMenu.Append(item);

    item = new wxMenuItem(&m_barStyleMenu, XRCID("ID_BAR_STYLE_COMMON_IMAGE"), _(L"Common Image"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/image.svg"));
    m_barStyleMenu.Append(item);

    // histogram bar style menu
    item = new wxMenuItem(&m_histoBarStyleMenu,
        XRCID("ID_EDIT_HISTOGRAM_BAR_COLOR"), _(L"Color..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/color-wheel.svg"));
    m_histoBarStyleMenu.Append(item);

    item = new wxMenuItem(&m_histoBarStyleMenu,
        XRCID("ID_EDIT_HISTOBAR_OPACITY"), _(L"Opacity..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-top-to-bottom.svg"));
    m_histoBarStyleMenu.Append(item);

    item = new wxMenuItem(&m_histoBarStyleMenu, XRCID("ID_HISTOGRAM_BAR_SELECT_BRUSH"),
        _(L"Select Stipple Image..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/image.svg"));
    m_histoBarStyleMenu.Append(item);

    item = new wxMenuItem(&m_histoBarStyleMenu,
        XRCID("ID_HISTOGRAM_BAR_SELECT_STIPPLE_SHAPE"), _(L"Select Stipple Shape..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/apple.svg"));
    m_histoBarStyleMenu.Append(item);

    item = new wxMenuItem(&m_histoBarStyleMenu, XRCID("ID_HISTOGRAM_BAR_SELECT_COMMON_IMAGE"),
        _(L"Select Common Image..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/image.svg"));
    m_histoBarStyleMenu.Append(item);

    m_histoBarStyleMenu.AppendSeparator();

    item = new wxMenuItem(&m_histoBarStyleMenu,
        XRCID("ID_HISTOGRAM_BAR_STYLE_SOLID"), _(L"Solid"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-solid.svg"));
    m_histoBarStyleMenu.Append(item);

    item = new wxMenuItem(&m_histoBarStyleMenu,
        XRCID("ID_HISTOGRAM_BAR_STYLE_GLASS"), _(L"Glass Effect"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-glass.svg"));
    m_histoBarStyleMenu.Append(item);

    item = new wxMenuItem(&m_histoBarStyleMenu,
        XRCID("ID_HISTOGRAM_BAR_STYLE_BTOT"), _(L"Color Fade, Bottom-to-top"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-bottom-to-top.svg"));
    m_histoBarStyleMenu.Append(item);

    item = new wxMenuItem(&m_histoBarStyleMenu,
        XRCID("ID_HISTOGRAM_BAR_STYLE_TTOB"), _(L"Color Fade, Top-to-bottom"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-top-to-bottom.svg"));
    m_histoBarStyleMenu.Append(item);

    item = new wxMenuItem(&m_histoBarStyleMenu,
        XRCID("ID_HISTOGRAM_BAR_STYLE_STIPPLE_IMAGE"), _(L"Stipple Image"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/image.svg"));
    m_histoBarStyleMenu.Append(item);

    item = new wxMenuItem(&m_histoBarStyleMenu,
        XRCID("ID_HISTOGRAM_BAR_STYLE_STIPPLE_SHAPE"), _(L"Stipple Shape"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/apple.svg"));
    m_histoBarStyleMenu.Append(item);

    item = new wxMenuItem(&m_histoBarStyleMenu,
        XRCID("ID_HISTOGRAM_BAR_STYLE_WATERCOLOR"), _(L"Watercolor"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/brush.svg"));
    m_histoBarStyleMenu.Append(item);

    item = new wxMenuItem(&m_histoBarStyleMenu,
        XRCID("ID_HISTOGRAM_BAR_STYLE_THICK_WATERCOLOR"), _(L"Thick Watercolor"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/brush.svg"));
    m_histoBarStyleMenu.Append(item);

    item = new wxMenuItem(&m_histoBarStyleMenu,
        XRCID("ID_HISTOGRAM_BAR_STYLE_COMMON_IMAGE"), _(L"Common Image"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/image.svg"));
    m_histoBarStyleMenu.Append(item);

    // box style menu
    item = new wxMenuItem(&m_boxStyleMenu, XRCID("ID_EDIT_BOX_COLOR"), _(L"Color..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/color-wheel.svg"));
    m_boxStyleMenu.Append(item);

    item = new wxMenuItem(&m_boxStyleMenu, XRCID("ID_EDIT_BOX_OPACITY"), _(L"Opacity..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-top-to-bottom.svg"));
    m_boxStyleMenu.Append(item);

    item = new wxMenuItem(&m_boxStyleMenu, XRCID("ID_BOX_SELECT_STIPPLE_IMAGE"), _(L"Select Stipple Image..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/image.svg"));
    m_boxStyleMenu.Append(item);

    item = new wxMenuItem(&m_boxStyleMenu,
        XRCID("ID_BOX_BAR_SELECT_STIPPLE_SHAPE"), _(L"Select Stipple Shape..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/apple.svg"));
    m_boxStyleMenu.Append(item);

    item = new wxMenuItem(&m_boxStyleMenu, XRCID("ID_BOX_SELECT_COMMON_IMAGE"), _(L"Select Common Image..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/image.svg"));
    m_boxStyleMenu.Append(item);

    m_boxStyleMenu.AppendSeparator();

    item = new wxMenuItem(&m_boxStyleMenu, XRCID("ID_BOX_STYLE_SOLID"), _(L"Solid"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-solid.svg"));
    m_boxStyleMenu.Append(item);

    item = new wxMenuItem(&m_boxStyleMenu, XRCID("ID_BOX_STYLE_GLASS"), _(L"Glass Effect"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-glass.svg"));
    m_boxStyleMenu.Append(item);

    item = new wxMenuItem(&m_boxStyleMenu, XRCID("ID_BOX_STYLE_LTOR"), _(L"Color Fade, Left-to-right"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-left-to-right.svg"));
    m_boxStyleMenu.Append(item);

    item = new wxMenuItem(&m_boxStyleMenu, XRCID("ID_BOX_STYLE_RTOL"), _(L"Color Fade, Right-to-left"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-right-to-left.svg"));
    m_boxStyleMenu.Append(item);

    item = new wxMenuItem(&m_boxStyleMenu, XRCID("ID_BOX_STYLE_STIPPLE_IMAGE"), _(L"Stipple Image"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/image.svg"));
    m_boxStyleMenu.Append(item);

    item = new wxMenuItem(&m_boxStyleMenu, XRCID("ID_BOX_STYLE_STIPPLE_SHAPE"), _(L"Stipple Shape"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/apple.svg"));
    m_boxStyleMenu.Append(item);

    item = new wxMenuItem(&m_boxStyleMenu, XRCID("ID_BOX_STYLE_WATERCOLOR"), _(L"Watercolor"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/brush.svg"));
    m_boxStyleMenu.Append(item);

    item = new wxMenuItem(&m_boxStyleMenu, XRCID("ID_BOX_STYLE_THICK_WATERCOLOR"), _(L"Thick Watercolor"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/brush.svg"));
    m_boxStyleMenu.Append(item);

    item = new wxMenuItem(&m_boxStyleMenu, XRCID("ID_BOX_STYLE_COMMON_IMAGE"), _(L"Common Image"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/image.svg"));
    m_boxStyleMenu.Append(item);

    // graph fonts
    item = new wxMenuItem(&m_graphFontsMenu, XRCID("ID_EDIT_X_AXIS_FONT"), _(L"X Axis..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/x-axis.svg"));
    m_graphFontsMenu.Append(item);

    item = new wxMenuItem(&m_graphFontsMenu, XRCID("ID_EDIT_Y_AXIS_FONT"), _(L"Y Axis..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/y-axis.svg"));
    m_graphFontsMenu.Append(item);

    m_graphFontsMenu.AppendSeparator();

    item = new wxMenuItem(&m_graphFontsMenu, XRCID("ID_EDIT_TOP_TITLES_FONT"), _(L"Top Titles..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/top-titles.svg"));
    m_graphFontsMenu.Append(item);

    item = new wxMenuItem(&m_graphFontsMenu, XRCID("ID_EDIT_BOTTOM_TITLES_FONT"), _(L"Bottom Titles..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/bottom-titles.svg"));
    m_graphFontsMenu.Append(item);

    item = new wxMenuItem(&m_graphFontsMenu, XRCID("ID_EDIT_LEFT_TITLES_FONT"), _(L"Left Titles..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/left-titles.svg"));
    m_graphFontsMenu.Append(item);

    item = new wxMenuItem(&m_graphFontsMenu, XRCID("ID_EDIT_RIGHT_TITLES_FONT"), _(L"Right Titles..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/right-titles.svg"));
    m_graphFontsMenu.Append(item);

    // document indexing menus
    m_lineEndsMenu.Append(new wxMenuItem(&m_lineEndsMenu, XRCID("ID_LE_ONLY_AFTER_VALID_SENTENCE"),
                          _(L"Only begin a new paragraph if following a valid sentence"),
                          wxString{}, wxITEM_CHECK));
    m_lineEndsMenu.Append(new wxMenuItem(&m_lineEndsMenu, XRCID("ID_LE_ALWAYS_NEW_PARAGRAPH"),
                          _(L"Always begin a new paragraph"), wxString{}, wxITEM_CHECK));

    m_longSentencesMenu.Append(new wxMenuItem(&m_longSentencesMenu, XRCID("ID_LS_LONGER_THAN"),
                               _(L"If longer than..."), wxString{}, wxITEM_CHECK));
    m_longSentencesMenu.Append(new wxMenuItem(&m_longSentencesMenu,
        XRCID("ID_LS_OUTLIER_RANGE"), _(L"Outside outlier range"), wxString{}, wxITEM_CHECK));

    m_textExclusionMenu.Append(new wxMenuItem(&m_textExclusionMenu,
        XRCID("ID_TE_ALL_INCOMPLETE"), _(L"Exclude all incomplete sentences"), wxString{}, wxITEM_CHECK));
    m_textExclusionMenu.Append(new wxMenuItem(&m_textExclusionMenu,
        XRCID("ID_TE_NO_EXCLUDE"), _(L"Do not exclude any text"), wxString{}, wxITEM_CHECK));
    m_textExclusionMenu.Append(new wxMenuItem(&m_textExclusionMenu,
        XRCID("ID_TE_ALL_INCOMPLETE_EXCEPT_HEADERS"),
        _(L"Exclude all incomplete sentences, except headings"), wxString{}, wxITEM_CHECK));

    m_exclusionTagsMenu.Append(new wxMenuItem(&m_exclusionTagsMenu,
        XRCID("ID_EXCLUSION_TAGS_NOT_ENABLED"), _(L"Not enabled"), wxString{}, wxITEM_CHECK));
    m_exclusionTagsMenu.Append(new wxMenuItem(&m_exclusionTagsMenu,
        XRCID("ID_EXCLUSION_TAGS_CAROTS"), _(L"^ and ^"), wxString{}, wxITEM_CHECK));
    m_exclusionTagsMenu.Append(new wxMenuItem(&m_exclusionTagsMenu,
        XRCID("ID_EXCLUSION_TAGS_ANGLES"), _(L"< and >"), wxString{}, wxITEM_CHECK));
    m_exclusionTagsMenu.Append(new wxMenuItem(&m_exclusionTagsMenu,
        XRCID("ID_EXCLUSION_TAGS_BRACES"), _(L"[ and ]"), wxString{}, wxITEM_CHECK));
    m_exclusionTagsMenu.Append(new wxMenuItem(&m_exclusionTagsMenu,
        XRCID("ID_EXCLUSION_TAGS_CURLIES"), _(L"{ and }"), wxString{}, wxITEM_CHECK));
    m_exclusionTagsMenu.Append(new wxMenuItem(&m_exclusionTagsMenu,
        XRCID("ID_EXCLUSION_TAGS_PARANS"), _(L"( and )"), wxString{}, wxITEM_CHECK));

    m_numeralSyllabicationMenu.Append(new wxMenuItem(&m_numeralSyllabicationMenu,
        XRCID("ID_NUMSYL_ONE"), _(L"Numerals are one syllable"), wxString{}, wxITEM_CHECK));
    m_numeralSyllabicationMenu.Append(new wxMenuItem(&m_numeralSyllabicationMenu,
        XRCID("ID_NUMSYL_EACH_DIGIT"), _(L"Sound out each digit"), wxString{}, wxITEM_CHECK));

    return subframe;
    }

//-------------------------------------------------------
bool BaseProjectView::OnClose(bool deleteWindow)
    {
    if (!wxView::OnClose(deleteWindow) )
        {
        wxLogWarning(L"Project view not successfully closed.");
        return false;
        }

    if (deleteWindow)
        { m_frame->Destroy(); }
    SetFrame(nullptr);
    Activate(false);

    if (wxGetApp().GetDocumentCount() == 1)
        {
        // show the empty mainframe when the last document is being closed
        wxArrayString mruFiles;
        for (size_t i = 0; i < wxGetApp().GetDocManager()->GetFileHistory()->GetCount(); ++i)
            { mruFiles.Add(wxGetApp().GetDocManager()->GetFileHistory()->GetHistoryFile(i)); }
        wxGetApp().GetMainFrameEx()->GetStartPage()->SetMRUList(mruFiles);
        wxGetApp().GetMainFrame()->CenterOnScreen();
        wxGetApp().GetMainFrame()->Show();
        }

    return true;
    }

//-------------------------------------------------------
void BaseProjectView::Present()
    {
#ifdef __WXOSX__
    // Associate the menu with this view.
    if (m_frame->GetMenuBar() == nullptr)
        { m_frame->SetMenuBar(GetMenuBar()); }
#endif
    if (m_frame->GetMenuBar())
        {
        wxGetApp().GetDocManager()->FileHistoryUseMenu(m_frame->GetMenuBar()->GetMenu(0));
        if (m_frame->GetMenuBar()->GetMenu(0)->FindItem(wxID_FILE1) == nullptr)
            { wxGetApp().GetDocManager()->FileHistoryAddFilesToMenu(m_frame->GetMenuBar()->GetMenu(0)); }
        }
    // disable unlicensed features
    BaseProjectDoc* doc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureEnglishReadabilityTestsCode()))
        {
        for (auto rTest = doc->GetReadabilityTests().get_tests().begin();
            rTest != doc->GetReadabilityTests().get_tests().end();
            ++rTest)
            {
            if (rTest->get_test().has_language(readability::test_language::english_test))
                { MenuBarEnableAll(GetMenuBar(), rTest->get_test().get_interface_id(), false); }
            }
        }

    // update menus that might change due to the project's language being changed
    if (GetMenuBar())
        {
        int readMenuIndex = GetMenuBar()->FindMenu(_(L"Readability"));
        // add menu if needed, although this shouldn't happen
        if (readMenuIndex == wxNOT_FOUND)
            {
            GetMenuBar()->Append(new wxMenu(), _(L"Readability"));
            readMenuIndex = static_cast<int>(GetMenuBar()->GetMenuCount()-1);
            }
        wxMenu* readMenu = GetMenuBar()->GetMenu(readMenuIndex);
        if (readMenu)
            {
            // remove all the submenus
            wxMenuItemList menuItems = readMenu->GetMenuItems();
            for (wxMenuItemList::iterator iter = menuItems.begin(); iter != menuItems.end(); ++iter)
                { readMenu->Destroy(*iter); }
            // add all the submenus for the different groupings
            wxMenu* primaryMenu = new wxMenu;
            wxMenu* secondaryMenu = new wxMenu;
            wxMenu* adultMenu = new wxMenu;
            wxMenu* secondLanguageMenu = new wxMenu;
            m_customTestsRegularMenu = new wxMenu;
            m_testsBundleRegularMenu = new wxMenu;
            readMenu->AppendSubMenu(primaryMenu, _(L"Primary-age Reading (5-12 years old)"));
            readMenu->AppendSubMenu(secondaryMenu, _(L"Secondary-age Reading (13 years old and above)"));
            readMenu->AppendSubMenu(adultMenu, _(L"Adult Reading (technical forms and documents)"));
            readMenu->AppendSubMenu(secondLanguageMenu, _(L"Second Language Reading"));
            MainFrame::FillReadabilityMenu(primaryMenu, secondaryMenu, adultMenu, secondLanguageMenu, doc);
            // add the custom test menu to the bottom
            readMenu->AppendSeparator();
            readMenu->AppendSubMenu(m_customTestsRegularMenu, _(L"Custom Tests"));
            MainFrame::FillMenuWithCustomTests(m_customTestsRegularMenu, doc, true);
            // add test bundles
            readMenu->AppendSeparator();
            readMenu->AppendSubMenu(m_testsBundleRegularMenu, _(L"Bundles"));
            MainFrame::FillMenuWithTestBundles(m_testsBundleRegularMenu, doc, true);

            // remove any submenus that don't have anything in them
            menuItems = readMenu->GetMenuItems();
            for (wxMenuItemList::iterator iter = menuItems.begin(); iter != menuItems.end(); ++iter)
                {
                if ((*iter)->IsSubMenu() &&
                    (!(*iter)->GetSubMenu() || (*iter)->GetSubMenu()->GetMenuItemCount() == 0))
                    { readMenu->Destroy(*iter); }
                }
            }
        }
    MainFrame::FillReadabilityMenu(&m_primaryAgeTestsMenu, &m_secondaryAgeTestsMenu, &m_adultTestsMenu,
                                   &m_secondLanguageTestsMenu, doc);
    MainFrame::FillMenuWithCustomTests(&m_customTestsMenu, doc, true);
    MainFrame::FillMenuWithTestBundles(&m_testsBundleMenu, doc, true);
    MainFrame::FillMenuWithWordList(&m_wordListMenu);
    // grade scale menu
    MainFrame::FillMenuWithGradeScales(&m_gradeScaleMenu);
    const readability::grade_scale gs = doc->GetReadabilityMessageCatalog().GetGradeScale();
    if (gs == readability::grade_scale::k12_plus_united_states)
        { m_gradeScaleMenu.Check(XRCID("ID_K12_US"), true); }
    else if (gs == readability::grade_scale::k12_plus_newfoundland_and_labrador)
        { m_gradeScaleMenu.Check(XRCID("ID_K12_NEWFOUNDLAND"), true); }
    else if (gs == readability::grade_scale::k12_plus_british_columbia)
        { m_gradeScaleMenu.Check(XRCID("ID_K12_BC"), true); }
    else if (gs == readability::grade_scale::k12_plus_newbrunswick)
        { m_gradeScaleMenu.Check(XRCID("ID_K12_NEW_BRUNSWICK"), true); }
    else if (gs == readability::grade_scale::k12_plus_nova_scotia)
        { m_gradeScaleMenu.Check(XRCID("ID_K12_NOVA_SCOTIA"), true); }
    else if (gs == readability::grade_scale::k12_plus_ontario)
        { m_gradeScaleMenu.Check(XRCID("ID_K12_ONTARIO"), true); }
    else if (gs == readability::grade_scale::k12_plus_saskatchewan)
        { m_gradeScaleMenu.Check(XRCID("ID_K12_SASKATCHEWAN"), true); }
    else if (gs == readability::grade_scale::k12_plus_prince_edward_island)
        { m_gradeScaleMenu.Check(XRCID("ID_K12_PE"), true); }
    else if (gs == readability::grade_scale::k12_plus_manitoba)
        { m_gradeScaleMenu.Check(XRCID("ID_K12_MANITOBA"), true); }
    else if (gs == readability::grade_scale::k12_plus_northwest_territories)
        { m_gradeScaleMenu.Check(XRCID("ID_K12_NT"), true); }
    else if (gs == readability::grade_scale::k12_plus_alberta)
        { m_gradeScaleMenu.Check(XRCID("ID_K12_ALBERTA"), true); }
    else if (gs == readability::grade_scale::k12_plus_nunavut)
        { m_gradeScaleMenu.Check(XRCID("ID_K12_NUNAVUT"), true); }
    else if (gs == readability::grade_scale::quebec)
        { m_gradeScaleMenu.Check(XRCID("ID_QUEBEC"), true); }
    else if (gs == readability::grade_scale::key_stages_england_wales)
        { m_gradeScaleMenu.Check(XRCID("ID_ENGLAND"), true); }

    MainFrame::FillMenuWithBlankGraphs(&m_blankGraphMenu);
    if (GetMenuBar())
        {
        const wxMenuItem* exampleMenuItem = GetMenuBar()->FindItem(XRCID("ID_EXAMPLES"));
        if (exampleMenuItem)
            { wxGetApp().GetMainFrameEx()->AddExamplesToMenu(exampleMenuItem->GetSubMenu()); }
        }
    wxGetApp().GetMainFrameEx()->AddExamplesToMenu(&m_exampleMenu);

    if (doc->GetRaygorStyle() == Wisteria::Graphs::RaygorStyle::Original)
        { m_raygorStyleMenu.Check(XRCID("ID_EDIT_GRAPH_RAYGOR_ORIGINAL"), true); }
    else if (doc->GetRaygorStyle() == Wisteria::Graphs::RaygorStyle::BaldwinKaufman)
        { m_raygorStyleMenu.Check(XRCID("ID_EDIT_GRAPH_RAYGOR_BALDWIN_KAUFMAN"), true); }
    else if (doc->GetRaygorStyle() == Wisteria::Graphs::RaygorStyle::Modern)
        { m_raygorStyleMenu.Check(XRCID("ID_EDIT_GRAPH_RAYGOR_MODERN"), true); }

    // other menu items with checks that need to be checked here
    wxMenuItem* fadeOption = m_graphBackgroundMenu.FindItem(XRCID("ID_GRAPH_BKCOLOR_FADE"));
    if (fadeOption)
        { fadeOption->Check(doc->GetGraphBackGroundLinearGradient()); }

    if (auto tempMenuItem = m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_EFFECT_NO_EFFECT"));
        tempMenuItem != nullptr)
        { tempMenuItem->Check(doc->GetPlotBackGroundImageEffect() == ImageEffect::NoEffect); }
    if (auto tempMenuItem = m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_EFFECT_GRAYSCALE"));
        tempMenuItem != nullptr)
        { tempMenuItem->Check(doc->GetPlotBackGroundImageEffect() == ImageEffect::Grayscale); }
    if (auto tempMenuItem = m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_EFFECT_BLUR_HORIZONTALLY"));
        tempMenuItem != nullptr)
        { tempMenuItem->Check(doc->GetPlotBackGroundImageEffect() == ImageEffect::BlurHorizontal); }
    if (auto tempMenuItem = m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_EFFECT_BLUR_VERTICALLY"));
        tempMenuItem != nullptr)
        { tempMenuItem->Check(doc->GetPlotBackGroundImageEffect() == ImageEffect::BlurVertical); }
    if (auto tempMenuItem = m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_EFFECT_SEPIA"));
        tempMenuItem != nullptr)
        { tempMenuItem->Check(doc->GetPlotBackGroundImageEffect() == ImageEffect::Sepia); }
    if (auto tempMenuItem = m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_EFFECT_FROSTED_GLASS"));
        tempMenuItem != nullptr)
        { tempMenuItem->Check(doc->GetPlotBackGroundImageEffect() == ImageEffect::FrostedGlass); }
    if (auto tempMenuItem = m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_EFFECT_OIL_PAINTING"));
        tempMenuItem != nullptr)
        { tempMenuItem->Check(doc->GetPlotBackGroundImageEffect() == ImageEffect::OilPainting); }

    if (auto tempMenuItem = m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_FIT_CROP_AND_CENTER"));
        tempMenuItem != nullptr)
        { tempMenuItem->Check(doc->GetPlotBackGroundImageFit() == ImageFit::CropAndCenter); }
    if (auto tempMenuItem = m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_FIT_SHRINK"));
        tempMenuItem != nullptr)
        { tempMenuItem->Check(doc->GetPlotBackGroundImageFit() == ImageFit::Shrink); }

    // histogram bin labels
        {
        for (size_t i = 0; i < m_histobarLabelsMenu.GetMenuItemCount(); ++i)
            { m_histobarLabelsMenu.FindItemByPosition(i)->Check(false); }
        wxMenuItem* item = m_histobarLabelsMenu.FindItem(
            (doc->GetHistrogramBinLabelDisplay() == BinLabelDisplay::BinPercentage) ?
                XRCID("ID_HISTOBAR_LABELS_PERCENTAGE") :
            (doc->GetHistrogramBinLabelDisplay() == BinLabelDisplay::BinValue) ?
                XRCID("ID_HISTOBAR_LABELS_COUNT") :
            (doc->GetHistrogramBinLabelDisplay() == BinLabelDisplay::BinValueAndPercentage) ?
                XRCID("ID_HISTOBAR_LABELS_COUNT_AND_PERCENT") :
            XRCID("ID_HISTOBAR_NO_LABELS"));
        if (item)
            { item->Check(true); }
        }
    // long sentences
        {
        for (size_t i = 0; i < m_longSentencesMenu.GetMenuItemCount(); ++i)
            { m_longSentencesMenu.FindItemByPosition(i)->Check(false); }
        wxMenuItem* item = m_longSentencesMenu.FindItem(
            (doc->GetLongSentenceMethod() == LongSentence::LongerThanSpecifiedLength) ?
            XRCID("ID_LS_LONGER_THAN") :
            XRCID("ID_LS_OUTLIER_RANGE"));
        if (item)
            { item->Check(true); }
        }
    // paragraph parsing
        {
        for (size_t i = 0; i < m_lineEndsMenu.GetMenuItemCount(); ++i)
            { m_lineEndsMenu.FindItemByPosition(i)->Check(false); }
        wxMenuItem* item = m_lineEndsMenu.FindItem(
            (doc->GetParagraphsParsingMethod() == ParagraphParse::OnlySentenceTerminatedNewLinesAreParagraphs) ?
            XRCID("ID_LE_ONLY_AFTER_VALID_SENTENCE") : XRCID("ID_LE_ALWAYS_NEW_PARAGRAPH"));
        if (item)
            { item->Check(true); }
        }
    // text exclusion
        {
        for (size_t i = 0; i < m_textExclusionMenu.GetMenuItemCount(); ++i)
            { m_textExclusionMenu.FindItemByPosition(i)->Check(false); }
        wxMenuItem* item = m_textExclusionMenu.FindItem(
            (doc->GetInvalidSentenceMethod() == InvalidSentence::ExcludeFromAnalysis) ?
            XRCID("ID_TE_ALL_INCOMPLETE") :
            (doc->GetInvalidSentenceMethod() == InvalidSentence::IncludeAsFullSentences) ?
            XRCID("ID_TE_NO_EXCLUDE") :
            XRCID("ID_TE_ALL_INCOMPLETE_EXCEPT_HEADERS"));
        if (item)
            { item->Check(true); }
        }
    // tag exclusion
        {
        for (size_t i = 0; i < m_exclusionTagsMenu.GetMenuItemCount(); ++i)
            { m_exclusionTagsMenu.FindItemByPosition(i)->Check(false); }
        wxMenuItem* item = m_exclusionTagsMenu.FindItem(
            (doc->GetExclusionBlockTags().size() == 0) ?
                XRCID("ID_EXCLUSION_TAGS_NOT_ENABLED") :
            (doc->GetExclusionBlockTags().at(0) == std::make_pair(L'^', L'^')) ?
                XRCID("ID_EXCLUSION_TAGS_CAROTS") :
            (doc->GetExclusionBlockTags().at(0) == std::make_pair(L'<', L'>')) ?
                XRCID("ID_EXCLUSION_TAGS_ANGLES") :
            (doc->GetExclusionBlockTags().at(0) == std::make_pair(L'[', L']')) ?
                XRCID("ID_EXCLUSION_TAGS_BRACES") :
            (doc->GetExclusionBlockTags().at(0) == std::make_pair(L'{', L'}')) ?
                XRCID("ID_EXCLUSION_TAGS_CURLIES") :
            (doc->GetExclusionBlockTags().at(0) == std::make_pair(L'(', L')')) ?
                XRCID("ID_EXCLUSION_TAGS_PARANS") :
                XRCID("ID_EXCLUSION_TAGS_NOT_ENABLED"));
        if (item)
            { item->Check(true); }
        }
    // numerals
        {
        for (size_t i = 0; i < m_numeralSyllabicationMenu.GetMenuItemCount(); ++i)
            { m_numeralSyllabicationMenu.FindItemByPosition(i)->Check(false); }
        wxMenuItem* item = m_numeralSyllabicationMenu.FindItem(
            (doc->GetNumeralSyllabicationMethod() == NumeralSyllabize::WholeWordIsOneSyllable) ?
            XRCID("ID_NUMSYL_ONE") :
            XRCID("ID_NUMSYL_EACH_DIGIT"));
        if (item)
            { item->Check(true); }
        }

    if (GetRibbon())
        {
        wxWindow* deductionButtonBar =
            GetRibbon()->FindWindow(MainFrame::ID_PARAGRAPH_DEDUCTION_RIBBON_BUTTON_BAR);
        if (deductionButtonBar && deductionButtonBar->IsKindOf(CLASSINFO(wxRibbonButtonBar)))
            {
            dynamic_cast<wxRibbonButtonBar*>(deductionButtonBar)->ToggleButton(
                XRCID("ID_IGNORE_BLANK_LINES"), doc->GetIgnoreBlankLinesForParagraphsParser());
            dynamic_cast<wxRibbonButtonBar*>(deductionButtonBar)->ToggleButton(
                XRCID("ID_IGNORE_INDENTING"), doc->GetIgnoreIndentingForParagraphsParser());
            dynamic_cast<wxRibbonButtonBar*>(deductionButtonBar)->ToggleButton(
                XRCID("ID_SENTENCES_CAPITALIZED"), doc->GetSentenceStartMustBeUppercased());
            if (dynamic_cast<BaseProjectDoc*>(GetDocument())->GetParagraphsParsingMethod() ==
                ParagraphParse::EachNewLineIsAParagraph)
                {
                dynamic_cast<wxRibbonButtonBar*>(deductionButtonBar)->EnableButton(
                    XRCID("ID_IGNORE_BLANK_LINES"), false);
                dynamic_cast<wxRibbonButtonBar*>(deductionButtonBar)->EnableButton(
                    XRCID("ID_IGNORE_INDENTING"), false);
                }
            }
        wxWindow* exclusionButtonBar = GetRibbon()->FindWindow(MainFrame::ID_TEXT_EXCLUSION_RIBBON_BUTTON_BAR);
        if (exclusionButtonBar && exclusionButtonBar->IsKindOf(CLASSINFO(wxRibbonButtonBar)))
            {
            dynamic_cast<wxRibbonButtonBar*>(exclusionButtonBar)->ToggleButton(
                XRCID("ID_EXCLUDE_AGGRESSIVELY"), doc->IsExcludingAggressively());
            dynamic_cast<wxRibbonButtonBar*>(exclusionButtonBar)->ToggleButton(
                XRCID("ID_EXCLUDE_COPYRIGHT_NOTICES"), doc->IsIgnoringTrailingCopyrightNoticeParagraphs());
            dynamic_cast<wxRibbonButtonBar*>(exclusionButtonBar)->ToggleButton(
                XRCID("ID_EXCLUDE_TRAILING_CITATIONS"), doc->IsIgnoringTrailingCitations());
            dynamic_cast<wxRibbonButtonBar*>(exclusionButtonBar)->ToggleButton(
                XRCID("ID_EXCLUDE_FILE_ADDRESSES"), doc->IsIgnoringFileAddresses());
            dynamic_cast<wxRibbonButtonBar*>(exclusionButtonBar)->ToggleButton(
                XRCID("ID_EXCLUDE_NUMERALS"), doc->IsIgnoringNumerals());
            dynamic_cast<wxRibbonButtonBar*>(exclusionButtonBar)->ToggleButton(
                XRCID("ID_EXCLUDE_PROPER_NOUNS"), doc->IsIgnoringProperNouns());
            // disable exclusion buttons
            dynamic_cast<wxRibbonButtonBar*>(exclusionButtonBar)->EnableButton(
                XRCID("ID_EXCLUDE_AGGRESSIVELY"),
                (doc->GetInvalidSentenceMethod() != InvalidSentence::IncludeAsFullSentences));
            dynamic_cast<wxRibbonButtonBar*>(exclusionButtonBar)->EnableButton(
                XRCID("ID_EXCLUDE_COPYRIGHT_NOTICES"),
                (doc->GetInvalidSentenceMethod() != InvalidSentence::IncludeAsFullSentences));
            dynamic_cast<wxRibbonButtonBar*>(exclusionButtonBar)->EnableButton(
                XRCID("ID_EXCLUDE_TRAILING_CITATIONS"),
                (doc->GetInvalidSentenceMethod() != InvalidSentence::IncludeAsFullSentences));
            dynamic_cast<wxRibbonButtonBar*>(exclusionButtonBar)->EnableButton(
                XRCID("ID_EXCLUDE_FILE_ADDRESSES"),
                (doc->GetInvalidSentenceMethod() != InvalidSentence::IncludeAsFullSentences));
            dynamic_cast<wxRibbonButtonBar*>(exclusionButtonBar)->EnableButton(
                XRCID("ID_EXCLUDE_NUMERALS"),
                (doc->GetInvalidSentenceMethod() != InvalidSentence::IncludeAsFullSentences));
            dynamic_cast<wxRibbonButtonBar*>(exclusionButtonBar)->EnableButton(
                XRCID("ID_EXCLUDE_PROPER_NOUNS"),
                (doc->GetInvalidSentenceMethod() != InvalidSentence::IncludeAsFullSentences));
            dynamic_cast<wxRibbonButtonBar*>(exclusionButtonBar)->EnableButton(
                XRCID("ID_EXCLUDE_WORD_LIST"),
                (doc->GetInvalidSentenceMethod() != InvalidSentence::IncludeAsFullSentences));
            dynamic_cast<wxRibbonButtonBar*>(exclusionButtonBar)->EnableButton(
                XRCID("ID_EXCLUSION_TAGS"),
                (doc->GetInvalidSentenceMethod() != InvalidSentence::IncludeAsFullSentences));
            }
        wxWindow* numeralButtonBar = GetRibbon()->FindWindow(MainFrame::ID_NUMERALS_RIBBON_BUTTON_BAR);
        if (numeralButtonBar && numeralButtonBar->IsKindOf(CLASSINFO(wxRibbonButtonBar)))
            {
            dynamic_cast<wxRibbonButtonBar*>(numeralButtonBar)->EnableButton(
                XRCID("ID_NUMERAL_SYLLABICATION"),
                (doc->GetInvalidSentenceMethod() == InvalidSentence::IncludeAsFullSentences) ?
                    true : !doc->IsIgnoringNumerals());
            }
        }

    // fit the sidebar to its labels' sizes
    const size_t sideBarMinimumWidth = GetSideBar()->AdjustWidthToFitItems();
    GetSplitter()->SetMinimumPaneSize(sideBarMinimumWidth);
    GetSplitter()->SplitVertically(GetSideBar(), GetSplitter()->GetWindow2(), sideBarMinimumWidth);

    m_presentedSuccessfully = true;

    Activate(true);

    if (WarningManager::HasWarning(_DT(L"note-export-from-save")))
        { ShowInfoMessage(*WarningManager::GetWarning(_DT(L"note-export-from-save"))); }
    }

//-------------------------------------------------------
void BaseProjectView::ShowInfoMessage(const WarningMessage& message)
    {
    if (m_lastShownMessageId == message.GetId())
        { return; }
    auto warningIter = WarningManager::GetWarning(message.GetId());
    if (warningIter != WarningManager::GetWarnings().end() &&
        warningIter->ShouldBeShown())
        {
        if (GetInfoBar()->IsShown())
            { AddQueuedMessage(message); }
        else
            {
            GetInfoBar()->IncludeDontShowAgainCheckbox(!message.ShouldOnlyBeShownOnce());
            GetInfoBar()->ShowMessage(message.GetMessage(), message.GetFlags());
            m_lastShownMessageId = message.GetId();
            }
        }
    // message isn't in the general warning system, so just generically add it to be shown
    else if (warningIter == WarningManager::GetWarnings().end())
        { AddQueuedMessage(message); }
    }

//-------------------------------------------------------
void BaseProjectView::OnCloseInfoBar([[maybe_unused]] wxCommandEvent& event)
    {
    if (m_lastShownMessageId.length())
        {
        auto warningIter = WarningManager::GetWarning(m_lastShownMessageId);
        if (warningIter != WarningManager::GetWarnings().end() )
            {
            if (warningIter->ShouldOnlyBeShownOnce())
                { warningIter->Show(false); }
            else
                {
                if (GetInfoBar()->IsDontShowAgainChecked())
                    { warningIter->Show(false); }
                }
            }
        m_lastShownMessageId.clear();
        }
    if (GetQueuedMessages().size())
        {
        auto nextMessage = m_queuedMessages.begin();
        ShowInfoMessage(*nextMessage);
        m_queuedMessages.erase(nextMessage);
        }
    }

//-------------------------------------------------------
void BaseProjectView::ShowSideBar(const bool show /*= true*/)
    {
    wxWindowUpdateLocker noUpdates(GetDocFrame());
    if (show)
        {
        GetSideBar()->AdjustWidthToFitItems();
        GetSplitter()->SetMinimumPaneSize(GetSideBar()->GetMinSize().GetWidth());
        GetSplitter()->SetSashPosition(GetSideBar()->GetMinSize().GetWidth());
        }
    else
        {
        GetSideBar()->SetMinSize({1, 1});
        GetSplitter()->SetMinimumPaneSize(1);
        GetSplitter()->SetSashPosition(1);
        }
    // update the ribbon bar
    wxWindow* projectButtonBarWindow = GetRibbon()->FindWindow(MainFrame::ID_PROJECT_RIBBON_BUTTON_BAR);
    if (projectButtonBarWindow && projectButtonBarWindow->IsKindOf(CLASSINFO(wxRibbonButtonBar)))
        {
        const auto projectButtonBar = dynamic_cast<wxRibbonButtonBar*>(projectButtonBarWindow);
        assert(projectButtonBar && L"Error casting project ribbon bar!");
        if (projectButtonBar)
            { projectButtonBar->ToggleButton(XRCID("ID_SHOW_SIDEBAR"), m_sidebarShown); }
        }
    }

//-------------------------------------------------------
bool BaseProjectView::OnCreate(wxDocument* doc, [[maybe_unused]] long flags)
    {
    // hide the empty mainframe when a document window is opened
    wxGetApp().GetMainFrame()->Hide();

    m_frame = CreateChildFrame(doc, this);
    SetFrame(m_frame);

    m_maxColumnWidth = GetDocFrame()->FromDIP(wxSize(200, 200)).GetWidth();

    m_splitter = new wxSplitterWindow(m_frame, SPLITTER_ID,
                        wxDefaultPosition, m_frame->GetClientSize(),
                        wxSP_LIVE_UPDATE|wxSP_NOBORDER);

    // main sidebar (left side of splitter)
    m_sideBar = wxGetApp().CreateSideBar(m_splitter, BaseProjectView::LEFT_PANE);

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer* quickAccessToolbarSizer = new wxBoxSizer(wxHORIZONTAL);
    // quick access toolbar
    m_quickToolbar = new wxAuiToolBar(m_frame, wxID_ANY);
    auto toolbarArt = new Wisteria::UI::ThemedAuiToolbarArt();
    toolbarArt->SetThemeColor(wxGetApp().GetAppOptions().GetRibbonInactiveTabColor());
    GetQuickToolbar()->SetArtProvider(toolbarArt);
    quickAccessToolbarSizer->Add(GetQuickToolbar(), 1, wxEXPAND);
    // save
    GetQuickToolbar()->AddTool(wxID_SAVE, _(L"Save the project"),
        wxArtProvider::GetBitmapBundle(wxART_FILE_SAVE, wxART_BUTTON));
    // copy
    GetQuickToolbar()->AddTool(wxID_COPY, _(L"Copy selection"),
        wxArtProvider::GetBitmapBundle(wxART_COPY, wxART_BUTTON));
    // print
    GetQuickToolbar()->AddTool(wxID_PRINT, _(L"Print the selected window"),
        wxArtProvider::GetBitmapBundle(wxART_PRINT, wxART_BUTTON));
    GetQuickToolbar()->Realize();

    m_searchCtrl = new SearchPanel(m_frame, wxID_ANY);
    GetSearchPanel()->SetBackgroundColour(wxGetApp().GetAppOptions().GetRibbonInactiveTabColor());
    quickAccessToolbarSizer->Add(GetSearchPanel());
    mainSizer->Add(quickAccessToolbarSizer, 0, wxEXPAND);

    m_ribbon = wxGetApp().CreateRibbon(m_frame, doc);
    mainSizer->Add(m_ribbon, 0, wxEXPAND);

    m_infoBar = new InfoBarEx(m_frame);
    GetInfoBar()->Connect(wxID_CLOSE, wxEVT_BUTTON,
                          wxCommandEventHandler(BaseProjectView::OnCloseInfoBar),
                          nullptr, this);
    GetInfoBar()->SetEffectDuration(250);
    GetInfoBar()->SetShowHideEffects(wxSHOW_EFFECT_SLIDE_TO_BOTTOM, wxSHOW_EFFECT_SLIDE_TO_TOP);
    mainSizer->Add(GetInfoBar(),0,wxEXPAND);

    // initialize ribbon menus
    wxGetApp().FillPrintMenu(m_printMenu, ReadabilityApp::RibbonType::StandardProjectRibbon);
    m_fileOpenMenu.Append(wxID_OPEN, _(L"Open Project...") + L"\tCtrl+O");
    wxGetApp().GetDocManager()->FileHistoryUseMenu(&m_fileOpenMenu);
    if (m_fileOpenMenu.FindItem(wxID_FILE1) == nullptr)
        { wxGetApp().GetDocManager()->FileHistoryAddFilesToMenu(&m_fileOpenMenu); }
    m_workSpaceSizer = new wxBoxSizer(wxHORIZONTAL);
    m_workSpaceSizer->Add(m_splitter,1,wxEXPAND);

    mainSizer->Add(m_workSpaceSizer,1,wxEXPAND);

    m_frame->SetSizer(mainSizer);

    wxAcceleratorEntry accelEntries[16];
    accelEntries[0].Set(wxACCEL_NORMAL, WXK_F1, wxID_HELP);
    accelEntries[1].Set(wxACCEL_CMD, static_cast<int>(L'N'), wxID_NEW);
    accelEntries[2].Set(wxACCEL_CMD, static_cast<int>(L'O'), wxID_OPEN);
    accelEntries[3].Set(wxACCEL_CMD, static_cast<int>(L'V'), wxID_PASTE);
    accelEntries[4].Set(wxACCEL_CMD, static_cast<int>(L'P'), wxID_PRINT);
    accelEntries[5].Set(wxACCEL_CMD, static_cast<int>(L'F'), wxID_FIND);
    accelEntries[6].Set(wxACCEL_CMD, static_cast<int>(L'C'), wxID_COPY);
    accelEntries[7].Set(wxACCEL_CMD, static_cast<int>(L'S'), wxID_SAVE);
    accelEntries[8].Set(wxACCEL_CMD, static_cast<int>(L'A'), wxID_SELECTALL);
    accelEntries[9].Set(wxACCEL_NORMAL, WXK_F5, XRCID("ID_DOCUMENT_REFRESH"));
    accelEntries[10].Set(wxACCEL_CMD, WXK_BACK, XRCID("ID_REMOVE_TEST"));
    accelEntries[11].Set(wxACCEL_CMD, WXK_NUMPAD_DELETE, XRCID("ID_REMOVE_TEST"));
    accelEntries[12].Set(wxACCEL_CMD, WXK_DELETE, XRCID("ID_REMOVE_TEST"));
    accelEntries[13].Set(wxACCEL_NORMAL, WXK_F2, XRCID("ID_SHOW_SIDEBAR"));
    accelEntries[14].Set(wxACCEL_NORMAL, WXK_F3, XRCID("ID_FIND_NEXT"));
    accelEntries[15].Set(wxACCEL_NORMAL, WXK_F4, XRCID("ID_TOGGLE_RIBBON"));
    wxAcceleratorTable accelTable(std::size(accelEntries), accelEntries);
    m_frame->SetAcceleratorTable(accelTable);

    return true;
    }

//-------------------------------------------------------
BaseProjectView::~BaseProjectView()
    {
    if (GetMenuBar())
        { wxGetApp().GetDocManager()->FileHistoryRemoveMenu(GetMenuBar()->GetMenu(0)); }
    wxGetApp().GetDocManager()->FileHistoryRemoveMenu(&m_fileOpenMenu);
    /* In case the document failed to load and we didn't set these to the parent,
       then clean them up ourselves*/
    if (!m_presentedSuccessfully)
        {
        wxLogWarning(L"Project was not fully loaded; Project UI being destroyed manually.");
        m_frame->Destroy(); m_frame = nullptr;
        if (GetMenuBar())
            { m_menuBar->Destroy(); m_menuBar = nullptr; }
        }
    // OnClose() should handle this (even on failure), but doesn't hurt to check this here too.
    if (wxGetApp().GetDocumentCount() == 1)
        {
        // show the empty mainframe when the last document is being closed
        wxArrayString mruFiles;
        for (size_t i = 0; i < wxGetApp().GetDocManager()->GetFileHistory()->GetCount(); ++i)
            { mruFiles.Add(wxGetApp().GetDocManager()->GetFileHistory()->GetHistoryFile(i)); }
        wxGetApp().GetMainFrameEx()->GetStartPage()->SetMRUList(mruFiles);
        wxGetApp().GetMainFrame()->CenterOnScreen();
        wxGetApp().GetMainFrame()->Show();
        wxGetApp().SafeYield(wxGetApp().GetMainFrame(),true);
        }
    }

//-------------------------------------------------------
void BaseProjectView::OnActivateView(bool activate, wxView*, wxView*)
    {
    // if the frame (and its views) are ready for showing then show it
    if (activate && m_presentedSuccessfully && GetDocFrame())
        {
        GetDocFrame()->Show(true);
        GetSplitter()->Show(true);
        if (GetSplitter()->GetWindow2())
            { GetSplitter()->GetWindow2()->Show(true); }
        }
    }

//-------------------------------------------------------
void BaseProjectView::OnProjectSettings([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    BaseProjectDoc* doc = dynamic_cast<BaseProjectDoc*>(GetDocument());
    assert(doc);
    if (!doc || !doc->IsSafeToUpdate())
        { return; }
    ToolsOptionsDlg optionsDlg(GetDocFrame(), doc);
    if (optionsDlg.ShowModal() == wxID_OK)
        {
        doc->RefreshProject();
        UpdateRibbonState();
        }
    }

//-------------------------------------------------------
void BaseProjectView::OnDocumentRefreshMenu([[maybe_unused]] wxCommandEvent& event)
    {
    wxRibbonButtonBarEvent cmd;
    OnDocumentRefresh(cmd);
    }

//-------------------------------------------------------
void BaseProjectView::OnDocumentRefresh([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    BaseProjectDoc* activeProject = dynamic_cast<BaseProjectDoc*>(GetDocument());
    if (!activeProject || !activeProject->IsSafeToUpdate())
        { return; }
    // if a standard project with manually entered text then there is no file to link to
    if (activeProject->IsKindOf(CLASSINFO(ProjectDoc)) &&
        activeProject->GetTextSource() == TextSource::EnteredText)
        {
        wxMessageBox(
            _(L"Only projects that originated from a file can be reloaded. "
               "This project had its text manually typed in and cannot be linked to a file."),
            wxGetApp().GetAppName(), wxOK|wxICON_INFORMATION);
        return;
        }
    // only refresh if we are linking to an external document.
    // if the documents were embedded, then ask if they want to change this to link to the files.
    else if (activeProject->GetDocumentStorageMethod() == TextStorage::EmbedText)
        {
        if (wxMessageBox(
            _(L"Only projects linked to its source document can be reloaded. "
               "This project currently has the original document's text embedded in it. "
               "Do you wish to directly link to the source document?"),
            wxGetApp().GetAppName(), wxYES_NO|wxICON_QUESTION) == wxNO)
            { return; }
        if (activeProject->IsKindOf(CLASSINFO(ProjectDoc)))
            {
            FilePathResolver resolvePath(activeProject->GetOriginalDocumentFilePath(), true);
            if (resolvePath.IsInvalidFile() ||
                (resolvePath.IsLocalOrNetworkFile() &&
                    !wxFile::Exists(activeProject->GetOriginalDocumentFilePath())) )
                {
                wxMessageBox(wxString::Format(
                    _(L"%s: file not found."), activeProject->GetOriginalDocumentFilePath() ),
                    wxGetApp().GetAppName(), wxOK|wxICON_INFORMATION);
                return;
                }
            }
        activeProject->SetDocumentStorageMethod(TextStorage::NoEmbedText);
        }
    // project will need to do a full re-indexing
    activeProject->RefreshRequired(ProjectRefresh::FullReindexing);
    activeProject->RefreshProject();
    }
