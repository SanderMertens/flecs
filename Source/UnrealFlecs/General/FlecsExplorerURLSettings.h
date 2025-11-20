// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "SolidMacros/Macros.h"

#include "FlecsExplorerURLSettings.generated.h"

// @TODO: add data validation for this struct.

/**
 * @brief Struct representing the URL settings to connect to the Flecs Explorer.
 */
USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsEditorExplorerURL
{
	GENERATED_BODY()

public:
	FFlecsEditorExplorerURL()
	{
		AdditionalQueryParameters.Add("refresh=auto");
		AdditionalQueryParameters.Add("remote=true");
	}

	/**
	 * @brief Address used to connect to the Flecs Explorer.
	 */
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Flecs Explorer Address"))
	FString Address = TEXT("https://www.flecs.dev/explorer/");

	/**
	 * @brief Host used to connect to the Flecs Explorer. (ip or domain)
	 */
	UPROPERTY(EditAnywhere, config)
	FString Host = TEXT("localhost");

	/**
	 * @brief Port used to connect to the Flecs Explorer.
	 */
	UPROPERTY(EditAnywhere, meta = (ClampMin = "1024", UIMin = "1024", ClampMax = "65535", UIMax = "65535"))
	uint32 Port = 27750;

	/**
	 * @brief Whether to increment the port for each client instance when PIE has more than 1 instance/client.
	 */
	UPROPERTY(EditAnywhere)
	bool IncrementPortForClientInstances = true;

	/**
	 * @brief Additional query parameters to append to the Flecs Explorer URL. ex: "refresh=auto", "remote=true"
	 */
	UPROPERTY(EditAnywhere)
	TArray<FString> AdditionalQueryParameters;

	NO_DISCARD FORCEINLINE FString ToURLString(const uint16 ClientInstanceIndex = 0) const
	{
		FString URL = Address;

		if (!Host.IsEmpty())
		{
			URL.Appendf(TEXT("?host=%s"), *Host);

			URL.Appendf(TEXT(":%d"), Port + ClientInstanceIndex);
		}

		for (const FString& Param : AdditionalQueryParameters)
		{
			if (URL.Contains(TEXT("?")))
			{
				URL.Appendf(TEXT("&%s"), *Param);
			}
			else
			{
				URL.Appendf(TEXT("?%s"), *Param);
			}
		}

		return URL;
	}

}; // struct FFlecsEditorExplorerURL

