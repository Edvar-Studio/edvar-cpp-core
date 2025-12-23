#pragma once

namespace Edvar::Containers {

namespace _TuplePrivate {
// Helper to get type at index (use signed index to allow I-1 without unsigned wrap)
template <int32_t I, typename... Ts> struct TupleElementType;

template <int32_t I, typename Head, typename... Tail> struct TupleElementType<I, Head, Tail...> {
    using Type = typename TupleElementType<I - 1, Tail...>::Type;
};

template <typename Head, typename... Tail> struct TupleElementType<0, Head, Tail...> {
    using Type = Head;
};

template <int32_t I, typename... Ts> using TupleElementTypeT = typename TupleElementType<I, Ts...>::Type;

// Helper to get reference to element type at index
template <int32_t I, typename... Ts> struct TupleElementReference {
    using Type = TupleElementTypeT<I, Ts...>&;
};

template <int32_t I, typename... Ts> struct TupleElementConstReference {
    using Type = const TupleElementTypeT<I, Ts...>&;
};

template <int32_t I, typename... Ts> using TupleElementReferenceT = typename TupleElementReference<I, Ts...>::Type;
template <int32_t I, typename... Ts>
using TupleElementConstReferenceT = typename TupleElementConstReference<I, Ts...>::Type;

// Helper to count tuple elements
template <typename... Ts> struct TupleSize;

template <> struct TupleSize<> {
    static constexpr uint32_t Value = 0;
};

template <typename Head, typename... Tail> struct TupleSize<Head, Tail...> {
    static constexpr uint32_t Value = 1 + TupleSize<Tail...>::Value;
};

template <typename... Ts> constexpr uint32_t TupleSizeV = TupleSize<Ts...>::Value;

// Forward declaration
template <typename... Ts> class TupleStorage;

// Base case: empty tuple
template <> class TupleStorage<> {
public:
    TupleStorage() = default;
    ~TupleStorage() = default;

    template <int32_t I> void Get() {
        static_assert(I < 0, "Tuple index out of bounds");
        Platform::GetPlatform().OnFatalError(u"Tuple index out of bounds");
    }

    template <int32_t I> void Get() const {
        static_assert(I < 0, "Tuple index out of bounds");
        Platform::GetPlatform().OnFatalError(u"Tuple index out of bounds");
    }
};

// Recursive case: holds one element and delegates to the rest
template <typename Head, typename... Tail> class TupleStorage<Head, Tail...> : private TupleStorage<Tail...> {
private:
    Head head;

    using BaseType = TupleStorage<Tail...>;

public:
    TupleStorage() : BaseType(), head() {}

    template <typename H, typename... T> explicit TupleStorage(const H& h, const T&... t) : BaseType(t...), head(h) {}

    template <typename H, typename... T>
    explicit TupleStorage(H&& h, T&&... t) : BaseType(static_cast<T&&>(t)...), head(static_cast<H&&>(h)) {}

    ~TupleStorage() = default;

    // Get element at index 0 (head)
    template <int32_t I>
    Head& Get()
        requires(I == 0)
    {
        return head;
    }

    template <int32_t I>
    const Head& Get() const
        requires(I == 0)
    {
        return head;
    }

    // Get element at index > 0 (delegate to base)
    template <int32_t I>
    auto Get() -> decltype(std::declval<BaseType>().template Get<I - 1>())
        requires(I > 0)
    {
        return BaseType::template Get<I - 1>();
    }

    template <int32_t I>
    auto Get() const -> decltype(std::declval<const BaseType>().template Get<I - 1>())
        requires(I > 0)
    {
        return BaseType::template Get<I - 1>();
    }

    // Helper to apply callable with all elements as arguments
    template <typename ReturnT, typename Func, uint32_t... Is>
    ReturnT ApplyWithIndices(Func& func, ::Edvar::Utils::IndexSequence<Is...>) {
        return func(Get<static_cast<int32_t>(Is)>()...);
    }

    template <typename ReturnT, typename Func, uint32_t... Is>
    ReturnT ApplyWithIndicesConst(Func& func, ::Edvar::Utils::IndexSequence<Is...>) const {
        return func(Get<static_cast<int32_t>(Is)>()...);
    }
};

} // namespace _TuplePrivate

/**
 * @brief A generic tuple container that holds heterogeneous types
 * @tparam Ts The types to store in the tuple
 *
 * Example usage:
 *   Tuple<int, double, char> t(42, 3.14, 'a');
 *   auto first = t.Get<0>();  // int
 *   auto second = t.Get<1>(); // double
 *   auto third = t.Get<2>();  // char
 */
template <typename... Ts> class Tuple : private _TuplePrivate::TupleStorage<Ts...> {
private:
    using BaseType = _TuplePrivate::TupleStorage<Ts...>;

public:
    /**
     * @brief Default constructor - initializes all elements with their default values
     */
    Tuple() : BaseType() {}

    /**
     * @brief Constructor with values
     * @tparam Args Types of the values to initialize with
     * @param args Values to initialize the tuple with
     */
    template <typename... Args> explicit Tuple(const Args&... args) : BaseType(args...) {}

    /**
     * @brief Move constructor
     * @tparam Args Types of the values to initialize with
     * @param args Values to initialize the tuple with (moved)
     */
    template <typename... Args> explicit Tuple(Args&&... args) : BaseType(static_cast<Args&&>(args)...) {}

    /**
     * @brief Copy constructor
     */
    Tuple(const Tuple&) = default;

    /**
     * @brief Move constructor
     */
    Tuple(Tuple&&) = default;

    /**
     * @brief Copy assignment operator
     */
    Tuple& operator=(const Tuple&) = default;

    /**
     * @brief Move assignment operator
     */
    Tuple& operator=(Tuple&&) = default;

    /**
     * @brief Destructor
     */
    ~Tuple() = default;

    /**
     * @brief Get element at specified index
     * @tparam I The index of the element to retrieve
     * @return Reference to the element at index I
     */
    template <int32_t I>
    auto Get() -> decltype(std::declval<BaseType>().template Get<I>())
        requires(I < static_cast<int32_t>(_TuplePrivate::TupleSizeV<Ts...>))
    {
        return BaseType::template Get<I>();
    }

    /**
     * @brief Get const element at specified index
     * @tparam I The index of the element to retrieve
     * @return Const reference to the element at index I
     */
    template <int32_t I>
    auto Get() const -> decltype(std::declval<const BaseType>().template Get<I>())
        requires(I < static_cast<int32_t>(_TuplePrivate::TupleSizeV<Ts...>))
    {
        return BaseType::template Get<I>();
    }

    /**
     * @brief Get the size of the tuple
     * @return The number of elements in the tuple
     */
    static constexpr uint32_t Size() { return _TuplePrivate::TupleSizeV<Ts...>; }

    /**
     * @brief Check if tuple is empty
     * @return True if the tuple contains no elements, false otherwise
     */
    static constexpr bool IsEmpty() { return Size() == 0; }

    /**
     * @brief Apply a callable with all tuple elements as arguments
     * @tparam Func The callable type
     * @param func The callable to invoke with all tuple elements as arguments
     *
     * Example usage:
     *   Tuple<int, double, char> t(42, 3.14, 'a');
     *   t.Apply([](int i, double d, char c) { ... });
     */
    template <typename ReturnT, typename FuncT> ReturnT Apply(FuncT& func) {
        return BaseType::ApplyWithIndices(func, Utils::MakeIndexSequence<Size()>{});
    }

    /**
     * @brief Apply a callable with all tuple elements as arguments (rvalue callable)
     * @tparam Func The callable type
     * @param func The callable to invoke with all tuple elements as arguments
     */
    template <typename ReturnT, typename Func> void Apply(Func&& func) {
        return BaseType::ApplyWithIndices(func, Utils::MakeIndexSequence<Size()>{});
    }

    /**
     * @brief Apply a callable with all const tuple elements as arguments
     * @tparam Func The callable type
     * @param func The callable to invoke with all tuple elements as arguments
     */
    template <typename ReturnT, typename Func> ReturnT Apply(Func& func) const {
        return BaseType::ApplyWithIndicesConst(func, Utils::MakeIndexSequence<Size()>{});
    }

    /**
     * @brief Apply a callable with all const tuple elements as arguments (rvalue callable)
     * @tparam Func The callable type
     * @param func The callable to invoke with all tuple elements as arguments
     */
    template <typename ReturnT, typename Func> ReturnT Apply(Func&& func) const {
        return BaseType::ApplyWithIndicesConst(func, Utils::MakeIndexSequence<Size()>{});
    }

    template <int FirstElementCount, typename... ArgsT> constexpr static bool StartsSame() {
        if constexpr (FirstElementCount > static_cast<int>(Size()) ||
                      FirstElementCount > static_cast<int>(sizeof...(ArgsT))) {
            return false;
        } else {
            if constexpr (FirstElementCount == 0) {
                return true;
            } else {
                if constexpr (std::is_same_v<
                                  _TuplePrivate::TupleElementTypeT<static_cast<int32_t>(Size()) - FirstElementCount,
                                                                   Ts...>,
                                  _TuplePrivate::TupleElementTypeT<
                                      static_cast<int32_t>(sizeof...(ArgsT)) - FirstElementCount, ArgsT...>>) {
                    return StartsSame<FirstElementCount - 1, ArgsT...>();
                } else {
                    return false;
                }
            }
        }
        return false;
    }
};

template<typename... Ts> requires (sizeof...(Ts) <= 0) Tuple<Ts...> MakeTuple() {return Tuple<Ts...>();}

// Helper function to create a tuple from arguments
template <typename... Ts> requires(sizeof...(Ts) > 0) Tuple<Ts...> MakeTuple(const Ts&... values) { return Tuple<Ts...>(values...); }

// Helper function to create a tuple from rvalue arguments
template <typename... Ts> requires(sizeof...(Ts) > 0) Tuple<Ts...> MakeTuple(Ts&&... values) { return Tuple<Ts...>(static_cast<Ts&&>(values)...); }

template <int32_t I, typename... Ts> using TupleElementT = _TuplePrivate::TupleElementTypeT<I, Ts...>;
} // namespace Edvar::Containers