:::: {.minipage data-latex="{\textwidth}"}
## SetBlockExclusionTags {#batch-setblockexclusiontags .unnumbered}

Specifies a pair of tags that will exclude all text between them.

### Syntax {-}

```{sql}
SetBlockExclusionTags(string tags)
```

### Parameters {-}

**Parameter** | **Description**
| :-- | :-- |
tags | String containing a pair of tags that will have all text between them excluded.

::: {.notesection data-latex=""}
Pass in an empty string to turn off this option.
:::

::: {.tipsection data-latex=""}
This can be optimized by placing it in between calls to **[DelayReloading()](#standard-delayreloading)** and **[Reload()](#standard-reload)**.
:::
::::

***
