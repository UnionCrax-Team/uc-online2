using System.Text;

namespace UCO2.Patcher.Core;

public static class PeInspector
{
    private static readonly byte[][] SteamStubSignatures =
    [
        [0xe8, 0, 0, 0, 0, 0x50, 0x53, 0x51, 0x52, 0x56, 0x57, 0x55, 0x41, 0x50],
        [0xe8, 0, 0, 0, 0, 0x50, 0x53, 0x51, 0x52, 0x56, 0x57, 0x55, 0x8b, 0x44, 0x24, 0x1c, 0x2d, 5, 0, 0, 0, 0x8b, 0xcc, 0x83, 0xe4, 0xf0, 0x51, 0x51, 0x51, 0x50],
        [0x53, 0x51, 0x52, 0x56, 0x57, 0x55, 0x8b, 0xec, 0x81, 0xec, 0, 0x10, 0, 0],
        [0x60, 0x81, 0xec, 0, 0x10, 0, 0, 0xbe]
    ];

    public static SteamStubStatus DetectSteamStub(string executablePath)
    {
        try
        {
            using FileStream stream = File.Open(executablePath, FileMode.Open, FileAccess.Read, FileShare.ReadWrite);
            using BinaryReader reader = new(stream, Encoding.ASCII, leaveOpen: true);
            if (stream.Length < 256 || reader.ReadUInt16() != 0x5A4D)
                return SteamStubStatus.Unreadable;

            stream.Position = 0x3c;
            int peOffset = reader.ReadInt32();
            if (peOffset < 0 || peOffset + 24 >= stream.Length)
                return SteamStubStatus.Unreadable;

            stream.Position = peOffset;
            if (reader.ReadUInt32() != 0x00004550)
                return SteamStubStatus.Unreadable;

            stream.Position = peOffset + 6;
            ushort sectionCount = reader.ReadUInt16();
            stream.Position = peOffset + 20;
            ushort optionalSize = reader.ReadUInt16();
            long sectionTable = peOffset + 24L + optionalSize;

            for (int index = 0; index < sectionCount; index++)
            {
                long offset = sectionTable + index * 40L;
                if (offset + 40 > stream.Length)
                    break;

                stream.Position = offset;
                string name = Encoding.ASCII.GetString(reader.ReadBytes(8)).TrimEnd('\0');
                stream.Position = offset + 16;
                uint rawSize = reader.ReadUInt32();
                uint rawOffset = reader.ReadUInt32();
                if (!name.Equals(".bind", StringComparison.Ordinal))
                    continue;
                if (rawOffset >= stream.Length)
                    return SteamStubStatus.Unreadable;

                int count = checked((int)Math.Min(rawSize, stream.Length - rawOffset));
                stream.Position = rawOffset;
                byte[] section = reader.ReadBytes(count);
                return SteamStubSignatures.Any(signature => Contains(section, signature))
                    ? SteamStubStatus.Detected
                    : SteamStubStatus.UnrecognizedBindSection;
            }

            return SteamStubStatus.NotDetected;
        }
        catch (Exception ex) when (ex is IOException or UnauthorizedAccessException or OverflowException)
        {
            return SteamStubStatus.Unreadable;
        }
    }

    private static bool Contains(ReadOnlySpan<byte> haystack, ReadOnlySpan<byte> needle)
    {
        for (int i = 0; i <= haystack.Length - needle.Length; i++)
        {
            if (haystack.Slice(i, needle.Length).SequenceEqual(needle))
                return true;
        }
        return false;
    }
}
