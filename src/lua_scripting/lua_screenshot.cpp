///////////////////////////////////////////////////////////////////////////////
// Name:        lua_screenshot.cpp
// Author:      Blake Madden
// Copyright:   (c) 2005-2023 Blake Madden
// Licence:     3-Clause BSD licence
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "lua_screenshot.h"
#include <wx/msgdlg.h>
#include "../app/readability_app.h"
#include "../Wisteria-Dataviz/src/ui/dialogs/listctrlsortdlg.h"
#include "../Wisteria-Dataviz/src/ui/dialogs/listctrlitemviewdlg.h"
#include "../Wisteria-Dataviz/src/ui/dialogs/radioboxdlg.h"
#include "../Wisteria-Dataviz/src/ui/dialogs/archivedlg.h"
#include "../Wisteria-Dataviz/src/ui/dialogs/getdirdlg.h"
#include "../Wisteria-Dataviz/src/ui/dialogs/gridexportdlg.h"
#include "../Wisteria-Dataviz/src/util/screenshot.h"
#include "../ui/dialogs/filtered_text_preview_dlg.h"
#include "../ui/dialogs/webharvesterdlg.h"
#include "../ui/dialogs/editwordlistdlg.h"
#include "../ui/dialogs/doc_group_select_dlg.h"
#include "../ui/dialogs/custom_test_dlg.h"
#include "../ui/dialogs/test_bundle_dlg.h"
#include "../ui/dialogs/tools_options_dlg.h"
#include "../ui/dialogs/project_wizard_dlg.h"

wxDECLARE_APP(ReadabilityApp);

using namespace Wisteria;
using namespace Wisteria::UI;

namespace LuaScripting
    {
    ToolsOptionsDlg* LuaOptionsDlg = nullptr;
    TestBundleDlg* LuaTestBundleDlg = nullptr;
    CustomTestDlg* LuaCustomTestDlg = nullptr;
    ProjectWizardDlg* LuaStandardProjectWizard = nullptr;
    ProjectWizardDlg* LuaBatchProjectWizard = nullptr;
    PrinterHeaderFooterDlg* LuaPrinterOptions = nullptr;
    GridExportDlg* LuaListExportDlg = nullptr;
    ListCtrlItemViewDlg* LuaListViewItemDlg = nullptr;
    GetDirFilterDialog* LuaGetDirDlg = nullptr;
    ArchiveDlg* LuaGetArchiveDlg = nullptr;
    RadioBoxDlg* LuaSelectProjectType = nullptr;
    WebHarvesterDlg* LuaWebHarvesterDlg = nullptr;
    FilteredTextPreviewDlg* LuaFilteredTextPreviewDlg = nullptr;
    ListCtrlSortDlg* LuaListCtrlSortDlg = nullptr;
    EditWordListDlg* LuaEditWordListDlg = nullptr;
    DocGroupSelectDlg* LuaDocGroupSelectDlg = nullptr;

    //-------------------------------------------------------------
    int SnapScreenshot(lua_State *L)
        {
        const wxString path(luaL_checkstring(L, 1), wxConvUTF8);
        int startWindowToHighlight = -1, endWindowToHighlight = -1;
        if (lua_gettop(L) > 1)
            { startWindowToHighlight = lua_tonumber(L, 2); }
        if (lua_gettop(L) > 2)
            { endWindowToHighlight = lua_tonumber(L, 3); }
        lua_pushboolean(L, Screenshot::SaveScreenshot(path,
            startWindowToHighlight, endWindowToHighlight));
        return 1;
        }

    //-------------------------------------------------------------
    int SnapScreenshotOfTextWindow(lua_State *L)
        {
        const wxString path(luaL_checkstring(L, 1), wxConvUTF8);
        std::vector<std::pair<long,long>> highlightPoints;
        if (lua_gettop(L) > 3)
            {
            for (long i = 4; i <= lua_gettop(L); i+=2)
                {
                highlightPoints.push_back(
                    std::make_pair(lua_tonumber(L, i), lua_tonumber(L, i+1)));
                }
            }
        lua_pushboolean(L, Screenshot::SaveScreenshotOfTextWindow(path,
            lua_tonumber(L, 2), lua_toboolean(L, 3), highlightPoints));
        return 1;
        }

    //-------------------------------------------------------------
    int SnapScreenshotOfRibbon(lua_State *L)
        {
        const wxString path(luaL_checkstring(L, 1), wxConvUTF8);
        int pageToSelect{0}, buttonBarID{-1};
        if (lua_gettop(L) >= 2)
            { pageToSelect = lua_tonumber(L, 2); }
        if (lua_gettop(L) >= 3)
            { buttonBarID = lua_tonumber(L, 3); }
        lua_pushboolean(L, Screenshot::SaveScreenshotOfRibbon(path, pageToSelect, buttonBarID));
        return 1;
        }

    //-------------------------------------------------------------
    int SnapScreenshotOfListControl(lua_State *L)
        {
        const wxString path(luaL_checkstring(L, 1), wxConvUTF8);
        int startRow{-1}, endRow{-1}, startColumn{-1}, endColumn{-1}, cuttOffRow{-1};
        if (lua_gettop(L) >= 3)
            { startRow = lua_tonumber(L, 3); }
        if (lua_gettop(L) >= 4)
            { startColumn = lua_tonumber(L, 4); }
        if (lua_gettop(L) >= 5)
            { endRow = lua_tonumber(L, 5); }
        if (lua_gettop(L) >= 6)
            { endColumn = lua_tonumber(L, 6); }
        if (lua_gettop(L) >= 7)
            { cuttOffRow = lua_tonumber(L, 7); }
        lua_pushboolean(L, Screenshot::SaveScreenshotOfListControl(path,
            lua_tonumber(L, 2),
            // make zero-indexed
            startRow == -1 ? -1 : startRow-1,
            endRow == -1 ? -1 : endRow-1,
            startColumn == -1 ? -1 : startColumn-1,
            endColumn == -1 ? -1 : endColumn-1,
            cuttOffRow == -1 ? -1 : cuttOffRow-1));
        return 1;
        }

    //-------------------------------------------------------------
    int SnapScreenshotOfPropertGrid(lua_State *L)
        {
        const wxString path(luaL_checkstring(L, 1), wxConvUTF8);
        int startWindowToHighlight = -1;
        wxString propertyStart, propertyEnd;
        if (lua_gettop(L) > 1)
            { startWindowToHighlight = lua_tonumber(L, 2); }
        if (lua_gettop(L) > 2)
            { propertyStart = wxString(luaL_checkstring(L, 3), wxConvUTF8); }
        if (lua_gettop(L) > 3)
            { propertyEnd = wxString(luaL_checkstring(L, 4), wxConvUTF8); }
        lua_pushboolean(L, Screenshot::SaveScreenshotOfPropertyGrid(path,
            startWindowToHighlight, propertyStart, propertyEnd));
        return 1;
        }

    //-------------------------------------------------------------
    int HighlightScreenshot(lua_State *L)
        {
        const wxString path(luaL_checkstring(L, 1), wxConvUTF8);
        lua_pushboolean(L,
            Screenshot::HighlightItemInScreenshot(path,
                wxPoint(lua_tonumber(L, 2), lua_tonumber(L, 3)),
            wxPoint(lua_tonumber(L, 4), lua_tonumber(L, 5))));
        return 1;
        }

    //-------------------------------------------------------------
    int ShowDocGroupSelectDlg(lua_State* L)
        {
        if (LuaDocGroupSelectDlg == nullptr)
            { LuaDocGroupSelectDlg = new DocGroupSelectDlg(wxGetApp().GetMainFrame()); }
        if (lua_gettop(L) > 0)
            { LuaDocGroupSelectDlg->SetSelection(lua_tonumber(L, 1)-1/*make zero indexed*/); }
        if (lua_gettop(L) > 1)
            { LuaDocGroupSelectDlg->SetGroupingLabel(
                wxString(luaL_checkstring(L, 2), wxConvUTF8)); }
        LuaDocGroupSelectDlg->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int CloseDocGroupSelectDlg(lua_State*)
        {
        if (LuaDocGroupSelectDlg)
            {
            LuaDocGroupSelectDlg->Destroy();
            LuaDocGroupSelectDlg = nullptr;
            }
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowEditWordListDlg(lua_State* L)
        {
        if (LuaEditWordListDlg == nullptr)
            {
            LuaEditWordListDlg = new EditWordListDlg(wxGetApp().GetMainFrame(),
                wxID_ANY, _("Edit Phrase List"));
            }
        LuaEditWordListDlg->SetPhraseFileMode(true);
        LuaEditWordListDlg->SetFilePath(wxString(luaL_checkstring(L, 1), wxConvUTF8));
        LuaEditWordListDlg->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int CloseEditWordListDlg(lua_State*)
        {
        if (LuaEditWordListDlg)
            {
            LuaEditWordListDlg->Destroy();
            LuaEditWordListDlg = nullptr;
            }
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowSortListDlg(lua_State* L)
        {
        wxArrayString columns;
        for (int i = 1; i < lua_gettop(L)+1; ++i)
            { columns.Add(wxGetTranslation(wxString(luaL_checkstring(L, i), wxConvUTF8))); }
        if (LuaListCtrlSortDlg == nullptr)
            {
            LuaListCtrlSortDlg = new ListCtrlSortDlg(wxGetApp().GetMainFrame(), columns);
            }
        std::vector<std::pair<size_t,Wisteria::SortDirection>> sortInfo;
        for (size_t i = 0; i < columns.size(); ++i)
            { sortInfo.push_back(std::pair<size_t,Wisteria::SortDirection>(i, Wisteria::SortDirection::SortAscending)); }
        LuaListCtrlSortDlg->FillSortCriteria(sortInfo);
        LuaListCtrlSortDlg->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int CloseSortListDlg(lua_State*)
        {
        if (LuaListCtrlSortDlg)
            {
            LuaListCtrlSortDlg->Destroy();
            LuaListCtrlSortDlg = nullptr;
            }
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowFilteredTextPreviewDlg(lua_State*)
        {
        if (LuaFilteredTextPreviewDlg == nullptr)
            {
            LuaFilteredTextPreviewDlg = new FilteredTextPreviewDlg(wxGetApp().GetMainFrame(), InvalidSentence::ExcludeFromAnalysis, true, true, true, true, true, true, true, true);
            }
        wxDocument* currentDoc = wxGetApp().GetMainFrame()->GetDocumentManager()->GetCurrentDocument();
        if (currentDoc && currentDoc->IsKindOf(CLASSINFO(ProjectDoc)) )
            {
            wxString filteredText;
            dynamic_cast<ProjectDoc*>(currentDoc)->FormatFilteredText(filteredText, true, true, true, true, true, true);
            LuaFilteredTextPreviewDlg->SetFilteredValue(filteredText);
            }
        LuaFilteredTextPreviewDlg->ShowDetails();
        LuaFilteredTextPreviewDlg->FindWindow(wxID_OK)->SetFocus();
        LuaFilteredTextPreviewDlg->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int CloseFilteredTextPreviewDlg(lua_State*)
        {
        if (LuaFilteredTextPreviewDlg)
            {
            LuaFilteredTextPreviewDlg->Destroy();
            LuaFilteredTextPreviewDlg = nullptr;
            }
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowStandardProjectWizardLanguagePage(lua_State*)
        {
        if (LuaStandardProjectWizard != nullptr)
            { CloseStandardProjectWizard(nullptr); }
        LuaStandardProjectWizard = new ProjectWizardDlg(wxGetApp().GetMainFrame(), ProjectType::StandardProject);
        LuaStandardProjectWizard->SelectPage(0);
        LuaStandardProjectWizard->FindWindow(wxID_FORWARD)->SetFocus();
        LuaStandardProjectWizard->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowStandardProjectWizardTextSourcePage(lua_State *L)
        {
        if (LuaStandardProjectWizard != nullptr)
            { CloseStandardProjectWizard(nullptr); }
        LuaStandardProjectWizard = new ProjectWizardDlg(wxGetApp().GetMainFrame(), ProjectType::StandardProject);
        if (lua_gettop(L) > 0 &&
            lua_tonumber(L, 1) == 1/*make zero-indexed*/)
            { LuaStandardProjectWizard->SetTextFromFileSelected(); }
        if (lua_gettop(L) > 0 &&
            lua_tonumber(L, 1) == 2/*make zero-indexed*/)
            { LuaStandardProjectWizard->SetManualTextEntrySelected(); }
        LuaStandardProjectWizard->SelectPage(0);
        LuaStandardProjectWizard->FindWindow(wxID_FORWARD)->SetFocus();
        LuaStandardProjectWizard->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowStandardProjectWizardTextFromFilePage(lua_State *L)
        {
        if (LuaStandardProjectWizard != nullptr)
            { CloseStandardProjectWizard(nullptr); }
        LuaStandardProjectWizard = new ProjectWizardDlg(wxGetApp().GetMainFrame(), ProjectType::StandardProject,
            wxString(luaL_checkstring(L, 1), wxConvUTF8));
        LuaStandardProjectWizard->SetTextFromFileSelected();
        LuaStandardProjectWizard->SetFilePath(wxString(luaL_checkstring(L, 1), wxConvUTF8));
        LuaStandardProjectWizard->SelectPage(0);
        LuaStandardProjectWizard->FindWindow(wxID_FORWARD)->SetFocus();
        LuaStandardProjectWizard->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int SetStandardProjectWizardTextFromFilePath(lua_State *L)
        {
        if (LuaStandardProjectWizard != nullptr)
            { CloseStandardProjectWizard(nullptr); }
        LuaStandardProjectWizard = new ProjectWizardDlg(wxGetApp().GetMainFrame(), ProjectType::StandardProject,
            wxString(luaL_checkstring(L, 1), wxConvUTF8));
        LuaStandardProjectWizard->SetTextFromFileSelected();
        LuaStandardProjectWizard->SetFilePath(wxString(luaL_checkstring(L, 1), wxConvUTF8));
        LuaStandardProjectWizard->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowStandardProjectWizardTextEntryPage(lua_State *L)
        {
        if (LuaStandardProjectWizard != nullptr)
            { CloseStandardProjectWizard(nullptr); }
        LuaStandardProjectWizard = new ProjectWizardDlg(wxGetApp().GetMainFrame(), ProjectType::StandardProject);
        LuaStandardProjectWizard->SetManualTextEntrySelected();
        if (lua_gettop(L) > 0)
            { LuaStandardProjectWizard->SetEnteredText(wxString(luaL_checkstring(L, 1), wxConvUTF8)); }
        LuaStandardProjectWizard->SelectPage(0);
        LuaStandardProjectWizard->FindWindow(wxID_FORWARD)->SetFocus();
        LuaStandardProjectWizard->Show();
        wxGetApp().Yield();
        ::wxSleep(2);
        return 0;
        }

    //-------------------------------------------------------------
    int ShowStandardProjectWizardTestRecommendationPage(lua_State *L)
        {
        if (LuaStandardProjectWizard != nullptr)
            { CloseStandardProjectWizard(nullptr); }
        LuaStandardProjectWizard = new ProjectWizardDlg(wxGetApp().GetMainFrame(), ProjectType::StandardProject);
        if (lua_gettop(L) > 0)
            { LuaStandardProjectWizard->SetTestSelectionMethod(lua_tonumber(L, 1)-1/*make zero-indexed*/); }
        LuaStandardProjectWizard->SelectPage(2);
        LuaStandardProjectWizard->FindWindow(wxID_FORWARD)->SetFocus();
        LuaStandardProjectWizard->Show();
        wxGetApp().Yield();
        ::wxSleep(2);//animation for radio button change takes a second (and it's in a separate thread), so wait for the UI to finish updating
        return 0;
        }

    //-------------------------------------------------------------
    int ShowStandardProjectWizardTestByIndustryPage(lua_State *L)
        {
        if (LuaStandardProjectWizard != nullptr)
            { CloseStandardProjectWizard(nullptr); }
        LuaStandardProjectWizard = new ProjectWizardDlg(wxGetApp().GetMainFrame(), ProjectType::StandardProject);
        LuaStandardProjectWizard->SetTestSelectionMethod(1);
        LuaStandardProjectWizard->SelectPage(2);
        if (lua_gettop(L) > 0)
            {
            const int selected = lua_tonumber(L, 1)-1;//make zero-indexed
            LuaStandardProjectWizard->SelectIndustryType(static_cast<readability::industry_classification>(selected));
            }
        LuaStandardProjectWizard->FindWindow(wxID_FORWARD)->SetFocus();
        LuaStandardProjectWizard->Show();
        wxGetApp().Yield();
        ::wxSleep(2);
        return 0;
        }

    //-------------------------------------------------------------
    int ShowStandardProjectWizardTestByDocumentTypePage(lua_State *L)
        {
        if (LuaStandardProjectWizard != nullptr)
            { CloseStandardProjectWizard(nullptr); }
        LuaStandardProjectWizard = new ProjectWizardDlg(wxGetApp().GetMainFrame(), ProjectType::StandardProject);
        LuaStandardProjectWizard->SetTestSelectionMethod(0);
        LuaStandardProjectWizard->SelectPage(2);
        if (lua_gettop(L) > 0)
            {
            const int selection = lua_tonumber(L, 1)-1;//make zero-indexed
            LuaStandardProjectWizard->SelectDocumentType(static_cast<readability::document_classification>(selection));
            }
        LuaStandardProjectWizard->FindWindow(wxID_FORWARD)->SetFocus();
        LuaStandardProjectWizard->Show();
        wxGetApp().Yield();
        ::wxSleep(2);
        return 0;
        }

    //-------------------------------------------------------------
    int ShowStandardProjectWizardDocumentStructurePage(lua_State *L)
        {
        if (LuaStandardProjectWizard != nullptr)
            { CloseStandardProjectWizard(nullptr); }
        LuaStandardProjectWizard = new ProjectWizardDlg(wxGetApp().GetMainFrame(), ProjectType::StandardProject);
        if (lua_gettop(L) > 0)
            {
            const int selectedDocType = lua_tonumber(L, 1)-1/*make zero-indexed*/;
            (selectedDocType == 0) ?
                LuaStandardProjectWizard->SetNarrativeSelected() :
                LuaStandardProjectWizard->SetFragmentedTextSelected();
            if (lua_gettop(L) > 1)
                { LuaStandardProjectWizard->SetSplitLinesSelected(int_to_bool(lua_toboolean(L, 2))); }
            if (lua_gettop(L) > 2)
                { LuaStandardProjectWizard->SetCenteredTextSelected(int_to_bool(lua_toboolean(L, 3))); }
            if (lua_gettop(L) > 3)
                { LuaStandardProjectWizard->SetNewLinesAlwaysNewParagraphsSelected(int_to_bool(lua_toboolean(L, 4))); }
            }
        LuaStandardProjectWizard->SelectPage(1);
        LuaStandardProjectWizard->FindWindow(wxID_FORWARD)->SetFocus();
        LuaStandardProjectWizard->Show();
        wxGetApp().Yield();
        ::wxSleep(2);
        return 0;
        }

    //-------------------------------------------------------------
    int ShowStandardProjectWizardManualTestSelectionPage(lua_State* L)
        {
        if (LuaStandardProjectWizard != nullptr)
            { CloseStandardProjectWizard(nullptr); }
        LuaStandardProjectWizard = new ProjectWizardDlg(wxGetApp().GetMainFrame(), ProjectType::StandardProject);
        LuaStandardProjectWizard->SetTestSelectionMethod(2);
        LuaStandardProjectWizard->SelectPage(2);
        for (int i = 1; i <= lua_gettop(L); ++i)
            {
            LuaStandardProjectWizard->SelectStandardTestManually(
                wxString(luaL_checkstring(L, i), wxConvUTF8));
            }
        LuaStandardProjectWizard->FindWindow(wxID_FORWARD)->SetFocus();
        LuaStandardProjectWizard->Show();
        wxGetApp().Yield();
        ::wxSleep(2);
        return 0;
        }

    //-------------------------------------------------------------
    int ShowStandardProjectWizardTestByBundlePage(lua_State *L)
        {
        if (LuaStandardProjectWizard != nullptr)
            { CloseStandardProjectWizard(nullptr); }
        LuaStandardProjectWizard = new ProjectWizardDlg(wxGetApp().GetMainFrame(), ProjectType::StandardProject);
        LuaStandardProjectWizard->SelectPage(2);
        LuaStandardProjectWizard->SetTestSelectionMethod(3);
        if (lua_gettop(L) > 0)
            {
            LuaStandardProjectWizard->SetSelectedTestBundle(wxString(luaL_checkstring(L, 1), wxConvUTF8));
            }
        LuaStandardProjectWizard->FindWindow(wxID_FORWARD)->SetFocus();
        LuaStandardProjectWizard->Show();
        wxGetApp().Yield();
        ::wxSleep(2);
        return 0;
        }

    //-------------------------------------------------------------
    int CloseStandardProjectWizard(lua_State*)
        {
        if (LuaStandardProjectWizard)
            {
            LuaStandardProjectWizard->Destroy();
            LuaStandardProjectWizard = nullptr;
            }
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowBatchProjectWizardLanguagePage(lua_State *L)
        {
        if (LuaBatchProjectWizard != nullptr)
            { CloseBatchProjectWizard(nullptr); }
        if (lua_gettop(L) > 0)
            {
            LuaBatchProjectWizard = new ProjectWizardDlg(wxGetApp().GetMainFrame(), ProjectType::BatchProject,
                wxString(luaL_checkstring(L, 1), wxConvUTF8));
            }
        else
            { LuaBatchProjectWizard = new ProjectWizardDlg(wxGetApp().GetMainFrame(), ProjectType::BatchProject); }
        LuaBatchProjectWizard->SelectPage(0);
        LuaBatchProjectWizard->FindWindow(wxID_FORWARD)->SetFocus();
        LuaBatchProjectWizard->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowBatchProjectWizardTextSourcePage(lua_State *L)
        {
        if (LuaBatchProjectWizard != nullptr)
            { CloseBatchProjectWizard(nullptr); }
        LuaBatchProjectWizard = new ProjectWizardDlg(wxGetApp().GetMainFrame(), ProjectType::BatchProject,
            wxString(luaL_checkstring(L, 1), wxConvUTF8));
        LuaBatchProjectWizard->SelectPage(0);
        LuaBatchProjectWizard->FindWindow(wxID_FORWARD)->SetFocus();
        LuaBatchProjectWizard->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowBatchProjectWizardTestRecommendationPage(lua_State *L)
        {
        if (LuaBatchProjectWizard != nullptr)
            { CloseBatchProjectWizard(nullptr); }
        LuaBatchProjectWizard = new ProjectWizardDlg(wxGetApp().GetMainFrame(), ProjectType::BatchProject);
        if (lua_gettop(L) > 0)
            { LuaBatchProjectWizard->SetTestSelectionMethod(lua_tonumber(L, 1)-1/*make zero-indexed*/); }
        LuaBatchProjectWizard->SelectPage(2);
        LuaBatchProjectWizard->FindWindow(wxID_FORWARD)->SetFocus();
        LuaBatchProjectWizard->Show();
        wxGetApp().Yield();
        ::wxSleep(2);
        return 0;
        }

    //-------------------------------------------------------------
    int ShowBatchProjectWizardTestByIndustryPage(lua_State *L)
        {
        if (LuaBatchProjectWizard != nullptr)
            { CloseBatchProjectWizard(nullptr); }
        LuaBatchProjectWizard = new ProjectWizardDlg(wxGetApp().GetMainFrame(), ProjectType::BatchProject);
        LuaBatchProjectWizard->SetTestSelectionMethod(1);
        LuaBatchProjectWizard->SelectPage(2);
        if (lua_gettop(L) > 0)
            {
            const int selected = lua_tonumber(L, 1)-1;//make zero-indexed
            LuaBatchProjectWizard->SelectIndustryType(static_cast<readability::industry_classification>(selected));
            }
        LuaBatchProjectWizard->FindWindow(wxID_FORWARD)->SetFocus();
        LuaBatchProjectWizard->Show();
        wxGetApp().Yield();
        ::wxSleep(2);
        return 0;
        }

    //-------------------------------------------------------------
    int ShowBatchProjectWizardTestByDocumentTypePage(lua_State *L)
        {
        if (LuaBatchProjectWizard != nullptr)
            { CloseBatchProjectWizard(nullptr); }
        LuaBatchProjectWizard = new ProjectWizardDlg(wxGetApp().GetMainFrame(), ProjectType::BatchProject);
        LuaBatchProjectWizard->SetTestSelectionMethod(0);
        LuaBatchProjectWizard->SelectPage(2);
        if (lua_gettop(L) > 0)
            {
            const int selection = lua_tonumber(L, 1)-1;//make zero-indexed
            LuaBatchProjectWizard->SelectDocumentType(static_cast<readability::document_classification>(selection));
            }
        LuaBatchProjectWizard->FindWindow(wxID_FORWARD)->SetFocus();
        LuaBatchProjectWizard->Show();
        wxGetApp().Yield();
        ::wxSleep(2);
        return 0;
        }

    //-------------------------------------------------------------
    int ShowBatchProjectWizardDocumentStructurePage(lua_State *L)
        {
        if (LuaBatchProjectWizard != nullptr)
            { CloseBatchProjectWizard(nullptr); }
        LuaBatchProjectWizard = new ProjectWizardDlg(wxGetApp().GetMainFrame(), ProjectType::BatchProject);
        if (lua_gettop(L) > 0)
            {
            const int selectedDocType = lua_tonumber(L, 1)-1/*make zero-indexed*/;
            (selectedDocType == 0) ?
                LuaBatchProjectWizard->SetNarrativeSelected() :
                LuaBatchProjectWizard->SetFragmentedTextSelected();
            if (lua_gettop(L) > 1)
                { LuaBatchProjectWizard->SetSplitLinesSelected(int_to_bool(lua_toboolean(L, 2))); }
            if (lua_gettop(L) > 2)
                { LuaBatchProjectWizard->SetCenteredTextSelected(int_to_bool(lua_toboolean(L, 3))); }
            if (lua_gettop(L) > 3)
                { LuaBatchProjectWizard->SetNewLinesAlwaysNewParagraphsSelected(int_to_bool(lua_toboolean(L, 4))); }
            }
        LuaBatchProjectWizard->SelectPage(1);
        LuaBatchProjectWizard->FindWindow(wxID_FORWARD)->SetFocus();
        LuaBatchProjectWizard->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowBatchProjectWizardManualTestSelectionPage(lua_State* L)
        {
        if (LuaBatchProjectWizard != nullptr)
            { CloseBatchProjectWizard(nullptr); }
        LuaBatchProjectWizard = new ProjectWizardDlg(wxGetApp().GetMainFrame(), ProjectType::BatchProject);
        LuaBatchProjectWizard->SetTestSelectionMethod(2);
        LuaBatchProjectWizard->SelectPage(2);
        for (int i = 1; i <= lua_gettop(L); ++i)
            {
            LuaStandardProjectWizard->SelectStandardTestManually(
                wxString(luaL_checkstring(L, i), wxConvUTF8));
            }
        LuaBatchProjectWizard->FindWindow(wxID_FORWARD)->SetFocus();
        LuaBatchProjectWizard->Show();
        wxGetApp().Yield();
        ::wxSleep(2);
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProjectWizardTextSourcePageSetFiles(lua_State *L)
        {
        if (LuaBatchProjectWizard != nullptr)
            { CloseBatchProjectWizard(nullptr); }
        LuaBatchProjectWizard = new ProjectWizardDlg(wxGetApp().GetMainFrame(), ProjectType::BatchProject);
        LuaBatchProjectWizard->GetFileList()->DeleteAllItems();
        if (lua_gettop(L) > 0)
            {
            LuaBatchProjectWizard->GetFileList()->SetVirtualDataSize(1,1);
            LuaBatchProjectWizard->GetFileList()->SetItemText(0,0,wxString(luaL_checkstring(L, 1), wxConvUTF8));
            }
        LuaBatchProjectWizard->FindWindow(wxID_FORWARD)->SetFocus();
        LuaBatchProjectWizard->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int CloseBatchProjectWizard(lua_State*)
        {
        if (LuaBatchProjectWizard)
            {
            LuaBatchProjectWizard->Destroy();
            LuaBatchProjectWizard = nullptr;
            }
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowTestBundleDialog(lua_State *L)
        {
        if (LuaTestBundleDlg)
            { LuaTestBundleDlg->Close(); }

        TestBundle bundle(L"");
        for (int i = 2; i < lua_gettop(L); ++i)
            { bundle.GetTestGoals().insert(TestGoal{ wxString(luaL_checkstring(L, i+1), wxConvUTF8).wc_str() }); }
        LuaTestBundleDlg = new TestBundleDlg(wxGetApp().GetMainFrame(), bundle);
        if (lua_gettop(L) > 0)
            { LuaTestBundleDlg->SetTestBundleName(wxString(luaL_checkstring(L, 1), wxConvUTF8).wc_str()); }
        if (lua_gettop(L) > 1)
            { LuaTestBundleDlg->SelectPage(lua_tonumber(L, 2)); }
        LuaTestBundleDlg->Show();
        LuaTestBundleDlg->FindWindow(wxID_OK)->SetFocus();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int CloseTestBundleDialog(lua_State*)
        {
        if (LuaTestBundleDlg)
            {
            LuaTestBundleDlg->Destroy();
            LuaTestBundleDlg = nullptr;
            }
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowCustomTestDialogFunctionBrowser(lua_State*)
        {
        if (LuaCustomTestDlg == nullptr)
            { LuaCustomTestDlg = new CustomTestDlg(wxGetApp().GetMainFrame()); }
        LuaCustomTestDlg->GetFunctionBrowser()->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int CloseCustomTestDialogFunctionBrowser(lua_State*)
        {
        if (LuaCustomTestDlg == nullptr)
            { return 0; }
        LuaCustomTestDlg->GetFunctionBrowser()->Hide();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowCustomTestDialogGeneralSettings(lua_State *L)
        {
        if (LuaCustomTestDlg == nullptr)
            { LuaCustomTestDlg = new CustomTestDlg(wxGetApp().GetMainFrame()); }
        LuaCustomTestDlg->SelectPage(CustomTestDlg::ID_GENERAL_PAGE);
        // set the test name (if provided)
        if (lua_gettop(L) > 0)
            { LuaCustomTestDlg->SetTestName(wxString(luaL_checkstring(L, 1), wxConvUTF8), false); }
        // set formula (if provided)
        if (lua_gettop(L) > 1)
            { LuaCustomTestDlg->SetFormula(wxString(luaL_checklstring(L, 2, nullptr), wxConvUTF8)); }
        // set the test type (if provided)
        if (lua_gettop(L) > 2)
            { LuaCustomTestDlg->SetTestType(lua_tonumber(L, 3)); }
        LuaCustomTestDlg->FindWindow(wxID_OK)->SetFocus();
        LuaCustomTestDlg->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowCustomTestDialogProperNounsAndNumbers(lua_State *L)
        {
        if (LuaCustomTestDlg == nullptr)
            { LuaCustomTestDlg = new CustomTestDlg(wxGetApp().GetMainFrame()); }
        LuaCustomTestDlg->SelectPage(CustomTestDlg::ID_PROPER_NUMERALS_PAGE);
        if (lua_gettop(L) > 0)
            { LuaCustomTestDlg->SetProperNounMethod(lua_tonumber(L, 1)); }
        if (lua_gettop(L) > 1)
            { LuaCustomTestDlg->SetIncludingNumeric(int_to_bool(lua_toboolean(L, 2))); }
        LuaCustomTestDlg->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowCustomTestDialogFamiliarWords(lua_State *L)
        {
        if (LuaCustomTestDlg == nullptr)
            { LuaCustomTestDlg = new CustomTestDlg(wxGetApp().GetMainFrame()); }
        LuaCustomTestDlg->SelectPage(CustomTestDlg::ID_WORD_LIST_PAGE);
        //set the custom words
        if (lua_gettop(L) > 0)
            {
            wxString wordFile(luaL_checklstring(L, 1, nullptr), wxConvUTF8);
            if (wordFile.length())
                {
                LuaCustomTestDlg->SetIncludingCustomWordList(true);
                LuaCustomTestDlg->SetWordListFilePath(wordFile);
                }
            }
        // set the stemming type (if provided)
        if (lua_gettop(L) > 1)
            { LuaCustomTestDlg->SetStemmingType(static_cast<stemming::stemming_type>(static_cast<int>(lua_tonumber(L, 2)))); }
        // other word lists
        if (lua_gettop(L) > 2)
            { LuaCustomTestDlg->SetIncludingDaleChallList(int_to_bool(lua_toboolean(L, 3))); }
        if (lua_gettop(L) > 3)
            { LuaCustomTestDlg->SetIncludingSpacheList(int_to_bool(lua_toboolean(L, 4))); }
        if (lua_gettop(L) > 4)
            { LuaCustomTestDlg->SetIncludingHJList(int_to_bool(lua_toboolean(L, 5))); }
        if (lua_gettop(L) > 5)
            { LuaCustomTestDlg->SetIncludingStockerList(int_to_bool(lua_toboolean(L, 6))); }
        // whether a union of word lists should be used
        if (lua_gettop(L) > 6)
            { LuaCustomTestDlg->SetFamiliarWordsMustBeOnEachIncludedList(int_to_bool(lua_toboolean(L, 7))); }
        LuaCustomTestDlg->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowCustomTestDialogClassification(lua_State*)
        {
        if (LuaCustomTestDlg == nullptr)
            { LuaCustomTestDlg = new CustomTestDlg(wxGetApp().GetMainFrame()); }
        LuaCustomTestDlg->SelectPage(CustomTestDlg::ID_CLASSIFICATION_PAGE);
        LuaCustomTestDlg->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int SetCustomTestDialogDocumentTypes(lua_State *L)
        {
        if (lua_gettop(L) < 5)
            {
            wxMessageBox(
                wxString::Format(_("%s: Invalid number of arguments."), wxString(__WXFUNCTION__)),
                _("Script Error"), wxOK|wxICON_EXCLAMATION);
            lua_pushboolean(L, false);
            return 1;
            }
        if (LuaCustomTestDlg == nullptr)
            { LuaCustomTestDlg = new CustomTestDlg(wxGetApp().GetMainFrame()); }
        LuaCustomTestDlg->SetGeneralDocumentSelected(int_to_bool(lua_tonumber(L, 1)));
        LuaCustomTestDlg->SetTechnicalDocumentSelected(int_to_bool(lua_toboolean(L, 2)));
        LuaCustomTestDlg->SetNonNarrativeFormSelected(int_to_bool(lua_toboolean(L, 3)));
        LuaCustomTestDlg->SetYoungAdultAndAdultLiteratureSelected(int_to_bool(lua_toboolean(L, 4)));
        LuaCustomTestDlg->SetChildrensLiteratureSelected(int_to_bool(lua_toboolean(L, 5)));
        wxGetApp().Yield();
        lua_pushboolean(L, true);
        return 1;
        }

    //-------------------------------------------------------------
    int SetCustomTestDialogIndustries(lua_State *L)
        {
        if (lua_gettop(L) < 7)
            {
            wxMessageBox(wxString::Format(
                _("%s: Invalid number of arguments."), wxString(__WXFUNCTION__)),
                _("Script Error"), wxOK|wxICON_EXCLAMATION);
            lua_pushboolean(L, false);
            return 1;
            }
        if (LuaCustomTestDlg == nullptr)
            { LuaCustomTestDlg = new CustomTestDlg(wxGetApp().GetMainFrame()); }
        LuaCustomTestDlg->SetChildrensPublishingSelected(int_to_bool(lua_toboolean(L, 1)));
        LuaCustomTestDlg->SetAdultPublishingSelected(int_to_bool(lua_toboolean(L, 2)));
        LuaCustomTestDlg->SetChildrensHealthCareTestSelected(int_to_bool(lua_toboolean(L, 3)));
        LuaCustomTestDlg->SetAdultHealthCareTestSelected(int_to_bool(lua_toboolean(L, 4)));
        LuaCustomTestDlg->SetMilitaryTestSelected(int_to_bool(lua_toboolean(L, 5)));
        LuaCustomTestDlg->SetSecondaryLanguageSelected(int_to_bool(lua_toboolean(L, 6)));
        LuaCustomTestDlg->SetBroadcastingSelected(int_to_bool(lua_toboolean(L, 7)));
        wxGetApp().Yield();
        lua_pushboolean(L, true);
        return 1;
        }

    //-------------------------------------------------------------
    int SetCustomTestDialogIncludedLists(lua_State *L)
        {
        if (LuaCustomTestDlg == nullptr)
            { LuaCustomTestDlg = new CustomTestDlg(wxGetApp().GetMainFrame()); }
        LuaCustomTestDlg->SetIncludingCustomWordList(int_to_bool(lua_toboolean(L, 1)));
        LuaCustomTestDlg->SetIncludingDaleChallList(int_to_bool(lua_toboolean(L, 2)));
        LuaCustomTestDlg->SetIncludingSpacheList(int_to_bool(lua_toboolean(L, 3)));
        LuaCustomTestDlg->SetIncludingHJList(int_to_bool(lua_toboolean(L, 4)));
        LuaCustomTestDlg->SetIncludingStockerList(int_to_bool(lua_toboolean(L, 5)));
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int CloseCustomTestDialog(lua_State*)
        {
        if (LuaCustomTestDlg)
            {
            LuaCustomTestDlg->Destroy();
            LuaCustomTestDlg = nullptr;
            }
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int OpenOptions(lua_State *L)
        {
        if (LuaOptionsDlg == nullptr)
            { LuaOptionsDlg = new ToolsOptionsDlg(wxGetApp().GetMainFrame()); }
        LuaOptionsDlg->SelectPage(lua_tonumber(L, 1));
        LuaOptionsDlg->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int CloseOptions(lua_State*)
        {
        if (LuaOptionsDlg)
            {
            LuaOptionsDlg->Destroy();
            LuaOptionsDlg = nullptr;
            }
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowPrinterHeaderFooterOptions(lua_State*)
        {
        if (LuaPrinterOptions == nullptr)
            {
            LuaPrinterOptions =new PrinterHeaderFooterDlg(
                wxGetApp().GetMainFrame(),
                wxGetApp().GetAppOptions().GetLeftPrinterHeader(),
                wxGetApp().GetAppOptions().GetCenterPrinterHeader(),
                wxGetApp().GetAppOptions().GetRightPrinterHeader(),
                wxGetApp().GetAppOptions().GetLeftPrinterFooter(),
                wxGetApp().GetAppOptions().GetCenterPrinterFooter(),
                wxGetApp().GetAppOptions().GetRightPrinterFooter());
            }
        LuaPrinterOptions->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ClosePrinterHeaderFooterOptions(lua_State*)
        {
        if (LuaPrinterOptions)
            {
            LuaPrinterOptions->Destroy();
            LuaPrinterOptions = nullptr;
            }
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowListExpordDlg(lua_State *L)
        {
        if (LuaListExportDlg == nullptr)
            {
            LuaListExportDlg = new GridExportDlg(wxGetApp().GetMainFrame(),
                lua_tonumber(L, 1), lua_tonumber(L, 2), GridExportFormat::ExportHtml);
            }
        LuaListExportDlg->IncludeColumnHeaders(int_to_bool(lua_toboolean(L, 3)));
        LuaListExportDlg->ExportSelectedRowsOnly(int_to_bool(lua_toboolean(L, 4)));
        LuaListExportDlg->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int CloseListExpordDlg(lua_State*)
        {
        if (LuaListExportDlg)
            {
            LuaListExportDlg->Destroy();
            LuaListExportDlg = nullptr;
            }
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int CloseListViewItemDlg(lua_State*)
        {
        if (LuaListViewItemDlg)
            {
            LuaListViewItemDlg->Destroy();
            LuaListViewItemDlg = nullptr;
            }
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowListViewItemDlg(lua_State *L)
        {
        if (LuaListViewItemDlg)
            { CloseListViewItemDlg(L); }
        LuaListViewItemDlg = new ListCtrlItemViewDlg;
        FilePathResolver fileResolve;
        for (int i = 1; i < lua_gettop(L); i += 2)
            {
            fileResolve.ResolvePath(
                wxString(luaL_checklstring(L, i + 1, nullptr), wxConvUTF8), false);
            LuaListViewItemDlg->AddValue(
                wxGetTranslation(wxString(luaL_checklstring(L, i, nullptr), wxConvUTF8)),
                wxGetTranslation(wxString(luaL_checklstring(L, i + 1, nullptr), wxConvUTF8)),
                !fileResolve.IsInvalidFile());
            }
        LuaListViewItemDlg->Create(wxGetApp().GetMainFrame());
        LuaListViewItemDlg->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowGetArchiveDlg(lua_State *L)
        {
        if (LuaGetArchiveDlg == nullptr)
            {
            LuaGetArchiveDlg = new ArchiveDlg(wxGetApp().GetMainFrame(),
                wxGetApp().GetAppOptions().GetDocumentFilter());
            }
        LuaGetArchiveDlg->SetPath(wxString(luaL_checkstring(L, 1), wxConvUTF8));
        LuaGetArchiveDlg->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int CloseGetArchiveDlg(lua_State*)
        {
        if (LuaGetArchiveDlg)
            {
            LuaGetArchiveDlg->Destroy();
            LuaGetArchiveDlg = nullptr;
            }
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowGetDirDlg(lua_State *L)
        {
        if (LuaGetDirDlg == nullptr)
            {
            LuaGetDirDlg = new GetDirFilterDialog(wxGetApp().GetMainFrame(),
                wxGetApp().GetAppOptions().GetDocumentFilter());
            }
        LuaGetDirDlg->SetPath(wxString(luaL_checkstring(L, 1), wxConvUTF8));
        LuaGetDirDlg->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int CloseGetDirDlg(lua_State*)
        {
        if (LuaGetDirDlg)
            {
            LuaGetDirDlg->Destroy();
            LuaGetDirDlg = nullptr;
            }
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowSelectProjectTypeDlg(lua_State *L)
        {
        if (LuaSelectProjectType == nullptr)
            {
            wxArrayString docNames;
            docNames.Add(_("Standard Project"));
            docNames.Add(_("Batch Project"));
            wxArrayString docDescriptions;
            LuaSelectProjectType = new RadioBoxDlg(wxGetApp().GetMainFrame(),
                _("Select Project Type"), wxEmptyString,
                _("Project types:"), _("New Project"),
                docNames, docDescriptions);
            }
        LuaSelectProjectType->SetSelection(lua_tonumber(L, 1)-1/*make zero-indexed*/);
        LuaSelectProjectType->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int CloseSelectProjectTypeDlg(lua_State*)
        {
        if (LuaSelectProjectType)
            {
            LuaSelectProjectType->Destroy();
            LuaSelectProjectType = nullptr;
            }
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowWebHarvesterDlg(lua_State*)
        {
        if (LuaSelectProjectType == nullptr)
            {
            LuaWebHarvesterDlg = new WebHarvesterDlg(wxGetApp().GetMainFrame(),
                wxGetApp().GetLastSelectedWebPages(),
                wxGetApp().m_harvesterOptions.GetDepthLevel(),
                wxGetApp().GetAppOptions().GetDocumentFilter(), wxGetApp().GetLastSelectedDocFilter(),
                false,
                wxGetApp().m_harvesterOptions.IsDownloadingFilesWhileCrawling(),
                wxGetApp().m_harvesterOptions.IsKeepingWebPathWhenDownloading(),
                wxGetApp().m_harvesterOptions.GetDownloadDirectory(),
                wxGetApp().m_harvesterOptions.GetUserAgent(),
                static_cast<int>(wxGetApp().m_harvesterOptions.GetDomainRestriction()),
                wxGetApp().m_harvesterOptions.GetAllowableWebFolders(),
                static_cast<int>(wxGetApp().m_harvesterOptions.GetMinimumDownloadFileSizeInKilobytes()));
            }
        LuaWebHarvesterDlg->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int CloseWebHarvesterDlg(lua_State*)
        {
        if (LuaWebHarvesterDlg)
            {
            LuaWebHarvesterDlg->Destroy();
            LuaWebHarvesterDlg = nullptr;
            }
        wxGetApp().Yield();
        return 0;
        }
    }
