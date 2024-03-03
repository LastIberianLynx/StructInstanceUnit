# StructInstanceUnit
This project is an exercise to solve a prevalent issue when using Structs in Unreal Engine for unit data.
Its here as a back up of a solution, that is the prelude for an ECS system I might use in the future.

Normally units use the class AActor.
But when using instances of Instanced Static Mesh Component, its not worth it to use Actors because they already have a world representation.
If we use Actor, we are just doubling the physical representation.
We would also have to vinterp both the actor and the instance, when moving.

Having this in mind. We only really need a struct to store the data of a unit.
Though that comes with the inconvenience that it is not garbage collected.
We have no reliable way to verify if the struct is still valid.

To solve this issue, I created 2 variables that together identify a unit and check their validity.
First the UnitID that is FGuid, and the instance index in the ISM, that is also the index in the Array where we store all Unit structs.
By using these 2 variables we can make sure that wherever else we are working with the unit,
and wherever else we have it stored we can check if they are still valid.
Additionally i created a TMap to confirm if the unit has just changed its index or its actually removed from the game entirely.

So when we create a unit we create a struct FUnit, generate an ID, and get the index of the instance it corresponds to.
Then when we need to move this unit in the world.
We use these 2 variables only in an array.
As we iterate if for any reason the unit is no longer valid, the simple check will detect if it changed index or if it is destroyed.

To further expand this system into an ECS system i would have to then separate the index, the id, into 2 arrays components, and the location component array.
