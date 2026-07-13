#include "mono_runtime.h"
#include <string.h>
#include <stdio.h>

extern "C" void MONO_Log(const char* fmt, ...);

// ============================================================
// Function-pointer signatures for the Mono APIs we need
// ============================================================
typedef MonoDomain*    (*Fn_mono_get_root_domain)();
typedef MonoDomain*    (*Fn_mono_thread_attach)(MonoDomain*);
typedef MonoAssembly*  (*Fn_mono_domain_assembly_open)(MonoDomain*, const char*);
typedef MonoImage*     (*Fn_mono_assembly_get_image)(MonoAssembly*);
typedef const char*    (*Fn_mono_image_get_name)(MonoImage*);
typedef MonoClass*     (*Fn_mono_class_from_name)(MonoImage*, const char*, const char*);
typedef MonoMethod*    (*Fn_mono_class_get_method_from_name)(MonoClass*, const char*, int);
typedef void*          (*Fn_mono_compile_method)(MonoMethod*);
typedef MonoString*    (*Fn_mono_string_new)(MonoDomain*, const char*);
typedef void*          (*Fn_g_list_first)(void*);
typedef void*          (*Fn_mono_assembly_foreach)(void (*)(MonoAssembly*, void*), void*);
typedef MonoClass*     (*Fn_mono_object_get_class)(MonoObject*);
typedef MonoObject*    (*Fn_mono_value_box)(MonoDomain*, MonoClass*, void*);
typedef MonoObject*    (*Fn_mono_runtime_invoke)(MonoMethod*, void*, void**, MonoObject**);
typedef MonoClass*     (*Fn_mono_get_byte_class)();
typedef char*          (*Fn_mono_string_to_utf8)(MonoString*);
typedef void           (*Fn_mono_free)(void*);
typedef const char*    (*Fn_mono_class_get_name)(MonoClass*);
typedef void*          (*Fn_mono_class_get_field_from_name)(MonoClass*, const char*);
typedef void*          (*Fn_mono_class_vtable)(MonoDomain*, MonoClass*);
typedef void           (*Fn_mono_field_static_set_value)(void* /*MonoVTable*/, void* /*MonoClassField*/, void*);
typedef uint32_t       (*Fn_mono_field_get_offset)(void* /*MonoClassField*/);

// Internal helper: walk every assembly in a domain. Mono exposes
// `mono_assembly_foreach(callback, userData)` for this.

static HMODULE g_hMono = nullptr;
static bool    g_bReady = false;

static Fn_mono_get_root_domain            g_get_root_domain            = nullptr;
static Fn_mono_thread_attach              g_thread_attach              = nullptr;
static Fn_mono_domain_assembly_open       g_domain_assembly_open       = nullptr;
static Fn_mono_assembly_get_image         g_assembly_get_image         = nullptr;
static Fn_mono_image_get_name             g_image_get_name             = nullptr;
static Fn_mono_class_from_name            g_class_from_name            = nullptr;
static Fn_mono_class_get_method_from_name g_class_get_method_from_name = nullptr;
static Fn_mono_compile_method             g_compile_method             = nullptr;
static Fn_mono_string_new                 g_string_new                 = nullptr;
static Fn_mono_assembly_foreach           g_assembly_foreach           = nullptr;
static Fn_mono_object_get_class           g_object_get_class           = nullptr;
static Fn_mono_value_box                  g_value_box                  = nullptr;
static Fn_mono_runtime_invoke             g_runtime_invoke             = nullptr;
static Fn_mono_get_byte_class             g_get_byte_class             = nullptr;
static Fn_mono_string_to_utf8             g_string_to_utf8             = nullptr;
static Fn_mono_free                       g_free                       = nullptr;
static Fn_mono_class_get_name             g_class_get_name             = nullptr;
static Fn_mono_class_get_field_from_name  g_class_get_field_from_name  = nullptr;
static Fn_mono_class_vtable               g_class_vtable               = nullptr;
static Fn_mono_field_static_set_value     g_field_static_set_value     = nullptr;
static Fn_mono_field_get_offset           g_field_get_offset           = nullptr;

#define RESOLVE(name) \
    do { \
        g_##name = (Fn_mono_##name)GetProcAddress(g_hMono, "mono_" #name); \
        if (!g_##name) { MONO_Log("[mono] missing export: mono_" #name); return false; } \
    } while (0)

bool MONO_TryInit(void)
{
    if (g_bReady) return true;

    // Unity Mono uses mono-2.0-bdwgc.dll. Some legacy builds use
    // mono.dll. Try both.
    static const char* kCandidates[] = { "mono-2.0-bdwgc.dll", "mono.dll", nullptr };
    for (int i = 0; kCandidates[i] && !g_hMono; ++i)
    {
        g_hMono = GetModuleHandleA(kCandidates[i]);
    }
    if (!g_hMono) return false;

    RESOLVE(get_root_domain);
    RESOLVE(thread_attach);
    RESOLVE(domain_assembly_open);
    RESOLVE(assembly_get_image);
    RESOLVE(image_get_name);
    RESOLVE(class_from_name);
    RESOLVE(class_get_method_from_name);
    RESOLVE(compile_method);
    RESOLVE(string_new);
    RESOLVE(assembly_foreach);
    RESOLVE(object_get_class);
    RESOLVE(value_box);
    RESOLVE(runtime_invoke);
    // mono_get_byte_class is optional; some Mono builds don't export it.
    g_get_byte_class = (Fn_mono_get_byte_class)GetProcAddress(g_hMono, "mono_get_byte_class");
    g_string_to_utf8 = (Fn_mono_string_to_utf8)GetProcAddress(g_hMono, "mono_string_to_utf8");
    g_free           = (Fn_mono_free)          GetProcAddress(g_hMono, "mono_free");
    g_class_get_name = (Fn_mono_class_get_name)GetProcAddress(g_hMono, "mono_class_get_name");
    g_class_get_field_from_name = (Fn_mono_class_get_field_from_name)GetProcAddress(g_hMono, "mono_class_get_field_from_name");
    g_class_vtable              = (Fn_mono_class_vtable)             GetProcAddress(g_hMono, "mono_class_vtable");
    g_field_static_set_value    = (Fn_mono_field_static_set_value)   GetProcAddress(g_hMono, "mono_field_static_set_value");
    g_field_get_offset          = (Fn_mono_field_get_offset)         GetProcAddress(g_hMono, "mono_field_get_offset");

    // Attach the caller's thread so subsequent managed calls
    // don't crash on Mono's thread-local-state assertions.
    MonoDomain* dom = g_get_root_domain();
    if (dom) g_thread_attach(dom);

    g_bReady = true;
    MONO_Log("[mono] runtime resolved, domain=%p", dom);
    return true;
}

bool MONO_IsReady(void) { return g_bReady; }

// Collect-into-array helper used by MONO_FindClass's full scan.
struct FindClassState
{
    const char* ns;
    const char* name;
    MonoClass*  result;
    MonoImage*  resultImg;
};

static void FindClassCallback(MonoAssembly* asm_, void* user)
{
    FindClassState* st = (FindClassState*)user;
    if (st->result) return;
    MonoImage* img = g_assembly_get_image(asm_);
    if (!img) return;
    MonoClass* k = g_class_from_name(img, st->ns, st->name);
    if (k) { st->result = k; st->resultImg = img; }
}

struct NamedClassState
{
    const char* ns;
    const char* name;
    const char* imageName;
    MonoClass*  result;
};

static void NamedClassCallback(MonoAssembly* asm_, void* user)
{
    NamedClassState* st = (NamedClassState*)user;
    if (st->result) return;
    MonoImage* img = g_assembly_get_image(asm_);
    if (!img) return;
    const char* imgName = g_image_get_name(img);
    if (!imgName) return;

    // Match imageName both with and without ".dll" suffix.
    size_t inl = strlen(st->imageName);
    size_t nl  = strlen(imgName);
    bool match = (strcmp(imgName, st->imageName) == 0) ||
                 (nl == inl + 4 && strncmp(imgName, st->imageName, inl) == 0 &&
                  strcmp(imgName + inl, ".dll") == 0);
    if (!match) return;

    MonoClass* k = g_class_from_name(img, st->ns, st->name);
    if (k) st->result = k;
}

MonoClass* MONO_FindClass(const char* imageName,
                          const char* namespaceName,
                          const char* className)
{
    if (!g_bReady) return nullptr;

    if (imageName)
    {
        NamedClassState st = { namespaceName, className, imageName, nullptr };
        g_assembly_foreach(&NamedClassCallback, &st);
        return st.result;  // may be null (caller asked for specific image)
    }

    FindClassState st = { namespaceName, className, nullptr, nullptr };
    g_assembly_foreach(&FindClassCallback, &st);
    if (st.result && st.resultImg)
    {
        const char* name = g_image_get_name(st.resultImg);
        MONO_Log("[mono] found %s.%s in image '%s' (caller asked '(any)')",
                 namespaceName ? namespaceName : "", className,
                 name ? name : "?");
    }
    return st.result;
}

MonoMethod* MONO_FindMethod(MonoClass* klass, const char* methodName, int argCount)
{
    if (!g_bReady || !klass) return nullptr;
    return g_class_get_method_from_name(klass, methodName, argCount);
}

void* MONO_GetMethodNativePtr(MonoMethod* method)
{
    if (!g_bReady || !method) return nullptr;
    return g_compile_method(method);
}

void* MONO_FindMethodPtr(const char* imageName,
                        const char* namespaceName,
                        const char* className,
                        const char* methodName,
                        int argCount)
{
    MonoClass* k = MONO_FindClass(imageName, namespaceName, className);
    if (!k)
    {
        MONO_Log("[mono] class not found: %s::%s.%s",
                 imageName ? imageName : "(any)",
                 namespaceName ? namespaceName : "", className);
        return nullptr;
    }
    MonoMethod* m = MONO_FindMethod(k, methodName, argCount);
    if (!m)
    {
        MONO_Log("[mono] method not found: %s::%s.%s.%s(argc=%d)",
                 imageName ? imageName : "(any)",
                 namespaceName ? namespaceName : "", className,
                 methodName, argCount);
        return nullptr;
    }
    return MONO_GetMethodNativePtr(m);
}

bool MONO_InvokeInstanceVoidMethodNoArgs(MonoClass* klass, const char* methodName, void* pThis)
{
    if (!g_bReady || !klass || !methodName || !pThis) return false;
    if (!g_class_get_method_from_name || !g_runtime_invoke) return false;
    MonoMethod* m = g_class_get_method_from_name(klass, methodName, 0);
    if (!m) {
        MONO_Log("[mono] InvokeInstanceVoidMethodNoArgs: %s not found", methodName);
        return false;
    }
    MonoObject* exc = nullptr;
    g_runtime_invoke(m, pThis, nullptr, &exc);
    if (exc) {
        MONO_Log("[mono] InvokeInstanceVoidMethodNoArgs: %s threw exception", methodName);
        return false;
    }
    return true;
}

int MONO_GetFieldOffset(MonoClass* klass, const char* fieldName)
{
    if (!g_bReady || !klass || !fieldName) return -1;
    if (!g_class_get_field_from_name || !g_field_get_offset) return -1;
    void* field = g_class_get_field_from_name(klass, fieldName);
    if (!field) return -1;
    return (int)g_field_get_offset(field);
}

bool MONO_SetStaticFieldByName(MonoClass* klass, const char* fieldName, void* valuePtr)
{
    if (!g_bReady || !klass || !fieldName || !valuePtr) return false;
    if (!g_class_get_field_from_name || !g_class_vtable || !g_field_static_set_value) {
        MONO_Log("[mono] SetStaticFieldByName: required exports missing");
        return false;
    }
    void* field = g_class_get_field_from_name(klass, fieldName);
    if (!field) {
        MONO_Log("[mono] SetStaticFieldByName: field '%s' not found", fieldName);
        return false;
    }
    MonoDomain* dom = g_get_root_domain();
    if (!dom) return false;
    void* vtable = g_class_vtable(dom, klass);
    if (!vtable) return false;
    g_field_static_set_value(vtable, field, valuePtr);
    return true;
}

MonoString* MONO_StringNew(const char* utf8)
{
    if (!g_bReady || !g_string_new || !utf8) return nullptr;
    MonoDomain* dom = g_get_root_domain();
    if (!dom) return nullptr;
    return g_string_new(dom, utf8);
}

bool MONO_DictByteByteSetItem(MonoObject* dict, uint8_t key, uint8_t value)
{
    if (!g_bReady || !dict) return false;
    if (!g_object_get_class || !g_value_box || !g_runtime_invoke) return false;

    // Resolve System.Byte class (used to box the value into `object`).
    static MonoClass* byteClass = nullptr;
    if (!byteClass)
    {
        if (g_get_byte_class) byteClass = g_get_byte_class();
        if (!byteClass)
        {
            // Fallback: look up via mscorlib
            byteClass = MONO_FindClass("mscorlib", "System", "Byte");
        }
        if (!byteClass)
        {
            MONO_Log("[mono] could not resolve System.Byte class");
            return false;
        }
    }

    // Get the dict's instantiated class (e.g. Dictionary<byte, object>)
    // and look up its set_Item(TKey, TValue) method on that exact type.
    MonoClass* dictClass = g_object_get_class(dict);
    if (!dictClass) { MONO_Log("[mono] dict has no class"); return false; }

    MonoMethod* setItem = g_class_get_method_from_name(dictClass, "set_Item", 2);
    if (!setItem)
    {
        MONO_Log("[mono] Dictionary.set_Item not found on dict class");
        return false;
    }

    MonoDomain* dom = g_get_root_domain();
    if (!dom) return false;

    // For value-type args to mono_runtime_invoke, pass a pointer to
    // the raw bytes. For reference-type args (TValue=object here),
    // pass the boxed object.
    uint8_t keyByte   = key;
    uint8_t valueByte = value;
    MonoObject* boxedVal = g_value_box(dom, byteClass, &valueByte);
    if (!boxedVal) { MONO_Log("[mono] failed to box byte value"); return false; }

    void* args[2] = { &keyByte, boxedVal };
    MonoObject* exc = nullptr;
    g_runtime_invoke(setItem, dict, args, &exc);
    if (exc)
    {
        MONO_Log("[mono] Dictionary.set_Item threw an exception");
        return false;
    }
    return true;
}

bool MONO_DictByteStringSetItem(MonoObject* dict, uint8_t key, const char* utf8Value)
{
    if (!g_bReady || !dict || !utf8Value) return false;
    if (!g_object_get_class || !g_runtime_invoke || !g_string_new) return false;

    MonoClass* dictClass = g_object_get_class(dict);
    if (!dictClass) return false;
    MonoMethod* setItem = g_class_get_method_from_name(dictClass, "set_Item", 2);
    if (!setItem) return false;

    MonoDomain* dom = g_get_root_domain();
    if (!dom) return false;
    MonoString* mStr = g_string_new(dom, utf8Value);
    if (!mStr) return false;

    uint8_t keyByte = key;
    // For reference-type value args, pass the object directly (not a pointer).
    void* args[2] = { &keyByte, mStr };
    MonoObject* exc = nullptr;
    g_runtime_invoke(setItem, dict, args, &exc);
    return (exc == nullptr);
}

MonoObject* MONO_DictByteGetItem(MonoObject* dict, uint8_t key)
{
    if (!g_bReady || !dict) return nullptr;
    if (!g_object_get_class || !g_runtime_invoke) return nullptr;

    MonoClass* dictClass = g_object_get_class(dict);
    if (!dictClass) return nullptr;
    // get_Item throws KeyNotFound if missing; use ContainsKey first.
    MonoMethod* containsKey = g_class_get_method_from_name(dictClass, "ContainsKey", 1);
    MonoMethod* getItem     = g_class_get_method_from_name(dictClass, "get_Item",    1);
    if (!getItem) return nullptr;

    uint8_t keyByte = key;
    void* args[1] = { &keyByte };
    MonoObject* exc = nullptr;

    if (containsKey)
    {
        MonoObject* boxedBool = g_runtime_invoke(containsKey, dict, args, &exc);
        if (exc || !boxedBool) return nullptr;
        // boxed bool value sits at offset 0x10 from MonoObject*
        if (*((uint8_t*)boxedBool + 0x10) == 0) return nullptr;
    }

    MonoObject* result = g_runtime_invoke(getItem, dict, args, &exc);
    if (exc) return nullptr;
    return result;
}

bool MONO_DescribeObject(MonoObject* obj, char* out, size_t outSize)
{
    if (!out || outSize == 0) return false;
    out[0] = 0;
    if (!obj) { _snprintf_s(out, outSize, _TRUNCATE, "(null)"); return true; }

    MonoClass* k = g_object_get_class ? g_object_get_class(obj) : nullptr;
    const char* clsName = (k && g_class_get_name) ? g_class_get_name(k) : "?";

    // Best-effort: if it's a String, convert.
    if (k && g_string_to_utf8 && clsName && strcmp(clsName, "String") == 0)
    {
        char* utf8 = g_string_to_utf8((MonoString*)obj);
        if (utf8)
        {
            _snprintf_s(out, outSize, _TRUNCATE, "String=\"%s\"", utf8);
            if (g_free) g_free(utf8);
            return true;
        }
    }

    // Fallback: type name + first 8 bytes of object payload after the header.
    uint8_t* payload = (uint8_t*)obj + 0x10;
    _snprintf_s(out, outSize, _TRUNCATE,
        "%s {%02X %02X %02X %02X %02X %02X %02X %02X}",
        clsName ? clsName : "?",
        payload[0], payload[1], payload[2], payload[3],
        payload[4], payload[5], payload[6], payload[7]);
    return true;
}
