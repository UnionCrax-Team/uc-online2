using System.ComponentModel;
using System.Runtime.InteropServices;
using System.Text;
using System.Text.Json;

namespace UCO2.Patcher.Core;

public sealed class BackendProfileStore
{
    private static readonly JsonSerializerOptions JsonOptions = new() { WriteIndented = true };
    private readonly string profilePath;
    private readonly DpapiUserProtector protector = new();

    public BackendProfileStore(string? profilePath = null)
    {
        this.profilePath = profilePath ?? Path.Combine(
            Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData),
            "UCOnline2", "Patcher", "backend-profiles.dat");
    }

    public BackendProfiles Load(out string? warning)
    {
        warning = null;
        if (!File.Exists(profilePath)) return new BackendProfiles();
        try
        {
            byte[] encrypted = File.ReadAllBytes(profilePath);
            byte[] json = protector.Unprotect(encrypted);
            return JsonSerializer.Deserialize<BackendProfiles>(json, JsonOptions) ?? new BackendProfiles();
        }
        catch (Exception ex)
        {
            warning = $"Saved backend settings could not be read: {ex.Message}";
            return new BackendProfiles();
        }
    }

    public IReadOnlyList<string> SaveFrom(PatchOptions options)
    {
        BackendProfiles profiles = Load(out string? warning);
        if (warning is not null) throw new InvalidDataException(warning);

        var saved = new List<string>();
        DateTimeOffset now = DateTimeOffset.UtcNow;
        if (options.InstallPhoton && HasAny(options.PhotonRealtimeAppId, options.PhotonFusionAppId, options.PhotonVoiceAppId))
        {
            profiles.Photon = new PhotonBackendProfile
            {
                UpdatedAt = now,
                RealtimeAppId = options.PhotonRealtimeAppId,
                FusionAppId = options.PhotonFusionAppId,
                VoiceAppId = options.PhotonVoiceAppId
            };
            saved.Add("Photon");
        }
        if (options.InstallEos && HasAny(options.EosProductId, options.EosSandboxId, options.EosDeploymentId, options.EosClientId, options.EosClientSecret, options.DisplayName))
        {
            profiles.Eos = new EosBackendProfile
            {
                UpdatedAt = now,
                ProductId = options.EosProductId,
                SandboxId = options.EosSandboxId,
                DeploymentId = options.EosDeploymentId,
                ClientId = options.EosClientId,
                ClientSecret = options.EosClientSecret,
                DisplayName = options.DisplayName
            };
            saved.Add("EOS");
        }
        if (options.InstallPlayFab && HasAny(options.PlayFabTitleId))
        {
            profiles.PlayFab = new PlayFabBackendProfile
            {
                UpdatedAt = now,
                TitleId = options.PlayFabTitleId
            };
            saved.Add("PlayFab");
        }

        if (saved.Count == 0) return saved;
        byte[] json = JsonSerializer.SerializeToUtf8Bytes(profiles, JsonOptions);
        byte[] encrypted = protector.Protect(json);
        string? directory = Path.GetDirectoryName(profilePath);
        if (!string.IsNullOrWhiteSpace(directory)) Directory.CreateDirectory(directory);
        string temporaryPath = profilePath + ".tmp." + Guid.NewGuid().ToString("N");
        try
        {
            File.WriteAllBytes(temporaryPath, encrypted);
            File.Move(temporaryPath, profilePath, overwrite: true);
        }
        finally
        {
            if (File.Exists(temporaryPath)) File.Delete(temporaryPath);
        }
        return saved;
    }

    private static bool HasAny(params string[] values) => values.Any(value => !string.IsNullOrWhiteSpace(value));
}

public sealed class BackendProfiles
{
    public int FormatVersion { get; set; } = 1;
    public PhotonBackendProfile? Photon { get; set; }
    public EosBackendProfile? Eos { get; set; }
    public PlayFabBackendProfile? PlayFab { get; set; }
}

public sealed class PhotonBackendProfile
{
    public DateTimeOffset UpdatedAt { get; set; }
    public string RealtimeAppId { get; set; } = "";
    public string FusionAppId { get; set; } = "";
    public string VoiceAppId { get; set; } = "";
}

public sealed class EosBackendProfile
{
    public DateTimeOffset UpdatedAt { get; set; }
    public string ProductId { get; set; } = "";
    public string SandboxId { get; set; } = "";
    public string DeploymentId { get; set; } = "";
    public string ClientId { get; set; } = "";
    public string ClientSecret { get; set; } = "";
    public string DisplayName { get; set; } = "";
}

public sealed class PlayFabBackendProfile
{
    public DateTimeOffset UpdatedAt { get; set; }
    public string TitleId { get; set; } = "";
}

internal sealed class DpapiUserProtector
{
    private const int CryptprotectUiForbidden = 0x1;

    public byte[] Protect(byte[] value) => Transform(value, protect: true);
    public byte[] Unprotect(byte[] value) => Transform(value, protect: false);

    private static byte[] Transform(byte[] value, bool protect)
    {
        if (!OperatingSystem.IsWindows()) throw new PlatformNotSupportedException("Backend profile protection requires Windows.");
        IntPtr inputPointer = Marshal.AllocHGlobal(value.Length);
        var input = new DataBlob { Size = value.Length, Data = inputPointer };
        DataBlob output = default;
        try
        {
            Marshal.Copy(value, 0, inputPointer, value.Length);
            bool succeeded = protect
                ? CryptProtectData(ref input, "UCOnline2 backend settings", IntPtr.Zero, IntPtr.Zero, IntPtr.Zero, CryptprotectUiForbidden, out output)
                : CryptUnprotectData(ref input, IntPtr.Zero, IntPtr.Zero, IntPtr.Zero, IntPtr.Zero, CryptprotectUiForbidden, out output);
            if (!succeeded) throw new Win32Exception(Marshal.GetLastWin32Error());
            byte[] result = new byte[output.Size];
            Marshal.Copy(output.Data, result, 0, output.Size);
            return result;
        }
        finally
        {
            Marshal.FreeHGlobal(inputPointer);
            if (output.Data != IntPtr.Zero) LocalFree(output.Data);
        }
    }

    [StructLayout(LayoutKind.Sequential)]
    private struct DataBlob
    {
        public int Size;
        public IntPtr Data;
    }

    [DllImport("crypt32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
    [return: MarshalAs(UnmanagedType.Bool)]
    private static extern bool CryptProtectData(
        ref DataBlob dataIn, string description, IntPtr optionalEntropy, IntPtr reserved,
        IntPtr prompt, int flags, out DataBlob dataOut);

    [DllImport("crypt32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
    [return: MarshalAs(UnmanagedType.Bool)]
    private static extern bool CryptUnprotectData(
        ref DataBlob dataIn, IntPtr description, IntPtr optionalEntropy, IntPtr reserved,
        IntPtr prompt, int flags, out DataBlob dataOut);

    [DllImport("kernel32.dll")]
    private static extern IntPtr LocalFree(IntPtr memory);
}
