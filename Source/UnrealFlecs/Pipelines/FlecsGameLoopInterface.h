// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Types/SolidNotNull.h"

#include "Modules/FlecsModuleInterface.h"

#include "FlecsGameLoopInterface.generated.h"

class UFlecsWorld;

// This class does not need to be modified.
UINTERFACE(meta = (CannotImplementInterfaceInBlueprint))
class UNREALFLECS_API UFlecsGameLoopInterface : public UFlecsModuleInterface
{
	GENERATED_BODY()
}; // class UFlecsGameLoopInterface

class UNREALFLECS_API IFlecsGameLoopInterface : public IFlecsModuleInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void InitializeModule(TSolidNotNull<UFlecsWorld*> InWorld, const FFlecsEntityHandle& InModuleEntity) override;
	
	virtual void InitializeGameLoop(TSolidNotNull<UFlecsWorld*> InWorld, const FFlecsEntityHandle& InGameLoopEntity)
		PURE_VIRTUAL(IFlecsGameLoopInterface::InitializeGameLoop,);
	
	virtual bool Progress(double DeltaTime, TSolidNotNull<UFlecsWorld*> InWorld)
		PURE_VIRTUAL(IFlecsGameLoopInterface::Progress, return false;)

	virtual bool IsMainLoop() const;
	
}; // class IFlecsGameLoopInterface
