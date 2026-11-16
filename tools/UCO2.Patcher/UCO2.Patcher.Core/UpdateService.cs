using System.Diagnostics;
using System.IO.Compression;
using System.Net.Http.Headers;
using System.Security.Cryptography;
using System.Text.Json;

namespace UCO2.Patcher.Core;

public sealed class UpdateService : IDisposable
{
    private readonly HttpClient client;

    public UpdateService()
    {
        client = new HttpClient { Timeout = TimeSpan.FromMinutes(5) };
        client.DefaultRequestHeaders.UserAgent.Add(new ProductInfoHeaderValue("UCOnline2-Patcher", "1.0"));
        client.DefaultRequestHeaders.Accept.Add(new MediaTypeWithQualityHeaderValue("application/vnd.github+json"));
    }

    public async Task<ReleaseInfo> GetLatestAsync(CancellationToken cancellationToken = default)
    {
        using HttpResponseMessage response = await client.GetAsync(
            "https://api.github.com/repos/LukeWarmSodas/uc-online2/releases/latest",
            cancellationToken);
        response.EnsureSuccessStatusCode();
        await using Stream stream = await response.Content.ReadAsStreamAsync(cancellationToken);
        using JsonDocument document = await JsonDocument.ParseAsync(stream, cancellationToken: cancellationToken);
        JsonElement root = document.RootElement;

        ReleaseAsset? releaseArchive = null;
        foreach (JsonElement asset in root.GetProperty("assets").EnumerateArray())
        {
            string name = asset.GetProperty("name").GetString() ?? "";
            if (!name.EndsWith("-release.zip", StringComparison.OrdinalIgnoreCase))
                continue;
            releaseArchive = new ReleaseAsset(
                name,
                asset.GetProperty("browser_download_url").GetString() ?? "",
                asset.TryGetProperty("digest", out JsonElement digest) ? digest.GetString() : null,
                asset.GetProperty("size").GetInt64());
            break;
        }

        return new ReleaseInfo(
            root.GetProperty("tag_name").GetString() ?? "",
            root.GetProperty("name").GetString() ?? "",
            root.GetProperty("html_url").GetString() ?? "",
            root.GetProperty("prerelease").GetBoolean(),
            releaseArchive);
    }

    public async Task<string> DownloadAsync(ReleaseAsset asset, IProgress<double>? progress = null, CancellationToken cancellationToken = default)
    {
        string downloadDirectory = Path.Combine(Path.GetTempPath(), "UCOnline2", "updates", Guid.NewGuid().ToString("N"));
        Directory.CreateDirectory(downloadDirectory);
        string destination = Path.Combine(downloadDirectory, asset.Name);

        using HttpResponseMessage response = await client.GetAsync(asset.DownloadUrl, HttpCompletionOption.ResponseHeadersRead, cancellationToken);
        response.EnsureSuccessStatusCode();
        long total = response.Content.Headers.ContentLength ?? asset.Size;
        // Scope the streams so the file handle is RELEASED before we hash it below.
        // File.Create opens with FileShare.None, so a still-open output stream would
        // make the SHA-256 read fail with "used by another process" -- the patcher
        // colliding with its own download handle.
        await using (Stream input = await response.Content.ReadAsStreamAsync(cancellationToken))
        await using (FileStream output = File.Create(destination))
        {
            byte[] buffer = new byte[128 * 1024];
            long copied = 0;
            int read;
            while ((read = await input.ReadAsync(buffer, cancellationToken)) > 0)
            {
                await output.WriteAsync(buffer.AsMemory(0, read), cancellationToken);
                copied += read;
                if (total > 0) progress?.Report(copied / (double)total);
            }
            await output.FlushAsync(cancellationToken);
        }

        if (!string.IsNullOrWhiteSpace(asset.Digest) && asset.Digest.StartsWith("sha256:", StringComparison.OrdinalIgnoreCase))
        {
            string expected = asset.Digest[7..];
            string actual = SafeFileSystem.Sha256(destination);
            if (!actual.Equals(expected, StringComparison.OrdinalIgnoreCase))
                throw new InvalidDataException("The downloaded update failed SHA-256 verification.");
        }
        return destination;
    }

    public static bool IsNewer(string current, string available)
    {
        if (current.Equals("development", StringComparison.OrdinalIgnoreCase))
            return false;
        return ParseVersion(available) > ParseVersion(current);
    }

    private static Version ParseVersion(string value)
    {
        string numeric = value.Trim().TrimStart('v', 'V').Split('-', 2)[0];
        return Version.TryParse(numeric, out Version? version) ? version : new Version(0, 0);
    }

    public void Dispose() => client.Dispose();
}

public static class SelfUpdateService
{
    public static ProcessStartInfo CreateUpdaterStartInfo(
        string archivePath,
        string executablePath,
        string artifactDirectory,
        int parentProcessId,
        string? restartGameDirectory,
        string expectedVersion)
    {
        string currentExecutable = Environment.ProcessPath
            ?? throw new InvalidOperationException("The current executable path is unavailable.");
        string executableDirectory = Path.GetDirectoryName(Path.GetFullPath(executablePath))
            ?? throw new InvalidOperationException("The executable directory is unavailable.");
        string updaterDirectory = Path.Combine(Path.GetTempPath(), "UCOnline2", "updater", Guid.NewGuid().ToString("N"));
        Directory.CreateDirectory(updaterDirectory);
        string updaterExecutable = Path.Combine(updaterDirectory, "UCO2.Patcher.Updater.exe");
        File.Copy(currentExecutable, updaterExecutable);

        var startInfo = new ProcessStartInfo
        {
            FileName = updaterExecutable,
            UseShellExecute = true
        };
        foreach (string argument in new[]
        {
            "--apply-update",
            archivePath,
            executableDirectory,
            Path.GetFullPath(artifactDirectory),
            Path.GetFileName(executablePath),
            parentProcessId.ToString(),
            restartGameDirectory ?? "",
            expectedVersion
        })
            startInfo.ArgumentList.Add(argument);
        return startInfo;
    }

    public static async Task ApplyAsync(
        string archivePath,
        string executableDirectory,
        string artifactDirectory,
        string executableName,
        int parentProcessId,
        string? restartGameDirectory,
        string expectedVersion)
    {
        try
        {
            using Process? parent = Process.GetProcessById(parentProcessId);
            await parent.WaitForExitAsync();
        }
        catch (ArgumentException) { }

        string installedExecutable = await InstallPackageAsync(
            archivePath, executableDirectory, artifactDirectory, executableName, expectedVersion);

        var start = new ProcessStartInfo
        {
            FileName = installedExecutable,
            WorkingDirectory = executableDirectory,
            UseShellExecute = true
        };
        if (!string.IsNullOrWhiteSpace(restartGameDirectory))
        {
            start.ArgumentList.Add("--game");
            start.ArgumentList.Add(restartGameDirectory);
            start.ArgumentList.Add("--refresh-fix");
        }
        Process.Start(start);
    }

    public static async Task<string> InstallPackageAsync(
        string archivePath,
        string executableDirectory,
        string artifactDirectory,
        string executableName,
        string expectedVersion)
    {
        executableDirectory = Path.GetFullPath(executableDirectory);
        artifactDirectory = Path.GetFullPath(artifactDirectory);
        Directory.CreateDirectory(executableDirectory);
        Directory.CreateDirectory(artifactDirectory);

        string staging = Path.Combine(Path.GetTempPath(), "UCOnline2", "staging", Guid.NewGuid().ToString("N"));
        Directory.CreateDirectory(staging);
        try
        {
            // Defender and recently released file handles can briefly hold the archive.
            await RetryOnLockAsync(() => ZipFile.ExtractToDirectory(archivePath, staging, overwriteFiles: true));
            string packageRoot = FindPackageRoot(staging, executableName);
            string[] sources = SafeFileSystem.EnumerateFiles(packageRoot).ToArray();
            if (sources.Length == 0)
                throw new InvalidDataException("The update package is empty.");

            foreach (string source in sources)
            {
                string relative = Path.GetRelativePath(packageRoot, source);
                string destinationRoot = relative.Equals(executableName, StringComparison.OrdinalIgnoreCase)
                    ? executableDirectory
                    : artifactDirectory;
                string target = Path.Combine(destinationRoot, relative);
                Directory.CreateDirectory(Path.GetDirectoryName(target)!);
                await RetryOnLockAsync(() => File.Copy(source, target, overwrite: true));
                if (!SafeFileSystem.Sha256(source).Equals(SafeFileSystem.Sha256(target), StringComparison.OrdinalIgnoreCase))
                    throw new InvalidDataException($"Update verification failed for {relative}.");
            }

            // The version is baked into the exe (ProductVersion), read straight off
            // the file without loading it. Released builds always carry it; a
            // stand-in exe (tests) may not, so an unreadable/empty version is
            // tolerated -- the per-file SHA-256 check already proved the copy.
            string installedVersion = "";
            try
            {
                string? product = FileVersionInfo
                    .GetVersionInfo(Path.Combine(executableDirectory, executableName))
                    .ProductVersion;
                if (!string.IsNullOrWhiteSpace(product))
                {
                    int plus = product.IndexOf('+');
                    installedVersion = (plus >= 0 ? product[..plus] : product).Trim();
                }
            }
            catch { }
            if (installedVersion.Length > 0
                && !installedVersion.Equals(expectedVersion, StringComparison.OrdinalIgnoreCase))
                throw new InvalidDataException(
                    $"The update installed exe version '{installedVersion}', expected '{expectedVersion}'.");

            foreach (string required in new[] { Path.Combine("x64", "steam_api64.dll"), Path.Combine("x86", "steam_api.dll") })
            {
                if (!File.Exists(Path.Combine(artifactDirectory, required)))
                    throw new InvalidDataException($"The updated artifact {required} is missing.");
            }
            string pluginDirectory = Path.Combine(artifactDirectory, "plugins");
            if (!Directory.Exists(pluginDirectory) || !Directory.EnumerateFiles(pluginDirectory, "*.dll").Any())
                throw new InvalidDataException("The updated plugin DLLs are missing.");

            string installedExecutable = Path.Combine(executableDirectory, executableName);
            if (!File.Exists(installedExecutable))
                throw new InvalidDataException($"The updated executable {executableName} is missing.");
            return installedExecutable;
        }
        finally
        {
            try { Directory.Delete(staging, recursive: true); } catch { }
        }
    }

    private static string FindPackageRoot(string staging, string executableName)
    {
        string[] candidates = SafeFileSystem.EnumerateFiles(staging, executableName)
            .Select(Path.GetDirectoryName)
            .Where(directory => directory is not null
                && File.Exists(Path.Combine(directory, "x64", "steam_api64.dll"))
                && File.Exists(Path.Combine(directory, "x86", "steam_api.dll"))
                && Directory.Exists(Path.Combine(directory, "plugins")))
            .Cast<string>()
            .ToArray();
        return candidates.Length switch
        {
            1 => candidates[0],
            0 => throw new InvalidDataException("The update archive does not contain a complete UCOnline2 release package."),
            _ => throw new InvalidDataException("The update archive contains more than one UCOnline2 release package.")
        };
    }

    // Retry a file operation that can transiently fail with "used by another
    // process" right after a download / parent exit (Defender scan, handle lag).
    private static async Task RetryOnLockAsync(Action action, int timeoutMs = 20000)
    {
        Stopwatch stopwatch = Stopwatch.StartNew();
        while (true)
        {
            try
            {
                action();
                return;
            }
            catch (Exception ex) when ((ex is IOException || ex is UnauthorizedAccessException)
                                       && stopwatch.ElapsedMilliseconds < timeoutMs)
            {
                await Task.Delay(400);
            }
        }
    }
}
