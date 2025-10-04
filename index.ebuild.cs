namespace CppCore;

using System.Runtime.InteropServices;
using ebuild.api;
using ebuild.api.Compiler;

class EdvarCppCore : ModuleBase
{

    [ModuleOption(ChangesResultBinary = true,
    Description = "Sets the CPU extension to use for this module. If true the math will use SSE4.2 instructions.If false will use SSE2 instructions.\n" +
    "Both are available widely according to the \"Steam Hardware & Software Survey: September 2025\". SSE2 : 99.94%, SSE4.2: 99.78%",
    Name = "UseSSE4_2")]
    bool UseSSE4_2 = true;

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
        if (UseSSE4_2)
        {
            this.Definitions.Public.Add("EDVAR_CPP_CORE_USE_SSE4_2=1");
            this.CPUExtension = CPUExtensions.SSE4_2;
        }
        else
        {
            this.Definitions.Public.Add("EDVAR_CPP_CORE_USE_SSE4_2=0");
            this.CPUExtension = CPUExtensions.SSE2;
        }



        // Add ICU combined as a static library to this module
        this.Dependencies.Public.Add("static:ThirdParty/icu/icu.ebuild.cs");
    }
}