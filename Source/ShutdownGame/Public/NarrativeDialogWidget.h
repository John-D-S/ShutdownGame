#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NarrativeDialogWidget.generated.h"

UCLASS()
class SHUTDOWNGAME_API UNarrativeDialogWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/// An event that is called from C++ but implemented in the child Blueprint (WBP_Dialog).
	/// This will be triggered to tell the UI to populate itself with the current segment data.
	UFUNCTION(BlueprintImplementableEvent, Category = "UI Events")
	void OnDisplaySegment();
};