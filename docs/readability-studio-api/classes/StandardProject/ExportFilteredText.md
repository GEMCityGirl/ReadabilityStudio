::: {.minipage data-latex="{\textwidth}"}
## ExportFilteredText {#standard-exportfilteredtext .unnumbered}

Saves a copy of the project's document with excluded text (and other optional items) filtered out. Returns `true` if successful.

### Syntax {-}

```{sql}
ExportFilteredText(string outputFilePath,
                   boolean romanizeText,
                   boolean removeEllipses,
                   boolean removeBullets,
                   boolean removeFilePaths,
                   boolean stripAbbreviations)
```

### Parameters {-}

**Parameter** | **Description**
| :-- | :-- |
outputFilePath | The folder to save the filtered document.
romanizeText | Whether to converted extended ASCII characters to simpler versions (e.g., replacing smart quotes with straight quotes)
removeEllipses | Whether to remove ellipses.
removeBullets | Whether to remove bullets.
removeFilePaths | Whether to remove filepaths.
stripAbbreviations | Whether to remove the trailing periods off of abbreviations.
:::

***
