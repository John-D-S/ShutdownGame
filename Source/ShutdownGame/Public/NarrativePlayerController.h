#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NarrativePlayerController.generated.h"

class UUserWidget;

UCLASS()
class SHUTDOWNGAME_API ANarrativePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	/// A reference to the main HUD widget instance after it's created. Accessible to other Blueprints.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UUserWidget> HUDWidgetInstance;

	/// Sets the sun pivot's target X-axis (Roll) rotation to an absolute angle.
	UFUNCTION(BlueprintCallable, Category = "World Control")
	void SetSunRotation(float TargetRotationRoll);
	
	/// Increments the sun pivot's target X-axis (Roll) rotation by a given angle.
	UFUNCTION(BlueprintCallable, Category = "World Control")
	void IncrementSunRotation(float AngleIncrementRoll);

	/// Smoothly moves the camera to a target location and rotation.
	UFUNCTION(BlueprintCallable, Category = "World Control")
	void MoveCameraToTarget(FVector TargetLocation, FRotator TargetRotation);

	/// Smoothly moves the camera back to its default starting position.
	UFUNCTION(BlueprintCallable, Category = "World Control")
	void ReturnCameraToDefault();

protected:
	/// Called when the game starts or when spawned.
	virtual void BeginPlay() override;

	/// Called every frame.
	virtual void Tick(float DeltaTime) override;

private:
	/// The Blueprint class for the main HUD widget. Set this in the Blueprint child class defaults.
	UPROPERTY(EditDefaultsOnly, Category = "UI Classes")
	TSubclassOf<UUserWidget> HUDWidgetClass;

	/// The actor in the world tagged as 'SunPivot' that controls the sun's rotation.
	UPROPERTY()
	TObjectPtr<AActor> SunPivotActor;

	/// The current, continuously interpolated roll of the sun.
	float CurrentSunRoll;

	/// The target roll the sun is moving towards.
	float TargetSunRoll;

	/// The target location for the camera. The Tick function will smoothly interpolate to this value.
	UPROPERTY()
	FVector TargetCameraLocation;

	/// The target rotation for the camera. The Tick function will smoothly interpolate to this value.
	UPROPERTY()
	FRotator TargetCameraRotation;

	/// The default location the camera should return to.
	UPROPERTY()
	FVector DefaultCameraLocation;

	/// The default rotation the camera should return to.
	UPROPERTY()
	FRotator DefaultCameraRotation;

	/// The speed at which the camera and sun interpolate to their targets.
	UPROPERTY(EditDefaultsOnly, Category = "World Control")
	float InterpolationSpeed = 2.0f;
};