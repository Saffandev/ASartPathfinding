// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NavigationData.h"
#include "Navigation/PathFollowingComponent.h"
#include "CustomPathFollowingComponent.generated.h"

/**
 * 
 */
UCLASS()
class ASARTPATHFINDING_API UCustomPathFollowingComponent : public UPathFollowingComponent//, public ANavigationData
{
	GENERATED_BODY()
public:
	virtual void BeginPlay ( ) override;
	FNavigationPath* Path;
};
