// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitManager.h"


// Sets default values
AUnitManager::AUnitManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ISM_Units = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("ISMUnits"));
	//ISM_Units->SetupAttachment(CollisionMesh);
	ISM_Units->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ISM_Units->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ISM_Units->CanCharacterStepUp(false);
	ISM_Units->SetCastShadow(false);
	ISM_Units->SetComponentTickEnabled(false);
	ISM_Units->NumCustomDataFloats = 1;
	ISM_Units->SetGenerateOverlapEvents(false);
	ISM_Units->SetSimulatePhysics(false);

}

// Called when the game starts or when spawned
void AUnitManager::BeginPlay()
{
	Super::BeginPlay();
	SpawnUnit(FVector(100, 100, 100));
	int32 Index = SpawnUnit(FVector(400, 400, 400));


	SpawnUnit(FVector(800, 800, 800));
	FGuid Guid2;
	//DestroyUnit(Guid2, Index);
	CheckValid(Index, &Guid2);
	CheckValidAndDeadFix(Index, &Guid2);

	FUnitRef UnitRef;
	UnitRef.UnitID = AllUnits[Index].UnitID;
	UnitRef.UnitIndex = Index;
		
	UnitsMovingForward.Add(UnitRef);
	FTimerHandle T; GetWorld()->GetTimerManager().SetTimer(T, [&, Index]() {
		FGuid Guid2;
		DestroyUnit(Guid2, Index);
		//And now move the unit 1 again that should be the 3rd one.
		UnitRef.UnitID = AllUnits[1].UnitID;
		UnitRef.UnitIndex = 1;
		UnitsMovingForward.Add(UnitRef);

		}, 4.f, false);
}

// Called every frame
void AUnitManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FTransform T;
	FVector	AdvanceLocation;
	int32 CurUnit;
	for (int32 i = UnitsMovingForward.Num() - 1; i >= 0; --i) {
		if (!CheckValidAndDeadFix(UnitsMovingForward[i].UnitIndex, &UnitsMovingForward[i].UnitID)) {
			continue;
		}
		CurUnit = UnitsMovingForward[i].UnitIndex;
		AdvanceLocation = FMath::VInterpConstantTo(AllUnits[CurUnit].WorldLocation, FVector(5000.f, 5000.f, 0.f), DeltaTime, 10.f/*A->Cur_Speed*/);
		AllUnits[CurUnit].WorldLocation = AdvanceLocation;
		T.SetTranslation(AdvanceLocation);
		ISM_Units->UpdateInstanceTransform(CurUnit, T, true);
	}
	ISM_Units->MarkRenderStateDirty();
}

int32 AUnitManager::SpawnUnit(FVector Location) {
	FRotator Rot = FRotator();
	FUnit Unit;
	Unit.UnitIndex = ISM_Units->GetInstanceCount();
	Unit.WorldLocation = Location;
	Unit.UnitID = FGuid::NewGuid();
	FTransform T = FTransform(Unit.WorldLocation);
	ISM_Units->AddInstanceWorldSpace(T);
	//Unit.Checkpoints.Add(Unit.WorldLocation + FVector(100.f, 100.f, 0)); //Move Forward

	AllUnits.Add(Unit);
	ID_INDEX_MAP.Add(Unit.UnitID, Unit.UnitIndex);
	return Unit.UnitIndex;
}


void AUnitManager::DestroyUnit(FGuid UnitId, int32 UnitIndex) {
	//When destroying an instance of ISM (different than HISM), all instances above the index will be reordered.
	//So we need to do the swap manually to keep their indexes intact.
	//With HISM, it does the swap automatically, though we are using ISM, 
	//that appropriate for when you have individual LODS.

	if (UnitIndex == -1) {
		UnitIndex = ID_INDEX_MAP.FindRef(UnitId);

	}
	else {
		if (!UnitId.IsValid()) {
			UnitId = *ID_INDEX_MAP.FindKey(UnitIndex);
			// Or get it from the array, though this might be unsafe
			// UnitId = AllUnits[UnitIndex].UnitID; //its faster
		}
	}

	AllUnits[UnitIndex] = AllUnits.Last(); //First we copy the last to the index we want to destroy.

	//ISM_Units->GetInstanceCount() - 1;
	FTransform T;
	ISM_Units->GetInstanceTransform(AllUnits.Num() - 1, T, true);
	ISM_Units->UpdateInstanceTransform(UnitIndex, T, true, true, false);
	//Must update the Map
	//Remove the destroyed unit from the map
	//And update the last unit index because it now replaces the index of the destroyed unit.
	ID_INDEX_MAP.Remove(UnitId);
	ID_INDEX_MAP[AllUnits.Last().UnitID] = UnitIndex;
	//This will copy the last instance to the current instance we want to destroy. 
	//effectively destroying the current unit physical representation.
	//Then we need to destroy the copy at the last index.
	//If there are custom data floats that change color of the instance these must be also copied.
	ISM_Units->RemoveInstance(AllUnits.Num()-1);
	AllUnits.RemoveAt(AllUnits.Num() - 1);

}

bool AUnitManager::CheckValid(int32& Index, FGuid* UnitId) {
	//If the unit dies the unitid and the unit index will be off, So we will check if its valid.
	//And then double check if its really dead or if it just changed index in the ID_INDEX_MAP
	if (AllUnits[Index].UnitID == *UnitId)
		return true;
	else
		return false;
}

bool AUnitManager::CheckValidAndDeadFix(int32& Index, FGuid* UnitId) {
	if (!CheckValid(Index, UnitId)) {
		//Now check if it just changed Index or if it is actually dead in the TMap
		if (ID_INDEX_MAP.Contains(*UnitId)) {
			Index = ID_INDEX_MAP[*UnitId];
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return true;
	}
}
