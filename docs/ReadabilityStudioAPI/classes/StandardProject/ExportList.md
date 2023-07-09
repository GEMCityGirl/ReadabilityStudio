::: {.minipage data-latex="{\textwidth}"}
## ExportList {#standard-exportlist .unnumbered}

Saves\index{Lists!exporting} the specified list as an HTML, text, or LaTeX file to *OutputFilePath*.

### Syntax {-}

```{sql}
ExportList(ListType type,
           string outputFilePath,
           number fromRow,
           number toRow,
           number fromColumn,
           number toColumn,
           boolean includeHeaders,
           boolean IncludePageBreaks)
```

### Parameters {-}

**Parameter** | **Description**
| :-- | :-- |
type | The [list](#listtype) to export.
string outputFilePath | The folder to save the list.
fromRow | (Optional) Specifies the row to begin from. The default is the first row.
toRow | (Optional) Specifies the last row. Enter **-1** to specify the last row in the list.
fromColumn | (Optional) Specifies the row to column from. The default is the first column.
toColumn | (Optional) Specifies the last column. Enter **-1** to specify the last column in the list.
includeHeaders | (Optional) Specifies whether to include the column headers.
includePageBreaks | (Optional) Specifies whether to insert page breaks. Note that this is only applicable if exporting as HTML.

### Example {-}

```{sql}
-- Open a project from the user's "Documents" folder
consentForm = StandardProject(Application.GetDocumentsPath() ..
                              "Consent Form.rsp")
-- Sort the "All Words" list.
-- Will sort by the third column (Z-A sorting), and in case of a tie, then sort
-- by the second then the first column.
consentForm:SortList(ListType.AllWords,
            3, SortOrder.SortDescending,
            2, SortOrder.SortDescending,
            1, SortOrder.SortDescending)
-- export the list after sorting it
consentForm:ExportList(ListType.AllWords,
              Application.GetDocumentsPath() ..
              "ExportList/ConsentFormAllWordsListSortedDesc.htm")

consentForm:Close()
```
:::

***
