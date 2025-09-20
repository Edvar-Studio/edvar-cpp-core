#pragma once

namespace edvar {

class platform_base {
public:
    static platform_base& get();

    virtual ~platform_base() = default;
    virtual bool is_debugger_present();
    virtual void debug_break(const wchar_t* message);
    virtual void on_fatal(const wchar_t* message);

    virtual void get_stack_trace(int skip_count, int count);
};
} // namespace edvar