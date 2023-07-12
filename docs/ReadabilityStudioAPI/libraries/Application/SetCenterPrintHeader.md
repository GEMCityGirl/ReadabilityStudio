::: {.minipage data-latex="{\textwidth}"}
## SetCenterPrintHeader {-}

Sets the label to appear in the center top of printouts\index{Printing!headers & footers}.

### Syntax {-}

```{sql}
SetCenterPrintHeader(string label)
```

### Parameters {-}

**Parameter** | **Description**
| :-- | :-- |
label | The label to appear in the center top of printouts.
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
