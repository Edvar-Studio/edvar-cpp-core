#pragma once

#include "force_include.h"
namespace edvar::diagnostics{
    EDVAR_CPP_CORE_API void check(const bool check_value, const wchar_t* message = nullptr);

    template<typename... args>
    inline void check_printf(const bool check_value, const wchar_t* format,const args&... arguments){

    }
}