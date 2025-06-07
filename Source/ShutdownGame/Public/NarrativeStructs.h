#pragma once

#include "CoreMinimal.h"
#include "NarrativeStructs.generated.h"

// Holds the data for a single world outcome mapping.
USTRUCT(BlueprintType)
struct FWorldOutcome
{
	GENERATED_BODY()

	/// Whether the sun was saved in this outcome.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Narrative")
	bool bSunIsOn = false;

	/// The AI-generated text explaining this world outcome.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Narrative")
	FString Explanation;
};

// Holds the dynamic state for a single character throughout the game.
USTRUCT(BlueprintType)
struct FCharacterState
{
	GENERATED_BODY()

	/// The current path string of P/N choices (e.g., "P", "PN", "PNP").
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character State")
	FString CurrentPathString = "start";

	/// The current cumulative weighted score for this character.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character State")
	float CumulativeScore = 0.0f;

	/// The current location of the character on the map.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character State")
	FName CurrentLocation;
};


// Holds the data for a single parsed line of dialogue or narration.
USTRUCT(BlueprintType)
struct FNarrativeLine
{
	GENERATED_BODY()

	/// The name of the speaker (e.g., NARRATOR, CRYSTAL).
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Narrative")
	FName Speaker;

	/// The line of text itself.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Narrative")
	FText Line;
};

// Holds the data for a challenge outcome (either positive or negative).
USTRUCT(BlueprintType)
struct FChallengeOutcomeData
{
	GENERATED_BODY()

	/// The sequence of narrative lines for this outcome.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Narrative")
	TArray<FNarrativeLine> NarrativeLines;

	/// The descriptions for any images used in this outcome.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Narrative")
	TArray<FString> ImageDescriptions;

	/// The new location the character moves to upon this outcome.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Narrative")
	FName NewLocation;

	/// The path string for the next segment (e.g., path_P.txt, path_PN.txt, or END_OF_5_DAY_ARC).
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Narrative")
	FString NextPathSegment;
};

// Holds the data for a complete token challenge block.
USTRUCT(BlueprintType)
struct FTokenChallengeData
{
	GENERATED_BODY()

	/// The text describing the action the player is attempting.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Narrative")
	FText ActionToAttemptText;

	/// The data for the positive outcome of this challenge.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Narrative")
	FChallengeOutcomeData PositiveOutcome;

	/// The data for the negative outcome of this challenge.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Narrative")
	FChallengeOutcomeData NegativeOutcome;
};

// The main container struct holding all parsed data for a single .txt file.
USTRUCT(BlueprintType)
struct FNarrativeSegmentData
{
	GENERATED_BODY()

	/// The narrative lines that appear before the token challenge.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Narrative")
	TArray<FNarrativeLine> InitialNarrativeLines;

	/// The descriptions for any images used before the challenge.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Narrative")
	TArray<FString> InitialImageDescriptions;

	/// The token challenge data for this segment.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Narrative")
	FTokenChallengeData TokenChallenge;

	/// Indicates if the parsing was successful and data is valid.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Narrative")
	bool bWasSuccessfullyParsed = false;
};