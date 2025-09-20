#pragma once

namespace edvar::memory {
template <typename storage_type> class heap_allocator {
public:
    storage_type* allocate(size_t count) { return static_cast<storage_type*>(::operator new[](sizeof(storage_type) * count)); }
    void deallocate(storage_type* ptr) { ::operator delete[](ptr); }
};
} // namespace edvar::memory