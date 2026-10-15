using System.Diagnostics;
using System.IO;
using System.Security.Principal;
using System.Windows;
using Microsoft.Win32;
using UCO2.Patcher.Core;

namespace UCO2.Patcher.Gui;

public partial class MainWindow : Window
{
    private readonly SteamStoreSearchService steamStore = new();
    private readonly GameScanner scanner = new();
    private readonly ArtifactLocator artifacts;
    private readonly BackupService backups = new();
    private readonly InstalledFixService installedFixes = new();
    private readonly FixPackager packager = new();
    private readonly BackendProfileStore backendProfiles = new();
    private readonly string? initialGameDirectory;
    private bool refreshFixAfterUpdate;
    private bool loadingSteamMatches;
    private bool busy;
    private bool advancedExpandedForBackendNotice;
    private string normalStatus = "Choose a game folder to begin";
    private CancellationTokenSource? appIdLookupCancellation;
    private GameScanResult? scan;
    private PatchPlan? plan;

    public MainWindow(string? initialGameDirectory = null, bool refreshFixAfterUpdate = false)
    {
        InitializeComponent();
        this.initialGameDirectory = initialGameDirectory;
        this.refreshFixAfterUpdate = refreshFixAfterUpdate;
        artifacts = new ArtifactLocator(FindArtifactRoot());
        VersionText.Text = $"Version {artifacts.CurrentVersion}  |  {(IsAdministrator() ? "Administrator" : "Standard user")}";
        Loaded += MainWindow_Loaded;
    }

    private async void MainWindow_Loaded(object sender, RoutedEventArgs e)
    {
        if (!string.IsNullOrWhiteSpace(initialGameDirectory) && Directory.Exists(initialGameDirectory))
            await ScanGameAsync(initialGameDirectory);
        await CheckForUpdatesAsync(interactive: false);
    }

    private async Task ScanGameAsync(string directory)
    {
        SetBusy(true, "Scanning game files...");
        try
        {
            scan = await scanner.ScanAsync(directory);
            string folderName = Path.GetFileName(Path.TrimEndingDirectorySeparator(directory));
            SelectedTitle.Text = folderName;
            SelectedPath.Text = scan.GameDirectory;
            FolderPathBox.Text = scan.GameDirectory;
            FolderHintText.Text = "Scan complete";
            EngineValue.Text = scan.EngineLabel;
            ExecutableValue.Text = scan.ExecutablePath;
            SteamApiValue.Text = scan.SteamApiPaths.Count > 1
                ? $"{scan.SteamApiPath}  (+{scan.SteamApiPaths.Count - 1} more — all replaced)"
                : scan.SteamApiPath;
            ConfigValue.Text = scan.ConfigDirectory;
            ServicesValue.Text = scan.BackendLabel;
            SteamStubValue.Text = scan.SteamStub.ToString();
            WarningList.ItemsSource = scan.Warnings;

            loadingSteamMatches = true;
            LoadConfiguration(scan, null);
            loadingSteamMatches = false;
            SelectDetectedBackendPlugins(scan);
            await SearchSteamAsync(scan.GameDirectory);
            OfferSavedBackendSettings();
            await RefreshBackupsAsync();
            RefreshPlan(selectChangesTab: false);
            ApplyButton.IsEnabled = true;
            Log($"Scanned {scan.GameDirectory}: {scan.EngineLabel}, {scan.BackendLabel}.");
            SetNormalStatus("Preflight complete");

            UpdateBackendPrompts();
            IReadOnlyList<MissingBackendSettings> missingBackends = GetMissingBackendSettings();
            if (missingBackends.Count > 0)
            {
                ExpandAdvancedForBackendNotice();
                string names = string.Join(", ", missingBackends.Select(item => item.Backend));
                SetNormalStatus($"{names} detected - fill the required plugin fields or turn those plugins off.");
                Log($"Required backend settings are missing for {names}. Fill them under Advanced settings or disable those plugins.");
            }

            if (scan.Backends.HasFlag(BackendKind.Coherence))
                OfferCoherenceSchemaUpload();

            if (refreshFixAfterUpdate)
            {
                refreshFixAfterUpdate = false;
                if (NativeDialog.Confirm(this, "Update installed fix",
                        "The patcher was updated and its bundled DLLs may be newer. Back up and refresh this installed fix now?"))
                    await ApplyPlanAsync("Update installed fix");
            }
        }
        catch (Exception ex)
        {
            plan = null;
            UpdateFixButton.IsEnabled = false;
            PackageButton.IsEnabled = false;
            if (scan is null)
            {
                ApplyButton.IsEnabled = false;
                ShowError("Game scan failed", ex);
            }
            else
            {
                ApplyButton.IsEnabled = true;
                string warning = $"Preflight needs attention: {ex.Message}";
                WarningList.ItemsSource = scan.Warnings.Append(warning).ToArray();
                Log($"PREFLIGHT WARNING: {ex.Message}");
                SetNormalStatus(warning);
            }
        }
        finally
        {
            SetBusy(false);
            if (scan is not null)
            {
                SelectDetectedBackendPlugins(scan);
                UpdateBackendPrompts();
                RefreshNormalStatus();
            }
        }
    }

    private void SelectDetectedBackendPlugins(GameScanResult game)
    {
        if (game.Backends.HasFlag(BackendKind.PhotonRealtime) || game.Backends.HasFlag(BackendKind.PhotonFusion))
            PhotonCheck.IsChecked = true;
        if (game.Backends.HasFlag(BackendKind.Eos)) EosCheck.IsChecked = true;
        if (game.Backends.HasFlag(BackendKind.PlayFab)) PlayFabCheck.IsChecked = true;
        if (game.Backends.HasFlag(BackendKind.Coherence)) CoherenceCheck.IsChecked = true;
    }

    private async Task SearchSteamAsync(string gameDirectory)
    {
        string existingAppId = OriginalAppIdBox.Text.Trim();
        string term = SteamStoreSearchService.BuildSearchTerm(gameDirectory);
        SteamSearchStatus.Text = $"Searching Steam for '{term}'...";
        try
        {
            IReadOnlyList<SteamSearchResult> results = await steamStore.SearchAsync(gameDirectory);
            loadingSteamMatches = true;
            SteamMatchBox.ItemsSource = results;
            SteamSearchResult? match = uint.TryParse(existingAppId, out uint configuredAppId)
                ? results.FirstOrDefault(result => result.AppId == configuredAppId)
                : null;
            if (match is null && configuredAppId > 0)
            {
                SteamSearchResult? configured = await steamStore.LookupAppAsync(configuredAppId);
                if (configured is not null)
                {
                    results = [configured, .. results.Where(result => result.AppId != configured.AppId)];
                    SteamMatchBox.ItemsSource = results;
                    match = configured;
                }
            }
            match ??= SteamStoreSearchService.FindBestMatch(gameDirectory, results);
            SteamMatchBox.SelectedItem = match;
            if (match is not null)
            {
                if (!uint.TryParse(existingAppId, out _))
                    SetOriginalAppId(match.AppId);
                SelectedTitle.Text = match.Name;
                SteamSearchStatus.Text = $"{results.Count} match(es) found; verify the selected AppId";
                Log($"Steam Store matched '{term}' to {match.Name} ({match.AppId}).");
            }
            else
            {
                SteamSearchStatus.Text = "No Steam match found; enter the AppId manually";
                Log($"Steam Store returned no matches for '{term}'.");
            }
        }
        catch (Exception ex)
        {
            SteamMatchBox.ItemsSource = null;
            SteamSearchStatus.Text = "Steam search unavailable; enter the AppId manually";
            Log($"Steam search failed: {ex.Message}");
        }
        finally
        {
            loadingSteamMatches = false;
        }
    }

    private void LoadConfiguration(GameScanResult game, uint? steamAppId)
    {
        string iniPath = Path.Combine(game.ConfigDirectory, "union-crax.ini");
        IniDocument ini = IniDocument.Load(iniPath);
        AppIdBox.Text = ini.Get("Settings", "AppId", "480");
        OriginalAppIdBox.Text = ini.Get("Settings", "ogAppId", steamAppId?.ToString() ?? "");
        OverlayCheck.IsChecked = game.Architecture == GameArchitecture.X64;
        QuarantineCheck.IsChecked = true;
        UnlockDlcCheck.IsChecked = ini.GetBool("DLC", "UnlockAll", true);
        SteamStubCheck.IsChecked = ini.GetBool("Settings", "GetStubbedLol", game.SteamStub == SteamStubStatus.Detected);
        LoadOverlayCheck.IsChecked = ini.GetBool("Settings", "LoadOverlay", true);
        LogOverlayCheck.IsChecked = ini.GetBool("Settings", "LogOverlay");
        WarnOverlayCheck.IsChecked = ini.GetBool("Settings", "WarnOverlayDisabled");
        VerboseLogCheck.IsChecked = ini.GetBool("Settings", "VerboseLog");
        ForceOwnershipCheck.IsChecked = ini.GetBool("Settings", "ForceOwnership", true);
        PassthroughTicketCheck.IsChecked = ini.GetBool("Settings", "PassthroughTicket");
        EmulateTicketCheck.IsChecked = ini.GetBool("Settings", "EmulateTicket");
        SdrCheck.IsChecked = ini.GetBool("Settings", "SDR");
        InventoryGrantCheck.IsChecked = ini.GetBool("Settings", "InventoryAutoGrant");
        LegacyClientBox.Text = ini.Get("Settings", "Client");

        PhotonCheck.IsChecked = game.Backends.HasFlag(BackendKind.PhotonRealtime) || game.Backends.HasFlag(BackendKind.PhotonFusion)
            || File.Exists(Path.Combine(game.ConfigDirectory, "plugins", "photon_universal.dll"));
        EosCheck.IsChecked = game.Backends.HasFlag(BackendKind.Eos)
            || File.Exists(Path.Combine(game.ConfigDirectory, "plugins", "EOS_custom.dll"));
        PlayFabCheck.IsChecked = game.Backends.HasFlag(BackendKind.PlayFab)
            || File.Exists(Path.Combine(game.ConfigDirectory, "plugins", "playfab_universal.dll"));
        CoherenceCheck.IsChecked = game.Backends.HasFlag(BackendKind.Coherence)
            || File.Exists(Path.Combine(game.ConfigDirectory, "plugins", "coherence_universal.dll"));

        PhotonRealtimeBox.Text = ini.Get("Realtime", "PhotonAppIdRealtime");
        PhotonFusionBox.Text = ini.Get("Fusion", "PhotonAppIdFusion");
        PhotonVoiceBox.Text = ini.Get("Realtime", "PhotonAppIdVoice", ini.Get("Fusion", "PhotonAppIdVoice"));
        EosProductBox.Text = ini.Get("EOS", "ProductId");
        EosSandboxBox.Text = ini.Get("EOS", "SandboxId");
        EosDeploymentBox.Text = ini.Get("EOS", "DeploymentId");
        EosClientBox.Text = ini.Get("EOS", "ClientId");
        EosSecretBox.Password = ini.Get("EOS", "ClientSecret");
        DisplayNameBox.Text = ini.Get("EOS", "DisplayName", "Player");
        EosKeepGameAppCheck.IsChecked = ini.GetBool("EOS", "KeepGameApp");
        EosNoPresenceCheck.IsChecked = ini.GetBool("EOS", "NoPresence");
        PlayFabTitleBox.Text = ini.Get("PlayFab", "TitleId");
        PlayFabKeepGameTitleCheck.IsChecked = ini.GetBool("PlayFab", "KeepGameTitle");
        CoherenceKeyBox.Text = ini.Get("Coherence", "RuntimeKey");
        PhotonNicknameBox.Text = ini.Get("Realtime", "Nickname", ini.Get("Fusion", "Nickname"));
        EosVerboseCheck.IsChecked = ini.GetBool("EOS", "VerboseLog");
        PhotonVerboseCheck.IsChecked = ini.GetBool("Realtime", "VerboseLog") || ini.GetBool("Fusion", "VerboseLog");
        PlayFabVerboseCheck.IsChecked = ini.GetBool("PlayFab", "VerboseLog");
        CoherenceProjectBox.Text = ini.Get("Coherence", "ProjectId");
        CoherenceLocalServerCheck.IsChecked = ini.GetBool("Coherence", "LaunchReplicationServer");

        // [VersionProxy] loader flags. LoadDLLsEarly/SdrSafe default to the auto
        // behavior (on when a plugin is deployed / when SDR is on); an ini value wins.
        bool anyPluginSelected = EosCheck.IsChecked == true || PhotonCheck.IsChecked == true
            || PlayFabCheck.IsChecked == true || CoherenceCheck.IsChecked == true;
        LoadDllsEarlyCheck.IsChecked = ini.GetBool("VersionProxy", "LoadDLLsEarly", anyPluginSelected);
        SdrSafeCheck.IsChecked = ini.GetBool("VersionProxy", "SdrSafe", SdrCheck.IsChecked == true);
        RequireSteamCheck.IsChecked = ini.GetBool("VersionProxy", "RequireSteam", true);
        RealAppIdEnvCheck.IsChecked = ini.GetBool("Settings", "RealAppIdEnv");
        LocalSavesCheck.IsChecked = ini.GetBool("Settings", "LocalSaves", true);

        // Round-trip any [DLC] entries already in the ini (everything but UnlockAll)
        // so re-patching doesn't silently drop hand-added DLC.
        ManualDlcBox.Text = string.Join(Environment.NewLine, ini.GetSection("DLC")
            .Where(pair => !string.Equals(pair.Key, "UnlockAll", StringComparison.OrdinalIgnoreCase))
            .Select(pair => $"{pair.Key}={pair.Value}"));

        string[] known =
        [
            "AppId", "ogAppId", "PluginsFolder", "GetStubbedLol", "LoadOverlay", "LogOverlay",
            "WarnOverlayDisabled", "VerboseLog", "ForceOwnership", "PassthroughTicket", "EmulateTicket",
            "SDR", "InventoryAutoGrant", "Client", "RealAppIdEnv", "LocalSaves"
        ];
        AdditionalFlagsBox.Text = string.Join(Environment.NewLine, ini.GetSection("Settings")
            .Where(pair => !known.Contains(pair.Key, StringComparer.OrdinalIgnoreCase))
            .Select(pair => $"{pair.Key}={pair.Value}"));
    }

    private PatchOptions ReadOptions()
    {
        if (!uint.TryParse(AppIdBox.Text.Trim(), out uint appId) || appId == 0)
            throw new InvalidOperationException("Steam AppId must be a positive number.");
        if (!uint.TryParse(OriginalAppIdBox.Text.Trim(), out uint originalAppId) || originalAppId == 0)
            throw new InvalidOperationException("Enter the game's real Steam AppId.");

        var options = new PatchOptions
        {
            AppId = appId,
            OriginalAppId = originalAppId,
            UnlockAllDlc = UnlockDlcCheck.IsChecked == true,
            InstallOverlayProxy = OverlayCheck.IsChecked == true,
            QuarantineCompetingFiles = QuarantineCheck.IsChecked == true,
            EnableSteamStub = SteamStubCheck.IsChecked == true,
            LoadOverlay = LoadOverlayCheck.IsChecked == true,
            LogOverlay = LogOverlayCheck.IsChecked == true,
            WarnOverlayDisabled = WarnOverlayCheck.IsChecked == true,
            VerboseLog = VerboseLogCheck.IsChecked == true,
            ForceOwnership = ForceOwnershipCheck.IsChecked == true,
            PassthroughTicket = PassthroughTicketCheck.IsChecked == true,
            EmulateTicket = EmulateTicketCheck.IsChecked == true,
            EnableSdr = SdrCheck.IsChecked == true,
            InventoryAutoGrant = InventoryGrantCheck.IsChecked == true,
            InstallPhoton = PhotonCheck.IsChecked == true,
            InstallEos = EosCheck.IsChecked == true,
            InstallPlayFab = PlayFabCheck.IsChecked == true,
            InstallCoherence = CoherenceCheck.IsChecked == true,
            PhotonRealtimeAppId = PhotonRealtimeBox.Text,
            PhotonFusionAppId = PhotonFusionBox.Text,
            PhotonVoiceAppId = PhotonVoiceBox.Text,
            EosProductId = EosProductBox.Text,
            EosSandboxId = EosSandboxBox.Text,
            EosDeploymentId = EosDeploymentBox.Text,
            EosClientId = EosClientBox.Text,
            EosClientSecret = EosSecretBox.Password,
            DisplayName = DisplayNameBox.Text,
            EosKeepGameApp = EosKeepGameAppCheck.IsChecked == true,
            EosNoPresence = EosNoPresenceCheck.IsChecked == true,
            PlayFabTitleId = PlayFabTitleBox.Text,
            PlayFabKeepGameTitle = PlayFabKeepGameTitleCheck.IsChecked == true,
            PlayFabVerboseLog = PlayFabVerboseCheck.IsChecked == true,
            CoherenceRuntimeKey = CoherenceKeyBox.Text,
            CoherenceProjectId = CoherenceProjectBox.Text,
            CoherenceLaunchReplicationServer = CoherenceLocalServerCheck.IsChecked == true,
            PhotonNickname = PhotonNicknameBox.Text,
            PhotonVerboseLog = PhotonVerboseCheck.IsChecked == true,
            EosVerboseLog = EosVerboseCheck.IsChecked == true,
            LoadDllsEarly = LoadDllsEarlyCheck.IsChecked,
            SdrSafe = SdrSafeCheck.IsChecked,
            RequireSteam = RequireSteamCheck.IsChecked == true,
            RealAppIdEnv = RealAppIdEnvCheck.IsChecked == true,
            LocalSaves = LocalSavesCheck.IsChecked == true,
            ManualDlc = ManualDlcBox.Text,
            LegacyClientVersion = LegacyClientBox.Text,
            // The "Advanced ini" box is the raw escape hatch: merged verbatim into
            // the generated config (any section, overrides generated values).
            AdvancedIni = AdditionalFlagsBox.Text
        };
        return options;
    }

    private void RefreshPlan(bool selectChangesTab)
    {
        if (scan is null) return;
        plan = new PatchPlanner(artifacts).Create(scan, ReadOptions());
        OperationList.ItemsSource = plan.Operations;
        WarningList.ItemsSource = plan.Warnings;
        IReadOnlyList<InstalledFileStatus> statuses = installedFixes.Inspect(plan);
        int stale = statuses.Count(status => !status.Current);
        UpdateFixButton.IsEnabled = stale > 0;
        PackageButton.IsEnabled = statuses.Any(status => status.Exists);
        SetNormalStatus(stale == 0 ? "Installed fix is current" : $"{stale} installed file(s) need an update");
        if (selectChangesTab) MainTabs.SelectedIndex = 0; // Changes is the first details tab now
    }

    // The Changes/Backups/Activity strip stays hidden until the user reviews.
    private void RevealDetails(int tab = 0)
    {
        DetailsExpander.Visibility = Visibility.Visible;
        DetailsExpander.IsExpanded = true;
        MainTabs.SelectedIndex = tab;
    }

    private void BackendField_Changed(object sender, System.Windows.Controls.TextChangedEventArgs e)
    {
        UpdateBackendPrompts();
        RefreshNormalStatus();
    }

    private void EosSecret_Changed(object sender, RoutedEventArgs e)
    {
        UpdateBackendPrompts();
        RefreshNormalStatus();
    }

    private void BackendCheck_Toggled(object sender, RoutedEventArgs e)
    {
        if (!busy && scan is not null)
        {
            try
            {
                RefreshPlan(selectChangesTab: false);
            }
            catch (Exception ex)
            {
                plan = null;
                Log($"Could not refresh the patch plan after changing a plugin: {ex.Message}");
            }
        }

        UpdateBackendPrompts();
        if (GetMissingBackendSettings().Count > 0)
        {
            ExpandAdvancedForBackendNotice();
        }
        else if (advancedExpandedForBackendNotice)
        {
            AdvancedExpander.IsExpanded = false;
            advancedExpandedForBackendNotice = false;
        }
        RefreshNormalStatus();
    }

    private void ExpandAdvancedForBackendNotice()
    {
        if (AdvancedExpander.IsExpanded) return;
        AdvancedExpander.IsExpanded = true;
        advancedExpandedForBackendNotice = true;
    }

    private IReadOnlyList<MissingBackendSettings> GetMissingBackendSettings()
    {
        if (scan is null) return [];
        var options = new PatchOptions
        {
            InstallPhoton = PhotonCheck.IsChecked == true,
            PhotonRealtimeAppId = PhotonRealtimeBox.Text,
            PhotonFusionAppId = PhotonFusionBox.Text,
            PhotonVoiceAppId = PhotonVoiceBox.Text,
            InstallEos = EosCheck.IsChecked == true,
            EosProductId = EosProductBox.Text,
            EosSandboxId = EosSandboxBox.Text,
            EosDeploymentId = EosDeploymentBox.Text,
            EosClientId = EosClientBox.Text,
            EosClientSecret = EosSecretBox.Password,
            EosKeepGameApp = EosKeepGameAppCheck.IsChecked == true,
            EosNoPresence = EosNoPresenceCheck.IsChecked == true,
            InstallPlayFab = PlayFabCheck.IsChecked == true,
            PlayFabTitleId = PlayFabTitleBox.Text,
            PlayFabKeepGameTitle = PlayFabKeepGameTitleCheck.IsChecked == true
        };
        return BackendSettingsValidator.FindMissing(scan, options);
    }

    private void UpdateBackendPrompts()
    {
        if (EosCredsGroup is null || PhotonCredsGroup is null || PlayFabCredsGroup is null) return;
        HashSet<string> missing = GetMissingBackendSettings().Select(item => item.Backend).ToHashSet(StringComparer.OrdinalIgnoreCase);
        SetBackendGroupState(EosCredsGroup, EosRequiredBadge, missing.Contains("EOS"));
        SetBackendGroupState(PhotonCredsGroup, PhotonRequiredBadge, missing.Contains("Photon"));
        SetBackendGroupState(PlayFabCredsGroup, PlayFabRequiredBadge, missing.Contains("PlayFab"));
    }

    private void SetBackendGroupState(System.Windows.Controls.Border group, UIElement badge, bool needed)
    {
        badge.Visibility = needed ? Visibility.Visible : Visibility.Collapsed;
        group.BorderBrush = (System.Windows.Media.Brush)FindResource(needed ? "WarnBrush" : "BorderBrush");
        group.Background = (System.Windows.Media.Brush)FindResource(needed ? "WarnBgBrush" : "PanelAltBrush");
    }

    private bool ValidateSelectedBackendSettings()
    {
        IReadOnlyList<MissingBackendSettings> missing = GetMissingBackendSettings();
        UpdateBackendPrompts();
        if (missing.Count == 0) return true;

        AdvancedExpander.IsExpanded = true;
        advancedExpandedForBackendNotice = true;
        string details = string.Join(Environment.NewLine, missing.Select(item =>
            $"- {item.Backend}: {string.Join(", ", item.Fields)}"));
        NativeDialog.Warn(this, "Plugin settings required",
            "The selected plugins need these fields before they can be installed:\n\n" + details +
            "\n\nFill the fields under Advanced settings, or turn off any plugin you do not want.");
        return false;
    }

    private void OfferSavedBackendSettings()
    {
        BackendProfiles profiles = backendProfiles.Load(out string? warning);
        if (warning is not null)
        {
            Log(warning);
            return;
        }

        bool offerPhoton = PhotonCheck.IsChecked == true && profiles.Photon is not null
            && (NeedsSavedValue(PhotonRealtimeBox.Text, profiles.Photon.RealtimeAppId)
                || NeedsSavedValue(PhotonFusionBox.Text, profiles.Photon.FusionAppId)
                || NeedsSavedValue(PhotonVoiceBox.Text, profiles.Photon.VoiceAppId));
        bool offerEos = EosCheck.IsChecked == true && profiles.Eos is not null
            && (NeedsSavedValue(EosProductBox.Text, profiles.Eos.ProductId)
                || NeedsSavedValue(EosSandboxBox.Text, profiles.Eos.SandboxId)
                || NeedsSavedValue(EosDeploymentBox.Text, profiles.Eos.DeploymentId)
                || NeedsSavedValue(EosClientBox.Text, profiles.Eos.ClientId)
                || NeedsSavedValue(EosSecretBox.Password, profiles.Eos.ClientSecret)
                || NeedsSavedValue(DisplayNameBox.Text, profiles.Eos.DisplayName));
        bool offerPlayFab = PlayFabCheck.IsChecked == true && profiles.PlayFab is not null
            && NeedsSavedValue(PlayFabTitleBox.Text, profiles.PlayFab.TitleId);

        var available = new List<string>();
        if (offerPhoton) available.Add("Photon");
        if (offerEos) available.Add("EOS");
        if (offerPlayFab) available.Add("PlayFab");
        if (available.Count == 0) return;

        string names = string.Join(", ", available);
        if (!NativeDialog.Confirm(this, "Use saved backend settings",
                $"Saved settings are available for {names}.\n\nUse them to fill the blank fields for this game? Existing values will not be changed."))
            return;

        if (offerPhoton)
        {
            FillBlank(PhotonRealtimeBox, profiles.Photon!.RealtimeAppId);
            FillBlank(PhotonFusionBox, profiles.Photon.FusionAppId);
            FillBlank(PhotonVoiceBox, profiles.Photon.VoiceAppId);
        }
        if (offerEos)
        {
            FillBlank(EosProductBox, profiles.Eos!.ProductId);
            FillBlank(EosSandboxBox, profiles.Eos.SandboxId);
            FillBlank(EosDeploymentBox, profiles.Eos.DeploymentId);
            FillBlank(EosClientBox, profiles.Eos.ClientId);
            if (string.IsNullOrWhiteSpace(EosSecretBox.Password)) EosSecretBox.Password = profiles.Eos.ClientSecret;
            FillBlank(DisplayNameBox, profiles.Eos.DisplayName);
        }
        if (offerPlayFab) FillBlank(PlayFabTitleBox, profiles.PlayFab!.TitleId);
        UpdateBackendPrompts();
        Log($"Loaded saved encrypted backend settings for {names}; existing fields were preserved.");
    }

    private static bool NeedsSavedValue(string current, string saved) =>
        string.IsNullOrWhiteSpace(current) && !string.IsNullOrWhiteSpace(saved);

    private static void FillBlank(System.Windows.Controls.TextBox field, string saved)
    {
        if (string.IsNullOrWhiteSpace(field.Text) && !string.IsNullOrWhiteSpace(saved)) field.Text = saved;
    }

    private async Task ApplyPlanAsync(string actionName)
    {
        if (scan is null) return;
        if (!ValidateSelectedBackendSettings()) return;
        try
        {
            RefreshPlan(selectChangesTab: false);
        }
        catch (Exception ex)
        {
            ShowError("Cannot build patch plan", ex);
            return;
        }
        if (plan is null) return;

        var review = new ReviewWindow(plan, SelectedTitle.Text, actionName) { Owner = this };
        if (review.ShowDialog() != true) return;

        RevealDetails(2); // show the Activity log while patching runs
        SetBusy(true, actionName + "...");
        try
        {
            var progress = new Progress<string>(message => { StatusText.Text = message; Log(message); });
            BackupManifest manifest = await backups.ApplyAsync(plan, progress);
            Log($"Patch complete. Snapshot {manifest.Id} contains {manifest.Entries.Count} operation(s).");
            string? profileWarning = null;
            try
            {
                IReadOnlyList<string> savedProfiles = backendProfiles.SaveFrom(plan.Options);
                if (savedProfiles.Count > 0)
                    Log($"Saved encrypted backend settings for {string.Join(", ", savedProfiles)} after the verified patch.");
            }
            catch (Exception ex)
            {
                profileWarning = ex.Message;
                Log($"Backend settings were not saved: {ex.Message}");
            }
            await RefreshBackupsAsync();
            RefreshPlan(selectChangesTab: false);
            SetNormalStatus("Patch completed and verified");
            string completion = profileWarning is null
                ? "The fix was backed up, installed, and verified."
                : $"The fix was backed up, installed, and verified.\n\nSaved backend settings could not be updated: {profileWarning}";
            if (profileWarning is null) NativeDialog.Info(this, "Fix installed", completion);
            else NativeDialog.Warn(this, "Fix installed", completion);
        }
        catch (Exception ex)
        {
            ShowError("Patch failed", ex);
            if (ContainsUnauthorized(ex) && !IsAdministrator())
            {
                if (NativeDialog.Confirm(this, "Administrator access required", "This game folder requires administrator access. Restart the patcher elevated?"))
                    RestartElevated();
            }
        }
        finally { SetBusy(false); }
    }

    private async Task RefreshBackupsAsync()
    {
        if (scan is null) return;
        IReadOnlyList<BackupSnapshot> snapshots = await backups.ListAsync(scan.GameDirectory);
        BackupGrid.ItemsSource = snapshots;
        BackupGrid.SelectedItem = snapshots.FirstOrDefault(snapshot => snapshot.Status == "Complete");
        PackageButton.IsEnabled = PackageButton.IsEnabled || snapshots.Any(snapshot => snapshot.Status == "Complete");
    }

    private async void Apply_Click(object sender, RoutedEventArgs e) => await ApplyPlanAsync("Back up and patch");
    private async void UpdateFix_Click(object sender, RoutedEventArgs e) => await ApplyPlanAsync("Update installed fix");

    private void Preview_Click(object sender, RoutedEventArgs e)
    {
        try
        {
            RefreshPlan(selectChangesTab: false);
            if (plan is null) return;

            if (DetailsExpander.Visibility == Visibility.Visible
                && DetailsExpander.IsExpanded
                && MainTabs.SelectedIndex == 0)
            {
                DetailsExpander.IsExpanded = false;
            }
            else
            {
                RevealDetails(0);
            }
        }
        catch (Exception ex) { ShowError("Cannot build patch plan", ex); }
    }

    private async void Restore_Click(object sender, RoutedEventArgs e)
    {
        if (BackupGrid.SelectedItem is not BackupSnapshot snapshot) return;
        if (!NativeDialog.Confirm(this, "Restore backup", $"Restore snapshot {snapshot.Id}? Current files at those paths will be replaced.", warning: true)) return;
        SetBusy(true, "Restoring backup...");
        try
        {
            await backups.RestoreAsync(snapshot.ManifestPath, new Progress<string>(message => { StatusText.Text = message; Log(message); }));
            await RefreshBackupsAsync();
            RefreshPlan(selectChangesTab: false);
            NativeDialog.Info(this, "Backup restored", "Backup restored and verified.");
        }
        catch (Exception ex) { ShowError("Restore failed", ex); }
        finally { SetBusy(false); }
    }

    private async void PackageFix_Click(object sender, RoutedEventArgs e)
    {
        if (plan is null) return;
        if (!NativeDialog.Confirm(this, "Package current fix", "The package contains the deployed union-crax.ini, including any backend credentials in it. Create a shareable ZIP in the game root?", warning: true)) return;
        SetBusy(true, "Packaging current fix...");
        try
        {
            FixPackageResult result = await packager.CreateAsync(plan, new Progress<string>(message => { StatusText.Text = message; Log(message); }));
            Log($"Created {result.ArchivePath} with {result.FileCount} file(s).");
            NativeDialog.Info(this, "Fix package ready", $"Created {Path.GetFileName(result.ArchivePath)} in the game root.");
            Process.Start(new ProcessStartInfo { FileName = "explorer.exe", Arguments = $"/select,\"{result.ArchivePath}\"", UseShellExecute = true });
        }
        catch (Exception ex) { ShowError("Fix packaging failed", ex); }
        finally { SetBusy(false); }
    }

    private async void CheckUpdates_Click(object sender, RoutedEventArgs e) =>
        await CheckForUpdatesAsync(interactive: true);

    private async Task CheckForUpdatesAsync(bool interactive)
    {
        SetBusy(true, "Checking GitHub releases...");
        try
        {
            using var updates = new UpdateService();
            ReleaseInfo release = await updates.GetLatestAsync();
            if (!UpdateService.IsNewer(artifacts.CurrentVersion, release.TagName))
            {
                if (interactive)
                {
                    NativeDialog.Info(this, "UCOnline2 updates",
                        artifacts.CurrentVersion == "development"
                            ? $"Development build. Latest regular release: {release.TagName}"
                            : $"You already have {artifacts.CurrentVersion}.");
                }
                else
                {
                    Log($"Startup update check complete. Latest regular release: {release.TagName}.");
                }
                return;
            }
            if (release.ReleaseArchive is null) throw new InvalidOperationException("The latest release has no release ZIP.");
            if (!NativeDialog.Confirm(this, "UCOnline2 update available", $"Download and install {release.TagName}? The selected game can be refreshed with the new DLLs after restart.")) return;
            string archive = await updates.DownloadAsync(release.ReleaseArchive, new Progress<double>(value => Progress.Value = value));
            string executable = Environment.ProcessPath ?? throw new InvalidOperationException("Executable path unavailable.");
            Process.Start(SelfUpdateService.CreateUpdaterStartInfo(
                archive,
                executable,
                artifacts.BaseDirectory,
                Environment.ProcessId,
                scan?.GameDirectory,
                release.TagName));
            Application.Current.Shutdown();
        }
        catch (Exception ex)
        {
            if (interactive)
                ShowError("Update failed", ex);
            else
                Log($"Startup update check failed: {ex.Message}");
        }
        finally { SetBusy(false); }
    }

    private void OpenBackupFolder_Click(object sender, RoutedEventArgs e)
    {
        if (BackupGrid.SelectedItem is not BackupSnapshot snapshot) return;
        Process.Start(new ProcessStartInfo { FileName = Path.GetDirectoryName(snapshot.ManifestPath)!, UseShellExecute = true });
    }

    private async void ScanAgain_Click(object sender, RoutedEventArgs e) { if (scan is not null) await ScanGameAsync(scan.GameDirectory); }

    private void OptionsScroller_Loaded(object sender, RoutedEventArgs e) => OptionsScroller.ScrollToTop();

    private async void BrowseFolder_Click(object sender, RoutedEventArgs e)
    {
        var dialog = new OpenFolderDialog { Title = "Choose the game folder", Multiselect = false };
        if (dialog.ShowDialog(this) == true)
            await ScanGameAsync(dialog.FolderName);
    }

    private void SteamMatchBox_SelectionChanged(object sender, System.Windows.Controls.SelectionChangedEventArgs e)
    {
        if (loadingSteamMatches || SteamMatchBox.SelectedItem is not SteamSearchResult match) return;
        SetOriginalAppId(match.AppId);
        SelectedTitle.Text = match.Name;
        SteamSearchStatus.Text = $"Selected {match.Name} ({match.AppId})";
        try
        {
            RefreshPlan(selectChangesTab: false);
            RefreshNormalStatus();
        }
        catch (Exception ex) { ShowError("Cannot update AppId", ex); }
    }

    private async void OriginalAppIdBox_TextChanged(object sender, System.Windows.Controls.TextChangedEventArgs e)
    {
        if (OriginalAppIdPlaceholder is not null)
            OriginalAppIdPlaceholder.Visibility = string.IsNullOrWhiteSpace(OriginalAppIdBox.Text) ? Visibility.Visible : Visibility.Collapsed;
        if (loadingSteamMatches) return;

        appIdLookupCancellation?.Cancel();
        appIdLookupCancellation?.Dispose();
        appIdLookupCancellation = new CancellationTokenSource();
        CancellationToken cancellationToken = appIdLookupCancellation.Token;

        string raw = OriginalAppIdBox.Text.Trim();
        if (!uint.TryParse(raw, out uint appId) || appId == 0)
        {
            SteamSearchStatus.Text = raw.Length == 0 ? "Enter the game's Steam AppId" : "AppId must be a positive number";
            if (scan is not null) SetNormalStatus("Enter the game's real Steam AppId.");
            return;
        }

        try
        {
            await Task.Delay(500, cancellationToken);
            SteamSearchStatus.Text = $"Looking up AppId {appId}...";
            StatusText.Text = $"Looking up Steam AppId {appId}...";
            SteamSearchResult? match = await steamStore.LookupAppAsync(appId, cancellationToken);
            if (match is null)
            {
                SteamSearchStatus.Text = $"Steam did not recognize AppId {appId}";
                SetNormalStatus($"Steam did not recognize AppId {appId}.");
                return;
            }

            loadingSteamMatches = true;
            SteamMatchBox.ItemsSource = new[] { match };
            SteamMatchBox.SelectedItem = match;
            SelectedTitle.Text = match.Name;
            SteamSearchStatus.Text = $"Found {match.Name} ({match.AppId})";
            Log($"Manual AppId lookup found {match.Name} ({match.AppId}).");
            if (scan is not null) RefreshPlan(selectChangesTab: false);
            RefreshNormalStatus();
        }
        catch (OperationCanceledException) { }
        catch (Exception ex)
        {
            SteamSearchStatus.Text = $"Could not look up AppId {appId}";
            SetNormalStatus($"Could not look up AppId {appId}.");
            Log($"Manual AppId lookup failed: {ex.Message}");
        }
        finally
        {
            loadingSteamMatches = false;
        }
    }

    private void SetOriginalAppId(uint appId)
    {
        loadingSteamMatches = true;
        OriginalAppIdBox.Text = appId.ToString();
        loadingSteamMatches = false;
    }

    private void RunElevated_Click(object sender, RoutedEventArgs e) => RestartElevated();

    private void RestartElevated()
    {
        string executable = Environment.ProcessPath ?? throw new InvalidOperationException("Executable path unavailable.");
        var start = new ProcessStartInfo { FileName = executable, UseShellExecute = true, Verb = "runas" };
        if (scan is not null) start.Arguments = $"--game \"{scan.GameDirectory}\"";
        try { Process.Start(start); Application.Current.Shutdown(); }
        catch (System.ComponentModel.Win32Exception ex) when (ex.NativeErrorCode == 1223) { }
    }

    private void SetBusy(bool isBusy, string? status = null)
    {
        Progress.IsIndeterminate = isBusy;
        if (!isBusy) Progress.Value = 0;
        ApplyButton.IsEnabled = !isBusy && scan is not null;
        if (status is not null) StatusText.Text = status;

        if (!isBusy && busy)
            StatusText.Text = normalStatus;
        busy = isBusy;
    }

    private void SetNormalStatus(string status)
    {
        normalStatus = status;
        if (!busy) StatusText.Text = status;
    }

    private void RefreshNormalStatus()
    {
        if (scan is null)
        {
            SetNormalStatus("Choose a game folder to begin");
            return;
        }
        if (!uint.TryParse(OriginalAppIdBox.Text.Trim(), out uint appId) || appId == 0)
        {
            SetNormalStatus("Enter the game's real Steam AppId.");
            return;
        }
        IReadOnlyList<MissingBackendSettings> missing = GetMissingBackendSettings();
        if (missing.Count > 0)
        {
            string names = string.Join(", ", missing.Select(item => item.Backend));
            SetNormalStatus($"{names} selected - fill the required fields or turn those plugins off.");
            return;
        }
        SetNormalStatus("Ready to review and apply");
    }

    private void Log(string message)
    {
        ActivityLog.AppendText($"[{DateTime.Now:HH:mm:ss}] {message}{Environment.NewLine}");
        ActivityLog.ScrollToEnd();
    }

    private void ShowError(string title, Exception exception)
    {
        Log($"ERROR: {exception.Message}");
        StatusText.Text = exception.Message;
        NativeDialog.Error(this, title, exception.Message);
    }

    // coherence multiplayer needs the game's schema uploaded to the user's own
    // coherence project -- a Unity-Editor-driven flow that lives in the shipped
    // tools\coherence_schema pipeline. We don't reimplement it; when a coherence
    // game is scanned we just offer to launch that tool.
    private void OfferCoherenceSchemaUpload()
    {
        if (!NativeDialog.Confirm(this, "coherence game detected",
                "coherence multiplayer needs the game's network schema uploaded to your own coherence project.\n\n" +
                "Open the schema upload tool now? It walks you through extracting the schema and uploading it " +
                "(needs the Unity Editor plus your coherence project ID and token)."))
            return;

        string? tool = artifacts.FindCoherenceSchemaTool();
        if (tool is null)
        {
            NativeDialog.Warn(this, "Schema tool not found",
                "The coherence schema tool (tools\\coherence_schema) isn't in this build. " +
                "Use the full release package, which bundles it.");
            return;
        }
        try
        {
            Process.Start(new ProcessStartInfo
            {
                FileName = tool,
                WorkingDirectory = Path.GetDirectoryName(tool)!,
                UseShellExecute = true
            });
            Log("Launched the coherence schema upload tool.");
        }
        catch (Exception ex) { ShowError("Could not launch the schema tool", ex); }
    }

    private static bool ContainsUnauthorized(Exception exception)
    {
        for (Exception? current = exception; current is not null; current = current.InnerException)
            if (current is UnauthorizedAccessException) return true;
        return false;
    }

    private static bool IsAdministrator()
    {
        using WindowsIdentity identity = WindowsIdentity.GetCurrent();
        return new WindowsPrincipal(identity).IsInRole(WindowsBuiltInRole.Administrator);
    }

    private static string FindArtifactRoot()
    {
        DirectoryInfo? directory = new(AppContext.BaseDirectory);
        while (directory is not null)
        {
            if (File.Exists(Path.Combine(directory.FullName, "uc_online2.vcxproj")) || Directory.Exists(Path.Combine(directory.FullName, "x64")))
                return directory.FullName;
            directory = directory.Parent;
        }
        return AppContext.BaseDirectory;
    }

    protected override void OnClosed(EventArgs e)
    {
        steamStore.Dispose();
        base.OnClosed(e);
    }

}
