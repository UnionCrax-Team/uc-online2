using System.IO;
using System.Windows;
using UCO2.Patcher.Core;

namespace UCO2.Patcher.Gui;

public partial class ReviewWindow : Window
{
    public ReviewWindow(PatchPlan plan, string gameName, string actionName, bool allowConfirm = true)
    {
        InitializeComponent();
        HeadingText.Text = actionName;
        ConfirmButton.Visibility = allowConfirm ? Visibility.Visible : Visibility.Collapsed;
        CancelButton.Content = allowConfirm ? "Cancel" : "Close";
        FooterText.Text = allowConfirm ? "Existing files are backed up before any write" : $"{plan.Operations.Count} file change(s) planned";
        GameValue.Text = gameName;
        FolderValue.Text = plan.Game.GameDirectory;
        DetectedValue.Text = $"{plan.Game.EngineLabel} | {plan.Game.BackendLabel}";
        GameAppIdValue.Text = plan.Options.OriginalAppId.ToString();
        EmulatedAppIdValue.Text = plan.Options.AppId.ToString();

        IReadOnlyList<ReviewSetting> settings = BuildSettings(plan.Options);
        ReviewSetting[] enabled = settings.Where(setting => setting.Enabled).ToArray();
        ReviewSetting[] disabled = settings.Where(setting => !setting.Enabled).ToArray();
        EnabledSettingsList.ItemsSource = enabled;
        EnabledCountText.Text = $"{enabled.Length} selected";
        DisabledSettingsText.Text = disabled.Length == 0
            ? "All available options are selected."
            : "Not selected: " + string.Join(", ", disabled.Select(setting => setting.Name));

        WarningsList.ItemsSource = plan.Warnings;
        WarningsPanel.Visibility = plan.Warnings.Count > 0 ? Visibility.Visible : Visibility.Collapsed;
        NoWarningsPanel.Visibility = plan.Warnings.Count == 0 ? Visibility.Visible : Visibility.Collapsed;

        ReviewChange[] changes = plan.Operations.Select(operation =>
        {
            string relativePath = Path.GetRelativePath(plan.Game.GameDirectory, operation.TargetPath);
            return new ReviewChange(
                DescribeAction(operation.Kind),
                operation.Description,
                Path.GetFileName(relativePath),
                Path.GetDirectoryName(relativePath) is { Length: > 0 } directory ? directory : "Game root");
        }).ToArray();
        ChangesList.ItemsSource = changes;
        ChangeCountText.Text = $"{changes.Length} FILE CHANGE{(changes.Length == 1 ? "" : "S")}";
    }

    private static IReadOnlyList<ReviewSetting> BuildSettings(PatchOptions options)
    {
        var settings = new List<ReviewSetting>
        {
            State("Overlay proxy", options.InstallOverlayProxy),
            State("Steam overlay", options.LoadOverlay),
            State("Unlock DLC", options.UnlockAllDlc),
            State("Remove competing loaders", options.QuarantineCompetingFiles),
            State("Force ownership", options.ForceOwnership),
            State("SteamStub", options.EnableSteamStub),
            State("Ticket passthrough", options.PassthroughTicket),
            State("Ticket emulation", options.EmulateTicket),
            State("SDR", options.EnableSdr),
            State("Overlay logging", options.LogOverlay),
            State("Verbose logging", options.VerboseLog),
            State("Photon", options.InstallPhoton),
            State("EOS", options.InstallEos),
            State("PlayFab", options.InstallPlayFab),
            State("Coherence", options.InstallCoherence)
        };
        if (!string.IsNullOrWhiteSpace(options.EosClientSecret))
            settings.Add(new ReviewSetting("EOS credentials configured", true));
        if (options.AdditionalSettings.Count > 0)
            settings.Add(new ReviewSetting($"Additional flags ({options.AdditionalSettings.Count})", true));
        return settings;
    }

    private static ReviewSetting State(string name, bool enabled) => new(name, enabled);

    private static string DescribeAction(PatchOperationKind kind) => kind switch
    {
        PatchOperationKind.ReplaceFile => "Replace file",
        PatchOperationKind.WriteText => "Write config",
        PatchOperationKind.WriteBytes => "Patch data",
        PatchOperationKind.RemoveFile => "Remove file",
        _ => kind.ToString()
    };

    private void Confirm_Click(object sender, RoutedEventArgs e) => DialogResult = true;

    private sealed record ReviewSetting(string Name, bool Enabled);
    private sealed record ReviewChange(string Action, string Description, string FileName, string Directory);
}
