using UnrealBuildTool;

public class UnrealFlecsEditor : ModuleRules
{
    public UnrealFlecsEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

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
                "SlateCore",
                "ToolMenus",
                "EditorStyle",
                "Projects",
                "PropertyEditor",
                "UnrealFlecs",
                "FlecsLibrary",
                "StructUtils",
                "InputCore",
                "UnrealEd",
                "GraphEditor",
                "BlueprintGraph",
                "StructUtilsEditor",
                "AssetRegistry",
            }
        );
    }
}