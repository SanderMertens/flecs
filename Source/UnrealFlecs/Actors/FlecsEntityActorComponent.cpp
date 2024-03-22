// Fill out your copyright notice in the Description page of Project Settings.

#include "FlecsEntityActorComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsEntityActorComponent)

UFlecsEntityActorComponent::UFlecsEntityActorComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UFlecsEntityActorComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UFlecsEntityActorComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                               FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

