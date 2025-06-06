#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "NarrativeStructs.h" // Include our structs
#include "NarrativeGameState.generated.h"

UCLASS()
class SHUTDOWNGAME_API ANarrativeGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	/// Called when the game starts or when spawned.
	virtual void BeginPlay() override;
	
	/// Exposes CurrentDay to Blueprints, read-only.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
	int32 CurrentDay = 0;

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

	/// Holds the parsed data for the currently active narrative segment. UI will read from this.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
	FNarrativeSegmentData CurrentSegmentData;

	/// This function will be callable from our Blueprints.
	UFUNCTION(BlueprintCallable, Category = "Game Logic")
	void InitializeNewDay(int32 MinTokens, int32 MaxTokens);
};