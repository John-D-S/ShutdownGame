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
	bool bSuccess = false;

	// Check if the game is in the final resolution phase.
	if (GS->bIsInResolutionPhase)
	{
		// If yes, parse the character's specific resolution file.
		TArray<FNarrativeLine> ResolutionLines = UNarrativeDataHelper::ParseResolutionFile(CharacterName, CharState->CurrentPathString, GS->bSunWasSaved);
		ParsedData.InitialNarrativeLines = ResolutionLines;
		bSuccess = ResolutionLines.Num() > 0;
	}
	else
	{
		// If no, do the normal gameplay: parse a segment file with a challenge.
		bSuccess = UNarrativeDataHelper::ParseNarrativeFile(CharacterName, CharState->CurrentPathString, ParsedData);
	}
	

	if (bSuccess)
	{
		GS->CurrentSegmentData = ParsedData;
		CurrentGameState = EGameState::EDialogView;
		OnGameStateChanged(CurrentGameState);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to parse file for %s at path string '%s'"), *CharacterName, *CharState->CurrentPathString);
	}
}

void ANarrativeGameMode::ReturnToMapView()
{
	ANarrativeGameState* GS = GetGameState<ANarrativeGameState>();
	if (!GS) return;

	// Add the character that was just being played to the completed list.
	if (!GS->ActiveCharacterName.IsEmpty())
	{
		GS->PlayedCharactersThisDay.AddUnique(GS->ActiveCharacterName);
	}

	// In normal gameplay phase, advance time. Time does not advance during resolutions.
	if (!GS->bIsInResolutionPhase)
	{
		GS->CurrentTimeOfDayIndex = GS->PlayedCharactersThisDay.Num();
		
		ANarrativePlayerController* PC = Cast<ANarrativePlayerController>(UGameplayStatics::GetPlayerController(this, 0));
		if (PC)
		{
			PC->IncrementSunRotation(60.0f);
		}
	}
	
	GS->ActiveCharacterName = "";
	
	const int32 TotalCharacters = 4;
	if (GS->PlayedCharactersThisDay.Num() >= TotalCharacters)
	{
		if (GS->bIsInResolutionPhase)
		{
			CurrentGameState = EGameState::EGameFinished;
		}
		else
		{
			CurrentGameState = EGameState::EEndOfDay;
		}
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

	// This function no longer tracks played characters; ReturnToMapView does.

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
	
	OnChoiceResolved(bSucceeded);
}

void ANarrativeGameMode::StartNewDay()
{
	ANarrativeGameState* GS = GetGameState<ANarrativeGameState>();
	if (!GS) return;

	const int32 FinalDay = 5;
	if (GS->CurrentDay >= FinalDay)
	{
		BeginEndGameSequence();
		return;
	}

	GS->PlayedCharactersThisDay.Empty();
	GS->CurrentDay++;
	GS->CurrentTimeOfDayIndex = 0;
	GS->OnNewDayStarted();

	ANarrativePlayerController* PC = Cast<ANarrativePlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (PC)
	{
		PC->IncrementSunRotation(120.0f);
	}
	
	CurrentGameState = EGameState::EMapView;
	OnGameStateChanged(CurrentGameState);
}

void ANarrativeGameMode::DisplayWorldOutcome()
{
	CurrentGameState = EGameState::EDisplayingWorldOutcome;
	OnGameStateChanged(CurrentGameState);
}

void ANarrativeGameMode::PrepareForResolutions()
{
	ANarrativeGameState* GS = GetGameState<ANarrativeGameState>();
	if(GS)
	{
		GS->bIsInResolutionPhase = true;
		GS->PlayedCharactersThisDay.Empty();
	}
	
	CurrentGameState = EGameState::EMapView;
	OnGameStateChanged(CurrentGameState);
}

void ANarrativeGameMode::BeginEndGameSequence()
{
	ANarrativeGameState* GS = GetGameState<ANarrativeGameState>();
	if (!GS) return;

	ANarrativePlayerController* PC = Cast<ANarrativePlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (PC)
	{
		PC->IncrementSunRotation(180.0f);
	}
	
	TArray<FString> CharacterNames = {"Crystal", "Shae", "Dax", "Trent"};
	FString WorldOutcomeKey;

	for (int32 i = 0; i < CharacterNames.Num(); ++i)
	{
		const FString& Name = CharacterNames[i];
		FCharacterState* CharState = GS->CharacterStates.Find(Name);
		TMap<FString, FString> OutcomeMap = UNarrativeDataHelper::LoadOutcomeMapping(Name);

		if (CharState && OutcomeMap.Num() > 0)
		{
			FString* OutcomeLabel = OutcomeMap.Find(CharState->CurrentPathString);
			if (OutcomeLabel)
			{
				GS->FinalCharacterOutcomes.Add(Name, *OutcomeLabel);
				WorldOutcomeKey.Append(*OutcomeLabel);
				if (i < CharacterNames.Num() - 1)
				{
					WorldOutcomeKey.Append(",");
				}
			}
		}
	}

	TMap<FString, FWorldOutcome> WorldMap = UNarrativeDataHelper::LoadWorldOutcomeMapping();
	if (WorldMap.Num() > 0)
	{
		FWorldOutcome* WorldOutcome = WorldMap.Find(WorldOutcomeKey);
		if (WorldOutcome)
		{
			GS->bSunWasSaved = WorldOutcome->bSunIsOn;
			GS->WorldOutcomeExplanation = WorldOutcome->Explanation;
		}
	}

	CurrentGameState = EGameState::EShowingWorldOutcome;
	OnGameStateChanged(CurrentGameState);
}