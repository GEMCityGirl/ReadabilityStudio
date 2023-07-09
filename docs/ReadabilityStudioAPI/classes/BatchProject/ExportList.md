
## ExportList {#batch-exportlist .unnumbered}

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
