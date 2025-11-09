#pragma once

namespace edvar::container::iterator {
template <class storage_type, class allocator_type, bool is_const> class array_iterator {
public:
    using element_pointer = storage_type*;
    using element_ref = storage_type&;
    using array_type = array<storage_type, allocator_type>;

    array_iterator(const array<storage_type, allocator_type>& arr) : _data(&arr), _current_index(0) {}
    array_iterator(const array_iterator& other) : _data(other._data), _current_index(other._current_index) {}
    array_iterator(array_iterator&& other) noexcept
        : _data(std::move(other._data)), _current_index(std::move(other._current_index)) {
        other._data = nullptr;
        other._current_index = 0;
    }
    ~array_iterator() = default;

    bool has_next() const { return _current_index < _data->length(); }
    bool has_previous() const { return _current_index > 0; }
    element_ref next() { return _data->data()[_current_index++]; }
    element_ref previous() { return _data->data()[--_current_index]; }
    array_iterator& seek(int32_t index) {
        if (index < 0) {
            _current_index = 0;
        } else if (static_cast<uint32_t>(index) > _data->length()) {
            _current_index = _data->length();
        } else {
            _current_index = static_cast<uint32_t>(index);
        }
        return *this;
    }
    array_iterator& reset() {
        _current_index = 0;
        return *this;
    }
   int32_t position() const { return static_cast<int32_t>(_current_index); }
    element_ref current() const { return _data[_current_index]; }
    array_iterator& operator++() {
        if (has_next())
            ++_current_index;
        return *this;
    }
    array_iterator operator++(int) {
        array_iterator temp = *this;
        ++(*this);
        return temp;
    }
    array_iterator& operator--() {
        if (has_previous())
            --_current_index;
        return *this;
    }
    array_iterator operator--(int) {
        array_iterator temp = *this;
        --(*this);
        return temp;
    }
    element_ref operator*() { return current(); }
    element_pointer operator->() { return &current(); }
    bool operator==(const array_iterator& other) const {
        return _data == other._data && _current_index == other._current_index;
    }
    bool operator!=(const array_iterator& other) const { return !(*this == other); }

    array_iterator& replace(const storage_type& value) requires (!is_const){
        (*_data)[_current_index] = value;
        return *this;
    }
    array_iterator& replace(storage_type&& value) requires (!is_const) {
        (*_data)[_current_index] = std::move(value);
        return *this;
    }

    array_iterator& remove() requires (!is_const) {
        if (_data && _current_index < static_cast<uint32_t>(_data->length())) {
            _data->remove_at(static_cast<int32_t>(_current_index));
        }
        if (_current_index >= static_cast<uint32_t>(_data->length())) {
            _current_index = _data->length();
        }
        _current_index--; // move back to previous valid element
        return *this;
    }

private:
    const array_type* _data;
    uint32_t _current_index;
};
} // namespace edvar::container::iterator