::: {.minipage data-latex="{\textwidth}"}
## ExcludeTrailingCitations {#standard-excludetrailingcitations .unnumbered}

Specifies whether to exclude trailing citations.

### Syntax {-}

```{sql}
ExcludeTrailingCitations(boolean exclude)
```

### Parameters {-}

**Parameter** | **Description**
| :-- | :-- |
exclude | `true` to exclude trailing citations.
:::

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
-- Reindex the document so that the changes take effect.
consentForm:Reload()

-- assuming we have a custom test by this name.
consentForm:AddTest("Patient Consent Formula")
```

::: {.tipsection data-latex=""}
This can be optimized by placing it in between calls to **[DelayReloading()](#standard-delayreloading)** and **[Reload()](#standard-reload)**.
:::
***
