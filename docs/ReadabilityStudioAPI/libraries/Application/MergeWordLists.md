::: {.minipage data-latex="{\textwidth}"}
## MergeWordLists {-}

Merges multiple word lists into an output file.

### Syntax {-}

```{sql}
boolean MergeWordLists(string outputFilePath,
                       string input1,
                       string input2,
                       ...)
```

### Parameters {-}

**Parameter** | **Description**
| :-- | :-- |
outputFilePath | The output filepaths to merge the files into.
input1,input2,... | A list of files to be merged into *outputFilePath*.

### Return value {-}

Type: **boolean** \

Returns **true** if files were successfully merged.
:::
***
