// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsIdPropertyEditor.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "ScopedTransaction.h"
#include "Entities/FlecsDefaultEntityEngineSubsystem.h"
#include "Widgets/Input/SSearchBox.h"
#include "Worlds/FlecsWorldSubsystem.h"

#define LOCTEXT_NAMESPACE "Flecs"

TSharedRef<IPropertyTypeCustomization> FFlecsIdCustomization::MakeInstance()
{
	return MakeShareable(new FFlecsIdCustomization());
}

FFlecsIdCustomization::FFlecsIdCustomization()
	: NoneEntityText(MakeShared<FName>(FName(TEXT("None"))))
{
}

void FFlecsIdCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> InPropertyHandle,
	FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
}

void FFlecsIdCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> InPropertyHandle,
	IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	PropertyHandle = InPropertyHandle;

	// Clear previous options.
	Options.Empty();
	FilteredOptions.Empty();
	EntityOptions.Empty();
	
	Options.Add(NoneEntityText);
	EntityOptions.Add(flecs::entity::null());

	// Populate the options from the default entity query.
	FFlecsDefaultEntityEngine::Get().DefaultEntityQuery.each([this](const flecs::entity InEntity)
	{
		Options.Add(MakeShared<FName>(FName(InEntity.name().c_str())));
		EntityOptions.Add(InEntity);
	});
	
	ApplyMetadataFilters();
	FilteredOptions = Options;
	
	SelectedItem = GetCurrentItemLabel();

	// Create the custom row with a drop-down box.
	ChildBuilder.AddCustomRow(LOCTEXT("FlecsEntityHandle", "Flecs Entity Handle"))
	.NameContent()
	[
		SNew(STextBlock)
		.Text(LOCTEXT("SelectedEntity", "Selected Entity"))
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
			SAssignNew(NameComboBox, SComboBox<TSharedPtr<FName>>)
			.OptionsSource(&FilteredOptions)
			.OnComboBoxOpening(this, &FFlecsIdCustomization::OnComboBoxOpening)
			.OnSelectionChanged(this, &FFlecsIdCustomization::OnEntitySelected)
			.ComboBoxStyle(&FCoreStyle::Get().GetWidgetStyle<FComboBoxStyle>("ComboBox"))
			.InitiallySelectedItem(SelectedItem)
			.OnGenerateWidget(this, &FFlecsIdCustomization::GenerateEntityWidget)
			.HasDownArrow(true)
			.ContentPadding(2)
			[
				SNew(STextBlock)
				.Text(this, &FFlecsIdCustomization::GetSelectedItemText)
			]
		]
	];

	// In case there are no options, display a simple message.
	if (Options.IsEmpty())
	{
		ChildBuilder.AddCustomRow(LOCTEXT("NoEntitiesFound", "No entities found."))
		.NameContent()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("NoEntitiesFound", "No entities found."))
			.Font(IDetailLayoutBuilder::GetDetailFont())
		];
	}
}

FText FFlecsIdCustomization::GetSelectedItemText() const
{
	TSharedPtr<FName> CurrentItem = GetCurrentItemLabel();
	return CurrentItem.IsValid() ? FText::FromName(*CurrentItem) : LOCTEXT("None", "None");
}

void FFlecsIdCustomization::OnSearchTextChanged(const FText& InSearchText)
{
	const FString SearchString = InSearchText.ToString();
	FilteredOptions.Empty();
	
	if (SearchString.IsEmpty())
	{
		FilteredOptions.Append(Options);
	}
	else
	{
		for (const TSharedPtr<FName>& Option : Options)
		{
			if (Option->ToString().Contains(SearchString))
			{
				FilteredOptions.Add(Option);
			}
		}
	}
	
	if (NameComboBox.IsValid())
	{
		NameComboBox->RefreshOptions();
	}
}

void FFlecsIdCustomization::OnEntitySelected(TSharedPtr<FName> NewValue, ESelectInfo::Type SelectInfo)
{
	if (NewValue.IsValid())
	{
		// Look up the entity corresponding to the selected name.
		int32 FoundIndex = INDEX_NONE;
		
		for (int32 Index = 0; Index < Options.Num(); ++Index)
		{
			if (Options[Index]->IsEqual(*NewValue))
			{
				FoundIndex = Index;
				break;
			}
		}

		const flecs::entity Entity = (FoundIndex != INDEX_NONE) ? EntityOptions[FoundIndex] : flecs::entity::null();
		SetPropertyWithEntity(Entity);
		SelectedItem = NewValue;
	}
}

void FFlecsIdCustomization::OnComboBoxOpening()
{
	FilteredOptions = Options;

	if (NameComboBox.IsValid())
	{
		NameComboBox->RefreshOptions();
	}
}

TSharedPtr<FName> FFlecsIdCustomization::GetCurrentItemLabel() const
{
	TSharedPtr<FName> CommonValue;

	if (PropertyHandle.IsValid() && PropertyHandle->IsValidHandle())
	{
		PropertyHandle->EnumerateRawData([&CommonValue, this](void* RawData, int32 DataIndex, int32 NumDatas)
		{
			if (RawData == nullptr)
			{
				CommonValue.Reset();
				return false;
			}

			const FFlecsId* FlecsId = static_cast<FFlecsId*>(RawData);
			const flecs::entity Entity = FFlecsDefaultEntityEngine::Get().DefaultEntityWorld->get_alive(FlecsId->GetId());

			// If the ID is "None" or the entity is invalid, then return "None".
			if (FlecsId->GetId() == flecs::entity::null().id() || !Entity.is_valid())
			{
				CommonValue.Reset();
				return false;
			}

			const FName CurrentValue(Entity.name().c_str());
			// If multiple objects are edited, ensure the values agree.
			if (DataIndex > 0 && CommonValue.IsValid() && (CommonValue->GetNumber() != CurrentValue.GetNumber()))
			{
				CommonValue.Reset();
				return false;
			}

			CommonValue = MakeShared<FName>(CurrentValue);
			return true;
		});
	}

	return CommonValue.IsValid() ? CommonValue : NoneEntityText;
}

void FFlecsIdCustomization::ApplyMetadataFilters()
{
	FString ExcludedList = PropertyHandle->GetProperty()->GetMetaData(TEXT("Excluded"));
	FString IncludedList = PropertyHandle->GetProperty()->GetMetaData(TEXT("Included"));

	TArray<FString> ExcludedItems;
	ExcludedList.ParseIntoArray(ExcludedItems, TEXT(","), true);

	TArray<FString> IncludedItems;
	IncludedList.ParseIntoArray(IncludedItems, TEXT(","), true);

	TArray<TSharedPtr<FName>> NewOptions;
	TArray<flecs::entity> NewEntityOptions;

	// Always include the first ("None") option.
	for (int32 Index = 0; Index < Options.Num(); ++Index)
	{
		if (Index == 0)
		{
			NewOptions.Add(Options[Index]);
			NewEntityOptions.Add(EntityOptions[Index]);
			continue;
		}

		const FString OptionName = Options[Index]->ToString();
		bool bInclude = true;

		if (IncludedItems.Num() > 0)
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

void FFlecsIdCustomization::SetPropertyWithEntity(const flecs::entity& Entity)
{
	if (PropertyHandle.IsValid() && PropertyHandle->IsValidHandle())
	{
		const FScopedTransaction Transaction(LOCTEXT("ChangeEntity", "Change Entity"));
		PropertyHandle->NotifyPreChange();

		PropertyHandle->EnumerateRawData([Entity](void* RawData, int32 DataIndex, int32 NumDatas)
		{
			if (RawData)
			{
				FFlecsId* FlecsId = static_cast<FFlecsId*>(RawData);
				FlecsId->Id = Entity.id();
			}
			
			return true;
		});

		PropertyHandle->NotifyPostChange(EPropertyChangeType::ValueSet);
		PropertyHandle->NotifyFinishedChangingProperties();
	}
}

TSharedRef<SWidget> FFlecsIdCustomization::GenerateEntityWidget(TSharedPtr<FName> InOption)
{
	return SNew(STextBlock).Text(FText::FromName(*InOption));
}

#undef LOCTEXT_NAMESPACE
