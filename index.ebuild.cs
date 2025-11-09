namespace CppCore;

using System.Runtime.InteropServices;
using ebuild.api;
using ebuild.api.Compiler;

class EdvarCppCore : ModuleBase
{

    [ModuleOption(ChangesResultBinary = true,
    Description = "Sets the CPU extension to use SSE4.2 for this module. If true the math will use SSE4.2 instructions.If false will use SSE2 instructions.\n" +
    "Both are available widely according to the \"Steam Hardware & Software Survey: September 2025\". SSE2 : 99.94%, SSE4.2: 99.78%",
    Name = "UseSSE4_2")]
    bool UseSSE4_2 = true;
    [ModuleOption(ChangesResultBinary = true,
    Description = "Sets the CPU extension to use AVX2 for this module. If true the math will use AVX2 instructions. If false will use SSE4.2 instructions.\n" +
    "Both are available widely according to the \"Steam Hardware & Software Survey: September 2025\". SSE4.2 : 99.78%, AVX2: 95.03%",
    Name = "UseAVX2")]
    bool UseAVX2 = true;

    //TODO: Add Neon support for ARM CPUs. This is very much later down the line.

    public EdvarCppCore(ModuleContext context) : base(context)
    {
        this.Name = "edvar-cpp-core";
        this.Type = ModuleType.SharedLibrary;
        this.CppStandard = CppStandards.Cpp20;
        this.SourceFiles = [.. ModuleUtilities.GetAllSourceFiles(this, "Source", "cpp", "h")];
        this.Definitions.Private.Add("BUILDING_EDVAR_CPP_CORE=1");
        this.Includes.Public.Add("Source/Public");
        this.Includes.Private.Add("Source/Private");
        this.ForceIncludes.Public.Add("Source/Private/force_include.h");
        if (UseAVX2)
        {
            this.Definitions.Public.Add("EDVAR_CPP_CORE_USE_SSE4_2=1");
            this.Definitions.Public.Add("EDVAR_CPP_CORE_USE_AVX2=1");
            this.CPUExtension = CPUExtensions.AVX2;
        }
        else if (UseSSE4_2)
        {
            this.Definitions.Public.Add("EDVAR_CPP_CORE_USE_SSE4_2=1");
            this.CPUExtension = CPUExtensions.SSE4_2;
        }
        else
        {
            this.Definitions.Public.Add("EDVAR_CPP_CORE_USE_SSE4_2=0");
            this.CPUExtension = CPUExtensions.SSE2;
        }

        if(context.Toolchain.Name == "msvc")
        {
            this.Definitions.Private.Add("UNICODE");
            this.CompilerOptions.Add("/diagnostics:caret");
        }


        // Add ICU combined as a static library to this module
        this.Dependencies.Public.Add("static:ThirdParty/icu/icu.ebuild.cs");
    }
}