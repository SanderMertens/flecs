#include "UnrealFlecsEditor.h"
#include "UnrealFlecsEditorStyle.h"

#define LOCTEXT_NAMESPACE "FUnrealFlecsEditorModule"

void FUnrealFlecsEditorModule::StartupModule()
{
	FUnrealFlecsEditorStyle::Initialize();
	
    UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this,
    	&FUnrealFlecsEditorModule::RegisterExplorerMenuExtension));
}

void FUnrealFlecsEditorModule::ShutdownModule()
{
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
			FExecuteAction::CreateLambda([]() {
				FPlatformProcess::LaunchURL(TEXT("https://flecs.dev/explorer?remote=true"), nullptr, nullptr);
			})
		),
		INVTEXT("Open Flecs Explorer"),
		INVTEXT("Open Flecs Explorer"),
		FSlateIcon("UnrealFlecsEditorStyle", "UnrealFlecs.FlecsEditor.FlecsLogo")
	));
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FUnrealFlecsEditorModule, UnrealFlecsEditor)