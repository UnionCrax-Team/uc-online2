// ============================================================
// Minimal IL2CPP runtime wrapper
//
// Unity IL2CPP games (like Outbound) compile their C# to native
// code in GameAssembly.dll, accessible at runtime via the
// il2cpp_* C exports of that DLL. We use those exports to walk
// the loaded type system, find a specific C# method by name,
// and get a native function pointer to its compiled body. That
// pointer can then be MinHook'd just like any other function.
//
// This mirrors what OnlineFix's per-game Custom.dll does. We
// confirmed via API Monitor that Custom.dll resolves the same
// family of il2cpp_* functions at runtime.
//
// Compatible with Unity 2020+ IL2CPP runtimes. Older Unity
// versions may have a different MethodInfo layout; we only
// touch the first field (methodPointer) which has been stable
// across versions.
// ============================================================
#pragma once

#include <Windows.h>
#include <stdint.h>

// Opaque types -- we never dereference these except for MethodInfo
typedef struct Il2CppDomain    Il2CppDomain;
typedef struct Il2CppAssembly  Il2CppAssembly;
typedef struct Il2CppImage     Il2CppImage;
typedef struct Il2CppClass     Il2CppClass;
typedef struct Il2CppObject    Il2CppObject;
typedef struct Il2CppType      Il2CppType;

// MethodInfo: only the first field is documented stable across
// Unity versions. We treat the rest as opaque.
typedef void* Il2CppMethodPointer;
typedef struct MethodInfo
{
    Il2CppMethodPointer methodPointer;
    // ... other fields we don't touch ...
} MethodInfo;

#ifdef __cplusplus
extern "C" {
#endif

// ---- Lifecycle ----

// Returns true if GameAssembly.dll is loaded AND we successfully
// resolved every il2cpp_* function we need. Safe to call repeatedly.
// Logs once on success.
bool IL2CPP_TryInit(void);

// True once IL2CPP_TryInit has succeeded at least once.
bool IL2CPP_IsReady(void);

// ---- Lookups ----

// Find a class by namespace + name in a specific assembly image.
// `imageName` is the assembly basename without extension, e.g.
// "Assembly-CSharp", "UnityEngine.CoreModule".
//
// Returns nullptr if not found.
Il2CppClass* IL2CPP_FindClass(const char* imageName,
                              const char* namespaceName,
                              const char* className);

// Find a method by name on a class. argCount = -1 disables the
// arg-count filter (matches any overload).
// Returns nullptr if not found.
const MethodInfo* IL2CPP_FindMethod(Il2CppClass* klass,
                                    const char* methodName,
                                    int argCount);

// Convenience: find a method directly by full path. Returns the
// native function pointer of its compiled body (NOT the
// MethodInfo). nullptr on failure.
void* IL2CPP_FindMethodPtr(const char* imageName,
                           const char* namespaceName,
                           const char* className,
                           const char* methodName,
                           int argCount);

// Allocate a new managed System.String from a UTF-8 C string.
// Returned pointer is GC-managed -- the il2cpp domain owns it.
// Returns nullptr if the runtime isn't ready or allocation
// fails.
void* IL2CPP_StringNew(const char* utf8);

// Walk every method on `klass` and log its name + parameter
// count via IL2CPP_Log, prefixed with `[il2cpp][dump <name>]`.
// Useful for diagnosing "I know the class but not the method"
// situations. No-op if the runtime can't enumerate methods.
void IL2CPP_DumpClassMethods(Il2CppClass* klass, const char* classNameForLog);

#ifdef __cplusplus
}
#endif
