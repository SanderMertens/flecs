// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsOSAPI.h"

FFlecsRunnable::FFlecsRunnable(ecs_os_thread_callback_t InCallback, void* InData): Callback(InCallback)
	, Data(InData)
	, bStopped(false)
{
}

uint32 FFlecsRunnable::Run()
{
	while (!bStopped.load())
	{
		Callback(Data);
		break;
	}
		
	return 0;
}

void FFlecsRunnable::Stop()
{
	bStopped.store(true);
}

FFlecsThreadWrapper::FFlecsThreadWrapper(ecs_os_thread_callback_t Callback, void* Data)
{
	Runnable = new FFlecsRunnable(Callback, Data);
	RunnableThread = FRunnableThread::Create(
		Runnable, TEXT("FlecsThreadWrapper"), 0, TaskThread);
}

FFlecsThreadWrapper::~FFlecsThreadWrapper()
{
	if (!bJoined.load() && RunnableThread)
	{
		Stop();
		Join();

		delete RunnableThread;
		RunnableThread = nullptr;
	}
}

void FFlecsThreadWrapper::Stop() const
{
	if (Runnable)
	{
		Runnable->Stop();
	}
}

void FFlecsThreadWrapper::Join()
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

#if WITH_EDITOR

std::atomic<uint32> FFlecsThreadTask::TaskCounter = 0;

#endif // WITH_EDITOR

FFlecsThreadTask::FFlecsThreadTask(const ecs_os_thread_callback_t Callback, void* Data)
{
#if WITH_EDITOR

	const FString TaskName = FString::Printf(TEXT("FlecsTask_%u"), GetNextTaskID());

#else

	FString TaskName;

#endif // WITH_EDITOR
	
	TaskEvent = UE::Tasks::Launch(*TaskName,
		[Callback, Data]()
		{
			Callback(Data);
		}, TaskThreadPriority);
}

FFlecsThreadTask::~FFlecsThreadTask()
{
	Wait();
	ReleaseTaskID();
}

void FFlecsThreadTask::Wait() const
{
	if (TaskEvent.IsValid())
	{
		TaskEvent.Wait();
	}
}
