dofile(Application.GetLuaConstantsPath())

ScreenshotLib.ShowScriptEditor(false)

-- input
ScreenshotProjectsFolder = Application.GetAbsoluteFilePath(
            Debug.GetScriptFolderPath(),
            "Projects/")

-- output
ImagePath = Debug.GetScriptFolderPath().."ReadabilityStudioDocs/images/"
FileExtension = "bmp"

function wait(seconds)
  local start = os.time()
  repeat until os.time() > start + seconds
end

wait(4)

-- reset settings for the screenshots
Application.RemoveAllCustomTests()
Application.RemoveAllCustomTestBundles()
Application.ResetSettings()
Application.DisableAllWarnings()
Application.SetWindowSize(1000, 700)
Application.SetReviewer("Isabelle M.")

Application.SetCenterPrintHeader("@TITLE@")
Application.SetRightPrintHeader("A. Clark")
Application.SetLeftPrintFooter("Page @PAGENUM@ of @PAGESCNT@")
Application.SetRightPrintFooter("@DATE@")

ScreenshotLib.ShowTestBundleDialog("4-F")
ScreenshotLib.SnapScreenshot(ImagePath .. "TestBundle." .. FileExtension)

ScreenshotLib.ShowTestBundleDialog("4-F", 20002, "FORCAST", "flesch-kincaid", "fry", "gunning fog")
ScreenshotLib.SnapScreenshot(ImagePath .. "TestBundle4F." .. FileExtension)

ScreenshotLib.CloseTestBundleDialog()

ScreenshotLib.ShowCustomTestDialogFunctionBrowser()
ScreenshotLib.SnapScreenshot(ImagePath .. "FunctionBrowser." .. FileExtension)
ScreenshotLib.CloseCustomTestDialogFunctionBrowser()

ScreenshotLib.ShowWebHarvesterDlg()
ScreenshotLib.SnapScreenshot(ImagePath .. "webharvester." .. FileExtension)
ScreenshotLib.CloseWebHarvesterDlg()

ScreenshotLib.ShowSelectProjectTypeDlg(2)
ScreenshotLib.SnapScreenshot(ImagePath .. "selectprojecttype." .. FileExtension)
ScreenshotLib.CloseSelectProjectTypeDlg()

ScreenshotLib.ShowGetDirDlg("/home/rdoyle/ReadabilityStudio/Topics")
ScreenshotLib.SnapScreenshot(ImagePath .. "selectdirectory." .. FileExtension)
ScreenshotLib.CloseGetDirDlg()

ScreenshotLib.ShowListViewItemDlg("Document", "C:\\Licenses\\EULA.rtf", "Count", "1", "Passive Voice", "\"be purchased\"")
ScreenshotLib.SnapScreenshot(ImagePath .. "batchpassivevoiceviewitem." .. FileExtension)
ScreenshotLib.CloseListViewItemDlg()

ScreenshotLib.ShowListViewItemDlg("Document", "C:\\Licenses\\EULA.rtf", "Count", "2", "Article Mismatches", "\"a agreement\" * 2")
ScreenshotLib.SnapScreenshot(ImagePath .. "viewitemarticlemismatch." .. FileExtension)
ScreenshotLib.CloseListViewItemDlg()

ScreenshotLib.ShowListViewItemDlg("Document", "C:\\Topics2008\\Dialogs\\NewProject.html", "Automated Readability Index", "12.1", "New Dale-Chall", "16+", "Flesch-Kincaid", "12.0", "New Fog Count", "8.8", "Simplified Automated Readability Index", "11.0")
ScreenshotLib.SnapScreenshot(ImagePath .. "viewitemrawscores." .. FileExtension)
ScreenshotLib.CloseListViewItemDlg()

ScreenshotLib.ShowListViewItemDlg("Document", "C:\\Topics2008\\Overviews\\BatchProject\\Grammar\\ReviewingRepeatedWords.html", "Count", "3", "Repeated Words", "\"the the\" * 2, \"to to\"")
ScreenshotLib.SnapScreenshot(ImagePath .. "batchrepeatedviewitem." .. FileExtension)
ScreenshotLib.CloseListViewItemDlg()

ScreenshotLib.ShowListViewItemDlg("Document", "C:\\Topics2008\\Overviews\\IntroductoryOverview.html", "Count", "3", "Wordy Phrases", "\"is able to\", \"it is important to note\", \"take into account\"", "Suggestions", "\"can\", \"note\", \"consider\"")
ScreenshotLib.SnapScreenshot(ImagePath .. "viewitemwordyphrase." .. FileExtension)
ScreenshotLib.CloseListViewItemDlg()

ScreenshotLib.ShowListViewItemDlg("Document", "C:\\Topics2008\\Overviews\\BatchProject\\Grammar\\ReviewingRedundantPhrases.html", "Count", "7", "Redundant Phrases", "\"brief summary\" * 3, \"collaborated together\" * 2, \"join together\" * 2", "Suggestions", "\"summary\", \"collaborated\", \"join, combine\"")
ScreenshotLib.SnapScreenshot(ImagePath .. "batchredundantphrasesviewitem." .. FileExtension)
ScreenshotLib.CloseListViewItemDlg()

ScreenshotLib.ShowListViewItemDlg("Document", "C:\\Topics2008\\Overviews\\BatchProject\\Grammar\\ReviewingBatchCliches.html", "Count", "4", "Clichés", "\"off the wall\" * 4", "Explanations/Suggestions", "\"unusual\"")
ScreenshotLib.SnapScreenshot(ImagePath .. "batchclichesviewitem." .. FileExtension)
ScreenshotLib.CloseListViewItemDlg()

ScreenshotLib.ShowListViewItemDlg("Document", "C:\\Topics2008\\Overviews\\StandardProject\\Grammar\\ReviewingHighlightedText.html", "Overly-long Sentences", "9", "Longest Sentence Length", "36", "Longest Sentence", "As a special note for documents with numerous sentences over twenty words long, it is recommended to open the Project Properties dialog and click the Document Analysis icon and select the option Outside sentence-length outlier range.")
ScreenshotLib.SnapScreenshot(ImagePath .. "batchlongestsentenceviewitem." .. FileExtension)
ScreenshotLib.CloseListViewItemDlg()

ScreenshotLib.ShowListViewItemDlg("Document", "C:\\Program Files\\Readability Studio\\examples\\The Frog Prince.txt", "Count", "3", "Starting Word", "\"stop\", \"thy\", \"you\"")
ScreenshotLib.SnapScreenshot(ImagePath .. "batchlowersentenceviewitem." .. FileExtension)
ScreenshotLib.CloseListViewItemDlg()

ScreenshotLib.ShowListViewItemDlg("Document", "C:\\Topics2008\\Overviews\\BatchProject\\Grammar\\WebHarvester.html", "Total", "1", "Unique Count", "1", "Misspellings", "\"specifiy\"")
ScreenshotLib.SnapScreenshot(ImagePath .. "batchmisspellingsviewitem." .. FileExtension)
ScreenshotLib.CloseListViewItemDlg()

ScreenshotLib.ShowListViewItemDlg("Document", "C:\\Topics2008\\Overviews\\BatchProject\\Grammar\\LongSentenceSearching.html", "Count", "1", "Wording Errors", "\"in parenthesis\"")
ScreenshotLib.SnapScreenshot(ImagePath .. "batchwordingerrorsviewitem." .. FileExtension)
ScreenshotLib.CloseListViewItemDlg()

ScreenshotLib.OpenOptions(OptionsPageType.AnalysisDocumentIndexing)
ScreenshotLib.SnapScreenshot(ImagePath .. "OptionsDlg." .. FileExtension)
ScreenshotLib.CloseOptions()

ScreenshotLib.ShowListViewItemDlg("Document", "C:\\Program Files\\Readability Studio\\examples\\The Frog Prince.txt", "Count", "16", "Conjunctions", "\"And\" * 4, \"But\" * 9, \"So\" * 3")
ScreenshotLib.SnapScreenshot(ImagePath .. "batchconjunctionviewitem." .. FileExtension)
ScreenshotLib.CloseListViewItemDlg()

ScreenshotLib.ShowPrinterHeaderFooterOptions()
ScreenshotLib.SnapScreenshot(ImagePath .. "printerheadersfooters." .. FileExtension)
ScreenshotLib.ClosePrinterHeaderFooterOptions()

-- List export
ScreenshotLib.ShowListExpordDlg(1031,1,false,true)
ScreenshotLib.SnapScreenshot(ImagePath .. "exportoptions." .. FileExtension)
ScreenshotLib.CloseListExpordDlg()

-- Custom test dialog
ScreenshotLib.ShowCustomTestDialogGeneralSettings()
ScreenshotLib.SnapScreenshot(ImagePath .. "AddCustomTest." .. FileExtension)

ScreenshotLib.ShowCustomTestDialogFamiliarWords()
ScreenshotLib.SetCustomTestDialogIncludedLists(false,true,false,true)
ScreenshotLib.SnapScreenshot(ImagePath .. "CustomTestIncludeLists." .. FileExtension, 1003, 1005)

ScreenshotLib.ShowCustomTestDialogProperNounsAndNumbers()
ScreenshotLib.SnapScreenshot(ImagePath .. "CustomTestTreat." .. FileExtension)

ScreenshotLib.SetCustomTestDialogDocumentTypes(false,false,false,false,true)
ScreenshotLib.SetCustomTestDialogIndustries(false,false,true,false,false,false,false)
ScreenshotLib.ShowCustomTestDialogClassification()
ScreenshotLib.SnapScreenshot(ImagePath .. "CustomTestWizardSettings." .. FileExtension)

ScreenshotLib.CloseCustomTestDialog()

-- Standard project wizard
ScreenshotLib.ShowStandardProjectWizardTextSourcePage(1)
ScreenshotLib.SnapScreenshot(ImagePath .. "wizard1." .. FileExtension)

ScreenshotLib.SetStandardProjectWizardTextFromFilePath("")
ScreenshotLib.SnapScreenshot(ImagePath .. "exampleselectfile." .. FileExtension, 10001)
ScreenshotLib.SetStandardProjectWizardTextFromFilePath("https://www.libreoffice.org/about-us/who-are-we/")
ScreenshotLib.SnapScreenshot(ImagePath .. "webpath." .. FileExtension)

ScreenshotLib.ShowStandardProjectWizardTestByDocumentTypePage(4)
ScreenshotLib.SnapScreenshot(ImagePath .. "wizarddoctypelitselected." .. FileExtension)

ScreenshotLib.ShowStandardProjectWizardDocumentStructurePage(1, false, false, true)
ScreenshotLib.SnapScreenshot(ImagePath .. "wizarddocstructurehardreturns." .. FileExtension, 10020, 10021)

ScreenshotLib.ShowStandardProjectWizardDocumentStructurePage(1, false, false, false)
ScreenshotLib.SnapScreenshot(ImagePath .. "wizarddocstructure." .. FileExtension)
ScreenshotLib.SnapScreenshot(ImagePath .. "wizarddocstructurenarrativeselected." .. FileExtension, 10024, 10025)
-- option highlighted (but not selected) for example
ScreenshotLib.SnapScreenshot(ImagePath .. "wizarddocstructuresentencessplit." .. FileExtension, 10026, 10027)

ScreenshotLib.ShowStandardProjectWizardDocumentStructurePage(2, false, false, false)
ScreenshotLib.SnapScreenshot(ImagePath .. "wizarddocstructurenonnarrative." .. FileExtension, 10022, 10023)

ScreenshotLib.ShowStandardProjectWizardTestByIndustryPage(2)
ScreenshotLib.SnapScreenshot(ImagePath .. "wizardindustry." .. FileExtension)

ScreenshotLib.ShowStandardProjectWizardTextEntryPage("   ")
ScreenshotLib.SnapScreenshot(ImagePath .. "wizardentertext." .. FileExtension)

ScreenshotLib.ShowStandardProjectWizardManualTestSelectionPage()
ScreenshotLib.SnapScreenshot(ImagePath .. "wizardtests." .. FileExtension)

ScreenshotLib.ShowStandardProjectWizardTestByBundlePage("Patient Consent Forms")
ScreenshotLib.SnapScreenshot(ImagePath .. "wizardbundles." .. FileExtension)

ScreenshotLib.ShowStandardProjectWizardTestRecommendationPage(1)
ScreenshotLib.SnapScreenshot(ImagePath .. "wizardreadabilitymethods." .. FileExtension)

ScreenshotLib.CloseStandardProjectWizard()

-- DDX is really weird with this page for some reason on Vista, so we need to get this separately
ScreenshotLib.ShowStandardProjectWizardLanguagePage()
ScreenshotLib.ShowStandardProjectWizardTestByDocumentTypePage(2)
ScreenshotLib.SnapScreenshot(ImagePath .. "wizarddoctypetechselected." .. FileExtension)
ScreenshotLib.CloseStandardProjectWizard()

-- Batch project wizard
ScreenshotLib.ShowBatchProjectWizardTextSourcePage(ScreenshotProjectsFolder .. "Topics2008")
ScreenshotLib.SnapScreenshot(ImagePath .. "wizardbatchselectfilesloaded." .. FileExtension)
ScreenshotLib.BatchProjectWizardTextSourcePageSetFiles()
ScreenshotLib.SnapScreenshot(ImagePath .. "wizardbatchselectfiles." .. FileExtension)
ScreenshotLib.SnapScreenshot(ImagePath .. "randomsample." .. FileExtension, 10011, 10012)

ScreenshotLib.ShowBatchProjectWizardTestByDocumentTypePage(2)
ScreenshotLib.SnapScreenshot(ImagePath .. "wizardbatchdoctype." .. FileExtension)

ScreenshotLib.ShowBatchProjectWizardTestByIndustryPage(2)
ScreenshotLib.SnapScreenshot(ImagePath .. "wizardbatchindustry." .. FileExtension)

ScreenshotLib.ShowBatchProjectWizardTestRecommendationPage(1)
ScreenshotLib.SnapScreenshot(ImagePath .. "wizardbatchreadabilitymethods." .. FileExtension)

ScreenshotLib.ShowBatchProjectWizardManualTestSelectionPage()
ScreenshotLib.SnapScreenshot(ImagePath .. "wizardbatchtests." .. FileExtension)

ScreenshotLib.CloseBatchProjectWizard()

-- Batch project
Application.EnableWarning("document-less-than-100-words")
bp = BatchProject(ScreenshotProjectsFolder .. "Topics2008.rsbp")
bp:SetSpellCheckerOptions(true,true,true,true,true)
bp:Reload()
bp:ExportGraph(SideBarSection.ReadabilityScores, GraphType.Fry, ImagePath .. "MultipleFry." .. FileExtension)

bp:SelectWindow(SideBarSection.Grammar, ListType.Cliches)
ScreenshotLib.SnapScreenshotOfListControl(ImagePath .. "batchclichemultiple." .. FileExtension,
    ListType.Cliches, 1, 3, 2, 3, 3)

bp:SelectWindow(SideBarSection.SentencesBreakdown, ListType.LongSentences)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "batchlongsentences." .. FileExtension, 2)

bp:SelectWindow(SideBarSection.Grammar, ListType.OverusedWordsBySentence)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "batchoverusedwordsbysentence." .. FileExtension, 2)

bp:SelectWindow(SideBarSection.Grammar, ListType.MisspelledWords)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "batchmisspellings." .. FileExtension, 2)

bp:SelectWindow(SideBarSection.Grammar, ListType.WordingErrors)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "batchwordingerrors." .. FileExtension, 2)

bp:SelectWindow(SideBarSection.Grammar, ListType.RedundantPhrases)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "batchredundantphrases." .. FileExtension, 2)

bp:SelectWindow(SideBarSection.Grammar, ListType.Wordiness)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "batchwordyphrases." .. FileExtension, 2)

bp:SelectWindow(SideBarSection.Grammar, ListType.PassiveVoice)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "batchpassivevoice." .. FileExtension, 2)

bp:SelectWindow(SideBarSection.WordsBreakdown)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "batchdifficultwordsverbose." .. FileExtension, 1)

bp:SelectWindow(SideBarSection.Warnings)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "Warnings." .. FileExtension, 1)

bp:SelectWindow(SideBarSection.SentencesBreakdown, ListType.LongSentences)
bp:SortList(ListType.LongSentences, 3, SortOrder.SortDescending)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "batchlongestsentence." .. FileExtension, 1)

bp:Close()
Application.DisableAllWarnings()

bp = BatchProject(ScreenshotProjectsFolder .. "Topics2008(Improved) Bundle.rsbp")
bp:SelectWindow(SideBarSection.ReadabilityScores, ListType.Goals)
ScreenshotLib.SnapScreenshotOfListControl(ImagePath .. "BatchProjectGoals." .. FileExtension,
    ListType.Goals, -1, 3)
bp:Close()

Application.RemoveAllCustomTests()
Application.DisableAllWarnings()

-- Batch project (examples)
bp = BatchProject(ScreenshotProjectsFolder .. "examples.rsbp")
bp:SelectWindow(SideBarSection.Grammar, ListType.Cliches)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "batchcliches." .. FileExtension, 2)
bp:SelectWindow(SideBarSection.Grammar, ListType.LowercasedSentences)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "batchlowersentences." .. FileExtension, 2)

bp:SelectWindow(SideBarSection.Grammar, ListType.ConjunctionStartingSentences)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "batchconjunctionstart." .. FileExtension, 2)

bp:SelectWindow(SideBarSection.Dolch, DolchResultType.DolchCoverageBatchSummary)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "batchdolchcoverage." .. FileExtension, 1)

bp:SelectWindow(SideBarSection.Dolch, DolchResultType.DolchWordsBatchSummary)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "batchdolchwords." .. FileExtension, 1)

bp:SelectWindow(SideBarSection.Dolch, DolchResultType.NonDolchWordsBatchSummary)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "batchdolchnonwords." .. FileExtension, 1)

bp:Close()
Application.RemoveAllCustomTests()

-- Batch project
bp = BatchProject(ScreenshotProjectsFolder .. "Topics2008More.rsbp")

bp:ShowSidebar(false)

bp:SelectWindow(SideBarSection.ReadabilityScores, BatchScoreResultType.RawScores)
ScreenshotLib.SnapScreenshot(ImagePath .. "batchrawscores." .. FileExtension)

bp:SelectWindow(SideBarSection.ReadabilityScores, BatchScoreResultType.AggregatedScoreStatistics)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "batchscorestats." .. FileExtension, 1)
bp:Close()
Application.RemoveAllCustomTests()

-- Batch project
bp = BatchProject(ScreenshotProjectsFolder .. "Topics2008(Less Tests).rsbp")

bp:ExportGraph(SideBarSection.Histograms, Application.GetTestId("flesch-kincaid-test"), ImagePath .. "histogrades." .. FileExtension)
bp:ExportGraph(SideBarSection.Histograms, Application.GetTestId("flesch-test"), ImagePath .. "histoindex." .. FileExtension)

bp:ExportGraph(SideBarSection.BoxPlots, Application.GetTestId("flesch-test"), ImagePath .. "boxindex." .. FileExtension)

bp:ShowSidebar(false)
bp:SelectWindow(SideBarSection.ReadabilityScores, BatchScoreResultType.AggregatedGradeScoresByDocument)
ScreenshotLib.SnapScreenshot(ImagePath .. "BatchGradeScoresByDocument." .. FileExtension)

bp:SelectWindow(SideBarSection.ReadabilityScores, BatchScoreResultType.AggregatedClozeScoresByDocument)
ScreenshotLib.SnapScreenshot(ImagePath .. "BatchClozeScoresByDocument." .. FileExtension)

bp:SelectWindow(SideBarSection.WordsBreakdown)
ScreenshotLib.SnapScreenshot(ImagePath .. "batchdifficultwords." .. FileExtension)
bp:Close()

Application.RemoveAllCustomTests()

bp = BatchProject(ScreenshotProjectsFolder .. "Topics2008(Improved).rsbp")
bp:ExportGraph(SideBarSection.Histograms, Application.GetTestId("flesch-kincaid-test"), ImagePath .. "histoimproved." .. FileExtension)
bp:Close()

-- Standard project
-------------------
sp = StandardProject(ScreenshotProjectsFolder .. "Cocoa Desserts.rsp")
sp:ScrollTextWindow(HighlightedReportType.GrammarHighlightedIssues, 500)
ScreenshotLib.SnapScreenshot(ImagePath .. "longsentences." .. FileExtension) 
sp:SelectHighlightedWordReport(HighlightedReportType.ThreePlusSyllableHighlightedWords, 4335, 4347)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "difficultwordselected." .. FileExtension, 1)
sp:SelectWindow(SideBarSection.WordsBreakdown, ListType.ThreePlusSyllableWords)
sp:SortList(ListType.ThreePlusSyllableWordsList, 2, SortOrder.SortDescending)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "exampledifficultwords." .. FileExtension, 1)
sp:SelectWindow(SideBarSection.SentencesBreakdown, ListType.LongSentences)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "examplelongsentences." .. FileExtension, 1)
sp:SelectWindow(SideBarSection.Statistics, ReportType.StatisticsSummaryReport)
ScreenshotLib.SnapScreenshot(ImagePath .. "examplestats." .. FileExtension)
sp:SelectWindow(SideBarSection.ReadabilityScores, ReportType.ReadabilityScoresTabularReport)
ScreenshotLib.SnapScreenshot(ImagePath .. "testscores." .. FileExtension)
sp:SelectTextGrammarWindow(4228, 4469)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "longsentenceselected." .. FileExtension, 1)

sp:DelayReloading(true)
sp:SetDocumentFilePath("/home/mcrane/Cocoa Desserts.rtf") -- use generic filepath
sp:SetReviewer("Maris")
sp:OpenProperties(OptionsPageType.ProjectSettings)
ScreenshotLib.SnapScreenshot(ImagePath .. "projectproperties." .. FileExtension)
ScreenshotLib.SnapScreenshot(ImagePath .. "linktofile." .. FileExtension, 1085, -1, 1083)
sp:CloseProperties()

sp:Close()
Application.RemoveAllCustomTests()

-- Standard project (Frog Prince)
---------------------------------
sp = StandardProject(ScreenshotProjectsFolder .. "The Frog Prince.rsp")
sp:SelectWindow(SideBarSection.Grammar, ListType.ConjunctionStartingSentences)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "conjunctionsent." .. FileExtension, 1)

sp:SelectWindow(SideBarSection.Dolch, DolchResultType.HighlightedNonDolchWordsReport)
ScreenshotLib.SnapScreenshot(ImagePath .. "dolchhighlightednonwords." .. FileExtension)

sp:SelectWindow(SideBarSection.Dolch, DolchResultType.HighlightedDolchWordsReport)
ScreenshotLib.SnapScreenshot(ImagePath .. "dolchhighlightedwords." .. FileExtension)

sp:SelectWindow(SideBarSection.Dolch, DolchResultType.NonDolchWordsList)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "dolchnonwordslist." .. FileExtension, 1)

sp:SelectWindow(SideBarSection.Dolch, DolchResultType.DolchStatisticsSummary)
ScreenshotLib.SnapScreenshot(ImagePath .. "dolchsummary." .. FileExtension)

sp:SelectWindow(SideBarSection.Dolch, DolchResultType.UnusedDolchWordsList)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "dolchunusedwordslist." .. FileExtension, 1)

sp:SelectWindow(SideBarSection.Dolch, DolchResultType.DolchWordsList)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "dolchwordslist." .. FileExtension, 1)
sp:SortList(DolchResultType.DolchWordsList, 2, SortOrder.SortDescending)
ScreenshotLib.SnapScreenshotOfListControl(ImagePath .. "columnheader." .. FileExtension,
    DolchResultType.DolchWordsList, -1, -1, -1, -1, 4)
sp:ExportGraph(GraphType.DolchWordBarChart, ImagePath .. "DolchWordBreakdown." .. FileExtension)
sp:ExportGraph(GraphType.DolchCoverageChart, ImagePath .. "DolchCoverageChart." .. FileExtension)

sp:ExportGraph(GraphType.SentenceHeatmap, ImagePath .. "SentencesHeatmap." .. FileExtension)

sp:Close()
Application.RemoveAllCustomTests()

sp = StandardProject(ScreenshotProjectsFolder .. "Consent.rsp")
sp:SelectWindow(SideBarSection.ReadabilityScores, ListType.Goals)
ScreenshotLib.SnapScreenshotOfListControl(ImagePath .. "StandardProjectGoals." .. FileExtension,
    ListType.Goals)
sp:Close()

-- Standard project (A Christmas Carol)
sp = StandardProject(ScreenshotProjectsFolder .. "A Christmas Carol.rsp")
sp:SelectWindow(SideBarSection.Grammar, ListType.LowercasedSentences)
ScreenshotLib.SnapScreenshot(ImagePath .. "lowercasedsent." .. FileExtension)
sp:SelectWindow(SideBarSection.Grammar, ListType.OverusedWordsBySentence)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "overusedwordsbysentence." .. FileExtension, 2)

sp:SelectWindow(SideBarSection.Grammar, ListType.Wordiness)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "wordylist." .. FileExtension, 2)

sp:SelectWindow(SideBarSection.Grammar, ListType.RedundantPhrases)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "redundantphrases." .. FileExtension, 2)

sp:SelectWindow(SideBarSection.Grammar, ListType.Cliches)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "clichelist." .. FileExtension, 2)
sp:SelectWindow(SideBarSection.Grammar, ListType.PassiveVoice)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "passivevoice." .. FileExtension, 2)

sp:SelectReadabilityTest(4)
ScreenshotLib.SnapScreenshot(ImagePath .. "scores." .. FileExtension)
sp:SelectWindow(SideBarSection.WordsBreakdown, ListType.ThreePlusSyllableWords)
sp:SortList(ListType.ThreePlusSyllableWordsList,2,SortOrder.SortDescending,3,SortOrder.SortDescending)
ScreenshotLib.SnapScreenshot(ImagePath .. "difficultwords." .. FileExtension)

sp:SelectRowsInWordsBreakdownList(ListType.ThreePlusSyllableWords, 1, 3, 4, 5, 7, 8, 9, 10, 12, 13, 15,
                                  16, 18, 19, 22, 23, 25, 26, 27, 30, 31, 32, 33, 34, 35, 36, 37, 40)
ScreenshotLib.SnapScreenshot(ImagePath .. "difficultwords2." .. FileExtension, 500)

sp:ExportGraph(GraphType.WordBarChart, ImagePath .. "GraphBarsRectangular." .. FileExtension)
sp:SetStippleImage(ScreenshotProjectsFolder .. "Book.png")
sp:SetBarChartBarEffect(BoxEffect.StippleImage)
sp:ExportGraph(GraphType.WordBarChart, ImagePath .. "GraphCustomBrush." .. FileExtension)

sp:SetStippleShape("newspaper")
sp:SetBarChartBarEffect(BoxEffect.StippleShape)
sp:ExportGraph(GraphType.WordBarChart, ImagePath .. "GraphStippleShape." .. FileExtension)

sp:Close()
Application.RemoveAllCustomTests()

-- Standard project (Features)
sp = StandardProject(ScreenshotProjectsFolder .. "Features.rsp")
sp:SelectReadabilityTest(2)
ScreenshotLib.SnapScreenshot(ImagePath .. "featuresscores." .. FileExtension)

sp:SelectWindow(SideBarSection.SentencesBreakdown, ListType.LongSentences)
sp:SortList(ListType.LongSentences, 2, SortOrder.SortAscending)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "featuressentencelist." .. FileExtension, 1)

sp:SelectWindow(SideBarSection.Statistics, ReportType.StatisticsSummaryReport)
ScreenshotLib.SnapScreenshot(ImagePath .. "featuresstats." .. FileExtension)
sp:SelectWindow(SideBarSection.SentencesBreakdown, ListType.LongSentences)
sp:SortList(ListType.LongSentences, 2, SortOrder.SortDescending)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "featuressentencelistsorted." .. FileExtension, 1)

sp:OpenProperties(OptionsPageType.AnalysisDocumentIndexing)
ScreenshotLib.SnapScreenshot(ImagePath .. "featuressentoptions." .. FileExtension, 1040, 1041)
sp:CloseProperties()

sp:ScrollTextWindow(HighlightedReportType.GrammarHighlightedIssues, 1300)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "featuressentences." .. FileExtension, 1)
sp:Close()
Application.RemoveAllCustomTests()

-- Standard project (Features with sentence outlier)
sp = StandardProject(ScreenshotProjectsFolder .. "Features(Outlier).rsp")
sp:SelectWindow(SideBarSection.Grammar, HighlightedReportType.GrammarHighlightedIssues)
ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "featuressentencesoutliers." .. FileExtension,
HighlightedReportType.GrammarHighlightedIssues, true,
    2437, 2439, 2959, 2961)

ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "featuressentences25." .. FileExtension,
HighlightedReportType.GrammarHighlightedIssues, true,
    1627, 1629, 1788, 1790)

sp:SelectWindow(SideBarSection.Statistics, ReportType.StatisticsSummaryReport)
ScreenshotLib.SnapScreenshot(ImagePath .. "featuressentences43." .. FileExtension)
sp:Close()
Application.RemoveAllCustomTests()

-- Standard project (improved Cocoa Desserts)
sp = StandardProject(ScreenshotProjectsFolder .. "Cocoa Desserts (Improved).rsp")
ScreenshotLib.SnapScreenshotOfRibbon(ImagePath .. "RibbonEditDocumentOptions." .. FileExtension, 1)
sp:SelectReadabilityTest(2)
ScreenshotLib.SnapScreenshot(ImagePath .. "improvedscores." .. FileExtension)
sp:Close()
Application.RemoveAllCustomTests()

-- Standard project (for wording errors)
sp = StandardProject(ScreenshotProjectsFolder .. "LongSentenceSearching.rsp")
sp:SelectWindow(SideBarSection.Grammar, ListType.WordingErrors)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "wordingerrors." .. FileExtension, 1)
sp:Close()
Application.RemoveAllCustomTests()

-- Standard project (for misspellings)
sp = StandardProject(ScreenshotProjectsFolder .. "WebHarvester.rsp")
sp:SelectWindow(SideBarSection.Grammar, ListType.MisspelledWords)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "misspellings." .. FileExtension, 1)
sp:Close()
Application.RemoveAllCustomTests()

-- Standard project (License)
sp = StandardProject(ScreenshotProjectsFolder .. "license.rsp")
sp:SelectWindow(SideBarSection.Grammar, ListType.RepeatedWords)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "repeatedwords." .. FileExtension, 2)
sp:Close()
Application.RemoveAllCustomTests()

-- Batch project
bp = BatchProject(ScreenshotProjectsFolder .. "Distribution Documents.rsbp")
bp:SelectWindow(SideBarSection.Grammar, ListType.RepeatedWords)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "batchrepeatedwords." .. FileExtension, 2)
bp:Close()
Application.RemoveAllCustomTests()

-- Batch showing multiple items in lists
bp = BatchProject(ScreenshotProjectsFolder .. "Topics2020ForScreenshots.rsbp")
bp:SetSpellCheckerOptions(true,true,true,true,true)
bp:SetWindowSize(1200, 700)

bp:SelectWindow(SideBarSection.Grammar, ListType.Cliches)
ScreenshotLib.SnapScreenshotOfListControl(ImagePath .. "batchclichemultiple." .. FileExtension,
    ListType.Cliches, 1, 4, 2, 4)

bp:SelectWindow(SideBarSection.Grammar, ListType.RedundantPhrases)
ScreenshotLib.SnapScreenshotOfListControl(ImagePath .. "batchredundantphrasesmultiple." .. FileExtension,
    ListType.RedundantPhrases, 3, 4, 4, 4)

bp:SelectWindow(SideBarSection.Grammar, ListType.RepeatedWords)
ScreenshotLib.SnapScreenshotOfListControl(ImagePath .. "batchrepeatedmultiple." .. FileExtension,
    ListType.RepeatedWords, 2, 4, 2, 4)

bp:SelectWindow(SideBarSection.Grammar, ListType.WordyItems)
ScreenshotLib.SnapScreenshotOfListControl(ImagePath .. "batchwordyphrasesmultiple." .. FileExtension,
    ListType.WordyItems, 2, 4, 6, 4, 10)

bp:Close()

-- Standard project (Peter Rabbit)
----------------------------------
sp = StandardProject(ScreenshotProjectsFolder .. "The Tale of Peter Rabbit.rsp")
sp:OpenProperties(OptionsPageType.AnalysisDocumentIndexing)
ScreenshotLib.SnapScreenshot(ImagePath .. "IgnoreBlankLines." .. FileExtension, 1042, 1043)
sp:CloseProperties()

sp:SelectWindow(SideBarSection.WordsBreakdown, HighlightedReportType.ThreePlusSyllableHighlightedWords)
ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "PeterRabbitChainedSentences." .. FileExtension,
  HighlightedReportType.ThreePlusSyllableHighlightedWords, true,
  1046, 1194)

sp:Close()
Application.RemoveAllCustomTests()

sp = StandardProject(ScreenshotProjectsFolder .. "The Tale of Peter Rabbit(Split Sentences).rsp")
sp:ScrollTextWindow(HighlightedReportType.ThreePlusSyllableHighlightedWords, 1200)
ScreenshotLib.SnapScreenshot(ImagePath .. "PeterRabbitSplitSentences." .. FileExtension)

sp:Close()
Application.RemoveAllCustomTests()

sp = StandardProject(ScreenshotProjectsFolder .. "Crawford Sample.rsp")
sp:ExportGraph(GraphType.Crawford, ImagePath .. "CrawfordGraph." .. FileExtension)
sp:Close()
Application.RemoveAllCustomTests()

a = StandardProject(ScreenshotProjectsFolder .. "turkey brining.rsp")

a:ExportGraph(GraphType.WordBarChart, ImagePath .. "barchart." .. FileExtension)
a:ExportGraph(GraphType.SentenceBoxPlox, ImagePath .. "SentencesBoxplot." .. FileExtension)
a:ExportGraph(GraphType.SentenceHistogram, ImagePath .. "SentencesHistogram." .. FileExtension)
a:SelectWindow(SideBarSection.SentencesBreakdown, GraphType.SentenceBoxPlox)
ScreenshotLib.SnapScreenshotOfRibbon(ImagePath .. "RibbonEditGraphs." .. FileExtension, 0, RibbonButtonBars.Edit)
a:SelectWindow(SideBarSection.WordsBreakdown, GraphType.WordBarChart)
ScreenshotLib.SnapScreenshotOfRibbon(ImagePath .. "RibbonEditBarCharts." .. FileExtension, 0, RibbonButtonBars.Edit)

a:ExportGraph(GraphType.SyllableHistogram, ImagePath .. "SyllableCountHistogram." .. FileExtension)
a:AddTest(Tests.DanielsonBryan2)
a:ExportGraph(GraphType.DanielsonBryan2, ImagePath .. "DB2Plot." .. FileExtension)
a:ExportGraph(GraphType.Fry, ImagePath .. "frygraph." .. FileExtension)
a:ExportGraph(GraphType.Raygor, ImagePath .. "raygorgraph." .. FileExtension)
a:ExportGraph(GraphType.Flesch, ImagePath .. "FleschChart." .. FileExtension)
a:ExportGraph(GraphType.Lix, ImagePath .. "LixGauge." .. FileExtension)
a:Close()

-- German graph
a = StandardProject(ScreenshotProjectsFolder .. "turkey brining.rtf")
a:SetProjectLanguage(Language.German)
a:AddTest(Tests.Schwartz)
a:AddTest(Tests.LixGermanTechnical)
a:ExportGraph(GraphType.Schwartz, ImagePath .. "Schwartz." .. FileExtension)
a:ExportGraph(GraphType.GermanLix, ImagePath .. "GermanLixGauge." .. FileExtension)
a:Close()

a = StandardProject(ScreenshotProjectsFolder .. "BadRaygorScore.txt")
a:AddTest("raygor-test")
a:ExportGraph(GraphType.Raygor, ImagePath .. "raygorbadgrade." .. FileExtension)
a:Close()
Application.RemoveAllCustomTests()

a = StandardProject(ScreenshotProjectsFolder .. "GPM graph.rsp")
a:ExportGraph(GraphType.GpmFry, ImagePath .. "GpmFry." .. FileExtension)
a:Close()
Application.RemoveAllCustomTests()

-- Custom test example images
ScreenshotLib.ShowCustomTestDialogGeneralSettings("Flesch-Kincaid (unclamped)", "(11.8*(B/W)) + (.39*(W/S)) - 15.59", TestType.GradeLevel)
ScreenshotLib.SnapScreenshot(ImagePath .. "CustomTestFKUnclamped." .. FileExtension)

ScreenshotLib.ShowCustomTestDialogGeneralSettings("Flesch (IC)", "ROUND(206.835 - (84.6*(B/W)) - (1.015*(W/S)))", TestType.IndexValue)
ScreenshotLib.SnapScreenshot(ImagePath .. "CustomTestExample1TestName." .. FileExtension)

ScreenshotLib.ShowCustomTestDialogGeneralSettings("Flesch (IC)", "ROUND(206.835 -\n(84.6*(SyllableCount()/WordCount())) -\n(1.015*(WordCount()/IndependentClauseCount())))", TestType.IndexValue)
ScreenshotLib.SnapScreenshot(ImagePath .. "CustomTestExample1Finished." .. FileExtension)

ScreenshotLib.CloseCustomTestDialog()

a = StandardProject(ScreenshotProjectsFolder .. "FleschIC.rsp")
a:SelectReadabilityTest(1)
ScreenshotLib.SnapScreenshot(ImagePath .. "CustomTestExample1Score." .. FileExtension)
a:Close()
Application.RemoveAllCustomTests()

-- article mismating
sp = StandardProject(ScreenshotProjectsFolder .. "Eula.rtf")
sp:SelectWindow(SideBarSection.Grammar, ListType.ArticleMismatch)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "articlemismatches." .. FileExtension, 1)
sp:Close()

Application.RemoveAllCustomTests()
bp = BatchProject(ScreenshotProjectsFolder .. "Eula.rsbp")
bp:SelectWindow(SideBarSection.Grammar, ListType.ArticleMismatch)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "batcharticlemismatches." .. FileExtension, 2)
bp:Close()

Application.RemoveAllCustomTests()

-- Custom test example 2
------------------------
ScreenshotLib.ShowCustomTestDialogGeneralSettings("New Dale-Chall (Baking)")
ScreenshotLib.SnapScreenshot(ImagePath .. "CustomTestExample2Name." .. FileExtension, 7000)

ScreenshotLib.ShowCustomTestDialogGeneralSettings("New Dale-Chall (Baking)", "CustomNewDaleChall()")
ScreenshotLib.SnapScreenshot(ImagePath .. "CustomTestExample2FormulaFinished." .. FileExtension, 7002)

ScreenshotLib.ShowCustomTestDialogFamiliarWords("/home/Niles/CookingWords.txt")
ScreenshotLib.SnapScreenshotOfPropertGrid(ImagePath .. "CustomTestExample2WordLoaded." .. FileExtension, -1, "File containing familiar words")

ScreenshotLib.ShowCustomTestDialogFamiliarWords("/home/Niles/CookingWords.txt", StemmingType.English, true)
ScreenshotLib.SnapScreenshot(ImagePath .. "CustomTestExample2WordSettingsFinished." .. FileExtension)

ScreenshotLib.CloseCustomTestDialog()

-- Custom index Test example
----------------------------
ScreenshotLib.ShowCustomTestDialogGeneralSettings("Buzz Index")
ScreenshotLib.SnapScreenshot(ImagePath .. "CustomIndexTestExampleName." .. FileExtension, 7000)

ScreenshotLib.ShowCustomTestDialogGeneralSettings("Buzz Index", "", TestType.IndexValue)
ScreenshotLib.SnapScreenshot(ImagePath .. "CustomIndexTestExampleTestType." .. FileExtension, 7001)

ScreenshotLib.ShowCustomTestDialogGeneralSettings("Buzz Index", "IF((FamiliarWordCount()/WordCount())*100 > 15, 3,\n  IF((FamiliarWordCount()/WordCount())*100 > 5, 2, 1) )", TestType.IndexValue)
ScreenshotLib.SnapScreenshot(ImagePath .. "CustomIndexTestExampleFormula." .. FileExtension, 7002)

ScreenshotLib.ShowCustomTestDialogFamiliarWords("C:\\Users\\Roz\\WordLists\\Buzz Words.txt")
ScreenshotLib.SnapScreenshotOfPropertGrid(ImagePath .. "CustomIndexTestExampleWordsLoaded." .. FileExtension, -1, "File containing familiar words")

ScreenshotLib.ShowCustomTestDialogFamiliarWords("C:\\Users\\Roz\\WordLists\\Buzz Words.txt", StemmingType.English)
ScreenshotLib.SnapScreenshot(ImagePath .. "CustomIndexTestExampleWordSettingsFinished." .. FileExtension)

ScreenshotLib.ShowCustomTestDialogProperNounsAndNumbers(0, false)
ScreenshotLib.SnapScreenshot(ImagePath .. "CustomIndexTestExampleProperAndNumeralsFinished." .. FileExtension)

ScreenshotLib.CloseCustomTestDialog()

sp = StandardProject(ScreenshotProjectsFolder .. "Press Release.rsp")
sp:SelectReadabilityTest(1)
ScreenshotLib.SnapScreenshot(ImagePath .. "CustomIndexTestExampleScore." .. FileExtension)
sp:Close()

-- Custom word example 2
------------------------
a = StandardProject(ScreenshotProjectsFolder .. "Chocolate Eclairs.rsp")
a:SelectReadabilityTest(2)
ScreenshotLib.SnapScreenshot(ImagePath .. "CustomTestExample2Score." .. FileExtension)
ScreenshotLib.SnapScreenshotOfRibbon(ImagePath .. "RibbonEditScores." .. FileExtension, 0, RibbonButtonBars.Edit)

a:SelectHighlightedWordReport(Application.GetTestId("New Dale-Chall (Baking)"))
ScreenshotLib.SnapScreenshot(ImagePath .. "CustomTestExample2CustomText." .. FileExtension)
ScreenshotLib.SnapScreenshotOfRibbon(ImagePath .. "RibbonEditTextWindow." .. FileExtension, 0, RibbonButtonBars.Edit)

a:SelectWindow(SideBarSection.WordsBreakdown, ListType.DaleChallUnfamiliarWords)
a:SortList(ListType.DaleChallUnfamiliarWordsList,1,SortOrder.SortAscending)
ScreenshotLib.SnapScreenshot(ImagePath .. "CustomTestExample2DCWordList." .. FileExtension)

a:SelectWindow(SideBarSection.WordsBreakdown, Application.GetTestId("New Dale-Chall (Baking)"))
a:SortList(Application.GetTestId("New Dale-Chall (Baking)"),1,SortOrder.SortAscending)
ScreenshotLib.SnapScreenshot(ImagePath .. "CustomTestExample2CustomWordList." .. FileExtension)

a:SelectWindow(SideBarSection.Statistics, ReportType.StatisticsSummaryReport)
ScreenshotLib.SnapScreenshotOfRibbon(ImagePath .. "RibbonEditStats." .. FileExtension, 0, RibbonButtonBars.Edit)

ScreenshotLib.ShowFilteredTextPreviewDlg()
ScreenshotLib.SnapScreenshot(ImagePath .. "FilterTextPreview." .. FileExtension)
ScreenshotLib.CloseFilteredTextPreviewDlg()

a:Close()
Application.RemoveAllCustomTests()

a = StandardProject(ScreenshotProjectsFolder .. "Frase.rsp")
a:ExportGraph(GraphType.Frase, ImagePath .. "FraseGraph." .. FileExtension)
a:Close()
Application.RemoveAllCustomTests()

ScreenshotLib.ShowSortListDlg("Word", "Syllable Count", "Frequency")
ScreenshotLib.SnapScreenshot(ImagePath .. "sortcolumnsdialog." .. FileExtension)
ScreenshotLib.CloseSortListDlg()

-- Text exclusion example
-------------------------
sp = StandardProject(ScreenshotProjectsFolder .. "Danielson-Bryan.html")

sp:ExcludeFileAddress(false)
sp:ExcludeProperNouns(false)
sp:AggressivelyExclude(false)
sp:ExcludeCopyrightNotices(false)
sp:ExcludeTrailingCitations(false)
sp:ExcludeNumerals(false)

sp:Reload()

sp:SelectWindow(SideBarSection.Grammar, HighlightedReportType.GrammarHighlightedIssues)
ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "ExclusionExampleHeaderExcluded." .. FileExtension,
HighlightedReportType.GrammarHighlightedIssues, true, 229, 252)

ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "ExclusionExamplePartialExcluded." .. FileExtension,
HighlightedReportType.GrammarHighlightedIssues, true, 929, 934, 999, 1133)

sp:OpenProperties(OptionsPageType.AnalysisDocumentIndexing)

ScreenshotLib.SnapScreenshot(ImagePath .. "ExclusionExampleAllExcluded." .. FileExtension, 1051, 1058)

sp:CloseProperties()

sp:AggressivelyExclude(true)

sp:Reload()

sp:OpenProperties(OptionsPageType.AnalysisDocumentIndexing)

ScreenshotLib.SnapScreenshot(ImagePath .. "ExclusionExampleAggressiveList." .. FileExtension, 1051)

sp:CloseProperties()

ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "ExclusionExampleWhereExcluded." .. FileExtension,
HighlightedReportType.GrammarHighlightedIssues, true, 929, 934)

sp:SetIncludeIncompleteTolerance(20)

sp:Reload()

sp:OpenProperties(OptionsPageType.AnalysisDocumentIndexing)

ScreenshotLib.SnapScreenshot(ImagePath .. "ExclusionExampleIncompLengthValue." .. FileExtension, 1076, 1077)

sp:CloseProperties()

ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "ExclusionExampleLongSentenceExcluded." .. FileExtension,
  HighlightedReportType.GrammarHighlightedIssues, true, 989, 1123)

sp:ScrollTextWindow(HighlightedReportType.GrammarHighlightedIssues, 2000)

ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "ExclusionExampleCopyrightIncluded." .. FileExtension,
  HighlightedReportType.GrammarHighlightedIssues, true, 2195, 2240)

sp:ExcludeCopyrightNotices(true)

sp:Reload()

sp:OpenProperties(OptionsPageType.AnalysisDocumentIndexing)

ScreenshotLib.SnapScreenshot(ImagePath .. "ExclusionExampleCopyrightExcluded." .. FileExtension, 1054)

sp:CloseProperties()

sp:SelectWindow(SideBarSection.Grammar, HighlightedReportType.GrammarHighlightedIssues)
ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "ExclusionExampleCopyrightExcludedNow." .. FileExtension,
  HighlightedReportType.GrammarHighlightedIssues, true, 2195, 2240)

sp:ExcludeFileAddress(true)

sp:Reload()

sp:OpenProperties(OptionsPageType.AnalysisDocumentIndexing)

ScreenshotLib.SnapScreenshot(ImagePath .. "ExclusionExampleUrlExcluded." .. FileExtension, 1056)

sp:CloseProperties()

sp:SelectWindow(SideBarSection.Grammar, HighlightedReportType.GrammarHighlightedIssues)
ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "ExclusionExampleUrlExcludedNow." .. FileExtension,
    HighlightedReportType.GrammarHighlightedIssues, true, 2162, 2183)

sp:ExcludeProperNouns(true)
sp:ExcludeNumerals(true)

sp:Reload()

ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "ExclusionExampleNumbersExcludedNow." .. FileExtension,
    HighlightedReportType.GrammarHighlightedIssues, true, 281, 284, 539, 543)

ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "ExclusionExampleProperExcludedNow." .. FileExtension,
HighlightedReportType.GrammarHighlightedIssues, true, 262, 277, 532, 538)

sp:OpenProperties(OptionsPageType.AnalysisDocumentIndexing)

ScreenshotLib.SnapScreenshot(ImagePath .. "ExclusionExampleEditListButton." .. FileExtension, 1052)

sp:CloseProperties()

-- Exclusion word list
----------------------
ScreenshotLib.ShowEditWordListDlg(ScreenshotProjectsFolder .. "ExclusionWords.txt")
ScreenshotLib.SnapScreenshotWithAnnotation(ImagePath .. "ExclusionExampleCustomWords." .. FileExtension, "ExclusionWords.txt", 9000)
ScreenshotLib.CloseEditWordListDlg()

sp:SetPhraseExclusionList(ScreenshotProjectsFolder .. "ExclusionWords.txt")

sp:Reload()

sp:SelectWindow(SideBarSection.Grammar, HighlightedReportType.GrammarHighlightedIssues)
ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "ExclusionExampleCustomWordsExcludedNow." .. FileExtension,
    HighlightedReportType.GrammarHighlightedIssues, true, 371, 386, 619, 630)

sp:ExcludeTrailingCitations(true)

sp:Reload()

sp:SelectWindow(SideBarSection.Grammar, HighlightedReportType.GrammarHighlightedIssues)
ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "ExclusionExampleCitationExcluded." .. FileExtension,
HighlightedReportType.GrammarHighlightedIssues, true, 1926, 2090)

sp:SelectWindow(SideBarSection.WordsBreakdown, ListType.ThreePlusSyllableWords)
ScreenshotLib.SnapScreenshotOfRibbon(ImagePath .. "RibbonEditListOptionsWithExcludeSelected." .. FileExtension, 0, RibbonButtonBars.Edit)

sp:Close()

-- exlcuded tags example
------------------------
sp = StandardProject(ScreenshotProjectsFolder .. "Press Release.rtf")

sp:SelectWindow(SideBarSection.Grammar, HighlightedReportType.GrammarHighlightedIssues)
ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "ExclusionTagsExampleListExcluded." .. FileExtension,
HighlightedReportType.GrammarHighlightedIssues, true, 1318, 1408)

sp:SelectWindow(SideBarSection.Grammar, HighlightedReportType.GrammarHighlightedIssues)
ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "ExclusionTagsExampleNotExcluding." .. FileExtension,
HighlightedReportType.GrammarHighlightedIssues, true,
    865, 874, 892, 903, 1415, 1530)

sp:SetBlockExclusionTags("<>")
sp:Reload()

ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "ExclusionTagsExampleExcluding." .. FileExtension,
HighlightedReportType.GrammarHighlightedIssues, true,
    865, 872, 892, 901, 1414, 1526)

sp:OpenProperties(OptionsPageType.AnalysisDocumentIndexing)
ScreenshotLib.SnapScreenshot(ImagePath .. "ExclusionTagsExampleOptions." .. FileExtension, 1078, 1079)
sp:CloseProperties()

sp:Close(false)

-- sentence map example
-----------------------
sp = StandardProject(ScreenshotProjectsFolder .. "Importing and Exporting Fixed-Width Data with R.docx")
sp:SetParagraphsParsingMethod(ParagraphParse.EachNewLineIsAParagraph)
sp:ExportGraph(GraphType.SentenceHeatmap, ImagePath .. "FixedWithRSentenceMap." .. FileExtension)
sp:Close()

sp = StandardProject(ScreenshotProjectsFolder .. "Instructional Disclaimer.odt")
sp:SetParagraphsParsingMethod(ParagraphParse.EachNewLineIsAParagraph)
sp:ExportGraph(GraphType.SentenceHeatmap, ImagePath .. "DisclaimerSentenceMap." .. FileExtension)
sp:Close()

sp = StandardProject(ScreenshotProjectsFolder .. "A Christmas Carol.txt")
sp:SetParagraphsParsingMethod(ParagraphParse.EachNewLineIsAParagraph)
sp:ExportGraph(GraphType.SentenceHeatmap, ImagePath .. "ChristmasCarolSentenceMap." .. FileExtension)
sp:Close()

-- hard returns example
-----------------------
sp = StandardProject(ScreenshotProjectsFolder .. "Job Posting.odt")
sp:SetParagraphsParsingMethod(ParagraphParse.EachNewLineIsAParagraph)
sp:SelectWindow(SideBarSection.Grammar, HighlightedReportType.GrammarHighlightedIssues)
ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "JobPostingHighlightedText." .. FileExtension,
    HighlightedReportType.GrammarHighlightedIssues, true, 257, 399)
sp:Close(false)

-- Flyer example
----------------
sp = StandardProject(ScreenshotProjectsFolder .. "Summer Code Camp.odt")
sp:SetTextExclusion(TextExclusionType.DoNotExcludeAnyText)
sp:AddTest(Tests.Forcast)
sp:SelectWindow(SideBarSection.WordsBreakdown, HighlightedReportType.ThreePlusSyllableHighlightedWords)
ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "SummerCodeCampUpperHalf1." .. FileExtension,
    HighlightedReportType.ThreePlusSyllableHighlightedWords, true, 111, 168)
ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "SummerCodeCampUpperHalf2." .. FileExtension,
    HighlightedReportType.ThreePlusSyllableHighlightedWords, true, 472, 525)
ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "SummerCodeCampLowerHalf." .. FileExtension,
    HighlightedReportType.ThreePlusSyllableHighlightedWords, true, 640, 893)
sp:Close(false)

-- addendum example
-------------------
ScreenshotLib.ShowStandardProjectWizardManualTestSelectionPage("Flesch Reading Ease")
ScreenshotLib.SnapScreenshot(ImagePath .. "AddendumWizardManualTests." .. FileExtension)
ScreenshotLib.CloseStandardProjectWizard()

sp = StandardProject(ScreenshotProjectsFolder .. "YA Enterprise Software Symposium.odt")
sp:SetTextExclusion(TextExclusionType.ExcludeIncompleteSentences)
sp:AddTest(Tests.Flesch)
sp:ExportGraph(GraphType.Flesch, ImagePath .. "AddendumFlesch1." .. FileExtension)
sp:SelectWindow(SideBarSection.WordsBreakdown, HighlightedReportType.ThreePlusSyllableHighlightedWords)
ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "AddendumHighlightedText1." .. FileExtension,
    HighlightedReportType.ThreePlusSyllableHighlightedWords, false, 350, 452)
ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "AddendumHighlightedText2." .. FileExtension,
    HighlightedReportType.ThreePlusSyllableHighlightedWords, false, 790, 1122)

sp:SetAppendedDocumentFilePath(ScreenshotProjectsFolder .. "Instructional Disclaimer.odt")

sp:ExportGraph(GraphType.Flesch, ImagePath .. "AddendumFlesch2." .. FileExtension)
sp:SelectWindow(SideBarSection.WordsBreakdown, HighlightedReportType.ThreePlusSyllableHighlightedWords)
ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "AddendumHighlightedText3." .. FileExtension,
    HighlightedReportType.ThreePlusSyllableHighlightedWords, true, 3401, 4359)
sp:SelectWindow(SideBarSection.Grammar, HighlightedReportType.GrammarHighlightedIssues)
ScreenshotLib.SnapScreenshotOfTextWindow(ImagePath .. "AddendumHighlightedText4." .. FileExtension,
    HighlightedReportType.GrammarHighlightedIssues, true, 4434, 5218)

-- show a generic set of file paths in the properties dialog and get a screenshot of that
sp:DelayReloading(true)
sp:SetReviewer("Daphne")
sp:SetDocumentFilePath("/home/dmoon/YA Enterprise Software Symposium.odt")
sp:SetAppendedDocumentFilePath("/home/dmoon/Instructional Disclaimer.odt")
sp:OpenProperties(OptionsPageType.ProjectSettings)
ScreenshotLib.SnapScreenshot(ImagePath .. "AddendumProperties." .. FileExtension, 1083, 1080)
sp:CloseProperties()
sp:Close()

-- batch labeling examples
--------------------------
bp = BatchProject(ScreenshotProjectsFolder .. "Statistics Manual Ch. 1.rsbp")
bp:ShowSidebar(false)
bp:SelectWindow(SideBarSection.ReadabilityScores, ListType.BatchRawScores)
ScreenshotLib.SnapScreenshotOfListControl(ImagePath .. "BatchDescLabelsScores." .. FileExtension,
    ListType.BatchRawScores, -1, 2, -1, 2)
bp:Close()

bp = BatchProject(ScreenshotProjectsFolder .. "Topics(Grouped).rsbp")
bp:ShowSidebar(false)
bp:ExportGraph(SideBarSection.ReadabilityScores, GraphType.Fry, ImagePath .. "FryGrouped." .. FileExtension)
bp:ExportGraph(SideBarSection.BoxPlots, Application.GetTestId("fry-test"), ImagePath .. "boxgrouped." .. FileExtension)
bp:SelectWindow(SideBarSection.ReadabilityScores, ListType.BatchRawScores)
bp:SortList(ListType.BatchRawScores, 3, SortAscending)
ScreenshotLib.SnapScreenshotOfListControl(ImagePath .. "BatchGroupLabelsScores." .. FileExtension,
    ListType.BatchRawScores, -1, 2, -1, 4)
bp:Close()

ScreenshotLib.ShowBatchProjectWizardTextSourcePage(
            Application.GetAbsoluteFilePath(
            Debug.GetScriptFolderPath(),
            ScreenshotProjectsFolder .. "Statistics Manual Ch. 1"))
ScreenshotLib.SnapScreenshot(ImagePath .. "wizardbatchselectfilessamenames." .. FileExtension)
ScreenshotLib.CloseBatchProjectWizard()

ScreenshotLib.ShowDocGroupSelectDlg(1)
ScreenshotLib.SnapScreenshot(ImagePath .. "GroupLabelDescSelected." .. FileExtension)
ScreenshotLib.CloseDocGroupSelectDlg()

ScreenshotLib.ShowDocGroupSelectDlg(2, "Examples")
ScreenshotLib.SnapScreenshot(ImagePath .. "GroupLabelExamples." .. FileExtension)
ScreenshotLib.CloseDocGroupSelectDlg()

ScreenshotLib.ShowDocGroupSelectDlg(2, "Notes")
ScreenshotLib.SnapScreenshot(ImagePath .. "GroupLabelNotes." .. FileExtension)
ScreenshotLib.CloseDocGroupSelectDlg()

-- Graph editing example (selective colorization)
-------------------------------------------------
sp = StandardProject(ScreenshotProjectsFolder .. "Chocolate Eclairs.txt")

sp:SelectWindow(SideBarSection.WordsBreakdown, GraphType.WordBarChart)

-- remove the bar labels
sp:DisplayBarChartLabels(false)

-- change bar chart effects
sp:SetGraphCommonImage(ScreenshotProjectsFolder .. "bookstore.png")
sp:SetBarChartBarEffect(BoxEffect.CommonImage)
sp:Reload()
sp:ExportGraph(GraphType.WordBarChart, ImagePath .. "ExampleBarChartCommonImage." .. FileExtension)

-- change the background to an image
sp:SetPlotBackgroundImage(ScreenshotProjectsFolder .. "bookstore.png")
sp:SetPlotBackgroundImageFit(ImageFit.CropAndCenter)
sp:Reload()
sp:ExportGraph(GraphType.WordBarChart, ImagePath .. "ExampleGraphBackgroundImageForCommonImage." .. FileExtension)

-- apply an effect to the background image
sp:SetPlotBackgroundImageEffect(ImageEffect.Grayscale)
sp:SetPlotBackgroundImageOpacity(100)
sp:Reload()
sp:ExportGraph(GraphType.WordBarChart, ImagePath .. "ExampleGraphBackgroundImageEffectGrayscaled." .. FileExtension)

sp:Close()

-- Graph editing example
------------------------
sp = StandardProject(ScreenshotProjectsFolder .. "Chocolate Eclairs.txt")

sp:SelectWindow(SideBarSection.WordsBreakdown, GraphType.WordBarChart)
sp:ExportGraph(GraphType.WordBarChart, ImagePath .. "ExampleGraphDefault." .. FileExtension)

-- sort the graph
sp:SortGraph(GraphType.WordBarChart, SortOrder.SortAscending)
sp:ExportGraph(GraphType.WordBarChart, ImagePath .. "ExampleGraphSorted." .. FileExtension)

-- change orientation
sp:SetBarChartOrientation(Orientation.Vertical)
sp:Reload()
sp:ExportGraph(GraphType.WordBarChart, ImagePath .. "ExampleBarChartOrientationChanged." .. FileExtension)

-- change bar chart effects
sp:SetBarChartBarEffect(BoxEffect.ThickWatercolor)
sp:Reload()
sp:ExportGraph(GraphType.WordBarChart, ImagePath .. "ExampleBarChartEffectsChanged." .. FileExtension)

-- remove the bar labels
sp:DisplayBarChartLabels(false)
sp:Reload()
sp:ExportGraph(GraphType.WordBarChart, ImagePath .. "ExampleBarChartNoLabelsChanged." .. FileExtension)

-- change color of background
sp:SetGraphBackgroundColor(250, 235, 215)
sp:ApplyGraphBackgroundFade(true)
sp:Reload()
sp:ExportGraph(GraphType.WordBarChart, ImagePath .. "ExampleGraphBackgroundColorChanged." .. FileExtension)

-- change the background to an image
sp:SetPlotBackgroundImage(ScreenshotProjectsFolder .. "bookstore.png")
sp:Reload()
sp:ExportGraph(GraphType.WordBarChart, ImagePath .. "ExampleGraphBackgroundImageChanged." .. FileExtension)

-- apply an effect to the background image
sp:SetPlotBackgroundImageEffect(ImageEffect.OilPainting)
sp:Reload()
sp:ExportGraph(GraphType.WordBarChart, ImagePath .. "ExampleGraphBackgroundImageEffectChanged." .. FileExtension)

-- add watermark
sp:SetGraphWatermark("INTERNAL USE ONLY\nProcessed on @DATE@")
sp:Reload()
sp:ExportGraph(GraphType.WordBarChart, ImagePath .. "ExampleWatermarkChanged." .. FileExtension)

-- add logo
sp:SetGraphLogoImage(ScreenshotProjectsFolder .. "CoffeeLogo.png")
sp:Reload()
sp:ExportGraph(GraphType.WordBarChart, ImagePath .. "ExampleLogoChanged." .. FileExtension)

sp:Close()

-- Website example
-------------------
sp = StandardProject(ScreenshotProjectsFolder .. "LibreOffice.rsp")
sp:ScrollTextWindow(HighlightedReportType.ThreePlusSyllableHighlightedWords, 1800)
ScreenshotLib.SnapScreenshotOfActiveProject(ImagePath .. "incompletesentences." .. FileExtension, 1)

sp:DelayReloading(true) -- prevent reloading website in case we are offline
sp:SetDocumentStorageMethod(TextStorage.NoEmbedText)
sp:OpenProperties(OptionsPageType.ProjectSettings)
ScreenshotLib.SnapScreenshot(ImagePath .. "reloadwebpage." .. FileExtension, 1085, -1, 1083)

sp:OpenProperties(OptionsPageType.AnalysisDocumentIndexing)
ScreenshotLib.SnapScreenshot(ImagePath .. "excludeincompletesentences." .. FileExtension, 1036, -1, 1084)
sp:CloseProperties()

sp:SetTextExclusion(TextExclusionType.DoNotExcludeAnyText)
sp:OpenProperties(OptionsPageType.AnalysisDocumentIndexing)
ScreenshotLib.SnapScreenshot(ImagePath .. "donotexcludeincompletesentences." .. FileExtension, 1036, -1, 1084)
sp:CloseProperties()

sp:Close()
Application.RemoveAllCustomTests()

-- rescale images and convert from BMP to PNG
bitmaps = Application.FindFiles(ImagePath, "*." .. FileExtension)

for i,v in ipairs(bitmaps)
do
    ScreenshotLib.ConvertImage(v, 1000, 1000)
end

ScreenshotLib.ShowScriptEditor()
