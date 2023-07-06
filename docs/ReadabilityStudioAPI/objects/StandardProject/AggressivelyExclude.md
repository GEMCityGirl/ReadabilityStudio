:::: {.minipage data-latex="{\textwidth}"}
## AggressivelyExclude {#standard-aggressivelyexclude .unnumbered}

Specifies whether to use aggressive list deduction.

```{sql}
AggressivelyExclude(BeAggressive)
```

### Parameters {-}

**Parameter** | **Type** | **Description**
| :-- | :-- | :-- |
BeAggressive | **boolean** | Set to **true** to use aggressive text exclusion.

::: {.tipsection data-latex=""}
This can be optimized by placing it in between calls to **[DelayReloading()](#standard-delayreloading)** and **[Reload()](#standard-reload)**. (Refer to the [standard project overview](#standard-project) for details.)
:::
::::

***

