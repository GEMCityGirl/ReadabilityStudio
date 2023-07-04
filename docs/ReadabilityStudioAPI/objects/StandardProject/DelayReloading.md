## DelayReloading {#standard-delayreloading .unnumbered}

Prevents a project from updating while settings are being changed. This will require an explicit call to [Reload()](#standard-reload) for the changes to take affect. This is efficient for when multiple settings are being altered at once.

```{sql}
DelayReloading(Delay)
```

### Parameters {-}

**Parameter** | **Description**
| :-- | :-- |
Delay | Specifies whether to delay reloading the project.