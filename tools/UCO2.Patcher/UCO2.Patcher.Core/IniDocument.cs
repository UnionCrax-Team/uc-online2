namespace UCO2.Patcher.Core;

public sealed class IniDocument
{
    private readonly Dictionary<string, Dictionary<string, string>> sections =
        new(StringComparer.OrdinalIgnoreCase);

    public static IniDocument Load(string path)
    {
        var document = new IniDocument();
        if (!File.Exists(path)) return document;
        string section = "Settings";
        foreach (string raw in File.ReadLines(path))
        {
            string line = raw.Trim();
            if (line.Length == 0 || line.StartsWith(';') || line.StartsWith('#')) continue;
            if (line.StartsWith('[') && line.EndsWith(']'))
            {
                section = line[1..^1].Trim();
                continue;
            }
            int separator = line.IndexOf('=');
            if (separator <= 0) continue;
            document.Set(section, line[..separator].Trim(), line[(separator + 1)..].Trim());
        }
        return document;
    }

    public string Get(string section, string key, string fallback = "")
    {
        return sections.TryGetValue(section, out Dictionary<string, string>? values)
            && values.TryGetValue(key, out string? value) ? value : fallback;
    }

    public bool GetBool(string section, string key, bool fallback = false)
    {
        string value = Get(section, key);
        return value.Length == 0 ? fallback : value.Equals("true", StringComparison.OrdinalIgnoreCase)
            || value.Equals("yes", StringComparison.OrdinalIgnoreCase)
            || value.Equals("on", StringComparison.OrdinalIgnoreCase)
            || value == "1";
    }

    public IReadOnlyDictionary<string, string> GetSection(string section)
    {
        return sections.TryGetValue(section, out Dictionary<string, string>? values)
            ? values
            : new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);
    }

    private void Set(string section, string key, string value)
    {
        if (!sections.TryGetValue(section, out Dictionary<string, string>? values))
        {
            values = new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);
            sections[section] = values;
        }
        values[key] = value;
    }
}
