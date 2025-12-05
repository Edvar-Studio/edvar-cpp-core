#pragma once

namespace Edvar::Utils {
template <typename Signature> struct Function;

template <typename RetT, typename... ArgsT> struct Function<RetT(ArgsT...)> {
    using ReturnType = RetT;
    using ArgsTuple = std::tuple<ArgsT...>;

private:
    struct ICallable {
        virtual ~ICallable() = default;
        virtual RetT Invoke(ArgsT... args) = 0;
        virtual ICallable* Clone() const = 0;
    };

    template <typename FuncT> struct CallableImpl : public ICallable {
        FuncT func;
        CallableImpl(FuncT&& f) : func(std::forward<FuncT>(f)) {}
        RetT Invoke(ArgsT... args) override { return func(std::forward<ArgsT>(args)...); }
        ICallable* Clone() const override { return new CallableImpl<FuncT>(func); }
    };
    ICallable* callable = nullptr;

public:
    Function() = default;
    ~Function() { delete callable; }

    Function(const Function& other) : callable(other.callable != nullptr ? other.callable->Clone() : nullptr) {}
    Function(Function&& other) noexcept : callable(other.callable) { other.callable = nullptr; }

    Function(RetT (*funcPtr)(ArgsT...)) : callable(new CallableImpl(funcPtr)) {}
    template <typename FuncT>
    Function(FuncT&& func) : callable(new CallableImpl<std::decay_t<FuncT>>(std::forward<FuncT>(func))) {}

    bool IsValid() const { return callable != nullptr; }
    void Invoke(ArgsT... args) const {
        if (callable != nullptr) {
            callable->Invoke(std::forward<ArgsT>(args)...);
        }
    }
    operator bool() const { return IsValid(); }
    RetT operator()(ArgsT... args) const { return Invoke(std::forward<ArgsT>(args)...); }
    Function const& operator=(const Function& other) {
        if (this != &other) {
            delete callable;
            callable = other.callable != nullptr ? other.callable->Clone() : nullptr;
        }
        return *this;
    }
    Function const& operator=(Function&& other) noexcept {
        if (this != &other) {
            delete callable;
            callable = other.callable;
            other.callable = nullptr;
        }
        return *this;
    }
};

template <typename Signature> struct FunctionRef;

template <typename RetT, typename... ArgsT> struct FunctionRef<RetT(ArgsT...)> {
    using ReturnType = RetT;
    using ArgsTuple = std::tuple<ArgsT...>;
    template <typename FuncT> FunctionRef(FuncT& func) {
        obj = static_cast<void*>(&func);
        caller = [](void* o, ArgsT... args) -> RetT { return (*static_cast<FuncT*>(o))(std::forward<ArgsT>(args)...); };
    }
    template <typename FuncT> FunctionRef(FuncT&&) = delete;
    RetT Invoke(ArgsT... args) const { return caller(obj, std::forward<ArgsT>(args)...); }
    bool IsValid() const { return obj != nullptr && caller != nullptr; }
    operator bool() const { return IsValid(); }
    RetT operator()(ArgsT... args) const { return Invoke(std::forward<ArgsT>(args)...); }

private:
    void* obj = nullptr;
    RetT (*caller)(void*, ArgsT...) = nullptr;
};

template <typename Signature> struct Delegate;

template <typename RetT, typename... ArgsT> struct Delegate<RetT(ArgsT...)> {
    using ReturnType = RetT;
    using ArgsTuple = std::tuple<ArgsT...>;

private:
    struct ICallable {
        virtual ~ICallable() = default;
        virtual RetT Invoke(ArgsT... args) = 0;
        virtual ICallable* Clone() const = 0;
    };

    template <typename FuncT, typename... BoundArgumentsT> struct FreeCallableImpl : public ICallable {
        FuncT func;
        FreeCallableImpl(FuncT&& f, BoundArgumentsT&&... boundArgs)
            : func(std::forward<FuncT>(f)), boundArguments(std::forward<BoundArgumentsT>(boundArgs)...) {}
        RetT Invoke(ArgsT... args) override {
            return boundArguments.template Apply<RetT>([&](BoundArgumentsT&&... boundVars) {
                return func(std::forward<ArgsT>(args)..., std::forward<BoundArgumentsT>(boundVars)...);
            });
        }
        ICallable* Clone() const override { return new FreeCallableImpl<FuncT>(func); }

        Containers::Tuple<BoundArgumentsT...> boundArguments;
    };

    template <typename ObjT, typename MethodT, typename... BoundArgumentsT> struct RawCallableImpl : public ICallable {
        ObjT* object;
        MethodT method;
        Containers::Tuple<BoundArgumentsT...> boundArguments;
        RawCallableImpl(ObjT* obj, MethodT m, BoundArgumentsT&&... boundArgs)
            : object(obj), method(m), boundArguments(std::forward<BoundArgumentsT>(boundArgs)...) {}
        RetT Invoke(ArgsT... args) override {
            return boundArguments.template Apply<RetT>([&](BoundArgumentsT&&... boundVars) {
                return (object->*method)(std::forward<ArgsT>(args)..., std::forward<BoundArgumentsT>(boundVars)...);
            });
        }
        ICallable* Clone() const override {
            return new RawCallableImpl<ObjT, MethodT, BoundArgumentsT...>(object, method, boundArguments);
        }
    };

    template <typename ObjT, typename MethodT, typename... BoundArgumentsT> struct WeakCallableImpl : public ICallable {
        WeakPointer<ObjT> object;
        MethodT method;
        Containers::Tuple<BoundArgumentsT...> boundArguments;
        WeakCallableImpl(const WeakPointer<ObjT>& obj, MethodT m, BoundArgumentsT&&... boundArgs)
            : object(obj), method(m), boundArguments(std::forward<BoundArgumentsT>(boundArgs)...) {}
        RetT Invoke(ArgsT... args) override {
            if (SharedPointer<ObjT> objShared = object.Lock()) {
                return boundArguments.template Apply<RetT>([&](BoundArgumentsT&&... boundVars) {
                    return (objShared->*method)(std::forward<ArgsT>(args)...,
                                                std::forward<BoundArgumentsT>(boundVars)...);
                });
            }
        }
        ICallable* Clone() const override { return new WeakCallableImpl<ObjT, MethodT>(object, method); }
    };

    template <typename ObjT, typename MethodT, typename... BoundArgumentsT>
    struct SharedCallableImpl : public ICallable {
        // It is shared, but we don't want to hold a hard reference to avoid ownership/lifetime issues. So hold as weak.
        WeakPointer<ObjT> object;
        MethodT method;
        Containers::Tuple<BoundArgumentsT...> boundArguments;
        SharedCallableImpl(const SharedPointer<ObjT>& obj, MethodT m, BoundArgumentsT&&... boundArgs)
            : object(obj), method(m), boundArguments(std::forward<BoundArgumentsT>(boundArgs)...) {}
        RetT Invoke(ArgsT... args) override {
            if (object.IsValid()) {
                return boundArguments.template Apply<RetT>([&](BoundArgumentsT&&... boundVars) {
                    return (object->*method)(std::forward<ArgsT>(args)..., std::forward<BoundArgumentsT>(boundVars)...);
                });
            }
        }
        ICallable* Clone() const override {
            return new SharedCallableImpl<ObjT, MethodT, BoundArgumentsT...>(object, method, boundArguments);
        }
    };

    ICallable* binding = nullptr;

public:
    Delegate() = default;
    ~Delegate() { delete binding; }

    bool IsValid() const { return binding != nullptr; }

    void InvokeIfBound(RetT& returnValue, ArgsT&&... args) const {
        if (binding != nullptr) {
            returnValue = binding->Invoke(std::forward<ArgsT>(args)...);
        }
    }

    RetT Invoke(ArgsT&&... args) const {
        if (binding != nullptr) {
            return binding->Invoke(std::forward<ArgsT>(args)...);
        }
        Platform::Get().OnFatalError(
            Utils::CStrings::CreatePrintFString(u"Delegate: Attempted to invoke an invalid delegate."));
        // Make sure compile is happy about this.
        return RetT();
    }

    template <typename... BoundArgumentsT> void BindStatic(RetT (*func)(ArgsT...), BoundArgumentsT&&... boundArgs) {
        delete binding;
        binding = new FreeCallableImpl<decltype(func)>(func, std::forward<BoundArgumentsT>(boundArgs)...);
    }

    template <typename ObjectT, typename... BoundArgumentsT>
    void BindRaw(ObjectT* object, RetT (ObjectT::*method)(ArgsT...), BoundArgumentsT&&... boundArgs) {
        delete binding;
        binding = new RawCallableImpl<ObjectT, decltype(method), BoundArgumentsT...>(
            object, method, std::forward<BoundArgumentsT>(boundArgs)...);
    }

    template <typename ObjectT, typename... BoundArgumentsT>
    void BindShared(const SharedPointer<ObjectT>& object, RetT (ObjectT::*method)(ArgsT...),
                    BoundArgumentsT&&... boundArgs) {
        delete binding;
        binding = new SharedCallableImpl<ObjectT, decltype(method), BoundArgumentsT...>(
            object, method, std::forward<BoundArgumentsT>(boundArgs)...);
    }

    template <typename ObjectT, typename... BoundArgumentsT>
    void BindWeak(const WeakPointer<ObjectT>& object, RetT (ObjectT::*method)(ArgsT...),
                  BoundArgumentsT&&... boundArgs) {
        delete binding;
        binding = new WeakCallableImpl<ObjectT, decltype(method), BoundArgumentsT...>(
            object, method, std::forward<BoundArgumentsT>(boundArgs)...);
    }

    void Reset() {
        delete binding;
        binding = nullptr;
    }
};
} // namespace Edvar::Utils