#ifndef UCO2_VERSION_H
#define UCO2_VERSION_H

// Single source of truth for the DLL's version resource (uc_online2.rc).
//
// On a tagged release, .github/workflows/release.yml regenerates this file from
// the pushed git tag before building, so the shipped DLLs always carry the
// release version with no manual edit. The values committed here are the
// fallback used for local (non-tagged) builds -- keep them roughly current.
#define VER_FILEVERSION      1,20,6,0
#define VER_FILEVERSION_STR  "1.20.6.0"

#endif // UCO2_VERSION_H
