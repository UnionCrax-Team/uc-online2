using System.Text;
using System.Text.RegularExpressions;

namespace UCO2.Patcher.Core;

public static partial class ConfigBuilder
{
    public static string Build(GameScanResult game, PatchOptions options)
    {
        var output = new StringBuilder();
        output.AppendLine("[Settings]");
        output.AppendLine($"AppId={options.AppId}");
        output.AppendLine($"ogAppId={options.OriginalAppId}");
        output.AppendLine("PluginsFolder=plugins");
        output.AppendLine($"GetStubbedLol={Bool(options.EnableSteamStub)}");
        output.AppendLine($"LoadOverlay={Bool(options.LoadOverlay)}");
        output.AppendLine($"LogOverlay={Bool(options.LogOverlay)}");
        output.AppendLine($"WarnOverlayDisabled={Bool(options.WarnOverlayDisabled)}");
        output.AppendLine($"VerboseLog={Bool(options.VerboseLog)}");
        output.AppendLine($"ForceOwnership={Bool(options.ForceOwnership)}");
        output.AppendLine($"PassthroughTicket={Bool(options.PassthroughTicket)}");
        output.AppendLine($"EmulateTicket={Bool(options.EmulateTicket)}");
        output.AppendLine($"SDR={Bool(options.EnableSdr)}");
        output.AppendLine($"InventoryAutoGrant={Bool(options.InventoryAutoGrant)}");
        output.AppendLine($"RealAppIdEnv={Bool(options.RealAppIdEnv)}");
        output.AppendLine($"LocalSaves={Bool(options.LocalSaves)}");
        if (!string.IsNullOrWhiteSpace(options.LegacyClientVersion))
            output.AppendLine($"Client={SingleLine(options.LegacyClientVersion)}");
        foreach ((string key, string value) in options.AdditionalSettings.OrderBy(pair => pair.Key, StringComparer.OrdinalIgnoreCase))
            output.AppendLine($"{SingleLine(key)}={SingleLine(value)}");

        output.AppendLine();
        output.AppendLine("[DLC]");
        output.AppendLine($"UnlockAll={Bool(options.UnlockAllDlc)}");
        // Scanner-discovered DLC first, then the user's manual entries on top so a
        // hand-entered name/id wins over whatever was found in the game folder.
        var dlc = new Dictionary<uint, string>();
        foreach ((uint appId, string name) in FindDlcEntries(game)) dlc[appId] = name;
        foreach ((uint appId, string name) in ParseManualDlc(options.ManualDlc)) dlc[appId] = name;
        foreach (KeyValuePair<uint, string> entry in dlc.OrderBy(pair => pair.Key))
            output.AppendLine($"{entry.Key}={SingleLine(entry.Value)}");

        // Early-load proxy (version.dll) config.
        //   LoadDLLsEarly: when any plugin is deployed, preload steam_api64
        //     before the game runs so the plugin's hooks land before the game
        //     inits its backend (Unity loads steam_api64 lazily).
        //   SdrSafe: when SDR is on, keep the process Steam context on the real
        //     AppId so the overlay's early spacewar load doesn't poison relay auth.
        //   RequireSteam: abort the launch if Steam isn't running (default on).
        // LoadDLLsEarly/SdrSafe default to the auto behavior above but can be forced
        // either way from the GUI (LoadDllsEarly/SdrSafe are null = auto).
        bool anyPlugin = options.InstallPhoton || options.InstallEos
            || options.InstallCoherence || options.InstallPlayFab;
        bool loadEarly = options.LoadDllsEarly ?? anyPlugin;
        bool sdrSafe = options.SdrSafe ?? options.EnableSdr;
        if (loadEarly || sdrSafe || !options.RequireSteam)
        {
            output.AppendLine();
            output.AppendLine("[VersionProxy]");
            if (loadEarly)
                output.AppendLine("LoadDLLsEarly=true");
            if (sdrSafe)
                output.AppendLine("SdrSafe=true");
            if (!options.RequireSteam)
                output.AppendLine("RequireSteam=false");
        }

        if (options.InstallPhoton)
        {
            output.AppendLine();
            bool fusion = game.Backends.HasFlag(BackendKind.PhotonFusion);
            output.AppendLine(fusion ? "[Fusion]" : "[Realtime]");
            if (fusion)
                output.AppendLine($"PhotonAppIdFusion={SingleLine(options.PhotonFusionAppId)}");
            else
                output.AppendLine($"PhotonAppIdRealtime={SingleLine(options.PhotonRealtimeAppId)}");
            if (!string.IsNullOrWhiteSpace(options.PhotonVoiceAppId))
                output.AppendLine($"PhotonAppIdVoice={SingleLine(options.PhotonVoiceAppId)}");
            if (!string.IsNullOrWhiteSpace(options.PhotonNickname))
                output.AppendLine($"Nickname={SingleLine(options.PhotonNickname)}");
            output.AppendLine("ForcedAuthType=0");
            if (options.PhotonVerboseLog)
                output.AppendLine("VerboseLog=1");
        }

        if (options.InstallEos)
        {
            output.AppendLine();
            output.AppendLine("[EOS]");
            if (options.EosKeepGameApp)
            {
                // Device ID login on the game's OWN Epic app -- no app credentials needed.
                output.AppendLine("KeepGameApp=1");
            }
            else
            {
                output.AppendLine($"ProductId={SingleLine(options.EosProductId)}");
                output.AppendLine($"SandboxId={SingleLine(options.EosSandboxId)}");
                output.AppendLine($"DeploymentId={SingleLine(options.EosDeploymentId)}");
                output.AppendLine($"ClientId={SingleLine(options.EosClientId)}");
                output.AppendLine($"ClientSecret={SingleLine(options.EosClientSecret)}");
            }
            // Non-presence lobbies + integrated-platform off. Independent of the redirect
            // vs KeepGameApp choice above, so written for either.
            if (options.EosNoPresence)
                output.AppendLine("NoPresence=1");
            if (options.EosVerboseLog)
                output.AppendLine("VerboseLog=1");
            output.AppendLine($"DisplayName={SingleLine(options.DisplayName)}");
        }

        if (options.InstallCoherence)
        {
            output.AppendLine();
            output.AppendLine("[Coherence]");
            output.AppendLine("ForceGuestLogin=true");
            output.AppendLine($"RuntimeKey={SingleLine(options.CoherenceRuntimeKey)}");
            if (!string.IsNullOrWhiteSpace(options.CoherenceProjectId))
                output.AppendLine($"ProjectId={SingleLine(options.CoherenceProjectId)}");
            output.AppendLine("LocalMode=false");
            if (options.CoherenceLaunchReplicationServer)
                output.AppendLine("LaunchReplicationServer=true");
        }

        if (options.InstallPlayFab)
        {
            output.AppendLine();
            output.AppendLine("[PlayFab]");
            if (options.PlayFabKeepGameTitle)
                output.AppendLine("KeepGameTitle=1");
            if (!string.IsNullOrWhiteSpace(options.PlayFabTitleId))
                output.AppendLine($"TitleId={SingleLine(options.PlayFabTitleId)}");
            if (options.PlayFabVerboseLog)
                output.AppendLine("VerboseLog=1");
        }

        string merged = MergeAdvancedIni(output.ToString(), options.AdvancedIni);
        return merged.Replace("\r\n", "\n").Replace("\n", Environment.NewLine);
    }

    // Merge the raw "Advanced ini" box into the generated config. Lines under a
    // [Section] header target that section; a key with no header goes to [Settings].
    // An existing key is overridden in place; a new key is appended to its section;
    // a section not already present is added at the end. One header per section, so
    // GetPrivateProfileString (first-section-wins) still reads every value.
    private static string MergeAdvancedIni(string baseIni, string? advanced)
    {
        if (string.IsNullOrWhiteSpace(advanced)) return baseIni;

        var extras = new List<(string Section, string Key, string Value)>();
        string current = "Settings";
        foreach (string rawLine in advanced.Replace("\r\n", "\n").Split('\n'))
        {
            string line = rawLine.Trim();
            if (line.Length == 0 || line.StartsWith(';') || line.StartsWith('#')) continue;
            if (line.StartsWith('[') && line.EndsWith(']')) { current = line[1..^1].Trim(); continue; }
            int eq = line.IndexOf('=');
            if (eq <= 0) continue;   // ignore malformed lines
            extras.Add((current, line[..eq].Trim(), line[(eq + 1)..].Trim()));
        }
        if (extras.Count == 0) return baseIni;

        var lines = baseIni.Replace("\r\n", "\n").Split('\n').ToList();

        static bool IsHeader(string l) => l.TrimStart().StartsWith('[') && l.TrimEnd().EndsWith(']');
        static string HeaderName(string l) { string t = l.Trim(); return t[1..^1].Trim(); }
        static string KeyName(string l) { int i = l.IndexOf('='); return i <= 0 ? "" : l[..i].Trim(); }

        foreach ((string section, string key, string value) in extras)
        {
            int header = lines.FindIndex(l => IsHeader(l) &&
                HeaderName(l).Equals(section, StringComparison.OrdinalIgnoreCase));
            if (header < 0)
            {
                if (lines.Count > 0 && lines[^1].Trim().Length != 0) lines.Add("");
                lines.Add($"[{section}]");
                lines.Add($"{key}={value}");
                continue;
            }
            int end = header + 1;
            while (end < lines.Count && !IsHeader(lines[end])) end++;
            int keyIdx = -1;
            for (int i = header + 1; i < end; i++)
                if (KeyName(lines[i]).Equals(key, StringComparison.OrdinalIgnoreCase)) { keyIdx = i; break; }
            if (keyIdx >= 0)
            {
                lines[keyIdx] = $"{key}={value}";
            }
            else
            {
                int insertAt = end;
                while (insertAt - 1 > header && lines[insertAt - 1].Trim().Length == 0) insertAt--;
                lines.Insert(insertAt, $"{key}={value}");
            }
        }
        return string.Join("\n", lines);
    }

    // Manual [DLC] entries typed in the GUI: one "appId=name" per line, name optional
    // (blank names become "DLC <appId>"). Blank lines and ; / # comments are ignored,
    // as are lines whose id isn't a positive number.
    private static IReadOnlyList<(uint AppId, string Name)> ParseManualDlc(string? text)
    {
        var list = new List<(uint, string)>();
        if (string.IsNullOrWhiteSpace(text)) return list;
        foreach (string raw in text.Replace("\r\n", "\n").Split('\n'))
        {
            string line = raw.Trim();
            if (line.Length == 0 || line.StartsWith(';') || line.StartsWith('#')) continue;
            int eq = line.IndexOf('=');
            string idPart = eq >= 0 ? line[..eq].Trim() : line;
            string name = eq >= 0 ? line[(eq + 1)..].Trim() : "";
            if (!uint.TryParse(idPart, out uint appId) || appId == 0) continue;
            list.Add((appId, name.Length > 0 ? name : $"DLC {appId}"));
        }
        return list;
    }

    private static IReadOnlyList<(uint AppId, string Name)> FindDlcEntries(GameScanResult game)
    {
        string? config = SafeFileSystem.EnumerateFiles(game.GameDirectory, "configs.app.ini").FirstOrDefault();
        var entries = new Dictionary<uint, string>();
        if (config is not null)
        {
            try
            {
                foreach (string line in File.ReadLines(config))
                {
                    Match match = DlcLineRegex().Match(line);
                    if (match.Success && uint.TryParse(match.Groups["id"].Value, out uint appId))
                        entries[appId] = match.Groups["name"].Value.Trim();
                }
            }
            catch (Exception ex) when (ex is IOException or UnauthorizedAccessException) { }
        }

        if (entries.Count == 0)
        {
            try
            {
                foreach (string directory in Directory.EnumerateDirectories(game.ConfigDirectory, "*", SearchOption.TopDirectoryOnly))
                {
                    if (uint.TryParse(Path.GetFileName(directory), out uint appId))
                        entries[appId] = $"DLC {appId}";
                }
            }
            catch (Exception ex) when (ex is IOException or UnauthorizedAccessException) { }
        }
        return entries.Select(pair => (pair.Key, pair.Value)).ToArray();
    }

    private static string SingleLine(string? value) => (value ?? "").Replace("\r", "").Replace("\n", "").Trim();
    private static string Bool(bool value) => value ? "true" : "false";

    [GeneratedRegex("^\\s*(?<id>[0-9]+)\\s*=\\s*(?<name>.*)$")]
    private static partial Regex DlcLineRegex();
}
