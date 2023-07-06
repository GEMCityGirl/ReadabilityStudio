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

::: {.warningsection data-latex=""}
You must call [Reload()](#standard-reload) after calling this function to run the test(s). For optimization, multiple calls to **AddTest()** should be queued together, followed by a call to **Reload()** to begin processing them.
:::

### Return value {-}

**true** if the test was successfully added; otherwise, **false**.
