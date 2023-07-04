## ExportFilteredText {#standard-exportfilteredtext .unnumbered}

Saves a copy of the project's document with excluded text (and other optional items) filtered out. Returns true if successful.

```{sql}
ExportFilteredText(OutputFilePath,
                   RomanizeText,
                   RemoveEllipses,
                   RemoveBullets,
                   RemoveFilePaths,
                   StripAbbreviations)
```

### Parameters {-}

**Parameter** | **Description**
| :-- | :-- |
OutputFilePath | The folder to save the filtered document.
RomanizeText | Whether to converted extended ASCII characters to simpler versions (e.g., replacing smart quotes with straight quotes)
RemoveEllipses | Whether to remove ellipses.
RemoveFilePaths | Whether to remove filepaths.
StripAbbreviations | Whether to remove the trailing periods off of abbreviations.
