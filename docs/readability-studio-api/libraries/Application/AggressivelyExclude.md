::: {.minipage data-latex="{\textwidth}"}
## AggressivelyExclude {-}

Specifies whether to use aggressive text exclusion.

### Syntax {-}

```{sql}
AggressivelyExclude(boolean beAggressive)
```

### Parameters {-}

**Parameter** | **Description**
| :-- | :-- |
beAggressive | Specifies whether to use aggressive text exclusion.

### Example {-}

```{sql}
-- Change various program options.
Application.AggressivelyExclude(true)
Application.ExcludeCopyrightNotices(true)
Application.ExcludeFileAddress(true)
Application.ExcludeNumerals(true)
Application.ExcludeProperNouns(true)
Application.ExcludeTrailingCitations(true)
Application.EnableWarning("document-less-than-100-word")
```
:::

***
