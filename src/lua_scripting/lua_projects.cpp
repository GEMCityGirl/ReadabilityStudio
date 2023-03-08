///////////////////////////////////////////////////////////////////////////////
// Name:        lua_projects.cpp
// Author:      Blake Madden
// Copyright:   (c) 2005-2023 Blake Madden
// Licence:     3-Clause BSD licence
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "lua_projects.h"
#include "../app/readability_app.h"
#include "../projects/standard_project_doc.h"
#include "../projects/batch_project_doc.h"
#include "../projects/batch_project_view.h"
#include "../ui/dialogs/tools_options_dlg.h"

using namespace Wisteria;
using namespace Wisteria::Graphs;
using namespace Wisteria::Colors;
using namespace Wisteria::UI;

wxDECLARE_APP(ReadabilityApp);

/* Note: with Luna, an extra boolean argument is passed into class functions at the front,
   this should be skipped over. It seems to be an indicator of the function being successfully called,
   but not sure about that.*/

namespace LuaScripting
    {
    const char StandardProject::className[] = "StandardProject";

    StandardProject::StandardProject(lua_State *L) : m_project(nullptr), m_settingsDlg(nullptr), m_delayReloading(false)
        {
        if (lua_gettop(L) > 1)//see if a path was passed in
            {
            wxString path(luaL_checkstring(L, 2), wxConvUTF8);
            wxFileName fn(path);
            fn.Normalize(wxPATH_NORM_LONG|wxPATH_NORM_DOTS|wxPATH_NORM_TILDE|wxPATH_NORM_ABSOLUTE);
            if (fn.GetExt().CmpNoCase(_DT(L"rsp")) == 0)
                {
                m_project = dynamic_cast<ProjectDoc*>(wxGetApp().GetMainFrame()->OpenFile(path));
                }
            else if (fn.GetExt().CmpNoCase(_DT(L"rsbp")) == 0)
                {
                m_project = nullptr;
                wxMessageBox(_("A standard project cannot open a batch project file."),
                             _("Project File Mismatch"), wxOK|wxICON_EXCLAMATION);
                return;
                }
            else
                {
                //create a standard project and dump the text into it
                const wxList& templateList = wxGetApp().GetMainFrame()->GetDocumentManager()->GetTemplates();
                for (size_t i = 0; i < templateList.GetCount(); ++i)
                    {
                    wxDocTemplate* docTemplate = dynamic_cast<wxDocTemplate*>(templateList.Item(i)->GetData());
                    if (docTemplate && docTemplate->GetDocClassInfo()->IsKindOf(CLASSINFO(ProjectDoc)))
                        {
                        m_project = dynamic_cast<ProjectDoc*>(docTemplate->CreateDocument(path, wxDOC_NEW));
                        if (m_project && !m_project->OnNewDocument() )
                            {
                            //Document is implicitly deleted by DeleteAllViews
                            m_project->DeleteAllViews();
                            m_project = nullptr;
                            }
                        break;
                        }
                    }
                }
            }
        wxGetApp().Yield();//yield so that the view can be fully refreshed before proceeding
        }

    bool StandardProject::ReloadIfNotDelayed()
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!m_delayReloading)
            {
            m_project->RefreshRequired(ProjectRefresh::FullReindexing);
            m_project->RefreshProject();
            wxGetApp().Yield();
            }
        return 0;
        }
    bool StandardProject::ReloadIfNotDelayedSimple()
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!m_delayReloading)
            {
            m_project->RefreshRequired(ProjectRefresh::Minimal);
            m_project->RefreshProject();
            wxGetApp().Yield();
            }
        return 0;
        }
    int StandardProject::DelayReloading(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_delayReloading = int_to_bool(lua_toboolean(L, 2));
        return 0;
        }

    int StandardProject::GetTitle(lua_State* L)
        {
        lua_pushstring(L, m_project ? m_project->GetTitle() : wxString{});
        return 1;
        }

    int StandardProject::SetWindowSize(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 3, __WXFUNCTION__))
            { return 0; }
        if (m_project->GetFirstView() && m_project->GetFirstView()->GetFrame())
            {
            m_project->GetFirstView()->GetFrame()->SetSize(lua_tonumber(L, 2), lua_tonumber(L, 3));
            m_project->GetFirstView()->GetFrame()->Center();
            wxGetApp().Yield();
            }
        return 0;
        }

    int StandardProject::GetSentenceCount(lua_State* L)
        {
        lua_pushinteger(L, m_project ? m_project->GetTotalSentences() : 0);
        return 1;
        }

    int StandardProject::GetIndependentClauseCount(lua_State* L)
        {
        lua_pushinteger(L, m_project ? m_project->GetTotalSentenceUnits() : 0);
        return 1;
        }

    int StandardProject::GetNumeralCount(lua_State* L)
        {
        lua_pushinteger(L, m_project ? m_project->GetTotalNumerals() : 0);
        return 1;
        }

    int StandardProject::GetProperNounCount(lua_State* L)
        {
        if (m_project && m_project->GetProjectLanguage() == readability::test_language::german_test)
            {
            wxMessageBox(_("ProperNounCount() not supported for German projects."),
                _("Script Error"), wxOK|wxICON_EXCLAMATION);
            lua_pushinteger(L, 0);
            return 1;
            }
        lua_pushinteger(L, m_project ? m_project->GetTotalProperNouns() : 0);
        return 1;
        }

    int StandardProject::GetUniqueWordCount(lua_State* L)
        {
        lua_pushinteger(L, m_project ? m_project->GetTotalUniqueWords() : 0);
        return 1;
        }

    int StandardProject::GetWordCount(lua_State* L)
        {
        lua_pushinteger(L, m_project ? m_project->GetTotalWords() : 0);
        return 1;
        }

    int StandardProject::GetCharacterAndPunctuationCount(lua_State* L)
        {
        lua_pushinteger(L, m_project ? m_project->GetTotalCharactersPlusPunctuation() : 0);
        return 1;
        }

    int StandardProject::GetCharacterCount(lua_State* L)
        {
        lua_pushinteger(L, m_project ? m_project->GetTotalCharacters() : 0);
        return 1;
        }

    int StandardProject::GetSyllableCount(lua_State* L)
        {
        lua_pushinteger(L, m_project ? m_project->GetTotalSyllables() : 0);
        return 1;
        }

    int StandardProject::GetUnique3SyllablePlusWordCount(lua_State* L)
        {
        lua_pushinteger(L, m_project ? m_project->GetTotalUnique3PlusSyllableWords() : 0);
        return 1;
        }

    int StandardProject::Get3SyllablePlusWordCount(lua_State* L)
        {
        lua_pushinteger(L, m_project ? m_project->GetTotal3PlusSyllabicWords() : 0);
        return 1;
        }

    int StandardProject::GetUnique1SyllableWordCount(lua_State* L)
        {
        lua_pushinteger(L, m_project ? m_project->GetTotalUniqueMonoSyllablicWords() : 0);
        return 1;
        }

    int StandardProject::Get1SyllableWordCount(lua_State* L)
        {
        lua_pushinteger(L, m_project ? m_project->GetTotalMonoSyllabicWords() : 0);
        return 1;
        }

    int StandardProject::Get7CharacterPlusWordCount(lua_State* L)
        {
        lua_pushinteger(L, m_project ? m_project->GetTotalHardLixRixWords() : 0);
        return 1;
        }

    int StandardProject::GetUnique6CharPlusWordCount(lua_State* L)
        {
        lua_pushinteger(L, m_project ? m_project->GetTotalUnique6CharsPlusWords() : 0);
        return 1;
        }

    int StandardProject::Get6CharacterPlusWordCount(lua_State* L)
        {
        lua_pushinteger(L, m_project ? m_project->GetTotalLongWords() : 0);
        return 1;
        }

    int StandardProject::GetUnfamSpacheWordCount(lua_State* L)
        {
        lua_pushinteger(L, m_project ? m_project->GetTotalHardWordsSpache() : 0);
        return 1;
        }

    int StandardProject::GetUnfamDCWordCount(lua_State* L)
        {
        lua_pushinteger(L, m_project ? m_project->GetTotalHardWordsDaleChall() : 0);
        return 1;
        }

    int StandardProject::GetUnfamHJWordCount(lua_State* L)
        {
        lua_pushinteger(L, m_project ? m_project->GetTotalHardWordsHarrisJacobson() : 0);
        return 1;
        }
    int StandardProject::SetTextExclusion(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->SetInvalidSentenceMethod(static_cast<InvalidSentence>(static_cast<int>(lua_tonumber(L, 2))));
        ReloadIfNotDelayed();
        return 0;
        };
    int StandardProject::SetIncludeIncompleteTolerance(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->SetIncludeIncompleteSentencesIfLongerThanValue(lua_tonumber(L, 2));
        return 0;
        };
    int StandardProject::AggressivelyDeduceLists(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->AggressiveExclusion(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        };
    int StandardProject::ExcludeCopyrightNotices(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->IgnoreTrailingCopyrightNoticeParagraphs(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        };
    int StandardProject::ExcludeTrailingCitations(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->IgnoreTrailingCitations(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        };
    int StandardProject::ExcludeFileAddress(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->IgnoreFileAddresses(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        };
    int StandardProject::ExcludeNumerals(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->IgnoreNumerals(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        };
    int StandardProject::ExcludeProperNouns(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->IgnoreProperNouns(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        };
    //Sets the filepath to the phrase exclusion list
    //filePath Path to phrase exclusion list
    int StandardProject::SetPhraseExclusionList(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->SetExcludedPhrasesPath(wxString(luaL_checkstring(L, 2), wxConvUTF8));
        m_project->LoadExcludePhrases();
        ReloadIfNotDelayed();
        return 0;
        };

    ///Sets the tags to exclude blocks of text.
    int StandardProject::SetBlockExclusionTags(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        const wxString exclusionTags(luaL_checkstring(L, 2), wxConvUTF8);
        m_project->GetExclusionBlockTags().clear();
        if (exclusionTags.length() >= 2)
            { m_project->GetExclusionBlockTags().push_back(std::make_pair(exclusionTags[0],exclusionTags[1])); }
        ReloadIfNotDelayed();
        return 0;
        };

    //-------------------------------------------------
    int StandardProject::SetAppendedDocumentFilePath(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->SetAppendedDocumentFilePath(wxString(luaL_checkstring(L, 2), wxConvUTF8));
        m_project->LoadAppendedDocument();
        ReloadIfNotDelayed();
        return 0;
        };

    //GRAPH OPTIONS
    int StandardProject::SetGraphBackgroundColor(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 4, __WXFUNCTION__))
            { return 0; }
        m_project->SetBackGroundColor(
            wxColour(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4)));
        ReloadIfNotDelayedSimple();
        return 0;
        }
    int StandardProject::ApplyGraphBackgroundFade(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->SetGraphBackGroundLinearGradient(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayedSimple();
        return 0;
        }
    int StandardProject::SetGraphBackgroundImage(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->SetBackGroundImagePath(wxString(luaL_checkstring(L, 2), wxConvUTF8));
        ReloadIfNotDelayedSimple();
        return 0;
        }
    int StandardProject::SetGraphBackgroundOpacity(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->SetGraphBackGroundOpacity(lua_tonumber(L, 2));
        ReloadIfNotDelayedSimple();
        return 0;
        }
    int StandardProject::SetPlotBackgroundColor(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 4, __WXFUNCTION__))
            { return 0; }
        m_project->SetPlotBackGroundColor(
            wxColour(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4)));
        ReloadIfNotDelayedSimple();
        return 0;
        }
    int StandardProject::SetPlotBackgroundOpacity(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->SetGraphPlotBackGroundOpacity(lua_tonumber(L, 2));
        ReloadIfNotDelayedSimple();
        return 0;
        }
    int StandardProject::SetGraphWatermark(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->SetWatermark(wxString(luaL_checkstring(L, 2), wxConvUTF8));
        ReloadIfNotDelayedSimple();
        return 0;
        }
    int StandardProject::SetGraphLogoImage(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->SetWatermarkLogoPath(wxString(luaL_checkstring(L, 2), wxConvUTF8));
        ReloadIfNotDelayedSimple();
        return 0;
        }
    int StandardProject::SetGraphCustomBrushImage(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->SetStippleImagePath(wxString(luaL_checkstring(L, 2), wxConvUTF8));
        ReloadIfNotDelayedSimple();
        return 0;
        }
    int StandardProject::DisplayGraphDropShadows(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->DisplayDropShadows(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayedSimple();
        return 0;
        }
    int StandardProject::SetBarChartBarColor(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 4, __WXFUNCTION__))
            { return 0; }
        m_project->SetBarChartBarColor(
            wxColour(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4)));
        ReloadIfNotDelayedSimple();
        return 0;
        }
    int StandardProject::SetBarChartBarOpacity(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->SetGraphBarOpacity(lua_tonumber(L, 2));
        ReloadIfNotDelayedSimple();
        return 0;
        }
    int StandardProject::SetBarChartBarEffect(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->SetGraphBarEffect(static_cast<BoxEffect>(static_cast<int>(lua_tonumber(L, 2))));
        ReloadIfNotDelayedSimple();
        return 0;
        }
    int StandardProject::SetBarChartOrientation(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->SetBarChartOrientation(static_cast<Wisteria::Orientation>(static_cast<int>(lua_tonumber(L, 2))));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //PROJECT SETTINGS
    int StandardProject::SetProjectLanguage(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->SetProjectLanguage(static_cast<readability::test_language>(static_cast<int>(lua_tonumber(L, 2))));
        ReloadIfNotDelayed();
        return 0;
        }
    int StandardProject::GetProjectLanguage(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        lua_pushnumber(L,static_cast<int>(m_project->GetProjectLanguage()));
        return 1;
        }
    int StandardProject::SetReviewer(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        wxGetApp().GetAppOptions().SetReviewer(wxString(luaL_checkstring(L, 2), wxConvUTF8));
        ReloadIfNotDelayed();
        return 0;
        }
    int StandardProject::GetReviewer(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        lua_pushstring(L, wxGetApp().GetAppOptions().GetReviewer().utf8_str());
        return 1;
        }
    int StandardProject::SetDocumentStorageMethod(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->SetDocumentStorageMethod(static_cast<TextStorage>(static_cast<int>(lua_tonumber(L, 2))));
        ReloadIfNotDelayed();
        return 0;
        };
    int StandardProject::GetDocumentStorageMethod(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        lua_pushnumber(L,static_cast<int>(m_project->GetDocumentStorageMethod()));
        return 1;
        };

    int StandardProject::SetParagraphsParsingMethod(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->SetParagraphsParsingMethod(static_cast<ParagraphParse>(static_cast<int>(lua_tonumber(L, 2))));
        ReloadIfNotDelayed();
        return 0;
        };
    int StandardProject::GetParagraphsParsingMethod(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        lua_pushnumber(L,static_cast<int>(m_project->GetParagraphsParsingMethod()));
        return 1;
        };

    int StandardProject::SetSpellCheckerOptions(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        if (lua_gettop(L) >= 2)
            { m_project->SpellCheckIgnoreProperNouns(int_to_bool(lua_toboolean(L, 2))); }
        if (lua_gettop(L) >= 3)
            { m_project->SpellCheckIgnoreUppercased(int_to_bool(lua_toboolean(L, 3))); }
        if (lua_gettop(L) >= 4)
            { m_project->SpellCheckIgnoreNumerals(int_to_bool(lua_toboolean(L, 4))); }
        if (lua_gettop(L) >= 5)
            { m_project->SpellCheckIgnoreFileAddresses(int_to_bool(lua_toboolean(L, 5))); }
        if (lua_gettop(L) >= 6)
            { m_project->SpellCheckIgnoreProgrammerCode(int_to_bool(lua_toboolean(L, 6))); }
        if (lua_gettop(L) >= 7)
            { m_project->SpellCheckAllowColloquialisms(int_to_bool(lua_toboolean(L, 7))); }
        ReloadIfNotDelayed();
        return 0;
        };

    int StandardProject::AddTest(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            {
            lua_pushboolean(L, false);
            return 1;
            }
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            {
            lua_pushboolean(L, false);
            return 1;
            }
        ProjectView* view = dynamic_cast<ProjectView*>(m_project->GetFirstView());
        if (view)
            {
            const wxString testName(luaL_checkstring(L, 2), wxConvUTF8);
            if (m_project->GetReadabilityTests().has_test(testName))
                {
                std::pair<std::vector<readability::readability_project_test>::const_iterator, bool> result =
                    m_project->GetReadabilityTests().find_test(testName);
                wxCommandEvent cmd(wxEVT_NULL, result.first->get_test().get_interface_id());
                view->OnAddTest(cmd);
                }
            else if (testName.CmpNoCase(_DT(L"dolch")) == 0)
                {
                wxCommandEvent cmd(wxEVT_NULL, XRCID("ID_DOLCH"));
                view->OnAddTest(cmd);
                }
            else
                {
                std::map<int, wxString>::const_iterator pos;
                for (pos = MainFrame::GetCustomTestMenuIds().begin();
                    pos != MainFrame::GetCustomTestMenuIds().end();
                    ++pos)
                    {
                    if (pos->second.CmpNoCase(testName) == 0)
                        { break; }
                    }
                if (pos != MainFrame::GetCustomTestMenuIds().end())
                    {
                    m_project->AddCustomReadabilityTest(pos->second, true);
                    CustomReadabilityTestCollection::iterator testIter = 
                        std::find(BaseProject::m_custom_word_tests.begin(), BaseProject::m_custom_word_tests.end(), testName);
                    //find the test
                    if (testIter != BaseProject::m_custom_word_tests.end())
                        {
                        //projects will need to do a full re-indexing
                        m_project->RefreshRequired(ProjectRefresh::FullReindexing);
                        m_project->RefreshProject();
                        }
                    else
                        {
                        lua_pushboolean(L, false);
                        return 1;
                        }
                    }
                else
                    {
                    wxMessageBox(wxString::Format(_("%s: Unknown test could not be added."), testName),
                        _("Script Error"), wxOK|wxICON_EXCLAMATION);
                    lua_pushboolean(L, false);
                    return 1;
                    }
                }
            wxGetApp().Yield();
            lua_pushboolean(L, true);
            return 1;
            }
        lua_pushboolean(L, false);
        return 1;
        }

    //Reanalyzes the documents.
    int StandardProject::Reload(lua_State*)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        m_project->RefreshRequired(ProjectRefresh::FullReindexing);
        m_project->RefreshProject();
        wxGetApp().Yield();
        return 0;
        }

    /// Closes the project.
    /// @SaveChanges Specifies whether to save any changes made to the project before closing it. Default is to not save any changes.
    int StandardProject::Close(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }

        wxGetApp().Yield();

        // save it or turn off modified flag before closing
        if (m_project->IsModified())
            {
            if (lua_gettop(L) > 1 && int_to_bool(lua_toboolean(L, 2)) == true)
                {
                if (!m_project->Save())
                    { return 0; }
                }
            else
                { m_project->Modify(false); }
            }

        m_project->GetDocumentManager()->CloseDocument(m_project, true);
        wxGetApp().ProcessIdle(); // the view won't be fully deleted until idle processing takes place
        m_project = nullptr;
        return 0;
        }

    //Exports all of the results from the project into a folder.
    //FolderPath The folder to save the project's results.
    int StandardProject::ExportAll(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        wxString outputPath(luaL_checkstring(L, 2), wxConvUTF8);
        ProjectView* view = dynamic_cast<ProjectView*>(m_project->GetFirstView());
        if (view)
            {
            view->ExportAll(outputPath, BaseProjectDoc::GetExportListExt(),
                            BaseProjectDoc::GetExportTextViewExt(), BaseProjectDoc::GetExportGraphExt(),
                            true, true, true, true, true, true, true, true, Wisteria::UI::ImageExportOptions());
            }
        return 0;
        }

    //Saves the scores to an HTML file.
    //FilePath The file path to save the scores.
    int StandardProject::ExportScores(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }

        ProjectView* view = dynamic_cast<ProjectView*>(m_project->GetFirstView());
        if (view)
            {
            ExplanationListCtrl* scoresWindow = dynamic_cast<ExplanationListCtrl*>(view->GetReadabilityResultsView().FindWindowById(BaseProjectView::READABILITY_SCORES_PAGE_ID) );
            if (scoresWindow)
                {
                const ProjectDoc* doc = dynamic_cast<ProjectDoc*>(view->GetDocument());
                const wxString originalLabel = scoresWindow->GetLabel();
                scoresWindow->SetLabel(originalLabel + wxString::Format(wxT(" [%s]"), wxFileName::StripExtension(doc->GetTitle())));
                lua_pushboolean(L, scoresWindow->Save(wxString(luaL_checklstring(L, 2, nullptr), wxConvUTF8)) );
                scoresWindow->SetLabel(originalLabel);
                }
            else
                {
                wxMessageBox(_("Unable to find the scores in the project."),
                     _("Script Error"), wxOK|wxICON_EXCLAMATION);
                }
            }
        return 0;
        }

    //Saves a graph from the project as an image.
    //GraphType Which graph to save. Available types are: GraphsTypes.WordBarChart,
    //GraphsTypes.Fry, GraphsTypes.GpmFry, GraphsTypes.FRASE, GraphsTypes.Raygor,
    //GraphsTypes.DolchCoverageChart, GraphsTypes.DolchWordBarChart, GraphsTypes.Flesch.
    //FilePath The file path to save the graph.
    //GrayScale Whether to save the image in black & white.
    //Width The width of the output image.
    //Height The height of the output image.
    int StandardProject::ExportGraph(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 3, __WXFUNCTION__))
            { return 0; }

        ProjectView* view = dynamic_cast<ProjectView*>(m_project->GetFirstView());
        if (view)
            {
            Wisteria::Canvas* graphWindow = dynamic_cast<Wisteria::Canvas*>(view->GetReadabilityResultsView().FindWindowById(lua_tonumber(L, 2)) );
            if (!graphWindow)//look in stats summary section if not in the readability section
                { graphWindow = dynamic_cast<Wisteria::Canvas*>(view->GetSummaryView().FindWindowById(lua_tonumber(L, 2)) ); }
            if (!graphWindow)//look in word section if not in the stats section
                { graphWindow = dynamic_cast<Wisteria::Canvas*>(view->GetWordsBreakdownView().FindWindowById(lua_tonumber(L, 2)) ); }
            if (!graphWindow)//look in sentence section if not in the words section
                { graphWindow = dynamic_cast<Wisteria::Canvas*>(view->GetSentencesBreakdownView().FindWindowById(lua_tonumber(L, 2)) ); }
            if (!graphWindow)//look in Dolch section if not in the stats summary section
                { graphWindow = dynamic_cast<Wisteria::Canvas*>(view->GetDolchSightWordsView().FindWindowById(lua_tonumber(L, 2)) ); }
            if (graphWindow)
                {
                const ProjectDoc* doc = dynamic_cast<ProjectDoc*>(view->GetDocument());
                const wxString originalLabel = graphWindow->GetLabel();
                graphWindow->SetLabel(originalLabel + wxString::Format(wxT(" [%s]"), wxFileName::StripExtension(doc->GetTitle())));
                Wisteria::UI::ImageExportOptions opt;
                if (lua_gettop(L) >= 4 && lua_toboolean(L, 4))
                    { opt.m_mode = static_cast<decltype(opt.m_mode)>(Wisteria::UI::ImageExportOptions::ColorMode::Greyscale); }
                if (lua_gettop(L) >= 5)
                    { opt.m_imageSize.SetWidth(lua_tonumber(L, 5)); }
                if (lua_gettop(L) >= 6)
                    { opt.m_imageSize.SetHeight(lua_tonumber(L, 6)); }
                lua_pushboolean(L, graphWindow->Save(wxString(luaL_checklstring(L, 3, nullptr), wxConvUTF8), opt) );
                graphWindow->SetLabel(originalLabel);
                }
            else
                {
                wxMessageBox(wxString::Format(_("Unable to find the specified graph (%d) in the project."), static_cast<int>(lua_tonumber(L, 2))),
                     _("Script Error"), wxOK|wxICON_EXCLAMATION);
                }
            }
        return 0;
        }

    //Saves a highlighted words report from the project.
    //ReportType Which report to save.
    //FilePath The file path to save the report.
    int StandardProject::ExportHighlightedWords(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 3, __WXFUNCTION__))
            { return 0; }

        ProjectView* view = dynamic_cast<ProjectView*>(m_project->GetFirstView());
        if (view)
            {
            wxFormattedTextCtrl* textWindow = dynamic_cast<wxFormattedTextCtrl*>(view->GetWordsBreakdownView().FindWindowById(lua_tonumber(L, 2), CLASSINFO(wxFormattedTextCtrl)) );
            if (!textWindow)//look in grammar section if not in the highlighted words section
                { textWindow = dynamic_cast<wxFormattedTextCtrl*>(view->GetGrammarView().FindWindowById(lua_tonumber(L, 2)) ); }
            if (!textWindow)//look in Dolch section if not in the grammar section
                { textWindow = dynamic_cast<wxFormattedTextCtrl*>(view->GetDolchSightWordsView().FindWindowById(lua_tonumber(L, 2)) ); }
            if (textWindow)
                {
                const ProjectDoc* doc = dynamic_cast<ProjectDoc*>(view->GetDocument());
                const wxString originalLabel = textWindow->GetLabel();
                textWindow->SetTitleName(originalLabel + wxString::Format(wxT(" [%s]"), wxFileName::StripExtension(doc->GetTitle())));
                lua_pushboolean(L, textWindow->Save(wxString(luaL_checklstring(L, 3, nullptr), wxConvUTF8)) );
                textWindow->SetTitleName(originalLabel);
                }
            else
                {
                wxMessageBox(wxString::Format(_("Unable to find the specified highlighted words (%d) in the project."), static_cast<int>(lua_tonumber(L, 2))),
                     _("Script Error"), wxOK|wxICON_EXCLAMATION);
                }
            }
        return 0;
        }

    //Saves a summary report from the project.
    //ReportType Which report to save.
    //FilePath The file path to save the report.
    int StandardProject::ExportReport(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 3, __WXFUNCTION__))
            { return 0; }

        ProjectView* view = dynamic_cast<ProjectView*>(m_project->GetFirstView());
        if (view)
            {
            const ProjectDoc* doc = dynamic_cast<ProjectDoc*>(view->GetDocument());
            if (lua_tonumber(L, 2) == BaseProjectView::STATS_REPORT_PAGE_ID)
                {
                HtmlTableWindow* window = dynamic_cast<HtmlTableWindow*>(view->GetSummaryView().FindWindowById(lua_tonumber(L, 2)) );
                if (window)
                    {
                    const wxString originalLabel = window->GetLabel();
                    window->SetLabel(originalLabel + wxString::Format(wxT(" [%s]"), wxFileName::StripExtension(doc->GetTitle())));
                    lua_pushboolean(L, window->Save(wxString(luaL_checklstring(L, 3, nullptr), wxConvUTF8)));
                    window->SetLabel(originalLabel);
                    }
                }
            else if (lua_tonumber(L, 2) == BaseProjectView::READABILITY_SCORES_SUMMARY_REPORT_PAGE_ID)
                {
                HtmlTableWindow* window = dynamic_cast<HtmlTableWindow*>(view->GetReadabilityResultsView().FindWindowById(lua_tonumber(L, 2)) );
                if (window)
                    {
                    const wxString originalLabel = window->GetLabel();
                    window->SetLabel(originalLabel + wxString::Format(wxT(" [%s]"), wxFileName::StripExtension(doc->GetTitle())));
                    lua_pushboolean(L, window->Save(wxString(luaL_checklstring(L, 3, nullptr), wxConvUTF8)));
                    window->SetLabel(originalLabel);
                    }
                }
            else if (lua_tonumber(L, 2) == BaseProjectView::READABILITY_SCORES_PAGE_ID)
                {
                ExplanationListCtrl* window = dynamic_cast<ExplanationListCtrl*>(view->GetReadabilityResultsView().FindWindowById(lua_tonumber(L, 2)) );
                if (window)
                    {
                    const wxString originalLabel = window->GetLabel();
                    window->SetLabel(originalLabel + wxString::Format(wxT(" [%s]"), wxFileName::StripExtension(doc->GetTitle())));
                    lua_pushboolean(L, window->Save(wxString(luaL_checklstring(L, 3, nullptr), wxConvUTF8)));
                    window->SetLabel(originalLabel);
                    }
                }
            else
                {
                wxMessageBox(wxString::Format(_("Unable to find the specified list (%d) in the project."), static_cast<int>(lua_tonumber(L, 2))),
                     _("Script Error"), wxOK|wxICON_EXCLAMATION);
                }
            }
        return 0;
        }

    //Saves a list from the project.
    //ListType Which list to save.
    //FilePath The file path to save the list.
    int StandardProject::ExportList(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 3, __WXFUNCTION__))
            { return 0; }

        ProjectView* view = dynamic_cast<ProjectView*>(m_project->GetFirstView());
        if (view)
            {
            ListCtrlEx* listWindow = dynamic_cast<ListCtrlEx*>(view->GetWordsBreakdownView().FindWindowById(lua_tonumber(L, 2)) );
            if (!listWindow)//look in grammar section if not in the highlighted words section
                { listWindow = dynamic_cast<ListCtrlEx*>(view->GetGrammarView().FindWindowById(lua_tonumber(L, 2)) ); }
            if (!listWindow)//look in sentences section if not in the grammar section
                { listWindow = dynamic_cast<ListCtrlEx*>(view->GetSentencesBreakdownView().FindWindowById(lua_tonumber(L, 2)) ); }
            if (!listWindow)//look in Dolch section if not in the sentences section
                { listWindow = dynamic_cast<ListCtrlEx*>(view->GetDolchSightWordsView().FindWindowById(lua_tonumber(L, 2)) ); }
            if (!listWindow)//look in stats section if not in the Dolch section
                { listWindow = dynamic_cast<ListCtrlEx*>(view->GetSummaryView().FindWindowById(lua_tonumber(L, 2)) ); }
            if (listWindow)
                {
                const ProjectDoc* doc = dynamic_cast<ProjectDoc*>(view->GetDocument());
                const wxString originalLabel = listWindow->GetLabel();
                listWindow->SetLabel(originalLabel + wxString::Format(wxT(" [%s]"), wxFileName::StripExtension(doc->GetTitle())));
                GridExportOptions exportOptions;
                exportOptions.m_fromRow = (lua_gettop(L) > 3) ? lua_tonumber(L, 4) : 1;
                exportOptions.m_toRow = (lua_gettop(L) > 4) ? lua_tonumber(L, 5) : -1;
                exportOptions.m_fromColumn = (lua_gettop(L) > 5) ? lua_tonumber(L, 6) : 1;
                exportOptions.m_toColumn = (lua_gettop(L) > 6) ? lua_tonumber(L, 7) : -1;
                exportOptions.m_includeColumnHeaders = (lua_gettop(L) > 7) ? int_to_bool(lua_toboolean(L, 8)) : true;
                exportOptions.m_pageUsingPrinterSettings = (lua_gettop(L) > 8) ? int_to_bool(lua_toboolean(L, 9)) : false;
                lua_pushboolean(L, listWindow->Save(wxString(luaL_checklstring(L, 3, nullptr), wxConvUTF8), exportOptions));
                listWindow->SetLabel(originalLabel);
                }
            else
                {
                wxMessageBox(wxString::Format(_("Unable to find the specified list (%d) in the project."), static_cast<int>(lua_tonumber(L, 2))),
                     _("Script Error"), wxOK|wxICON_EXCLAMATION);
                }
            }
        return 0;
        }

    //Saves a copy of the project's document with excluded text (and other optional items) filtered out.
    int StandardProject::ExportFilteredText(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 7, __WXFUNCTION__))
            { return 0; }
        wxString filteredText;
        m_project->FormatFilteredText(filteredText, int_to_bool(lua_toboolean(L, 3)),
                                      int_to_bool(lua_toboolean(L, 4)), int_to_bool(lua_toboolean(L, 5)),
                                      int_to_bool(lua_toboolean(L, 6)), int_to_bool(lua_toboolean(L, 7)),
                                      int_to_bool(lua_toboolean(L, 8)));

        const wxString exportFilePath = wxString(luaL_checklstring(L, 2, nullptr), wxConvUTF8);
        wxFileName::Mkdir(wxFileName(exportFilePath).GetPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
        wxFileName(exportFilePath).SetPermissions(wxS_DEFAULT);
        wxFile filteredFile(exportFilePath, wxFile::write);
        //write out UTF-8 Windows marker if text isn't being Romanized
   #ifdef __WXMSW__
        if (filteredText.length() && !lua_toboolean(L, 3))
            { filteredFile.Write(utf8::bom, sizeof(utf8::bom)); }
    #endif
        if (!filteredFile.Write(filteredText))
            {
            wxLogError(L"Unable to write to output file.");
            lua_pushboolean(L, false);
            return 1;
            }
        lua_pushboolean(L, true);
        return 1;
        }

    /*Selects a text window in the Highlighted Reports section. Also optionally selects a range of text in that window.
    WindowToSelect The text window to select.
    StartPosition Character position to begin selection.
    EndPosition Character position to end selection.*/
    int StandardProject::SelectHighlightedWordReport(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        ProjectView* view = dynamic_cast<ProjectView*>(m_project->GetFirstView());
        if (view)
            {
            const int windowToSelect = lua_tonumber(L, 2);
            Wisteria::UI::SideBar::SideBarSubItem subItem;
            subItem.m_id = windowToSelect;
            subItem.m_iconIndex = 0;
            const auto [parentPos, childPos] = view->GetSideBar()->FindSubItem(
                BaseProjectView::SIDEBAR_WORDS_BREAKDOWN_SECTION_ID, subItem);
            if (parentPos.has_value() && childPos.has_value())
                {
                view->GetSideBar()->CollapseAll();
                view->GetSideBar()->SelectSubItem(parentPos.value(), childPos.value());
                if (lua_gettop(L) >= 3)
                    {
                    wxWindow* selWindow = view->GetWordsBreakdownView().FindWindowById(windowToSelect, wxCLASSINFO(wxFormattedTextCtrl));
                    if (selWindow && selWindow->IsKindOf(wxCLASSINFO(wxFormattedTextCtrl)))
                        {
                        dynamic_cast<wxFormattedTextCtrl*>(selWindow)->ShowPosition(lua_tonumber(L, 3));
                        dynamic_cast<wxFormattedTextCtrl*>(selWindow)->SetSelection(lua_tonumber(L, 3), lua_tonumber(L, 4));
                        selWindow->SetFocus();
                        }
                    }
                }
            }
        wxGetApp().Yield();//yield so that the view can be fully refreshed before proceeding
        return 0;
        }
    /// Shows or hides the sidebar
    int StandardProject::ShowSidebar(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        auto view = dynamic_cast<ProjectView*>(m_project->GetFirstView());
        if (view)
            {
            lua_toboolean(L, 2) ? view->GetSideBar()->Maximize() : view->GetSideBar()->Minimize();
            wxWindowUpdateLocker noUpdates(view->GetDocFrame());
            view->GetSplitter()->SetMinimumPaneSize(view->GetSideBar()->GetMinSize().GetWidth());
            view->GetSplitter()->SetSashPosition(view->GetSideBar()->GetMinSize().GetWidth());
            }
        return 0;
        }
    /*Selects the specified section and subwindow.
    Section The section to select.
    Window The subwindow in the section to select.*/
    int StandardProject::SelectWindow(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        ProjectView* view = dynamic_cast<ProjectView*>(m_project->GetFirstView());
        if (view)
            {
            const auto [parentPos, childPos] = view->GetSideBar()->FindSubItem(lua_tonumber(L, 2), lua_tonumber(L, 3));
            if (parentPos.has_value() && childPos.has_value())
                {
                view->GetSideBar()->CollapseAll();
                view->GetSideBar()->SelectSubItem(parentPos.value(), childPos.value());
                }
            else
                {
                const auto index = view->GetSideBar()->FindFolder(lua_tonumber(L, 2));
                if (index.has_value())
                    {
                    view->GetSideBar()->CollapseAll();
                    view->GetSideBar()->SelectFolder(index.value());
                    }
                }
            }
        wxGetApp().Yield();//yield so that the view can be fully refreshed before proceeding
        return 0;
        }
    /*Selects the Readability Results section of the project and highlights a test by index.
    TestToSelect The test to select, based on position in the list.*/
    int StandardProject::SelectReadabilityTest(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        ProjectView* view = dynamic_cast<ProjectView*>(m_project->GetFirstView());
        if (view)
            {
            const auto index = view->GetSideBar()->FindFolder(BaseProjectView::SIDEBAR_READABILITY_SCORES_SECTION_ID);
            if (index.has_value())
                {
                view->GetSideBar()->CollapseAll();
                view->GetSideBar()->SelectFolder(index.value());
                view->GetReadabilityScoresList()->GetResultsListCtrl()->
                    Select(lua_tonumber(L, 2)-1/*make it zero-indexed*/);
                }
            }
        wxGetApp().Yield();//yield so that the view can be fully refreshed before proceeding
        return 0;
        }
    /*Sorts a list.
    ListToSort The list window to sort. Refer to ListTypes enumeration.
    ColumnToSort The column in the list to sort.
    Order The order to sort.*/
    int StandardProject::SortList(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 3, __WXFUNCTION__))
            { return 0; }
        ProjectView* view = dynamic_cast<ProjectView*>(m_project->GetFirstView());
        if (view)
            {
            const wxWindowID listID = lua_tonumber(L, 2);
            // look in the words section
            ListCtrlEx* listWindow = dynamic_cast<ListCtrlEx*>(view->GetWordsBreakdownView().FindWindowById(listID) );
            if (!listWindow)//look in grammar section if not in the highlighted words section
                { listWindow = dynamic_cast<ListCtrlEx*>(view->GetGrammarView().FindWindowById(listID) ); }
            if (!listWindow)//look in Dolch section if not in the grammar section
                { listWindow = dynamic_cast<ListCtrlEx*>(view->GetDolchSightWordsView().FindWindowById(listID) ); }
            if (!listWindow)//look in stats section if not in the Dolch section
                { listWindow = dynamic_cast<ListCtrlEx*>(view->GetSummaryView().FindWindowById(listID) ); }
            if (!listWindow)//look in stats section if not in the sentences section
                { listWindow = dynamic_cast<ListCtrlEx*>(view->GetSentencesBreakdownView().FindWindowById(listID) ); }
            if (listWindow)
                {
                std::vector<std::pair<size_t,Wisteria::SortDirection>> columns;
                for (int i = 3; i <= lua_gettop(L); i+=2)
                    { columns.push_back(std::pair<size_t,Wisteria::SortDirection>(lua_tonumber(L, i)-1/*make it zero-indexed*/, static_cast<Wisteria::SortDirection>(static_cast<int>(lua_tonumber(L, i+1))))); }
                listWindow->SortColumns(columns);
                }
            }
        wxGetApp().Yield();//yield so that the view can be fully refreshed before proceeding
        return 0;
        }
    /* Sorts a graph.
       GraphToSort The graph window to sort. Refer to GraphTypes enumeration.
       Order The order to sort.*/
    int StandardProject::SortGraph(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 3, __WXFUNCTION__))
            { return 0; }
        ProjectView* view = dynamic_cast<ProjectView*>(m_project->GetFirstView());
        if (view)
            {
            const wxWindowID listID = lua_tonumber(L, 2);
            Wisteria::Canvas* graphWindow = dynamic_cast<Wisteria::Canvas*>(view->GetSummaryView().FindWindowById(listID) );
            if (!graphWindow) // look in Dolch section if not in the summary section
                { graphWindow = dynamic_cast<Wisteria::Canvas*>(view->GetDolchSightWordsView().FindWindowById(listID)); }
            if (!graphWindow) // look in words section
                { graphWindow = dynamic_cast<Wisteria::Canvas*>(view->GetWordsBreakdownView().FindWindowById(listID)); }
            if (!graphWindow) // look in sentences section
                { graphWindow = dynamic_cast<Wisteria::Canvas*>(view->GetSentencesBreakdownView().FindWindowById(listID)); }
            if (!graphWindow) // look in grammar section
                { graphWindow = dynamic_cast<Wisteria::Canvas*>(view->GetGrammarView().FindWindowById(listID)); }
            if (graphWindow)
                {
                std::dynamic_pointer_cast<BarChart>(
                    graphWindow->GetFixedObject(0,0))->SortBars(
                        BarChart::BarSortComparison::SortByBarLength,
                        static_cast<Wisteria::SortDirection>(static_cast<int>(lua_tonumber(L, 3))));
                }
            }
        wxGetApp().Yield(); // yield so that the view can be fully refreshed before proceeding
        return 0;
        }
    /*Selects a set of rows in a list in the Words Breakdown section.
    WindowToSelect The list window to select items in.
    RowsToSelect Rows to select. This can be a variable number of arguments.*/
    int StandardProject::SelectRowsInWordsBreakdownList(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        ProjectView* view = dynamic_cast<ProjectView*>(m_project->GetFirstView());
        if (view)
            {
            const int windowToSelect = lua_tonumber(L, 2);
            const auto [parentPos, childPos] = view->GetSideBar()->FindSubItem(BaseProjectView::SIDEBAR_WORDS_BREAKDOWN_SECTION_ID, windowToSelect);
            if (parentPos.has_value() && childPos.has_value())
                {
                view->GetSideBar()->CollapseAll();
                view->GetSideBar()->SelectSubItem(parentPos.value(), childPos.value());
                wxWindow* selWindow = view->GetWordsBreakdownView().FindWindowById(windowToSelect);
                if (selWindow && selWindow->IsKindOf(CLASSINFO(ListCtrlEx)))
                    {
                    for (int i = 3; i <= lua_gettop(L); ++i)
                        { dynamic_cast<ListCtrlEx*>(selWindow)->Select(lua_tonumber(L, i)-1/*make it zero-indexed*/); }
                    }
                if (selWindow)
                    { selWindow->SetFocus(); }
                }
            }
        wxGetApp().Yield();//yield so that the view can be fully refreshed before proceeding
        return 0;
        }
    /*Selects and scrolls down a text window.
    WindowToSelect The text window to select.
    Position Character position to scroll into view.*/
    int StandardProject::ScrollTextWindow(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        ProjectView* view = dynamic_cast<ProjectView*>(m_project->GetFirstView());
        if (view)
            {
            const int windowToSelect = lua_tonumber(L, 2);
            auto item = view->GetSideBar()->FindSubItem(BaseProjectView::SIDEBAR_WORDS_BREAKDOWN_SECTION_ID, windowToSelect);
            if (!item.second.has_value())
                { item = view->GetSideBar()->FindSubItem(BaseProjectView::SIDEBAR_GRAMMAR_SECTION_ID, windowToSelect); }
            if (item.second.has_value())
                {
                view->GetSideBar()->CollapseAll();
                view->GetSideBar()->SelectSubItem(item.first.value(), item.second.value());
                wxWindow* selWindow = view->GetWordsBreakdownView().FindWindowById(windowToSelect);
                if (selWindow == nullptr)
                    { selWindow = view->GetGrammarView().FindWindowById(windowToSelect); }
                if (selWindow && selWindow->IsKindOf(CLASSINFO(wxFormattedTextCtrl)))
                    {
                    dynamic_cast<wxFormattedTextCtrl*>(selWindow)->ShowPosition(lua_tonumber(L, 3));
                    }
                }
            }
        wxGetApp().Yield();//yield so that the view can be fully refreshed before proceeding
        return 0;
        }
    /*Select the text window in the Grammar section. Also can optionally select a range of text.
    StartPosition Character position to begin selection.
    EndPosition Character position to end selection.*/
    int StandardProject::SelectTextGrammarWindow(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        ProjectView* view = dynamic_cast<ProjectView*>(m_project->GetFirstView());
        if (view)
            {
            const auto [parentPos, childPos] = view->GetSideBar()->FindSubItem(BaseProjectView::SIDEBAR_GRAMMAR_SECTION_ID, BaseProjectView::LONG_SENTENCES_AND_WORDINESS_TEXT_PAGE_ID);
            if (parentPos.has_value() && childPos.has_value())
                {
                if (lua_gettop(L) >= 2)
                    {
                    wxWindow* selWindow = view->GetGrammarView().FindWindowById(BaseProjectView::LONG_SENTENCES_AND_WORDINESS_TEXT_PAGE_ID);
                    if (selWindow && selWindow->IsKindOf(CLASSINFO(wxFormattedTextCtrl)))
                        {
                        dynamic_cast<wxFormattedTextCtrl*>(selWindow)->ShowPosition(lua_tonumber(L, 2));
                        dynamic_cast<wxFormattedTextCtrl*>(selWindow)->SetSelection(lua_tonumber(L, 2), lua_tonumber(L, 3));
                        selWindow->SetFocus();
                        }
                    }
                view->GetSideBar()->CollapseAll();
                view->GetSideBar()->SelectSubItem(parentPos.value(), childPos.value());
                }
            }
        wxGetApp().Yield();//yield so that the view can be fully refreshed before proceeding
        return 0;
        }
    //HIDDEN interfaces for testing and screenshots
    //Opens the properties dialog and the specified page
    int StandardProject::OpenProperties(lua_State *L)
        {
        if (m_project == nullptr)
            { return 0; }
        if (m_settingsDlg == nullptr)
            { m_settingsDlg = new ToolsOptionsDlg(wxGetApp().GetMainFrame(), m_project); }
        m_settingsDlg->SelectPage(lua_tonumber(L, 2));
        m_settingsDlg->Show();
        wxGetApp().Yield();
        return 0;
        }

    int StandardProject::CloseProperties(lua_State*)
        {
        if (m_settingsDlg)
            {
            m_settingsDlg->Destroy();
            m_settingsDlg = nullptr;
            }
        return 0;
        }

    Luna<StandardProject>::PropertyType StandardProject::properties[] =
        {
            { nullptr, nullptr }
        };

    Luna<StandardProject>::FunctionType StandardProject::methods[] = {
      LUNA_DECLARE_METHOD(StandardProject, Close),
      LUNA_DECLARE_METHOD(StandardProject, DelayReloading),
      LUNA_DECLARE_METHOD(StandardProject, GetTitle),
      LUNA_DECLARE_METHOD(StandardProject, SetWindowSize),
      LUNA_DECLARE_METHOD(StandardProject, GetSentenceCount),
      LUNA_DECLARE_METHOD(StandardProject, GetIndependentClauseCount),
      LUNA_DECLARE_METHOD(StandardProject, GetNumeralCount),
      LUNA_DECLARE_METHOD(StandardProject, GetProperNounCount),
      LUNA_DECLARE_METHOD(StandardProject, GetUniqueWordCount),
      LUNA_DECLARE_METHOD(StandardProject, GetWordCount),
      LUNA_DECLARE_METHOD(StandardProject, GetCharacterAndPunctuationCount),
      LUNA_DECLARE_METHOD(StandardProject, GetCharacterCount),
      LUNA_DECLARE_METHOD(StandardProject, GetSyllableCount),
      LUNA_DECLARE_METHOD(StandardProject, GetUnique3SyllablePlusWordCount),
      LUNA_DECLARE_METHOD(StandardProject, Get3SyllablePlusWordCount),
      LUNA_DECLARE_METHOD(StandardProject, GetUnique1SyllableWordCount),
      LUNA_DECLARE_METHOD(StandardProject, Get1SyllableWordCount),
      LUNA_DECLARE_METHOD(StandardProject, Get7CharacterPlusWordCount),
      LUNA_DECLARE_METHOD(StandardProject, GetUnique6CharPlusWordCount),
      LUNA_DECLARE_METHOD(StandardProject, Get6CharacterPlusWordCount),
      LUNA_DECLARE_METHOD(StandardProject, GetUnfamSpacheWordCount),
      LUNA_DECLARE_METHOD(StandardProject, GetUnfamDCWordCount),
      LUNA_DECLARE_METHOD(StandardProject, GetUnfamHJWordCount),
      LUNA_DECLARE_METHOD(StandardProject, SetPhraseExclusionList),
      LUNA_DECLARE_METHOD(StandardProject, SetBlockExclusionTags),
      LUNA_DECLARE_METHOD(StandardProject, SetAppendedDocumentFilePath),
      LUNA_DECLARE_METHOD(StandardProject, AggressivelyDeduceLists),
      LUNA_DECLARE_METHOD(StandardProject, SetTextExclusion),
      LUNA_DECLARE_METHOD(StandardProject, SetIncludeIncompleteTolerance),
      LUNA_DECLARE_METHOD(StandardProject, ExcludeCopyrightNotices),
      LUNA_DECLARE_METHOD(StandardProject, ExcludeTrailingCitations),
      LUNA_DECLARE_METHOD(StandardProject, ExcludeFileAddress),
      LUNA_DECLARE_METHOD(StandardProject, ExcludeNumerals),
      LUNA_DECLARE_METHOD(StandardProject, ExcludeProperNouns),
      LUNA_DECLARE_METHOD(StandardProject, SetProjectLanguage),
      LUNA_DECLARE_METHOD(StandardProject, GetProjectLanguage),
      LUNA_DECLARE_METHOD(StandardProject, SetReviewer),
      LUNA_DECLARE_METHOD(StandardProject, GetReviewer),
      LUNA_DECLARE_METHOD(StandardProject, SetDocumentStorageMethod),
      LUNA_DECLARE_METHOD(StandardProject, SetParagraphsParsingMethod),
      LUNA_DECLARE_METHOD(StandardProject, GetParagraphsParsingMethod),
      LUNA_DECLARE_METHOD(StandardProject, GetDocumentStorageMethod),
      LUNA_DECLARE_METHOD(StandardProject, SetSpellCheckerOptions),
      LUNA_DECLARE_METHOD(StandardProject, SetGraphBackgroundColor),
      LUNA_DECLARE_METHOD(StandardProject, ApplyGraphBackgroundFade),
      LUNA_DECLARE_METHOD(StandardProject, SetGraphBackgroundImage),
      LUNA_DECLARE_METHOD(StandardProject, SetGraphBackgroundOpacity),
      LUNA_DECLARE_METHOD(StandardProject, SetGraphWatermark),
      LUNA_DECLARE_METHOD(StandardProject, SetGraphLogoImage),
      LUNA_DECLARE_METHOD(StandardProject, SetPlotBackgroundColor),
      LUNA_DECLARE_METHOD(StandardProject, SetPlotBackgroundOpacity),
      LUNA_DECLARE_METHOD(StandardProject, SetGraphCustomBrushImage),
      LUNA_DECLARE_METHOD(StandardProject, DisplayGraphDropShadows),
      LUNA_DECLARE_METHOD(StandardProject, SetBarChartBarColor),
      LUNA_DECLARE_METHOD(StandardProject, SetBarChartBarOpacity),
      LUNA_DECLARE_METHOD(StandardProject, SetBarChartBarEffect),
      LUNA_DECLARE_METHOD(StandardProject, SetBarChartOrientation),
      LUNA_DECLARE_METHOD(StandardProject, ExcludeFileAddress),
      LUNA_DECLARE_METHOD(StandardProject, AddTest),
      LUNA_DECLARE_METHOD(StandardProject, Reload),
      LUNA_DECLARE_METHOD(StandardProject, ExportAll),
      LUNA_DECLARE_METHOD(StandardProject, ExportScores),
      LUNA_DECLARE_METHOD(StandardProject, ExportGraph),
      LUNA_DECLARE_METHOD(StandardProject, ExportHighlightedWords),
      LUNA_DECLARE_METHOD(StandardProject, ExportReport),
      LUNA_DECLARE_METHOD(StandardProject, ExportList),
      LUNA_DECLARE_METHOD(StandardProject, ExportFilteredText),
      LUNA_DECLARE_METHOD(StandardProject, OpenProperties),
      LUNA_DECLARE_METHOD(StandardProject, CloseProperties),
      LUNA_DECLARE_METHOD(StandardProject, SelectHighlightedWordReport),
      LUNA_DECLARE_METHOD(StandardProject, SelectWindow),
      LUNA_DECLARE_METHOD(StandardProject, ShowSidebar),
      LUNA_DECLARE_METHOD(StandardProject, SelectReadabilityTest),
      LUNA_DECLARE_METHOD(StandardProject, SortList),
      LUNA_DECLARE_METHOD(StandardProject, SortGraph),
      LUNA_DECLARE_METHOD(StandardProject, SelectRowsInWordsBreakdownList),
      LUNA_DECLARE_METHOD(StandardProject, ScrollTextWindow),
      LUNA_DECLARE_METHOD(StandardProject, SelectTextGrammarWindow),
      { nullptr, nullptr }
    };

    BatchProject::BatchProject(lua_State *L) : m_project(nullptr), m_settingsDlg(nullptr), m_delayReloading(false)
        {
        if (lua_gettop(L) > 1)//see if a path was passed in
            {
            wxString path(luaL_checkstring(L, 2), wxConvUTF8);
            wxFileName fn(path);
            fn.Normalize(wxPATH_NORM_LONG|wxPATH_NORM_DOTS|wxPATH_NORM_TILDE|wxPATH_NORM_ABSOLUTE);
            if (fn.GetExt().CmpNoCase(_DT(L"rsbp")) == 0)
                {
                m_project = dynamic_cast<BatchProjectDoc*>(wxGetApp().GetMainFrame()->OpenFile(path));
                }
            else if (fn.GetExt().CmpNoCase(_DT(L"rsp")) == 0)
                {
                m_project = nullptr;
                wxMessageBox(_("A batch project cannot open a standard project file."),
                             _("Project File Mismatch"), wxOK|wxICON_EXCLAMATION);
                return;
                }
            else
                {
                //create a batch project and load a (tab delimited) list of files into it
                const wxList& templateList = wxGetApp().GetMainFrame()->GetDocumentManager()->GetTemplates();
                for (size_t i = 0; i < templateList.GetCount(); ++i)
                    {
                    wxDocTemplate* docTemplate = dynamic_cast<wxDocTemplate*>(templateList.Item(i)->GetData());
                    if (docTemplate && docTemplate->GetDocClassInfo()->IsKindOf(CLASSINFO(BatchProjectDoc)))
                        {
                        m_project = dynamic_cast<BatchProjectDoc*>(docTemplate->CreateDocument(path, wxDOC_NEW));
                        if (m_project && !m_project->OnNewDocument() )
                            {
                            //Document is implicitly deleted by DeleteAllViews
                            m_project->DeleteAllViews();
                            m_project = nullptr;
                            }
                        break;
                        }
                    }
                }
            }
        wxGetApp().Yield();//yield so that the view can be fully refreshed before proceeding
        }
    bool BatchProject::ReloadIfNotDelayed()
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!m_delayReloading)
            {
            m_project->RefreshRequired(ProjectRefresh::FullReindexing);
            m_project->RefreshProject();
            wxGetApp().Yield();
            }
        return 0;
        }
    bool BatchProject::ReloadIfNotDelayedSimple()
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!m_delayReloading)
            {
            m_project->RefreshRequired(ProjectRefresh::Minimal);
            m_project->RefreshProject();
            wxGetApp().Yield();
            }
        return 0;
        }
    int BatchProject::DelayReloading(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_delayReloading = int_to_bool(lua_toboolean(L, 2));
        return 0;
        }
    int BatchProject::GetTitle(lua_State* L)
        {
        lua_pushstring(L, m_project ? m_project->GetTitle() : wxString{});
        return 1;
        }
    int BatchProject::SetWindowSize(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 3, __WXFUNCTION__))
            { return 0; }
        if (m_project->GetFirstView() && m_project->GetFirstView()->GetFrame())
            {
            m_project->GetFirstView()->GetFrame()->SetSize(lua_tonumber(L, 2), lua_tonumber(L, 3));
            m_project->GetFirstView()->GetFrame()->Center();
            wxGetApp().Yield();
            }
        return 0;
        }
    int BatchProject::SetIncludeIncompleteTolerance(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->SetIncludeIncompleteSentencesIfLongerThanValue(lua_tonumber(L, 2));
        ReloadIfNotDelayed();
        return 0;
        };
    int BatchProject::SetTextExclusion(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->SetInvalidSentenceMethod(static_cast<InvalidSentence>(static_cast<int>(lua_tonumber(L, 2))));
        ReloadIfNotDelayed();
        return 0;
        };
    int BatchProject::AggressivelyDeduceLists(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->AggressiveExclusion(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        };
    int BatchProject::ExcludeCopyrightNotices(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->IgnoreTrailingCopyrightNoticeParagraphs(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        };
    int BatchProject::ExcludeTrailingCitations(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->IgnoreTrailingCitations(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayed();
        lua_pushboolean(L, true);
        return 0;
        };
    int BatchProject::ExcludeFileAddress(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->IgnoreFileAddresses(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        };
    int BatchProject::ExcludeNumerals(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->IgnoreNumerals(int_to_bool(lua_toboolean(L, 2)));
        return 0;
        };
    int BatchProject::ExcludeProperNouns(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->IgnoreProperNouns(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        };
    //Sets the filepath to the phrase exclusion list
    //filePath Path to phrase exclusion list
    int BatchProject::SetPhraseExclusionList(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->SetExcludedPhrasesPath(wxString(luaL_checkstring(L, 2), wxConvUTF8));
        m_project->LoadExcludePhrases();
        ReloadIfNotDelayed();
        return 0;
        };
    ///Sets the tags to exclude blocks of text.
    int BatchProject::SetBlockExclusionTags(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        const wxString exclusionTags(luaL_checkstring(L, 2), wxConvUTF8);
        m_project->GetExclusionBlockTags().clear();
        if (exclusionTags.length() >= 2)
            { m_project->GetExclusionBlockTags().push_back(std::make_pair(exclusionTags[0],exclusionTags[1])); }
        ReloadIfNotDelayed();
        return 0;
        };

    //-------------------------------------------------
    int BatchProject::SetAppendedDocumentFilePath(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->SetAppendedDocumentFilePath(wxString(luaL_checkstring(L, 2), wxConvUTF8));
        m_project->LoadAppendedDocument();
        ReloadIfNotDelayed();
        return 0;
        };

    int BatchProject::SetGraphBackgroundColor(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 4, __WXFUNCTION__))
            { return 0; }
        m_project->SetBackGroundColor(
            wxColour(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4)));
        ReloadIfNotDelayedSimple();
        return 0;
        }
    int BatchProject::ApplyGraphBackgroundFade(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->SetGraphBackGroundLinearGradient(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayedSimple();
        return 0;
        }
    int BatchProject::SetGraphBackgroundImage(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->SetBackGroundImagePath(wxString(luaL_checkstring(L, 2), wxConvUTF8));
        ReloadIfNotDelayedSimple();
        return 0;
        }
    int BatchProject::SetGraphBackgroundOpacity(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->SetGraphBackGroundOpacity(lua_tonumber(L, 2));
        ReloadIfNotDelayedSimple();
        return 0;
        }
    int BatchProject::SetPlotBackgroundColor(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 4, __WXFUNCTION__))
            { return 0; }
        m_project->SetPlotBackGroundColor(
            wxColour(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4)));
        ReloadIfNotDelayedSimple();
        return 0;
        }
    int BatchProject::SetPlotBackgroundOpacity(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->SetGraphPlotBackGroundOpacity(lua_tonumber(L, 2));
        ReloadIfNotDelayedSimple();
        return 0;
        }
    int BatchProject::SetGraphWatermark(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->SetWatermark(wxString(luaL_checkstring(L, 2), wxConvUTF8));
        ReloadIfNotDelayedSimple();
        return 0;
        }
    int BatchProject::SetGraphLogoImage(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->SetWatermarkLogoPath(wxString(luaL_checkstring(L, 2), wxConvUTF8));
        ReloadIfNotDelayedSimple();
        return 0;
        }
    int BatchProject::SetGraphCustomBrushImage(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->SetStippleImagePath(wxString(luaL_checkstring(L, 2), wxConvUTF8));
        ReloadIfNotDelayedSimple();
        return 0;
        }
    int BatchProject::DisplayGraphDropShadows(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->DisplayDropShadows(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    int BatchProject::SetProjectLanguage(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->SetProjectLanguage(static_cast<readability::test_language>(static_cast<int>(lua_tonumber(L, 2))));
        ReloadIfNotDelayed();
        return 0;
        }
    int BatchProject::GetProjectLanguage(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        lua_pushnumber(L,static_cast<int>(m_project->GetProjectLanguage()));
        return 1;
        }
    int BatchProject::SetReviewer(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        wxGetApp().GetAppOptions().SetReviewer(wxString(luaL_checkstring(L, 2), wxConvUTF8));
        ReloadIfNotDelayed();
        return 0;
        }
    int BatchProject::GetReviewer(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        lua_pushstring(L, wxGetApp().GetAppOptions().GetReviewer().utf8_str());
        return 1;
        }
    int BatchProject::SetDocumentStorageMethod(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->SetDocumentStorageMethod(static_cast<TextStorage>(static_cast<int>(lua_tonumber(L, 2))));
        ReloadIfNotDelayed();
        return 0;
        };
    int BatchProject::GetDocumentStorageMethod(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        lua_pushnumber(L,static_cast<int>(m_project->GetDocumentStorageMethod()));
        return 1;
        };

    int BatchProject::SetParagraphsParsingMethod(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->SetParagraphsParsingMethod(static_cast<ParagraphParse>(static_cast<int>(lua_tonumber(L, 2))));
        ReloadIfNotDelayed();
        return 0;
        };
    int BatchProject::GetParagraphsParsingMethod(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        lua_pushnumber(L,static_cast<int>(m_project->GetParagraphsParsingMethod()));
        return 1;
        };

    int BatchProject::SetMinDocWordCountForBatch(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->SetMinDocWordCountForBatch(static_cast<int>(lua_tonumber(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        };
    int BatchProject::GetMinDocWordCountForBatch(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        lua_pushnumber(L,static_cast<int>(m_project->GetMinDocWordCountForBatch()));
        return 0;
        };
    int BatchProject::SetFilePathDisplayMode(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        m_project->SetFilePathTruncationMode(static_cast<ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode>(static_cast<int>(lua_tonumber(L, 2))));
        ReloadIfNotDelayed();
        return 0;
        };
    int BatchProject::GetFilePathDisplayMode(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        lua_pushnumber(L,static_cast<int>(m_project->GetFilePathTruncationMode()));
        return 1;
        };

    int BatchProject::SetSpellCheckerOptions(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        if (lua_gettop(L) >= 2)
            { m_project->SpellCheckIgnoreProperNouns(int_to_bool(lua_toboolean(L, 2))); }
        if (lua_gettop(L) >= 3)
            { m_project->SpellCheckIgnoreUppercased(int_to_bool(lua_toboolean(L, 3))); }
        if (lua_gettop(L) >= 4)
            { m_project->SpellCheckIgnoreNumerals(int_to_bool(lua_toboolean(L, 4))); }
        if (lua_gettop(L) >= 5)
            { m_project->SpellCheckIgnoreFileAddresses(int_to_bool(lua_toboolean(L, 5))); }
        if (lua_gettop(L) >= 6)
            { m_project->SpellCheckIgnoreProgrammerCode(int_to_bool(lua_toboolean(L, 6))); }
        if (lua_gettop(L) >= 7)
            { m_project->SpellCheckAllowColloquialisms(int_to_bool(lua_toboolean(L, 7))); }
        ReloadIfNotDelayed();
        return 0;
        };
    //Adds a test to the project.
    //TestName The name of the test to add to the project.
    int BatchProject::AddTest(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            {
            lua_pushboolean(L, false);
            return 1;
            }
        if (!VerifyParameterCount(L, 1, __WXFUNCTION__))
            {
            lua_pushboolean(L, false);
            return 1;
            }
        BatchProjectView* view = dynamic_cast<BatchProjectView*>(m_project->GetFirstView());
        if (view)
            {
            const wxString testName(luaL_checkstring(L, 2), wxConvUTF8);
            if (m_project->GetReadabilityTests().has_test(testName))
                {
                std::pair<std::vector<readability::readability_project_test>::const_iterator, bool> result =
                    m_project->GetReadabilityTests().find_test(testName);
                wxCommandEvent cmd(0, result.first->get_test().get_interface_id());
                view->OnAddTest(cmd);
                }
            else if (testName.CmpNoCase(_DT(L"dolch")) == 0)
                {
                wxCommandEvent cmd(0, XRCID("ID_DOLCH"));
                view->OnAddTest(cmd);
                }
            else
                {
                std::map<int, wxString>::const_iterator pos;
                for (pos = MainFrame::GetCustomTestMenuIds().begin();
                    pos != MainFrame::GetCustomTestMenuIds().end();
                    ++pos)
                    {
                    if (pos->second.CmpNoCase(testName) == 0)
                        { break; }
                    }
                if (pos != MainFrame::GetCustomTestMenuIds().end())
                    {
                    wxCommandEvent cmd(0, pos->first);
                    view->OnCustomTest(cmd);
                    }
                else
                    {
                    wxMessageBox(wxString::Format(_("%s: Unknown test could not be added."), testName),
                        _("Script Error"), wxOK|wxICON_EXCLAMATION);
                    lua_pushboolean(L, false);
                    return 1;
                    }
                }
            lua_pushboolean(L, true);
            return 1;
            }
        ReloadIfNotDelayed();
        lua_pushboolean(L, false);
        return 1;
        }
    //Reanalyzes the documents.
    int BatchProject::Reload(lua_State*)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        m_project->RefreshRequired(ProjectRefresh::FullReindexing);
        m_project->RefreshProject();
        wxGetApp().Yield();
        return 0;
        }

    /// Closes the project.
    /// @SaveChanges Specifies whether to save any changes made to the project before closing it. Default is to not save any changes.
    int BatchProject::Close(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }

        wxGetApp().Yield();

        // save it or turn off modified flag before closing
        if (m_project->IsModified())
            {
            if (lua_gettop(L) > 1 && int_to_bool(lua_toboolean(L, 2)) == true)
                {
                if (!m_project->Save())
                    { return 0; }
                }
            else
                { m_project->Modify(false); }
            }

        m_project->GetDocumentManager()->CloseDocument(m_project, true);
        wxGetApp().ProcessIdle(); // the view won't be fully deleted until idle processing takes place
        m_project = nullptr;
        return 0;
        }

    //Saves a list from the project.
    //ListType Which list to save.
    //FilePath The file path to save the list.
    int BatchProject::ExportList(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 3, __WXFUNCTION__))
            { return 0; }

        BatchProjectView* view = dynamic_cast<BatchProjectView*>(m_project->GetFirstView());
        if (view)
            {
            ListCtrlEx* listWindow = nullptr;
            const auto [parentPos, childPos] = view->GetSideBar()->FindSubItem(lua_tonumber(L, 2));
            if (parentPos.has_value() && childPos.has_value())
                {
                view->GetSideBar()->SelectSubItem(parentPos.value(), childPos.value());
                wxWindow* selWindow = view->GetActiveProjectWindow();
                if (selWindow && selWindow->IsKindOf(CLASSINFO(ListCtrlEx)) )
                    { listWindow = dynamic_cast<ListCtrlEx*>(selWindow); }
                }
            else
                {
                const auto index = view->GetSideBar()->FindFolder(lua_tonumber(L, 2));
                if (index.has_value())
                    {
                    view->GetSideBar()->SelectFolder(index.value());
                    wxWindow* selWindow = view->GetActiveProjectWindow();
                    if (selWindow && selWindow->IsKindOf(CLASSINFO(ListCtrlEx)) )
                        { listWindow = dynamic_cast<ListCtrlEx*>(selWindow); }
                    }
                }
            if (listWindow)
                {
                const BatchProjectDoc* doc = dynamic_cast<BatchProjectDoc*>(view->GetDocument());
                const wxString originalLabel = listWindow->GetLabel();
                listWindow->SetLabel(originalLabel + wxString::Format(wxT(" [%s]"), wxFileName::StripExtension(doc->GetTitle())));
                GridExportOptions exportOptions;
                exportOptions.m_fromRow = (lua_gettop(L) > 3) ? lua_tonumber(L, 4) : 1;
                exportOptions.m_toRow = (lua_gettop(L) > 4) ? lua_tonumber(L, 5) : -1;
                exportOptions.m_fromColumn = (lua_gettop(L) > 5) ? lua_tonumber(L, 6) : 1;
                exportOptions.m_toColumn = (lua_gettop(L) > 6) ? lua_tonumber(L, 7) : -1;
                exportOptions.m_includeColumnHeaders = (lua_gettop(L) > 7) ? int_to_bool(lua_toboolean(L, 8)) : true;
                exportOptions.m_pageUsingPrinterSettings = (lua_gettop(L) > 8) ? int_to_bool(lua_toboolean(L, 9)) : false;
                lua_pushboolean(L, listWindow->Save(wxString(luaL_checklstring(L, 3, nullptr), wxConvUTF8), exportOptions));
                listWindow->SetLabel(originalLabel);
                listWindow->SetFocus();
                }
            }
        wxGetApp().Yield();//yield so that the view can be fully refreshed before proceeding
        return 0;
        }
    //Saves a graph from the project as an image.
    //SideBarSection The section that the graph is in.
    //GraphId Which graph to save.
    //FilePath The file path to save the graph.
    //GrayScale Whether to save the image in black & white.
    //Width The width of the output image.
    //Height The height of the output image.
    int BatchProject::ExportGraph(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 4, __WXFUNCTION__))
            { return 0; }

        BatchProjectView* view = dynamic_cast<BatchProjectView*>(m_project->GetFirstView());
        if (view)
            {
            Wisteria::Canvas* graphWindow = nullptr;
            const auto [parentPos, childPos] = view->GetSideBar()->FindSubItem(lua_tonumber(L, 2),lua_tonumber(L, 3));
            if (parentPos.has_value() && childPos.has_value())
                {
                view->GetSideBar()->SelectSubItem(parentPos.value(), childPos.value());
                wxWindow* selWindow = view->GetActiveProjectWindow();
                if (selWindow && typeid(*selWindow) == typeid(Wisteria::Canvas))
                    { graphWindow = dynamic_cast<Wisteria::Canvas*>(selWindow); }
                }
            if (graphWindow)
                {
                const BatchProjectDoc* doc = dynamic_cast<BatchProjectDoc*>(view->GetDocument());
                const wxString originalLabel = graphWindow->GetLabel();
                graphWindow->SetLabel(originalLabel + wxString::Format(wxT(" [%s]"), wxFileName::StripExtension(doc->GetTitle())));
                Wisteria::UI::ImageExportOptions opt;
                if (lua_gettop(L) >= 5 && lua_toboolean(L, 5))
                    { opt.m_mode = static_cast<decltype(opt.m_mode)>(Wisteria::UI::ImageExportOptions::ColorMode::Greyscale); }
                if (lua_gettop(L) >= 6)
                    { opt.m_imageSize.SetWidth(lua_tonumber(L, 6)); }
                if (lua_gettop(L) >= 7)
                    { opt.m_imageSize.SetHeight(lua_tonumber(L, 7)); }
                lua_pushboolean(L, graphWindow->Save(wxString(luaL_checklstring(L, 4, nullptr), wxConvUTF8), opt) );
                graphWindow->SetLabel(originalLabel);
                }
            else
                {
                wxMessageBox(wxString::Format(_("Unable to find the specified graph (%d) in the project."), static_cast<int>(lua_tonumber(L, 2))),
                     _("Script Error"), wxOK|wxICON_EXCLAMATION);
                }
            }
        wxGetApp().Yield();//yield so that the view can be fully refreshed before proceeding
        return 0;
        }
    //Exports all of the results from the project into a folder.
    //FolderPath The folder to save the project's results.
    int BatchProject::ExportAll(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        wxString outputPath(luaL_checkstring(L, 2), wxConvUTF8);
        BatchProjectView* view = dynamic_cast<BatchProjectView*>(m_project->GetFirstView());
        if (view)
            {
            view->ExportAll(outputPath,BaseProjectDoc::GetExportListExt(),
                            BaseProjectDoc::GetExportGraphExt(),
                            true, true, true, true, true, true, true, Wisteria::UI::ImageExportOptions());
            }
        return 0;
        }
    /// Shows or hides the sidebar
    int BatchProject::ShowSidebar(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __WXFUNCTION__))
            { return 0; }
        auto view = dynamic_cast<BatchProjectView*>(m_project->GetFirstView());
        if (view)
            {
            lua_toboolean(L, 2) ? view->GetSideBar()->Maximize() : view->GetSideBar()->Minimize();
            wxWindowUpdateLocker noUpdates(view->GetDocFrame());
            view->GetSplitter()->SetMinimumPaneSize(view->GetSideBar()->GetMinSize().GetWidth());
            view->GetSplitter()->SetSashPosition(view->GetSideBar()->GetMinSize().GetWidth());
            }
        return 0;
        }
    /*Selects the specified section and subwindow.
    Section The section to select.
    Window The subwindow in the section to select.*/
    int BatchProject::SelectWindow(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        BatchProjectView* view = dynamic_cast<BatchProjectView*>(m_project->GetFirstView());
        if (view)
            {
            const auto [parentPos, childPos] = view->GetSideBar()->FindSubItem(lua_tonumber(L, 2), lua_tonumber(L, 3));
            if (parentPos.has_value() && childPos.has_value())
                {
                view->GetSideBar()->CollapseAll();
                view->GetSideBar()->SelectSubItem(parentPos.value(), childPos.value());
                }
            const auto index = view->GetSideBar()->FindFolder(lua_tonumber(L, 2));
            if (index.has_value())
                {
                view->GetSideBar()->CollapseAll();
                view->GetSideBar()->SelectFolder(index.value());
                }
            }
        wxGetApp().Yield();//yield so that the view can be fully refreshed before proceeding
        return 0;
        }
    /*Selects and sorts a list.
    ListToSort The list window to sort. Refer to ListTypes enumeration.
    ColumnToSort The column in the list to sort.
    Order The order to sort.*/
    int BatchProject::SortList(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__WXFUNCTION__))
            { return 0; }
        BatchProjectView* view = dynamic_cast<BatchProjectView*>(m_project->GetFirstView());
        if (view)
            {
            std::vector<std::pair<size_t,Wisteria::SortDirection>> columns;
            for (int i = 3; i <= lua_gettop(L); i+=2)
                { columns.push_back(std::pair<size_t,Wisteria::SortDirection>(lua_tonumber(L, i)-1, static_cast<Wisteria::SortDirection>(static_cast<int>(lua_tonumber(L, i+1))))); }
            const auto [parentPos, childPos] = view->GetSideBar()->FindSubItem(lua_tonumber(L, 2));
            if (parentPos.has_value() && childPos.has_value())
                {
                view->GetSideBar()->SelectSubItem(parentPos.value(), childPos.value());
                wxWindow* selWindow = view->GetActiveProjectWindow();
                if (selWindow && selWindow->IsKindOf(CLASSINFO(ListCtrlEx)))
                    { dynamic_cast<ListCtrlEx*>(selWindow)->SortColumns(columns); }
                }
            else
                {
                const auto index = view->GetSideBar()->FindFolder(lua_tonumber(L, 2));
                if (index.has_value())
                    {
                    view->GetSideBar()->SelectFolder(index.value());
                    wxWindow* selWindow = view->GetActiveProjectWindow();
                    if (selWindow && selWindow->IsKindOf(CLASSINFO(ListCtrlEx)))
                        { dynamic_cast<ListCtrlEx*>(selWindow)->SortColumns(columns); }
                    }
                }
            }
        wxGetApp().Yield();//yield so that the view can be fully refreshed before proceeding
        return 0;
        }
    //hidden functions just used for screenshots
    int BatchProject::OpenProperties(lua_State *L)
        {
        if (m_project == nullptr)
            { return 0; }
        if (m_settingsDlg == nullptr)
            { m_settingsDlg = new ToolsOptionsDlg(wxGetApp().GetMainFrame(), m_project); }
        m_settingsDlg->SelectPage(lua_tonumber(L, 2));
        m_settingsDlg->Show();
        wxGetApp().Yield();
        return 0;
        }

    int BatchProject::CloseProperties(lua_State*)
        {
        if (m_settingsDlg)
            {
            m_settingsDlg->Destroy();
            m_settingsDlg = nullptr;
            }
        return 0;
        }

    const char BatchProject::className[] = "BatchProject";

    Luna<BatchProject>::PropertyType BatchProject::properties[] =
        {
            { nullptr, nullptr }
        };

    Luna<BatchProject>::FunctionType BatchProject::methods[] = {
      LUNA_DECLARE_METHOD(BatchProject, Close),
      LUNA_DECLARE_METHOD(BatchProject, GetTitle),
      LUNA_DECLARE_METHOD(BatchProject, SetWindowSize),
      LUNA_DECLARE_METHOD(BatchProject, DelayReloading),
      LUNA_DECLARE_METHOD(BatchProject, SetProjectLanguage),
      LUNA_DECLARE_METHOD(BatchProject, GetProjectLanguage),
      LUNA_DECLARE_METHOD(BatchProject, SetReviewer),
      LUNA_DECLARE_METHOD(BatchProject, GetReviewer),
      LUNA_DECLARE_METHOD(BatchProject, SetDocumentStorageMethod),
      LUNA_DECLARE_METHOD(BatchProject, SetParagraphsParsingMethod),
      LUNA_DECLARE_METHOD(BatchProject, GetParagraphsParsingMethod),
      LUNA_DECLARE_METHOD(BatchProject, GetDocumentStorageMethod),
      LUNA_DECLARE_METHOD(BatchProject, SetMinDocWordCountForBatch),
      LUNA_DECLARE_METHOD(BatchProject, GetMinDocWordCountForBatch),
      LUNA_DECLARE_METHOD(BatchProject, SetSpellCheckerOptions),
      LUNA_DECLARE_METHOD(BatchProject, ExcludeFileAddress),
      LUNA_DECLARE_METHOD(BatchProject, SetPhraseExclusionList),
      LUNA_DECLARE_METHOD(BatchProject, SetBlockExclusionTags),
      LUNA_DECLARE_METHOD(BatchProject, SetAppendedDocumentFilePath),
      LUNA_DECLARE_METHOD(BatchProject, AggressivelyDeduceLists),
      LUNA_DECLARE_METHOD(BatchProject, SetTextExclusion),
      LUNA_DECLARE_METHOD(BatchProject, SetIncludeIncompleteTolerance),
      LUNA_DECLARE_METHOD(BatchProject, ExcludeCopyrightNotices),
      LUNA_DECLARE_METHOD(BatchProject, ExcludeTrailingCitations),
      LUNA_DECLARE_METHOD(BatchProject, ExcludeFileAddress),
      LUNA_DECLARE_METHOD(BatchProject, ExcludeNumerals),
      LUNA_DECLARE_METHOD(BatchProject, ExcludeProperNouns),
      LUNA_DECLARE_METHOD(BatchProject, SetGraphBackgroundColor),
      LUNA_DECLARE_METHOD(BatchProject, ApplyGraphBackgroundFade),
      LUNA_DECLARE_METHOD(BatchProject, SetGraphBackgroundImage),
      LUNA_DECLARE_METHOD(BatchProject, SetGraphBackgroundOpacity),
      LUNA_DECLARE_METHOD(BatchProject, SetGraphWatermark),
      LUNA_DECLARE_METHOD(BatchProject, SetGraphLogoImage),
      LUNA_DECLARE_METHOD(BatchProject, SetPlotBackgroundColor),
      LUNA_DECLARE_METHOD(BatchProject, SetPlotBackgroundOpacity),
      LUNA_DECLARE_METHOD(BatchProject, SetGraphCustomBrushImage),
      LUNA_DECLARE_METHOD(BatchProject, DisplayGraphDropShadows),
      LUNA_DECLARE_METHOD(BatchProject, AddTest),
      LUNA_DECLARE_METHOD(BatchProject, Reload),
      LUNA_DECLARE_METHOD(BatchProject, ExportAll),
      LUNA_DECLARE_METHOD(BatchProject, ExportList),
      LUNA_DECLARE_METHOD(BatchProject, ExportGraph),
      LUNA_DECLARE_METHOD(BatchProject, SelectWindow),
      LUNA_DECLARE_METHOD(BatchProject, ShowSidebar),
      LUNA_DECLARE_METHOD(BatchProject, SortList),
      LUNA_DECLARE_METHOD(BatchProject, OpenProperties),
      LUNA_DECLARE_METHOD(BatchProject, CloseProperties),
      { nullptr, nullptr }
    };
    }
