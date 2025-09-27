#pragma once

#ifdef BUILDING_EDVAR_CPP_CORE
#ifdef _WIN32
#define EDVAR_CPP_CORE_API __declspec(dllexport)
#else
#define EDVAR_CPP_CORE_API __attribute__((visibility("default")))
#endif
#else
#ifdef _WIN32
#define EDVAR_CPP_CORE_API __declspec(dllimport)
#else
#define EDVAR_CPP_CORE_API
#endif
#endif

// forward declarations
namespace edvar {
namespace internationalization {
class locale;
}
} // namespace edvar
#include <new>                     // IWYU pragma: export
#include <initializer_list>        // IWYU pragma: export
#include "platform/basic_types.h"  // IWYU pragma: export
#include "utils/value_or_error.h"  // IWYU pragma: export
#include "utils/hash.h"            // IWYU pragma: export
#include "meta/meta.h"             // IWYU pragma: export
#include "utils/objects.h"         // IWYU pragma: export
#include "containers/tuple.h"      // IWYU pragma: export
#include "containers/array.h"      // IWYU pragma: export
#include "memory/smart_pointers.h" // IWYU pragma: export
#include "containers/string.h"     // IWYU pragma: export
#include "containers/map.h"        // IWYU pragma: export