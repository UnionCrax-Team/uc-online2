/**
 *  I know it's named dll_loader.h, but in reality it does more than just that.
 *  I added the other features after I had made it and established it would be
 *  just a dll injector / loader and didn't realize my fuckup. I'm gonna try and
 *  change it at some point and hope it doesn't break anything later on down the
 *  road. Right now, this handles the injection loader, the ini configuration,
 *  and the appid customization stuff. And now, it handles the steam stub loading
 *  and live patching as well. The actual code can be found at the end of dllmain.cpp.
 *
 *  ~veeλnti<3 2026
 */

/** 
 *  Finally got around to this, lol.
 *  - 5/18/2026
 * 
 *  ~vλ<3
 */
#pragma once

#include <Windows.h>
#include <Shlwapi.h>
#include <vector>
#include <algorithm>
#include <string>

#include "uco_plugin.h"

class CDLLLoader
{
private:
	std::vector<HMODULE> m_Modules;
	char m_IniPath[MAX_PATH];

public:
	CDLLLoader() { m_IniPath[0] = '\0'; }
	~CDLLLoader() { UnloadAll(); }

	void ReadConfig()
	{
		char exeDir[MAX_PATH] = { 0 };
		DWORD len = GetModuleFileNameA(nullptr, exeDir, MAX_PATH);
		if (len == 0) return;
		if (!PathRemoveFileSpecA(exeDir)) return;

		_snprintf_s(m_IniPath, MAX_PATH, _TRUNCATE, "%s\\union-crax.ini", exeDir);

		DWORD attribs = GetFileAttributesA(m_IniPath);
		if (attribs == INVALID_FILE_ATTRIBUTES)
			m_IniPath[0] = '\0';
	}

	// Path to union-crax.ini, or "" when it wasn't found next to the exe.
	const char* GetIniPath() const { return m_IniPath; }


	// ------------------------------------------------------------
	// INI value reading.
	//
	// GetPrivateProfileString does NOT strip inline comments -- Windows only
	// treats ';' at the START of a line as a comment. So
	//     EmulateTicket=true  # Enable ticket emulation
	// reads back as the whole string "true  # Enable ticket emulation", and an
	// exact compare against "true" fails. That silently disabled EmulateTicket
	// for anyone who copied the commented example out of the README -- the
	// feature reported nothing, it simply never armed.
	//
	// A comment is only recognised when the '#' or ';' is preceded by
	// whitespace, so a value that legitimately contains one (a path, a key)
	// survives intact.
	// ------------------------------------------------------------
	static void IniCleanValue(char* v)
	{
		if (!v) return;
		for (char* c = v; *c; ++c)
		{
			if ((*c == '#' || *c == ';') && c > v && (c[-1] == ' ' || c[-1] == '\t'))
			{
				*c = '\0';
				break;
			}
		}
		size_t n = strlen(v);
		while (n && (v[n - 1] == ' ' || v[n - 1] == '\t' || v[n - 1] == '\r' || v[n - 1] == '\n'))
			v[--n] = '\0';
		char* q = v;
		while (*q == ' ' || *q == '\t') ++q;
		if (q != v) memmove(v, q, strlen(q) + 1);
	}

	// Reads a value with comments and surrounding whitespace removed. The buffer
	// is sized for the RAW line, not the value, so a long trailing comment can
	// never truncate the value itself.
	void IniReadString(const char* key, const char* def, char* out, size_t cch) const
	{
		char raw[512] = { 0 };
		GetPrivateProfileStringA("Settings", key, def, raw, sizeof(raw), m_IniPath);
		IniCleanValue(raw);
		strncpy_s(out, cch, raw, _TRUNCATE);
	}

	bool IniReadBool(const char* key, bool def) const
	{
		char v[64] = { 0 };
		IniReadString(key, def ? "true" : "false", v, sizeof(v));
		return (_stricmp(v, "true") == 0 || _stricmp(v, "1") == 0 ||
		        _stricmp(v, "yes") == 0  || _stricmp(v, "on") == 0);
	}

 	uint32 GetAppId()
 	{
 		if (m_IniPath[0] == '\0')
 			return 480;

 		char buf[64] = { 0 };
 		IniReadString("AppId", "480", buf, sizeof(buf));

 		if (buf[0] == '\0')
 			return 480;

 		uint32 id = (uint32)strtoul(buf, nullptr, 10);
 		return (id == 0) ? 480 : id;
 	}

uint32 GetOgAppId()
  	{
  		if (m_IniPath[0] == '\0')
  	        return 0;

  		char buf[64] = { 0 };
  		IniReadString("ogAppId", "", buf, sizeof(buf));

  		if (buf[0] == '\0')
  			return 0;

  		uint32 id = (uint32)strtoul(buf, nullptr, 10);
  		return id;
  	}

	bool GetSDREnabled()
	{
		if (m_IniPath[0] == '\0')
			return false;

		return IniReadBool("SDR", false);
	}

	// [Settings] RealAppIdEnv=true -- expose ogAppId in the SteamAppId/SteamGameId
	// environment variables for games that read that env at startup and refuse to
	// run if it isn't their real AppId (e.g. Valheim's SteamManager: "Invalid APPID"
	// -> Application.Quit(), checked BEFORE it calls SteamAPI_Init). The Steam
	// connection itself still uses the forced (spoof) AppId, so ownership rides the
	// free app. Needs ogAppId set.
	bool GetRealAppIdEnv()
	{
		if (m_IniPath[0] == '\0')
			return false;

		return IniReadBool("RealAppIdEnv", false);
	}

	std::vector<uint32> GetUnlockDLCAppIds()
	{
		std::vector<uint32> appIds;
		if (m_IniPath[0] == '\0')
			return appIds;

		char buf[1024] = { 0 };
		IniReadString("UnlockDLC", "", buf, sizeof(buf));

		if (buf[0] == '\0')
			return appIds;

		char* token = strtok(buf, ",");
		while (token != nullptr)
		{
			while (*token == ' ' || *token == '\t') token++;
			if (token[0] != '\0')
			{
				uint32 id = (uint32)strtoul(token, nullptr, 10);
				if (id != 0)
					appIds.push_back(id);
			}
			token = strtok(nullptr, ",");
		}

		return appIds;
	}

	bool GetEmulateTicketEnabled()
	{
		if (m_IniPath[0] == '\0')
			return false;

		return IniReadBool("EmulateTicket", false);
	}

	// Explicitly keep Steam's genuine ticket path. This is useful for games that
	// only need a real ticket to advance to a downstream auth layer which a
	// plugin handles separately. It intentionally overrides EmulateTicket when
	// both keys are present, making old/generated configs safe to amend.
	bool GetPassthroughTicketEnabled()
	{
		if (m_IniPath[0] == '\0')
			return false;

		return IniReadBool("PassthroughTicket", false);
	}

	bool GetSteamStubEnabled()
	{
		if (m_IniPath[0] == '\0')
			return false;

		return IniReadBool("GetStubbedLol", false);
	}

	// [Settings] InventoryAutoGrant=1 -- emulate ISteamInventory locally so
	// in-game purchases/grants succeed and persist (real Steam has no items for
	// the spoofed AppId). Off by default; prices/definitions still pass through.
	bool GetInventoryAutoGrant()
	{
		if (m_IniPath[0] == '\0')
			return false;

		return IniReadBool("InventoryAutoGrant", false);
	}

	// [Settings] LocalSaves=true (DEFAULT ON) -- keep Steam Cloud saves out of the
	// spoofed AppId's shared bucket. Real Steam binds ISteamRemoteStorage to the
	// RUNNING app, so every UCO2 game otherwise writes into 480/remote together and
	// games that pick a common name ("SaveData") overwrite each other -- and that
	// bucket really does sync, so another machine pulls the wrong game's save down.
	// Reports cloud as disabled (so a game with its own save path uses it) and
	// serves the file API from <game>\uco_cloud for games that have no other path.
	bool GetLocalSaves()
	{
		if (m_IniPath[0] == '\0')
			return true;

		return IniReadBool("LocalSaves", true);
	}

	// [Settings] VerboseLog=true re-enables the very chatty per-frame /
	// per-callback log lines (RunCallbacks, ContextInit, GetHSteamPipe,
	// callback dispatch traces). Off by default: those fire every frame and
	// bury the useful lines under tens of thousands of entries.
	bool GetVerboseLog()
	{
		if (m_IniPath[0] == '\0')
			return false;

		return IniReadBool("VerboseLog", false);
	}

	bool GetWarnOverlayDisabled()
	{
		if (m_IniPath[0] == '\0')
			return false;

		return IniReadBool("WarnOverlayDisabled", false);
	}

	// [Settings] Client pins what the exported SteamClient() accessor hands
	// back, written as just the version number:  Client=017
	//
	// A game built against an old SDK calls that accessor and then indexes the
	// returned vtable using the offsets from ITS headers. Hand such a caller a
	// modern SteamClient023 and every getter lands on the wrong slot: Rivals of
	// Aether asked for GetISteamUGC, got something else, stored the result
	// without checking, and died dereferencing it later.
	//
	// Empty (the default) keeps the modern interface, which is right for
	// everything else. "017" and "SteamClient017" are both accepted.
	void GetClientVersion(char* out, size_t cch) const
	{
		if (cch) out[0] = '\0';
		if (m_IniPath[0] == '\0') return;

		char raw[64] = { 0 };
		IniReadString("Client", "", raw, sizeof(raw));
		if (raw[0] == '\0') return;

		// Bare digits are the documented form; expand them to the real name.
		bool digitsOnly = true;
		for (const char* c = raw; *c; ++c)
			if (*c < '0' || *c > '9') { digitsOnly = false; break; }

		if (digitsOnly)
			_snprintf_s(out, cch, _TRUNCATE, "SteamClient%s", raw);
		else
			strncpy_s(out, cch, raw, _TRUNCATE);
	}

	// [Settings] LoadOverlay=no stops UCOnline2 pulling Steam's
	// GameOverlayRenderer into the process. Normally that load is what gives a
	// directly-launched game its overlay, but the renderer hooks D3D/DXGI, and a
	// game that dislikes being hooked that early has no other way to opt out.
	// Default on, because for most titles it is the whole point.
	bool GetLoadOverlay()
	{
		if (m_IniPath[0] == '\0')
			return true;

		return IniReadBool("LoadOverlay", true);
	}

	// This does not need to be set!! It will automatically run as true!!
	bool GetForceOwnership()
	{
    	if (m_IniPath[0] == '\0')
        	return true;
    
    	return IniReadBool("ForceOwnership", true);
	}

	void LoadPlugins()
	{
		if (m_IniPath[0] == '\0')
			return;

		char exeDir[MAX_PATH] = { 0 };
		GetModuleFileNameA(nullptr, exeDir, MAX_PATH);
		PathRemoveFileSpecA(exeDir);

		char folderName[MAX_PATH] = { 0 };
		IniReadString("PluginsFolder", "", folderName, MAX_PATH);

		if (folderName[0] == '\0')
			return;

		char dllPath[MAX_PATH] = { 0 };
		if (_snprintf_s(dllPath, MAX_PATH, _TRUNCATE, "%s\\%s", exeDir, folderName) == _TRUNCATE)
			return;

		DWORD folderAttribs = GetFileAttributesA(dllPath);
		if (folderAttribs == INVALID_FILE_ATTRIBUTES || !(folderAttribs & FILE_ATTRIBUTE_DIRECTORY))
			return;

		char findPattern[MAX_PATH] = { 0 };
		if (_snprintf_s(findPattern, MAX_PATH, _TRUNCATE, "%s\\*.dll", dllPath) == _TRUNCATE)
			return;

		std::vector<std::string> names;
		std::vector<std::string> paths;

		WIN32_FIND_DATAA fd = { 0 };
		HANDLE hFind = FindFirstFileA(findPattern, &fd);

		if (hFind == INVALID_HANDLE_VALUE)
			return;

		do
		{
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				continue;

			char fullPath[MAX_PATH] = { 0 };
			if (_snprintf_s(fullPath, MAX_PATH, _TRUNCATE, "%s\\%s", dllPath, fd.cFileName) == _TRUNCATE)
				continue;

			names.push_back(fd.cFileName);
			paths.push_back(fullPath);
		} while (FindNextFileA(hFind, &fd));

		FindClose(hFind);

		for (size_t i = 0; i < names.size(); i++)
		{
			size_t minIdx = i;
			for (size_t j = i + 1; j < names.size(); j++)
			{
				if (_stricmp(names[j].c_str(), names[minIdx].c_str()) < 0)
					minIdx = j;
			}

			if (minIdx != i)
			{
				std::swap(names[i], names[minIdx]);
				std::swap(paths[i], paths[minIdx]);
			}
		}

		for (size_t i = 0; i < names.size(); i++)
		{
			HMODULE hMod = LoadLibraryExA(paths[i].c_str(), nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
			if (hMod)
			{
				m_Modules.push_back(hMod);
				m_Names.push_back(names[i]);
				UCOLOG("[UCOnline2] Loaded plugin: %s", names[i].c_str());
			}
			else
			{
				UCOLOG("[UCOnline2] Failed to load plugin: %s (error %lu)", names[i].c_str(), GetLastError());
			}
		}
	}

	// Call UCO_PluginInit on every loaded plugin that exports it.
	// Returns the number of plugins that returned 0 (success).
	size_t InitPlugins(const UCO_PluginContext* ctx)
	{
		size_t ok = 0;
		for (size_t i = 0; i < m_Modules.size(); i++)
		{
			HMODULE hMod = m_Modules[i];
			if (!hMod) continue;

			UCO_PluginInit_Fn pInit =
				(UCO_PluginInit_Fn)GetProcAddress(hMod, "UCO_PluginInit");
			if (!pInit) continue;

			int rc = pInit(ctx);
			if (rc == 0)
			{
				ok++;
				UCOLOG("[UCOnline2] Plugin init OK: %s", m_Names[i].c_str());
			}
			else
			{
				UCOLOG("[UCOnline2] Plugin init returned %d: %s", rc, m_Names[i].c_str());
			}
		}
		return ok;
	}

	// Call UCO_PluginShutdown on every loaded plugin (reverse load
	// order) that exports it. Idempotent.
	void ShutdownPlugins()
	{
		if (m_bShutdownCalled) return;
		m_bShutdownCalled = true;

		for (size_t ri = m_Modules.size(); ri > 0; --ri)
		{
			size_t i = ri - 1;
			HMODULE hMod = m_Modules[i];
			if (!hMod) continue;
			UCO_PluginShutdown_Fn pShut =
				(UCO_PluginShutdown_Fn)GetProcAddress(hMod, "UCO_PluginShutdown");
			if (pShut) pShut();
		}
	}

	void UnloadAll()
	{
		ShutdownPlugins();
		for (size_t i = 0; i < m_Modules.size(); i++)
		{
			if (m_Modules[i])
				FreeLibrary(m_Modules[i]);
		}
		m_Modules.clear();
		m_Names.clear();
	}

	size_t LoadedCount() const { return m_Modules.size(); }

private:
	std::vector<std::string> m_Names;
	bool m_bShutdownCalled = false;
};
