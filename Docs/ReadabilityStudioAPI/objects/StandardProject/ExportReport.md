## ExportReport {#standard-exportreport .unnumbered}

Saves the specified report.

```{sql}
ExportReport(ReportType, OutputFilePath) 
```

### Parameters {-}

Parameter | Description
| :-- | :-- |
ReportType | The [report](#reporttype) to save.
OutputFilePath | The folder to save the scores.

### Example {-}

```{sql}
-- Open a project from the user's "Documents" folder and save its scores summary
sp = StandardProject(Application.GetDocumentsPath() .. "Consent Form.rsp")
sp:ExportReport(ReportType.ReadabilityScoresTabularReport,
                Application.GetDocumentsPath() .. "Review/ConsentScoresSummary.htm")

sp:Close()
```