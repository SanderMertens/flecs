// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "SGraphPinFlecsId.h"
#include "FlecsIdSelector.h"
#include "EdGraph/EdGraphPin.h"
#include "Entities/FlecsDefaultEntityEngineSubsystem.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "SGraphPinFlecsId"

void SGraphPinFlecsId::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
{
	
	SGraphPin::Construct(SGraphPin::FArguments(), InGraphPinObj);
}

TSharedRef<SWidget> SGraphPinFlecsId::GetDefaultValueWidget()
{
	
	// Add a "None" option first.
	Options.Add(MakeShared<FName>(FName(TEXT("None"))));
	EntityOptions.Add(FFlecsId(flecs::entity::null().id()));
	
	// Populate options using the default entity engine.
	FFlecsDefaultEntityEngine::Get().DefaultEntityQuery.each([this](flecs::entity Entity)
	{
		const uint64 Id = Entity.id();
		EntityOptions.Add(FFlecsId(Id));
		Options.Add(MakeShared<FName>(FName(Entity.name().c_str())));
	});
	
	FString DefaultValueString = GraphPinObj->GetDefaultAsString();
	DefaultValueString.RemoveFromStart(TEXT("FlecsId="));
	uint64 DefaultId = FCString::Strtoui64(*DefaultValueString, nullptr, 10);
	
	TSharedPtr<FName> InitialSelectedItem = Options[0];
	for (int32 Index = 0; Index < EntityOptions.Num(); ++Index)
	{
		if (EntityOptions[Index].Id == DefaultId)
		{
			InitialSelectedItem = Options[Index];
			break;
		}
	}
	
	return SNew(SFlecsIdSelector)
		.SelectedItem(GetSelectedName())
		.InitialSelectedItem(InitialSelectedItem)
		.NoneEntityText(FName(TEXT("None")))
		.PropertyHandle(nullptr)
		.Visibility(this, &SGraphPin::GetDefaultValueVisibility)
		.OnEntitySelected(FOnFlecsEntitySelected::CreateLambda([this](const FFlecsId NewEntity)
		{
			if (GraphPinObj)
			{
				const FString NewDefaultValue = FString::Printf(TEXT("FlecsId=%llu"), NewEntity.Id);
				GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, NewDefaultValue);
			}
		}));
}

TSharedPtr<FName> SGraphPinFlecsId::GetSelectedName() const
{
	// Get the current value of the pin.
	FString CurrentValue = GraphPinObj->GetDefaultAsString();
	CurrentValue.RemoveFromStart(TEXT("FlecsId="));
	const uint64 CurrentId = FCString::Strtoui64(*CurrentValue, nullptr, 10);
	
	// Find the corresponding name for the current value.
	for (int32 Index = 0; Index < Options.Num(); ++Index)
	{
		if (EntityOptions[Index].Id == CurrentId)
		{
			return Options[Index];
		}
	}
	
	return Options[0];
}

#undef LOCTEXT_NAMESPACE
