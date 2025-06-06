#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "NarrativeStructs.h"
#include "NarrativeGameState.generated.h"

UCLASS()
class SHUTDOWNGAME_API ANarrativeGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	/// Called when the game starts or when spawned.
	virtual void BeginPlay() override;
	
	/// Exposes CurrentDay to Blueprints, read-only. Starts at Day 1.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
	int32 CurrentDay = 1;

	/// Exposes PlayerTokens to Blueprints for reading and writing.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Game State")
	int32 PlayerTokens = 0;

	/// An array of text for the times of day.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game State")
	TArray<FText> TimesOfDay;

	/// The current index into the TimesOfDay array.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
	int32 CurrentTimeOfDayIndex = 0;

	/// Stores the state for each playable character, mapped by their name.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
	TMap<FString, FCharacterState> CharacterStates;
	
	/// The name of the character whose segment is currently being played.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Flow")
	FString ActiveCharacterName;

	/// Holds the parsed data for the currently active narrative segment. UI will read from this.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
	FNarrativeSegmentData CurrentSegmentData;

	/// A list of character names that have been played during the current day.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
	TArray<FString> PlayedCharactersThisDay;

	/// A map of character names to their final G/N/B outcome label. Populated at the end of Day 5.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State|End Game")
	TMap<FString, FString> FinalCharacterOutcomes;
	
	/// The final state of the sun. Set at the end of Day 5.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State|End Game")
	bool bSunWasSaved = false;

	/// The final world outcome explanation text. Set at the end of Day 5.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State|End Game")
	FString WorldOutcomeExplanation;

	/// A flag to indicate the game is in the final resolution phase.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Flow")
	bool bIsInResolutionPhase = false;

	/// Event called from C++ when a new day starts, implemented in the Blueprint child.
	UFUNCTION(BlueprintImplementableEvent, Category = "Game Logic")
	void OnNewDayStarted();
};