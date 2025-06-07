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
	/// Parses a narrative segment file (.txt) based on character name and path string.
	UFUNCTION(BlueprintCallable, Category = "Narrative Data")
	static bool ParseNarrativeFile(const FString& CharacterName, const FString& PathString, FNarrativeSegmentData& OutData);

	/// Loads a character's outcome_mapping.txt file into a TMap.
	UFUNCTION(BlueprintCallable, Category = "Narrative Data")
	static TMap<FString, FString> LoadOutcomeMapping(const FString& CharacterName);

	/// Loads the sun_outcome_mappings.txt file into a TMap.
	UFUNCTION(BlueprintCallable, Category = "Narrative Data")
	static TMap<FString, FWorldOutcome> LoadWorldOutcomeMapping();
	
	/// Parses a simple resolution file into an array of narrative lines.
	UFUNCTION(BlueprintCallable, Category = "Narrative Data")
	static TArray<FNarrativeLine> ParseResolutionFile(const FString& CharacterName, const FString& PathString, bool bSunIsOn);
};