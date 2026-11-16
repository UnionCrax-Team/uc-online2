# Coherence schema pipeline

This tool installs the coherence Unity SDK (which provides the coherence Hub),
prepares a Unity project, and performs a reproducible schema upload. The Hub is
not a separate Windows program; it is part of the `io.coherence.sdk` package.

The script uses the official scoped registry:

- registry: `https://registry.npmjs.org`
- scope and package: `io.coherence.sdk`
- default version: `1.6.3` (the version verified against Vampire Survivors)

It uses the documented Unity Editor entry points:

- `Coherence.Editor.BakeUtil.Bake()` when `-Bake` is supplied.
- `Coherence.Editor.Portal.Schemas.Upload(projectId, projectToken)`.

The pipeline does not guess a schema boundary. It requires the shipped
`combined.schema` to begin byte-for-byte with the project's `Toolkit.schema`,
then writes the remaining content after exactly one separator newline to
`Assets/coherence/Gathered.schema`.

## If the game does not ship `combined.schema`

Many coherence games bake the schema into
`<Game>_Data/globalgamemanagers.assets` and ship no loose `combined.schema`.
Run `Extract-CoherenceSchema.ps1` first to recover it:

```powershell
.\Extract-CoherenceSchema.ps1 "C:\path\to\game"
```

It writes `combined.schema`, `Toolkit.schema` and `Gathered.schema` next to the
assets file, self-validated against the sha1 ids baked in alongside each schema
(and against the combined id the client will request). Pass the first two on to
the pipeline via `-CombinedSchemaPath` and `-ToolkitSchemaPath`. See the plugin
README's "When the game does not ship combined.schema" for the byte layout and
why a hand-carve fails.

## What the user needs

- The Unity project folder they want to use for the upload.
- The game's `combined.schema` — either the shipped
  `StreamingAssets/combined.schema`, or the one `Extract-CoherenceSchema.ps1`
  recovers from `globalgamemanagers.assets` (above).
- The matching Unity Editor installed through Unity Hub. The script reads the
  required version from `ProjectSettings/ProjectVersion.txt`.
- Their coherence organization ID, project ID, and project token.

For SDK 1.6, the project token is available under Dashboard > Project >
Settings. coherence's CI flow exposes that token to Unity as
`COHERENCE_PORTAL_TOKEN`; this runner does the same only for the Unity process.

The project token is entered as a hidden prompt. It is passed to Unity through
the process environment and is never written into the project or logs.

## Easiest usage

Double-click `Run-CoherenceSchemaPipeline.bat`. It only starts PowerShell; all
work is performed by `Invoke-CoherenceSchemaPipeline.ps1`.

The script prompts for:

1. Unity project folder.
2. `combined.schema` path.
3. coherence project ID.
4. coherence project token (hidden).
5. coherence organization ID.

It then installs the coherence package, waits for Unity to import it, verifies
that its real `Toolkit.schema` exactly matches the start of `combined.schema`,
writes `Assets/coherence/Gathered.schema`, and uploads the schema.

## PowerShell usage

Every prompted value can also be supplied for unattended use:

```powershell
$env:COHERENCE_PROJECT_ID = 'your-project-id'
$env:COHERENCE_PROJECT_TOKEN = 'your-project-token'

& .\tools\coherence_schema\Invoke-CoherenceSchemaPipeline.ps1 `
  -ProjectPath 'D:\Work\MyCoherenceProject' `
  -CombinedSchemaPath 'D:\Build\Game_Data\StreamingAssets\combined.schema' `
  -UnityPath 'C:\Program Files\Unity\Hub\Editor\2022.3.40f1\Editor\Unity.exe' `
  -OrganizationId 'your-organization-id' `
  -KeepEditorHelper
```

Use `-CoherenceVersion` only when targeting a game built with a different SDK.
An existing `io.coherence.sdk` version in the project is preserved.

Official references: [installing the Unity SDK](https://docs.coherence.io/1.6/getting-started/installation)
and [SDK 1.6 CI schema uploads](https://docs.coherence.io/1.6/manual/advanced-topics/team-workflows/continuous-integration-setup).

Pass `-Bake` only when the Unity project itself must regenerate the schema.
For a shipped schema that has already been validated, omit `-Bake` so the
supplied `Gathered.schema` is uploaded unchanged.

`-ToolkitSchemaPath` can be supplied when the project contains more than one
`Toolkit.schema`. If omitted, exactly one copy must exist under the project.

## Backups and failure detection

Each run creates `.uco2-schema-backups/<timestamp>` inside the Unity project.
The package manifest, package lock, existing `Gathered.schema`, and injected
Editor helper are backed up before modification. The run records a JSON
manifest plus separate package-install and schema-upload Unity logs.

The PowerShell process fails when:

- the project, schema, matching Unity executable, credentials, or helper is missing;
- the project has no `ProjectSettings` folder;
- Unity cannot install or compile the coherence package;
- the Toolkit prefix does not match byte-for-byte;
- the separator newline or derived gathered schema is missing;
- Unity exits nonzero; or
- Unity exits zero without emitting `UCO2_SCHEMA_PIPELINE:SUCCESS`.

The project token is read from the environment and is never written into the
Unity project or a generated file.
