namespace CppCore;

using System.Runtime.InteropServices;
using ebuild.api;


class EdvarCppCore : ModuleBase
{
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


        // Add ICU combined as a static library to this module
        this.Dependencies.Public.Add("static:ThirdParty/icu/icu.ebuild.cs");
    }
}