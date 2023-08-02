#ifndef __MAIN_APP_H__
#define __MAIN_APP_H__

#include <wx/wx.h>
#include <wx/splitter.h>
#include <wx/file.h>
#include <wx/textdlg.h>
#include <wx/imaglist.h>
#include <wx/srchctrl.h>
#include <wx/ribbon/bar.h>
#include <wx/ribbon/buttonbar.h>
#include <wx/ribbon/gallery.h>
#include <wx/ribbon/toolbar.h>
#include <wx/ribbon/art.h>
#include <wx/evtloop.h>
#include <map>
#include <algorithm>
#include "../../../Licensing/LicenseAdmin.h"
#include "../Wisteria-Dataviz/src/ui/controls/codeeditor.h"
#include "../Wisteria-Dataviz/src/ui/dialogs/printerheaderfooterdlg.h"
#include "../Wisteria-Dataviz/src/util/formulaformat.h"
#include "../Wisteria-Dataviz/src/ui/mainframe.h"
#include "../Wisteria-Dataviz/src/ui/dialogs/functionbrowserdlg.h"
#include "../Wisteria-Dataviz/src/ui/controls/listctrlex.h"
#include "../Wisteria-Dataviz/src/util/idhelpers.h"
#include "../Wisteria-Dataviz/src/math/mathematics.h"
#include "../Wisteria-Dataviz/src/i18n-check/src/donttranslate.h"
#include "../Wisteria-Dataviz/src/util/LogFile.h"
#include "../Wisteria-Dataviz/src/ui/mainframe.h"
#include "../Wisteria-Dataviz/src/ui/app.h"
#include "../Wisteria-Dataviz/src/wxStartPage/startpage.h"
#include "../Wisteria-Dataviz/src/util/xml_format.h"
#include "../Wisteria-Dataviz/src/util/screenshot.h"
#include "../webharvester/webharvester.h"
#include "../readability/custom_readability_test.h"
#include "../readability/readability_project_test.h"
#include "../projects/standard_project_doc.h"
#include "../app/readability_app_options.h"
#include "../ui/dialogs/word_list_dlg.h"
#include "../ui/dialogs/aboutdlgex.h"
#include "../ui/dialogs/lua_editor_dlg.h"
#include "../ui/dialogs/webharvesterdlg.h"
#include "../lua_scripting/lua_interface.h"

/// @brief Extended icon provider, which is connected to application's
///     custom icons.
class RSArtProvider final : public wxArtProvider
    {
public:
    RSArtProvider();
protected:
    [[nodiscard]]
    wxBitmapBundle CreateBitmapBundle(const wxArtID& id,
        const wxArtClient& client,
        const wxSize& size) final;
private:
    std::map<wxArtID, wxString> m_idFileMap;
    };

class MainFrame final : public Wisteria::UI::BaseMainFrame
    {
public:
    friend class ReadabilityApp;

    MainFrame(wxDocManager* manager, wxFrame* frame,
              const wxArrayString& defaultFileExtensions, const wxString& title,
              const wxPoint& pos, const wxSize& size, long type);
    MainFrame(const MainFrame&) = delete;
    MainFrame& operator=(const MainFrame&) = delete;
    ~MainFrame()
        {
        wxLogDebug(__WXFUNCTION__);
        }
    void OnAbout([[maybe_unused]] wxCommandEvent& event);
    void OnStartPageClick(wxCommandEvent& event);
    void OnOpenDocument([[maybe_unused]] wxCommandEvent& event);
    void OnPaste([[maybe_unused]] wxCommandEvent& event);
    void OnPrinterHeaderFooter([[maybe_unused]] wxCommandEvent& event);
    void OnBlankGraph(wxCommandEvent& event);
    void OnTestsOverview([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnViewLogReport([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnViewProfileReport([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnToolsOptions([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnEditWordList([[maybe_unused]] wxCommandEvent& event);
    void OnEditPhraseList([[maybe_unused]] wxCommandEvent& event);
    void OnToolsWebHarvest([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnToolsChapterSplit([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnFindDuplicateFiles([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnHelpContents([[maybe_unused]] wxCommandEvent& event) final;
    void OnHelpManual([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnHelpCheckForUpdates([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnOpenExample(wxCommandEvent& event);
    // pane events
    void OnWordListByPage(wxCommandEvent& event);
    void OnScriptEditor([[maybe_unused]] wxCommandEvent& event);
    // custom test events
    void OnAddCustomTest(wxCommandEvent& event);
    void OnEditCustomTest([[maybe_unused]] wxCommandEvent& event);
    void OnRemoveCustomTest([[maybe_unused]] wxCommandEvent& event);
    // custom test bundles
    void OnAddCustomTestBundle([[maybe_unused]] wxCommandEvent& event);
    void OnEditCustomTestBundle([[maybe_unused]] wxCommandEvent& event);
    void OnRemoveCustomTestBundle([[maybe_unused]] wxCommandEvent& event);
    // ribbon events
    void OnWordList(wxRibbonButtonBarEvent& event);
    void OnWordListDropdown(wxRibbonButtonBarEvent& event);
    void OnBlankGraphDropdown(wxRibbonButtonBarEvent& event);
    void OnNewDropdown(wxRibbonButtonBarEvent& event);
    void OnOpenDropdown(wxRibbonButtonBarEvent& event);
    void OnPrintDropdown(wxRibbonButtonBarEvent& event);
    void OnCustomTestsDropdown(wxRibbonButtonBarEvent& event);
    void OnTestBundlesDropdown(wxRibbonButtonBarEvent& event);
    void OnExampleDropdown(wxRibbonButtonBarEvent& event);
    void OnDictionaryDropdown(wxRibbonButtonBarEvent& event);
    void OnRibbonBarHelpClicked([[maybe_unused]] wxRibbonBarEvent& event);

    void OnEditEnglishDictionary([[maybe_unused]] wxCommandEvent& event);
    void OnEditDictionarySettings([[maybe_unused]] wxCommandEvent& event);

    static void FillMenuWithGradeScales(wxMenu* menu);
    static void FillMenuWithWordList(wxMenu* wordListMenu);
    static void FillMenuWithBlankGraphs(wxMenu* blankGraphsMenu);
    static void FillMenuWithTestBundles(wxMenu* testBundleMenu, const BaseProject* project,
                                        const bool includeDocMenuItems);
    static void FillMenuWithCustomTests(wxMenu* customTestMenu, const BaseProject* project,
                                        const bool includeDocMenuItems);
    void AddExamplesToMenu(wxMenu* exampleMenu);
    static void FillReadabilityMenu(wxMenu* primaryMenu, wxMenu* secondaryMenu,
                                    wxMenu* adultMenu, wxMenu* secondLanguageMenu,
                                    const BaseProject* project);

    [[nodiscard]]
    static const std::map<int, wxString>& GetTestBundleMenuIds() noexcept
        { return m_testBundleMenuIds; }

    [[nodiscard]]
    static const std::map<int, wxString>& GetCustomTestMenuIds() noexcept
        { return m_customTestMenuIds; }

    [[nodiscard]]
    static const std::map<int, wxString>& GetExamplesMenuIds() noexcept
        { return m_examplesMenuIds; }

    void AddTestBundleToMenus(const wxString& bundleName);
    void RemoveTestBundleFromMenus(const wxString& bundleName);
    /// This also adds the new test to the menu of each open document/view, as well as the mainframe.
    void AddCustomTestToMenus(const wxString& testName);
    void RemoveCustomTestFromMenus(const wxString& testName);

    void Paste();

    [[nodiscard]]
    std::vector<wxBitmapBundle>& GetProjectSideBarImageList() noexcept
        { return m_projectSideBarImageList; }
    [[nodiscard]]
    wxStartPage* GetStartPage() const noexcept
        { return m_startPage; }

    /// @brief Sets the About dialog image.
    /// @param bmp The image.
    void SetAboutDialogImage(const wxBitmap& bmp)
        { m_aboutBmp = bmp; }
    /// @returns The image used for a custom About dialog.
    [[nodiscard]]
    const wxBitmap& GetAboutDialogImage() const noexcept
        { return m_aboutBmp; }

    static constexpr int ID_EDIT_RIBBON_BUTTON_BAR = wxID_HIGHEST;
    static constexpr int ID_PROOFING_RIBBON_BUTTON_BAR = wxID_HIGHEST + 1;
    static constexpr int ID_PARAGRAPH_DEDUCTION_RIBBON_BUTTON_BAR = wxID_HIGHEST + 2;
    static constexpr int ID_TEXT_EXCLUSION_RIBBON_BUTTON_BAR = wxID_HIGHEST + 3;
    static constexpr int ID_NUMERALS_RIBBON_BUTTON_BAR = wxID_HIGHEST + 4;

    IdRangeLock CUSTOM_TEST_RANGE;
    IdRangeLock EXAMPLE_RANGE;
    IdRangeLock TEST_BUNDLE_RANGE;

    wxMenu m_fileOpenMenu;
    wxMenu m_printMenu;
    wxMenu m_exampleMenu;
    wxMenu m_customTestsMenu;
    wxMenu m_wordListMenu;
    wxMenu m_blankGraphMenu;
    wxMenu* m_customTestsRegularMenu{ nullptr };
    wxMenu m_testsBundleMenu;
    wxMenu* m_testsBundleRegularMenu{ nullptr };
    /// @returns The lua editor dialog (may be null if not opened yet).
    LuaEditorDlg* GetLuaEditor() noexcept
        { return m_luaEditor; }
private:
    static std::map<int, wxString> m_testBundleMenuIds;
    static std::map<int, wxString> m_customTestMenuIds;
    static std::map<int, wxString> m_examplesMenuIds;

    std::vector<wxBitmapBundle> m_projectSideBarImageList;

    wxStartPage* m_startPage{ nullptr };
    LuaEditorDlg* m_luaEditor{ nullptr };

    wxBitmap m_aboutBmp;

    wxDECLARE_CLASS(MainFrame);
    wxDECLARE_EVENT_TABLE();
    };

// Define a new application
class ReadabilityApp final : public Wisteria::UI::BaseApp
    {
public:
    ReadabilityApp() : m_appOptions(nullptr)
        {}
    ReadabilityApp(const ReadabilityApp&) = delete;
    ReadabilityApp& operator=(const ReadabilityApp&) = delete;
    ~ReadabilityApp()
        { wxDELETE(m_appOptions); }
    void OnEventLoopEnter(wxEventLoopBase* loop) final;
    bool OnInit() final;
    int OnExit() final;

    void LoadInterface();
    void LoadInterfaceLicensableFeatures();

    [[nodiscard]]
    MainFrame* GetMainFrameEx() noexcept
        { return dynamic_cast<MainFrame*>(GetMainFrame()); }

    [[nodiscard]]
    wxString GetAppVersion() const
        { return L"2021.2"; }
    void EditCustomTest(CustomReadabilityTest& selectedTest);
    /// web harvester info
    [[nodiscard]]
    wxArrayString GetLastSelectedWebPages() const
        { return m_lastSelectedWebPages; }
    void SetLastSelectedWebPages(const wxArrayString& webpages)
        { m_lastSelectedWebPages = webpages; }
    [[nodiscard]]
    wxString GetLastSelectedDocFilter() const
        { return m_lastSelectedDocFilter; }
    void SetLastSelectedDocFilter(const wxString& filter)
        { m_lastSelectedDocFilter = filter; }
    // stores last used options of web harvester
    WebHarvester m_harvesterOptions;

    [[nodiscard]]
    ReadabilityAppOptions& GetAppOptions()
        {
        if (m_appOptions == nullptr)
            { m_appOptions = new ReadabilityAppOptions; }
        return *m_appOptions;
        }

    [[nodiscard]]
    LicenseAdmin& GetLicenseAdmin() noexcept
        { return m_licenseAdmin; }
    // licensing codes
    [[nodiscard]]
    static const wxString FeatureEnglishReadabilityTestsCode()
        { return _DT(L"ERDT"); }
    [[nodiscard]]
    static const wxString FeatureProfessionalCode()
        { return _DT(L"RPRO"); }
    [[nodiscard]]
    static const wxString FeatureLanguagePackCode()
        { return _DT(L"LPRT"); }

    LuaInterpreter& GetLuaRunner() noexcept
        { return m_LuaRunner; }

    [[nodiscard]]
    WebHarvester& GetWebHarvester() noexcept
        { return m_webHarvester; }

    enum class RibbonType
        {
        MainFrameRibbon,
        StandardProjectRibbon,
        BatchProjectRibbon
        };

    void InitStartPage();
    void InitProjectSidebar();
    void InitScriptEditor();

    Wisteria::UI::SideBar* CreateSideBar(wxWindow* frame, const wxWindowID id);
    wxRibbonBar* CreateRibbon(wxWindow* frame, const wxDocument* doc);

    void UpdateRibbonTheme();
    void UpdateDocumentThemes();
    void UpdateStartPageTheme();
    void UpdateScriptEditorTheme();
    void UpdateSideBarTheme(Wisteria::UI::SideBar* sidebar);
    void UpdateRibbonTheme(wxRibbonBar* ribbon);

    void FillPrintMenu(wxMenu& printMenu, const RibbonType rtype);
    /// Adds a list of words to the custom dictionary
    ///     (based on language of the project that it is coming from).
    void AddWordsToDictionaries(const wxArrayString& theWords, const readability::test_language lang);
    void EditDictionary(const readability::test_language lang);
    /// This is just used for testing purposes, to validate that the word lists are properly presorted.
    bool VerifyWordLists();
    /// @returns @c true if parsers should import extra content (dependent on parser).
    [[nodiscard]]
    bool IsUsingAdvancedImport() const noexcept
        { return m_advancedImport; }
    void RemoveAllCustomTestBundles();
    /// @returns The DPI scaling factor (e.g., can be 2 on HiDPI displays).
    ///     This is a convenient way of getting this value when a window isn't available.
    [[nodiscard]]
    double GetDPIScaleFactor() const noexcept
        { return m_dpiScaleFactor; }

    void AddSplashscreenImagePath(const wxString& imagePath)
        { m_splashscreenImagePaths.Add(imagePath); }

    /** @returns A bitmap from the resource manager, scaling it to the system's DPI.
        @param image The path to the image from the resource manager (e.g., "ribbon/logo.png").
        @param type The image type of @c image.
        @param imageSize The size of the image (in pixels).\n
            This function will scale this according to the system's DPI.*/
    [[nodiscard]]
    wxBitmap GetScaledImage(const wxString& image,
                                          const wxBitmapType type = wxBITMAP_TYPE_PNG,
                                          const wxSize imageSize = wxSize(32, 32))
        {
        assert(GetMainFrame());
        const wxSize scaledSize = GetMainFrame()->FromDIP(imageSize);
        wxBitmap bmp = GetResourceManager().GetBitmap(image, type);

        // Only resize if being downscaled. If the original image is smaller than the
        // requested size, then return the original image (i.e., don't upscale it).
        const auto [width,height] = geometry::downscaled_size(
            std::make_pair(bmp.GetWidth(), bmp.GetHeight()),
            std::make_pair(scaledSize.GetWidth(), scaledSize.GetHeight()));

        return bmp.ConvertToImage().Rescale(width, height, wxIMAGE_QUALITY_HIGH);
        }

    [[nodiscard]]
    const wxArrayString& GetSplashscreenPaths() const noexcept
        { return m_splashscreenImagePaths; }

    /// @returns A random number generator engine for use in calls to std::uniform_xxx().
    [[nodiscard]]
    std::mt19937_64& GetRandomNumberEngine() noexcept
        { return m_mersenneTwister; }

    void ShowSplashscreen();

    [[nodiscard]]
    static const std::map<wxWindowID, wxWindowID>& GetDynamicIdMap() noexcept
        { return m_dynamicIdMap; }
private:
    void InitializeReadabilityFeatures();

    /// @brief IDs exposed to scripting and their respective dynamic IDs in the framework.
    /// @details Set (or add to this) in your framework's initialization.
    static std::map<wxWindowID, wxWindowID> m_dynamicIdMap;

    LicenseAdmin m_licenseAdmin;
    ReadabilityAppOptions* m_appOptions{ nullptr };
    bool LoadWordLists(const wxString& AppSettingFolderPath);
    wxArrayString m_lastSelectedWebPages;
    wxString m_lastSelectedDocFilter;
    LuaInterpreter m_LuaRunner;
    wxString m_CustomEnglishDictionaryPath;
    wxString m_CustomSpanishDictionaryPath;
    wxString m_CustomGermanDictionaryPath;
    bool m_advancedImport{ false };
    double m_dpiScaleFactor{ 1 };
    wxArrayString m_splashscreenImagePaths;
    WebHarvester m_webHarvester;
    std::mt19937_64 m_mersenneTwister;
    };

#endif //__MAIN_APP_H__
