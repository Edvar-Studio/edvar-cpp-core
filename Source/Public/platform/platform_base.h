#pragma once
namespace edvar {
struct argument_parser;
struct platform_file_system;

class platform_base {
public:
#include "./generic_type_usage"
    typedef void* process_handle;

    static platform_base& get();

    virtual ~platform_base() = default;
    virtual bool is_debugger_present() = 0;
    virtual void debug_break(const wchar_t* message) = 0;
    virtual void on_fatal(const wchar_t* message) = 0;

    virtual edvar::string_base<char_utf16> get_stack_trace(int skip_count, int count) = 0;

    virtual process_handle get_this_process_handle() = 0;
    virtual void abort(int32 exit_code, const process_handle& in_process = nullptr) = 0;
    virtual edvar::string_base<char_utf16>& get_process_name(const process_handle& in_process = nullptr) = 0;
    virtual bool is_valid_process(const process_handle& in_process) = 0;
    virtual edvar::string_base<char_utf16> get_process_executable_path(const process_handle& in_process = nullptr) = 0;
    virtual edvar::string_base<char_utf16>
    get_process_executable_directory(const process_handle& in_process = nullptr) = 0;
    virtual edvar::string_base<char_utf16> get_start_arguments_string() = 0;

    virtual platform_file_system& get_file_system() = 0;

    struct {
        edvar::function_reference_array<void, argument_parser&>* on_startup;
        edvar::function_reference_array<void>* on_shutdown;
        edvar::function_reference_array<void>* on_fatal;

    } application_messages;
};

} // namespace edvar