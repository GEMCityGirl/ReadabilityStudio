## SetTextExclusion {-}

Specifies how text should be excluded.

```{sql}
SetTextExclusion(TextExclusionType)
```

### Parameters {-}

Parameter | Description
| :-- | :-- |
TextExclusionType | How text should be [excluded](#textexclusiontype).

### Example {-}

```{sql}
-- don't exclude any text for all future projects
Application.SetTextExclusion(TextExclusionType.DoNotExcludeAnyText)
```