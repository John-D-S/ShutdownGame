#include "NarrativeDataHelper.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

// Define a uniquely named enum to keep track of our parsing state to avoid engine collisions.
enum class ENarrativeParseState : uint8
{
	Initial,
	InChallenge,
	InPositiveOutcome,
	InNegativeOutcome
};

bool UNarrativeDataHelper::ParseNarrativeFile(const FString& CharacterName, const FString& PathString, FNarrativeSegmentData& OutData)
{
	// Construct the filename from the path string.
	FString FileName = PathString.Equals("start", ESearchCase::IgnoreCase) ? "path_start.txt" : FString::Printf(TEXT("path_%s.txt"), *PathString);
	
	// Correctly construct the full path including the "Characters" subfolder.
	FString FullPath = FPaths::ProjectContentDir() / TEXT("Narrative") / TEXT("Characters") / CharacterName / FileName;

	// Load the file content into a string.
	FString FileContent;
	if (!FFileHelper::LoadFileToString(FileContent, *FullPath))
	{
		UE_LOG(LogTemp, Error, TEXT("ParseNarrativeFile: Failed to load file: %s"), *FullPath);
		OutData.bWasSuccessfullyParsed = false;
		return false;
	}

	TArray<FString> Lines;
	FileContent.ParseIntoArrayLines(Lines, true);

	ENarrativeParseState CurrentState = ENarrativeParseState::Initial;
	FChallengeOutcomeData* CurrentOutcomeData = nullptr;

	for (const FString& Line : Lines)
	{
		FString TrimmedLine = Line.TrimStartAndEnd();
		if (TrimmedLine.IsEmpty() || TrimmedLine.StartsWith("# "))
		{
			continue;
		}

		// Handle specific tags that change our state.
		if (TrimmedLine.StartsWith("TOKEN_CHALLENGE_START"))
		{
			CurrentState = ENarrativeParseState::InChallenge;
			continue;
		}
		if (TrimmedLine.StartsWith("POSITIVE_PATH_OUTCOME_TEXT_START"))
		{
			CurrentState = ENarrativeParseState::InPositiveOutcome;
			CurrentOutcomeData = &OutData.TokenChallenge.PositiveOutcome;
			continue;
		}
		if (TrimmedLine.StartsWith("NEGATIVE_PATH_OUTCOME_TEXT_START"))
		{
			CurrentState = ENarrativeParseState::InNegativeOutcome;
			CurrentOutcomeData = &OutData.TokenChallenge.NegativeOutcome;
			continue;
		}
		if (TrimmedLine.EndsWith("_END"))
		{
			// Revert state when an outcome block ends.
			CurrentState = (CurrentState == ENarrativeParseState::InPositiveOutcome || CurrentState == ENarrativeParseState::InNegativeOutcome) ? ENarrativeParseState::InChallenge : ENarrativeParseState::Initial;
			CurrentOutcomeData = nullptr;
			continue;
		}

		// Handle image placeholder comments.
		if (TrimmedLine.StartsWith("# IMAGE:"))
		{
			FString ImageDesc = TrimmedLine.RightChop(8).TrimStart();
			if (CurrentState == ENarrativeParseState::InPositiveOutcome || CurrentState == ENarrativeParseState::InNegativeOutcome)
			{
				if(CurrentOutcomeData) CurrentOutcomeData->ImageDescriptions.Add(ImageDesc);
			}
			else
			{
				OutData.InitialImageDescriptions.Add(ImageDesc);
			}
			continue;
		}
		
		// Handle keyword lines based on current state.
		if (CurrentState == ENarrativeParseState::InChallenge)
		{
			if (TrimmedLine.StartsWith("ACTION_TO_ATTEMPT_TEXT:"))
			{
				OutData.TokenChallenge.ActionToAttemptText = FText::FromString(TrimmedLine.RightChop(23).TrimStart());
			}
		}
		else // Handle narrative/dialog lines for Initial, Positive, and Negative states.
		{
			FNarrativeLine NewNarrativeLine;
			FString SpeakerStr, LineStr;
			
			if (TrimmedLine.Split(TEXT(":"), &SpeakerStr, &LineStr))
			{
				// Line has a speaker: "SPEAKER: Text".
				NewNarrativeLine.Speaker = FName(*SpeakerStr.TrimStartAndEnd());
				NewNarrativeLine.Line = FText::FromString(LineStr.TrimStart());
			}
			else // Line has no speaker, assume NARRATOR.
			{
				NewNarrativeLine.Speaker = FName("NARRATOR");
				NewNarrativeLine.Line = FText::FromString(TrimmedLine);
			}

			if (CurrentState == ENarrativeParseState::InPositiveOutcome || CurrentState == ENarrativeParseState::InNegativeOutcome)
			{
				// Check for keywords within outcome blocks before adding as narrative.
				if (TrimmedLine.StartsWith("LOCATION:"))
				{
					if(CurrentOutcomeData) CurrentOutcomeData->NewLocation = FName(*TrimmedLine.RightChop(9).TrimStart());
				}
				else if (TrimmedLine.StartsWith("NEXT_PATH_SEGMENT:"))
				{
					if(CurrentOutcomeData) CurrentOutcomeData->NextPathSegment = TrimmedLine.RightChop(18).TrimStart();
				}
				else
				{
					if(CurrentOutcomeData) CurrentOutcomeData->NarrativeLines.Add(NewNarrativeLine);
				}
			}
			else // EParseState::Initial
			{
				OutData.InitialNarrativeLines.Add(NewNarrativeLine);
			}
		}
	}
	
	OutData.bWasSuccessfullyParsed = true;
	return true;
}

TMap<FString, FString> UNarrativeDataHelper::LoadOutcomeMapping(const FString& CharacterName)
{
	TMap<FString, FString> OutcomeMap;
	FString FullPath = FPaths::ProjectContentDir() / TEXT("Narrative") / TEXT("Characters") / CharacterName / TEXT("outcome_mapping.txt");

	TArray<FString> Lines;
	if (!FFileHelper::LoadFileToStringArray(Lines, *FullPath))
	{
		UE_LOG(LogTemp, Error, TEXT("LoadOutcomeMapping: Failed to load file: %s"), *FullPath);
		return OutcomeMap;
	}

	for (const FString& Line : Lines)
	{
		if (Line.StartsWith("#") || Line.IsEmpty()) continue;

		FString PathKey, OutcomeValue;
		if (Line.Split(TEXT(":"), &PathKey, &OutcomeValue))
		{
			FString MainValue, Comment;
			OutcomeValue.Split(TEXT("#"), &MainValue, &Comment);
			OutcomeMap.Add(PathKey.TrimStartAndEnd(), MainValue.TrimStartAndEnd());
		}
	}
	return OutcomeMap;
}

TMap<FString, FWorldOutcome> UNarrativeDataHelper::LoadWorldOutcomeMapping()
{
	TMap<FString, FWorldOutcome> WorldOutcomeMap;
	FString FullPath = FPaths::ProjectContentDir() / TEXT("Narrative") / TEXT("World") / TEXT("sun_outcome_mappings.txt");

	TArray<FString> Lines;
	if (!FFileHelper::LoadFileToStringArray(Lines, *FullPath))
	{
		UE_LOG(LogTemp, Error, TEXT("LoadWorldOutcomeMapping: Failed to load file: %s"), *FullPath);
		return WorldOutcomeMap;
	}

	for (const FString& Line : Lines)
	{
		if (Line.StartsWith("#") || Line.IsEmpty()) continue;

		FString ComboKey, RestOfLine;
		if (Line.Split(TEXT(":"), &ComboKey, &RestOfLine))
		{
			FString StateStr, ExplanationStr;
			if (RestOfLine.Split(TEXT("|"), &StateStr, &ExplanationStr))
			{
				FWorldOutcome OutcomeData;
				OutcomeData.bSunIsOn = StateStr.TrimStartAndEnd().Equals("SUN_ON");
				
				FString MainExplanation, CommentPart;
				ExplanationStr.TrimStartAndEnd().Split(TEXT("#"), &MainExplanation, &CommentPart);
				OutcomeData.Explanation = MainExplanation.TrimEnd();
				
				WorldOutcomeMap.Add(ComboKey.TrimStartAndEnd(), OutcomeData);
			}
		}
	}
	return WorldOutcomeMap;
}

TArray<FNarrativeLine> UNarrativeDataHelper::ParseResolutionFile(const FString& CharacterName, const FString& PathString, bool bSunIsOn)
{
	TArray<FNarrativeLine> ResolutionLines;
	FString SunStateStr = bSunIsOn ? "sun_on" : "sun_off";
	
	// Corrected filename format to match the generated files.
	FString FileName = FString::Printf(TEXT("path_%s_sun_%s.txt"), *PathString, *SunStateStr);
	
	FString FullPath = FPaths::ProjectContentDir() / TEXT("Narrative") / TEXT("Characters") / CharacterName / TEXT("resolutions") / FileName;

	TArray<FString> Lines;
	if (!FFileHelper::LoadFileToStringArray(Lines, *FullPath))
	{
		UE_LOG(LogTemp, Error, TEXT("ParseResolutionFile: Failed to load file: %s"), *FullPath);
		return ResolutionLines;
	}

	for (const FString& Line : Lines)
	{
		FString TrimmedLine = Line.TrimStartAndEnd();
		if (TrimmedLine.IsEmpty()) continue;

		if (TrimmedLine.StartsWith("# IMAGE:"))
		{
			FNarrativeLine ImageLine;
			ImageLine.Speaker = FName("IMAGE");
			ImageLine.Line = FText::FromString(TrimmedLine.RightChop(8).TrimStart());
			ResolutionLines.Add(ImageLine);
		}
		else if (!TrimmedLine.StartsWith("#"))
		{
			FNarrativeLine NewLine;
			FString SpeakerStr, LineStr;
			if (TrimmedLine.Split(TEXT(":"), &SpeakerStr, &LineStr))
			{
				NewLine.Speaker = FName(*SpeakerStr.TrimStartAndEnd());
				NewLine.Line = FText::FromString(LineStr.TrimStart());
			}
			else
			{
				NewLine.Speaker = FName("NARRATOR");
				NewLine.Line = FText::FromString(TrimmedLine);
			}
			ResolutionLines.Add(NewLine);
		}
	}
	return ResolutionLines;
}