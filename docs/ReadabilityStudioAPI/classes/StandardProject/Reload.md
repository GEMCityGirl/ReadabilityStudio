::: {.minipage data-latex="{\textwidth}"}
## Reload {#standard-reload .unnumbered}

Reanalyzes the project's document.

### Syntax {-}

```{sql}
Reload()
```

### Example {-}

```{sql}
-- Opens a document from user's "Documents" folder
-- and changes a few options.
consentForm = StandardProject(Application.GetDocumentsPath() ..
                              "Consent Form.docx")

-- Delay reloading.
consentForm:DelayReloading(true)
-- Change a few options.
consentForm:AggressivelyExclude(true)
consentForm:ExcludeTrailingCitations(true)
consentForm:ExcludeNumerals(true)
-- Reindex the document so that the changes take effect.
consentForm:Reload()

-- assuming we have a custom test by this name.
consentForm:AddTest("Patient Consent Formula")
```

### See also {-}

[DelayReloading()](#standard-delayreloading)
:::

***
