## ExportList {#standard-exportlist .unnumbered}

Saves the specified list as a HTML or text file to **OutputFilePath**.

```{sql}
ExportList(ListType,
           OutputFilePath,
           FromRow,
           ToRow,
           FromColumn,
           ToColumn,
           IncludeHeaders,
           IncludePageBreaks)
```

### Parameters {-}

**Parameter** | **Description**
| :-- | :-- |
ListType | The [list](#listtype) to export.
OutputFilePath | The folder to save the list.
FromRow | (Optional) Specifies the row to begin from. The default is the first row.
ToRow | (Optional) Specifies the last row. Enter -1 to specify the last row in the list.
FromColumn | (Optional) Specifies the row to column from. The default is the first column.
ToColumn | (Optional) Specifies the last column. Enter -1 to specify the last column in the list.
IncludeHeaders | (Optional) Specifies whether to include the column headers.
IncludePageBreaks | (Optional) Specifies whether to insert page breaks. Note that this is only applicable if exporting as HTML.

### Example {-}

```{sql}
-- Open a project from the user's "Documents" folder
sp = StandardProject(Application.GetDocumentsPath() .. "Consent Form.rsp")
-- Sort the "All Words" list.
-- Will sort by the third column (Z-A sorting), and in case of a tie, then sort
-- by the second then the first column.
sp:SortList(ListType.AllWords, 3, SortOrder.SortDescending,
            2, SortOrder.SortDescending,
            1, SortOrder.SortDescending)
-- export the list after sorting it
sp:ExportList(ListType.AllWords,
              Application.GetDocumentsPath() ..
              "ExportList/ChristmasCarolAllWordsListSortedDesc.htm")

sp:Close()
```