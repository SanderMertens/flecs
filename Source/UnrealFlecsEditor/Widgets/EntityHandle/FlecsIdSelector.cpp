// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsIdSelector.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Views/STableRow.h"
#include "ScopedTransaction.h"
#include "Entities/FlecsDefaultEntityEngineSubsystem.h"

#define LOCTEXT_NAMESPACE "Flecs"

void SFlecsIdSelector::Construct(const FArguments& InArgs)
{
	SelectedItem = InArgs._SelectedItem;
	NoneEntityText = MakeShared<FName>(InArgs._NoneEntityText);
	InitialSelectedItem = InArgs._SelectedItem;

	CurrentSelectedItem = InArgs._SelectedItem.IsBound() ?
		InArgs._SelectedItem.Get() : MakeShared<FName>(InArgs._NoneEntityText);
	
	OnEntitySelected = InArgs._OnEntitySelected;
	PropertyHandle = InArgs._PropertyHandle;
	
	Options.Add(NoneEntityText);
	EntityOptions.Add(FFlecsId(flecs::entity::null().id()));
	
	FFlecsDefaultEntityEngine::Get().DefaultEntityQuery.each([this](flecs::entity Entity)
	{
		const uint64 Id = Entity.id();
		const FFlecsId CurrentEntity(Id);
		EntityOptions.Add(CurrentEntity);
		Options.Add(MakeShared<FName>(Entity.name().c_str()));
	});
	
	FilteredOptions = Options;

	if (InitialSelectedItem.Get().IsValid())
	{
		CurrentSelectedItem = InitialSelectedItem.Get();
	}
	else if (PropertyHandle.IsValid())
	{
		FString CurrentValue;
		if (PropertyHandle->GetValueAsFormattedString(CurrentValue) == FPropertyAccess::Success)
		{
			CurrentValue.RemoveFromStart(TEXT("FlecsId="));
			const uint64 Id = FCString::Strtoui64(*CurrentValue, nullptr, 10);
			const FFlecsId CurrentEntity(Id);
			for (int32 Index = 0; Index < EntityOptions.Num(); Index++)
			{
				if (EntityOptions[Index].Id == CurrentEntity.Id)
				{
					CurrentSelectedItem = Options[Index];
					break;
				}
			}
		}
	}
	
	ChildSlot
	[
		SAssignNew(ComboButton, SComboButton)
		.ContentPadding(2)
		.OnGetMenuContent(this, &SFlecsIdSelector::GetComboContent)
		.ButtonContent()
		[
			SNew(STextBlock)
			.Text(this, &SFlecsIdSelector::GetCurrentItemText)
		]
	];
}

FText SFlecsIdSelector::GetCurrentItemText() const
{
	if (SelectedItem.Get().IsValid())
	{
		return FText::FromName(*CurrentSelectedItem);
	}
	
	return FText::GetEmpty();
}

TSharedRef<SWidget> SFlecsIdSelector::GetComboContent()
{
	return SNew(SVerticalBox)
	+ SVerticalBox::Slot()
	.AutoHeight()
	.Padding(3)
	[
		SNew(SSearchBox)
		.HintText(LOCTEXT("SearchHint", "Search..."))
		.OnTextChanged(this, &SFlecsIdSelector::OnSearchTextChanged)
	]
	+ SVerticalBox::Slot()
	.AutoHeight()
	[
		SNew(SBox)
		.HeightOverride(350.f)
		.WidthOverride(150.f)
		[
			SAssignNew(ListViewWidget, SListView<TSharedPtr<FName>>)
			.ListItemsSource(&FilteredOptions)
			.OnGenerateRow(this, &SFlecsIdSelector::OnGenerateRowForList)
			.OnSelectionChanged(this, &SFlecsIdSelector::OnListSelectionChanged)
		]
	];
}

void SFlecsIdSelector::OnSearchTextChanged(const FText& InFilter)
{
	const FString& FilterString = InFilter.ToString();
	FilteredOptions.Empty();
	
	for (const TSharedPtr<FName>& Option : Options)
	{
		if(Option->ToString().Contains(FilterString))
		{
			FilteredOptions.Add(Option);
		}
	}

	if (ListViewWidget.IsValid())
	{
		ListViewWidget->RequestListRefresh();
	}
}

TSharedRef<ITableRow> SFlecsIdSelector::OnGenerateRowForList(TSharedPtr<FName> InItem,
	const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STableRow<TSharedPtr<FName>>, OwnerTable)
	[
		SNew(STextBlock)
		.Text(FText::FromName(*InItem))
	];
}

void SFlecsIdSelector::OnListSelectionChanged(TSharedPtr<FName> NewValue, ESelectInfo::Type SelectInfo)
{
	if (NewValue.IsValid())
	{
		int32 FoundIndex = INDEX_NONE;
		
		for (int32 Index = 0; Index < Options.Num(); ++Index)
		{
			if (Options[Index]->IsEqual(*NewValue))
			{
				FoundIndex = Index;
				break;
			}
		}
		
		const FFlecsId Entity
			= (FoundIndex != INDEX_NONE) ? EntityOptions[FoundIndex] : FFlecsId(flecs::entity::null().id());

		if (OnEntitySelected.IsBound())
		{
			OnEntitySelected.Execute(Entity);
		}
		
		CurrentSelectedItem = NewValue;

		if (SelectedItem.IsBound())
		{
			SelectedItem.Set(NewValue);
		}

		if (PropertyHandle.IsValid())
		{
			const FString NewValueStr = FString::Printf(TEXT("FlecsId=%llu"), Entity.Id);
			PropertyHandle->SetValueFromFormattedString(NewValueStr);
		}
		
		if (ComboButton.IsValid())
		{
			ComboButton->SetIsOpen(false);
		}
	}
}

#undef LOCTEXT_NAMESPACE
