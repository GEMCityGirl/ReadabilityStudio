## GetAbsoluteFilePath {-}

Combines a base path with a relative path and returns the full file path.

```{sql}
string GetAbsoluteFilePath()
```

### Parameters {-}

Parameter | Description
| :-- | :-- |
BasePath | The full base path.
RelativePath | The path of the file that is relative to the base path.

### Return value {-}

The full file path.

### Example {-}

```{sql}
-- Assuming that there is a file "ProfileReport.txt"
-- in the folder "Mizzie/Documents" 2 levels up from
-- where the current script is running, this will print
-- the full path of where "ProfileReport.txt" is at.
Debug.Print(
    Application.GetAbsoluteFilePath(
    Debug.GetScriptFolderPath(),
    "../../Mizzie/Documents/ProfileReport.txt"))
```