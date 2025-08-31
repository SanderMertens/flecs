// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsComponentProperties.h"

FFlecsComponentPropertiesRegistry FFlecsComponentPropertiesRegistry::Instance;

void FFlecsComponentPropertiesRegistry::RegisterComponentProperties(const std::string& Name, UScriptStruct* Struct,
	const uint32 Size, const uint16 Alignment, const Unreal::Flecs::FFlecsComponentFunction& RegistrationFunction)
{
	solid_checkf(!Name.empty(), TEXT("Component properties name is empty!"));
		
	ComponentProperties[Name] = FFlecsComponentProperties
	{
		.Name = Name,
		.Struct = Struct,
		.Size = Size, .Alignment = Alignment,
		.RegistrationFunction = RegistrationFunction
	};
		
	OnComponentPropertiesRegistered.Broadcast(ComponentProperties[Name]);
}

bool FFlecsComponentPropertiesRegistry::ContainsComponentProperties(const std::string_view Name) const
{
	return ComponentProperties.contains(Name.data());
}

bool FFlecsComponentPropertiesRegistry::ContainsComponentProperties(const FString& Name) const
{
	return ComponentProperties.contains(Unreal::Flecs::ToCString(Name));
}

const FFlecsComponentProperties& FFlecsComponentPropertiesRegistry::GetComponentProperties(
	const std::string_view Name) const
{
	checkf(!Name.empty(), TEXT("Component properties name is empty!"));
	checkf(ComponentProperties.contains(Name.data()), TEXT("Component properties not found!"));
	
	return ComponentProperties.at(Name.data());
}

const FFlecsComponentProperties& FFlecsComponentPropertiesRegistry::GetComponentProperties(const FString& Name) const
{
	solid_checkf(!Name.IsEmpty(), TEXT("Component properties name is empty!"));
	solid_checkf(ComponentProperties.contains(Unreal::Flecs::ToCString(Name)),
	             TEXT("Component properties not found!"));
	
	return ComponentProperties.at(Unreal::Flecs::ToCString(Name));
}
