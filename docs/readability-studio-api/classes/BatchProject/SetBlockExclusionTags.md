## SetBlockExclusionTags {#batch-setblockexclusiontags .unnumbered}

Specifies a pair of tags that will exclude all text between them.

```{sql}
SetBlockExclusionTags(Tags)
```

### Parameters {-}

**Parameter** | **Description**
| :-- | :-- |
Tags | The pair of tags that will exclude all text between them.

::: {.notesection data-latex=""}
Pass in an empty string to turn off this option.
:::

::: {.tipsection data-latex=""}
This can be optimized by placing it in between calls to **[DelayReloading()](#standard-delayreloading)** and **[Reload()](#standard-reload)**.
:::
