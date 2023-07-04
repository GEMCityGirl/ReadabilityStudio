## AddTest {-}

Adds a test to the project.

```{sql}
boolean AddTest(Test)
```

### Parameters {-}

**Parameter** | **Description**
| :-- | :-- |
Test | The [ID](#tests) of the test to add. (For a custom test, use the name of the test.)

::: {.warningsection data-latex=""}
You must call [Reload()](#standard-reload) after calling this function to run the test(s). For optimization, multiple calls to `AddTest()` should be queued together, followed by a call to `Reload()` to begin processing them.
:::

### Return value {-}

`true` if the test was successfully added; otherwise, `false`.

### Example {-}

```{sql}
-- Opens a document from user's "Documents" folder,
-- exports 3 of its graphs, then closes the project.
sp = StandardProject(Application.GetDocumentsPath() ..
                     "Consent Form.docx")

sp:AddTest(Tests.DanielsonBryan1)
sp:AddTest(Tests.DanielsonBryan2)
-- assuming we have a custom test by this name
sp:AddTest("Patient Consent Formula")
-- run all queued-up tests
sp:Reload()

sp:Close()
```
