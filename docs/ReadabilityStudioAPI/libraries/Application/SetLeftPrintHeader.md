::: {.minipage data-latex="{\textwidth}"}
## SetLeftPrintHeader {-}

Sets the label to appear in the top left corner of printouts\index{Printing!headers \& footers}.

### Syntax {-}

```{sql}
SetLeftPrintHeader(string label)
```

### Parameters {-}

**Parameter** | **Description**
| :-- | :-- |
label | The label to appear in the top left corner of printouts.
:::

### Example {-}

```{sql}
-- Set headers and footers for printouts.
Application.SetLeftPrintHeader("")
Application.SetCenterPrintHeader("@TITLE@")
Application.SetRightPrintHeader("Page @PAGENUM@ of @PAGESCNT@")

Application.SetRightPrintFooter("Page @PAGENUM@")
Application.SetCenterPrintFooter("")
Application.SetRightPrintFooter("Printed on @DATE@")
```

***
