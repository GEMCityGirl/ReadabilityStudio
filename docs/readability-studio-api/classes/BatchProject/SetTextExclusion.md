## SetTextExclusion {#batch-settextexclusion .unnumbered}

Specifies how text should be excluded while parsing the source document.

```{sql}
SetTextExclusion(TextExclusionType)
```

### Parameters {-}

**Parameter** | **Description**
| :-- | :-- |
TextExclusionType | With [text exclusion method](#textexclusiontype) to use for the project.

::: {.tipsection data-latex=""}
This can be optimized by placing it in between calls to **[DelayReloading()](#standard-delayreloading)** and **[Reload()](#standard-reload)**.
:::