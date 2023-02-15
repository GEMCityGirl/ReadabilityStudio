## SetBlockExclusionTags {#batch-setblockexclusiontags .unnumbered}

Specifies a pair of tags that will exclude all text between them.

```{sql}
SetBlockExclusionTags(Tags)
```

### Parameters {-}

Parameter | Description
| :-- | :-- |
Tags | The pair of tags that will exclude all text between them.

::: {.note}

Pass in an empty string to turn off this option.
:::