// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "NavNodes.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AGrid::AGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	ISMComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("HierarchicalInstancedStaticMeshComponent"));
	RootComponent = ISMComponent;
	bCanDraw = false;
}

void AGrid::CreateNavMesh()
{
	if ( !ISMComponent || !ISMComponent->GetStaticMesh ( ) )
	{
		return;
	}
	ISMComponent->ClearInstances ( );
	GridArray.Empty();
	GridArray.SetNum(Size_X * Size_Y);
	NodesTransforms.SetNum ( Size_X * Size_Y );

	//ParallelFor ( Size_X, [&] ( int X )
	for(int32 X = 0 ; X < Size_X ; X++ )
	{
		FVector InstanceTransformLocation;
		FBoxSphereBounds MeshBounds = ISMComponent->GetStaticMesh ( )->GetBounds ( );
		InstanceTransformLocation.X = MeshBounds.BoxExtent.X * 2.01 * X;

		//ParallelFor ( Size_Y, [&] ( int Y )
		for ( int Y = 0; Y < Size_Y; Y++ )
		{
		// GEngine->AddOnScreenDebugMessage ( -1, 1.0f, FColor::Yellow, TEXT ( "Inside Paraller for" ) );

			InstanceTransformLocation.Y = MeshBounds.BoxExtent.Y * 2.01 * Y;
			FVector TransformInstanceLoc = UKismetMathLibrary::TransformLocation ( GetActorTransform ( ), InstanceTransformLocation );
			FVector BoxTraceHalfBound = FVector ( MeshBounds.BoxExtent.X, MeshBounds.BoxExtent.Y, 10.f );
			FHitResult Hit;
			TArray<AActor*> ActorToIgnore;
			ActorToIgnore.Add ( this );

			bool bIsHit = UKismetSystemLibrary::BoxTraceSingle ( GetWorld ( ),
																	TransformInstanceLoc,
																	TransformInstanceLoc,
																	BoxTraceHalfBound,
																	FRotator::ZeroRotator,
																	UEngineTypes::ConvertToTraceType ( ECC_Visibility ),
																	false,
																	ActorToIgnore,
																	EDrawDebugTrace::None,
																	Hit,
																	true );
			int NodeIndex = CalculateIndex ( X, Y );
			FTransform InstanceTransform;

			if ( !bIsHit )
			{
				InstanceTransform.SetLocation ( InstanceTransformLocation );
				InstanceTransform.SetScale3D ( FVector3d ( 1, 1, 1 ) );
				CreateNavNode ( NodeIndex, TransformInstanceLoc );
			}

			else
			{
				InstanceTransform.SetLocation ( InstanceTransformLocation );
				InstanceTransform.SetScale3D ( FVector3d::ZeroVector );
			}
			NodesTransforms[ NodeIndex ] = InstanceTransform;

		} //);
	} //);
	
	int t = GridArray.Num();
	SetNeighbourNode ( );
	
}

void AGrid::SetNeighbourNode ( )
{
	UE_LOG ( LogTemp, Error, TEXT ( "Grid Array Size %i" ), GridArray.Num ( ) );
	for ( UNavNodes* Node : GridArray )
	{
		if ( Node )
		{
			int32 NodeIndex = GridArray.Find ( Node );
			int32 X, Y;
			Calculate2DIndex ( NodeIndex, X, Y );
			AddValidNeighbourNode ( Node,X + 1, Y + 1 );//digaonal
			AddValidNeighbourNode ( Node,X - 1, Y - 1 );//digaonal
			AddValidNeighbourNode ( Node,X + 1, Y - 1 );//digaonal
			AddValidNeighbourNode ( Node,X - 1, Y + 1 );//digaonal
			AddValidNeighbourNode ( Node,X + 1, Y );
			AddValidNeighbourNode ( Node,X - 1, Y );
			AddValidNeighbourNode ( Node,X, Y + 1 );
			AddValidNeighbourNode ( Node,X, Y - 1 );

		}
	}
}

void AGrid::AddValidNeighbourNode ( UNavNodes* Node, int32 IndexX, int32 IndexY )
{
	int32 Index = CalculateIndex ( IndexX, IndexY );
	if ( Index >= 0 && Index < GridArray.Num ( ) )
	{
		UNavNodes* TempNode = GridArray[ Index ];
		if ( TempNode )
		{
			Node->NeighbourNode.Add ( TempNode );
		}
	}
	return;

}

TArray<FVector> AGrid::FindPathPoints ( int32 StartIndex, int32 EndIndex )
{
	SetStartNode ( StartIndex );
	SetEndNode ( EndIndex );
	
	TArray<FVector> Points = CalculatAStar ( );
	return Points;
}

void AGrid::NavNodeInit ( int X, int Y )
{
	FVector InstanceTransformLocation;
	FBoxSphereBounds MeshBounds = ISMComponent->GetStaticMesh ( )->GetBounds ( );
	InstanceTransformLocation.X = MeshBounds.BoxExtent.X * 2.01 * X;

	

	
	InstanceTransformLocation.Y = MeshBounds.BoxExtent.Y * 2.01 * Y;
	FVector TransformInstanceLoc = UKismetMathLibrary::TransformLocation(GetActorTransform(), InstanceTransformLocation);
	FVector BoxTraceHalfBound = FVector ( MeshBounds.BoxExtent.X, MeshBounds.BoxExtent.Y, 10.f );
	FHitResult Hit;
	TArray<AActor*> ActorToIgnore;
	ActorToIgnore.Add(this);

	bool bIsHit = UKismetSystemLibrary::BoxTraceSingle ( GetWorld ( ),
															TransformInstanceLoc,
															TransformInstanceLoc,
															BoxTraceHalfBound,
															FRotator::ZeroRotator,
															UEngineTypes::ConvertToTraceType ( ECC_Visibility ),
															false,
															ActorToIgnore,
															EDrawDebugTrace::None,
															Hit,
															true );

	if ( !bIsHit )
	{
		FTransform InstanceTransform;
		InstanceTransform.SetLocation ( InstanceTransformLocation );
		InstanceTransform.SetRotation ( FRotator::ZeroRotator.Quaternion ( ) );
		InstanceTransform.SetScale3D ( FVector3d (1,1,1));
	//	DrawDebugSphere ( GetWorld ( ), TransformInstanceLoc, 50, 10, FColor::Red, false, 5.f );

		ISMComponent->AddInstance ( InstanceTransform );
		int NodeIndex = CalculateIndex ( X, Y );
		CreateNavNode (  NodeIndex, TransformInstanceLoc );
	}
	
	else
	{
		FTransform InstanceTransform;
		InstanceTransform.SetLocation ( InstanceTransformLocation );
		InstanceTransform.SetRotation ( FRotator::ZeroRotator.Quaternion ( ) );
		InstanceTransform.SetScale3D ( FVector3d::ZeroVector );

		ISMComponent->AddInstance ( InstanceTransform );
	}
	
}

void AGrid::CreateNavNode ( int index, FVector NavNodeLocation )
{ 
	UNavNodes* TempNode = NewObject<UNavNodes> ( );
	if ( TempNode )
	{
		TempNode->Reset ( );
		TempNode->NodeLoc = NavNodeLocation;
		GridArray[ index ] = TempNode;
		
		//DrawDebugSphere ( GetWorld ( ), NavNodeLocation, 50, 10, FColor::Red, false, 3.f );

	}
}

int AGrid::CalculateIndex ( int indexX, int indexY )
{
	
	return Size_Y * indexX + indexY;
}

void AGrid::Calculate2DIndex ( int Index, int& indexX, int& indexY )
{
	indexX = Index / Size_X;
	indexY = Index % Size_Y;
}

void AGrid::SetStartNode ( int Index )
{ 
	StartNode = nullptr;
	if ( Index >=0 && Index < GridArray.Num ( ) )
	{
		StartNode = GridArray[ Index ];
		//if ( StartNode )
		//{
		//	//DrawDebugSphere ( GetWorld ( ), StartNode->NodeLoc, 50, 10, FColor::Red, false, 3.f );
		//	GEngine->AddOnScreenDebugMessage ( -1, 15.0f, FColor::Yellow, TEXT ( "Start Node" ) );
		//}
		//else
		//{
		//	GEngine->AddOnScreenDebugMessage ( -1, 15.0f, FColor::Yellow, TEXT ( "No Start Node" ) );

		//}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage ( -1, 15.0f, FColor::Yellow, TEXT ( "No Start Node 2" ) );

	}
}

void AGrid::SetEndNode ( int Index )
{ 
	EndNode = nullptr;
	if ( Index >= 0 && Index < GridArray.Num ( ) )
	{
		EndNode = GridArray[ Index ];
		if ( EndNode )
		{
			//DrawDebugSphere ( GetWorld ( ), EndNode->NodeLoc, 50, 10, FColor::Blue, false, 3.f );

		}
	}
}

void AGrid::SetCurrentNode ( )
{ 
	CurrentNode = nullptr;
	if ( StartNode && EndNode)
	{
		StartNode->LocalGoal = 0.f;
		StartNode->GloablGoal = CalculateDistance ( StartNode, EndNode );
		CurrentNode = StartNode;
		//DrawDebugSphere ( GetWorld ( ), CurrentNode->NodeLoc, 70, 10, FColor::Green, false, 3.f);

	}
}

float AGrid::CalculateDistance ( UNavNodes* nodeA, UNavNodes* nodeB )
{
	float Distance = 0.f;
	if ( nodeA && nodeB )
	{
		Distance = UKismetMathLibrary::Vector_DistanceSquared ( nodeA->NodeLoc, nodeB->NodeLoc );
	}
	return Distance;
}

TArray<FVector> AGrid::CalculatAStar ( )
{
	TArray<FVector> FinalPathPoints;
	for ( UNavNodes* NavNode : GridArray )
	{
		if ( NavNode )
		{
			NavNode->Reset ( );
		}
	}

	if ( !StartNode || !EndNode )
	{
		GEngine->AddOnScreenDebugMessage ( -1, 15.0f, FColor::Yellow, TEXT ( "No Start/End node" ) );
		return TArray<FVector>();
	}
	//AsyncTask ( ENamedThreads::GameThread, [&] 
	//{
		SetCurrentNode ( );
		//GEngine->AddOnScreenDebugMessage ( -1, 15.0f, FColor::Yellow, TEXT ( "enter Astar" ) );
		if ( CurrentNode == nullptr )
		{
			return TArray<FVector>();
		}
		TArray<UNavNodes*> NonTestedNodes;
		NonTestedNodes.Add ( CurrentNode );
		while ( !NonTestedNodes.IsEmpty ( )  )
		{
			if ( CurrentNode == EndNode )
			{
				break;
			}
			//GEngine->AddOnScreenDebugMessage ( -1, 15.0f, FColor::Yellow, TEXT ( "1st while" ) );

			SortNodeGroup ( NonTestedNodes );
			while ( !NonTestedNodes.IsEmpty ( )  && NonTestedNodes[0]->bIsVisited )
			{
				//GEngine->AddOnScreenDebugMessage ( -1, 0.5f, FColor::Red, TEXT ( "Popping node" ) );
				//DrawDebugSphere ( GetWorld ( ), NonTestedNodes[ 0 ]->NodeLoc, 80, 10, FColor::Orange,false, 5.f );
				NonTestedNodes.RemoveAt(0);
			}

			if ( NonTestedNodes.IsEmpty ( ) )
			{
				GEngine->AddOnScreenDebugMessage ( -1, 0.5f, FColor::Red, TEXT ( "Test Node Empty" ) );
				if ( EndNode->ParentNode == nullptr )
				{
					GEngine->AddOnScreenDebugMessage ( -1, 0.5f, FColor::Red, TEXT ( "test Ndee empty no end node parent" ) );

				}

				break;
			}
			
			CurrentNode = NonTestedNodes[0];
			CurrentNode->bIsVisited = true;
			for ( UNavNodes* NeighbourNode : CurrentNode->NeighbourNode )
			{
				if ( NeighbourNode )
				{
					if ( !NeighbourNode->bIsVisited)
					{
						NonTestedNodes.Add ( NeighbourNode );
					}
					float NeighbourNodeLocalGoal = CalculateDistance ( CurrentNode, NeighbourNode ) + CurrentNode->LocalGoal;

					if ( NeighbourNodeLocalGoal  < NeighbourNode->LocalGoal )
					{
						NeighbourNode->ParentNode = CurrentNode;
						NeighbourNode->LocalGoal = NeighbourNodeLocalGoal;
						NeighbourNode->GloablGoal = NeighbourNode->LocalGoal + CalculateDistance ( NeighbourNode, EndNode );
					}
				}
			}

		}
			FinalPathPoints = OptimisePath ( );

		return FinalPathPoints;
	//} );
	
	//DrawPath ( );
	//TArray<FVector> FinalPathPoints = OptimisePath ( );
//	FinalPathPoints = OptimisePath ( );


	//return FinalPathPoints;
}


void AGrid::SortNodeGroup ( TArray<UNavNodes*>& NavGroup )
{
	NavGroup.Sort ( );

}

void AGrid::MergeSort ( TArray<UNavNodes*>& NavGroup, int Start, int End )
{
	if ( Start < End )
	{
		GEngine->AddOnScreenDebugMessage ( -1, 15.0f, FColor::Yellow, TEXT ( "Sort" ) );
		int Mid = ( Start + End ) / 2;
		MergeSort ( NavGroup, Start, Mid );
		MergeSort ( NavGroup, Mid+1, End );

		Merge ( NavGroup, Start, Mid, End );

	}
}

void AGrid::Merge ( TArray<UNavNodes*>&NavGroup, int Start, int Mid, int End )
{ 
	TArray<UNavNodes*> NavGroup1;
	TArray<UNavNodes*> NavGroup2;
	TArray<UNavNodes*> TempNavGroup;

	int Index1 = Mid - Start + 1;
	int Index2 = End - Mid;

	for ( int i = 0; i < Index1; i++ )
	{
		if(NavGroup.Num( ) > Start+i)
		NavGroup1.Add(NavGroup[ Start + i ]);
	}
	for ( int i = 0 + 1; i < Index2; i++ )
	{
		if ( NavGroup.Num ( )  > Mid + 1 + i )
		NavGroup2.Add(NavGroup[ Mid + 1 + i ]);
	}

	int PointerX = 0;
	int PointerY = 0;

	while ( PointerX < Index1 && PointerY < Index2 )
	{
		if ( (NavGroup1.Num()>PointerX && NavGroup2.Num() > PointerY) && (NavGroup1[PointerX] < NavGroup2[PointerY] ))
		{
			TempNavGroup.Add ( NavGroup[ PointerX ] );
			PointerX++;
		}
		else if ( NavGroup2.Num ( ) > PointerY )
		{
			TempNavGroup.Add ( NavGroup2[ PointerY ] );
			PointerY++;
		}
	}
	while ( PointerX < Index1 )
	{
		TempNavGroup.Add ( NavGroup1[ PointerX ] );
		PointerX++;
	}
	while ( PointerY < Index2 )
	{
		TempNavGroup.Add ( NavGroup2[ PointerY ] );
		PointerY++;
	}
	NavGroup = TempNavGroup;
}

void AGrid::DrawPath ( )
{
	//GEngine->AddOnScreenDebugMessage ( -1, 15.0f, FColor::Yellow, TEXT ( "Draw Path" ) );
	UNavNodes* TempNode = EndNode;
	//DrawDebugSphere ( GetWorld ( ), CurrentNode->NodeLoc + FVector(30,30,0.f ), 30, 10, FColor::Orange, false, 6.f );
	//DrawDebugSphere ( GetWorld ( ), EndNode->NodeLoc, 30, 10, FColor::Purple, false, 6.f );

	if ( EndNode->ParentNode == nullptr )
	{
		GEngine->AddOnScreenDebugMessage ( -1, 15.0f, FColor::Yellow, TEXT ( "No end Node Parent" ) );
		return;

	}
	TArray<FVector> Path = OptimisePath ( );

	//while ( TempNode->ParentNode != nullptr )
	for(const FVector& PathPoints: Path )
	{
		//GEngine->AddOnScreenDebugMessage ( -1, 15.0f, FColor::Yellow, TEXT ( "Drawn Node" ) );

		DrawDebugSphere ( GetWorld ( ), PathPoints, 30, 10, FColor::Purple, false, 6.f );
		//DrawDebugSphere ( GetWorld ( ), TempNode->NodeLoc, 30, 10, FColor::Black, false, 6.f );
		//TempNode = TempNode->ParentNode;
	}
}

TArray<FVector> AGrid::OptimisePath ( )
{
	UNavNodes* TempNode = EndNode;
	TArray<FVector> TempPathPoints;
	if ( TempNode->ParentNode == nullptr )
	{
		GEngine->AddOnScreenDebugMessage ( -1, 4.f, FColor::Red, TEXT ( "Null Parent Node" ) );
		return TempPathPoints;
	}
	while ( TempNode->ParentNode != nullptr )
	{
		TempPathPoints.Add ( TempNode->NodeLoc );
		//DrawDebugSphere ( GetWorld ( ), TempNode->NodeLoc, 20, 10, FColor::Black, false, 6.f );
		TempNode = TempNode->ParentNode;
	}
	return TempPathPoints;

	TArray<FVector> FinalPathPoints;
	FinalPathPoints.Add ( TempPathPoints[0] );
	FVector CurrentPoint ;
	FVector PrevPoint;
	int CurrentIndex = 0;
	int PrevIndex = 0;
	TArray<AActor*> ActorsToIgnore;
	FHitResult Hit;
	while(CurrentPoint != TempPathPoints.Last( ) )
	{
		CurrentPoint = TempPathPoints[ CurrentIndex ] ;
		PrevPoint = TempPathPoints[PrevIndex];

		bool bIsHit = UKismetSystemLibrary::SphereTraceSingle ( GetWorld ( ), 
																 CurrentPoint + FVector(0,0,50 ),
																 PrevPoint + FVector(0,0,50),
																 5,
																 UEngineTypes::ConvertToTraceType ( ECollisionChannel::ECC_Visibility ),
																 false, 
																 ActorsToIgnore,
																 EDrawDebugTrace::None, 
																 Hit, 
																 true );
		if ( bIsHit )
		{
		//	DrawDebugSphere ( GetWorld ( ), TempPathPoints[ CurrentIndex - 1 ], 50, 10, FColor::Red, false, 5 );
			FinalPathPoints.Add ( TempPathPoints[CurrentIndex-1] );
			PrevIndex = CurrentIndex-1;
		}
		else
		{
			CurrentIndex++;
		}
	}
	FinalPathPoints.Add ( TempPathPoints.Last ( ) );
	UE_LOG ( LogTemp, Warning, TEXT ( "Length of temp last points %i" ), TempPathPoints.Num ( ) );

	return FinalPathPoints;
}

// Called when the game starts or when spawned
void AGrid::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG ( LogTemp, Error, TEXT ( "Begin play" ) );

}

// Called every frame
void AGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//AsyncTask ( ENamedThreads::AnyHiPriThreadNormalTask, [&] ( )
			
