#pragma once
#include "Platform/IPlatformThreading.hpp"
#include "Threading/Semaphore.hpp"

namespace Edvar::Threading {

class ThreadPool : public Memory::EnableSharedFromThis<ThreadPool> {
public:
    ThreadPool(const Containers::String& name, const int32_t maxThreads)
        : taskReadySemaphore(0, maxThreads), maxThreadCount(maxThreads), poolName(name) {
        workerThreads = new Platform::IThreadImplementation*[maxThreadCount];
        for (int32_t i = 0; i < maxThreadCount; ++i) {
            Containers::String threadName = name + u"_Worker_" + Containers::String::Format(u"{}", i);
            workerThreads[i] = (&Platform::Get().GetThreading().CreateThread(threadName, WorkerThreadFunction, this));
            workerThreads[i]->OnThreadCrashed.AddShared(this, &ThreadPool::OnThreadCrashed);
        }
    }

    ThreadPool(const ThreadPool& other) = delete;

    ThreadPool(ThreadPool&& other) noexcept : taskReadySemaphore(std::move(other.taskReadySemaphore)), poolMutex(other.poolMutex) {
        workerThreads = other.workerThreads;
        maxThreadCount = other.maxThreadCount;
        other.workerThreads = nullptr;
        other.maxThreadCount = 0;
        other.poolMutex = Mutex();
    }
    ~ThreadPool() { delete[] workerThreads; }

    ThreadPool& operator=(ThreadPool&& other) noexcept {
        if (this != &other) {
            poolMutex = other.poolMutex;
            delete[] workerThreads;
            workerThreads = other.workerThreads;
            maxThreadCount = other.maxThreadCount;
            other.poolMutex = Mutex();
            other.workerThreads = nullptr;
            other.maxThreadCount = 0;
        }
        return *this;
    }

    ThreadPool& operator=(const ThreadPool& other) = delete;

    template <typename... ArgsT> void EnqueueJob(const Utils::Function<int(ArgsT...)>& job, ArgsT... args) {
        const Utils::Function<int()> boundJob = [job, args...]() -> int { return job(std::forward<ArgsT>(args)...); };
        Threading::ScopedLock lock(poolMutex);
        jobQueue.Add(boundJob);
        // Each thread can handle one task at a time. So only signal if there are available threads.
        if (taskReadySemaphore.GetCurrentCount() < maxThreadCount) {
            taskReadySemaphore.Signal(1);
        }
    }

    int32_t GetRemainingJobCount() {
        Threading::ScopedLock lock(poolMutex);
        return jobQueue.Length();
    }

private:
    typedef Utils::Function<int()> JobFunctionType;

    static int WorkerThreadFunction(void* arg) {
        ThreadPool* threadPool = static_cast<ThreadPool*>(arg);

        while (true) {
            threadPool->taskReadySemaphore.Wait(1000);
            bool shouldBeKilled = Platform::Get().GetThreading().GetCurrentThread().ShouldBeKilled();
            // Wait for a task to be available. Time out must exist to check for thread termination. 1 second timeout
            // looks fine.
            if (shouldBeKilled) {
                break;
            }
            JobFunctionType job;
            {
                Threading::ScopedLock lock(threadPool->poolMutex);
                if (threadPool->jobQueue.Length() > 0) {
                    job = threadPool->jobQueue.Get(0);
                    threadPool->jobQueue.RemoveAt(0);
                }
            }
            if (job.IsValid()) {
                job.Invoke();
            }
        }
        return 0;
    }

    Threading::Semaphore taskReadySemaphore;
    Threading::Mutex poolMutex;
    Platform::IThreadImplementation** workerThreads;
    int32_t maxThreadCount;
    Containers::List<JobFunctionType> jobQueue;
    String poolName;

    void OnThreadCrashed(Platform::CrashReason reason, String reasonMessage,
                         Platform::IThreadImplementation& threadImpl) {

    }
};
} // namespace Edvar::Threading