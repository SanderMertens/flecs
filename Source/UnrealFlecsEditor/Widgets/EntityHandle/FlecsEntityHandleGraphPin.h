// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SGraphPin.h"

class FFlecsEntityHandleGraphPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(FFlecsEntityHandleGraphPin) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin);

protected:
	// SGraphPin interface
	virtual TSharedRef<SWidget>	GetDefaultValueWidget() override;

private:
	// Helper methods
	void OnEntitySelected(TSharedPtr<FName> NewValue, ESelectInfo::Type SelectInfo);
	FText GetSelectedEntityName() const;
	TSharedRef<SWidget> OnGenerateEntityPicker();
	void OnFilterTextChanged(const FText& InFilterText);
	void PopulateEntityOptions();

	// Variables
	TArray<TSharedPtr<FName>> Options;
	TArray<TSharedPtr<FName>> FilteredOptions;
	TSharedPtr<FName> SelectedItem;

	TSharedPtr<SComboButton> ComboButton;
	TSharedPtr<SSearchBox> SearchBox;
	TSharedPtr<SListView<TSharedPtr<FName>>> ListViewWidget;
}; // class FFlecsEntityHandleGraphPin