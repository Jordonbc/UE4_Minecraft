// Fill out your copyright notice in the Description page of Project Settings.


#include "VoxelActor.h"

const int32 bTriangles[] = { 2, 1, 0, 0, 3, 2 };
const FVector2D bUVs[] = {FVector2D(0.0, 0.0), FVector2D(0.0, 1.0), FVector2D(1.0, 1.0), FVector2D(1.0, 0.0)};
const FVector bNormals0[] = {FVector(0, 0, 1), FVector(0, 0, 1), FVector(0, 0, 1), FVector(0, 0, 1)};
const FVector bNormals1[] = { FVector(0, 0, -1), FVector(0, 0, -1), FVector(0, 0, -1), FVector(0, 0, -1) };
const FVector bNormals2[] = { FVector(0, 1, 0), FVector(0, 1, 0), FVector(0, 1, 0), FVector(0, 1, 0) };
const FVector bNormals3[] = { FVector(0, -1, 0), FVector(0, -1, 0), FVector(0, -1, 0), FVector(0, -1, 0) };	
const FVector bNormals4[] = { FVector(1, 0, 0), FVector(1, 0, 0), FVector(1, 0, 0), FVector(1, 0, 0) };
const FVector bNormals5[] = { FVector(-1, 0, 0), FVector(-1, 0, 0), FVector(-1, 0, 0), FVector(-1, 0, 0) };
const FVector bMask[] = { FVector(0, 0, 1), FVector(0, 0, -1), FVector(0, 1, 0), FVector(0, -1, 0), FVector(1, 0, 0), FVector(-1, 0, 0) };



// Sets default values
AVoxelActor::AVoxelActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AVoxelActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVoxelActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AVoxelActor::OnConstruction(const FTransform & Transform)
{
	FName YourObjectName("Test");
	chunkZElements = 80;
	chunkTotalElements = chunkLineElements * chunkLineElements	* chunkZElements;
	chunkLineElementsP2 = chunkLineElements * chunkLineElements;
	VoxelSizeHalf = voxelSize / 2;

	FString string = "voxel_" + FString::FromInt(chunkXindex) + "+" + FString::FromInt(chunkYindex);
	FName name = FName(*string);
	proceduralComponent = NewObject<UProceduralMeshComponent>(this, name);
	proceduralComponent->RegisterComponent();

	RootComponent = proceduralComponent;
	RootComponent->SetWorldTransform(Transform);

	RootComponent->SetMobility(EComponentMobility::Static);


	Super::OnConstruction(Transform);

	GenerateChunk();
	UpdateMesh();
}


void AVoxelActor::GenerateChunk()
{
	FRandomStream RandomStream = FRandomStream(randomSeed);
	TArray<FIntVector> TreeCenters;

	chunkFields.SetNumUninitialized(chunkTotalElements);

	TArray <int32> noise = calculateNoise();


	for (int32 x = 0; x < chunkLineElements; x++)
	{
		for (int32 y = 0; y < chunkLineElements; y++)
		{
			for (int32 z = 0; z < chunkZElements; z++)
			{
				int32 index = x + (y * chunkLineElements) + (z * chunkLineElementsP2);

				//chunkFields[index] = (z < 30) ? 1 : 0; // Generate no noise

				if (z == 31 + noise[x + y * chunkLineElements] && RandomStream.FRand() < 0.02)
					TreeCenters.Add(FIntVector(x, y, z)); // WOOD
				else if (z == 30 + noise[x + y * chunkLineElements])
					chunkFields[index] = 1; // GRASS
				else if (z >= 30 - 3 + noise[x + y * chunkLineElements] && z < 30 + noise[x + y * chunkLineElements])
					chunkFields[index] = 2; // DIRT

				else if (z < 30 - 3 + noise[x + y * chunkLineElements])
					chunkFields[index] = 3; // STONE
				else chunkFields[index] = 0; // AIR




				/*if (z == 30 + noise[x + y * chunkLineElements])
				chunkFields[index] = 1;
				else if (z >= 27 + noise[x + y * chunkLineElements] && z < 30 + noise[x + y * chunkLineElements])
					chunkFields[index] = 2;

				else if (z < 27 + noise[x + y * chunkLineElements])
					chunkFields[index] = 3;
				else chunkFields[index] = 0;*/

				//chunkFields[index] = (z == 29 + noise[x + y * chunkLineElements]) ? 2 : 0;
				//chunkFields[index] = (z < 29 + noise[x + y * chunkLineElements]) ? 3 : 0;
			}
		}
	}

	for (FIntVector TreeCenter : TreeCenters)
	{
		int32 TreeHeight = RandomStream.RandRange(3, 6);
		int32 randomX = RandomStream.RandRange(0, 2);
		int32 randomY = RandomStream.RandRange(0, 2);
		int32 randomZ = RandomStream.RandRange(0, 2);

		for (int32 TreeX = -2; TreeX < 3; TreeX++)
		{
			for (int32 TreeY = -2; TreeY < 3; TreeY++)
			{
				for (int32 TreeZ = -2; TreeZ < 3; TreeZ++)
				{
					if (inRange(TreeX + TreeCenter.X, chunkLineElements) && inRange(TreeY + TreeCenter.Y, chunkLineElements) && inRange(TreeZ + TreeCenter.Z, chunkZElements))
					{
						float radius = FVector(TreeX * randomX, TreeY * randomY, TreeZ * randomZ).Size();

						if (radius <= 2.8)
						{
							if (RandomStream.FRand() < 0.5 || radius <= 1.2)
							{
								chunkFields[TreeCenter.X + TreeX + (chunkLineElements * (TreeCenter.Y + TreeY)) + (chunkLineElementsP2 * (TreeCenter.Z + TreeZ + TreeHeight))] = 21;// LEAF
							}
						}
					}
				}
			}
		}

	}
}

void AVoxelActor::UpdateMesh()
{
	TArray<FMeshSection> meshSections;
	meshSections.SetNum(Materials.Num());
	int32 el_num = 0;

	for (int32 x = 0; x < chunkLineElements; x++)
	{
		for (int32 y = 0; y < chunkLineElements; y++)
		{
			for (int32 z = 0; z < chunkZElements; z++)
			{
				int32 index = x + (chunkLineElements * y) + (chunkLineElementsP2 * z);
				int32 meshIndex = chunkFields[index];

				if (meshIndex > 0)
				{
					meshIndex--;

					TArray<FVector> &Vertices = meshSections[meshIndex].Vertices;
					TArray<int32> &Triangles = meshSections[meshIndex].Triangles;
					TArray<FVector> &Normals = meshSections[meshIndex].Normals;
					TArray<FVector2D> &UVs = meshSections[meshIndex].UVs;
					TArray<FProcMeshTangent> &Tangents = meshSections[meshIndex].Tangents;
					TArray<FColor> &VertexColors = meshSections[meshIndex].VertexColors;
					int32 ElementID = meshSections[meshIndex].ElementID;

					int triangle_num = 0;
					for (int i = 0; i < 6; i++)
					{
						int NewIndex = index + bMask[i].X + (bMask[i].Y * chunkLineElements) + (bMask[i].Z * chunkLineElementsP2);

						bool flag = false;
						if (meshIndex >= 20) flag = true;
						else if ((x + bMask[i].X < chunkLineElements) && (x + bMask[i].X >= 0) && (y + bMask[i].Y < chunkLineElements) && (y + bMask[i].Y >= 0))
						{
							if (NewIndex < chunkFields.Num() && NewIndex >= 0)
								if (chunkFields[NewIndex] < 1) flag = true;
						}
						else flag = true;
						if (flag)
						{
							Triangles.Add(bTriangles[0] + triangle_num + ElementID);
							Triangles.Add(bTriangles[1] + triangle_num + ElementID);
							Triangles.Add(bTriangles[2] + triangle_num + ElementID);
							Triangles.Add(bTriangles[3] + triangle_num + ElementID);
							Triangles.Add(bTriangles[4] + triangle_num + ElementID);
							Triangles.Add(bTriangles[5] + triangle_num + ElementID);
							triangle_num += 4;

							switch (i)
							{
							case 0: {
								Vertices.Add(FVector(-VoxelSizeHalf + (x * voxelSize), VoxelSizeHalf + (y * voxelSize), VoxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(-VoxelSizeHalf + (x * voxelSize), -VoxelSizeHalf + (y * voxelSize), VoxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(VoxelSizeHalf + (x * voxelSize), -VoxelSizeHalf + (y * voxelSize), VoxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(VoxelSizeHalf + (x * voxelSize), VoxelSizeHalf + (y * voxelSize), VoxelSizeHalf + (z * voxelSize)));

								Normals.Append(bNormals0, ARRAY_COUNT(bNormals0));
								break;
							}

							case 1: {
								Vertices.Add(FVector(VoxelSizeHalf + (x * voxelSize), -VoxelSizeHalf + (y * voxelSize), -VoxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(-VoxelSizeHalf + (x * voxelSize), -VoxelSizeHalf + (y * voxelSize), -VoxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(-VoxelSizeHalf + (x * voxelSize), VoxelSizeHalf + (y * voxelSize), -VoxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(VoxelSizeHalf + (x * voxelSize), VoxelSizeHalf + (y * voxelSize), -VoxelSizeHalf + (z * voxelSize)));

								Normals.Append(bNormals1, ARRAY_COUNT(bNormals1));
								break;
							}

							case 2: {
								Vertices.Add(FVector(VoxelSizeHalf + (x * voxelSize), VoxelSizeHalf + (y * voxelSize), VoxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(VoxelSizeHalf + (x * voxelSize), VoxelSizeHalf + (y * voxelSize), -VoxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(-VoxelSizeHalf + (x * voxelSize), VoxelSizeHalf + (y * voxelSize), -VoxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(-VoxelSizeHalf + (x * voxelSize), VoxelSizeHalf + (y * voxelSize), VoxelSizeHalf + (z * voxelSize)));

								Normals.Append(bNormals2, ARRAY_COUNT(bNormals2));
								break;
							}

							case 3: {
								Vertices.Add(FVector(-VoxelSizeHalf + (x * voxelSize), -VoxelSizeHalf + (y * voxelSize), VoxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(-VoxelSizeHalf + (x * voxelSize), -VoxelSizeHalf + (y * voxelSize), -VoxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(VoxelSizeHalf + (x * voxelSize), -VoxelSizeHalf + (y * voxelSize), -VoxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(VoxelSizeHalf + (x * voxelSize), -VoxelSizeHalf + (y * voxelSize), VoxelSizeHalf + (z * voxelSize)));

								Normals.Append(bNormals3, ARRAY_COUNT(bNormals3));
								break;
							}

							case 4: {
								Vertices.Add(FVector(VoxelSizeHalf + (x * voxelSize), -VoxelSizeHalf + (y * voxelSize), VoxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(VoxelSizeHalf + (x * voxelSize), -VoxelSizeHalf + (y * voxelSize), -VoxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(VoxelSizeHalf + (x * voxelSize), VoxelSizeHalf + (y * voxelSize), -VoxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(VoxelSizeHalf + (x * voxelSize), VoxelSizeHalf + (y * voxelSize), VoxelSizeHalf + (z * voxelSize)));

								Normals.Append(bNormals4, ARRAY_COUNT(bNormals4));
								break;
							}

							case 5: {
								Vertices.Add(FVector(-VoxelSizeHalf + (x * voxelSize), VoxelSizeHalf + (y * voxelSize), VoxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(-VoxelSizeHalf + (x * voxelSize), VoxelSizeHalf + (y * voxelSize), -VoxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(-VoxelSizeHalf + (x * voxelSize), -VoxelSizeHalf + (y * voxelSize), -VoxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(-VoxelSizeHalf + (x * voxelSize), -VoxelSizeHalf + (y * voxelSize), VoxelSizeHalf + (z * voxelSize)));

								Normals.Append(bNormals5, ARRAY_COUNT(bNormals5));
								break;
							}
							}
							UVs.Append(bUVs, ARRAY_COUNT(bUVs));

							FColor color = FColor(255, 255, 255, i);
							VertexColors.Add(color); VertexColors.Add(color); VertexColors.Add(color); VertexColors.Add(color);
						}
					}
					el_num += triangle_num;
					meshSections[meshIndex].ElementID += triangle_num;
				}
			}
		}
	}

	proceduralComponent->ClearAllMeshSections();

	for (int i = 0; i < meshSections.Num(); i++)
	{
		if (meshSections[i].Vertices.Num() > 0)
			proceduralComponent->CreateMeshSection(i, meshSections[i].Vertices, meshSections[i].Triangles, meshSections[i].Normals, meshSections[i].UVs, meshSections[i].VertexColors, meshSections[i].Tangents, collision);
	}


	int s = 0;
	while (s < Materials.Num())
	{
		proceduralComponent->SetMaterial(s, Materials[s]);
		s++;
	}

}

TArray<int32> AVoxelActor::calculateNoise_Implementation()
{
	TArray <int32> aa;
	aa.SetNum(chunkLineElementsP2);
	return aa;
}

bool AVoxelActor::inRange(int32 value, int32 range)
{
	return (value >= 0 && value < range);
}








//void AVoxelActor::UpdateMesh() // NO NOISE WORKING
//{
//	TArray<FVector> Vertices;
//	TArray<int32> Triangles;
//	TArray<FVector> Normals;
//	TArray<FVector2D> UVs;
//	TArray<FProcMeshTangent> Tangents;
//	TArray<FColor> VertexColors;
//
//	int32 ElementID = 0;
//
//	for (int32 x = 0; x < chunkLineElements; x++)
//	{
//		for (int32 y = 0; y < chunkLineElements; y++)
//		{
//			for (int32 z = 0; z < chunkZElements; z++)
//			{
//				int32 index = x + (chunkLineElements * y) + (chunkLineElementsP2 * z);
//				int32 meshIndex = chunkFields[index];
//
//				if (meshIndex > 0)
//				{
//
//					meshIndex--;
//
//					int triangle_num = 0;
//					for (int i = 0; i < 6; i++)
//					{
//						int NewIndex = index + bMask[i].X + (bMask[i].Y * chunkLineElements) + (bMask[i].Z * chunkLineElementsP2);
//
//						bool flag = false;
//						if (meshIndex >= 20) flag = true;
//						else if ((x + bMask[i].X < chunkLineElements) && (x + bMask[i].X >= 0) && (y + bMask[i].Y < chunkLineElements) && (y + bMask[i].Y >= 0))
//						{
//							if (NewIndex < chunkFields.Num() && NewIndex >= 0)
//								if (chunkFields[NewIndex] < 1) flag = true;
//						}
//						else flag = true;
//						if (flag)
//						{
//							Triangles.Add(bTriangles[0] + triangle_num + ElementID);
//							Triangles.Add(bTriangles[1] + triangle_num + ElementID);
//							Triangles.Add(bTriangles[2] + triangle_num + ElementID);
//							Triangles.Add(bTriangles[3] + triangle_num + ElementID);
//							Triangles.Add(bTriangles[4] + triangle_num + ElementID);
//							Triangles.Add(bTriangles[5] + triangle_num + ElementID);
//							triangle_num += 4;
//
//							switch (i)
//							{
//							case 0: {
//								Vertices.Add(FVector(-VoxelSizeHalf + (x * voxelSize), VoxelSizeHalf + (y * voxelSize), VoxelSizeHalf + (z * voxelSize)));
//								Vertices.Add(FVector(-VoxelSizeHalf + (x * voxelSize), -VoxelSizeHalf + (y * voxelSize), VoxelSizeHalf + (z * voxelSize)));
//								Vertices.Add(FVector(VoxelSizeHalf + (x * voxelSize), -VoxelSizeHalf + (y * voxelSize), VoxelSizeHalf + (z * voxelSize)));
//								Vertices.Add(FVector(VoxelSizeHalf + (x * voxelSize), VoxelSizeHalf + (y * voxelSize), VoxelSizeHalf + (z * voxelSize)));
//
//								Normals.Append(bNormals0, ARRAY_COUNT(bNormals0));
//								break;
//							}
//
//							case 1: {
//								Vertices.Add(FVector(VoxelSizeHalf + (x * voxelSize), -VoxelSizeHalf + (y * voxelSize), -VoxelSizeHalf + (z * voxelSize)));
//								Vertices.Add(FVector(-VoxelSizeHalf + (x * voxelSize), -VoxelSizeHalf + (y * voxelSize), -VoxelSizeHalf + (z * voxelSize)));
//								Vertices.Add(FVector(-VoxelSizeHalf + (x * voxelSize), VoxelSizeHalf + (y * voxelSize), -VoxelSizeHalf + (z * voxelSize)));
//								Vertices.Add(FVector(VoxelSizeHalf + (x * voxelSize), VoxelSizeHalf + (y * voxelSize), -VoxelSizeHalf + (z * voxelSize)));
//
//								Normals.Append(bNormals1, ARRAY_COUNT(bNormals1));
//								break;
//							}
//
//							case 2: {
//								Vertices.Add(FVector(VoxelSizeHalf + (x * voxelSize), VoxelSizeHalf + (y * voxelSize), VoxelSizeHalf + (z * voxelSize)));
//								Vertices.Add(FVector(VoxelSizeHalf + (x * voxelSize), VoxelSizeHalf + (y * voxelSize), -VoxelSizeHalf + (z * voxelSize)));
//								Vertices.Add(FVector(-VoxelSizeHalf + (x * voxelSize), VoxelSizeHalf + (y * voxelSize), -VoxelSizeHalf + (z * voxelSize)));
//								Vertices.Add(FVector(-VoxelSizeHalf + (x * voxelSize), VoxelSizeHalf + (y * voxelSize), VoxelSizeHalf + (z * voxelSize)));
//
//								Normals.Append(bNormals2, ARRAY_COUNT(bNormals2));
//								break;
//							}
//
//							case 3: {
//								Vertices.Add(FVector(-VoxelSizeHalf + (x * voxelSize), -VoxelSizeHalf + (y * voxelSize), VoxelSizeHalf + (z * voxelSize)));
//								Vertices.Add(FVector(-VoxelSizeHalf + (x * voxelSize), -VoxelSizeHalf + (y * voxelSize), -VoxelSizeHalf + (z * voxelSize)));
//								Vertices.Add(FVector(VoxelSizeHalf + (x * voxelSize), -VoxelSizeHalf + (y * voxelSize), -VoxelSizeHalf + (z * voxelSize)));
//								Vertices.Add(FVector(VoxelSizeHalf + (x * voxelSize), -VoxelSizeHalf + (y * voxelSize), VoxelSizeHalf + (z * voxelSize)));
//
//								Normals.Append(bNormals3, ARRAY_COUNT(bNormals3));
//								break;
//							}
//
//							case 4: {
//								Vertices.Add(FVector(VoxelSizeHalf + (x * voxelSize), -VoxelSizeHalf + (y * voxelSize), VoxelSizeHalf + (z * voxelSize)));
//								Vertices.Add(FVector(VoxelSizeHalf + (x * voxelSize), -VoxelSizeHalf + (y * voxelSize), -VoxelSizeHalf + (z * voxelSize)));
//								Vertices.Add(FVector(VoxelSizeHalf + (x * voxelSize), VoxelSizeHalf + (y * voxelSize), -VoxelSizeHalf + (z * voxelSize)));
//								Vertices.Add(FVector(VoxelSizeHalf + (x * voxelSize), VoxelSizeHalf + (y * voxelSize), VoxelSizeHalf + (z * voxelSize)));
//
//								Normals.Append(bNormals4, ARRAY_COUNT(bNormals4));
//								break;
//							}
//
//							case 5: {
//								Vertices.Add(FVector(-VoxelSizeHalf + (x * voxelSize), VoxelSizeHalf + (y * voxelSize), VoxelSizeHalf + (z * voxelSize)));
//								Vertices.Add(FVector(-VoxelSizeHalf + (x * voxelSize), VoxelSizeHalf + (y * voxelSize), -VoxelSizeHalf + (z * voxelSize)));
//								Vertices.Add(FVector(-VoxelSizeHalf + (x * voxelSize), -VoxelSizeHalf + (y * voxelSize), -VoxelSizeHalf + (z * voxelSize)));
//								Vertices.Add(FVector(-VoxelSizeHalf + (x * voxelSize), -VoxelSizeHalf + (y * voxelSize), VoxelSizeHalf + (z * voxelSize)));
//
//								Normals.Append(bNormals5, ARRAY_COUNT(bNormals5));
//								break;
//							}
//							}
//							UVs.Append(bUVs, ARRAY_COUNT(bUVs));
//
//							FColor color = FColor(255, 255, 255, i);
//							VertexColors.Add(color); VertexColors.Add(color); VertexColors.Add(color); VertexColors.Add(color);
//						}
//					}
//					ElementID += triangle_num;
//				}
//			}
//		}
//	}
//
//
//	proceduralComponent->ClearAllMeshSections();
//	proceduralComponent->CreateMeshSection(0, Vertices, Triangles, Normals, UVs, VertexColors, Tangents, true);
//
//}
