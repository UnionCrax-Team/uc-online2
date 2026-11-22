/**
 *  This may cause antiviruses to shit huge ass cinderblocks, but oh well.
 *  It is genuinely only to find where Steam is installed to get to the
 *  steamclient(64).dll file, since when you install it, it always adds
 *  the path to your registry. I wish it would just add it to PATH or do
 *  that too so I could get around that possible malware flag, but hey.
 *  Life ain't fair, now is it? lol.
 *
 *  ~veeλnti<3 2026
 */
#pragma once

#include "platform.h"

LSTATUS GetRegistryDWORD(const char* cszPath, const char* cszKey, DWORD& dwValue)
{
	HKEY hKeyNode = nullptr;
	DWORD dwType = REG_DWORD;
	DWORD dwSize = sizeof(DWORD);

	LSTATUS GetRegKey = ERROR_SUCCESS;

	GetRegKey = RegOpenKeyExA(HKEY_CURRENT_USER, cszPath, 0, KEY_READ, &hKeyNode);

	if (GetRegKey != ERROR_SUCCESS)
		return GetRegKey;

	GetRegKey = RegQueryValueExA(hKeyNode, cszKey, nullptr, &dwType, (LPBYTE)&dwValue, &dwSize);

	RegCloseKey(hKeyNode);

	return GetRegKey;
}

LSTATUS GetRegistryString(const char* cszPath, const char* cszKey, char* szString, unsigned int uMaxBuf)
{
	DWORD dwType = REG_SZ;
	DWORD dwSize = uMaxBuf;

	LSTATUS GetRegKey = RegGetValueA(HKEY_CURRENT_USER, cszPath, cszKey, RRF_RT_REG_SZ, &dwType, szString, &dwSize);

	return GetRegKey;
}
