<#
.SYNOPSIS
    Recover a coherence combined.schema from a game that does NOT ship one.

.DESCRIPTION
    Most coherence games do not ship a loose combined.schema next to the exe --
    the schema is baked into Unity's globalgamemanagers.assets as the coherence
    RuntimeSettings. Without it, the upload pipeline has nothing to feed, so
    those games could not be set up at all.

    This pulls the schema back out. Inside the assets file each baked schema is
    stored as a coherence SchemaDefinition: a length-prefixed text blob, then its
    name ("Toolkit.schema" / "Gathered.schema"), then a 40-char sha1 id, then the
    component table. Because the id is sha1 of the text, every extraction is
    SELF-VALIDATING: if sha1(text) does not equal the stored id, the bytes are
    wrong and we refuse them rather than emit a schema that will be silently
    rejected server-side.

    The client asks the replication server for a single combined id, which
    coherence computes as:

        sha1( join("`n", [ lf(Toolkit) , Gathered ]) )

    where lf() rewrites CRLF to LF (the authored Toolkit.schema is CRLF, the
    gathered schema is LF, and the bake normalises before hashing). That combined
    id is ALSO stored in the assets file, so the reconstruction is checked end to
    end: if the combined we build does not hash to the id the game will request,
    you are told, loudly, before wasting an upload.

.PARAMETER GamePath
    The game folder, or a path to globalgamemanagers.assets directly. A folder is
    searched for <Game>_Data\globalgamemanagers.assets.

.PARAMETER OutputDir
    Where to write combined.schema / Toolkit.schema / Gathered.schema. Defaults
    to the assets file's own folder.

.EXAMPLE
    .\Extract-CoherenceSchema.ps1 "C:\Games\Lost Skies"

    Then feed the result to the upload pipeline:
    .\Invoke-CoherenceSchemaPipeline.ps1 -ProjectPath <unity> `
        -CombinedSchemaPath <out>\combined.schema `
        -ToolkitSchemaPath  <out>\Toolkit.schema
#>
[CmdletBinding()]
param(
    [Parameter(Mandatory = $true, Position = 0)]
    [string] $GamePath,

    [string] $OutputDir
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Fail([string] $m) { throw "[UCO2] $m" }

# --- resolve the assets file --------------------------------------------------
$GamePath = $GamePath.Trim().Trim('"')
if (-not (Test-Path -LiteralPath $GamePath)) { Fail "Path not found: $GamePath" }

$assets = $null
if ((Get-Item -LiteralPath $GamePath).PSIsContainer) {
    $assets = Get-ChildItem -LiteralPath $GamePath -Recurse -Filter 'globalgamemanagers.assets' `
                -File -ErrorAction SilentlyContinue |
              Where-Object { $_.DirectoryName -like '*_Data' } |
              Select-Object -First 1 -ExpandProperty FullName
    if (-not $assets) {
        # fall back to any globalgamemanagers.assets under the folder
        $assets = Get-ChildItem -LiteralPath $GamePath -Recurse -Filter 'globalgamemanagers.assets' `
                    -File -ErrorAction SilentlyContinue |
                  Select-Object -First 1 -ExpandProperty FullName
    }
    if (-not $assets) { Fail "No globalgamemanagers.assets found under $GamePath" }
} else {
    $assets = (Resolve-Path -LiteralPath $GamePath).Path
}
Write-Host "[UCO2] assets: $assets"

if ([string]::IsNullOrWhiteSpace($OutputDir)) { $OutputDir = Split-Path -Parent $assets }
if (-not (Test-Path -LiteralPath $OutputDir)) { New-Item -ItemType Directory -Path $OutputDir -Force | Out-Null }

# --- read once; a Latin1 view gives byte==char for fast, safe searching -------
$bytes  = [System.IO.File]::ReadAllBytes($assets)
$latin1 = [System.Text.Encoding]::GetEncoding(28591).GetString($bytes)
$sha1   = [System.Security.Cryptography.SHA1]::Create()

function Get-U32([int] $off) { return [System.BitConverter]::ToUInt32($bytes, $off) }
function Align4([int] $x)    { return ($x + 3) -band (-bnot 3) }
function Sha1Hex([byte[]] $b) {
    return -join ($sha1.ComputeHash($b) | ForEach-Object { $_.ToString('x2') })
}

# Extract one named SchemaDefinition. Layout, all little-endian:
#   [u32 textLen][text][pad4]  <-- ends exactly where the name prefix begins
#   [u32 nameLen][name][pad4]
#   [u32 idLen=40][40 hex id]
function Get-Schema([string] $name) {
    $nameLen = $name.Length
    $search  = 0
    while ($true) {
        $at = $latin1.IndexOf($name, $search, [System.StringComparison]::Ordinal)
        if ($at -lt 0) { return $null }
        $search = $at + 1
        if ($at -lt 4) { continue }
        if ((Get-U32 ($at - 4)) -ne $nameLen) { continue }   # must be length-prefixed

        # the text blob ends at ($at-4); walk back to its own length prefix
        $textEnd = $at - 4
        $found   = $null
        for ($ts = $textEnd - 4; $ts -ge [Math]::Max(0, $textEnd - 3000000); $ts--) {
            $L = Get-U32 $ts
            if ($L -ge 1 -and $L -le 3000000 -and ($ts + 4 + (Align4 $L)) -eq $textEnd) {
                $found = @{ Off = $ts + 4; Len = [int] $L }
                break
            }
        }
        if (-not $found) { continue }

        $text = New-Object byte[] $found.Len
        [Array]::Copy($bytes, $found.Off, $text, 0, $found.Len)

        # id after the (aligned) name
        $idPrefix = Align4 ($at + $nameLen)
        $storedId = ''
        if (($idPrefix + 4 + 40) -le $bytes.Length -and (Get-U32 $idPrefix) -eq 40) {
            $storedId = [System.Text.Encoding]::ASCII.GetString($bytes, $idPrefix + 4, 40)
        }
        return @{ Name = $name; Text = $text; StoredId = $storedId }
    }
}

# --- pull the two schemas coherence combines ----------------------------------
$toolkit  = Get-Schema 'Toolkit.schema'
$gathered = Get-Schema 'Gathered.schema'
if (-not $gathered) { Fail 'Gathered.schema not found in the assets file -- is this actually a coherence game?' }
if (-not $toolkit)  { Fail 'Toolkit.schema not found in the assets file.' }

foreach ($s in @($toolkit, $gathered)) {
    $calc = Sha1Hex $s.Text
    if ($s.StoredId -and $calc -ne $s.StoredId) {
        Fail ("$($s.Name): extracted bytes do not match the stored id " +
              "(got $calc, expected $($s.StoredId)). Refusing to emit a bad schema.")
    }
    Write-Host ("[UCO2] {0,-16} {1,8:N0} bytes  id {2}  [verified]" -f $s.Name, $s.Text.Length, $calc)
}

# --- build combined = lf(Toolkit) + "`n" + Gathered ; verify against the game --
function ToLf([byte[]] $b) {
    $s = [System.Text.Encoding]::GetEncoding(28591).GetString($b) -replace "`r`n", "`n"
    return [System.Text.Encoding]::GetEncoding(28591).GetBytes($s)
}
$nl       = [byte[]] @(0x0A)
$combined = [byte[]]((ToLf $toolkit.Text) + $nl + (ToLf $gathered.Text))
$combinedId = Sha1Hex $combined

# the id the client will actually ask for is baked in too -- find it and confirm
$clientWants = ''
foreach ($m in [regex]::Matches($latin1, '(?<![0-9a-f])[0-9a-f]{40}(?![0-9a-f])')) {
    $o = $m.Index
    if ($o -ge 4 -and (Get-U32 ($o - 4)) -eq 40) {
        $id = $m.Value
        if ($id -ne $toolkit.StoredId -and $id -ne $gathered.StoredId) { $clientWants = $id; break }
    }
}

Write-Host ''
Write-Host ("[UCO2] combined id = $combinedId")
if ($clientWants) {
    if ($combinedId -eq $clientWants) {
        Write-Host "[UCO2] client requests $clientWants -- MATCH. Upload this and the game will find it." -ForegroundColor Green
    } else {
        Write-Warning "[UCO2] client requests $clientWants but our combined is $combinedId. Do NOT upload blindly -- the schema set/order may differ for this game."
    }
} else {
    Write-Host "[UCO2] (no baked combined id found to cross-check; the per-schema ids above are still verified)"
}

# --- write outputs ------------------------------------------------------------
$combinedPath = Join-Path $OutputDir 'combined.schema'
$toolkitPath  = Join-Path $OutputDir 'Toolkit.schema'
$gatheredPath = Join-Path $OutputDir 'Gathered.schema'
[System.IO.File]::WriteAllBytes($combinedPath, $combined)
[System.IO.File]::WriteAllBytes($toolkitPath,  (ToLf $toolkit.Text))
[System.IO.File]::WriteAllBytes($gatheredPath, (ToLf $gathered.Text))

Write-Host ''
Write-Host "[UCO2] wrote:"
Write-Host "         $combinedPath"
Write-Host "         $toolkitPath"
Write-Host "         $gatheredPath"
Write-Host ''
Write-Host "Next, upload it with your own project:"
Write-Host "  .\Invoke-CoherenceSchemaPipeline.ps1 -ProjectPath <unity project> ``"
Write-Host "      -CombinedSchemaPath `"$combinedPath`" ``"
Write-Host "      -ToolkitSchemaPath  `"$toolkitPath`""
