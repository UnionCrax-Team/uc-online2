using System.Text;

namespace UCO2.Patcher.Core;

public static class BinaryInspector
{
    public static bool ContainsAscii(string path, string value)
    {
        if (!File.Exists(path) || string.IsNullOrEmpty(value))
            return false;

        byte[] needle = Encoding.ASCII.GetBytes(value);
        byte[] buffer = new byte[128 * 1024 + needle.Length];
        int carry = 0;

        using FileStream stream = File.Open(path, FileMode.Open, FileAccess.Read, FileShare.ReadWrite | FileShare.Delete);
        while (true)
        {
            int read = stream.Read(buffer, carry, buffer.Length - carry);
            int count = carry + read;
            if (IndexOf(buffer.AsSpan(0, count), needle) >= 0)
                return true;
            if (read == 0)
                return false;

            carry = Math.Min(needle.Length - 1, count);
            buffer.AsSpan(count - carry, carry).CopyTo(buffer);
        }
    }

    private static int IndexOf(ReadOnlySpan<byte> haystack, ReadOnlySpan<byte> needle)
    {
        for (int i = 0; i <= haystack.Length - needle.Length; i++)
        {
            if (haystack.Slice(i, needle.Length).SequenceEqual(needle))
                return i;
        }
        return -1;
    }
}
