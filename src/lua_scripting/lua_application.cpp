///////////////////////////////////////////////////////////////////////////////
// Name:        lua_application.cpp
// Author:      Blake Madden
// Copyright:   (c) 2005-2023 Blake Madden
// Licence:     3-Clause BSD licence
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "lua_application.h"
#include "../app/readability_app.h"
#include "../projects/batch_project_doc.h"
#include "../projects/base_project.h"
#include "../projects/standard_project_doc.h"
#include "../indexing/word_list.h"
#include "../indexing/phrase.h"
#include "../indexing/characters.h"
#include "../Wisteria-Dataviz/src/util/frequencymap.h"
#include "../Wisteria-Dataviz/src/util/memorymappedfile.h"
#include "../Wisteria-Dataviz/src/CRCpp/inc/CRC.h"
#include "../Wisteria-Dataviz/src/import/html_extract_text.h"

using namespace Wisteria;
using namespace Wisteria::Graphs;
using namespace Wisteria::Colors;
using namespace Wisteria::GraphItems;

wxDECLARE_APP(ReadabilityApp);

namespace LuaScripting
    {
    //-------------------------------------------------------------
    void VerifyLink(const wchar_t* link, const size_t length,
        const bool isImage, const wxString& currentFile,
        std::multimap<wxString,wxString>& badLinks,
        std::multimap<wxString,wxString>& badImageSizes,
        const bool IncludeExternalLinks)
        {
        lily_of_the_valley::html_extract_text extract;
        // see if it's a weblink before anything else
        wxString webPath(link, length);
        FilePathResolver resolve(webPath, false);
        if (resolve.IsHTTPFile() || resolve.IsHTTPSFile())
            {
            if (IncludeExternalLinks)
                {
                // now try to connect to it
                resolve.ResolvePath(webPath, true);
                // it's a web link that we couldn't connect to
                if (resolve.IsInvalidFile())
                    {
                    badLinks.insert(std::make_pair(currentFile, webPath));
                    return;
                    }
                }
            else
                { return; }
            }
        // encoded links will hex encode non-alphanumeric values in them,
        // so convert that to match the real name of the local file
        const wchar_t* filteredFileName = extract(link, length, true, false);
        if (!filteredFileName)
            { return; }
        wxString path(filteredFileName);
        if (path == L"./" || path == L".")
            { return; }
        for (;;)
            {
            const size_t percentIndex = path.find(L'%');
            if (percentIndex == wxString::npos ||
                static_cast<size_t>(percentIndex) > path.length()-3)
                { break; }
            size_t hexLength(static_cast<size_t>(-1));
            const wchar_t value =
                string_util::axtoi(path.substr(percentIndex+1, 2).wc_str(), hexLength);
            path.replace(percentIndex, 3, wxString(1, value));
            }
        // strip off bookmark (if there is one)
        const size_t bookMarkIndex = path.find(L'#', true);
        if (bookMarkIndex != wxString::npos)
            {
            path.erase(bookMarkIndex);
            if (path.length() == 0)
                { return; }
            }
        // if link to a local CHM file then try to see if the
        // decompiled file is in the local file system
        if (path.find(_DT(L".chm::")) != wxString::npos ||
            path.find(_DT(L".CHM::")) != wxString::npos)
            {
            path.Replace(_DT(L".chm::"), wxString{});
            path.Replace(_DT(L".CHM::"), wxString{});
            path.Prepend(wxFileName::GetCwd() + L'\\');
            if (!wxFileName::FileExists(path))
                {
                badLinks.insert(std::make_pair(currentFile, path));
                return;
                }
            }
        // ignore mail links
        if (path.StartsWith(_(L"mailto")))
            { return; }
        // otherwise, it is a regular link
        wxFileName fn(path);
        if (fn.IsRelative())
            {
            [[maybe_unused]] auto absRes = fn.MakeAbsolute(wxFileName(currentFile).GetPath());
            }
        // if an image, then check its dimensions
        if (isImage)
            {
            const std::wstring widthStr =
                lily_of_the_valley::html_extract_text::read_attribute_as_string(
                    link, _DT(L"width"), 5, true);
            const std::wstring heightStr =
                lily_of_the_valley::html_extract_text::read_attribute_as_string(
                    link, _DT(L"height"), 6, true);
            if (widthStr.length() && heightStr.length())
                {
                const long width =
                    static_cast<long>(std::wcstol(widthStr.c_str(), nullptr, 10));
                const long height =
                    static_cast<long>(std::wcstol(heightStr.c_str(), nullptr, 10));
                if (fn.FileExists())
                    {
                    wxLogNull logNo;
                    wxImage img;
                    img.LoadFile(fn.GetFullPath());
                    if (!img.IsOk())
                        {
                        badImageSizes.insert(std::make_pair(currentFile,
                            wxString::Format(_(L"%s (image is possibly corrupt))"),
                                             fn.GetFullPath())));
                        }
                    else if (!is_within<long>(img.GetHeight(), height-5, height+5) ||
                        !is_within<long>(img.GetWidth(), width-5, width+5) )
                        {
                        badImageSizes.insert(std::make_pair(currentFile,
                            wxString::Format(_(L"%s (%ld x %ld vs. %d x %d))"),
                                fn.GetFullPath(), width, height, img.GetWidth(),
                                img.GetHeight())));
                        }
                    }
                }
            }
        if (!fn.FileExists())
            { badLinks.insert(std::make_pair(currentFile, fn.GetFullPath())); }
        }

    //-------------------------------------------------------------
    int GetProgramPath(lua_State *L)
        {
        lua_pushstring(L,
            wxFileName(wxStandardPaths::Get().GetExecutablePath()).
                GetPath(wxPATH_GET_SEPARATOR|wxPATH_GET_VOLUME).utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int GetLuaConstantsPath(lua_State *L)
        {
        lua_pushstring(L, wxGetApp().FindResourceFile(L"RSConstants.lua").utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int GetActiveBatchProject(lua_State *L)
        {
        wxDocument* currentDoc =
            wxGetApp().GetMainFrame()->GetDocumentManager()->GetCurrentDocument();
        if (currentDoc && currentDoc->IsKindOf(CLASSINFO(BatchProjectDoc)) )
            {
            BatchProject* batchProject = new BatchProject(L);
            batchProject->SetProject(dynamic_cast<BatchProjectDoc*>(currentDoc) );
            Luna<LuaScripting::BatchProject>::push(L, batchProject);
            return 1;
            }
        else
            {
            wxList docs = wxGetApp().GetDocManager()->GetDocuments();
            for (size_t i = 0; i < docs.GetCount(); ++i)
                {
                if (docs.Item(i)->GetData()->IsKindOf(CLASSINFO(BatchProjectDoc)))
                    {
                    BatchProject* batchProject = new BatchProject(L);
                    batchProject->SetProject(dynamic_cast<BatchProjectDoc*>(docs.Item(i)->GetData()) );
                    Luna<LuaScripting::BatchProject>::push(L, batchProject);
                    return 1;
                    }
                }
            wxLogError(L"No active batch project found.");
            return 0;
            }
        }

    int GetActiveStandardProject(lua_State *L)
        {
        wxDocument* currentDoc =
            wxGetApp().GetMainFrame()->GetDocumentManager()->GetCurrentDocument();
        if (currentDoc && currentDoc->IsKindOf(CLASSINFO(ProjectDoc)) )
            {
            StandardProject* standardProject = new StandardProject(L);
            standardProject->SetProject(dynamic_cast<ProjectDoc*>(currentDoc) );
            Luna<LuaScripting::StandardProject>::push(L, standardProject);
            return 1;
            }
        else
            {
            wxList docs = wxGetApp().GetDocManager()->GetDocuments();
            for (size_t i = 0; i < docs.GetCount(); ++i)
                {
                if (docs.Item(i)->GetData()->IsKindOf(CLASSINFO(ProjectDoc)))
                    {
                    StandardProject* standardProject = new StandardProject(L);
                    standardProject->SetProject(dynamic_cast<ProjectDoc*>(docs.Item(i)->GetData()) );
                    Luna<LuaScripting::StandardProject>::push(L, standardProject);
                    return 1;
                    }
                }
            wxLogError(L"No active standard project found.");
            return 0;
            }
        }

    //-------------------------------------------------------------
    int GetTestId(lua_State *L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        const wxString testName(luaL_checkstring(L, 1), wxConvUTF8);
        const auto testPos =
            BaseProject::GetDefaultReadabilityTestsTemplate().find_test(testName);

        if (testPos.second)
            {
            lua_pushnumber(L, testPos.first->get_interface_id());
            return 1;
            }

        CustomReadabilityTestCollection::const_iterator testIter =
            std::find(BaseProject::m_custom_word_tests.begin(),
                      BaseProject::m_custom_word_tests.end(), testName);
        if (testIter != BaseProject::m_custom_word_tests.end())
            { lua_pushnumber(L, testIter->get_interface_id()); }
        else
            { lua_pushnumber(L, wxNOT_FOUND); }
        return 1;
        }

    //-------------------------------------------------------------
    int MsgBox(lua_State *L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        if (lua_isboolean(L,1))
            {
            wxMessageBox(lua_toboolean(L,1) ? _(L"true") : _(L"false"),
                (lua_gettop(L) > 1) ?
                wxString(luaL_checkstring(L, 2), wxConvUTF8) : wxGetApp().GetAppName(),
                wxOK|wxICON_INFORMATION);
            }
        else if (lua_isstring(L,1))
            {
            wxMessageBox(wxString(luaL_checkstring(L, 1), wxConvUTF8),
                (lua_gettop(L) > 1) ?
                wxString(luaL_checkstring(L, 2), wxConvUTF8) : wxGetApp().GetAppName(),
                wxOK|wxICON_INFORMATION);
            }
        return 0;
        }

     //-------------------------------------------------------------
    int DownloadFile(lua_State *L)
        {
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        wxString urlPath(luaL_checkstring(L, 1), wxConvUTF8);
        const wxString downloadFolder(luaL_checklstring(L, 2, nullptr), wxConvUTF8);

        wxGetApp().GetWebHarvester().SetUrl(urlPath);
        wxGetApp().GetWebHarvester().SetDownloadDirectory(downloadFolder);
        wxGetApp().GetWebHarvester().KeepWebPathWhenDownloading(false);

        lua_pushstring(L, wxGetApp().GetWebHarvester().DownloadFile(urlPath).utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int FindFiles(lua_State *L)
        {
        // should be passed a folder to search and the file filter to use
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        lua_newtable(L);

        wxDir dir;
        wxArrayString files;
        const size_t fileCount =
            dir.GetAllFiles(wxString(luaL_checkstring(L, 1), wxConvUTF8), &files,
                            wxString(luaL_checkstring(L, 2), wxConvUTF8), wxDIR_FILES|wxDIR_DIRS);

        for (size_t i = 1; i <= fileCount; ++i)
            {
            lua_pushnumber(L,i);
            lua_pushstring(L,files[i-1].mb_str());
            lua_settable(L,-3);
            }

        return 1;
        }

    //-------------------------------------------------------------
    int GetImageInfo(lua_State *L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        const wxString path(luaL_checkstring(L, 1), wxConvUTF8);
        if (!wxFile::Exists(path) )
            {
            wxMessageBox(wxString::Format(_(L"%s: Invalid image file path."), path),
                _(L"Script Error"), wxOK|wxICON_EXCLAMATION);
            lua_pushboolean(L, false);
            return 1;
            }
        const wxImage img(path);
        if (!img.IsOk())
            {
            wxMessageBox(wxString::Format(_(L"%s: Unable to load image."), path),
                _(L"Script Error"), wxOK|wxICON_EXCLAMATION);
            lua_pushboolean(L, false);
            return 1;
            }
        lua_newtable(L);

        lua_pushstring(L, _DT("Width"));
        lua_pushinteger(L, img.GetSize().GetWidth());
        lua_settable(L, -3);

        lua_pushstring(L, _DT("Height"));
        lua_pushinteger(L, img.GetSize().GetHeight());
        lua_settable(L, -3);

        return 1;
        }

    //-------------------------------------------------------------
    int GetFileCheckSum(lua_State *L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        const wxString filePath(luaL_checkstring(L, 1), wxConvUTF8);

        std::uint32_t crc{ 0 };
        try
            {
            MemoryMappedFile file(filePath, true, true);
            crc = CRC::Calculate(file.GetStream(), file.GetMapSize(), CRC::CRC_32());
            }
        catch (...)
            {
            lua_pushnumber(L, 0);
            return 1;
            }

        lua_pushnumber(L, crc);
        return 1;
        }

    //-------------------------------------------------------------
    int LogMessage(lua_State *L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        // avoid printf injection
        wxLogMessage(L"%s", wxString(luaL_checkstring(L, 1), wxConvUTF8));
        return 0;
        }

    //-------------------------------------------------------------
    int SplashScreen(lua_State *L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        assert(wxGetApp().GetSplashscreenPaths().GetCount());
        const auto index =
            std::clamp<size_t>(lua_tonumber(L, 1)-1/*make it zero-indexed*/, 0,
                               wxGetApp().GetSplashscreenPaths().GetCount()-1);
        wxBitmap bitmap =
            wxGetApp().GetScaledImage(wxGetApp().GetSplashscreenPaths()[index],
                wxBITMAP_TYPE_PNG,
                wxSize(wxSystemSettings::GetMetric(wxSystemMetric::wxSYS_SCREEN_X)*.5,
                        wxSystemSettings::GetMetric(wxSystemMetric::wxSYS_SCREEN_Y)*.5));
        if (bitmap.IsOk())
            {
            bitmap = wxGetApp().CreateSplashscreen(bitmap, wxGetApp().GetAppName(),
                wxGetApp().GetAppSubName(), wxGetApp().GetVendorName(), true);

            wxSplashScreen* splash = new wxSplashScreen(bitmap,
                wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT,
                60000, nullptr, -1, wxDefaultPosition, wxDefaultSize,
                wxFRAME_NO_TASKBAR|wxSTAY_ON_TOP|wxSPLASH_NO_TIMEOUT);
            wxUnusedVar(splash);
            }
        lua_pushboolean(L, true);
        return 1;
        }

    //-------------------------------------------------------------
    int QAVerify(lua_State *L)
        {
        lua_pushboolean(L, wxGetApp().VerifyWordLists());
        return 1;
        }

    //-------------------------------------------------------------
    int CheckHtmlLinks(lua_State *L)
        {
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        wxString path(luaL_checkstring(L, 1), wxConvUTF8);

        std::multimap<wxString,wxString> badLinks;
        std::multimap<wxString,wxString> badImageSizes;
        std::set<traits::case_insensitive_wstring_ex> validFiles;

        // get the valid files
            {
            wxDir dir;
            wxArrayString files;
            // analysis the topics
            dir.GetAllFiles(path, &files, _DT(L"*.hhp*"));
            wxString fileContent;
            for (size_t i = 0; i < files.Count(); ++i)
                {
                wxFileName fn(files[i]);
                const wxString rootFolder = fn.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR);

                wxFFile file(files[i], L"r");
                file.ReadAll(&fileContent, *wxConvCurrent);
                file.Close();

                size_t start = fileContent.find(L"[FILES]");
                if (start == wxString::npos)
                    { continue; }
                start += 7;
                size_t end = fileContent.find(L"\n[", start);
                if (end == wxString::npos)
                    {
                    end = fileContent.find(L"\r[", start);
                    if (end == wxString::npos)
                        { continue; }
                    }
                fileContent = fileContent.substr(start, end-start);

                wxStringTokenizer tkz(fileContent, L"\n\r");
                while (tkz.HasMoreTokens() )
                    {
                    wxString token = tkz.GetNextToken();
                    if (token.length())
                        {
                        validFiles.insert((rootFolder+token).wc_str());
                        }
                    }
                }
            }

        // get the physical files in the system
        wxDir dir;
        wxArrayString files;
        dir.GetAllFiles(path, &files, _DT(L"*.htm*"));

        wxString fileContent;
        files.clear();
        dir.GetAllFiles(path, &files, _DT(L"*.htm*"));
        const bool IncludeExternalLinks = int_to_bool(lua_toboolean(L, 2));
        for (size_t i = 0; i < files.Count(); ++i)
            {
            Wisteria::TextStream::ReadFile(files[i], fileContent);

            std::set<wxString> BookmarksInCurrentPage;
            std::pair<const wchar_t*,std::wstring> foundBookMark;
            const wchar_t* const htmlEnd = foundBookMark.first+fileContent.length();
            while (foundBookMark.first)
                {
                foundBookMark =
                    lily_of_the_valley::html_extract_text::find_bookmark(
                        foundBookMark.first, htmlEnd);
                if (foundBookMark.first)
                    {
                    BookmarksInCurrentPage.insert(foundBookMark.second);
                    foundBookMark.first += foundBookMark.second.length();
                    }
                else
                    { break; }
                }

            html_utilities::html_hyperlink_parse hparse(fileContent.c_str(),
                                                        fileContent.length());
            const wchar_t* currentLink;
            while ((currentLink = hparse()) != nullptr)
                {
                // review bookmarks in the current page
                if (hparse.get_current_hyperlink_length() > 2 &&
                    currentLink[0] == L'#')
                    {
                    wxString currentBookmark(currentLink + 1,
                                             hparse.get_current_hyperlink_length()-1);
                    if (BookmarksInCurrentPage.find(currentBookmark) ==
                        BookmarksInCurrentPage.end())
                        {
                        badLinks.emplace(wxFileName(files[i]).GetFullName(),
                                         wxString(currentLink,
                                             hparse.get_current_hyperlink_length()));
                        }
                    }
                else
                    {
                    VerifyLink(currentLink, hparse.get_current_hyperlink_length(),
                        hparse.is_current_link_an_image(), files[i],
                        badLinks, badImageSizes, IncludeExternalLinks);
                    }
                }
            }

        // write out the warnings of bad links
        for (const auto& badLink : badLinks)
            { wxLogError(L"%s: %s", badLink.first, badLink.second); }
        if (badLinks.size() == 0)
            { wxLogMessage(L"No bad links found in help folder."); }
        // write out the warnings of images whose size don't
        // match their size specified in a topic
        for (const auto& badImage : badImageSizes)
            { wxLogError(L"%s: %s", badImage.first, badImage.second); }
        if (badImageSizes.size() == 0)
            { wxLogMessage(L"No image issues found in help folder."); }

        lua_pushboolean(L, true);
        return 1;
        }

    //-------------------------------------------------------------
    int Crash(lua_State*)
        {
#ifndef NDEBUG
        wxString* bogus{ nullptr };
        // cppcheck-suppress ignoredReturnValue
        bogus->length();
#endif
        return 0;
        }

    // Closes the application
    //-------------------------------------------------------------
    int Close(lua_State*)
        {
        if (wxGetApp().GetMainFrame()->GetDocumentManager()->CloseDocuments())
            { wxGetApp().GetTopWindow()->Close(); }
        return 0;
        }

    //-------------------------------------------------------------
    int GetDocumentsPath(lua_State *L)
        {
        lua_pushstring(L, wxStandardPaths::Get().GetDocumentsDir().mb_str());
        return 1;
        }

    //-------------------------------------------------------------
    int GetAbsoluteFilePath(lua_State *L)
        {
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }

        // set to relative path
        wxFileName fn(wxString(luaL_checkstring(L, 2), wxConvUTF8));
        if (fn.MakeAbsolute(wxString(luaL_checkstring(L, 1), wxConvUTF8)))
            {
            lua_pushstring(L, fn.GetFullPath().mb_str().data());
            }
        else
            {
            DebugPrint(wxString::Format(_(L"%sWarning%s: unable to make %s path absolute."),
                L"<span style='color:blue; font-weight:bold;'>", L"</span>",
                wxString(luaL_checkstring(L, 2), wxConvUTF8)));
            lua_pushstring(L, wxString(luaL_checkstring(L, 2), wxConvUTF8));
            }

        return 1;
        }

    //-------------------------------------------------------------
    int WriteToFile(lua_State *L)
        {
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        wxString path(luaL_checkstring(L, 1), wxConvUTF8);
        wxString outputStr(luaL_checkstring(L, 2), wxConvUTF8);

        // create the folder to the filepath, if necessary
        wxFileName::Mkdir(wxFileName(path).GetPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

        wxFileName(path).SetPermissions(wxS_DEFAULT);
        wxFile outputFile(path, wxFile::write);
        lua_pushboolean(L,
            outputFile.Write(outputStr, wxConvUTF8));
        return 1;
        }

    //-------------------------------------------------------------
    int LogError(lua_State *L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        wxLogError(L"%s", wxString(luaL_checkstring(L, 1), wxConvUTF8));
        return 0;
        }

    //-------------------------------------------------------------
    int RemoveAllCustomTests(lua_State*)
        {
        ProjectDoc::RemoveAllGlobalCustomReadabilityTests();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int RemoveAllCustomTestBundles(lua_State*)
        {
        wxGetApp().RemoveAllCustomTestBundles();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int CrossReferenceWordLists(lua_State* L)
        {
        if (!VerifyParameterCount(L, 3, __WXFUNCTION__))
            { return 0; }
        wxString path(luaL_checkstring(L, 1), wxConvUTF8);
        if (!wxFile::Exists(path) )
            {
            wxMessageBox(wxString::Format(_(L"%s: Invalid file path."), wxString(__WXFUNCTION__)),
                _(L"Script Error"), wxOK|wxICON_EXCLAMATION);
            lua_pushboolean(L, false);
            return 1;
            }
        grammar::phrase_collection phraseList;
        wxString inputFileBuffer;
        std::vector<word_list::word_type> newStrings;
        if (Wisteria::TextStream::ReadFile(path, inputFileBuffer) )
            { phraseList.load_phrases(inputFileBuffer, true, false); }
        phraseList.remove_duplicates();

        // load the word list being cross referenced against
        path = wxString(luaL_checkstring(L, 2), wxConvUTF8);
        if (!wxFile::Exists(path) )
            {
            wxMessageBox(wxString::Format(_(L"%s: Invalid file path."), wxString(__WXFUNCTION__)),
                _(L"Script Error"), wxOK|wxICON_EXCLAMATION);
            lua_pushboolean(L, false);
            return 1;
            }
        word_list wordList;
        if (Wisteria::TextStream::ReadFile(path, inputFileBuffer) )
            { wordList.load_words(inputFileBuffer, true, false); }

        wxString outputStr;
        wxStringTokenizer tokenizer;
        for (const auto& phrase : phraseList.get_phrases())
            {
            tokenizer.SetString(phrase.second.c_str(), L",", wxTOKEN_STRTOK);
            while (tokenizer.HasMoreTokens())
                {
                wxString replacementStr = tokenizer.GetNextToken();
                replacementStr.Trim(true); replacementStr.Trim(false);
                if (replacementStr.find(L' ') == wxString::npos)
                    {
                    if (wordList.contains(replacementStr))
                        {
                        outputStr += wxString::Format(L"%s\t%s\r\n",
                            phrase.first.to_string().c_str(), replacementStr);
                        break;
                        }
                    }
                }
            }

        // create the folder to the filepath, if necessary
        wxString outputPath(luaL_checkstring(L, 3), wxConvUTF8);
        wxFileName::Mkdir(wxFileName(outputPath).GetPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

        wxFileName(outputPath).SetPermissions(wxS_DEFAULT);
        wxFile outputFile(outputPath, wxFile::write);
        outputFile.Write(outputStr, wxConvUTF8);

        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int PhraseListToWordList(lua_State* L)
        {
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        wxString path(luaL_checkstring(L, 1), wxConvUTF8);
        if (!wxFile::Exists(path) )
            {
            wxMessageBox(wxString::Format(_(L"%s: Invalid file path."), wxString(__WXFUNCTION__)),
                _(L"Script Error"), wxOK|wxICON_EXCLAMATION);
            lua_pushboolean(L, false);
            return 1;
            }
        grammar::phrase_collection phraseList;
        wxString inputFileBuffer;
        std::vector<word_list::word_type> newStrings;
        if (Wisteria::TextStream::ReadFile(path, inputFileBuffer) )
            { phraseList.load_phrases(inputFileBuffer, true, false); }
        phraseList.remove_duplicates();

        wxString outputStr;
        for (auto pos = phraseList.get_phrases().cbegin();
            pos != phraseList.get_phrases().cend();
            ++pos)
            {
            if (pos->first.get_word_count() == 1 &&
                pos->first.get_type() == grammar::phrase_type::phrase_wordy &&
                pos->second.length())
                {
                outputStr += wxString::Format(L"%s\t%s\r\n", pos->first.to_string().c_str(),
                                              pos->second.c_str());
                }
            }

        // create the folder to the filepath, if necessary
        wxString outputPath(luaL_checkstring(L, 2), wxConvUTF8);
        wxFileName::Mkdir(wxFileName(outputPath).GetPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

        wxFileName(outputPath).SetPermissions(wxS_DEFAULT);
        wxFile outputFile(outputPath, wxFile::write);
        outputFile.Write(outputStr, wxConvUTF8);

        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ExpandWordList(lua_State* L)
        {
        if (!VerifyParameterCount(L, 3, __WXFUNCTION__))
            { return 0; }
        wxString path(luaL_checkstring(L, 1), wxConvUTF8);
        if (!wxFile::Exists(path) )
            {
            wxMessageBox(wxString::Format(_(L"%s: Invalid file path."), wxString(__WXFUNCTION__)),
                _(L"Script Error"), wxOK|wxICON_EXCLAMATION);
            lua_pushboolean(L, false);
            return 1;
            }
        word_list wordList;
        wxString inputFileBuffer;
        std::vector<word_list::word_type> newStrings;
        if (Wisteria::TextStream::ReadFile(path, inputFileBuffer) )
            { wordList.load_words(inputFileBuffer, false, false); }
        for (int i = 3; i <= lua_gettop(L); ++i)
            {
            const wxString itemToAppend(luaL_checkstring(L, i), wxConvUTF8);
            for (size_t j = 0; j < wordList.get_list_size(); ++j)
                {
                // English rule for adding an 's' to the end of a word
                if (itemToAppend.CmpNoCase(L"s") == 0 &&
                    wordList.get_words().at(j).length() > 1)
                    {
                    const wchar_t secondToLastLetter =
                        wordList.get_words().at(j).at(wordList.get_words().at(j).length() - 2);
                    const wchar_t lastLetter =
                        wordList.get_words().at(j).at(wordList.get_words().at(j).length() - 1);
                    if ((secondToLastLetter == L's' || secondToLastLetter == L'S') &&
                        (lastLetter == L'h' || lastLetter == L'H'))
                        {
                        newStrings.push_back(wordList.get_words().at(j)+_DT(L"es"));
                        continue;
                        }
                    else if ((secondToLastLetter == L'c' || secondToLastLetter == L'C') &&
                        (lastLetter == L'h' || lastLetter == L'H'))
                        {
                        newStrings.push_back(wordList.get_words().at(j)+_DT(L"es"));
                        continue;
                        }
                    else if (lastLetter == L's' || lastLetter == L'S' ||
                        lastLetter == L'x' || lastLetter == L'X' ||
                        lastLetter == L'z' || lastLetter == L'Z')
                        {
                        newStrings.push_back(wordList.get_words().at(j)+_DT(L"es"));
                        continue;
                        }
                    else if (characters::is_character::is_consonant(secondToLastLetter) &&
                        (lastLetter == L'y' || lastLetter == L'Y'))
                        {
                        newStrings.push_back(
                            wordList.get_words().at(j).
                                substr(0, wordList.get_words().at(j).length()-1) + _DT(L"ies"));
                        continue;
                        }
                    }
                newStrings.push_back(wordList.get_words().at(j)+itemToAppend.wc_str());
                }
            }
        wordList.add_words(newStrings);
        wordList.remove_duplicates();

        wxString outputStr;
        // save the new list
        outputStr.reserve(wordList.get_list_size()*5);
        for (size_t i = 0; i < wordList.get_list_size(); ++i)
            { outputStr += wordList.get_words().at(i).c_str() + wxString(L"\n"); }
        outputStr.Trim(true); outputStr.Trim(false);
        // create the folder to the filepath, if necessary
        wxString outputPath(luaL_checkstring(L, 2), wxConvUTF8);
        wxFileName::Mkdir(wxFileName(outputPath).GetPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

        wxFileName(outputPath).SetPermissions(wxS_DEFAULT);
        wxFile outputFile(outputPath, wxFile::write);
        outputFile.Write(outputStr, wxConvUTF8);

        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int MergePhraseLists(lua_State* L)
        {
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }

        grammar::phrase_collection phrases;
        wxString inputFile;
        for (int i = 2; i <= lua_gettop(L); ++i)
            {
            inputFile = wxString(luaL_checkstring(L, i), wxConvUTF8);
            if (wxFile::Exists(inputFile) )
                {
                wxString inputFileBuffer;
                if (Wisteria::TextStream::ReadFile(inputFile, inputFileBuffer) )
                    { phrases.load_phrases(inputFileBuffer, false, true); }
                }
            else
                {
                wxMessageBox(wxString::Format(_(L"%s: File not found."), inputFile),
                    _(L"Script Error"), wxOK | wxICON_EXCLAMATION);
                lua_pushboolean(L, false);
                return 1;
                }
            }

        phrases.remove_duplicates();

        for (const auto& phrase : phrases.get_phrases())
            {
            if (phrase.first.to_string() == phrase.second)
                {
                wxMessageBox(wxString::Format(_(L"%s: Phrase '%s' and suggested replacement are "
                                                L"the same.\nPlease review your phrase file."),
                                              inputFile, phrase.first.to_string().c_str()),
                             _(L"Warning"), wxOK | wxICON_EXCLAMATION);
                }
            }

        wxString outputStr;
        wxString expStr;
        for (const auto& phrase : phrases.get_phrases())
            {
            outputStr += phrase.first.to_string().c_str();
            // don't bother exporting blank columns
            if (phrase.second.length() || phrase.first.get_proceeding_exceptions().size() ||
                phrase.first.get_trailing_exceptions().size())
                {
                outputStr += L'\t';
                outputStr += phrase.second.c_str();
                outputStr += L'\t';
                outputStr += wxString::Format(L"%d", static_cast<int>(phrase.first.get_type()));
                outputStr += L'\t';
                expStr.clear();
                for (const auto& exp : phrase.first.get_proceeding_exceptions())
                    { expStr.append(exp.c_str()).append(L";"); }
                if (expStr.ends_with(L";"))
                    { expStr.RemoveLast(1); }
                outputStr += expStr;
                outputStr += L'\t';

                expStr.clear();
                for (const auto& exp : phrase.first.get_trailing_exceptions())
                    { expStr.append(exp.c_str()).append(L";"); }
                if (expStr.ends_with(L";"))
                    { expStr.RemoveLast(1); }
                outputStr += expStr;
                }
            outputStr += L"\r\n";
            }

        // create the folder to the filepath, if necessary
        wxString path(luaL_checkstring(L, 1), wxConvUTF8);
        wxFileName::Mkdir(wxFileName(path).GetPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

        wxFileName(path).SetPermissions(wxS_DEFAULT);
        wxFile outputFile(path, wxFile::write);
        lua_pushboolean(L, outputFile.Write(outputStr, wxConvUTF8));

        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int MergeWordLists(lua_State* L)
        {
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        word_list wordList;
        wxString inputFileBuffer;
        for (int i = 2; i <= lua_gettop(L); ++i)
            {
            wxString inputFile(luaL_checkstring(L, i), wxConvUTF8);
            if (wxFile::Exists(inputFile) )
                {
                if (Wisteria::TextStream::ReadFile(inputFile, inputFileBuffer) )
                    { wordList.load_words(inputFileBuffer, false, true); }
                }
            else
                {
                wxMessageBox(wxString::Format(_(L"%s: File not found."), inputFile),
                    _(L"Script Error"), wxOK | wxICON_EXCLAMATION);
                lua_pushboolean(L, false);
                return 1;
                }
            }
        wordList.remove_duplicates();

        wxString outputStr;
        // save the new list
        outputStr.reserve(wordList.get_list_size()*5);
        for (size_t i = 0; i < wordList.get_list_size(); ++i)
            { outputStr += wordList.get_words().at(i).c_str() + wxString(L"\n"); }
        outputStr.Trim(true); outputStr.Trim(false);

        // create the folder to the filepath, if necessary
        wxString path(luaL_checkstring(L, 1), wxConvUTF8);
        wxFileName::Mkdir(wxFileName(path).GetPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

        wxFileName(path).SetPermissions(wxS_DEFAULT);
        wxFile outputFile(path, wxFile::write);
        lua_pushboolean(L, outputFile.Write(outputStr, wxConvUTF8));

        wxGetApp().Yield();
        return 1;
        }

    // PROJECT SETTINGS
    //-------------------------------------------------------------
    int SetReviewer(lua_State *L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        wxGetApp().GetAppOptions().SetReviewer(
            wxString(luaL_checkstring(L, 1), wxConvUTF8));
        wxGetApp().Yield();
        return 0;
        }
    //-------------------------------------------------------------
    int GetReviewer(lua_State *L)
        {
        lua_pushstring(L,wxGetApp().GetAppOptions().GetReviewer().utf8_str());
        wxGetApp().Yield();
        return 1;
        }
    //-------------------------------------------------------------
    int SetProjectLanguage(lua_State *L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        wxGetApp().GetAppOptions().SetProjectLanguage(
            static_cast<readability::test_language>(static_cast<int>(lua_tonumber(L, 1))));
        wxGetApp().Yield();
        return 0;
        }
    //-------------------------------------------------------------
    int GetProjectLanguage(lua_State *L)
        {
        lua_pushnumber(L,
            static_cast<int>(wxGetApp().GetAppOptions().GetProjectLanguage()));
        wxGetApp().Yield();
        return 1;
        }
    //-------------------------------------------------------------
    int SetDocumentStorageMethod(lua_State *L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        wxGetApp().GetAppOptions().SetDocumentStorageMethod(
            static_cast<TextStorage>(static_cast<int>(lua_tonumber(L, 1))));
        return 0;
        }
    //-------------------------------------------------------------
    int GetDocumentStorageMethod(lua_State *L)
        {
        lua_pushnumber(L,
            static_cast<int>(wxGetApp().GetAppOptions().GetDocumentStorageMethod()));
        return 1;
        }
    //-------------------------------------------------------------
    int SetParagraphsParsingMethod(lua_State *L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        wxGetApp().GetAppOptions().SetParagraphsParsingMethod(
            static_cast<ParagraphParse>(static_cast<int>(lua_tonumber(L, 1))));
        return 0;
        }
    //-------------------------------------------------------------
    int GetParagraphsParsingMethod(lua_State *L)
        {
        lua_pushnumber(L,
            static_cast<int>(wxGetApp().GetAppOptions().GetParagraphsParsingMethod()));
        return 1;
        }
    //-------------------------------------------------------------
    int SetMinDocWordCountForBatch(lua_State *L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        wxGetApp().GetAppOptions().SetMinDocWordCountForBatch(
            static_cast<int>(lua_tonumber(L, 1)));
        return 0;
        }
    //-------------------------------------------------------------
    int GetMinDocWordCountForBatch(lua_State *L)
        {
        lua_pushnumber(L,
            static_cast<int>(wxGetApp().GetAppOptions().GetMinDocWordCountForBatch()));
        return 1;
        }
    //-------------------------------------------------------------
    int SetFilePathDisplayMode(lua_State *L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        wxGetApp().GetAppOptions().SetFilePathTruncationMode(
            static_cast<ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode>(
                static_cast<int>(lua_tonumber(L, 1))));
        return 0;
        }
    //-------------------------------------------------------------
    int GetFilePathDisplayMode(lua_State *L)
        {
        lua_pushnumber(L,
            static_cast<int>(wxGetApp().GetAppOptions().GetFilePathTruncationMode()));
        return 1;
        }
    //-------------------------------------------------------------
    int ImportSettings(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        wxString filePath(luaL_checkstring(L, 1), wxConvUTF8);
        const auto reviewer = wxGetApp().GetAppOptions().GetReviewer();
        lua_pushboolean(L,
            wxGetApp().GetAppOptions().LoadOptionsFile(filePath, false, false));
        wxGetApp().GetAppOptions().SetReviewer(reviewer);
        wxGetApp().Yield();
        return 1;
        }
    //-------------------------------------------------------------
    int ExportSettings(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        wxString filePath(luaL_checkstring(L, 1), wxConvUTF8);
        lua_pushboolean(L,
            wxGetApp().GetAppOptions().SaveOptionsFile(filePath));
        wxGetApp().Yield();
        return 1;
        }
    //-------------------------------------------------------------
    int ResetSettings(lua_State*)
        {
        wxGetApp().GetAppOptions().ResetSettings();
        wxGetApp().Yield();
        return 0;
        }
    //-------------------------------------------------------------
    int DisableAllWarnings(lua_State*)
        {
        wxGetApp().GetAppOptions().DisableWarnings();
        return 0;
        }
    //-------------------------------------------------------------
    int EnableAllWarnings(lua_State*)
        {
        wxGetApp().GetAppOptions().EnableWarnings();
        return 0;
        }
    //-------------------------------------------------------------
    int EnableWarning(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        wxGetApp().GetAppOptions().EnableWarning(
            wxString(luaL_checkstring(L, 1), wxConvUTF8));
        return 0;
        }
    //-------------------------------------------------------------
    int DisableWarning(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        wxGetApp().GetAppOptions().DisableWarning(
            wxString(luaL_checkstring(L, 1), wxConvUTF8));
        return 0;
        }
    //-------------------------------------------------------------
    int SetTextExclusion(lua_State *L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        wxGetApp().GetAppOptions().SetInvalidSentenceMethod(
            static_cast<InvalidSentence>(static_cast<int>(lua_tonumber(L, 1))));
        return 0;
        }
    //-------------------------------------------------------------
    int SetIncludeIncompleteTolerance(lua_State *L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        wxGetApp().GetAppOptions().
            SetIncludeIncompleteSentencesIfLongerThanValue(lua_tonumber(L, 1));
        return 0;
        }
    //-------------------------------------------------------------
    int AggressivelyExclude(lua_State *L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        wxGetApp().GetAppOptions().AggressiveExclusion(int_to_bool(lua_toboolean(L, 1)));
        return 0;
        };
    //-------------------------------------------------------------
    int ExcludeCopyrightNotices(lua_State *L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        wxGetApp().GetAppOptions().IgnoreTrailingCopyrightNoticeParagraphs(
            int_to_bool(lua_toboolean(L, 1)));
        return 0;
        };
    //-------------------------------------------------------------
    int ExcludeTrailingCitations(lua_State *L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        wxGetApp().GetAppOptions().IgnoreTrailingCitations(int_to_bool(lua_toboolean(L, 1)));
        return 0;
        };
    //-------------------------------------------------------------
    int ExcludeFileAddress(lua_State *L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        wxGetApp().GetAppOptions().IgnoreFileAddresses(int_to_bool(lua_toboolean(L, 1)));
        return 0;
        };
    //-------------------------------------------------------------
    int ExcludeNumerals(lua_State *L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        wxGetApp().GetAppOptions().IgnoreNumerals(int_to_bool(lua_toboolean(L, 1)));
        return 0;
        };
    //-------------------------------------------------------------
    int ExcludeProperNouns(lua_State *L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        wxGetApp().GetAppOptions().IgnoreProperNouns(int_to_bool(lua_toboolean(L, 1)));
        return 0;
        };
    //-------------------------------------------------------------
    int SetPhraseExclusionList(lua_State *L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        wxGetApp().GetAppOptions().SetExcludedPhrasesPath(
            wxString(luaL_checkstring(L, 1), wxConvUTF8));
        return 0;
        };
    //-------------------------------------------------------------
    int SetBlockExclusionTags(lua_State *L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        const wxString exclusionTags(luaL_checkstring(L, 1), wxConvUTF8);
        wxGetApp().GetAppOptions().GetExclusionBlockTags().clear();
        if (exclusionTags.length() >= 2)
            {
            wxGetApp().GetAppOptions().GetExclusionBlockTags().push_back(
                std::make_pair(exclusionTags[0],exclusionTags[1]));
            }
        return 0;
        };

    // GRAPH OPTIONS
    //-------------------------------------------------------------
    int SetGraphBackgroundColor(lua_State *L)
        {
        if (!VerifyParameterCount(L, 3, __WXFUNCTION__))
            { return 0; }
        wxGetApp().GetAppOptions().SetBackGroundColor(
            wxColour(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3)));
        return 0;
        }
    //-------------------------------------------------------------
    int ApplyGraphBackgroundFade(lua_State *L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        wxGetApp().GetAppOptions().SetGraphBackGroundLinearGradient(
            int_to_bool(lua_toboolean(L, 1)));
        return 0;
        }
    //-------------------------------------------------------------
    int SetPlotBackgroundImage(lua_State *L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        wxGetApp().GetAppOptions().SetPlotBackGroundImagePath(
            wxString(luaL_checkstring(L, 1), wxConvUTF8));
        return 0;
        }
    //-------------------------------------------------------------
    int SetPlotBackgroundImageOpacity(lua_State *L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        wxGetApp().GetAppOptions().SetPlotBackGroundImageOpacity(lua_tonumber(L, 1));
        return 0;
        }
    //-------------------------------------------------------------
    int SetPlotBackgroundColor(lua_State *L)
        {
        if (!VerifyParameterCount(L, 3, __WXFUNCTION__))
            { return 0; }
        wxGetApp().GetAppOptions().SetPlotBackGroundColor(
            wxColour(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3)));
        return 0;
        }
    //-------------------------------------------------------------
    int SetPlotBackgroundOpacity(lua_State *L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        wxGetApp().GetAppOptions().SetPlotBackGroundColorOpacity(lua_tonumber(L, 1));
        return 0;
        }
    //-------------------------------------------------------------
    int SetGraphWatermark(lua_State *L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        wxGetApp().GetAppOptions().SetWatermark(
            wxString(luaL_checkstring(L, 1), wxConvUTF8));
        return 0;
        }
    //-------------------------------------------------------------
    int SetGraphLogoImage(lua_State *L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        wxGetApp().GetAppOptions().SetWatermarkLogo(
            wxString(luaL_checkstring(L, 1), wxConvUTF8));
        return 0;
        }
    //-------------------------------------------------------------
    int SetStippleImage(lua_State *L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        wxGetApp().GetAppOptions().SetStippleImagePath(
            wxString(luaL_checkstring(L, 1), wxConvUTF8));
        return 0;
        }
    //-------------------------------------------------------------
    int SetStippleShape(lua_State *L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        wxGetApp().GetAppOptions().SetStippleShape(
            wxString(luaL_checkstring(L, 1), wxConvUTF8));
        return 0;
        }
    //-------------------------------------------------------------
    int DisplayGraphDropShadows(lua_State *L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        wxGetApp().GetAppOptions().DisplayDropShadows(int_to_bool(lua_toboolean(L, 1)));
        return 0;
        }
    //-------------------------------------------------------------
    int SetBarChartBarColor(lua_State *L)
        {
        if (!VerifyParameterCount(L, 3, __WXFUNCTION__))
            { return 0; }
        wxGetApp().GetAppOptions().SetBarChartBarColor(
            wxColour(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3)));
        return 0;
        }
    //-------------------------------------------------------------
    int SetBarChartBarOpacity(lua_State *L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        wxGetApp().GetAppOptions().SetGraphBarOpacity(lua_tonumber(L, 1));
        return 0;
        }
    //-------------------------------------------------------------
    int SetBarChartBarEffect(lua_State *L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        wxGetApp().GetAppOptions().SetGraphBarEffect(
            static_cast<BoxEffect>(static_cast<int>(lua_tonumber(L, 1))));
        return 0;
        }
    //-------------------------------------------------------------
    int SetBarChartOrientation(lua_State *L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        wxGetApp().GetAppOptions().SetBarChartOrientation(
            static_cast<Wisteria::Orientation>(static_cast<int>(lua_tonumber(L, 1))));
        return 0;
        }
    //-------------------------------------------------------------
    int SetSpellCheckerOptions(lua_State *L)
        {
        if (lua_gettop(L) >= 1)
            {
            wxGetApp().GetAppOptions().SpellCheckIgnoreProperNouns(
                int_to_bool(lua_toboolean(L, 1)));
            }
        if (lua_gettop(L) >= 2)
            {
            wxGetApp().GetAppOptions().SpellCheckIgnoreUppercased(
                int_to_bool(lua_toboolean(L, 2)));
            }
        if (lua_gettop(L) >= 3)
            {
            wxGetApp().GetAppOptions().SpellCheckIgnoreNumerals(
                int_to_bool(lua_toboolean(L, 3)));
            }
        if (lua_gettop(L) >= 4)
            {
            wxGetApp().GetAppOptions().SpellCheckIgnoreFileAddresses(
                int_to_bool(lua_toboolean(L, 4)));
            }
        if (lua_gettop(L) >= 5)
            {
            wxGetApp().GetAppOptions().SpellCheckIgnoreProgrammerCode(
                int_to_bool(lua_toboolean(L, 5)));
            }
        if (lua_gettop(L) >= 6)
            {
            wxGetApp().GetAppOptions().SpellCheckIgnoreSocialMediaTags(
                int_to_bool(lua_toboolean(L, 6)));
            }
        if (lua_gettop(L) >= 7)
            {
            wxGetApp().GetAppOptions().SpellCheckAllowColloquialisms(
                int_to_bool(lua_toboolean(L, 7)));
            }
        wxGetApp().Yield();
        return 0;
        }
    //-------------------------------------------------------------
    int SetWindowSize(lua_State *L)
        {
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        wxGetApp().GetMainFrame()->Maximize(false);
        wxGetApp().GetMainFrame()->SetSize(
            wxGetApp().GetMainFrame()->FromDIP(lua_tonumber(L, 1)),
            wxGetApp().GetMainFrame()->FromDIP(lua_tonumber(L, 2)));
        wxGetApp().GetMainFrame()->Center();
        wxGetApp().Yield();
        return 0;
        }
    //-------------------------------------------------------------
    int SetLeftPrintHeader(lua_State *L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        wxGetApp().GetAppOptions().SetLeftPrinterHeader(
            wxString(luaL_checkstring(L, 1), wxConvUTF8));
        return 0;
        }
    //-------------------------------------------------------------
    int SetCenterPrintHeader(lua_State *L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        wxGetApp().GetAppOptions().SetCenterPrinterHeader(
            wxString(luaL_checkstring(L, 1), wxConvUTF8));
        return 0;
        }
    //-------------------------------------------------------------
    int SetRightPrintHeader(lua_State *L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        wxGetApp().GetAppOptions().SetRightPrinterHeader(
            wxString(luaL_checkstring(L, 1), wxConvUTF8));
        return 0;
        }
    //-------------------------------------------------------------
    int SetLeftPrintFooter(lua_State *L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        wxGetApp().GetAppOptions().SetLeftPrinterFooter(
            wxString(luaL_checkstring(L, 1), wxConvUTF8));
        return 0;
        }
    //-------------------------------------------------------------
    int SetCenterPrintFooter(lua_State *L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        wxGetApp().GetAppOptions().SetCenterPrinterFooter(
            wxString(luaL_checkstring(L, 1), wxConvUTF8));
        return 0;
        }
    //-------------------------------------------------------------
    int SetRightPrintFooter(lua_State *L)
        {
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            { return 0; }
        wxGetApp().GetAppOptions().SetRightPrinterFooter(
            wxString(luaL_checkstring(L, 1), wxConvUTF8));
        return 0;
        }
    }
