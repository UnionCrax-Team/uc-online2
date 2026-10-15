using System;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Security.Cryptography;
using System.Threading.Tasks;
using UnityEditor;
using UnityEngine;

// Unity entry point for the PowerShell schema pipeline.
// The documented Coherence types are resolved by exact full name so this file
// does not require a version-specific assembly reference in the project.
public static class Uco2CoherenceSchemaPipeline
{
    private const string Marker = "UCO2_SCHEMA_PIPELINE:";

    public static void Run()
    {
        try
        {
            Log("START");
            var schemaPath = RequiredEnvironment("UCO2_COHERENCE_SCHEMA_PATH");
            var projectId = RequiredEnvironment("UCO2_COHERENCE_PROJECT_ID");
            var projectToken = RequiredEnvironment("UCO2_COHERENCE_PROJECT_TOKEN");
            // Optional on purpose. Only the SDK 1.x path needs it -- the 2.x
            // API is Upload(projectId, projectToken, mode) and never reads an
            // organization. Demanding it up front makes the caller go and find
            // a value that will not be used. ConfigureLegacyRuntimeSettings
            // checks for it when, and only when, that path is taken.
            var organizationId = Environment.GetEnvironmentVariable("UCO2_COHERENCE_ORGANIZATION_ID");
            var projectName = RequiredEnvironment("UCO2_COHERENCE_PROJECT_NAME");
            var expectedSha1 = RequiredEnvironment("UCO2_COHERENCE_EXPECTED_SHA1");
            var bake = Environment.GetEnvironmentVariable("UCO2_COHERENCE_BAKE") == "1";

            if (!File.Exists(schemaPath))
                throw new FileNotFoundException("Gathered.schema was not found", schemaPath);

            Log("SCHEMA path=" + schemaPath);
            var initialSha1 = Sha1(File.ReadAllBytes(schemaPath));
            Log("SCHEMA sha1=" + initialSha1);
            if (!string.Equals(initialSha1, expectedSha1, StringComparison.OrdinalIgnoreCase))
                throw new InvalidDataException("Gathered.schema changed after PowerShell validated it.");

            if (bake)
            {
                Log("BAKE begin");
                var bakeMethodType = FindType("Coherence.Editor.BakeUtil");
                var bakeMethod = FindMethod(bakeMethodType, "Bake", Array.Empty<object>());
                if (bakeMethod == null) throw new MissingMethodException(bakeMethodType.FullName, "Bake()");
                EnsureSuccess(InvokeAndWait(bakeMethod, Array.Empty<object>()), bakeMethod);
                AssetDatabase.Refresh(ImportAssetOptions.ForceUpdate);
                Log("BAKE complete");
            }
            else
            {
                Log("BAKE skipped (using supplied Gathered.schema)");
            }

            var actualSha1 = Sha1(File.ReadAllBytes(schemaPath));
            if (!string.Equals(actualSha1, expectedSha1, StringComparison.OrdinalIgnoreCase))
                throw new InvalidDataException("Gathered.schema changed inside Unity; its SHA-1 no longer matches the validated input.");

            Log("UPLOAD begin project=" + projectId);
            UploadSchema(projectId, projectToken, organizationId, projectName);
            Log("UPLOAD complete");
            Log("SUCCESS");
            EditorApplication.Exit(0);
        }
        catch (Exception ex)
        {
            var error = ex is TargetInvocationException tie && tie.InnerException != null ? tie.InnerException : ex;
            Log("FAILURE " + error);
            Debug.LogError("[UCO2] Coherence schema pipeline failed: " + error);
            EditorApplication.Exit(1);
        }
    }

    private static void UploadSchema(string projectId, string projectToken, string organizationId, string projectName)
    {
        var schemasType = FindType("Coherence.Editor.Portal.Schemas");

        // SDK 2.x public CI API. Declared as
        //   Upload(string projectId, string projectToken, InteractionMode mode = AutomatedAction)
        // so it has THREE parameters, not two. Matching on an exact parameter
        // count misses it, silently falls through to the SDK 1.x branch below,
        // and calls Upload() -- which uploads to whatever project the editor
        // has active and ignores the id and token supplied here. Match on the
        // leading parameters instead and fill the optional tail with its own
        // declared defaults.
        object[] directArgs;
        var directUpload = FindMethodWithOptionalTail(
            schemasType, "Upload", new object[] { projectId, projectToken }, out directArgs);
        if (directUpload != null)
        {
            Log("UPLOAD API=Schemas." + Describe(directUpload));
            EnsureSuccess(InvokeAndWait(directUpload, directArgs), directUpload);
            return;
        }

        // SDK 1.6 API. PortalToken is read from COHERENCE_PORTAL_TOKEN by
        // ProjectSettings; project identity is supplied through RuntimeSettings.
        ConfigureLegacyRuntimeSettings(projectId, organizationId, projectName);
        var uploadActive = FindMethod(schemasType, "Upload", Array.Empty<object>());
        if (uploadActive == null)
            throw new MissingMethodException(schemasType.FullName, "Upload(string,string) or Upload()");

        Log("UPLOAD API=Schemas.Upload() [SDK 1.x]");
        EnsureSuccess(InvokeAndWait(uploadActive, Array.Empty<object>()), uploadActive);
    }

    private static void ConfigureLegacyRuntimeSettings(string projectId, string organizationId, string projectName)
    {
        if (string.IsNullOrWhiteSpace(Environment.GetEnvironmentVariable("COHERENCE_PORTAL_TOKEN")))
            throw new InvalidOperationException("COHERENCE_PORTAL_TOKEN is required by the SDK 1.x upload API.");

        if (string.IsNullOrWhiteSpace(organizationId))
            throw new InvalidOperationException(
                "This SDK only exposes the 1.x upload API, which needs an organization ID. " +
                "Re-run with -OrganizationId (dashboard > organization settings).");

        var runtimeType = FindType("Coherence.RuntimeSettings");
        var instanceProperty = FindPropertyInHierarchy(runtimeType, "InstanceUnsafe", BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Static);
        var runtimeSettings = instanceProperty != null ? instanceProperty.GetValue(null) : null;
        if (runtimeSettings == null)
        {
            runtimeSettings = ScriptableObject.CreateInstance(runtimeType);
            Log("UPLOAD created an in-memory RuntimeSettings instance");
        }

        SetMember(runtimeType, runtimeSettings, "OrganizationID", "organizationID", organizationId);
        SetMember(runtimeType, runtimeSettings, "ProjectID", "projectID", projectId);
        SetMember(runtimeType, runtimeSettings, "ProjectName", "projectName", projectName);
    }

    private static void SetMember(Type type, object instance, string propertyName, string fieldName, string value)
    {
        var property = type.GetProperty(propertyName, BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance);
        var setter = property != null ? property.GetSetMethod(true) : null;
        if (setter != null)
        {
            setter.Invoke(instance, new object[] { value });
            return;
        }

        var field = type.GetField(fieldName, BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance);
        if (field == null) throw new MissingFieldException(type.FullName, fieldName);
        field.SetValue(instance, value);
    }

    private static PropertyInfo FindPropertyInHierarchy(Type type, string name, BindingFlags flags)
    {
        for (var current = type; current != null; current = current.BaseType)
        {
            var property = current.GetProperty(name, flags | BindingFlags.DeclaredOnly);
            if (property != null) return property;
        }
        return null;
    }

    private static Type FindType(string typeName)
    {
        var type = AppDomain.CurrentDomain.GetAssemblies()
            .Select(a => a.GetType(typeName, false))
            .FirstOrDefault(t => t != null);
        if (type == null) throw new TypeLoadException("Required Coherence editor type was not loaded: " + typeName);
        return type;
    }

    // Match a method by its LEADING parameters, allowing a tail of optional
    // ones, and return the full argument array with each optional filled from
    // its own declared default. Reflection will not apply defaults for you --
    // Invoke requires a value per parameter.
    private static MethodInfo FindMethodWithOptionalTail(Type type, string methodName,
                                                         object[] leading, out object[] fullArguments)
    {
        foreach (var m in type.GetMethods(BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Static))
        {
            if (m.Name != methodName) continue;

            var ps = m.GetParameters();
            if (ps.Length < leading.Length) continue;
            if (!ParametersMatch(ps.Take(leading.Length).ToArray(), leading)) continue;
            if (!ps.Skip(leading.Length).All(p => p.IsOptional)) continue;

            var args = new object[ps.Length];
            Array.Copy(leading, args, leading.Length);
            for (var i = leading.Length; i < ps.Length; i++) args[i] = ps[i].DefaultValue;

            fullArguments = args;
            return m;
        }

        fullArguments = null;
        return null;
    }

    private static string Describe(MethodInfo m)
    {
        return m.Name + "(" + string.Join(", ", m.GetParameters().Select(p => p.ParameterType.Name)) + ")";
    }

    private static MethodInfo FindMethod(Type type, string methodName, object[] arguments)
    {
        return type.GetMethods(BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Static)
            .Where(m => m.Name == methodName)
            .Where(m => m.GetParameters().Length == arguments.Length)
            .FirstOrDefault(m => ParametersMatch(m.GetParameters(), arguments));
    }

    private static object InvokeAndWait(MethodInfo method, object[] arguments)
    {
        var result = method.Invoke(null, arguments);
        if (result is Task task)
        {
            task.GetAwaiter().GetResult();
            var resultProperty = task.GetType().GetProperty("Result", BindingFlags.Public | BindingFlags.Instance);
            return resultProperty != null ? resultProperty.GetValue(task) : null;
        }
        return result;
    }

    private static void EnsureSuccess(object result, MethodInfo method)
    {
        if (method.ReturnType == typeof(bool) && result is bool succeeded && !succeeded)
            throw new InvalidOperationException(method.DeclaringType.FullName + "." + method.Name + " returned false.");
    }

    private static bool ParametersMatch(ParameterInfo[] parameters, object[] arguments)
    {
        for (var i = 0; i < parameters.Length; i++)
        {
            if (arguments[i] == null) continue;
            if (!parameters[i].ParameterType.IsAssignableFrom(arguments[i].GetType())) return false;
        }
        return true;
    }

    private static string RequiredEnvironment(string name)
    {
        var value = Environment.GetEnvironmentVariable(name);
        if (string.IsNullOrWhiteSpace(value)) throw new InvalidOperationException("Missing environment variable " + name);
        return value;
    }

    private static string Sha1(byte[] data)
    {
        using (var sha1 = SHA1.Create())
            return string.Concat(sha1.ComputeHash(data).Select(b => b.ToString("x2")));
    }

    private static void Log(string message)
    {
        var line = Marker + message;
        Console.WriteLine(line);
        Debug.Log(line);
    }
}
