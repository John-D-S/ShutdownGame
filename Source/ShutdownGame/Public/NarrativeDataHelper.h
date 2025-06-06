#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NarrativeStructs.h"
#include "NarrativeDataHelper.generated.h"

UCLASS()
class SHUTDOWNGAME_API UNarrativeDataHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	/// Parses a narrative .txt file based on character name and path string (e.g., "P", "PN", "PNP", or "start").
	UFUNCTION(BlueprintCallable, Category = "Narrative Data")
	static bool ParseNarrativeFile(const FString& CharacterName, const FString& PathString, FNarrativeSegmentData& OutData);
};