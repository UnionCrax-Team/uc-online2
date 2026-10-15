// Standalone exercise of include/api/remote_storage_emu.h against stubs for the
// handful of UCO2/Steamworks symbols it leans on. Not part of the build -- it is
// here so the path handling, enumeration and async slicing get run at least once
// before shipping a default-ON feature that owns people's saves.
//
// Build and run:  tests\build_cloudemu_test.bat
#include <windows.h>
#include <cstdio>
#include <cstdint>
#include <string>

typedef int32_t  int32;
typedef uint32_t uint32;
typedef int64_t  int64;
typedef uint64_t uint64;
typedef uint64   SteamAPICall_t;
typedef uint64   UGCFileWriteStreamHandle_t;
const UGCFileWriteStreamHandle_t k_UGCFileStreamHandleInvalid = 0xffffffffffffffffull;
#define S_CALLTYPE __cdecl

enum EResult { k_EResultOK = 1, k_EResultFail = 2, k_EResultFileNotFound = 9 };

struct SteamAPICallCompleted_t { enum { k_iCallback = 703 }; SteamAPICall_t m_hAsyncCall; int m_iCallback; uint32 m_cubParam; };
struct RemoteStorageFileWriteAsyncComplete_t { enum { k_iCallback = 1331 }; EResult m_eResult; };
struct RemoteStorageFileReadAsyncComplete_t { enum { k_iCallback = 1332 }; SteamAPICall_t m_hFileReadAsync; EResult m_eResult; uint32 m_nOffset; uint32 m_cubRead; };

static int g_posted = 0;
struct StubDispatcher { void PostCallback(int, const void*, int, int, bool, int) { ++g_posted; } };
static StubDispatcher s_disp;
static StubDispatcher* GetDispatcher() { return &s_disp; }
static int g_ClientUser = 1;

#define UCOLOG(...) do { printf("    log: "); printf(__VA_ARGS__); printf("\n"); } while (0)

#include "api/remote_storage_emu.h"

static int g_fail = 0;
static void Check(bool ok, const char* what) {
    printf("%s  %s\n", ok ? "[ok]  " : "[FAIL]", what);
    if (!ok) ++g_fail;
}

// Wipe the scratch tree so a previous run cannot make the counts lie.
static void RemoveTree(const std::string& dir) {
    WIN32_FIND_DATAA ffd{};
    HANDLE h = FindFirstFileA((dir + "\\*").c_str(), &ffd);
    if (h != INVALID_HANDLE_VALUE) {
        do {
            if (!strcmp(ffd.cFileName, ".") || !strcmp(ffd.cFileName, "..")) continue;
            std::string child = dir + "\\" + ffd.cFileName;
            if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) RemoveTree(child);
            else DeleteFileA(child.c_str());
        } while (FindNextFileA(h, &ffd));
        FindClose(h);
    }
    RemoveDirectoryA(dir.c_str());
}

int main() {
    char tmp[MAX_PATH]; GetTempPathA(MAX_PATH, tmp);
    std::string base = std::string(tmp) + "uco_cloud_test";
    RemoveTree(base);
    // Init() takes an ini path and stores beside it.
    CreateDirectoryA(base.c_str(), nullptr);
    std::string ini = base + "\\union-crax.ini";
    UcoCloudEmu::Init(ini.c_str());
    Check(UcoCloudEmu::Enabled(), "Init enables the emu");

    // --- basic write / read / exists / size
    const char* payload = "hello-save";
    Check(UcoCloudEmu::FileWrite("SaveData", payload, 10), "FileWrite");
    Check(UcoCloudEmu::FileExists("SaveData"), "FileExists after write");
    Check(UcoCloudEmu::GetFileSize("SaveData") == 10, "GetFileSize");
    Check(UcoCloudEmu::GetFileTimestamp("SaveData") > 1600000000LL, "GetFileTimestamp is a sane unix time");
    char buf[32] = {};
    Check(UcoCloudEmu::FileRead("SaveData", buf, sizeof(buf)) == 10 && memcmp(buf, payload, 10) == 0, "FileRead round-trip");

    // A short buffer must truncate, not overflow.
    char shortBuf[4] = {};
    Check(UcoCloudEmu::FileRead("SaveData", shortBuf, 4) == 4, "FileRead honours a short buffer");

    // --- nested paths, forward slashes (Unreal-style names)
    Check(UcoCloudEmu::FileWrite("Saved/SaveGames/slot0.dat", "x", 1), "FileWrite creates subdirectories");
    Check(UcoCloudEmu::FileExists("Saved/SaveGames/slot0.dat"), "nested FileExists (forward slashes)");
    Check(UcoCloudEmu::FileExists("Saved\\SaveGames\\slot0.dat"), "same file via backslashes");

    // --- traversal must be refused
    Check(!UcoCloudEmu::FileWrite("..\\escape.txt", "x", 1), "rejects ..\\ traversal");
    Check(!UcoCloudEmu::FileWrite("a/../../escape.txt", "x", 1), "rejects nested traversal");
    Check(!UcoCloudEmu::FileWrite("C:\\Windows\\escape.txt", "x", 1), "rejects an absolute path");
    Check(GetFileAttributesA((base + "\\escape.txt").c_str()) == INVALID_FILE_ATTRIBUTES, "nothing escaped the root");

    // --- enumeration
    int32 count = UcoCloudEmu::GetFileCount();
    Check(count == 2, "GetFileCount sees both files (recursively)");
    bool sawNested = false, sawFlat = false;
    for (int32 i = 0; i < count; ++i) {
        int32 sz = -1;
        const char* nm = UcoCloudEmu::GetFileNameAndSize(i, &sz);
        if (strcmp(nm, "SaveData") == 0 && sz == 10) sawFlat = true;
        if (strcmp(nm, "Saved/SaveGames/slot0.dat") == 0 && sz == 1) sawNested = true;
    }
    Check(sawFlat, "enumerated the flat file with its size");
    Check(sawNested, "enumerated the nested file with a forward-slash relative name");
    int32 sz = -1;
    Check(UcoCloudEmu::GetFileNameAndSize(99, &sz)[0] == '\0' && sz == 0, "out-of-range index is empty, not a crash");

    // --- cloud state
    Check(!UcoCloudEmu::IsCloudEnabled(), "cloud reports as disabled");
    uint64 total = 0, avail = 0;
    Check(UcoCloudEmu::GetQuota(&total, &avail) && total > 0 && avail > 0, "GetQuota");

    // --- async write + read, including the offset slice
    g_posted = 0;
    SteamAPICall_t wc = UcoCloudEmu::FileWriteAsync("async.bin", "ABCDEFGH", 8);
    Check(UcoCloudEmu::IsOurCall(wc), "FileWriteAsync returns one of our handles");
    Check(g_posted == 1, "FileWriteAsync posted a call-completed callback");
    RemoteStorageFileWriteAsyncComplete_t wres{};
    bool failed = true;
    Check(UcoCloudEmu::GetAPICallResult(wc, &wres, sizeof(wres), 0, &failed) && !failed && wres.m_eResult == k_EResultOK,
          "FileWriteAsync call result is OK");

    SteamAPICall_t rc = UcoCloudEmu::FileReadAsync("async.bin", 3, 4);
    RemoteStorageFileReadAsyncComplete_t rres{};
    Check(UcoCloudEmu::GetAPICallResult(rc, &rres, sizeof(rres), 0, &failed), "FileReadAsync call result fetched");
    Check(rres.m_hFileReadAsync == rc, "read result carries its own handle");
    Check(rres.m_eResult == k_EResultOK && rres.m_nOffset == 3 && rres.m_cubRead == 4, "read result reports offset/length");
    char rbuf[8] = {};
    Check(UcoCloudEmu::FileReadAsyncComplete(rc, rbuf, 4) && memcmp(rbuf, "DEFG", 4) == 0, "FileReadAsyncComplete returns the right slice");
    Check(!UcoCloudEmu::FileReadAsyncComplete(rc, rbuf, 4), "the buffered read is consumed once");

    SteamAPICall_t miss = UcoCloudEmu::FileReadAsync("nope.bin", 0, 4);
    RemoteStorageFileReadAsyncComplete_t mres{};
    UcoCloudEmu::GetAPICallResult(miss, &mres, sizeof(mres), 0, &failed);
    Check(mres.m_eResult == k_EResultFileNotFound, "missing file reads back as FileNotFound");

    // --- write streams
    UGCFileWriteStreamHandle_t h = UcoCloudEmu::FileWriteStreamOpen("stream/out.bin");
    Check(h != k_UGCFileStreamHandleInvalid, "FileWriteStreamOpen");
    Check(UcoCloudEmu::FileWriteStreamWriteChunk(h, "12345", 5), "stream chunk 1");
    Check(UcoCloudEmu::FileWriteStreamWriteChunk(h, "678", 3), "stream chunk 2");
    Check(!UcoCloudEmu::FileExists("stream/out.bin"), "stream is not written until Close");
    Check(UcoCloudEmu::FileWriteStreamClose(h), "FileWriteStreamClose");
    char sbuf[16] = {};
    Check(UcoCloudEmu::FileRead("stream/out.bin", sbuf, sizeof(sbuf)) == 8 && memcmp(sbuf, "12345678", 8) == 0,
          "stream chunks concatenated in order");
    UGCFileWriteStreamHandle_t h2 = UcoCloudEmu::FileWriteStreamOpen("stream/cancelled.bin");
    UcoCloudEmu::FileWriteStreamWriteChunk(h2, "zz", 2);
    Check(UcoCloudEmu::FileWriteStreamCancel(h2) && !UcoCloudEmu::FileExists("stream/cancelled.bin"), "cancelled stream writes nothing");

    // --- delete
    Check(UcoCloudEmu::FileDelete("SaveData") && !UcoCloudEmu::FileExists("SaveData"), "FileDelete");
    Check(!UcoCloudEmu::FileDelete("SaveData"), "deleting a missing file fails");

    printf("\n%s\n", g_fail ? "FAILURES" : "all checks passed");
    return g_fail ? 1 : 0;
}
