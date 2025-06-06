#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NarrativePlayerController.generated.h"

class UUserWidget; // Forward declaration

UCLASS()
class SHUTDOWNGAME_API ANarrativePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	/// A reference to the main HUD widget instance after it's created.
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UUserWidget> HUDWidgetInstance;
	
protected:
	/// Called when the game starts or when spawned.
	virtual void BeginPlay() override;

private:
	/// The Blueprint class for the main HUD widget. Set this in the Blueprint child class defaults.
	UPROPERTY(EditDefaultsOnly, Category = "UI Classes")
	TSubclassOf<UUserWidget> HUDWidgetClass;
};