## CheckHtmlLinks {-}

Checks for broken links and bad image sizes in a folder of HTML files.

:::{.note}
Issues are recorded into the program's log. View the log report to see any issues found when calling this function.
:::

```{sql}
CheckHtmlLinks(FolderPath,FollowExternalLinks)
```

### Parameters {-}

Parameter | Description
| :-- | :-- |
FolderPath | The folder containing the HTML files to review.
FollowExternalLinks | Whether to verify links going outside of the folder being reviewed.
