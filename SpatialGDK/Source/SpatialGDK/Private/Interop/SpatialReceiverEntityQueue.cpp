#include "Interop/SpatialReceiverEntityQueue.h"
#include "Interop/SpatialStaticComponentView.h"
#include "Schema/SpawnData.h"

SpatialReceiverEntityQueue::SpatialReceiverEntityQueue() = default;
SpatialReceiverEntityQueue::~SpatialReceiverEntityQueue() noexcept = default;

FVector SpatialReceiverEntityQueue::position (Worker_EntityId id, USpatialStaticComponentView* comp_view) {
    const SpatialGDK::SpawnData* const data = comp_view->GetComponentData<SpatialGDK::SpawnData>(id);
    if (data)
        return data->Location;
    else
        return FVector(0.0f);
}
