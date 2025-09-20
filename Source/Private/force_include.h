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

#include "platform/basic_types.h" // IWYU pragma: export
#include "utils/value_or_error.h" // IWYU pragma: export
#include "containers/array.h"     // IWYU pragma: export
#include <initializer_list>       // IWYU pragma: export