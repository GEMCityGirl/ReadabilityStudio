::: {.minipage data-latex="{\textwidth}"}
## GetAbsoluteFilePath {-}

Combines a base path with a relative path and returns the full file path.

### Syntax {-}

```{sql}
string GetAbsoluteFilePath(string basePath
                           string relativePath)
```

### Parameters {-}

**Parameter** | **Description**
| :-- | :-- |
basePath | The full base path.
relativePath | The path of the file that is relative to the base path.

### Return value {-}

Type: `string` \

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
:::

***