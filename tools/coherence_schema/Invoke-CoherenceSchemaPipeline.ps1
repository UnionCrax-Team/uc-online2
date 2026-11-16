[CmdletBinding()]
param(
    [string] $ProjectPath,

    [string] $CombinedSchemaPath,

    [string] $ToolkitSchemaPath,

    [string] $UnityPath,

    [ValidatePattern('^\d+\.\d+\.\d+([+-][0-9A-Za-z.-]+)?$')]
    [string] $CoherenceVersion = '1.6.3',

    [string] $ProjectId = $env:COHERENCE_PROJECT_ID,

    [string] $ProjectToken = $env:COHERENCE_PROJECT_TOKEN,

    [string] $OrganizationId = $env:COHERENCE_ORGANIZATION_ID,

    [string] $ProjectName = 'UCO2 Schema Upload',

    [switch] $Bake,

    [switch] $KeepEditorHelper,

    [string] $GatheredSchemaRelativePath = 'Assets/coherence/Gathered.schema'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$scriptRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$helperSource = Join-Path $scriptRoot 'Uco2CoherenceSchemaPipeline.cs'

function Fail([string] $Message) {
    throw "[UCO2] $Message"
}

function Normalize-InputPath([string] $Value) {
    if ($null -eq $Value) { return $Value }
    return $Value.Trim().Trim('"')
}

if ([string]::IsNullOrWhiteSpace($ProjectPath)) {
    $ProjectPath = Read-Host 'Enter the full path to the Unity project'
}
$ProjectPath = Normalize-InputPath $ProjectPath
if ([string]::IsNullOrWhiteSpace($ProjectPath)) {
    Fail 'A Unity project path is required.'
}

if ([string]::IsNullOrWhiteSpace($CombinedSchemaPath)) {
    $CombinedSchemaPath = Read-Host 'Enter the full path to the game combined.schema file'
}
$CombinedSchemaPath = Normalize-InputPath $CombinedSchemaPath
if ([string]::IsNullOrWhiteSpace($CombinedSchemaPath)) {
    Fail 'A combined.schema path is required.'
}

$projectPath = [IO.Path]::GetFullPath($ProjectPath)
$combinedPath = [IO.Path]::GetFullPath($CombinedSchemaPath)

function Assert-File([string] $Path, [string] $Description) {
    if (-not (Test-Path -LiteralPath $Path -PathType Leaf)) {
        Fail "$Description was not found: $Path"
    }
}

# [IO.Path]::GetRelativePath is .NET Core only, so it does not exist in Windows
# PowerShell 5.1 -- which is what Run-CoherenceSchemaPipeline.bat launches, and
# what most users will have. Uri.MakeRelativeUri is the Framework-era
# equivalent and works in both.
function Get-RelativePathCompat([string] $From, [string] $To) {
    $fromUri = New-Object System.Uri (([IO.Path]::GetFullPath($From)).TrimEnd('\') + '\')
    $toUri   = New-Object System.Uri ([IO.Path]::GetFullPath($To))
    if ($fromUri.Scheme -ne $toUri.Scheme) { return $To }

    $relative = [Uri]::UnescapeDataString($fromUri.MakeRelativeUri($toUri).ToString())
    return $relative -replace '/', [IO.Path]::DirectorySeparatorChar
}

function Backup-File([string] $Path, [string] $BackupRoot, [string] $ProjectRoot) {
    $fullPath = [IO.Path]::GetFullPath($Path)
    $relative = Get-RelativePathCompat $ProjectRoot $fullPath
    if ($relative.StartsWith('..')) {
        $safeName = ($fullPath -replace '[:\\/]', '_').TrimStart('_')
        $destination = Join-Path $BackupRoot (Join-Path '_external' $safeName)
    } else {
        $destination = Join-Path $BackupRoot $relative
    }

    $destinationParent = Split-Path -Parent $destination
    New-Item -ItemType Directory -Force -Path $destinationParent | Out-Null
    Copy-Item -LiteralPath $fullPath -Destination $destination -Force
    return $destination
}

function Quote-ProcessArgument([string] $Value) {
    if ($Value -notmatch '[\s"]') { return $Value }
    $escaped = [regex]::Replace($Value, '(\\*)"', '$1$1\"')
    $escaped = [regex]::Replace($escaped, '(\\+)$', '$1$1')
    return '"' + $escaped + '"'
}

function Invoke-Unity([string] $Executable, [string[]] $Arguments, [string] $Description, [string] $LogPath) {
    Write-Host "[UCO2] $Description"
    $argumentLine = ($Arguments | ForEach-Object { Quote-ProcessArgument $_ }) -join ' '
    $process = Start-Process -FilePath $Executable -ArgumentList $argumentLine -Wait -PassThru -NoNewWindow
    if ($process.ExitCode -ne 0) {
        # Unity sometimes finishes the job and then crashes on the way out --
        # observed as 0xC0000005 during shutdown, right after it fails to reach
        # its own telemetry hosts (config.uca.cloud.unity3d.com and friends).
        # The log still says the batch job completed:
        #
        #   Batchmode quit successfully invoked - shutting down!
        #   Curl error 6: Could not resolve host: cdp.cloud.unity3d.com
        #   Crash!!!
        #
        # Treating that as failure throws away completed work for a fault after
        # the fact. Trust the log's own statement of completion over the exit
        # code, and say clearly that it happened. The upload step does not rely
        # on this: it is additionally gated on the pipeline's SUCCESS marker.
        $completed = $false
        if (Test-Path -LiteralPath $LogPath) {
            $completed = (Select-String -LiteralPath $LogPath -SimpleMatch -Quiet `
                            -Pattern 'Batchmode quit successfully invoked')
        }

        if ($completed) {
            Write-Host "[UCO2] Unity exited $($process.ExitCode) but its log reports the batch job completed"
            Write-Host "[UCO2] (crash during shutdown, typically while reaching Unity's cloud endpoints). Continuing."
        } else {
            Fail "Unity exited with code $($process.ExitCode) while $Description. See $LogPath"
        }
    }
    Assert-File $LogPath 'Unity log'
}

function Install-CoherencePackage([string] $ManifestPath, [string] $Version) {
    $manifestText = [IO.File]::ReadAllText($ManifestPath)
    try { $manifest = $manifestText | ConvertFrom-Json }
    catch { Fail "Packages/manifest.json is not valid JSON: $($_.Exception.Message)" }

    if (-not $manifest.dependencies) {
        Fail 'Packages/manifest.json has no dependencies object.'
    }

    $existing = $manifest.dependencies.PSObject.Properties['io.coherence.sdk']
    if ($existing) {
        Write-Host "[UCO2] coherence SDK already declared: $($existing.Value)"
    } else {
        $manifest.dependencies | Add-Member -NotePropertyName 'io.coherence.sdk' -NotePropertyValue $Version
        Write-Host "[UCO2] adding coherence SDK $Version"
    }

    $scopedRegistriesProperty = $manifest.PSObject.Properties['scopedRegistries']
    $registries = if ($scopedRegistriesProperty) { @($scopedRegistriesProperty.Value) } else { @() }
    $registry = $registries | Where-Object {
        $urlProperty = $_.PSObject.Properties['url']
        $scopesProperty = $_.PSObject.Properties['scopes']
        $urlProperty -and $scopesProperty -and
            $urlProperty.Value.TrimEnd('/') -eq 'https://registry.npmjs.org' -and
            @($scopesProperty.Value) -contains 'io.coherence.sdk'
    } | Select-Object -First 1
    if (-not $registry) {
        $registries += [pscustomobject][ordered]@{
            name = 'coherence'
            url = 'https://registry.npmjs.org'
            scopes = @('io.coherence.sdk')
        }
        if ($manifest.PSObject.Properties['scopedRegistries']) {
            $manifest.scopedRegistries = $registries
        } else {
            $manifest | Add-Member -NotePropertyName scopedRegistries -NotePropertyValue $registries
        }
        Write-Host '[UCO2] adding the official coherence scoped registry.'
    } else {
        Write-Host '[UCO2] official coherence scoped registry already configured.'
    }

    $updated = ($manifest | ConvertTo-Json -Depth 100) + [Environment]::NewLine
    if ($updated -cne $manifestText) {
        [IO.File]::WriteAllText($ManifestPath, $updated, [Text.UTF8Encoding]::new($false))
        return $true
    }
    return $false
}

function Find-Unity([string] $Requested, [string] $ProjectRoot) {
    if ($Requested) {
        $Requested = Normalize-InputPath $Requested
        Assert-File $Requested 'Unity executable'
        return [IO.Path]::GetFullPath($Requested)
    }

    $projectVersionPath = Join-Path $ProjectRoot 'ProjectSettings\ProjectVersion.txt'
    Assert-File $projectVersionPath 'Unity ProjectVersion.txt'
    $versionMatch = [regex]::Match([IO.File]::ReadAllText($projectVersionPath), '(?m)^m_EditorVersion:\s*(\S+)')
    if (-not $versionMatch.Success) {
        Fail "Could not read m_EditorVersion from $projectVersionPath"
    }
    $requiredVersion = $versionMatch.Groups[1].Value

    # Editors installed manually rather than through Hub land outside all the
    # Hub paths -- e.g. "C:\Program Files\Unity 6000.4.5f1\Editor\Unity.exe".
    # Hub records those in editors-v2.json, so read it as well before giving up
    # and prompting.
    $hubRecord = Join-Path $env:APPDATA 'UnityHub\editors-v2.json'
    $hubPaths = @()
    if (Test-Path -LiteralPath $hubRecord) {
        try {
            $hubJson = Get-Content -LiteralPath $hubRecord -Raw | ConvertFrom-Json
            foreach ($entry in @($hubJson.data)) {
                if ($entry.version -ne $requiredVersion) { continue }
                $hubPaths += @($entry.location)
            }
        } catch {
            Write-Host "[UCO2] could not read $hubRecord ($($_.Exception.Message)); ignoring it."
        }
    }

    # Whole pipeline inside @( ), or a single hit unwraps to a bare FileInfo.
    $candidates = @(
        @(
            (Join-Path ${env:ProgramFiles} "Unity\Hub\Editor\$requiredVersion\Editor\Unity.exe"),
            (Join-Path ${env:ProgramFiles} "Unity Hub\Editor\$requiredVersion\Editor\Unity.exe"),
            (Join-Path ${env:ProgramFiles(x86)} "Unity\Hub\Editor\$requiredVersion\Editor\Unity.exe"),
            (Join-Path ${env:ProgramFiles(x86)} "Unity Hub\Editor\$requiredVersion\Editor\Unity.exe"),
            (Join-Path $env:LOCALAPPDATA "UnityHub\Editor\$requiredVersion\Editor\Unity.exe")
        ) + $hubPaths | ForEach-Object { Get-ChildItem -Path $_ -File -ErrorAction SilentlyContinue } |
            Sort-Object FullName -Unique
    )

    if (-not $candidates) {
        $manual = Normalize-InputPath (Read-Host "Unity $requiredVersion is required but was not found. Enter the full path to its Unity.exe")
        Assert-File $manual 'Unity executable'
        return [IO.Path]::GetFullPath($manual)
    }
    return $candidates[0].FullName
}

function Find-ToolkitSchema([string] $ProjectRoot, [string] $Requested) {
    if ($Requested) {
        Assert-File $Requested 'Toolkit.schema'
        return [IO.Path]::GetFullPath($Requested)
    }

    # The array subexpression must wrap the WHOLE pipeline. Wrapping only the
    # Get-ChildItem block and then piping to Sort-Object unwraps it again, so a
    # single match arrives as a bare FileInfo and .Count does not exist on it
    # under Windows PowerShell 5.1 + StrictMode. (PowerShell 7 tolerates this,
    # which is exactly how it survives testing on a dev machine and fails on
    # everyone else's.)
    #
    # Not named $matches: that is an automatic variable populated by -match.
    # Resolve the package directories FIRST, then recurse into each.
    #
    # "Get-ChildItem -Path <dir-with-wildcard> -Recurse" silently returns
    # nothing in Windows PowerShell -- the wildcard is treated as a leaf
    # pattern, so it never descends. Verified against this project: the
    # wildcard form finds 0 files while the same search rooted at
    # Library\PackageCache finds Toolkit.schema. It fails EMPTY rather than
    # erroring, which is why it looked like a missing SDK.
    $packageRoots = @(
        Get-ChildItem -Path (Join-Path $ProjectRoot 'Packages') -Directory -Filter 'io.coherence.sdk*' -ErrorAction SilentlyContinue
        Get-ChildItem -Path (Join-Path $ProjectRoot 'Library\PackageCache') -Directory -Filter 'io.coherence.sdk*' -ErrorAction SilentlyContinue
    )
    $toolkitMatches = @(
        @(
            foreach ($root in $packageRoots) {
                Get-ChildItem -Path $root.FullName -Filter 'Toolkit.schema' -File -Recurse -ErrorAction SilentlyContinue
            }
        ) | Sort-Object FullName -Unique
    )
    if ($toolkitMatches.Count -eq 0) {
        Fail 'The coherence SDK resolved, but Toolkit.schema was not found in Packages or Library/PackageCache.'
    }
    if ($toolkitMatches.Count -gt 1) {
        $hashes = @($toolkitMatches | ForEach-Object { (Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash } | Sort-Object -Unique)
        if ($hashes.Count -gt 1) {
            Fail "Multiple different Toolkit.schema files were found. Pass -ToolkitSchemaPath explicitly: $(($toolkitMatches | ForEach-Object { $_.FullName }) -join '; ')"
        }
    }
    return $toolkitMatches[0].FullName
}

function Test-Prefix([byte[]] $Data, [byte[]] $Prefix) {
    if ($Data.Length -lt $Prefix.Length) { return $false }
    for ($i = 0; $i -lt $Prefix.Length; $i++) {
        if ($Data[$i] -ne $Prefix[$i]) { return $false }
    }
    return $true
}

function Get-GatheredBytes([byte[]] $Combined, [byte[]] $Toolkit) {
    if (-not (Test-Prefix $Combined $Toolkit)) {
        Fail 'combined.schema does not begin with the project Toolkit.schema byte-for-byte. Refusing to guess the split point.'
    }

    $start = $Toolkit.Length
    if ($Combined.Length -ge ($start + 2) -and $Combined[$start] -eq 13 -and $Combined[$start + 1] -eq 10) {
        $start += 2
    } elseif ($Combined.Length -gt $start -and $Combined[$start] -eq 10) {
        $start += 1
    } else {
        Fail 'combined.schema has no single separator newline after Toolkit.schema.'
    }

    if ($start -ge $Combined.Length) {
        Fail 'The derived Gathered.schema would be empty.'
    }
    $result = New-Object byte[] ($Combined.Length - $start)
    [Array]::Copy($Combined, $start, $result, 0, $result.Length)
    return $result
}

function Get-Sha1([byte[]] $Bytes) {
    $sha1 = [Security.Cryptography.SHA1]::Create()
    try { return ([BitConverter]::ToString($sha1.ComputeHash($Bytes))).Replace('-', '').ToLowerInvariant() }
    finally { $sha1.Dispose() }
}

function Read-PlainTextSecret([string] $Prompt) {
    $secure = Read-Host $Prompt -AsSecureString
    $pointer = [Runtime.InteropServices.Marshal]::SecureStringToBSTR($secure)
    try { return [Runtime.InteropServices.Marshal]::PtrToStringBSTR($pointer) }
    finally { [Runtime.InteropServices.Marshal]::ZeroFreeBSTR($pointer) }
}

Write-Host '[UCO2] Coherence schema pipeline starting.'
if (-not (Test-Path -LiteralPath $projectPath -PathType Container)) {
    Fail "Unity project directory was not found: $projectPath"
}
if (-not (Test-Path -LiteralPath (Join-Path $projectPath 'ProjectSettings') -PathType Container)) {
    Fail "ProjectPath is not a Unity project (ProjectSettings folder missing): $projectPath"
}
Assert-File $combinedPath 'combined.schema'
Assert-File $helperSource 'Unity Editor helper'
if (-not $ProjectId) { $ProjectId = Read-Host 'Enter the coherence project ID' }
if (-not $ProjectToken) { $ProjectToken = Read-PlainTextSecret 'Enter the coherence project token (Dashboard > Project > Settings)' }
if (-not $OrganizationId) { $OrganizationId = Read-Host 'Enter the coherence organization ID (press Enter to skip on SDK 2.x)' }
if (-not $ProjectId) { Fail 'A coherence project ID is required.' }
if (-not $ProjectToken) { Fail 'A coherence project token is required.' }
# Not required here. Only the SDK 1.x upload API reads an organization; the 2.x
# API is Upload(projectId, projectToken, mode). PowerShell cannot tell which the
# installed SDK exposes, so let it through and let the Unity side fail with a
# precise message if the legacy path is actually taken.
if (-not $OrganizationId) {
    Write-Host '[UCO2] No organization ID given. Fine on SDK 2.x; the run will fail with a clear message if this SDK only has the 1.x upload API.'
}

$unity = Find-Unity $UnityPath $projectPath
$gatheredPath = Join-Path $projectPath ($GatheredSchemaRelativePath -replace '/', '\')
$editorDir = Join-Path $projectPath 'Assets\Editor'
$helperDestination = Join-Path $editorDir 'Uco2CoherenceSchemaPipeline.cs'
$helperMeta = $helperDestination + '.meta'
$packageManifest = Join-Path $projectPath 'Packages\manifest.json'
$packageLock = Join-Path $projectPath 'Packages\packages-lock.json'
$runId = Get-Date -Format 'yyyyMMdd-HHmmss'
$backupRoot = Join-Path $projectPath (Join-Path '.uco2-schema-backups' $runId)
$packageLog = Join-Path $backupRoot 'unity-package-install.log'
$unityLog = Join-Path $backupRoot 'unity-schema-upload.log'
$manifestPath = Join-Path $backupRoot 'backup-manifest.json'

Assert-File $packageManifest 'Unity package manifest'
New-Item -ItemType Directory -Force -Path $backupRoot | Out-Null
$manifest = [ordered]@{ created = (Get-Date).ToUniversalTime().ToString('o'); files = @() }
$backups = @{}

foreach ($path in @($packageManifest, $packageLock, $gatheredPath, $helperDestination, $helperMeta)) {
    if (Test-Path -LiteralPath $path -PathType Leaf) {
        $backup = Backup-File $path $backupRoot $projectPath
        $backups[$path] = $backup
        $manifest.files += [ordered]@{ path = $path; backup = $backup }
        Write-Host "[UCO2] backed up $path -> $backup"
    } else {
        Write-Host "[UCO2] new file will be created: $path"
    }
}
$manifest | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath $manifestPath -Encoding UTF8

$null = Install-CoherencePackage $packageManifest $CoherenceVersion
Invoke-Unity $unity @(
    '-batchmode', '-nographics', '-quit',
    '-projectPath', $projectPath,
    '-logFile', $packageLog
) 'installing the coherence SDK/Hub' $packageLog

$toolkitPath = Find-ToolkitSchema $projectPath $ToolkitSchemaPath
Write-Host "[UCO2] coherence SDK installed; Toolkit.schema: $toolkitPath"

$combined = [IO.File]::ReadAllBytes($combinedPath)
$toolkit = [IO.File]::ReadAllBytes($toolkitPath)
$gathered = Get-GatheredBytes $combined $toolkit
$canonical = New-Object byte[] ($toolkit.Length + 1 + $gathered.Length)
[Array]::Copy($toolkit, 0, $canonical, 0, $toolkit.Length)
$canonical[$toolkit.Length] = 10
[Array]::Copy($gathered, 0, $canonical, $toolkit.Length + 1, $gathered.Length)
$expectedSha1 = Get-Sha1 $canonical
$expectedGatheredSha1 = Get-Sha1 $gathered

New-Item -ItemType Directory -Force -Path (Split-Path -Parent $gatheredPath) | Out-Null
New-Item -ItemType Directory -Force -Path $editorDir | Out-Null
[IO.File]::WriteAllBytes($gatheredPath, $gathered)
Copy-Item -LiteralPath $helperSource -Destination $helperDestination -Force

Write-Host "[UCO2] wrote $gatheredPath ($($gathered.Length) bytes)"
Write-Host "[UCO2] canonical Toolkit + Gathered SHA-1: $expectedSha1"
Write-Host "[UCO2] Unity executable: $unity"

$oldProjectId = $env:UCO2_COHERENCE_PROJECT_ID
$oldProjectToken = $env:UCO2_COHERENCE_PROJECT_TOKEN
$oldPortalToken = $env:COHERENCE_PORTAL_TOKEN
$oldOrganizationId = $env:UCO2_COHERENCE_ORGANIZATION_ID
$oldProjectName = $env:UCO2_COHERENCE_PROJECT_NAME
$oldBake = $env:UCO2_COHERENCE_BAKE
$oldSchema = $env:UCO2_COHERENCE_SCHEMA_PATH
$oldExpected = $env:UCO2_COHERENCE_EXPECTED_SHA1
try {
    $env:UCO2_COHERENCE_PROJECT_ID = $ProjectId
    $env:UCO2_COHERENCE_PROJECT_TOKEN = $ProjectToken
    $env:COHERENCE_PORTAL_TOKEN = $ProjectToken
    $env:UCO2_COHERENCE_ORGANIZATION_ID = $OrganizationId
    $env:UCO2_COHERENCE_PROJECT_NAME = $ProjectName
    $env:UCO2_COHERENCE_BAKE = if ($Bake) { '1' } else { '0' }
    $env:UCO2_COHERENCE_SCHEMA_PATH = $gatheredPath
    $env:UCO2_COHERENCE_EXPECTED_SHA1 = $expectedGatheredSha1

    Invoke-Unity $unity @(
        '-batchmode', '-nographics', '-quit',
        '-projectPath', $projectPath,
        '-executeMethod', 'Uco2CoherenceSchemaPipeline.Run',
        '-logFile', $unityLog
    ) 'uploading the coherence schema' $unityLog
    $log = [IO.File]::ReadAllText($unityLog)
    if ($log -notmatch 'UCO2_SCHEMA_PIPELINE:SUCCESS') {
        Fail "Unity exited successfully but did not emit the success marker. See $unityLog"
    }
    Write-Host '[UCO2] schema bake/upload completed successfully.'
} finally {
    $env:UCO2_COHERENCE_PROJECT_ID = $oldProjectId
    $env:UCO2_COHERENCE_PROJECT_TOKEN = $oldProjectToken
    $env:COHERENCE_PORTAL_TOKEN = $oldPortalToken
    $env:UCO2_COHERENCE_ORGANIZATION_ID = $oldOrganizationId
    $env:UCO2_COHERENCE_PROJECT_NAME = $oldProjectName
    $env:UCO2_COHERENCE_BAKE = $oldBake
    $env:UCO2_COHERENCE_SCHEMA_PATH = $oldSchema
    $env:UCO2_COHERENCE_EXPECTED_SHA1 = $oldExpected
    if (-not $KeepEditorHelper) {
        foreach ($temporaryPath in @($helperDestination, $helperMeta)) {
            if ($backups.ContainsKey($temporaryPath)) {
                Copy-Item -LiteralPath $backups[$temporaryPath] -Destination $temporaryPath -Force
                Write-Host "[UCO2] restored $temporaryPath"
            } elseif (Test-Path -LiteralPath $temporaryPath -PathType Leaf) {
                Remove-Item -LiteralPath $temporaryPath -Force
                Write-Host "[UCO2] removed temporary file: $temporaryPath"
            }
        }
    }
}

Write-Host "[UCO2] backup and log: $backupRoot"
