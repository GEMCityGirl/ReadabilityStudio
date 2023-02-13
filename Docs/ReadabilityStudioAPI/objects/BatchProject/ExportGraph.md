## ExportGraph {#batch-exportgraph .unnumbered}

Saves the specified graph as an image.

```{sql}
ExportGraph(SideBarSection,
            GraphType,
            OutputFilePath,
            GrayScale,
            Width,
            Height)
```

### Parameters {-}

Parameter | Description
| :-- | :-- |
SideBarSection | Which [section](#sidebarsection) of the results the graph is in.
GraphType | The [graph](#graphtype) to export.
OutputFilePath | The folder to save the graph.
GrayScale | (Optional) Set to `true` to save the image as black &amp; white.
Width | (Optional) Specifies the width of the image.
Height | (Optional) Specifies the height of the image.