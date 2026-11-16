using System.IO.Compression;
using System.Text;
using UCO2.Patcher.Core;

namespace UCO2.Patcher.Tests;

internal static class Program
{
    private static int passed;

    public static async Task<int> Main(string[] args)
    {
        if (args.Length == 2 && args[0].Equals("--scan", StringComparison.OrdinalIgnoreCase))
        {
            GameScanResult result = await new GameScanner().ScanAsync(args[1]);
            Console.WriteLine($"{result.EngineLabel}|{result.ExecutablePath}|{result.SteamApiPath}|{result.BackendLabel}");
            return 0;
        }

        string root = Path.Combine(Path.GetTempPath(), "UCO2.Patcher.Tests", Guid.NewGuid().ToString("N"));
        Directory.CreateDirectory(root);
        try
        {
            TestSteamVdfParsing();
            TestSteamStoreSearch();
            TestConfigFlags(root);
            TestBackendProfiles(root);
            TestBackendValidation(root);
            TestPlayFabPlanning(root);
            TestEosPlanning(root);
            TestSettingsFlagsAndManualDlc(root);
            TestEosNoPresenceConfig(root);
            TestPhotonVerboseConfig(root);
            TestAdvancedAndProxyConfig(root);
            await TestSelfUpdateLayout(root);
            await TestBackupRestoreAndPackage(root);
            Console.WriteLine($"PASS: {passed} tests");
            return 0;
        }
        catch (Exception ex)
        {
            Console.Error.WriteLine("FAIL: " + ex);
            return 1;
        }
        finally
        {
            try { Directory.Delete(root, recursive: true); } catch { }
        }
    }

    private static void TestSteamStoreSearch()
    {
        Equal("Phasmophobia", SteamStoreSearchService.BuildSearchTerm(@"C:\Games\Phasmophobia.Build.24434979"), "Release suffix cleanup");
        const string json = """
            {"total":2,"items":[
              {"type":"app","name":"Schedule I","id":3164500,"tiny_image":"https://example.invalid/one.jpg"},
              {"type":"app","name":"Schedule I: Free Sample","id":3205720}
            ]}
            """;
        using var stream = new MemoryStream(Encoding.UTF8.GetBytes(json));
        IReadOnlyList<SteamSearchResult> results = SteamStoreSearchService.ParseResults(stream);
        Equal(2, results.Count, "Steam result count");
        Equal((uint)3164500, results[0].AppId, "Steam result AppId");
        Equal((uint)3164500, SteamStoreSearchService.FindBestMatch(@"C:\Games\Schedule.I", results)!.AppId, "Exact Steam match");

        const string detailsJson = """
            {"3164500":{"success":true,"data":{"type":"game","name":"Schedule I","header_image":"https://example.invalid/header.jpg"}}}
            """;
        using var detailsStream = new MemoryStream(Encoding.UTF8.GetBytes(detailsJson));
        SteamSearchResult? details = SteamStoreSearchService.ParseAppDetails(detailsStream, 3164500);
        Equal("Schedule I", details!.Name, "Manual AppId name");
        Equal((uint)3164500, details.AppId, "Manual AppId result");
        passed++;
    }

    private static void TestSteamVdfParsing()
    {
        const string vdf = "\"libraryfolders\"\n{\n \"0\" { \"path\" \"C:\\\\Program Files (x86)\\\\Steam\" }\n \"1\" { \"path\" \"D:\\\\Steam Library\" }\n}";
        IReadOnlyList<string> paths = SteamLibraryService.ParseLibraryFolders(vdf);
        Equal(2, paths.Count, "VDF library count");
        True(paths.Contains(@"D:\Steam Library", StringComparer.OrdinalIgnoreCase), "VDF escaped path");

        Dictionary<string, string> app = SteamLibraryService.ParseFlatVdf("\"appid\" \"480\"\n\"name\" \"Spacewar\"\n\"installdir\" \"Spacewar\"");
        Equal("Spacewar", app["name"], "Manifest name");
        passed++;
    }

    private static void TestConfigFlags(string root)
    {
        GameScanResult game = FakeGame(root);
        var options = new PatchOptions
        {
            OriginalAppId = 123,
            InstallOverlayProxy = false,
            EnableSteamStub = true,
            LoadOverlay = false,
            PassthroughTicket = true,
            LegacyClientVersion = "017",
            VerboseLog = true
        };
        options.AdditionalSettings["CustomFlag"] = "yes";
        string config = ConfigBuilder.Build(game, options);
        True(config.Contains("GetStubbedLol=true"), "SteamStub flag");
        True(config.Contains("LoadOverlay=false"), "Overlay flag");
        True(config.Contains("PassthroughTicket=true"), "Passthrough flag");
        True(config.Contains("Client=017"), "Client flag");
        True(config.Contains("CustomFlag=yes"), "Custom flag");
        passed++;
    }

    // [Settings] RealAppIdEnv / LocalSaves + the manual [DLC] box: parsing,
    // and manual-wins-over-scanner.
    private static void TestSettingsFlagsAndManualDlc(string root)
    {
        GameScanResult game = FakeGame(root);

        // Written either way, so re-patching can turn it back off.
        True(ConfigBuilder.Build(game, new PatchOptions { OriginalAppId = 892970, InstallOverlayProxy = false, RealAppIdEnv = true })
            .Contains("RealAppIdEnv=true"), "RealAppIdEnv=true written");
        True(ConfigBuilder.Build(game, new PatchOptions { OriginalAppId = 892970, InstallOverlayProxy = false })
            .Contains("RealAppIdEnv=false"), "RealAppIdEnv off by default");

        // LocalSaves is the one [Settings] toggle that defaults ON, so the default-
        // constructed options must write it as true and an explicit false must stick.
        True(ConfigBuilder.Build(game, new PatchOptions { OriginalAppId = 1, InstallOverlayProxy = false })
            .Contains("LocalSaves=true"), "LocalSaves on by default");
        True(ConfigBuilder.Build(game, new PatchOptions { OriginalAppId = 1, InstallOverlayProxy = false, LocalSaves = false })
            .Contains("LocalSaves=false"), "LocalSaves can be turned off");

        // Manual entries: "id=name", a bare id, and junk/comments/zero dropped.
        string cfg = ConfigBuilder.Build(game, new PatchOptions
        {
            OriginalAppId = 123,
            InstallOverlayProxy = false,
            ManualDlc = "   \n; a comment\n# another\n2000 = Deluxe Pack \n2001\nnot-a-number=x\n0=zero\n",
        });
        True(cfg.Contains("2000=Deluxe Pack"), "manual DLC id=name (trimmed)");
        True(cfg.Contains("2001=DLC 2001"), "bare manual DLC id gets a placeholder name");
        True(!cfg.Contains("not-a-number") && !cfg.Contains("comment"), "junk and comments dropped");
        True(!cfg.Contains("0=zero"), "appId 0 dropped");

        // A manual entry overrides the same id discovered by the scanner.
        string dlcRoot = Path.Combine(root, "manual-dlc");
        Directory.CreateDirectory(dlcRoot);
        File.WriteAllText(Path.Combine(dlcRoot, "configs.app.ini"), "3000=Scanned Name\n3001=Kept Name\n");
        GameScanResult scanned = FakeGame(dlcRoot);
        string merged = ConfigBuilder.Build(scanned, new PatchOptions
        {
            OriginalAppId = 123,
            InstallOverlayProxy = false,
            ManualDlc = "3000=Manual Name",
        });
        True(merged.Contains("3000=Manual Name") && !merged.Contains("Scanned Name"), "manual DLC beats the scanner");
        True(merged.Contains("3001=Kept Name"), "scanner DLC kept when not overridden");
        passed++;
    }

    // [Realtime] VerboseLog: written into the Photon section only when asked for.
    private static void TestPhotonVerboseConfig(string root)
    {
        GameScanResult game = FakeGame(root);
        static string PhotonSection(string cfg)
        {
            int start = cfg.IndexOf("[Realtime]", StringComparison.Ordinal);
            if (start < 0) return "";
            int next = cfg.IndexOf("\n[", start + 1, StringComparison.Ordinal);
            return next < 0 ? cfg[start..] : cfg[start..next];
        }

        string on = ConfigBuilder.Build(game, new PatchOptions
        {
            OriginalAppId = 123, InstallOverlayProxy = false, InstallPhoton = true,
            PhotonRealtimeAppId = "rt", PhotonVerboseLog = true,
        });
        True(PhotonSection(on).Contains("VerboseLog=1"), "Photon VerboseLog written under [Realtime]");

        string off = ConfigBuilder.Build(game, new PatchOptions
        {
            OriginalAppId = 123, InstallOverlayProxy = false, InstallPhoton = true, PhotonRealtimeAppId = "rt",
        });
        True(PhotonSection(off).Length > 0 && !PhotonSection(off).Contains("VerboseLog"), "Photon VerboseLog absent by default");
        passed++;
    }

    // [EOS] NoPresence: written under [EOS] when set (either login mode), absent otherwise.
    private static void TestEosNoPresenceConfig(string root)
    {
        GameScanResult game = FakeGame(root);

        var keep = new PatchOptions
        {
            OriginalAppId = 123,
            InstallOverlayProxy = false,
            InstallEos = true,
            EosKeepGameApp = true,
            EosNoPresence = true,
        };
        string keepCfg = ConfigBuilder.Build(game, keep);
        True(keepCfg.Contains("KeepGameApp=1") && keepCfg.Contains("NoPresence=1"),
            "NoPresence written alongside KeepGameApp");

        var off = new PatchOptions
        {
            OriginalAppId = 123,
            InstallOverlayProxy = false,
            InstallEos = true,
            EosKeepGameApp = true,
        };
        True(!ConfigBuilder.Build(game, off).Contains("NoPresence"),
            "NoPresence absent when the flag is off");
        passed++;
    }

    // [VersionProxy] auto/override + the "Advanced ini" raw escape-hatch merge.
    private static void TestAdvancedAndProxyConfig(string root)
    {
        GameScanResult game = FakeGame(root);

        // LoadDLLsEarly auto-on when a plugin is deployed; forced off overrides it.
        var auto = new PatchOptions { OriginalAppId = 1, InstallOverlayProxy = false, InstallEos = true, EosKeepGameApp = true };
        True(ConfigBuilder.Build(game, auto).Contains("LoadDLLsEarly=true"), "LoadDLLsEarly auto-on with a plugin");
        var off = new PatchOptions { OriginalAppId = 1, InstallOverlayProxy = false, InstallEos = true, EosKeepGameApp = true, LoadDllsEarly = false };
        True(!ConfigBuilder.Build(game, off).Contains("LoadDLLsEarly=true"), "LoadDLLsEarly forced off");

        // RequireSteam only written when explicitly turned off.
        True(ConfigBuilder.Build(game, new PatchOptions { OriginalAppId = 1, InstallOverlayProxy = false, RequireSteam = false }).Contains("RequireSteam=false"),
            "RequireSteam=false written");
        True(!ConfigBuilder.Build(game, new PatchOptions { OriginalAppId = 1, InstallOverlayProxy = false }).Contains("RequireSteam"),
            "RequireSteam omitted by default");

        // Advanced ini: overrides an existing key in place, merges into an existing
        // generated section (one header), and appends a brand-new section.
        var adv = new PatchOptions
        {
            AppId = 480, OriginalAppId = 1, InstallOverlayProxy = false,
            InstallPlayFab = true, PlayFabTitleId = "ABCD",
            AdvancedIni = "AppId=999\n[PlayFab]\nRedirectNativeHttp=1\n[Custom]\nFoo=bar"
        };
        string advCfg = ConfigBuilder.Build(game, adv);
        True(advCfg.Contains("AppId=999") && !advCfg.Contains("AppId=480"), "Advanced overrides AppId in place");
        True(advCfg.Contains("RedirectNativeHttp=1") && advCfg.Contains("TitleId=ABCD"), "Advanced key merged into existing [PlayFab]");
        True(advCfg.Split("[PlayFab]").Length - 1 == 1, "single [PlayFab] header after merge");
        True(advCfg.Contains("[Custom]") && advCfg.Contains("Foo=bar"), "Advanced new section appended");
        passed++;
    }

    private static void TestBackendProfiles(string root)
    {
        string path = Path.Combine(root, "backend-profiles.dat");
        var store = new BackendProfileStore(path);
        var options = new PatchOptions
        {
            OriginalAppId = 123,
            InstallPhoton = true,
            PhotonRealtimeAppId = "photon-realtime",
            PhotonVoiceAppId = "photon-voice",
            InstallEos = true,
            EosProductId = "eos-product",
            EosSandboxId = "eos-sandbox",
            EosDeploymentId = "eos-deployment",
            EosClientId = "eos-client",
            EosClientSecret = "super-secret",
            DisplayName = "Player One",
            InstallPlayFab = true,
            PlayFabTitleId = "ABCDE"
        };

        IReadOnlyList<string> saved = store.SaveFrom(options);
        Equal(3, saved.Count, "Saved backend profile count");
        True(!Encoding.UTF8.GetString(File.ReadAllBytes(path)).Contains("super-secret", StringComparison.Ordinal), "Backend profile encrypted at rest");

        BackendProfiles profiles = store.Load(out string? warning);
        True(warning is null, "Backend profile decrypt warning");
        Equal("photon-realtime", profiles.Photon!.RealtimeAppId, "Remembered Photon AppId");
        Equal("super-secret", profiles.Eos!.ClientSecret, "Remembered EOS secret");
        Equal("ABCDE", profiles.PlayFab!.TitleId, "Remembered PlayFab TitleId");
        passed++;
    }

    private static void TestBackendValidation(string root)
    {
        GameScanResult game = FakeGame(root);
        var selected = new PatchOptions
        {
            InstallPhoton = true,
            InstallEos = true,
            InstallPlayFab = true
        };
        IReadOnlyList<MissingBackendSettings> missing = BackendSettingsValidator.FindMissing(game, selected);
        // EOS creds are optional now (blank -> baked-in default app), so only
        // Photon + PlayFab are missing here.
        Equal(2, missing.Count, "Photon+PlayFab require settings; EOS is optional");
        True(missing.Any(item => item.Backend == "Photon"), "Photon requirement");
        True(!missing.Any(item => item.Backend == "EOS"), "Blank EOS is not required (built-in default)");
        True(missing.Any(item => item.Backend == "PlayFab"), "PlayFab requirement");

        // A PARTIAL EOS own-app block IS flagged (plugin ignores it -> default).
        var partialEos = new PatchOptions { InstallEos = true, EosProductId = "abc" };
        True(BackendSettingsValidator.FindMissing(game, partialEos).Any(item => item.Backend == "EOS"),
            "Partial EOS block is flagged");

        var disabled = new PatchOptions();
        Equal(0, BackendSettingsValidator.FindMissing(game, disabled).Count, "Disabled backends require no settings");
        passed++;
    }

    private static void TestPlayFabPlanning(string root)
    {
        string artifactRoot = Path.Combine(root, "PlannerArtifacts");
        string gameRoot = Path.Combine(root, "PlannerGame");
        Directory.CreateDirectory(Path.Combine(artifactRoot, "x64"));
        Directory.CreateDirectory(Path.Combine(artifactRoot, "plugins"));
        Directory.CreateDirectory(gameRoot);
        File.WriteAllBytes(Path.Combine(artifactRoot, "x64", "steam_api64.dll"), [1]);
        File.WriteAllBytes(Path.Combine(artifactRoot, "plugins", "playfab_universal.dll"), [2]);

        var planner = new PatchPlanner(new ArtifactLocator(artifactRoot));
        var options = new PatchOptions
        {
            OriginalAppId = 123,
            InstallOverlayProxy = false,
            InstallPlayFab = true
        };

        PatchPlan incomplete = planner.Create(FakeGame(gameRoot), options);
        True(incomplete.Warnings.Any(warning => warning.Contains("TitleId is empty", StringComparison.Ordinal)), "Empty PlayFab TitleId warning");
        True(!incomplete.Operations.Any(operation => operation.Description.Contains("playfab_universal", StringComparison.Ordinal)), "Empty PlayFab TitleId skips plugin");

        options.PlayFabTitleId = "ABCDE";
        PatchPlan complete = planner.Create(FakeGame(gameRoot), options);
        True(complete.Operations.Any(operation => operation.Description.Contains("playfab_universal", StringComparison.Ordinal)), "Configured PlayFab plugin planned");
        True(!complete.Warnings.Any(warning => warning.Contains("TitleId", StringComparison.Ordinal)), "Configured PlayFab has no TitleId warning");
        passed++;
    }

    private static void TestEosPlanning(string root)
    {
        string artifactRoot = Path.Combine(root, "EosArtifacts");
        string gameRoot = Path.Combine(root, "EosGame");
        Directory.CreateDirectory(Path.Combine(artifactRoot, "x64"));
        Directory.CreateDirectory(Path.Combine(artifactRoot, "plugins"));
        Directory.CreateDirectory(gameRoot);
        File.WriteAllBytes(Path.Combine(artifactRoot, "x64", "steam_api64.dll"), [1]);
        File.WriteAllBytes(Path.Combine(artifactRoot, "plugins", "EOS_custom.dll"), [2]);

        var planner = new PatchPlanner(new ArtifactLocator(artifactRoot));

        // EOS selected, NO credentials -> plugin installs (uses the baked-in default
        // Epic app) with no warning.
        var bare = new PatchOptions { OriginalAppId = 123, InstallOverlayProxy = false, InstallEos = true };
        PatchPlan bareplan = planner.Create(FakeGame(gameRoot), bare);
        True(bareplan.Operations.Any(operation => operation.Description.Contains("EOS_custom", StringComparison.Ordinal)), "No-cred EOS installs the plugin (baked default)");
        True(!bareplan.Warnings.Any(warning => warning.Contains("incomplete", StringComparison.Ordinal)), "No-cred EOS raises no incomplete warning");

        // A PARTIAL own-app block -> plugin still installs, but warns it will be ignored.
        var partial = new PatchOptions { OriginalAppId = 123, InstallOverlayProxy = false, InstallEos = true, EosProductId = "p" };
        PatchPlan partialPlan = planner.Create(FakeGame(gameRoot), partial);
        True(partialPlan.Operations.Any(operation => operation.Description.Contains("EOS_custom", StringComparison.Ordinal)), "Partial EOS still installs the plugin");
        True(partialPlan.Warnings.Any(warning => warning.Contains("incomplete", StringComparison.Ordinal)), "Partial EOS warns it will use the default");

        // KeepGameApp anon-logs into the game's own Epic app, so the plugin must be
        // installed WITHOUT redirect credentials and without a warning. (regression
        // guard: PatchPlanner used to require all five ids and skipped this path.)
        var keep = new PatchOptions { OriginalAppId = 123, InstallOverlayProxy = false, InstallEos = true, EosKeepGameApp = true };
        PatchPlan kept = planner.Create(FakeGame(gameRoot), keep);
        True(kept.Operations.Any(operation => operation.Description.Contains("EOS_custom", StringComparison.Ordinal)), "KeepGameApp installs EOS_custom without credentials");
        True(!kept.Warnings.Any(warning => warning.Contains("credentials are incomplete", StringComparison.Ordinal)), "KeepGameApp raises no credentials warning");

        // Full redirect credentials -> plugin planned.
        var full = new PatchOptions
        {
            OriginalAppId = 123,
            InstallOverlayProxy = false,
            InstallEos = true,
            EosProductId = "p",
            EosSandboxId = "s",
            EosDeploymentId = "d",
            EosClientId = "c",
            EosClientSecret = "x"
        };
        PatchPlan complete = planner.Create(FakeGame(gameRoot), full);
        True(complete.Operations.Any(operation => operation.Description.Contains("EOS_custom", StringComparison.Ordinal)), "Configured EOS redirect plugin planned");
        passed++;
    }

    private static async Task TestSelfUpdateLayout(string root)
    {
        string sourceRoot = Path.Combine(root, "UpdateSource");
        string packageRoot = Path.Combine(sourceRoot, "uc-online2-v9.9.9-release");
        Directory.CreateDirectory(Path.Combine(packageRoot, "x64"));
        Directory.CreateDirectory(Path.Combine(packageRoot, "x86"));
        Directory.CreateDirectory(Path.Combine(packageRoot, "plugins"));
        File.WriteAllText(Path.Combine(packageRoot, "UCO2.Patcher.exe"), "new-patcher");
        File.WriteAllText(Path.Combine(packageRoot, "x64", "steam_api64.dll"), "new-x64");
        File.WriteAllText(Path.Combine(packageRoot, "x86", "steam_api.dll"), "new-x86");
        File.WriteAllText(Path.Combine(packageRoot, "plugins", "playfab_universal.dll"), "new-plugin");

        string archive = Path.Combine(root, "update package.zip");
        ZipFile.CreateFromDirectory(sourceRoot, archive);
        string executableDirectory = Path.Combine(root, "Nested Executable");
        string artifactDirectory = Path.Combine(root, "Artifact Root");
        Directory.CreateDirectory(executableDirectory);
        Directory.CreateDirectory(artifactDirectory);
        File.WriteAllText(Path.Combine(executableDirectory, "UCO2.Patcher.exe"), "old-patcher");

        string installedExecutable = await SelfUpdateService.InstallPackageAsync(
            archive, executableDirectory, artifactDirectory, "UCO2.Patcher.exe", "v9.9.9");

        Equal("new-patcher", File.ReadAllText(installedExecutable), "Updater replaces nested executable");
        Equal("new-x64", File.ReadAllText(Path.Combine(artifactDirectory, "x64", "steam_api64.dll")), "Updater replaces x64 Steam API");
        Equal("new-x86", File.ReadAllText(Path.Combine(artifactDirectory, "x86", "steam_api.dll")), "Updater replaces x86 Steam API");
        Equal("new-plugin", File.ReadAllText(Path.Combine(artifactDirectory, "plugins", "playfab_universal.dll")), "Updater replaces plugin DLLs");

        string gameWithSpaces = Path.Combine(root, "Game With Spaces");
        var startInfo = SelfUpdateService.CreateUpdaterStartInfo(
            archive, installedExecutable, artifactDirectory, 42, gameWithSpaces, "v9.9.9");
        Equal(8, startInfo.ArgumentList.Count, "Updater argument count");
        Equal(archive, startInfo.ArgumentList[1], "Updater archive path with spaces");
        Equal(artifactDirectory, startInfo.ArgumentList[3], "Updater artifact path with spaces");
        Equal(gameWithSpaces, startInfo.ArgumentList[6], "Updater game path with spaces");
        try { Directory.Delete(Path.GetDirectoryName(startInfo.FileName)!, recursive: true); } catch { }
        passed++;
    }

    private static async Task TestBackupRestoreAndPackage(string root)
    {
        string gameRoot = Path.Combine(root, "Game");
        string sourceRoot = Path.Combine(root, "Source");
        Directory.CreateDirectory(Path.Combine(gameRoot, "bin"));
        Directory.CreateDirectory(sourceRoot);
        string target = Path.Combine(gameRoot, "bin", "steam_api64.dll");
        string source = Path.Combine(sourceRoot, "steam_api64.dll");
        await File.WriteAllTextAsync(target, "original");
        await File.WriteAllTextAsync(source, "replacement");

        var plan = new PatchPlan
        {
            Game = FakeGame(gameRoot),
            Options = new PatchOptions { OriginalAppId = 123 },
            Operations =
            [
                new PatchOperation { Kind = PatchOperationKind.ReplaceFile, SourcePath = source, TargetPath = target, Description = "Replace Steam API" },
                new PatchOperation { Kind = PatchOperationKind.WriteText, TextContent = "[Settings]\nAppId=480\n", TargetPath = Path.Combine(gameRoot, "union-crax.ini"), Description = "Write config" }
            ]
        };

        string backupRoot = Path.Combine(root, "Backups");
        var backups = new BackupService(backupRoot);
        BackupManifest manifest = await backups.ApplyAsync(plan);
        Equal("replacement", await File.ReadAllTextAsync(target), "Replacement applied");
        True(File.Exists(Path.Combine(gameRoot, "union-crax.ini")), "Created file applied");

        BackupSnapshot snapshot = (await backups.ListAsync(gameRoot)).Single();
        var packager = new FixPackager();
        FixPackageResult package = await packager.CreateAsync(snapshot.ManifestPath);
        using (ZipArchive zip = ZipFile.OpenRead(package.ArchivePath))
        {
            True(zip.GetEntry("bin/steam_api64.dll") is not null, "Package preserves nested path");
            True(zip.GetEntry("union-crax.ini") is not null, "Package contains config");
            True(zip.GetEntry("uco2-fix-manifest.json") is not null, "Package manifest");
        }

        await backups.RestoreAsync(snapshot.ManifestPath);
        Equal("original", await File.ReadAllTextAsync(target), "Original restored");
        True(!File.Exists(Path.Combine(gameRoot, "union-crax.ini")), "Created file removed on restore");
        passed++;
    }

    private static GameScanResult FakeGame(string root) => new()
    {
        GameDirectory = root,
        Engine = EngineKind.Generic,
        Architecture = GameArchitecture.X64,
        SteamApiPath = Path.Combine(root, "bin", "steam_api64.dll"),
        ExecutablePath = Path.Combine(root, "bin", "game.exe"),
        ConfigDirectory = root,
        SteamStub = SteamStubStatus.NotDetected
    };

    private static void True(bool value, string name)
    {
        if (!value) throw new InvalidOperationException($"Assertion failed: {name}");
    }

    private static void Equal<T>(T expected, T actual, string name) where T : notnull
    {
        if (!EqualityComparer<T>.Default.Equals(expected, actual))
            throw new InvalidOperationException($"Assertion failed: {name}. Expected {expected}, got {actual}.");
    }
}
