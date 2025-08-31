// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsSystemInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsSystemInterface)

void IFlecsSystemInterface::InitializeSystem_Internal(const flecs::world& InWorld)
{
	const FFlecsSystemSettingsInfo& Settings = GetSystemSettings();
		
	flecs::system_builder Builder(InWorld, StringCast<char>(*Settings.Name).Get());

	if (Settings.Kind != FFlecsEntityHandle::GetNullHandle())
	{
		Builder.kind(Settings.Kind);
	}

	switch (Settings.TimerKind)
	{
		case EFlecsSystemTimerKind::None:
			break;
		case EFlecsSystemTimerKind::Interval:
			Builder.interval(Settings.Interval);
			break;
		case EFlecsSystemTimerKind::Rate:
			Builder.rate(Settings.Rate);
			break;
	};

	if (Settings.TickSource != FFlecsEntityHandle::GetNullHandle())
	{
		Builder.tick_source(Settings.TickSource);
	}

	Builder.priority(Settings.Priority);
		
	BuildSystem(Builder);

	System = FFlecsSystem(Builder); // Builder.build();
		
	Settings.SystemRecord.ApplyRecordToEntity(System.GetEntity());
	//@TODO: Add FlecsSystemObject Component Target Type
	//System.GetEntity().Set<FFlecsUObjectComponent>({ _getUObject() });
		
	InitializeSystem();
	BP_InitializeSystem();
}

FFlecsEntityHandle IFlecsSystemInterface::GetEntityHandle() const
{
	return System.GetEntity();
}
