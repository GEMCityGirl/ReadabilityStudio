::: {.minipage data-latex="{\textwidth}"}
## FindFiles {-}

Searches the specified folder (recursively) for files matching the given file filter (e.g., "*.docx") and returns the retrieved filepaths.

### Syntax {-}

```{sql}
table FindFiles(FolderPath,FileFilter)
```

### Parameters {-}

**Parameter** | **Type** | **Description**
| :-- | :-- | :-- |
FolderPath | **string** | The folder to search in.
FileFilter | **string** | The file pattern to search for.

### Return value {-}

Type: **table**

A table listing the files.
:::

***
