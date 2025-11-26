namespace thirdparty.mimalloc;

using System.Reflection.Emit;
using System.Runtime.InteropServices;
using System.Text.RegularExpressions;
using ebuild.api;


public class Mimalloc : ModuleBase
{
    public static string MimallocSource = "https://github.com/microsoft/mimalloc/archive/refs/tags/v3.1.5.zip";
    [ModuleOption]
    bool Secure = false;
    [ModuleOption]
    bool Padding = false;
    [ModuleOption]
    bool NoPadding = false;
    [ModuleOption]
    bool Override = true;
    [ModuleOption]
    bool XMalloc = false;
    [ModuleOption]
    bool ShowErrors = false;
    [ModuleOption]
    bool TrackValgrind = false;
    [ModuleOption]
    bool TrackAsan = false;
    [ModuleOption]
    bool TrackETW = false;
    [ModuleOption]
    bool UseCxx = false;
    [ModuleOption]
    bool OptimizeForArchitecture = false;
    [ModuleOption]
    bool OsxInterpose = true;
    [ModuleOption]
    bool OsxZone = true;
    [ModuleOption]
    bool WindowsRedirect = true;
    [ModuleOption]
    bool WindowsUseFixedTls = false;
    [ModuleOption]
    bool LocalDynamicTls = false;
    [ModuleOption]
    bool LibCMusl = false;
    [ModuleOption]
    bool Guarded = false;
    [ModuleOption]
    bool SkipCollectOnExit = false;

    public Mimalloc(ModuleContext context) : base(context)
    {
        this.Type = ModuleType.StaticLibrary;
        this.Name = "mimalloc";
        this.OutputDirectory = "Binaries/mimalloc";
        this.UseVariants = false;
        this.CppStandard = CppStandards.Cpp20;
        var downloadDir = Path.Join(context.ModuleDirectory.FullName, "source");

        ModuleUtilities.GetAndExtractSourceFromArchiveUrl(MimallocSource, downloadDir, null);
        var mimallocSourceRoot = Path.Join(downloadDir, "mimalloc-3.1.5");

        var miSources = new List<string>([
            "src/alloc.c",
            "src/alloc-aligned.c",
            "src/alloc-posix.c",
            "src/arena.c",
            "src/arena-meta.c",
            "src/bitmap.c",
            "src/heap.c",
            "src/init.c",
            "src/libc.c",
            "src/options.c",
            "src/os.c",
            "src/page.c",
            "src/random.c",
            "src/stats.c",
            "src/prim/prim.c"
        ]);

        var miCFlags = new List<string>();
        var miCFlagsStatic = new List<string>();
        var miCFlagsDynamic = new List<string>();
        var miLibraries = new List<string>();
        var miDefines = new List<string>();

        var miArch = string.Empty;
        var optArchFlags = string.Empty;
        if (context.TargetArchitecture == System.Runtime.InteropServices.Architecture.X64)
        {
            miArch = "x64";
        }
        else if (context.TargetArchitecture == System.Runtime.InteropServices.Architecture.Arm64)
        {
            miArch = "arm64";
        }
        else if (context.TargetArchitecture == System.Runtime.InteropServices.Architecture.X86)
        {
            miArch = "x86";
        }
        else if (context.TargetArchitecture == System.Runtime.InteropServices.Architecture.Arm)
        {
            miArch = "arm";
        }
        if (context.TargetArchitecture == Architecture.Arm64)
        {
            OptimizeForArchitecture = true;
        }
        if (Override)
        {
            if (context.Platform.Name == "apple")
            {
                if (OsxZone)
                {
                    miSources.Add("src/prim/osx/alloc-override-zone.c");
                    miDefines.Add("MI_OSX_ZONE=1");
                    if (!OsxInterpose)
                    {
                        context.AddMessage(ModuleContext.Message.SeverityTypes.Warning, "Zone overrinding usually also needs interpose (Enable OsxInterpose option).");
                    }
                }
                if (OsxInterpose)
                {
                    context.AddMessage(ModuleContext.Message.SeverityTypes.Info, "Use interpose to override malloc (OsxInterpose is enabled).");
                    miDefines.Add("MI_OSX_INTERPOSE=1");
                    if (!OsxZone)
                    {
                        context.AddMessage(ModuleContext.Message.SeverityTypes.Warning, "Interpose usually also needs zone overriding (Enable OsxZone option).");
                    }
                }
                if (UseCxx && OsxInterpose)
                {
                    context.AddMessage(ModuleContext.Message.SeverityTypes.Warning, "If dynamically overriding malloc/free, it is more reliable to build mimalloc as C code (Disable UseCxx option).");
                }
            }
        }

        if (context.Platform.Name == "windows")
        {
            if (!WindowsRedirect)
            {
                miDefines.Add("MI_WIN_NOREDIRECT=1");
            }
        }

        if (Secure)
        {
            context.AddMessage(ModuleContext.Message.SeverityTypes.Info, "Set full secure build (Secure option enabled).");
            miDefines.Add("MI_SECURE=4");
        }
        var hasValgrind = false;
        if (TrackValgrind)
        {
            hasValgrind = ModuleUtilities.CheckIncludeFilesExists(this, "valgrind/valgrind.h;valgrind/memcheck.h").Result;
            if (!hasValgrind)
            {
                context.AddMessage(ModuleContext.Message.SeverityTypes.Warning, "Cannot find the 'valgrind/valgrind.h' and 'valgrind/memcheck.h' -- install valgrind first?");
                context.AddMessage(ModuleContext.Message.SeverityTypes.Info, "Disabling Valgrind support (TrackValgrind option).");
            }
            else
            {
                context.AddMessage(ModuleContext.Message.SeverityTypes.Info, "Enable Valgrind support (TrackValgrind option is enabled).");
                miDefines.Add("MI_TRACK_VALGRIND=1");
            }
        }

        if (TrackAsan)
        {
            // macOS + override cannot use ASan
            if (context.Platform.Name == "apple" && Override)
            {
                TrackAsan = false;
                context.AddMessage(ModuleContext.Message.SeverityTypes.Warning, "Cannot enable address sanitizer support on macOS if MI_OVERRIDE is ON (TrackAsan=OFF)");
            }

            // ASan and Valgrind are mutually exclusive
            if (TrackAsan && TrackValgrind)
            {
                TrackAsan = false;
                context.AddMessage(ModuleContext.Message.SeverityTypes.Warning, "Cannot enable address sanitizer support with Valgrind support enabled (TrackAsan=OFF)");
            }

            if (TrackAsan)
            {
                var hasAsanH = ModuleUtilities.CheckIncludeFilesExists(this, "sanitizer/asan_interface.h").Result;
                if (!hasAsanH)
                {
                    TrackAsan = false;
                    context.AddMessage(ModuleContext.Message.SeverityTypes.Warning, "Cannot find the 'sanitizer/asan_interface.h' -- install address sanitizer support first");
                    context.AddMessage(ModuleContext.Message.SeverityTypes.Info, "Compile without address sanitizer support (TrackAsan=OFF)");
                }
                else
                {
                    context.AddMessage(ModuleContext.Message.SeverityTypes.Info, "Compile with address sanitizer support (TrackAsan=ON)");
                    miDefines.Add("MI_TRACK_ASAN=1");
                    miCFlags.Add("-fsanitize=address");
                    miLibraries.Add("-fsanitize=address");
                }
            }
        }

        // ETW handling (converted from CMake logic)
        if (TrackETW)
        {
            // ETW only supported on Windows
            if (context.Platform.Name != "windows")
            {
                TrackETW = false;
                context.AddMessage(ModuleContext.Message.SeverityTypes.Warning, "Can only enable ETW support on Windows (TrackETW=OFF)");
            }

            // ETW cannot be enabled together with Valgrind or ASan
            if (TrackETW && (TrackValgrind || TrackAsan))
            {
                TrackETW = false;
                context.AddMessage(ModuleContext.Message.SeverityTypes.Warning, "Cannot enable ETW support with also Valgrind or ASAN support enabled (TrackETW=OFF)");
            }

            if (TrackETW)
            {
                context.AddMessage(ModuleContext.Message.SeverityTypes.Info, "Compile with Windows event tracing support (TrackETW=ON)");
                miDefines.Add("MI_TRACK_ETW=1");
            }
        }

        // Guard pages handling (from CMake MI_GUARDED)
        if (Guarded)
        {
            context.AddMessage(ModuleContext.Message.SeverityTypes.Info, "Compile guard pages behind certain object allocations (MI_GUARDED=ON)");
            miDefines.Add("MI_GUARDED=1");

            // The original CMake toggles MI_NO_PADDING=ON if it was OFF.
            // In this C# conversion the 'Padding' option represents MI_NO_PADDING (false == padding enabled).
            if (!NoPadding)
            {
                context.AddMessage(ModuleContext.Message.SeverityTypes.Info, "  Disabling padding due to guard pages (MI_NO_PADDING=ON)");
                NoPadding = true;
            }
        }

        if (SkipCollectOnExit)
        {
            context.AddMessage(ModuleContext.Message.SeverityTypes.Info, "Skip collecting memory on program exit (SkipCollectOnExit=ON)");
            miDefines.Add("MI_SKIP_COLLECT_ON_EXIT=1");
        }
        if (NoPadding)
        {
            context.AddMessage(ModuleContext.Message.SeverityTypes.Info, "Suppress any padding of heap blocks (MI_NO_PADDING=ON)");
            miDefines.Add("MI_PADDING=0");
        }
        else
        {
            if (Padding) { context.AddMessage(ModuleContext.Message.SeverityTypes.Info, "Enable explicit padding of heap blocks (MI_PADDING=ON)"); miDefines.Add("MI_PADDING=1"); }
        }

        if (XMalloc)
        {
            context.AddMessage(ModuleContext.Message.SeverityTypes.Info, "Enable abort() calls on memory allocation failure (XMalloc is on)");
            miDefines.Add("MI_XMALLOC=1");
        }

        if (ShowErrors)
        {
            context.AddMessage(ModuleContext.Message.SeverityTypes.Info, "Enable printing of error and warning messages by default (ShowErrors is on)");
            miDefines.Add("MI_SHOW_ERRORS=1");
        }

        if (UseCxx)
        {
            context.AddMessage(ModuleContext.Message.SeverityTypes.Info, "Use the C++ compiler to compile (UseCxx=ON)");
            CStandard = null;
            CppStandard = CppStandards.Cpp17;
            if (context.Toolchain.Name.Contains("clang"))
            {
                miCFlags.Add("-Wno-deprecated");
            }
            if (context.Toolchain.Name.Contains("intel") && !context.Toolchain.Name.Contains("llvm"))
            {
                miCFlags.Add("-Kc++");
            }
        }

        if (LibCMusl)
        {
            context.AddMessage(ModuleContext.Message.SeverityTypes.Info, "Assume using musl libc (LibCMusl=ON)");
            miDefines.Add("MI_LIBC_MUSL=1");
        }

        if (WindowsUseFixedTls)
        {
            context.AddMessage(ModuleContext.Message.SeverityTypes.Info, "Use fixed TLS slot on Windows to avoid extra tests in the malloc fast path (WindowsUseFixedTls=ON)");
            miDefines.Add("MI_WIN_USE_FIXED_TLS=1");
        }


        if (File.Exists("/proc/cpuinfo"))
        {
            Regex regex = new("^mmu[ \t]+:[ \t]+sv39$");
            if (regex.IsMatch(File.ReadAllText("/proc/cpuinfo")))
            {
                context.AddMessage(ModuleContext.Message.SeverityTypes.Info, "Set virtual address bits to 39 (SV39 MMU detected)");
                miDefines.Add("MI_DEFAULT_VIRTUAL_ADDRESS_BITS=39");
            }
        }

        if (context.Toolchain.Name.Contains("clang") || context.Toolchain.Name.Contains("gcc"))
        {
            miCFlags.Add("-Wno-unknown-pragmas");
            miCFlags.Add("-fvisibility=hidden");
            if (!UseCxx)
            {
                miCFlags.Add("-Wstring-prototypes");
            }
            if (context.Toolchain.Name.Contains("clang"))
            {
                miCFlags.Add("-Wno-static-in-inline");
            }
        }

        if (context.Toolchain.Name == "intel")
        {
            miCFlags.Add("-fvisibility=hidden");
        }

        if (Regex.IsMatch(context.Toolchain.Name, "clang|gcc|intel", RegexOptions.IgnoreCase)
            && !string.Equals(context.Platform.Name, "haiku", StringComparison.OrdinalIgnoreCase))
        {
            if (LocalDynamicTls)
            {
                miCFlags.Add("-ftls-model=local-dynamic");
            }
            else
            {
                if (LibCMusl)
                {
                    miCFlagsStatic.Add("-ftls-model=local-dynamic");
                    miCFlagsDynamic.Add("-ftls-model=initial-exec");
                    context.AddMessage(ModuleContext.Message.SeverityTypes.Info, "Use local dynamic TLS for the static build (since LibCMusl=ON)");
                }
                else
                {
                    miCFlags.Add("-ftls-model=initial-exec");
                }
            }

            if (Override)
            {
                miCFlags.Add("-fno-builtin-malloc");
            }
        }

        if (Regex.IsMatch(context.Toolchain.Name ?? string.Empty, "AppleClang|Clang|GNU|Intel|gcc", RegexOptions.IgnoreCase)
    && !string.Equals(context.Platform.Name, "haiku", StringComparison.OrdinalIgnoreCase))
        {
            if (OptimizeForArchitecture) // MI_OPT_ARCH
            {
                // Apple + clang with CMAKE_OSX_ARCHITECTURES set -> support multi-arch binaries
                if (context.Platform.Name == "apple" && Regex.IsMatch(context.Toolchain.Name ?? string.Empty, "AppleClang|Clang", RegexOptions.IgnoreCase))
                {
                    if (context.TargetArchitecture == Architecture.Arm64)
                    {
                        // equivalent to: list(APPEND MI_OPT_ARCH_FLAGS "-Xarch_arm64;-march=armv8.1-a")
                        optArchFlags = "-Xarch_arm64;-march=armv8.1-a";
                    }
                }
                else if (miArch == "arm64")
                {
                    // MI_ARCH == "arm64" -> fast atomics
                    optArchFlags = "-march=armv8.1-a";
                }
            }
        }

        if (context.Toolchain.Name!.Contains("msvc"))
        {
            miCFlags.Add("/Zc:__cplusplus");
            if (OptimizeForArchitecture)
            {
                if (miArch == "arm64")
                {
                    optArchFlags = "/arch:armv8.1";
                }
            }
        }

        if (context.Toolchain.Name.Contains("mingw"))
        {
            Definitions.Private.Add("_WIN32_WINNT=0x0600");
        }

        if (optArchFlags != string.Empty)
        {
            miCFlags.Add(optArchFlags);
            context.AddMessage(ModuleContext.Message.SeverityTypes.Info, $"Architecture specific optimization is enabled (with {optArchFlags}) (OptimizeForArchitecture=ON)");
        }

        if (context.Platform.Name == "windows")
        {
            miLibraries.AddRange(["psapi.lib", "shell32.lib", "user32.lib", "advapi32.lib", "bcrypt.lib"]);
        }
        else
        {
            miLibraries.AddRange(["libpthread.so", "librt.so", "libatomic.so"]);
        }
        CompilerOptions.AddRange(miCFlags);
        if (context.RequestedOutput is "shared" or "dynamic")
        {
            Definitions.Public.Add("MI_SHARED_LIB=1");
            Definitions.Private.Add("MI_SHARED_LIB_EXPORT=1");

            CompilerOptions.AddRange(miCFlagsDynamic);
            Libraries.Private.AddRange(miLibraries);

            if (Override)
            {
                Definitions.Private.Add("MI_MALLOC_OVERRIDE=1");
            }
        }
        else if (context.RequestedOutput is "static" or "default")
        {
            Definitions.Public.Add("MI_STATIC_LIB=1");
            CompilerOptions.AddRange(miCFlagsStatic);
            Libraries.Private.AddRange(miLibraries);
        }
        else
        {
            CompilerOptions.AddRange(miCFlags);
            Libraries.Private.AddRange(miLibraries);
        }

        if (context.Platform.Name != "windows" && Override)
        {
            Definitions.Private.Add("MI_MALLOC_OVERRIDE=1");
        }

        SourceFiles = miSources.ConvertAll(file => Path.Join(mimallocSourceRoot, file));
        if (UseCxx)
            CppStandard = CppStandards.Cpp17;
        else
            CStandard = CStandards.C11;
        Definitions.Public.AddRange(miDefines.ConvertAll(def => new Definition(def)));
        Includes.Public.Add(Path.Join(mimallocSourceRoot, "include"));
        Includes.Private.Add(Path.Join(mimallocSourceRoot, "src"));
        // Add the mimalloc redirect libraries.
        if (context.Platform.Name == "windows" && Override)
        {
            var redirectLibDir = Path.Join(mimallocSourceRoot, "bin");
            var redirectSuffix = miArch;

            if (context.Platform.GetType().GetProperty("IsArm64EC") != null &&
               (bool)context.Platform.GetType().GetProperty("IsArm64EC")!.GetValue(context.Platform)!)
            {
                redirectSuffix += "-arm64ec";
            }
            else if (miArch == "x64")
            {
                redirectSuffix = "";
                if (RuntimeInformation.ProcessArchitecture == Architecture.Arm64)
                {
                    context.AddMessage(ModuleContext.Message.SeverityTypes.Warning,
                    "Note: x64 code emulated on Windows for arm64 should use an arm64ec build of 'mimalloc.dll'" +
                    "      together with 'mimalloc-redirect-arm64ec.dll'. See the 'bin/readme.md' for more information.");
                }
            }
            else if (miArch == "x86")
            {
                redirectSuffix = "32";
            }
            else
            {
                redirectSuffix = "-" + miArch;
            }
            var redirectLibFileName = $"mimalloc-redirect{(redirectSuffix != "" ? "-" : "")}{redirectSuffix}";
            var redirectLibPath = Path.Join(redirectLibDir, redirectLibFileName);
            Libraries.Private.Add(redirectLibPath + ".lib");
            // we need the redirect Dll next to the runtime dependencies.
            AdditionalDependencies.Public.Add(new AdditionalDependency(redirectLibPath, $"${{RuntimeDir}}/{redirectLibFileName}.dll"));
        }
    }
}