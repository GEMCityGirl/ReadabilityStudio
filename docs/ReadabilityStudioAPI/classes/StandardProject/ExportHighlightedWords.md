::: {.minipage data-latex="{\textwidth}"}
## ExportHighlightedWords {#standard-exporthighlightedwords .unnumbered}

Saves\index{Highlighted reports!exporting} the specified highlighted words as an RTF or HTML report.

### Syntax {-}

```{sql}
ExportHighlightedWords(HighlightedWordReportType type,
                       string outputFilePath)
```

### Parameters {-}

**Parameter** | **Description**
| :-- | :-- |
type | The [highlighted text window](#highlightedreporttype) to save.
outputFilePath | The folder to save the scores.

::: {.notesection data-latex=""}
Use [Application.GetTestId()](#gettestid) to export the highlighted-words report for a custom test (see example below).
:::

### Example {-}

```{sql}
-- Open a project from the user's "Documents" folder
consentForm = StandardProject(Application.GetDocumentsPath() .. "Consent Form.rsp")
-- save a highlighted report of unfamiliar words from
-- a custom test named "Patient Test"
consentForm:ExportHighlightedWords(Application.GetTestId("Patient Test"),
                          Application.GetDocumentsPath() ..
                          "Review/ConsentUnfamiliarWords.htm")
-- as well as a highlighted report of 3+ syllable words
consentForm:ExportHighlightedWords(HighlightedReportType.ThreePlusSyllableHighlightedWords,
                          Application.GetDocumentsPath() ..
                          "Review/ConsentLongWords.htm")

consentForm:Close()
```
:::

***
