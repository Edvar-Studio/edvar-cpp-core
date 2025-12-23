// ReSharper disable CppRedundantTypenameKeyword
#pragma once
#include "Containers/List.hpp"
#include "Platform/IPlatform.hpp"

namespace Edvar::Containers {
template <typename T, typename AllocatorT> List<T, AllocatorT>::List() : Size(0), Capacity(0), Allocator() {}
template <typename T, typename AllocatorT>
List<T, AllocatorT>::List(const List& other)
    requires(std::is_copy_constructible_v<T>)
    : Size(other.Size), Capacity(other.Capacity), Allocator() {
    Allocator.Allocate(Capacity);
    for (int32_t i = 0; i < Size; ++i) {
        new (Allocator.Data() + i) T(other.Allocator[i]);
    }
}
template <typename T, typename AllocatorT>
List<T, AllocatorT>::List(List&& other) noexcept
    : Size(other.Size), Capacity(other.Capacity), Allocator(std::move(other.Allocator)) {
    other.Size = 0;
    other.Capacity = 0;
    other.Allocator = AllocatorT();
}
template <typename T, typename AllocatorT> List<T, AllocatorT>::~List() {
    for (int32_t i = 0; i < Size; ++i) {
        Allocator[i].~T();
    }
}
template <typename T, typename AllocatorT> List<T, AllocatorT>& List<T, AllocatorT>::operator=(const List& other) {
    if (this != &other) {
        for (int32_t i = 0; i < Size; ++i) {
            Allocator[i].~T();
        }
        Size = other.Size;
        Capacity = other.Capacity;
        Allocator.Allocate(Capacity);
        for (int32_t i = 0; i < Size; ++i) {
            new (Allocator.Data() + i) T(other.Allocator[i]);
        }
    }
    return *this;
}
template <typename T, typename AllocatorT> List<T, AllocatorT>& List<T, AllocatorT>::operator=(List&& other) noexcept {
    if (this != &other) {
        for (int32_t i = 0; i < Size; ++i) {
            Allocator[i].~T();
        }
        Size = other.Size;
        Capacity = other.Capacity;
        Allocator = std::move(other.Allocator);
        other.Size = 0;
        other.Capacity = 0;
        other.Allocator = AllocatorT();
    }
    return *this;
}
template <typename T, typename AllocatorT> void List<T, AllocatorT>::Clear() {
    if (Allocator.HasAllocated()) {
        for (int32_t i = 0; i < Size; ++i) {
            Allocator[i].~T();
        }
        Allocator.Resize(0);
        Size = 0;
    }
}
template <typename T, typename AllocatorT> void List<T, AllocatorT>::EnsureCapacity(int32_t NewCapacity) {
    if (!Allocator.HasAllocated()) {
        Allocator.Allocate(NewCapacity);
    } else if (NewCapacity > Capacity) {
        Allocator.Resize(NewCapacity);
    }
    Capacity = NewCapacity;
}
template <typename T, typename AllocatorT> void List<T, AllocatorT>::Resize(int32_t NewSize, const bool CanShrink) {
    if (NewSize > Capacity) {
        EnsureCapacity(NewSize);
    } else if (CanShrink && NewSize < Capacity) {
        Allocator.Resize(NewSize);
        Capacity = NewSize;
    }
    Size = NewSize;
}
template <typename T, typename AllocatorT>
int32_t List<T, AllocatorT>::Add(const DataType& InElement)
    requires(std::is_copy_constructible_v<DataType>)
{
    if (Size + 1 > Capacity) {
        EnsureCapacity(Capacity == 0 ? 1 : Capacity * 2);
    }
    new (Allocator.Data() + Size) DataType(InElement);
    ++Size;
    return Size - 1;
}
template <typename T, typename AllocatorT>
int32_t List<T, AllocatorT>::Add(DataType&& InElement)
    requires(std::is_move_constructible_v<DataType>)
{
    if (Size + 1 > Capacity) {
        EnsureCapacity(Capacity == 0 ? 1 : Capacity * 2);
    }
    new (Allocator.Data() + Size) DataType(std::move(InElement));
    ++Size;
    return Size - 1;
}
template <typename T, typename AllocatorT>
int32_t List<T, AllocatorT>::AddUnique(const DataType& InElement)
    requires(std::is_copy_constructible_v<DataType>)
{
    if (const int32_t Index = IndexOf(InElement); Index != -1) {
        return Index;
    }
    return Add(InElement);
}
template <typename T, typename AllocatorT>
int32_t List<T, AllocatorT>::AddUnique(DataType&& InElement)
    requires(std::is_move_constructible_v<DataType>)
{
    if (const int32_t Index = IndexOf(InElement); Index != -1) {
        return Index;
    }
    return Add(std::move(InElement));
}
template <typename T, typename AllocatorT> int32_t List<T, AllocatorT>::AddZeroed(int32_t Count) {
    if (Size + Count > Capacity) {
        EnsureCapacity(Capacity == 0 ? 1 : Capacity * 2);
    }
    Memory::ZeroMemory(Allocator.Data() + Size, Count);
    Size += Count;
    return Size - Count;
}
template <typename T, typename AllocatorT>
int32_t List<T, AllocatorT>::AddDefaulted(const int32_t Count)
    requires(std::is_default_constructible_v<DataType>)
{
    if (Size + Count > Capacity) {
        EnsureCapacity(Capacity == 0 ? 1 : Capacity * 2);
    }
    for (int32_t i = 0; i < Count; ++i) {
        new (Allocator.Data() + Size + i) DataType();
    }
    Size += Count;
    return Size - Count;
}
template <typename T, typename AllocatorT>
void List<T, AllocatorT>::Append(const DataType* InElements, const int32_t Count)
    requires(std::is_copy_constructible_v<DataType>)
{
    if (Size + Count > Capacity) {
        EnsureCapacity(Size + Count);
    }
    for (int32_t i = 0; i < Count; ++i) {
        new (Allocator.Data() + Size + i) DataType(InElements[i]);
    }
    Size += Count;
}
template <typename T, typename AllocatorT>
template <bool ShouldEnsureCapacity>
void List<T, AllocatorT>::ShiftElements(const int32_t StartIndex, const int64_t ShiftBy) {
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
        for (int32_t i = StartIndex; i < Size; ++i) {
            Allocator[i + ShiftBy] = std::move(Allocator[i]);
        }
    }
    Size += ShiftBy;
}
template <typename T, typename AllocatorT>
void List<T, AllocatorT>::Insert(int32_t Index, const DataType& InElement)
    requires(std::is_copy_constructible_v<DataType>)
{
    ShiftElements<true>(Index, 1);
    new (Allocator.Data() + Index) DataType(InElement);
}
template <typename T, typename AllocatorT>
void List<T, AllocatorT>::Insert(int32_t Index, DataType&& InElement)
    requires(std::is_move_constructible_v<DataType>)
{
    ShiftElements<true>(Index, 1);
    new (Allocator.Data() + Index) DataType(std::move(InElement));
}
template <typename T, typename AllocatorT>
template <typename... ArgsT>
void List<T, AllocatorT>::Emplace(int32_t Index, ArgsT&&... Args)
    requires(std::is_constructible_v<DataType, ArgsT...>)
{
    ShiftElements<true>(Index, 1);
    new (Allocator.Data() + Index) DataType(std::forward<ArgsT>(Args)...);
}
template <typename T, typename AllocatorT>
typename List<T, AllocatorT>::DataType List<T, AllocatorT>::RemoveAt(int32_t Index)
    requires(std::is_move_constructible_v<DataType> || std::is_copy_constructible_v<DataType>)
{
    if (Index >= Size) {
        Platform::GetPlatform().OnFatalError(
            *String::Format("Array: Index {} out of bounds [0, {}].", Index, Size - 1));
        // Make sure compiler is happy about this.
        return Allocator[0];
    }
    if constexpr (std::is_move_constructible_v<DataType>) {
        DataType RemovedElement(std::move(Allocator[Index]));
        Allocator[Index].~DataType();
        ShiftElements<false>(Index + 1, -1);
        return RemovedElement;
    } else {
        DataType RemovedElement(Allocator[Index]);
        Allocator[Index].~DataType();
        ShiftElements<false>(Index + 1, -1);
        return RemovedElement;
    }
}
template <typename T, typename AllocatorT> int32_t List<T, AllocatorT>::IndexOf(const T& Value) const {
    for (int32_t i = 0; i < Size; ++i) {
        if (Allocator[i] == Value) {
            return i;
        }
    }
    return -1;
}
template <typename T, typename AllocatorT> int32_t List<T, AllocatorT>::IndexOf(bool (*Predicate)(const T&)) const {
    for (int32_t i = 0; i < Size; ++i) {
        if (Predicate(Allocator[i])) {
            return i;
        }
    }
    return -1;
}
template <typename T, typename AllocatorT>
typename List<T, AllocatorT>::DataType List<T, AllocatorT>::Remove(const T& Value)
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
template <typename T, typename AllocatorT>
void List<T, AllocatorT>::Push(const DataType& InElement)
    requires(std::is_copy_constructible_v<DataType>)
{
    Add(InElement);
}
template <typename T, typename AllocatorT>
void List<T, AllocatorT>::Push(DataType&& InElement)
    requires(std::is_move_constructible_v<DataType>)
{
    Add(std::move(InElement));
}
template <typename T, typename AllocatorT>
typename List<T, AllocatorT>::DataType List<T, AllocatorT>::Pop()
    requires(std::is_move_constructible_v<DataType> || std::is_copy_constructible_v<DataType>)
{
    if (Size == 0) {
        Platform::GetPlatform().OnFatalError(*String::Format("Array: pop from empty array."));
        return DataType();
    }
    return RemoveAt(Size - 1);
}
template <typename T, typename AllocatorT>
typename List<T, AllocatorT>::DataType& List<T, AllocatorT>::Get(int32_t Index) {
    return Allocator[Index];
}
template <typename T, typename AllocatorT>
const typename List<T, AllocatorT>::DataType& List<T, AllocatorT>::Get(int32_t Index) const {
    return Allocator[Index];
}
template <typename T, typename AllocatorT> int32_t List<T, AllocatorT>::Length() const { return Size; }
template <typename T, typename AllocatorT> typename List<T, AllocatorT>::DataType* List<T, AllocatorT>::Data() {
    return Allocator.Data();
}
template <typename T, typename AllocatorT>
const typename List<T, AllocatorT>::DataType* List<T, AllocatorT>::Data() const {
    return Allocator.Data();
}
template <typename T, typename AllocatorT>
typename List<T, AllocatorT>::DataType& List<T, AllocatorT>::operator[](int32_t Index) {
    return Allocator[Index];
}
template <typename T, typename AllocatorT>
const typename List<T, AllocatorT>::DataType& List<T, AllocatorT>::operator[](int32_t Index) const {
    return Allocator[Index];
}
template <typename T, typename AllocatorT> List<T, AllocatorT>::IteratorType List<T, AllocatorT>::GetIterator() {
    return IteratorType(*this);
}
template <typename T, typename AllocatorT>
List<T, AllocatorT>::ConstIteratorType List<T, AllocatorT>::GetIterator() const {
    return ConstIteratorType(*this);
}
template <typename T, typename AllocatorT> List<T, AllocatorT>::IteratorType List<T, AllocatorT>::begin() {
    return GetIterator();
}
template <typename T, typename AllocatorT> List<T, AllocatorT>::IteratorType List<T, AllocatorT>::end() {
    IteratorType it = GetIterator();
    it.Seek(Size - 1);
    return it;
}
template <typename T, typename AllocatorT> List<T, AllocatorT>::ConstIteratorType List<T, AllocatorT>::begin() const {
    return GetIterator();
}
template <typename T, typename AllocatorT> List<T, AllocatorT>::ConstIteratorType List<T, AllocatorT>::end() const {
    ConstIteratorType it = GetIterator();
    it.Seek(Size - 1);
    return it;
}
template <typename T, typename AllocatorT> List<T, AllocatorT>::ConstIteratorType List<T, AllocatorT>::cbegin() const {
    return GetIterator();
}
template <typename T, typename AllocatorT> List<T, AllocatorT>::ConstIteratorType List<T, AllocatorT>::cend() const {
    ConstIteratorType it = GetIterator();
    it.Seek(Size - 1);
    return it;
}
template <typename ListT, bool IsConst>
ListIterator<ListT, IsConst>::ListIterator(ListT& InList) : listRef(InList), currentIndex(0) {}
template <typename ListT, bool IsConst> bool ListIterator<ListT, IsConst>::HasNext() const {
    return currentIndex < listRef.Length();
}
template <typename ListT, bool IsConst>
typename ListIterator<ListT, IsConst>::DataType& ListIterator<ListT, IsConst>::Next() {
    return listRef[currentIndex++];
}
template <typename ListT, bool IsConst> bool ListIterator<ListT, IsConst>::HasPrevious() const {
    return currentIndex > 0;
}
template <typename ListT, bool IsConst>
typename ListIterator<ListT, IsConst>::DataType& ListIterator<ListT, IsConst>::Previous() {
    return listRef[--currentIndex];
}
template <typename ListT, bool IsConst> void ListIterator<ListT, IsConst>::Seek(int32_t Index) {
    if (Index < 0 || Index >= listRef.Length()) [[unlikely]] {
        Platform::GetPlatform().OnFatalError(
            *String::Format(u"ListIterator: Index {} out of bounds [0, {}].", Index, listRef.Length() - 1));
        return;
    }
    currentIndex = Index;
}
template <typename ListT, bool IsConst>
void ListIterator<ListT, IsConst>::Insert(const DataType& InElement)
    requires(std::is_copy_constructible_v<DataType> && !IsConst)
{
    listRef.Insert(currentIndex, InElement);
}
template <typename ListT, bool IsConst>
void ListIterator<ListT, IsConst>::Insert(DataType&& InElement)
    requires(std::is_move_constructible_v<DataType> && !IsConst)
{
    listRef.Insert(currentIndex, std::move(InElement));
}
template <typename ListT, bool IsConst>
template <typename... ArgsT>
void ListIterator<ListT, IsConst>::Emplace(ArgsT&&... Args)
    requires(std::is_constructible_v<DataType, ArgsT...> && !IsConst)
{
    listRef.Emplace(currentIndex, std::forward<ArgsT>(Args)...);
}
template <typename ListT, bool IsConst> void ListIterator<ListT, IsConst>::Remove() {
    listRef.RemoveAt(currentIndex);
    --currentIndex;
}
template <typename ListT, bool IsConst> void ListIterator<ListT, IsConst>::Reset() { currentIndex = 0; }
template <typename ListT, bool IsConst> ListIterator<ListT, IsConst>& ListIterator<ListT, IsConst>::operator++() {
    Next();
    return *this;
}
template <typename ListT, bool IsConst> ListIterator<ListT, IsConst>& ListIterator<ListT, IsConst>::operator--() {
    Previous();
    return *this;
}
template <typename ListT, bool IsConst> bool ListIterator<ListT, IsConst>::operator==(const ListIterator& other) const {
    return &listRef == &other.listRef && currentIndex == other.currentIndex;
}
template <typename ListT, bool IsConst> bool ListIterator<ListT, IsConst>::operator!=(const ListIterator& other) const {
    return !((*this) == other);
}
template <typename ListT, bool IsConst>
typename ListIterator<ListT, IsConst>::DataType& ListIterator<ListT, IsConst>::operator*() {
    return listRef[currentIndex];
}
template <typename ListT, bool IsConst>
typename ListIterator<ListT, IsConst>::DataType* ListIterator<ListT, IsConst>::operator->() {
    return &listRef[currentIndex];
}

} // namespace Edvar::Containers