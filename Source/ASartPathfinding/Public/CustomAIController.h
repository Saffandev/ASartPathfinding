// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CustomAIController.generated.h"

/**
 * 
 */
UCLASS()
class ASARTPATHFINDING_API ACustomAIController : public AAIController//, public UPathFollowingComponent
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay ( ) override;
	//FNavigationPath* Path;
};
