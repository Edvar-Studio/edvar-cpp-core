#include "Platform/Windows/WindowsPlatform.hpp"
#ifdef _WIN32
#include "Platform/Windows/WindowsPlatformThreading.hpp"
#include "Platform/Windows/WindowsPlatformWindowing.hpp"
#include "Platform/Windows/WindowsPlatformInput.hpp"
namespace Edvar::Platform::Windows {
IPlatformThreading& WindowsPlatform::GetThreading() const {
    static WindowsPlatformThreading threading;
    return threading;
}

IPlatformWindowing& WindowsPlatform::GetWindowing() const {
    static WindowsPlatformWindowing windowing;
    return windowing;
}

IPlatformInput& WindowsPlatform::GetInput() const {
    static WindowsPlatformInput input;
    return input;
}
} // namespace Edvar::Platform::Windows
#endif