#pragma once

#ifndef WITH_MIMALLOC
#    define WITH_MIMALLOC 1
#endif

#ifdef EDVAR_CPP_CORE_DYNAMIC
#    ifdef BUILDING_EDVAR_CPP_CORE
#        if defined(_WIN32) || defined(__CYGWIN__)
#            define EDVAR_CPP_CORE_API __declspec(dllexport)
#        else
#            define EDVAR_CPP_CORE_API __attribute__((visibility("default")))
#        endif
#    else
#        ifdef _WIN32
#            define EDVAR_CPP_CORE_API __declspec(dllimport)
#        else
#            define EDVAR_CPP_CORE_API
#        endif
#    endif
#else
#    define EDVAR_CPP_CORE_API
#endif

#if defined(_MSC_VER)
#    define EDVAR_CPP_CORE_FORCE_INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#    define EDVAR_CPP_CORE_FORCE_INLINE inline __attribute__((always_inline))
#else
#    define EDVAR_CPP_CORE_FORCE_INLINE inline
#endif

#include <cstdint>                                    // IWYU pragma: export
#include <type_traits>                                // IWYU pragma: export
#include <utility>                                    // IWYU pragma: export
#include "Memory/MemoryAllocator.hpp"                 // IWYU pragma: export
#include "Utils/Optional.hpp"                         // IWYU pragma: export
#include "Containers/Allocators/DefaultAllocator.hpp" // IWYU pragma: export
namespace Edvar::Containers {
template <typename CharT> struct StringBase;
template <typename DataT, typename AllocatorT = Allocators::DefaultAllocator<DataT>> struct List;
using String = Edvar::Containers::StringBase<char16_t>;
} // namespace Edvar::Containers
using String = Edvar::Containers::String;
#include "Utils/Meta.hpp"      // IWYU pragma: export
#include "Memory/Ops.hpp"      // IWYU pragma: export
#include "Threading/Mutex.hpp" // IWYU pragma: export

#include "Containers/List.hpp"   // IWYU pragma: export
#include "Containers/String.hpp" // IWYU pragma: export

#include "Math/All.hpp" // IWYU pragma: export

#include "Utils/Hash.hpp" // IWYU pragma: export

#include "Containers/Tuple.hpp"     // IWYU pragma: export
#include "Memory/SmartPointers.hpp" // IWYU pragma: export

#include "Utils/CString.hpp" // IWYU pragma: export

#include "I18N/Locale.hpp"      // IWYU pragma: export
#include "Utils/Functional.hpp" // IWYU pragma: export





#include "Containers/String.inl"