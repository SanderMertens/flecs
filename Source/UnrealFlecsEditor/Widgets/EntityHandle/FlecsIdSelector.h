// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Entities/FlecsId.h"
#include "PropertyHandle.h" // Provides property handle support
#include "SNameComboBox.h"

DECLARE_DELEGATE_OneParam(FOnFlecsEntitySelected, const FFlecsId);

class SFlecsIdSelector : public SNameComboBox
{
public:
	SLATE_BEGIN_ARGS(SFlecsIdSelector)
		: _SelectedItem(nullptr)
		, _NoneEntityText(FName(TEXT("None")))
		, _PropertyHandle(nullptr)
	{ }
	SLATE_ATTRIBUTE(TSharedPtr<FName>, SelectedItem)
	SLATE_ATTRIBUTE(TSharedPtr<FName>, InitialSelectedItem)
	SLATE_ARGUMENT(FName, NoneEntityText)
	SLATE_ARGUMENT(TSharedPtr<IPropertyHandle>, PropertyHandle)
	SLATE_EVENT(FOnFlecsEntitySelected, OnEntitySelected)
	SLATE_END_ARGS()

	/** Constructs this widget */
	void Construct(const FArguments& InArgs);

private:

	/** Returns the text for the currently selected item to display on the combo button */
	FText GetCurrentItemText() const;

	/** Creates the drop-down content with a search bar and a list view */
	TSharedRef<SWidget> GetComboContent();

	/** Called when the search text changes to filter options */
	void OnSearchTextChanged(const FText& InFilter);

	/** Generates a row widget for each item in the list view */
	TSharedRef<ITableRow> OnGenerateRowForList(TSharedPtr<FName> InItem,
		const TSharedRef<STableViewBase>& OwnerTable);

	/** Called when an item in the list view is selected */
	void OnListSelectionChanged(TSharedPtr<FName> NewValue, ESelectInfo::Type SelectInfo);

	TSharedPtr<FName> CurrentSelectedItem;
	TArray<TSharedPtr<FName>> Options;
	TArray<TSharedPtr<FName>> FilteredOptions;
	TArray<FFlecsId> EntityOptions;
	TAttribute<TSharedPtr<FName>> SelectedItem;
	TAttribute<TSharedPtr<FName>> InitialSelectedItem;
	TSharedPtr<FName> NoneEntityText;
	FOnFlecsEntitySelected OnEntitySelected;
	TSharedPtr<IPropertyHandle> PropertyHandle;
	TSharedPtr<SComboButton> ComboButton;
	TSharedPtr<SListView<TSharedPtr<FName>>> ListViewWidget;
}; // class SFlecsIdSelector
