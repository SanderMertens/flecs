// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsIdPinFactory.h"
#include "SGraphPinFlecsId.h"
#include "Entities/FlecsEntityHandle.h"

TSharedPtr<SGraphPin> FFlecsIdPinFactory::CreatePin(UEdGraphPin* InPin) const
{
	if (InPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct &&
		InPin->PinType.PinSubCategoryObject == FFlecsId::StaticStruct())
	{
		return SNew(SGraphPinFlecsId, InPin);
	}

	return nullptr;
}
