// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsCollectionDataAsset.h"

#include "Misc/DataValidation.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsCollectionDataAsset)

UFlecsCollectionDataAsset::UFlecsCollectionDataAsset()
{
}

FPrimaryAssetId UFlecsCollectionDataAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId("FlecsCollectionDataAsset", GetFName());
}

FFlecsCollectionDefinition UFlecsCollectionDataAsset::MakeCollectionDefinition() const
{
	FFlecsCollectionDefinition Definition;
	Definition.Name = GetName();
	Definition.Collections = Collections;
	Definition.Record = Record;
	Definition.SubEntityCollections = SubEntityCollections;
	return Definition;
}

#if WITH_EDITOR

EDataValidationResult UFlecsCollectionDataAsset::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult SuperResult = Super::IsDataValid(Context);

	const EDataValidationResult CollectionsResult = ValidateCollections(Context);
	
	return CombineDataValidationResults(SuperResult, CollectionsResult);
}

EDataValidationResult UFlecsCollectionDataAsset::ValidateCollections(FDataValidationContext& Context) const
{
	// @TODO: Implement collection validation!
	/*for (const UFlecsCollectionDataAsset* Collection : Collections)
	{
		if UNLIKELY_IF(!IsValid(Collection))
		{
			Context.AddError(FText::FromString(TEXT("Collections array contains null element!")));
			return EDataValidationResult::Invalid;
		}

		if UNLIKELY_IF(Collection == this)
		{
			Context.AddError(FText::FromString(TEXT("Collections array contains self reference!")));
			return EDataValidationResult::Invalid;
		}
	}

	if (TSet<const UFlecsCollectionDataAsset*>{Collections}.Num() != Collections.Num())
	{
		Context.AddError(FText::FromString(TEXT("Collections contains duplicates!")));
		return EDataValidationResult::Invalid;
	}
	
	return EDataValidationResult::Valid;*/

	return EDataValidationResult::Valid;
}

#endif // WITH_EDITOR
