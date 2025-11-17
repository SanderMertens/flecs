// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsIdCustomization.h"

#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "FlecsIdSelector.h"
#include "IDetailChildrenBuilder.h"
#include "Entities/FlecsDefaultEntityEngine.h"

#define LOCTEXT_NAMESPACE "FlecsIdCustomization"

TSharedRef<IPropertyTypeCustomization> FFlecsIdCustomization::MakeInstance()
{
	return MakeShareable(new FFlecsIdCustomization());
}

void FFlecsIdCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle,
	FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	PropertyHandle = StructPropertyHandle;
	
	HeaderRow.NameContent()
	[
		StructPropertyHandle->CreatePropertyNameWidget()
	];
}

void FFlecsIdCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle,
	IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	PropertyHandle = StructPropertyHandle;

	StructBuilder.AddCustomRow(LOCTEXT("FlecsEntityHandle", "Flecs Entity Handle"))
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
		SNew(SFlecsIdSelector)
		.SelectedItem(GetSelectedItem())
		.NoneEntityText(FName(TEXT("None")))
		.PropertyHandle(StructPropertyHandle)
		.OnEntitySelected(FOnFlecsEntitySelected::CreateLambda([this](const FFlecsId NewEntity)
		{
			if (PropertyHandle.IsValid() && PropertyHandle->IsValidHandle())
			{
				const FScopedTransaction Transaction(LOCTEXT("ChangeEntity", "Change Entity"));
				PropertyHandle->NotifyPreChange();

				PropertyHandle->EnumerateRawData([this, &NewEntity](void* RawData, int32 DataIndex, int32 NumDatas)
				{
					if (RawData == nullptr)
					{
						return false;
					}

					FFlecsId* EntityId = static_cast<FFlecsId*>(RawData);
					*EntityId = NewEntity;

					return true;
				});

				PropertyHandle->NotifyPostChange(EPropertyChangeType::ValueSet);
				PropertyHandle->NotifyFinishedChangingProperties();
			}
		}))
	];
}

TSharedPtr<FName> FFlecsIdCustomization::GetSelectedItem() const
{
	TSharedPtr<FName> SelectedItem;
	
	if (PropertyHandle.IsValid() && PropertyHandle->IsValidHandle())
	{
		PropertyHandle->EnumerateRawData([this, &SelectedItem](void* RawData, int32 DataIndex, int32 NumDatas)
		{
			if (RawData == nullptr)
			{
				SelectedItem.Reset();
				return false;
			}

			const FFlecsId* EntityId = static_cast<FFlecsId*>(RawData);
			const flecs::entity Entity = FFlecsDefaultEntityEngine::Get().DefaultEntityWorld->get_alive(*EntityId);

			if (*EntityId == FFlecsId::Null() || !Entity.is_valid())
			{
				SelectedItem.Reset();
				return false;
			}

			const FName CurrentValue(Entity.name().c_str());

			if (DataIndex > 0 && SelectedItem.IsValid() && (SelectedItem->GetNumber() != CurrentValue.GetNumber()))
			{
				SelectedItem.Reset();
				return false;
			}

			SelectedItem = MakeShared<FName>(CurrentValue);
			return true;
		});
	}

	return SelectedItem.IsValid() ? SelectedItem : MakeShared<FName>(FName(TEXT("None")));
}


#undef LOCTEXT_NAMESPACE
