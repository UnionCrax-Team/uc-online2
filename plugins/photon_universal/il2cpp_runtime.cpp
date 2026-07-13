#include "il2cpp_runtime.h"

#include <string.h>
#include <stdio.h>

// Logger from outbound_plugin.cpp (set during UCO_PluginInit).
extern "C" void IL2CPP_Log(const char* fmt, ...);

// ============================================================
// IL2CPP function pointers we resolve from GameAssembly.dll
// ============================================================
typedef Il2CppDomain*        (*Fn_il2cpp_domain_get)();
typedef const Il2CppAssembly** (*Fn_il2cpp_domain_get_assemblies)(Il2CppDomain*, size_t*);
typedef const Il2CppImage*   (*Fn_il2cpp_assembly_get_image)(const Il2CppAssembly*);
typedef const char*          (*Fn_il2cpp_image_get_name)(const Il2CppImage*);
typedef Il2CppClass*         (*Fn_il2cpp_class_from_name)(const Il2CppImage*, const char*, const char*);
typedef const MethodInfo*    (*Fn_il2cpp_class_get_method_from_name)(Il2CppClass*, const char*, int);
typedef void                 (*Fn_il2cpp_thread_attach)(Il2CppDomain*);
typedef void*                (*Fn_il2cpp_string_new)(const char*);
typedef Il2CppClass*         (*Fn_il2cpp_object_get_class)(Il2CppObject*);
typedef Il2CppObject*        (*Fn_il2cpp_value_box)(Il2CppClass*, void*);
typedef Il2CppObject*        (*Fn_il2cpp_runtime_invoke)(const MethodInfo*, void*, void**, Il2CppObject**);
typedef const char*          (*Fn_il2cpp_class_get_name)(Il2CppClass*);
typedef char*                (*Fn_il2cpp_string_to_utf8)(void*);
typedef void                 (*Fn_il2cpp_free)(void*);
typedef const MethodInfo*    (*Fn_il2cpp_class_get_methods)(Il2CppClass*, void**);
typedef const char*          (*Fn_il2cpp_method_get_name)(const MethodInfo*);
typedef uint32_t             (*Fn_il2cpp_method_get_param_count)(const MethodInfo*);

static HMODULE g_hGameAssembly = nullptr;
static bool    g_bReady        = false;

static Fn_il2cpp_domain_get                  g_domain_get                  = nullptr;
static Fn_il2cpp_domain_get_assemblies       g_domain_get_assemblies       = nullptr;
static Fn_il2cpp_assembly_get_image          g_assembly_get_image          = nullptr;
static Fn_il2cpp_image_get_name              g_image_get_name              = nullptr;
static Fn_il2cpp_class_from_name             g_class_from_name             = nullptr;
static Fn_il2cpp_class_get_method_from_name  g_class_get_method_from_name  = nullptr;
static Fn_il2cpp_thread_attach               g_thread_attach               = nullptr;
static Fn_il2cpp_string_new                  g_string_new                  = nullptr;
static Fn_il2cpp_object_get_class            g_object_get_class            = nullptr;
static Fn_il2cpp_value_box                   g_value_box                   = nullptr;
static Fn_il2cpp_runtime_invoke              g_runtime_invoke              = nullptr;
static Fn_il2cpp_class_get_name              g_class_get_name              = nullptr;
static Fn_il2cpp_string_to_utf8              g_string_to_utf8              = nullptr;
static Fn_il2cpp_free                        g_free                        = nullptr;
static Fn_il2cpp_class_get_methods           g_class_get_methods           = nullptr;
static Fn_il2cpp_method_get_name             g_method_get_name             = nullptr;
static Fn_il2cpp_method_get_param_count      g_method_get_param_count      = nullptr;
static Il2CppClass*                          g_byteClass                   = nullptr;

#define RESOLVE(name) \
    do { \
        g_##name = (Fn_il2cpp_##name)GetProcAddress(g_hGameAssembly, "il2cpp_" #name); \
        if (!g_##name) { \
            IL2CPP_Log("[il2cpp] GameAssembly missing il2cpp_" #name); \
            return false; \
        } \
    } while (0)

bool IL2CPP_TryInit(void)
{
    if (g_bReady) return true;

    g_hGameAssembly = GetModuleHandleA("GameAssembly.dll");
    if (!g_hGameAssembly) return false; // not loaded yet -- caller retries

    RESOLVE(domain_get);
    RESOLVE(domain_get_assemblies);
    RESOLVE(assembly_get_image);
    RESOLVE(image_get_name);
    RESOLVE(class_from_name);
    RESOLVE(class_get_method_from_name);
    RESOLVE(thread_attach);
    RESOLVE(string_new);

    // Optional helpers used for dict manipulation + method enum.
    // Don't fail if missing -- callers fall back gracefully.
    g_object_get_class       = (Fn_il2cpp_object_get_class)
        GetProcAddress(g_hGameAssembly, "il2cpp_object_get_class");
    g_value_box              = (Fn_il2cpp_value_box)
        GetProcAddress(g_hGameAssembly, "il2cpp_value_box");
    g_runtime_invoke         = (Fn_il2cpp_runtime_invoke)
        GetProcAddress(g_hGameAssembly, "il2cpp_runtime_invoke");
    g_class_get_name         = (Fn_il2cpp_class_get_name)
        GetProcAddress(g_hGameAssembly, "il2cpp_class_get_name");
    g_string_to_utf8         = (Fn_il2cpp_string_to_utf8)
        GetProcAddress(g_hGameAssembly, "il2cpp_string_to_utf8");
    g_free                   = (Fn_il2cpp_free)
        GetProcAddress(g_hGameAssembly, "il2cpp_free");
    g_class_get_methods      = (Fn_il2cpp_class_get_methods)
        GetProcAddress(g_hGameAssembly, "il2cpp_class_get_methods");
    g_method_get_name        = (Fn_il2cpp_method_get_name)
        GetProcAddress(g_hGameAssembly, "il2cpp_method_get_name");
    g_method_get_param_count = (Fn_il2cpp_method_get_param_count)
        GetProcAddress(g_hGameAssembly, "il2cpp_method_get_param_count");

    // Attach the current native thread to the IL2CPP domain so we
    // can call into managed code safely. Required before any
    // il2cpp_class_* call on most Unity versions.
    Il2CppDomain* dom = g_domain_get();
    if (dom)
        g_thread_attach(dom);

    g_bReady = true;
    IL2CPP_Log("[il2cpp] runtime resolved, domain=%p", dom);

    // Dump loaded image names once so we have ground truth in
    // the log for diagnosing class-not-found failures.
    if (dom)
    {
        size_t count = 0;
        const Il2CppAssembly** asms = g_domain_get_assemblies(dom, &count);
        IL2CPP_Log("[il2cpp] %u assemblies loaded:", (unsigned)count);
        for (size_t i = 0; i < count; ++i)
        {
            const Il2CppImage* img = g_assembly_get_image(asms[i]);
            const char* name = img ? g_image_get_name(img) : nullptr;
            if (name) IL2CPP_Log("[il2cpp]   %s", name);
        }
    }
    return true;
}

bool IL2CPP_IsReady(void) { return g_bReady; }

// Walks the loaded assemblies, finds one whose image name matches
// `imageName`, then looks up class by namespace+name.
//
// If `imageName` is null or no image matches it, falls back to
// searching every loaded assembly. This makes the helper robust
// against per-game assembly-name variation (e.g. Fusion ships
// `Fusion.Realtime.dll` but PUN can be `PhotonRealtimeAPI.dll`
// or `Assembly-CSharp.dll` depending on Unity build settings).
Il2CppClass* IL2CPP_FindClass(const char* imageName,
                              const char* namespaceName,
                              const char* className)
{
    if (!g_bReady) return nullptr;
    Il2CppDomain* dom = g_domain_get();
    if (!dom) return nullptr;

    size_t count = 0;
    const Il2CppAssembly** asms = g_domain_get_assemblies(dom, &count);
    if (!asms || count == 0) return nullptr;

    // First pass: prefer the named image if one matches. Skip
    // entirely if caller passed nullptr.
    if (imageName)
    {
        for (size_t i = 0; i < count; ++i)
        {
            const Il2CppImage* img = g_assembly_get_image(asms[i]);
            if (!img) continue;
            const char* name = g_image_get_name(img);
            if (!name) continue;

            bool match = strcmp(name, imageName) == 0;
            if (!match)
            {
                size_t inl = strlen(imageName);
                size_t nl = strlen(name);
                if (nl == inl + 4 &&
                    strncmp(name, imageName, inl) == 0 &&
                    strcmp(name + inl, ".dll") == 0)
                {
                    match = true;
                }
            }
            if (!match) continue;

            Il2CppClass* k = g_class_from_name(img, namespaceName, className);
            if (k) return k;
        }
    }

    // Fallback: scan every loaded image. Returns the first hit.
    for (size_t i = 0; i < count; ++i)
    {
        const Il2CppImage* img = g_assembly_get_image(asms[i]);
        if (!img) continue;
        Il2CppClass* k = g_class_from_name(img, namespaceName, className);
        if (k)
        {
            const char* name = g_image_get_name(img);
            IL2CPP_Log("[il2cpp] found %s.%s in image '%s' (caller asked '%s')",
                       namespaceName ? namespaceName : "", className,
                       name ? name : "?", imageName ? imageName : "(any)");
            return k;
        }
    }
    return nullptr;
}

const MethodInfo* IL2CPP_FindMethod(Il2CppClass* klass,
                                    const char* methodName,
                                    int argCount)
{
    if (!g_bReady || !klass) return nullptr;
    return g_class_get_method_from_name(klass, methodName, argCount);
}

void* IL2CPP_FindMethodPtr(const char* imageName,
                           const char* namespaceName,
                           const char* className,
                           const char* methodName,
                           int argCount)
{
    Il2CppClass* k = IL2CPP_FindClass(imageName, namespaceName, className);
    if (!k)
    {
        IL2CPP_Log("[il2cpp] class not found: %s::%s.%s",
                   imageName, namespaceName ? namespaceName : "", className);
        return nullptr;
    }
    const MethodInfo* m = IL2CPP_FindMethod(k, methodName, argCount);
    if (!m)
    {
        IL2CPP_Log("[il2cpp] method not found: %s::%s.%s.%s(argc=%d)",
                   imageName, namespaceName ? namespaceName : "", className,
                   methodName, argCount);
        return nullptr;
    }
    return m->methodPointer;
}

void* IL2CPP_StringNew(const char* utf8)
{
    if (!g_bReady || !g_string_new || !utf8) return nullptr;
    return g_string_new(utf8);
}

void IL2CPP_DumpClassMethods(Il2CppClass* klass, const char* classNameForLog)
{
    if (!g_bReady || !klass) return;
    if (!g_class_get_methods || !g_method_get_name)
    {
        IL2CPP_Log("[il2cpp][dump %s] enumeration unavailable",
                   classNameForLog ? classNameForLog : "?");
        return;
    }
    int count = 0;
    void* iter = nullptr;
    while (const MethodInfo* m = g_class_get_methods(klass, &iter))
    {
        const char* name = g_method_get_name(m);
        uint32_t pc = g_method_get_param_count ? g_method_get_param_count(m) : 0xFFFFFFFFu;
        if (name)
            IL2CPP_Log("[il2cpp][dump %s] %s argc=%u",
                       classNameForLog ? classNameForLog : "?", name, pc);
        if (++count > 256) break;
    }
}

// ------------------------------------------------------------
// Dictionary helpers (used by SendOperation hook to rewrite
// AppId / authType in PUN's wire params dict).
// ------------------------------------------------------------

static Il2CppClass* GetByteClass()
{
    if (g_byteClass) return g_byteClass;
    g_byteClass = IL2CPP_FindClass("mscorlib", "System", "Byte");
    return g_byteClass;
}

bool IL2CPP_DictByteByteSetItem(Il2CppObject* dict, uint8_t key, uint8_t value)
{
    if (!g_bReady || !dict) return false;
    if (!g_object_get_class || !g_value_box || !g_runtime_invoke) return false;

    Il2CppClass* byteClass = GetByteClass();
    if (!byteClass) return false;

    Il2CppClass* dictClass = g_object_get_class(dict);
    if (!dictClass) return false;
    const MethodInfo* setItem = g_class_get_method_from_name(dictClass, "set_Item", 2);
    if (!setItem) return false;

    uint8_t keyByte   = key;
    uint8_t valueByte = value;
    Il2CppObject* boxedVal = g_value_box(byteClass, &valueByte);
    if (!boxedVal) return false;

    void* args[2] = { &keyByte, boxedVal };
    Il2CppObject* exc = nullptr;
    g_runtime_invoke(setItem, dict, args, &exc);
    return (exc == nullptr);
}

bool IL2CPP_DictByteStringSetItem(Il2CppObject* dict, uint8_t key, const char* utf8Value)
{
    if (!g_bReady || !dict || !utf8Value) return false;
    if (!g_object_get_class || !g_runtime_invoke || !g_string_new) return false;

    Il2CppClass* dictClass = g_object_get_class(dict);
    if (!dictClass) return false;
    const MethodInfo* setItem = g_class_get_method_from_name(dictClass, "set_Item", 2);
    if (!setItem) return false;

    void* mStr = g_string_new(utf8Value);
    if (!mStr) return false;

    uint8_t keyByte = key;
    void* args[2] = { &keyByte, mStr };
    Il2CppObject* exc = nullptr;
    g_runtime_invoke(setItem, dict, args, &exc);
    return (exc == nullptr);
}

Il2CppObject* IL2CPP_DictByteGetItem(Il2CppObject* dict, uint8_t key)
{
    if (!g_bReady || !dict) return nullptr;
    if (!g_object_get_class || !g_runtime_invoke) return nullptr;

    Il2CppClass* dictClass = g_object_get_class(dict);
    if (!dictClass) return nullptr;
    const MethodInfo* containsKey = g_class_get_method_from_name(dictClass, "ContainsKey", 1);
    const MethodInfo* getItem     = g_class_get_method_from_name(dictClass, "get_Item",    1);
    if (!getItem) return nullptr;

    uint8_t keyByte = key;
    void* args[1] = { &keyByte };
    Il2CppObject* exc = nullptr;

    if (containsKey)
    {
        Il2CppObject* boxedBool = g_runtime_invoke(containsKey, dict, args, &exc);
        if (exc || !boxedBool) return nullptr;
        if (*((uint8_t*)boxedBool + 0x10) == 0) return nullptr;
    }
    Il2CppObject* result = g_runtime_invoke(getItem, dict, args, &exc);
    if (exc) return nullptr;
    return result;
}

bool IL2CPP_DescribeObject(Il2CppObject* obj, char* out, size_t outSize)
{
    if (!out || outSize == 0) return false;
    out[0] = 0;
    if (!obj) { _snprintf_s(out, outSize, _TRUNCATE, "(null)"); return true; }

    Il2CppClass* k = g_object_get_class ? g_object_get_class(obj) : nullptr;
    const char* clsName = (k && g_class_get_name) ? g_class_get_name(k) : "?";

    if (k && g_string_to_utf8 && clsName && strcmp(clsName, "String") == 0)
    {
        char* utf8 = g_string_to_utf8(obj);
        if (utf8)
        {
            _snprintf_s(out, outSize, _TRUNCATE, "String=\"%s\"", utf8);
            if (g_free) g_free(utf8);
            return true;
        }
    }

    uint8_t* payload = (uint8_t*)obj + 0x10;
    _snprintf_s(out, outSize, _TRUNCATE,
        "%s {%02X %02X %02X %02X %02X %02X %02X %02X}",
        clsName ? clsName : "?",
        payload[0], payload[1], payload[2], payload[3],
        payload[4], payload[5], payload[6], payload[7]);
    return true;
}
