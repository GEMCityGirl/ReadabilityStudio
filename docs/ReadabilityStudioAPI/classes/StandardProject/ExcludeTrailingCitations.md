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
exclude | **true** to exclude trailing citations.
:::

::: {.tipsection data-latex=""}
This can be optimized by placing it in between calls to **[DelayReloading()](#standard-delayreloading)** and **[Reload()](#standard-reload)**.
:::

***
