#include "NarrativeGameMode.h"
#include "NarrativeDataHelper.h"
#include "NarrativeGameState.h"
#include "Kismet/GameplayStatics.h"

// Define weights here or in a more accessible place if needed elsewhere.
static const TMap<int32, float> PathWeights = {
	{1, 1.0f}, {2, 1.2f}, {3, 1.75f}, {4, 2.3f}, {5, 3.0f}
};


void ANarrativeGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Start the game in the Map View.
	CurrentGameState = EGameState::EMapView;
	// Call the Blueprint event to show the map UI.
	OnGameStateChanged(CurrentGameState);
}

void ANarrativeGameMode::StartCharacterSegment(const FString& CharacterName)
{
	// Get the Game State to access character data.
	ANarrativeGameState* GS = GetGameState<ANarrativeGameState>();
	if (!GS) return;

	// Set the active character name in the Game State.
	GS->ActiveCharacterName = CharacterName;
	
	// Find the current state for the requested character.
	FCharacterState* CharState = GS->CharacterStates.Find(CharacterName);
	if (!CharState) return;
	
	// Use our C++ parser to load the narrative file content.
	FNarrativeSegmentData ParsedData;
	bool bSuccess = UNarrativeDataHelper::ParseNarrativeFile(CharacterName, CharState->CurrentPathString, ParsedData);

	if (bSuccess)
	{
		// Store the parsed data in the Game State so UI can access it.
		GS->CurrentSegmentData = ParsedData;

		// Change the game state and notify Blueprints to switch UI.
		CurrentGameState = EGameState::EDialogView;
		OnGameStateChanged(CurrentGameState);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to parse narrative file for %s at path string '%s'"), *CharacterName, *CharState->CurrentPathString);
	}
}

void ANarrativeGameMode::ReturnToMapView()
{
	ANarrativeGameState* GS = GetGameState<ANarrativeGameState>();
	if (GS)
	{
		// Clear the active character name when returning to map.
		GS->ActiveCharacterName = "";
	}

	// Change the game state back and notify Blueprints.
	CurrentGameState = EGameState::EMapView;
	OnGameStateChanged(CurrentGameState);
}

void ANarrativeGameMode::ResolveTokenChallenge(const FString& CharacterName, bool bSucceeded)
{
	ANarrativeGameState* GS = GetGameState<ANarrativeGameState>();
	if (!GS) return;

	FCharacterState* CharState = GS->CharacterStates.Find(CharacterName);
	if (!CharState) return;

	// Determine which outcome data to use based on success.
	const FChallengeOutcomeData& OutcomeData = bSucceeded ? GS->CurrentSegmentData.TokenChallenge.PositiveOutcome : GS->CurrentSegmentData.TokenChallenge.NegativeOutcome;
	const FString OutcomeChoice = bSucceeded ? "P" : "N";

	// Update Character State
	if (CharState->CurrentPathString.Equals("start", ESearchCase::IgnoreCase))
	{
		CharState->CurrentPathString = OutcomeChoice;
	}
	else
	{
		CharState->CurrentPathString.Append(OutcomeChoice);
	}

	// Update Score
	const float* Weight = PathWeights.Find(GS->CurrentDay + 1); // +1 because CurrentDay starts at 0
	if (Weight)
	{
		CharState->CumulativeScore += bSucceeded ? *Weight : -(*Weight);
	}

	// Update Location
	if (!OutcomeData.NewLocation.IsNone())
	{
		CharState->CurrentLocation = OutcomeData.NewLocation;
	}

	// TODO: Display the outcome narrative text here before returning to map.
	
	// Advance time.
	GS->CurrentTimeOfDayIndex++;
	// TODO: Add logic here to check if CurrentTimeOfDayIndex has reached the end,
	// which would trigger a new day via GS->InitializeNewDay().

	// Return to map view.
	ReturnToMapView();
}