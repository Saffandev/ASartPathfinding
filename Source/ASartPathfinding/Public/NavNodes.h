// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NavNodes.generated.h"

/**
 * 
 */
//template <typename InNodeType>
UCLASS(BlueprintType)
class ASARTPATHFINDING_API UNavNodes : public UObject
{
	GENERATED_BODY()

public:
	
	/*bool operator()( const UNavNodes* A, const UNavNodes* B ) const
	{
		return A->GloablGoal < B->GloablGoal;
	}*/
	FORCEINLINE bool operator<(const UNavNodes &A ) const
	{
		return GloablGoal < A.GloablGoal;
	}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector NodeLoc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsVisited = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float GloablGoal = FLT_MAX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float LocalGoal = FLT_MAX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<UNavNodes*> NeighbourNode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UNavNodes* ParentNode = nullptr;

public:
	UFUNCTION(BlueprintCallable)
		void Reset()
	{
		bIsVisited = false;
		GloablGoal = FLT_MAX;
		LocalGoal = FLT_MAX;
		 ParentNode = nullptr;
	}
};

