using UnrealBuildTool;

public class SolidMacrosEditor : ModuleRules
{
    public SolidMacrosEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "SolidMacros",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "Kismet",
                "PropertyEditor",
                "UnrealEd",
                "ToolWidgets",
                "InputCore",
                
            }
        );
    }
}