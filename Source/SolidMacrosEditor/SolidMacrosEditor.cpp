#include "SolidMacrosEditor.h"
#include "Types/SolidEnumSelectorDetails.h"

#define LOCTEXT_NAMESPACE "FSolidMacrosEditorModule"

void FSolidMacrosEditorModule::StartupModule()
{
    RegisterDetailsCustomizations();
}

void FSolidMacrosEditorModule::ShutdownModule()
{
    UnregisterDetailsCustomizations();
}

void FSolidMacrosEditorModule::RegisterDetailsCustomizations()
{
	FPropertyEditorModule& PropertyEditor = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyEditor.RegisterCustomPropertyTypeLayout("SolidEnumSelector",
	 FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FSolidEnumSelectorDetails::MakeInstance));
}

void FSolidMacrosEditorModule::UnregisterDetailsCustomizations()
{
	FPropertyEditorModule& PropertyEditor = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyEditor.UnregisterCustomPropertyTypeLayout("SolidEnumSelector");
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FSolidMacrosEditorModule, SolidMacrosEditor)