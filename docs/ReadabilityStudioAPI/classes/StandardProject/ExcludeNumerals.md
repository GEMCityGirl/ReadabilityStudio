::: {.minipage data-latex="{\textwidth}"}
## ExcludeNumerals {#standard-excludenumerals .unnumbered}

Specifies whether to exclude numerals.

### Syntax {-}

```{sql}
ExcludeNumerals(boolean exclude)
```

### Parameters {-}

**Parameter** | **Description**
| :-- | :-- |
exclude | **true** to exclude numerals.
:::

::: {.tipsection data-latex=""}
This can be optimized by placing it in between calls to **[DelayReloading()](#standard-delayreloading)** and **[Reload()](#standard-reload)**.
:::

***
