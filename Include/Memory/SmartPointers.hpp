#pragma once

#include "Memory/Atomic.hpp"

namespace Edvar::Memory {
template <typename T, bool ThreadSafe = false> class SharedPointer;
template <typename T, bool ThreadSafe = false> class SharedReference;
template <typename T, bool ThreadSafe = false> class WeakPointer;
template <typename T, bool ThreadSafe = false> struct EnableSharedFromThis;
namespace _z__Private {
struct EnableSharedFromThisBase {
    virtual ~EnableSharedFromThisBase() = default;
};
// Trait to detect whether a type is complete. Using SFINAE on sizeof(T).
template <typename T, typename = void> struct is_complete : std::false_type {};
template <typename T> struct is_complete<T, std::void_t<decltype(sizeof(T))>> : std::true_type {};
template <typename T> inline constexpr bool is_complete_v = is_complete<T>::value;
// SFINAE-safe helper: first check completeness, then check base-of relationship.
template <typename T> constexpr bool is_derived_from_EnableSharedFromThisBase() {
    if constexpr (!is_complete_v<T>) {
        return false;
    } else {
        return std::is_base_of_v<EnableSharedFromThisBase, T>;
    }
}
} // namespace _z__Private

template <bool ThreadSafe = false> class ReferenceCounter {
public:
    enum class ZeroCounterTag { Tag };
    ReferenceCounter() : HardCounter(1), WeakCounter(0) {}
    ReferenceCounter(ZeroCounterTag type) : HardCounter(0), WeakCounter(0) {}
    ReferenceCounter(int hardCount, int weakCount) : HardCounter(hardCount), WeakCounter(weakCount) {}
    ~ReferenceCounter() = default;
    ReferenceCounter(const ReferenceCounter& other) = delete;
    ReferenceCounter& operator=(const ReferenceCounter& other) = delete;
    ReferenceCounter(ReferenceCounter&& other) noexcept
        : HardCounter(other.HardCounter), WeakCounter(other.WeakCounter) {
        other.HardCounter = 0;
        other.WeakCounter = 0;
    }
    ReferenceCounter& operator=(ReferenceCounter&& other) noexcept {
        if (this != &other) {
            HardCounter = other.HardCounter;
            WeakCounter = other.WeakCounter;
            other.HardCounter = 0;
            other.WeakCounter = 0;
        }
        return *this;
    }

    void IncrementHard() { ++HardCounter; }
    void DecrementHard() { --HardCounter; }
    int GetHardCount() const { return LoadHard(); }

    // Atomic-aware fetch/add helpers. Return the previous value.
    int FetchAddHard(int delta) { return FetchAddImpl(HardCounter, delta); }
    int FetchAddWeak(int delta) { return FetchAddImpl(WeakCounter, delta); }

    int LoadHard() const { return LoadImpl(HardCounter); }
    int LoadWeak() const { return LoadImpl(WeakCounter); }

    void IncrementWeak() { ++WeakCounter; }
    void DecrementWeak() { --WeakCounter; }
    int GetWeakCount() const { return LoadWeak(); }

private:
    using CounterValueType = std::conditional_t<ThreadSafe, Memory::Atomic<int>, int>;
    CounterValueType HardCounter;
    CounterValueType WeakCounter;

    template <typename CounterT> static int FetchAddImpl(CounterT& counter, int delta) {
        if constexpr (ThreadSafe) {
            return counter.FetchAdd(delta, Memory::MemoryOrder::AcquireAndRelease);
        } else {
            int prev = counter;
            counter += delta;
            return prev;
        }
    }

    template <typename CounterT> static int LoadImpl(const CounterT& counter) {
        if constexpr (ThreadSafe) {
            return counter.Load(Memory::MemoryOrder::Acquire);
        } else {
            return counter;
        }
    }
};

template <typename ValueT, bool ThreadSafe> class ReferenceCountedPointerBase {
public:
    virtual ~ReferenceCountedPointerBase() = default;
    ValueT* Get() const { return managedObject; }
    ValueT* operator->() const { return Get(); }
    ValueT& operator*() const { return *Get(); }

    [[nodiscard]] bool IsValid() const {
        return this->managedObject != nullptr && this->Counter != nullptr && this->Counter->LoadHard() > 0;
    }

    template <bool UsesHardCounter> void DecreaseCounter() {
        if (this->Counter == nullptr) {
            return;
        }
        if constexpr (UsesHardCounter) {
            int prev = this->Counter->FetchAddHard(-1);
            if (prev <= 0) {
                Platform::GetPlatform().OnFatalError(u"ReferenceCounter underflow (hard)");
            }
            if (prev == 1) {
                // last hard reference; destroy managed object
                delete this->managedObject;
                if (this->Counter->LoadWeak() == 0) {
                    delete this->Counter;
                }
            }
        } else {
            int prev = this->Counter->FetchAddWeak(-1);
            if (prev <= 0) {
                Platform::GetPlatform().OnFatalError(u"ReferenceCounter underflow (weak)");
            }
            if (prev == 1) {
                if (this->Counter->LoadHard() == 0) {
                    delete this->Counter;
                }
            }
        }
    }

    template <bool UsesHardCounter> void IncreaseCounter() {
        if (this->Counter == nullptr) {
            return;
        }
        if constexpr (UsesHardCounter) {
            this->Counter->FetchAddHard(1);
        } else {
            this->Counter->FetchAddWeak(1);
        }
    }

    template <bool UsesHardCounter> void SetCounter(ReferenceCounter<ThreadSafe>* newCounter) {
        // If we're assigning the same counter pointer, do nothing.
        // Decreasing then increasing the same counter can delete the managed
        // object (when prev == 1). Guarding avoids destroying the object
        // during self-assignment or when assigning from a temporary that
        // shares the same counter.
        if (this->Counter == newCounter) {
            return;
        }

        DecreaseCounter<UsesHardCounter>();
        this->Counter = newCounter;
    }

    ReferenceCounter<ThreadSafe>* InternalGetCounter() const { return this->Counter; }
    void InternalReleaseCounter() { this->Counter = nullptr; }
    void InternalSetManagedObject(ValueT* object);

protected:
    void InternalSetManagedObjectDirect(ValueT* object) { managedObject = object; }
    ReferenceCounter<ThreadSafe>* Counter;
    ReferenceCountedPointerBase(ReferenceCounter<ThreadSafe>* counter) : Counter(counter), managedObject(nullptr) {}

    ReferenceCountedPointerBase(const ReferenceCountedPointerBase& other)
        : Counter(other.Counter), managedObject(other.managedObject) {}
    ReferenceCountedPointerBase& operator=(const ReferenceCountedPointerBase& other) {
        if (this != &other) {
            Counter = other.InternalGetCounter();
        }
        return *this;
    }
    ReferenceCountedPointerBase(ReferenceCountedPointerBase&& other) noexcept
        : Counter(other.Counter), managedObject(other.managedObject) {
        other.managedObject = nullptr;
        other.Counter = nullptr;
    }
    ReferenceCountedPointerBase& operator=(ReferenceCountedPointerBase&& other) noexcept {
        if (this != &other) {
            Counter = other.Counter;
            other.Counter = nullptr;
            managedObject = other.managedObject;
            other.managedObject = nullptr;
        }
        return *this;
    }
    ValueT* managedObject;
    template <typename, bool> friend class SharedPointer;
    template <typename, bool> friend class SharedReference;
    template <typename, bool> friend class WeakPointer;
    template <typename, bool> friend struct EnableSharedFromThis;
};

template <typename T, bool ThreadSafe> class SharedPointer final : public ReferenceCountedPointerBase<T, ThreadSafe> {
public:
    SharedPointer() : ReferenceCountedPointerBase<T, ThreadSafe>(nullptr) {}
    // Explicit copy constructor to ensure reference count is incremented on copy.
    SharedPointer(const SharedPointer& other) : ReferenceCountedPointerBase<T, ThreadSafe>(other.InternalGetCounter()) {
        this->InternalSetManagedObject(other.Get());
        this->template IncreaseCounter<true>();
    }
    SharedPointer& operator=(const SharedPointer& other) {
        if (this != &other) {
            this->template SetCounter<true>(other.InternalGetCounter());
            this->InternalSetManagedObject(other.Get());
            this->template IncreaseCounter<true>();
        }
        return *this;
    }
    ~SharedPointer() override { this->template DecreaseCounter<true>(); }
    SharedPointer(T* object) : ReferenceCountedPointerBase<T, ThreadSafe>(new ReferenceCounter<ThreadSafe>()) {
        this->InternalSetManagedObject(object);
    }

    SharedPointer& operator=(T* object) {
        if (object == this->managedObject) {
            return *this;
        }
        // Decrease current hard counter
        this->template SetCounter<true>(new ReferenceCounter<ThreadSafe>());
        this->InternalSetManagedObject(object);
        return *this;
    }

    template <typename OtherT, bool OtherThreadSafe>
    SharedPointer(const SharedPointer<OtherT, OtherThreadSafe>& other)
        : ReferenceCountedPointerBase<T, ThreadSafe>(other.InternalGetCounter()) {
        static_assert(OtherThreadSafe == ThreadSafe, "Cannot convert between different thread-safety specializations");
        this->InternalSetManagedObject(static_cast<T*>(other.Get()));
        this->template IncreaseCounter<true>();
    }

    template <typename OtherT, bool OtherThreadSafe>
    SharedPointer(const SharedReference<OtherT, OtherThreadSafe>& other)
        : ReferenceCountedPointerBase<T, ThreadSafe>(other.InternalGetCounter()) {
        static_assert(OtherThreadSafe == ThreadSafe, "Cannot convert between different thread-safety specializations");
        this->InternalSetManagedObject(static_cast<T*>(other.Get()));
        this->template IncreaseCounter<true>();
    }

    template <typename OtherT, bool OtherThreadSafe>
    SharedPointer& operator=(const SharedPointer<OtherT, OtherThreadSafe>& other)
        requires(std::is_convertible_v<OtherT*, T*> || std::is_base_of_v<T, OtherT>)
    {
        static_assert(OtherThreadSafe == ThreadSafe, "Cannot convert between different thread-safety specializations");
        if (this->InternalGetCounter() != other.InternalGetCounter()) {
            this->template SetCounter<true>(other.InternalGetCounter());
            this->InternalSetManagedObject(static_cast<T*>(other.Get()));
            this->template IncreaseCounter<true>();
        }
        return *this;
    }
    template <typename OtherT, bool OtherThreadSafe>
    SharedPointer& operator=(SharedPointer<OtherT, OtherThreadSafe>&& other) noexcept {
        static_assert(OtherThreadSafe == ThreadSafe, "Cannot convert between different thread-safety specializations");
        if (this->Get() != other.Get()) {
            this->template SetCounter<true>(other.InternalGetCounter());
            this->InternalSetManagedObject(static_cast<T*>(other.Get()));
            other.InternalReleaseCounter();
            other.InternalSetManagedObject(nullptr);
        }
        return *this;
    }
    template <typename OtherT, bool OtherThreadSafe>
    SharedPointer(SharedPointer<OtherT, OtherThreadSafe>&& other) noexcept
        : ReferenceCountedPointerBase<T, ThreadSafe>(other.InternalGetCounter()) {
        this->InternalSetManagedObject(static_cast<T*>(other.Get()));
        other.InternalReleaseCounter();
        other.InternalSetManagedObject(nullptr);
    }
    template <typename OtherT, bool OtherThreadSafe>
    SharedPointer& operator=(const SharedReference<OtherT, OtherThreadSafe>& other) {
        this->template SetCounter<true>(other.InternalGetCounter());
        this->InternalSetManagedObject(other.Get());
        this->template IncreaseCounter<true>();
        return *this;
    }

    SharedReference<T, ThreadSafe> ToSharedReference() const { return SharedReference<T, ThreadSafe>(*this); }
    WeakPointer<T, ThreadSafe> ToWeakPointer() const { return WeakPointer<T, ThreadSafe>(*this); }

    bool operator==(const SharedPointer& other) const { return this->managedObject == other.managedObject; }
    bool operator!=(const SharedPointer& other) const { return !((*this) == other); }
    bool operator==(const T* other) const { return this->managedObject == other; }
    bool operator!=(const T* other) const { return !((*this) == other); }
    operator SharedReference<T, ThreadSafe>() const { return SharedReference<T, ThreadSafe>(*this); }
    operator WeakPointer<T, ThreadSafe>() const { return WeakPointer<T, ThreadSafe>(*this); }

    explicit operator bool() const { return this->IsValid(); }
    void Reset() {
        this->template DecreaseCounter<true>();
        this->Counter = nullptr;
        this->InternalSetManagedObject(nullptr);
    }

private:
    template <typename OtherT, bool OtherThreadSafe> friend class WeakPointer;
};
template <typename T, bool ThreadSafe> class SharedReference final : public ReferenceCountedPointerBase<T, ThreadSafe> {
public:
    SharedReference(nullptr_t) = delete;

    SharedReference(T* object) : ReferenceCountedPointerBase<T, ThreadSafe>(new ReferenceCounter<ThreadSafe>()) {
        if (object == nullptr) {
            Platform::GetPlatform().OnFatalError(u"SharedReference cannot be initialized with nullptr");
            return;
        }
        this->InternalSetManagedObject(object);
    }

    SharedReference(const SharedReference& other) : ReferenceCountedPointerBase<T, ThreadSafe>(other.Counter) {
        this->InternalSetManagedObject(other.managedObject);
        this->template IncreaseCounter<true>();
    }

    SharedReference(SharedReference&& other) noexcept : ReferenceCountedPointerBase<T, ThreadSafe>(other.Counter) {
        this->InternalSetManagedObject(other.managedObject);
        other.InternalReleaseCounter();
        other.InternalSetManagedObject(nullptr);
    }

    template <typename OtherT, bool OtherThreadSafe>
    SharedReference(const SharedPointer<OtherT, OtherThreadSafe>& sharedPtr)
        : ReferenceCountedPointerBase<T, ThreadSafe>(sharedPtr.InternalGetCounter()) {
        static_assert(OtherThreadSafe == ThreadSafe, "Cannot convert between different thread-safety specializations");
        this->InternalSetManagedObject(static_cast<T*>(sharedPtr.Get()));
        this->template IncreaseCounter<true>();
    }
    template <typename OtherT, bool OtherThreadSafe>
    SharedReference(const SharedReference<OtherT, OtherThreadSafe>& sharedRef)
        : ReferenceCountedPointerBase<T, ThreadSafe>(sharedRef.InternalGetCounter()) {
        static_assert(OtherThreadSafe == ThreadSafe, "Cannot convert between different thread-safety specializations");
        this->InternalSetManagedObject(static_cast<T*>(sharedRef.Get()));
        this->template IncreaseCounter<true>();
    }
    ~SharedReference() override { this->template DecreaseCounter<true>(); }
    SharedReference operator=(nullptr_t) = delete;

    SharedReference& operator=(const SharedReference& other) {
        if (this != &other) {
            this->template SetCounter<true>(other.Counter);
            this->InternalSetManagedObject(other.Get());
            this->template IncreaseCounter<true>();
            return *this;
        }
        return *this;
    }

    template <typename OtherT, bool OtherThreadSafe>
    SharedReference& operator=(const SharedPointer<OtherT, OtherThreadSafe>& sharedPtr) {
        static_assert(OtherThreadSafe == ThreadSafe, "Cannot convert between different thread-safety specializations");
        this->template SetCounter<true>(sharedPtr.Counter);
        this->template IncreaseCounter<true>();
        this->InternalSetManagedObject(sharedPtr.Get());
        return *this;
    }
    template <typename OtherT, bool OtherThreadSafe>
    SharedReference& operator=(const SharedReference<OtherT, OtherThreadSafe>& sharedRef) {
        static_assert(OtherThreadSafe == ThreadSafe, "Cannot convert between different thread-safety specializations");
        this->template SetCounter<true>(sharedRef.Counter);
        this->template IncreaseCounter<true>();
        this->InternalSetManagedObject(sharedRef.Get());
        return *this;
    }

    SharedReference& operator=(T* object) {
        if (object == nullptr) {
            Platform::GetPlatform().OnFatalError(u"SharedReference cannot be assigned with nullptr");
            return *this;
        }
        this->template SetCounter<true>(new ReferenceCounter<ThreadSafe>());
        this->InternalSetManagedObject(object);
        return *this;
    }
    bool operator==(const SharedReference& other) const { return this->managedObject == other.managedObject; }
    bool operator!=(const SharedReference& other) const { return !((*this) == other); }
    bool operator==(const T* other) const { return this->managedObject == other; }
    bool operator!=(const T* other) const { return !((*this) == other); }

    operator SharedPointer<T, ThreadSafe>() const { return SharedPointer<T, ThreadSafe>(*this); }
    operator WeakPointer<T, ThreadSafe>() const { return WeakPointer<T, ThreadSafe>(*this); }

private:
    SharedReference() : ReferenceCountedPointerBase<T, ThreadSafe>(new ReferenceCounter<ThreadSafe>()) {
        Platform::GetPlatform().OnFatalError(u"A null shared reference is detected. This should not be possible.");
    }
    template <typename, bool> friend struct EnableSharedFromThis;
};

template <typename T, bool ThreadSafe> class WeakPointer final : public ReferenceCountedPointerBase<T, ThreadSafe> {
public:
    WeakPointer() : ReferenceCountedPointerBase<T, ThreadSafe>(nullptr) {}
    // Explicit copy constructor and copy-assignment to ensure weak counters are handled.
    WeakPointer(const WeakPointer& other) : ReferenceCountedPointerBase<T, ThreadSafe>(other.InternalGetCounter()) {
        this->template IncreaseCounter<false>();
        this->managedObject = other.Get();
    }
    WeakPointer(nullptr_t) : ReferenceCountedPointerBase<T, ThreadSafe>(nullptr) {}
    WeakPointer& operator=(const WeakPointer& other) {
        if (this != &other) {
            this->template SetCounter<false>(other.InternalGetCounter());
            this->managedObject = other.Get();
            this->template IncreaseCounter<false>();
        }
        return *this;
    }
    WeakPointer& operator=(const nullptr_t) {
        Reset();
        return *this;
    }
    ~WeakPointer() override { this->template DecreaseCounter<false>(); }

    template <typename OtherT, bool OtherThreadSafe>
    WeakPointer(const SharedPointer<OtherT, OtherThreadSafe>& sharedPtr)
        : ReferenceCountedPointerBase<T, ThreadSafe>(sharedPtr.InternalGetCounter()) {
        static_assert(OtherThreadSafe == ThreadSafe, "Cannot convert between different thread-safety specializations");
        this->template IncreaseCounter<false>();
        this->managedObject = static_cast<T*>(sharedPtr.Get());
    }
    template <typename OtherT, bool OtherThreadSafe>
    WeakPointer(const SharedReference<OtherT, OtherThreadSafe>& sharedRef)
        : ReferenceCountedPointerBase<T, ThreadSafe>(sharedRef.InternalGetCounter()) {
        static_assert(OtherThreadSafe == ThreadSafe, "Cannot convert between different thread-safety specializations");
        this->template IncreaseCounter<false>();
        this->managedObject = static_cast<T*>(sharedRef.Get());
    }
    template <typename OtherT, bool OtherThreadSafe>
    WeakPointer(const WeakPointer<OtherT, OtherThreadSafe>& weakPtr)
        : ReferenceCountedPointerBase<T, ThreadSafe>(weakPtr.InternalGetCounter()) {
        static_assert(OtherThreadSafe == ThreadSafe, "Cannot convert between different thread-safety specializations");
        this->template IncreaseCounter<false>();
        this->managedObject = static_cast<T*>(weakPtr.Get());
    }

    template <typename OtherT, bool OtherThreadSafe>
    WeakPointer& operator=(const SharedPointer<OtherT, OtherThreadSafe>& sharedPtr) {
        static_assert(OtherThreadSafe == ThreadSafe, "Cannot convert between different thread-safety specializations");
        this->template SetCounter<false>(sharedPtr.InternalGetCounter());
        this->template IncreaseCounter<false>();
        this->managedObject = sharedPtr.Get();
        return *this;
    }
    template <typename OtherT, bool OtherThreadSafe>
    WeakPointer& operator=(const SharedReference<OtherT, OtherThreadSafe>& sharedRef) {
        static_assert(OtherThreadSafe == ThreadSafe, "Cannot convert between different thread-safety specializations");
        this->template SetCounter<false>(sharedRef.InternalGetCounter());
        this->managedObject = sharedRef.Get();
        this->template IncreaseCounter<false>();
        return *this;
    }
    template <typename OtherT, bool OtherThreadSafe>
    WeakPointer& operator=(const WeakPointer<OtherT, OtherThreadSafe>& weakPtr) {
        static_assert(OtherThreadSafe == ThreadSafe, "Cannot convert between different thread-safety specializations");
        if (this->InternalGetCounter() != weakPtr.InternalGetCounter()) {
            this->template SetCounter<false>(weakPtr.InternalGetCounter());
            this->managedObject = weakPtr.Get();
            this->template IncreaseCounter<false>();
        }
        return *this;
    }

    SharedPointer<T, ThreadSafe> Lock() const {
        if (this->Counter == nullptr) {
            return SharedPointer<T, ThreadSafe>();
        }
        int prev = this->Counter->FetchAddHard(1);
        if (prev == 0) {
            // object already expired; undo increment and fail
            this->Counter->FetchAddHard(-1);
            return SharedPointer<T, ThreadSafe>();
        }
        SharedPointer<T, ThreadSafe> locked;
        locked.template SetCounter<true>(this->Counter);
        locked.InternalSetManagedObject(this->managedObject);
        return locked;
    }

    void Reset() {
        this->InternalSetManagedObject(nullptr);
        this->template DecreaseCounter<false>();
        this->InternalReleaseCounter();
    }

    bool operator==(const WeakPointer& other) const { return this->managedObject == other.managedObject; }
    bool operator!=(const WeakPointer& other) const { return !((*this) == other); }
    bool operator==(const T* other) const { return this->managedObject == other; }
    bool operator!=(const T* other) const { return !((*this) == other); }
};

template <typename T> class UniquePointer {
public:
    UniquePointer() : ptr(nullptr) {}

    explicit UniquePointer(T* inPtr) : ptr(inPtr) {}
    ~UniquePointer() { delete ptr; }
    UniquePointer(const UniquePointer&) = delete;
    UniquePointer& operator=(const UniquePointer&) = delete;
    UniquePointer(UniquePointer&& other) noexcept : ptr(other.ptr) { other.ptr = nullptr; }
    UniquePointer& operator=(UniquePointer&& other) noexcept {
        if (this != &other) {
            delete ptr;
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    T& operator*() const { return *ptr; }
    T* operator->() const { return ptr; }
    T* Get() const { return ptr; }

    T* ReleaseOwnership() {
        T* temp = ptr;
        ptr = nullptr;
        return temp;
    }

private:
    T* ptr;
};

template <typename T, bool ThreadSafe> struct EnableSharedFromThis : _z__Private::EnableSharedFromThisBase {
public:
    virtual ~EnableSharedFromThis() = default;
    template <typename U> SharedPointer<U, ThreadSafe> SharedFromThis(const U* /*derived*/) const {
        if (weakThis.InternalGetCounter() == nullptr) {
            Platform::GetPlatform().OnFatalError(u"EnableSharedFromThis: Object is not managed by SharedPointer.");
            return SharedPointer<U, ThreadSafe>();
        }
        return weakThis.Lock();
    }

    SharedReference<T, ThreadSafe> AsShared() const {
        if (weakThis.InternalGetCounter() == nullptr) {
            Platform::GetPlatform().OnFatalError(u"EnableSharedFromThis: Object is not managed by SharedPointer.");
            return SharedReference<T, ThreadSafe>();
        }
        return weakThis.Lock().ToSharedReference();
    }

private:
    template <typename, bool> friend class ReferenceCountedPointerBase;
    WeakPointer<T, ThreadSafe> weakThis;
};

template <typename ValueT, bool ThreadSafe>
void ReferenceCountedPointerBase<ValueT, ThreadSafe>::InternalSetManagedObject(ValueT* object) {
    if (object == this->managedObject) {
        return;
    }
    managedObject = object;
    if constexpr (_z__Private::is_derived_from_EnableSharedFromThisBase<ValueT>()) {
        if (this->managedObject != nullptr) {
            auto* base = reinterpret_cast<EnableSharedFromThis<ValueT, ThreadSafe>*>(this->managedObject);
#if _DEBUG
            if (Counter->LoadHard() == 0) {
                Platform::GetPlatform().OnFatalError(
                    u"EnableSharedFromThis: Managed object has zero hard reference count. "
                    u"This should not be possible.");
            }
#endif
            if (!base->weakThis.IsValid()) {
                base->weakThis.template SetCounter<false>(static_cast<ReferenceCounter<ThreadSafe>*>(Counter));
                base->weakThis.InternalSetManagedObjectDirect(static_cast<ValueT*>(managedObject));
                base->weakThis.template IncreaseCounter<false>();
            }
        }
    }
}
} // namespace Edvar::Memory

namespace Edvar {
template <typename T, bool ThreadSafe = false> using WeakPointer = Edvar::Memory::WeakPointer<T, ThreadSafe>;

template <typename T, bool ThreadSafe = false> using SharedPointer = Edvar::Memory::SharedPointer<T, ThreadSafe>;

template <typename T, bool ThreadSafe = false> using SharedReference = Edvar::Memory::SharedReference<T, ThreadSafe>;

template <typename T, bool ThreadSafe = false> using UniquePointer = Edvar::Memory::UniquePointer<T>;

template <typename T, typename... ArgsT>
UniquePointer<T> MakeUnique(ArgsT&&... args)
    requires(std::is_constructible_v<T, ArgsT...>)
{
    return UniquePointer<T>(new T(std::forward<ArgsT>(args)...));
}

template <typename T, typename... ArgsT> SharedReference<T> MakeShared(ArgsT&&... args) {
    return SharedReference<T>(new T(std::forward<ArgsT>(args)...));
}

template <typename ToT, typename FromT>
SharedPointer<ToT> StaticCastSharedPointer(const SharedPointer<FromT>& fromPtr) {
    SharedPointer<ToT> castedPtr;
    if (fromPtr.IsValid()) {
        castedPtr.template SetCounter<true>(fromPtr.InternalGetCounter());
        ToT* castedObj = static_cast<ToT*>(fromPtr.Get());
        castedPtr.template IncreaseCounter<true>();
        castedPtr.InternalSetManagedObject(castedObj);
    }
    return castedPtr;
}

template <typename ToT, typename FromT>
SharedReference<ToT> StaticCastSharedReference(const SharedReference<FromT>& fromRef) {
    SharedReference<ToT> castedPtr = StaticCastSharedPointer<ToT>(SharedPointer<FromT>(fromRef)).ToSharedReference();
    return castedPtr;
}

template <typename ToT, typename FromT> WeakPointer<ToT> StaticCastWeakPointer(const WeakPointer<FromT>& fromPtr) {
    WeakPointer<ToT> castedPtr;
    if (fromPtr.IsValid()) {
        castedPtr.template SetCounter<false>(fromPtr.InternalGetCounter());
        ToT* castedObj = static_cast<ToT*>(fromPtr.Get());
        castedPtr.template IncreaseCounter<false>();
        castedPtr.InternalSetManagedObject(castedObj);
    }
    return castedPtr;
}
} // namespace Edvar
