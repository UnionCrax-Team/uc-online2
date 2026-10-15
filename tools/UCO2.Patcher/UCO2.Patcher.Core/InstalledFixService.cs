namespace UCO2.Patcher.Core;

public sealed class InstalledFixService
{
    public IReadOnlyList<InstalledFileStatus> Inspect(PatchPlan plan)
    {
        var statuses = new List<InstalledFileStatus>();
        foreach (PatchOperation operation in plan.Operations.Where(operation => operation.Kind != PatchOperationKind.RemoveFile))
        {
            bool exists = File.Exists(operation.TargetPath);
            bool current = exists && operation.Kind switch
            {
                PatchOperationKind.ReplaceFile when operation.SourcePath is not null && File.Exists(operation.SourcePath) =>
                    SafeFileSystem.Sha256(operation.TargetPath).Equals(SafeFileSystem.Sha256(operation.SourcePath), StringComparison.OrdinalIgnoreCase),
                PatchOperationKind.WriteText =>
                    SafeFileSystem.Sha256(operation.TargetPath).Equals(SafeFileSystem.Sha256Text(operation.TextContent ?? ""), StringComparison.OrdinalIgnoreCase),
                PatchOperationKind.WriteBytes when operation.BinaryContent is not null =>
                    SafeFileSystem.Sha256(operation.TargetPath).Equals(Convert.ToHexString(System.Security.Cryptography.SHA256.HashData(operation.BinaryContent)), StringComparison.OrdinalIgnoreCase),
                _ => false
            };
            statuses.Add(new InstalledFileStatus(operation.Description, operation.TargetPath, exists, current));
        }
        return statuses;
    }
}
