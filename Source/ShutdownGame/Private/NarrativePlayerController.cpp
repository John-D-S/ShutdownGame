#include "NarrativePlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/UserWidget.h"
#include "NarrativeDialogWidget.h" // Include our new widget base class header

void ANarrativePlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Find all actors with the "SunPivot" tag.
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("SunPivot"), FoundActors);

	if (FoundActors.Num() > 0)
	{
		// Assign the first found actor to our pivot actor variable.
		SunPivotActor = FoundActors[0];
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NarrativePlayerController could not find an actor with tag 'SunPivot'!"));
	}

	// Set initial camera target to its current position to prevent unwanted movement on start.
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn)
	{
		TargetCameraLocation = ControlledPawn->GetActorLocation();
		TargetCameraRotation = ControlledPawn->GetActorRotation();
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

void ANarrativePlayerController::ShowMapView()
{
	if (DialogWidgetInstance)
	{
		DialogWidgetInstance->RemoveFromParent();
	}

	if (MapWidgetClass && !MapWidgetInstance)
	{
		MapWidgetInstance = CreateWidget<UUserWidget>(this, MapWidgetClass);
	}
	
	if (MapWidgetInstance && !MapWidgetInstance->IsInViewport())
	{
		MapWidgetInstance->AddToViewport();
		SetInputMode(FInputModeGameAndUI());
		bShowMouseCursor = true;
	}
}

void ANarrativePlayerController::ShowDialogView()
{
	if (MapWidgetInstance)
	{
		MapWidgetInstance->RemoveFromParent();
	}

	if (DialogWidgetClass && !DialogWidgetInstance)
	{
		DialogWidgetInstance = CreateWidget<UNarrativeDialogWidget>(this, DialogWidgetClass);
	}

	if (DialogWidgetInstance && !DialogWidgetInstance->IsInViewport())
	{
		DialogWidgetInstance->AddToViewport();
		SetInputMode(FInputModeGameAndUI());

		bShowMouseCursor = true;
		
		// Call the BlueprintImplementableEvent on the widget to tell it to populate itself.
		DialogWidgetInstance->OnDisplaySegment();
	}
}

void ANarrativePlayerController::UpdateSunRotation(float NewRotationX)
{
	if (SunPivotActor)
	{
		// Set the target rotation. The Tick function will handle the smooth movement.
		TargetSunRotation = FRotator(NewRotationX, 0.0f, 0.0f);
	}
}

void ANarrativePlayerController::MoveCameraToTarget(FVector TargetLocation, FRotator TargetRotation)
{
	// Set the target location and rotation. The Tick function will handle the smooth movement.
	TargetCameraLocation = TargetLocation;
	TargetCameraRotation = TargetRotation;
}