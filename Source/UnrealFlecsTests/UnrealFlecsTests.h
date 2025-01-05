// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FUnrealFlecsTestsModule : public IModuleInterface
{
public: 
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
}; // class FUnrealFlecsTestsModule
