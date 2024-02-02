
using UnrealBuildTool;

public class FlecsLibrary : ModuleRules
{
    public FlecsLibrary(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        
        Type = ModuleType.External;
        CStandard = CStandardVersion.C99;
        CppStandard = CppStandardVersion.Cpp17;
        
        AppendStringToPublicDefinition("flecs_EXPORTS", "0");
        
        PublicIncludePaths.AddRange(
            new string[] {
                ModuleDirectory,
                ModuleDirectory + "/flecs/include"
            }
        );
        
        PrivateIncludePaths.AddRange(
            new string[] {
                
            }
        );

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore"
            }
        );
    }
}