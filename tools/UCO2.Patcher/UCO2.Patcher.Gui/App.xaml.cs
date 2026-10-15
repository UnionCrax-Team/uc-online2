using System.Windows;
using UCO2.Patcher.Core;

namespace UCO2.Patcher.Gui;

public partial class App : Application
{
    protected override async void OnStartup(StartupEventArgs e)
    {
        base.OnStartup(e);
        if (e.Args.Length > 0 && e.Args[0].Equals("--apply-update", StringComparison.OrdinalIgnoreCase))
        {
            // Updater mode has no window. Keep WPF alive across awaits until the
            // package has been copied, verified, and the replacement is started.
            ShutdownMode = ShutdownMode.OnExplicitShutdown;
            try
            {
                if (e.Args.Length < 8)
                    throw new InvalidOperationException("The updater was started without the required release paths.");
                await SelfUpdateService.ApplyAsync(
                    e.Args[1], e.Args[2], e.Args[3], e.Args[4], int.Parse(e.Args[5]), e.Args[6], e.Args[7]);
            }
            catch (Exception ex)
            {
                NativeDialog.Error(null, "UCOnline2 update failed", ex.Message);
            }
            Shutdown();
            return;
        }

        string? initialGame = null;
        bool refreshFix = false;
        for (int index = 0; index + 1 < e.Args.Length; index++)
        {
            if (e.Args[index].Equals("--game", StringComparison.OrdinalIgnoreCase))
                initialGame = e.Args[index + 1];
        }
        refreshFix = e.Args.Any(arg => arg.Equals("--refresh-fix", StringComparison.OrdinalIgnoreCase));
        new MainWindow(initialGame, refreshFix).Show();
    }
}
