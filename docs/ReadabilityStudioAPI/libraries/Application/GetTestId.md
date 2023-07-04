::: {.minipage data-latex="{\textwidth}"}
## GetTestId {-}

Returns the numeric ID of a test, which should be passed in as a string. This function can be used to find a custom test's window.

```{sql}
integer GetTestId(TestName)
```

### Parameters {-}

**Parameter** | **Description**
| :-- | :-- |
TestName | The name of the test.

### Return value {-}

The numeric ID of the test.
:::
