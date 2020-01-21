#include "Interop/SpatialReceiverEntityQueue.h"
#include "EngineClasses/SpatialPackageMapClient.h"

SpatialReceiverEntityQueue::SpatialReceiverEntityQueue() = default;
SpatialReceiverEntityQueue::~SpatialReceiverEntityQueue() noexcept = default;

AActor* SpatialReceiverEntityQueue::to_actor (Worker_EntityId id, USpatialPackageMapClient* PackageMap) {
    return Cast<AActor>(PackageMap->GetObjectFromEntityId(id));
}
