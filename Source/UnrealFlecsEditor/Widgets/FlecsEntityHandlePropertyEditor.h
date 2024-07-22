// Fill out your copyright notice in the Description page of Project Settings.

// ReSharper disable CppMemberFunctionMayBeStatic
// ReSharper disable CppExpressionWithoutSideEffects
#pragma once

#include "CoreMinimal.h"
#include "DetailLayoutBuilder.h"
#include "PropertyHandle.h"
#include "IDetailChildrenBuilder.h"
#include "DetailWidgetRow.h"
#include "Entities/FlecsDefaultEntityEngineSubsystem.h"
#include "Logs/FlecsCategories.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Input/SSearchBox.h"
#include "Worlds/FlecsWorldSubsystem.h"

class FFlecsEntityHandleCustomization final : public IPropertyTypeCustomization
{
	const FText NoneEntityText = NSLOCTEXT("Flecs", "NoneEntity", "None");
	
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

    Options.Empty();
    FilteredOptions.Empty();

	FMessageLog("Flecs").Info(FText::FromString("Customizing Flecs Entity Handle"));

    for (const auto& [EntityName, EntityId] :
        GEngine->GetEngineSubsystem<UFlecsDefaultEntityEngineSubsystem>()->DefaultEntityOptions)
    {
        UN_LOGF(LogFlecsEditor, Verbose,
            "Adding entity %s to entity handle options.", *EntityName);

    	bool bIsAddedEntity = false;

    	for (auto [EntityRecord, bIsOptionEntity]
    		: GEngine->GetEngineSubsystem<UFlecsDefaultEntityEngineSubsystem>()->AddedDefaultEntities)
    	{
    		if (EntityRecord.Name == EntityName)
			{
    			if (bIsOptionEntity)
    			{
    				Options.Add(FName(*EntityName));
    			}
    			
    			bIsAddedEntity = true;
				break;
			}
    	}

    	if (!bIsAddedEntity)
		{
			Options.Add(FName(*EntityName));
		}
    }

    ApplyMetadataFilters();
    FilteredOptions = Options; // Initialize FilteredOptions to Options

    const TSharedPtr<IPropertyHandle> WorldNameHandle
        = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFlecsEntityHandle, WorldName));
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
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SComboButton)
            .OnGetMenuContent(this, &FFlecsEntityHandleCustomization::OnGenerateWidgetWithSearch)
            .ContentPadding(2)
            .ButtonContent()
            [
                SNew(STextBlock)
                .Text(this, &FFlecsEntityHandleCustomization::GetCurrentItemLabel)
            ]
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SButton)
            .Text(NSLOCTEXT("Flecs", "ClearSelection", "Clear"))
            .OnClicked(this, &FFlecsEntityHandleCustomization::OnClearClicked)
            .ContentPadding(2)
        ]
    ];

    if (Options.IsEmpty())
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
	mutable TOptional<FName> SelectedItem;
	TArray<FName> Options;
	TArray<FName> FilteredOptions;

	TSharedPtr<IPropertyHandle> PropertyHandle;

	// ReSharper disable once CppMemberFunctionMayBeConst
	void OnEntitySelected(const FName NewValue, ESelectInfo::Type)
	{
		if LIKELY_IF(PropertyHandle && PropertyHandle->IsValidHandle())
		{
			FScopedTransaction Transaction(NSLOCTEXT("Flecs", "ChangeEntity", "Change Entity"));
			PropertyHandle->NotifyPreChange();

			PropertyHandle->EnumerateRawData(
				[&](void* RawData, MAYBE_UNUSED const int32 DataIndex, MAYBE_UNUSED const int32 NumDatas)
				{
					if UNLIKELY_IF(RawData == nullptr)
					{
						return false;
					}

					FFlecsEntityHandle* EntityId = static_cast<FFlecsEntityHandle*>(RawData);
					EntityId->SetEntity(GEngine->GetEngineSubsystem<UFlecsDefaultEntityEngineSubsystem>()
						->DefaultEntityOptions[NewValue.ToString()]);
					EntityId->DisplayName = NewValue;
					return true;
				});

			PropertyHandle->NotifyPostChange(EPropertyChangeType::ValueSet);
			PropertyHandle->NotifyFinishedChangingProperties();
		}
	}

	void OnFilterTextChanged(const FText& InFilterText)
	{
		const FString FilterString = InFilterText.ToString();

		if (FilterString.IsEmpty())
		{
			FilteredOptions = Options;
		}
		else
		{
			FilteredOptions = Options.FilterByPredicate([&FilterString](const FName& Option)
			{
				return Option.ToString().Contains(FilterString);
			});
		}
	}

	void OnComboBoxOpening()
	{
		FilteredOptions = Options;
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
				[&](void* RawData, const int32 DataIndex, MAYBE_UNUSED const int32 NumDatas)
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

		return NoneEntityText;
	}

	void ApplyMetadataFilters()
	{
		const FString ExcludedList = PropertyHandle->GetProperty()->GetMetaData(TEXT("Excluded"));
		const FString IncludedList = PropertyHandle->GetProperty()->GetMetaData(TEXT("Included"));

		TArray<FString> ExcludedItems;
		ExcludedList.ParseIntoArray(ExcludedItems, TEXT(","), true);

		TArray<FString> IncludedItems;
		IncludedList.ParseIntoArray(IncludedItems, TEXT(","), true);
		
		if (!IncludedItems.IsEmpty())
		{
			TArray<FName> MetaFilteredOptions;
			
			for (const FString& Item : IncludedItems)
			{
				MetaFilteredOptions.Add(FName(*Item));
			}
			
			Options = MetaFilteredOptions;
		}

		if (!ExcludedItems.IsEmpty())
		{
			for (const FString& ExcludedItem : ExcludedItems)
			{
				Options.Remove(FName(*ExcludedItem));
			}
		}
	}
	
	FReply OnClearClicked()
	{
		SelectedItem.Reset();

		if (PropertyHandle && PropertyHandle->IsValidHandle())
		{
			FScopedTransaction Transaction(NSLOCTEXT("Flecs", "ResetEntity", "Reset Entity"));
			PropertyHandle->NotifyPreChange();

			PropertyHandle->EnumerateRawData(
				[&](void* RawData, MAYBE_UNUSED const int32 DataIndex, MAYBE_UNUSED const int32 NumDatas)
				{
					if (RawData == nullptr)
					{
						return false;
					}

					FFlecsEntityHandle* EntityId = static_cast<FFlecsEntityHandle*>(RawData);
					EntityId->SetEntity(flecs::entity());
					EntityId->DisplayName = FName();
					return true;
				});

			PropertyHandle->NotifyPostChange(EPropertyChangeType::ValueSet);
			PropertyHandle->NotifyFinishedChangingProperties();
		}

		FilteredOptions = Options;

		return FReply::Handled();
	}

	TSharedRef<SWidget> OnGenerateWidgetWithSearch()
	{
		return
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(2)
			[
				SNew(SSearchBox)
				.OnTextChanged(this, &FFlecsEntityHandleCustomization::OnFilterTextChanged)
			]
			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			[
				SNew(SListView<FName>)
				.ListItemsSource(&FilteredOptions)
				.OnGenerateRow(this, &FFlecsEntityHandleCustomization::OnGenerateRow)
				.OnSelectionChanged(this, &FFlecsEntityHandleCustomization::OnEntitySelected)
			];
	}

	TSharedRef<ITableRow> OnGenerateRow(const FName InItem, const TSharedRef<STableViewBase>& OwnerTable)
	{
		return SNew(STableRow<FName>, OwnerTable)
			[
				SNew(STextBlock).Text(FText::FromName(InItem))
			];
	}
	
}; // class FFlecsEntityHandleCustomization