## SetBlockExclusionTags {-}

Specifies a pair of tags that will have all text between them excluded from the analysis.

:::{.note}

Pass in an empty string to turn off this option.
:::

```{sql}
SetBlockExclusionTags(Tags)
```

### Parameters {-}

Parameter | Description
| :-- | :-- |
Tags | A pair of exclusion tags.