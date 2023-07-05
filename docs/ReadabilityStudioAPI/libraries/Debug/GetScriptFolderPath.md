:::: {.minipage data-latex="{\textwidth}"}
## GetScriptFolderPath {-}

Returns the folder path of the currently running script.

### Syntax {-}

```{sql}
string GetScriptFolderPath()
```

### Return value {-}

Type: **string**

The folder path of the currently running script.

::: {.warningsection data-latex=""}
This will return an empty string if the current script has not been saved yet.
:::
::::

***
