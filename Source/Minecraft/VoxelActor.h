// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "VoxelActor.generated.h"

struct FMeshSection
{
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FProcMeshTangent> Tangents;
	TArray<FColor> VertexColors;
	int32 ElementID = 0;
};


UCLASS()
class MINECRAFT_API AVoxelActor : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AVoxelActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray <UMaterialInterface *> Materials;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
		int32 randomSeed = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
		int32 voxelSize = 200;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
		int32 chunkLineElements = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
		int32 chunkXindex = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
		int32 chunkYindex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool collision = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float xMult = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float yMult = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Weight = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float freq = 1;


	UPROPERTY()
		int32 chunkTotalElements;
	UPROPERTY()
		int32 chunkZElements;
	UPROPERTY()
		int32 chunkLineElementsP2;
	UPROPERTY()
		int32 VoxelSizeHalf;
	UPROPERTY()
		TArray <int32> chunkFields;

	UPROPERTY()
		UProceduralMeshComponent* proceduralComponent;

	UFUNCTION(BlueprintNativeEvent)
		TArray <int32> calculateNoise();
	virtual TArray <int32> calculateNoise_Implementation();

private:
	void GenerateChunk();
	void UpdateMesh();
	bool inRange(int32 value, int32 range);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform & Transform) override;

	
	
};
