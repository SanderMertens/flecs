// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Types/SolidNotNull.h"

#include "FlecsQueryExpression.h"
#include "Queries/Enums/FlecsQueryInOut.h"

#include "FlecsExpressionInOut.generated.h"

USTRUCT(BlueprintType, meta = (DisplayName = "In/Out Expression"))
struct UNREALFLECS_API FFlecsExpressionInOut final : public FFlecsQueryExpression
{
	GENERATED_BODY()

public:
	FFlecsExpressionInOut();
	
	virtual void Apply(TSolidNotNull<UFlecsWorld*> InWorld, flecs::query_builder<>& InQueryBuilder) const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Query")
	EFlecsQueryInOut InOut = EFlecsQueryInOut::Default;

	/** Set read/write access for stage. Use this when a system reads or writes
	 * components other than the ones provided by the query. This information 
	 * can be used by schedulers to insert sync/merge points between systems
	 * where deferred operations are flushed.
	 * 
	 * Setting this is optional. If not set, the value of the accessed component
	 * may be out of sync for at most one frame.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Query", AdvancedDisplay)
	bool bStage = true;
	
}; // struct FFlecsExpressionInOut
