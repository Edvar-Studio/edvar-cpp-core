#include "Memory/Atomic.hpp"

#if EDVAR_MEMORY_ATOMIC_WINDOWS
#    include <windows.h>
#    include <intrin.h>
#endif

namespace Edvar::Memory::_z_private_AtomicDetails {

#ifdef EDVAR_MEMORY_ATOMIC_GNUC
inline int32_t _getMemorder(MemoryOrder order) {
    switch (order) {
    case MemoryOrder::Relaxed:
        return __ATOMIC_RELAXED;
    case MemoryOrder::Consume:
        return __ATOMIC_CONSUME;
    case MemoryOrder::Acquire:
        return __ATOMIC_ACQUIRE;
    case MemoryOrder::Release:
        return __ATOMIC_RELEASE;
    case MemoryOrder::AcquireAndRelease:
        return __ATOMIC_ACQ_REL;
    case MemoryOrder::SequentiallyConsistent:
        return __ATOMIC_SEQ_CST;
    default:
        return __ATOMIC_SEQ_CST;
    }
}
#endif
void Impl::Store(Atomic<int8_t>& atomic, int8_t value, MemoryOrder order) {
#if EDVAR_MEMORY_ATOMIC_WINDOWS
    switch (order) {
    case MemoryOrder::Relaxed:
        _InterlockedExchange8(reinterpret_cast<volatile CHAR*>(&atomic.storage), value);
        break;
    case MemoryOrder::Release:
        MemoryBarrier();
        _InterlockedExchange8(reinterpret_cast<volatile CHAR*>(&atomic.storage), value);
        break;
    case MemoryOrder::SequentiallyConsistent:
    default:
        MemoryBarrier();
        _InterlockedExchange8(reinterpret_cast<volatile CHAR*>(&atomic.storage), value);
        MemoryBarrier();
        break;
    }
#elif EDVAR_MEMORY_ATOMIC_GNUC
    __atomic_store_n(reinterpret_cast<volatile int8_t*>(&atomic.storage), value, _getMemorder(order));
#else
    ScopedLock lock(*atomic.mutex);
    atomic.storage = value;
#endif
}
void Impl::Store(Atomic<int16_t>& atomic, int16_t value, MemoryOrder order) {
#if EDVAR_MEMORY_ATOMIC_WINDOWS
    switch (order) {
    case MemoryOrder::Relaxed:
        _InterlockedExchange16(reinterpret_cast<volatile SHORT*>(&atomic.storage), value);
        break;
    case MemoryOrder::Release:
        MemoryBarrier();
        _InterlockedExchange16(reinterpret_cast<volatile SHORT*>(&atomic.storage), value);
        break;
    case MemoryOrder::SequentiallyConsistent:
    default:
        MemoryBarrier();
        _InterlockedExchange16(reinterpret_cast<volatile SHORT*>(&atomic.storage), value);
        MemoryBarrier();
        break;
    }
#elif EDVAR_MEMORY_ATOMIC_GNUC
    __atomic_store_n(reinterpret_cast<volatile int16_t*>(&atomic.storage), value, _getMemorder(order));
#else
    ScopedLock lock(*atomic.mutex);
    atomic.storage = value;
#endif
}
void Impl::Store(Atomic<int32_t>& atomic, int32_t value, MemoryOrder order) {
#if EDVAR_MEMORY_ATOMIC_WINDOWS
    switch (order) {
    case MemoryOrder::Relaxed:
        _InterlockedExchange(reinterpret_cast<volatile LONG*>(&atomic.storage), value);
        break;
    case MemoryOrder::Release:
        MemoryBarrier();
        _InterlockedExchange(reinterpret_cast<volatile LONG*>(&atomic.storage), value);
        break;
    case MemoryOrder::SequentiallyConsistent:
    default:
        MemoryBarrier();
        _InterlockedExchange(reinterpret_cast<volatile LONG*>(&atomic.storage), value);
        MemoryBarrier();
        break;
    }
#elif EDVAR_MEMORY_ATOMIC_GNUC
    __atomic_store_n(reinterpret_cast<volatile int32_t*>(&atomic.storage), value, _getMemorder(order));
#else
    ScopedLock lock(*atomic.mutex);
    atomic.storage = value;
#endif
}
void Impl::Store(Atomic<int64_t>& atomic, int64_t value, MemoryOrder order) {
#if EDVAR_MEMORY_ATOMIC_WINDOWS
    switch (order) {
    case MemoryOrder::Relaxed:
        _InterlockedExchange64(reinterpret_cast<volatile LONGLONG*>(&atomic.storage), value);
        break;
    case MemoryOrder::Release:
        MemoryBarrier();
        _InterlockedExchange64(reinterpret_cast<volatile LONGLONG*>(&atomic.storage), value);
        break;
    case MemoryOrder::SequentiallyConsistent:
    default:
        MemoryBarrier();
        _InterlockedExchange64(reinterpret_cast<volatile LONGLONG*>(&atomic.storage), value);
        MemoryBarrier();
        break;
    }
#elif EDVAR_MEMORY_ATOMIC_GNUC
    __atomic_store_n(reinterpret_cast<volatile int64_t*>(&atomic.storage), value, _getMemorder(order));
#else
    ScopedLock lock(*atomic.mutex);
    atomic.storage = value;
#endif
}

int8_t Impl::Load(const Atomic<int8_t>& atomic, MemoryOrder order) {
#if EDVAR_MEMORY_ATOMIC_WINDOWS
    int8_t value;
    switch (order) {
    case MemoryOrder::Relaxed:
        value = _InterlockedExchangeAdd8(reinterpret_cast<volatile CHAR*>(const_cast<int8_t*>(&atomic.storage)), 0);
        break;
    case MemoryOrder::Acquire:
    case MemoryOrder::Consume:
        value = _InterlockedExchangeAdd8(reinterpret_cast<volatile CHAR*>(const_cast<int8_t*>(&atomic.storage)), 0);
        MemoryBarrier();
        break;
    case MemoryOrder::SequentiallyConsistent:
    default:
        MemoryBarrier();
        value = _InterlockedExchangeAdd8(reinterpret_cast<volatile CHAR*>(const_cast<int8_t*>(&atomic.storage)), 0);
        MemoryBarrier();
        break;
    }
    return value;
#elif EDVAR_MEMORY_ATOMIC_GNUC
    return __atomic_load_n(reinterpret_cast<volatile int8_t*>(const_cast<int8_t*>(&atomic.storage)),
                           _getMemorder(order));
#else
    ScopedLock lock(*atomic.mutex);
    return atomic.storage;
#endif
}
int16_t Impl::Load(const Atomic<int16_t>& atomic, MemoryOrder order) {
#if EDVAR_MEMORY_ATOMIC_WINDOWS
    int16_t value;
    switch (order) {
    case MemoryOrder::Relaxed:
        value = _InterlockedExchangeAdd16(reinterpret_cast<volatile SHORT*>(const_cast<int16_t*>(&atomic.storage)), 0);
        break;
    case MemoryOrder::Acquire:
    case MemoryOrder::Consume:
        value = _InterlockedExchangeAdd16(reinterpret_cast<volatile SHORT*>(const_cast<int16_t*>(&atomic.storage)), 0);
        MemoryBarrier();
        break;
    case MemoryOrder::SequentiallyConsistent:
    default:
        MemoryBarrier();
        value = _InterlockedExchangeAdd16(reinterpret_cast<volatile SHORT*>(const_cast<int16_t*>(&atomic.storage)), 0);
        MemoryBarrier();
        break;
    }
    return value;
#elif EDVAR_MEMORY_ATOMIC_GNUC
    return __atomic_load_n(reinterpret_cast<volatile int16_t*>(const_cast<int16_t*>(&atomic.storage)),
                           _getMemorder(order));
#else
    ScopedLock lock(*atomic.mutex);
    return atomic.storage;
#endif
}
int32_t Impl::Load(const Atomic<int32_t>& atomic, MemoryOrder order) {
#if EDVAR_MEMORY_ATOMIC_WINDOWS
    int32_t value;
    switch (order) {
    case MemoryOrder::Relaxed:
        value = _InterlockedExchangeAdd(reinterpret_cast<volatile LONG*>(const_cast<int32_t*>(&atomic.storage)), 0);
        break;
    case MemoryOrder::Acquire:
    case MemoryOrder::Consume:
        value = _InterlockedExchangeAdd(reinterpret_cast<volatile LONG*>(const_cast<int32_t*>(&atomic.storage)), 0);
        MemoryBarrier();
        break;
    case MemoryOrder::SequentiallyConsistent:
    default:
        MemoryBarrier();
        value = _InterlockedExchangeAdd(reinterpret_cast<volatile LONG*>(const_cast<int32_t*>(&atomic.storage)), 0);
        MemoryBarrier();
        break;
    }
    return value;
#elif EDVAR_MEMORY_ATOMIC_GNUC
    return __atomic_load_n(reinterpret_cast<volatile int32_t*>(const_cast<int32_t*>(&atomic.storage)),
                           _getMemorder(order));
#else
    ScopedLock lock(*atomic.mutex);
    return atomic.storage;
#endif
}
int64_t Impl::Load(const Atomic<int64_t>& atomic, MemoryOrder order) {
#if EDVAR_MEMORY_ATOMIC_WINDOWS
    int64_t value;
    switch (order) {
    case MemoryOrder::Relaxed:
        value =
            _InterlockedExchangeAdd64(reinterpret_cast<volatile LONGLONG*>(const_cast<int64_t*>(&atomic.storage)), 0);
        break;
    case MemoryOrder::Acquire:
    case MemoryOrder::Consume:
        value =
            _InterlockedExchangeAdd64(reinterpret_cast<volatile LONGLONG*>(const_cast<int64_t*>(&atomic.storage)), 0);
        MemoryBarrier();
        break;
    case MemoryOrder::SequentiallyConsistent:
    default:
        MemoryBarrier();
        value =
            _InterlockedExchangeAdd64(reinterpret_cast<volatile LONGLONG*>(const_cast<int64_t*>(&atomic.storage)), 0);
        MemoryBarrier();
        break;
    }
    return value;
#elif EDVAR_MEMORY_ATOMIC_GNUC
    return __atomic_load_n(reinterpret_cast<volatile int64_t*>(const_cast<int64_t*>(&atomic.storage)),
                           _getMemorder(order));
#else
    ScopedLock lock(*atomic.mutex);
    return atomic.storage;
#endif
}

int8_t Impl::FetchAdd(Atomic<int8_t>& atomic, int8_t value, MemoryOrder order) {
#if EDVAR_MEMORY_ATOMIC_WINDOWS
    int8_t oldValue;
    switch (order) {
    case MemoryOrder::Relaxed:
        oldValue = _InterlockedExchangeAdd8(reinterpret_cast<volatile CHAR*>(&atomic.storage), value);
        break;
    case MemoryOrder::Acquire:
    case MemoryOrder::Consume:
        oldValue = _InterlockedExchangeAdd8(reinterpret_cast<volatile CHAR*>(&atomic.storage), value);
        MemoryBarrier();
        break;
    case MemoryOrder::Release:
        MemoryBarrier();
        oldValue = _InterlockedExchangeAdd8(reinterpret_cast<volatile CHAR*>(&atomic.storage), value);
        break;
    case MemoryOrder::AcquireAndRelease:
        MemoryBarrier();
        oldValue = _InterlockedExchangeAdd8(reinterpret_cast<volatile CHAR*>(&atomic.storage), value);
        MemoryBarrier();
        break;
    case MemoryOrder::SequentiallyConsistent:
    default:
        MemoryBarrier();
        oldValue = _InterlockedExchangeAdd8(reinterpret_cast<volatile CHAR*>(&atomic.storage), value);
        MemoryBarrier();
        break;
    }
    return oldValue;
#elif EDVAR_MEMORY_ATOMIC_GNUC
    return __atomic_fetch_add(reinterpret_cast<volatile int8_t*>(&atomic.storage), value, _getMemorder(order));
#else
    ScopedLock lock(*atomic.mutex);
    const int8_t oldValue = atomic.storage;
    atomic.storage += value;
    return oldValue;
#endif
}
int16_t Impl::FetchAdd(Atomic<int16_t>& atomic, int16_t value, MemoryOrder order) {
#if EDVAR_MEMORY_ATOMIC_WINDOWS
    int16_t oldValue;
    switch (order) {
    case MemoryOrder::Relaxed:
        oldValue = _InterlockedExchangeAdd16(reinterpret_cast<volatile SHORT*>(&atomic.storage), value);
        break;
    case MemoryOrder::Acquire:
    case MemoryOrder::Consume:
        oldValue = _InterlockedExchangeAdd16(reinterpret_cast<volatile SHORT*>(&atomic.storage), value);
        MemoryBarrier();
        break;
    case MemoryOrder::Release:
        MemoryBarrier();
        oldValue = _InterlockedExchangeAdd16(reinterpret_cast<volatile SHORT*>(&atomic.storage), value);
        break;
    case MemoryOrder::AcquireAndRelease:
        MemoryBarrier();
        oldValue = _InterlockedExchangeAdd16(reinterpret_cast<volatile SHORT*>(&atomic.storage), value);
        MemoryBarrier();
        break;
    case MemoryOrder::SequentiallyConsistent:
    default:
        MemoryBarrier();
        oldValue = _InterlockedExchangeAdd16(reinterpret_cast<volatile SHORT*>(&atomic.storage), value);
        MemoryBarrier();
        break;
    }
    return oldValue;
#elif EDVAR_MEMORY_ATOMIC_GNUC
    return __atomic_fetch_add(reinterpret_cast<volatile int16_t*>(&atomic.storage), value, _getMemorder(order));
#else
    ScopedLock lock(*atomic.mutex);
    const int16_t oldValue = atomic.storage;
    atomic.storage += value;
    return oldValue;
#endif
}
int32_t Impl::FetchAdd(Atomic<int32_t>& atomic, int32_t value, MemoryOrder order) {
#if EDVAR_MEMORY_ATOMIC_WINDOWS
    int32_t oldValue;
    switch (order) {
    case MemoryOrder::Relaxed:
        oldValue = _InterlockedExchangeAdd(reinterpret_cast<volatile LONG*>(&atomic.storage), value);
        break;
    case MemoryOrder::Acquire:
    case MemoryOrder::Consume:
        oldValue = _InterlockedExchangeAdd(reinterpret_cast<volatile LONG*>(&atomic.storage), value);
        MemoryBarrier();
        break;
    case MemoryOrder::Release:
        MemoryBarrier();
        oldValue = _InterlockedExchangeAdd(reinterpret_cast<volatile LONG*>(&atomic.storage), value);
        break;
    case MemoryOrder::AcquireAndRelease:
        MemoryBarrier();
        oldValue = _InterlockedExchangeAdd(reinterpret_cast<volatile LONG*>(&atomic.storage), value);
        MemoryBarrier();
        break;
    case MemoryOrder::SequentiallyConsistent:
    default:
        MemoryBarrier();
        oldValue = _InterlockedExchangeAdd(reinterpret_cast<volatile LONG*>(&atomic.storage), value);
        MemoryBarrier();
        break;
    }
    return oldValue;
#elif EDVAR_MEMORY_ATOMIC_GNUC
    return __atomic_fetch_add(reinterpret_cast<volatile int32_t*>(&atomic.storage), value, _getMemorder(order));
#else
    ScopedLock lock(*atomic.mutex);
    const int32_t oldValue = atomic.storage;
    atomic.storage += value;
    return oldValue;
#endif
}
int64_t Impl::FetchAdd(Atomic<int64_t>& atomic, int64_t value, MemoryOrder order) {
#if EDVAR_MEMORY_ATOMIC_WINDOWS
    int64_t oldValue;
    switch (order) {
    case MemoryOrder::Relaxed:
        oldValue = _InterlockedExchangeAdd64(reinterpret_cast<volatile LONGLONG*>(&atomic.storage), value);
        break;
    case MemoryOrder::Acquire:
    case MemoryOrder::Consume:
        oldValue = _InterlockedExchangeAdd64(reinterpret_cast<volatile LONGLONG*>(&atomic.storage), value);
        MemoryBarrier();
        break;
    case MemoryOrder::Release:
        MemoryBarrier();
        oldValue = _InterlockedExchangeAdd64(reinterpret_cast<volatile LONGLONG*>(&atomic.storage), value);
        break;
    case MemoryOrder::AcquireAndRelease:
        MemoryBarrier();
        oldValue = _InterlockedExchangeAdd64(reinterpret_cast<volatile LONGLONG*>(&atomic.storage), value);
        MemoryBarrier();
        break;
    case MemoryOrder::SequentiallyConsistent:
    default:
        MemoryBarrier();
        oldValue = _InterlockedExchangeAdd64(reinterpret_cast<volatile LONGLONG*>(&atomic.storage), value);
        MemoryBarrier();
        break;
    }
    return oldValue;
#elif EDVAR_MEMORY_ATOMIC_GNUC
    return __atomic_fetch_add(reinterpret_cast<volatile int64_t*>(&atomic.storage), value, _getMemorder(order));
#else
    ScopedLock lock(*atomic.mutex);
    const int64_t oldValue = atomic.storage;
    atomic.storage += value;
    return oldValue;
#endif
}

}; // namespace Edvar::Memory::_z_private_AtomicDetails
