// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FFlecsIdPinFactory;

class FUnrealFlecsEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    void RegisterExplorerMenuExtension();
    void AddPrimaryAssetTypes();
    
    TSharedPtr<FFlecsIdPinFactory> FlecsIdPinFactory;
}; // class FUnrealFlecsEditorModule
