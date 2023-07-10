::: {.minipage data-latex="{\textwidth}"}
## ExcludeFileAddress {#standard-excludefileaddress .unnumbered}

Specifies whether to exclude filepaths and URLs.

### Syntax {-}

```{sql}
ExcludeFileAddress(boolean exclude)
```

### Parameters {-}

**Parameter** | **Description**
| :-- | :-- |
exclude | **true** to exclude filepaths and URLs.
:::

::: {.tipsection data-latex=""}
This can be optimized by placing it in between calls to **[DelayReloading()](#standard-delayreloading)** and **[Reload()](#standard-reload)**.
:::

***
