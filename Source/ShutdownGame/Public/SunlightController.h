#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SunlightController.generated.h"

class UMaterialParameterCollection;

// Defines if the sun is flickering normally or in a final transition.
UENUM(BlueprintType)
enum class ESunlightState : uint8
{
	Idle,
	Transitioning
};

UCLASS()
class SHUTDOWNGAME_API ASunlightController : public AActor
{
	GENERATED_BODY()
	
public:	
	/// Sets default values for this actor's properties.
	ASunlightController();

	/// Called every frame.
	virtual void Tick(float DeltaTime) override;
	
	/// Starts the final 5-second transition to the sun's fate.
	UFUNCTION(BlueprintCallable, Category = "Sun Flicker")
	void StartFinalSunTransition(bool bSunWillBeOn);

protected:
	/// Called when the game starts or when spawned.
	virtual void BeginPlay() override;

private:
	/// A map of Day -> (Flicker Amplitude, Brightness Level). Set this on the instance in the level.
	UPROPERTY(EditAnywhere, Category = "Sun Flicker")
	TMap<int32, FVector2D> DailyFlickerSettings;
	
	/// The Material Parameter Collection asset to control global sun brightness. Set this on the instance in the level.
	UPROPERTY(EditAnywhere, Category = "Sun Flicker")
	TObjectPtr<UMaterialParameterCollection> SunMPC;

	/// A reference to the game state, cached for performance.
	UPROPERTY()
	TObjectPtr<class ANarrativeGameState> NarrativeGameState;

	/// The current state of the sun's flicker effect.
	ESunlightState CurrentSunlightState = ESunlightState::Idle;
	
	/// The flicker settings at the start of the transition.
	FVector2D FlickerParams_Start;
	
	/// The target flicker settings for the end of the transition.
	FVector2D FlickerParams_Target;

	/// How long the final transition should take.
	float TransitionDuration = 5.0f;

	/// How long the transition has been running.
	float TransitionElapsedTime = 0.0f;
	
	/// Used to drive the Perlin noise function over time.
	float TimeAccumulator = 0.0f;

	/// References to the lights in the world we want to control.
	UPROPERTY()
	TArray<TObjectPtr<class ULightComponent>> WorldLightsToFlicker;

	/// The base intensity of each light, stored on BeginPlay.
	TArray<float> BaseLightIntensities;
};