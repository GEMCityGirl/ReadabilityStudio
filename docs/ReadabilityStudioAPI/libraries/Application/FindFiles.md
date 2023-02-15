## FindFiles {-}

Searches the specified folder (recursively) for files matching the given file filter (e.g., "*.docx") and returns the retrieved filepaths.

```{sql}
table FindFiles(FolderPath,FileFilter)
```

### Parameters {-}

Parameter | Description
| :-- | :-- |
FolderPath | The folder to search in.
FileFilter | The file pattern to search for.

### Return Value {-}

A table listing the files.