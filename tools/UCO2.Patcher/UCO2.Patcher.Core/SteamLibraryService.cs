using System.Text.RegularExpressions;
using Microsoft.Win32;

namespace UCO2.Patcher.Core;

public sealed partial class SteamLibraryService
{
    public Task<IReadOnlyList<SteamGame>> FindInstalledGamesAsync(CancellationToken cancellationToken = default)
    {
        return Task.Run<IReadOnlyList<SteamGame>>(() =>
        {
            var games = new Dictionary<uint, SteamGame>();
            foreach (string library in FindLibraryPaths())
            {
                cancellationToken.ThrowIfCancellationRequested();
                string steamApps = Path.Combine(library, "steamapps");
                if (!Directory.Exists(steamApps))
                    continue;

                foreach (string manifest in Directory.EnumerateFiles(steamApps, "appmanifest_*.acf", SearchOption.TopDirectoryOnly))
                {
                    cancellationToken.ThrowIfCancellationRequested();
                    try
                    {
                        string text = File.ReadAllText(manifest);
                        Dictionary<string, string> values = ParseFlatVdf(text);
                        if (!uint.TryParse(values.GetValueOrDefault("appid"), out uint appId))
                            continue;
                        string name = values.GetValueOrDefault("name") ?? $"Steam app {appId}";
                        string installDirName = values.GetValueOrDefault("installdir") ?? "";
                        string installDirectory = Path.Combine(steamApps, "common", installDirName);
                        if (!Directory.Exists(installDirectory))
                            continue;
                        games[appId] = new SteamGame(appId, name, installDirectory, manifest, library);
                    }
                    catch (Exception ex) when (ex is IOException or UnauthorizedAccessException) { }
                }
            }

            return games.Values
                .OrderBy(game => game.Name, StringComparer.CurrentCultureIgnoreCase)
                .ToArray();
        }, cancellationToken);
    }

    public IReadOnlyList<string> FindLibraryPaths()
    {
        var libraries = new HashSet<string>(StringComparer.OrdinalIgnoreCase);
        string? steamRoot = FindSteamRoot();
        if (string.IsNullOrWhiteSpace(steamRoot) || !Directory.Exists(steamRoot))
            return [];

        libraries.Add(Path.GetFullPath(steamRoot));
        string libraryFile = Path.Combine(steamRoot, "steamapps", "libraryfolders.vdf");
        if (File.Exists(libraryFile))
        {
            try
            {
                foreach (string path in ParseLibraryFolders(File.ReadAllText(libraryFile)))
                {
                    if (Directory.Exists(path))
                        libraries.Add(Path.GetFullPath(path));
                }
            }
            catch (Exception ex) when (ex is IOException or UnauthorizedAccessException) { }
        }

        return libraries.OrderBy(path => path, StringComparer.OrdinalIgnoreCase).ToArray();
    }

    public static IReadOnlyList<string> ParseLibraryFolders(string text)
    {
        var paths = new HashSet<string>(StringComparer.OrdinalIgnoreCase);
        foreach (Match match in VdfPairRegex().Matches(text))
        {
            if (!match.Groups["key"].Value.Equals("path", StringComparison.OrdinalIgnoreCase))
                continue;
            string value = UnescapeVdf(match.Groups["value"].Value);
            if (value.Length > 0)
                paths.Add(value);
        }
        return paths.ToArray();
    }

    public static Dictionary<string, string> ParseFlatVdf(string text)
    {
        var values = new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);
        foreach (Match match in VdfPairRegex().Matches(text))
            values[UnescapeVdf(match.Groups["key"].Value)] = UnescapeVdf(match.Groups["value"].Value);
        return values;
    }

    private static string? FindSteamRoot()
    {
        string? path = ReadRegistryString(Registry.CurrentUser, @"Software\Valve\Steam", "SteamPath")
            ?? ReadRegistryString(Registry.LocalMachine, @"SOFTWARE\WOW6432Node\Valve\Steam", "InstallPath")
            ?? ReadRegistryString(Registry.LocalMachine, @"SOFTWARE\Valve\Steam", "InstallPath");
        return path?.Replace('/', Path.DirectorySeparatorChar);
    }

    private static string? ReadRegistryString(RegistryKey hive, string subKey, string value)
    {
        try
        {
            using RegistryKey? key = hive.OpenSubKey(subKey);
            return key?.GetValue(value) as string;
        }
        catch (Exception ex) when (ex is UnauthorizedAccessException or IOException) { return null; }
    }

    private static string UnescapeVdf(string value) => value.Replace("\\\\", "\\").Replace("\\\"", "\"");

    [GeneratedRegex("\\\"(?<key>(?:\\\\.|[^\\\"])*)\\\"\\s+\\\"(?<value>(?:\\\\.|[^\\\"])*)\\\"", RegexOptions.CultureInvariant)]
    private static partial Regex VdfPairRegex();
}
