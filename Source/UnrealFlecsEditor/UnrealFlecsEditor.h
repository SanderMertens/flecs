// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FFlecsEntityHandlePinFactory;

class FUnrealFlecsEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    void RegisterExplorerMenuExtension();
    
    TSharedPtr<FFlecsEntityHandlePinFactory> FlecsEntityHandlePinFactory;
}; // class FUnrealFlecsEditorModule
