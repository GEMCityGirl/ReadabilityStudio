::: {.minipage data-latex="{\textwidth}"}
## SetLeftPrintFooter {-}

Sets the label to appear in the bottom left corner of printouts\index{Printing!headers \& footers}.

### Syntax {-}

```{sql}
SetLeftPrintFooter(string label)
```

### Parameters {-}

**Parameter** | **Description**
| :-- | :-- |
label | The label to appear in the bottom left corner of printouts.
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
