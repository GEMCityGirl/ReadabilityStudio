::: {.minipage data-latex="{\textwidth}"}
## ExportGraph {#standard-exportgraph .unnumbered}

Saves\index{Graphs!exporting} the specified graph as an image.

### Syntax {-}

```{sql}
ExportGraph(GraphType type,
            string outputFilePath,
            boolean grayScale,
            number width,
            number height)
```

### Parameters {-}

**Parameter** | **Description**
| :-- | :-- |
type | The [graph](#graphtype) to export.
outputFilePath | The folder to save the graph.
grayScale | (Optional) Set to **true** to save the image as black &amp; white.
width | (Optional) Specifies the width of the image.
height | (Optional) Specifies the height of the image.

### Example {-}

```{sql}
-- Opens a document from user's "Documents" folder,
-- exports 3 of its graphs, then closes the project.
recipe = StandardProject(Application.GetDocumentsPath() .. "Turkey Brining.docx")

recipe:ExportGraph(GraphType.SentenceBoxPlox,
               Application.GetDocumentsPath() .. "Review/RecipeSentBoxPlot.png")

recipe:ExportGraph(GraphType.WordBarChart,
               Application.GetDocumentsPath() .. "Review/RecipeWordBarChart.png",
               -- save it as black & white
               true)

recipe:ExportGraph(GraphType.SyllableHistogram,
               Application.GetDocumentsPath() .. "Review/RecipeSyllableHistogram.png",
               -- color image
               false,
               -- 500 pixels wide
               -- (height will be adjusted to maintain aspect ratio)
               500)

recipe:Close()
```
:::

***
