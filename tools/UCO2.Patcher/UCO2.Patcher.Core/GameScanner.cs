namespace UCO2.Patcher.Core;

public sealed class GameScanner
{
    private static readonly string[] CompetingNames =
    [
        "winmm.dll", "winmm.txt", "winmm.ini", "SteamFix64.dll", "SteamFix.ini",
        "OnlineFix64.dll", "OnlineFix.ini", "dlllist.txt"
    ];

    public Task<GameScanResult> ScanAsync(string gameDirectory, CancellationToken cancellationToken = default)
    {
        return Task.Run(() => Scan(gameDirectory, cancellationToken), cancellationToken);
    }

    public GameScanResult Scan(string gameDirectory, CancellationToken cancellationToken = default)
    {
        if (string.IsNullOrWhiteSpace(gameDirectory))
            throw new ArgumentException("Choose a game folder first.", nameof(gameDirectory));

        string root = Path.TrimEndingDirectorySeparator(Path.GetFullPath(gameDirectory));
        if (!Directory.Exists(root))
            throw new DirectoryNotFoundException($"Game folder not found: {root}");

        string? unityData = FindUnityData(root, cancellationToken);
        string? unrealExe = FindUnrealExecutable(root, cancellationToken);
        EngineKind engine = unityData is not null
            ? EngineKind.Unity
            : unrealExe is not null ? EngineKind.Unreal : EngineKind.Unknown;

        string[] x64Apis = SafeFileSystem.EnumerateFiles(root, "steam_api64.dll").ToArray();
        string[] x86Apis = SafeFileSystem.EnumerateFiles(root, "steam_api.dll").ToArray();
        if (engine == EngineKind.Unknown && (x64Apis.Length > 0 || x86Apis.Length > 0))
            engine = EngineKind.Generic;
        if (engine == EngineKind.Unknown)
            throw new InvalidOperationException("Could not identify Unity, Unreal, or a Steam API installation in this folder.");

        GameArchitecture architecture = x64Apis.Length > 0
            ? GameArchitecture.X64
            : x86Apis.Length > 0 ? GameArchitecture.X86 : GameArchitecture.Unknown;

        string? steamApi = SelectSteamApi(architecture == GameArchitecture.X64 ? x64Apis : x86Apis);
        string? executable = unrealExe;
        if (executable is null && unityData is not null)
            executable = FindUnityExecutable(unityData);
        if (executable is null && steamApi is not null)
            executable = FindLargestExecutable(Path.GetDirectoryName(steamApi)!);
        executable ??= SafeFileSystem.EnumerateFiles(root, "*.exe")
            .Where(path => !Path.GetFileName(path).Contains("CrashReport", StringComparison.OrdinalIgnoreCase))
            .OrderByDescending(FileLength)
            .FirstOrDefault();

        if (steamApi is null)
        {
            string fallback = engine switch
            {
                EngineKind.Unity when unityData is not null => Path.Combine(unityData, "Plugins", "x86_64", "steam_api64.dll"),
                EngineKind.Unreal when executable is not null => Path.Combine(Path.GetDirectoryName(executable)!, "steam_api64.dll"),
                _ => ""
            };
            if (fallback.Length > 0)
            {
                steamApi = fallback;
                architecture = GameArchitecture.X64;
            }
        }

        if (steamApi is null || executable is null)
            throw new InvalidOperationException("The running executable or Steam API location could not be identified.");

        string configDirectory = engine switch
        {
            EngineKind.Unity when unityData is not null => Path.GetDirectoryName(unityData)!,
            _ => Path.GetDirectoryName(executable)!
        };

        BackendKind backends = DetectBackends(root, unityData, executable, Path.GetDirectoryName(steamApi)!, cancellationToken);
        SteamStubStatus stub = PeInspector.DetectSteamStub(executable);
        string[] competing = FindCompetingFiles(root, configDirectory, Path.GetDirectoryName(steamApi)!);
        var warnings = new List<string>();

        if (File.Exists(Path.Combine(root, "steamclient64.ini")))
            warnings.Add("A ColdClientLoader/GBE configuration was found. Launch the game executable directly so it uses the real Steam client.");
        if (architecture == GameArchitecture.X86)
            warnings.Add("This is a 32-bit game. The x86 Steam API build will be installed and x64-only plugins will be skipped.");
        if (stub == SteamStubStatus.Detected)
            warnings.Add("SteamStub DRM detected. GetStubbedLol will try to unpack it at runtime; if the game still shows \"Application load error\", unpack the exe with Steamless (https://github.com/atom0s/Steamless) and re-scan.");
        if (stub == SteamStubStatus.UnrecognizedBindSection)
            warnings.Add("SteamStub DRM detected with an unrecognized .bind variant, so runtime patching stays disabled. Unpack the exe with Steamless (https://github.com/atom0s/Steamless), then re-scan.");
        if (competing.Length > 0)
            warnings.Add($"{competing.Length} competing loader file(s) will be backed up before removal when quarantine is enabled.");

        // Replace EVERY steam_api64.dll copy, not just the primary: a Unity game
        // that ships one in the root and one under Data/Plugins/x86_64 loads the
        // latter, so patching only one leaves the game on the real Steam DLL.
        string[] archApis = architecture == GameArchitecture.X86 ? x86Apis : x64Apis;
        IReadOnlyList<string> steamApiPaths = archApis.Length > 0
            ? archApis.Distinct(StringComparer.OrdinalIgnoreCase).ToArray()
            : [steamApi];

        return new GameScanResult
        {
            GameDirectory = root,
            Engine = engine,
            Architecture = architecture,
            SteamApiPath = steamApi,
            SteamApiPaths = steamApiPaths,
            ExecutablePath = executable,
            ConfigDirectory = configDirectory,
            UnityDataDirectory = unityData,
            Backends = backends,
            SteamStub = stub,
            CompetingFiles = competing,
            Warnings = warnings
        };
    }

    private static string? FindUnityData(string root, CancellationToken cancellationToken)
    {
        IEnumerable<string> direct;
        try { direct = Directory.EnumerateDirectories(root, "*_Data", SearchOption.TopDirectoryOnly); }
        catch (Exception ex) when (ex is IOException or UnauthorizedAccessException) { direct = []; }

        foreach (string directory in direct.Concat(SafeFileSystem.EnumerateDirectories(root).Where(path => Path.GetFileName(path).EndsWith("_Data", StringComparison.OrdinalIgnoreCase))))
        {
            cancellationToken.ThrowIfCancellationRequested();
            if (Directory.Exists(Path.Combine(directory, "Managed")) || Directory.Exists(Path.Combine(directory, "il2cpp_data")))
                return directory;
        }
        return null;
    }

    private static string? FindUnrealExecutable(string root, CancellationToken cancellationToken)
    {
        foreach (string file in SafeFileSystem.EnumerateFiles(root, "*-Win64-Shipping.exe"))
        {
            cancellationToken.ThrowIfCancellationRequested();
            if (!Path.GetFileName(file).Contains("CrashReport", StringComparison.OrdinalIgnoreCase))
                return file;
        }
        return null;
    }

    private static string? SelectSteamApi(IEnumerable<string> candidates)
    {
        return candidates
            .Select(path => new
            {
                Path = path,
                AdjacentExecutableSize = SafeTopLevelFiles(Path.GetDirectoryName(path)!, "*.exe").Select(FileLength).DefaultIfEmpty(0).Max()
            })
            .OrderByDescending(candidate => candidate.AdjacentExecutableSize)
            .ThenBy(candidate => candidate.Path.Length)
            .Select(candidate => candidate.Path)
            .FirstOrDefault();
    }

    private static string? FindUnityExecutable(string unityData)
    {
        string stem = Path.GetFileName(unityData)[..^5];
        string candidate = Path.Combine(Path.GetDirectoryName(unityData)!, stem + ".exe");
        return File.Exists(candidate) ? candidate : null;
    }

    private static string? FindLargestExecutable(string directory) => SafeTopLevelFiles(directory, "*.exe")
        .Where(path => !Path.GetFileName(path).Contains("CrashReport", StringComparison.OrdinalIgnoreCase))
        .OrderByDescending(FileLength)
        .FirstOrDefault();

    private static BackendKind DetectBackends(string root, string? unityData, string executable, string steamDirectory, CancellationToken cancellationToken)
    {
        BackendKind result = BackendKind.None;
        if (unityData is not null)
        {
            string managed = Path.Combine(unityData, "Managed");
            if (Directory.Exists(managed))
            {
                if (File.Exists(Path.Combine(managed, "Fusion.Realtime.dll"))) result |= BackendKind.PhotonFusion;
                if (File.Exists(Path.Combine(managed, "PhotonUnityNetworking.dll")) || File.Exists(Path.Combine(managed, "PhotonRealtime.dll"))) result |= BackendKind.PhotonRealtime;
                if (File.Exists(Path.Combine(managed, "PhotonVoice.dll")) || File.Exists(Path.Combine(managed, "PhotonVoice.PUN.dll"))) result |= BackendKind.PhotonVoice;
                if (SafeTopLevelFiles(managed, "PlayFab*.dll").Any()) result |= BackendKind.PlayFab;
                if (File.Exists(Path.Combine(managed, "Coherence.Toolkit.dll"))) result |= BackendKind.Coherence;
            }

            string metadata = Path.Combine(unityData, "il2cpp_data", "Metadata", "global-metadata.dat");
            if (File.Exists(metadata))
            {
                if (BinaryInspector.ContainsAscii(metadata, "NetworkRunner")) result |= BackendKind.PhotonFusion;
                if (BinaryInspector.ContainsAscii(metadata, "LoadBalancingClient") || BinaryInspector.ContainsAscii(metadata, "PhotonNetwork")) result |= BackendKind.PhotonRealtime;
                if (BinaryInspector.ContainsAscii(metadata, "PhotonVoice")) result |= BackendKind.PhotonVoice;
                if (BinaryInspector.ContainsAscii(metadata, "PlayFabSettings")) result |= BackendKind.PlayFab;
                if (BinaryInspector.ContainsAscii(metadata, "CoherenceBridge")) result |= BackendKind.Coherence;
            }
        }
        else
        {
            if (BinaryInspector.ContainsAscii(executable, "OnlineSubsystemEOS")) result |= BackendKind.Eos;
            // PlayFab in Unreal ships several ways: the OnlineSubsystemPlayFab OSS,
            // or the community/marketplace UE SDK (class names PlayFabClientAPI /
            // PlayFabSettings). UE monoliths compile it in, so there's no PlayFab*.dll
            // to find -- match any of these markers in the shipping exe.
            if (BinaryInspector.ContainsAscii(executable, "OnlineSubsystemPlayFab") ||
                BinaryInspector.ContainsAscii(executable, "PlayFabClientAPI") ||
                BinaryInspector.ContainsAscii(executable, "PlayFabSettings")) result |= BackendKind.PlayFab;
            if (BinaryInspector.ContainsAscii(executable, "PhotonUnityNetworking")) result |= BackendKind.PhotonRealtime;
        }

        cancellationToken.ThrowIfCancellationRequested();
        if (SafeTopLevelFiles(steamDirectory, "EOSSDK*.dll").Any() || SafeFileSystem.EnumerateFiles(root, "EOSSDK-Win64-Shipping.dll").Any() || SafeFileSystem.EnumerateFiles(root, "EOSSDK.dll").Any())
            result |= BackendKind.Eos;
        if (File.Exists(Path.Combine(steamDirectory, "PartyWin.dll")) || SafeTopLevelFiles(steamDirectory, "PlayFab*.dll").Any())
            result |= BackendKind.PlayFab;
        if (SafeFileSystem.EnumerateFiles(root, "combined.schema").Any())
            result |= BackendKind.Coherence;
        return result;
    }

    private static string[] FindCompetingFiles(string root, params string[] directories)
    {
        var found = new HashSet<string>(StringComparer.OrdinalIgnoreCase);
        foreach (string directory in directories.Append(root).Distinct(StringComparer.OrdinalIgnoreCase))
        {
            bool hasSteamFix = File.Exists(Path.Combine(directory, "SteamFix64.dll")) || File.Exists(Path.Combine(directory, "SteamFix64.dll.uco-disabled"));
            bool hasOnlineFix = File.Exists(Path.Combine(directory, "OnlineFix64.dll")) || File.Exists(Path.Combine(directory, "OnlineFix64.dll.uco-disabled"));
            if (!hasSteamFix && !hasOnlineFix)
                continue;

            foreach (string name in CompetingNames)
            {
                string path = Path.Combine(directory, name);
                if (File.Exists(path)) found.Add(path);
            }

            if (hasOnlineFix && (File.Exists(Path.Combine(directory, "OnlineFix.json")) || File.Exists(Path.Combine(directory, "OnlineFix.json.uco-disabled"))))
            {
                foreach (string name in new[] { "Launcher.exe", "OnlineFix.json", "OnlineFix.url", "PhotonBridge.dll" })
                {
                    string path = Path.Combine(directory, name);
                    if (File.Exists(path)) found.Add(path);
                }
            }
        }
        return found.ToArray();
    }

    private static IEnumerable<string> SafeTopLevelFiles(string directory, string pattern)
    {
        try { return Directory.Exists(directory) ? Directory.EnumerateFiles(directory, pattern, SearchOption.TopDirectoryOnly).ToArray() : []; }
        catch (Exception ex) when (ex is IOException or UnauthorizedAccessException) { return []; }
    }

    private static long FileLength(string path)
    {
        try { return new FileInfo(path).Length; }
        catch (Exception ex) when (ex is IOException or UnauthorizedAccessException) { return 0; }
    }
}
