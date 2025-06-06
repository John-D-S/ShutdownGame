#include "NarrativePlayerController.h"
#include "Blueprint/UserWidget.h"

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
	
	// Show the mouse cursor permanently for this game type.
	bShowMouseCursor = true;
	SetInputMode(FInputModeGameAndUI());
}