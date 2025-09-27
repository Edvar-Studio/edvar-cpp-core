#pragma once

namespace edvar {

// Configuration constants (avoid magic numbers)
static constexpr uint32 DEFAULT_INITIAL_BUCKET_COUNT = 16u;
static constexpr uint32 MIN_BUCKET_COUNT = 1u;
static constexpr uint32 BUCKET_MAX_LOAD = 4u;      // max average entries per bucket before rehash
static constexpr int32 BUCKET_GROW_MULTIPLIER = 2; // multiply bucket count on grow

template <typename Key, typename Value> class map {
public:
    using pair_type = edvar::pair<Key, Value>;

    // Internal entry stores pair + cached hash to avoid recomputing during lookups
    struct entry {
        pair_type pair;
        uint64 hash;
    };

    map(uint32 initial_bucket_count = DEFAULT_INITIAL_BUCKET_COUNT) { ensure_buckets(initial_bucket_count); }
    map(const map& other) : _buckets(other._buckets), _count(other._count) {}
    map(map&& other) noexcept : _buckets(edvar::move(other._buckets)), _count(other._count) { other._count = 0; }

    int32 length() const { return _count; }

    // Insert or update. Returns true if inserted, false if updated existing.
    bool insert(const Key& key, const Value& value) {
        ensure_buckets(1);
        uint64 h = hash(key);
        uint32 b = bucket_index_for_hash(h);
        auto& bucket = _buckets[b];
        for (int32 i = 0; i < bucket.length(); ++i) {
            if (bucket[i].hash == h && bucket[i].pair.key() == key) {
                bucket[i].pair.value() = value;
                return false;
            }
        }
        pair_type p;
        p.key() = key;
        p.value() = value;
        entry e;
        e.pair = edvar::move(p);
        e.hash = h;
        bucket.add(edvar::move(e));
        ++_count;
        // simple grow policy (based on average load)
        if (_count > _buckets.length() * static_cast<int32>(BUCKET_MAX_LOAD)) {
            rehash(_buckets.length() * BUCKET_GROW_MULTIPLIER);
        }
        return true;
    }

    // rvalue overload: move key and value into the map
    bool insert(Key&& key, Value&& value) {
        ensure_buckets(1);
        uint64 h = get_hash(key);
        uint32 b = bucket_index_for_hash(h);
        auto& bucket = _buckets[b];
        for (int32 i = 0; i < bucket.length(); ++i) {
            if (bucket[i].hash == h && bucket[i].pair.key() == key) {
                bucket[i].pair.value() = edvar::move(value);
                return false;
            }
        }
        pair_type p;
        {
            p.key() = edvar::move(key);
            p.value() = edvar::move(value);
        }
        entry e{.pair = edvar::move(p), .hash = h};
        bucket.add(edvar::move(e));
        ++_count;
        if (_count > _buckets.length() * static_cast<int32>(BUCKET_MAX_LOAD)) {
            rehash(_buckets.length() * BUCKET_GROW_MULTIPLIER);
        }
        return true;
    }

    // Try get value; returns value_or_error (copy of value) if found
    edvar::value_or_error<Value, int32> try_get(const Key& key) const {
        if (_buckets.length() == 0)
            return edvar::value_or_error<Value, int32>::from_error(-1);
        uint64 h = get_hash(key);
        uint32 b = bucket_index_for_hash(h);
        const auto& bucket = _buckets[b];
        for (int32 i = 0; i < bucket.length(); ++i) {
            if (bucket[i].hash == h && bucket[i].pair.key() == key) {
                return edvar::value_or_error<Value, int32>::from_value(bucket[i].pair.value());
            }
        }
        return edvar::value_or_error<Value, int32>::from_error(-1);
    }

    // Remove key; returns true if removed
    bool remove(const Key& key) {
        if (_buckets.length() == 0)
            return false;
        uint64 h = get_hash(key);
        uint32 b = bucket_index_for_hash(h);
        auto& bucket = _buckets[b];
        for (int32 i = 0; i < bucket.length(); ++i) {
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
        uint64 h = get_hash(key);
        uint32 b = bucket_index_for_hash(h);
        auto& bucket = _buckets[b];
        for (int32 i = 0; i < bucket.length(); ++i) {
            if (bucket[i].hash == h && bucket[i].pair.key() == key)
                return bucket[i].pair.value();
        }
        pair_type p;
        p.key() = key;
        p.value() = Value();
        entry e;
        e.pair = p;
        e.hash = h;
        int32 idx = bucket.add(e);
        ++_count;
        if (_count > _buckets.length() * static_cast<int32>(BUCKET_MAX_LOAD))
            rehash(_buckets.length() * BUCKET_GROW_MULTIPLIER);
        return bucket[idx].pair.value();
    }

    // rvalue overload for operator[]: move-key insertion
    Value& operator[](Key&& key) {
        ensure_buckets(1);
        uint64 h = get_hash(key);
        uint32 b = bucket_index_for_hash(h);
        auto& bucket = _buckets[b];
        for (int32 i = 0; i < bucket.length(); ++i) {
            if (bucket[i].hash == h && bucket[i].pair.key() == key)
                return bucket[i].pair.value();
        }
        pair_type p;
        p.key() = edvar::move(key);
        p.value() = Value();
        entry e;
        e.pair = edvar::move(p);
        e.hash = h;
        int32 idx = bucket.add(edvar::move(e));
        ++_count;
        if (_count > _buckets.length() * static_cast<int32>(BUCKET_MAX_LOAD))
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
        for (int32 bi = 0; bi < _buckets.length(); ++bi) {
            const auto& bucket = _buckets[bi];
            for (int32 i = 0; i < bucket.length(); ++i)
                k.add(bucket[i].pair.key());
        }
        return k;
    }
    array<Value> values() const {
        array<Value> v;
        for (int32 bi = 0; bi < _buckets.length(); ++bi) {
            const auto& bucket = _buckets[bi];
            for (int32 i = 0; i < bucket.length(); ++i)
                v.add(bucket[i].pair.value());
        }
        return v;
    }

    bool operator==(const map& other) const {
        if (length() != other.length())
            return false;
        for (int32 bi = 0; bi < _buckets.length(); ++bi) {
            const auto& bucket = _buckets[bi];
            for (int32 i = 0; i < bucket.length(); ++i) {
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
            _buckets = edvar::move(other._buckets);
            _count = other._count;
            other._count = 0;
            other._buckets = array<array<entry>>();
        }
        return *this;
    }

private:
    array<array<entry>> _buckets;
    int32 _count = 0;

    // helper: round up to next power of two (>=1)
    static uint32 next_power_of_two(uint32 v) {
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

    uint32 bucket_index_for_hash(uint64 h) const {
        int32 buckets = _buckets.length();
        if (buckets == 0)
            return 0;
        // bucket count is always power-of-two; use bitmask instead of modulo
        uint32 mask = static_cast<uint32>(buckets - 1);
        return static_cast<uint32>(h & mask);
    }

    void ensure_buckets(uint32 count) {
        if (_buckets.length() >= static_cast<int32>(count))
            return;
        uint32 desired = count < MIN_BUCKET_COUNT ? MIN_BUCKET_COUNT : count;
        uint32 pow2 = next_power_of_two(desired);
        if (_buckets.length() == 0) {
            _buckets = array<array<entry>>();
            for (uint32 i = 0; i < pow2; ++i)
                _buckets.add(array<entry>());
        } else {
            rehash(static_cast<int32>(pow2));
        }
    }

    void rehash(int32 new_bucket_count) {
        // ensure new_bucket_count is power-of-two
        uint32 pow2 = next_power_of_two(static_cast<uint32>(new_bucket_count));
        array<array<entry>> new_buckets;
        for (uint32 i = 0; i < pow2; ++i)
            new_buckets.add(array<entry>());
        // move existing elements by removing from old buckets (move-out) to avoid double moves
        for (int32 bi = 0; bi < _buckets.length(); ++bi) {
            auto& bucket = _buckets[bi];
            while (bucket.length() > 0) {
                // remove last element and move it
                entry ent = bucket.remove_at_ref(bucket.length() - 1);
                uint32 nb = static_cast<uint32>(ent.hash & (pow2 - 1));
                new_buckets[nb].add(edvar::move(ent));
            }
        }
        _buckets = edvar::move(new_buckets);
    }
};

} // namespace edvar
