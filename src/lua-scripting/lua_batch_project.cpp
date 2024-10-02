///////////////////////////////////////////////////////////////////////////////
// Name:        lua_batch_project.cpp
// Author:      Blake Madden
// Copyright:   (c) 2005-2023 Blake Madden
// Licence:     3-Clause BSD licence
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "lua_batch_project.h"
#include "../app/readability_app.h"
#include "../projects/standard_project_doc.h"
#include "../projects/batch_project_doc.h"
#include "../projects/batch_project_view.h"
#include "../ui/dialogs/tools_options_dlg.h"
#include "../Wisteria-Dataviz/src/base/label.h"
#include "../Wisteria-Dataviz/src/base/reportbuilder.h"

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
    BatchProject::BatchProject(lua_State *L)
        {
        // see if a path was passed in
        if (lua_gettop(L) > 1)
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
                wxMessageBox(_(L"A batch project cannot open a standard project file."),
                             _(L"Project File Mismatch"), wxOK|wxICON_EXCLAMATION);
                return;
                }
            else
                {
                // create a batch project and load a (tab delimited) list of files into it
                const wxList& templateList = wxGetApp().GetMainFrame()->GetDocumentManager()->GetTemplates();
                for (size_t i = 0; i < templateList.GetCount(); ++i)
                    {
                    wxDocTemplate* docTemplate = dynamic_cast<wxDocTemplate*>(templateList.Item(i)->GetData());
                    if (docTemplate && docTemplate->GetDocClassInfo()->IsKindOf(wxCLASSINFO(BatchProjectDoc)))
                        {
                        m_project = dynamic_cast<BatchProjectDoc*>(docTemplate->CreateDocument(path, wxDOC_NEW));
                        if (m_project && !m_project->OnNewDocument() )
                            {
                            // Document is implicitly deleted by DeleteAllViews
                            m_project->DeleteAllViews();
                            m_project = nullptr;
                            }
                        break;
                        }
                    }
                }
            }
        // yield so that the view can be fully refreshed before proceeding
        wxGetApp().Yield();
        }

    //-------------------------------------------------------------
    bool BatchProject::ReloadIfNotDelayed()
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }

        if (!m_delayReloading)
            {
            m_project->RefreshRequired(ProjectRefresh::FullReindexing);
            m_project->RefreshProject();
            wxGetApp().Yield();
            }
        return 0;
        }

    //-------------------------------------------------------------
    bool BatchProject::ReloadIfNotDelayedSimple()
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }

        if (!m_delayReloading)
            {
            m_project->RefreshRequired(ProjectRefresh::Minimal);
            m_project->RefreshProject();
            wxGetApp().Yield();
            }
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::DelayReloading(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
            { return 0; }

        m_delayReloading = int_to_bool(lua_toboolean(L, 2));
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetTitle(lua_State* L)
        {
        lua_pushstring(L, m_project ? m_project->GetTitle() : wxString{});
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetWindowSize(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __func__))
            { return 0; }

        if (m_project->GetFirstView() && m_project->GetFirstView()->GetFrame())
            {
            m_project->GetFirstView()->GetFrame()->SetSize(lua_tonumber(L, 2), lua_tonumber(L, 3));
            m_project->GetFirstView()->GetFrame()->Center();
            wxGetApp().Yield();
            }
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SetIncludeIncompleteTolerance(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
            { return 0; }

        m_project->SetIncludeIncompleteSentencesIfLongerThanValue(lua_tonumber(L, 2));
        ReloadIfNotDelayed();
        return 0;
        };

    //-------------------------------------------------------------
    int BatchProject::SetTextExclusion(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
            { return 0; }

        m_project->SetInvalidSentenceMethod(
            static_cast<InvalidSentence>(static_cast<int>(lua_tonumber(L, 2))));
        ReloadIfNotDelayed();
        return 0;
        };

    //-------------------------------------------------------------
    int BatchProject::AggressivelyExclude(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
            { return 0; }

        m_project->AggressiveExclusion(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        };

    //-------------------------------------------------------------
    int BatchProject::ExcludeCopyrightNotices(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
            { return 0; }

        m_project->IgnoreTrailingCopyrightNoticeParagraphs(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        };

    //-------------------------------------------------------------
    int BatchProject::ExcludeTrailingCitations(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
            { return 0; }

        m_project->IgnoreTrailingCitations(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayed();
        lua_pushboolean(L, true);
        return 0;
        };

    //-------------------------------------------------------------
    int BatchProject::ExcludeFileAddress(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
            { return 0; }

        m_project->IgnoreFileAddresses(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        };

    //-------------------------------------------------------------
    int BatchProject::ExcludeNumerals(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
            { return 0; }

        m_project->IgnoreNumerals(int_to_bool(lua_toboolean(L, 2)));
        return 0;
        };

    //-------------------------------------------------------------
    int BatchProject::ExcludeProperNouns(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
            { return 0; }

        m_project->IgnoreProperNouns(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        };

    // Sets the filepath to the phrase exclusion list
    // filePath Path to phrase exclusion list
    int BatchProject::SetPhraseExclusionList(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
            { return 0; }

        m_project->SetExcludedPhrasesPath(wxString(luaL_checkstring(L, 2), wxConvUTF8));
        m_project->LoadExcludePhrases();
        ReloadIfNotDelayed();
        return 0;
        };

    // Sets the tags to exclude blocks of text.
    int BatchProject::SetBlockExclusionTags(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
            { return 0; }

        const wxString exclusionTags(luaL_checkstring(L, 2), wxConvUTF8);
        m_project->GetExclusionBlockTags().clear();
        if (exclusionTags.length() >= 2)
            { m_project->GetExclusionBlockTags().push_back(
                std::make_pair(exclusionTags[0],exclusionTags[1])); }
        ReloadIfNotDelayed();
        return 0;
        };

    //-------------------------------------------------
    int BatchProject::SetAppendedDocumentFilePath(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
            { return 0; }

        m_project->SetAppendedDocumentFilePath(wxString(luaL_checkstring(L, 2), wxConvUTF8));
        ReloadIfNotDelayed();
        return 0;
        };

    //-------------------------------------------------------------
    int BatchProject::SetGraphBackgroundColor(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
            { return 0; }

        m_project->SetBackGroundColor(LoadColor(wxString{ luaL_checkstring(L, 2), wxConvUTF8 }));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::ApplyGraphBackgroundFade(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
            { return 0; }

        m_project->SetGraphBackGroundLinearGradient(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SetGraphCommonImage(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
            { return 0; }

        m_project->SetGraphCommonImagePath(wxString(luaL_checkstring(L, 2), wxConvUTF8));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SetPlotBackgroundImage(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
            { return 0; }

        m_project->SetPlotBackGroundImagePath(wxString(luaL_checkstring(L, 2), wxConvUTF8));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SetPlotBackgroundImageEffect(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
            { return 0; }

        m_project->SetPlotBackGroundImageEffect(
            static_cast<Wisteria::ImageEffect>(static_cast<int>(lua_tonumber(L, 2))) );
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SetPlotBackgroundImageFit(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
            { return 0; }

        m_project->SetPlotBackGroundImageFit(
            static_cast<Wisteria::ImageFit>(static_cast<int>(lua_tonumber(L, 2))) );
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SetPlotBackgroundImageOpacity(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
            { return 0; }

        m_project->SetPlotBackGroundImageOpacity(lua_tonumber(L, 2));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SetPlotBackgroundColor(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
            { return 0; }

        m_project->SetPlotBackGroundColor(LoadColor(wxString{ luaL_checkstring(L, 2), wxConvUTF8 }));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SetGraphInvalidRegionColor(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
            { return 0; }

        m_project->SetInvalidAreaColor(LoadColor(wxString{ luaL_checkstring(L, 2), wxConvUTF8 }));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SetStippleShapeColor(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
            { return 0; }

        m_project->SetStippleShapeColor(LoadColor(wxString{ luaL_checkstring(L, 2), wxConvUTF8 }));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SetPlotBackgroundColorOpacity(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
            { return 0; }

        m_project->SetPlotBackGroundColorOpacity(lua_tonumber(L, 2));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SetWatermark(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
            { return 0; }

        m_project->SetWatermark(wxString(luaL_checkstring(L, 2), wxConvUTF8));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SetGraphLogoImage(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
            { return 0; }

        m_project->SetWatermarkLogoPath(wxString(luaL_checkstring(L, 2), wxConvUTF8));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SetStippleImage(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
            { return 0; }

        m_project->SetStippleImagePath(wxString(luaL_checkstring(L, 2), wxConvUTF8));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SetStippleShape(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
            { return 0; }

        m_project->SetStippleShape(wxString(luaL_checkstring(L, 2), wxConvUTF8));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SetXAxisFont(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
            { return 0; }

        auto fontInfo = m_project->GetXAxisFont();
        auto fontColor= m_project->GetXAxisFontColor();
        LoadFontAttributes(L, fontInfo, fontColor, true);

        m_project->SetXAxisFont(fontInfo);
        m_project->SetXAxisFontColor(fontColor);
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SetYAxisFont(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
            { return 0; }

        auto fontInfo = m_project->GetYAxisFont();
        auto fontColor = m_project->GetYAxisFontColor();
        LoadFontAttributes(L, fontInfo, fontColor, true);

        m_project->SetYAxisFont(fontInfo);
        m_project->SetYAxisFontColor(fontColor);
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SetGraphTopTitleFont(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
            { return 0; }

        auto fontInfo = m_project->GetGraphTopTitleFont();
        auto fontColor = m_project->GetGraphTopTitleFontColor();
        LoadFontAttributes(L, fontInfo, fontColor, true);

        m_project->SetGraphTopTitleFont(fontInfo);
        m_project->SetGraphTopTitleFontColor(fontColor);
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::DisplayBarChartLabels(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
            { return 0; }

        m_project->DisplayBarChartLabels(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::DisplayGraphDropShadows(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
            { return 0; }

        m_project->DisplayDropShadows(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SetProjectLanguage(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
            { return 0; }

        m_project->SetProjectLanguage(
            static_cast<readability::test_language>(static_cast<int>(lua_tonumber(L, 2))));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetProjectLanguage(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }

        lua_pushnumber(L,static_cast<int>(m_project->GetProjectLanguage()));
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetReviewer(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
            { return 0; }

        m_project->SetReviewer(wxString(luaL_checkstring(L, 2), wxConvUTF8));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetReviewer(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }

        lua_pushstring(L, m_project->GetReviewer().utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetStatus(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
            { return 0; }

        m_project->SetStatus(wxString(luaL_checkstring(L, 2), wxConvUTF8));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SetDocumentStorageMethod(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
            { return 0; }

        m_project->SetDocumentStorageMethod(
            static_cast<TextStorage>(static_cast<int>(lua_tonumber(L, 2))));
        ReloadIfNotDelayed();
        return 0;
        };

    //-------------------------------------------------------------
    int BatchProject::GetDocumentStorageMethod(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }

        lua_pushnumber(L,static_cast<int>(m_project->GetDocumentStorageMethod()));
        return 1;
        };

    //-------------------------------------------------------------
    int BatchProject::SetParagraphsParsingMethod(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
            { return 0; }

        m_project->SetParagraphsParsingMethod(
            static_cast<ParagraphParse>(static_cast<int>(lua_tonumber(L, 2))));
        ReloadIfNotDelayed();
        return 0;
        };

    //-------------------------------------------------------------
    int BatchProject::GetParagraphsParsingMethod(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }

        lua_pushnumber(L,static_cast<int>(m_project->GetParagraphsParsingMethod()));
        return 1;
        };

    //-------------------------------------------------------------
    int BatchProject::SetMinDocWordCountForBatch(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
            { return 0; }

        m_project->SetMinDocWordCountForBatch(static_cast<int>(lua_tonumber(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        };

    //-------------------------------------------------------------
    int BatchProject::GetMinDocWordCountForBatch(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }

        lua_pushnumber(L,static_cast<int>(m_project->GetMinDocWordCountForBatch()));
        return 0;
        };

    //-------------------------------------------------------------
    int BatchProject::SetFilePathDisplayMode(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
            { return 0; }

        m_project->SetFilePathTruncationMode(
            static_cast<ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode>(
                static_cast<int>(lua_tonumber(L, 2))));
        ReloadIfNotDelayed();
        return 0;
        };

    //-------------------------------------------------------------
    int BatchProject::GetFilePathDisplayMode(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }

        lua_pushnumber(L,static_cast<int>(m_project->GetFilePathTruncationMode()));
        return 1;
        };

    //-------------------------------------------------------------
    int BatchProject::SetSpellCheckerOptions(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
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

    // Adds a test to the project.
    // TestName The name of the test to add to the project.
    int BatchProject::AddTest(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            lua_pushboolean(L, false);
            return 1;
            }
        if (!VerifyParameterCount(L, 1, __func__))
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
                auto result =
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
                    view->GetDocFrame()->OnCustomTest(cmd);
                    }
                else
                    {
                    wxMessageBox(wxString::Format(_(L"%s: Unknown test could not be added."), testName),
                        _(L"Script Error"), wxOK|wxICON_EXCLAMATION);
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

    // Reanalyzes the documents.
    int BatchProject::Reload(lua_State*)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }

        m_project->RefreshRequired(ProjectRefresh::FullReindexing);
        m_project->RefreshProject();
        wxGetApp().Yield();
        return 0;
        }

    /// Closes the project.
    /// @SaveChanges Specifies whether to save any changes made to the project before closing it.
    ///     Default is to not save any changes.
    int BatchProject::Close(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
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
        // the view won't be fully deleted until idle processing takes place
        wxGetApp().ProcessIdle();
        m_project = nullptr;
        return 0;
        }

    // Saves a list from the project.
    // ListType Which list to save.
    // FilePath The file path to save the list.
    int BatchProject::ExportList(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 2, __func__))
            { return 0; }

        BatchProjectView* view = dynamic_cast<BatchProjectView*>(m_project->GetFirstView());
        if (view)
            {
            ListCtrlEx* listWindow = nullptr;
            const auto listId = wxGetApp().GetDynamicIdMap().find(lua_tonumber(L, 2));
            if (listId == wxGetApp().GetDynamicIdMap().cend())
                {
                wxMessageBox(wxString::Format(
                    _(L"Unable to find the specified list (%d) in the project."),
                        static_cast<int>(lua_tonumber(L, 2))),
                    _(L"Script Error"), wxOK|wxICON_EXCLAMATION);
                return 0;
                }
            const auto [parentPos, childPos] = view->GetSideBar()->FindSubItem(listId->second);
            if (parentPos.has_value() && childPos.has_value())
                {
                view->GetSideBar()->SelectSubItem(parentPos.value(), childPos.value());
                wxWindow* selWindow = view->GetActiveProjectWindow();
                if (selWindow && selWindow->IsKindOf(wxCLASSINFO(ListCtrlEx)) )
                    { listWindow = dynamic_cast<ListCtrlEx*>(selWindow); }
                }
            else
                {
                const auto index = view->GetSideBar()->FindFolder(listId->second);
                if (index.has_value())
                    {
                    view->GetSideBar()->SelectFolder(index.value());
                    wxWindow* selWindow = view->GetActiveProjectWindow();
                    if (selWindow && selWindow->IsKindOf(wxCLASSINFO(ListCtrlEx)) )
                        { listWindow = dynamic_cast<ListCtrlEx*>(selWindow); }
                    }
                }
            if (listWindow)
                {
                const BatchProjectDoc* doc = dynamic_cast<BatchProjectDoc*>(view->GetDocument());
                const wxString originalLabel = listWindow->GetLabel();
                listWindow->SetLabel(originalLabel + wxString::Format(L" [%s]",
                                     wxFileName::StripExtension(doc->GetTitle())));
                GridExportOptions exportOptions;
                exportOptions.m_fromRow = (lua_gettop(L) > 3) ? lua_tonumber(L, 4) : 1;
                exportOptions.m_toRow = (lua_gettop(L) > 4) ? lua_tonumber(L, 5) : -1;
                exportOptions.m_fromColumn = (lua_gettop(L) > 5) ? lua_tonumber(L, 6) : 1;
                exportOptions.m_toColumn = (lua_gettop(L) > 6) ? lua_tonumber(L, 7) : -1;
                exportOptions.m_includeColumnHeaders =
                    (lua_gettop(L) > 7) ? int_to_bool(lua_toboolean(L, 8)) : true;
                exportOptions.m_pageUsingPrinterSettings =
                    (lua_gettop(L) > 8) ? int_to_bool(lua_toboolean(L, 9)) : false;
                lua_pushboolean(L, listWindow->Save(wxString(luaL_checklstring(L, 3, nullptr), wxConvUTF8),
                    exportOptions));
                listWindow->SetLabel(originalLabel);
                listWindow->SetFocus();
                }
            }
        wxGetApp().Yield();
        return 0;
        }

    // Saves a graph from the project as an image.
    // SideBarSection The section that the graph is in.
    // GraphId Which graph to save.
    // FilePath The file path to save the graph.
    // GrayScale Whether to save the image in black & white.
    // Width The width of the output image.
    // Height The height of the output image.
    int BatchProject::ExportGraph(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 3, __func__))
            { return 0; }

        BatchProjectView* view = dynamic_cast<BatchProjectView*>(m_project->GetFirstView());
        if (view)
            {
            Wisteria::Canvas* graphWindow = nullptr;
            const auto sectionId = wxGetApp().GetDynamicIdMap().find(lua_tonumber(L, 2));
            if (sectionId == wxGetApp().GetDynamicIdMap().cend())
                {
                wxMessageBox(wxString::Format(
                    _(L"Unable to find the specified section (%d) in the project."),
                        static_cast<int>(lua_tonumber(L, 2))),
                    _(L"Script Error"), wxOK|wxICON_EXCLAMATION);
                return 0;
                }
            // Use mapped value or numeric value from a script, which may be a literal (unmapped) ID.
            // This can happen for test-specific graphs that are using the XRC-generated ID from the test.
            wxWindowID userWindowId = lua_tonumber(L, 3);
            if (const auto windowMappedId = wxGetApp().GetDynamicIdMap().find(userWindowId);
                windowMappedId != wxGetApp().GetDynamicIdMap().cend())
                { userWindowId = windowMappedId->second; }

            const auto [parentPos, childPos] = view->GetSideBar()->FindSubItem(sectionId->second, userWindowId);
            if (parentPos.has_value() && childPos.has_value())
                {
                view->GetSideBar()->SelectSubItem(parentPos.value(), childPos.value());
                wxWindow* selWindow = view->GetActiveProjectWindow();
                if (selWindow && selWindow->IsKindOf(wxCLASSINFO(Wisteria::Canvas)))
                    { graphWindow = dynamic_cast<Wisteria::Canvas*>(selWindow); }
                }
            if (graphWindow)
                {
                const BatchProjectDoc* doc = dynamic_cast<BatchProjectDoc*>(view->GetDocument());
                const wxString originalLabel = graphWindow->GetLabel();
                graphWindow->SetLabel(originalLabel + wxString::Format(L" [%s]",
                                      wxFileName::StripExtension(doc->GetTitle())));
                Wisteria::UI::ImageExportOptions opt;
                if (lua_gettop(L) >= 5 && lua_toboolean(L, 5))
                    {
                    opt.m_mode = static_cast<decltype(opt.m_mode)>(
                        Wisteria::UI::ImageExportOptions::ColorMode::Greyscale);
                    }
                if (lua_gettop(L) >= 6)
                    { opt.m_imageSize.SetWidth(lua_tonumber(L, 6)); }
                if (lua_gettop(L) >= 7)
                    { opt.m_imageSize.SetHeight(lua_tonumber(L, 7)); }
                lua_pushboolean(L, graphWindow->Save(wxString(luaL_checklstring(L, 4, nullptr), wxConvUTF8), opt) );
                graphWindow->SetLabel(originalLabel);
                }
            else
                {
                wxMessageBox(wxString::Format(
                    _(L"Unable to find the specified graph (%d) in the project."),
                        static_cast<int>(lua_tonumber(L, 3))),
                    _(L"Script Error"), wxOK|wxICON_EXCLAMATION);
                return 0;
                }
            }
        wxGetApp().Yield();
        return 0;
        }

    // Exports all of the results from the project into a folder.
    // FolderPath The folder to save the project's results.
    int BatchProject::ExportAll(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }

        wxString outputPath(luaL_checkstring(L, 2), wxConvUTF8);
        BatchProjectView* view = dynamic_cast<BatchProjectView*>(m_project->GetFirstView());
        if (view)
            {
            view->ExportAll(outputPath,BaseProjectDoc::GetExportListExt(),
                            BaseProjectDoc::GetExportGraphExt(),
                            true, true, true, true, true, true, true, true, Wisteria::UI::ImageExportOptions());
            }
        return 0;
        }

    // Shows or hides the sidebar
    int BatchProject::ShowSidebar(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
            { return 0; }

        auto view = dynamic_cast<BatchProjectView*>(m_project->GetFirstView());
        if (view)
            { view->ShowSideBar(int_to_bool(lua_toboolean(L, 2))); }

        return 0;
        }

    /* Selects the specified section and subwindow.
       Section The section to select.
       Window The subwindow in the section to select.*/
    int BatchProject::SelectWindow(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
            { return 0; }

        BatchProjectView* view = dynamic_cast<BatchProjectView*>(m_project->GetFirstView());
        if (view)
            {
            const auto sectionId = wxGetApp().GetDynamicIdMap().find(lua_tonumber(L, 2));
            if (sectionId == wxGetApp().GetDynamicIdMap().cend())
                {
                wxMessageBox(wxString::Format(
                    _(L"Unable to find the specified section (%d) in the project."),
                        static_cast<int>(lua_tonumber(L, 2))),
                    _(L"Script Error"), wxOK|wxICON_EXCLAMATION);
                return 0;
                }
            std::optional<wxWindowID> windowId{ std::nullopt };
            // selecting section and subwindow
            if (lua_gettop(L) > 2)
                {
                const auto userWindowId = wxGetApp().GetDynamicIdMap().find(lua_tonumber(L, 3));
                if (userWindowId == wxGetApp().GetDynamicIdMap().cend())
                    {
                    wxMessageBox(wxString::Format(
                        _(L"Unable to find the specified window (%d) in the project."),
                            static_cast<int>(lua_tonumber(L, 3))),
                        _(L"Script Error"), wxOK|wxICON_EXCLAMATION);
                    return 0;
                    }
                windowId = userWindowId->second;
                }
            const auto [parentPos, childPos] =
                windowId ?
                view->GetSideBar()->FindSubItem(sectionId->second, windowId.value()) :
                view->GetSideBar()->FindSubItem(sectionId->second);
            if (parentPos.has_value() && childPos.has_value())
                {
                view->GetSideBar()->CollapseAll();
                view->GetSideBar()->SelectSubItem(parentPos.value(), childPos.value());
                }
            const auto index = view->GetSideBar()->FindFolder(sectionId->second);
            if (index.has_value())
                {
                view->GetSideBar()->CollapseAll();
                view->GetSideBar()->SelectFolder(index.value());
                }
            }
        wxGetApp().Yield();
        return 0;
        }

    /*Selects and sorts a list.
    ListToSort The list window to sort. Refer to ListTypes enumeration.
    ColumnToSort The column in the list to sort.
    Order The order to sort.*/
    int BatchProject::SortList(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
            { return 0; }

        BatchProjectView* view = dynamic_cast<BatchProjectView*>(m_project->GetFirstView());
        if (view)
            {
            std::vector<std::pair<size_t,Wisteria::SortDirection>> columns;
            for (int i = 3; i <= lua_gettop(L); i+=2)
                {
                columns.push_back(std::pair<size_t,Wisteria::SortDirection>(lua_tonumber(L, i)-1,
                                  static_cast<Wisteria::SortDirection>(static_cast<int>(lua_tonumber(L, i+1)))));
                }
            const auto windowOrSectionId = wxGetApp().GetDynamicIdMap().find(lua_tonumber(L, 2));
            if (windowOrSectionId == wxGetApp().GetDynamicIdMap().cend())
                {
                wxMessageBox(wxString::Format(
                    _(L"Unable to find the specified list (%d) in the project."),
                        static_cast<int>(lua_tonumber(L, 2))),
                    _(L"Script Error"), wxOK|wxICON_EXCLAMATION);
                return 0;
                }
            const auto [parentPos, childPos] = view->GetSideBar()->FindSubItem(windowOrSectionId->second);
            if (parentPos.has_value() && childPos.has_value())
                {
                view->GetSideBar()->SelectSubItem(parentPos.value(), childPos.value());
                wxWindow* selWindow = view->GetActiveProjectWindow();
                if (selWindow && selWindow->IsKindOf(wxCLASSINFO(ListCtrlEx)))
                    { dynamic_cast<ListCtrlEx*>(selWindow)->SortColumns(columns); }
                }
            else
                {
                const auto index = view->GetSideBar()->FindFolder(windowOrSectionId->second);
                if (index.has_value())
                    {
                    view->GetSideBar()->SelectFolder(index.value());
                    wxWindow* selWindow = view->GetActiveProjectWindow();
                    if (selWindow && selWindow->IsKindOf(wxCLASSINFO(ListCtrlEx)))
                        { dynamic_cast<ListCtrlEx*>(selWindow)->SortColumns(columns); }
                    }
                }
            }
        wxGetApp().Yield();
        return 0;
        }

    // hidden functions just used for screenshots
    //-------------------------------------------------------------
    int BatchProject::OpenProperties(lua_State *L)
        {
        if (!VerifyProjectIsOpen(__func__))
            { return 0; }
        if (!VerifyParameterCount(L, 1, __func__))
            { return 0; }

        if (m_settingsDlg == nullptr)
            { m_settingsDlg = new ToolsOptionsDlg(wxGetApp().GetMainFrame(), m_project); }

        const auto idPos = wxGetApp().GetDynamicIdMap().find(lua_tonumber(L, 2));
        if (idPos != wxGetApp().GetDynamicIdMap().cend())
            { m_settingsDlg->SelectPage(idPos->second); }
        else
            { m_settingsDlg->SelectPage(lua_tonumber(L, 2)); }
        m_settingsDlg->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
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
            { nullptr, nullptr, nullptr }
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
      LUNA_DECLARE_METHOD(BatchProject, SetStatus),
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
      LUNA_DECLARE_METHOD(BatchProject, AggressivelyExclude),
      LUNA_DECLARE_METHOD(BatchProject, SetTextExclusion),
      LUNA_DECLARE_METHOD(BatchProject, SetIncludeIncompleteTolerance),
      LUNA_DECLARE_METHOD(BatchProject, ExcludeCopyrightNotices),
      LUNA_DECLARE_METHOD(BatchProject, ExcludeTrailingCitations),
      LUNA_DECLARE_METHOD(BatchProject, ExcludeFileAddress),
      LUNA_DECLARE_METHOD(BatchProject, ExcludeNumerals),
      LUNA_DECLARE_METHOD(BatchProject, ExcludeProperNouns),
      LUNA_DECLARE_METHOD(BatchProject, SetGraphBackgroundColor),
      LUNA_DECLARE_METHOD(BatchProject, ApplyGraphBackgroundFade),
      LUNA_DECLARE_METHOD(BatchProject, SetGraphCommonImage),
      LUNA_DECLARE_METHOD(BatchProject, SetPlotBackgroundImage),
      LUNA_DECLARE_METHOD(BatchProject, SetPlotBackgroundImageEffect),
      LUNA_DECLARE_METHOD(BatchProject, SetPlotBackgroundImageFit),
      LUNA_DECLARE_METHOD(BatchProject, SetPlotBackgroundImageOpacity),
      LUNA_DECLARE_METHOD(BatchProject, SetWatermark),
      LUNA_DECLARE_METHOD(BatchProject, SetGraphLogoImage),
      LUNA_DECLARE_METHOD(BatchProject, SetPlotBackgroundColor),
      LUNA_DECLARE_METHOD(BatchProject, SetGraphInvalidRegionColor),
      LUNA_DECLARE_METHOD(BatchProject, SetStippleShapeColor),
      LUNA_DECLARE_METHOD(BatchProject, SetPlotBackgroundColorOpacity),
      LUNA_DECLARE_METHOD(BatchProject, SetStippleImage),
      LUNA_DECLARE_METHOD(BatchProject, SetStippleShape),
      LUNA_DECLARE_METHOD(BatchProject, SetXAxisFont),
      LUNA_DECLARE_METHOD(BatchProject, SetYAxisFont),
      LUNA_DECLARE_METHOD(BatchProject, SetGraphTopTitleFont),
      LUNA_DECLARE_METHOD(BatchProject, DisplayBarChartLabels),
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
