#pragma once

#if ((defined(__linux__) || defined(__APPLE__) || defined(__bsd__)) && defined(__GNUC__))
#    define EDVAR_MEMORY_ATOMIC_GNUC 1
#endif
#if defined(_WIN32)
#    define EDVAR_MEMORY_ATOMIC_WINDOWS 1
#endif
#if EDVAR_MEMORY_ATOMIC_GNUC || EDVAR_MEMORY_ATOMIC_WINDOWS
#    define EDVAR_MEMORY_PLATFORM_ATOMIC_SUPPORTED 1
#else
#    define EDVAR_MEMORY_PLATFORM_ATOMIC_SUPPORTED 0
#    include "Platform/IPlatformThreading.hpp"
#endif

namespace Edvar::Memory {
namespace _z_private_AtomicDetails {
struct Impl;
}

enum class MemoryOrder : uint8_t { Relaxed, Consume, Acquire, Release, AcquireAndRelease, SequentiallyConsistent };

template <typename ValueT> class Atomic {
    static_assert(std::is_trivially_copyable_v<ValueT>, "atomic needs values to be trivially copyable");
    static_assert(sizeof(ValueT) <= 8, "atomic types can at max be 64-bit");
    using StorageT = typename std::conditional<
        sizeof(ValueT) == 1, int8_t,
        typename std::conditional<sizeof(ValueT) == 2, int16_t,
                                  typename std::conditional<sizeof(ValueT) == 4, int32_t, int64_t>::type>::type>::type;

public:
    Atomic() {
#if !EDVAR_MEMORY_PLATFORM_ATOMIC_SUPPORTED
        mutex = &Platform::Get().GetThreading().CreateMutex();
#endif
    }
    Atomic(const ValueT& InitialValue) : storage(InitialValue), Atomic() {}

    ~Atomic() {
#if !EDVAR_MEMORY_PLATFORM_ATOMIC_SUPPORTED
        delete mutex;
#endif
    }

    ValueT Load(MemoryOrder order = MemoryOrder::SequentiallyConsistent) const;
    ValueT Store(const ValueT& newValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent);
    ValueT FetchAdd(const ValueT& value, MemoryOrder order = MemoryOrder::SequentiallyConsistent);

    operator ValueT() const { return Load(); }
    ValueT operator=(const ValueT& newValue) {
        Store(newValue);
        return newValue;
    }

    ValueT operator++() { return FetchAdd(1) + 1; }
    ValueT operator++(int) { return FetchAdd(1); }
    ValueT operator--() { return FetchAdd(-1) - 1; }
    ValueT operator--(int) { return FetchAdd(-1); }
    ValueT operator+=(const ValueT& value) { return FetchAdd(value) + value; }
    ValueT operator-=(const ValueT& value) { return FetchAdd(-value) - value; }

private:
    StorageT storage;
#if !EDVAR_MEMORY_PLATFORM_ATOMIC_SUPPORTED
    // non-windows and linux systems use a simple mutex to synchronize.
    class IMutexImplementation* mutex = nullptr;
#endif
    friend struct _z_private_AtomicDetails::Impl;
};

namespace _z_private_AtomicDetails {
struct Impl {
    static void BeforeFence(MemoryOrder order);
    static void AfterFence(MemoryOrder order);

    static void Store(Atomic<int8_t>& atomic, int8_t value, MemoryOrder order = MemoryOrder::SequentiallyConsistent);
    static void Store(Atomic<int16_t>& atomic, int16_t value, MemoryOrder order = MemoryOrder::SequentiallyConsistent);
    static void Store(Atomic<int32_t>& atomic, int32_t value, MemoryOrder order = MemoryOrder::SequentiallyConsistent);
    static void Store(Atomic<int64_t>& atomic, int64_t value, MemoryOrder order = MemoryOrder::SequentiallyConsistent);

    static int8_t Load(const Atomic<int8_t>& atomic, MemoryOrder order = MemoryOrder::SequentiallyConsistent);
    static int16_t Load(const Atomic<int16_t>& atomic, MemoryOrder order = MemoryOrder::SequentiallyConsistent);
    static int32_t Load(const Atomic<int32_t>& atomic, MemoryOrder order = MemoryOrder::SequentiallyConsistent);
    static int64_t Load(const Atomic<int64_t>& atomic, MemoryOrder order = MemoryOrder::SequentiallyConsistent);

    static int8_t FetchAdd(Atomic<int8_t>& atomic, int8_t value,
                           MemoryOrder order = MemoryOrder::SequentiallyConsistent);
    static int16_t FetchAdd(Atomic<int16_t>& atomic, int16_t value,
                            MemoryOrder order = MemoryOrder::SequentiallyConsistent);
    static int32_t FetchAdd(Atomic<int32_t>& atomic, int32_t value,
                            MemoryOrder order = MemoryOrder::SequentiallyConsistent);
    static int64_t FetchAdd(Atomic<int64_t>& atomic, int64_t value,
                            MemoryOrder order = MemoryOrder::SequentiallyConsistent);
};

}; // namespace _z_private_AtomicDetails
template <typename ValueT> inline ValueT Atomic<ValueT>::Load(MemoryOrder order) const {
    return static_cast<ValueT>(
        _z_private_AtomicDetails::Impl::Load(*(static_cast<const Atomic<StorageT>*>(this)), order));
}

template <typename ValueT> inline ValueT Atomic<ValueT>::Store(const ValueT& newValue, MemoryOrder order) {
    return static_cast<ValueT>(
        _z_private_AtomicDetails::Impl::Store(*static_cast<const Atomic<StorageT>*>(this), newValue, order));
}

template <typename ValueT> inline ValueT Atomic<ValueT>::FetchAdd(const ValueT& value, MemoryOrder order) {
    return static_cast<ValueT>(
        _z_private_AtomicDetails::Impl::FetchAdd(*static_cast<Atomic<StorageT>*>(this), value, order));
}

} // namespace Edvar::Memory