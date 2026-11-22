/**
 *  This should only be a debug build feature, as it is not needed by myself really.
 *  This also only dumps crashes that may occur when using this, I cannot guarantee stability. 
 *  So do expect potential issues. Thank you for your patience.
 *
 *  ~veeλnti<3 2026
 */

#pragma once

#if defined(_DEBUG) && defined(_WIN32)

#include <Windows.h>
#include <string>
#include <DbgHelp.h>

typedef BOOL(WINAPI* Fn_MiniDumpWriteDump)(HANDLE hProcess, DWORD ProcessId, HANDLE hFile,
	MINIDUMP_TYPE DumpType, CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
	CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
	CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

class CDumpHandler
{
private:
	HMODULE m_hDbgHelp;
	Fn_MiniDumpWriteDump m_pfnWriteDump;
	std::wstring m_Comment;
	SRWLOCK m_Lock;
	bool m_bReady;

public:
	CDumpHandler();
	~CDumpHandler();

	bool IsReady();
	void SetComment(const wchar_t* comment);
	size_t GetCommentByteSize();
	const wchar_t* GetComment();
	void ClearComment();
	void WriteDump(DWORD exceptionCode, _EXCEPTION_POINTERS* pExceptionInfo);
};

#endif
