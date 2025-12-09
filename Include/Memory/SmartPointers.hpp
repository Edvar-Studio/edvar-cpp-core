#pragma once

#include "Memory/Atomic.hpp"
#include <type_traits>
namespace Edvar::Memory {
template <typename T, bool ThreadSafe = false> class SharedPointer;
template <typename T, bool ThreadSafe = false> class SharedReference;
template <typename T, bool ThreadSafe = false> class WeakPointer;
template <typename T> struct EnableSharedFromThis;
namespace _z__Private {
struct EnableSharedFromThisBase {};
} // namespace _z__Private

template <bool ThreadSafe = false> class ReferenceCounter {
public:
    ReferenceCounter() : HardCounter(1), WeakCounter(0) {}
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
    int GetHardCount() const { return HardCounter; }

    void IncrementWeak() { ++WeakCounter; }
    void DecrementWeak() { --WeakCounter; }
    int GetWeakCount() const { return WeakCounter; }

private:
    using CounterValueType = std::conditional_t<ThreadSafe, Memory::Atomic<int>, int>;
    CounterValueType HardCounter;
    CounterValueType WeakCounter;
};

template <typename ValueT, bool ThreadSafe> class ReferenceCountedPointerBase {
public:
    virtual ~ReferenceCountedPointerBase() = default;
    ValueT* Get() const { return managedObject; }
    ValueT* operator->() const { return Get(); }
    ValueT& operator*() const { return *Get(); }

    [[nodiscard]] bool IsValid() const {
        return this->managedObject != nullptr && this->Counter != nullptr && this->Counter->GetHardCount() > 0;
    }

    template <bool UsesHardCounter> void DecreaseCounter() {
        if (!IsValid()) {
            return;
        }
        if constexpr (UsesHardCounter) {
            this->Counter->DecrementHard();
            if (this->Counter->GetHardCount() == 0) {
                delete this->managedObject;
                if (this->Counter->GetWeakCount() == 0) {
                    delete this->Counter;
                }
            }
        } else {
            this->Counter->DecrementWeak();
            if (this->Counter->GetHardCount() == 0 && this->Counter->GetWeakCount() == 0) {
                delete this->Counter;
            }
        }
    }

    template <bool UsesHardCounter> void IncreaseCounter() {
        if (!IsValid()) {
            return;
        }
        if constexpr (UsesHardCounter) {
            this->Counter->IncrementHard();
        } else {
            this->Counter->IncrementWeak();
        }
    }

    template <bool UsesHardCounter> void SetCounter(ReferenceCounter<ThreadSafe>* newCounter) {
        DecreaseCounter<UsesHardCounter>();
        this->Counter = newCounter;
    }

    ReferenceCounter<ThreadSafe>* InternalGetCounter() const { return this->Counter; }

    void InternalSetManagedObject(ValueT* object) {
        if (object == this->managedObject) {
            return;
        }
        managedObject = object;
        if constexpr (std::is_base_of_v<_z__Private::EnableSharedFromThisBase, ValueT>) {
            if (this->managedObject != nullptr) {
                auto* enableShared = static_cast<EnableSharedFromThis<ValueT>*>(this->managedObject);
                enableShared->weakThis = WeakPointer<ValueT, ThreadSafe>();
                enableShared->weakThis.SetCounter(this->Counter);
                enableShared->weakThis.managedObject = this->managedObject;
                enableShared->weakThis.template IncreaseCounter<false>();
            }
        }
    }

protected:
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
};

template <typename T, bool ThreadSafe> class SharedPointer final : public ReferenceCountedPointerBase<T, ThreadSafe> {
public:
    SharedPointer() : ReferenceCountedPointerBase<T, ThreadSafe>(new ReferenceCounter<ThreadSafe>()) {}
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
        : ReferenceCountedPointerBase<T, ThreadSafe>(other.GetCounter()) {
        this->InternalSetManagedObject(other.Get());
        this->template IncreaseCounter<true>();
    }

    template <typename OtherT, bool OtherThreadSafe>
    SharedPointer& operator=(const SharedPointer<OtherT, OtherThreadSafe>& other)
        requires(std::is_convertible_v<OtherT*, T*> || std::is_base_of_v<T, OtherT>)
    {
        if (this != &other) {
            this->template SetCounter<true>(other.GetCounter());
            this->InternalSetManagedObject(other.Get());
            this->template IncreaseCounter<true>();
        }
        return *this;
    }
    template <typename OtherT, bool OtherThreadSafe>
    SharedPointer& operator=(SharedPointer<OtherT, OtherThreadSafe>&& other) noexcept {
        if (this->Get() != other.Get()) {
            this->template SetCounter<true>(other.InternalGetCounter());
            this->InternalSetManagedObject(other.Get());
            other.template SetCounter<true>(nullptr);
            other.InternalSetManagedObject(nullptr);
        }
        return *this;
    }
    template <typename OtherT, bool OtherThreadSafe>
    SharedPointer(SharedPointer<OtherT, OtherThreadSafe>&& other) noexcept
        : ReferenceCountedPointerBase<T, ThreadSafe>(other.InternalGetCounter()) {
        this->InternalSetManagedObject(other.Get());
        other.template SetCounter<true>(nullptr);
        other.InternalSetManagedObject(nullptr);
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
            Platform::Get().OnFatalError(u"SharedReference cannot be initialized with nullptr");
            return;
        }
        this->InternalSetManagedObject(object);
        this->template IncreaseCounter<true>();
    }

    SharedReference(const SharedReference& other) : ReferenceCountedPointerBase<T, ThreadSafe>(other.Counter) {
        this->InternalSetManagedObject(other.managedObject);
        this->template IncreaseCounter<true>();
    }

    SharedReference(SharedReference&& other) noexcept : ReferenceCountedPointerBase<T, ThreadSafe>(other.Counter) {
        this->InternalSetManagedObject(other.managedObject);
        other.template SetCounter<true>(nullptr);
        other.InternalSetManagedObject(nullptr);
    }

    template <typename OtherT, bool OtherThreadSafe>
    SharedReference(const SharedPointer<OtherT, OtherThreadSafe>& sharedPtr)
        : ReferenceCountedPointerBase<T, ThreadSafe>(sharedPtr.InternalGetCounter()) {
        this->InternalSetManagedObject(sharedPtr.Get());
        this->template IncreaseCounter<true>();
    }
    template <typename OtherT, bool OtherThreadSafe>
    SharedReference(const SharedReference<OtherT, OtherThreadSafe>& sharedRef)
        : ReferenceCountedPointerBase<T, ThreadSafe>(sharedRef.InternalGetCounter()) {
        this->InternalSetManagedObject(sharedRef.Get());
        this->template IncreaseCounter<true>();
    }
    ~SharedReference() override { this->template DecreaseCounter<true>(); }
    SharedReference operator=(nullptr_t) = delete;

    SharedReference& operator=(const SharedReference& other) {
        if (this != &other) {
            this->template SetCounter<true>(other.Counter);
            this->template IncreaseCounter<true>();
            this->InternalSetManagedObject(other.Get());
            return *this;
        }
        return *this;
    }

    template <typename OtherT, bool OtherThreadSafe>
    SharedReference& operator=(const SharedPointer<OtherT, OtherThreadSafe>& sharedPtr) {
        this->template SetCounter<true>(sharedPtr.Counter);
        this->template IncreaseCounter<true>();
        this->InternalSetManagedObject(sharedPtr.Get());
        return *this;
    }
    template <typename OtherT, bool OtherThreadSafe>
    SharedReference& operator=(const SharedReference<OtherT, OtherThreadSafe>& sharedRef) {
        this->template SetCounter<true>(sharedRef.Counter);
        this->template IncreaseCounter<true>();
        this->InternalSetManagedObject(sharedRef.Get());
        return *this;
    }

    SharedReference& operator=(T* object) {
        if (object == nullptr) {
            Platform::Get().OnFatalError(u"SharedReference cannot be assigned with nullptr");
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
        Platform::Get().OnFatalError(u"A null shared reference is detected. This should not be possible.");
    }
    template <typename> friend struct EnableSharedFromThis;
};

template <typename T, bool ThreadSafe> class WeakPointer final : public ReferenceCountedPointerBase<T, ThreadSafe> {
public:
    WeakPointer() : ReferenceCountedPointerBase<T, ThreadSafe>(new ReferenceCounter<ThreadSafe>()) {}
    ~WeakPointer() override { this->template DecreaseCounter<false>(); }

    template <typename OtherT, bool OtherThreadSafe>
    WeakPointer(const SharedPointer<OtherT, OtherThreadSafe>& sharedPtr)
        : ReferenceCountedPointerBase<T, ThreadSafe>(sharedPtr.InternalGetCounter()) {
        this->template IncreaseCounter<false>();
        this->managedObject = sharedPtr.Get();
    }
    template <typename OtherT, bool OtherThreadSafe>
    WeakPointer(const SharedReference<OtherT, OtherThreadSafe>& sharedRef)
        : ReferenceCountedPointerBase<T, ThreadSafe>(sharedRef.InternalGetCounter()) {
        this->template IncreaseCounter<false>();
        this->managedObject = sharedRef.Get();
    }
    template <typename OtherT, bool OtherThreadSafe>
    WeakPointer(const WeakPointer<OtherT, OtherThreadSafe>& weakPtr)
        : ReferenceCountedPointerBase<T, ThreadSafe>(weakPtr.InternalGetCounter()) {
        this->template IncreaseCounter<false>();
    }

    template <typename OtherT, bool OtherThreadSafe>
    WeakPointer& operator=(const SharedPointer<OtherT, OtherThreadSafe>& sharedPtr) {
        this->template SetCounter<false>(sharedPtr.InternalGetCounter());
        this->template IncreaseCounter<false>();
        this->managedObject = sharedPtr.Get();
        return *this;
    }
    template <typename OtherT, bool OtherThreadSafe>
    WeakPointer& operator=(const SharedReference<OtherT, OtherThreadSafe>& sharedRef) {
        this->template SetCounter<false>(sharedRef.InternalGetCounter());
        this->template IncreaseCounter<false>();
        this->managedObject = sharedRef.Get();
        return *this;
    }
    template <typename OtherT, bool OtherThreadSafe>
    WeakPointer& operator=(const WeakPointer<OtherT, OtherThreadSafe>& weakPtr) {
        this->template SetCounter<false>(weakPtr.InternalGetCounter());
        this->template IncreaseCounter<false>();
        return *this;
    }

    SharedPointer<T, ThreadSafe> Lock() const {
        if (this->Counter->GetHardCount() > 0) {
            SharedPointer<T, ThreadSafe> locked = SharedPointer<T, ThreadSafe>(this->managedObject);
            locked.template SetCounter<true>(this->Counter);
            locked.template IncreaseCounter<true>();
            return locked;
        }
        return SharedPointer<T, ThreadSafe>();
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

template <typename T> struct EnableSharedFromThis {
public:
    virtual ~EnableSharedFromThis() = default;
    template <typename U> SharedPointer<U> SharedFromThis(const U* derived) const {
        if (weakThis.Get() == nullptr) {
            Platform::Get().OnFatalError(u"EnableSharedFromThis: Object is not managed by SharedPointer.");
            return SharedPointer<U>();
        }
        return weakThis.Lock().ToSharedPointer();
    }

    SharedReference<T> AsShared() const {
        if (weakThis.Get() == nullptr) {
            Platform::Get().OnFatalError(u"EnableSharedFromThis: Object is not managed by SharedPointer.");
            return SharedReference<T>();
        }
        return weakThis.Lock().ToSharedReference();
    }

private:
    template <typename, bool> friend class ReferenceCountedPointerBase;
    WeakPointer<T> weakThis;
};

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
    ToT* castedPtr = static_cast<ToT*>(fromPtr.Get());
    SharedPointer<ToT> toPtr(castedPtr);
    toPtr.template SetCounter<true>(fromPtr.InternalGetCounter());
    toPtr.template IncreaseCounter<true>();
    return toPtr;
}

template <typename ToT, typename FromT>
SharedReference<ToT> StaticCastSharedReference(const SharedReference<FromT>& fromRef) {
    ToT* castedPtr = static_cast<ToT*>(fromRef.Get());
    SharedReference<ToT> toRef(castedPtr);
    toRef.template SetCounter<true>(fromRef.InternalGetCounter());
    toRef.template IncreaseCounter<true>();
    return toRef;
}

template <typename ToT, typename FromT> WeakPointer<ToT> StaticCastWeakPointer(const WeakPointer<FromT>& fromPtr) {
    ToT* castedPtr = static_cast<ToT*>(fromPtr.Get());
    WeakPointer<ToT> toPtr;
    toPtr.managedObject = castedPtr;
    toPtr.template SetCounter<false>(fromPtr.InternalGetCounter());
    toPtr.template IncreaseCounter<false>();
    return toPtr;
}
} // namespace Edvar
