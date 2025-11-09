#pragma once

namespace edvar::container::iterator {

template <typename map_type, bool is_const> class map_iterator {
public:
    using pair_type = typename map_type::pair_type;
    using data_type = std::conditional_t<is_const, const map_type*, map_type*>;
    using pair_ref = std::conditional_t<is_const, const pair_type&, pair_type&>;
    using pair_ptr = std::conditional_t<is_const, const pair_type*, pair_type*>;

    map_iterator();
    map_iterator(const map_type& m);
    map_iterator(const map_iterator& other);
    map_iterator(map_iterator&& other) noexcept;
    ~map_iterator() = default;

    bool has_next() const;
    bool has_previous() const;
    pair_ref next();
    pair_ref previous();
    map_iterator& seekint32_t index);
    map_iterator& reset();
   int32_t position() const;
    pair_ref current() const;

    map_iterator& operator++();
    map_iterator operator++(int);
    map_iterator& operator--();
    map_iterator operator--(int);
    pair_ref operator*() { return current(); }
    pair_ptr operator->() { return &current(); }
    bool operator==(const map_iterator& other) const;
    bool operator!=(const map_iterator& other) const { return !(*this == other); }

    // modification helpers available only for non-const iterators
    std::enable_if_t<!is_const, map_iterator&> replace(const pair_type& p);
    std::enable_if_t<!is_const, map_iterator&> replace(pair_type&& p);
    std::enable_if_t<!is_const, map_iterator&> remove();

private:
    data_type _map;
    uint32_t _bucket_index;
    uint32_t _index_in_bucket;
    // cached absolute position (0..map.length()) for seek/position
   int32_t _absolute_pos;

    // advance to next valid element starting from current indices
    void advance_to_next();
    // retreat to previous valid element starting from current indices
    void retreat_to_previous();
};

template <typename map_type, bool is_const>
inline map_iterator<map_type, is_const>::map_iterator()
    : _map(nullptr), _bucket_index(0), _index_in_bucket(0), _absolute_pos(0) {}

template <typename map_type, bool is_const>
inline map_iterator<map_type, is_const>::map_iterator(const map_type& m)
    : _map(&m), _bucket_index(0), _index_in_bucket(0), _absolute_pos(0) {
    advance_to_next();
}

template <typename map_type, bool is_const>
inline map_iterator<map_type, is_const>::map_iterator(const map_iterator& other)
    : _map(other._map), _bucket_index(other._bucket_index), _index_in_bucket(other._index_in_bucket),
      _absolute_pos(other._absolute_pos) {}

template <typename map_type, bool is_const>
inline map_iterator<map_type, is_const>::map_iterator(map_iterator&& other) noexcept
    : _map(other._map), _bucket_index(other._bucket_index), _index_in_bucket(other._index_in_bucket),
      _absolute_pos(other._absolute_pos) {
    other._map = nullptr;
    other._bucket_index = 0;
    other._index_in_bucket = 0;
    other._absolute_pos = 0;
}

template <typename map_type, bool is_const>
inline bool map_iterator<map_type, is_const>::has_next() const {
    if (!_map)
        return false;
    return _absolute_pos < _map->length();
}

template <typename map_type, bool is_const>
inline bool map_iterator<map_type, is_const>::has_previous() const {
    return _absolute_pos > 0 && _map != nullptr;
}

template <typename map_type, bool is_const>
inline void map_iterator<map_type, is_const>::advance_to_next() {
    if (!_map)
        return;
   int32_t total = _map->length();
    if (_absolute_pos >= total) {
        // set to end
        _bucket_index = static_cast<uint32_t>(_map->_buckets.length());
        _index_in_bucket = 0;
        _absolute_pos = total;
        return;
    }
    // start scanning from current bucket/index
    uint32_t buckets = static_cast<uint32_t>(_map->_buckets.length());
    uint32_t bi = _bucket_index;
    uint32_t ii = _index_in_bucket;
    for (; bi < buckets; ++bi) {
        const auto& bucket = _map->_buckets[bi];
        for (; ii < static_cast<uint32_t>(bucket.length()); ++ii) {
            // found a valid entry
            _bucket_index = bi;
            _index_in_bucket = ii;
            return;
        }
        ii = 0;
    }
    // no more found -> set to end
    _bucket_index = buckets;
    _index_in_bucket = 0;
    _absolute_pos = _map->length();
}

template <typename map_type, bool is_const>
inline void map_iterator<map_type, is_const>::retreat_to_previous() {
    if (!_map)
        return;
    if (_absolute_pos <= 0) {
        _bucket_index = 0;
        _index_in_bucket = 0;
        _absolute_pos = 0;
        return;
    }
    // start searching backwards from current bucket/index
   int32_t pos = _absolute_pos - 1;
    // naive approach: walk from beginning until pos to find bucket/index
    _bucket_index = 0;
    _index_in_bucket = 0;
   int32_t count = 0;
    uint32_t buckets = static_cast<uint32_t>(_map->_buckets.length());
    for (uint32_t bi = 0; bi < buckets; ++bi) {
        const auto& bucket = _map->_buckets[bi];
        for int32_t i = 0; i < bucket.length(); ++i) {
            if (count == pos) {
                _bucket_index = bi;
                _index_in_bucket = static_cast<uint32_t>(i);
                _absolute_pos = pos;
                return;
            }
            ++count;
        }
    }
    // fallback to start
    _bucket_index = 0;
    _index_in_bucket = 0;
    _absolute_pos = 0;
}

template <typename map_type, bool is_const>
inline typename map_iterator<map_type, is_const>::pair_ref map_iterator<map_type, is_const>::next() {
    // return current then advance
    pair_ref cur = current();
    ++(*this);
    return cur;
}

template <typename map_type, bool is_const>
inline typename map_iterator<map_type, is_const>::pair_ref map_iterator<map_type, is_const>::previous() {
    --(*this);
    return current();
}

template <typename map_type, bool is_const>
inline map_iterator<map_type, is_const>& map_iterator<map_type, is_const>::seekint32_t index) {
    if (!_map) {
        _absolute_pos = 0;
        return *this;
    }
    if (index < 0)
        index = 0;
    if (index > _map->length())
        index = _map->length();
    // walk to find the index'th element
   int32_t count = 0;
    uint32_t buckets = static_cast<uint32_t>(_map->_buckets.length());
    for (uint32_t bi = 0; bi < buckets; ++bi) {
        const auto& bucket = _map->_buckets[bi];
        for int32_t i = 0; i < bucket.length(); ++i) {
            if (count == index) {
                _bucket_index = bi;
                _index_in_bucket = static_cast<uint32_t>(i);
                _absolute_pos = index;
                return *this;
            }
            ++count;
        }
    }
    // set to end
    _bucket_index = buckets;
    _index_in_bucket = 0;
    _absolute_pos = _map->length();
    return *this;
}

template <typename map_type, bool is_const>
inline map_iterator<map_type, is_const>& map_iterator<map_type, is_const>::reset() {
    _bucket_index = 0;
    _index_in_bucket = 0;
    _absolute_pos = 0;
    advance_to_next();
    return *this;
}

template <typename map_type, bool is_const>
inlineint32_t map_iterator<map_type, is_const>::position() const { return _absolute_pos; }

template <typename map_type, bool is_const>
inline typename map_iterator<map_type, is_const>::pair_ref map_iterator<map_type, is_const>::current() const {
    const auto& bucket = _map->_buckets[_bucket_index];
    return bucket[static_castint32_t>(_index_in_bucket)].pair;
}

template <typename map_type, bool is_const>
inline map_iterator<map_type, is_const>& map_iterator<map_type, is_const>::operator++() {
    if (!_map)
        return *this;
    if (_absolute_pos >= _map->length())
        return *this;
    ++_absolute_pos;
    // move to next index in bucket
    ++_index_in_bucket;
    uint32_t buckets = static_cast<uint32_t>(_map->_buckets.length());
    for (uint32_t bi = _bucket_index; bi < buckets; ++bi) {
        const auto& bucket = _map->_buckets[bi];
        for (uint32_t ii = (bi == _bucket_index ? _index_in_bucket : 0); ii < static_cast<uint32_t>(bucket.length()); ++ii) {
            _bucket_index = bi;
            _index_in_bucket = ii;
            return *this;
        }
        _index_in_bucket = 0;
    }
    // reached end
    _bucket_index = buckets;
    _index_in_bucket = 0;
    return *this;
}

template <typename map_type, bool is_const>
inline map_iterator<map_type, is_const> map_iterator<map_type, is_const>::operator++(int) {
    map_iterator temp = *this;
    ++(*this);
    return temp;
}

template <typename map_type, bool is_const>
inline map_iterator<map_type, is_const>& map_iterator<map_type, is_const>::operator--() {
    if (!_map)
        return *this;
    if (_absolute_pos == 0)
        return *this;
    --_absolute_pos;
    // walk from beginning to find new indices for absolute_pos
    seek(_absolute_pos);
    return *this;
}

template <typename map_type, bool is_const>
inline map_iterator<map_type, is_const> map_iterator<map_type, is_const>::operator--(int) {
    map_iterator temp = *this;
    --(*this);
    return temp;
}

template <typename map_type, bool is_const>
inline bool map_iterator<map_type, is_const>::operator==(const map_iterator& other) const {
    return _map == other._map && _bucket_index == other._bucket_index && _index_in_bucket == other._index_in_bucket &&
           _absolute_pos == other._absolute_pos;
}

template <typename map_type, bool is_const>
inline std::enable_if_t<!is_const, map_iterator<map_type, is_const>&>
map_iterator<map_type, is_const>::replace(const pair_type& p) {
    if (_map && _bucket_index < static_cast<uint32_t>(_map->_buckets.length())) {
        auto& bucket = _map->_buckets[_bucket_index];
        bucket[static_castint32_t>(_index_in_bucket)].pair = p;
    }
    return *this;
}

template <typename map_type, bool is_const>
inline std::enable_if_t<!is_const, map_iterator<map_type, is_const>&>
map_iterator<map_type, is_const>::replace(pair_type&& p) {
    if (_map && _bucket_index < static_cast<uint32_t>(_map->_buckets.length())) {
        auto& bucket = _map->_buckets[_bucket_index];
        bucket[static_castint32_t>(_index_in_bucket)].pair = std::move(p);
    }
    return *this;
}

template <typename map_type, bool is_const>
inline std::enable_if_t<!is_const, map_iterator<map_type, is_const>&> map_iterator<map_type, is_const>::remove() {
    if (!_map)
        return *this;
    if (_bucket_index >= static_cast<uint32_t>(_map->_buckets.length()))
        return *this;
    auto& bucket = _map->_buckets[_bucket_index];
    bucket.remove_at(static_castint32_t>(_index_in_bucket));
    --_absolute_pos;
    // adjust indices: if there is still an element at this bucket/index, keep it; otherwise advance
    if (_index_in_bucket >= static_cast<uint32_t>(bucket.length())) {
        // move to next valid
        advance_to_next();
    }
    return *this;
}

} // namespace edvar::container::iterator
