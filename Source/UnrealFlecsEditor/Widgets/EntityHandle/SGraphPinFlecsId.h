// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SGraphPin.h"
#include "Entities/FlecsId.h"

class SGraphPinFlecsId : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SGraphPinFlecsId) {}
	SLATE_END_ARGS()
	
	virtual void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj);

protected:
	virtual TSharedRef<SWidget> GetDefaultValueWidget() override;
	TSharedPtr<FName> GetSelectedName() const;

	TArray<TSharedPtr<FName>> Options;
	TArray<FFlecsId> EntityOptions;
}; // class SGraphPinFlecsId