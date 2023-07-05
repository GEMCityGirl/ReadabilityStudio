::: {.minipage data-latex="{\textwidth}"}
## SetTextExclusion {-}

Specifies how text should be excluded.

### Syntax {-}

```{sql}
SetTextExclusion(TextExclusionType)
```

### Parameters {-}

**Parameter** | **Type** | **Description**
| :-- | :-- | :-- |
TextExclusionType | **[TextExclusionType](#textexclusiontype)** | How text should be excluded.

### Example {-}

```{sql}
-- don't exclude any text for all future projects
Application.SetTextExclusion(TextExclusionType.DoNotExcludeAnyText)
```
:::

***
