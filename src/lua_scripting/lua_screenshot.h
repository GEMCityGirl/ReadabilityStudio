/** @addtogroup Scripting
    @brief Classes for the scripting.
    @date 2005-2023
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __LUASCREENSHOT_H__
#define __LUASCREENSHOT_H__

#include "lua.hpp"
#include <wx/wx.h>

/// @private
enum class ProjectScreenshotCropMode
    {
    NoCrop,
    CropToSidebarBottom,
    CropToSidebarSelectedItem
    };

/// @private
/// @internal This is documented in the ReadabilityStudioAPI help.
namespace LuaScripting
    {
    int ShowScriptEditor(lua_State *L);
    int SnapScreenshot(lua_State *L);
    int SnapScreenshotWithAnnotation(lua_State *L);
    int SnapScreenshotOfActiveProject(lua_State *L);
    int SnapScreenshotOfDialogWithPropertyGrid(lua_State *L);
    int SnapScreenshotOfTextWindow(lua_State *L);
    int SnapScreenshotOfListControl(lua_State *L);
    int SnapScreenshotOfRibbon(lua_State *L);
    int HighlightScreenshot(lua_State *L);
    int ConvertImage(lua_State *L);
    int ShowStandardProjectWizardLanguagePage(lua_State*);
    int ShowStandardProjectWizardTextSourcePage(lua_State *L);
    int ShowStandardProjectWizardTextFromFilePage(lua_State *L);
    int SetStandardProjectWizardTextFromFilePath(lua_State *L);
    int ShowStandardProjectWizardTextEntryPage(lua_State *L);
    int ShowStandardProjectWizardTestRecommendationPage(lua_State *L);
    int ShowStandardProjectWizardTestByIndustryPage(lua_State *L);
    int ShowStandardProjectWizardTestByDocumentTypePage(lua_State *L);
    int ShowStandardProjectWizardDocumentStructurePage(lua_State *L);
    int ShowStandardProjectWizardManualTestSelectionPage(lua_State* L);
    int ShowStandardProjectWizardTestByBundlePage(lua_State*);
    int CloseStandardProjectWizard(lua_State*);
    int ShowBatchProjectWizardLanguagePage(lua_State *L);
    int ShowBatchProjectWizardTextSourcePage(lua_State *L);
    int ShowBatchProjectWizardTestRecommendationPage(lua_State *L);
    int ShowBatchProjectWizardTestByIndustryPage(lua_State *L);
    int ShowBatchProjectWizardTestByDocumentTypePage(lua_State *L);
    int ShowBatchProjectWizardDocumentStructurePage(lua_State *L);
    int ShowBatchProjectWizardManualTestSelectionPage(lua_State* L);
    int BatchProjectWizardTextSourcePageSetFiles(lua_State *L);
    int CloseBatchProjectWizard(lua_State*);
    int ShowTestBundleDialog(lua_State *L);
    int CloseTestBundleDialog(lua_State*);
    int ShowCustomTestDialogFunctionBrowser(lua_State*);
    int CloseCustomTestDialogFunctionBrowser(lua_State*);
    int ShowCustomTestDialogGeneralSettings(lua_State *L);
    int ShowCustomTestDialogFamiliarWords(lua_State *L);
    int ShowCustomTestDialogProperNounsAndNumbers(lua_State *L);
    int ShowCustomTestDialogClassification(lua_State*);
    int SetCustomTestDialogDocumentTypes(lua_State *L);
    int SetCustomTestDialogIndustries(lua_State *L);
    int SetCustomTestDialogIncludedLists(lua_State *L);
    int CloseCustomTestDialog(lua_State*);
    int OpenOptions(lua_State *L);
    int CloseOptions(lua_State*);
    int ShowPrinterHeaderFooterOptions(lua_State*);
    int ClosePrinterHeaderFooterOptions(lua_State*);
    int ShowListExpordDlg(lua_State *L);
    int CloseListExpordDlg(lua_State*);
    int CloseListViewItemDlg(lua_State*);
    int ShowListViewItemDlg(lua_State *L);
    int ShowGetDirDlg(lua_State *L);
    int ShowGetArchiveDlg(lua_State *L);
    int CloseGetDirDlg(lua_State*);
    int CloseGetArchiveDlg(lua_State*);
    int ShowSelectProjectTypeDlg(lua_State *L);
    int CloseSelectProjectTypeDlg(lua_State*);
    int ShowWebHarvesterDlg(lua_State*);
    int CloseWebHarvesterDlg(lua_State*);
    int ShowFilteredTextPreviewDlg(lua_State*);
    int CloseFilteredTextPreviewDlg(lua_State*);
    int ShowSortListDlg(lua_State* L);
    int CloseSortListDlg(lua_State*);
    int ShowEditWordListDlg(lua_State* L);
    int CloseEditWordListDlg(lua_State*);
    int ShowDocGroupSelectDlg(lua_State* L);
    int CloseDocGroupSelectDlg(lua_State*);

    static const luaL_Reg ScreenshotLib[] =
        {
            { "ShowEditWordListDlg", ShowEditWordListDlg },
            { "CloseEditWordListDlg", CloseEditWordListDlg },
            { "ShowDocGroupSelectDlg", ShowDocGroupSelectDlg },
            { "CloseDocGroupSelectDlg", CloseDocGroupSelectDlg },
            { "ShowSortListDlg", ShowSortListDlg },
            { "CloseSortListDlg", CloseSortListDlg },
            { "ShowFilteredTextPreviewDlg", ShowFilteredTextPreviewDlg },
            { "CloseFilteredTextPreviewDlg", CloseFilteredTextPreviewDlg },
            { "ShowWebHarvesterDlg", ShowWebHarvesterDlg },
            { "CloseWebHarvesterDlg", CloseWebHarvesterDlg },
            { "ShowSelectProjectTypeDlg", ShowSelectProjectTypeDlg },
            { "CloseSelectProjectTypeDlg", CloseSelectProjectTypeDlg },
            { "ShowGetDirDlg", ShowGetDirDlg },
            { "CloseGetDirDlg", CloseGetDirDlg },
            { "ShowGetArchiveDlg", ShowGetArchiveDlg },
            { "CloseGetArchiveDlg", CloseGetArchiveDlg },
            { "ShowListViewItemDlg", ShowListViewItemDlg },
            { "CloseListViewItemDlg", CloseListViewItemDlg },
            { "ShowListExpordDlg", ShowListExpordDlg },
            { "CloseListExpordDlg", CloseListExpordDlg },
            { "ShowPrinterHeaderFooterOptions", ShowPrinterHeaderFooterOptions },
            { "ClosePrinterHeaderFooterOptions", ClosePrinterHeaderFooterOptions },
            { "ShowStandardProjectWizardLanguagePage", ShowStandardProjectWizardLanguagePage },
            { "ShowStandardProjectWizardTextSourcePage", ShowStandardProjectWizardTextSourcePage },
            { "ShowStandardProjectWizardTextFromFilePage", ShowStandardProjectWizardTextFromFilePage },
            { "SetStandardProjectWizardTextFromFilePath", SetStandardProjectWizardTextFromFilePath },
            { "ShowStandardProjectWizardTextEntryPage", ShowStandardProjectWizardTextEntryPage },
            { "ShowStandardProjectWizardTestRecommendationPage", ShowStandardProjectWizardTestRecommendationPage },
            { "ShowStandardProjectWizardTestByIndustryPage", ShowStandardProjectWizardTestByIndustryPage },
            { "ShowStandardProjectWizardTestByDocumentTypePage", ShowStandardProjectWizardTestByDocumentTypePage },
            { "ShowStandardProjectWizardManualTestSelectionPage", ShowStandardProjectWizardManualTestSelectionPage },
            { "ShowStandardProjectWizardTestByBundlePage", ShowStandardProjectWizardTestByBundlePage },
            { "ShowStandardProjectWizardDocumentStructurePage", ShowStandardProjectWizardDocumentStructurePage },
            { "ShowBatchProjectWizardLanguagePage", ShowBatchProjectWizardLanguagePage },
            { "ShowBatchProjectWizardTextSourcePage", ShowBatchProjectWizardTextSourcePage },
            { "ShowBatchProjectWizardTestRecommendationPage", ShowBatchProjectWizardTestRecommendationPage },
            { "ShowBatchProjectWizardTestByIndustryPage", ShowBatchProjectWizardTestByIndustryPage },
            { "ShowBatchProjectWizardTestByDocumentTypePage", ShowBatchProjectWizardTestByDocumentTypePage },
            { "ShowBatchProjectWizardDocumentStructurePage", ShowBatchProjectWizardDocumentStructurePage },
            { "ShowBatchProjectWizardManualTestSelectionPage", ShowBatchProjectWizardManualTestSelectionPage },
            { "BatchProjectWizardTextSourcePageSetFiles", BatchProjectWizardTextSourcePageSetFiles },
            { "CloseBatchProjectWizard", CloseBatchProjectWizard },
            { "CloseStandardProjectWizard", CloseStandardProjectWizard },
            { "OpenOptions", OpenOptions },
            { "CloseOptions", CloseOptions },
            { "ShowTestBundleDialog", ShowTestBundleDialog },
            { "CloseTestBundleDialog", CloseTestBundleDialog },
            { "ShowCustomTestDialogFunctionBrowser", ShowCustomTestDialogFunctionBrowser },
            { "CloseCustomTestDialogFunctionBrowser", CloseCustomTestDialogFunctionBrowser },
            { "ShowCustomTestDialogGeneralSettings", ShowCustomTestDialogGeneralSettings },
            { "ShowCustomTestDialogProperNounsAndNumbers", ShowCustomTestDialogProperNounsAndNumbers },
            { "ShowCustomTestDialogFamiliarWords", ShowCustomTestDialogFamiliarWords },
            { "ShowCustomTestDialogClassification", ShowCustomTestDialogClassification },
            { "SetCustomTestDialogIncludedLists", SetCustomTestDialogIncludedLists },
            { "SetCustomTestDialogDocumentTypes", SetCustomTestDialogDocumentTypes },
            { "SetCustomTestDialogIndustries", SetCustomTestDialogIndustries },
            { "CloseCustomTestDialog", CloseCustomTestDialog },
            { "SnapScreenshot", SnapScreenshot },
            { "SnapScreenshotWithAnnotation", SnapScreenshotWithAnnotation },
            { "SnapScreenshotOfActiveProject", SnapScreenshotOfActiveProject },
            { "SnapScreenshotOfDialogWithPropertyGrid", SnapScreenshotOfDialogWithPropertyGrid },
            { "SnapScreenshotOfTextWindow", SnapScreenshotOfTextWindow },
            { "SnapScreenshotOfListControl", SnapScreenshotOfListControl },
            { "SnapScreenshotOfRibbon", SnapScreenshotOfRibbon },
            { "HighlightScreenshot", HighlightScreenshot },
            { "ConvertImage", ConvertImage },
            { "ShowScriptEditor", ShowScriptEditor },
            { nullptr, nullptr }
        };
    }

/** @}*/

#endif //__LUASCREENSHOT_H__
