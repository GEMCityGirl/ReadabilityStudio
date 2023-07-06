:::: {.minipage data-latex="{\textwidth}"}
## AggressivelyDeduceLists {#standard-aggressivelydeducelists .unnumbered}

Specifies whether to use aggressive list deduction.

```{sql}
AggressivelyDeduceLists(BeAggressive)
```

### Parameters {-}

**Parameter** | **Type** | **Description**
| :-- | :-- | :-- |
BeAggressive | **boolean** |  Set to **true** to use aggressive list deduction.

::: {.tipsection data-latex=""}
This can be optimized by placing it in between calls to **[DelayReloading()](#standard-delayreloading)** and **[Reload()](#standard-reload)**. (Refer to the [standard project overview](#standard-project) for details.)
:::
::::

***

