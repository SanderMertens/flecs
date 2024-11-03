// Fill out your copyright notice in the Description page of Project Settings.

#include "FlecsEntityHandlePinFactory.h"
#include "FlecsEntityHandleGraphPin.h"
#include "Entities/FlecsEntityHandle.h"

TSharedPtr<SGraphPin> FFlecsEntityHandlePinFactory::CreatePin(UEdGraphPin* InPin) const
{
	if (InPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct &&
		InPin->PinType.PinSubCategoryObject == FFlecsEntityHandle::StaticStruct())
	{
		return SNew(FFlecsEntityHandleGraphPin, InPin);
	}

	return nullptr;
}
