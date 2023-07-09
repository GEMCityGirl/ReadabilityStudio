::: {.minipage data-latex="{\textwidth}"}
## Close {#standard-close .unnumbered}

Closes the project.

### Syntax {-}

```{sql}
Close(boolean saveChanges)
```

### Parameters {-}

**Parameter** | **Description**
| :-- | :-- |
saveChanges | Specifies whether to save any changes made to the project. The default is **false**.
:::

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
