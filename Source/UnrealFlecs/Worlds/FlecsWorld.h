// Solstice Games © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "Entities/FlecsId.h"
#include "Systems/FlecsSystem.h"
#include "FlecsWorld.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsWorld
{
	GENERATED_BODY()

public:
	FFlecsWorld() = default;
	FORCEINLINE FFlecsWorld(flecs::world InWorld) : World(&InWorld) {}

	FORCEINLINE void SetWorld(flecs::world& InWorld) { World = &InWorld; }

	FORCEINLINE NO_DISCARD flecs::world& GetFlecsWorld() { return *World; }
	FORCEINLINE NO_DISCARD const flecs::world& GetFlecsWorld() const { return *World; }

	FORCEINLINE operator flecs::world&() { return GetFlecsWorld(); }
	FORCEINLINE operator const flecs::world&() const { return GetFlecsWorld(); }

	FORCEINLINE NO_DISCARD bool IsValid() const { return World != nullptr; }

	FORCEINLINE bool operator!() const { return !IsValid(); }

	FORCEINLINE flecs::world& operator*() { return GetFlecsWorld(); }
	FORCEINLINE const flecs::world& operator*() const { return GetFlecsWorld(); }

	FORCEINLINE NO_DISCARD bool operator==(const FFlecsWorld& Other) const
	{
		return World == Other.World;
	}

	FORCEINLINE NO_DISCARD bool operator!=(const FFlecsWorld& Other) const
	{
		return World != Other.World;
	}

	FORCEINLINE NO_DISCARD bool operator==(const flecs::world& Other) const
	{
		return World == &Other;
	}

	FORCEINLINE NO_DISCARD bool operator!=(const flecs::world& Other) const
	{
		return World != &Other;
	}

	FORCEINLINE void Reset() const
	{
		World->reset();
	}

	FORCEINLINE void ResetClock() const
	{
		World->reset_clock();
	}

	template <typename ...TArgs>
	FORCEINLINE FFlecsEntityHandle CreateEntity(const TArgs&... Args) const
	{
		return World->entity(Args...);
	}

	template <typename FunctionType>
	FORCEINLINE void ForEach(FunctionType&& Function) const
	{
		World->each(std::forward<FunctionType>(Function));
	}

	template <typename T, typename FunctionType>
	FORCEINLINE void ForEach(FunctionType&& Function) const
	{
		World->each<T>(std::forward<FunctionType>(Function));
	}

	template <typename FunctionType>
	FORCEINLINE void ForEach(const FFlecsId& InTermId, FunctionType&& Function) const
	{
		World->each(InTermId.GetFlecsId(), std::forward<FunctionType>(Function));
	}

	template <typename T>
	FORCEINLINE void AddSingleton() const
	{
		World->add<T>();
	}

	template <typename T>
	FORCEINLINE void SetSingleton(const T& Value) const
	{
		World->set<T>(Value);
	}

	template <typename First, typename Second>
	FORCEINLINE void SetSingleton(const First& InFirst, const Second InSecond) const
	{
		World->set<First, Second>(InSecond, InFirst);
	}

	template <typename First, typename ...TArgs>
	FORCEINLINE void SetSingleton(const First& InFirst, const TArgs&... Args) const
	{
		World->set(std::forward<const TArgs&>(Args)..., InFirst);
	}

	template <typename T>
	FORCEINLINE void RemoveSingleton() const
	{
		World->remove<T>();
	}

	template <typename First, typename Second>
	FORCEINLINE void RemoveSingleton() const
	{
		World->remove<First, Second>();
	}

	template <typename First, typename Second>
	FORCEINLINE void RemoveSingleton(const Second& InSecond) const
	{
		World->remove<First>(InSecond);
	}
	
	template <typename First>
	FORCEINLINE void RemoveSingleton(const FFlecsId& InSecond) const
	{
		World->remove<First>(InSecond);
	}

	template <typename First, typename ...TArgs>
	FORCEINLINE void RemoveSingleton() const
	{
		World->remove<First, TArgs...>();
	}
	
	template <typename T>
	FORCEINLINE NO_DISCARD bool HasSingleton() const
	{
		return World->has<T>();
	}

	template <typename First, typename Second>
	FORCEINLINE NO_DISCARD bool HasSingleton() const
	{
		return World->has<First, Second>();
	}

	template <typename First, typename Second>
	FORCEINLINE NO_DISCARD bool HasSingleton(const Second& InSecond) const
	{
		return World->has<First>(InSecond);
	}
	
	template <typename First>
	FORCEINLINE NO_DISCARD bool HasSingleton(const FFlecsId& InSecond) const
	{
		return World->has<First>(InSecond);
	}

	template <typename First, typename ...TArgs>
	FORCEINLINE NO_DISCARD bool HasSingleton() const
	{
		return World->has<First, TArgs...>();
	}

	template <typename T>
	FORCEINLINE NO_DISCARD T GetSingleton() const
	{
		return *World->get<T>();
	}

	template <typename First, typename Second>
	FORCEINLINE NO_DISCARD First GetSingleton(const Second& InSecond) const
	{
		return *World->get<First, Second>(InSecond);
	}

	template <typename T>
	FORCEINLINE NO_DISCARD const T* GetSingletonPtr() const
	{
		return World->get<T>();
	}

	template <typename T>
	FORCEINLINE NO_DISCARD flecs::ref<T> GetSingletonRef() const
	{
		return World->get_ref<T>();
	}

	FORCEINLINE void Merge() const
	{
		World->merge();
	}

	FORCEINLINE void SetAutoMerge(const bool bInAutoMerge) const
	{
		World->set_automerge(bInAutoMerge);
	}

	FORCEINLINE NO_DISCARD FName GetName() const
	{
		return GetSingleton<FName>();
	}

	FORCEINLINE void SetName(const FName& InName) const
	{
		SetSingleton<FName>(InName);

		#if WITH_EDITOR

		const char* Name = TCHAR_TO_ANSI(*InName.ToString());

		GetWorldEntity().SetDocName(Name);
		
		World->entity<FName>().set_doc_name(Name);

		#endif // WITH_EDITOR
	}

	template <typename T>
	FORCEINLINE void ImportModule() const
	{
		World->import<T>();
	}

	FORCEINLINE bool BeginDefer() const
	{
		return World->defer_begin();
	}

	FORCEINLINE bool EndDefer() const
	{
		return World->defer_end();
	}

	FORCEINLINE NO_DISCARD bool IsDeferred() const
	{
		return World->is_deferred();
	}

	FORCEINLINE bool BeginReadOnly() const
	{
		return World->readonly_begin();
	}

	FORCEINLINE void EndReadOnly() const
	{
		World->readonly_end();
	}

	FORCEINLINE void SetContext(void* InContext) const
	{
		World->set_ctx(InContext);
	}

	FORCEINLINE bool Progress(const double DeltaTime = 0.0) const
	{
		return World->progress(DeltaTime);
	}

	FORCEINLINE void SetTimeScale(const double InTimeScale) const
	{
		World->set_time_scale(InTimeScale);
	}

	FORCEINLINE void SetTargetFps(const double InTargetFps) const
	{
		World->set_target_fps(InTargetFps);
	}

	FORCEINLINE void Quit() const
	{
		World->quit();
	}

	FORCEINLINE void DestroyWorld()
	{
		delete World;
		World = nullptr;
	}

	FORCEINLINE void SetPipeline(const FFlecsEntityHandle& InPipeline) const
	{
		World->set_pipeline(InPipeline);
	}

	template <typename T>
	FORCEINLINE void SetPipeline() const
	{
		World->set_pipeline<T>();
	}

	FORCEINLINE NO_DISCARD FFlecsEntityHandle GetPipeline() const
	{
		return World->get_pipeline();
	}

	FORCEINLINE FFlecsEntityHandle SetScope(const FFlecsEntityHandle& InScope) const
	{
		return World->set_scope(InScope);
	}

	template <typename T>
	FORCEINLINE FFlecsEntityHandle SetScope() const
	{
		return World->set_scope<T>();
	}

	FORCEINLINE NO_DISCARD FFlecsEntityHandle GetScope() const
	{
		return World->get_scope();
	}

	FORCEINLINE NO_DISCARD float GetDeltaTime() const
	{
		return World->delta_time();
	}

	FORCEINLINE NO_DISCARD FFlecsEntityHandle GetAlive(const FFlecsId& InId) const
	{
		return World->get_alive(InId.GetFlecsId());
	}

	FORCEINLINE FFlecsEntityHandle MakeAlive(const FFlecsId& InId) const
	{
		return World->make_alive(InId.GetFlecsId());
	}

	template <typename ...TComponents>
	FORCEINLINE NO_DISCARD FFlecsSystem CreateSystem(const FString& InName) const
	{
		return FFlecsSystem(World->system<TComponents...>(TCHAR_TO_ANSI(*InName)));
	}

	template <typename ...TComponents>
	FORCEINLINE NO_DISCARD flecs::system_builder<TComponents...> CreateSystem(const FString& InName) const
	{
		return World->system<TComponents...>(TCHAR_TO_ANSI(*InName));
	}

	FORCEINLINE NO_DISCARD FFlecsEntityHandle GetWorldEntity() const
	{
		return World->entity(EcsWorld);
	}

	FORCEINLINE void SetStageCount(const int32 InStageCount) const
	{
		World->set_stage_count(InStageCount);
	}

	FORCEINLINE NO_DISCARD int32 GetStageCount() const
	{
		return World->get_stage_count();
	}

	FORCEINLINE NO_DISCARD int32 GetStageId() const
	{
		return World->get_stage_id();
	}

	FORCEINLINE NO_DISCARD bool IsStage() const
	{
		return World->is_stage();
	}

	FORCEINLINE NO_DISCARD FFlecsWorld GetStage(const int32 InStageId) const
	{
		return World->get_stage(InStageId);
	}

	FORCEINLINE NO_DISCARD FFlecsWorld ObtainAsyncStage() const
	{
		return World->async_stage();
	}

	FORCEINLINE NO_DISCARD bool IsReadOnly() const
	{
		return World->is_readonly();
	}

	FORCEINLINE void PreallocateEntities(const int32 InEntityCount) const
	{
		World->dim(InEntityCount);
	}

	FORCEINLINE void SetEntityRange(const int32 InMin, const int32 InMax) const
	{
		World->set_entity_range(InMin, InMax);
	}

	FORCEINLINE void EnforceEntityRange(const bool bInEnforce) const
	{
		World->enable_range_check(bInEnforce);
	}

	template <typename FunctionType>
	FORCEINLINE void ForEachChild(FunctionType&& Function) const
	{
		World->children(std::forward<FunctionType>(Function));
	}

	FORCEINLINE void SetThreads(const int32 InThreadCount) const
	{
		World->set_threads(InThreadCount);
	}

	FORCEINLINE NO_DISCARD int32 GetThreads() const
	{
		return World->get_threads();
	}
	
private:
	flecs::world* World = nullptr;
}; // struct FFlecsWorld