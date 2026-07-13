// ============================================================
// Minimal Mono Embed runtime wrapper
//
// Unity games using the Mono runtime (not IL2CPP) ship the
// runtime as mono-2.0-bdwgc.dll in MonoBleedingEdge/EmbedRuntime/.
// It exports a stable C API for embedding, including
// reflection-style lookups of assemblies, classes, and methods,
// and JIT compilation to get native code pointers we can hook.
//
// This wrapper resolves the subset of mono_* APIs we need to
// find Photon's LoadBalancingPeer.OpAuthenticate method and
// patch its compiled native code via MinHook.
// ============================================================
#pragma once

#include <Windows.h>
#include <stdint.h>

typedef struct MonoDomain    MonoDomain;
typedef struct MonoAssembly  MonoAssembly;
typedef struct MonoImage     MonoImage;
typedef struct MonoClass     MonoClass;
typedef struct MonoMethod    MonoMethod;
typedef struct MonoString    MonoString;
typedef struct MonoObject    MonoObject;
typedef struct MonoThread    MonoThread;

#ifdef __cplusplus
extern "C" {
#endif

// Resolve mono-2.0-bdwgc.dll exports we need. Returns true once
// successful (idempotent). Safe to call repeatedly while waiting
// for Unity to load the runtime DLL.
bool MONO_TryInit(void);
bool MONO_IsReady(void);

// Look up a class by namespace + name in a specific assembly
// (basename without .dll extension). If imageName is null, scan
// every loaded assembly and return the first match.
MonoClass* MONO_FindClass(const char* imageName,
                          const char* namespaceName,
                          const char* className);

// Find a method by name on a class. argCount = -1 disables the
// arg-count filter.
MonoMethod* MONO_FindMethod(MonoClass* klass,
                            const char* methodName,
                            int argCount);

// Force the method to be JIT-compiled and return its native
// function pointer (suitable for MinHook). Returns nullptr on
// failure.
void* MONO_GetMethodNativePtr(MonoMethod* method);

// Convenience: full path resolution -> native pointer.
void* MONO_FindMethodPtr(const char* imageName,
                         const char* namespaceName,
                         const char* className,
                         const char* methodName,
                         int argCount);

// Allocate a managed System.String from a UTF-8 C string.
// GC-managed; do not free yourself.
MonoString* MONO_StringNew(const char* utf8);

// On a `Dictionary<byte, object>` instance, set entry (key) -> (boxed byte value).
// Used in the SendOperation hook to rewrite the auth-type param mid-wire.
// Returns true on success.
bool MONO_DictByteByteSetItem(MonoObject* dict, uint8_t key, uint8_t value);

// On a `Dictionary<byte, object>` instance, set entry (key) -> (managed string).
// Used to rewrite the ApplicationId param (key 224) mid-wire.
// Returns true on success.
bool MONO_DictByteStringSetItem(MonoObject* dict, uint8_t key, const char* utf8Value);

// Get the value at key (byte) in a Dictionary<byte, object>. Returns
// NULL if the key isn't present. Returns a MonoObject* that may be a
// boxed value type or a managed reference (e.g., MonoString*).
MonoObject* MONO_DictByteGetItem(MonoObject* dict, uint8_t key);

// Invoke an instance method by name with zero args, blocking
// synchronously. The Mono runtime handles JIT-compile and
// invocation. Returns true on success.
bool MONO_InvokeInstanceVoidMethodNoArgs(MonoClass* klass,
                                         const char* methodName,
                                         void* pThisInstance);

// Return the byte offset of `fieldName` from the start of a
// MonoObject of class `klass`. Returns -1 on failure. Use to do
// direct in-memory writes against instance pointers.
int MONO_GetFieldOffset(MonoClass* klass, const char* fieldName);

// Set a static field on `klass` by name. Returns true on success.
// `valuePtr` must point to a value of the correct size/type for the
// field (e.g. uint8_t* for a bool field, MonoString** for a string).
bool MONO_SetStaticFieldByName(MonoClass* klass,
                               const char* fieldName,
                               void* valuePtr);

// If `obj` is a MonoString, copy its UTF-8 representation into `out`
// (NUL-terminated, truncated to outSize). Returns true on success.
// For non-string objects, writes the type name + a short hex dump.
bool MONO_DescribeObject(MonoObject* obj, char* out, size_t outSize);

#ifdef __cplusplus
}
#endif
