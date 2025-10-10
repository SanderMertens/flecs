// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsWorldSettingsAsset.h"

#include "Pipelines/FlecsDefaultGameLoop.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsWorldSettingsAsset)

#define LOCTEXT_NAMESPACE "FlecsWorldSettingsAsset"

UFlecsWorldSettingsAsset::UFlecsWorldSettingsAsset()
{
	WorldSettings.WorldName = "DefaultFlecsWorld";
}

#if WITH_EDITOR

EDataValidationResult UFlecsWorldSettingsAsset::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);
	
	if (!IsValid(WorldSettings.GameLoop))
	{
		Context.AddError(FText::Format(
			LOCTEXT("InvalidGameLoop", "WorldSettings {0} does not have a valid GameLoop set."),
			FText::FromString(GetPathName())));
		
		Result = CombineDataValidationResults(Result, EDataValidationResult::Invalid);
	}
	
	return Result;
}

#endif // WITH_EDITOR

FPrimaryAssetId UFlecsWorldSettingsAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId("FlecsWorld", GetFNameSafe(this));
}

#undef LOCTEXT_NAMESPACE
