::: {.minipage data-latex="{\textwidth}"}
## WriteToFile {-}

Writes a string to the specified file path.

### Syntax {-}

```{sql}
boolean WriteToFile(string filePath,
                    string message)
```

### Parameters {-}

**Parameter** | **Description**
| :-- | :-- |
filePath | The file to write to (will be overwritten if file exists).
message | The text to write to the file.

### Return value {-}

Type: `boolean` \

Returns `true` if output was successfully written to the file.
:::
