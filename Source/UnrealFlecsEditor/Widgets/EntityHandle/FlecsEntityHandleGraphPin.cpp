// Fill out your copyright notice in the Description page of Project Settings.

#include "FlecsEntityHandleGraphPin.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Views/SListView.h"
#include "ScopedTransaction.h"
#include "EdGraphSchema_K2.h"
#include "Engine/World.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "SolidMacros/Macros.h"

void FFlecsEntityHandleGraphPin::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
    this->SetCursor(EMouseCursor::Default);

    bShowLabel = true;

    SGraphPin::Construct(SGraphPin::FArguments(), InPin);

    PopulateEntityOptions();
}

void FFlecsEntityHandleGraphPin::PopulateEntityOptions()
{
    Options.Empty();

    // Set the initial selected item based on the current pin value
    const FString DefaultValue = GraphPinObj->GetDefaultAsString();
    const FName DefaultEntityName = *DefaultValue;

    for (const TSharedPtr<FName>& Option : Options)
    {
        if (*Option == DefaultEntityName)
        {
            SelectedItem = Option;
            break;
        }
    }

    FilteredOptions = Options;
}

TSharedRef<SWidget> FFlecsEntityHandleGraphPin::GetDefaultValueWidget()
{
    return SAssignNew(ComboButton, SComboButton)
        .OnGetMenuContent(this, &FFlecsEntityHandleGraphPin::OnGenerateEntityPicker)
        .ContentPadding(2)
        .ButtonContent()
        [
            SNew(STextBlock)
            .Text(this, &FFlecsEntityHandleGraphPin::GetSelectedEntityName)
        ];
}

FText FFlecsEntityHandleGraphPin::GetSelectedEntityName() const
{
    if (SelectedItem.IsValid())
    {
        return FText::FromName(*SelectedItem);
    }

    return FText::FromString(TEXT("Select Entity"));
}

void FFlecsEntityHandleGraphPin::OnEntitySelected(TSharedPtr<FName> NewValue, MAYBE_UNUSED ESelectInfo::Type SelectInfo)
{
    if (NewValue.IsValid())
    {
        SelectedItem = NewValue;

        const FString NewDefaultValue = NewValue->ToString();

        if (GraphPinObj->GetDefaultAsString() != NewDefaultValue)
        {
            const FScopedTransaction Transaction(NSLOCTEXT("GraphEditor", "ChangeFlecsEntityHandlePinValue", "Change Flecs Entity Handle Pin Value"));
            GraphPinObj->Modify();
            
            GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, NewDefaultValue);
            
            FBlueprintEditorUtils::MarkBlueprintAsModified(
                FBlueprintEditorUtils::FindBlueprintForNode(GraphPinObj->GetOwningNode()));
        }

        // Close the combo menu
        ComboButton->SetIsOpen(false);
    }
}

void FFlecsEntityHandleGraphPin::OnFilterTextChanged(const FText& InFilterText)
{
    const FString FilterString = InFilterText.ToString();

    if (FilterString.IsEmpty())
    {
        FilteredOptions = Options;
    }
    else
    {
        FilteredOptions.Empty();

        for (const TSharedPtr<FName>& Option : Options)
        {
            if (Option->ToString().Contains(FilterString))
            {
                FilteredOptions.Add(Option);
            }
        }
    }

    if (ListViewWidget.IsValid())
    {
        ListViewWidget->RequestListRefresh();
    }
}

TSharedRef<SWidget> FFlecsEntityHandleGraphPin::OnGenerateEntityPicker()
{
    SAssignNew(SearchBox, SSearchBox)
        .OnTextChanged(this, &FFlecsEntityHandleGraphPin::OnFilterTextChanged);

    SAssignNew(ListViewWidget, SListView<TSharedPtr<FName>>)
        .ListItemsSource(&FilteredOptions)
        .OnGenerateRow_Lambda([](TSharedPtr<FName> InItem, const TSharedRef<STableViewBase>& OwnerTable)
        {
            return SNew(STableRow<TSharedPtr<FName>>, OwnerTable)
                [
                    SNew(STextBlock).Text(FText::FromName(*InItem))
                ];
        })
        .OnSelectionChanged(this, &FFlecsEntityHandleGraphPin::OnEntitySelected);

    // Pre-select the currently selected item
    if (SelectedItem.IsValid())
    {
        ListViewWidget->SetSelection(SelectedItem);
    }

    return SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(2)
        [
            SearchBox.ToSharedRef()
        ]
        + SVerticalBox::Slot()
        .MaxHeight(200.0f)
        [
            ListViewWidget.ToSharedRef()
        ];
}
