#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FUnrealFlecsTestsModule : public IModuleInterface
{
public:ERemoteFunctionSendPolicy 
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
