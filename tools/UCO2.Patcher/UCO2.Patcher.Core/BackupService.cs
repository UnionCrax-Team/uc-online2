using System.Security.Cryptography;
using System.Text;
using System.Text.Json;

namespace UCO2.Patcher.Core;

public sealed class BackupService
{
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        WriteIndented = true,
        PropertyNameCaseInsensitive = true
    };

    public BackupService(string? storageRoot = null)
    {
        StorageRoot = storageRoot ?? Path.Combine(
            Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData),
            "UCOnline2", "Patcher", "backups");
    }

    public string StorageRoot { get; }

    public Task<BackupManifest> ApplyAsync(PatchPlan plan, IProgress<string>? progress = null, CancellationToken cancellationToken = default)
    {
        return Task.Run(() => Apply(plan, progress, cancellationToken), cancellationToken);
    }

    public async Task<IReadOnlyList<BackupSnapshot>> ListAsync(string gameDirectory, CancellationToken cancellationToken = default)
    {
        string gameRoot = GetGameBackupRoot(gameDirectory);
        if (!Directory.Exists(gameRoot))
            return [];

        var snapshots = new List<BackupSnapshot>();
        foreach (string manifestPath in Directory.EnumerateFiles(gameRoot, "manifest.json", SearchOption.AllDirectories))
        {
            cancellationToken.ThrowIfCancellationRequested();
            try
            {
                await using FileStream stream = File.OpenRead(manifestPath);
                BackupManifest? manifest = await JsonSerializer.DeserializeAsync<BackupManifest>(stream, JsonOptions, cancellationToken);
                if (manifest is not null)
                    snapshots.Add(new BackupSnapshot(manifestPath, manifest.Id, manifest.CreatedAt, manifest.Status, manifest.Entries.Count));
            }
            catch (Exception ex) when (ex is IOException or UnauthorizedAccessException or JsonException) { }
        }
        return snapshots.OrderByDescending(snapshot => snapshot.CreatedAt).ToArray();
    }

    public Task RestoreAsync(string manifestPath, IProgress<string>? progress = null, CancellationToken cancellationToken = default)
    {
        return Task.Run(() =>
        {
            BackupManifest manifest = LoadManifest(manifestPath);
            RestoreInternal(manifest, Path.GetDirectoryName(manifestPath)!, progress, cancellationToken);
            manifest.Status = "Restored";
            manifest.Error = null;
            SaveManifest(manifestPath, manifest);
        }, cancellationToken);
    }

    private BackupManifest Apply(PatchPlan plan, IProgress<string>? progress, CancellationToken cancellationToken)
    {
        string id = DateTimeOffset.Now.ToString("yyyyMMdd-HHmmssfff");
        string snapshotRoot = Path.Combine(GetGameBackupRoot(plan.Game.GameDirectory), id);
        string filesRoot = Path.Combine(snapshotRoot, "files");
        string manifestPath = Path.Combine(snapshotRoot, "manifest.json");
        Directory.CreateDirectory(filesRoot);

        var manifest = new BackupManifest
        {
            Id = id,
            GameDirectory = plan.Game.GameDirectory,
            CreatedAt = DateTimeOffset.Now
        };
        SaveManifest(manifestPath, manifest);

        try
        {
            for (int index = 0; index < plan.Operations.Count; index++)
            {
                cancellationToken.ThrowIfCancellationRequested();
                PatchOperation operation = plan.Operations[index];
                string target = Path.GetFullPath(operation.TargetPath);
                if (!SafeFileSystem.IsInside(plan.Game.GameDirectory, target))
                    throw new InvalidOperationException($"Refusing to modify a path outside the selected game: {target}");

                bool existed = File.Exists(target);
                string? backupRelative = null;
                string? originalHash = null;
                if (existed)
                {
                    backupRelative = Path.Combine("files", $"{index:D3}-{SanitizeFileName(Path.GetFileName(target))}");
                    string backupPath = Path.Combine(snapshotRoot, backupRelative);
                    File.Copy(target, backupPath, overwrite: false);
                    originalHash = SafeFileSystem.Sha256(backupPath);
                    if (!SafeFileSystem.Sha256(target).Equals(originalHash, StringComparison.OrdinalIgnoreCase))
                        throw new IOException($"Backup verification failed for {target}");
                }

                var entry = new BackupEntry
                {
                    Kind = operation.Kind,
                    TargetPath = target,
                    Description = operation.Description,
                    Existed = existed,
                    OriginalHash = originalHash,
                    BackupRelativePath = backupRelative
                };
                manifest.Entries.Add(entry);
                // Mark the entry recoverable before the write starts. Restoring
                // an untouched original is harmless, while a process exit in
                // the narrow post-write/pre-save window must not orphan a change.
                entry.Applied = true;
                SaveManifest(manifestPath, manifest);

                progress?.Report(operation.Description);
                entry.ReplacementHash = ApplyOperation(operation, target);
                SaveManifest(manifestPath, manifest);
            }

            manifest.Status = "Complete";
            SaveManifest(manifestPath, manifest);
            progress?.Report($"Backup saved: {snapshotRoot}");
            return manifest;
        }
        catch (Exception ex)
        {
            manifest.Error = ex.Message;
            manifest.Status = "RollingBack";
            SaveManifest(manifestPath, manifest);
            try
            {
                RestoreInternal(manifest, snapshotRoot, progress, CancellationToken.None);
                manifest.Status = "RolledBack";
            }
            catch (Exception rollbackError)
            {
                manifest.Status = "RollbackFailed";
                manifest.Error = $"{ex.Message} | Rollback failed: {rollbackError.Message}";
            }
            SaveManifest(manifestPath, manifest);
            throw new InvalidOperationException($"Patching failed and status is {manifest.Status}: {manifest.Error}", ex);
        }
    }

    private static string? ApplyOperation(PatchOperation operation, string target)
    {
        if (operation.Kind == PatchOperationKind.RemoveFile)
        {
            if (File.Exists(target)) File.Delete(target);
            return null;
        }

        Directory.CreateDirectory(Path.GetDirectoryName(target)!);
        string temporary = target + ".uco2.tmp." + Guid.NewGuid().ToString("N");
        try
        {
            switch (operation.Kind)
            {
                case PatchOperationKind.ReplaceFile:
                    if (operation.SourcePath is null || !File.Exists(operation.SourcePath))
                        throw new FileNotFoundException("Patch source is missing.", operation.SourcePath);
                    File.Copy(operation.SourcePath, temporary, overwrite: false);
                    break;
                case PatchOperationKind.WriteText:
                    File.WriteAllText(temporary, operation.TextContent ?? "", new UTF8Encoding(encoderShouldEmitUTF8Identifier: false));
                    break;
                case PatchOperationKind.WriteBytes:
                    File.WriteAllBytes(temporary, operation.BinaryContent ?? throw new InvalidOperationException("Binary patch content is missing."));
                    break;
                default:
                    throw new InvalidOperationException($"Unsupported operation: {operation.Kind}");
            }

            string expectedHash = SafeFileSystem.Sha256(temporary);
            if (File.Exists(target))
            {
                FileAttributes attributes = File.GetAttributes(target);
                if ((attributes & FileAttributes.ReadOnly) != 0)
                    File.SetAttributes(target, attributes & ~FileAttributes.ReadOnly);
            }
            File.Move(temporary, target, overwrite: true);
            string actualHash = SafeFileSystem.Sha256(target);
            if (!actualHash.Equals(expectedHash, StringComparison.OrdinalIgnoreCase))
                throw new IOException($"Write verification failed for {target}");
            return actualHash;
        }
        finally
        {
            if (File.Exists(temporary)) File.Delete(temporary);
        }
    }

    private static void RestoreInternal(BackupManifest manifest, string snapshotRoot, IProgress<string>? progress, CancellationToken cancellationToken)
    {
        foreach (BackupEntry entry in manifest.Entries.Where(entry => entry.Applied).Reverse())
        {
            cancellationToken.ThrowIfCancellationRequested();
            progress?.Report($"Restore {Path.GetFileName(entry.TargetPath)}");
            if (!entry.Existed)
            {
                if (File.Exists(entry.TargetPath)) File.Delete(entry.TargetPath);
                continue;
            }

            if (entry.BackupRelativePath is null)
                throw new InvalidDataException($"Backup path missing for {entry.TargetPath}");
            string backup = Path.Combine(snapshotRoot, entry.BackupRelativePath);
            if (!File.Exists(backup))
                throw new FileNotFoundException("Backup file is missing.", backup);
            if (!string.Equals(SafeFileSystem.Sha256(backup), entry.OriginalHash, StringComparison.OrdinalIgnoreCase))
                throw new InvalidDataException($"Backup hash does not match for {entry.TargetPath}");

            Directory.CreateDirectory(Path.GetDirectoryName(entry.TargetPath)!);
            string temporary = entry.TargetPath + ".uco2.restore." + Guid.NewGuid().ToString("N");
            try
            {
                File.Copy(backup, temporary, overwrite: false);
                if (File.Exists(entry.TargetPath))
                {
                    FileAttributes attributes = File.GetAttributes(entry.TargetPath);
                    if ((attributes & FileAttributes.ReadOnly) != 0)
                        File.SetAttributes(entry.TargetPath, attributes & ~FileAttributes.ReadOnly);
                }
                File.Move(temporary, entry.TargetPath, overwrite: true);
            }
            finally
            {
                if (File.Exists(temporary)) File.Delete(temporary);
            }
        }
    }

    private string GetGameBackupRoot(string gameDirectory)
    {
        string normalized = Path.TrimEndingDirectorySeparator(Path.GetFullPath(gameDirectory)).ToUpperInvariant();
        string key = Convert.ToHexString(SHA256.HashData(Encoding.UTF8.GetBytes(normalized)))[..16];
        return Path.Combine(StorageRoot, key);
    }

    private static BackupManifest LoadManifest(string manifestPath)
    {
        BackupManifest? manifest = JsonSerializer.Deserialize<BackupManifest>(File.ReadAllText(manifestPath), JsonOptions);
        return manifest ?? throw new InvalidDataException("Backup manifest is empty or invalid.");
    }

    private static void SaveManifest(string manifestPath, BackupManifest manifest)
    {
        string temporary = manifestPath + ".tmp";
        File.WriteAllText(temporary, JsonSerializer.Serialize(manifest, JsonOptions), new UTF8Encoding(false));
        File.Move(temporary, manifestPath, overwrite: true);
    }

    private static string SanitizeFileName(string value)
    {
        foreach (char invalid in Path.GetInvalidFileNameChars())
            value = value.Replace(invalid, '_');
        return value;
    }
}
