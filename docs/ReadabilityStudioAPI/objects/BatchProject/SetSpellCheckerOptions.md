## SetSpellCheckerOptions {#batch-setspellcheckeroptions .unnumbered}

Sets which items the spell checker should ignore.

```{sql}
SetSpellCheckerOptions(IgnoreProperNouns,
                       IgnoreUppercased,
                       IgnoreNumerals,
                       IgnoreFileAddresses,
                       IgnoreProgrammerCode,
                       AllowColloquialisms)
```

### Parameters {-}

**Parameter** | **Description**
| :-- | :-- |
IgnoreProperNouns | Set to `true` to ignore proper nouns.
IgnoreUppercased | Set to `true` to ignore words that are in all caps.
IgnoreNumerals | Set to `true` to ignore numeric words.
IgnoreFileAddresses | Set to `true` to ignore words that are filepaths.
IgnoreProgrammerCode | Set to `true` to ignore words appear to be programming code.
AllowColloquialisms | Set to `true` to ignore colloquialisms.