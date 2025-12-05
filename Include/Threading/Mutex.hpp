#pragma once
namespace Edvar::Platform {
class IMutexImplementation;
}

namespace Edvar::Threading {
class Mutex {
public:
    Mutex();
    ~Mutex();

    void Lock();
    void Release();
private:
    Platform::IMutexImplementation* MutexImplementation;
};

template <typename MutexT> class ScopedLock {
    static constexpr bool IsMutexImplementation = std::is_base_of_v<Platform::IMutexImplementation, MutexT> ||
                                                  std::is_same_v<Platform::IMutexImplementation, MutexT>;
    static constexpr bool IsMutexWrapper = std::is_base_of_v<Edvar::Threading::Mutex, MutexT>;

    static_assert(
        IsMutexImplementation || IsMutexWrapper,
        "ScopedLock can only be used with types of Edvar::Platform::IMutexImplementation or Edvar::Threading::Mutex");

public:
    ScopedLock() = delete;
    ScopedLock(const ScopedLock&) = delete;
    ScopedLock(ScopedLock&& Other) noexcept : Mutex(std::move(Other.Mutex)) {}
    explicit ScopedLock(MutexT& InMutex) : Mutex(InMutex) { InMutex.Lock(); }
    ~ScopedLock() { Mutex.Release(); }

private:
    MutexT& Mutex;
};

} // namespace Edvar::Threading
