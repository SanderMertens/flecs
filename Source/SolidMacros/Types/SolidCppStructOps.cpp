// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "SolidCppStructOps.h"

FSolidMoveableStructRegistry& FSolidMoveableStructRegistry::Get()
{
	static FSolidMoveableStructRegistry Instance;
	return Instance;
}

bool FSolidMoveableStructRegistry::IsStructMovable(const TSolidNotNull<const UScriptStruct*> InStruct) const
{
	return MoveableStructs.Contains(InStruct);
}

bool FSolidMoveableStructRegistry::IsStructMoveConstructible(const TSolidNotNull<const UScriptStruct*> InStruct) const
{
	return IsStructMovable(InStruct) && GetStructTypeHookInfo(InStruct).MoveConstructor != nullptr;
}

bool FSolidMoveableStructRegistry::IsStructMoveAssignable(const TSolidNotNull<const UScriptStruct*> InStruct) const
{
	return IsStructMovable(InStruct) && GetStructTypeHookInfo(InStruct).MoveAssignment != nullptr;
}

const FSolidMoveableStructRegistry::FStructTypeHookInfo& FSolidMoveableStructRegistry::GetStructTypeHookInfo(
	const TSolidNotNull<const UScriptStruct*> InStruct) const
{
	solid_checkf(IsStructMovable(InStruct), TEXT("InStruct is not registered as moveable!"));
	return MoveableStructs.FindChecked(InStruct);
}

/*
namespace Solid
{
	void MoveConstructScriptStruct(const TSolidNotNull<const UScriptStruct*> InStruct, void* Dest, void* Src,
		const uint32 Count)
	{
		solid_checkf(IsValid(InStruct), TEXT("InStruct must be valid!"));
			
		solid_cassume(Dest);
		solid_cassume(Src);

		solid_checkf(FSolidMoveableStructRegistry::Get().IsStructMovable(InStruct),
					 TEXT("InStruct is not registered as moveable!"));
			
		const FSolidMoveableStructRegistry::FMoveFunc MoveFunction =
			FSolidMoveableStructRegistry::Get().GetStructTypeHookInfo(InStruct).MoveConstructor;
				
		for (uint32 Index = 0; Index < Count; ++Index)
		{
			MoveFunction(
				reinterpret_cast<uint8*>(Dest) + Index * InStruct->GetStructureSize(),
				reinterpret_cast<uint8*>(Src) + Index * InStruct->GetStructureSize()
				);
		}
	}

	void MoveAssignScriptStruct(const TSolidNotNull<const UScriptStruct*> InStruct, void* Dest, void* Src,
		const uint32 Count)
	{
		solid_checkf(IsValid(InStruct), TEXT("InStruct must be valid!"));
		
		solid_cassume(Dest);
		solid_cassume(Src);

		solid_checkf(FSolidMoveableStructRegistry::Get().IsStructMovable(InStruct),
		             TEXT("InStruct is not registered as moveable!"));
		
		const FSolidMoveableStructRegistry::FMoveFunc MoveFunction =
			FSolidMoveableStructRegistry::Get().GetStructTypeHookInfo(InStruct).MoveAssignment;
			
		for (uint32 Index = 0; Index < Count; ++Index)
		{
			MoveFunction(
				reinterpret_cast<uint8*>(Dest) + Index * InStruct->GetStructureSize(),
				reinterpret_cast<uint8*>(Src) + Index * InStruct->GetStructureSize()
				);
		}
	}

} // namespace Solid
*/
