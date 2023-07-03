## GetScriptFolderPath {-}

Returns the folder path of the currently running script.

```{sql}
string GetScriptFolderPath()
```

::: {.warningsection data-latex=""}
This will return an empty string if the current script has not been saved yet.
:::
