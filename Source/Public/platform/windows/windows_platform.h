#pragma once
#ifdef _WIN32
#include "platform/platform_base.h"

namespace edvar::platform_types {
class windows_platform : public platform_base {
public:
#include "../generic_type_usage"

    virtual bool is_debugger_present() override;
    virtual void debug_break(const wchar_t* message) override;
    virtual void on_fatal(const wchar_t* message) override;
};
} // namespace edvar::platform_types

#endif