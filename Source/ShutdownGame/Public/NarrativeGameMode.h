#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NarrativeGameMode.generated.h"

// Defines the main states of the game loop, including end-game states.
UENUM(BlueprintType)
enum class EGameState : uint8
{
	EMapView					UMETA(DisplayName = "Map View"),
	EDialogView					UMETA(DisplayName = "Dialog View"),
	EEndOfDay					UMETA(DisplayName = "End of Day"),
	EShowingWorldOutcome		UMETA(DisplayName = "Showing World Outcome"),
	EDisplayingWorldOutcome		UMETA(DisplayName = "Displaying World Outcome"),
	EGameFinished				UMETA(DisplayName = "Game Finished")
};


UCLASS()
class SHUTDOWNGAME_API ANarrativeGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	/// The current state of the game loop.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Flow")
	EGameState CurrentGameState;

public:
	/// Called when the game starts.
	virtual void BeginPlay() override;

	/// Attempts to parse and start a narrative segment or resolution for the given character.
	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void StartCharacterSegment(const FString& CharacterName);

	/// Returns the game to the map view after a segment is complete.
	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void ReturnToMapView();

	/// Processes the token challenge result and updates character state.
	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void ResolveTokenChallenge(const FString& CharacterName, bool bSucceeded);
	
	/// Resets daily progress and starts the next day, or triggers the end-game sequence.
	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void StartNewDay();

	/// Transitions the view to show the full world outcome text.
	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void DisplayWorldOutcome();
	
	/// Sets up the game state for playing the final character resolutions.
	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void PrepareForResolutions();

	/// Event called in Blueprints when the game state changes, used to switch UI.
	UFUNCTION(BlueprintImplementableEvent, Category = "Game Flow")
	void OnGameStateChanged(EGameState NewState);
	
	/// Event called in Blueprints to notify the HUD that a choice has been resolved.
	UFUNCTION(BlueprintImplementableEvent, Category = "Game Flow")
	void OnChoiceResolved(bool bWasSuccess);

private:
	/// Kicks off the end-game sequence once Day 5 is complete.
	void BeginEndGameSequence();
};