// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/PackageMapClient.h"

#include "Schema/UnrealMetadata.h"
#include "Schema/UnrealObjectRef.h"

#include <WorkerSDK/improbable/c_worker.h>

#include "SpatialPackageMapClient.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSpatialPackageMap, Log, All);

class USpatialClassInfoManager;
class USpatialNetDriver;

UCLASS()
class SPATIALGDK_API USpatialPackageMapClient : public UPackageMapClient
{
	GENERATED_BODY()		
public:
	void Init(USpatialNetDriver* NetDriver);

	Worker_EntityId AllocateEntityIdAndResolveActor(AActor* Actor);
	FNetworkGUID TryResolveObjectAsEntity(UObject* Value);

	bool IsEntityIdPendingCreation(Worker_EntityId EntityId) const;
	void RemovePendingCreationEntityId(Worker_EntityId EntityId);

	FNetworkGUID ResolveEntityActor(AActor* Actor, Worker_EntityId EntityId);
	void ResolveSubobject(UObject* Object, const FUnrealObjectRef& ObjectRef);

	void RemoveEntityActor(Worker_EntityId EntityId);
	void RemoveSubobject(const FUnrealObjectRef& ObjectRef);

	// This function is ONLY used in SpatialReceiver::GetOrCreateActor to undo
	// the unintended registering of objects when looking them up with static paths.
	void UnregisterActorObjectRefOnly(const FUnrealObjectRef& ObjectRef);

	FNetworkGUID ResolveStablyNamedObject(UObject* Object);
	
	FUnrealObjectRef GetUnrealObjectRefFromNetGUID(const FNetworkGUID& NetGUID) const;
	FNetworkGUID GetNetGUIDFromUnrealObjectRef(const FUnrealObjectRef& ObjectRef) const;
	FNetworkGUID GetNetGUIDFromEntityId(const Worker_EntityId& EntityId) const;

	TWeakObjectPtr<UObject> GetObjectFromUnrealObjectRef(const FUnrealObjectRef& ObjectRef);
	TWeakObjectPtr<UObject> GetObjectFromEntityId(const Worker_EntityId& EntityId);
	FUnrealObjectRef GetUnrealObjectRefFromObject(UObject* Object);
	Worker_EntityId GetEntityIdFromObject(const UObject* Object);

	// Expose FNetGUIDCache::CanClientLoadObject so we can include this info with UnrealObjectRef.
	bool CanClientLoadObject(UObject* Object);

	virtual bool SerializeObject(FArchive& Ar, UClass* InClass, UObject*& Obj, FNetworkGUID *OutNetGUID = NULL) override;

private:
	UPROPERTY()
	USpatialClassInfoManager* ClassInfoManager;

	UPROPERTY()
	USpatialNetDriver* NetDriver;

	// Entities that have been assigned on this server and not created yet
	TSet<Worker_EntityId_Key> PendingCreationEntityIds;
};

class SPATIALGDK_API FSpatialNetGUIDCache : public FNetGUIDCache
{
public:
	FSpatialNetGUIDCache(class USpatialNetDriver* InDriver);
		
	FNetworkGUID AssignNewEntityActorNetGUID(AActor* Actor, Worker_EntityId EntityId);
	void AssignNewSubobjectNetGUID(UObject* Subobject, const FUnrealObjectRef& SubobjectRef);

	void RemoveEntityNetGUID(Worker_EntityId EntityId);
	void RemoveSubobjectNetGUID(const FUnrealObjectRef& SubobjectRef);

	FNetworkGUID AssignNewStablyNamedObjectNetGUID(UObject* Object);
	
	FNetworkGUID GetNetGUIDFromUnrealObjectRef(const FUnrealObjectRef& ObjectRef);
	FUnrealObjectRef GetUnrealObjectRefFromNetGUID(const FNetworkGUID& NetGUID) const;
	FNetworkGUID GetNetGUIDFromEntityId(Worker_EntityId EntityId) const;

	void NetworkRemapObjectRefPaths(FUnrealObjectRef& ObjectRef, bool bReading) const;

	// This function is ONLY used in SpatialPackageMapClient::UnregisterActorObjectRefOnly
	// to undo the unintended registering of objects when looking them up with static paths.
	void UnregisterActorObjectRefOnly(const FUnrealObjectRef& ObjectRef);

private:
	FNetworkGUID GetNetGUIDFromUnrealObjectRefInternal(const FUnrealObjectRef& ObjectRef);

	FNetworkGUID GetOrAssignNetGUID_SpatialGDK(UObject* Object);
	void RegisterObjectRef(FNetworkGUID NetGUID, const FUnrealObjectRef& ObjectRef);
	
	FNetworkGUID RegisterNetGUIDFromPathForStaticObject(const FString& PathName, const FNetworkGUID& OuterGUID, bool bNoLoadOnClient);
	FNetworkGUID GenerateNewNetGUID(const int32 IsStatic);

	TMap<FNetworkGUID, FUnrealObjectRef> NetGUIDToUnrealObjectRef;
	TMap<FUnrealObjectRef, FNetworkGUID> UnrealObjectRefToNetGUID;
};

