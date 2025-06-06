#include "NarrativeGameState.h"

void ANarrativeGameState::BeginPlay()
{
	Super::BeginPlay();

	// Initialize the state for each character.
	CharacterStates.Add("Crystal", FCharacterState());
	CharacterStates.Add("Shae", FCharacterState());
	CharacterStates.Add("Dax", FCharacterState());
	CharacterStates.Add("Trent", FCharacterState());

	// Set initial locations based on the narrative design.
	if (FCharacterState* CrystalState = CharacterStates.Find("Crystal"))
	{
		CrystalState->CurrentLocation = FName("Southpoint");
	}
	if (FCharacterState* ShaeState = CharacterStates.Find("Shae"))
	{
		ShaeState->CurrentLocation = FName("Lumina");
	}
	if (FCharacterState* DaxState = CharacterStates.Find("Dax"))
	{
		DaxState->CurrentLocation = FName("Seaburn");
	}
	if (FCharacterState* TrentState = CharacterStates.Find("Trent"))
	{
		TrentState->CurrentLocation = FName("Coldry");
	}

	// Trigger the Blueprint event to set up tokens for the first day.
	OnNewDayStarted();
}