::: {.minipage data-latex="{\textwidth}"}
## ExportReport {#standard-exportreport .unnumbered}

Saves the specified report.

### Syntax {-}

```{sql}
ExportReport(Report reportType,
             string outputFilePath) 
```

### Parameters {-}

**Parameter** | **Description**
| :-- | :-- |
reportType | The [report](#reporttype) to save.
outputFilePath | The folder to save the scores.

### Example {-}

```{sql}
-- Open a project from the user's "Documents" folder and save its scores summary
sp = StandardProject(Application.GetDocumentsPath() .. "Consent Form.rsp")
sp:ExportReport(ReportType.ReadabilityScoresTabularReport,
                Application.GetDocumentsPath() .. "Review/ConsentScoresSummary.htm")

sp:Close()
```
:::

***
