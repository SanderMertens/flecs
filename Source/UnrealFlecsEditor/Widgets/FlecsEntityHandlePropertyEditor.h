// Fill out your copyright notice in the Description page of Project Settings.

// ReSharper disable CppMemberFunctionMayBeStatic
#pragma once

#include "CoreMinimal.h"
#include "DetailLayoutBuilder.h"
#include "PropertyHandle.h"
#include "IDetailChildrenBuilder.h"
#include "DetailWidgetRow.h"
#include "Entities/FlecsDefaultEntityEngineSubsystem.h"
#include "Widgets/Input/SComboBox.h"
#include "Worlds/FlecsWorldSubsystem.h"

class FFlecsEntityHandleCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance()
	{
		return MakeShareable(new FFlecsEntityHandleCustomization());
	}
	
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> InPropertyHandle, FDetailWidgetRow& HeaderRow,
		IPropertyTypeCustomizationUtils& CustomizationUtils) override
	{
		
	}

	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> InPropertyHandle, IDetailChildrenBuilder& ChildBuilder,
	                               IPropertyTypeCustomizationUtils& CustomizationUtils) override
	{
		PropertyHandle = InPropertyHandle;
		
		GEngine->GetEngineSubsystem<UFlecsDefaultEntityEngineSubsystem>()->GetEntityOptions().GenerateKeyArray(Options);

		UE_LOG(LogTemp, Warning, TEXT("Options array size: %d"), Options.Num());

		ApplyMetadataFilters();

		const TSharedPtr<IPropertyHandle> WorldNameHandle
			= PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFlecsEntityHandle,WorldName));
		if (WorldNameHandle && WorldNameHandle->IsValidHandle())
		{
			ChildBuilder.AddProperty(WorldNameHandle.ToSharedRef());
		}

		ChildBuilder.AddCustomRow(NSLOCTEXT("Flecs", "FlecsEntityHandle",
			"Flecs Entity Handle"))
		.NameContent()
		[
			SNew(STextBlock)
			.Text(NSLOCTEXT("Flecs", "SelectedEntity", "Selected Entity"))
			.Font(IDetailLayoutBuilder::GetDetailFont())
		]
		.ValueContent()
		.MaxDesiredWidth(250.f)
		[
			SNew(SComboBox<FName>)
			.OptionsSource(&Options)
			.OnSelectionChanged(this, &FFlecsEntityHandleCustomization::OnEntitySelected)
			.OnGenerateWidget(this, &FFlecsEntityHandleCustomization::GenerateEntityWidget)
			.ContentPadding(2)
			[
				SNew(STextBlock)
				.Text(this, &FFlecsEntityHandleCustomization::GetCurrentItemLabel)
			]
		];

		if UNLIKELY_IF(Options.Num() == 0)
		{
			ChildBuilder.AddCustomRow(NSLOCTEXT("Flecs", "NoEntitiesFound",
				"No entities found."))
			.NameContent()
			[
				SNew(STextBlock)
				.Text(NSLOCTEXT("Flecs", "NoEntitiesFound", "No entities found."))
				.Font(IDetailLayoutBuilder::GetDetailFont())
			];
		}
	}

private:
	TOptional<FName> SelectedItem;
	TArray<FName> Options;

	TSharedPtr<IPropertyHandle> PropertyHandle;

	// ReSharper disable once CppMemberFunctionMayBeConst
	void OnEntitySelected(const FName NewValue, ESelectInfo::Type)
	{
		if LIKELY_IF(PropertyHandle && PropertyHandle->IsValidHandle())
		{
			FScopedTransaction Transaction(NSLOCTEXT("Flecs", "ChangeEntity", "Change Entity"));
			PropertyHandle->NotifyPreChange();

			PropertyHandle->EnumerateRawData(
				[&](void* RawData, const int32 DataIndex, const int32 NumDatas)
				{
					if UNLIKELY_IF(RawData == nullptr)
					{
						return false;
					}

					FFlecsEntityHandle* EntityId = static_cast<FFlecsEntityHandle*>(RawData);
					EntityId->SetEntity(GEngine->GetEngineSubsystem<UFlecsDefaultEntityEngineSubsystem>()
						->GetEntityOptions()[NewValue]);
					EntityId->DisplayName = NewValue;
					return true;
				});

			PropertyHandle->NotifyPostChange(EPropertyChangeType::ValueSet);
			PropertyHandle->NotifyFinishedChangingProperties();
		}
	}

	void OnEntitySubmitted(const FName NewValue, ETextCommit::Type)
	{
		SelectedItem = NewValue;
	}

	TSharedRef<SWidget> GenerateEntityWidget(const FName InOption)
	{
		return SNew(STextBlock).Text(FText::FromName(InOption));
	}

	FText GetCurrentItemLabel() const
	{
		if (SelectedItem.IsSet())
		{
			return FText::FromName(SelectedItem.GetValue());
		}

		TOptional<FName> CommonValue;

		if (PropertyHandle && PropertyHandle->IsValidHandle())
		{
			PropertyHandle->EnumerateRawData(
				[&](void* RawData, const int32 DataIndex, const int32 NumDatas)
				{
					if UNLIKELY_IF(RawData == nullptr)
					{
						CommonValue.Reset();
						return false;
					}

					const FFlecsEntityHandle EntityId = *static_cast<FFlecsEntityHandle*>(RawData);
					const FName CurrentValue = EntityId.DisplayName;

					if (DataIndex > 0)
					{
						if (CommonValue.IsSet() && (CommonValue.GetValue() != CurrentValue))
						{
							CommonValue.Reset();
							return false;
						}
					}

					CommonValue = CurrentValue;
					return true;
				}
			);
		}

		if (CommonValue.IsSet())
		{
			return FText::FromName(CommonValue.GetValue());
		}

		return NSLOCTEXT("Flecs", "SelectAnEntity", "Select an entity...");
	}

	void ApplyMetadataFilters()
	{
		const FString ExcludedList = PropertyHandle->GetProperty()->GetMetaData(TEXT("Excluded"));
		const FString IncludedList = PropertyHandle->GetProperty()->GetMetaData(TEXT("Included"));

		TArray<FString> ExcludedItems;
		ExcludedList.ParseIntoArray(ExcludedItems, TEXT(","), true);

		TArray<FString> IncludedItems;
		IncludedList.ParseIntoArray(IncludedItems, TEXT(","), true);
		
		if (IncludedItems.Num() > 0)
		{
			TArray<FName> FilteredOptions;
			
			for (const FString& Item : IncludedItems)
			{
				FilteredOptions.Add(FName(*Item));
			}
			
			Options = FilteredOptions;
		}
		
		for (const FString& ExcludedItem : ExcludedItems)
		{
			Options.Remove(FName(*ExcludedItem));
		}
	}
	
}; // class FFlecsEntityHandleCustomization