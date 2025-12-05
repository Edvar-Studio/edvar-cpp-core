#pragma once

#include "Platform/IPlatform.hpp"
#include "Utils/CString.hpp"

namespace Edvar::Containers {
template <typename ListT, bool IsConst> class ListIterator;
template <typename T, typename AllocatorT> struct List {
public:
    List() : Size(0), Capacity(0), Allocator() {}

    using AllocatorType = AllocatorT;
    using DataType = T;
    using IndexType = int32_t;

    void EnsureCapacity(IndexType NewCapacity) {
        if (!Allocator.HasAllocated()) {
            Allocator.Allocate(NewCapacity);
        } else if (NewCapacity > Capacity) {
            Allocator.Resize(NewCapacity);
        }
        Capacity = NewCapacity;
        Size = 0;
    }

    void Resize(IndexType NewSize, const bool CanShrink = false) {
        if (NewSize > Capacity) {
            EnsureCapacity(NewSize);
        } else if (CanShrink && NewSize < Capacity) {
            Allocator.Resize(NewSize);
            Capacity = NewSize;
        }
        Size = NewSize;
    }

    IndexType Add(const DataType& InElement)
        requires(std::is_copy_constructible_v<DataType>)
    {
        if (Size + 1 > Capacity) {
            EnsureCapacity(Capacity == 0 ? 1 : Capacity * 2);
        }
        new (Allocator.Data() + Size) DataType(InElement);
        ++Size;
        return Size - 1;
    }

    IndexType Add(DataType&& InElement)
        requires(std::is_move_constructible_v<DataType>)
    {
        if (Size + 1 > Capacity) {
            EnsureCapacity(Capacity == 0 ? 1 : Capacity * 2);
        }
        new (Allocator.Data() + Size) DataType(std::move(InElement));
        ++Size;
        return Size - 1;
    }

    IndexType AddUnique(const DataType& InElement)
        requires(std::is_copy_constructible_v<DataType>)
    {
        if (const IndexType Index = IndexOf(InElement); Index != -1) {
            return Index;
        }
        return Add(InElement);
    }

    IndexType AddUnique(DataType&& InElement)
        requires(std::is_move_constructible_v<DataType>)
    {
        if (const IndexType Index = IndexOf(InElement); Index != -1) {
            return Index;
        }
        return Add(std::move(InElement));
    }

    IndexType AddZeroed(IndexType Count) {
        if (Size + Count > Capacity) {
            EnsureCapacity(Capacity == 0 ? 1 : Capacity * 2);
        }
        Memory::ZeroMemory(Allocator.Data() + Size, Count);
        Size += Count;
        return Size - Count;
    }

    IndexType AddDefaulted(const IndexType Count)
        requires(std::is_default_constructible_v<DataType>)
    {
        if (Size + Count > Capacity) {
            EnsureCapacity(Capacity == 0 ? 1 : Capacity * 2);
        }
        for (IndexType i = 0; i < Count; ++i) {
            new (Allocator.Data() + Size + i) DataType();
        }
        Size += Count;
        return Size - Count;
    }

    void Append(const DataType* InElements, const IndexType Count)
        requires(std::is_copy_constructible_v<DataType>)
    {
        if (Size + Count > Capacity) {
            EnsureCapacity(Size + Count);
        }
        for (IndexType i = 0; i < Count; ++i) {
            new (Allocator.Data() + Size + i) DataType(InElements[i]);
        }
        Size += Count;
    }

    template <bool ShouldEnsureCapacity = true> void ShiftElements(const IndexType StartIndex, const int64_t ShiftBy) {
        if constexpr (ShouldEnsureCapacity) {
            if (ShiftBy > 0 && Size + ShiftBy > Capacity) {
                EnsureCapacity(Capacity == 0 ? 1 : Capacity * 2);
            }
        }
        if (ShiftBy > 0) {
            for (int64_t i = Size - 1; i >= static_cast<int64_t>(StartIndex); --i) {
                Allocator[i + ShiftBy] = std::move(Allocator[i]);
            }
        } else if (ShiftBy < 0) {
            for (IndexType i = StartIndex; i < Size; ++i) {
                Allocator[i + ShiftBy] = std::move(Allocator[i]);
            }
        }
        Size += ShiftBy;
    }

    void Insert(IndexType Index, const DataType& InElement)
        requires(std::is_copy_constructible_v<DataType>)
    {
        ShiftElements<true>(Index, 1);
        new (Allocator.Data() + Index) DataType(InElement);
    }

    void Insert(IndexType Index, DataType&& InElement)
        requires(std::is_move_constructible_v<DataType>)
    {
        ShiftElements<true>(Index, 1);
        new (Allocator.Data() + Index) DataType(std::move(InElement));
    }

    template <typename... ArgsT>
    void Emplace(IndexType Index, ArgsT&&... Args)
        requires(std::is_constructible_v<DataType, ArgsT...>)
    {
        ShiftElements<true>(Index, 1);
        new (Allocator.Data() + Index) DataType(std::forward<ArgsT>(Args)...);
    }

    DataType RemoveAt(IndexType Index)
        requires(std::is_move_constructible_v<DataType> || std::is_copy_constructible_v<DataType>)
    {
        if (Index >= Size) {
            char16_t* errorMessage =
                Utils::CStrings::CreatePrintFString(u"Array: Index %d out of bounds [0, %s].", Index, Size - 1);
            Platform::Get().OnFatalError(errorMessage);
            delete[] errorMessage;
            return;
        }
        if constexpr (std::is_move_constructible_v<DataType>) {
            DataType RemovedElement(std::move(Allocator[Index]));
            Allocator[Index].~DataType();
            ShiftElements<false>(Index + 1, -1);
            return RemovedElement;
        } else {
            if (Index >= Size) {
                return;
            }
            DataType RemovedElement(Allocator[Index]);
            Allocator[Index].~DataType();
            ShiftElements<false>(Index + 1, -1);
            return RemovedElement;
        }
    }

    int32_t IndexOf(const T& Value) const {
        for (IndexType i = 0; i < Size; ++i) {
            if (Allocator[i] == Value) {
                return i;
            }
        }
        return -1;
    }

    int32_t IndexOf(bool (*Predicate)(const T&)) const {
        for (IndexType i = 0; i < Size; ++i) {
            if (Predicate(Allocator[i])) {
                return i;
            }
        }
        return -1;
    }

    DataType Remove(const T& Value)
        requires(std::is_move_constructible_v<DataType> || std::is_copy_constructible_v<DataType>)
    {
        if constexpr (std::is_move_constructible_v<DataType>) {
            int32_t Index = IndexOf(Value);
            if (Index != -1) {
                DataType RemovedElement(std::move(Allocator[Index]));
                Allocator[Index].~DataType();
                ShiftElements<false>(Index + 1, -1);
                return RemovedElement;
            }
        } else {
            int32_t Index = IndexOf(Value);
            if (Index != -1) {
                DataType RemovedElement(Allocator[Index]);
                Allocator[Index].~DataType();
                ShiftElements<false>(Index + 1, -1);
                return RemovedElement;
            }
        }
        return DataType();
    }

    void Push(const DataType& InElement)
        requires(std::is_copy_constructible_v<DataType>)
    {
        Add(InElement);
    }

    void Push(DataType&& InElement)
        requires(std::is_move_constructible_v<DataType>)
    {
        Add(std::move(InElement));
    }

    DataType Pop()
        requires(std::is_move_constructible_v<DataType> || std::is_copy_constructible_v<DataType>)
    {
        if (Size == 0) {
            char16_t* errorMessage = Utils::CStrings::CreatePrintFString(u"Array: Pop from empty array.");
            Platform::Get().OnFatalError(errorMessage);
            delete[] errorMessage;
            return DataType();
        }
        return RemoveAt(Size - 1);
    }

    [[nodiscard]] DataType& Get(IndexType Index) { return Allocator[Index]; }
    [[nodiscard]] const DataType& Get(IndexType Index) const { return Allocator[Index]; }

    [[nodiscard]] IndexType Length() const { return Size; }

    DataType* Data() { return Allocator.Data(); }
    const DataType* Data() const { return Allocator.Data(); }

    DataType& operator[](IndexType Index) { return Allocator[Index]; }
    const DataType& operator[](IndexType Index) const { return Allocator[Index]; }

    using IteratorType = ListIterator<List<T, AllocatorT>, false>;
    using ConstIteratorType = ListIterator<List<T, AllocatorT>, true>;

    IteratorType GetIterator() { return IteratorType(*this); }
    ConstIteratorType GetIterator() const { return ConstIteratorType(*this); }

    IteratorType begin() { return GetIterator(); }
    IteratorType end() {
        IteratorType it = GetIterator();
        it.Seek(Size - 1);
        return it;
    }

    ConstIteratorType begin() const { return GetIterator(); }
    ConstIteratorType end() const {
        ConstIteratorType it = GetIterator();
        it.Seek(Size - 1);
        return it;
    }
    ConstIteratorType cbegin() const { return GetIterator(); }
    ConstIteratorType cend() const {
        ConstIteratorType it = GetIterator();
        it.Seek(Size - 1);
        return it;
    }

private:
    IndexType Size;
    IndexType Capacity;
    AllocatorType Allocator;
};

template <typename ListT, bool IsConst> class ListIterator {
public:
    using DataType = typename ListT::DataType;
    using IndexType = typename ListT::IndexType;
    using AllocatorType = typename ListT::AllocatorType;

    ListIterator(ListT& InList) : listRef(InList), currentIndex(0) {}
    bool HasNext() const { return currentIndex < listRef.Length(); }
    DataType& Next() { return listRef[currentIndex++]; }

    bool HasPrevious() const { return currentIndex > 0; }
    DataType& Previous() { return listRef[--currentIndex]; }

    void Seek(IndexType Index) {
        if (Index < 0 || Index >= listRef.Length()) [[unlikely]] {
            char16_t* errorMessage = Utils::CStrings::CreatePrintFString(
                u"ListIterator: Index %d out of bounds [0, %s].", Index, listRef.Length() - 1);
            Platform::Get().OnFatalError(errorMessage);
            delete[] errorMessage;
            return;
        }
        currentIndex = Index;
    }

    void Insert(const DataType& InElement)
        requires(std::is_copy_constructible_v<DataType>)
    {
        listRef.Insert(currentIndex, InElement);
    }
    void Insert(DataType&& InElement)
        requires(std::is_move_constructible_v<DataType>)
    {
        listRef.Insert(currentIndex, std::move(InElement));
    }
    template <typename... ArgsT>
    void Emplace(ArgsT&&... Args)
        requires(std::is_constructible_v<DataType, ArgsT...>)
    {
        listRef.Emplace(currentIndex, std::forward<ArgsT>(Args)...);
    }

    void Remove() {
        listRef.RemoveAt(currentIndex);
        --currentIndex;
    }

    void Reset() { currentIndex = 0; }

    ListIterator& operator++() {
        Next();
        return *this;
    }
    ListIterator& operator--() {
        Previous();
        return *this;
    }

private:
    ListT& listRef;
    IndexType currentIndex;
};
} // namespace Edvar::Containers