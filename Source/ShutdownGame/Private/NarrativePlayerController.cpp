#include "NarrativePlayerController.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerStart.h" // Required include for APlayerStart
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
		// Get the intended starting location and rotation from the PlayerStart actor.
		DefaultCameraLocation = PlayerStartActor->GetActorLocation();
		DefaultCameraRotation = PlayerStartActor->GetActorRotation();

		// Set the pawn's position and rotation to match the PlayerStart.
		ControlledPawn->SetActorLocationAndRotation(DefaultCameraLocation, DefaultCameraRotation);

		// Crucially, set the controller's rotation to match as well.
		SetControlRotation(DefaultCameraRotation);

		// Set the initial targets for the tick interpolation to this correct starting transform.
		TargetCameraLocation = DefaultCameraLocation;
		TargetCameraRotation = DefaultCameraRotation;
	}
	else if (ControlledPawn) // Fallback if no PlayerStart is found.
	{
		DefaultCameraLocation = ControlledPawn->GetActorLocation();
		DefaultCameraRotation = ControlledPawn->GetActorRotation();
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

void ANarrativePlayerController::UpdateSunRotation(float NewRotationX)
{
	if (SunPivotActor)
	{
		TargetSunRotation = FRotator(NewRotationX, 0.0f, 0.0f);
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