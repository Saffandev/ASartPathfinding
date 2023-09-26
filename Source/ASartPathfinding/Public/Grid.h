// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grid.generated.h"

class UNavNodes;

//USTRUCT( )
//struct FHeapOrder
//{
//	GENERATED_BODY ( )
//
//		bool operator()( const UNavNodes* A, const UNavNodes* B ) const;
//
//	FHeapOrder ( ) = default;
//};

UCLASS()
class ASARTPATHFINDING_API AGrid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrid();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable )
	void CreateNavMesh();
	UFUNCTION(BlueprintCallable )
	void CreateNavNode ( int index, FVector NavNodeLocation );
	void NavNodeInit (int X, int Y );
	UFUNCTION(BlueprintPure )
	int CalculateIndex ( int indexX, int indexY );
	void Calculate2DIndex ( int Index, int& indexX, int& indexY );
	UFUNCTION ( BlueprintCallable )
	void SetStartNode ( int Index );
	UFUNCTION ( BlueprintCallable )
	void SetEndNode ( int Index );
	void SetCurrentNode ( );
	float CalculateDistance ( UNavNodes* nodeA, UNavNodes* nodeB );
	UFUNCTION(BlueprintCallable )
	TArray<FVector> CalculatAStar ( );
	void SortNodeGroup ( TArray<UNavNodes*>& NavGroup );
	void MergeSort ( TArray<UNavNodes*>& NavGroup, int Start, int End );
	void Merge ( TArray<UNavNodes*>& NavGroup, int Start, int Mid, int End );
	void DrawPath ( );
	TArray<FVector> OptimisePath ( );
	void SetNeighbourNode ( );
	void AddValidNeighbourNode ( UNavNodes* Node,int32 IndexX,int32 IndexY);
	UFUNCTION(BlueprintCallable )
	TArray<FVector> FindPathPoints ( int32 StartIndex, int32 EndIndex );

public:	
	UPROPERTY(VisibleAnywhere,Category = "A_NavMesh");
	class UInstancedStaticMeshComponent* ISMComponent;
	UPROPERTY ( VisibleAnywhere, BlueprintReadOnly, Category = "A_NavMesh" );
	TArray<UNavNodes*> GridArray;
	UPROPERTY ( VisibleAnywhere, BlueprintReadOnly, Category = "A_NavMesh" );
	TArray<FTransform> NodesTransforms;
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "A_NavMesh");
	int Size_X;
	UPROPERTY( EditAnywhere,BlueprintReadOnly, Category = "A_NavMesh");
	int Size_Y;
	UNavNodes* StartNode;
	UNavNodes* EndNode;
	UNavNodes* CurrentNode;
	UPROPERTY ( EditAnywhere, BlueprintReadOnly, Category = "A_NavMesh" );
	bool bCanDraw;
};


