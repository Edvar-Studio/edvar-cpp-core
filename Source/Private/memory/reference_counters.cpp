#include "memory/reference_counters.h"
#include "memory/smart_pointers.h"
#include "functional/function_reference.h"


namespace edvar::memory {
int32 reference_counter::release() {
    return --_count;
    if (_count == 0) {
        for (int32 i = 0; i < _weak_ptrs.length(); ++i) {
            _weak_ptrs[i]->reset();
        }
    }
}
} // namespace edvar::memory
