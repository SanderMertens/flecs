// Copyright 2022 Guganana. All Rights Reserved.
#pragma once

#include <DrawDebugHelpers.h>

// ------------------------------------------------------------------------------------
// Debug Visualization ( Early prototype )
// ------------------------------------------------------------------------------------

namespace Viz
{
    // TODO check for shipping since drawdebug isn't available on shipping
    // #define UNDEBUG( Type, ... ) Unlog::Debug< Viz::Type >( this, __VA_ARGS__ )

//    template< typename TVisualizer, typename... TArgs>
//    static void Debug(UObject* Owner, /*FName Name, */ TArgs... Args)
//    {
//#if UNLOG_ENABLED
//        TVisualizer::Display(Owner, Args...);
//#endif // UNLOG_ENABLED
//    }

    FORCEINLINE void DrawLabel(UWorld* World, const FVector& Position, const FString& Label, const FString& Value)
    {
        auto CategoryName = LogGeneral::Static().GetName();

        const FString& Message = FString::Format(
            TEXT("| Category: {0}\n")
            TEXT("| Value: {1}\n")
            TEXT("| {2}"),
            { CategoryName.ToString(), Value, Label }
        );

        DrawDebugString(World, Position, Message, nullptr, FColor::White, -1.000000, false, 1.f);
    }

    struct Location
    {
        FORCEINLINE static void Display(UObject* Owner, const FVector& Position, const FColor& Color = FColor::Red, float Radius = 10.f)
        {
            DrawDebugSphere(Owner->GetWorld(), Position, Radius, 12, Color, false, -1.f, SDPG_World, 2.f);
            DrawLabel(Owner->GetWorld(), Position, TEXT("Test position"), Position.ToString());
            //FVisualLogger::GeometryShapeLogf(Owner, "Test", ELogVerbosity::Log, Position, Radius, Color, TEXT(""));//, ##__VA_ARGS__)
        }
    };

    struct Direction
    {
        FORCEINLINE static void Display(UObject* Owner, const FVector& Start, const FVector& Direction, const FColor& Color = FColor::Cyan)
        {
            DrawDebugDirectionalArrow(Owner->GetWorld(), Start, Start + (Direction.GetSafeNormal() * 100.f), 10.f, Color, true, -1.f, SDPG_World, 2.f);
            DrawLabel(Owner->GetWorld(), Direction, TEXT("Test direction"), Direction.ToString());
            //FVisualLogger::ArrowLogf(Owner, "Test", ELogVerbosity::Log, Start, Start + Direction, Color, TEXT(""));//, ##__VA_ARGS__)
        }
    };
};
