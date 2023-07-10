::: {.minipage data-latex="{\textwidth}"}
## ExcludeProperNouns {#standard-excludepropernouns .unnumbered}

Specifies whether to exclude proper nouns.

### Syntax {-}

```{sql}
ExcludeProperNouns(boolean exclude)
```

### Parameters {-}

**Parameter** | **Description**
| :-- | :-- |
exclude | **true** to exclude proper nouns.
:::

::: {.tipsection data-latex=""}
This can be optimized by placing it in between calls to **[DelayReloading()](#standard-delayreloading)** and **[Reload()](#standard-reload)**.
:::

***
