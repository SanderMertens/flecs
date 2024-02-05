// Solstice Games © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "FlecsWorkerIterator.generated.h"

USTRUCT()
struct FFlecsWorkerIterator
{
    GENERATED_BODY()

public:

private:
    flecs::worker_iterable<> WorkerIterator;
};
