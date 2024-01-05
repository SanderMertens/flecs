using System.IO;
using UnrealBuildTool;

public class FlecsLibrary : ModuleRules
{
    public FlecsLibrary(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;

        PublicIncludePaths.AddRange(
            new string[] {
            ModuleDirectory,
            ModuleDirectory + "/flecs/include"
            }
        );
        
        AppendStringToPublicDefinition("flecs_EXPORTS", "0");
        
        PublicIncludePaths.AddRange(
            new string[] {
                ModuleDirectory,
                ModuleDirectory + "/include"
            }
        );
        
        PrivateIncludePaths.AddRange(
            new string[] {
                
            }
        );

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core"
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