// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"

class FFlecsLibraryWorldFixture
{
	flecs::world* World = nullptr;
public:
	void Setup()
	{
		World = new flecs::world();
	}

	void TearDown()
	{
		delete World;
		World = nullptr;
	}
	
}; // class FFlecsLibraryWorldFixture

#define FLECS_LIBRARY_FIXTURE_LIFECYCLE(FixtureName) \
	BeforeEach([this]() \
	{ \
		FixtureName.SetUp(); \
	}); \
	AfterEach([this]() \
	{ \
		FixtureName.TearDown(); \
	})
	
