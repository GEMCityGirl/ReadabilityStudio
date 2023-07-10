::: {.minipage data-latex="{\textwidth}"}
## SetAppendedDocumentFilePath {#batch-setappendeddocumentfilepath .unnumbered}

Specifies the file path to the document being appended for analysis.

### Syntax {-}

```{sql}
SetAppendedDocumentFilePath(string filePath)
```

### Parameters {-}

**Parameter** | **Description**
| :-- | :-- |
string flePath | The file path to the document being appended for analysis.

### Example {-}

```{sql}
-- Open a project from the user's "Documents" folder
consentForm = StandardProject(Application.GetDocumentsPath() ..
                              "Consent Form.docx")

-- Add an addendum to the document.
consentForm:SetAppendedDocumentFilePath(
    Application.GetDocumentsPath() ..
    "Addendum.docx")

consentForm:Close(false)
```
:::

::: {.tipsection data-latex=""}
This can be optimized by placing it in between calls to **[DelayReloading()](#standard-delayreloading)** and **[Reload()](#standard-reload)**.
:::
::::

***
