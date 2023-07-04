## AddTest {#batch-addtest .unnumbered}

Adds a test to the project.

```{sql}
boolean AddTest(Test)
```

### Parameters {-}

**Parameter** | **Description**
| :-- | :-- |
Test | The ID of the test to add.

::: {.notesection data-latex=""}
For a custom test, use the name of the test.
:::

### Return value {-}

`true` if the test was successfully added; otherwise, `false`.
