# StandardProject {#standard-project}

Provides the functionality of a standard project. Standard projects analyze a single document and provides numerous results related to readability and other style metrics.

Note that changing various features of a project once
it has been loaded will cause it to re-index its
document. As an optimization, this re-indexing can be
delayed until all these features have been changed.
This is done by:

- calling **[DelayReloading()](#standard-delayreloading)**
- calling any functions that require re-indexing
- and finally calling **[Reload()](#standard-reload)**

For example:

```{sql}
-- Opens a document from user's "Documents" folder
-- and changes a few options
sp = StandardProject(Application.GetDocumentsPath() ..
                     "Consent Form.docx")

-- Delay reloading
sp:DelayReloading(true)
-- Change a few options
sp:AggressivelyDeduceLists(true)
-- Reindex the document so that the
-- changes take effect.
sp:Reload()

-- assuming we have a custom test by this name
sp:AddTest("Patient Consent Formula")
```
