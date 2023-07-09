::: {.minipage data-latex="{\textwidth}"}
## CheckHtmlLinks {-}

Checks for broken links and bad image sizes in a folder of HTML files.

### Syntax {-}

```{sql}
CheckHtmlLinks(string folderPath,
               boolean followExternalLinks)
```

### Parameters {-}

**Parameter** | **Description**
| :-- | :-- |
folderPath | The folder containing the HTML files to review.
followExternalLinks | Whether to verify links going outside of the folder being reviewed.
:::

::: {.notesection data-latex=""}
Issues are recorded into the program's log.
View the log report to see any issues found when calling this function.
:::

***


