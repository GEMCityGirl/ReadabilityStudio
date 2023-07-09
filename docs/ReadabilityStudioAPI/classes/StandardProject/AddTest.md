::: {.minipage data-latex="{\textwidth}"}
## AddTest {-}

Adds a test to the project.

### Syntax {-}

```{sql}
boolean AddTest(Test (or string) test)
```

### Parameters {-}

**Parameter** | **Description**
| :-- | :-- |
test | **[Test](#tests)** or The ID of the test to add. (For a custom test, use the name of the test.)

### Return value {-}

Type: **boolean** \

**true** if the test was successfully added; otherwise, **false**.

### Example {-}

```{sql}
-- Opens a document from user's "Documents" folder
-- and adds some tests.
consentForm = StandardProject(Application.GetDocumentsPath() ..
                              "Consent Form.docx")

consentForm:AddTest(Tests.DanielsonBryan1)
consentForm:AddTest(Tests.DanielsonBryan2)
-- assuming we have a custom test by this name.
consentForm:AddTest("Patient Consent Formula")
```
:::

***
