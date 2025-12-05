#pragma once

namespace Edvar::Testing {
class ThreadTester;
class TestManager;

/**
 * Manages all the tests. Dispatches threads to run tests in sequence.
 * Threads are used for isolation and .
 */
class TestManager{

};
class ThreadTester{

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