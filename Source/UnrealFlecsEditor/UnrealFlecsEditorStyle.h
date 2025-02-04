// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Editor/EditorStyle/Public/EditorStyleSet.h"
#include "Runtime/Projects/Public/Interfaces/IPluginManager.h"
#include "Runtime/SlateCore/Public/Brushes/SlateImageBrush.h"
#include "Runtime/SlateCore/Public/Styling/SlateStyle.h"
#include "Runtime/SlateCore/Public/Styling/SlateStyleRegistry.h"

class FUnrealFlecsEditorStyle : public FEditorStyle
{
public:
	static void Initialize()
	{
		FString PluginContentDir = IPluginManager::Get().FindPlugin(TEXT("UnrealFlecs"))->GetBaseDir();
		const FString ContentDir = FPaths::Combine(PluginContentDir, TEXT("Resources"));

		FSlateStyleSet* StyleSet = new FSlateStyleSet("UnrealFlecsEditorStyle");
		StyleSet->SetContentRoot(ContentDir);

		const FVector2D IconSize(128.0f, 128.0f);
		const FString IconPath = FPaths::Combine(ContentDir, TEXT("Icon128.png"));
		StyleSet->Set("UnrealFlecs.FlecsEditor.FlecsLogo", new FSlateImageBrush(IconPath, IconSize));

		FSlateStyleRegistry::RegisterSlateStyle(*StyleSet);
	}

	static void Shutdown()
	{
		FSlateStyleRegistry::UnRegisterSlateStyle("UnrealFlecsEditorStyle");
	}
	
}; // struct FUnrealFlecsEditorStyle
