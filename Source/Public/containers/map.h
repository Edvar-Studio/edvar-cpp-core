#pragma once

namespace edvar {

namespace container {
namespace iterator {
template <typename MapType, bool IsConst> class map_iterator;
}

// Configuration constants (avoid magic numbers)
static constexpr uint32_t DEFAULT_INITIAL_BUCKET_COUNT = 16u;
static constexpr uint32_t MIN_BUCKET_COUNT = 1u;
static constexpr uint32_t BUCKET_MAX_LOAD = 4u;      // max average entries per bucket before rehash
static constexpr int32_t BUCKET_GROW_MULTIPLIER = 2; // multiply bucket count on grow

template <typename Key, typename Value> class map {
public:
    using pair_type = edvar::pair<Key, Value>;

    // Internal entry stores pair + cached hash to avoid recomputing during lookups
    struct entry {
        pair_type pair;
        uint64_t hash;
    };

    map(uint32_t initial_bucket_count = DEFAULT_INITIAL_BUCKET_COUNT) { ensure_buckets(initial_bucket_count); }
    map(const map& other) : _buckets(other._buckets), _count(other._count) {}
    map(map&& other) noexcept : _buckets(std::move(other._buckets)), _count(other._count) { other._count = 0; }

    int32_t length() const { return _count; }

    // Insert or update. Returns true if inserted, false if updated existing.
    bool insert(const Key& key, const Value& value) {
        ensure_buckets(1);
        uint64_t h = get_hash_of_element<Key>(key);
        uint32_t b = bucket_index_for_hash(h);
        auto& bucket = _buckets[b];
        for (int32_t i = 0; i < bucket.length(); ++i) {
            if (bucket[i].hash == h && bucket[i].pair.key() == key) {
                bucket[i].pair.value() = value;
                return false;
            }
        }
        pair_type p;
        p.key() = key;
        p.value() = value;
        entry e;
        e.pair = std::move(p);
        e.hash = h;
        bucket.add(std::move(e));
        ++_count;
        // simple grow policy (based on average load)
        if (_count > _buckets.length() * static_cast<int32_t>(BUCKET_MAX_LOAD)) {
            rehash(_buckets.length() * BUCKET_GROW_MULTIPLIER);
        }
        return true;
    }

    // rvalue overload: move key and value into the map
    bool insert(Key&& key, Value&& value) {
        ensure_buckets(1);
        uint64_t h = get_hash_of_element<Key>(key);
        uint32_t b = bucket_index_for_hash(h);
        auto& bucket = _buckets[b];
        for (int32_t i = 0; i < bucket.length(); ++i) {
            if (bucket[i].hash == h && bucket[i].pair.key() == key) {
                bucket[i].pair.value() = std::move(value);
                return false;
            }
        }
        pair_type p;
        {
            p.key() = std::move(key);
            p.value() = std::move(value);
        }
        entry e{.pair = std::move(p), .hash = h};
        bucket.add(std::move(e));
        ++_count;
        if (_count > _buckets.length() * static_cast<int32_t>(BUCKET_MAX_LOAD)) {
            rehash(_buckets.length() * BUCKET_GROW_MULTIPLIER);
        }
        return true;
    }

    // Try get value; returns value_or_error (copy of value) if found
    edvar::value_or_error<Value, int32_t> try_get(const Key& key) const {
        if (_buckets.length() == 0)
            return edvar::value_or_error<Value, int32_t>::from_error(-1);
        uint64_t h = get_hash_of_element<Key>(key);
        uint32_t b = bucket_index_for_hash(h);
        const auto& bucket = _buckets[b];
        for (int32_t i = 0; i < bucket.length(); ++i) {
            if (bucket[i].hash == h && bucket[i].pair.key() == key) {
                return edvar::value_or_error<Value, int32_t>::from_value(bucket[i].pair.value());
            }
        }
        return edvar::value_or_error<Value, int32_t>::from_error(-1);
    }

    // Remove key; returns true if removed
    bool remove(const Key& key) {
        if (_buckets.length() == 0)
            return false;
        uint64_t h = get_hash_of_element<Key>(key);
        uint32_t b = bucket_index_for_hash(h);
        auto& bucket = _buckets[b];
        for (int32_t i = 0; i < bucket.length(); ++i) {
            if (bucket[i].hash == h && bucket[i].pair.key() == key) {
                // remove in-place shifting elements
                bucket.remove_at(i);
                --_count;
                return true;
            }
        }
        return false;
    }

    // Access or insert default-constructed value
    Value& operator[](const Key& key) {
        ensure_buckets(1);
        uint64_t h = get_hash_of_element<Key>(key);
        uint32_t b = bucket_index_for_hash(h);
        auto& bucket = _buckets[b];
        for (int32_t i = 0; i < bucket.length(); ++i) {
            if (bucket[i].hash == h && bucket[i].pair.key() == key)
                return bucket[i].pair.value();
        }
        pair_type p;
        p.key() = key;
        p.value() = Value();
        entry e;
        e.pair = p;
        e.hash = h;
        int32_t idx = bucket.add(e);
        ++_count;
        if (_count > _buckets.length() * static_cast<int32_t>(BUCKET_MAX_LOAD))
            rehash(_buckets.length() * BUCKET_GROW_MULTIPLIER);
        return bucket[idx].pair.value();
    }

    // rvalue overload for operator[]: move-key insertion
    Value& operator[](Key&& key) {
        ensure_buckets(1);
        uint64_t h = get_hash_of_element<Key>(key);
        uint32_t b = bucket_index_for_hash(h);
        auto& bucket = _buckets[b];
        for (int32_t i = 0; i < bucket.length(); ++i) {
            if (bucket[i].hash == h && bucket[i].pair.key() == key)
                return bucket[i].pair.value();
        }
        pair_type p;
        p.key() = std::move(key);
        p.value() = Value();
        entry e;
        e.pair = std::move(p);
        e.hash = h;
        int32_t idx = bucket.add(std::move(e));
        ++_count;
        if (_count > _buckets.length() * static_cast<int32_t>(BUCKET_MAX_LOAD))
            rehash(_buckets.length() * BUCKET_GROW_MULTIPLIER);
        return bucket[idx].pair.value();
    }

    bool contains(const Key& key) const { return try_get(key).is_value(); }

    void clear() {
        _buckets = array<array<entry>>();
        _count = 0;
    }

    // Expose keys and values as arrays (copies)
    array<Key> keys() const {
        array<Key> k;
        for (int32_t bi = 0; bi < _buckets.length(); ++bi) {
            const auto& bucket = _buckets[bi];
            for (int32_t i = 0; i < bucket.length(); ++i)
                k.add(bucket[i].pair.key());
        }
        return k;
    }
    array<Value> values() const {
        array<Value> v;
        for (int32_t bi = 0; bi < _buckets.length(); ++bi) {
            const auto& bucket = _buckets[bi];
            for (int32_t i = 0; i < bucket.length(); ++i)
                v.add(bucket[i].pair.value());
        }
        return v;
    }

    // iterator support (forwarded to external iterator implementation)
    using iterator = edvar::container::iterator::map_iterator<map, false>;
    using const_iterator = edvar::container::iterator::map_iterator<map, true>;

    iterator begin() { return iterator(*this); }
    iterator end() { return iterator(*this).seek(_count); }
    const_iterator begin() const { return const_iterator(*this); }
    const_iterator end() const { return const_iterator(*this).seek(_count); }
    const_iterator cbegin() const { return const_iterator(*this); }
    const_iterator cend() const { return const_iterator(*this).seek(_count); }

    bool operator==(const map& other) const {
        if (length() != other.length())
            return false;
        for (int32_t bi = 0; bi < _buckets.length(); ++bi) {
            const auto& bucket = _buckets[bi];
            for (int32_t i = 0; i < bucket.length(); ++i) {
                const auto& ent = bucket[i];
                auto ov = other.try_get(ent.pair.key());
                if (ov.is_error())
                    return false;
                if (!(ent.pair.value() == ov.get_value()))
                    return false;
            }
        }
        return true;
    }
    bool operator!=(const map& other) const { return !(*this == other); }
    map& operator=(const map& other) noexcept {
        if (this != &other) {
            _buckets = other._buckets;
            _count = other._count;
        }
        return *this;
    }
    map& operator=(map&& other) noexcept {
        if (this != &other) {
            _buckets = std::move(other._buckets);
            _count = other._count;
            other._count = 0;
            other._buckets = array<array<entry>>();
        }
        return *this;
    }

private:
    array<array<entry>> _buckets;
    int32_t _count = 0;

    // allow the external iterator to access internal buckets for efficient traversal
    template <typename MapType, bool IsConst> friend class container::iterator::map_iterator;

    // helper: round up to next power of two (>=1)
    static uint32_t next_power_of_two(uint32_t v) {
        if (v == 0)
            return 1u;
        --v;
        v |= v >> 1;
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v |= v >> 16;
        ++v;
        return v;
    }

    // Use bitmask if bucket count is power-of-two

    uint32_t bucket_index_for_hash(uint64_t h) const {
        int32_t buckets = _buckets.length();
        if (buckets == 0)
            return 0;
        // bucket count is always power-of-two; use bitmask instead of modulo
        uint32_t mask = static_cast<uint32_t>(buckets - 1);
        return static_cast<uint32_t>(h & mask);
    }

    void ensure_buckets(uint32_t count) {
        if (_buckets.length() >= static_cast<int32_t>(count))
            return;
        uint32_t desired = count < MIN_BUCKET_COUNT ? MIN_BUCKET_COUNT : count;
        uint32_t pow2 = next_power_of_two(desired);
        if (_buckets.length() == 0) {
            _buckets = array<array<entry>>();
            for (uint32_t i = 0; i < pow2; ++i)
                _buckets.add(array<entry>());
        } else {
            rehash(static_cast<int32_t>(pow2));
        }
    }

    void rehash(int32_t new_bucket_count) {
        // ensure new_bucket_count is power-of-two
        uint32_t pow2 = next_power_of_two(static_cast<uint32_t>(new_bucket_count));
        array<array<entry>> new_buckets;
        for (uint32_t i = 0; i < pow2; ++i)
            new_buckets.add(array<entry>());
        // move existing elements by removing from old buckets (move-out) to avoid double moves
        for (int32_t bi = 0; bi < _buckets.length(); ++bi) {
            auto& bucket = _buckets[bi];
            while (bucket.length() > 0) {
                // remove last element and move it
                entry ent = bucket.remove_at_value(bucket.length() - 1);
                uint32_t nb = static_cast<uint32_t>(ent.hash & (pow2 - 1));
                new_buckets[nb].add(std::move(ent));
            }
        }
        _buckets = std::move(new_buckets);
    }

    template <typename in_type> static inline uint64_t get_hash_of_element(const in_type& element) {

        return edvar::hash(element);
    }
};
} // namespace container
} // namespace edvar
