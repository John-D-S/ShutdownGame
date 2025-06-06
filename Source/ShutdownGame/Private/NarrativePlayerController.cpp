#include "NarrativePlayerController.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

void ANarrativePlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Create the main HUD widget and add it to the viewport.
	if (HUDWidgetClass)
	{
		HUDWidgetInstance = CreateWidget<UUserWidget>(this, HUDWidgetClass);
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport();
		}
	}
	
	bShowMouseCursor = true;
	SetInputMode(FInputModeGameAndUI());
	
	// Find the sun pivot actor.
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("SunPivot"), FoundActors);
	if (FoundActors.Num() > 0)
	{
		SunPivotActor = FoundActors[0];
		// Set the sun's initial starting position for Day 1 (e.g., -60 degrees on the X-axis/Roll).
		SetSunRotation(-60.0f);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NarrativePlayerController could not find an actor with tag 'SunPivot'!"));
	}
	
	// Explicitly find the PlayerStart and set our default/initial transform from it.
	AActor* PlayerStartActor = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerStart::StaticClass());
	APawn* ControlledPawn = GetPawn();

	if (PlayerStartActor && ControlledPawn)
	{
		DefaultCameraLocation = PlayerStartActor->GetActorLocation();
		DefaultCameraRotation = PlayerStartActor->GetActorRotation();
		ControlledPawn->SetActorLocationAndRotation(DefaultCameraLocation, DefaultCameraRotation);
		SetControlRotation(DefaultCameraRotation);
		TargetCameraLocation = DefaultCameraLocation;
		TargetCameraRotation = DefaultCameraRotation;
	}
}

void ANarrativePlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Smoothly interpolate the camera to its target position and rotation.
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn)
	{
		FVector CurrentLocation = ControlledPawn->GetActorLocation();
		FRotator CurrentRotation = ControlledPawn->GetActorRotation();
		
		if (!CurrentLocation.Equals(TargetCameraLocation, 1.0f) || !CurrentRotation.Equals(TargetCameraRotation, 1.0f))
		{
			FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetCameraLocation, DeltaTime, InterpolationSpeed);
			FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetCameraRotation, DeltaTime, InterpolationSpeed);
			ControlledPawn->SetActorLocationAndRotation(NewLocation, NewRotation);
		}
	}

	// Smoothly interpolate the sun pivot to its target rotation.
	if (SunPivotActor)
	{
		FRotator CurrentSunRotation = SunPivotActor->GetActorRotation();
		if (!CurrentSunRotation.Equals(TargetSunRotation, 0.01f))
		{
			FRotator NewSunRotation = FMath::RInterpTo(CurrentSunRotation, TargetSunRotation, DeltaTime, InterpolationSpeed);
			SunPivotActor->SetActorRotation(NewSunRotation);
		}
	}
}

void ANarrativePlayerController::SetSunRotation(float TargetRotationRoll)
{
	if (SunPivotActor)
	{
		// Set the target rotation directly, applying the value to the Roll (X-axis).
		TargetSunRotation = FRotator(0.0f, 0.0f, TargetRotationRoll);
	}
}

void ANarrativePlayerController::IncrementSunRotation(float AngleIncrementRoll)
{
	if (SunPivotActor)
	{
		// Add the increment to the current target rotation's Roll (X-axis).
		TargetSunRotation.Roll += AngleIncrementRoll;
	}
}

void ANarrativePlayerController::MoveCameraToTarget(FVector TargetLocation, FRotator TargetRotation)
{
	TargetCameraLocation = TargetLocation;
	TargetCameraRotation = TargetRotation;
}

void ANarrativePlayerController::ReturnCameraToDefault()
{
	TargetCameraLocation = DefaultCameraLocation;
	TargetCameraRotation = DefaultCameraRotation;
}