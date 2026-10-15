using System.Security.Cryptography;

namespace UCO2.Patcher.Core;

public static class SafeFileSystem
{
    public static IEnumerable<string> EnumerateFiles(string root, string pattern = "*")
    {
        if (!Directory.Exists(root))
            yield break;

        var pending = new Stack<string>();
        pending.Push(Path.GetFullPath(root));

        while (pending.Count > 0)
        {
            string directory = pending.Pop();
            IEnumerable<string> files = [];
            IEnumerable<string> directories = [];

            try { files = Directory.EnumerateFiles(directory, pattern, SearchOption.TopDirectoryOnly); }
            catch (Exception ex) when (ex is UnauthorizedAccessException or IOException) { }

            foreach (string file in files)
                yield return file;

            try { directories = Directory.EnumerateDirectories(directory, "*", SearchOption.TopDirectoryOnly); }
            catch (Exception ex) when (ex is UnauthorizedAccessException or IOException) { }

            foreach (string child in directories)
            {
                try
                {
                    if ((File.GetAttributes(child) & FileAttributes.ReparsePoint) == 0)
                        pending.Push(child);
                }
                catch (Exception ex) when (ex is UnauthorizedAccessException or IOException) { }
            }
        }
    }

    public static IEnumerable<string> EnumerateDirectories(string root)
    {
        if (!Directory.Exists(root))
            yield break;

        var pending = new Stack<string>();
        pending.Push(Path.GetFullPath(root));

        while (pending.Count > 0)
        {
            string directory = pending.Pop();
            IEnumerable<string> directories = [];
            try { directories = Directory.EnumerateDirectories(directory, "*", SearchOption.TopDirectoryOnly); }
            catch (Exception ex) when (ex is UnauthorizedAccessException or IOException) { }

            foreach (string child in directories)
            {
                yield return child;
                try
                {
                    if ((File.GetAttributes(child) & FileAttributes.ReparsePoint) == 0)
                        pending.Push(child);
                }
                catch (Exception ex) when (ex is UnauthorizedAccessException or IOException) { }
            }
        }
    }

    public static string Sha256(string path)
    {
        using FileStream stream = File.OpenRead(path);
        return Convert.ToHexString(SHA256.HashData(stream));
    }

    public static string Sha256Text(string text)
    {
        return Convert.ToHexString(SHA256.HashData(System.Text.Encoding.UTF8.GetBytes(text)));
    }

    public static bool IsInside(string root, string path)
    {
        string normalizedRoot = Path.TrimEndingDirectorySeparator(Path.GetFullPath(root)) + Path.DirectorySeparatorChar;
        string normalizedPath = Path.GetFullPath(path);
        return normalizedPath.StartsWith(normalizedRoot, StringComparison.OrdinalIgnoreCase);
    }
}
