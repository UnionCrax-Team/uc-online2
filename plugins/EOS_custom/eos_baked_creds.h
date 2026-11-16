#ifndef UCO_EOS_BAKED_CREDS_H
#define UCO_EOS_BAKED_CREDS_H
// ============================================================
// Default Epic app credentials, baked into EOS_custom.dll at RELEASE time.
//
// These are EMPTY in source on purpose:
//   * local/dev builds ship no baked creds and stay ini-only (no behavior change);
//   * the release CI (.github/workflows/release.yml, step "Stamp EOS default app
//     creds") regenerates this file from GitHub secrets before building the
//     plugins, so the published DLL carries the project's default Epic app and
//     "just works" with no Epic app of the user's own.
//
// PRECEDENCE (see LoadEosConfig): a COMPLETE [EOS] redirect block in
// union-crax.ini overrides these (bring-your-own-app). [EOS] KeepGameApp=1 wins
// over both (stay on the game's own Epic app, no redirect).
//
// NOTE: anything baked here is recoverable from the shipped DLL (it is a public
// binary). Only ever bake an Epic app you are comfortable distributing.
// ============================================================
#define UCO_EOS_BAKED_PRODUCTID    ""
#define UCO_EOS_BAKED_SANDBOXID    ""
#define UCO_EOS_BAKED_DEPLOYMENTID ""
#define UCO_EOS_BAKED_CLIENTID     ""
#define UCO_EOS_BAKED_CLIENTSECRET ""
#endif
