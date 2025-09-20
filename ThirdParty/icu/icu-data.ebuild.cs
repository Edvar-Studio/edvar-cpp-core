namespace thirdparty.icu;

using System.ComponentModel;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using ebuild.api;


public class IcuData : ModuleBase
{
    public enum DataPackageType
    {
        Common,
        Static,
        Shared
    }

    DataPackageType PackageType = DataPackageType.Common;

    [ModuleOption(ChangesResultBinary = false, Description = "Set maximum number of parallel processes when compiling dependencies. Default is number of processors.")]
    int MaxDependencyCompilationProcesses = Environment.ProcessorCount;
    public IcuData(ModuleContext context) : base(context)
    {
        this.Type = ModuleType.LibraryLoader;
        this.Name = "icudt";
        this.OutputDirectory = "Binaries/icudt";
        this.UseVariants = false;
        Directory.CreateDirectory(GetBinaryOutputDirectory());

        PackageType = context.SelfReference.GetOutput() switch
        {
            "static" => DataPackageType.Static,
            "shared" => DataPackageType.Shared,
            "common" => DataPackageType.Common,
            _ => throw new Exception("Invalid output type for IcuData module. Must be either static, shared or common."),
        };
        var ebuildPath = typeof(ModuleBase).Assembly.Location.Replace("ebuild.api.dll", "ebuild.dll");
        if (!File.Exists(Path.Join(context.ModuleDirectory.FullName, "Binaries", "icupkg", "default", "icupkg" + context.Platform.ExtensionForExecutable)))
        {
            Console.WriteLine("icupkg not found, building icupkg first.");
            var processStartArgs = new System.Diagnostics.ProcessStartInfo
            {
                FileName = "dotnet",
                Arguments = $"\"{ebuildPath}\" build {Path.Join(context.ModuleDirectory.FullName, "icu-icupkg.ebuild.cs")} -p {MaxDependencyCompilationProcesses}",
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                UseShellExecute = false,
                CreateNoWindow = true,
            };
            var process = System.Diagnostics.Process.Start(processStartArgs) ?? throw new Exception("Failed to start ebuild process for icu-icupkg.");
            var output = process.StandardOutput.ReadToEnd();
            Console.WriteLine(output);
            process.WaitForExit();
            if (process.ExitCode != 0)
            {
                var errorOutput = process.StandardError.ReadToEnd();
                throw new Exception($"Ebuild process for icu-icupkg failed with exit code {process.ExitCode}. Error: {errorOutput}");
            }
        }
        else
        {
            Console.WriteLine("icupkg already built, skipping build.");
        }
        // unpack data if not already unpacked
        if (!Directory.Exists(Path.Join(context.ModuleDirectory.FullName, "temp", "pack_contents")))
        {
            Console.WriteLine("Unpacking icu data package for processing.");
            Directory.CreateDirectory(Path.Join(context.ModuleDirectory.FullName, "temp", "pack_contents"));
            var processStartArgs = new System.Diagnostics.ProcessStartInfo
            {
                FileName = Path.Join(context.ModuleDirectory.FullName, "Binaries", "icupkg", "default", "icupkg" + context.Platform.ExtensionForExecutable),
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                StandardOutputEncoding = System.Text.Encoding.UTF8,
                StandardErrorEncoding = System.Text.Encoding.UTF8,
                UseShellExecute = false,
                CreateNoWindow = true,
            };
            string[] argumentList = [
                    "-d",
                    "temp/pack_contents",
                    "-x",
                    "*",
                    "-l",
                    $"data/{(BitConverter.IsLittleEndian ? "little" : "big")}/icudt77{(BitConverter.IsLittleEndian ? "l" : "b")}.dat"
                ];
            foreach (var arg in argumentList)
            {
                processStartArgs.ArgumentList.Add(arg);
            }
            var process = System.Diagnostics.Process.Start(processStartArgs) ?? throw new Exception("Failed to start icupkg process for unpacking data.");
            var output = process.StandardOutput.ReadToEnd(); // read output to prevent deadlock and capture it for printing the list to the temp/pack_contents.lst
            process.WaitForExit();
            if (process.ExitCode != 0)
            {
                var errorOutput = process.StandardError.ReadToEnd();
                throw new Exception($"icupkg process for unpacking data failed with exit code {process.ExitCode}. Error: {errorOutput}");
            }
            else
            {
                Console.WriteLine("Unpacking icu data package completed successfully.");
            }

            Console.WriteLine("writing pack_contents.lst");
            File.WriteAllText(Path.Join(context.ModuleDirectory.FullName, "temp", "pack_contents.lst"), output);
        }
        else
        {
            Console.WriteLine("icu data already unpacked, skipping unpacking.");
        }

        // pkgdata
        if (!File.Exists(Path.Join(context.ModuleDirectory.FullName, "Binaries", "pkgdata", "default", "pkgdata" + context.Platform.ExtensionForExecutable)))
        {
            Console.WriteLine("pkgdata not found, building pkgdata first.");
            var processStartArgs = new System.Diagnostics.ProcessStartInfo
            {
                FileName = "dotnet",
                Arguments = $"\"{ebuildPath}\" build {Path.Join(context.ModuleDirectory.FullName, "icu-pkgdata.ebuild.cs")} -p {MaxDependencyCompilationProcesses}",
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                UseShellExecute = false,
                CreateNoWindow = true,
                StandardErrorEncoding = System.Text.Encoding.UTF8,
                StandardOutputEncoding = System.Text.Encoding.UTF8,
            };
            var process = System.Diagnostics.Process.Start(processStartArgs) ?? throw new Exception("Failed to start ebuild process for icu-pkgdata.");
            var output = process.StandardOutput.ReadToEnd();
            Console.WriteLine(output);
            process.WaitForExit();
            if (process.ExitCode != 0)
            {
                var errorOutput = process.StandardError.ReadToEnd();
                throw new Exception($"Ebuild process for icu-pkgdata failed with exit code {process.ExitCode}. Error: {errorOutput}");
            }
        }
        else
        {
            Console.WriteLine("pkgdata already built, skipping build.");
        }

        if (!File.Exists(Path.Join(context.ModuleDirectory.FullName, "binaries", "icudt", "default", "icudt" + context.Platform.ExtensionForStaticLibrary)))
        {
            Directory.CreateDirectory(Path.Join(context.ModuleDirectory.FullName, "temp", "packaging_temp"));
            // provide all the env vars for the required tools
            Console.WriteLine("icu data library not found, building icu data library.");
            var processStartArgs = new System.Diagnostics.ProcessStartInfo
            {
                FileName = Path.Join(context.ModuleDirectory.FullName, "Binaries", "pkgdata", "default", "pkgdata" + context.Platform.ExtensionForExecutable),
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                StandardOutputEncoding = System.Text.Encoding.UTF8,
                StandardErrorEncoding = System.Text.Encoding.UTF8,
                UseShellExecute = false,
                CreateNoWindow = true
            };
            List<string> argumentList = [
                "-c",
                "-m",
                PackageType switch
                {
                    DataPackageType.Common => "common",
                    DataPackageType.Static => "static",
                    DataPackageType.Shared => "library",
                    _ => throw new Exception("Invalid package type."),
                },
                "-v",
                "-d",
                Path.Join(context.ModuleDirectory.FullName, "Binaries","icudt"),
                "-s",
                Path.Join(context.ModuleDirectory.FullName, "temp", "pack_contents"),
                "-p",
                "icudt77",
                "-T",
                "temp/packaging_temp",
                "-L",
                "icudt77",
            ];
            if (context.Platform.Name == "windows" && context.RequestedOutput == "shared")
            {
                argumentList.Add("-a");
                argumentList.Add(context.TargetArchitecture switch
                {
                    Architecture.X64 => "x64",
                    Architecture.X86 => "x86",
                    Architecture.Arm64 => "arm64",
                    Architecture.Arm => "arm",
                    _ => throw new Exception("Invalid architecture.")
                });
            }
            argumentList.Add(Path.Join(context.ModuleDirectory.FullName, "temp", "pack_contents.lst"));
            foreach (var arg in argumentList)
            {
                processStartArgs.ArgumentList.Add(arg);
            }
            var compilerFactory = (context.Toolchain.GetCompilerFactory(this, context.InstancingParams ?? throw new Exception("InstancingParams is null")) ?? throw new Exception("CompilerFactory is null"))!;
            var targetPath = Path.GetDirectoryName(compilerFactory.GetExecutablePath(this, context.InstancingParams ?? throw new Exception("InstancingParams is null")));
            processStartArgs.Environment["PATH"] = targetPath + ";" + (Environment.GetEnvironmentVariable("PATH") ?? string.Empty);
            Console.WriteLine("Using PATH: " + processStartArgs.Environment["PATH"]);

            var process = System.Diagnostics.Process.Start(processStartArgs) ?? throw new Exception("Failed to start pkgdata process for packaging data.");
            var output = process.StandardOutput.ReadToEnd(); // read output to prevent deadlock and capture it for printing the list to the temp/pack_contents.lst
            var errorOutput = process.StandardError.ReadToEnd();
            process.WaitForExit();
            Console.WriteLine(output);
            Console.WriteLine(errorOutput);
            if (process.ExitCode != 0)
            {
                throw new Exception($"pkgdata process for packaging data failed with exit code {process.ExitCode}. Error: {errorOutput}");
            }
            else
            {
                Console.WriteLine("Packaging icu data library completed successfully.");
            }
            // Copy the output to the binaries directory.
            var libFile = Path.Join(context.ModuleDirectory.FullName, "temp", "pack_contents", "icudt77" + context.Platform.ExtensionForStaticLibrary);
            if (File.Exists(libFile))
            {
                File.Copy(libFile, Path.Join(GetBinaryOutputDirectory(), "icudt77" + context.Platform.ExtensionForStaticLibrary), true);
                File.Delete(libFile);
            }
            var dllFile = Path.Join(context.ModuleDirectory.FullName, "temp", "pack_contents", "icudt77" + context.Platform.ExtensionForSharedLibrary);
            if (File.Exists(dllFile))
            {
                File.Copy(dllFile, Path.Join(GetBinaryOutputDirectory(), "icudt77" + context.Platform.ExtensionForSharedLibrary), true);
                File.Delete(dllFile);
            }
        }
        else
        {
            Console.WriteLine("icu data library already built, skipping build.");
        }



        if (PackageType == DataPackageType.Shared || PackageType == DataPackageType.Static)
        {
            Libraries.Public.Add(Path.Join(context.ModuleDirectory.FullName, "Binaries", "icudt", "default", "icudt77" + context.Platform.ExtensionForStaticLibrary));
        }


    }
}