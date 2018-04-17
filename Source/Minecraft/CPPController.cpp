// Fill out your copyright notice in the Description page of Project Settings.

#include "CPPController.h"

void ACPPController::BeginPlay()
{
	Super::BeginPlay();

	ChunkSize = ChunkLineElements * VoxelSize;

	ChunkSizeHalf = ChunkSize / 1;

}

void ACPPController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}