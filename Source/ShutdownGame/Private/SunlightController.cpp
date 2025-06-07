#include "SunlightController.h"
#include "NarrativeGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Components/LightComponent.h"

ASunlightController::ASunlightController()
{
	// Set this actor to call Tick() every frame.
	PrimaryActorTick.bCanEverTick = true;
}

void ASunlightController::BeginPlay()
{
	Super::BeginPlay();

	// Get and cache a reference to the game state.
	NarrativeGameState = GetWorld()->GetGameState<ANarrativeGameState>();
	
	// Find all lights in the world with the "SunLight" tag.
	TArray<AActor*> FoundLightActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("SunLight"), FoundLightActors);
	for (AActor* LightActor : FoundLightActors)
	{
		ULightComponent* LightComponent = LightActor->FindComponentByClass<ULightComponent>();
		if (LightComponent)
		{
			WorldLightsToFlicker.Add(LightComponent);
			BaseLightIntensities.Add(LightComponent->Intensity);
		}
	}
}

void ASunlightController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Ensure we have a valid reference to the game state.
	if (!NarrativeGameState) return;

	FVector2D CurrentFlickerParams;
	
	if (CurrentSunlightState == ESunlightState::Transitioning)
	{
		TransitionElapsedTime += DeltaTime;
		const float Alpha = FMath::Clamp(TransitionElapsedTime / TransitionDuration, 0.0f, 1.0f);

		// Smoothly interpolate the flicker parameters themselves.
		CurrentFlickerParams = FMath::Lerp(FlickerParams_Start, FlickerParams_Target, Alpha);

		// If the transition is over, update the daily setting and return to Idle.
		if (Alpha >= 1.0f)
		{
			// This makes the transition's result permanent for the rest of the day.
			DailyFlickerSettings.Add(NarrativeGameState->CurrentDay, FlickerParams_Target);
			CurrentSunlightState = ESunlightState::Idle;
		}
	}
	else // ESunlightState::Idle
	{
		// During idle, just use the settings for the current day.
		const FVector2D* FoundSetting = DailyFlickerSettings.Find(NarrativeGameState->CurrentDay);
		if (FoundSetting)
		{
			CurrentFlickerParams = *FoundSetting;
		}
		else
		{
			// Default to full brightness if no setting for today.
			CurrentFlickerParams = FVector2D(0.0f, 1.0f);
		}
	}
	
	// Get the current flicker settings for today. X is Amplitude, Y is Level.
	const float FlickerAmplitude = CurrentFlickerParams.X;
	const float BrightnessLevel = CurrentFlickerParams.Y;
	
	// Increment time for the noise function.
	TimeAccumulator += DeltaTime;

	// Use Perlin Noise to get a smoothly random value between -1 and 1.
	const float Noise = FMath::PerlinNoise1D(TimeAccumulator);

	// Calculate the final brightness using the day's settings.
	const float Brightness = (Noise * FlickerAmplitude) + BrightnessLevel;
	const float ClampedBrightness = FMath::Clamp(Brightness, 0.0f, 1.0f);
	
	// Update the Material Parameter Collection.
	if (SunMPC)
	{
		UMaterialParameterCollectionInstance* MPCInstance = GetWorld()->GetParameterCollectionInstance(SunMPC);
		if (MPCInstance)
		{
			MPCInstance->SetScalarParameterValue("SunBrightnessMultiplier", ClampedBrightness);
		}
	}
	
	// Update the intensity of all tagged lights.
	for (int32 i = 0; i < WorldLightsToFlicker.Num(); ++i)
	{
		if (WorldLightsToFlicker.IsValidIndex(i) && BaseLightIntensities.IsValidIndex(i))
		{
			if(WorldLightsToFlicker[i])
			{
				WorldLightsToFlicker[i]->SetIntensity(BaseLightIntensities[i] * ClampedBrightness);
			}
		}
	}
}

void ASunlightController::StartFinalSunTransition(bool bSunWillBeOn)
{
	// Don't start a new transition if one is already happening.
	if (CurrentSunlightState == ESunlightState::Transitioning || !NarrativeGameState) return;

	// The transition starts from Day 6's flicker settings.
	const int32 FinalDayKey = 6;
	const FVector2D* FoundSetting = DailyFlickerSettings.Find(FinalDayKey);
	if (FoundSetting)
	{
		FlickerParams_Start = *FoundSetting;
	}
	else
	{
		// Fallback if no setting for Day 6 exists.
		FlickerParams_Start = FVector2D(0.125f, 0.275f);
	}

	// Set the target based on if the sun was saved.
	if (bSunWillBeOn)
	{
		// Target: No flicker (Amplitude=0), full brightness (Level=1).
		FlickerParams_Target = FVector2D(0.0f, 1.0f);
	}
	else
	{
		// Target: No flicker (Amplitude=0), no brightness (Level=0).
		FlickerParams_Target = FVector2D(0.0f, 0.0f);
	}

	TransitionElapsedTime = 0.0f;
	CurrentSunlightState = ESunlightState::Transitioning;
}