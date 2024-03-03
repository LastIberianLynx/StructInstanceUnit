// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UnitManager.generated.h"


USTRUCT(BlueprintType)
struct STRUCTINSTANCEUNIT_API FUnit
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FGuid UnitID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int32 UnitIndex; //Both instance index and AllUnits array index

	UPROPERTY()
		FVector WorldLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		APlayerState* PlayerOwner; //can be playerpawn, check roman dawn.

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FVector> Checkpoints;

};

USTRUCT(BlueprintType)
struct STRUCTINSTANCEUNIT_API FUnitRef
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FGuid UnitID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int32 UnitIndex; //Both instance index and AllUnits array index

};



UCLASS()
class STRUCTINSTANCEUNIT_API AUnitManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUnitManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) /*, meta = (ExposeOnSpawn = "true")*/
		UInstancedStaticMeshComponent* ISM_Units;

	TArray<FUnit> AllUnits;
	TMap<FGuid, int32> ID_INDEX_MAP; //Everytime a unit dies we will use this map to make sure everything stays in line.

	int32 SpawnUnit(FVector Location);
	void DestroyUnit(FGuid UnitId, int32 UnitIndex = -1);

	bool CheckValid(int32& Index, FGuid* UnitId); //this is very important, should be called before any operation with a unit

	bool CheckValidAndDeadFix(int32& Index, FGuid* UnitId);

	TArray<FUnitRef> UnitsMovingForward;
};
