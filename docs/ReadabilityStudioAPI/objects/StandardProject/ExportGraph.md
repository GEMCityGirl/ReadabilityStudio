## ExportGraph {#standard-exportgraph .unnumbered}

Saves the specified graph as an image.

```{sql}
ExportGraph(GraphType,
            OutputFilePath,
            GrayScale,
            Width,
            Height)
```

### Parameters {-}

Parameter | Description
| :-- | :-- |
GraphType | The [graph](#graphtype) to export.
OutputFilePath | The folder to save the graph.
GrayScale | (Optional) Set to `true` to save the image as black &amp; white.
Width | (Optional) Specifies the width of the image.
Height | (Optional) Specifies the height of the image.

### Example {-}

```{sql}
-- Opens a document from user's "Documents" folder,
-- exports 3 of its graphs, then closes the project.
sp = StandardProject(Application.GetDocumentsPath() .. "Turkey Brining.docx")

sp:ExportGraph(GraphType.SentenceBoxPlox,
               Application.GetDocumentsPath() .. "Review/RecipeSentBoxPlot.png")

sp:ExportGraph(GraphType.WordBarChart,
               Application.GetDocumentsPath() .. "Review/RecipeWordBarChart.png",
               -- save it as black & white
               true)

sp:ExportGraph(GraphType.SyllableHistogram,
               Application.GetDocumentsPath() .. "Review/RecipeSyllableHistogram.png",
               -- color image
               false,
               -- 500 pixels wide (height will be adjusted to maintain aspect ratio)
               500)

sp:Close()
```