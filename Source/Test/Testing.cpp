#include "Test/Testing.hpp"


namespace Edvar::Testing{
ITest::ITest(){
    CurrentThreadTester::GetForCurrentThread().RegisterTest(this);
}
}