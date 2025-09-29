#pragma once
#include "containers/container_utilities.h"
#include "memory/allocators.h"
#include "containers/array_view.h"
#include "containers/iterators/array_iterator.h"

namespace edvar::container {

// An array needs elements to be destructible and either move-constructible or copy-constructible
// Also they should preferably be move-assignable for best performance. Throw is already disabled for this library.
template <typename in_type>
inline constexpr bool can_store_in_array =
    edvar::meta::is_destructible<in_type> &&
    (edvar::meta::is_move_constructible<in_type> || edvar::meta::is_copy_constructible<in_type>);

template <typename storage_type, typename allocator_type> class array {
public:
    static_assert(edvar::meta::is_destructible<storage_type>,
                  "Type stored in edvar::container::array must be destructible");
    static_assert(edvar::meta::is_move_constructible<storage_type> || edvar::meta::is_copy_constructible<storage_type>,
                  "Type stored in edvar::container::array must be either move-constructible or copy-constructible");

    typedef value_or_error_code<int32> indexed_result;
    array();
    array(storage_type* elems, uint32 size);
    array(const array& other);
    array(const std::initializer_list<storage_type>& init_list);
    array(array&& other) noexcept;
    ~array() { empty(); }

    /**
     * Get the number of elements stored in this array.
     * @returns Number of elements in the array.
     */
    inline int32 length() const { return _size; }
    /**
     * Get the current capacity of this array. If the allocator allows, this can be
     * dynamically increased as needed when adding elements.
     * @returns Current capacity of the array.
     */
    inline int32 capacity() const { return _capacity; }
    /**
     * Get a pointer to the internal data array. This pointer may be invalidated
     * when adding or removing elements.
     * @returns Pointer to the internal data array.
     */
    inline storage_type* data() { return _data; }
    /**
     * Get a const pointer to the internal data array. This pointer may be invalidated
     * when adding or removing elements.
     * @returns Const pointer to the internal data array.
     */
    inline const storage_type* data() const { return _data; }
    /**
     * Check if the array is empty (length is zero).
     * @returns True if the array is empty, false otherwise.
     */
    inline bool is_empty() const { return _size == 0; }
    /**
     * Find the first occurrence of a value in the array and return its index.
     * If not found, returns an error code. The code doesn't have a specific meaning
     * just indicates failure.
     * @param value The value to search for.
     * @returns Indexed result containing the index if found, or an error code if not found.
     */
    indexed_result find(const storage_type& value) const;
    /**
     * Find the first occurrence of a value in the array that matches a given predicate
     * and return its index. If not found, returns an error code. The code doesn't have a specific meaning
     * just indicates failure.
     * @param predicate A callable that takes a const reference to storage_type and returns a bool.
     * @returns Indexed result containing the index if found, or an error code if not found.
     */
    template <typename predicate_type> indexed_result find(predicate_type predicate) const;
    /**
     * Find all occurrences of a value in the array and return their indices.
     * @param value The value to search for.
     * @returns An array of indices where the value was found. If not found, returns an empty array.
     */
    array<int> find_all(const storage_type& value) const;
    /**
     * Find all occurrences of a value in the array that matches a given predicate
     * and return their indices.
     * @param predicate A callable that takes a const reference to storage_type and returns a bool.
     * @returns An array of indices where the value was found. If not found, returns an empty array.
     */
    template <typename predicate_type> array<int> find_all(predicate_type predicate) const;
    /**
     * Create a new array containing only the elements that match a given predicate.
     * @param predicate A callable that takes a const reference to storage_type and returns a bool.
     * @returns A new array containing only the elements that match the predicate.
     */
    template <typename predicate_type, typename filtered_array_allocator_type = memory::heap_allocator<storage_type>>
    array<storage_type, filtered_array_allocator_type> filter(predicate_type predicate) const;
    /**
     * Create a new array by applying a transformation function to each element of the array.
     * @param predicate A callable that takes a const reference to storage_type and returns the new type.
     * @returns A new array containing the transformed elements.
     */
    template <typename predicate_type, typename map_array_allocator_type = memory::heap_allocator<storage_type>>
    array<decltype(predicate_type()), map_array_allocator_type> map(predicate_type predicate) const;
    /**
     * Execute a given function for each element in the array.
     * @param predicate A callable that takes a reference to storage_type.
     */
    template <typename predicate_type> void for_each(predicate_type predicate) const;
    /**
     * Check if any element in the array matches a given predicate.
     * @param predicate A callable that takes a const reference to storage_type and returns a bool.
     * @returns True if any element matches the predicate, false otherwise.
     */
    template <typename predicate_type> bool any(predicate_type predicate) const;
    /**
     * Check if all elements in the array match a given predicate.
     * @param predicate A callable that takes a const reference to storage_type and returns a bool.
     * @returns True if all elements match the predicate, false otherwise.
     */
    template <typename predicate_type> bool all(predicate_type predicate) const;
    /**
     * Remove all elements from the array, optionally setting a new capacity.
     * @param new_capacity The new capacity to set after emptying the array. Default is 0.
     */
    void empty(uint32 new_capacity = 0);
    /**
     * Append elements from an initializer list to the array.
     * @param init_list The initializer list containing elements to append.
     */
    void append(const std::initializer_list<storage_type>& init_list);
    /**
     * Append elements from another array to this array.
     * @param other The other array to append elements from.
     */
    template <typename in_array_allocator_type = memory::heap_allocator<storage_type>>
    void append(const array<storage_type, in_array_allocator_type>& other);
    /**
     * Append elements from a raw array to this array.
     * @param in_array Pointer to the raw array containing elements to append.
     * @param element_count Number of elements in the raw array.
     */
    void append(storage_type* in_array, uint32 element_count);
    /**
     * Append elements from an array view to this array.
     * @param view The array view containing elements to append.
     */
    void append(const array_view<storage_type>& view);
    /**
     * Access an element at a given index without checking bounds.
     * @param index The index of the element to access.
     * @returns A const reference to the element at the given index.
     */
    inline const storage_type& operator[](int32 index) const { return _data[index]; }
    /**
     * Access an element at a given index without checking bounds.
     * @param index The index of the element to access.
     * @returns A reference to the element at the given index.
     */
    inline storage_type& operator[](int32 index) { return _data[index]; }
    /**
     * Add an element to the end of the array by copy-constructing it.
     * Enabled only if storage_type is copy-constructible.
     * @param value The value to add.
     * @returns The index of the newly added element.
     */
    template <typename = edvar::meta::enable_if<edvar::meta::is_copy_constructible<storage_type>>>
    int32 add(const storage_type& value);
    /**
     * Add an element to the end of the array by move-constructing it.
     * Enabled only if storage_type is move-constructible.
     * @param value The value to add.
     * @returns The index of the newly added element.
     */
    template <typename = edvar::meta::enable_if<edvar::meta::is_move_constructible<storage_type>>>
    int32 add(storage_type&& value);

    /**
     * Construct and add an element to the end of the array in place.
     * @param args The arguments to forward to the constructor of storage_type.
     * @returns The index of the newly added element.
     */
    template <typename... Args> int32 emplace(Args&&... args);
    /**
     * Add multiple default-initialized elements to the end of the array.
     * @param count The number of elements to add.
     */
    void add_initialized(const int32 count);

    /**
     * Add multiple uninitialized elements to the end of the array.
     * @param count The number of elements to add.
     */
    void add_uninitialized(const int32 count);
    /**
     * Insert an element at a specific index, shifting subsequent elements.
     * @param index The index at which to insert the element.
     * @param value The value to insert.
     */
    void insert(int32 index, const storage_type& value);
    /**
     * Insert an element at a specific index, shifting subsequent elements.
     * @param index The index at which to insert the element.
     * @param value The value to insert.
     */
    void remove_at(int32 index);
    /**
     * Remove and return an element at a specific index, shifting subsequent elements.
     * @param index The index of the element to remove.
     * @returns The removed element.
     */
    storage_type remove_at_value(int32 index);
    /**
     * Ensure the array has at least the specified capacity, reallocating if necessary.
     * @param new_size The desired minimum capacity.
     */
    void ensure_capacity(int32 new_size);
    /**
     * Shrink the array's capacity to match its current size, freeing unused memory.
     */
    void shrink();
    /**
     * Create an array view from the array.
     * @returns An array view representing the current elements in the array.
     */
    container::array_view<storage_type> to_view() {
        return array_view<storage_type>(_data, static_cast<uint32>(_size));
    }
    /**
     * Create a const array view from the array.
     * @returns A const array view representing the current elements in the array.
     */
    const container::array_view<storage_type> to_view() const {
        return array_view<storage_type>(_data, static_cast<uint32>(_size));
    }
    /**
     * Copy assignment operator.
     * @param other The other array to copy from.
     * @returns A reference to this array.
     */
    array& operator=(const array& other);
    /**
     * Move assignment operator.
     * @param other The other array to move from.
     * @returns A reference to this array.
     */
    array& operator=(array&& other) noexcept;
    /**
     * Iterator types and functions to support range-based for loops.
     */
    using iterator = edvar::container::iterator::array_iterator<storage_type, allocator_type, false>;
    /**
     * Const iterator type for iterating over const arrays.
     */
    using const_iterator = edvar::container::iterator::array_iterator<storage_type, allocator_type, true>;
    /**
     * Get an iterator to the beginning of the array.
     * @returns An iterator to the first element.
     */
    iterator begin() { return iterator(*this); }
    /**
     * Get an iterator to the end of the array.
     * @returns An iterator to one past the last element.
     */
    iterator end() { return iterator(*this).seek(_size); }
    /**
     * Get a const iterator to the beginning of the array.
     * @returns A const iterator to the first element.
     */
    const_iterator begin() const { return const_iterator(*this); }
    /**
     * Get a const iterator to the end of the array.
     * @returns A const iterator to one past the last element.
     */
    const_iterator end() const { return const_iterator(*this).seek(_size); }
    /**
     * Get a const iterator to the beginning of the array.
     * @returns A const iterator to the first element.
     */
    const_iterator cbegin() const { return const_iterator(*this); }
    /**
     * Get a const iterator to the end of the array.
     * @returns A const iterator to one past the last element.
     */
    const_iterator cend() const { return const_iterator(*this).seek(_size); }

private:
    allocator_type _allocator;
    storage_type* _data;
    int32 _size;
    int32 _capacity;
};

template <typename storage_type, typename allocator_type>
inline void array<storage_type, allocator_type>::append(const array_view<storage_type>& view) {
    append(view.data(), view.length());
}

template <typename storage_type, typename allocator_type> inline void array<storage_type, allocator_type>::shrink() {
    if (_size < _capacity) {
        storage_type* new_data = _allocator.allocate(_size);
        for (int32 i = 0; i < _size; ++i) {
            new (&new_data[i]) storage_type(edvar::move(_data[i]));
        }
        for (int32 i = 0; i < _size; ++i)
            _data[i].~storage_type();
        _allocator.deallocate(_data);
        _data = new_data;
        _capacity = _size;
    }
}

template <typename storage_type, typename allocator_type>
inline array<storage_type, allocator_type>& array<storage_type, allocator_type>::operator=(const array& other) {
    if (this != &other) {
        empty(other._size);
        append(other);
    }
    return *this;
}

template <typename storage_type, typename allocator_type>
inline array<storage_type, allocator_type>& array<storage_type, allocator_type>::operator=(array&& other) noexcept {
    if (this != &other) {
        empty(other._capacity);
        _data = edvar::move(other._data);
        _size = edvar::move(other._size);
        _allocator = edvar::move(other._allocator);
        other._data = nullptr;
        other._size = 0;
        other._capacity = 0;
    }
    return *this;
}

template <typename storage_type, typename allocator_type>
inline void array<storage_type, allocator_type>::ensure_capacity(int32 new_size) {
    if (new_size > _capacity) {
        int32 new_capacity = _capacity == 0 ? 1 : _capacity;
        while (new_capacity < new_size) {
            new_capacity *= 2;
        }
        storage_type* new_data = _allocator.allocate(new_capacity);
        // Move-construct into new storage using placement-new with exception safety
        // Move-construct into new storage using placement-new
        for (int32 i = 0; i < _size; ++i) {
            new (&new_data[i]) storage_type(edvar::move(_data[i]));
        }
        // destroy old elements
        for (int32 i = 0; i < _size; ++i)
            _data[i].~storage_type();
        _allocator.deallocate(_data);
        _data = new_data;
        _capacity = new_capacity;
    }
}

template <typename storage_type, typename allocator_type>
inline storage_type array<storage_type, allocator_type>::remove_at_value(int32 index) {
    edvar::container::utilities::check_bounds(0, _size - 1, index);
    uint32 uindex = static_cast<uint32>(index);
    storage_type removed = edvar::move(_data[uindex]);
    // destroy source and shift remaining
    _data[uindex].~storage_type();
    for (uint32 i = uindex; i < static_cast<uint32>(_size) - 1; ++i) {
        if constexpr (edvar::meta::is_nothrow_move_assignable<storage_type>) {
            _data[i] = edvar::move(_data[i + 1]);
            _data[i + 1].~storage_type();
        } else {
            new (&_data[i]) storage_type(edvar::move(_data[i + 1]));
            _data[i + 1].~storage_type();
        }
    }
    --_size;
    return removed;
}

template <typename storage_type, typename allocator_type>
inline void array<storage_type, allocator_type>::remove_at(int32 index) {
    edvar::container::utilities::check_bounds(0, _size - 1, index);
    uint32 uindex = static_cast<uint32>(index);
    // destroy the element and shift subsequent elements down; prefer move-assignment
    _data[uindex].~storage_type();
    for (uint32 i = uindex; i < static_cast<uint32>(_size) - 1; ++i) {
        if constexpr (edvar::meta::is_nothrow_move_assignable<storage_type>) {
            _data[i] = edvar::move(_data[i + 1]);
            _data[i + 1].~storage_type();
        } else {
            new (&_data[i]) storage_type(edvar::move(_data[i + 1]));
            _data[i + 1].~storage_type();
        }
    }
    --_size;
}

template <typename storage_type, typename allocator_type>
inline void array<storage_type, allocator_type>::insert(int32 index, const storage_type& value) {
    edvar::container::utilities::check_bounds(0, _size, index);
    ensure_capacity(static_cast<uint32>(_size) + 1);
    // make room: construct tail slot
    new (&_data[_size]) storage_type(edvar::move(_data[_size - 1]));
    // shift elements up; prefer noexcept move-assignment when available
    for (uint32 i = static_cast<uint32>(_size) - 1; i > static_cast<uint32>(index); --i) {
        if constexpr (edvar::meta::is_nothrow_move_assignable<storage_type> ||
                      edvar::meta::is_move_assignable<storage_type>) {
            // if there is noexcept move-assignment, use it
            _data[i] = edvar::move(_data[i - 1]);
        } else {
            // if there is no noexcept move-assignment, use placement-new + move-construction + destroy old
            new (&_data[i]) storage_type(edvar::move(_data[i - 1]));
            _data[i - 1].~storage_type();
        }
    }
    if constexpr (edvar::meta::is_nothrow_move_assignable<storage_type>) {
        _data[index] = value;
    } else {
        new (&_data[index]) storage_type(value);
    }
    ++_size;
}

template <typename storage_type, typename allocator_type>
inline void array<storage_type, allocator_type>::add_uninitialized(const int32 count) {
    ensure_capacity(static_cast<uint32>(_size) + count);
    // leave memory uninitialized, but increase size value to reflect reserved slots
    _size += count;
}

template <typename storage_type, typename allocator_type>
inline void array<storage_type, allocator_type>::add_initialized(const int32 count) {
    ensure_capacity(static_cast<uint32>(_size) + count);
    for (int32 i = 0; i < count; ++i) {
        new (&_data[_size + i]) storage_type();
    }
    _size += count;
}

template <typename storage_type, typename allocator_type>
template <typename... Args>
inline int32 array<storage_type, allocator_type>::emplace(Args&&... args) {
    ensure_capacity(static_cast<uint32>(_size) + 1);
    new (&_data[_size]) storage_type(edvar::forward<Args>(args)...);
    ++_size;
    return _size - 1;
}

template <typename storage_type, typename allocator_type>
template <typename>
inline int32 array<storage_type, allocator_type>::add(storage_type&& value) {
    ensure_capacity(static_cast<uint32>(_size) + 1);
    // placement-new move-construct in place
    new (&_data[_size]) storage_type(edvar::move(value));
    ++_size;
    return _size - 1;
}

template <typename storage_type, typename allocator_type>
template <typename>
inline int32 array<storage_type, allocator_type>::add(const storage_type& value) {
    ensure_capacity(static_cast<uint32>(_size) + 1);
    // placement-new copy-construct in place
    new (&_data[_size]) storage_type(value);
    ++_size;
    return _size - 1;
}

template <typename storage_type, typename allocator_type>
inline void array<storage_type, allocator_type>::append(storage_type* in_array, uint32 element_count) {
    ensure_capacity(_size + element_count);
    for (uint32 i = 0; i < element_count; ++i) {
        new (&_data[_size]) storage_type(in_array[i]);
    }
    _size += element_count;
}

template <typename storage_type, typename allocator_type>
template <typename in_array_allocator_type>
inline void array<storage_type, allocator_type>::append(const array<storage_type, in_array_allocator_type>& other) {
    append(other._data, other._size);
}

template <typename storage_type, typename allocator_type>
inline void array<storage_type, allocator_type>::append(const std::initializer_list<storage_type>& init_list) {
    ensure_capacity(_size + init_list.size());
    for (const auto& element : init_list) {
        add(element);
    }
}

template <typename storage_type, typename allocator_type>
inline void array<storage_type, allocator_type>::empty(uint32 new_capacity) {
    // destroy existing elements
    for (uint32 i = 0; i < static_cast<uint32>(_size); ++i) {
        _data[i].~storage_type();
    }
    _allocator.deallocate(_data);
    _data = nullptr;
    _size = 0;
    _capacity = 0;
    if (new_capacity > 0) {
        ensure_capacity(new_capacity);
    }
}

template <typename storage_type, typename allocator_type>
template <typename predicate_type>
inline bool array<storage_type, allocator_type>::all(predicate_type predicate) const {
    for (uint32 i = 0; i < _size; ++i) {
        if (!predicate(_data[i])) {
            return false;
        }
    }
    return true;
}

template <typename storage_type, typename allocator_type>
template <typename predicate_type>
inline bool array<storage_type, allocator_type>::any(predicate_type predicate) const {
    for (uint32 i = 0; i < _size; ++i) {
        if (predicate(_data[i])) {
            return true;
        }
    }
    return false;
}

template <typename storage_type, typename allocator_type>
template <typename predicate_type>
inline void array<storage_type, allocator_type>::for_each(predicate_type predicate) const {
    for (uint32 i = 0; i < _size; ++i) {
        predicate(_data[i]);
    }
}

template <typename storage_type, typename allocator_type>
template <typename predicate_type, typename map_array_allocator_type>
inline array<decltype(predicate_type()), map_array_allocator_type>
array<storage_type, allocator_type>::map(predicate_type predicate) const {
    using map_return_type = decltype(predicate_type());
    array<map_return_type, map_array_allocator_type> mapped_array;
    mapped_array.ensure_capacity(_size);
    for (uint32 i = 0; i < _size; ++i) {
        mapped_array.insert(i, predicate(_data[i]));
    }
    return mapped_array;
}

template <typename storage_type, typename allocator_type>
template <typename predicate_type, typename filtered_array_allocator_type>
inline array<storage_type, filtered_array_allocator_type>
array<storage_type, allocator_type>::filter(predicate_type predicate) const {
    array<storage_type, filtered_array_allocator_type> return_array;
    for (uint32 i = 0; i < _size; ++i) {
        if (predicate(_data[i])) {
            return_array.add(_data[i]);
        }
    }
    return return_array;
}

template <typename storage_type, typename allocator_type>
template <typename predicate_type>
inline array<int> array<storage_type, allocator_type>::find_all(predicate_type predicate) const {
    array<int> return_arr;
    for (uint32 i = 0; i < _size; ++i) {
        if (predicate(_data[i])) {
            return_arr.add(i);
        }
    }
    return return_arr;
}

template <typename storage_type, typename allocator_type>
inline array<int> array<storage_type, allocator_type>::find_all(const storage_type& value) const {
    array<int> return_arr;
    for (uint32 i = 0; i < _size; ++i) {
        if (_data[i] == value) {
            return_arr.add(i);
        }
    }
    return return_arr;
}

template <typename storage_type, typename allocator_type>
template <typename predicate_type>
inline typename array<storage_type, allocator_type>::indexed_result
array<storage_type, allocator_type>::find(predicate_type predicate) const {
    for (uint32 i = 0; i < _size; ++i) {
        if (predicate(_data[i])) {
            return indexed_result::from_value(static_cast<int32>(i));
        }
    }
    return indexed_result::from_error(-1).attach_value_without_state_change(-1);
}

template <typename storage_type, typename allocator_type>
inline typename array<storage_type, allocator_type>::indexed_result
array<storage_type, allocator_type>::find(const storage_type& value) const {
    for (uint32 i = 0; i < _size; ++i) {
        if (_data[i] == value) {
            return indexed_result::from_value(static_cast<int32>(i));
        }
    }
    return indexed_result::from_error(-1).attach_value_without_state_change(-1);
}

template <typename storage_type, typename allocator_type>
inline array<storage_type, allocator_type>::array(array&& other) noexcept
    : _data(other._data), _size(other._size), _capacity(other._capacity) {
    other._data = nullptr;
    other._size = 0;
    other._capacity = 0;
}

template <typename storage_type, typename allocator_type>
inline array<storage_type, allocator_type>::array(const std::initializer_list<storage_type>& init_list) {
    append(init_list);
}

template <typename storage_type, typename allocator_type>
inline array<storage_type, allocator_type>::array(const array& other) : _data(nullptr), _size(0), _capacity(0) {
    append(other);
}

template <typename storage_type, typename allocator_type>
inline array<storage_type, allocator_type>::array(storage_type* elems, uint32 size) {
    append(elems, size);
}

template <typename storage_type, typename allocator_type>
inline array<storage_type, allocator_type>::array() : _data(nullptr), _size(0), _capacity(0) {}

} // namespace edvar::container