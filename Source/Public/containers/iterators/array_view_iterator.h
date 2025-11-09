#pragma once

namespace edvar::container::iterator {
template <typename storage_type, bool is_const> class array_view_iterator {
public:
    using data_type = std::conditional_t<is_const, const storage_type*, storage_type*>;
    using data_ref_type = std::conditional_t<is_const, const storage_type&, storage_type&>;

    array_view_iterator(storage_type* start, uint32_t length);
    array_view_iterator(const array_view<storage_type>& view);
    array_view_iterator(const array_view_iterator& other);
    array_view_iterator(array_view_iterator&& other) noexcept;
    ~array_view_iterator() = default;

    bool has_next() const { return _current_index < _length; }
    bool has_previous() const { return _current_index > 0; }
    data_ref_type next() { return _data[_current_index++]; }
    data_ref_type previous() { return _data[--_current_index]; }
    array_view_iterator& seek(int32_t index);
    int32_t position() const { return static_cast<int32_t>(_current_index); }
    data_ref_type current() const { return _data[_current_index]; }
    array_view_iterator& operator++();
    array_view_iterator operator++(int);
    array_view_iterator& operator--();
    array_view_iterator operator--(int);
    data_ref_type operator*() { return current(); }
    data_type operator->() { return &current(); }
    bool operator==(const array_view_iterator& other) const;
    bool operator!=(const array_view_iterator& other) const { return !(*this == other); }

private:
    data_type _data;
    uint32_t _length;
    uint32_t _current_index;
};


template <typename storage_type, bool is_const>
inline array_view_iterator<storage_type, is_const>::array_view_iterator(const array_view<storage_type>& view)
    : _data(view.data()), _length(view.length()), _current_index(0) {}

template <typename storage_type, bool is_const>
inline bool array_view_iterator<storage_type, is_const>::operator==(const array_view_iterator& other) const {
    return _data == other._data && _length == other._length && _current_index == other._current_index;
}

template <typename storage_type, bool is_const>
inline array_view_iterator<storage_type, is_const> array_view_iterator<storage_type, is_const>::operator--(int) {
    array_view_iterator temp = *this;
    --(*this);
    return temp;
}

template <typename storage_type, bool is_const>
inline array_view_iterator<storage_type, is_const>& array_view_iterator<storage_type, is_const>::operator--() {
    if (has_previous())
        --_current_index;
    return *this;
}

template <typename storage_type, bool is_const>
inline array_view_iterator<storage_type, is_const> array_view_iterator<storage_type, is_const>::operator++(int) {
    array_view_iterator temp = *this;
    ++(*this);
    return temp;
}

template <typename storage_type, bool is_const>
inline array_view_iterator<storage_type, is_const>& array_view_iterator<storage_type, is_const>::operator++() {
    if (has_next())
        ++_current_index;
    return *this;
}

template <typename storage_type, bool is_const>
inline array_view_iterator<storage_type, is_const>& array_view_iterator<storage_type, is_const>::seek(int32_t index) {
    if (index < 0) {
        _current_index = 0;
    } else if (static_cast<uint32_t>(index) > _length) {
        _current_index = _length;
    } else {
        _current_index = static_cast<uint32_t>(index);
    }
    return *this;
}

template <typename storage_type, bool is_const>
inline array_view_iterator<storage_type, is_const>::array_view_iterator(array_view_iterator&& other) noexcept
    : _data(other._data), _length(other._length), _current_index(other._current_index) {
    other._data = nullptr;
    other._length = 0;
    other._current_index = 0;
}

template <typename storage_type, bool is_const>
inline array_view_iterator<storage_type, is_const>::array_view_iterator(const array_view_iterator& other)
    : _data(other._data), _length(other._length), _current_index(other._current_index) {}

template <typename storage_type, bool is_const>
inline array_view_iterator<storage_type, is_const>::array_view_iterator(storage_type* start, uint32_t length)
    : _data(start), _length(length), _current_index(0) {}

} // namespace edvar::container::iterator