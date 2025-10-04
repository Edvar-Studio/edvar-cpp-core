#pragma once
#include "containers/iterators/array_view_iterator.h"

namespace edvar::container {
template <typename storage_type> class array_view {
public:
    array_view() : _data(nullptr), _length(0) {}
    array_view(storage_type* data, uint32 length) : _data(data), _length(length) {}
    array_view(const array_view& other) : _data(other._data), _length(other._length) {}
    uint32 length() const { return _length; }
    const storage_type* data() const { return _data; }

    using iterator = edvar::container::iterator::array_view_iterator<storage_type, false>;
    using const_iterator = edvar::container::iterator::array_view_iterator<storage_type, true>;

    iterator begin() { return iterator(const_cast<storage_type*>(_data), _length); }
    iterator end() { return iterator(const_cast<storage_type*>(_data), _length).seek(_length); }
    const_iterator begin() const { return const_iterator(_data, _length); }
    const_iterator end() const { return const_iterator(_data, _length).seek(_length); }
    const_iterator cbegin() const { return const_iterator(_data, _length); }
    const_iterator cend() const { return const_iterator(_data, _length).seek(_length); }

    const storage_type& operator[](uint32 index) const { return _data[index]; }
    storage_type& operator[](uint32 index) { return _data[index]; }

private:
    storage_type* _data;
    uint32 _length;
};
} // namespace edvar::container