namespace CppCore;

using System.Runtime.InteropServices;
using ebuild.api;
using ebuild.api.Compiler;

class EdvarCppCore : ModuleBase
{
    [ModuleOption(ChangesResultBinary = true, Name = "allow-simd", Description = "Enable or disable SIMD optimizations in math library")]
    public bool AllowSimd = true;
    [ModuleOption(ChangesResultBinary = true, Name = "enable-avx", Description = "Enable or disable AVX optimizations in math library")]
    public bool EnableAvx = true;
    [ModuleOption(ChangesResultBinary = true, Name = "debug-trace-allocators", Description = "Enable or disable tracing of allocators in debug mode")]
    public bool DebugTraceAllocators = false;

    public EdvarCppCore(ModuleContext context) : base(context)
    {
        this.Name = "edvar-cpp-core";
        this.Type = ModuleType.SharedLibrary;
        this.CppStandard = CppStandards.Cpp20;
        this.SourceFiles = [.. ModuleUtilities.GetAllSourceFiles(this, "Source", "cpp")];
        this.Definitions.Private.Add("BUILDING_EDVAR_CPP_CORE=1");
        if (context.RequestedOutput is "dynamic" or "shared" or "default")
        {
            this.Definitions.Public.Add("EDVAR_CPP_CORE_DYNAMIC=1");
        }else{
            this.Definitions.Public.Add("EDVAR_CPP_CORE_STATIC=1");
            this.Type = ModuleType.StaticLibrary;
        }
        this.Includes.Public.Add("Include");
        this.Includes.Private.Add("Source");

        this.ForceIncludes.Public.Add("Source/ForceInclude.hpp");

        this.Definitions.Private.Add($"EDVAR_CPP_CORE_MATH_ALLOW_SIMD={(this.AllowSimd ? 1 : 0)}");
        this.Definitions.Private.Add($"EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX={(this.EnableAvx ? 1 : 0)}");

        if(this.DebugTraceAllocators && context.Configuration == "debug")
        {
            this.Definitions.Public.Add("EDVAR_CPP_CORE_ALLOCATOR_TRACING=1");
        }

        if (context.Toolchain.Name == "msvc")
        {
            this.Definitions.Private.Add("UNICODE");
            this.CompilerOptions.Add("/diagnostics:caret");
            this.Definitions.Private.Add("_CRT_SECURE_NO_WARNINGS");
        }


        if (context.Platform.Name == "windows")
        {
            this.Definitions.Private.Add("PLATFORM_WINDOWS=1");
            this.Libraries.Private.Add("User32.lib");
            this.Libraries.Private.Add("Advapi32.lib");
            this.Libraries.Private.Add("dbghelp.lib");
            this.Libraries.Private.Add("Xinput.lib");
            this.Libraries.Private.Add("d3d12.lib");
            this.Libraries.Private.Add("dxgi.lib");
            this.Libraries.Private.Add("Shcore.lib");
        }
        else if (context.Platform.Name == "linux")
        {
            this.Definitions.Private.Add("PLATFORM_LINUX=1");
            this.Libraries.Private.Add("pthread");
            this.Libraries.Private.Add("dl");
        }
        // Add ICU combined as a static library to this module
        this.Dependencies.Public.Add("ThirdParty/mimalloc/mimalloc.ebuild.cs");
        this.Dependencies.Public.Add("static:ThirdParty/icu/icu.ebuild.cs");
    }
}