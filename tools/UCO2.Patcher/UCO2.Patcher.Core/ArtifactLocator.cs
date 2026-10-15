using System.Reflection;

namespace UCO2.Patcher.Core;

public sealed class ArtifactLocator
{
    public ArtifactLocator(string? baseDirectory = null)
    {
        BaseDirectory = Path.GetFullPath(baseDirectory ?? AppContext.BaseDirectory);
    }

    public string BaseDirectory { get; }

    public string? FindSteamApi(GameArchitecture architecture)
    {
        return architecture switch
        {
            GameArchitecture.X64 => FirstExisting(
                Path.Combine(BaseDirectory, "relbuild", "x64", "steam_api64.dll"),
                Path.Combine(BaseDirectory, "x64", "steam_api64.dll"),
                Path.Combine(BaseDirectory, "steam_api64.dll"),
                Path.Combine(BaseDirectory, "..", "..", "relbuild", "x64", "steam_api64.dll")),
            GameArchitecture.X86 => FirstExisting(
                Path.Combine(BaseDirectory, "relbuild", "x86", "steam_api.dll"),
                Path.Combine(BaseDirectory, "x86", "steam_api.dll"),
                Path.Combine(BaseDirectory, "steam_api.dll"),
                Path.Combine(BaseDirectory, "..", "..", "relbuild", "x86", "steam_api.dll")),
            _ => null
        };
    }

    public string? FindOverlayProxy() => FirstExisting(
        Path.Combine(BaseDirectory, "plugins", "steam_overlay", "relbuild", "x64", "overlay_proxy.dll"),
        Path.Combine(BaseDirectory, "plugins", "overlay_proxy.dll"),
        Path.Combine(BaseDirectory, "overlay_proxy.dll"),
        Path.Combine(BaseDirectory, "..", "..", "plugins", "steam_overlay", "relbuild", "x64", "overlay_proxy.dll"));

    public string? FindCoherenceSchemaTool() => FirstExisting(
        Path.Combine(BaseDirectory, "tools", "coherence_schema", "Run-CoherenceSchemaPipeline.bat"),
        Path.Combine(BaseDirectory, "..", "..", "tools", "coherence_schema", "Run-CoherenceSchemaPipeline.bat"));

    public string? FindPlugin(string name) => FirstExisting(
        Path.Combine(BaseDirectory, "plugins", name, "relbuild", "x64", name + ".dll"),
        Path.Combine(BaseDirectory, "plugins", name + ".dll"),
        Path.Combine(BaseDirectory, name + ".dll"),
        Path.Combine(BaseDirectory, "..", "..", "plugins", name, "relbuild", "x64", name + ".dll"));

    // The version is baked into the exe at release time
    // (-p:InformationalVersion=<tag>); dev builds carry "development". Read it
    // from the running assembly so no loose version.txt has to ship beside the
    // exe. A back-compat version.txt (older installs) is still honored.
    public string CurrentVersion
    {
        get
        {
            string? baked = Assembly.GetEntryAssembly()
                ?.GetCustomAttribute<AssemblyInformationalVersionAttribute>()
                ?.InformationalVersion;
            if (!string.IsNullOrWhiteSpace(baked))
            {
                int plus = baked.IndexOf('+'); // strip any "+<sha>" build metadata
                baked = (plus >= 0 ? baked[..plus] : baked).Trim();
                if (baked.Length > 0 && baked != "1.0.0")
                    return baked;
            }
            string versionFile = Path.Combine(BaseDirectory, "version.txt");
            if (File.Exists(versionFile))
                return File.ReadAllText(versionFile).Trim();
            return "development";
        }
    }

    private static string? FirstExisting(params string[] paths)
    {
        return paths.Select(Path.GetFullPath).FirstOrDefault(File.Exists);
    }
}
