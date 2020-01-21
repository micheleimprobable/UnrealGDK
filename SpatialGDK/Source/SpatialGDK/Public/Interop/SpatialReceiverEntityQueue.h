#pragma once

#include "StdArray.h"
#include <WorkerSDK/improbable/c_worker.h>
#include <Containers/Array.h>
#include <utility>
#include <algorithm>

class USpatialPackageMapClient;

class SpatialReceiverEntityQueue {
public:
    typedef StdArray<Worker_EntityId> ActorQueue;
    typedef StdArray<Worker_EntityId, true> ConstActorQueue;
    typedef ActorQueue::size_type SizeType;

    SpatialReceiverEntityQueue();
    ~SpatialReceiverEntityQueue() noexcept;

    ConstActorQueue low_prio_queue() const { return ConstActorQueue(&m_low_prio); }
    ConstActorQueue high_prio_queue() const { return ConstActorQueue(&m_high_prio); }
    ActorQueue low_prio_queue() { return ActorQueue(&m_low_prio); }
    ActorQueue high_prio_queue() { return ActorQueue(&m_high_prio); }

    static AActor* to_actor (Worker_EntityId id, USpatialPackageMapClient* PackageMap);

private:
    TArray<Worker_EntityId> m_high_prio;
    TArray<Worker_EntityId> m_low_prio;
};
