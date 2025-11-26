#include "Platform/Windows/WindowsPlatform.hpp"
#ifdef _WIN32
#include "Platform/Windows/WindowsPlatformThreading.hpp"
namespace Edvar::Platform::Windows {
IPlatformThreading& WindowsPlatform::GetThreading() const {
    static WindowsPlatformThreading threading;
    return threading;
}
} // namespace Edvar::Platform::Windows
#endif