#pragma once

namespace edvar {
struct time {
    time() : _nanoseconds(0) {}
    explicit time(uint64 nanoseconds) : _nanoseconds(nanoseconds) {}

    uint64 to_nanoseconds() const {
        return _nanoseconds;
    }
private:
    uint64 _nanoseconds;
};
} // namespace edvar