## ExportHighlightedWords {#standard-exporthighlightedwords .unnumbered}

Saves the specified highlighted words as an RTF or HTML report.

```{sql}
ExportHighlightedWords(HighlightedWordReportType, OutputFilePath)
```

### Parameters {-}

Parameter | Description
| :-- | :-- |
HighlightedWordReportType | The [highlighted text window](#highlightedreporttype) to save.
OutputFilePath | The folder to save the scores.

:::{.note}
Use [Application.GetTestId()](#gettestid) to export the highlighted-words report for a custom test (see example below).
:::

### Example {-}

```{sql}
-- Open a project from the user's "Documents" folder
sp = StandardProject(Application.GetDocumentsPath() .. "Consent Form.rsp")
-- save a highlighted report of unfamiliar words from a custom test named "Patient Test"
sp:ExportHighlightedWords(Application.GetTestId("Patient Test"),
                          Application.GetDocumentsPath() .. "Review/ConsentUnfamiliarWords.htm")
-- as well as a highlighted report of 3+ syllable words
sp:ExportHighlightedWords(HighlightedReportType.ThreePlusSyllableHighlightedWords,
                          Application.GetDocumentsPath() .. "Review/ConsentLongWords.htm")

sp:Close()
```