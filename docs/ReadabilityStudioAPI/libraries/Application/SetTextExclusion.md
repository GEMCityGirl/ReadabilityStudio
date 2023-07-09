::: {.minipage data-latex="{\textwidth}"}
## SetTextExclusion {-}

Specifies how text should be excluded.

### Syntax {-}

```{sql}
SetTextExclusion(TextExclusionType textExclusionType)
```

### Parameters {-}

**Parameter** | **Description**
| :-- | :-- |
textExclusionType | How text should be [excluded](#textexclusiontype).

### Example {-}

```{sql}
-- don't exclude any text for all future projects
Application.SetTextExclusion(TextExclusionType.DoNotExcludeAnyText)
```
:::

***
