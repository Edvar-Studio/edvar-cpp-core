#pragma once
#include "containers/container_utilities.h"
#include "memory/allocators.h"

namespace edvar {
template <typename storage_type, typename allocator_type = memory::heap_allocator<storage_type>> class array {
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
        _allocator.deallocate(_data);
        _size = 0;
        _capacity = 0;
    }

    inline int32 length() const { return _size; }
    inline int32 capacity() const { return _capacity; }
    inline indexed_result find(const storage_type& value) const {
        for (size_t i = 0; i < _size; ++i) {
            if (_data[i] == value) {
                return indexed_result::from_value(static_cast<int32>(i));
            }
        }
        return indexed_result::from_error(-1).attach_value_without_state_change(-1);
    }
    template <typename predicate_type> inline indexed_result find(predicate_type predicate) const {
        for (size_t i = 0; i < _size; ++i) {
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
            add(in_array[i]);
        }
    }

    inline const storage_type& operator[](int32 index) const { return _data[index]; }

    inline storage_type& operator[](int32 index) { return _data[index]; }
    int32 add(const storage_type& value) {
        ensure_capacity(_size + 1);
        _data[_size - 1] = value;
        return _size - 1;
    }
    void insert(int32 index, const storage_type& value) {
        edvar::container::utilities::check_bounds(0, _size, index);
        ensure_capacity(_size + 1);
        for (size_t i = _size - 1; i > static_cast<size_t>(index); --i) {
            _data[i] = _data[i - 1];
        }
        _data[index] = value;
    }
    void ensure_capacity(size_t new_size) {
        if (new_size > _capacity) {
            size_t new_capacity = _capacity == 0 ? 1 : _capacity;
            while (new_capacity < new_size) {
                new_capacity *= 2;
            }
            storage_type* new_data = _allocator.allocate(new_capacity);
            for (size_t i = 0; i < _size; ++i) {
                new_data[i] = _data[i];
            }
            _allocator.deallocate(_data);
            _data = new_data;
            _capacity = new_capacity;
            _size = new_size;
        }
    }
    void shrink() {
        if (_size < _capacity) {
            storage_type* new_data = _allocator.allocate(_size);
            for (size_t i = 0; i < _size; ++i) {
                new_data[i] = _data[i];
            }
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