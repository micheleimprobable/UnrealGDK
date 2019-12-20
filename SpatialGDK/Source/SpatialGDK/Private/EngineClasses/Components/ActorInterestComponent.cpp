// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#include "EngineClasses/Components/ActorInterestComponent.h"

#include "Schema/Interest.h"
#include "Interop/SpatialClassInfoManager.h"
#include "Interop/SpatialSender.h"
#include "EngineClasses/SpatialNetDriver.h"
#include "UObject/WeakObjectPtr.h"

UActorInterestComponent::UActorInterestComponent() {
    {
        FScriptDelegate f;
        f.BindUFunction(this, "on_activated");
        this->OnComponentActivated.Add(f);
    }
    {
        FScriptDelegate f;
        f.BindUFunction(this, "on_deactivated");
        this->OnComponentDeactivated.Add(f);
    }
    //this->OnComponentDeactivated = [this](){this->on_deactivated();};
}

void UActorInterestComponent::CreateQueries(const USpatialClassInfoManager& ClassInfoManager, const SpatialGDK::QueryConstraint& AdditionalConstraints, TArray<SpatialGDK::Query>& OutQueries) const
{
	for (const auto& QueryData : Queries)
	{
		if (!QueryData.Constraint)
		{
			continue;
		}

		SpatialGDK::Query NewQuery{};
		// Avoid creating an unnecessary AND constraint if there are no AdditionalConstraints to consider.
		if (AdditionalConstraints.IsValid())
		{
			SpatialGDK::QueryConstraint ComponentConstraints;
			QueryData.Constraint->CreateConstraint(ClassInfoManager, ComponentConstraints);

			NewQuery.Constraint.AndConstraint.Add(ComponentConstraints);
			NewQuery.Constraint.AndConstraint.Add(AdditionalConstraints);
		}
		else
		{
			QueryData.Constraint->CreateConstraint(ClassInfoManager, NewQuery.Constraint);
		}
		NewQuery.Frequency = QueryData.Frequency;
		NewQuery.FullSnapshotResult = true;

		if (NewQuery.Constraint.IsValid())
		{
			OutQueries.Push(NewQuery);
		}
	}

}

void UActorInterestComponent::refresh() {
    auto* const owner = this->GetOwner();
    USpatialNetDriver* const net_driver = Cast<USpatialNetDriver>(GetWorld()->GetNetDriver());
    USpatialSender* const sender = net_driver->Sender;
    sender->UpdateInterestComponent(owner);
}

void UActorInterestComponent::on_activated(bool) {
    //this->refresh();
}

void UActorInterestComponent::on_deactivated() {
    bool needs_refresh = false;
    for (auto& query : this->Queries) {
        //switch (query.Constraint->
        auto* const sphere = Cast<USphereConstraint>(query.Constraint);
        if (sphere) {
            FVector v(0.0f, 0.0f, -10000.0f);
            needs_refresh |= (v != sphere->Center);
            sphere->Center = v;
        }
    }
    if (needs_refresh)
        this->refresh();
}
