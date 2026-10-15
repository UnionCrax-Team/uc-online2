namespace UCO2.Patcher.Core;

public sealed class PatchPlanner(ArtifactLocator artifacts)
{
    public PatchPlan Create(GameScanResult game, PatchOptions options)
    {
        if (options.AppId == 0) throw new InvalidOperationException("AppId must be greater than zero.");
        if (options.OriginalAppId == 0) throw new InvalidOperationException("Enter the game's real Steam AppId.");
        if (options.EnableSdr && options.OriginalAppId == 0) throw new InvalidOperationException("SDR requires ogAppId.");

        var operations = new List<PatchOperation>();
        var warnings = new List<string>(game.Warnings);
        string? steamSource = artifacts.FindSteamApi(game.Architecture);
        if (steamSource is null)
            throw new FileNotFoundException($"The {game.Architecture} UCOnline2 Steam API build was not found beside the patcher.");

        // Replace EVERY steam_api64.dll copy in the game folder. A Unity game
        // that ships one in the root and one under Data/Plugins/x86_64 loads the
        // Data/Plugins copy, so patching only the primary leaves it on the real
        // Steam DLL and UCOnline2 never gets called.
        IReadOnlyList<string> apiTargets = game.SteamApiPaths.Count > 0
            ? game.SteamApiPaths
            : [game.SteamApiPath];
        foreach (string target in apiTargets)
        {
            operations.Add(new PatchOperation
            {
                Kind = PatchOperationKind.ReplaceFile,
                SourcePath = steamSource,
                TargetPath = target,
                Description = apiTargets.Count > 1
                    ? $"Install {Path.GetFileName(target)} ({Path.GetFileName(Path.GetDirectoryName(target))})"
                    : $"Install {Path.GetFileName(target)}"
            });
        }

        operations.Add(new PatchOperation
        {
            Kind = PatchOperationKind.WriteText,
            TextContent = ConfigBuilder.Build(game, options),
            TargetPath = Path.Combine(game.ConfigDirectory, "union-crax.ini"),
            Description = "Write union-crax.ini"
        });

        if (options.InstallOverlayProxy && game.Architecture == GameArchitecture.X64)
            AddOverlay(game, operations, warnings);

        if (game.Architecture == GameArchitecture.X64)
        {
            if (options.InstallPhoton) AddPlugin("photon_universal", game, operations);
            if (options.InstallEos)
            {
                // EOS_custom is always installed when EOS is selected. With no ini
                // credentials it redirects to the Epic app baked into the DLL at
                // release ("just works"); a COMPLETE [EOS] block overrides that with
                // your own app, and KeepGameApp overrides both (game's own app, no
                // redirect). Only warn on a PARTIAL own-app block, which the plugin
                // ignores in favour of the baked default.
                AddPlugin("EOS_custom", game, operations);
                int filled = new[] { options.EosProductId, options.EosSandboxId, options.EosDeploymentId,
                    options.EosClientId, options.EosClientSecret }.Count(v => !string.IsNullOrWhiteSpace(v));
                if (!options.EosKeepGameApp && filled > 0 && filled < 5)
                    warnings.Add("EOS: your Epic app block is incomplete, so EOS_custom will ignore it and use the built-in default app. Fill all five ids to use your own app, or clear them.");
            }
            if (options.InstallPlayFab)
            {
                if (!string.IsNullOrWhiteSpace(options.PlayFabTitleId))
                    AddPlugin("playfab_universal", game, operations);
                else
                    warnings.Add("PlayFab was selected but its TitleId is empty, so playfab_universal will not be installed.");
            }
            if (options.InstallCoherence) AddPlugin("coherence_universal", game, operations);
        }
        else if (options.InstallPhoton || options.InstallEos || options.InstallPlayFab || options.InstallCoherence)
        {
            warnings.Add("Plugins are currently x64-only and will be skipped for this 32-bit game.");
        }

        if (options.InstallCoherence && !string.IsNullOrWhiteSpace(options.CoherenceRuntimeKey) && game.UnityDataDirectory is not null)
        {
            string assets = Path.Combine(game.UnityDataDirectory, "globalgamemanagers.assets");
            if (File.Exists(assets))
            {
                operations.Add(new PatchOperation
                {
                    Kind = PatchOperationKind.WriteBytes,
                    BinaryContent = CoherenceKeyPatcher.CreatePatchedCopy(assets, options.CoherenceRuntimeKey),
                    TargetPath = assets,
                    Description = "Patch coherence runtime key"
                });
            }
            else warnings.Add("globalgamemanagers.assets was not found, so the coherence runtime key cannot be patched automatically.");
        }

        if (options.QuarantineCompetingFiles)
        {
            operations.AddRange(game.CompetingFiles.Select(path => new PatchOperation
            {
                Kind = PatchOperationKind.RemoveFile,
                TargetPath = path,
                Description = $"Quarantine competing loader {Path.GetFileName(path)}"
            }));
        }

        return new PatchPlan { Game = game, Options = options, Operations = operations, Warnings = warnings };
    }

    private void AddOverlay(GameScanResult game, List<PatchOperation> operations, List<string> warnings)
    {
        if (Path.GetFileName(game.ExecutablePath).Equals("Phasmophobia.exe", StringComparison.OrdinalIgnoreCase))
        {
            warnings.Add("Phasmophobia rejects an extra version.dll, so the optional overlay proxy will be skipped.");
            return;
        }

        string? source = artifacts.FindOverlayProxy();
        if (source is null)
        {
            warnings.Add("overlay_proxy.dll is not included in this build, so early overlay loading will be skipped.");
            return;
        }

        // One binary, two identities. Unreal shipping exes statically import
        // winmm.dll (timeGetTime), which loads before OEP -- early enough to arm
        // the SteamStub bypass. (The old XINPUT1_3.dll identity loaded after UE5's
        // D3D12 renderer, too late for the stub; patch.bat retires any left behind.)
        string? fileName = game.Engine switch
        {
            EngineKind.Unity => "version.dll",
            EngineKind.Unreal => "winmm.dll",
            _ => null
        };
        if (fileName is null)
            return;

        operations.Add(new PatchOperation
        {
            Kind = PatchOperationKind.ReplaceFile,
            SourcePath = source,
            TargetPath = Path.Combine(Path.GetDirectoryName(game.ExecutablePath)!, fileName),
            Description = $"Install early overlay proxy as {fileName}"
        });
    }

    private void AddPlugin(string name, GameScanResult game, List<PatchOperation> operations)
    {
        string? source = artifacts.FindPlugin(name);
        if (source is null)
            throw new FileNotFoundException($"Selected plugin {name}.dll was not found beside the patcher.");
        operations.Add(new PatchOperation
        {
            Kind = PatchOperationKind.ReplaceFile,
            SourcePath = source,
            TargetPath = Path.Combine(game.ConfigDirectory, "plugins", name + ".dll"),
            Description = $"Install {name}.dll"
        });
    }
}
