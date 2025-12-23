#pragma once

namespace Edvar::Containers {
template <typename ListT, bool IsConst> class ListIterator;
template <typename T, typename AllocatorT> struct List {
public:
    List();
    List(const List& other)
        requires(std::is_copy_constructible_v<T>);
    List(List&& other) noexcept;
    ~List();

    List& operator=(const List& other);

    List& operator=(List&& other) noexcept;

    using AllocatorType = AllocatorT;
    using DataType = T;

    void Clear();

    void EnsureCapacity(int32_t NewCapacity);

    void Resize(int32_t NewSize, bool CanShrink = false);

    int32_t Add(const DataType& InElement)
        requires(std::is_copy_constructible_v<DataType>);

    int32_t Add(DataType&& InElement)
        requires(std::is_move_constructible_v<DataType>);

    int32_t AddUnique(const DataType& InElement)
        requires(std::is_copy_constructible_v<DataType>);

    int32_t AddUnique(DataType&& InElement)
        requires(std::is_move_constructible_v<DataType>);

    int32_t AddZeroed(int32_t Count);

    int32_t AddDefaulted(int32_t Count)
        requires(std::is_default_constructible_v<DataType>);

    void Append(const DataType* InElements, int32_t Count)
        requires(std::is_copy_constructible_v<DataType>);

    template <bool ShouldEnsureCapacity = true> void ShiftElements(int32_t StartIndex, int64_t ShiftBy);

    void Insert(int32_t Index, const DataType& InElement)
        requires(std::is_copy_constructible_v<DataType>);

    void Insert(int32_t Index, DataType&& InElement)
        requires(std::is_move_constructible_v<DataType>);

    template <typename... ArgsT>
    void Emplace(int32_t Index, ArgsT&&... Args)
        requires(std::is_constructible_v<DataType, ArgsT...>);

    DataType RemoveAt(int32_t Index)
        requires(std::is_move_constructible_v<DataType> || std::is_copy_constructible_v<DataType>);

    int32_t IndexOf(const T& Value) const;

    int32_t IndexOf(bool (*Predicate)(const T&)) const;

    DataType Remove(const T& Value)
        requires(std::is_move_constructible_v<DataType> || std::is_copy_constructible_v<DataType>);

    void Push(const DataType& InElement)
        requires(std::is_copy_constructible_v<DataType>);

    void Push(DataType&& InElement)
        requires(std::is_move_constructible_v<DataType>);

    DataType Pop()
        requires(std::is_move_constructible_v<DataType> || std::is_copy_constructible_v<DataType>);

    [[nodiscard]] DataType& Get(int32_t Index);
    [[nodiscard]] const DataType& Get(int32_t Index) const;

    [[nodiscard]] int32_t Length() const;

    DataType* Data();
    const DataType* Data() const;

    DataType& operator[](int32_t Index);
    const DataType& operator[](int32_t Index) const;

    using IteratorType = ListIterator<List<T, AllocatorT>, false>;
    using ConstIteratorType = ListIterator<List<T, AllocatorT>, true>;

    IteratorType GetIterator();
    ConstIteratorType GetIterator() const;

    IteratorType begin();
    IteratorType end();

    ConstIteratorType begin() const;
    ConstIteratorType end() const;
    ConstIteratorType cbegin() const;
    ConstIteratorType cend() const;

private:
    int32_t Size;
    int32_t Capacity;
    AllocatorType Allocator;
};

template <typename ListT, bool IsConst> class ListIterator {
public:
    using DataType = ListT::DataType;
    using AllocatorType = ListT::AllocatorType;

    ListIterator(ListT& InList);
    [[nodiscard]] bool HasNext() const;
    DataType& Next();

    [[nodiscard]] bool HasPrevious() const;
    DataType& Previous();

    void Seek(int32_t Index);

    void Insert(const DataType& InElement)
        requires(std::is_copy_constructible_v<DataType> && !IsConst);
    void Insert(DataType&& InElement)
        requires(std::is_move_constructible_v<DataType> && !IsConst);
    template <typename... ArgsT>
    void Emplace(ArgsT&&... Args)
        requires(std::is_constructible_v<DataType, ArgsT...> && !IsConst);

    void Remove();

    void Reset();

    ListIterator& operator++();
    ListIterator& operator--();

    bool operator==(const ListIterator& other) const;
    bool operator!=(const ListIterator& other) const;
    DataType& operator*();
    DataType* operator->();

    operator bool() const { return HasNext(); }

private:
    ListT& listRef;
    int32_t currentIndex;
};

} // namespace Edvar::Containers