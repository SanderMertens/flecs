// Elie Wiese-Namir © 2025. All Rights Reserved.

// ReSharper disable CppMemberFunctionMayBeStatic
// ReSharper disable CppExpressionWithoutSideEffects
#pragma once

#include "CoreMinimal.h"
#include "DetailLayoutBuilder.h"
#include "PropertyHandle.h"
#include "IDetailChildrenBuilder.h"
#include "DetailWidgetRow.h"
#include "Entities/FlecsDefaultEntityEngineSubsystem.h"
#include "SNameComboBox.h"
#include "Widgets/Input/SComboBox.h"
#include "Worlds/FlecsWorldSubsystem.h"

#define LOCTEXT_NAMESPACE "Flecs"

class FFlecsIdCustomization final : public IPropertyTypeCustomization
{
	TSharedRef<FName> NoneEntityText = MakeShared<FName>(FName(TEXT("None")));
	
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance()
	{
		return MakeShareable(new FFlecsIdCustomization());
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
		EntityOptions.Empty();

		Options.Add(NoneEntityText);
		EntityOptions.Add(flecs::entity::null());
			
	    FFlecsDefaultEntityEngine::Get().DefaultEntityQuery.each([this](const flecs::entity InEntity)
	    {
	        Options.Add(MakeShared<FName>(FName(InEntity.name().c_str())));
    		EntityOptions.Add(InEntity);
	    });

	    ApplyMetadataFilters();
	    FilteredOptions = Options;

		SelectedItem = GetCurrentItemLabel();

	    ChildBuilder.AddCustomRow(NSLOCTEXT("Flecs", "FlecsEntityHandle", "Flecs Entity Handle"))
	    .NameContent()
	    [
	        SNew(STextBlock)
	            .Text(NSLOCTEXT("Flecs", "SelectedEntity", "Selected Entity"))
	            .Font(IDetailLayoutBuilder::GetDetailFont())
	    ]
	    .ValueContent()
		.MinDesiredWidth(250.f)
	    .MaxDesiredWidth(250.f)
	    [
	        SNew(SVerticalBox)
	        + SVerticalBox::Slot()
	            .AutoHeight()
	            [
	                SAssignNew(NameComboBox, SNameComboBox)
	                    .OptionsSource(&FilteredOptions)
	                    .OnComboBoxOpening(this, &FFlecsIdCustomization::OnComboBoxOpening)
	                    .OnSelectionChanged(this, &FFlecsIdCustomization::OnEntitySelected)
						.ComboBoxStyle(&FCoreStyle::Get().GetWidgetStyle<FComboBoxStyle>("ComboBox"))
            			.InitiallySelectedItem(SelectedItem)
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
	mutable TSharedPtr<FName> SelectedItem;
	TArray<flecs::entity> EntityOptions;
	TArray<TSharedPtr<FName>> Options;
	TArray<TSharedPtr<FName>> FilteredOptions;

	TSharedPtr<IPropertyHandle> PropertyHandle;
	TSharedPtr<SNameComboBox> NameComboBox;

	// ReSharper disable once CppMemberFunctionMayBeConst
	void OnEntitySelected(TSharedPtr<FName> NewValue, ESelectInfo::Type SelectInfo)
	{
		if UNLIKELY_IF(!NewValue.IsValid() && !SelectedItem.IsValid())
		{
			return;
		}
		
		if LIKELY_IF(PropertyHandle && PropertyHandle->IsValidHandle())
		{
			FScopedTransaction Transaction(NSLOCTEXT("Flecs", "ChangeEntity", "Change Entity"));
			PropertyHandle->NotifyPreChange();

			PropertyHandle->EnumerateRawData(
				[NewValue, this]
				(void* RawData, MAYBE_UNUSED const int32 DataIndex, MAYBE_UNUSED const int32 NumDatas)
				{
					if UNLIKELY_IF(RawData == nullptr)
					{
						return false;
					}

					if (NewValue.IsValid() && NewValue == NoneEntityText)
					{
						static_cast<FFlecsId*>(RawData)->Id = flecs::entity::null().id();
						SelectedItem.Reset();
						return true;
					}
					
					FFlecsId* EntityId = static_cast<FFlecsId*>(RawData);

					const TSharedPtr<FName> SearchValue = NewValue.IsValid() ? NewValue : SelectedItem;
					int32 FoundIndex = INDEX_NONE;
					
					for (int32 Index = 0; Index < Options.Num(); ++Index)
					{
						if (Options[Index]->IsEqual(*SearchValue))
						{
							FoundIndex = Index;
							break;
						}
					}
					
					flecs::entity Entity = EntityOptions[FoundIndex];
					check(Entity.is_valid());
					SelectedItem = NewValue;
					EntityId->Id = Entity.id();
					
					return true;
				});

			PropertyHandle->NotifyPostChange(EPropertyChangeType::ValueSet);
			PropertyHandle->NotifyFinishedChangingProperties();
		}
	}

	void OnFilterTextChanged(const FText& InFilterText)
	{
		const FString& FilterString = InFilterText.ToString();

		if (FilterString.IsEmpty())
		{
			FilteredOptions = Options;
		}
		else
		{
			FilteredOptions = Options.FilterByPredicate([&FilterString](const TSharedPtr<FName>& Option)
			{
				return Option->ToString().Contains(FilterString);
			});
		}
	}

	void OnComboBoxOpening()
	{
		FilteredOptions = Options;

		TSharedPtr<FName> SelectedName = GetCurrentItemLabel();
		if (SelectedName.IsValid())
		{
			NameComboBox->SetSelectedItem(SelectedName);
		}
	}

	TSharedRef<SWidget> GenerateEntityWidget(const TSharedPtr<FName>& InOption)
	{
		return SNew(STextBlock).Text(FText::FromName(*InOption.Get()));
	}

	TSharedPtr<FName> GetCurrentItemLabel() const
	{
		if (SelectedItem.IsValid())
		{
			return SelectedItem;
		}

		TSharedPtr<FName> CommonValue;

		if (PropertyHandle && PropertyHandle->IsValidHandle())
		{
			PropertyHandle->EnumerateRawData(
				[&CommonValue, this](void* RawData, const int32 DataIndex, MAYBE_UNUSED const int32 NumDatas)
				{
					if UNLIKELY_IF(RawData == nullptr)
					{
						CommonValue.Reset();
						return false;
					}

					const flecs::entity Entity = FFlecsDefaultEntityEngine::Get().DefaultEntityWorld->get_alive(
						static_cast<FFlecsId*>(RawData)->GetId());

					if UNLIKELY_IF(static_cast<FFlecsId*>(RawData)->GetId() == flecs::entity::null().id())
					{
						CommonValue.Reset();
						return false;
					}

					if UNLIKELY_IF(!Entity.is_valid())
					{
						CommonValue.Reset();
						return false;
					}
					
					const FName CurrentValue = FName(Entity.name().c_str());

					if (DataIndex > 0)
					{
						if (CommonValue.IsValid() && (CommonValue->GetNumber() != CurrentValue.GetNumber()))
						{
							CommonValue.Reset();
							return false;
						}
					}

					CommonValue = MakeShared<FName>(CurrentValue);
					return true;
				}
			);
		}

		if (CommonValue.IsValid())
		{
			SelectedItem = CommonValue;
			return CommonValue;
		}

		SelectedItem.Reset();
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
		
		TArray<TSharedPtr<FName>> NewOptions;
		TArray<flecs::entity> NewEntityOptions;
		
		for (int32 Index = 0; Index < Options.Num(); ++Index)
		{
			FString OptionName = Options[Index]->ToString();
			
			if (Index == 0)
			{
				NewOptions.Add(Options[Index]);
				NewEntityOptions.Add(EntityOptions[Index]);
				continue;
			}

			bool bInclude = true;
			
			if (!IncludedItems.IsEmpty())
			{
				bInclude = IncludedItems.Contains(OptionName);
			}

			if (ExcludedItems.Contains(OptionName))
			{
				bInclude = false;
			}

			if (bInclude)
			{
				NewOptions.Add(Options[Index]);
				NewEntityOptions.Add(EntityOptions[Index]);
			}
		}
		
		Options = NewOptions;
		EntityOptions = NewEntityOptions;
	}

	TSharedRef<ITableRow> OnGenerateRow(const FName InItem, const TSharedRef<STableViewBase>& OwnerTable)
	{
		return SNew(STableRow<FName>, OwnerTable)
			[
				SNew(STextBlock).Text(FText::FromName(InItem))
			];
	}
	
}; // class FFlecsIdCustomization

#undef LOCTEXT_NAMESPACE
