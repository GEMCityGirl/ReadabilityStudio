::: {.minipage data-latex="{\textwidth}"}
## ExportReport {#standard-exportreport .unnumbered}

Saves\index{Reports!exporting} the specified report.

### Syntax {-}

```{sql}
ExportReport(ReportType type,
             string outputFilePath) 
```

### Parameters {-}

**Parameter** | **Description**
| :-- | :-- |
type | The [report](#reporttype) to save.
outputFilePath | The folder to save the scores.

### Example {-}

```{sql}
-- Open a project from the user's "Documents" folder and save its scores summary
consentForm = StandardProject(Application.GetDocumentsPath() ..
                              "Consent Form.rsp")
consentForm:ExportReport(ReportType.ReadabilityScoresTabularReport,
                Application.GetDocumentsPath() .. "Review/ConsentScoresSummary.htm")
-- Close, don't bother saving.
consentForm:Close(false)
```
:::

***
