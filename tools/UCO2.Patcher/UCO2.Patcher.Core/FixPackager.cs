using System.IO.Compression;
using System.Text.Json;

namespace UCO2.Patcher.Core;

public sealed class FixPackager
{
    public Task<FixPackageResult> CreateAsync(PatchPlan plan, IProgress<string>? progress = null, CancellationToken cancellationToken = default)
    {
        string[] paths = plan.Operations
            .Where(operation => operation.Kind != PatchOperationKind.RemoveFile && File.Exists(operation.TargetPath))
            .Select(operation => operation.TargetPath)
            .Distinct(StringComparer.OrdinalIgnoreCase)
            .ToArray();
        return Task.Run(() => CreateArchive(plan.Game.GameDirectory, paths, progress, cancellationToken), cancellationToken);
    }

    public Task<FixPackageResult> CreateAsync(string manifestPath, IProgress<string>? progress = null, CancellationToken cancellationToken = default)
    {
        return Task.Run(() => Create(manifestPath, progress, cancellationToken), cancellationToken);
    }

    private static FixPackageResult Create(string manifestPath, IProgress<string>? progress, CancellationToken cancellationToken)
    {
        BackupManifest? manifest = JsonSerializer.Deserialize<BackupManifest>(File.ReadAllText(manifestPath), new JsonSerializerOptions { PropertyNameCaseInsensitive = true });
        if (manifest is null) throw new InvalidDataException("The selected backup manifest is invalid.");
        if (!Directory.Exists(manifest.GameDirectory)) throw new DirectoryNotFoundException(manifest.GameDirectory);

        string[] paths = manifest.Entries
            .Where(entry => entry.Applied && entry.Kind != PatchOperationKind.RemoveFile && File.Exists(entry.TargetPath))
            .Select(entry => entry.TargetPath)
            .Distinct(StringComparer.OrdinalIgnoreCase)
            .ToArray();
        return CreateArchive(manifest.GameDirectory, paths, progress, cancellationToken);
    }

    private static FixPackageResult CreateArchive(string gameDirectory, IReadOnlyList<string> paths, IProgress<string>? progress, CancellationToken cancellationToken)
    {
        if (paths.Count == 0) throw new InvalidOperationException("No deployed fix files were found to package.");

        string gameName = Sanitize(Path.GetFileName(gameDirectory));
        string archive = Path.Combine(gameDirectory, $"UCOnline2-Fix-{gameName}-{DateTime.Now:yyyyMMdd-HHmmss}.zip");
        string temporary = archive + ".tmp";
        var packagedFiles = new List<object>();

        try
        {
            using FileStream stream = File.Create(temporary);
            using var zip = new ZipArchive(stream, ZipArchiveMode.Create);
            foreach (string targetPath in paths)
            {
                cancellationToken.ThrowIfCancellationRequested();
                if (!SafeFileSystem.IsInside(gameDirectory, targetPath))
                    throw new InvalidDataException($"The fix contains a path outside the game: {targetPath}");
                string relative = Path.GetRelativePath(gameDirectory, targetPath).Replace('\\', '/');
                progress?.Report($"Package {relative}");
                zip.CreateEntryFromFile(targetPath, relative, CompressionLevel.Optimal);
                packagedFiles.Add(new { path = relative, sha256 = SafeFileSystem.Sha256(targetPath) });
            }

            ZipArchiveEntry packageManifest = zip.CreateEntry("uco2-fix-manifest.json", CompressionLevel.Optimal);
            using StreamWriter writer = new(packageManifest.Open());
            writer.Write(JsonSerializer.Serialize(new
            {
                formatVersion = 1,
                createdAt = DateTimeOffset.Now,
                game = Path.GetFileName(gameDirectory),
                files = packagedFiles
            }, new JsonSerializerOptions { WriteIndented = true }));
        }
        catch
        {
            if (File.Exists(temporary)) File.Delete(temporary);
            throw;
        }

        File.Move(temporary, archive, overwrite: false);
        return new FixPackageResult(archive, paths.Count);
    }

    private static string Sanitize(string value)
    {
        foreach (char invalid in Path.GetInvalidFileNameChars()) value = value.Replace(invalid, '_');
        return value;
    }
}
