::: {.minipage data-latex="{\textwidth}"}
## GetImageInfo {-}

Returns the details of a given file.

### Syntax {-}

```{sql}
table GetImageInfo()
```

### Parameters {-}

**Parameter** | **Type** | **Description**
| :-- | :-- | :-- |
ImagePath | **string** | The file path of the image.

### Return value {-}

Type: **table** \

A table of the image's information.

### Example {-}

```{sql}
info = Application.GetImageInfo("C:\\images\\GraphLogo.png")

-- print information about an image
for i,v in pairs(info)
do
    Debug.Print("<b>"..i.."</b>: "..v)
end
```
:::

***