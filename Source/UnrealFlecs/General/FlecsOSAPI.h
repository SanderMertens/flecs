// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include <condition_variable>

#include "flecs.h"
#include "flecs/os_api.h"

#include "CoreMinimal.h"

#include "Async/TaskGraphInterfaces.h"
#include "Experimental/Async/ConditionVariable.h"

#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"

#include "SolidMacros/Macros.h"
#include "Types/SolidNotNull.h"

#include "Logs/FlecsCategories.h"
#include "Tasks/Task.h"

DECLARE_STATS_GROUP(TEXT("FlecsOS"), STATGROUP_FlecsOS, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("FlecsOS::TaskThread"), STAT_FlecsOS, STATGROUP_FlecsOS);

class FFlecsRunnable final : public FRunnable
{
public:
	FFlecsRunnable(ecs_os_thread_callback_t InCallback, void* InData);

	virtual uint32 Run() override;

	virtual void Stop() override;

private:
	ecs_os_thread_callback_t Callback;
	void* Data;
	std::atomic<bool> bStopped;
}; // class FFlecsRunnable

struct FFlecsThreadWrapper
{
	static constexpr EThreadPriority TaskThread = TPri_Highest;
	
	FFlecsRunnable* Runnable = nullptr;
	FRunnableThread* RunnableThread = nullptr;
	std::atomic<bool> bJoined { false };

	FFlecsThreadWrapper(ecs_os_thread_callback_t Callback, void* Data);

	~FFlecsThreadWrapper();

	void Stop() const;

	void Join();

}; // struct FFlecsThreadWrapper

struct FFlecsThreadTask
{
	static constexpr UE::Tasks::ETaskPriority TaskThreadPriority = UE::Tasks::ETaskPriority::High;
	
	UE::Tasks::TTask<void> TaskEvent;

#if WITH_EDITOR

	static std::atomic<uint32> TaskCounter;

	static NO_DISCARD uint32 GetNextTaskID()
	{
		return TaskCounter.fetch_add(1, std::memory_order_relaxed);
	}

	static uint32 ReleaseTaskID()
	{
		return TaskCounter.fetch_sub(1, std::memory_order_relaxed);
	}

	static NO_DISCARD uint32 GetActiveTaskCount()
	{
		return TaskCounter.load(std::memory_order_relaxed);
	}

#else
	
	static constexpr uint32 GetNextTaskID()
	{
		return 0;
	}
	static constexpr uint32 ReleaseTaskID()
	{
		return 0;
	}
	
	static constexpr uint32 GetActiveTaskCount()
	{
		return 0;
	}
	
#endif // WITH_EDITOR

	FFlecsThreadTask(const ecs_os_thread_callback_t Callback, void* Data);

	~FFlecsThreadTask();

	void Wait() const;

}; // struct FFlecsThreadTask

struct FFlecsConditionWrapper
{
	UE::FConditionVariable ConditionalVariable;
}; // struct ConditionWrapper

struct FOSApiInitializer
{
	static constexpr uint32 FlecsMemoryDefaultAlignment = PLATFORM_CACHE_LINE_SIZE;
	
	FOSApiInitializer()
	{
		UE_LOG(LogFlecsCore, Log, TEXT("Initializing Flecs OS API"));
		InitializeOSAPI();
	}
	
	void InitializeOSAPI()
	{
        ecs_os_set_api_defaults();
		
        ecs_os_api_t os_api = ecs_os_api;

	    os_api.mutex_new_ = []() -> ecs_os_mutex_t
	    {
	    	FCriticalSection* Mutex = new FCriticalSection();
	        return reinterpret_cast<ecs_os_mutex_t>(Mutex);
	    };

		os_api.mutex_free_ = [](ecs_os_mutex_t Mutex)
		{
			FCriticalSection* MutexPtr = reinterpret_cast<FCriticalSection*>(Mutex);
			delete MutexPtr;
		};

		os_api.mutex_lock_ = [](ecs_os_mutex_t Mutex)
		{
			solid_cassumef(Mutex, TEXT("Mutex is nullptr"));
			
			const TSolidNotNull<FCriticalSection*> MutexPtr = reinterpret_cast<FCriticalSection*>(Mutex);
			MutexPtr->Lock();
		};

		os_api.mutex_unlock_ = [](ecs_os_mutex_t Mutex)
		{
			solid_cassumef(Mutex, TEXT("Mutex is nullptr"));
			
			const TSolidNotNull<FCriticalSection*> MutexPtr = reinterpret_cast<FCriticalSection*>(Mutex);
			MutexPtr->Unlock();
		};

		os_api.cond_new_ = []() -> ecs_os_cond_t
		{
			FFlecsConditionWrapper* Wrapper = new FFlecsConditionWrapper();
			return reinterpret_cast<ecs_os_cond_t>(Wrapper);
		};

		os_api.cond_free_ = [](ecs_os_cond_t Cond)
		{
			solid_cassumef(Cond, TEXT("Condition variable is nullptr"));
			FFlecsConditionWrapper* Wrapper = reinterpret_cast<FFlecsConditionWrapper*>(Cond);
			
			delete Wrapper;
		};

		os_api.cond_signal_ = [](ecs_os_cond_t Cond)
		{
			solid_cassumef(Cond, TEXT("Condition variable is nullptr"));
			
			const TSolidNotNull<FFlecsConditionWrapper*> Wrapper = reinterpret_cast<FFlecsConditionWrapper*>(Cond);
			Wrapper->ConditionalVariable.NotifyOne();
		};

		os_api.cond_broadcast_ = [](ecs_os_cond_t Cond)
		{
			solid_cassumef(Cond, TEXT("Condition variable is nullptr"));
			
			const TSolidNotNull<FFlecsConditionWrapper*> Wrapper = reinterpret_cast<FFlecsConditionWrapper*>(Cond);
			Wrapper->ConditionalVariable.NotifyAll();
		};

		os_api.cond_wait_ = [](ecs_os_cond_t Cond, ecs_os_mutex_t Mutex)
		{
			solid_cassumef(Cond, TEXT("Condition variable is nullptr"));
			solid_cassumef(Mutex, TEXT("Mutex is nullptr"));
			
			const TSolidNotNull<FFlecsConditionWrapper*> Wrapper = reinterpret_cast<FFlecsConditionWrapper*>(Cond);
			const TSolidNotNull<FCriticalSection*> CritSection = reinterpret_cast<FCriticalSection*>(Mutex);

			Wrapper->ConditionalVariable.Wait(*CritSection);
		};

		os_api.thread_new_ = [](ecs_os_thread_callback_t Callback, void* Data) -> ecs_os_thread_t
		{
			FFlecsThreadWrapper* ThreadWrapper = new FFlecsThreadWrapper(Callback, Data);
			return reinterpret_cast<ecs_os_thread_t>(ThreadWrapper);
		};

		os_api.thread_join_ = [](ecs_os_thread_t Thread) -> void*
		{
			if LIKELY_IF(FFlecsThreadWrapper* ThreadWrapper = reinterpret_cast<FFlecsThreadWrapper*>(Thread))
			{
				ThreadWrapper->Join();
				delete ThreadWrapper;
			}
			
			return nullptr;
		};

		os_api.thread_self_ = []() -> ecs_os_thread_id_t
		{
			return FPlatformTLS::GetCurrentThreadId();
		};

		os_api.task_new_ = [](ecs_os_thread_callback_t Callback, void* Data) -> ecs_os_thread_t
		{
			FFlecsThreadTask* FlecsTask = new FFlecsThreadTask(Callback, Data);
			return reinterpret_cast<ecs_os_thread_t>(FlecsTask);
		};

		os_api.task_join_ = [](ecs_os_thread_t Thread) -> void*
		{
			FFlecsThreadTask* FlecsTask = reinterpret_cast<FFlecsThreadTask*>(Thread);
			solid_check(FlecsTask);
			
			FlecsTask->Wait();
			delete FlecsTask;
			
			return nullptr;
		};
		
        os_api.sleep_ = [](int32_t Seconds, int32_t Nanoseconds)
        {
            const double TotalSeconds = Seconds + (Nanoseconds / 1e9);
            FPlatformProcess::SleepNoStats(static_cast<float>(TotalSeconds));
        };
		
		os_api.now_ = []() -> uint64_t
		{
			static const uint64 BaseCycles = FPlatformTime::Cycles64();
			const uint64 Cycles = FPlatformTime::Cycles64() - BaseCycles;
			
			static const double NanoSecondsPerCycle = 1e9 * FPlatformTime::GetSecondsPerCycle();
			return static_cast<uint64_t>(Cycles * NanoSecondsPerCycle);
		};

        os_api.get_time_ = [](ecs_time_t* TimeOut)
        {
        	const uint64 NanoSeconds = ecs_os_now();
        	TimeOut->sec = static_cast<uint32_t>(NanoSeconds / 1e9);
        	TimeOut->nanosec = static_cast<uint32_t>(NanoSeconds % static_cast<uint64>(1e9));
        };

        os_api.abort_ = []()
        {
        	#if !NO_LOGGING
        		UE_LOG(LogFlecsCore, Fatal, TEXT("Flecs - Aborting..."));
			#endif // !NO_LOGGING
        	
        	FGenericPlatformMisc::RequestExit(false);
        };

        os_api.log_ = [](int32_t Level, const char* File, int32_t Line, const char* Message)
        {
#if !NO_LOGGING
            switch (Level)
            {
                case -4: // Fatal
	                {
                		UE_LOGFMT(LogFlecsCore, Fatal,
                			"Flecs - File: {FileName}, Line: {LineNumber}, Message: {Message}",
							File, Line, Message);

                		break;
	                }
                case -3: // Error
	                {
                		UE_LOGFMT(LogFlecsCore, Error,
							"Flecs - File: {FileName}, Line: {LineNumber}, Message: {Message}",
							File, Line, Message);

                		break;
	                }
                case -2: // Warning
	                {
                		UE_LOGFMT(LogFlecsCore, Warning,
                			"Flecs - File: {FileName}, Line: {LineNumber}, Message: {Message}",
                				File, Line, Message);

                		break;
	                }
            	case 0: // Verbose
            		{
            			UE_LOGFMT(LogFlecsCore, Verbose,
            				"Flecs - File: {FileName}, Line: {LineNumber}, Message: {Message}",
							File, Line, Message);

                		break;
            		}
            	case 4: // Bookmark/Journal
            		{
            			TRACE_BOOKMARK(TEXT("Flecs - File: %s, Line: %d, Message: %s"),
            				StringCast<TCHAR>(File).Get(), Line, StringCast<TCHAR>(Message).Get());
            			UE_LOGFMT(LogFlecsJournal, VeryVerbose,
            				"Flecs - File: {FileName}, Line: {LineNumber}, Message: {Message}",
            				File, Line, Message);

                		break;
            		}
                default: // Info and Debug
            		{
            			UE_LOGFMT(LogFlecsCore, Log,
							"Flecs - File: {FileName}, Line: {LineNumber}, Message: {Message}",
							File, Line, Message);

                		break;
            		}
            }
#endif // UNLOG_ENABLED
        };

		#ifdef FLECS_PERF_TRACE

		struct FFlecsProfilerTrace
		{
			FString FileName;
			uint32 Line;
			FString Name;

			FORCEINLINE FFlecsProfilerTrace(const FString& InFileName, const uint32 InLine, const FString& InName)
				: FileName(InFileName), Line(InLine), Name(InName)
			{
			}

			NO_DISCARD FORCEINLINE FString ToString() const
			{
				return FString::Printf(TEXT("FlecsProfilerTrace - File: %s, Line: %d, Name: %s"),
					*FileName, Line, *Name);
			}
			
		}; // struct FFlecsProfilerTrace

		thread_local TArray<FFlecsProfilerTrace> FlecsProfilerTraces;

		#endif // FLECS_PERF_TRACE

		os_api.perf_trace_push_ = [](const char* FileName, size_t Line, const char* Name)
		{
			#ifdef FLECS_PERF_TRACE
			
				solid_check(Line < std::numeric_limits<uint32>::max());
			
				FlecsProfilerTraces.Emplace(
					StringCast<TCHAR>(FileName).Get(), static_cast<uint32>(Line),
					StringCast<TCHAR>(Name).Get());
			
				FCpuProfilerTrace::OutputBeginDynamicEvent(Name, FileName, static_cast<uint32>(Line));
			
			#endif // FLECS_PERF_TRACE
		};

		os_api.perf_trace_pop_ = [](const char* FileName, size_t Line, const char* Name)
		{
			#ifdef FLECS_PERF_TRACE
			
				solid_check(Line < std::numeric_limits<uint32>::max());
			
				if LIKELY_IF(!FlecsProfilerTraces.IsEmpty())
				{
					const FFlecsProfilerTrace& Trace = FlecsProfilerTraces.Last();

					if (Trace.FileName != FileName || Trace.Name != Name)
					{
						UE_LOGFMT(LogFlecsCore, Error,
							"Flecs - Mismatched profiler trace pop: "
							"Got {TraceName} from {TraceFileName}:{TraceLine}, "
							"Expected {Name} from {FileName}:{Line}",
							Trace.Name, Trace.FileName, Trace.Line,
							Name, FileName, static_cast<uint32>(Line));
					}
					else
					{
						FlecsProfilerTraces.Pop();
					}
				}
				else
				{
					solid_checkf(false, TEXT("No matching Flecs profiler trace found for pop"));
				}
			
				FCpuProfilerTrace::OutputEndEvent();
			
			#endif // FLECS_PERF_TRACE
		};

		os_api.adec_ = [](int32_t* Value) -> int32
		{
			return FPlatformAtomics::InterlockedDecrement(Value);
		};

		os_api.ainc_ = [](int32_t* Value) -> int32
		{
			return FPlatformAtomics::InterlockedIncrement(Value);
		};

		os_api.lainc_ = [](int64_t* Value) -> int64
		{
			return FPlatformAtomics::InterlockedIncrement(Value);
		};

		os_api.ladec_ = [](int64_t* Value) -> int64
		{
			return FPlatformAtomics::InterlockedDecrement(Value);
		};

		os_api.malloc_ = [](int Size) -> void*
		{
			return FMemory::Malloc(Size, FlecsMemoryDefaultAlignment);
		};

		os_api.realloc_ = [](void* Ptr, int Size) -> void*
		{
			return FMemory::Realloc(Ptr, Size, FlecsMemoryDefaultAlignment);
		};

		os_api.calloc_ = [](int Size) -> void*
		{
			return FMemory::MallocZeroed(Size, FlecsMemoryDefaultAlignment);
		};

		os_api.free_ = [](void* Ptr)
		{
			FMemory::Free(Ptr);
		};
		
        ecs_os_set_api(&os_api);
	}
	
}; // struct FOSApiInitializer
