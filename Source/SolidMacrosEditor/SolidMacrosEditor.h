#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FSolidMacrosEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    void RegisterDetailsCustomizations();
    void UnregisterDetailsCustomizations();
};
