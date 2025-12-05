namespace CppCore;

using System.Runtime.InteropServices;
using ebuild.api;
using ebuild.api.Compiler;

class EdvarCppCore : ModuleBase
{

    public EdvarCppCore(ModuleContext context) : base(context)
    {
        this.Name = "edvar-cpp-core";
        this.Type = ModuleType.SharedLibrary;
        this.CppStandard = CppStandards.Cpp20;
        this.SourceFiles = [.. ModuleUtilities.GetAllSourceFiles(this, "Source", "cpp")];
        this.Definitions.Private.Add("BUILDING_EDVAR_CPP_CORE=1");
        if(context.RequestedOutput is "dynamic" or "shared"){
            this.Definitions.Public.Add("EDVAR_CPP_CORE_DYNAMIC=1");
        }
        this.Includes.Public.Add("Include");
        this.Includes.Private.Add("Source");

        this.ForceIncludes.Private.Add("Source/ForceInclude.hpp");

        if (context.Toolchain.Name == "msvc")
        {
            this.Definitions.Private.Add("UNICODE");
            this.CompilerOptions.Add("/diagnostics:caret");
            this.Definitions.Private.Add("_CRT_SECURE_NO_WARNINGS");
        }

        // Add ICU combined as a static library to this module
        this.Dependencies.Public.Add("ThirdParty/mimalloc/mimalloc.ebuild.cs");
        this.Dependencies.Public.Add("static:ThirdParty/icu/icu.ebuild.cs");
    }
}