// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CPPController.generated.h"

/**
 * 
 */
UCLASS()
class MINECRAFT_API ACPPController : public APlayerController
{
	GENERATED_BODY()

	public:
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float RandomSeed = 12345;
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			FVector2D chunkCoords;
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			TArray<AActor*> Chunks;
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float ChunkX;
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float ChunkY;
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			FVector CharacterPosition;
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float ChunkSizeHalf;
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float ChunkSize;
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float VoxelSize = 100;
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float ChunkLineElements = 20;
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float RenderRange = 3;
	
	public:
		virtual void Tick(float DeltaTime) override;


	protected:
		virtual void BeginPlay() override;
	
};
