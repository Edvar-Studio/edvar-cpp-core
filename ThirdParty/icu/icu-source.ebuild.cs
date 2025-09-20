namespace thirdparty.icu;

using ebuild.api;

public class IcuSource : ModuleBase
{
    private static readonly string sourceUrl = "https://github.com/unicode-org/icu/releases/download/release-77-1/icu4c-77_1-src.zip";
    private static readonly string littleEndianDataUrl = "https://github.com/unicode-org/icu/releases/download/release-77-1/icu4c-77_1-data-bin-l.zip";
    private static readonly string bigEndianDataUrl = "https://github.com/unicode-org/icu/releases/download/release-77-1/icu4c-77_1-data-bin-b.zip";
    public IcuSource(ModuleContext context) : base(context)
    {
        this.Type = ModuleType.LibraryLoader;
        this.Name = "icu-source";
        Console.WriteLine("Looking for ICU sources");
        if (!ModuleUtilities.GetAndExtractSourceFromArchiveUrl(sourceUrl, Path.Join(context.ModuleDirectory.FullName, "source"), "D5CF533CF70CD49044D89EDA3E74880328EB9426E6FD2B3CC8F9A963D2AD480E"))
            throw new Exception("Couldn't download ICU sources");
        string dataBinPath;
        if (BitConverter.IsLittleEndian)
        {
            Console.WriteLine("Looking for Little Endian ICU data.");
            dataBinPath = Path.Join(context.ModuleDirectory.FullName, "data", "little");
            if (!ModuleUtilities.GetAndExtractSourceFromArchiveUrl(littleEndianDataUrl, dataBinPath, "0913674FF673C585F8BC08370916B6A6CCC30FFB6408A5C1BC3EDBF5A687FD96"))
                throw new Exception("Couldn't download ICU little endian data.");

        }
        else
        {
            Console.WriteLine("Looking for Big Endian ICU data.");
            dataBinPath = Path.Join(context.ModuleDirectory.FullName, "data", "big");
            if (!ModuleUtilities.GetAndExtractSourceFromArchiveUrl(bigEndianDataUrl, dataBinPath, "D8BE12E03F782DA350508B15354738ED97A3289008A787B6BD2A85434374BFF4"))
                throw new Exception("Couldn't download ICU little endian data.");
        }
        var dataFileName = $"icudt77{(BitConverter.IsLittleEndian ? "l" : "b")}.dat";
        File.Copy(Path.Join(dataBinPath, dataFileName), Path.Join(context.ModuleDirectory.FullName, "source", "icu", "source", "data", "in", dataFileName), true);
        var includeDir = Path.Join(context.ModuleDirectory.FullName, "include", "unicode");
        if (!Directory.Exists(includeDir))
        {
            Directory.CreateDirectory(includeDir);
            var copyFromDir = Path.Join(context.ModuleDirectory.FullName, "source", "icu", "source", "common", "unicode");
            foreach (var file in Directory.GetFiles(copyFromDir, "*.h"))
            {
                var destFile = Path.Join(includeDir, Path.GetFileName(file));
                File.Copy(file, destFile, true);
            }
        }
    }
}