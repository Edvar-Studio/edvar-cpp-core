#include "Test/Testing.hpp"

namespace Edvar::Testing {
ITest::ITest() { ThreadTester::GetForCurrentThread().RegisterTest(this); }
static bool Assert::IsTrue(bool condition) {
    if (!condition) {
        ThreadTester::GetForCurrentThread().
        return false;
    }
    return true;
}
} // namespace Edvar::Testing