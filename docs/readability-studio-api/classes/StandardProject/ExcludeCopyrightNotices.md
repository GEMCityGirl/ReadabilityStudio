::: {.minipage data-latex="{\textwidth}"}
## ExcludeCopyrightNotices {#standard-excludecopyrightnotices .unnumbered}

Specifies whether to exclude copyright notices.

### Syntax {-}

```{sql}
ExcludeCopyrightNotices(boolean exclude)
```

### Parameters {-}

**Parameter** | **Description**
| :-- | :-- |
exclude | `true` to exclude copyright notices.
:::

::: {.tipsection data-latex=""}
This can be optimized by placing it in between calls to **[DelayReloading()](#standard-delayreloading)** and **[Reload()](#standard-reload)**.
:::

***
