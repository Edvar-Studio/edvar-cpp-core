#pragma once

namespace Edvar::Memory {
class IMemoryAllocator;
}

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
class IPlatform {
public:
    virtual ~IPlatform() = default;

    virtual void Abort();
    virtual void OnFatalError(const char16_t* message);

    [[nodiscard]] virtual Memory::IMemoryAllocator* GetAllocator() const;
    [[nodiscard]] virtual IPlatformThreading& GetThreading() const = 0;
    [[nodiscard]] virtual IPlatformWindowing& GetWindowing() const = 0;
    [[nodiscard]] virtual IPlatformInput& GetInput() const = 0;

    [[nodiscard]] virtual const char* GetName() const = 0;
};

IPlatform& Get();
} // namespace Edvar::Platform