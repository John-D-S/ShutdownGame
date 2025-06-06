#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NarrativeGameMode.generated.h"

// Defines the main states of the game loop.
UENUM(BlueprintType)
enum class EGameState : uint8
{
	EMapView		UMETA(DisplayName = "Map View"),
	EDialogView		UMETA(DisplayName = "Dialog View"),
	EEndOfDay		UMETA(DisplayName = "End of Day"),
	EResolutionView	UMETA(DisplayName = "Resolution View")
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

	/// Attempts to parse and start a narrative segment for the given character.
	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void StartCharacterSegment(const FString& CharacterName);

	/// Returns the game to the map view after a segment is complete.
	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void ReturnToMapView();

	/// Processes the token challenge result, updates character state, and advances time.
	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void ResolveTokenChallenge(const FString& CharacterName, bool bSucceeded);
	
	/// Resets daily progress and starts the next day.
	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void StartNewDay();

	/// Event called in Blueprints when the game state changes, used to switch UI.
	UFUNCTION(BlueprintImplementableEvent, Category = "Game Flow")
	void OnGameStateChanged(EGameState NewState);
};