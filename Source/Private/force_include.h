#pragma once

#ifdef STATIC_EDVAR_CPP_CORE
#    define EDVAR_CPP_CORE_API
#else
// Non static library import/export handling
#    if defined(BUILDING_EDVAR_CPP_CORE)
#        if defined(_WIN32)
#            define EDVAR_CPP_CORE_API __declspec(dllexport)
#        elif defined(__GNUC__) && __GNUC__ >= 4
#            define EDVAR_CPP_CORE_API __attribute__((visibility("default")))
#        else
#            define EDVAR_CPP_CORE_API
#        endif
#    else
#        if defined(_WIN32)
#            define EDVAR_CPP_CORE_API __declspec(dllimport)
#        else
#            define EDVAR_CPP_CORE_API
#        endif
#    endif
#endif

#include <new>                    // IWYU pragma: export
#include <initializer_list>       // IWYU pragma: export
#include "platform/basic_types.h" // IWYU pragma: export
#include "meta/meta.h"            // IWYU pragma: export
#include "memory/operations.h"    // IWYU pragma: export
#include "memory/allocators.h"    // IWYU pragma: export

// forward declarations
namespace edvar {
namespace internationalization {
class locale;
}
namespace container {
template <typename storage_type, typename allocator_type = memory::heap_allocator<storage_type>> class array;
template <typename storage_type> class array_view;
} // namespace container
template <typename storage_type, typename allocator_type = edvar::memory::heap_allocator<storage_type>>
using array = container::array<storage_type, allocator_type>;
} // namespace edvar

#include "utils/objects.h"         // IWYU pragma: export
#include "utils/value_or_error.h"  // IWYU pragma: export
#include "utils/hash.h"            // IWYU pragma: export
#include "containers/tuple.h"      // IWYU pragma: export
#include "containers/array.h"      // IWYU pragma: export
#include "memory/smart_pointers.h" // IWYU pragma: export
#include "containers/string.h"     // IWYU pragma: export
#include "containers/map.h"        // IWYU pragma: export