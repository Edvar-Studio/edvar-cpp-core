#pragma once
namespace edvar {
struct argument_parser;

class platform_base {
public:
#include "./generic_type_usage"
    typedef void* application_handle;

    static platform_base& get();

    virtual ~platform_base() = default;
    virtual bool is_debugger_present();
    virtual void debug_break(const wchar_t* message);
    virtual void on_fatal(const wchar_t* message);
    virtual edvar::string_base<char_utf16>& abort(int32 exit_code) = 0;

    virtual edvar::string_base<char_utf16> get_stack_trace(int skip_count, int count);
    virtual edvar::string_base<char_utf16>& get_app_name() = 0;
    virtual application_handle get_application_handle() = 0;
    
    virtual edvar::string_base<char_utf16> get_executable_path() = 0;
    virtual edvar::string_base<char_utf16> get_executable_directory() = 0;
    virtual edvar::string_base<char_utf16> get_start_arguments() = 0;

    struct {
        edvar::function_reference_array<void, argument_parser&>* on_startup;
        edvar::function_reference_array<void>* on_shutdown;
        edvar::function_reference_array<void>* on_fatal;


    } application_messages;
};
} // namespace edvar