// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Types/SolidNotNull.h"
#include "FlecsScopedDeferWindow.generated.h"

class UFlecsWorld;

USTRUCT()
struct UNREALFLECS_API FFlecsScopedDeferWindow
{
	GENERATED_BODY()

	UE_NONCOPYABLE(FFlecsScopedDeferWindow);

public:
	explicit FFlecsScopedDeferWindow(const TSolidNotNull<const UFlecsWorld*> InFlecsWorld);
	~FFlecsScopedDeferWindow();

private:
	UPROPERTY()
	TObjectPtr<const UFlecsWorld> FlecsWorld;
	
}; // struct FFlecsScopedDeferWindow


