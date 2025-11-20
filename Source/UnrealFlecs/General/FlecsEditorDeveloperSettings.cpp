// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsEditorDeveloperSettings.h"

#include "FlecsEditorPerProjectDeveloperSettings.h"
#include "Types/SolidNotNull.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsEditorDeveloperSettings)

UFlecsEditorDeveloperSettings::UFlecsEditorDeveloperSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FlecsExplorerURL = FFlecsEditorExplorerURL();
}

FFlecsEditorExplorerURL UFlecsEditorDeveloperSettings::GetFlecsExplorerURL() const
{
	const TSolidNotNull<const UFlecsEditorPerProjectDeveloperSettings*> PerProjectSettings =
		GetDefault<UFlecsEditorPerProjectDeveloperSettings>();

	const FFlecsEditorExplorerURL& URLToUse = PerProjectSettings->FlecsExplorerURLOverride.IsSet()
		? PerProjectSettings->FlecsExplorerURLOverride.GetValue()
		: FlecsExplorerURL;

	return URLToUse;
}

#if WITH_EDITOR

EDataValidationResult UFlecsEditorDeveloperSettings::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult SuperResult = Super::IsDataValid(Context);

	EDataValidationResult Result = EDataValidationResult::Valid;

	Result = CombineDataValidationResults(Result, SuperResult);
	return Result;
}

#endif // WITH_EDITOR
