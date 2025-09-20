#pragma once

namespace edvar::memory {
namespace __private {
class weak_pointer_base;
}
template <typename T> class weak_pointer;

class reference_counter {
public:
    reference_counter() : _count(1) {}
    void add_ref() { ++_count; }
    void add_weak_ref(__private::weak_pointer_base* weak_ptr) { _weak_ptrs.add(weak_ptr); }
    EDVAR_CPP_CORE_API int32 release();
    int32 get_count() const { return _count; }

private:
    int32 _count;
    array<__private::weak_pointer_base*> _weak_ptrs;
};
} // namespace edvar::memory