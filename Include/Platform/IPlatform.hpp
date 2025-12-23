#pragma once

namespace Edvar::Platform {

/**
 * Some basic craash reasons that can be reported by the platform.
 */
enum class CrashReason : int32_t {
    None,
    Unknown,
    AccessViolation,
    StackOverflow,
};

class IPlatformThreading;
class IPlatformWindowing;
class IPlatformInput;
class EDVAR_CPP_CORE_API IPlatform {
public:
    virtual ~IPlatform() = default;

    virtual void Abort();
    [[noreturn]] virtual void OnFatalError(const char16_t* message);
    virtual void PrintMessageToDebugger(const char16_t* message);

    [[nodiscard]] virtual IPlatformThreading& GetThreading() const = 0;
    [[nodiscard]] virtual IPlatformWindowing& GetWindowing() const = 0;
    [[nodiscard]] virtual IPlatformInput& GetInput() const = 0;

    [[nodiscard]] virtual const char* GetName() const = 0;
};

EDVAR_CPP_CORE_API IPlatform& GetPlatform();
} // namespace Edvar::Platform