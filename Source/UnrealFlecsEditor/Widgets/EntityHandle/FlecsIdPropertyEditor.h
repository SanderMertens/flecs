// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"
#include "PropertyHandle.h"
#include "flecs.h"

class IDetailChildrenBuilder;
class IDetailLayoutBuilder;
class SNameComboBox;

/**
 * Customizes a FlecsId property to use a drop‐down for selecting an entity.
 */
class FFlecsIdCustomization : public IPropertyTypeCustomization
{
public:
	/** Makes an instance of this customization. */
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	FFlecsIdCustomization();

	//~ Begin IPropertyTypeCustomization interface
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> InPropertyHandle,
		FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> InPropertyHandle,
		IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	//~ End IPropertyTypeCustomization interface

private:
	/** Callback for when an entity is selected from the combo box. */
	void OnEntitySelected(TSharedPtr<FName> NewValue, ESelectInfo::Type SelectInfo);

	/** Callback for when the combo box is opened. */
	void OnComboBoxOpening();

	/** Generates a widget for a given entity option. */
	TSharedRef<SWidget> GenerateEntityWidget(TSharedPtr<FName> InOption);

	/** Retrieves the currently selected item label. */
	TSharedPtr<FName> GetCurrentItemLabel() const;

	/** Applies metadata filters (Excluded/Included) to the entity options. */
	void ApplyMetadataFilters();

	void SetPropertyWithEntity(const flecs::entity& Entity);

	FText GetSelectedItemText() const;
	
	void OnSearchTextChanged(const FText& InSearchText);

private:
	// The currently selected entity name.
	TSharedPtr<FName> SelectedItem;

	// Arrays of entity display names and corresponding entity values.
	TArray<TSharedPtr<FName>> Options;
	TArray<TSharedPtr<FName>> FilteredOptions;
	TArray<flecs::entity> EntityOptions;

	// Handle to the property being customized.
	TSharedPtr<IPropertyHandle> PropertyHandle;
	
	TSharedPtr<SComboBox<TSharedPtr<FName>>> NameComboBox;

	// The text used when no entity is selected.
	TSharedRef<FName> NoneEntityText;
}; // struct FFlecsIdCustomization
