#include "UnrealFlecsEditor.h"
#include "UnrealFlecsEditorStyle.h"
#include "Widgets/FlecsEntityHandlePropertyEditor.h"

#define LOCTEXT_NAMESPACE "FUnrealFlecsEditorModule"

void FUnrealFlecsEditorModule::StartupModule()
{
	FUnrealFlecsEditorStyle::Initialize();
	
    UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this,
    	&FUnrealFlecsEditorModule::RegisterExplorerMenuExtension));

	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyEditorModule.RegisterCustomPropertyTypeLayout("FlecsEntityHandle",
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FFlecsEntityHandleCustomization::MakeInstance));
}

void FUnrealFlecsEditorModule::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyEditorModule.UnregisterCustomPropertyTypeLayout("FlecsEntityHandle");
	}
	
	FUnrealFlecsEditorStyle::Shutdown();
	
    UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);
}

void FUnrealFlecsEditorModule::RegisterExplorerMenuExtension()
{
	FToolMenuOwnerScoped OwnerScoped(this);

	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.ModesToolBar");

	FToolMenuSection& Section = Menu->FindOrAddSection("Content");
	
	Section.AddEntry(FToolMenuEntry::InitToolBarButton(
		"OpenFlecsExplorer",
		FUIAction(
			FExecuteAction::CreateLambda([]()
			{
				FPlatformProcess::LaunchURL(TEXT("https://flecs.dev/explorer/v4/?remote=true"), nullptr, nullptr);
			})
		),
		INVTEXT("Open Flecs Explorer"),
		INVTEXT("Open Flecs Explorer"),
		FSlateIcon("UnrealFlecsEditorStyle", "UnrealFlecs.FlecsEditor.FlecsLogo")
	));
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FUnrealFlecsEditorModule, UnrealFlecsEditor)