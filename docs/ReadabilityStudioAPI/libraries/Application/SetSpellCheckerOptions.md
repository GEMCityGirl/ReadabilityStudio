::: {.minipage data-latex="{\textwidth}"}
## SetSpellCheckerOptions {-}

Sets which items the spell checker should ignore.

### Syntax {-}

```{sql}
SetSpellCheckerOptions(IgnoreProperNouns,
                       IgnoreUppercased,
                       IgnoreNumerals,
                       IgnoreFileAddresses,
                       IgnoreProgrammerCode)
```

### Parameters {-}

**Parameter** | **Type** | **Description**
| :-- | :-- | :-- |
IgnoreProperNouns | **boolean** | **true** to ignore proper nouns.
IgnoreUppercased | **boolean** | **true** to ignore uppercased words.
IgnoreNumerals | **boolean** | **true** to ignore numerals.
IgnoreFileAddresses | **boolean** | **true** to ignore filepaths and URLs.
IgnoreProgrammerCode | **boolean** | **true** to ignore programmer code.
:::

***
