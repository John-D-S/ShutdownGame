#include "NarrativeGameMode.h"
#include "NarrativeDataHelper.h"
#include "NarrativeGameState.h"
#include "NarrativePlayerController.h"
#include "Kismet/GameplayStatics.h"

// Define weights here or in a more accessible place if needed elsewhere.
static const TMap<int32, float> PathWeights = {
	{1, 1.0f}, {2, 1.2f}, {3, 1.75f}, {4, 2.3f}, {5, 3.0f}
};


void ANarrativeGameMode::BeginPlay()
{
	Super::BeginPlay();

	CurrentGameState = EGameState::EMapView;
	OnGameStateChanged(CurrentGameState);
}

void ANarrativeGameMode::StartCharacterSegment(const FString& CharacterName)
{
	ANarrativeGameState* GS = GetGameState<ANarrativeGameState>();
	if (!GS) return;

	GS->ActiveCharacterName = CharacterName;
	
	FCharacterState* CharState = GS->CharacterStates.Find(CharacterName);
	if (!CharState) return;
	
	FNarrativeSegmentData ParsedData;
	bool bSuccess = UNarrativeDataHelper::ParseNarrativeFile(CharacterName, CharState->CurrentPathString, ParsedData);

	if (bSuccess)
	{
		GS->CurrentSegmentData = ParsedData;
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
		GS->ActiveCharacterName = "";
	}

	const int32 TotalCharacters = 4;
	if (GS && GS->PlayedCharactersThisDay.Num() >= TotalCharacters)
	{
		CurrentGameState = EGameState::EEndOfDay;
	}
	else
	{
		CurrentGameState = EGameState::EMapView;
	}
	
	OnGameStateChanged(CurrentGameState);
}

void ANarrativeGameMode::ResolveTokenChallenge(const FString& CharacterName, bool bSucceeded)
{
	ANarrativeGameState* GS = GetGameState<ANarrativeGameState>();
	if (!GS) return;

	GS->PlayedCharactersThisDay.AddUnique(CharacterName);

	FCharacterState* CharState = GS->CharacterStates.Find(CharacterName);
	if (!CharState) return;

	const FChallengeOutcomeData& OutcomeData = bSucceeded ? GS->CurrentSegmentData.TokenChallenge.PositiveOutcome : GS->CurrentSegmentData.TokenChallenge.NegativeOutcome;
	const FString OutcomeChoice = bSucceeded ? "P" : "N";

	if (CharState->CurrentPathString.Equals("start", ESearchCase::IgnoreCase))
	{
		CharState->CurrentPathString = OutcomeChoice;
	}
	else
	{
		CharState->CurrentPathString.Append(OutcomeChoice);
	}

	const float* Weight = PathWeights.Find(GS->CurrentDay);
	if (Weight)
	{
		CharState->CumulativeScore += bSucceeded ? *Weight : -(*Weight);
	}

	if (!OutcomeData.NewLocation.IsNone())
	{
		CharState->CurrentLocation = OutcomeData.NewLocation;
	}
	
	// This function NO LONGER advances time or returns to the map.
	// It just updates the state and then notifies the UI that the choice was resolved.
	OnChoiceResolved(bSucceeded);
}

void ANarrativeGameMode::StartNewDay()
{
	ANarrativeGameState* GS = GetGameState<ANarrativeGameState>();
	if (GS)
	{
		GS->PlayedCharactersThisDay.Empty();
		GS->CurrentDay++;
		GS->CurrentTimeOfDayIndex = 0;
		
		GS->OnNewDayStarted();
	}

	ANarrativePlayerController* PC = Cast<ANarrativePlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (PC)
	{
		PC->IncrementSunRotation(120.0f);
	}
	
	CurrentGameState = EGameState::EMapView;
	OnGameStateChanged(CurrentGameState);
}