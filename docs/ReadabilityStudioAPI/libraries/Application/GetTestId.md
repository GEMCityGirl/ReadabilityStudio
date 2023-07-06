::: {.minipage data-latex="{\textwidth}"}
## GetTestId {-}

Returns the numeric ID of a test, which should be passed in as a string. This function can be used to find a custom test's window.

### Syntax {-}

```{sql}
integer GetTestId(TestName)
```

### Parameters {-}

**Parameter** | **Type** | **Description**
| :-- | :-- | :-- |
TestName | **string** | The name of the test.

### Return value {-}

Type: **number** \

The numeric ID of the test.
:::

***
