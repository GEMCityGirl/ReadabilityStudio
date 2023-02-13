## SortList {#batch-sortlist .unnumbered}

Sorts the specified list using a series of columns and sorting orders.

```{sql}
SortList(ListType,
         ColumnToSort,
         SortOrder,
         ...) 
```

### Parameters {-}

Parameter | Description
| :-- | :-- |
ListType | The [list](#listtype) to sort.
ColumnToSort | The column to sort by.
SortOrder | The [sorting direction](#sortorder) to use.
... | Any additional sets of columns and sort orders to use.