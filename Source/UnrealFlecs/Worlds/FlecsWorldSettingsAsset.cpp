// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsWorldSettingsAsset.h"

#include "Pipelines/FlecsDefaultGameLoop.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsWorldSettingsAsset)

#define LOCTEXT_NAMESPACE "FlecsWorldSettingsAsset"

UFlecsWorldSettingsAsset::UFlecsWorldSettingsAsset()
{
	WorldSettings.WorldName = "DefaultFlecsWorld";
	
	//WorldSettings.GameLoop = NewObject<UFlecsDefaultGameLoop>(this);
}

#if WITH_EDITOR

EDataValidationResult UFlecsWorldSettingsAsset::IsDataValid(FDataValidationContext& Context) const
{
	if (!IsValid(WorldSettings.GameLoop))
	{
		Context.AddError(FText::Format(
			LOCTEXT("InvalidGameLoop", "WorldSettings {0} does not have a valid GameLoop set."),
			FText::FromString(GetPathName())));
		
		return EDataValidationResult::Invalid;
	}
	
	return Super::IsDataValid(Context);
}

#endif // WITH_EDITOR


#undef LOCTEXT_NAMESPACE
