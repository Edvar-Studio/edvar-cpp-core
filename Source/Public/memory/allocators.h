#pragma once

namespace edvar::memory {
template <typename storage_type> class heap_allocator {
public:
    std::remove_cvref_t<storage_type>* allocate(uint32_t count) { return static_cast<std::remove_cvref_t<storage_type>*>(::operator new[](sizeof(storage_type) * count)); }
    void deallocate(std::remove_cvref_t<storage_type>* ptr) { ::operator delete[](ptr); }
};
} // namespace edvar::memory