// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Entities/FlecsId.h"
#include "SGraphPin.h"

class UNREALFLECSEDITOR_API SFlecsIdGraphPin : public SGraphPin
{

public:
	SLATE_BEGIN_ARGS(SFlecsIdGraphPin)
		{
		}

	SLATE_END_ARGS()
	
	void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj);

	FFlecsId FlecsId;
};
