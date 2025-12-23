#include "Platform/IPlatform.hpp"
/**
 * Add include of the platform headers here to allow them to be selected.
 */
#include "Platform/Windows/WindowsPlatform.hpp"
namespace Edvar::Platform {

void IPlatform::Abort() { std::abort(); }
void IPlatform::OnFatalError(const char16_t* message) {
    PrintMessageToDebugger(message);
    Abort();
}
void IPlatform::PrintMessageToDebugger(const char16_t* message) { (void)message; }

IPlatform& GetPlatform() {
    static PlatformType platform;
    return platform;
}
} // namespace Edvar::Platform
