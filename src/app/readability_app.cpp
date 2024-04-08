#include "../document_helpers/chapter_split.h"
#include "../projects/standard_project_doc.h"
#include "../projects/batch_project_doc.h"
#include "../projects/batch_project_view.h"
#include "../projects/standard_project_view.h"
#include "../ui/dialogs/custom_test_dlg.h"
#include "../ui/dialogs/edit_word_list_dlg.h"
#include "../ui/dialogs/new_custom_test_simple_dlg.h"
#include "../ui/dialogs/project_wizard_dlg.h"
#include "../ui/dialogs/test_bundle_dlg.h"
#include "../ui/dialogs/tools_options_dlg.h"
#include "../Wisteria-Dataviz/src/CRCpp/inc/CRC.h"
#include "../Wisteria-Dataviz/src/ui/dialogs/filelistdlg.h"
#include "../Wisteria-Dataviz/src/ui/dialogs/getdirdlg.h"
#include "../Wisteria-Dataviz/src/ui/dialogs/graphdlg.h"
#include "../Wisteria-Dataviz/src/ui/dialogs/radioboxdlg.h"
#include "../Wisteria-Dataviz/src/ui/ribbon/artmetro.h"
#include "readability_app.h"

using namespace Wisteria;
using namespace Wisteria::GraphItems;
using namespace Wisteria::Graphs;
using namespace Wisteria::UI;

// ===========================================================================
// implementation
// ===========================================================================
wxIMPLEMENT_APP(ReadabilityApp);

std::map<wxWindowID, wxWindowID> ReadabilityApp::m_dynamicIdMap;

RSArtProvider::RSArtProvider()
    {
    // cppcheck-suppress useInitializationList
    m_idFileMap = {
        { wxART_FILE_OPEN, L"ribbon/file-open.svg" },
        { wxART_FILE_SAVE, L"ribbon/file-save.svg" },
        { wxART_NEW, L"ribbon/document.svg" },
        { wxART_PRINT, L"ribbon/print.svg" },
        { wxART_CLOSE, L"ribbon/delete.svg" },
        { wxART_DELETE, L"ribbon/delete.svg" },
        { wxART_COPY, L"ribbon/copy.svg" },
        { wxART_CUT, L"ribbon/cut.svg" },
        { wxART_PASTE, L"ribbon/paste.svg" },
        { wxART_UNDO, L"ribbon/undo.svg" },
        { wxART_REDO, L"ribbon/redo.svg" },
        { wxART_HELP_BOOK, L"ribbon/help.svg" },
        { wxART_GO_DOWN, L"ribbon/down-arrow.svg" },
        { wxART_GO_UP, L"ribbon/up-arrow.svg" },
        { wxART_FIND, L"ribbon/find.svg" },
        { wxART_FIND_AND_REPLACE, L"ribbon/find-replace.svg" },
        { L"ID_ALIGN_LEFT", L"ribbon/align-left.svg" },
        { L"ID_ALIGN_CENTER", L"ribbon/align-center.svg" },
        { L"ID_ALIGN_RIGHT", L"ribbon/align-right.svg" },
        { L"ID_ALIGN_JUSTIFIED", L"ribbon/justified.svg" },
        { L"ID_PARAGRAPH_INDENT", L"ribbon/paragraph-indent.svg" },
        { L"ID_LINE_SPACING", L"ribbon/line-spacing.svg" },
        { L"ID_LIST_SORT", L"ribbon/sort.svg" },
        { L"ID_ADD", L"ribbon/add.svg" },
        { L"ID_GROUP", L"ribbon/group.svg" },
        { L"ID_RUN", L"ribbon/run.svg" },
        { L"ID_CLEAR", L"ribbon/clear.svg" },
        { L"ID_DOCUMENT", L"ribbon/document.svg" },
        { L"ID_DOCUMENTS", L"ribbon/documents.svg" },
        { L"ID_EDIT_DOCUMENT", L"ribbon/edit-document.svg" },
        { L"ID_FUNCTION", L"ribbon/function.svg" },
        { L"ID_SPREADSHEET", L"ribbon/spreadsheet.svg" },
        { L"ID_ARCHIVE", L"ribbon/archive.svg" },
        { L"ID_GEARS", L"ribbon/gears.svg" },
        { L"ID_WEB_EXPORT", L"ribbon/web-export.svg" },
        { L"ID_EDIT", L"ribbon/edit.svg" },
        { L"ID_FONT", L"ribbon/font.svg" },
        { L"ID_SELECT_ALL", L"ribbon/select-all.svg" },
        { L"ID_REFRESH", L"ribbon/reload.svg" },
        { L"ID_REALTIME_UPDATE", L"ribbon/realtime.svg" },
    };
    }

//-------------------------------------------
wxBitmapBundle RSArtProvider::CreateBitmapBundle(const wxArtID& id, const wxArtClient& client,
                                                 const wxSize& size)
    {
    const auto filePath = m_idFileMap.find(id);

    return (filePath != m_idFileMap.cend()) ?
               wxGetApp().GetResourceManager().GetSVG(filePath->second) :
               wxArtProvider::CreateBitmapBundle(id, client, size);
    }

//-------------------------------------------
void ReadabilityApp::EditDictionary(const readability::test_language lang)
    {
    EditWordListDlg editDlg(
        GetMainFrame(), wxID_ANY,
        (lang == readability::test_language::spanish_test) ? _(L"Custom Spanish Dictionary") :
        (lang == readability::test_language::german_test)  ? _(L"Custom German Dictionary") :
                                                             _(L"Custom English Dictionary"));
    if (lang == readability::test_language::spanish_test)
        {
        editDlg.SetFilePath(m_CustomSpanishDictionaryPath);
        }
    else if (lang == readability::test_language::german_test)
        {
        editDlg.SetFilePath(m_CustomGermanDictionaryPath);
        }
    else
        {
        editDlg.SetFilePath(m_CustomEnglishDictionaryPath);
        }
    editDlg.SetHelpTopic(GetMainFrame()->GetHelpDirectory(), L"document-analysis.html");
    if (editDlg.ShowModal() == wxID_OK)
        {
        // custom dictionary will be written back to with edit words at this point,
        // so just reload it.
        wxBusyCursor wait;
        // reload the custom dictionary
        if (lang == readability::test_language::spanish_test)
            {
            wxString ExtraDictionaryText;
            if (wxFile::Exists(m_CustomSpanishDictionaryPath) &&
                Wisteria::TextStream::ReadFile(m_CustomSpanishDictionaryPath, ExtraDictionaryText))
                {
                BaseProject::known_custom_spanish_spellings.load_words(ExtraDictionaryText, true,
                                                                       false);
                }
            }
        else if (lang == readability::test_language::german_test)
            {
            wxString ExtraDictionaryText;
            if (wxFile::Exists(m_CustomGermanDictionaryPath) &&
                Wisteria::TextStream::ReadFile(m_CustomGermanDictionaryPath, ExtraDictionaryText))
                {
                BaseProject::known_custom_german_spellings.load_words(ExtraDictionaryText, true,
                                                                      false);
                }
            }
        else
            {
            wxString ExtraDictionaryText;
            if (wxFile::Exists(m_CustomEnglishDictionaryPath) &&
                Wisteria::TextStream::ReadFile(m_CustomEnglishDictionaryPath, ExtraDictionaryText))
                {
                BaseProject::known_custom_english_spellings.load_words(ExtraDictionaryText, true,
                                                                       false);
                }
            }

        // reload the projects
        auto& docs = wxGetApp().GetDocManager()->GetDocuments();
        for (size_t i = 0; i < docs.GetCount(); ++i)
            {
            BaseProjectDoc* doc = dynamic_cast<BaseProjectDoc*>(docs.Item(i)->GetData());
            doc->RefreshRequired(ProjectRefresh::FullReindexing);
            doc->RefreshProject();
            }
        }
    }

//-------------------------------------------
void ReadabilityApp::ShowSplashscreen()
    {
    if (m_splashscreenImagePaths.GetCount() > 0 &&
        // holding down shift will prevent the splashscreen from showing
        !wxGetMouseState().ShiftDown() &&
        // if there are command lines then don't show the splashscreen,
        // an error message may appear under it and lock the program
        argc < 2)
        {
        std::uniform_int_distribution<size_t> randNum(0, GetSplashscreenPaths().GetCount() - 1);
        const size_t imageIndex = randNum(GetRandomNumberEngine());
        if (imageIndex < GetSplashscreenPaths().GetCount())
            {
            wxString ext{ GetSplashscreenPaths()[imageIndex] };
            wxBitmap bitmap =
                GetScaledImage(GetSplashscreenPaths()[imageIndex],
                               Image::GetImageFileTypeFromExtension(ext), wxSize(800, 600));
            if (bitmap.IsOk())
                {
                bitmap = CreateSplashscreen(bitmap, GetAppName(), GetAppSubName(), GetVendorName(),
                                            true);

                [[maybe_unused]] wxSplashScreen* splash = new wxSplashScreen(
                    bitmap, wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_TIMEOUT, 2000, GetMainFrame(), -1,
                    wxDefaultPosition, wxDefaultSize, wxFRAME_NO_TASKBAR | wxSTAY_ON_TOP);
                }
            }
        }
    Yield();
    }

//-------------------------------------------
void ReadabilityApp::AddWordsToDictionaries(const wxArrayString& theWords,
                                            const readability::test_language lang) const
    {
    wxString customDictionaryPath = m_CustomEnglishDictionaryPath;
    if (lang == readability::test_language::english_test)
        {
        customDictionaryPath = m_CustomEnglishDictionaryPath;
        // add words to the currently loaded (custom) dictionary
        for (size_t i = 0; i < theWords.GetCount(); ++i)
            {
            BaseProject::known_custom_english_spellings.add_word(theWords[i].wc_str());
            }
        }
    else if (lang == readability::test_language::spanish_test)
        {
        customDictionaryPath = m_CustomSpanishDictionaryPath;
        // add words to the currently loaded (custom) dictionary
        for (size_t i = 0; i < theWords.GetCount(); ++i)
            {
            BaseProject::known_custom_spanish_spellings.add_word(theWords[i].wc_str());
            }
        }
    else if (lang == readability::test_language::german_test)
        {
        customDictionaryPath = m_CustomGermanDictionaryPath;
        // add words to the currently loaded (custom) dictionary
        for (size_t i = 0; i < theWords.GetCount(); ++i)
            {
            BaseProject::known_custom_german_spellings.add_word(theWords[i].wc_str());
            }
        }

    // add to custom dictionary
    wxString text;
    if (!Wisteria::TextStream::ReadFile(customDictionaryPath, text))
        {
        wxMessageBox(_(L"Error loading custom dictionary file."), _(L"Error"),
                     wxOK | wxICON_EXCLAMATION);
        return;
        }

    word_list customWords;
    customWords.load_words(text, true, false);
    for (size_t i = 0; i < theWords.GetCount(); ++i)
        {
        customWords.add_word(theWords[i].wc_str());
        }
    // save the new list back to the original file
    wxString outputStr;
    outputStr.reserve(customWords.get_list_size() * 5);
    for (size_t i = 0; i < customWords.get_list_size(); ++i)
        {
        outputStr += customWords.get_words().at(i).c_str() + wxString(L"\n");
        }
    outputStr.Trim();
    wxFileName(customDictionaryPath).SetPermissions(wxS_DEFAULT);
    wxFile outputFile(customDictionaryPath, wxFile::write);
    outputFile.Write(outputStr, wxConvUTF8);
    }

//-------------------------------------------
void ReadabilityApp::OnEventLoopEnter(wxEventLoopBase* loop)
    {
    if (loop && loop->IsOk() && loop->IsMain())
        {
        // this prevents logic blocks in here from overlapping
        // (showing dialogs in here can cause this function to be reentered)
        static bool initEventProcessing = false;
        static bool hasCommandLineBeenParsed = false;

        // The command line may need an active loop (if a progress bar is used),
        // so we handle it here instead of OnInit().
        if (!hasCommandLineBeenParsed && !initEventProcessing)
            {
            initEventProcessing = true;
            hasCommandLineBeenParsed = true;
            // parse the command line
            const wxCmdLineEntryDesc cmdLineDesc[] = {
                { wxCMD_LINE_SWITCH, _DT("help"), _DT("help"), wxTRANSLATE("Displays the help"),
                  wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
                { wxCMD_LINE_OPTION, _DT("bg"), _DT("background"),
                  wxTRANSLATE("Sets the graph background image for the input project"),
                  wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
                { wxCMD_LINE_OPTION, _DT("lua"), _DT("lua"), wxTRANSLATE("Runs a Lua script"),
                  wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
                { wxCMD_LINE_OPTION, _DT("loglevel"), _DT("loglevel"),
                  wxTRANSLATE("Log report level (0 = none, 1 = standard, 2 = verbose, 3 = max)."),
                  wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL },
                { wxCMD_LINE_PARAM, nullptr, nullptr, wxTRANSLATE("Input file"),
                  wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
                { wxCMD_LINE_NONE, nullptr, nullptr, nullptr, wxCMD_LINE_VAL_NONE, 0 }
            };
            wxCmdLineParser cmdParser(cmdLineDesc, argc, argv);
            int CommandLineResult = 0;
                {
                wxLogNull logger;
                CommandLineResult = cmdParser.Parse(false);
                }

            // set the logging level
            wxLog::EnableLogging(true);
            // change the log level (if command line is asking for it)
            long loglevel = 1;
            if (cmdParser.Found(_DT(L"loglevel"), &loglevel))
                {
                if (loglevel <= 0)
                    {
                    wxLog::EnableLogging(false);
                    }
                else if (loglevel == 1)
                    {
                    wxLog::SetLogLevel(wxLOG_Status);
                    }
                else if (loglevel > 1)
                    {
                    wxLog::SetVerbose(true);
                    wxLog::SetLogLevel(wxLOG_Max);
                    }
                }
            // run a Lua script
            wxString luaScriptPath;
            if (GetLicenseAdmin().IsFeatureEnabled(FeatureProfessionalCode()) &&
                cmdParser.Found(_DT(L"lua"), &luaScriptPath))
                {
                wxString luaScript, errorMessage;
                if (Wisteria::TextStream::ReadFile(luaScriptPath, luaScript))
                    {
                    GetLuaRunner().RunLuaCode(luaScript, luaScriptPath, errorMessage);
                    }
                }

            // see if they want to display the help
            if (CommandLineResult == -1)
                {
                GetMainFrame()->DisplayHelp();
                }
            else if (CommandLineResult == 0 && cmdParser.GetParamCount() > 0)
                {
                wxFileName fn(cmdParser.GetParam(0));
                fn.Normalize(wxPATH_NORM_LONG | wxPATH_NORM_DOTS | wxPATH_NORM_TILDE |
                             wxPATH_NORM_ABSOLUTE);
                // allow ability to open native files or import something from command line
                size_t i = 0;
                for (i = 0; i < GetMainFrame()->GetDefaultFileExtentions().GetCount(); ++i)
                    {
                    if (fn.GetExt().CmpNoCase(GetMainFrame()->GetDefaultFileExtentions()[i]) == 0)
                        {
                        GetMainFrame()->OpenFile(fn.GetFullPath());
                        break;
                        }
                    }
                // not a recognized project extension, so open the file as a new standard project
                // (note that we will bypass the wizard)
                if (i == GetMainFrame()->GetDefaultFileExtentions().GetCount())
                    {
                    const auto& templateList = GetDocManager()->GetTemplates();
                    for (size_t j = 0; j < templateList.GetCount(); ++j)
                        {
                        wxDocTemplate* docTemplate =
                            dynamic_cast<wxDocTemplate*>(templateList.Item(j)->GetData());
                        if (docTemplate &&
                            docTemplate->GetDocClassInfo()->IsKindOf(CLASSINFO(ProjectDoc)))
                            {
                            ProjectDoc* newDoc = dynamic_cast<ProjectDoc*>(
                                docTemplate->CreateDocument(fn.GetFullPath(), wxDOC_NEW));
                            if (newDoc && !newDoc->OnNewDocument())
                                {
                                // Document is implicitly deleted by DeleteAllViews
                                newDoc->DeleteAllViews();
                                newDoc = nullptr;
                                }
                            if (newDoc && newDoc->GetFirstView())
                                {
                                newDoc->GetFirstView()->Activate(true);
                                GetDocManager()->ActivateView(newDoc->GetFirstView());
                                if (newDoc->GetDocumentWindow())
                                    {
                                    newDoc->GetDocumentWindow()->SetFocus();
                                    }
                                wxString graphBackgroundPath;
                                if (cmdParser.Found(_DT(L"bg"), &graphBackgroundPath))
                                    {
                                    newDoc->SetPlotBackGroundImagePath(graphBackgroundPath);
                                    newDoc->RefreshRequired(ProjectRefresh::Minimal);
                                    newDoc->RefreshProject();
                                    }
                                }
                            break;
                            }
                        }
                    }
                }
            initEventProcessing = false;
            }
        }
    }

//-------------------------------------------
void ReadabilityApp::InitializeReadabilityFeatures()
    {
    const wxString SERIAL_NUMBER_AND_FEATURE_DEFAULT_PRIVATE_KEY = _DT(L"PyGf5k8d");

    // licensing codes
    const ByteInfo FEATURE_ENGLISH_READABILITY_TESTS_BYTEINFO(0, 0, true);
    const wxString FEATURE_ENGLISH_READABILITY_PRIVATE_KEY = _DT(L"lkI6f42Z");

    const ByteInfo FEATURE_LANGUAGE_PACK_READABILITY_TESTS_BYTEINFO(0, 1, false);
    const wxString FEATURE_LANGUAGE_PACK_READABILITY_PRIVATE_KEY = _DT(L"Ji92Vbpm");

    const wxString FEATURE_HIGHLIGHTED_TEXT_VIEWS = _DT(L"RDHT");
    const ByteInfo FEATURE_HIGHLIGHTED_TEXT_VIEWS_BYTEINFO(1, 0, true);
    const wxString FEATURE_HIGHLIGHTED_TEXT_VIEWS_PRIVATE_KEY = _DT(L"SteP7hUS");

    const wxString FEATURE_SUMMARY_STATISTICS = _DT(L"RDSS");
    const ByteInfo FEATURE_SUMMARY_STATISTICS_BYTEINFO(1, 1, true);
    const wxString FEATURE_SUMMARY_STATISTICS_PRIVATE_KEY = _DT(L"OuI69Jul");

    const wxString FEATURE_SUMMARY_GRAPHS = _DT(L"RDSG");
    const ByteInfo FEATURE_SUMMARY_GRAPHS_BYTEINFO(1, 2, true);
    const wxString FEATURE_SUMMARY_GRAPHS_PRIVATE_KEY = _DT(L"75FyNlx3");

    const wxString FEATURE_DIFFICULT_WORDS = _DT(L"RDDW");
    const ByteInfo FEATURE_DIFFICULT_WORDS_BYTEINFO(1, 3, true);
    const wxString FEATURE_DIFFICULT_WORDS_PRIVATE_KEY = _DT(L"MKjqS8y4");

    const ByteInfo FEATURE_PROFESSIONAL_BYTEINFO(2, 0, false);
    const wxString FEATURE_PROFESSIONAL_PRIVATE_KEY = _DT(L"JH93popC");

    GetLicenseAdmin().SetPrivateKey(SERIAL_NUMBER_AND_FEATURE_DEFAULT_PRIVATE_KEY);

    GetLicenseAdmin().SetFeaturePrivateKey(FeatureEnglishReadabilityTestsCode(),
                                           FEATURE_ENGLISH_READABILITY_PRIVATE_KEY);
    GetLicenseAdmin().SetFeatureDescription(FeatureEnglishReadabilityTestsCode(),
                                            _(L"English readability tests"));

    GetLicenseAdmin().SetFeaturePrivateKey(FeatureLanguagePackCode(),
                                           FEATURE_LANGUAGE_PACK_READABILITY_PRIVATE_KEY);
    GetLicenseAdmin().SetFeatureDescription(FeatureLanguagePackCode(),
                                            _(L"Language Pack (non-English readability tests)"));

    GetLicenseAdmin().SetFeaturePrivateKey(FEATURE_HIGHLIGHTED_TEXT_VIEWS,
                                           FEATURE_HIGHLIGHTED_TEXT_VIEWS_PRIVATE_KEY);
    GetLicenseAdmin().SetFeatureDescription(FEATURE_HIGHLIGHTED_TEXT_VIEWS,
                                            _(L"Highlighted document display"));

    GetLicenseAdmin().SetFeaturePrivateKey(FEATURE_SUMMARY_STATISTICS,
                                           FEATURE_SUMMARY_STATISTICS_PRIVATE_KEY);
    GetLicenseAdmin().SetFeatureDescription(FEATURE_SUMMARY_STATISTICS,
                                            _(L"Summary statistics"));

    GetLicenseAdmin().SetFeaturePrivateKey(FEATURE_SUMMARY_GRAPHS, FEATURE_SUMMARY_GRAPHS_PRIVATE_KEY);
    GetLicenseAdmin().SetFeatureDescription(FEATURE_SUMMARY_GRAPHS, _(L"Summary graph"));

    GetLicenseAdmin().SetFeaturePrivateKey(FEATURE_DIFFICULT_WORDS, FEATURE_DIFFICULT_WORDS_PRIVATE_KEY);
    GetLicenseAdmin().SetFeatureDescription(FEATURE_DIFFICULT_WORDS, _(L"Words breakdown"));

    GetLicenseAdmin().SetFeaturePrivateKey(FeatureProfessionalCode(), FEATURE_PROFESSIONAL_PRIVATE_KEY);
    GetLicenseAdmin().SetFeatureDescription(FeatureProfessionalCode(), _(L"Professional"));

#if defined SERIAL_NUMBER_GENERATOR_ENABLED || defined LICENSE_FILE_GENERATOR_ENABLED
    GetLicenseAdmin().SetFeatureByteInfo(FeatureEnglishReadabilityTestsCode(),
        FEATURE_ENGLISH_READABILITY_TESTS_BYTEINFO);
    GetLicenseAdmin().SetFeatureByteInfo(FeatureLanguagePackCode(),
        FEATURE_LANGUAGE_PACK_READABILITY_TESTS_BYTEINFO);

    GetLicenseAdmin().SetFeatureByteInfo(FEATURE_HIGHLIGHTED_TEXT_VIEWS,
                                         FEATURE_HIGHLIGHTED_TEXT_VIEWS_BYTEINFO);
    GetLicenseAdmin().SetFeatureByteInfo(FEATURE_SUMMARY_STATISTICS,
                                         FEATURE_SUMMARY_STATISTICS_BYTEINFO);
    GetLicenseAdmin().SetFeatureByteInfo(FEATURE_SUMMARY_GRAPHS,
                                         FEATURE_SUMMARY_GRAPHS_BYTEINFO);
    GetLicenseAdmin().SetFeatureByteInfo(FEATURE_DIFFICULT_WORDS,
                                         FEATURE_DIFFICULT_WORDS_BYTEINFO);
    GetLicenseAdmin().SetFeatureByteInfo(FeatureProfessionalCode(),
                                         FEATURE_PROFESSIONAL_BYTEINFO);
#endif

    GetLicenseAdmin().AddProduct(ProductType(L"101", _(L"Single-user License"),
        ProductType::License::SingleUser, ProductType::Type::Standard));
    GetLicenseAdmin().AddProduct(ProductType(L"102", _(L"Single-user License"),
        ProductType::License::SingleUser, ProductType::Type::Standard));
    GetLicenseAdmin().AddProduct(ProductType(L"103", _(L"Site License"),
        ProductType::License::SiteLicense, ProductType::Type::Standard));

    GetLicenseAdmin().AddProduct(ProductType(_DT(L"104"), _(L"Professional Single-user License"),
        ProductType::License::SingleUser, ProductType::Type::Professional));
    GetLicenseAdmin().AddProduct(ProductType(_DT(L"105"), _(L"Professional Single-user License"),
        ProductType::License::SingleUser, ProductType::Type::Professional));
    GetLicenseAdmin().AddProduct(ProductType(_DT(L"106"), _(L"Professional Site License"),
        ProductType::License::SiteLicense, ProductType::Type::Professional));

    GetLicenseAdmin().AddProduct(ProductType(_DT(L"107"), _(L"Professional Single-user License"),
        ProductType::License::SingleUser, ProductType::Type::ProfessionalUpgrade));
    GetLicenseAdmin().AddProduct(ProductType(_DT(L"108"), _(L"Professional Site License"),
        ProductType::License::SiteLicense, ProductType::Type::ProfessionalUpgrade));

    GetLicenseAdmin().AddProduct(ProductType(_DT(L"109"), _(L"Single-user (with Language Pack) License"),
        ProductType::License::SingleUser, ProductType::Type::StandardWithLanguagePack));
    GetLicenseAdmin().AddProduct(ProductType(_DT(L"110"), _(L"Single-user (with Language Pack) License"),
        ProductType::License::SingleUser, ProductType::Type::StandardWithLanguagePack));
    GetLicenseAdmin().AddProduct(ProductType(_DT(L"111"), _(L"Site License (with Language Pack)"),
        ProductType::License::SiteLicense, ProductType::Type::StandardWithLanguagePack));
    }

//-------------------------------------------
bool ReadabilityApp::OnInit()
    {
    SetAppName(_DT(L"Readability Studio"));
    SetAppSubName(GetAppVersion());
    SetVendorName(_DT(L"Oleander Software"));

    // set licensing feature information
    InitializeReadabilityFeatures();

    wxString AppSettingFolderPath;
    // if app-specific data folder can't be determined
    // (really just relic behavior from Win9.x) then use documents dir
    if (wxStandardPaths::Get().GetUserDataDir().empty())
        {
        AppSettingFolderPath =
            wxStandardPaths::Get().GetAppDocumentsDir() + wxFileName::GetPathSeparator();
        }
    // write to app folder in User's data folder (this should be the norm)
    else
        {
        AppSettingFolderPath =
            wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator();
        }

    if (!wxFileName::DirExists(AppSettingFolderPath))
        {
        if (!wxFileName::Mkdir(AppSettingFolderPath, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL))
            {
            /* desperation move that should never happen--
               just save the settings to the root folder*/
            #ifdef __WXMSW__
                AppSettingFolderPath = L"C:\\";
            #elif defined(__WXOSX__)
                AppSettingFolderPath = L"/Users/Shared/";
            #else
                AppSettingFolderPath = L"/usr/share/";
            #endif
            }
        }

    if (!BaseApp::OnInit())
        { return false; }

#ifndef NDEBUG
    wxFileTranslationsLoader::AddCatalogLookupPathPrefix(L".");
#endif
    wxTranslations* const translations{ new wxTranslations{} };
    wxTranslations::Set(translations);
    if (!translations->AddCatalog(_READSTUDIO_BINARY_NAME))
        {
        wxLogDebug(L"Could not find application's translation catalog.");
        }
    if (!translations->AddStdCatalog())
        {
        wxLogDebug(L"Could not find standard translation catalog.");
        }

    GetResourceManager().LoadArchive(FindResourceFile(L"res.wad"));

    const auto splashScreens = GetResourceManager().GetFilesInFolder(_DT(L"splash"));
    for (const auto& splash : splashScreens)
        { AddSplashscreenImagePath(splash); }

    if (!LoadWordLists(AppSettingFolderPath) )
        { return false; }

    // init random number generators
    m_mersenneTwister = std::mt19937_64(std::random_device{}());

    SetSupportEmail(L"support@oleandersoftware.com");

    m_webHarvester.SetEventHandler(this);

    // holding down SHIFT while the program starts will turn on verbose logging
    if (wxGetMouseState().ShiftDown())
        {
        wxLog::SetVerbose(true);
        wxLog::SetLogLevel(wxLOG_Max);
        m_advancedImport = true;
        }

    GetAppOptions().LoadOptionsFile(AppSettingFolderPath + L"Settings.xml", true);
    wxLogMessage(L"Settings file loaded from: " + AppSettingFolderPath + L"Settings.xml");

    // load map of graph icons to human readable strings
    m_shapeMap = { std::make_pair(_(L"Sun"), DONTTRANSLATE(L"sun")),
                   std::make_pair(_(L"Book"), DONTTRANSLATE(L"book")),
                   std::make_pair(_(L"Fall leaf"), DONTTRANSLATE(L"fall-leaf")),
                   std::make_pair(_(L"Man"), DONTTRANSLATE(L"man")),
                   std::make_pair(_(L"Woman"), DONTTRANSLATE(L"woman")),
                   std::make_pair(_(L"Business woman"), DONTTRANSLATE(L"business-woman")),
                   std::make_pair(_(L"Tire"), DONTTRANSLATE(L"tire")),
                   std::make_pair(_(L"Flower"), DONTTRANSLATE(L"flower")),
                   std::make_pair(_(L"Warning road sign"), DONTTRANSLATE(L"warning-road-sign")),
                   std::make_pair(_(L"Location marker"), DONTTRANSLATE(L"location-marker")),
                   std::make_pair(_(L"Graduation cap"), DONTTRANSLATE(L"graduation-cap")),
                   std::make_pair(_(L"Car"), DONTTRANSLATE(L"car")),
                   std::make_pair(_(L"Newspaper"), DONTTRANSLATE(L"newspaper")),
                   std::make_pair(_(L"Snowflake"), DONTTRANSLATE(L"snowflake")),
                   std::make_pair(_(L"Blackboard"), DONTTRANSLATE(L"blackboard")),
                   std::make_pair(_(L"Clock"), DONTTRANSLATE(L"clock")),
                   std::make_pair(_(L"Ruler"), DONTTRANSLATE(L"ruler")) };

    // do the same for the color schemes (for the graphs)
    m_colorSchemeMap = { std::make_pair(_(L"Dusk"), DONTTRANSLATE(L"dusk")),
                         std::make_pair(_(L"Earth Tones"), DONTTRANSLATE(L"earthtones")),
                         std::make_pair(_DT(L"1920s"), DONTTRANSLATE(L"decade1920s")),
                         std::make_pair(_DT(L"1940s"), DONTTRANSLATE(L"decade1940s")),
                         std::make_pair(_DT(L"1950s"), DONTTRANSLATE(L"decade1950s")),
                         std::make_pair(_DT(L"1960s"), DONTTRANSLATE(L"decade1960s")),
                         std::make_pair(_DT(L"1970s"), DONTTRANSLATE(L"decade1970s")),
                         std::make_pair(_DT(L"1980s"), DONTTRANSLATE(L"decade1980s")),
                         std::make_pair(_DT(L"1990s"), DONTTRANSLATE(L"decade1990s")),
                         std::make_pair(_DT(L"2000s"), DONTTRANSLATE(L"decade2000s")),
                         std::make_pair(_(L"October"), DONTTRANSLATE(L"october")),
                         std::make_pair(_(L"Slytherin"), DONTTRANSLATE(L"slytherin")),
                         std::make_pair(_(L"Campfire"), DONTTRANSLATE(L"campfire")),
                         std::make_pair(_(L"Coffee Shop"), DONTTRANSLATE(L"coffeeshop")),
                         std::make_pair(_(L"Arctic Chill"), DONTTRANSLATE(L"arcticchill")),
                         std::make_pair(_(L"Back to School"), DONTTRANSLATE(L"backtoschool")),
                         std::make_pair(_(L"Box of Chocolates"), DONTTRANSLATE(L"boxofchocolates")),
                         std::make_pair(_(L"Cosmopolitan"), DONTTRANSLATE(L"cosmopolitan")),
                         std::make_pair(_(L"Day and Night"), DONTTRANSLATE(L"dayandnight")),
                         std::make_pair(_(L"Fresh Flowers"), DONTTRANSLATE(L"freshflowers")),
                         std::make_pair(_(L"Icecream"), DONTTRANSLATE(L"icecream")),
                         std::make_pair(_(L"Urban Oasis"), DONTTRANSLATE(L"urbanoasis")),
                         std::make_pair(_(L"Typewriter"), DONTTRANSLATE(L"typewriter")),
                         std::make_pair(_(L"Tasty Waves"), DONTTRANSLATE(L"tastywaves")),
                         std::make_pair(_(L"Spring"), DONTTRANSLATE(L"spring")),
                         std::make_pair(_(L"Shabby Chic"), DONTTRANSLATE(L"shabbychic")),
                         std::make_pair(_(L"Rolling Thunder"), DONTTRANSLATE(L"rollingthunder")),
                         std::make_pair(_(L"Produce Section"), DONTTRANSLATE(L"producesection")),
                         std::make_pair(_(L"Nautical"), DONTTRANSLATE(L"nautical")),
                         std::make_pair(_(L"Semesters"), DONTTRANSLATE(L"semesters")),
                         std::make_pair(_(L"Seasons"), DONTTRANSLATE(L"seasons")),
                         std::make_pair(_(L"Meadow Sunset"), DONTTRANSLATE(L"meadowsunset")) };

    // this needs to be called before prompting for the
    // serial number because wxGetTextFromUser will need a parent
    LoadInterface();

    wxString licensePath = FindResourceFile(L"rs.xml");
    // if couldn't be loaded from the shared users' folder, then go to the user's settings folder.
    if (!GetLicenseAdmin().LoadLicenseFile(licensePath, GetAppName()) &&
        wxFileName::FileExists(AppSettingFolderPath + L"rs.xml") )
        { licensePath = AppSettingFolderPath + L"rs.xml"; }
    if (!GetLicenseAdmin().LoadLicenseFile(licensePath, GetAppName()))
        {
        // loop until they hit cancel or enter a valid serial number
        while (true)
            {
            wxString serialNumber = wxGetTextFromUser(_(L"Serial number:"), _(L"Please Enter Your Serial Number"));
            if (serialNumber.empty())
                { return false; }
            if (!GetLicenseAdmin().LoadSerialNumber(serialNumber))
                { continue; }
            else
                { break; }
            }

    // set the user name and company info
    #ifdef __WXMSW__
        {
        // get this info from our Windows installer
        wxLogNull logNo;
        wxRegKey key(wxRegKey::HKLM,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{FBFBEAB6-622A-4E16-893D-216C25FF6A69}_is1");
        key.Open(wxRegKey::Read);
        wxString value;
        if (key.QueryValue(_DT(L"Inno Setup: User Info: Organization"), value))
            { GetLicenseAdmin().SetUserOrganization(value); }
        if (key.QueryValue(_DT(L"Inno Setup: User Info: Name"), value))
            { GetLicenseAdmin().SetUserName(value); }
        }
    #endif
        if (GetLicenseAdmin().GetUserName().empty())
            { GetLicenseAdmin().SetUserName(wxGetUserName()); }

        // try to save it to the shared users' application data folder
    #ifdef __WXMSW__
        if (wxFileName::Mkdir(wxStandardPaths::Get().GetConfigDir(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL))
            {
            licensePath = wxStandardPaths::Get().GetConfigDir() + wxFileName::GetPathSeparator() + L"rs.xml";
            }
        else
            { licensePath = AppSettingFolderPath + L"rs.xml"; }
    #elif defined (__WXOSX__)
        /* starting around 10.15, you can no longer write to
           "/Library/Application Support/AppName", you can only write to the
           user's Library folder. A shared license file won't be possible on macOS.*/
        licensePath = AppSettingFolderPath + L"rs.xml";
    #else
        licensePath = AppSettingFolderPath + L"rs.xml";
    #endif
        // save the licensing info now that we have it and have validated it
        if (!GetLicenseAdmin().SaveLicenseFile(licensePath))
            {
            // if couldn't be saved in the shared users' folder, then save to the user's settings folder.
            licensePath = AppSettingFolderPath + L"rs.xml";
            if (!GetLicenseAdmin().SaveLicenseFile(licensePath))
                {
                wxMessageBox(wxString::Format(
                    _(L"Unable to save license file:\n\n\"%s\"\n\nFile may be write-protected or "
                       "you may not have privileges to write it."), licensePath),
                    _(L"Error"), wxOK|wxICON_ERROR, GetMainFrame());
                return false;
                }
            }
        // Let's try it again from the top
        if (!GetLicenseAdmin().LoadLicenseFile(licensePath, GetAppName()))
            {
            wxMessageBox(wxString::Format(_(L"Unable to load license file:\n\n\"%s\""), licensePath),
                _(L"Error"), wxOK|wxICON_ERROR, GetMainFrame());
            return false;
            }
        }

    wxLogMessage(L"License file loaded from: " + GetLicenseAdmin().GetLicenseFilePath());

    ShowSplashscreen();

    // now load any menus which are affected by licensing
    LoadInterfaceLicensableFeatures();

    m_dynamicIdMap = {
        // These are the IDs that this constants map to in "Resources/Scripting/RSConstants.lua";
        // When adding a new constant (e.g., enum value) to RSConstants.lua, that numeric ID from
        // there to the respective window ID here.
        { 30001, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR },
        { 30002, MainFrame::ID_PROOFING_RIBBON_BUTTON_BAR },
        { 30003, MainFrame::ID_PARAGRAPH_DEDUCTION_RIBBON_BUTTON_BAR },
        { 30004, MainFrame::ID_TEXT_EXCLUSION_RIBBON_BUTTON_BAR },
        { 30005, MainFrame::ID_NUMERALS_RIBBON_BUTTON_BAR },
        { 30006, MainFrame::ID_VIEW_RIBBON_BUTTON_BAR },
        { 30007, MainFrame::ID_CLIPBOARD_RIBBON_BUTTON_BAR },
        { 30008, MainFrame::ID_DOCUMENT_RIBBON_BUTTON_BAR },
        { 30009, MainFrame::ID_PROJECT_RIBBON_BUTTON_BAR },
        { 1000, ToolsOptionsDlg::GENERAL_SETTINGS_PAGE },
        { 2000, ToolsOptionsDlg::PROJECT_SETTINGS_PAGE },
        { 3000, ToolsOptionsDlg::DOCUMENT_DISPLAY_GENERAL_PAGE },
        { 3002, ToolsOptionsDlg::DOCUMENT_DISPLAY_DOLCH_PAGE },
        { 4000, ToolsOptionsDlg::SCORES_TEST_OPTIONS_PAGE },
        { 4001, ToolsOptionsDlg::SCORES_DISPLAY_PAGE },
        { 5000, ToolsOptionsDlg::ANALYSIS_INDEXING_PAGE },
        { 5001, ToolsOptionsDlg::GRAMMAR_PAGE },
        { 5002, ToolsOptionsDlg::ANALYSIS_STATISTICS_PAGE },
        { 6000, ToolsOptionsDlg::GRAPH_GENERAL_PAGE },
        { 6001, ToolsOptionsDlg::GRAPH_AXIS_PAGE },
        { 6002, ToolsOptionsDlg::GRAPH_TITLES_PAGE },
        { 6003, ToolsOptionsDlg::GRAPH_READABILITY_GRAPHS_PAGE },
        { 6004, ToolsOptionsDlg::GRAPH_BAR_CHART_PAGE },
        { 6005, ToolsOptionsDlg::GRAPH_HISTOGRAM_PAGE },
        { 6006, ToolsOptionsDlg::GRAPH_BOX_PLOT_PAGE },
        { 1036, ToolsOptionsDlg::ID_TEXT_EXCLUDE_METHOD },
        { 1040, ToolsOptionsDlg::ID_SENTENCE_LONGER_THAN_BUTTON },
        { 1041, ToolsOptionsDlg::ID_SENTENCE_OUTLIER_LENGTH_BUTTON },
        { 1042, ToolsOptionsDlg::ID_IGNORE_BLANK_LINES_BUTTON },
        { 1043, ToolsOptionsDlg::ID_IGNORE_INDENTING_BUTTON },
        { 1051, ToolsOptionsDlg::ID_AGGRESSIVE_LIST_DEDUCTION_CHECKBOX },
        { 1052, ToolsOptionsDlg::ID_EXCLUDED_PHRASES_FILE_EDIT_BUTTON },
        { 1054, ToolsOptionsDlg::ID_EXCLUDE_COPYRIGHT_CHECKBOX },
        { 1056, ToolsOptionsDlg::ID_EXCLUDE_FILE_ADDRESS_CHECKBOX },
        { 1058, ToolsOptionsDlg::ID_EXCLUDE_PROPER_NOUNS_CHECKBOX },
        { 1076, ToolsOptionsDlg::ID_INCOMPLETE_SENTENCE_VALID_LABEL_START },
        { 1077, ToolsOptionsDlg::ID_INCOMPLETE_SENTENCE_VALID_LABEL_END },
        { 1078, ToolsOptionsDlg::ID_EXCLUSION_TAG_BLOCK_LABEL },
        { 1079, ToolsOptionsDlg::ID_EXCLUSION_TAG_BLOCK_SELCTION },
        { 1080, ToolsOptionsDlg::ID_ADDITIONAL_FILE_BROWSE_BUTTON },
        { 1083, ToolsOptionsDlg::ID_ADDITIONAL_FILE_FIELD },
        { 1084, ToolsOptionsDlg::ID_INCOMPLETE_SENTENCE_VALID_VALUE_BOX },
        { 1085, ToolsOptionsDlg::ID_DOCUMENT_STORAGE_RADIO_BOX },
        { 1086, ToolsOptionsDlg::ID_SENTENCES_MUST_BE_CAP_BUTTON },
        { 1087, ToolsOptionsDlg::ID_FILE_BROWSE_BUTTON },
        { 1088, ToolsOptionsDlg::ID_DOCUMENT_DESCRIPTION_FIELD },
        { 20002, TestBundleDlg::ID_STANDARD_TEST_PAGE },
        { 7000, CustomTestDlg::ID_TEST_NAME_FIELD },
        { 7001, CustomTestDlg::ID_TEST_TYPE_COMBO },
        { 7002, CustomTestDlg::ID_FORMULA_FIELD },
        { 7003, CustomTestDlg::ID_WORD_LIST_PROPERTY_GRID },
        { 7004, CustomTestDlg::ID_PROPER_NOUN_PROPERTY_GRID },
        { 7005, CustomTestDlg::ID_CLASSIFICATION_PROPERTY_GRID },
        { 8001, BaseProjectView::SENTENCES_CONJUNCTION_START_LIST_PAGE_ID },
        { 8002, BaseProjectView::SENTENCES_LOWERCASE_START_LIST_PAGE_ID },
        { 8003, BaseProjectView::WORDY_PHRASES_LIST_PAGE_ID },
        { 8004, BaseProjectView::REDUNDANT_PHRASE_LIST_PAGE_ID },
        { 8005, BaseProjectView::CLICHES_LIST_PAGE_ID },
        { 8006, BaseProjectView::DUPLICATES_LIST_PAGE_ID },
        { 8007, BaseProjectView::INCORRECT_ARTICLE_PAGE_ID },
        { 8008, BaseProjectView::PASSIVE_VOICE_PAGE_ID },
        { 8009, BaseProjectView::LONG_SENTENCES_LIST_PAGE_ID },
        { 8010, BaseProjectView::HARD_WORDS_LIST_PAGE_ID },
        { 8011, BaseProjectView::LONG_WORDS_LIST_PAGE_ID },
        { 8012, BaseProjectView::DC_WORDS_LIST_PAGE_ID },
        { 8013, BaseProjectView::SPACHE_WORDS_LIST_PAGE_ID },
        { 8014, BaseProjectView::ALL_WORDS_LIST_PAGE_ID },
        { 8015, BaseProjectView::ALL_WORDS_CONDENSED_LIST_PAGE_ID },
        { 8016, BaseProjectView::PROPER_NOUNS_LIST_PAGE_ID },
        { 8017, BaseProjectView::CONTRACTIONS_LIST_PAGE_ID },
        { 8018, BaseProjectView::OVERUSED_WORDS_BY_SENTENCE_LIST_PAGE_ID },
        { 8019, BaseProjectView::HARRIS_JACOBSON_WORDS_LIST_PAGE_ID },
        { 8020, BaseProjectView::MISSPELLED_WORD_LIST_PAGE_ID },
        { 8021, BaseProjectView::WORDING_ERRORS_LIST_PAGE_ID },
        { 8022, BaseProjectView::WORD_BREAKDOWN_PAGE_ID },
        { 8023, BaseProjectView::FRY_PAGE_ID },
        { 8024, BaseProjectView::RAYGOR_PAGE_ID },
        { 8025, BaseProjectView::FRASE_PAGE_ID },
        { 8026, BaseProjectView::SCHWARTZ_PAGE_ID },
        { 8027, BaseProjectView::LIX_GAUGE_PAGE_ID },
        { 8028, BaseProjectView::LIX_GAUGE_GERMAN_PAGE_ID },
        { 8029, BaseProjectView::DB2_PAGE_ID },
        { 8030, BaseProjectView::FLESCH_CHART_PAGE_ID },
        { 8031, BaseProjectView::CRAWFORD_GRAPH_PAGE_ID },
        { 8032, BaseProjectView::DOLCH_COVERAGE_CHART_PAGE_ID },
        { 8033, BaseProjectView::DOLCH_BREAKDOWN_PAGE_ID },
        { 8034, BaseProjectView::GPM_FRY_PAGE_ID },
        { 8035, BaseProjectView::SENTENCE_BOX_PLOT_PAGE_ID },
        { 8036, BaseProjectView::SENTENCE_HISTOGRAM_PAGE_ID },
        { 8037, BaseProjectView::SYLLABLE_HISTOGRAM_PAGE_ID },
        { 8038, BaseProjectView::SENTENCE_HEATMAP_PAGE_ID },
        { 8039, BaseProjectView::WORD_CLOUD_PAGE_ID },
        { 8040, BaseProjectView::ID_SCORE_LIST_PAGE_ID },
        { 8041, BaseProjectView::ID_DIFFICULT_WORDS_LIST_PAGE_ID },
        { 8042, BaseProjectView::ID_DOLCH_COVERAGE_LIST_PAGE_ID },
        { 8043, BaseProjectView::ID_DOLCH_WORDS_LIST_PAGE_ID },
        { 8044, BaseProjectView::ID_NON_DOLCH_WORDS_LIST_PAGE_ID },
        { 8045, BaseProjectView::ID_WARNING_LIST_PAGE_ID },
        { 8046, BaseProjectView::ID_SCORE_STATS_LIST_PAGE_ID },
        { 8047, BaseProjectView::ID_AGGREGATED_DOC_SCORES_LIST_PAGE_ID },
        { 8048, BaseProjectView::ID_AGGREGATED_CLOZE_SCORES_LIST_PAGE_ID },
        { 8049, BaseProjectView::DOLCH_WORDS_LIST_PAGE_ID },
        { 8050, BaseProjectView::NON_DOLCH_WORDS_LIST_PAGE_ID },
        { 8051, BaseProjectView::UNUSED_DOLCH_WORDS_LIST_PAGE_ID },
        { 8052, BaseProjectView::DOLCH_WORDS_TEXT_PAGE_ID },
        { 8053, BaseProjectView::NON_DOLCH_WORDS_TEXT_PAGE_ID },
        { 8054, BaseProjectView::DOLCH_STATS_PAGE_ID },
        { 8055, BaseProjectView::STATS_REPORT_PAGE_ID },
        { 8056, BaseProjectView::STATS_LIST_PAGE_ID },
        { 8057, BaseProjectView::READABILITY_SCORES_PAGE_ID },
        { 8058, BaseProjectView::READABILITY_SCORES_SUMMARY_REPORT_PAGE_ID },
        { 8059, BaseProjectView::READABILITY_GOALS_PAGE_ID },
        { 8060, BaseProjectView::READABILITY_GOALS_PAGE_ID },
        { 8061, BaseProjectView::LONG_WORDS_TEXT_PAGE_ID },
        { 8062, BaseProjectView::DC_WORDS_TEXT_PAGE_ID },
        { 8063, BaseProjectView::SPACHE_WORDS_TEXT_PAGE_ID },
        { 8064, BaseProjectView::HARRIS_JACOBSON_WORDS_TEXT_PAGE_ID },
        { 8065, BaseProjectView::HARD_WORDS_TEXT_PAGE_ID },
        { 8066, BaseProjectView::LONG_SENTENCES_AND_WORDINESS_TEXT_PAGE_ID },
        { 8067, BaseProjectView::SIDEBAR_WORDS_BREAKDOWN_SECTION_ID },
        { 8068, BaseProjectView::SIDEBAR_READABILITY_SCORES_SECTION_ID },
        { 8069, BaseProjectView::SIDEBAR_GRAMMAR_SECTION_ID },
        { 8070, BaseProjectView::SIDEBAR_DOLCH_SECTION_ID },
        { 8071, BaseProjectView::SIDEBAR_BOXPLOTS_SECTION_ID },
        { 8072, BaseProjectView::SIDEBAR_HISTOGRAMS_SECTION_ID },
        { 8073, BaseProjectView::SIDEBAR_STATS_SUMMARY_SECTION_ID },
        { 8074, BaseProjectView::SIDEBAR_SENTENCES_BREAKDOWN_SECTION_ID },
        { 8075, BaseProjectView::SYLLABLE_PIECHART_PAGE_ID },
        { 9000, EditWordListDlg::ID_FILE_PATH_FIELD },
        { 10001, ProjectWizardDlg::ID_FILE_BROWSE_BUTTON },
        { 10002, ProjectWizardDlg::ID_FROM_FILE_BUTTON },
        { 10003, ProjectWizardDlg::ID_MANUALLY_ENTERED_TEXT_BUTTON },
        { 10004, ProjectWizardDlg::NARRATIVE_WITH_ILLUSTRATIONS_LINK_ID },
        { 10005, ProjectWizardDlg::FRAGMENTED_LINK_ID },
        { 10006, ProjectWizardDlg::CENTERED_TEXT_LINK_ID },
        { 10007, ProjectWizardDlg::TEST_SELECT_METHOD_BUTTON },
        { 10008, ProjectWizardDlg::LANGUAGE_BUTTON },
        { 10009, ProjectWizardDlg::ID_BATCH_FOLDER_BROWSE_BUTTON },
        { 10010, ProjectWizardDlg::ID_BATCH_FILE_BROWSE_BUTTON },
        { 10011, ProjectWizardDlg::ID_RANDOM_SAMPLE_CHECK },
        { 10012, ProjectWizardDlg::ID_RANDOM_SAMPLE_LABEL },
        { 10013, ProjectWizardDlg::ID_WEB_PAGES_BROWSE_BUTTON },
        { 10014, ProjectWizardDlg::ID_ARCHIVE_FILE_BROWSE_BUTTON },
        { 10015, ProjectWizardDlg::ID_SPREADSHEET_FILE_BROWSE_BUTTON },
        { 10016, ProjectWizardDlg::NEWLINES_ALWAYS_NEW_PARAGRAPH_LINK_ID },
        { 10017, ProjectWizardDlg::ID_WEB_PAGE_BROWSE_BUTTON },
        { 10018, ProjectWizardDlg::ID_ADD_FILE_BUTTON },
        { 10019, ProjectWizardDlg::ID_DELETE_FILE_BUTTON },
        { 10020, ProjectWizardDlg::ID_HARD_RETURN_CHECKBOX },
        { 10021, ProjectWizardDlg::ID_HARD_RETURN_LABEL },
        { 10022, ProjectWizardDlg::ID_NONNARRATIVE_RADIO_BUTTON },
        { 10023, ProjectWizardDlg::ID_NONNARRATIVE_LABEL },
        { 10024, ProjectWizardDlg::ID_NARRATIVE_RADIO_BUTTON },
        { 10025, ProjectWizardDlg::ID_NARRATIVE_LABEL },
        { 10026, ProjectWizardDlg::ID_SENTENCES_SPLIT_RADIO_BUTTON },
        { 10027, ProjectWizardDlg::ID_SENTENCES_SPLIT_LABEL }
    };

    SetAppFileExtension(_DT(L"rsp"));
    SetDocumentTypeName(_DT(L"ReadabilityStudio.Document") );
    SetDocumentVersionNumber(_READSTUDIO_PROGRAM_NUMBERED_VERSION);

    // create the document template
    [[maybe_unused]] wxDocTemplate* docTemplate =
        new wxDocTemplate(GetDocManager(), _(L"Standard Project"), _DT(L"*.rsp"),
            wxEmptyString, GetAppFileExtension(),
            GetAppFileExtension() + _DT(L" Doc"), _DT(L"View"),
            CLASSINFO(ProjectDoc),
            CLASSINFO(ProjectView));

    if (GetLicenseAdmin().IsFeatureEnabled(FeatureProfessionalCode()))
        {
        [[maybe_unused]] wxDocTemplate* batchDocTemplate =
            new wxDocTemplate(GetDocManager(), _(L"Batch Project"), _DT(L"*.rsbp"),
                wxEmptyString, _DT(L"rsbp"),
                _DT(L"rsbp Doc"), _DT(L"View"),
                CLASSINFO(BatchProjectDoc),
                CLASSINFO(BatchProjectView));
        }

    wxArrayString extensions;
    extensions.Add(GetAppFileExtension());
    if (GetLicenseAdmin().IsFeatureEnabled(FeatureProfessionalCode()))
        { extensions.Add(_DT(L"rsbp")); }
    GetMainFrame()->SetDefaultFileExtentions(extensions);

    // printer options
    GetMainFrame()->GetDocumentManager()->GetPageSetupDialogData().
        GetPrintData().SetPaperId(static_cast<wxPaperSize>(GetAppOptions().GetPaperId()));
    GetMainFrame()->GetDocumentManager()->GetPageSetupDialogData().
        GetPrintData().SetOrientation(GetAppOptions().GetPaperOrientation());
    GetMainFrame()->GetDocumentManager()->GetPageSetupDialogData().EnableMargins(false);

    // get a random image for the About box
    std::uniform_int_distribution<size_t> randNum(0, GetSplashscreenPaths().GetCount()-1);
    const size_t imageIndex = randNum(GetRandomNumberEngine());
    if (imageIndex < GetSplashscreenPaths().GetCount())
        {
        wxString ext{ GetSplashscreenPaths()[imageIndex] };
        auto scaledBmp = GetScaledImage(GetSplashscreenPaths()[imageIndex],
                                        Image::GetImageFileTypeFromExtension(ext),
                                        wxSize(500,400));
        // crop the bottom
        GetMainFrameEx()->SetAboutDialogImage(
            wxBitmap(wxImage(scaledBmp.ConvertToImage()).Resize(
                     GetMainFrame()->FromDIP(wxSize(500, 200)), wxPoint(0, 0))));
        }

    // set the help
    GetMainFrame()->SetHelpDirectory(FindResourceDirectory(L"ReadabilityStudioDocs"));
    wxLogMessage(L"Documentation Location: %s", GetMainFrame()->GetHelpDirectory());

    // load the full set of user settings
    GetAppOptions().LoadOptionsFile(AppSettingFolderPath + L"Settings.xml", false);

    // clang-format off
    // add some standard test bundles
    // PSK
    TestBundle PskBundle(ReadabilityMessages::GetPskBundleName().wc_str());
    PskBundle.GetTestGoals().insert(TestGoal{ ReadabilityMessages::PSK_DALE_CHALL().wc_str() });
    PskBundle.GetTestGoals().insert(TestGoal{ ReadabilityMessages::PSK_FLESCH().wc_str() });
    PskBundle.GetTestGoals().insert(TestGoal{ ReadabilityMessages::PSK_GUNNING_FOG().wc_str() });
    PskBundle.GetTestGoals().insert(TestGoal{ ReadabilityMessages::PSK_FARR_JENKINS_PATERSON().wc_str() });
    PskBundle.SetDescription(
        _(L"Powers, Sumner, and Kearl's four adjusted formulas, which were recalculated "
           "using the McCall-Crabbs 1950 tests. These formulas were also adjusted to predict "
           "closer scores to each other.").wc_str());
    PskBundle.SetLanguage(readability::test_language::english_test);
    PskBundle.Lock();
    BaseProject::m_testBundles.insert(PskBundle);
    dynamic_cast<MainFrame*>(GetMainFrame())->AddTestBundleToMenus(PskBundle.GetName().c_str());

    // Kincaid's Navy Personnel tests
    TestBundle NavyBundle(ReadabilityMessages::GetKincaidNavyBundleName().wc_str());
    NavyBundle.GetTestGoals().insert(TestGoal{ ReadabilityMessages::FLESCH_KINCAID().wc_str() });
    NavyBundle.GetTestGoals().insert(TestGoal{ ReadabilityMessages::NEW_ARI().wc_str() });
    NavyBundle.GetTestGoals().insert(TestGoal{ ReadabilityMessages::NEW_FOG().wc_str() });
    NavyBundle.SetDescription(
        _(L"Kincaid's collection of recalculated tests, "
           "designed for enlisted U.S. Navy Personnel.").wc_str());
    NavyBundle.SetLanguage(readability::test_language::english_test);
    NavyBundle.Lock();
    BaseProject::m_testBundles.insert(NavyBundle);
    dynamic_cast<MainFrame*>(GetMainFrame())->AddTestBundleToMenus(NavyBundle.GetName().c_str());

    // Grundner's Consent Forms
    TestBundle ConsentFormsBundle(ReadabilityMessages::GetConsentFormsBundleName().wc_str());
    ConsentFormsBundle.GetTestGoals() =
        {
        TestGoal{ ReadabilityMessages::ELF().wc_str(), std::numeric_limits<double>::quiet_NaN(), 12 },
        // intersection of 4.5 (y axis) and 150 (x axis)
        TestGoal{ ReadabilityMessages::FRY().wc_str(), std::numeric_limits<double>::quiet_NaN(), 10 },
        // Academic, Scientific, or Quality are too difficult (i.e., below 60)
        TestGoal{ ReadabilityMessages::FLESCH().wc_str(), 60, std::numeric_limits<double>::quiet_NaN() },
        // no constraints recommended
        TestGoal{ ReadabilityMessages::SMOG().wc_str() }
        };
    ConsentFormsBundle.GetStatGoals() =
        {
        // specific Fry statistic constraints
        { _DT(L"sentences-per-100-words"), 4.5, std::numeric_limits<double>::quiet_NaN() },
        { _DT(L"syllables-per-100-words"), std::numeric_limits<double>::quiet_NaN(), 150 }
        };
    ConsentFormsBundle.SetDescription(
        _(L"Grundner's recommendations for patient consent forms.\n\n"
           "This bundle includes the tests Fry, Flesch Reading Ease, SMOG, and "
           "the Easy Listening Formula, as recommended by T. M. Grundner (\"Consent Forms\" 9-10). "
           "Also included are the recommended test scores (i.e., goals) that consent forms "
           "should fall within. (Note that SMOG did not have a recommended test score.)").wc_str());
    ConsentFormsBundle.SetLanguage(readability::test_language::english_test);
    ConsentFormsBundle.Lock();
    BaseProject::m_testBundles.insert(ConsentFormsBundle);
    dynamic_cast<MainFrame*>(GetMainFrame())->AddTestBundleToMenus(ConsentFormsBundle.GetName().c_str());
    // clang-format on

    // See if ClearType is turned on. If not, then graphs will look awful, so ask user about turning it on.
#ifdef __WXMSW__
    int fontSmoothing{ 0 }, smoothingType{ 0 };
    ::SystemParametersInfo(SPI_GETFONTSMOOTHING, 0, &fontSmoothing, 0);
    ::SystemParametersInfo(SPI_GETFONTSMOOTHINGTYPE, 0, &smoothingType, 0);
    if (fontSmoothing == 0 || smoothingType != FE_FONTSMOOTHINGCLEARTYPE)
        {
        std::vector<WarningMessage>::iterator warningIter =
            WarningManager::GetWarning(_DT(L"clear-type-turned-off"));
        if (warningIter != WarningManager::GetWarnings().end() &&
            warningIter->ShouldBeShown())
            {
            wxRichMessageDialog msg(GetMainFrame(), warningIter->GetMessage(),
                                            warningIter->GetTitle(), warningIter->GetFlags());
            msg.ShowCheckBox(_(L"Remember my answer"));
            const int dlgResponse = msg.ShowModal();
            if (warningIter != WarningManager::GetWarnings().end() &&
                msg.IsCheckBoxChecked())
                {
                warningIter->Show(false);
                warningIter->SetPreviousResponse(dlgResponse);
                }
            if (dlgResponse == wxID_YES)
                {
                SystemParametersInfo(SPI_SETFONTSMOOTHING, TRUE, 0, SPIF_UPDATEINIFILE|SPIF_SENDCHANGE);
                SystemParametersInfo(SPI_SETFONTSMOOTHINGTYPE, 0, (PVOID)FE_FONTSMOOTHINGCLEARTYPE, SPIF_UPDATEINIFILE|SPIF_SENDCHANGE);
                }
            }
        else if (warningIter != WarningManager::GetWarnings().end() &&
            warningIter->GetPreviousResponse() == wxID_YES)
            {
            SystemParametersInfo(SPI_SETFONTSMOOTHING, TRUE, 0, SPIF_UPDATEINIFILE|SPIF_SENDCHANGE);
            SystemParametersInfo(SPI_SETFONTSMOOTHINGTYPE, 0, (PVOID)FE_FONTSMOOTHINGCLEARTYPE, SPIF_UPDATEINIFILE|SPIF_SENDCHANGE);
            }
        }
#endif

    GetAppOptions().SaveOptionsFile();

    return true;
    }

//-----------------------------------
bool ReadabilityApp::LoadWordLists(const wxString& AppSettingFolderPath)
    {
    // load the Word & phrase files--note that these should be pre-sorted
    wxFile theFile;
    if (!theFile.Open(FindResourceFile(L"Words.wad"), wxFile::read))
        {
        wxMessageBox(_(L"Word & phrase file missing or corrupt. Please reinstall."), _(L"Error"),
                     wxOK | wxICON_EXCLAMATION);
        return false;
        }
    auto wordyZipFileText = std::make_unique<char[]>(theFile.Length() + 1);
    const size_t readSize = theFile.Read(wordyZipFileText.get(), theFile.Length());
    Wisteria::ZipCatalog cat(wordyZipFileText.get(), readSize);
    // read in the wordy items
    wxString englishWordyPhraseFileText = cat.ReadTextFile(L"WordyPhrases/English.txt");
    wxString spanishWordyPhraseFileText = cat.ReadTextFile(L"WordyPhrases/Spanish.txt");
    wxString germanWordyPhraseFileText = cat.ReadTextFile(L"WordyPhrases/German.txt");
    wxString DCReplacementFileText = cat.ReadTextFile(L"WordyPhrases/Dale-ChallReplacements.txt");
    wxString SpacheReplacementFileText = cat.ReadTextFile(L"WordyPhrases/SpacheReplacements.txt");
    wxString HarrisJacobsonReplacementFileText =
        cat.ReadTextFile(L"WordyPhrases/Harris-JacobsonReplacements.txt");
    wxString DifficultWordReplacementFileText =
        cat.ReadTextFile(L"WordyPhrases/SingleWordReplacementsEnglish.txt");
    // proper noun stop word list
    wxString properNounStopList = cat.ReadTextFile(L"StopWords/ProperNounsStopList.txt");
    // copyright notices
    wxString copyRightNoticePhraseFileText = cat.ReadTextFile(L"CopyrightNotices/List.txt");
    // citation headers
    wxString citationPhraseFileText = cat.ReadTextFile(L"CitationHeaders/List.txt");
    // known proper nouns
    wxString properNounsFileText = cat.ReadTextFile(L"ProperNouns/All.txt");
    wxString personalNounsFileText = cat.ReadTextFile(L"ProperNouns/Personal.txt");
    // stop lists
    wxString EnglishStopList = cat.ReadTextFile(L"StopWords/English.txt");
    wxString SpanishStopList = cat.ReadTextFile(L"StopWords/Spanish.txt");
    wxString GermanStopList = cat.ReadTextFile(L"StopWords/German.txt");
    // known spellings
    wxString EnglishSpellingsFileText = cat.ReadTextFile(L"Dictionaries/English.txt");
    wxString SpanishspellingsFileText = cat.ReadTextFile(L"Dictionaries/Spanish.txt");
    wxString GermanspellingsFileText = cat.ReadTextFile(L"Dictionaries/German.txt");
    wxString ProgrammingSpellingsFileText = cat.ReadTextFile(L"Programming/All Languages.txt");
    // articles
    wxString aExceptionsFileText = cat.ReadTextFile(_DT(L"Articles/AExceptions.txt"));
    wxString anExceptionsFileText = cat.ReadTextFile(_DT(L"Articles/AnExceptions.txt"));
    // read in the abbreviations
    wxString abbreviationsFileText = cat.ReadTextFile(_DT(L"Abbreviations/List.txt"));
    wxString nonAbbreviationsFileText =
        cat.ReadTextFile(_DT(L"Abbreviations/non-abbreviations.txt"));
    // read in the past-participle exceptions
    wxString pastParticipleExceptionsFileText =
        cat.ReadTextFile(_DT(L"PastParticiples/Exceptions.txt"));
    // read in the DC words
    wxString dcFileText = cat.ReadTextFile(_DT(L"WordLists/New Dale-Chall.txt"));
    // read in the DC Catholic supplementary words
    wxString stockerCatholicFileText =
        cat.ReadTextFile(_DT(L"WordLists/Stocker Catholic Supplement.txt"));
    // read in the Spache words
    wxString spacheFileText = cat.ReadTextFile(_DT(L"WordLists/Revised Spache.txt"));
    // Harris-Jacobson words
    wxString harrisJacobsonFileText = cat.ReadTextFile(_DT(L"WordLists/Harris-Jacobson.txt"));
    // read in the Dolch words
    wxString dolchFileText = cat.ReadTextFile(_DT(L"WordLists/Dolch.txt"));

    BaseProject::english_wordy_phrases.load_phrases(englishWordyPhraseFileText, false, false);
    BaseProject::spanish_wordy_phrases.load_phrases(spanishWordyPhraseFileText, false, false);
    BaseProject::german_wordy_phrases.load_phrases(germanWordyPhraseFileText, false, false);

    BaseProject::dale_chall_replacement_list.load_words(DCReplacementFileText, false);
    BaseProject::spache_replacement_list.load_words(SpacheReplacementFileText, false);
    BaseProject::harris_jacobson_replacement_list.load_words(HarrisJacobsonReplacementFileText,
                                                             false);
    BaseProject::difficult_word_replacement_list.load_words(DifficultWordReplacementFileText,
                                                            false);

    BaseProject::known_proper_nouns.load_words(properNounsFileText, false, false);
    BaseProject::known_personal_nouns.load_words(personalNounsFileText, false, false);
    BaseProject::english_stoplist.load_words(EnglishStopList, false, false);
    BaseProject::spanish_stoplist.load_words(SpanishStopList, false, false);
    BaseProject::german_stoplist.load_words(GermanStopList, false, false);

    BaseProject::m_dale_chall_word_list.load_words(dcFileText, false, false);
    BaseProject::m_stocker_catholic_word_list.load_words(stockerCatholicFileText, false, false);
    BaseProject::m_dale_chall_plus_stocker_catholic_word_list.load_words(dcFileText, false, false);
    BaseProject::m_dale_chall_plus_stocker_catholic_word_list.load_words(stockerCatholicFileText,
                                                                         true, true);
    BaseProject::m_spache_word_list.load_words(spacheFileText, false, false);
    BaseProject::m_harris_jacobson_word_list.load_words(harrisJacobsonFileText, false, false);
    BaseProject::m_dolch_word_list.load_words(dolchFileText);

    BaseProject::known_english_spellings.load_words(EnglishSpellingsFileText, false, false);
    BaseProject::known_programming_spellings.load_words(ProgrammingSpellingsFileText, false, false);
    m_CustomEnglishDictionaryPath = AppSettingFolderPath + L"DictionaryEN.txt";
    wxString ExtraDictionaryText;
    if (wxFile::Exists(m_CustomEnglishDictionaryPath) &&
        Wisteria::TextStream::ReadFile(m_CustomEnglishDictionaryPath, ExtraDictionaryText))
        {
        BaseProject::known_custom_english_spellings.load_words(ExtraDictionaryText, true, false);
        }
    // if the custom dictionary doesn't exist, then create an empty file
    else if (!wxFile::Exists(m_CustomEnglishDictionaryPath))
        {
        wxFile outputFile(m_CustomEnglishDictionaryPath, wxFile::write);
        // put a space in the file so that it isn't zero length, that will prevent memory map
        // exceptions later
        outputFile.Write(L" ", wxConvUTF8);
        }

    BaseProject::known_spanish_spellings.load_words(SpanishspellingsFileText, false, false);
    m_CustomSpanishDictionaryPath = AppSettingFolderPath + L"DictionaryES.txt";
    if (wxFile::Exists(m_CustomSpanishDictionaryPath) &&
        Wisteria::TextStream::ReadFile(m_CustomSpanishDictionaryPath, ExtraDictionaryText))
        {
        BaseProject::known_custom_spanish_spellings.load_words(ExtraDictionaryText, true, false);
        }
    // if the custom dictionary doesn't exist, then create an empty file
    else if (!wxFile::Exists(m_CustomSpanishDictionaryPath))
        {
        wxFile outputFile(m_CustomSpanishDictionaryPath, wxFile::write);
        outputFile.Write(L" ", wxConvUTF8);
        }

    BaseProject::known_german_spellings.load_words(GermanspellingsFileText, false, false);
    m_CustomGermanDictionaryPath = AppSettingFolderPath + L"DictionaryDE.txt";
    if (wxFile::Exists(m_CustomGermanDictionaryPath) &&
        Wisteria::TextStream::ReadFile(m_CustomGermanDictionaryPath, ExtraDictionaryText))
        {
        BaseProject::known_custom_german_spellings.load_words(ExtraDictionaryText, true, false);
        }
    // if the custom dictionary doesn't exist, then create an empty file
    else if (!wxFile::Exists(m_CustomGermanDictionaryPath))
        {
        wxFile outputFile(m_CustomGermanDictionaryPath, wxFile::write);
        outputFile.Write(L" ", wxConvUTF8);
        }

    // clang-format off
    BaseProject::copyright_notice_phrases.load_phrases(
        copyRightNoticePhraseFileText, false, false);
    BaseProject::citation_phrases.load_phrases(
        citationPhraseFileText, false, false);
    grammar::is_non_proper_word::get_word_list().load_words(
        properNounStopList, true, false);
    grammar::is_abbreviation::get_abbreviations().load_words(
        abbreviationsFileText, true, false);
    grammar::is_abbreviation::get_non_abbreviations().load_words(
        nonAbbreviationsFileText, true, false);
    grammar::is_english_passive_voice::get_past_participle_exeptions().load_words(
        pastParticipleExceptionsFileText, true, false);
    grammar::is_incorrect_english_article::get_a_exceptions().load_words(
        aExceptionsFileText, true, false);
    grammar::is_incorrect_english_article::get_an_exceptions().load_words(
        anExceptionsFileText, true, false);
    // clang-format on

    return true;
    }

//-----------------------------------
bool ReadabilityApp::VerifyWordLists()
    {
    bool retVal = true;
    // verify the dictionaries are sorted
    // English
    if (!BaseProject::known_english_spellings.is_sorted())
        {
        wxLogError(L"English dictionary is not sorted.");
        retVal = false;
        }
    else
        { wxLogMessage(L"English dictionary is sorted properly."); }
    if (!BaseProject::known_programming_spellings.is_sorted())
        {
        wxLogError(L"Programming dictionary is not sorted.");
        retVal = false;
        }
    else
        { wxLogMessage(L"Programming dictionary is sorted properly."); }
    if (!BaseProject::known_custom_english_spellings.is_sorted())
        {
        wxLogError(L"Custom English dictionary is not sorted.");
        retVal = false;
        }
    else
        { wxLogMessage(L"Custom English dictionary is sorted properly."); }

    // Spanish
    if (!BaseProject::known_spanish_spellings.is_sorted())
        {
        wxLogError(L"Spanish dictionary is not sorted.");
        retVal = false;
        }
    else
        { wxLogMessage(L"Spanish dictionary is sorted properly."); }
    if (!BaseProject::known_custom_spanish_spellings.is_sorted())
        {
        wxLogError(L"Custom Spanish dictionary is not sorted.");
        retVal = false;
        }
    else
        { wxLogMessage(L"Custom Spanish dictionary is sorted properly."); }

    // German
    if (!BaseProject::known_german_spellings.is_sorted())
        {
        wxLogError(_DT(L"German dictionary is not sorted."));
        retVal = false;
        }
    else
        { wxLogMessage(_DT(L"German dictionary is sorted properly.")); }
    if (!BaseProject::known_custom_german_spellings.is_sorted())
        {
        wxLogError(_DT(L"Custom German dictionary is not sorted."));
        retVal = false;
        }
    else
        { wxLogMessage(_DT(L"Custom German dictionary is sorted properly.")); }

    // Proper nouns list
    if (!BaseProject::known_proper_nouns.is_sorted())
        {
        wxLogError(_DT(L"Proper nouns are not sorted."));
        retVal = false;
        }
    else
        { wxLogMessage(_DT(L"Proper nouns are sorted properly.")); }
    if (!BaseProject::known_personal_nouns.is_sorted())
        {
        wxLogError(_DT(L"Personal nouns are not sorted."));
        retVal = false;
        }
    else
        { wxLogMessage(_DT(L"Personal nouns are sorted properly.")); }

    // word lists
    if (!BaseProject::m_dale_chall_word_list.is_sorted())
        {
        wxLogError(_DT(L"DC words are not sorted."));
        retVal = false;
        }
    else
        { wxLogMessage(_DT(L"DC words are sorted properly.")); }

    if (!BaseProject::m_dale_chall_plus_stocker_catholic_word_list.is_sorted() ||
        !BaseProject::m_stocker_catholic_word_list.is_sorted())
        {
        wxLogError(_DT(L"Stocker words are not sorted."));
        retVal = false;
        }
    else
        { wxLogMessage(_DT(L"Stocker words are sorted properly.")); }

    if (!BaseProject::m_spache_word_list.is_sorted())
        {
        wxLogError(_DT(L"Spache words are not sorted."));
        retVal = false;
        }
    else
        { wxLogMessage(_DT(L"Spache words are sorted properly.")); }

    if (!BaseProject::m_harris_jacobson_word_list.is_sorted())
        {
        wxLogError(_DT(L"HJ words are not sorted."));
        retVal = false;
        }
    else
        { wxLogMessage(_DT(L"HJ words are sorted properly.")); }

    // the phrases
    if (!BaseProject::english_wordy_phrases.is_sorted())
        {
        wxLogError(_DT(L"English phrases are not sorted."));
        retVal = false;
        }
    else
        { wxLogMessage(_DT(L"English phrases are sorted properly.")); }

    if (!BaseProject::spanish_wordy_phrases.is_sorted())
        {
        wxLogError(_DT(L"Spanish phrases are not sorted."));
        retVal = false;
        }
    else
        { wxLogMessage(_DT(L"Spanish phrases are sorted properly.")); }

    if (!BaseProject::german_wordy_phrases.is_sorted())
        {
        wxLogError(_DT(L"German phrases are not sorted."));
        retVal = false;
        }
    else
        { wxLogMessage(_DT(L"German phrases are sorted properly.")); }

    if (!BaseProject::copyright_notice_phrases.is_sorted())
        {
        wxLogError(_DT(L"Copyright notices are not sorted."));
        retVal = false;
        }
    else
        { wxLogMessage(_DT(L"Copyright notices are sorted properly.")); }

    if (!BaseProject::citation_phrases.is_sorted())
        {
        wxLogError(_DT(L"Citations are not sorted."));
        retVal = false;
        }
    else
        { wxLogMessage(_DT(L"Citations are sorted properly.")); }

    return retVal;
    }

//-----------------------------------
void ReadabilityApp::LoadInterfaceLicensableFeatures()
    {
    FillWordListsMenu(GetMainFrameEx()->m_wordListMenu);
    FillBlankGraphsMenu(GetMainFrameEx()->m_blankGraphMenu);
    }

//-----------------------------------
void ReadabilityApp::LoadInterface()
    {
    wxArtProvider::Push(new RSArtProvider);
    // load the menu resources
#ifdef __WXOSX__
    if (!wxXmlResource::Get()->Load(GetResourceManager().GetResourceFilePath(_DT(L"menus/menu.xrc"))) )
        { wxLogError(L"Menu file is missing or corrupt. Please reinstall."); }
#endif
    if (!wxXmlResource::Get()->Load(GetResourceManager().GetResourceFilePath(_DT(L"menus/contextmenu.xrc"))) )
        { wxLogError(L"Context menu file is missing or corrupt. Please reinstall."); }

    wxArrayString extensions;
    extensions.Add(GetAppFileExtension());
    // create the main frame window
    SetMainFrame(new MainFrame(GetDocManager(), nullptr, extensions, GetAppName(),
                                wxPoint(0, 0),
                                wxSize(GetAppOptions().GetAppWindowWidth(), GetAppOptions().GetAppWindowHeight()),
                                wxDEFAULT_FRAME_STYLE) );

    wxLogMessage(L"Display Scaling Factor: %.2f", GetMainFrame()->GetDPIScaleFactor());
    m_dpiScaleFactor = GetMainFrame()->GetDPIScaleFactor();

    if (GetAppOptions().IsAppWindowMaximized())
        {
        GetMainFrame()->Maximize();
        GetMainFrame()->SetSize(GetMainFrame()->GetSize());
        }
    wxIcon appIcon;
    appIcon.CopyFromBitmap(wxGetApp().GetResourceManager().
            GetSVG(L"ribbon/app-logo.svg").GetBitmap(GetMainFrame()->FromDIP(wxSize(32, 32))));
    GetMainFrame()->SetIcon(appIcon);
    GetMainFrame()->SetLogo(wxGetApp().GetResourceManager().
            GetSVG(L"ribbon/app-logo.svg").GetBitmap(GetMainFrame()->FromDIP(wxSize(128, 128))));
    // set up the ribbon (and its submenus)
    GetMainFrame()->InitControls(CreateRibbon(GetMainFrame(),nullptr));

    auto menuItem = new wxMenuItem(&GetMainFrameEx()->m_fileOpenMenu,
        wxID_OPEN, _(L"Open Project...") + _DT(L"\tCtrl+O"));
    menuItem->SetBitmap(GetResourceManager().GetSVG(L"ribbon/file-open.svg"));
    GetMainFrameEx()->m_fileOpenMenu.Append(menuItem);
    GetDocManager()->FileHistoryUseMenu(&GetMainFrameEx()->m_fileOpenMenu);
    if (GetMainFrameEx()->m_fileOpenMenu.FindItem(wxID_FILE1) == nullptr)
        { GetDocManager()->FileHistoryAddFilesToMenu(&GetMainFrameEx()->m_fileOpenMenu); }
    FillPrintMenu(GetMainFrameEx()->m_printMenu, RibbonType::MainFrameRibbon);
    GetMainFrameEx()->AddExamplesToMenu(&GetMainFrameEx()->m_exampleMenu);
    MainFrame::FillMenuWithCustomTests(&GetMainFrameEx()->m_customTestsMenu, nullptr, false);
    MainFrame::FillMenuWithTestBundles(&GetMainFrameEx()->m_testsBundleMenu, nullptr, false);
    // create the menubar (macOS only)
#ifdef __WXOSX__
    wxMenuBar* menuBar = wxXmlResource::Get()->LoadMenuBar(_DT(L"ID_MENUBAR"));
    assert(menuBar);
    if (menuBar)
        { GetMainFrame()->SetMenuBar(menuBar); }
    else
        { wxLogError(L"Menubar loading failed."); }
#endif

    LoadFileHistoryMenu();

    if (GetMainFrame()->GetMenuBar())
        {
        // add examples to the menus
        const wxMenuItem* exampleMenuItem = GetMainFrame()->GetMenuBar()->FindItem(XRCID("ID_EXAMPLES"));
        if (exampleMenuItem)
            { GetMainFrameEx()->AddExamplesToMenu(exampleMenuItem->GetSubMenu()); }
        // add the custom tests and bundles
        int readMenuIndex = GetMainFrame()->GetMenuBar()->FindMenu(_(L"Readability"));
        if (readMenuIndex != wxNOT_FOUND)
            {
            wxMenu* readMenu = GetMainFrame()->GetMenuBar()->GetMenu(readMenuIndex);
            if (readMenu)
                {
                // custom tests
                GetMainFrameEx()->m_customTestsRegularMenu = new wxMenu;
                readMenu->AppendSubMenu(GetMainFrameEx()->m_customTestsRegularMenu, _(L"Custom Tests"));
                MainFrame::FillMenuWithCustomTests(GetMainFrameEx()->m_customTestsRegularMenu, nullptr, false);
                // add test bundles
                GetMainFrameEx()->m_testsBundleRegularMenu= new wxMenu;
                readMenu->AppendSubMenu(GetMainFrameEx()->m_testsBundleRegularMenu, _(L"Test Bundles"));
                MainFrame::FillMenuWithTestBundles(GetMainFrameEx()->m_testsBundleRegularMenu, nullptr, false);
                }
            }
        }

    InitProjectSidebar();
    InitStartPage();

    // show the interface
    GetMainFrame()->Centre();
    GetMainFrame()->Show();
    GetMainFrame()->Update();
    SetTopWindow(GetMainFrame());
    }

//-----------------------------------
int ReadabilityApp::OnExit()
    {
    wxLogDebug(__WXFUNCTION__);
    m_webHarvester.CancelPending();
    GetAppOptions().SaveOptionsFile();

    return BaseApp::OnExit();
    }

//-----------------------------------
void ReadabilityApp::EditCustomTest(CustomReadabilityTest& selectedTest)
    {
    // make sure there aren't any projects getting updated before we start changing these tests
    const auto& docs = GetDocManager()->GetDocuments();
    for (size_t i = 0; i < docs.GetCount(); ++i)
        {
        const BaseProjectDoc* doc = dynamic_cast<BaseProjectDoc*>(docs.Item(i)->GetData());
        if (!doc->IsSafeToUpdate())
            { return; }
        }

    CustomTestDlg dlg(GetMainFrame(), wxID_ANY, selectedTest.get_name().c_str());
    dlg.SetStemmingType(selectedTest.get_stemming_type());
    dlg.SetFormula(selectedTest.get_formula().c_str());
    dlg.SetTestType(static_cast<int>(selectedTest.get_test_type()));
    dlg.SetWordListFilePath(selectedTest.get_familiar_word_list_file_path().c_str());
    dlg.SetIncludingCustomWordList(selectedTest.is_including_custom_familiar_word_list());
    dlg.SetIncludingDaleChallList(selectedTest.is_including_dale_chall_list());
    dlg.SetIncludingSpacheList(selectedTest.is_including_spache_list());
    dlg.SetIncludingHJList(selectedTest.is_including_harris_jacobson_list());
    dlg.SetIncludingStockerList(selectedTest.is_including_stocker_list());
    dlg.SetFamiliarWordsMustBeOnEachIncludedList(selectedTest.is_familiar_words_must_be_on_each_included_list());
    dlg.SetProperNounMethod(static_cast<int>(selectedTest.get_proper_noun_method()));
    dlg.SetIncludingNumeric(selectedTest.is_including_numeric_as_familiar());
    dlg.SetChildrensPublishingSelected(selectedTest.has_industry_classification(
        readability::industry_classification::childrens_publishing_industry));
    dlg.SetAdultPublishingSelected(selectedTest.has_industry_classification(
        readability::industry_classification::adult_publishing_industry));
    dlg.SetSecondaryLanguageSelected(selectedTest.has_industry_classification(
        readability::industry_classification::sedondary_language_industry));
    dlg.SetBroadcastingSelected(selectedTest.has_industry_classification(
        readability::industry_classification::broadcasting_industry));
    dlg.SetChildrensHealthCareTestSelected(selectedTest.has_industry_classification(
        readability::industry_classification::childrens_healthcare_industry));
    dlg.SetAdultHealthCareTestSelected(selectedTest.has_industry_classification(
        readability::industry_classification::adult_healthcare_industry));
    dlg.SetMilitaryTestSelected(selectedTest.has_industry_classification(
        readability::industry_classification::military_government_industry));
    dlg.SetGeneralDocumentSelected(selectedTest.has_document_classification(
        readability::document_classification::general_document));
    dlg.SetTechnicalDocumentSelected(selectedTest.has_document_classification(
        readability::document_classification::technical_document));
    dlg.SetNonNarrativeFormSelected(selectedTest.has_document_classification(
        readability::document_classification::nonnarrative_document));
    dlg.SetYoungAdultAndAdultLiteratureSelected(selectedTest.has_document_classification(
        readability::document_classification::adult_literature_document));
    dlg.SetChildrensLiteratureSelected(selectedTest.has_document_classification(
        readability::document_classification::childrens_literature_document));
    if (dlg.IsIncludingCustomWordList())
        { dlg.SelectPage(CustomTestDlg::ID_WORD_LIST_PAGE); }
    if (dlg.ShowModal() == wxID_OK)
        {
        wxBusyCursor wait;

        // get the word file text here, in case the path is wrong and the user needs to correct it
        wxString filePath = dlg.GetWordListFilePath();
        wxString fileText;
        // load custom word file if they are using one. If not then just load an empty string into this list
        if (dlg.IsIncludingCustomWordList())
            {
            if (!Wisteria::TextStream::ReadFile(filePath, fileText))
                { return; }
            }

        // update the test's info now
        selectedTest.set_stemming_type(dlg.GetStemmingType());
        selectedTest.set_formula(dlg.GetFormula().wc_str());
        selectedTest.set_test_type(static_cast<readability::readability_test_type>(dlg.GetTestType()));
        selectedTest.set_familiar_word_list_file_path(filePath.wc_str());
        selectedTest.include_custom_familiar_word_list(dlg.IsIncludingCustomWordList());
        selectedTest.include_dale_chall_list(dlg.IsIncludingDaleChallList());
        selectedTest.include_spache_list(dlg.IsIncludingSpacheList());
        selectedTest.include_harris_jacobson_list(dlg.IsIncludingHJList());
        selectedTest.include_stocker_list(dlg.IsIncludingStockerList());
        selectedTest.set_familiar_words_must_be_on_each_included_list(dlg.IsFamiliarWordsMustBeOnEachIncludedList());
        selectedTest.set_proper_noun_method(
            static_cast<readability::proper_noun_counting_method>(dlg.GetProperNounMethod()));
        selectedTest.include_numeric_as_familiar(dlg.IsIncludingNumeric());
        selectedTest.add_industry_classification(
            readability::industry_classification::childrens_publishing_industry,
            dlg.IsChildrensPublishingSelected());
        selectedTest.add_industry_classification(
            readability::industry_classification::adult_publishing_industry,
            dlg.IsAdultPublishingSelected());
        selectedTest.add_industry_classification(
            readability::industry_classification::sedondary_language_industry,
            dlg.IsSecondaryLanguageSelected());
        selectedTest.add_industry_classification(
            readability::industry_classification::broadcasting_industry,
            dlg.IsBroadcastingSelected());
        selectedTest.add_industry_classification(
            readability::industry_classification::childrens_healthcare_industry,
            dlg.IsChildrensHealthCareTestSelected());
        selectedTest.add_industry_classification(
            readability::industry_classification::adult_healthcare_industry,
            dlg.IsAdultHealthCareTestSelected());
        selectedTest.add_industry_classification(
            readability::industry_classification::military_government_industry,
            dlg.IsMilitaryTestSelected());
        selectedTest.add_document_classification(
            readability::document_classification::general_document,
            dlg.IsGeneralDocumentSelected());
        selectedTest.add_document_classification(
            readability::document_classification::technical_document,
            dlg.IsTechnicalDocumentSelected());
        selectedTest.add_document_classification(
            readability::document_classification::nonnarrative_document,
            dlg.IsNonNarrativeFormSelected());
        selectedTest.add_document_classification(
            readability::document_classification::adult_literature_document,
            dlg.IsYoungAdultAndAdultLiteratureSelected());
        selectedTest.add_document_classification(
            readability::document_classification::childrens_literature_document,
            dlg.IsChildrensLiteratureSelected());

        // reload the word file
        selectedTest.load_custom_familiar_words(fileText);

        // reload any projects that have this test in it
        if ((docs.GetCount() > 0) &&
            wxMessageBox(_(L"Do you wish to recalculate any open projects that include this test?"),
                _(L"Project Update"), wxYES_NO|wxICON_QUESTION) == wxYES)
            {
            for (size_t i = 0; i < docs.GetCount(); ++i)
                {
                BaseProjectDoc* doc = dynamic_cast<BaseProjectDoc*>(docs.Item(i)->GetData());
                if (doc && doc->HasCustomTest(selectedTest.get_name().c_str()))
                    {
                    doc->Modify(true);
                    // projects will need to do a full re-indexing
                    doc->RefreshRequired(ProjectRefresh::FullReindexing);
                    doc->RefreshProject();
                    }
                }
            }
        }
    }

//-------------------------------------------------------
void ReadabilityApp::FillBlankGraphsMenu(wxMenu& blankGraphsMenu)
    {
    while (blankGraphsMenu.GetMenuItemCount())
        { blankGraphsMenu.Destroy(blankGraphsMenu.FindItemByPosition(0)); }

    wxMenuItem* menuItem =
        new wxMenuItem(&blankGraphsMenu, XRCID("ID_BLANK_DB2_GRAPH"), BaseProjectView::GetDB2Label());
    menuItem->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"tests/danielson-bryan-2.svg"));
    blankGraphsMenu.Append(menuItem);

    menuItem = new wxMenuItem(&blankGraphsMenu, XRCID("ID_BLANK_FLESCH_GRAPH"), _DT(L"Flesch Reading Ease"));
    menuItem->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"tests/flesch-test.svg"));
    blankGraphsMenu.Append(menuItem);

    menuItem = new wxMenuItem(&blankGraphsMenu, XRCID("ID_BLANK_FRY_GRAPH"), _DT(L"Fry"));
    menuItem->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"tests/fry-test.svg"));
    blankGraphsMenu.Append(menuItem);

    menuItem = new wxMenuItem(&blankGraphsMenu, XRCID("ID_BLANK_LIX_GRAPH"), _DT(L"Lix Gauge"));
    menuItem->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"tests/lix-test.svg"));
    blankGraphsMenu.Append(menuItem);

    menuItem = new wxMenuItem(&blankGraphsMenu, XRCID("ID_BLANK_RAYGOR_GRAPH"), _DT(L"Raygor"));
    menuItem->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"tests/raygor-test.svg"));
    blankGraphsMenu.Append(menuItem);

    if (wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureProfessionalCode()) ||
        wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureLanguagePackCode()))
        {
        // Spanish
        blankGraphsMenu.AppendSeparator();

        menuItem = new wxMenuItem(&blankGraphsMenu, XRCID("ID_BLANK_CRAWFORD_GRAPH"), _DT(L"Crawford"));
        menuItem->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"tests/crawford.svg"));
        blankGraphsMenu.Append(menuItem);

        menuItem = new wxMenuItem(&blankGraphsMenu, XRCID("ID_BLANK_FRASE_GRAPH"), _DT(L"FRASE"));
        menuItem->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"tests/frase.svg"));
        blankGraphsMenu.Append(menuItem);

        menuItem = new wxMenuItem(&blankGraphsMenu, XRCID("ID_BLANK_GPM_GRAPH"),
                                    _DT(L"Gilliam-Pe\U000000F1a-Mountain"));
        menuItem->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"tests/gilliam-pena-mountain-fry-graph.svg"));
        blankGraphsMenu.Append(menuItem);

        // German
        blankGraphsMenu.AppendSeparator();

        menuItem = new wxMenuItem(&blankGraphsMenu, XRCID("ID_BLANK_GERMAN_LIX_GRAPH"), _DT(L"German Lix Gauge"));
        menuItem->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"tests/lix-test.svg"));
        blankGraphsMenu.Append(menuItem);

        menuItem = new wxMenuItem(&blankGraphsMenu, XRCID("ID_BLANK_SCHWARTZ_GRAPH"), _DT(L"Schwartz"));
        menuItem->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"tests/schwartz.svg"));
        blankGraphsMenu.Append(menuItem);
        }
    }

//-------------------------------------------------------
void ReadabilityApp::FillWordListsMenu(wxMenu& wordListMenu)
    {
    while (wordListMenu.GetMenuItemCount())
        { wordListMenu.Destroy(wordListMenu.FindItemByPosition(0)); }
    wxMenuItem* menuItem = new wxMenuItem(&wordListMenu, XRCID("ID_DOLCH_WORD_LIST_WINDOW"),
                                          _(L"Dolch Sight Words"));
    menuItem->SetBitmap(
        wxGetApp().GetResourceManager().GetSVG(L"tests/dolch.svg"));
    wordListMenu.Append(menuItem);

    menuItem = new wxMenuItem(&wordListMenu, XRCID("ID_HARRIS_JACOBSON_WORD_LIST_WINDOW"),
                              _DT(L"Harris-Jacobson"));
    menuItem->SetBitmap(
        wxGetApp().GetResourceManager().GetSVG(L"tests/spache-test.svg"));
    wordListMenu.Append(menuItem);

    menuItem = new wxMenuItem(&wordListMenu, XRCID("ID_DC_WORD_LIST_WINDOW"), _DT(L"New Dale-Chall"));
    menuItem->SetBitmap(
        wxGetApp().GetResourceManager().GetSVG(L"tests/dale-chall-test.svg"));
    wordListMenu.Append(menuItem);

    menuItem = new wxMenuItem(&wordListMenu, XRCID("ID_SPACHE_WORD_LIST_WINDOW"), _DT("Spache Revised"));
    menuItem->SetBitmap(
        wxGetApp().GetResourceManager().GetSVG(L"tests/spache-test.svg"));
    wordListMenu.Append(menuItem);

    menuItem = new wxMenuItem(&wordListMenu, XRCID("ID_STOCKER_CATHOLIC_WORD_LIST_WINDOW"),
                              _(L"Stocker's Catholic Supplement"));
    menuItem->SetBitmap(
        wxGetApp().GetResourceManager().GetSVG(L"tests/stocker.svg"));
    wordListMenu.Append(menuItem);

    wordListMenu.AppendSeparator();
    menuItem = new wxMenuItem(&wordListMenu, XRCID("ID_EDIT_WORD_LIST"), _(L"Edit Word List..."));
    menuItem->SetBitmap(wxArtProvider::GetBitmapBundle(L"ID_EDIT"));
    wordListMenu.Append(menuItem);
    }

//-------------------------------------------------------
void ReadabilityApp::FillGradeScalesMenu(wxMenu& menu)
    {
    while (menu.GetMenuItemCount())
        { menu.Destroy(menu.FindItemByPosition(0)); }

    // these options get checked, so can't use icons on them
    menu.Append(new wxMenuItem(&menu, XRCID("ID_K12_US"), _(L"K-12+ (United States of America)"),
                               wxString{}, wxITEM_CHECK));
    menu.Append(new wxMenuItem(&menu, XRCID("ID_K12_NEWFOUNDLAND"),
                               _(L"K-12+ (Newfoundland and Labrador)"),
                               wxString{}, wxITEM_CHECK));
    menu.Append(new wxMenuItem(&menu, XRCID("ID_K12_BC"), _(L"K-12+ (British Columbia/Yukon)"),
                               wxString{}, wxITEM_CHECK));
    menu.Append(new wxMenuItem(&menu, XRCID("ID_K12_NEW_BRUNSWICK"), _(L"K-12+ (New Brunswick)"),
                               wxString{}, wxITEM_CHECK));
    menu.Append(new wxMenuItem(&menu, XRCID("ID_K12_NOVA_SCOTIA"), _(L"K-12+ (Nova Scotia)"),
                               wxString{}, wxITEM_CHECK));
    menu.Append(new wxMenuItem(&menu, XRCID("ID_K12_ONTARIO"), _(L"K-12+ (Ontario)"),
                               wxString{}, wxITEM_CHECK));
    menu.Append(new wxMenuItem(&menu, XRCID("ID_K12_SASKATCHEWAN"), _(L"K-12+ (Saskatchewan)"),
                               wxString{}, wxITEM_CHECK));
    menu.Append(new wxMenuItem(&menu, XRCID("ID_K12_PE"), _(L"K-12+ (Prince Edward Island)"),
                               wxString{}, wxITEM_CHECK));
    menu.Append(new wxMenuItem(&menu, XRCID("ID_K12_MANITOBA"), _(L"K-12+ (Manitoba)"),
                               wxString{}, wxITEM_CHECK));
    menu.Append(new wxMenuItem(&menu, XRCID("ID_K12_NT"), _(L"K-12+ (Northwest Territories)"),
                               wxString{}, wxITEM_CHECK));
    menu.Append(new wxMenuItem(&menu, XRCID("ID_K12_ALBERTA"), _(L"K-12+ (Alberta)"),
                               wxString{}, wxITEM_CHECK));
    menu.Append(new wxMenuItem(&menu, XRCID("ID_K12_NUNAVUT"), _(L"K-12+ (Nunavut)"),
                               wxString{}, wxITEM_CHECK));
    menu.Append(new wxMenuItem(&menu, XRCID("ID_QUEBEC"), _DT(L"Quebec"),
                               wxString{}, wxITEM_CHECK));
    menu.Append(new wxMenuItem(&menu, XRCID("ID_ENGLAND"), _(L"Key stages (England && Wales)"),
                               wxString{}, wxITEM_CHECK));
    }

//-----------------------------------
void ReadabilityApp::FillSaveMenu(wxMenu& saveMenu, const RibbonType rtype)
    {
    assert(rtype != RibbonType::MainFrameRibbon && L"Mainframe should not have a save menu!");

    const auto filterIcon = GetResourceManager().GetSVG(L"ribbon/filter.svg");
    const auto saveIcon = GetResourceManager().GetSVG(L"ribbon/file-save.svg");
    const auto exportAllIcon = GetResourceManager().GetSVG(L"ribbon/export-all.svg");
    const auto reportIcon = GetResourceManager().GetSVG(L"ribbon/report.svg");

    if (rtype == RibbonType::StandardProjectRibbon)
        {
        wxMenuItem* item = new wxMenuItem(&saveMenu, wxID_SAVE, _(L"Save") + L"\tCtrl+S");
        item->SetBitmap(saveIcon);
        saveMenu.Append(item);

        item = new wxMenuItem(&saveMenu, wxID_SAVEAS, _(L"Save As..."));
        item->SetBitmap(saveIcon);
        saveMenu.Append(item);
        saveMenu.AppendSeparator();

        item = new wxMenuItem(&saveMenu, XRCID("ID_SAVE_ITEM"), _(L"Export..."));
        saveMenu.Append(item);

        item = new wxMenuItem(&saveMenu, XRCID("ID_EXPORT_ALL"), _(L"Export All..."));
        item->SetBitmap(exportAllIcon);
        saveMenu.Append(item);
        saveMenu.AppendSeparator();

        item = new wxMenuItem(&saveMenu, XRCID("ID_EXPORT_FILTERED_DOCUMENT"),
                _(L"Export Filtered Document..."));
        item->SetBitmap(filterIcon);
        saveMenu.Append(item);
        }
    else if (rtype == RibbonType::BatchProjectRibbon)
        {
        wxMenuItem* item = new wxMenuItem(&saveMenu, wxID_SAVE, _(L"Save") + L"\tCtrl+S");
        item->SetBitmap(saveIcon);
        saveMenu.Append(item);

        item = new wxMenuItem(&saveMenu, wxID_SAVEAS, _(L"Save As..."));
        item->SetBitmap(saveIcon);
        saveMenu.Append(item);
        saveMenu.AppendSeparator();

        item = new wxMenuItem(&saveMenu, XRCID("ID_SAVE_ITEM"), _(L"Export..."));
        saveMenu.Append(item);

        item = new wxMenuItem(&saveMenu, XRCID("ID_EXPORT_ALL"), _(L"Export All..."));
        item->SetBitmap(exportAllIcon);
        saveMenu.Append(item);
        saveMenu.AppendSeparator();

        item = new wxMenuItem(&saveMenu, XRCID("ID_EXPORT_SCORES_AND_STATISTICS"),
                _(L"Export Scores && Statistics..."));
        item->SetBitmap(reportIcon);
        saveMenu.Append(item);

        item = new wxMenuItem(&saveMenu, XRCID("ID_EXPORT_STATISTICS"),
                _(L"Export Statistics Report..."));
        item->SetBitmap(reportIcon);
        saveMenu.Append(item);
        saveMenu.AppendSeparator();

        item = new wxMenuItem(&saveMenu, XRCID("ID_EXPORT_FILTERED_DOCUMENT"),
                _(L"Export Filtered Document..."));
        item->SetBitmap(filterIcon);
        saveMenu.Append(item);

        item = new wxMenuItem(&saveMenu, XRCID("ID_BATCH_EXPORT_FILTERED_DOCUMENTS"),
                _(L"Batch Export Filtered Document..."));
        item->SetBitmap(filterIcon);
        saveMenu.Append(item);
        }
    }

//-----------------------------------
void ReadabilityApp::FillPrintMenu(wxMenu& printMenu, const RibbonType rtype)
    {
    wxMenuItem* item(nullptr);
    if (rtype != RibbonType::MainFrameRibbon)
        {
        item = new wxMenuItem(&printMenu, wxID_PRINT, _(L"Print...")+_DT(L"\tCtrl+P"));
        item->SetBitmap(GetResourceManager().GetSVG(L"ribbon/print.svg"));
        printMenu.Append(item);
    // macOS's and GTK+'s print dialogs have their own built-in preview option
    #ifdef __WXMSW__
        item = new wxMenuItem(&printMenu, wxID_PREVIEW, _(L"Print Preview..."));
        item->SetBitmap(GetResourceManager().GetSVG(L"ribbon/preview.svg"));
        printMenu.Append(item);
    #endif
        }
    item = new wxMenuItem(&printMenu, wxID_PRINT_SETUP, _(L"Page Setup..."));
    item->SetBitmap(GetResourceManager().GetSVG(L"ribbon/print-setup.svg"));
    printMenu.Append(item);
    item = new wxMenuItem(&printMenu, XRCID("ID_PRINTER_HEADER_FOOTER"), _(L"Headers && Footers..."));
    item->SetBitmap(GetResourceManager().GetSVG(L"ribbon/print-header-footer.svg"));
    printMenu.Append(item);
    }

//-----------------------------------
void ReadabilityApp::UpdateSideBarTheme(Wisteria::UI::SideBar* sidebar)
    {
    SideBarColorScheme colorScheme;
    colorScheme.m_backgroundColor = GetAppOptions().GetSideBarBackgroundColor();
    colorScheme.m_foregroundColor = GetAppOptions().GetSideBarFontColor();
    colorScheme.m_selectedColor = GetAppOptions().GetSideBarActiveColor();
    colorScheme.m_selectedFontColor = GetAppOptions().GetSideBarActiveFontColor();
    colorScheme.m_highlightColor = GetAppOptions().GetSideBarHoverColor();
    colorScheme.m_highlightFontColor = GetAppOptions().GetSideBarHoverFontColor();
    colorScheme.m_parentColor = GetAppOptions().GetSideBarParentColor();

    sidebar->SetColorScheme(colorScheme);
    }

//-----------------------------------
Wisteria::UI::SideBar* ReadabilityApp::CreateSideBar(wxWindow* frame, const wxWindowID id)
    {
    auto sideBar = new SideBar(frame, id);
    sideBar->SetImageList(dynamic_cast<MainFrame*>(GetMainFrame())->GetProjectSideBarImageList());

    UpdateSideBarTheme(sideBar);

    return sideBar;
    }

//-----------------------------------
wxImage ReadabilityApp::ReadRibbonSvgIcon(const wxString& path)
    {
    return GetResourceManager().
        GetSVG(path).
        GetBitmap(GetMainFrame()->FromDIP(wxSize(32, 32))).
        // Hack for icon to work with ribbon; otherwise, the ribbon never resizes
        // the image when the button gets smaller. Ribbon must be relying on some
        // sort of information that converting a bitmap to an image and back performs.
        ConvertToImage();
    }

//-----------------------------------
wxRibbonBar* ReadabilityApp::CreateRibbon(wxWindow* frame, const wxDocument* doc)
    {
    const RibbonType rtype = (doc == nullptr) ? RibbonType::MainFrameRibbon :
        doc->IsKindOf(CLASSINFO(ProjectDoc)) ? RibbonType::StandardProjectRibbon : RibbonType::BatchProjectRibbon;
    // Home tab
    wxRibbonBar* ribbon = new wxRibbonBar(frame, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                          wxRIBBON_BAR_SHOW_PAGE_ICONS|wxRIBBON_BAR_DEFAULT_STYLE);
        {
        wxRibbonPage* homePage = new wxRibbonPage(ribbon, wxID_ANY,
            _(L"Home"),
           GetResourceManager().
           GetSVG(L"ribbon/home.svg").
           GetBitmap(GetMainFrame()->FromDIP(wxSize(16, 16))));
        wxRibbonPanel* projectPanel = new wxRibbonPanel(homePage, wxID_ANY, _(L"Project"), wxNullBitmap,
                                                        wxDefaultPosition, wxDefaultSize,
                                                        wxRIBBON_PANEL_NO_AUTO_MINIMISE);
        wxRibbonButtonBar* projectButtonBar =
            new wxRibbonButtonBar(projectPanel, MainFrame::ID_PROJECT_RIBBON_BUTTON_BAR);
        projectButtonBar->AddHybridButton(wxID_NEW, _(L"New"),
            ReadRibbonSvgIcon(L"ribbon/document.svg"),
            _(L"Create a new project."));
        projectButtonBar->AddHybridButton(wxID_OPEN, _(L"Open"),
            ReadRibbonSvgIcon(L"ribbon/file-open.svg"),
            _(L"Open an existing project."));
        if (rtype == RibbonType::BatchProjectRibbon)
            {
            wxRibbonPanel* documentsPanel = new wxRibbonPanel(homePage, wxID_ANY, _(L"Documents"), wxNullBitmap,
                                                              wxDefaultPosition, wxDefaultSize,
                                                              wxRIBBON_PANEL_NO_AUTO_MINIMISE);
            wxRibbonButtonBar* documentsButtonBar = new wxRibbonButtonBar(documentsPanel);
            documentsButtonBar->AddButton(XRCID("ID_SEND_TO_STANDARD_PROJECT"),
                _(L"Subproject"),
                ReadRibbonSvgIcon(L"ribbon/send-to-subproject.svg"),
                _(L"Create a standard project from the selected document."));
            documentsButtonBar->AddToggleButton(XRCID("ID_STATISTICS_WINDOW"),
                _(L"Statistics"),
                ReadRibbonSvgIcon(L"ribbon/stats.svg"),
                _(L"Display statistics for the selected document."));
            documentsButtonBar->AddButton(XRCID("ID_REMOVE_DOCUMENT"),
                _(L"Remove Document"),
                ReadRibbonSvgIcon(L"ribbon/delete-document.svg"),
                _(L"Remove the selected document from the project."));
            }
        if (rtype != RibbonType::MainFrameRibbon)
            {
            projectButtonBar->AddButton(XRCID("ID_DOCUMENT_REFRESH"),
                _(L"Reload"),
                ReadRibbonSvgIcon(L"ribbon/reload.svg"),
                _(L"Reanalyze the document."));
            if (rtype == RibbonType::StandardProjectRibbon)
                {
                projectButtonBar->AddToggleButton(XRCID("ID_REALTIME_UPDATE"),
                    _(L"Real-time Update"),
                    ReadRibbonSvgIcon(L"ribbon/realtime.svg"),
                    _(L"Automatically reload the project as the source document is edited externally."));
                }
            projectButtonBar->AddDropdownButton(XRCID("ID_SAVE_OPTIONS"),
                _(L"Save"),
                ReadRibbonSvgIcon(L"ribbon/file-save.svg"),
                _(L"Save the project."));
            projectButtonBar->AddHybridButton(wxID_PRINT,
                _(L"Print"),
                ReadRibbonSvgIcon(L"ribbon/print.svg"),
                _(L"Print the selected window."));
            projectButtonBar->AddButton(wxID_PROPERTIES,
                _(L"Properties"),
                ReadRibbonSvgIcon(L"ribbon/project-settings.svg"),
                _(L"Change the settings for this project."));
            projectButtonBar->AddToggleButton(XRCID("ID_SHOW_SIDEBAR"),
                _(L"Sidebar"),
                ReadRibbonSvgIcon(L"ribbon/toggle-sidebar.svg"),
                _(L"Shows or hides the sidebar."));
            projectButtonBar->ToggleButton(XRCID("ID_SHOW_SIDEBAR"), true);

            // edit sections
            //--------------
            // list button edit panel (Copy, Select, View, Sort)
                {
                wxRibbonPanel* editPanel =
                    new wxRibbonPanel(homePage, MainFrame::ID_EDIT_RIBBON_LIST_PANEL,
                        _(L"Edit"), wxNullBitmap,
                    wxDefaultPosition, wxDefaultSize,
                    wxRIBBON_PANEL_NO_AUTO_MINIMISE);

                wxRibbonButtonBar* editButtonBar =
                    new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

                editButtonBar->AddHybridButton(wxID_COPY, _(L"Copy"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/copy.svg"),
                    _(L"Copy the selected row(s)."));
                editButtonBar->AddButton(wxID_SELECTALL,
                    _(L"Select All"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/select-all.svg"),
                    _(L"Select All"));
                editButtonBar->AddButton(XRCID("ID_VIEW_ITEM"),
                    _(L"View Item"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/view-spreadsheet-item.svg"),
                    _(L"View the selected row in tabular format."));
                editButtonBar->AddButton(XRCID("ID_LIST_SORT"),
                    _(L"Sort"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/sort.svg"),
                    _(L"Sort the list."));
                }
            // list button edit panel (Long Format, Grade Scales, Copy, Select, View, Sort)
                {
                wxRibbonPanel* editPanel =
                    new wxRibbonPanel(homePage, MainFrame::ID_EDIT_RIBBON_LIST_TEST_SCORES_PANEL,
                        _(L"Edit"), wxNullBitmap,
                    wxDefaultPosition, wxDefaultSize,
                    wxRIBBON_PANEL_NO_AUTO_MINIMISE);

                wxRibbonButtonBar* editButtonBar =
                    new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

                editButtonBar->AddToggleButton(XRCID("ID_LONG_FORMAT"),
                    _(L"Long Format"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/grade-display.svg"),
                    _(L"Display scores in long format."));
                editButtonBar->AddDropdownButton(XRCID("ID_GRADE_SCALES"),
                    _(L"Grade Scale"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/education.svg"),
                    _(L"Change the grade scale display of the scores."));
                editButtonBar->AddHybridButton(wxID_COPY,
                    _(L"Copy"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/copy.svg"),
                    _(L"Copy the selected row(s)."));
                editButtonBar->AddButton(wxID_SELECTALL,
                    _(L"Select All"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/select-all.svg"),
                    _(L"Select All"));
                editButtonBar->AddButton(XRCID("ID_VIEW_ITEM"),
                    _(L"View Item"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/view-spreadsheet-item.svg"),
                    _(L"View the selected row in tabular format."));
                editButtonBar->AddButton(XRCID("ID_LIST_SORT"),
                    _(L"Sort"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/sort.svg"),
                    _(L"Sort the list."));
                }
             // list button edit panel (Copy, Select, View, Sort, Sum)
                {
                wxRibbonPanel* editPanel =
                    new wxRibbonPanel(homePage, MainFrame::ID_EDIT_RIBBON_LIST_CSVSS_PANEL,
                        _(L"Edit"), wxNullBitmap,
                    wxDefaultPosition, wxDefaultSize,
                    wxRIBBON_PANEL_NO_AUTO_MINIMISE);

                wxRibbonButtonBar* editButtonBar =
                    new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

                editButtonBar->AddHybridButton(wxID_COPY,
                    _(L"Copy"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/copy.svg"),
                    _(L"Copy the selected row(s)."));
                editButtonBar->AddButton(wxID_SELECTALL,
                    _(L"Select All"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/select-all.svg"),
                    _(L"Select All"));
                editButtonBar->AddButton(XRCID("ID_VIEW_ITEM"),
                    _(L"View Item"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/view-spreadsheet-item.svg"),
                    _(L"View the selected row in tabular format."));
                editButtonBar->AddButton(XRCID("ID_LIST_SORT"),
                    _(L"Sort"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/sort.svg"),
                    _(L"Sort the list."));
                editButtonBar->AddButton(XRCID("ID_SUMMATION"),
                    _(L"Sum"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/sum.svg"),
                    _(L"Total the values from the selected column."));
                }
            // list button edit panel (Copy, Select, Sort)
                {
                wxRibbonPanel* editPanel =
                    new wxRibbonPanel(homePage, MainFrame::ID_EDIT_RIBBON_LIST_SIMPLE_PANEL,
                        _(L"Edit"), wxNullBitmap,
                    wxDefaultPosition, wxDefaultSize,
                    wxRIBBON_PANEL_NO_AUTO_MINIMISE);

                wxRibbonButtonBar* editButtonBar =
                    new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

                editButtonBar->AddHybridButton(wxID_COPY, _(L"Copy"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/copy.svg"),
                    _(L"Copy"));
                editButtonBar->AddButton(wxID_SELECTALL,
                    _(L"Select All"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/select-all.svg"),
                    _(L"Select All"));
                editButtonBar->AddButton(XRCID("ID_LIST_SORT"),
                        _(L"Sort"),
                        wxGetApp().ReadRibbonSvgIcon(L"ribbon/sort.svg"),
                        _(L"Sort the list."));
                }
            // list button edit panel (Copy, Select, Sort, Sum)
                {
                wxRibbonPanel* editPanel =
                    new wxRibbonPanel(homePage, MainFrame::ID_EDIT_RIBBON_LIST_SIMPLE_WITH_SUM_PANEL,
                        _(L"Edit"), wxNullBitmap,
                    wxDefaultPosition, wxDefaultSize,
                    wxRIBBON_PANEL_NO_AUTO_MINIMISE);

                wxRibbonButtonBar* editButtonBar =
                    new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

                editButtonBar->AddHybridButton(wxID_COPY, _(L"Copy"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/copy.svg"),
                    _(L"Copy"));
                editButtonBar->AddButton(wxID_SELECTALL,
                    _(L"Select All"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/select-all.svg"),
                    _(L"Select All"));
                editButtonBar->AddButton(XRCID("ID_LIST_SORT"),
                    _(L"Sort"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/sort.svg"),
                    _(L"Sort the list."));
                editButtonBar->AddButton(XRCID("ID_SUMMATION"),
                    _(L"Sum"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/sum.svg"),
                    _(L"Total the values from the selected column."));
                }
            // list button edit panel (Copy, Select, Exclude, Sum, Sort)
                {
                wxRibbonPanel* editPanel =
                    new wxRibbonPanel(homePage, MainFrame::ID_EDIT_RIBBON_LIST_SIMPLE_WITH_SUM_AND_EXCLUDE_PANEL,
                        _(L"Edit"), wxNullBitmap,
                        wxDefaultPosition, wxDefaultSize,
                        wxRIBBON_PANEL_NO_AUTO_MINIMISE);

                wxRibbonButtonBar* editButtonBar =
                    new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

                editButtonBar->AddHybridButton(wxID_COPY, _(L"Copy"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/copy.svg"),
                    _(L"Copy"));
                editButtonBar->AddButton(wxID_SELECTALL,
                    _(L"Select All"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/select-all.svg"),
                    _(L"Select All"));
                editButtonBar->AddButton(XRCID("ID_LIST_SORT"),
                    _(L"Sort"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/sort.svg"),
                    _(L"Sort the list."));
                editButtonBar->AddButton(XRCID("ID_EXCLUDE_SELECTED"),
                    _(L"Exclude Selected"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/exclude-selected.svg"),
                    _(L"Exclude selected word(s)."));
                editButtonBar->AddButton(XRCID("ID_SUMMATION"),
                    _(L"Sum"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/sum.svg"),
                    _(L"Total the values from the selected column."));
                }
            // HTML window
                {
                wxRibbonPanel* editPanel =
                    new wxRibbonPanel(homePage, MainFrame::ID_EDIT_RIBBON_SUMMARY_REPORT_PANEL,
                        _(L"Edit"), wxNullBitmap,
                        wxDefaultPosition, wxDefaultSize,
                        wxRIBBON_PANEL_NO_AUTO_MINIMISE);

                wxRibbonButtonBar* editButtonBar =
                    new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

                editButtonBar->AddButton(wxID_COPY, _(L"Copy"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/copy.svg"),
                    _(L"Copy the report."));
                }
            // explanation list control
                {
                wxRibbonPanel* editPanel =
                    new wxRibbonPanel(homePage, MainFrame::ID_EDIT_RIBBON_EXPLANATION_LIST_PANEL,
                        _(L"Edit"), wxNullBitmap,
                        wxDefaultPosition, wxDefaultSize,
                        wxRIBBON_PANEL_NO_AUTO_MINIMISE);

                wxRibbonButtonBar* editButtonBar =
                    new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

                editButtonBar->AddToggleButton(XRCID("ID_LONG_FORMAT"),
                    _(L"Long Format"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/grade-display.svg"),
                    _(L"Display scores in long format."));
                editButtonBar->AddDropdownButton(XRCID("ID_GRADE_SCALES"),
                    _(L"Grade Scale"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/education.svg"),
                    _(L"Change the grade scale display of the scores."));
                editButtonBar->AddButton(wxID_COPY, _(L"Copy"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/copy.svg"),
                    _(L"Copy the selected row(s)."));
                editButtonBar->AddButton(XRCID("ID_LIST_SORT"),
                    _(L"Sort"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/sort.svg"),
                    _(L"Sort the list."));
                }
            // formatted text control
                {
                wxRibbonPanel* editPanel =
                    new wxRibbonPanel(homePage, MainFrame::ID_EDIT_RIBBON_REPORT_PANEL,
                        _(L"Edit"), wxNullBitmap,
                        wxDefaultPosition, wxDefaultSize,
                        wxRIBBON_PANEL_NO_AUTO_MINIMISE);

                wxRibbonButtonBar* editButtonBar =
                    new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

                editButtonBar->AddButton(XRCID("ID_TEXT_WINDOW_FONT"), _(L"Font"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/font.svg"),
                    _(L"Change the font."));
                editButtonBar->AddButton(XRCID("ID_TEXT_WINDOW_COLORS"),
                    _(L"Highlight"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/highlighting.svg"),
                    _(L"Change the highlight colors."));
                editButtonBar->AddButton(wxID_COPY, _(L"Copy"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/copy.svg"),
                    _(L"Copy"));
                editButtonBar->AddButton(wxID_SELECTALL, _(L"Select All"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/select-all.svg"),
                    _(L"Select All"));
                }
            // statistics list report in a standard project
                {
                wxRibbonPanel* editPanel =
                    new wxRibbonPanel(homePage, MainFrame::ID_EDIT_RIBBON_STATS_LIST_PANEL,
                        _(L"Edit"), wxNullBitmap,
                        wxDefaultPosition, wxDefaultSize,
                        wxRIBBON_PANEL_NO_AUTO_MINIMISE);

                wxRibbonButtonBar* editButtonBar =
                    new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

                editButtonBar->AddButton(XRCID("ID_EDIT_STATS_REPORT"),
                    _(L"Edit Report"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/edit-report.svg"),
                    _(L"Select which statistics to include in the report."));
                editButtonBar->AddHybridButton(wxID_COPY,
                    _(L"Copy"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/copy.svg"),
                    _(L"Copy the selected row(s)."));
                editButtonBar->AddButton(wxID_SELECTALL,
                    _(L"Select All"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/select-all.svg"),
                    _(L"Select All"));
                editButtonBar->AddButton(XRCID("ID_VIEW_ITEM"),
                    _(L"View Item"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/view-spreadsheet-item.svg"),
                    _(L"View the selected row in tabular format."));
                editButtonBar->AddButton(XRCID("ID_LIST_SORT"),
                    _(L"Sort"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/sort.svg"),
                    _(L"Sort the list."));
                }
            // statistics HTML report in a standard project
                {
                wxRibbonPanel* editPanel =
                    new wxRibbonPanel(homePage, MainFrame::ID_EDIT_RIBBON_STATS_SUMMARY_REPORT_PANEL,
                        _(L"Edit"), wxNullBitmap,
                        wxDefaultPosition, wxDefaultSize,
                        wxRIBBON_PANEL_NO_AUTO_MINIMISE);

                wxRibbonButtonBar* editButtonBar =
                    new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

                editButtonBar->AddButton(XRCID("ID_EDIT_STATS_REPORT"),
                    _(L"Edit Report"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/edit-report.svg"),
                    _(L"Select which statistics to include in the report."));
                editButtonBar->AddButton(wxID_COPY,
                    _(L"Copy"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/copy.svg"),
                    _(L"Copy"));
                }
            // bar chart panel
                {
                wxRibbonPanel* editPanel =
                    new wxRibbonPanel(homePage, MainFrame::ID_EDIT_RIBBON_BAR_CHART_PANEL,
                        _(L"Edit"), wxNullBitmap,
                        wxDefaultPosition, wxDefaultSize,
                        wxRIBBON_PANEL_NO_AUTO_MINIMISE);

                wxRibbonButtonBar* editButtonBar =
                    new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

                editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_BACKGROUND"),
                    _(L"Background"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/photos.svg"),
                    _(L"Set the graph's background."));
                editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_FONTS"),
                    _(L"Font"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/font.svg"),
                    _(L"Change the graph's fonts."));
                editButtonBar->AddButton(XRCID("ID_EDIT_WATERMARK"),
                    _(L"Watermark"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/watermark.svg"),
                    _(L"Add a watermark to the graph."));
                editButtonBar->AddButton(XRCID("ID_EDIT_LOGO"),
                    _(L"Logo"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/logo.svg"),
                    _(L"Add a logo to the graph."));
                editButtonBar->AddToggleButton(XRCID("ID_DROP_SHADOW"),
                    _(L"Shadows"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/shadow.svg"),
                    _(L"Display drop shadows on the graphs."));
                editButtonBar->AddDropdownButton(XRCID("ID_EDIT_BAR_STYLE"),
                    _(L"Bar Style"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/bar-top-to-bottom.svg"),
                    _(L"Changes the bar appearance."));
                editButtonBar->AddToggleButton(XRCID("ID_EDIT_BAR_LABELS"),
                    _(L"Labels"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/bar-labels.svg"),
                    _(L"Shows or hides the bars' labels."));
                editButtonBar->AddDropdownButton(XRCID("ID_EDIT_BAR_ORIENTATION"),
                    _(L"Orientation"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/axis-orientation.svg"),
                    _(L"Changes the axis orientation."));
                editButtonBar->AddDropdownButton(XRCID("ID_GRAPH_SORT"),
                    _(L"Sort"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/bar-sort.svg"),
                    _(L"Sort the bars in the graph."));
                editButtonBar->AddButton(wxID_COPY, _(L"Copy"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/copy.svg"),
                    _(L"Copy"));
                }
            // box plot panel
                {
                wxRibbonPanel* editPanel =
                    new wxRibbonPanel(homePage, MainFrame::ID_EDIT_RIBBON_BOX_PLOT_PANEL,
                        _(L"Edit"), wxNullBitmap,
                        wxDefaultPosition, wxDefaultSize,
                        wxRIBBON_PANEL_NO_AUTO_MINIMISE);

                wxRibbonButtonBar* editButtonBar =
                    new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

                editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_BACKGROUND"),
                    _(L"Background"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/photos.svg"),
                    _(L"Set the graph's background."));
                editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_FONTS"),
                    _(L"Font"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/font.svg"),
                    _(L"Change the graph's fonts."));
                editButtonBar->AddButton(XRCID("ID_EDIT_WATERMARK"),
                    _(L"Watermark"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/watermark.svg"),
                    _(L"Add a watermark to the graph."));
                editButtonBar->AddButton(XRCID("ID_EDIT_LOGO"),
                    _(L"Logo"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/logo.svg"),
                    _(L"Add a logo to the graph."));
                editButtonBar->AddToggleButton(XRCID("ID_DROP_SHADOW"),
                    _(L"Shadows"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/shadow.svg"),
                    _(L"Display drop shadows on the graphs."));
                editButtonBar->AddDropdownButton(XRCID("ID_EDIT_BOX_STYLE"),
                    _(L"Box Style"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/bar-top-to-bottom.svg"),
                    _(L"Changes the box appearance."));
                editButtonBar->AddToggleButton(XRCID("ID_BOX_PLOT_DISPLAY_LABELS"),
                    _(L"Display Labels"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/show-all-labels.svg"),
                    _(L"Displays labels on the box and whiskers."));
                editButtonBar->AddToggleButton(XRCID("ID_BOX_PLOT_DISPLAY_ALL_POINTS"),
                    _(L"Display Points"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/show-all-points.svg"),
                    _(L"Displays all data points onto the plot."));
                editButtonBar->AddButton(wxID_COPY, _(L"Copy"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/copy.svg"),
                    _(L"Copy the graph."));
                editButtonBar->AddHybridButton(wxID_ZOOM_IN,
                    _(L"Zoom"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/zoom-in.svg"),
                    _(L"Zoom"));
                }
            // histogram panel
                {
                wxRibbonPanel* editPanel =
                    new wxRibbonPanel(homePage, MainFrame::ID_EDIT_RIBBON_HISTOGRAM_PANEL,
                        _(L"Edit"), wxNullBitmap,
                        wxDefaultPosition, wxDefaultSize,
                        wxRIBBON_PANEL_NO_AUTO_MINIMISE);

                wxRibbonButtonBar* editButtonBar =
                    new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

                editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_BACKGROUND"),
                    _(L"Background"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/photos.svg"),
                    _(L"Set the graph's background."));
                editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_FONTS"),
                    _(L"Font"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/font.svg"),
                    _(L"Change the graph's fonts."));
                editButtonBar->AddButton(XRCID("ID_EDIT_WATERMARK"),
                    _(L"Watermark"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/watermark.svg"),
                    _(L"Add a watermark to the graph."));
                editButtonBar->AddButton(XRCID("ID_EDIT_LOGO"),
                    _(L"Logo"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/logo.svg"),
                    _(L"Add a logo to the graph."));
                editButtonBar->AddToggleButton(XRCID("ID_DROP_SHADOW"),
                    _(L"Shadows"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/shadow.svg"),
                    _(L"Display drop shadows on the graphs."));
                editButtonBar->AddDropdownButton(XRCID("ID_EDIT_HISTOGRAM_BAR_STYLE"),
                    _(L"Bar Style"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/bar-top-to-bottom.svg"),
                    _(L"Changes the bar appearance."));
                editButtonBar->AddDropdownButton(XRCID("ID_EDIT_HISTOBAR_LABELS"),
                    _(L"Labels"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/bar-labels.svg"),
                    _(L"Changes what is displayed on the bars' labels."));
                editButtonBar->AddButton(wxID_COPY, _(L"Copy"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/copy.svg"),
                    _(L"Copy the graph."));
                editButtonBar->AddHybridButton(wxID_ZOOM_IN,
                    _(L"Zoom"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/zoom-in.svg"),
                    _(L"Zoom"));
                }
            // batch project histogram panel
                {
                wxRibbonPanel* editPanel =
                    new wxRibbonPanel(homePage, MainFrame::ID_EDIT_RIBBON_HISTOGRAM_BATCH_PANEL,
                        _(L"Edit"), wxNullBitmap,
                        wxDefaultPosition, wxDefaultSize,
                        wxRIBBON_PANEL_NO_AUTO_MINIMISE);

                wxRibbonButtonBar* editButtonBar =
                    new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

                editButtonBar->AddButton(XRCID("ID_EDIT_GRAPH_COLOR_SCHEME"), _(L"Colors"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/color-wheel.svg"),
                    _(L"Select the color scheme for the grouped histogram."));
                editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_BACKGROUND"),
                    _(L"Background"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/photos.svg"),
                    _(L"Set the graph's background."));
                editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_FONTS"),
                    _(L"Font"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/font.svg"),
                    _(L"Change the graph's fonts."));
                editButtonBar->AddButton(XRCID("ID_EDIT_WATERMARK"),
                    _(L"Watermark"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/watermark.svg"),
                    _(L"Add a watermark to the graph."));
                editButtonBar->AddButton(XRCID("ID_EDIT_LOGO"),
                    _(L"Logo"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/logo.svg"),
                    _(L"Add a logo to the graph."));
                editButtonBar->AddToggleButton(XRCID("ID_DROP_SHADOW"),
                    _(L"Shadows"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/shadow.svg"),
                    _(L"Display drop shadows on the graphs."));
                editButtonBar->AddDropdownButton(XRCID("ID_GRADE_SCALES"),
                    _(L"Grade Scale"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/education.svg"),
                    _(L"Change the grade scale display of the scores."));
                editButtonBar->AddDropdownButton(XRCID("ID_EDIT_HISTOGRAM_BAR_STYLE"),
                    _(L"Bar Style"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/bar-top-to-bottom.svg"),
                    _(L"Changes the bar appearance."));
                editButtonBar->AddDropdownButton(XRCID("ID_EDIT_HISTOBAR_LABELS"),
                    _(L"Labels"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/bar-labels.svg"),
                    _(L"Changes what is displayed on the bars' labels."));
                editButtonBar->AddButton(wxID_COPY, _(L"Copy"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/copy.svg"),
                    _(L"Copy the graph."));
                editButtonBar->AddHybridButton(wxID_ZOOM_IN,
                    _(L"Zoom"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/zoom-in.svg"),
                    _(L"Zoom"));
                }
            // wordcloud panel
                {
                wxRibbonPanel* editPanel =
                    new wxRibbonPanel(homePage, MainFrame::ID_EDIT_RIBBON_WORDCLOUD_PANEL,
                        _(L"Edit"), wxNullBitmap,
                        wxDefaultPosition, wxDefaultSize,
                        wxRIBBON_PANEL_NO_AUTO_MINIMISE);

                wxRibbonButtonBar* editButtonBar =
                    new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

                editButtonBar->AddButton(XRCID("ID_EDIT_GRAPH_COLOR_SCHEME"), _(L"Colors"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/color-wheel.svg"),
                    _(L"Select the color scheme for the pie chart."));

                editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_BACKGROUND"),
                    _(L"Background"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/photos.svg"),
                    _(L"Set the graph's background."));
                editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_FONTS"),
                    _(L"Font"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/font.svg"),
                    _(L"Change the graph's fonts."));
                editButtonBar->AddButton(XRCID("ID_EDIT_WATERMARK"),
                    _(L"Watermark"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/watermark.svg"),
                    _(L"Add a watermark to the graph."));
                editButtonBar->AddButton(XRCID("ID_EDIT_LOGO"),
                    _(L"Logo"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/logo.svg"),
                    _(L"Add a logo to the graph."));
                editButtonBar->AddToggleButton(XRCID("ID_DROP_SHADOW"),
                    _(L"Shadows"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/shadow.svg"),
                    _(L"Display drop shadows on the graphs."));

                editButtonBar->AddButton(wxID_COPY,
                    _(L"Copy"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/copy.svg"),
                    _(L"Copy the graph."));
                editButtonBar->AddHybridButton(wxID_ZOOM_IN,
                    _(L"Zoom"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/zoom-in.svg"),
                    _(L"Zoom"));
                }
            // pie chart panel
                {
                wxRibbonPanel* editPanel =
                    new wxRibbonPanel(homePage, MainFrame::ID_EDIT_RIBBON_PIE_CHART_PANEL,
                        _(L"Edit"), wxNullBitmap,
                        wxDefaultPosition, wxDefaultSize,
                        wxRIBBON_PANEL_NO_AUTO_MINIMISE);

                wxRibbonButtonBar* editButtonBar =
                    new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

                editButtonBar->AddButton(XRCID("ID_EDIT_GRAPH_COLOR_SCHEME"), _(L"Colors"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/color-wheel.svg"),
                    _(L"Select the color scheme for the pie chart."));
                editButtonBar->AddToggleButton(
                    XRCID("ID_EDIT_GRAPH_SHOWCASE_COMPLEX_WORDS"), _(L"Showcase Complexity"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/showcase.svg"),
                    _(L"Toggle whether complex word slices are being showcased."));

                editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_BACKGROUND"),
                    _(L"Background"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/photos.svg"),
                    _(L"Set the graph's background."));
                editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_FONTS"),
                    _(L"Font"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/font.svg"),
                    _(L"Change the graph's fonts."));
                editButtonBar->AddButton(XRCID("ID_EDIT_WATERMARK"),
                    _(L"Watermark"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/watermark.svg"),
                    _(L"Add a watermark to the graph."));
                editButtonBar->AddButton(XRCID("ID_EDIT_LOGO"),
                    _(L"Logo"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/logo.svg"),
                    _(L"Add a logo to the graph."));
                editButtonBar->AddToggleButton(XRCID("ID_DROP_SHADOW"),
                    _(L"Shadows"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/shadow.svg"),
                    _(L"Display drop shadows on the graphs."));

                editButtonBar->AddButton(wxID_COPY,
                    _(L"Copy"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/copy.svg"),
                    _(L"Copy the graph."));
                editButtonBar->AddHybridButton(wxID_ZOOM_IN,
                    _(L"Zoom"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/zoom-in.svg"),
                    _(L"Zoom"));
                }
            // graph panel
                {
                wxRibbonPanel* editPanel =
                    new wxRibbonPanel(homePage, MainFrame::ID_EDIT_RIBBON_GRAPH_PANEL,
                        _(L"Edit"), wxNullBitmap,
                        wxDefaultPosition, wxDefaultSize,
                        wxRIBBON_PANEL_NO_AUTO_MINIMISE);

                wxRibbonButtonBar* editButtonBar =
                    new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

                editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_BACKGROUND"),
                    _(L"Background"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/photos.svg"),
                    _(L"Set the graph's background."));
                editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_FONTS"),
                    _(L"Font"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/font.svg"),
                    _(L"Change the graph's fonts."));
                editButtonBar->AddButton(XRCID("ID_EDIT_WATERMARK"),
                    _(L"Watermark"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/watermark.svg"),
                    _(L"Add a watermark to the graph."));
                editButtonBar->AddButton(XRCID("ID_EDIT_LOGO"),
                    _(L"Logo"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/logo.svg"),
                    _(L"Add a logo to the graph."));
                editButtonBar->AddToggleButton(XRCID("ID_DROP_SHADOW"),
                    _(L"Shadows"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/shadow.svg"),
                    _(L"Display drop shadows on the graphs."));

                editButtonBar->AddButton(wxID_COPY,
                    _(L"Copy"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/copy.svg"),
                    _(L"Copy the graph."));
                editButtonBar->AddHybridButton(wxID_ZOOM_IN,
                    _(L"Zoom"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/zoom-in.svg"),
                    _(L"Zoom"));
                }
            // Lix (German) panel
                {
                wxRibbonPanel* editPanel =
                    new wxRibbonPanel(homePage, MainFrame::ID_EDIT_RIBBON_LIX_GERMAN_PANEL,
                        _(L"Edit"), wxNullBitmap,
                        wxDefaultPosition, wxDefaultSize,
                        wxRIBBON_PANEL_NO_AUTO_MINIMISE);

                wxRibbonButtonBar* editButtonBar =
                    new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

                editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_BACKGROUND"), _(L"Background"),
                                                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/photos.svg"),
                                                    _(L"Set the graph's background."));
                editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_FONTS"), _(L"Font"),
                                                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/font.svg"),
                                                    _(L"Change the graph's fonts."));
                editButtonBar->AddButton(XRCID("ID_EDIT_WATERMARK"), _(L"Watermark"),
                                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/watermark.svg"),
                                            _(L"Add a watermark to the graph."));
                editButtonBar->AddButton(XRCID("ID_EDIT_LOGO"), _(L"Logo"),
                                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/logo.svg"),
                                            _(L"Add a logo to the graph."));
                editButtonBar->AddToggleButton(XRCID("ID_DROP_SHADOW"), _(L"Shadows"),
                                                wxGetApp().ReadRibbonSvgIcon(L"ribbon/shadow.svg"),
                                                _(L"Display drop shadows on the graphs."));
                editButtonBar->AddToggleButton(XRCID("ID_USE_ENGLISH_LABELS"),
                    _(L"English Labels"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/german2english.svg"),
                    _(L"Use translated (English) labels for the brackets."));
                editButtonBar->AddButton(wxID_COPY, _(L"Copy"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/copy.svg"),
                    _(L"Copy the graph."));
                editButtonBar->AddHybridButton(wxID_ZOOM_IN, _(L"Zoom"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/zoom-in.svg"),
                    _(L"Zoom"));
                }
            // Raygor panel
                {
                wxRibbonPanel* editPanel =
                    new wxRibbonPanel(homePage, MainFrame::ID_EDIT_RIBBON_RAYGOR_PANEL,
                        _(L"Edit"), wxNullBitmap,
                        wxDefaultPosition, wxDefaultSize,
                        wxRIBBON_PANEL_NO_AUTO_MINIMISE);

                wxRibbonButtonBar* editButtonBar =
                    new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

                editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_BACKGROUND"), _(L"Background"),
                                                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/photos.svg"),
                                                    _(L"Set the graph's background."));
                editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_FONTS"), _(L"Font"),
                                                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/font.svg"),
                                                    _(L"Change the graph's fonts."));
                editButtonBar->AddButton(XRCID("ID_EDIT_WATERMARK"), _(L"Watermark"),
                                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/watermark.svg"),
                                            _(L"Add a watermark to the graph."));
                editButtonBar->AddButton(XRCID("ID_EDIT_LOGO"), _(L"Logo"),
                                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/logo.svg"),
                                            _(L"Add a logo to the graph."));
                editButtonBar->AddToggleButton(XRCID("ID_DROP_SHADOW"), _(L"Shadows"),
                                                wxGetApp().ReadRibbonSvgIcon(L"ribbon/shadow.svg"),
                                                _(L"Display drop shadows on the graphs."));
                editButtonBar->AddButton(XRCID("ID_INVALID_REGION_COLOR"),
                    _(L"Invalid Region"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/invalid-region.svg"),
                    _(L"Change the color of the invalid regions."));
                editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_RAYGOR_STYLE"),
                    _(L"Raygor Style"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/raygor-style.svg"),
                    _(L"Change the layout style of the Raygor graph."));
                editButtonBar->AddButton(wxID_COPY, _(L"Copy"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/copy.svg"),
                    _(L"Copy the graph."));
                editButtonBar->AddHybridButton(wxID_ZOOM_IN, _(L"Zoom"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/zoom-in.svg"),
                    _(L"Zoom"));
                }
             // Fry panel
                {
                wxRibbonPanel* editPanel =
                    new wxRibbonPanel(homePage, MainFrame::ID_EDIT_RIBBON_FRY_PANEL,
                        _(L"Edit"), wxNullBitmap,
                        wxDefaultPosition, wxDefaultSize,
                        wxRIBBON_PANEL_NO_AUTO_MINIMISE);

                wxRibbonButtonBar* editButtonBar =
                    new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

                editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_BACKGROUND"), _(L"Background"),
                                                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/photos.svg"),
                                                    _(L"Set the graph's background."));
                editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_FONTS"), _(L"Font"),
                                                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/font.svg"),
                                                    _(L"Change the graph's fonts."));
                editButtonBar->AddButton(XRCID("ID_EDIT_WATERMARK"), _(L"Watermark"),
                                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/watermark.svg"),
                                            _(L"Add a watermark to the graph."));
                editButtonBar->AddButton(XRCID("ID_EDIT_LOGO"), _(L"Logo"),
                                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/logo.svg"),
                                            _(L"Add a logo to the graph."));
                editButtonBar->AddToggleButton(XRCID("ID_DROP_SHADOW"), _(L"Shadows"),
                                                wxGetApp().ReadRibbonSvgIcon(L"ribbon/shadow.svg"),
                                                _(L"Display drop shadows on the graphs."));
                editButtonBar->AddButton(XRCID("ID_INVALID_REGION_COLOR"),
                    _(L"Invalid Region"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/invalid-region.svg"),
                    _(L"Change the color of the invalid regions."));
                editButtonBar->AddButton(wxID_COPY, _(L"Copy"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/copy.svg"),
                    _(L"Copy the graph."));
                editButtonBar->AddHybridButton(wxID_ZOOM_IN, _(L"Zoom"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/zoom-in.svg"),
                    _(L"Zoom"));
                }
            // Flesch panel
                {
                wxRibbonPanel* editPanel =
                    new wxRibbonPanel(homePage, MainFrame::ID_EDIT_RIBBON_FLESCH_PANEL,
                        _(L"Edit"), wxNullBitmap,
                        wxDefaultPosition, wxDefaultSize,
                        wxRIBBON_PANEL_NO_AUTO_MINIMISE);

                wxRibbonButtonBar* editButtonBar =
                    new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

                editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_BACKGROUND"), _(L"Background"),
                                                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/photos.svg"),
                                                    _(L"Set the graph's background."));
                editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_FONTS"), _(L"Font"),
                                                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/font.svg"),
                                                    _(L"Change the graph's fonts."));
                editButtonBar->AddButton(XRCID("ID_EDIT_WATERMARK"), _(L"Watermark"),
                                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/watermark.svg"),
                                            _(L"Add a watermark to the graph."));
                editButtonBar->AddButton(XRCID("ID_EDIT_LOGO"), _(L"Logo"),
                                            wxGetApp().ReadRibbonSvgIcon(L"ribbon/logo.svg"),
                                            _(L"Add a logo to the graph."));
                editButtonBar->AddToggleButton(XRCID("ID_DROP_SHADOW"), _(L"Shadows"),
                                                wxGetApp().ReadRibbonSvgIcon(L"ribbon/shadow.svg"),
                                                _(L"Display drop shadows on the graphs."));
                editButtonBar->AddToggleButton(XRCID("ID_FLESCH_DISPLAY_LINES"),
                    _(L"Connect Points"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/flesch-line.svg"),
                    _(L"Display the line through the factors and score."));
                editButtonBar->AddButton(wxID_COPY, _(L"Copy"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/copy.svg"),
                    _(L"Copy the graph."));
                editButtonBar->AddHybridButton(wxID_ZOOM_IN, _(L"Zoom"),
                    wxGetApp().ReadRibbonSvgIcon(L"ribbon/zoom-in.svg"),
                    _(L"Zoom"));
                }
            }
        else // rtype == RibbonType::MainFrameRibbon
            {
            // settings section
            wxRibbonPanel* settingsPanel =
                new wxRibbonPanel(homePage, wxID_ANY, _(L"Settings"), wxNullBitmap, wxDefaultPosition,
                                  wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
            wxRibbonButtonBar* settingsButtonBar = new wxRibbonButtonBar(settingsPanel);
            settingsButtonBar->AddDropdownButton(XRCID("ID_PRINT_OPTIONS"),
                _(L"Printing"),
                ReadRibbonSvgIcon(L"ribbon/print.svg"),
                _(L"Change print settings."));
            settingsButtonBar->AddDropdownButton(XRCID("ID_EDIT_DICTIONARY"), _(L"Spell Checker"),
                ReadRibbonSvgIcon(L"ribbon/misspellings.svg"),
                _(L"Edit the spell checker's dictionary."));
            settingsButtonBar->AddButton(wxID_PREFERENCES, _(L"Options"),
                ReadRibbonSvgIcon(L"ribbon/configure.svg"),
                _(L"Change the program's general options."));
            // test section
            wxRibbonPanel* readabilityTestsPanel =
                new wxRibbonPanel(homePage, wxID_ANY, _(L"Readability Tests"), wxNullBitmap, wxDefaultPosition,
                                  wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
            wxRibbonButtonBar* readabilityTestsBar = new wxRibbonButtonBar(readabilityTestsPanel);
            readabilityTestsBar->AddDropdownButton(XRCID("ID_CUSTOM_TESTS"), _(L"Custom"),
                ReadRibbonSvgIcon(L"ribbon/formula.svg"),
                _(L"Create or edit custom tests."));
            readabilityTestsBar->AddDropdownButton(XRCID("ID_TEST_BUNDLES"),
                _(L"Bundles"),
                ReadRibbonSvgIcon(L"ribbon/bundles.svg"),
                _(L"Add multiple tests to a project at once."));
            readabilityTestsBar->AddButton(XRCID("ID_TESTS_OVERVIEW"), _(L"Tests Overview"),
                ReadRibbonSvgIcon(L"ribbon/tests-overview.svg"),
                _(L"View information about each readability test."));
            readabilityTestsBar->AddHybridButton(XRCID("ID_WORD_LISTS"), _(L"Word Lists"),
                ReadRibbonSvgIcon(L"tests/dale-chall-test.svg"),
                _(L"View the word lists used by readability tests."));
            readabilityTestsBar->AddDropdownButton(XRCID("ID_BLANK_GRAPHS"),
                _(L"Blank Graphs"),
                ReadRibbonSvgIcon(L"ribbon/blank-graphs.svg"),
                _(L"Print or save blank readability graph templates."));
            // tools section
            wxRibbonPanel* toolsPanel =
                new wxRibbonPanel(homePage, wxID_ANY, _(L"Tools"), wxNullBitmap, wxDefaultPosition,
                                  wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
            wxRibbonButtonBar* toolButtonBar = new wxRibbonButtonBar(toolsPanel);
            toolButtonBar->AddButton(XRCID("ID_WEB_HARVEST"), _(L"Web Harvester"),
                ReadRibbonSvgIcon(L"ribbon/web-export.svg"),
                _(L"Download and analyze multiple webpages."));
    #ifndef NDEBUG
            toolButtonBar->AddButton(XRCID("ID_CHAPTER_SPLIT"),
                _(L"Chapter Split"),
                ReadRibbonSvgIcon(L"ribbon/chapter-split.svg"),
                _(L"Split a document into smaller documents."));
            toolButtonBar->AddButton(XRCID("ID_FIND_DUPLICATE_FILES"),
                _(L"Find Duplicates"),
                ReadRibbonSvgIcon(L"ribbon/duplicate-files.svg"),
                _(L"Search for (and remove) duplicate files."));
    #endif
            toolButtonBar->AddButton(XRCID("ID_VIEW_LOG_REPORT"),
                    _(L"Log Report"),
                     ReadRibbonSvgIcon(L"ribbon/log-book.svg"));
            if (wxGetMouseState().ShiftDown()
                #ifndef NDEBUG
                    || true
                #endif
                )
                {
                toolButtonBar->AddButton(XRCID("ID_SCRIPT_WINDOW"),
                    _(L"Lua Script"),
                    ReadRibbonSvgIcon(L"ribbon/lua.svg"));
                }
            #ifndef NDEBUG
                #ifdef ENABLE_PROFILING
                    toolButtonBar->AddButton(XRCID("ID_VIEW_PROFILE_REPORT"),
                        _(L"Profile Report"),
                        ReadRibbonSvgIcon(L"ribbon/clock.svg"));
                #endif
            #endif
            }
        }

    if (rtype != RibbonType::MainFrameRibbon)
        {
        // Document tab
        wxRibbonPage* documentPage = new wxRibbonPage(ribbon, wxID_ANY, _(L"Document"));
        // proofing section
        wxRibbonPanel* proofingPanel = new wxRibbonPanel(documentPage, wxID_ANY,
            _(L"Proofing"), wxNullBitmap, wxDefaultPosition,
            wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
        wxRibbonButtonBar* proofingButtonBar =
            new wxRibbonButtonBar(proofingPanel, MainFrame::ID_PROOFING_RIBBON_BUTTON_BAR);
        proofingButtonBar->AddButton(XRCID("ID_LAUNCH_SOURCE_FILE"), _(L"Edit Document"),
                                     ReadRibbonSvgIcon(L"ribbon/edit-document.svg"),
                                     _(L"Edit the document that is being analyzed."));
        proofingButtonBar->AddHybridButton(XRCID("ID_EDIT_DICTIONARY"), _(L"Spell Checker"),
                                           ReadRibbonSvgIcon(L"ribbon/misspellings.svg"),
                                           _(L"Edit the spell checker's dictionary."));
        // sentence section
        wxRibbonPanel* sentencePanel = new wxRibbonPanel(documentPage, wxID_ANY,
            _(L"Sentences"), wxNullBitmap, wxDefaultPosition,
            wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
        wxRibbonButtonBar* sentenceButtonBar = new wxRibbonButtonBar(sentencePanel);
        sentenceButtonBar->AddDropdownButton(XRCID("ID_SENTENCE_LENGTHS"), _(L"Long Sentences"),
                                             ReadRibbonSvgIcon(L"ribbon/long-sentence.svg"),
                                             _(L"Control how overly long sentences are determined."));
        // sentence/paragraph deduction
        wxRibbonPanel* deductionPanel =
            new wxRibbonPanel(documentPage, wxID_ANY, _(L"Sentence & Paragraph Deduction"),
                wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
        wxRibbonButtonBar* deductionButtonBar =
            new wxRibbonButtonBar(deductionPanel, MainFrame::ID_PARAGRAPH_DEDUCTION_RIBBON_BUTTON_BAR);
        deductionButtonBar->AddDropdownButton(XRCID("ID_LINE_ENDS"), _(L"Line Ends"),
            ReadRibbonSvgIcon(L"ribbon/paragraph.svg"),
            _(L"Control how line ends effect paragraph detection."));
        deductionButtonBar->AddToggleButton(XRCID("ID_IGNORE_BLANK_LINES"), _(L"Ignore Blank Lines"),
            ReadRibbonSvgIcon(L"ribbon/blank-lines.svg"),
            _(L"Control whether blank lines should affect how paragraph breaks are detected."));
        deductionButtonBar->AddToggleButton(XRCID("ID_IGNORE_INDENTING"), _(L"Ignore Indenting"),
            ReadRibbonSvgIcon(L"ribbon/indenting.svg"),
            _(L"Control whether indenting should affect how paragraph breaks are detected."));
        deductionButtonBar->AddToggleButton(XRCID("ID_SENTENCES_CAPITALIZED"), _(L"Strict Capitalization"),
            ReadRibbonSvgIcon(L"ribbon/capital-letter.svg"),
            _(L"Change whether sentences must begin with capital letters when determining sentence breaks."));
        // text exclusion
        wxRibbonPanel* exclusionPanel =
            new wxRibbonPanel(documentPage, wxID_ANY, _(L"Text Exclusion"), wxNullBitmap, wxDefaultPosition,
                              wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
        wxRibbonButtonBar* exclusionButtonBar =
            new wxRibbonButtonBar(exclusionPanel, MainFrame::ID_TEXT_EXCLUSION_RIBBON_BUTTON_BAR);
        exclusionButtonBar->AddDropdownButton(XRCID("ID_TEXT_EXCLUSION"), _(L"Exclusion"),
            ReadRibbonSvgIcon(L"ribbon/proofreading-delete.svg"),
            _(L"Select how text should be excluded from the analysis."));
        exclusionButtonBar->AddButton(XRCID("ID_INCOMPLETE_THRESHOLD"), _(L"Incomplete Threshold"),
            ReadRibbonSvgIcon(L"ribbon/period-needed.svg"),
            _(L"Specify the minimum length of an incomplete sentence that should be considered valid."));
        exclusionButtonBar->AddToggleButton(XRCID("ID_EXCLUDE_AGGRESSIVELY"), _(L"Aggressive"),
            ReadRibbonSvgIcon(L"ribbon/aggressive-list.svg"),
            _(L"Aggressively excludes list items, citations, and copyright notices (if applicable)."));
        exclusionButtonBar->AddToggleButton(XRCID("ID_EXCLUDE_COPYRIGHT_NOTICES"), _(L"Copyrights"),
            ReadRibbonSvgIcon(L"ribbon/ignore-copyright.svg"),
            _(L"Exclude trailing copyright statements from the analysis."));
        exclusionButtonBar->AddToggleButton(XRCID("ID_EXCLUDE_TRAILING_CITATIONS"), _(L"Citations"),
            ReadRibbonSvgIcon(L"ribbon/citation.svg")),
            _(L"Exclude trailing citations from the analysis.");
        exclusionButtonBar->AddToggleButton(XRCID("ID_EXCLUDE_FILE_ADDRESSES"), _(L"File Names"),
            ReadRibbonSvgIcon(L"ribbon/internet.svg"),
            _(L"Exclude file names and website addresses from the analysis."));
        exclusionButtonBar->AddToggleButton(XRCID("ID_EXCLUDE_NUMERALS"), _(L"Numerals"),
            ReadRibbonSvgIcon(L"ribbon/ignore-numerals.svg"),
            _(L"Exclude numbers from the analysis."));
        exclusionButtonBar->AddToggleButton(XRCID("ID_EXCLUDE_PROPER_NOUNS"), _(L"Proper Nouns"),
            ReadRibbonSvgIcon(L"ribbon/person.svg"),
            _(L"Exclude proper names from the analysis."));
        exclusionButtonBar->AddButton(XRCID("ID_EXCLUDE_WORD_LIST"), _(L"Exclude Words"),
            ReadRibbonSvgIcon(L"ribbon/exclusion-list.svg"),
            _(L"Add a custom list of words and phrases to exclude from the analysis."));
        exclusionButtonBar->AddDropdownButton(XRCID("ID_EXCLUSION_TAGS"), _(L"Exclusion Tags"),
            ReadRibbonSvgIcon(L"ribbon/exclusion-tags.svg"),
            _(L"Specify tags that will exclude all text between them."));
        // numeral syllabizing
        wxRibbonPanel* numeralsPanel =
            new wxRibbonPanel(documentPage, wxID_ANY, _(L"Numerals"), wxNullBitmap, wxDefaultPosition,
                              wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
        wxRibbonButtonBar* numeralsButtonBar =
            new wxRibbonButtonBar(numeralsPanel, MainFrame::ID_NUMERALS_RIBBON_BUTTON_BAR);
        numeralsButtonBar->AddDropdownButton(XRCID("ID_NUMERAL_SYLLABICATION"), _(L"Syllabication"),
                                             ReadRibbonSvgIcon(L"ribbon/number-syllabize.svg"),
                                             _(L"Specify how syllables should be counted for numbers."));

        // Readability tests tab
        wxRibbonPage* testsPage = new wxRibbonPage(ribbon, wxID_ANY, _(L"Readability"), wxNullBitmap);
        wxRibbonPanel* standardTestsPanel =
            new wxRibbonPanel(testsPage, wxID_ANY, _(L"Tests"), wxNullBitmap, wxDefaultPosition,
                              wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
        wxRibbonButtonBar* standardTestsBar = new wxRibbonButtonBar(standardTestsPanel);

        standardTestsBar->AddDropdownButton(XRCID("ID_PRIMARY_AGE_TESTS_BUTTON"),
            _(L"Primary"),
            ReadRibbonSvgIcon(L"tests/dolch.svg"),
            _(L"Tests meant for primary-age materials."));
        standardTestsBar->AddDropdownButton(XRCID("ID_SECONDARY_AGE_TESTS_BUTTON"),
            _(L"Secondary"),
            ReadRibbonSvgIcon(L"ribbon/secondary.svg"),
            _(L"Tests meant for secondary-age materials."));
        standardTestsBar->AddDropdownButton(XRCID("ID_ADULT_TESTS_BUTTON"),
            _(L"Adult"),
            ReadRibbonSvgIcon(L"ribbon/adult.svg"),
            _(L"Tests meant for adult-level materials."));
        standardTestsBar->AddDropdownButton(XRCID("ID_SECOND_LANGUAGE_TESTS_BUTTON"),
            _(L"Second Language"),
            ReadRibbonSvgIcon(L"ribbon/esl.svg"),
            _(L"Tests for materials designed for second-language readers."));
        standardTestsBar->AddDropdownButton(XRCID("ID_CUSTOM_TESTS"), _(L"Custom"),
            ReadRibbonSvgIcon(L"ribbon/formula.svg"),
            _(L"Create or edit custom tests."));
        standardTestsBar->AddDropdownButton(XRCID("ID_TEST_BUNDLES"),
            _(L"Bundles"),
            ReadRibbonSvgIcon(L"ribbon/bundles.svg"),
            _(L"Add multiple tests to a project at once."));
        standardTestsBar->AddButton(XRCID("ID_REMOVE_TEST"),
            _(L"Remove"),
            ReadRibbonSvgIcon(L"ribbon/delete.svg"),
            _(L"Remove the selected test from the project."));
        // readability tools section
        wxRibbonPanel* readabilityToolsPanel =
            new wxRibbonPanel(testsPage, wxID_ANY, _(L"Tools"), wxNullBitmap, wxDefaultPosition,
                              wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
        wxRibbonButtonBar* readabilityToolBar = new wxRibbonButtonBar(readabilityToolsPanel);
        readabilityToolBar->AddButton(XRCID("ID_TESTS_OVERVIEW"),
            _(L"Tests Overview"),
            ReadRibbonSvgIcon(L"ribbon/tests-overview.svg"),
            _(L"View information about each readability test."));
        if (rtype == RibbonType::BatchProjectRibbon)
            { readabilityToolBar->AddToggleButton(XRCID("ID_TEST_EXPLANATIONS_WINDOW"),
                _(L"Test Explanations"),
                ReadRibbonSvgIcon(L"ribbon/formula.svg"),
                _(L"Read an explanation of the selected test score.")); }
        readabilityToolBar->AddHybridButton(XRCID("ID_WORD_LISTS"),
            _(L"Word Lists"),
            ReadRibbonSvgIcon(L"tests/dale-chall-test.svg"),
            _(L"View the word lists used by readability tests."));
        readabilityToolBar->AddDropdownButton(XRCID("ID_BLANK_GRAPHS"),
            _(L"Blank Graphs"),
            ReadRibbonSvgIcon(L"ribbon/blank-graphs.svg"),
            _(L"Print or save blank readability graph templates."));

        // Tools tab
        wxRibbonPage* toolsPage = new wxRibbonPage(ribbon, wxID_ANY, _(L"Tools"));
        wxRibbonPanel* toolsPanel =
            new wxRibbonPanel(toolsPage, wxID_ANY, _(L"Tools & Settings"), wxNullBitmap, wxDefaultPosition,
                              wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
        wxRibbonButtonBar* toolButtonBar = new wxRibbonButtonBar(toolsPanel);
        toolButtonBar->AddButton(XRCID("ID_WEB_HARVEST"),
            _(L"Web Harvester"),
            ReadRibbonSvgIcon(L"ribbon/web-export.svg"),
            _(L"Download and analyze multiple webpages."));
    #ifndef NDEBUG
        toolButtonBar->AddButton(XRCID("ID_CHAPTER_SPLIT"),
            _(L"Chapter Split"),
            ReadRibbonSvgIcon(L"ribbon/chapter-split.svg"),
            _(L"Split a document into smaller documents."));
        toolButtonBar->AddButton(XRCID("ID_FIND_DUPLICATE_FILES"),
            _(L"Find Duplicates"),
            ReadRibbonSvgIcon(L"ribbon/duplicate-files.svg"),
            _(L"Search for and remove duplicate files."));
    #endif
        toolButtonBar->AddButton(wxID_PREFERENCES,
            _(L"Options"),
            ReadRibbonSvgIcon(L"ribbon/configure.svg"),
            _(L"Change the program's general options."));
        toolButtonBar->AddButton(XRCID("ID_VIEW_LOG_REPORT"),
            _(L"Log Report"),
            ReadRibbonSvgIcon(L"ribbon/log-book.svg"));
        if (wxGetMouseState().ShiftDown()
            // turn on for debug (not release) build
            #ifndef NDEBUG
                || true
            #endif
            )
            {
            toolButtonBar->AddButton(XRCID("ID_SCRIPT_WINDOW"),
                _(L"Lua Script"),
                ReadRibbonSvgIcon(L"ribbon/lua.svg"));
            }
        #ifndef NDEBUG
            #ifdef ENABLE_PROFILING
                toolButtonBar->AddButton(XRCID("ID_VIEW_PROFILE_REPORT"),
                    _(L"Profile Report"),
                    ReadRibbonSvgIcon(L"ribbon/clock.svg"));
            #endif
        #endif
        }

    // Help tab
        {
        wxRibbonPage* helpPage = new wxRibbonPage(ribbon, wxID_ANY, _(L"Help"));
        wxRibbonPanel* helpPanel =
            new wxRibbonPanel(helpPage, wxID_ANY, _(L"Documentation"), wxNullBitmap,
                              wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
        wxRibbonButtonBar* helpButtonBar = new wxRibbonButtonBar(helpPanel);
        helpButtonBar->AddButton(wxID_HELP, _(L"Content"),
            ReadRibbonSvgIcon(L"ribbon/web-help.svg"),
            _(L"Read the documentation in a browser."));
        helpButtonBar->AddButton(XRCID("ID_HELP_MANUAL"),
            _(L"Manual"),
            ReadRibbonSvgIcon(L"ribbon/help-manual.svg"),
            _(L"Read the manual."));
        helpButtonBar->AddButton(XRCID("ID_TESTS_REFERENCE"),
            _(L"Tests Reference"),
            ReadRibbonSvgIcon(L"ribbon/tests-overview.svg"),
            _(L"Read the readability tests reference."));
        helpButtonBar->AddButton(XRCID("ID_SHORTCUTS_CHEATSHEET"),
            _(L"Shortcuts"),
            ReadRibbonSvgIcon(L"ribbon/keyboard-shortcuts.svg"),
            _(L"Display the keyboard shortcuts cheatsheet."));
        helpButtonBar->AddDropdownButton(XRCID("ID_EXAMPLES"),
            _(L"Example Documents"),
            ReadRibbonSvgIcon(L"ribbon/examples.svg"),
            _(L"Analyze example documents from the help."));

        wxRibbonPanel* supportPanel = new wxRibbonPanel(helpPage, wxID_ANY, _(L"Support"), wxNullBitmap);
        wxRibbonButtonBar* supportButtonBar = new wxRibbonButtonBar(supportPanel);
        supportButtonBar->AddButton(XRCID("ID_CHECK_FOR_UPDATES"),
            _(L"Updates"),
            ReadRibbonSvgIcon(L"ribbon/updates.svg"),
            _(L"Check for updates."));
        supportButtonBar->AddButton(wxID_ABOUT, _(L"About"),
            ReadRibbonSvgIcon(L"ribbon/app-logo.svg"),
            _(L"Learn more about the program."));
        }
    ribbon->SetArtProvider(new RibbonMetroArtProvider);
    UpdateRibbonTheme(ribbon);

    ribbon->Realize();

    return ribbon;
    }

//---------------------------------------------------
void ReadabilityApp::UpdateRibbonTheme(wxRibbonBar* ribbon)
    {
    assert(ribbon != nullptr && L"Attempting to theme a null ribbon!");
    if (ribbon != nullptr)
        {
        ribbon->GetArtProvider()->SetColourScheme(GetAppOptions().GetRibbonActiveTabColor(),
            GetAppOptions().GetRibbonInactiveTabColor(),
            GetAppOptions().GetRibbonHoverColor());

        ribbon->GetArtProvider()->SetColour(
            wxRIBBON_ART_BUTTON_BAR_LABEL_COLOUR,
            GetAppOptions().GetRibbonActiveFontColor());
        ribbon->GetArtProvider()->SetColour(
            wxRIBBON_ART_BUTTON_BAR_LABEL_DISABLED_COLOUR,
            GetAppOptions().GetRibbonInactiveFontColor());
        ribbon->GetArtProvider()->SetColour(
            wxRIBBON_ART_BUTTON_BAR_LABEL_HIGHLIGHT_TOP_COLOUR,
            GetAppOptions().GetRibbonActiveFontColor());
        ribbon->GetArtProvider()->SetColour(
            wxRIBBON_ART_BUTTON_BAR_LABEL_HIGHLIGHT_GRADIENT_TOP_COLOUR,
            GetAppOptions().GetRibbonActiveFontColor());
        ribbon->GetArtProvider()->SetColour(
            wxRIBBON_ART_BUTTON_BAR_LABEL_HIGHLIGHT_COLOUR,
            GetAppOptions().GetRibbonHoverFontColor());
        ribbon->GetArtProvider()->SetColour(
            wxRIBBON_ART_BUTTON_BAR_LABEL_HIGHLIGHT_GRADIENT_COLOUR,
            GetAppOptions().GetRibbonActiveFontColor());
        ribbon->GetArtProvider()->SetColour(
            wxRIBBON_ART_TAB_ACTIVE_LABEL_COLOUR,
            GetAppOptions().GetRibbonActiveFontColor());
        ribbon->GetArtProvider()->SetColour(
            wxRIBBON_ART_PANEL_LABEL_COLOUR,
            GetAppOptions().GetRibbonActiveFontColor());
        ribbon->GetArtProvider()->SetColour(
            wxRIBBON_ART_PANEL_MINIMISED_LABEL_COLOUR,
            GetAppOptions().GetRibbonActiveFontColor());
        ribbon->GetArtProvider()->SetColour(
            wxRIBBON_ART_PANEL_HOVER_LABEL_COLOUR,
            GetAppOptions().GetRibbonHoverFontColor());
        ribbon->GetArtProvider()->SetColour(
            wxRIBBON_ART_TAB_LABEL_COLOUR,
            GetAppOptions().GetRibbonInactiveFontColor());
        ribbon->GetArtProvider()->SetColour(
            wxRIBBON_ART_TAB_HOVER_LABEL_COLOUR,
            GetAppOptions().GetRibbonHoverFontColor());
        }
    }

//---------------------------------------------------
void ReadabilityApp::UpdateRibbonTheme()
    { UpdateRibbonTheme(GetMainFrameEx()->GetRibbon()); }

//---------------------------------------------------
void ReadabilityApp::RemoveAllCustomTestBundles()
    {
    std::for_each(BaseProject::m_testBundles.cbegin(), BaseProject::m_testBundles.cend(),
        [this](const TestBundle& bundle) { if (!bundle.IsLocked())
        { GetMainFrameEx()->RemoveTestBundleFromMenus(bundle.GetName().c_str()); } });

    for (auto bundle = BaseProject::m_testBundles.begin();
         bundle != BaseProject::m_testBundles.end();
         /*in loop*/)
        {
        if (bundle->IsLocked())
            { ++bundle; }
        else
            { bundle = BaseProject::m_testBundles.erase(bundle); }
        }
    }

wxIMPLEMENT_CLASS(MainFrame, Wisteria::UI::BaseMainFrame);

std::map<int, wxString> MainFrame::m_testBundleMenuIds;
std::map<int, wxString> MainFrame::m_customTestMenuIds;
std::map<int, wxString> MainFrame::m_examplesMenuIds;

//-------------------------------------------------------
void MainFrame::OnAbout([[maybe_unused]] wxCommandEvent& event)
    {
    wxDateTime buildDate;
    buildDate.ParseDate(__DATE__);

    wxString licenseAgreementPath = wxGetApp().FindResourceFile(L"license.rtf");
    wxString eula;
    if (wxFile::Exists(licenseAgreementPath))
        {
        try
            {
            lily_of_the_valley::rtf_extract_text filter_rtf(
                lily_of_the_valley::rtf_extract_text::rtf_extraction_type::rtf_to_html);
            MemoryMappedFile licFile(licenseAgreementPath, true, true);
            const wchar_t* eulaContent =
                filter_rtf(static_cast<char*>(licFile.GetStream()), licFile.GetMapSize());
            if (eulaContent != nullptr)
                {
                eula = eulaContent;
                }
            }
        catch (...)
            {
            wxLogMessage(L"Unable to read EULA from '%s'", licenseAgreementPath);
            }
        }

    AboutDialogEx aboutDlg(this, GetAboutDialogImage(),
        wxGetApp().GetAppVersion(),
        wxString::Format(_(L"Copyright %c2006-%d %s. All rights reserved."), 0xA9,
                         buildDate.GetYear(), wxGetApp().GetVendorDisplayName()),
        &wxGetApp().GetLicenseAdmin(), eula);
    wxGetApp().UpdateSideBarTheme(aboutDlg.GetSideBar());

    aboutDlg.ShowModal();
    }

void MainFrame::OnRibbonBarHelpClicked([[maybe_unused]] wxRibbonBarEvent& event)
    {
    wxCommandEvent cmd;
    OnHelpContents(cmd);
    }

//-------------------------------------------------------
void MainFrame::OnViewProfileReport([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    DUMP_PROFILER_REPORT();

    const wxSize screenSize{ wxSystemSettings::GetMetric(wxSystemMetric::wxSYS_SCREEN_X),
                             wxSystemSettings::GetMetric(wxSystemMetric::wxSYS_SCREEN_Y) };
    ListDlg profileReportDialog(
        this, wxGetApp().GetAppOptions().GetRibbonActiveTabColor(),
        wxGetApp().GetAppOptions().GetRibbonHoverColor(),
        wxGetApp().GetAppOptions().GetRibbonActiveFontColor(),
        LD_SAVE_BUTTON | LD_COPY_BUTTON | LD_PRINT_BUTTON | LD_SELECT_ALL_BUTTON | LD_FIND_BUTTON |
            LD_COLUMN_HEADERS | LD_SORT_BUTTON,
        wxID_ANY, _(L"Profile Report"), wxString{}, wxDefaultPosition,
        wxSize{ static_cast<int>(screenSize.GetWidth() * math_constants::half),
                static_cast<int>(screenSize.GetHeight() * math_constants::half) });

    wxGetApp().UpdateRibbonTheme(profileReportDialog.GetRibbon());
    profileReportDialog.GetListCtrl()->ClearAll();
    profileReportDialog.GetListCtrl()->InsertColumn(0, _DT(L"Function", DTExplanation::DebugMessage));
    profileReportDialog.GetListCtrl()->InsertColumn(1, _DT(L"Times Called"));
    profileReportDialog.GetListCtrl()->InsertColumn(2, _DT(L"Total Time (Milliseconds)"));
    profileReportDialog.GetListCtrl()->InsertColumn(3, _DT(L"Total Time (%)"));
    profileReportDialog.GetListCtrl()->InsertColumn(4, _DT(L"Lowest Call Time"));
    profileReportDialog.GetListCtrl()->InsertColumn(5, _DT(L"Highest Call Time"));
    profileReportDialog.GetListCtrl()->InsertColumn(6, _DT(L"Average Call Time"));
    BaseProjectDoc::UpdateListOptions(profileReportDialog.GetListCtrl());

    wxString filePath = wxGetApp().GetProfileReportPath();
    wxString buffer;
    Wisteria::TextStream::ReadFile(filePath, buffer);

    const lily_of_the_valley::text_column_delimited_character_parser
                                              parser(L'\t');
    lily_of_the_valley::text_column<lily_of_the_valley::text_column_delimited_character_parser>
                                    myColumn(parser, std::nullopt);
    lily_of_the_valley::text_row<ListCtrlExDataProvider::ListCellString> myRow(std::nullopt);
    myRow.add_column(myColumn);
    myRow.treat_consecutive_delimitors_as_one(false);

    lily_of_the_valley::text_matrix<ListCtrlExDataProvider::ListCellString>
        importer(&profileReportDialog.GetData()->GetMatrix());

    // header row, which will just be skipped
    importer.add_row_definition(lily_of_the_valley::text_row<ListCtrlExDataProvider::ListCellString>(1));
    importer.add_row_definition(myRow);

    // see how many lines are in the file
    lily_of_the_valley::text_preview preview;
    size_t rowCount = preview(buffer, L'\t', true, false);
    // now read it
    rowCount = importer.read(buffer, rowCount, 7, true);

    profileReportDialog.GetListCtrl()->SetVirtualDataSize(rowCount, 7);
    profileReportDialog.GetListCtrl()->SetItemCount(static_cast<long>((rowCount)));

    // fit the columns
    profileReportDialog.GetListCtrl()->DistributeColumns(-1);
    // sort by time, largest to smallest
    profileReportDialog.GetListCtrl()->SortColumn(2, Wisteria::SortDirection::SortDescending);

    profileReportDialog.ShowModal();
    }

//-------------------------------------------------------
void MainFrame::OnViewLogReport([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    if (m_logWindow == nullptr)
        {
        const wxSize screenSize{ wxSystemSettings::GetMetric(wxSystemMetric::wxSYS_SCREEN_X),
                                 wxSystemSettings::GetMetric(wxSystemMetric::wxSYS_SCREEN_Y) };
        m_logWindow =
            new ListDlg(nullptr, wxGetApp().GetAppOptions().GetRibbonActiveTabColor(),
                        wxGetApp().GetAppOptions().GetRibbonHoverColor(),
                        wxGetApp().GetAppOptions().GetRibbonActiveFontColor(),
                        LD_SAVE_BUTTON | LD_COPY_BUTTON | LD_PRINT_BUTTON | LD_SELECT_ALL_BUTTON |
                            LD_FIND_BUTTON | LD_COLUMN_HEADERS | LD_SORT_BUTTON | LD_CLEAR_BUTTON |
                            LD_REFRESH_BUTTON | LD_LOG_VERBOSE_BUTTON,
                        wxID_ANY, _(L"Log Report"), wxString{}, wxDefaultPosition,
                        wxSize{ static_cast<int>(screenSize.GetWidth() * math_constants::half),
                                static_cast<int>(screenSize.GetHeight() * math_constants::half) },
                        wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxDIALOG_NO_PARENT);
        // move over to the right side of the screen
        const int screenWidth{ wxSystemSettings::GetMetric(wxSystemMetric::wxSYS_SCREEN_X) };
        int xPos{ 0 }, yPos{ 0 };
        m_logWindow->GetScreenPosition(&xPos, &yPos);
        m_logWindow->Move(
            wxPoint{ xPos + (screenWidth - (xPos + m_logWindow->GetSize().GetWidth())), yPos });

        m_logWindow->SetSortHelpTopic(
            GetHelpDirectory(), _DT(L"column-sorting.html"));
        }
    wxGetApp().UpdateRibbonTheme(m_logWindow->GetRibbon());
    m_logWindow->SetActiveLog(wxGetApp().GetLogFile());
    m_logWindow->Readlog();
    // fit the columns
    m_logWindow->GetListCtrl()->DistributeColumns(-1);

    BaseProjectDoc::UpdateListOptions(m_logWindow->GetListCtrl());

    m_logWindow->Show();
    m_logWindow->SetFocus();
    }

//-------------------------------------------------------
void MainFrame::OnTestsOverview([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    const wxSize screenSize{ wxSystemSettings::GetMetric(wxSystemMetric::wxSYS_SCREEN_X),
                             wxSystemSettings::GetMetric(wxSystemMetric::wxSYS_SCREEN_Y) };
    // test overview dialog
    ListDlg testsOverviewDlg(
        this, wxGetApp().GetAppOptions().GetRibbonActiveTabColor(),
        wxGetApp().GetAppOptions().GetRibbonHoverColor(),
        wxGetApp().GetAppOptions().GetRibbonActiveFontColor(),
        LD_SAVE_BUTTON | LD_COPY_BUTTON | LD_PRINT_BUTTON | LD_SELECT_ALL_BUTTON |
            LD_COLUMN_HEADERS | LD_FIND_BUTTON | LD_SORT_BUTTON,
        wxID_ANY, _(L"Readability Tests Overview"), wxString{}, wxDefaultPosition,
        wxSize{ static_cast<int>(screenSize.GetWidth() * math_constants::three_quarters),
                static_cast<int>(screenSize.GetHeight() * math_constants::half) });

    wxGetApp().UpdateRibbonTheme(testsOverviewDlg.GetRibbon());
    testsOverviewDlg.GetListCtrl()->ClearAll();
    testsOverviewDlg.GetListCtrl()->InsertColumn(0, _(L"Name"));
    testsOverviewDlg.GetListCtrl()->InsertColumn(1, _(L"Score Type"));
    testsOverviewDlg.GetListCtrl()->InsertColumn(2, _(L"Language(s)"));
    testsOverviewDlg.GetListCtrl()->InsertColumn(3, _(L"Word Complexity"));
    testsOverviewDlg.GetListCtrl()->InsertColumn(4, _(L"Word Length"));
    testsOverviewDlg.GetListCtrl()->InsertColumn(5, _(L"Word Familiarity"));
    testsOverviewDlg.GetListCtrl()->InsertColumn(6, _(L"Sentence Length"));
    testsOverviewDlg.GetListCtrl()->InsertColumn(7, _(L"Description"));
    testsOverviewDlg.GetListCtrl()->SetVirtualDataSize(
        BaseProject::GetDefaultReadabilityTestsTemplate().get_tests().size(), 8);
    BaseProjectDoc::UpdateListOptions(testsOverviewDlg.GetListCtrl());
    size_t i = 0;
    for (auto testPos = BaseProject::GetDefaultReadabilityTestsTemplate().get_tests().begin();
         testPos != BaseProject::GetDefaultReadabilityTestsTemplate().get_tests().end();
         ++testPos, ++i)
        {
        // test type
        testsOverviewDlg.GetListCtrl()->SetItemText(i, 0,
            wxString(testPos->get_long_name().c_str()));
        switch (testPos->get_test_type())
            {
        case readability::readability_test_type::grade_level:
            testsOverviewDlg.GetListCtrl()->SetItemText(i, 1, _(L"Grade level"));
            break;
        case readability::readability_test_type::index_value:
            testsOverviewDlg.GetListCtrl()->SetItemText(i, 1, _(L"Index score"));
            break;
        case readability::readability_test_type::index_value_and_grade_level:
            testsOverviewDlg.GetListCtrl()->SetItemText(i, 1, _(L"Grade level and index score"));
            break;
        case readability::readability_test_type::predicted_cloze_score:
            testsOverviewDlg.GetListCtrl()->SetItemText(i, 1, _(L"Cloze score"));
            break;
        case readability::readability_test_type::grade_level_and_predicted_cloze_score:
            testsOverviewDlg.GetListCtrl()->SetItemText(i, 1, _(L"Grade level and cloze score"));
            break;
        default:
            testsOverviewDlg.GetListCtrl()->SetItemText(i, 1, _(L"Grade level"));
            };
        // language
        wxString languages;
        if (testPos->has_language(readability::test_language::english_test))
            { languages += _(L"English") + _DT(L"/"); }
        if (testPos->has_language(readability::test_language::spanish_test))
            { languages += _(L"Spanish") + _DT(L"/"); }
        if (testPos->has_language(readability::test_language::german_test))
            { languages += _(L"German") + _DT(L"/"); }
        if (languages.length())
            { languages.RemoveLast(); }
        testsOverviewDlg.GetListCtrl()->SetItemText(i, 2, languages);
        // word complexity
        const wxString wordComplexity =
            testPos->has_factor(readability::test_factor::word_complexity_2_plus_syllables) ?
            _(L" X (2 or more syllables)") :
                testPos->has_factor(readability::test_factor::word_complexity_3_plus_syllables) ?
            _(L" X (3 or more syllables)") :
                testPos->has_factor(readability::test_factor::word_complexity_density) ?
            _(L" X (syllable density)") :
                testPos->has_factor(readability::test_factor::word_complexity) ?
            wxString(_DT(L" X ")) : wxString{};
        testsOverviewDlg.GetListCtrl()->SetItemText(i, 3, wordComplexity);
        // word length
        const wxString wordLength =
            testPos->has_factor(readability::test_factor::word_length_3_less) ?
            _(L" X (3 or less characters)") :
                testPos->has_factor(readability::test_factor::word_length_6_plus) ?
            _(L" X (6 or more characters)") :
                testPos->has_factor(readability::test_factor::word_length_7_plus) ?
            _(L" X (7 or more characters)") :
                testPos->has_factor(readability::test_factor::word_length) ?
            wxString(_DT(L" X ")) : wxString{};
        testsOverviewDlg.GetListCtrl()->SetItemText(i, 4, wordLength);
        // word familiarity
        const wxString wordFamiliarity =
            testPos->has_factor(readability::test_factor::word_familiarity_spache) ?
            _(L" X (Spache rules)") :
                testPos->has_factor(readability::test_factor::word_familiarity_dale_chall) ?
            _(L" X (Dale-Chall rules)") :
                testPos->has_factor(readability::test_factor::word_familiarity_harris_jacobson) ?
            _(L" X (Harris-Jacobson rules)") : wxString{};
        testsOverviewDlg.GetListCtrl()->SetItemText(i, 5, wordFamiliarity);
        // sentence length
        testsOverviewDlg.GetListCtrl()->SetItemText(i, 6,
            testPos->has_factor(readability::test_factor::sentence_length) ?
            wxString{ _DT(L" X ") } : wxString{});
        // description
        lily_of_the_valley::html_extract_text filter_html;
        testsOverviewDlg.GetListCtrl()->SetItemText(i, 7,
            wxString(filter_html(testPos->get_description().c_str(),
            testPos->get_description().length(), true, false)) );
        }
    // fit the columns
    testsOverviewDlg.GetListCtrl()->DistributeColumns(-1);

    testsOverviewDlg.ShowModal();
    }

//-------------------------------------------------------
void MainFrame::OnBlankGraph(wxCommandEvent& event)
    {
    BaseProject project;
    if (event.GetId() == XRCID("ID_BLANK_FRASE_GRAPH"))
        {
        GraphDlg graphDlg(this, wxID_ANY, wxString::Format(_(L"Blank \"%s\" Graph"), _DT(L"FRASE")));
        graphDlg.GetCanvas()->SetFixedObject(0, 0, std::make_shared<FraseGraph>(graphDlg.GetCanvas()));
        wxGetApp().GetAppOptions().UpdateGraphOptions(graphDlg.GetCanvas());
        graphDlg.ShowModal();
        }
    else if (event.GetId() == XRCID("ID_BLANK_CRAWFORD_GRAPH"))
        {
        GraphDlg graphDlg(this, wxID_ANY, wxString::Format(_(L"Blank \"%s\" Graph"), _DT(L"Crawford")));
        graphDlg.GetCanvas()->SetFixedObject(0, 0, std::make_shared<CrawfordGraph>(graphDlg.GetCanvas()));
        wxGetApp().GetAppOptions().UpdateGraphOptions(graphDlg.GetCanvas());
        graphDlg.ShowModal();
        }
    else if (event.GetId() == XRCID("ID_BLANK_FRY_GRAPH"))
        {
        GraphDlg graphDlg(this, wxID_ANY, wxString::Format(_(L"Blank \"%s\" Graph"), _DT(L"Fry")));
        auto fryGraph = std::make_shared<FryGraph>(graphDlg.GetCanvas(),
            FryGraph::FryGraphType::Traditional);
        // update custom settings on graph
        fryGraph->SetMessageCatalog(project.GetReadabilityMessageCatalogPtr());
        fryGraph->SetInvalidAreaColor(wxGetApp().GetAppOptions().GetInvalidAreaColor());

        graphDlg.GetCanvas()->SetFixedObject(0, 0, fryGraph);
        wxGetApp().GetAppOptions().UpdateGraphOptions(graphDlg.GetCanvas());
        graphDlg.ShowModal();
        }
    else if (event.GetId() == XRCID("ID_BLANK_GPM_GRAPH"))
        {
        GraphDlg graphDlg(this, wxID_ANY, wxString::Format(_(L"Blank \"%s\" Graph"),
                         _DT(L"Gilliam-Pe\U000000F1a-Mountain")));
        auto gFryGraph = std::make_shared<FryGraph>(graphDlg.GetCanvas(),
            FryGraph::FryGraphType::GPM);
        // update custom settings on graph
        gFryGraph->SetMessageCatalog(project.GetReadabilityMessageCatalogPtr());
        gFryGraph->SetInvalidAreaColor(wxGetApp().GetAppOptions().GetInvalidAreaColor());

        graphDlg.GetCanvas()->SetFixedObject(0, 0, gFryGraph);
        wxGetApp().GetAppOptions().UpdateGraphOptions(graphDlg.GetCanvas());
        graphDlg.ShowModal();
        }
    else if (event.GetId() == XRCID("ID_BLANK_RAYGOR_GRAPH"))
        {
        GraphDlg graphDlg(this, wxID_ANY, wxString::Format(_(L"Blank \"%s\" Graph"), _DT(L"Raygor")));
        auto raygorGraph = std::make_shared<RaygorGraph>(graphDlg.GetCanvas());
        // update custom settings on graph
        raygorGraph->SetMessageCatalog(project.GetReadabilityMessageCatalogPtr());
        raygorGraph->SetInvalidAreaColor(wxGetApp().GetAppOptions().GetInvalidAreaColor());

        graphDlg.GetCanvas()->SetFixedObject(0, 0, raygorGraph);
        wxGetApp().GetAppOptions().UpdateGraphOptions(graphDlg.GetCanvas());
        graphDlg.ShowModal();
        }
    else if (event.GetId() == XRCID("ID_BLANK_FLESCH_GRAPH"))
        {
        GraphDlg graphDlg(this, wxID_ANY, wxString::Format(_(L"Blank \"%s\" Graph"), _DT(L"Flesch Reading Ease")));
        auto fleschChart = std::make_shared<FleschChart>(graphDlg.GetCanvas());
        fleschChart->ShowConnectionLine(wxGetApp().GetAppOptions().IsConnectingFleschPoints());

        graphDlg.GetCanvas()->SetFixedObject(0, 0, fleschChart);
        wxGetApp().GetAppOptions().UpdateGraphOptions(graphDlg.GetCanvas());
        graphDlg.ShowModal();
        }
    else if (event.GetId() == XRCID("ID_BLANK_DB2_GRAPH"))
        {
        GraphDlg graphDlg(this, wxID_ANY, wxString::Format(_(L"Blank \"%s\" Graph"), BaseProjectView::GetDB2Label()));
        graphDlg.GetCanvas()->SetFixedObject(0, 0,std::make_shared<DanielsonBryan2Plot>(graphDlg.GetCanvas()));
        wxGetApp().GetAppOptions().UpdateGraphOptions(graphDlg.GetCanvas());
        graphDlg.ShowModal();
        }
    else if (event.GetId() == XRCID("ID_BLANK_SCHWARTZ_GRAPH"))
        {
        GraphDlg graphDlg(this, wxID_ANY, wxString::Format(_(L"Blank \"%s\" Graph"), _DT(L"Schwartz")));
        auto schwartzGraph = std::make_shared < SchwartzGraph>(graphDlg.GetCanvas());
        schwartzGraph->SetMessageCatalog(project.GetReadabilityMessageCatalogPtr());
        schwartzGraph->SetInvalidAreaColor(wxGetApp().GetAppOptions().GetInvalidAreaColor());

        graphDlg.GetCanvas()->SetFixedObject(0, 0, schwartzGraph);
        wxGetApp().GetAppOptions().UpdateGraphOptions(graphDlg.GetCanvas());
        graphDlg.ShowModal();
        }
    else if (event.GetId() == XRCID("ID_BLANK_LIX_GRAPH"))
        {
        GraphDlg graphDlg(this, wxID_ANY, _(L"Blank Lix Gauge"));
        graphDlg.GetCanvas()->SetFixedObject(0, 0, std::make_shared<LixGauge>(graphDlg.GetCanvas()));
        wxGetApp().GetAppOptions().UpdateGraphOptions(graphDlg.GetCanvas());
        graphDlg.ShowModal();
        }
    else if (event.GetId() == XRCID("ID_BLANK_GERMAN_LIX_GRAPH"))
        {
        GraphDlg graphDlg(this, wxID_ANY,_(L"Blank German Lix Gauge"));
        auto lixGauge = std::make_shared<LixGaugeGerman>(graphDlg.GetCanvas());
        lixGauge->UseEnglishLabels(wxGetApp().GetAppOptions().IsUsingEnglishLabelsForGermanLix());

        graphDlg.GetCanvas()->SetFixedObject(0, 0, lixGauge);
        wxGetApp().GetAppOptions().UpdateGraphOptions(graphDlg.GetCanvas());
        graphDlg.ShowModal();
        }
    }

//-------------------------------------------------------
MainFrame::MainFrame(wxDocManager* manager, wxFrame* frame,
          const wxArrayString& defaultFileExtensions, const wxString& title,
          const wxPoint& pos, const wxSize& size, long type) :
          Wisteria::UI::BaseMainFrame(manager, frame, defaultFileExtensions, title, pos, size, type),
          CUSTOM_TEST_RANGE(1000),
          EXAMPLE_RANGE(300),
          TEST_BUNDLE_RANGE(300)
    {
    Bind(wxEVT_MENU, &MainFrame::OnOpenExample, this, EXAMPLE_RANGE.GetFirstId(), EXAMPLE_RANGE.GetLastId());
    wxAcceleratorEntry accelEntries[4];
    accelEntries[0].Set(wxACCEL_NORMAL, WXK_F1, wxID_HELP);
    accelEntries[1].Set(wxACCEL_CMD, static_cast<int>(L'N'), wxID_NEW);
    accelEntries[2].Set(wxACCEL_CMD, static_cast<int>(L'O'), wxID_OPEN);
    accelEntries[3].Set(wxACCEL_CMD, static_cast<int>(L'V'), wxID_PASTE);
    wxAcceleratorTable accelTable(std::size(accelEntries), accelEntries);
    SetAcceleratorTable(accelTable);

    // bind menu events to their respective ribbon button events
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &MainFrame::OnToolsWebHarvest, this, XRCID("ID_WEB_HARVEST"));
    Bind(wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
            {
            wxRibbonButtonBarEvent event;
            OnToolsWebHarvest(event);
            },
        XRCID("ID_WEB_HARVEST"));

    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &MainFrame::OnFindDuplicateFiles, this, XRCID("ID_FIND_DUPLICATE_FILES"));
    Bind(wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
            {
            wxRibbonButtonBarEvent event;
            OnFindDuplicateFiles(event);
            },
        XRCID("ID_FIND_DUPLICATE_FILES"));

    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &MainFrame::OnToolsChapterSplit, this, XRCID("ID_CHAPTER_SPLIT"));
    Bind(wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
            {
            wxRibbonButtonBarEvent event;
            OnToolsChapterSplit(event);
            },
        XRCID("ID_CHAPTER_SPLIT"));

    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &MainFrame::OnToolsOptions, this, wxID_PREFERENCES);
    Bind(wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
            {
            wxRibbonButtonBarEvent event;
            OnToolsOptions(event);
            },
        wxID_PREFERENCES);

    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &MainFrame::OnWordList, this, XRCID("ID_WORD_LISTS"));
    Bind(wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
            {
            wxRibbonButtonBarEvent event;
            OnWordList(event);
            },
        XRCID("ID_WORD_LISTS"));

    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &MainFrame::OnTestsOverview, this, XRCID("ID_TESTS_OVERVIEW"));
    Bind(wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
            {
            wxRibbonButtonBarEvent event;
            OnTestsOverview(event);
            },
        XRCID("ID_TESTS_OVERVIEW"));

    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &MainFrame::OnViewLogReport, this, XRCID("ID_VIEW_LOG_REPORT"));
    Bind(wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
            {
            wxRibbonButtonBarEvent event;
            OnViewLogReport(event);
            },
        XRCID("ID_VIEW_LOG_REPORT"));

    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &MainFrame::OnViewProfileReport, this, XRCID("ID_VIEW_PROFILE_REPORT"));
    Bind(wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
            {
            wxRibbonButtonBarEvent event;
            OnViewProfileReport(event);
            },
        XRCID("ID_VIEW_PROFILE_REPORT"));

    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &MainFrame::OnHelpCheckForUpdates, this, XRCID("ID_CHECK_FOR_UPDATES"));
    Bind(wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
            {
            wxRibbonButtonBarEvent event;
            OnHelpCheckForUpdates(event);
            },
        XRCID("ID_CHECK_FOR_UPDATES"));

    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &MainFrame::OnHelpManual, this, XRCID("ID_HELP_MANUAL"));
    Bind(wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
            {
            wxRibbonButtonBarEvent event;
            OnHelpManual(event);
            },
        XRCID("ID_HELP_MANUAL"));

    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED,
        [this]([[maybe_unused]] wxRibbonButtonBarEvent&)
            {
            const wxString manualPath = GetHelpDirectory() + wxFileName::GetPathSeparator() +
                _DT(L"ReadabilityTestReference.pdf");
            wxLaunchDefaultApplication(manualPath);
            },
        XRCID("ID_TESTS_REFERENCE"));

    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED,
        [this]([[maybe_unused]] wxRibbonButtonBarEvent&)
            {
            const wxString manualPath = GetHelpDirectory() + wxFileName::GetPathSeparator() +
                _DT(L"ShortcutsCheatsheet.pdf");
            wxLaunchDefaultApplication(manualPath);
            },
        XRCID("ID_SHORTCUTS_CHEATSHEET"));

    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &MainFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
            {
            wxRibbonButtonBarEvent event;
            OnAbout(event);
            },
        wxID_ABOUT);

    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &MainFrame::OnPrintDropdown, this,
         XRCID("ID_PRINT_OPTIONS"));
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &MainFrame::OnWordListDropdown, this,
         XRCID("ID_WORD_LISTS"));
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &MainFrame::OnBlankGraphDropdown, this,
         XRCID("ID_BLANK_GRAPHS"));
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &MainFrame::OnDictionaryDropdown, this, XRCID("ID_EDIT_DICTIONARY"));
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &MainFrame::OnExampleDropdown, this,
         XRCID("ID_EXAMPLES"));
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &MainFrame::OnCustomTestsDropdown, this,
         XRCID("ID_CUSTOM_TESTS"));
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &MainFrame::OnTestBundlesDropdown, this,
         XRCID("ID_TEST_BUNDLES"));

    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &MainFrame::OnNewDropdown, this, wxID_NEW);
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &MainFrame::OnOpenDropdown, this, wxID_OPEN);
    Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnClose, this);

    Bind(wxEVT_RIBBONBAR_HELP_CLICK, &MainFrame::OnRibbonBarHelpClicked, this);
    Bind(wxEVT_STARTPAGE_CLICKED, &MainFrame::OnStartPageClick, this);

    Bind(wxEVT_MENU, &MainFrame::OnOpenDocument, this, XRCID("ID_OPEN_DOCUMENT"));

    // custom test menu
    Bind(wxEVT_MENU, &MainFrame::OnAddCustomTest, this, XRCID("ID_ADD_CUSTOM_TEST"));
    Bind(wxEVT_MENU, &MainFrame::OnAddCustomTest, this, XRCID("ID_ADD_CUSTOM_NEW_DALE_CHALL_TEST"));
    Bind(wxEVT_MENU, &MainFrame::OnAddCustomTest, this, XRCID("ID_ADD_CUSTOM_SPACHE_TEST"));
    Bind(wxEVT_MENU, &MainFrame::OnAddCustomTest, this,
         XRCID("ID_ADD_CUSTOM_HARRIS_JACOBSON_TEST"));
    Bind(wxEVT_MENU, &MainFrame::OnAddCustomTest, this, XRCID("ID_ADD_CUSTOM_TEST_BASED_ON"));
    Bind(wxEVT_MENU, &MainFrame::OnEditCustomTest, this, XRCID("ID_EDIT_CUSTOM_TEST"));
    Bind(wxEVT_MENU, &MainFrame::OnRemoveCustomTest, this, XRCID("ID_REMOVE_CUSTOM_TEST"));
    Bind(wxEVT_MENU, &MainFrame::OnAddCustomTestBundle, this, XRCID("ID_ADD_CUSTOM_TEST_BUNDLE"));
    Bind(wxEVT_MENU, &MainFrame::OnEditCustomTestBundle, this, XRCID("ID_EDIT_CUSTOM_TEST_BUNDLE"));
    Bind(wxEVT_MENU, &MainFrame::OnRemoveCustomTestBundle, this,
         XRCID("ID_REMOVE_CUSTOM_TEST_BUNDLE"));

    // list editing
    Bind(wxEVT_MENU, &MainFrame::OnEditWordList, this, XRCID("ID_EDIT_WORD_LIST"));
    Bind(wxEVT_MENU, &MainFrame::OnEditPhraseList, this, XRCID("ID_EDIT_PHRASE_LIST"));
    Bind(wxEVT_MENU, &MainFrame::OnScriptEditor, this, XRCID("ID_SCRIPT_WINDOW"));

    // dictionary menu
    Bind(wxEVT_MENU, &MainFrame::OnEditEnglishDictionary, this,
         XRCID("ID_EDIT_ENGLISH_DICTIONARY"));
    Bind(wxEVT_MENU, &MainFrame::OnEditDictionarySettings, this,
         XRCID("ID_EDIT_DICTIONARY_SETTINGS"));

    // print and paste
    Bind(wxEVT_MENU, &MainFrame::OnPrinterHeaderFooter, this, XRCID("ID_PRINTER_HEADER_FOOTER"));
    Bind(wxEVT_MENU, &MainFrame::OnPaste, this, wxID_PASTE);

    // blank graph menu
    Bind(wxEVT_MENU, &MainFrame::OnBlankGraph, this, XRCID("ID_BLANK_FRASE_GRAPH"));
    Bind(wxEVT_MENU, &MainFrame::OnBlankGraph, this, XRCID("ID_BLANK_FRY_GRAPH"));
    Bind(wxEVT_MENU, &MainFrame::OnBlankGraph, this, XRCID("ID_BLANK_GPM_GRAPH"));
    Bind(wxEVT_MENU, &MainFrame::OnBlankGraph, this, XRCID("ID_BLANK_RAYGOR_GRAPH"));
    Bind(wxEVT_MENU, &MainFrame::OnBlankGraph, this, XRCID("ID_BLANK_CRAWFORD_GRAPH"));
    Bind(wxEVT_MENU, &MainFrame::OnBlankGraph, this, XRCID("ID_BLANK_FLESCH_GRAPH"));
    Bind(wxEVT_MENU, &MainFrame::OnBlankGraph, this, XRCID("ID_BLANK_DB2_GRAPH"));
    Bind(wxEVT_MENU, &MainFrame::OnBlankGraph, this, XRCID("ID_BLANK_SCHWARTZ_GRAPH"));
    Bind(wxEVT_MENU, &MainFrame::OnBlankGraph, this, XRCID("ID_BLANK_LIX_GRAPH"));
    Bind(wxEVT_MENU, &MainFrame::OnBlankGraph, this, XRCID("ID_BLANK_GERMAN_LIX_GRAPH"));

    // word list menu
    Bind(wxEVT_MENU, &MainFrame::OnWordListByPage, this, XRCID("ID_DC_WORD_LIST_WINDOW"));
    Bind(wxEVT_MENU, &MainFrame::OnWordListByPage, this, XRCID("ID_STOCKER_CATHOLIC_WORD_LIST_WINDOW"));
    Bind(wxEVT_MENU, &MainFrame::OnWordListByPage, this, XRCID("ID_SPACHE_WORD_LIST_WINDOW"));
    Bind(wxEVT_MENU, &MainFrame::OnWordListByPage, this, XRCID("ID_HARRIS_JACOBSON_WORD_LIST_WINDOW"));
    Bind(wxEVT_MENU, &MainFrame::OnWordListByPage, this, XRCID("ID_DOLCH_WORD_LIST_WINDOW"));
    }

//---------------------------------------------------
void ReadabilityApp::UpdateStartPageTheme()
    {
    GetMainFrameEx()->GetStartPage()->SetButtonAreaBackgroundColor(
        GetAppOptions().GetStartPageBackstageBackgroundColor());
    GetMainFrameEx()->GetStartPage()->SetMRUBackgroundColor(
        GetAppOptions().GetStartPageDetailBackgroundColor());
    }

//---------------------------------------------------
void ReadabilityApp::UpdateScriptEditorTheme()
    {
    GetMainFrameEx()->m_luaEditor->SetThemeColor(GetAppOptions().GetControlBackgroundColor());
    }

//---------------------------------------------------
void ReadabilityApp::UpdateLogWindowTheme()
    {
    UpdateRibbonTheme(GetMainFrameEx()->m_logWindow->GetRibbon());
    }

//---------------------------------------------------
void ReadabilityApp::InitProjectSidebar()
    {
    auto& imgList{ GetMainFrameEx()->GetProjectSideBarImageList() };
    // Fill in the icons for the projects' sidebars.
    // Do NOT change the ordering of these (indices are used by UpdateSideBarIcons());
    // new ones always get added at the bottom.
    imgList.emplace_back(GetResourceManager().GetSVG(L"ribbon/highlightedwords.svg"));
    imgList.emplace_back(GetResourceManager().GetSVG(L"tests/flesch-test.svg"));
    imgList.emplace_back(GetResourceManager().GetSVG(L"ribbon/stats.svg"));
    imgList.emplace_back(GetResourceManager().GetSVG(L"ribbon/difficult.svg"));
    imgList.emplace_back(GetResourceManager().GetSVG(L"ribbon/grammar.svg"));
    imgList.emplace_back(GetResourceManager().GetSVG(L"tests/dolch.svg"));
    imgList.emplace_back(GetResourceManager().GetSVG(L"ribbon/histogram.svg"));
    imgList.emplace_back(GetResourceManager().GetSVG(L"ribbon/boxplot.svg"));
    imgList.emplace_back(GetResourceManager().GetSVG(L"ribbon/warning.svg"));
    imgList.emplace_back(GetResourceManager().GetSVG(L"ribbon/bullet.svg"));
    imgList.emplace_back(GetResourceManager().GetSVG(L"ribbon/configure.svg"));
    imgList.emplace_back(GetResourceManager().GetSVG(L"ribbon/project-settings.svg"));
    imgList.emplace_back(GetResourceManager().GetSVG(L"ribbon/document-structure.svg"));
    imgList.emplace_back(GetResourceManager().GetSVG(L"ribbon/words.svg"));
    imgList.emplace_back(GetResourceManager().GetSVG(L"ribbon/sentences.svg"));
    imgList.emplace_back(GetResourceManager().GetSVG(L"ribbon/list.svg"));
    imgList.emplace_back(GetResourceManager().GetSVG(L"ribbon/bar-chart.svg"));
    imgList.emplace_back(GetResourceManager().GetSVG(L"ribbon/report.svg"));
    imgList.emplace_back(GetResourceManager().GetSVG(L"tests/flesch-test.svg"));
    imgList.emplace_back(GetResourceManager().GetSVG(L"tests/frase.svg"));
    imgList.emplace_back(GetResourceManager().GetSVG(L"tests/fry-test.svg"));
    imgList.emplace_back(GetResourceManager().GetSVG(L"tests/raygor-test.svg"));
    imgList.emplace_back(GetResourceManager().GetSVG("tests/crawford.svg"));
    imgList.emplace_back(GetResourceManager().GetSVG(L"ribbon/scores-window.svg"));
    imgList.emplace_back(GetResourceManager().GetSVG(L"ribbon/heatmap.svg"));
    imgList.emplace_back(GetResourceManager().GetSVG(L"tests/schwartz.svg"));
    imgList.emplace_back(GetResourceManager().GetSVG(L"tests/lix-test.svg"));
    imgList.emplace_back(GetResourceManager().GetSVG(L"tests/danielson-bryan-2.svg"));
    imgList.emplace_back(GetResourceManager().GetSVG(L"ribbon/checklist.svg"));
    imgList.emplace_back(GetResourceManager().GetSVG(L"ribbon/word-cloud.svg"));
    imgList.emplace_back(GetResourceManager().GetSVG(L"ribbon/donut-subgrouped.svg"));
    }

//---------------------------------------------------
void ReadabilityApp::InitStartPage()
    {
    // add start area inside mainframe
    wxArrayString mruFiles;
    for (size_t i = 0; i < GetDocManager()->GetFileHistory()->GetCount(); ++i)
        { mruFiles.Add(GetDocManager()->GetFileHistory()->GetHistoryFile(i)); }
    GetMainFrameEx()->m_startPage = new wxStartPage(GetMainFrameEx(), wxID_ANY, mruFiles,
        GetResourceManager().GetSVG(L"ribbon/app-logo.svg"));

    GetMainFrameEx()->GetStartPage()->SetUserName(GetAppOptions().GetReviewer());
    GetMainFrameEx()->GetStartPage()->AddButton(
        GetResourceManager().GetSVG(L"ribbon/document.svg"),
        _(L"Create a New Project"));
    GetMainFrameEx()->GetStartPage()->AddButton(
        wxArtProvider::GetBitmapBundle(wxART_FILE_OPEN),
        _(L"Open a Project"));
    GetMainFrameEx()->GetStartPage()->AddButton(
        GetResourceManager().GetSVG(L"ribbon/examples.svg"),
        _(L"Read the Examples"));
    GetMainFrameEx()->GetStartPage()->AddButton(
        GetResourceManager().GetSVG(L"ribbon/help.svg"),
        _(L"Read the Notes"));
    GetMainFrameEx()->GetStartPage()->AddButton(
        GetResourceManager().GetSVG(L"ribbon/help-manual.svg"),
        _(L"Read the Manual"));
    GetMainFrameEx()->GetStartPage()->AddButton(
        GetResourceManager().GetSVG(L"ribbon/configure.svg"),
        _(L"Review Program Options"));

    UpdateStartPageTheme();

    GetMainFrameEx()->GetSizer()->Add(GetMainFrameEx()->GetStartPage(), 1, wxEXPAND);
    }

//---------------------------------------------------
void ReadabilityApp::UpdateDocumentThemes()
    {
    const auto& docs = GetDocManager()->GetDocuments();
    for (size_t i = 0; i < docs.GetCount(); ++i)
        {
        BaseProjectDoc* doc = dynamic_cast<BaseProjectDoc*>(docs.Item(i)->GetData());
        if (doc)
            {
            BaseProjectView* view = dynamic_cast<BaseProjectView*>(doc->GetFirstView());
            if (view)
                {
                UpdateRibbonTheme(view->GetRibbon());
                UpdateSideBarTheme(view->GetSideBar());
                view->GetQuickToolbar()->SetBackgroundColour(GetAppOptions().GetRibbonInactiveTabColor());
                view->GetSearchPanel()->SetBackgroundColour(GetAppOptions().GetRibbonInactiveTabColor());
                doc->GetDocumentWindow()->Refresh();
                }
            }
        }
    }

//---------------------------------------------------
void MainFrame::OnStartPageClick(wxCommandEvent& event)
    {
    if (GetStartPage()->IsCustomButtonId(event.GetId()))
        {
        if (event.GetId() == GetStartPage()->GetButtonID(0))
            { OpenFileNew(wxEmptyString); }
        else if (event.GetId() == GetStartPage()->GetButtonID(1))
            {
            wxFileDialog dialog
                    (this,
                    _(L"Select Project to Open"),
                    wxGetApp().GetAppOptions().GetProjectPath(),
                    wxEmptyString,
                    wxGetApp().GetLicenseAdmin().
                        IsFeatureEnabled(wxGetApp().FeatureProfessionalCode()) ?
                        _(L"Readability Studio Project (*.rsp;*.rsbp)|*.rsp;*.rsbp") :
                        _(L"Readability Studio Project (*.rsp)|*.rsp"),
                    wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_PREVIEW);
            if (dialog.ShowModal() == wxID_OK)
                {
                wxGetApp().GetAppOptions().SetProjectPath(wxFileName(dialog.GetPath()).GetPath());
                OpenFile(dialog.GetPath());
                }
            }
        else if (event.GetId() == GetStartPage()->GetButtonID(2))
            { DisplayHelp(_DT(L"getting-started.html")); }
        else if (event.GetId() == GetStartPage()->GetButtonID(3))
            { DisplayHelp(_DT(L"analysis-notes.html")); }
        else if (event.GetId() == GetStartPage()->GetButtonID(4))
            {
            const wxString manualPath = GetHelpDirectory() + wxFileName::GetPathSeparator() +
                _DT(L"ReadabilityStudioManual.pdf");
            wxLaunchDefaultApplication(manualPath);
            }
        else if (event.GetId() == GetStartPage()->GetButtonID(5))
            {
            ToolsOptionsDlg optionsDlg(this);
            optionsDlg.SelectPage(ToolsOptionsDlg::GENERAL_SETTINGS_PAGE);
            if (optionsDlg.ShowModal() == wxID_OK)
                { wxGetApp().GetAppOptions().SaveOptionsFile(); }
            }
        }
    else if (GetStartPage()->IsFileId(event.GetId()))
        {
        OpenFile(event.GetString());
        }
    else if (GetStartPage()->IsFileListClearId(event.GetId()))
        {
        wxGetApp().ClearFileHistoryMenu();
        }
    }

void MainFrame::OnNewDropdown(wxRibbonButtonBarEvent& event)
    {
    wxMenu menu;
    auto menuItem = new wxMenuItem(&menu,
        wxID_NEW, _(L"New Project...") + _DT(L"\tCtrl+N"));
    menuItem->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/document.svg"));
    menu.Append(menuItem);
    menu.Append(wxID_PASTE, _(L"New Project from Clipboard...") + _DT(L"\tCtrl+V"));
    event.PopupMenu(&menu);
    }

void MainFrame::OnOpenDropdown(wxRibbonButtonBarEvent& event)
    { event.PopupMenu(&m_fileOpenMenu); }

void MainFrame::OnPrintDropdown(wxRibbonButtonBarEvent& event)
    { event.PopupMenu(&m_printMenu); }

void MainFrame::OnCustomTestsDropdown(wxRibbonButtonBarEvent& event)
    { event.PopupMenu(&m_customTestsMenu); }

void MainFrame::OnTestBundlesDropdown(wxRibbonButtonBarEvent& event)
    { event.PopupMenu(&m_testsBundleMenu); }

void MainFrame::OnExampleDropdown(wxRibbonButtonBarEvent& event)
    { event.PopupMenu(&m_exampleMenu); }

void MainFrame::OnDictionaryDropdown(wxRibbonButtonBarEvent& event)
    {
    wxMenu menu;
    menu.Append(XRCID("ID_EDIT_ENGLISH_DICTIONARY"), _(L"Edit Custom Dictionary..."));
    menu.Append(XRCID("ID_EDIT_DICTIONARY_SETTINGS"), _(L"Settings..."));
    event.PopupMenu(&menu);
    }

void MainFrame::OnWordList([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    WordListDlg wordListsDlg(this);
    wordListsDlg.ShowModal();
    }

void MainFrame::OnWordListDropdown(wxRibbonButtonBarEvent& event)
    { event.PopupMenu(&m_wordListMenu); }

void MainFrame::OnBlankGraphDropdown(wxRibbonButtonBarEvent& event)
    { event.PopupMenu(&m_blankGraphMenu); }

//---------------------------------------------------
void MainFrame::OnEditEnglishDictionary([[maybe_unused]] wxCommandEvent& event)
    { wxGetApp().EditDictionary(readability::test_language::english_test); }

//---------------------------------------------------
void MainFrame::OnEditDictionarySettings([[maybe_unused]] wxCommandEvent& event)
    {
    ToolsOptionsDlg optionsDlg(this);
    optionsDlg.SelectPage(ToolsOptionsDlg::GRAMMAR_PAGE);
    if (optionsDlg.ShowModal() == wxID_OK)
        { wxGetApp().GetAppOptions().SaveOptionsFile(); }
    }

//-------------------------------------------------------
void MainFrame::OnOpenExample(wxCommandEvent& event)
    {
    const std::map<int, wxString>::const_iterator pos = MainFrame::GetExamplesMenuIds().find(event.GetId());
    if (pos == MainFrame::GetExamplesMenuIds().cend())
        { return; }
    const FilePathResolver fn(pos->second, false);
    // create a batch project from the example file
    if (FilePathResolver::IsSpreadsheet(fn.GetResolvedPath()) ||
        FilePathResolver::IsArchive(fn.GetResolvedPath()))
        {
        wxDocTemplate* docTemplate = wxGetApp().GetDocManager()->FindTemplate(CLASSINFO(BatchProjectDoc));
        if (docTemplate)
            {
            BatchProjectDoc* newDoc =
                dynamic_cast<BatchProjectDoc*>(docTemplate->CreateDocument(fn.GetResolvedPath(), wxDOC_NEW));
            if (newDoc && !newDoc->OnNewDocument() )
                {
                // Document is implicitly deleted by DeleteAllViews
                newDoc->DeleteAllViews();
                }
            if (newDoc && newDoc->GetFirstView() )
                {
                newDoc->GetFirstView()->Activate(true);
                wxGetApp().GetDocManager()->ActivateView(newDoc->GetFirstView());
                if (newDoc->GetDocumentWindow() )
                    { newDoc->GetDocumentWindow()->SetFocus(); }
                }
            }
        }
    // create a standard project from the example file
    else
        {
        wxArrayString choices;
        choices.push_back(_(L"Create a new project"));
        choices.push_back(_(L"View document"));
        wxArrayString descriptions;
        descriptions.push_back(_(L"Create a new project using the example document"));
        descriptions.push_back(_(L"View the document in your system's default editor"));
        RadioBoxDlg choiceDlg(this,
            _(L"Select how to open the example document"), wxEmptyString, wxEmptyString,
            _(L"Open Example Document"), choices, descriptions);
        if (choiceDlg.ShowModal() == wxID_CANCEL)
            { return; }
        if (choiceDlg.GetSelection() == 0)
            {
            wxDocTemplate* docTemplate = wxGetApp().GetDocManager()->FindTemplate(CLASSINFO(ProjectDoc));
            if (docTemplate)
                {
                ProjectDoc* newDoc =
                    dynamic_cast<ProjectDoc*>(docTemplate->CreateDocument(fn.GetResolvedPath(), wxDOC_NEW));
                if (newDoc && !newDoc->OnNewDocument() )
                    {
                    // Document is implicitly deleted by DeleteAllViews
                    newDoc->DeleteAllViews();
                    newDoc = nullptr;
                    }
                if (newDoc && newDoc->GetFirstView() )
                    {
                    newDoc->GetFirstView()->Activate(true);
                    wxGetApp().GetDocManager()->ActivateView(newDoc->GetFirstView());
                    if (newDoc->GetDocumentWindow() )
                        { newDoc->GetDocumentWindow()->SetFocus(); }
                    }
                }
            }
        else
            {
            wxLaunchDefaultApplication(fn.GetResolvedPath());
            }
        }
    }

//-------------------------------------------------------
void MainFrame::OnWordListByPage(wxCommandEvent& event)
    {
    WordListDlg wordListsDlg(this);
    // New Dale-Chall Words
    if (event.GetId() == XRCID("ID_DC_WORD_LIST_WINDOW"))
        { wordListsDlg.SelectPage(WordListDlg::DALE_CHALL_PAGE_ID); }
    // Stocker list
    else if (event.GetId() == XRCID("ID_STOCKER_CATHOLIC_WORD_LIST_WINDOW"))
        { wordListsDlg.SelectPage(WordListDlg::STOCKER_PAGE_ID); }
    // Spache list
    else if (event.GetId() == XRCID("ID_SPACHE_WORD_LIST_WINDOW"))
        { wordListsDlg.SelectPage(WordListDlg::SPACHE_PAGE_ID); }
    // Harris-Jacobson
    else if (event.GetId() == XRCID("ID_HARRIS_JACOBSON_WORD_LIST_WINDOW"))
        { wordListsDlg.SelectPage(WordListDlg::HARRIS_JACOBSON_PAGE_ID); }
     // Dolch words
    else if (event.GetId() == XRCID("ID_DOLCH_WORD_LIST_WINDOW"))
        { wordListsDlg.SelectPage(WordListDlg::DOLCH_PAGE_ID); }
    wordListsDlg.ShowModal();
    }

//-------------------------------------------------------
void MainFrame::OnScriptEditor([[maybe_unused]] wxCommandEvent& event)
    {
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureProfessionalCode()))
        {
        wxMessageBox(
            _(L"Lua Scripting is only available in the Professional Edition of Readability Studio."),
            _(L"Feature Not Licensed"), wxOK|wxICON_INFORMATION);
        return;
        }
    if (m_luaEditor == nullptr)
        {
        const wxSize screenSize{ wxSystemSettings::GetMetric(wxSystemMetric::wxSYS_SCREEN_X),
                                 wxSystemSettings::GetMetric(wxSystemMetric::wxSYS_SCREEN_Y) };
        m_luaEditor = new LuaEditorDlg(
            nullptr, wxID_ANY, _(L"Lua Script"), wxDefaultPosition,
            wxSize{ static_cast<int>(screenSize.GetWidth() * math_constants::third),
                    static_cast<int>(screenSize.GetHeight() * math_constants::three_quarters) });
        }
    m_luaEditor->SetThemeColor(wxGetApp().GetAppOptions().GetControlBackgroundColor());
    m_luaEditor->Show();
    m_luaEditor->SetFocus();
    }

//-------------------------------------------------------
void MainFrame::AddCustomTestToMenus(const wxString& testName)
    {
    const int menuId = CUSTOM_TEST_RANGE.GetNextId();
    if (menuId == wxNOT_FOUND)
        {
        wxMessageBox(
            _(L"Unable to add custom test to menu: not enough menu IDs, "
               "please contact software vendor to remove this limitation."),
            _(L"Error"), wxOK|wxICON_ERROR);
        return;
        }
    m_customTestMenuIds.insert(std::make_pair(menuId, testName));
    // set a unique ID for this test for use in the sidebar (this is different from the menu ID).
    CustomReadabilityTestCollection::iterator testIter =
        std::find(BaseProject::m_custom_word_tests.begin(), BaseProject::m_custom_word_tests.end(), testName);
    if (testIter != BaseProject::m_custom_word_tests.end())
        { testIter->set_interface_id(BaseProjectView::GetCustomTestSidebarIdRange().GetNextId()); }
    // add it to any open views' menus now
    auto& docs = wxGetApp().GetDocManager()->GetDocuments();
    for (size_t i = 0; i < docs.GetCount(); ++i)
        {
        BaseProjectDoc* doc = dynamic_cast<BaseProjectDoc*>(docs.Item(i)->GetData());
        BaseProjectView* view = dynamic_cast<BaseProjectView*>(doc->GetFirstView());
        FillMenuWithCustomTests(&view->GetDocFrame()->m_customTestsMenu, doc, true);
        FillMenuWithCustomTests(view->GetDocFrame()->m_customTestsRegularMenu, doc, true);
        }
    FillMenuWithCustomTests(&m_customTestsMenu, nullptr, false);
    FillMenuWithCustomTests(m_customTestsRegularMenu, nullptr, false);
    }

//-------------------------------------------------------
void MainFrame::AddTestBundleToMenus(const wxString& bundleName)
    {
    const int menuId = TEST_BUNDLE_RANGE.GetNextId();
    if (menuId == wxNOT_FOUND)
        {
        wxMessageBox(
            _(L"Unable to add test bundle to menu: not enough menu IDs, "
               "please contact software vendor to remove this limitation."),
            _(L"Error"), wxOK|wxICON_ERROR);
        return;
        }
    m_testBundleMenuIds.insert(std::make_pair(menuId, bundleName));
    // add it to any open views' menus now
    auto& docs = wxGetApp().GetDocManager()->GetDocuments();
    for (size_t i = 0; i < docs.GetCount(); ++i)
        {
        BaseProjectDoc* doc = dynamic_cast<BaseProjectDoc*>(docs.Item(i)->GetData());
        BaseProjectView* view = dynamic_cast<BaseProjectView*>(doc->GetFirstView());
        FillMenuWithTestBundles(&view->GetDocFrame()->m_testsBundleMenu, doc, true);
        FillMenuWithTestBundles(view->GetDocFrame()->m_testsBundleRegularMenu, doc, true);
        }
    FillMenuWithTestBundles(&m_testsBundleMenu, nullptr, false);
    FillMenuWithTestBundles(m_testsBundleRegularMenu, nullptr, false);
    }

//-------------------------------------------------------
void MainFrame::RemoveTestBundleFromMenus(const wxString& bundleName)
    {
   auto menuPos = m_testBundleMenuIds.begin();
    for (/*initialized already*/;
        menuPos != m_testBundleMenuIds.end();
        ++menuPos)
        {
        if (menuPos->second == bundleName)
            { break; }
        }
    if (menuPos == m_testBundleMenuIds.end())
        { return; }
    m_testBundleMenuIds.erase(menuPos);

    // remove it from any open views' menus now
    auto& docs = wxGetApp().GetDocManager()->GetDocuments();
    for (size_t i = 0; i < docs.GetCount(); ++i)
        {
        BaseProjectDoc* doc = dynamic_cast<BaseProjectDoc*>(docs.Item(i)->GetData());
        BaseProjectView* view = dynamic_cast<BaseProjectView*>(doc->GetFirstView());
        FillMenuWithTestBundles(&view->GetDocFrame()->m_testsBundleMenu, doc, true);
        FillMenuWithTestBundles(view->GetDocFrame()->m_testsBundleRegularMenu, doc, true);
        }
    FillMenuWithTestBundles(&m_testsBundleMenu, nullptr, false);
    FillMenuWithTestBundles(m_testsBundleRegularMenu, nullptr, false);
    }

//-------------------------------------------------------
void MainFrame::RemoveCustomTestFromMenus(const wxString& testName)
    {
    std::map<int, wxString>::iterator menuPos = m_customTestMenuIds.begin();
    for (/*initialized already*/;
        menuPos != m_customTestMenuIds.end();
        ++menuPos)
        {
        if (menuPos->second == testName)
            { break; }
        }
    if (menuPos == m_customTestMenuIds.end())
        { return; }
    m_customTestMenuIds.erase(menuPos);

    // remove it from any open views' menus now
    auto& docs = wxGetApp().GetDocManager()->GetDocuments();
    for (size_t i = 0; i < docs.GetCount(); ++i)
        {
        BaseProjectDoc* doc = dynamic_cast<BaseProjectDoc*>(docs.Item(i)->GetData());
        BaseProjectView* view = dynamic_cast<BaseProjectView*>(doc->GetFirstView());
        FillMenuWithCustomTests(&view->GetDocFrame()->m_customTestsMenu, doc, true);
        FillMenuWithCustomTests(view->GetDocFrame()->m_customTestsRegularMenu, doc, true);
        }
    FillMenuWithCustomTests(&m_customTestsMenu, nullptr, false);
    FillMenuWithCustomTests(m_customTestsRegularMenu, nullptr, false);
    }

//-------------------------------------------------------
void MainFrame::FillReadabilityMenu(wxMenu* primaryMenu, wxMenu* secondaryMenu,
                                    wxMenu* adultMenu, wxMenu* secondLanguageMenu,
                                    const BaseProject* project)
    {
    if (primaryMenu && secondaryMenu && adultMenu && secondLanguageMenu && project)
        {
        // clear the menus
        while (primaryMenu->GetMenuItemCount())
            { primaryMenu->Destroy(primaryMenu->FindItemByPosition(0)); }
        while (secondaryMenu->GetMenuItemCount())
            { secondaryMenu->Destroy(secondaryMenu->FindItemByPosition(0)); }
        while (adultMenu->GetMenuItemCount())
            { adultMenu->Destroy(adultMenu->FindItemByPosition(0)); }
        while (secondLanguageMenu->GetMenuItemCount())
            { secondLanguageMenu->Destroy(secondLanguageMenu->FindItemByPosition(0)); }
        // add the tests
        for (const auto& rTest : project->GetReadabilityTests().get_tests())
            {
            if (rTest.get_test().has_teaching_level(readability::test_teaching_level::primary_grade) &&
                rTest.get_test().has_language(project->GetProjectLanguage()))
                {
                wxMenuItem* testItem = new wxMenuItem(primaryMenu, rTest.get_test().get_interface_id(),
                    rTest.get_test().get_long_name().c_str(), rTest.get_test().get_long_name().c_str());
                const auto bp = wxGetApp().GetResourceManager().GetSVG(
                        wxString::Format(L"tests/%s.svg",
                        rTest.get_test().get_id().c_str()));
                if (bp.IsOk())
                    { testItem->SetBitmap(bp); }
                primaryMenu->Append(testItem);
                }
            if (rTest.get_test().has_teaching_level(readability::test_teaching_level::secondary_grade) &&
                rTest.get_test().has_language(project->GetProjectLanguage()))
                {
                wxMenuItem* testItem = new wxMenuItem(secondaryMenu, rTest.get_test().get_interface_id(),
                    rTest.get_test().get_long_name().c_str(), rTest.get_test().get_long_name().c_str());
                const auto bp = wxGetApp().GetResourceManager().GetSVG(
                            wxString::Format(L"tests/%s.svg",
                            rTest.get_test().get_id().c_str()));
                if (bp.IsOk())
                    { testItem->SetBitmap(bp); }
                secondaryMenu->Append(testItem);
                }
            if (rTest.get_test().has_teaching_level(readability::test_teaching_level::adult_level) &&
                rTest.get_test().has_language(project->GetProjectLanguage()))
                {
                wxMenuItem* testItem = new wxMenuItem(adultMenu, rTest.get_test().get_interface_id(),
                    rTest.get_test().get_long_name().c_str(), rTest.get_test().get_long_name().c_str());
                const auto bp = wxGetApp().GetResourceManager().GetSVG(
                                wxString::Format(L"tests/%s.svg",
                                rTest.get_test().get_id().c_str()));
                if (bp.IsOk())
                    { testItem->SetBitmap(bp); }
                adultMenu->Append(testItem);
                }
            if (rTest.get_test().has_teaching_level(readability::test_teaching_level::second_language) &&
                rTest.get_test().has_language(project->GetProjectLanguage()))
                {
                wxMenuItem* testItem = new wxMenuItem(secondLanguageMenu, rTest.get_test().get_interface_id(),
                    rTest.get_test().get_long_name().c_str(), rTest.get_test().get_long_name().c_str());
                const auto bp = wxGetApp().GetResourceManager().GetSVG(
                            wxString::Format(L"tests/%s.svg",
                            rTest.get_test().get_id().c_str()));
                if (bp.IsOk())
                    { testItem->SetBitmap(bp); }
                secondLanguageMenu->Append(testItem);
                }
            }
        // add Dolch
        if (project->GetProjectLanguage() == readability::test_language::english_test)
            {
            const auto bp = wxGetApp().GetResourceManager().GetSVG(L"tests/dolch.svg");
            assert(bp.IsOk());

                {
                wxMenuItem* dolchItem = new wxMenuItem(primaryMenu, XRCID("ID_DOLCH"),
                    _(L"Dolch Sight Words"), _(L"Dolch Sight Words"));
                dolchItem->SetBitmap(bp);
                primaryMenu->AppendSeparator();
                primaryMenu->Append(dolchItem);
                }

                {
                wxMenuItem* dolchItem = new wxMenuItem(secondLanguageMenu, XRCID("ID_DOLCH"),
                    _(L"Dolch Sight Words"), _(L"Dolch Sight Words"));
                dolchItem->SetBitmap(bp);
                secondLanguageMenu->AppendSeparator();
                secondLanguageMenu->Append(dolchItem);
                }
            }
        }
    }

//-------------------------------------------------------
void MainFrame::AddExamplesToMenu(wxMenu* exampleMenu)
    {
    if (exampleMenu)
        {
        wxDir dir;
        wxArrayString files;
        const wxString exampleFolder = wxGetApp().FindResourceDirectory(_DT(L"Examples"));
        if (!wxFileName::DirExists(exampleFolder) ||
            dir.GetAllFiles(exampleFolder, &files, wxEmptyString, wxDIR_FILES) == 0)
            {
            wxLogWarning(L"Unable to find examples folder:\n%s", exampleFolder);
            return;
            }
        files.Sort();
        // go through all the example files and add them to the menu
        for (size_t i = 0; i < files.GetCount(); ++i)
            {
            wxFileName fName(files[i]);
            // see if we already have a menu ID for this example file; otherwise, make a new ID
            auto menuPos = m_examplesMenuIds.begin();
            for (/*initialized already*/;
                menuPos != m_examplesMenuIds.end();
                ++menuPos)
                {
                if (menuPos->second == files[i])
                    { break; }
                }
            const int menuId = (menuPos == m_examplesMenuIds.end()) ? EXAMPLE_RANGE.GetNextId() : menuPos->first;
            // bail if we run out of menu IDs
            if (menuId == wxNOT_FOUND)
                { break; }
            if (menuPos == m_examplesMenuIds.end())
                { m_examplesMenuIds.insert(std::make_pair(menuId, files[i])); }
            if (exampleMenu->FindItem(menuId) == nullptr)
                { exampleMenu->Append(menuId, fName.GetName(), files[i]); }
            }
        }
    }

//-------------------------------------------------------
void MainFrame::FillMenuWithTestBundles(wxMenu* testBundleMenu, const BaseProject* project,
                                        const bool includeDocMenuItems)
    {
    if (testBundleMenu)
        {
        // clear the menu
        while (testBundleMenu->GetMenuItemCount())
            { testBundleMenu->Destroy(testBundleMenu->FindItemByPosition(0)); }
        if (includeDocMenuItems)
            {
            auto industryMenu = new wxMenu();
            industryMenu->Append(XRCID("ID_ADD_CHILDRENS_PUBLISHING_TEST_BUNDLE"), _(L"Children's Publishing"));
            industryMenu->Append(XRCID("ID_ADD_ADULT_PUBLISHING_TEST_BUNDLE"), _(L"Young Adult and Adult Publishing"));
            industryMenu->Append(XRCID("ID_ADD_CHILDRENS_HEALTHCARE_TEST_BUNDLE"), _(L"Children's Healthcare"));
            industryMenu->Append(XRCID("ID_ADD_ADULT_HEALTHCARE_TEST_BUNDLE"), _(L"Adult Healthcare"));
            industryMenu->Append(XRCID("ID_ADD_MILITARY_COVERNMENT_TEST_BUNDLE"), _(L"Military & Government"));
            industryMenu->Append(XRCID("ID_ADD_SECOND_LANGUAGE_TEST_BUNDLE"), _(L"Second Language Education"));
            industryMenu->Append(XRCID("ID_ADD_BROADCASTING_TEST_BUNDLE"), _(L"Broadcasting"));
            testBundleMenu->AppendSubMenu(industryMenu, _(L"Industry"));
            auto docTypeMenu = new wxMenu();
            docTypeMenu->Append(XRCID("ID_GENERAL_DOCUMENT_TEST_BUNDLE"), _(L"General Document"));
            docTypeMenu->Append(XRCID("ID_TECHNICAL_DOCUMENT_TEST_BUNDLE"), _(L"Technical Document"));
            docTypeMenu->Append(XRCID("ID_NONNARRATIVE_DOCUMENT_TEST_BUNDLE"), _(L"Structured Form"));
            docTypeMenu->Append(XRCID("ID_ADULT_LITERATURE_DOCUMENT_TEST_BUNDLE"), _(L"Literature"));
            docTypeMenu->Append(XRCID("ID_CHILDRENS_LITERATURE_DOCUMENT_TEST_BUNDLE"), _(L"Children's Literature"));
            testBundleMenu->AppendSubMenu(docTypeMenu, _(L"General Document Type"));
            }

        // add the bundles to the menu
        if (includeDocMenuItems && MainFrame::GetTestBundleMenuIds().size() > 0)
            {
            bool separatorNeeded = true;
            // add all of the global test bundles to this menu (if they aren't already on it)
            for (const auto& bundle : MainFrame::GetTestBundleMenuIds())
                {
                if (testBundleMenu->FindItem(bundle.first) == nullptr)
                    {
                    std::set<TestBundle>::const_iterator testIter =
                        BaseProject::m_testBundles.find(TestBundle(bundle.second.wc_str()));
                    if (testIter == BaseProject::m_testBundles.cend())
                        {
                        wxMessageBox(
                            _(L"Unable to add test bundle to menu: internal error, please contact software vendor."),
                            _(L"Error"), wxOK|wxICON_ERROR);
                        return;
                        }
                    // make sure that if the bundle has a language (meaning it is a standard system one),
                    // then make sure it matches the project.
                    if (project &&
                        (testIter->GetLanguage() == readability::test_language::unknown_language ||
                         testIter->GetLanguage() == project->GetProjectLanguage()) )
                        {
                        if (separatorNeeded)
                            {
                            testBundleMenu->AppendSeparator();
                            separatorNeeded = false;
                            }
                        wxMenuItem* item = new wxMenuItem(testBundleMenu, bundle.first, testIter->GetName().c_str());
                        if (testIter->GetName() == ReadabilityMessages::GetPskBundleName().wc_str())
                            {
                            item->SetBitmap(
                                wxGetApp().GetResourceManager().GetSVG(L"tests/psk-test.svg"));
                            }
                        else if (testIter->GetName() == ReadabilityMessages::GetKincaidNavyBundleName().wc_str())
                            {
                            item->SetBitmap(
                                wxGetApp().GetResourceManager().GetSVG(L"tests/flesch-kincaid-test.svg"));
                            }
                        else if (testIter->GetName() == ReadabilityMessages::GetConsentFormsBundleName().wc_str())
                            {
                            item->SetBitmap(
                                wxGetApp().GetResourceManager().GetSVG(L"ribbon/cdc-femaledoc.svg"));
                            }
                        testBundleMenu->Append(item);
                        }
                    }
                }
            }
        // Add/Edit/Remove options
        if (includeDocMenuItems)
            { testBundleMenu->AppendSeparator(); }
        wxMenuItem* menuItem = new wxMenuItem(testBundleMenu, XRCID("ID_ADD_CUSTOM_TEST_BUNDLE"), _(L"Add..."));
        menuItem->SetBitmap(
            wxGetApp().GetResourceManager().GetSVG(L"ribbon/add.svg"));
        testBundleMenu->Append(menuItem);
        // if there are editable bundles, then add menu items for them
        bool hasRemovableBundles{ false };
        for (const auto& bundle : BaseProject::m_testBundles)
            {
            if (!bundle.IsLocked())
                {
                hasRemovableBundles = true;
                break;
                }
            }
        // locked ones can still be viewed from the editor
        menuItem = new wxMenuItem(testBundleMenu, XRCID("ID_EDIT_CUSTOM_TEST_BUNDLE"), _(L"Edit..."));
        menuItem->SetBitmap(wxArtProvider::GetBitmapBundle(L"ID_EDIT"));
        testBundleMenu->Append(menuItem);
        if (hasRemovableBundles)
            {
            menuItem = new wxMenuItem(testBundleMenu, XRCID("ID_REMOVE_CUSTOM_TEST_BUNDLE"), _(L"Remove..."));
            menuItem->SetBitmap(wxArtProvider::GetBitmapBundle(wxART_DELETE));
            testBundleMenu->Append(menuItem);
            }
        }
    }

//-------------------------------------------------------
void MainFrame::FillMenuWithCustomTests(wxMenu* customTestMenu, const BaseProject* project,
                                        const bool includeDocMenuItems)
    {
    if (customTestMenu)
        {
        // clear the menu
        while (customTestMenu->GetMenuItemCount())
            { customTestMenu->Destroy(customTestMenu->FindItemByPosition(0)); }

        // if there are tests then add editing options and the tests themselves
        if (includeDocMenuItems && MainFrame::GetCustomTestMenuIds().size() > 0)
            {
            // add all of the global custom tests to this view's menu (if they aren't already on it)
            for (std::map<int, wxString>::const_iterator pos = MainFrame::GetCustomTestMenuIds().begin();
                pos != MainFrame::GetCustomTestMenuIds().end();
                ++pos)
                {
                if (customTestMenu->FindItem(pos->first) == nullptr)
                    {
                    CustomReadabilityTestCollection::const_iterator testIter =
                        std::find(BaseProject::m_custom_word_tests.begin(),
                                  BaseProject::m_custom_word_tests.end(), pos->second);
                    if (testIter == BaseProject::m_custom_word_tests.end())
                        {
                        wxMessageBox(
                            _(L"Unable to add custom test to menu: internal error, please contact software vendor."),
                            _(L"Error"), wxOK|wxICON_ERROR);
                        return;
                        }
                    wxMenuItem* item = new wxMenuItem(customTestMenu, pos->first, testIter->get_name().c_str());
                    customTestMenu->Append(item);
                    }
                }
            }
        if (includeDocMenuItems && MainFrame::GetCustomTestMenuIds().size() > 0)
            { customTestMenu->AppendSeparator(); }
        wxMenuItem* menuItem = new wxMenuItem(customTestMenu, XRCID("ID_ADD_CUSTOM_TEST"), _(L"Add..."));
        menuItem->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/add.svg"));
        customTestMenu->Append(menuItem);

        menuItem = new wxMenuItem(customTestMenu, XRCID("ID_ADD_CUSTOM_TEST_BASED_ON"),
                                  _(L"Add Custom Test Based on..."));
        menuItem->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/add.svg"));
        customTestMenu->Append(menuItem);
        if (!project ||
            project->GetProjectLanguage() == readability::test_language::english_test)
            {
            menuItem = new wxMenuItem(customTestMenu, XRCID("ID_ADD_CUSTOM_NEW_DALE_CHALL_TEST"),
                wxString::Format(_(L"Add Custom \"%s\"..."), _DT(L"New Dale-Chall")) );
            menuItem->SetBitmap(
                wxGetApp().GetResourceManager().GetSVG(L"tests/dale-chall-test.svg"));
            customTestMenu->Append(menuItem);

            menuItem = new wxMenuItem(customTestMenu, XRCID("ID_ADD_CUSTOM_SPACHE_TEST"),
                wxString::Format(_(L"Add Custom \"%s\"..."), _DT(L"Spache")) );
            menuItem->SetBitmap(
                wxGetApp().GetResourceManager().GetSVG(L"tests/spache-test.svg"));
            customTestMenu->Append(menuItem);

            menuItem = new wxMenuItem(customTestMenu, XRCID("ID_ADD_CUSTOM_HARRIS_JACOBSON_TEST"),
                wxString::Format(_(L"Add Custom \"%s\"..."), _DT(L"Harris-Jacobson")) );
            menuItem->SetBitmap(
                wxGetApp().GetResourceManager().GetSVG(L"tests/spache-test.svg"));
            customTestMenu->Append(menuItem);
            }

        // Remove or Edit options, depending on whether there are any tests
        if (MainFrame::GetCustomTestMenuIds().size() > 0)
            {
            customTestMenu->AppendSeparator();
            menuItem = new wxMenuItem(customTestMenu, XRCID("ID_EDIT_CUSTOM_TEST"), _(L"Edit..."));
            menuItem->SetBitmap(wxArtProvider::GetBitmapBundle(L"ID_EDIT"));
            customTestMenu->Append(menuItem);

            menuItem = new wxMenuItem(customTestMenu, XRCID("ID_REMOVE_CUSTOM_TEST"), _(L"Remove..."));
            menuItem->SetBitmap(wxArtProvider::GetBitmapBundle(wxART_DELETE));
            customTestMenu->Append(menuItem);
            }
        }
    }

//-------------------------------------------------------
void MainFrame::OnRemoveCustomTest([[maybe_unused]] wxCommandEvent& event)
    {
    // make sure there aren't any projects getting updated before we start changing these tests
    const auto& docs = wxGetApp().GetDocManager()->GetDocuments();
    for (size_t i = 0; i < docs.GetCount(); ++i)
        {
        const BaseProjectDoc* doc = dynamic_cast<BaseProjectDoc*>(docs.Item(i)->GetData());
        if (!doc->IsSafeToUpdate())
            { return; }
        }
    wxArrayString testNames;
    for (CustomReadabilityTestCollection::const_iterator pos = BaseProject::m_custom_word_tests.cbegin();
        pos != BaseProject::m_custom_word_tests.cend();
        ++pos)
        { testNames.Add(pos->get_name().c_str()); }
    wxSingleChoiceDialog dlg(this,
                             _(L"Select test to remove:"), _(L"Remove Test"),
                             testNames);
    dlg.SetSize(400, -1);
    dlg.Center();
    if (dlg.ShowModal() == wxID_CANCEL)
        { return; }
    const auto selectedTestIndex = dlg.GetSelection();
    if (selectedTestIndex < 0 ||
        selectedTestIndex >= static_cast<int>(BaseProject::m_custom_word_tests.size()))
        { return; }

    CustomReadabilityTest selectedTest =
        BaseProject::m_custom_word_tests[selectedTestIndex];

    BaseProjectDoc::RemoveGlobalCustomReadabilityTest(selectedTest.get_name().c_str());
    }

//-------------------------------------------------------
void MainFrame::OnAddCustomTestBundle([[maybe_unused]] wxCommandEvent& event)
    {
    TestBundle bundle(L"");
    TestBundleDlg bundleDlg(this, bundle);
    bundleDlg.SetHelpTopic(GetHelpDirectory(), L"test-bundles.html");
    if (bundleDlg.ShowModal() == wxID_OK)
        {
        BaseProject::m_testBundles.insert(bundle);
        dynamic_cast<MainFrame*>(wxGetApp().GetMainFrame())->AddTestBundleToMenus(bundle.GetName().c_str());
        }
    }

//-------------------------------------------------------
void MainFrame::OnEditCustomTestBundle([[maybe_unused]] wxCommandEvent& event)
    {
    wxArrayString bundleNames;
    for (std::set<TestBundle>::const_iterator pos = BaseProject::m_testBundles.begin();
        pos != BaseProject::m_testBundles.end();
        ++pos)
        { bundleNames.Add(pos->GetName().c_str()); }// locked bundled will be viewed as read only
    wxSingleChoiceDialog dlg(this, _(L"Select bundle to edit:"), _(L"Edit Bundle"), bundleNames);
    dlg.SetSize(400, -1);
    dlg.Center();
    if (dlg.ShowModal() == wxID_CANCEL)
        { return; }

    std::set<TestBundle>::iterator testBundleIter =
        BaseProject::m_testBundles.find(TestBundle(dlg.GetStringSelection().wc_str()));
    if (testBundleIter == BaseProject::m_testBundles.end())// shouldn't happen
        { return; }

    // set iterators cannot be edited directly. We will make a copy of the bundle and then reinsert it.
    TestBundle bundle = *testBundleIter;
    TestBundleDlg bundleDlg(this, bundle);
    bundleDlg.SetHelpTopic(GetHelpDirectory(), L"test-bundles.html");
    if (bundleDlg.ShowModal() == wxID_OK && !bundle.IsLocked())
        {
        BaseProject::m_testBundles.erase(testBundleIter);
        BaseProject::m_testBundles.insert(bundle);
        }
    }

//-------------------------------------------------------
void MainFrame::OnRemoveCustomTestBundle([[maybe_unused]] wxCommandEvent& event)
    {
    wxArrayString bundleNames;
    for (std::set<TestBundle>::const_iterator pos = BaseProject::m_testBundles.begin();
        pos != BaseProject::m_testBundles.end();
        ++pos)
        {
        if (!pos->IsLocked())
            { bundleNames.Add(pos->GetName().c_str()); }
        }
    wxSingleChoiceDialog dlg(this, _(L"Select bundle to remove:"), _(L"Remove Bundle"), bundleNames);
    dlg.SetSize(400, -1);
    dlg.Center();
    if (dlg.ShowModal() == wxID_CANCEL)
        { return; }

    std::set<TestBundle>::iterator testBundleIter =
        BaseProject::m_testBundles.find(TestBundle(dlg.GetStringSelection().wc_str()));
    if (testBundleIter == BaseProject::m_testBundles.end())// shouldn't happen
        { return; }
    BaseProject::m_testBundles.erase(testBundleIter);

    dynamic_cast<MainFrame*>(wxGetApp().GetMainFrame())->RemoveTestBundleFromMenus(dlg.GetStringSelection());
    }

//-------------------------------------------------------
void MainFrame::OnAddCustomTest(wxCommandEvent& event)
    {
    // make sure there aren't any projects getting updated before we start changing these tests
    const auto& docs = wxGetApp().GetDocManager()->GetDocuments();
    for (size_t i = 0; i < docs.GetCount(); ++i)
        {
        const BaseProjectDoc* doc = dynamic_cast<BaseProjectDoc*>(docs.Item(i)->GetData());
        if (!doc->IsSafeToUpdate())
            { return; }
        }
    if (event.GetId() == XRCID("ID_ADD_CUSTOM_NEW_DALE_CHALL_TEST"))
        {
        NewCustomWordTestSimpleDlg dlg(this,
            wxID_ANY, wxString::Format(_(L"Add Custom \"%s\""), _DT(L"New Dale-Chall")) );
        dlg.SetHelpTopic(GetHelpDirectory(), _DT(L"add-custom-word-test.html"));
        if (dlg.ShowModal() == wxID_OK)
            {
            CustomReadabilityTest cTest(dlg.GetTestName().wc_str(),
                    ReadabilityFormulaParser::GetCustomNewDaleChallSignature().wc_str(),
                    readability::readability_test_type::grade_level,
                    dlg.GetWordListFilePath().wc_str(),
                    stemming::stemming_type::no_stemming,
                    true,
                    true, &BaseProject::m_dale_chall_word_list,
                    false, &BaseProject::m_spache_word_list,
                    false, &BaseProject::m_harris_jacobson_word_list,
                    wxGetApp().GetAppOptions().IsIncludingStockerCatholicSupplement(),
                    &BaseProject::m_stocker_catholic_word_list,
                    false,
                    wxGetApp().GetAppOptions().GetDaleChallProperNounCountingMethod(),
                    true,
                    false, false, false, false, false, false, false, false, false, false, false, false);
            ProjectDoc::AddGlobalCustomReadabilityTest(cTest);
            }
        }
    else if (event.GetId() == XRCID("ID_ADD_CUSTOM_SPACHE_TEST"))
        {
        NewCustomWordTestSimpleDlg dlg(this,
            wxID_ANY, wxString::Format(_(L"Add Custom \"%s\""), _DT(L"Spache")) );
        dlg.SetHelpTopic(GetHelpDirectory(), _DT(L"add-custom-word-test.html"));
        if (dlg.ShowModal() == wxID_OK)
            {
            CustomReadabilityTest cTest(dlg.GetTestName().wc_str(),
                    ReadabilityFormulaParser::GetCustomSpacheSignature().wc_str(),
                    readability::readability_test_type::grade_level,
                    dlg.GetWordListFilePath().wc_str(),
                    stemming::stemming_type::no_stemming,
                    true,
                    false, &BaseProject::m_dale_chall_word_list,
                    true, &BaseProject::m_spache_word_list,
                    false, &BaseProject::m_harris_jacobson_word_list,
                    false, &BaseProject::m_stocker_catholic_word_list,
                    false,
                    readability::proper_noun_counting_method::all_proper_nouns_are_familiar,
                    true,
                    false, false, false, false, false, false, false, false, false, false, false, false);
            ProjectDoc::AddGlobalCustomReadabilityTest(cTest);
            }
        }
    else if (event.GetId() == XRCID("ID_ADD_CUSTOM_TEST_BASED_ON"))
        {
        wxArrayString tests;
        for (const auto& rTest : BaseProject::GetDefaultReadabilityTestsTemplate().get_tests())
            {
            if (rTest.get_formula().length())
                { tests.push_back(rTest.get_long_name().c_str()); }
            }
        wxSingleChoiceDialog lDlg(this,
            _(L"Select a linear-regression-based test to use as the basis for a new custom test:"),
            _(L"Select a Test"), tests);
        if (lDlg.ShowModal() == wxID_OK)
            {
            auto selected = lDlg.GetStringSelection();
            if (selected.length())
                {
                auto [testIterator, found] = BaseProject::GetDefaultReadabilityTestsTemplate().find_test(selected);
                if (found)
                    {
                    CustomTestDlg dlg(this);
                    dlg.SetTestName(_(L"Custom ") + testIterator->get_long_name().c_str());
                    dlg.SetFormula(testIterator->get_formula().c_str());
                    dlg.SetTestType(static_cast<int>(testIterator->get_test_type()));
                    if (dlg.ShowModal() == wxID_OK)
                        {
                        CustomReadabilityTest cTest(dlg.GetTestName().wc_str(),
                                dlg.GetFormula().wc_str(),
                                static_cast<readability::readability_test_type>(dlg.GetTestType()),
                                dlg.GetWordListFilePath().wc_str(),
                                dlg.GetStemmingType(),
                                dlg.IsIncludingCustomWordList(),
                                dlg.IsIncludingDaleChallList(), &BaseProject::m_dale_chall_word_list,
                                dlg.IsIncludingSpacheList(), &BaseProject::m_spache_word_list,
                                dlg.IsIncludingHJList(), &BaseProject::m_harris_jacobson_word_list,
                                dlg.IsIncludingStockerList(), &BaseProject::m_stocker_catholic_word_list,
                                dlg.IsFamiliarWordsMustBeOnEachIncludedList(),
                                static_cast<readability::proper_noun_counting_method>(dlg.GetProperNounMethod()),
                                dlg.IsIncludingNumeric(),
                                dlg.IsChildrensPublishingSelected(),
                                dlg.IsAdultPublishingSelected(), dlg.IsSecondaryLanguageSelected(),
                                dlg.IsChildrensHealthCareTestSelected(), dlg.IsAdultHealthCareTestSelected(),
                                dlg.IsMilitaryTestSelected(), dlg.IsBroadcastingSelected(),
                                dlg.IsGeneralDocumentSelected(),
                                dlg.IsTechnicalDocumentSelected(), dlg.IsNonNarrativeFormSelected(),
                                dlg.IsYoungAdultAndAdultLiteratureSelected(),
                                dlg.IsChildrensLiteratureSelected());
                        ProjectDoc::AddGlobalCustomReadabilityTest(cTest);
                        }
                    }
                else
                    {
                    wxMessageBox(wxString::Format(_(L"Internal error: unable to find '%s'."), selected[0]),
                        wxGetApp().GetAppName(), wxOK | wxICON_WARNING);
                    }
                }
            }
        }
    else if (event.GetId() == XRCID("ID_ADD_CUSTOM_HARRIS_JACOBSON_TEST"))
        {
        NewCustomWordTestSimpleDlg dlg(this,
            wxID_ANY, wxString::Format(_(L"Add Custom \"%s\""), _DT(L"Harris-Jacobson")) );
        dlg.SetHelpTopic(GetHelpDirectory(), _DT(L"add-custom-word-test.html"));
        if (dlg.ShowModal() == wxID_OK)
            {
            CustomReadabilityTest cTest(dlg.GetTestName().wc_str(),
                    ReadabilityFormulaParser::GetCustomHarrisJacobsonSignature().wc_str(),
                    readability::readability_test_type::grade_level,
                    dlg.GetWordListFilePath().wc_str(),
                    stemming::stemming_type::no_stemming,
                    true,
                    false, &BaseProject::m_dale_chall_word_list,
                    false, &BaseProject::m_spache_word_list,
                    true, &BaseProject::m_harris_jacobson_word_list,
                    false, &BaseProject::m_stocker_catholic_word_list,
                    false,
                    readability::proper_noun_counting_method::all_proper_nouns_are_familiar,
                    true,
                    false, false, false, false, false, false, false, false, false, false, false, false);
            ProjectDoc::AddGlobalCustomReadabilityTest(cTest);
            }
        }
    else
        {
        CustomTestDlg dlg(this);
        if (dlg.ShowModal() == wxID_OK)
            {
            CustomReadabilityTest cTest(dlg.GetTestName().wc_str(),
                    dlg.GetFormula().wc_str(),
                    static_cast<readability::readability_test_type>(dlg.GetTestType()),
                    dlg.GetWordListFilePath().wc_str(),
                    dlg.GetStemmingType(),
                    dlg.IsIncludingCustomWordList(),
                    dlg.IsIncludingDaleChallList(), &BaseProject::m_dale_chall_word_list,
                    dlg.IsIncludingSpacheList(), &BaseProject::m_spache_word_list,
                    dlg.IsIncludingHJList(), &BaseProject::m_harris_jacobson_word_list,
                    dlg.IsIncludingStockerList(), &BaseProject::m_stocker_catholic_word_list,
                    dlg.IsFamiliarWordsMustBeOnEachIncludedList(),
                    static_cast<readability::proper_noun_counting_method>(dlg.GetProperNounMethod()),
                    dlg.IsIncludingNumeric(),
                    dlg.IsChildrensPublishingSelected(),
                    dlg.IsAdultPublishingSelected(), dlg.IsSecondaryLanguageSelected(),
                    dlg.IsChildrensHealthCareTestSelected(), dlg.IsAdultHealthCareTestSelected(),
                    dlg.IsMilitaryTestSelected(), dlg.IsBroadcastingSelected(),
                    dlg.IsGeneralDocumentSelected(),
                    dlg.IsTechnicalDocumentSelected(), dlg.IsNonNarrativeFormSelected(),
                    dlg.IsYoungAdultAndAdultLiteratureSelected(),
                    dlg.IsChildrensLiteratureSelected());
            ProjectDoc::AddGlobalCustomReadabilityTest(cTest);
            }
        }
    }

//-------------------------------------------------------
void MainFrame::OnEditCustomTest([[maybe_unused]] wxCommandEvent& event)
    {
    // make sure there aren't any projects getting updated before we start changing these tests
    const auto& docs = wxGetApp().GetDocManager()->GetDocuments();
    for (size_t i = 0; i < docs.GetCount(); ++i)
        {
        const BaseProjectDoc* doc = dynamic_cast<BaseProjectDoc*>(docs.Item(i)->GetData());
        if (!doc->IsSafeToUpdate())
            { return; }
        }
    wxArrayString testNames;
    for (CustomReadabilityTestCollection::const_iterator pos = BaseProject::m_custom_word_tests.begin();
        pos != BaseProject::m_custom_word_tests.end();
        ++pos)
        { testNames.Add(pos->get_name().c_str()); }
    wxSingleChoiceDialog selDlg(this,
                             _(L"Select test to edit:"), _(L"Edit Test"),
                             testNames);
    selDlg.SetSize(400, -1);
    selDlg.Center();
    if (selDlg.ShowModal() == wxID_CANCEL)
        { return; }
    const auto selectedTestIndex = selDlg.GetSelection();
    if (selectedTestIndex < 0 ||
        selectedTestIndex >= static_cast<int>(BaseProject::m_custom_word_tests.size()))
        { return; }

    wxGetApp().EditCustomTest(BaseProject::m_custom_word_tests[selectedTestIndex]);
    }

//-------------------------------------------------------
void MainFrame::Paste()
    {
    if (wxTheClipboard->Open())
        {
        if (wxTheClipboard->IsSupported(wxDF_TEXT))
            {
            wxTextDataObject data;
            wxTheClipboard->GetData(data);
            // create a standard project and dump the text into it
            const auto& templateList = m_docManager->GetTemplates();
            for (size_t i = 0; i < templateList.GetCount(); ++i)
                {
                wxDocTemplate* docTemplate = dynamic_cast<wxDocTemplate*>(templateList.Item(i)->GetData());
                if (docTemplate && docTemplate->GetDocClassInfo()->IsKindOf(CLASSINFO(ProjectDoc)))
                    {
                    ProjectDoc* newDoc =
                        dynamic_cast<ProjectDoc*>(docTemplate->CreateDocument(data.GetText(), wxDOC_NEW));
                    if (newDoc && !newDoc->OnNewDocument() )
                        {
                        // Document is implicitly deleted by DeleteAllViews
                        newDoc->DeleteAllViews();
                        newDoc = nullptr;
                        }
                    if (newDoc && newDoc->GetFirstView() )
                        {
                        newDoc->GetFirstView()->Activate(true);
                        wxGetApp().GetDocManager()->ActivateView(newDoc->GetFirstView());
                        if (newDoc->GetDocumentWindow() )
                            { newDoc->GetDocumentWindow()->SetFocus(); }
                        }
                    break;
                    }
                }
            }
        else if (wxTheClipboard->IsSupported(wxDF_FILENAME))
            {
            wxFileDataObject data;
            wxTheClipboard->GetData(data);
            wxArrayString files = data.GetFilenames();
            for (size_t i = 0; i < files.GetCount(); ++i)
                {
                m_docManager->CreateDocument(files[i], wxDOC_NEW);
                }
            }
        else
            {
            wxMessageBox(_(L"No text is currently on the clipboard."),
                    wxGetApp().GetAppName(), wxOK|wxICON_INFORMATION);
            }
        wxTheClipboard->Close();
        }
    }

//-------------------------------------------------------
void MainFrame::OnPaste([[maybe_unused]] wxCommandEvent& event)
    { Paste(); }

//-------------------------------------------------------
void MainFrame::OnPrinterHeaderFooter([[maybe_unused]] wxCommandEvent& event)
    {
    PrinterHeaderFooterDlg dlg(this, wxGetApp().GetAppOptions().GetLeftPrinterHeader(),
        wxGetApp().GetAppOptions().GetCenterPrinterHeader(), wxGetApp().GetAppOptions().GetRightPrinterHeader(),
        wxGetApp().GetAppOptions().GetLeftPrinterFooter(), wxGetApp().GetAppOptions().GetCenterPrinterFooter(),
        wxGetApp().GetAppOptions().GetRightPrinterFooter());
    dlg.SetHelpTopic(GetHelpDirectory(), _DT(L"printing.html"));
    if (dlg.ShowModal() == wxID_OK)
        {
        wxGetApp().GetAppOptions().SetLeftPrinterHeader(dlg.GetLeftPrinterHeader());
        wxGetApp().GetAppOptions().SetCenterPrinterHeader(dlg.GetCenterPrinterHeader());
        wxGetApp().GetAppOptions().SetRightPrinterHeader(dlg.GetRightPrinterHeader());
        wxGetApp().GetAppOptions().SetLeftPrinterFooter(dlg.GetLeftPrinterFooter());
        wxGetApp().GetAppOptions().SetCenterPrinterFooter(dlg.GetCenterPrinterFooter());
        wxGetApp().GetAppOptions().SetRightPrinterFooter(dlg.GetRightPrinterFooter());
        }
    }

//-------------------------------------------------------
void MainFrame::OnClose(wxCloseEvent& event)
    {
    const auto& docs = wxGetApp().GetDocManager()->GetDocuments();
    for (size_t i = 0; i < docs.GetCount(); ++i)
        {
        const BaseProjectDoc* doc = dynamic_cast<BaseProjectDoc*>(docs.Item(i)->GetData());
        if (doc->IsProcessing())
            {
            wxMessageBox(_(L"Project still processing. Please wait before closing."),
                wxFileName::StripExtension(doc->GetTitle()), wxOK|wxICON_EXCLAMATION);
            event.Veto();
            }
        }
    wxGetApp().GetAppOptions().SetAppWindowMaximized(IsMaximized());
    wxGetApp().GetAppOptions().SetAppWindowWidth(GetSize().GetWidth());
    wxGetApp().GetAppOptions().SetAppWindowHeight(GetSize().GetHeight());
    wxGetApp().GetAppOptions().SetPaperId(static_cast<int>(wxGetApp().GetPrintData()->GetPaperId()));
    wxGetApp().GetAppOptions().SetPaperOrientation(wxGetApp().GetPrintData()->GetOrientation());
    event.Skip();
    }

//-------------------------------------------------------
void MainFrame::OnOpenDocument([[maybe_unused]] wxCommandEvent& event)
    {
    wxFileDialog dialog(
            this,
            _(L"Select Document to Analyze"),
            wxEmptyString,
            wxEmptyString,
            wxGetApp().GetAppOptions().GetDocumentFilter(),
            wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_PREVIEW);
    if (dialog.ShowModal() == wxID_CANCEL)
        { return; }
    const auto& templateList = wxGetApp().GetDocManager()->GetTemplates();
    for (size_t i = 0; i < templateList.GetCount(); ++i)
        {
        wxDocTemplate* docTemplate = dynamic_cast<wxDocTemplate*>(templateList.Item(i)->GetData());
        if (docTemplate && docTemplate->GetDocClassInfo()->IsKindOf(CLASSINFO(ProjectDoc)))
            {
            ProjectDoc* newDoc =
                dynamic_cast<ProjectDoc*>(docTemplate->CreateDocument(dialog.GetPath(), wxDOC_NEW));
            if (newDoc && !newDoc->OnNewDocument() )
                {
                // Document is implicitly deleted by DeleteAllViews
                newDoc->DeleteAllViews();
                newDoc = nullptr;
                }
            if (newDoc && newDoc->GetFirstView() )
                {
                newDoc->GetFirstView()->Activate(true);
                wxGetApp().GetDocManager()->ActivateView(newDoc->GetFirstView());
                if (newDoc->GetDocumentWindow() )
                    { newDoc->GetDocumentWindow()->SetFocus(); }
                }
            break;
            }
        }
    }

//-------------------------------------------------------
void MainFrame::OnHelpContents([[maybe_unused]] wxCommandEvent& event)
    {
    const BaseProjectDoc* activeProject =
        dynamic_cast<const BaseProjectDoc*>(GetDocumentManager()->GetCurrentDocument());
    if (!activeProject)
        { DisplayHelp(); }
    else if (activeProject->IsKindOf(CLASSINFO(ProjectDoc)))
        {
        const ProjectView* view = dynamic_cast<const ProjectView*>(activeProject->GetFirstView());

        switch (view->GetActiveProjectWindow()->GetId())
            {
        case BaseProjectView::HARD_WORDS_LIST_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::LONG_WORDS_LIST_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::DC_WORDS_LIST_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::SPACHE_WORDS_LIST_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::HARRIS_JACOBSON_WORDS_LIST_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::ALL_WORDS_LIST_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::WORD_BREAKDOWN_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::SYLLABLE_HISTOGRAM_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::SYLLABLE_PIECHART_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::HARD_WORDS_TEXT_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::LONG_WORDS_TEXT_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::DC_WORDS_TEXT_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::SPACHE_WORDS_TEXT_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::HARRIS_JACOBSON_WORDS_TEXT_PAGE_ID:
            DisplayHelp(_DT(L"reviewing-word-breakdowns.html"));
            break;
        case BaseProjectView::FLESCH_CHART_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::DB2_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::FRY_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::RAYGOR_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::CRAWFORD_GRAPH_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::FRASE_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::GPM_FRY_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::SCHWARTZ_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::LIX_GAUGE_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::LIX_GAUGE_GERMAN_PAGE_ID:
            DisplayHelp(_DT(L"reviewing-readability-graphs.html"));
            break;
        case BaseProjectView::STATS_REPORT_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::STATS_LIST_PAGE_ID:
            DisplayHelp(_DT(L"reviewing-statistics.html"));
            break;
        case BaseProjectView::SENTENCE_BOX_PLOT_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::SENTENCE_HISTOGRAM_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::LONG_SENTENCES_LIST_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::SENTENCE_HEATMAP_PAGE_ID:
            DisplayHelp(_DT(L"reviewing-sentences-breakdown.html"));
            break;
        case BaseProjectView::WORD_CLOUD_PAGE_ID:
            DisplayHelp(_DT(L"reviewing-word-breakdowns.html"));
            break;
        case BaseProjectView::READABILITY_SCORES_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::READABILITY_SCORES_SUMMARY_REPORT_PAGE_ID:
            DisplayHelp(_DT(L"reviewing-test-scores.html"));
            break;
        case BaseProjectView::LONG_SENTENCES_AND_WORDINESS_TEXT_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::SENTENCES_CONJUNCTION_START_LIST_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::SENTENCES_LOWERCASE_START_LIST_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::DUPLICATES_LIST_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::INCORRECT_ARTICLE_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::OVERUSED_WORDS_BY_SENTENCE_LIST_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::MISSPELLED_WORD_LIST_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::PASSIVE_VOICE_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::WORDING_ERRORS_LIST_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::REDUNDANT_PHRASE_LIST_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::WORDY_PHRASES_LIST_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::CLICHES_LIST_PAGE_ID:
            DisplayHelp(_DT(L"reviewing-standard-grammar.html"));
            break;
        case BaseProjectView::DOLCH_STATS_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::DOLCH_COVERAGE_CHART_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::DOLCH_BREAKDOWN_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::DOLCH_WORDS_TEXT_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::NON_DOLCH_WORDS_TEXT_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::UNUSED_DOLCH_WORDS_LIST_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::NON_DOLCH_WORDS_LIST_PAGE_ID:
            [[fallthrough]];
        case BaseProjectView::DOLCH_WORDS_LIST_PAGE_ID:
            DisplayHelp(_DT(L"reviewing-dolch.html"));
            break;
        default:
            DisplayHelp(_DT(L"reviewing-word-breakdowns.html"));
            };
        }
    else if (activeProject->IsKindOf(CLASSINFO(BatchProjectDoc)))
        {
        const BatchProjectView* view = dynamic_cast<const BatchProjectView*>(activeProject->GetFirstView());
        const auto selectedID = view->GetSideBar()->GetSelectedFolderId();
        if (!selectedID.has_value())
            { DisplayHelp(_DT(L"reviewing-batch-scores.html")); }
        else if (selectedID == BatchProjectView::SIDEBAR_READABILITY_SCORES_SECTION_ID)
            { DisplayHelp(_DT(L"reviewing-batch-scores.html")); }
        else if (selectedID == BatchProjectView::SIDEBAR_HISTOGRAMS_SECTION_ID)
            { DisplayHelp(_DT(L"reviewing-batch-histograms.html")); }
        else if (selectedID == BatchProjectView::SIDEBAR_BOXPLOTS_SECTION_ID)
            { DisplayHelp(_DT(L"reviewing-batch-box-plots.html")); }
        else if (selectedID == BatchProjectView::SIDEBAR_WORDS_BREAKDOWN_SECTION_ID)
            { DisplayHelp(_DT(L"reviewing-batch-words.html")); }
        else if (selectedID == BatchProjectView::SIDEBAR_DOLCH_SECTION_ID)
            { DisplayHelp(_DT(L"reviewing-batch-dolch.html")); }
        else if (selectedID == BatchProjectView::SIDEBAR_WARNINGS_SECTION_ID)
            { DisplayHelp(_DT(L"reviewing-batch-warnings.html")); }
        else if (selectedID == BatchProjectView::SIDEBAR_SENTENCES_BREAKDOWN_SECTION_ID)
            {
            switch (view->GetActiveProjectWindow()->GetId())
                {
            case BaseProjectView::LONG_SENTENCES_LIST_PAGE_ID:
                DisplayHelp(_DT(L"reviewing-batch-long-sentences.html"));
                break;
                };
            }
        else if (selectedID == BatchProjectView::SIDEBAR_GRAMMAR_SECTION_ID)
            { DisplayHelp(_DT(L"reviewing-batch-grammar.html")); }
        }
    }

//-------------------------------------------------------
void MainFrame::OnHelpManual([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    const wxString manualPath =
        GetHelpDirectory() + wxFileName::GetPathSeparator() + _DT(L"ReadabilityStudioManual.pdf");
    wxLaunchDefaultApplication(manualPath);
    }

//-------------------------------------------------------
void MainFrame::OnHelpCheckForUpdates([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    wxString updateFileContent, contentType, statusText;
#ifdef __WXOSX__
    wxString updatedFilePath =
        _DT(L"http:// oleandersoftware.com/downloads/readabilitystudio/CurrentMacVersionReadabilityStudio.txt");
    int responseCode;
    if (!WebHarvester::ReadWebPage(updatedFilePath, updateFileContent, contentType, statusText, responseCode, false) )
        {
        updatedFilePath =
            _DT(L"https:// oleandersoftware.com/downloads/readabilitystudio/CurrentVersionReadabilityStudio.txt");
        if (!WebHarvester::ReadWebPage(updatedFilePath, updateFileContent,
                                       contentType, statusText, responseCode, false) )
            {
            wxMessageBox(wxString::Format(_(L"An error occurred while trying to connect to the website:\t%s"),
                                            QueueDownload::GetResponseMessage(responseCode)),
                        _(L"Connection Error"), wxOK|wxICON_EXCLAMATION);
            return;
            }
        }
#else
    wxString updatedFilePath =
        _DT(L"https:// oleandersoftware.com/downloads/readabilitystudio/CurrentVersionReadabilityStudio.txt");
    int responseCode;
    if (!wxGetApp().GetWebHarvester().ReadWebPage(updatedFilePath, updateFileContent,
                                                  contentType, statusText, responseCode, false) )
        {
        wxMessageBox(wxString::Format(_(L"An error occurred while trying to connect to the website:\t%s"),
                                        QueueDownload::GetResponseMessage(responseCode)),
                    _(L"Connection Error"), wxOK|wxICON_EXCLAMATION);
        return;
        }
#endif

    wxString currentVersion = updateFileContent.Strip(wxString::both);

    if (wxGetApp().GetAppVersion() < currentVersion)
        {
        if (wxMessageBox(wxString::Format(
            _(L"There is a new version of %s currently available.\nDo you wish to go to the download page?"),
            wxGetApp().GetAppName()),
            wxGetApp().GetAppName(), wxYES_NO|wxICON_QUESTION) == wxYES)
            {
            if (!::wxLaunchDefaultBrowser(_DT(L"https:// oleandersoftware.com/readabilitystudioupdate.html")))
                {
                wxMessageBox(
                    _(L"Unable to open default browser. Please make sure that you have an Internet browser "
                       "installed and are connected to the Internet."),
                    _(L"Error"), wxOK|wxICON_ERROR);
                }
            }
        }
    else
        {
        wxMessageBox(_(L"There are no updates currently available."),
            wxGetApp().GetAppName(), wxOK|wxICON_INFORMATION);
        }
    }

//-------------------------------------------------------
void MainFrame::OnToolsOptions([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    ToolsOptionsDlg optionsDlg(this);
    if (optionsDlg.ShowModal() == wxID_OK)
        {
        wxGetApp().GetAppOptions().SaveOptionsFile();
        }
    }

//-------------------------------------------------------
void MainFrame::OnEditWordList([[maybe_unused]] wxCommandEvent& event)
    {
    EditWordListDlg editDlg(this, wxID_ANY, _(L"Edit Word List"));
    editDlg.SetDefaultDir(wxGetApp().GetAppOptions().GetWordListPath());
    editDlg.SetHelpTopic(GetHelpDirectory(), _DT(L"document-analysis.html"));
    if (editDlg.ShowModal() == wxID_OK)
        {
        wxGetApp().GetAppOptions().SetWordListPath(wxFileName(editDlg.GetFilePath()).GetPath());
        }
    }

//-------------------------------------------------------
void MainFrame::OnEditPhraseList([[maybe_unused]] wxCommandEvent& event)
    {
    EditWordListDlg editDlg(this, wxID_ANY, _(L"Edit Phrase List"));
    editDlg.SetDefaultDir(wxGetApp().GetAppOptions().GetWordListPath());
    editDlg.SetHelpTopic(GetHelpDirectory(), _DT(L"document-analysis.html"));
    editDlg.SetPhraseFileMode(true);
    editDlg.ShowModal();
    }

//-------------------------------------------------------
void MainFrame::OnFindDuplicateFiles([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    GetDirFilterDialog dirDlg(this, wxGetApp().GetAppOptions().GetDocumentFilter() + L"|" +
                                        wxGetApp().GetAppOptions().GetImageFileFilter() + L"|" +
                                        _(L"All Files (*.*)|*.*"));
    if (dirDlg.ShowModal() != wxID_OK || dirDlg.GetPath().empty())
        {
        return;
        }

    wxWindowDisabler disableAll;
    // get the list of files
    wxArrayString files;
        {
        wxBusyInfo wait(_(L"Retrieving files..."));
        wxDir::GetAllFiles(dirDlg.GetPath(), &files, wxEmptyString,
                           dirDlg.IsRecursive() ? wxDIR_FILES | wxDIR_DIRS : wxDIR_FILES);
        files = FilterFiles(files, ExtractExtensionsFromFileFilter(dirDlg.GetSelectedFileFilter()));
        }

    // get the checksums
    multi_value_aggregate_map<std::uint32_t, wxString> filesMap;
        {
        wxProgressDialog progressDlg(
            _(L"Duplicate Files"), _(L"Searching for duplicate files..."), files.size(), this,
            wxPD_AUTO_HIDE | wxPD_SMOOTH | wxPD_ELAPSED_TIME | wxPD_ESTIMATED_TIME |
                wxPD_REMAINING_TIME | wxPD_CAN_ABORT | wxPD_APP_MODAL);
        progressDlg.Centre();

        int counter{ 1 };
        for (const auto& curFile : files)
            {
            progressDlg.SetTitle(wxString::Format(
                _(L"Processing %s of %s files..."),
                wxNumberFormatter::ToString(counter, 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                wxNumberFormatter::ToString(files.size(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep)));
            if (!progressDlg.Update(counter, wxFileName(curFile).GetFullName()))
                {
                return;
                }
            MemoryMappedFile memFile(curFile, true, true);
            if (memFile.IsOk())
                {
                const std::uint32_t crc =
                    CRC::Calculate(memFile.GetStream(), memFile.GetMapSize(), CRC::CRC_32());
                filesMap.insert(crc, curFile);
                }
            ++counter;
            }
        }

    FileListDlg fileListDlg(this, wxID_ANY, _(L"Duplicate Files"));
    fileListDlg.GetListCtrl()->SetVirtualDataSize(files.size());
    size_t rowCount{ 0 };
    fileListDlg.GetListCtrl()->SetForegroundColour(*wxBLACK);
        // catalogue duplicates
        {
        wxBusyInfo wait(_(L"Loading duplicates..."));
        unsigned long groupId{ 1 };
        bool alternatingColor{ true };
        for (const auto& mapVal : filesMap.get_data())
            {
            if (mapVal.second.second > 1)
                {
                for (const auto& curFile : mapVal.second.first)
                    {
                    const wxFileName fn(curFile);
                    wxItemAttr attribs;
                    attribs.SetBackgroundColour(
                        (alternatingColor ? (*wxGREEN).ChangeLightness(160) : *wxWHITE));
                    fileListDlg.GetListCtrlData()->SetRowAttributes(rowCount, attribs);
                    fileListDlg.GetListCtrlData()->SetItemText(rowCount, 0, fn.GetFullName());
                    fileListDlg.GetListCtrlData()->SetItemText(rowCount, 1, fn.GetPath());
                    fileListDlg.GetListCtrlData()->SetItemValue(rowCount++, 2, groupId);
                    }
                // flip it for next group of duplicates
                alternatingColor = !alternatingColor;
                ++groupId;
                }
            }
        fileListDlg.GetListCtrl()->SetVirtualDataSize(rowCount);
        fileListDlg.GetListCtrl()->DistributeColumns(-1);
        fileListDlg.GetInforBar()->ShowMessage(
            wxString::Format(
                _(L"Found %lu duplicate files. You can select and delete duplicates from a group, "
                  "leaving one file for the group."),
                static_cast<decltype(groupId)>(fileListDlg.GetListCtrl()->GetItemCount()) -
                    (groupId - 1)),
            wxICON_INFORMATION);
        }
    if (rowCount == 0)
        {
        wxMessageBox(_(L"No duplicate files found."), wxGetApp().GetAppDisplayName(),
                     wxICON_INFORMATION | wxOK);
        }
    else
        {
        fileListDlg.ShowModal();
        }
    }

//-------------------------------------------------------
void MainFrame::OnToolsChapterSplit([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    wxFileDialog dialog
                (this,
                _(L"Select File to Split"),
                wxGetApp().GetAppOptions().GetProjectPath(),
                wxEmptyString,
                wxGetApp().GetAppOptions().GetDocumentFilter(),
                wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_PREVIEW);

    if (dialog.ShowModal() == wxID_OK)
        {
        ChapterSplit cSplit;
        cSplit.SetOutputFolder(wxFileName(dialog.GetPath()).GetPath());
        try
            {
            MemoryMappedFile filemap(dialog.GetPath(), true, true);
            BaseProject project;
            auto extractedResult =
                project.ExtractRawText(
                    {
                    static_cast<const char*>(filemap.GetStream()), static_cast<size_t>(filemap.GetMapSize())
                    },
                    wxFileName(dialog.GetPath()).GetExt());
            if (extractedResult.first)
                { cSplit.SplitIntoChapters(extractedResult.second.wc_str()); }
            else
                {
                wxMessageBox(_(L"Unable to split document."), wxGetApp().GetAppDisplayName(),
                             wxICON_EXCLAMATION|wxOK);
                }
            }
        catch (...)
            {
            wxMessageBox(_(L"Unable to split document."), wxGetApp().GetAppDisplayName(), wxICON_EXCLAMATION|wxOK);
            return;
            }
        }
    }

//-------------------------------------------------------
void MainFrame::OnToolsWebHarvest([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureProfessionalCode()))
        {
        wxMessageBox(_(L"Web Harvester is only available in the "
                        "Professional Edition of Readability Studio."),
            _(L"Feature Not Licensed"), wxOK|wxICON_INFORMATION);
        return;
        }

    WebHarvesterDlg webHarvestDlg(this, wxGetApp().GetLastSelectedWebPages(),
        wxGetApp().GetWebHarvester().GetDepthLevel(),
            wxString::Format(_(L"Documents & Images (%s;%s)|%s;%s|"),
                wxGetApp().GetAppOptions().ALL_DOCUMENTS_WILDCARD.data(),
                wxGetApp().GetAppOptions().ALL_IMAGES_WILDCARD.data(),
                wxGetApp().GetAppOptions().ALL_DOCUMENTS_WILDCARD.data(),
                wxGetApp().GetAppOptions().ALL_IMAGES_WILDCARD.data()) +
            wxGetApp().GetAppOptions().GetDocumentFilter() + L"|" +
            wxGetApp().GetAppOptions().GetImageFileFilter(),
        wxGetApp().GetLastSelectedDocFilter(),
        // hide the option to change local file downloading
        true, true,
        wxGetApp().GetWebHarvester().IsKeepingWebPathWhenDownloading(),
        wxGetApp().GetWebHarvester().GetDownloadDirectory(),
        static_cast<int>(wxGetApp().GetWebHarvester().GetDomainRestriction()),
        wxGetApp().GetWebHarvester().GetAllowableWebFolders(),
        wxGetApp().GetWebHarvester().IsPeerVerifyDisabled(),
        wxGetApp().GetWebHarvester().GetUserAgent());
    webHarvestDlg.SetMinimumDownloadFileSizeInKilobytes(
        wxGetApp().GetWebHarvester().GetMinimumDownloadFileSizeInKilobytes().value_or(5));
    webHarvestDlg.UpdateFromHarvesterSettings(wxGetApp().GetWebHarvester());
    // force downloading locally
    webHarvestDlg.DownloadFilesLocally(true);
    webHarvestDlg.SetHelpTopic(GetHelpDirectory(), _DT(L"web-harvester.html"));

    if (webHarvestDlg.ShowModal() != wxID_OK)
        { return; }

    wxGetApp().SetLastSelectedWebPages(webHarvestDlg.GetUrls());
    wxGetApp().SetLastSelectedDocFilter(webHarvestDlg.GetSelectedDocFilter());
    webHarvestDlg.UpdateHarvesterSettings(wxGetApp().GetWebHarvester());

    bool showLogReport{ false };

    for (size_t i = 0; i < webHarvestDlg.GetUrls().GetCount(); ++i)
        {
        FilePathResolver resolver(webHarvestDlg.GetUrls().Item(i), true);
        wxGetApp().GetWebHarvester().SetUrl(resolver.GetResolvedPath());
        webHarvestDlg.UpdateHarvesterSettings(wxGetApp().GetWebHarvester());

        // if user cancelled harvesting, then stop
        if (!wxGetApp().GetWebHarvester().CrawlLinks())
            { break; }

        for (const auto& bLink : wxGetApp().GetWebHarvester().GetBrokenLinks())
            { wxLogWarning(L"Broken link '%s' (from '%s')", bLink.first, bLink.second); }

        if (wxGetApp().GetWebHarvester().GetDownloadedFilePaths().size() == 0)
            {
            wxMessageBox(
                wxString::Format(
                    _(L"No files were downloaded from '%s'. "
                       "Please review the log report for any possible connection issues."),
                    wxGetApp().GetWebHarvester().GetUrl()),
                _(L"Warning"), wxOK | wxICON_WARNING);
            showLogReport = true;
            }
        }

    if (showLogReport)
        {
        wxRibbonButtonBarEvent unusedCmd;
        OnViewLogReport(unusedCmd);
        }
    // crawl a block of HTML text that the user may have entered
    // (only enabled if user holds down SHIFT key to see hidden interface)
    if (webHarvestDlg.GetRawHtmlPage().length())
        {
        // Turn off domain restriction temporarily because if we are grabbing links
        // from a block of HTML, then there is no starting domain.
        const auto dResBak = wxGetApp().GetWebHarvester().GetDomainRestriction();
        wxGetApp().GetWebHarvester().SetDomainRestriction(
            WebHarvester::DomainRestriction::NotRestricted);
        wxGetApp().GetWebHarvester().SetUrl(webHarvestDlg.GetRawHtmlPage());
        wxGetApp().GetWebHarvester().AttemptToCrawlFromRawHtml(true);
        webHarvestDlg.UpdateHarvesterSettings(wxGetApp().GetWebHarvester());
        if (wxGetApp().GetWebHarvester().CrawlLinks())
            {
            for (const auto& bLink : wxGetApp().GetWebHarvester().GetBrokenLinks())
                { wxLogWarning(L"Broken link '%s' (from '%s')", bLink.first, bLink.second); }
            }
        // this feature is not commonly used, so turn it back off
        wxGetApp().GetWebHarvester().AttemptToCrawlFromRawHtml(false);
        wxGetApp().GetWebHarvester().SetDomainRestriction(dResBak);
        }

    // update global internet options that mirror the same options from the dialog
    wxGetApp().GetWebHarvester().DisablePeerVerify(webHarvestDlg.IsPeerVerifyDisabled());
    wxGetApp().GetAppOptions().DisablePeerVerify(webHarvestDlg.IsPeerVerifyDisabled());
    wxGetApp().GetWebHarvester().SetUserAgent(webHarvestDlg.GetUserAgent());
    wxGetApp().GetAppOptions().SetUserAgent(webHarvestDlg.GetUserAgent());
    wxGetApp().GetWebHarvester().SetMinimumDownloadFileSizeInKilobytes(
        webHarvestDlg.GetMinimumDownloadFileSizeInKilobytes());

    wxMessageBox(_(L"Web crawl complete."), _(L"Web Harvester"), wxOK | wxICON_INFORMATION);
    }
