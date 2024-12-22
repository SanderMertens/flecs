// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <condition_variable>

#include "CoreMinimal.h"
#include "flecs/os_api.h"
#include "Logs/FlecsCategories.h"
#include "SolidMacros/Macros.h"
#include "Unlog/Unlog.h"

LLM_DEFINE_TAG(FlecsMemoryTag, "Flecs Memory");

DECLARE_STATS_GROUP(TEXT("FlecsOS"), STATGROUP_FlecsOS, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("FlecsOS::TaskThread"), STAT_FlecsOS, STATGROUP_FlecsOS);

class FFlecsRunnable : public FRunnable
{
public:
	FFlecsRunnable(ecs_os_thread_callback_t InCallback, void* InData)
		: Callback(InCallback)
		, Data(InData)
		, bStopped(false)
	{
	}

	virtual uint32 Run() override
	{
		while (!bStopped.load())
		{
			Callback(Data);
			break;
		}
		
		return 0;
	}

	virtual void Stop() override
	{
		bStopped.store(true);
	}

private:
	ecs_os_thread_callback_t Callback;
	void* Data;
	std::atomic<bool> bStopped;
}; // class FFlecsRunnable

struct FFlecsThreadWrapper
{
	static constexpr EThreadPriority TaskThread = TPri_Highest;
	
	FRunnable* Runnable = nullptr;
	FRunnableThread* RunnableThread = nullptr;
	std::atomic<bool> bJoined { false };

	FFlecsThreadWrapper(ecs_os_thread_callback_t Callback, void* Data)
	{
		Runnable = new FFlecsRunnable(Callback, Data);
		RunnableThread = FRunnableThread::Create(Runnable, TEXT("FlecsThreadWrapper"), 0,
			TaskThread);
	}

	~FFlecsThreadWrapper()
	{
		if (!bJoined.load() && RunnableThread)
		{
			Stop();
			Join();

			delete RunnableThread;
		}
	}

	void Stop() const
	{
		if (Runnable)
		{
			Runnable->Stop();
		}
	}

	void Join()
	{
		if (!bJoined.exchange(true))
		{
			if (RunnableThread)
			{
				RunnableThread->WaitForCompletion();
				delete RunnableThread;
				RunnableThread = nullptr;
			}
			
			if (Runnable)
			{
				delete Runnable;
				Runnable = nullptr;
			}
		}
	}
	
}; // struct FFlecsThreadWrapper

struct FFlecsTask
{
	static constexpr ENamedThreads::Type TaskThread = ENamedThreads::AnyHiPriThreadNormalTask;
	
	FGraphEventRef TaskEvent;

	FORCEINLINE FFlecsTask(const ecs_os_thread_callback_t Callback, void* Data)
	{
		TaskEvent = FFunctionGraphTask::CreateAndDispatchWhenReady(
			[Callback, Data]()
			{
				Callback(Data);
			},
			GET_STATID(STAT_FlecsOS), nullptr, TaskThread);
	}

	FORCEINLINE ~FFlecsTask()
	{
		if (TaskEvent.IsValid())
		{
			TaskEvent->Wait();
		}
	}

	FORCEINLINE void Wait() const
	{
		if (TaskEvent.IsValid())
		{
			FTaskGraphInterface::Get().WaitUntilTaskCompletes(TaskEvent);
		}
	}
	
}; // struct FFlecsTask

struct FFlecsConditionWrapper
{
	std::condition_variable ConditionalVariable;
	std::mutex* Mutex;
}; // struct ConditionWrapper

struct FOSApiInitializer
{
	
	FOSApiInitializer()
	{
		UN_LOG(LogFlecsCore, Log, "Initializing Flecs OS API");
		InitializeOSAPI();
	}
	
	void InitializeOSAPI()
	{
        ecs_os_set_api_defaults();
		
        ecs_os_api_t os_api = ecs_os_api;

	    os_api.mutex_new_ = []() -> ecs_os_mutex_t
	    {
	    	std::mutex* Mutex = new std::mutex();
	        return reinterpret_cast<ecs_os_mutex_t>(Mutex);
	    };

		os_api.mutex_free_ = [](ecs_os_mutex_t Mutex)
		{
			std::mutex* MutexPtr = reinterpret_cast<std::mutex*>(Mutex);
			delete MutexPtr;
		};

		os_api.mutex_lock_ = [](ecs_os_mutex_t Mutex)
		{
			std::mutex* MutexPtr = reinterpret_cast<std::mutex*>(Mutex);
			MutexPtr->lock();
		};

		os_api.mutex_unlock_ = [](ecs_os_mutex_t Mutex)
		{
			std::mutex* MutexPtr = reinterpret_cast<std::mutex*>(Mutex);
			MutexPtr->unlock();
		};

		os_api.cond_new_ = []() -> ecs_os_cond_t
		{
			FFlecsConditionWrapper* Wrapper = new FFlecsConditionWrapper();
			Wrapper->Mutex = new std::mutex();
			return reinterpret_cast<ecs_os_cond_t>(Wrapper);
		};

		os_api.cond_free_ = [](ecs_os_cond_t Cond)
		{
			if (Cond)
			{
				FFlecsConditionWrapper* Wrapper = reinterpret_cast<FFlecsConditionWrapper*>(Cond);
				delete Wrapper->Mutex;
				delete Wrapper;
			}
		};

		os_api.cond_signal_ = [](ecs_os_cond_t Cond)
		{
			if (Cond)
			{
				FFlecsConditionWrapper* Wrapper = reinterpret_cast<FFlecsConditionWrapper*>(Cond);
				Wrapper->ConditionalVariable.notify_one();
			}
		};

		os_api.cond_broadcast_ = [](ecs_os_cond_t Cond)
		{
			if (Cond)
			{
				FFlecsConditionWrapper* Wrapper = reinterpret_cast<FFlecsConditionWrapper*>(Cond);
				Wrapper->ConditionalVariable.notify_all();
			}
		};

		os_api.cond_wait_ = [](ecs_os_cond_t Cond, ecs_os_mutex_t Mutex)
		{
			if (Cond && Mutex)
			{
				FFlecsConditionWrapper* Wrapper = reinterpret_cast<FFlecsConditionWrapper*>(Cond);
				std::mutex* std_mutex = reinterpret_cast<std::mutex*>(Mutex);
				std::unique_lock<std::mutex> Lock(*std_mutex, std::adopt_lock);
				Wrapper->ConditionalVariable.wait(Lock);
				Lock.release(); // Don't unlock on destruction since Flecs manages the lock
			}
		};

		os_api.thread_new_ = [](ecs_os_thread_callback_t Callback, void* Data) -> ecs_os_thread_t
		{
			FFlecsThreadWrapper* ThreadWrapper = new FFlecsThreadWrapper(Callback, Data);
			return reinterpret_cast<ecs_os_thread_t>(ThreadWrapper);
		};

		os_api.thread_join_ = [](ecs_os_thread_t Thread) -> void*
		{
			if (FFlecsThreadWrapper* ThreadWrapper = reinterpret_cast<FFlecsThreadWrapper*>(Thread))
			{
				ThreadWrapper->Join();
				delete ThreadWrapper;
			}
			
			return nullptr;
		};

		os_api.thread_self_ = []() -> ecs_os_thread_t
		{
			return FPlatformTLS::GetCurrentThreadId();
		};

		os_api.task_new_ = [](ecs_os_thread_callback_t Callback, void* Data) -> ecs_os_thread_t
		{
			FFlecsTask* FlecsTask = new FFlecsTask(Callback, Data);
			return reinterpret_cast<ecs_os_thread_t>(FlecsTask);
		};

		os_api.task_join_ = [](ecs_os_thread_t Thread) -> void*
		{
			const FFlecsTask* FlecsTask = reinterpret_cast<FFlecsTask*>(Thread);
			solid_check(FlecsTask);
			
			if (FlecsTask)
			{
				FlecsTask->Wait();
				delete FlecsTask;
			}
			
			return nullptr;
		};
		
        os_api.sleep_ = [](int32_t Seconds, int32_t Nanoseconds)
        {
            const double TotalSeconds = Seconds + Nanoseconds / 1e9;
            FPlatformProcess::SleepNoStats(static_cast<float>(TotalSeconds));
        };
		
        os_api.now_ = []() -> uint64_t
        {
            return FPlatformTime::Cycles64();
        };

        os_api.get_time_ = [](ecs_time_t* TimeOut)
        {
            const double Seconds = FPlatformTime::Seconds();
            TimeOut->sec = static_cast<int32_t>(Seconds);
            TimeOut->nanosec = static_cast<int32_t>((Seconds - TimeOut->sec) * 1e9);
        };

        os_api.abort_ = []()
        {
            UN_LOG(LogFlecsCore, Fatal, "Flecs: abort");
        };

        os_api.log_ = [](int32_t Level, const char* File, int32_t Line, const char* Message)
        {
#if UNLOG_ENABLED
            switch (Level)
            {
                case -4: // Fatal
                    UN_LOGF(LogFlecsCore, Fatal, "Flecs - File: %s, Line: %d, Message: %s",
						StringCast<TCHAR>(File).Get(), Line, StringCast<TCHAR>(Message).Get());
                    break;
                case -3: // Error
                	UN_LOGF(LogFlecsCore, Warning, "Error Flecs - File: %s, Line: %d, Message: %s",
						StringCast<TCHAR>(File).Get(), Line, StringCast<TCHAR>(Message).Get());
                    break;
                case -2: // Warning
                	UN_LOGF(LogFlecsCore, Warning, "Flecs - File: %s, Line: %d, Message: %s",
						StringCast<TCHAR>(File).Get(), Line, StringCast<TCHAR>(Message).Get());
                    break;
            	case 0: // Verbose
                	UN_LOGF(LogFlecsCore, Verbose, "Flecs - File: %s, Line: %d, Message: %s",
                		StringCast<TCHAR>(File).Get(), Line, StringCast<TCHAR>(Message).Get());
                    break;
                default: // Info and Debug
                	UN_LOGF(LogFlecsCore, Log, "Flecs - File: %s, Line: %d, Message: %s",
						StringCast<TCHAR>(File).Get(), Line, StringCast<TCHAR>(Message).Get());
                    break;
            }
#endif // UNLOG_ENABLED
        };

		os_api.perf_trace_push_ = [](const char* FileName, size_t Line, const char* Name)
		{
			FCpuProfilerTrace::OutputBeginDynamicEvent(Name, FileName, Line);
		};

		os_api.perf_trace_pop_ = [](const char* FileName, size_t Line, const char* Name)
		{
			FCpuProfilerTrace::OutputEndEvent();
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
			LLM_SCOPE_BYTAG(FlecsMemoryTag);
			return FMemory::Malloc(Size);
		};

		os_api.realloc_ = [](void* Ptr, int Size) -> void*
		{
			LLM_SCOPE_BYTAG(FlecsMemoryTag);
			return FMemory::Realloc(Ptr, Size);
		};

		os_api.calloc_ = [](int Size) -> void*
		{
			LLM_SCOPE_BYTAG(FlecsMemoryTag);
			return FMemory::MallocZeroed(Size);
		};

		os_api.free_ = [](void* Ptr)
		{
			LLM_SCOPE_BYTAG(FlecsMemoryTag);
			FMemory::Free(Ptr);
		};
		
        ecs_os_set_api(&os_api);
	}
	
}; // struct FOSApiInitializer
