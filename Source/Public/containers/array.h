#pragma once
#include "containers/container_utilities.h"
#include "memory/allocators.h"

namespace edvar {

template <typename T>
inline constexpr bool can_store_in_array =
    edvar::meta::is_move_assignable<T> || edvar::meta::is_nothrow_move_assignable<T>;

template <typename storage_type> class array_view {
public:
    array_view() : _data(nullptr), _length(0) {}
    array_view(storage_type* data, uint32 length) : _data(data), _length(length) {}
    array_view(const array_view& other) : _data(other._data), _length(other._length) {}
    uint32 length() const { return _length; }
    const storage_type* data() const { return _data; }

private:
    storage_type* _data;
    uint32 _length;
};

template <typename storage_type, typename allocator_type = memory::heap_allocator<storage_type>,
          typename = edvar::meta::enable_if<can_store_in_array<storage_type>>>
class array {
public:
    typedef value_or_error_code<int32> indexed_result;

    array() : _data(nullptr), _size(0), _capacity(0) {}
    array(storage_type* elems, uint32 size) { append(elems, size); }
    array(const array& other) : _data(nullptr), _size(0), _capacity(0) { append(other); }
    array(const std::initializer_list<storage_type>& init_list) { append(init_list); }
    array(array&& other) noexcept : _data(other._data), _size(other._size), _capacity(other._capacity) {
        other._data = nullptr;
        other._size = 0;
        other._capacity = 0;
    }
    ~array() {
        // destroy constructed elements
        for (uint32 i = 0; i < static_cast<uint32>(_size); ++i) {
            _data[i].~storage_type();
        }
        _allocator.deallocate(_data);
        _size = 0;
        _capacity = 0;
    }

    inline int32 length() const { return _size; }
    inline int32 capacity() const { return _capacity; }
    inline indexed_result find(const storage_type& value) const {
        for (uint32 i = 0; i < _size; ++i) {
            if (_data[i] == value) {
                return indexed_result::from_value(static_cast<int32>(i));
            }
        }
        return indexed_result::from_error(-1).attach_value_without_state_change(-1);
    }
    template <typename predicate_type> inline indexed_result find(predicate_type predicate) const {
        for (uint32 i = 0; i < _size; ++i) {
            if (predicate(_data[i])) {
                return indexed_result::from_value(static_cast<int32>(i));
            }
        }
        return indexed_result::from_error(-1).attach_value_without_state_change(-1);
    }
    inline array<int> find_all(const storage_type& value) const {
        array<int> return_arr;
        for (uint32 i = 0; i < _size; ++i) {
            if (_data[i] == value) {
                return_arr.add(i);
            }
        }
        return return_arr;
    }
    template <typename predicate_type> inline array<int> find_all(predicate_type predicate) const {
        array<int> return_arr;
        for (uint32 i = 0; i < _size; ++i) {
            if (predicate(_data[i])) {
                return_arr.add(i);
            }
        }
        return return_arr;
    }
    template <typename predicate_type, typename filtered_array_allocator_type = memory::heap_allocator<storage_type>>
    inline array<storage_type, filtered_array_allocator_type> filter(predicate_type predicate) const {
        array<storage_type, filtered_array_allocator_type> return_array;
        for (uint32 i = 0; i < _size; ++i) {
            if (predicate(_data[i])) {
                return_array.add(_data[i]);
            }
        }
        return return_array;
    }
    template <typename predicate_type, typename map_array_allocator_type = memory::heap_allocator<storage_type>>
    inline array<decltype(predicate_type()), map_array_allocator_type> map(predicate_type predicate) const {
        using map_return_type = decltype(predicate_type());
        array<map_return_type, map_array_allocator_type> mapped_array;
        mapped_array.ensure_capacity(_size);
        for (uint32 i = 0; i < _size; ++i) {
            mapped_array.insert(i, predicate(_data[i]));
        }
        return mapped_array;
    }
    template <typename predicate_type> inline void for_each(predicate_type predicate) const {
        for (uint32 i = 0; i < _size; ++i) {
            predicate(_data[i]);
        }
    }
    template <typename predicate_type> inline bool any(predicate_type predicate) const {
        for (uint32 i = 0; i < _size; ++i) {
            if (predicate(_data[i])) {
                return true;
            }
        }
        return false;
    }
    template <typename predicate_type> inline bool all(predicate_type predicate) const {
        for (uint32 i = 0; i < _size; ++i) {
            if (!predicate(_data[i])) {
                return false;
            }
        }
        return true;
    }
    inline void append(const std::initializer_list<storage_type>& init_list) {
        ensure_capacity(_size + init_list.size());
        for (const auto& element : init_list) {
            add(element);
        }
    }
    template <typename in_array_allocator_type = memory::heap_allocator<storage_type>>
    inline void append(const array<storage_type, in_array_allocator_type>& other) {
        append(other._data, other._size);
    }
    inline void append(storage_type* in_array, uint32 element_count) {
        ensure_capacity(_size + element_count);
        for (uint32 i = 0; i < element_count; ++i) {
            new (&_data[_size]) storage_type(in_array[i]);
        }
        _size += element_count;
    }

    inline const storage_type& operator[](int32 index) const { return _data[index]; }

    inline storage_type& operator[](int32 index) { return _data[index]; }
    int32 add(const storage_type& value) {
        ensure_capacity(static_cast<uint32>(_size) + 1);
        // placement-new copy-construct in place
        new (&_data[_size]) storage_type(value);
        ++_size;
        return _size - 1;
    }
    int32 add(storage_type&& value) {
        ensure_capacity(static_cast<uint32>(_size) + 1);
        // placement-new move-construct in place
        new (&_data[_size]) storage_type(edvar::move(value));
        ++_size;
        return _size - 1;
    }

    // Emplace: construct directly in place with forwarded args
    template <typename... Args> int32 emplace(Args&&... args) {
        ensure_capacity(static_cast<uint32>(_size) + 1);
        new (&_data[_size]) storage_type(edvar::forward<Args>(args)...);
        ++_size;
        return _size - 1;
    }
    void add_initialized(const int32 count) {
        ensure_capacity(static_cast<uint32>(_size) + count);
        for (int32 i = 0; i < count; ++i) {
            new (&_data[_size + i]) storage_type();
        }
        _size += count;
    }
    void add_uninitialized(const int32 count) {
        ensure_capacity(static_cast<uint32>(_size) + count);
        // leave memory uninitialized, but increase size value to reflect reserved slots
        _size += count;
    }
    void insert(int32 index, const storage_type& value) {
        edvar::container::utilities::check_bounds(0, _size, index);
        ensure_capacity(static_cast<uint32>(_size) + 1);
        // make room: construct tail slot
        new (&_data[_size]) storage_type(edvar::move(_data[_size - 1]));
        // shift elements up; prefer noexcept move-assignment when available
        for (uint32 i = static_cast<uint32>(_size) - 1; i > static_cast<uint32>(index); --i) {
            if constexpr (edvar::meta::is_nothrow_move_assignable<storage_type>) {
                _data[i] = edvar::move(_data[i - 1]);
            } else {
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
    void remove_at(int32 index) {
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
    // Remove element and return it by value (move out before shifting)
    storage_type remove_at_ref(int32 index) {
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
    void ensure_capacity(uint32 new_size) {
        if (new_size > static_cast<uint32>(_capacity)) {
            uint32 new_capacity = _capacity == 0 ? 1u : static_cast<uint32>(_capacity);
            while (new_capacity < new_size) {
                new_capacity *= 2u;
            }
            storage_type* new_data = _allocator.allocate(new_capacity);
            // Move-construct into new storage using placement-new with exception safety
            // Move-construct into new storage using placement-new
            for (uint32 i = 0; i < static_cast<uint32>(_size); ++i) {
                new (&new_data[i]) storage_type(edvar::move(_data[i]));
            }
            // destroy old elements
            for (uint32 i = 0; i < static_cast<uint32>(_size); ++i)
                _data[i].~storage_type();
            _allocator.deallocate(_data);
            _data = new_data;
            _capacity = static_cast<int32>(new_capacity);
        }
    }
    void shrink() {
        if (_size < _capacity) {
            storage_type* new_data = _allocator.allocate(static_cast<uint32>(_size));
            for (uint32 i = 0; i < static_cast<uint32>(_size); ++i) {
                new (&new_data[i]) storage_type(edvar::move(_data[i]));
            }
            for (uint32 i = 0; i < static_cast<uint32>(_size); ++i)
                _data[i].~storage_type();
            _allocator.deallocate(_data);
            _data = new_data;
            _capacity = _size;
        }
    }

private:
    allocator_type _allocator;
    storage_type* _data;
    int32 _size;
    int32 _capacity;
};
} // namespace edvar