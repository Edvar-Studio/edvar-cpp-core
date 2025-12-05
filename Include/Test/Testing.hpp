#pragma once

#include "Threading/ThreadPool.hpp"

namespace Edvar::Testing {
class ThreadTester;
class TestManager;

struct TestData {
    TestData(const String& name, const Utils::Function<void()>& function) : testFunction(function), testName(name) {}
    Utils::Function<void()> testFunction;
    Containers::String testName;
};

/**
 * Manages all the tests. Dispatches threads to run tests in sequence.
 * Threads are used for isolation and .
 */
class TestManager {
public:
    TestManager() : threadPool("TestManager_ThreadPool", 1) {}

    Threading::Mutex testManagerMutex;
    static void Initialize() { instance = MakeUnique<TestManager>(); }
    static TestManager* Get() { return instance.Get(); }

    void RegisterTest(const String& testName, const Utils::Function<void()>& testFunction) {
        Threading::ScopedLock lock(testManagerMutex);
        tests.Add(TestData(testName, testFunction));
        auto testJob = [this, testName]() -> int {
            for (const TestData& test : tests) {
                if (test.testName == testName) {
                    test.testFunction();
                    break;
                }
            }
            return 0;
        };
        Utils::Function<int()> boundTestJob = testJob;
        threadPool.EnqueueJob(boundTestJob);
    }

private:
    Threading::ThreadPool threadPool;
    static UniquePointer<TestManager> instance;
    Containers::List<TestData> tests;
};

namespace Assert {
static bool IsTrue(bool condition);
static bool IsFalse(bool condition);

static bool IsNullptr(const void* ptr);
static bool IsNotNullptr(const void* ptr);
static bool AreEqual(const String& a, const String& b);
static bool AreNotEqual(const String& a, const String& b);

static bool AreRawArraysEqual(const void* a, const void* b, const size_t size);
static bool AreRawArraysNotEqual(const void* a, const void* b, const size_t size);

static bool Skip();
static bool Fail();
static bool Pass();
} // namespace Assert
} // namespace Edvar::Testing