using System.Text;
using System.Text.RegularExpressions;

namespace UCO2.Patcher.Core;

public static partial class CoherenceKeyPatcher
{
    public static byte[] CreatePatchedCopy(string file, string newKey)
    {
        if (!RuntimeKeyRegex().IsMatch(newKey))
            throw new InvalidOperationException("The coherence runtime key must be exactly 32 hexadecimal characters.");

        byte[] bytes = File.ReadAllBytes(file);
        string text = Encoding.Latin1.GetString(bytes);
        foreach (Match schema in SchemaIdRegex().Matches(text))
        {
            string window = text.Substring(schema.Index, Math.Min(400, text.Length - schema.Index));
            if (!window.Contains("localhost", StringComparison.OrdinalIgnoreCase))
                continue;

            foreach (Match candidate in RuntimeKeyRegex().Matches(window))
            {
                if (candidate.Value.Distinct().Count() < 8)
                    continue;
                if (candidate.Value.Equals(newKey, StringComparison.OrdinalIgnoreCase))
                    return bytes;

                byte[] replacement = Encoding.ASCII.GetBytes(newKey);
                Array.Copy(replacement, 0, bytes, schema.Index + candidate.Index, replacement.Length);
                return bytes;
            }
        }

        throw new InvalidOperationException("The coherence runtime key could not be located safely in globalgamemanagers.assets.");
    }

    [GeneratedRegex("\\b[0-9a-fA-F]{40}\\b")]
    private static partial Regex SchemaIdRegex();

    [GeneratedRegex("\\b[0-9a-fA-F]{32}\\b")]
    private static partial Regex RuntimeKeyRegex();
}
