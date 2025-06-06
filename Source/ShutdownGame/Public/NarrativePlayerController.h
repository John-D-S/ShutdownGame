#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NarrativePlayerController.generated.h"

class UUserWidget; // Forward declaration for Map Widget
class UNarrativeDialogWidget; // Forward declaration for our new Dialog base class

UCLASS()
class SHUTDOWNGAME_API ANarrativePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	/// Shows the Map UI widget and hides the Dialog UI.
	UFUNCTION(BlueprintCallable, Category = "UI Control")
	void ShowMapView();

	/// Shows the Dialog UI widget and hides the Map UI.
	UFUNCTION(BlueprintCallable, Category = "UI Control")
	void ShowDialogView();

	/// Smoothly moves the sun's parent pivot to a new rotation on the X axis.
	UFUNCTION(BlueprintCallable, Category = "World Control")
	void UpdateSunRotation(float NewRotationX);

	/// Smoothly moves the camera to a target location and rotation.
	UFUNCTION(BlueprintCallable, Category = "World Control")
	void MoveCameraToTarget(FVector TargetLocation, FRotator TargetRotation);

protected:
	/// Called when the game starts or when spawned.
	virtual void BeginPlay() override;

	/// Called every frame.
	virtual void Tick(float DeltaTime) override;

private:
	/// The Blueprint class for the Map widget. Set this in the Blueprint child class defaults.
	UPROPERTY(EditDefaultsOnly, Category = "UI Classes")
	TSubclassOf<UUserWidget> MapWidgetClass;
	
	/// The Blueprint class for the Dialog widget. Must inherit from NarrativeDialogWidget.
	UPROPERTY(EditDefaultsOnly, Category = "UI Classes")
	TSubclassOf<UNarrativeDialogWidget> DialogWidgetClass;

	/// The created instance of the Map widget.
	UPROPERTY()
	TObjectPtr<UUserWidget> MapWidgetInstance;

	/// The created instance of the Dialog widget.
	UPROPERTY()
	TObjectPtr<UNarrativeDialogWidget> DialogWidgetInstance;

	/// The actor in the world tagged as 'SunPivot' that controls the sun's rotation.
	UPROPERTY()
	TObjectPtr<AActor> SunPivotActor;

	/// The target rotation for the sun pivot. The Tick function will smoothly interpolate to this value.
	UPROPERTY()
	FRotator TargetSunRotation;

	/// The target location for the camera. The Tick function will smoothly interpolate to this value.
	UPROPERTY()
	FVector TargetCameraLocation;

	/// The target rotation for the camera. The Tick function will smoothly interpolate to this value.
	UPROPERTY()
	FRotator TargetCameraRotation;

	/// The speed at which the camera and sun interpolate to their targets.
	UPROPERTY(EditDefaultsOnly, Category = "World Control")
	float InterpolationSpeed = 2.0f;
};