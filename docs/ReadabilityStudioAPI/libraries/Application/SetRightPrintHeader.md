::: {.minipage data-latex="{\textwidth}"}
## SetRightPrintHeader {-}

Sets the label to appear in the top right corner of printouts\index{Printing!headers & footers}.

### Syntax {-}

```{sql}
SetRightPrintHeader(string label)
```

### Parameters {-}

**Parameter** | **Description**
| :-- | :-- |
label | The label to appear in the top right corner of printouts.
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
