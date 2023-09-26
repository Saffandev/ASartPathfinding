// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NavNode.generated.h"
/**
 * 
 */

USTRUCT(BlueprintType)
struct FPathNode
{
	GENERATED_BODY()


	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FVector NodeLoc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsVisited;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float GlobalGoal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LocalGoal;

	TArray<FPathNode> NeighbourNode;

	FPathNode* N;
};
class ASARTPATHFINDING_API NavNode
{

public:
	
};
