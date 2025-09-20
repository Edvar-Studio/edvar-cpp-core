namespace thirdparty.icu;

using System.Diagnostics;
using System.Reflection;
using System.Runtime.InteropServices;
using ebuild.api;

public class Icu : ModuleBase
{
    [ModuleOption(ChangesResultBinary = true, Description = "The data mode for the ICU module. Can be 'static', 'shared or 'common'. Default is 'static'.")]
    string DataMode = "static";
    public Icu(ModuleContext context) : base(context)
    {
        if (DataMode is not "static" and not "shared" and not "common")
        {
            throw new Exception("Invalid DataMode for Icu module. Must be either 'static', 'shared' or 'common'.");
        }

        this.Type = ModuleType.StaticLibrary;
        this.Name = "icu";
        this.UseVariants = false;
        this.CppStandard = CppStandards.Cpp17;

        var mode = context.SelfReference.GetOutput() switch
        {
            "static" => "static",
            "shared" => "shared",
            _ => throw new Exception("Invalid output type for Icu module. Must be either static or shared."),
        };


        this.Includes.Public.Add("include");

        this.Dependencies.Private.Add($"{mode}:icu-i18n");
        this.Dependencies.Private.Add($"{mode}:icu-common");
        this.Dependencies.Private.Add($"{DataMode}:icu-data");

    }
}