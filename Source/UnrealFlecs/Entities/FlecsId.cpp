// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsId.h"

#include "Worlds/FlecsWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsId)

FString FFlecsId::ToString() const
{
	return FString::Printf(TEXT("Index: %d, Generation: %d"), GetIndex(), GetGeneration());
}

bool FFlecsId::ImportTextItem(const TCHAR*& Buffer, int32 PortFlags, UObject* Parent, FOutputDevice* ErrorText)
{
	uint64 TempId = 0;
        
	if LIKELY_IF(FParse::Value(Buffer, TEXT("FlecsId="), TempId))
	{
		Id = TempId;
		return true;
	}

	return false;
}

bool FFlecsId::ExportTextItem(FString& ValueStr, const FFlecsId& DefaultValue, UObject* Parent, int32 PortFlags,
	UObject* ExportRootScope) const
{
	ValueStr = FString::Printf(TEXT("FlecsId=%llu"), Id);
	return true;
}
