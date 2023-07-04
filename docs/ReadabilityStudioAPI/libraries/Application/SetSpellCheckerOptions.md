::: {.minipage data-latex="{\textwidth}"}
## SetSpellCheckerOptions {-}

Sets which items the spell checker should ignore.

```{sql}
SetSpellCheckerOptions(IgnoreProperNouns,
                       IgnoreUppercased,
                       IgnoreNumerals,
                       IgnoreFileAddresses,
                       IgnoreProgrammerCode)
```

### Parameters {-}

**Parameter** | **Description**
| :-- | :-- |
IgnoreProperNouns | **true** to ignore proper nouns.
IgnoreUppercased | **true** to ignore uppercased words.
IgnoreNumerals | **true** to ignore numerals.
IgnoreFileAddresses | **true** to ignore filepaths and URLs.
IgnoreProgrammerCode | **true** to ignore programmer code.
:::

***
