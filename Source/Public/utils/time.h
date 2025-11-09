#pragma once

namespace edvar {
struct time {
    time() : _nanoseconds(0) {}
    explicit time(uint64_t nanoseconds) : _nanoseconds(nanoseconds) {}

    uint64_t to_nanoseconds() const {
        return _nanoseconds;
    }
private:
    uint64_t _nanoseconds;
};
} // namespace edvar