:::: {.minipage data-latex="{\textwidth}"}
## Print {-}

Prints a message to the script editor's debug window.

```{sql}
Print(Message)
```

### Parameters {-}

**Parameter** | **Description**
| :-- | :-- |
Message | The message to print.

::: {.tipsection data-latex=""}
`Message` can be HTML-formatted text.
:::

### Example {-}

```{sql}
info = Application.GetImageInfo("C:\\images\\GraphLogo.png")

-- print information about an image
for i,v in pairs(info)
do
    Debug.Print("<b>"..i.."</b>: "..v)
end
```
::::
