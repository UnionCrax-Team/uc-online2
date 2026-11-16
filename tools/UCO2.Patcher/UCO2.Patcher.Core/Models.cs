using System.Text.Json.Serialization;

namespace UCO2.Patcher.Core;

public enum EngineKind
{
    Unknown,
    Unity,
    Unreal,
    Generic
}

public enum GameArchitecture
{
    Unknown,
    X86,
    X64
}

public enum SteamStubStatus
{
    NotDetected,
    Detected,
    UnrecognizedBindSection,
    Unreadable
}

[Flags]
public enum BackendKind
{
    None = 0,
    PhotonRealtime = 1,
    PhotonFusion = 2,
    PhotonVoice = 4,
    Eos = 8,
    PlayFab = 16,
    Coherence = 32
}

public sealed record SteamGame(
    uint AppId,
    string Name,
    string InstallDirectory,
    string ManifestPath,
    string LibraryPath);

public sealed record SteamSearchResult(
    uint AppId,
    string Name,
    string? ImageUrl)
{
    public override string ToString() => $"{Name}  ({AppId})";
}

public sealed class GameScanResult
{
    public required string GameDirectory { get; init; }
    public required EngineKind Engine { get; init; }
    public required GameArchitecture Architecture { get; init; }
    public required string SteamApiPath { get; init; }
    // Every steam_api64.dll copy in the game folder. Unity games sometimes ship
    // one in the root AND one under Data/Plugins/x86_64; the engine loads the
    // Data/Plugins copy, so we replace them ALL rather than guess which is live.
    public IReadOnlyList<string> SteamApiPaths { get; init; } = [];
    public required string ExecutablePath { get; init; }
    public required string ConfigDirectory { get; init; }
    public string? UnityDataDirectory { get; init; }
    public BackendKind Backends { get; init; }
    public SteamStubStatus SteamStub { get; init; }
    public IReadOnlyList<string> CompetingFiles { get; init; } = [];
    public IReadOnlyList<string> Warnings { get; init; } = [];

    [JsonIgnore]
    public string EngineLabel => Architecture == GameArchitecture.Unknown
        ? Engine.ToString()
        : $"{Engine} / {Architecture}";

    [JsonIgnore]
    public string BackendLabel => Backends == BackendKind.None
        ? "Steam only"
        : string.Join(", ", Enum.GetValues<BackendKind>()
            .Where(value => value != BackendKind.None && Backends.HasFlag(value))
            .Select(value => value switch
            {
                BackendKind.PhotonRealtime => "Photon Realtime",
                BackendKind.PhotonFusion => "Photon Fusion",
                BackendKind.PhotonVoice => "Photon Voice",
                BackendKind.Eos => "EOS",
                BackendKind.PlayFab => "PlayFab",
                BackendKind.Coherence => "coherence",
                _ => value.ToString()
            }));
}

public sealed class PatchOptions
{
    public uint AppId { get; set; } = 480;
    public uint OriginalAppId { get; set; }
    public bool UnlockAllDlc { get; set; } = true;
    public bool InstallOverlayProxy { get; set; } = true;
    public bool QuarantineCompetingFiles { get; set; } = true;
    public bool EnableSteamStub { get; set; }
    public bool LoadOverlay { get; set; } = true;
    public bool LogOverlay { get; set; }
    public bool WarnOverlayDisabled { get; set; }
    public bool VerboseLog { get; set; }
    public bool ForceOwnership { get; set; } = true;
    public bool PassthroughTicket { get; set; }
    public bool EmulateTicket { get; set; }
    public bool EnableSdr { get; set; }
    public bool InventoryAutoGrant { get; set; }
    // [Settings] RealAppIdEnv -- expose ogAppId in the SteamAppId env for games that
    // read it at startup and quit if it isn't their real AppId (e.g. Valheim).
    public bool RealAppIdEnv { get; set; }
    // [Settings] LocalSaves -- DEFAULT ON. Steam Cloud is bound to the RUNNING
    // app, so without this every UCO2 game shares the spoofed AppId's one
    // remote folder and games that pick a common save name overwrite each other.
    // Reports cloud as disabled (so a game with its own save path uses it) and
    // serves the Steam file API from <game>/uco_cloud for games that have none.
    public bool LocalSaves { get; set; } = true;
    // [VersionProxy] early-loader flags. LoadDllsEarly/SdrSafe are null = auto
    // (LoadDLLsEarly on when any plugin is deployed; SdrSafe on when SDR is on);
    // set true/false to force. RequireSteam defaults on (abort if Steam is closed).
    public bool? LoadDllsEarly { get; set; }
    public bool? SdrSafe { get; set; }
    public bool RequireSteam { get; set; } = true;
    public bool InstallPhoton { get; set; }
    public bool InstallEos { get; set; }
    public bool InstallPlayFab { get; set; }
    public bool InstallCoherence { get; set; }
    public string PhotonRealtimeAppId { get; set; } = "";
    public string PhotonFusionAppId { get; set; } = "";
    public string PhotonVoiceAppId { get; set; } = "";
    public string PhotonNickname { get; set; } = "";
    // [Realtime]/[Fusion] VerboseLog -- log every Photon response and status change.
    // Failed connect/auth and dropped connections are logged regardless.
    public bool PhotonVerboseLog { get; set; }
    public string EosProductId { get; set; } = "";
    public string EosSandboxId { get; set; } = "";
    public string EosDeploymentId { get; set; } = "";
    public string EosClientId { get; set; } = "";
    public string EosClientSecret { get; set; } = "";
    public string DisplayName { get; set; } = "Player";
    // Device ID login on the game's OWN Epic app (no redirect); wins over the ids above.
    public bool EosKeepGameApp { get; set; }
    // Steam+EOS games whose co-op rides presence / integrated-platform lobbies that an
    // anonymous Device ID login can't satisfy: force non-presence lobbies AND disable the
    // EOS integrated platform. Applies in both KeepGameApp and redirect modes.
    public bool EosNoPresence { get; set; }
    // [EOS] VerboseLog -- route the EOS SDK's full verbose log into the host log.
    public bool EosVerboseLog { get; set; }
    public string PlayFabTitleId { get; set; } = "";
    // Anonymous login on the game's OWN PlayFab title (no redirect); wins over TitleId.
    public bool PlayFabKeepGameTitle { get; set; }
    // [PlayFab] VerboseLog -- log every PlayFab request/rewrite/response.
    public bool PlayFabVerboseLog { get; set; }
    public string CoherenceRuntimeKey { get; set; } = "";
    public string CoherenceProjectId { get; set; } = "";
    // [Coherence] LaunchReplicationServer -- host a local replication server (LAN mode).
    public bool CoherenceLaunchReplicationServer { get; set; }
    public string LegacyClientVersion { get; set; } = "";
    // Manual [DLC] entries, one "appId=name" per line (name optional). Merged over
    // the DLC the scanner discovers in the game folder; a manual entry wins.
    public string ManualDlc { get; set; } = "";
    public Dictionary<string, string> AdditionalSettings { get; set; } = new(StringComparer.OrdinalIgnoreCase);
    // Raw ini appended/merged verbatim: the escape hatch for any flag without a
    // dedicated control. Lines under a [Section] header target that section (a key
    // with no header goes to [Settings]); an existing key is overridden in place.
    public string AdvancedIni { get; set; } = "";
}

public enum PatchOperationKind
{
    ReplaceFile,
    WriteText,
    WriteBytes,
    RemoveFile
}

public sealed class PatchOperation
{
    public required PatchOperationKind Kind { get; init; }
    public required string TargetPath { get; init; }
    public string? SourcePath { get; init; }
    public string? TextContent { get; init; }
    [JsonIgnore]
    public byte[]? BinaryContent { get; init; }
    public required string Description { get; init; }
}

public sealed class PatchPlan
{
    public required GameScanResult Game { get; init; }
    public required PatchOptions Options { get; init; }
    public required IReadOnlyList<PatchOperation> Operations { get; init; }
    public IReadOnlyList<string> Warnings { get; init; } = [];
}

public sealed class BackupManifest
{
    public int FormatVersion { get; set; } = 1;
    public required string Id { get; set; }
    public required string GameDirectory { get; set; }
    public required DateTimeOffset CreatedAt { get; set; }
    public string Status { get; set; } = "InProgress";
    public string? Error { get; set; }
    public List<BackupEntry> Entries { get; set; } = [];
}

public sealed class BackupEntry
{
    public required PatchOperationKind Kind { get; set; }
    public required string TargetPath { get; set; }
    public required string Description { get; set; }
    public bool Existed { get; set; }
    public string? OriginalHash { get; set; }
    public string? BackupRelativePath { get; set; }
    public string? ReplacementHash { get; set; }
    public bool Applied { get; set; }
}

public sealed record BackupSnapshot(
    string ManifestPath,
    string Id,
    DateTimeOffset CreatedAt,
    string Status,
    int FileCount);

public sealed record ReleaseAsset(
    string Name,
    string DownloadUrl,
    string? Digest,
    long Size);

public sealed record ReleaseInfo(
    string TagName,
    string Name,
    string HtmlUrl,
    bool IsPrerelease,
    ReleaseAsset? ReleaseArchive);

public sealed record InstalledFileStatus(string Description, string TargetPath, bool Exists, bool Current);

public sealed record FixPackageResult(string ArchivePath, int FileCount);
