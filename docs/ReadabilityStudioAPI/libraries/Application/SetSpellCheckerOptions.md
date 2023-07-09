::: {.minipage data-latex="{\textwidth}"}
## SetSpellCheckerOptions {-}

Sets which items the spell checker should ignore.

### Syntax {-}

```{sql}
SetSpellCheckerOptions(boolean ignoreProperNouns,
                       boolean ignoreUppercased,
                       boolean ignoreNumerals,
                       boolean ignoreFileAddresses,
                       boolean ignoreProgrammerCode)
```

### Parameters {-}

**Parameter** | **Description**
| :-- | :-- |
ignoreProperNouns | **true** to ignore proper nouns.
ignoreUppercased | **true** to ignore uppercased words.
ignoreNumerals | **true** to ignore numerals.
ignoreFileAddresses | **true** to ignore filepaths and URLs.
ignoreProgrammerCode | **true** to ignore programmer code.
:::

***
