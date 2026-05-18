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

 	uint32 GetAppId()
 	{
 		if (m_IniPath[0] == '\0')
 			return 480;

 		char buf[16] = { 0 };
 		GetPrivateProfileStringA("Settings", "AppId", "480", buf, sizeof(buf), m_IniPath);

 		if (buf[0] == '\0')
 			return 480;

 		uint32 id = (uint32)strtoul(buf, nullptr, 10);
 		return (id == 0) ? 480 : id;
 	}

 	uint32 GetOgAppId()
 	{
 		if (m_IniPath[0] == '\0')
 	        return 0;

 		char buf[16] = { 0 };
 		GetPrivateProfileStringA("Settings", "ogAppId", "", buf, sizeof(buf), m_IniPath);

 		if (buf[0] == '\0')
 			return 0;

 		uint32 id = (uint32)strtoul(buf, nullptr, 10);
 		return id;
 	}

	bool GetSteamStubEnabled()
	{
		if (m_IniPath[0] == '\0')
			return false;

		char buf[8] = { 0 };
		GetPrivateProfileStringA("Settings", "GetStubbedLol", "false", buf, sizeof(buf), m_IniPath);

		return (_stricmp(buf, "true") == 0 || _stricmp(buf, "1") == 0 || _stricmp(buf, "yes") == 0);
	}

	void LoadPlugins()
	{
		if (m_IniPath[0] == '\0')
			return;

		char exeDir[MAX_PATH] = { 0 };
		GetModuleFileNameA(nullptr, exeDir, MAX_PATH);
		PathRemoveFileSpecA(exeDir);

		char folderName[MAX_PATH] = { 0 };
		GetPrivateProfileStringA("Settings", "PluginsFolder", "", folderName, MAX_PATH, m_IniPath);

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
				UCOLOG("[UCOnline2] Loaded plugin: %s", names[i].c_str());
			}
			else
			{
				UCOLOG("[UCOnline2] Failed to load plugin: %s (error %lu)", names[i].c_str(), GetLastError());
			}
		}
	}

	void UnloadAll()
	{
		for (size_t i = 0; i < m_Modules.size(); i++)
		{
			if (m_Modules[i])
				FreeLibrary(m_Modules[i]);
		}
		m_Modules.clear();
	}

	size_t LoadedCount() const { return m_Modules.size(); }
};
