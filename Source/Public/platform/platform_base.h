#pragma once
namespace edvar {
struct argument_parser;
struct platform_file_system;
struct platform_windowing_system;
namespace app {
class application_base;
}
class platform_base {
public:
    platform_base();
    typedef void* process_handle;

    static platform_base& get();

    virtual ~platform_base() = default;
    virtual bool is_debugger_present() = 0;
    virtual void debug_break(const wchar_t* message) = 0;
    virtual void on_fatal(const wchar_t* message) = 0;

    virtual edvar::string_base<char_utf16> get_stack_trace(int skip_count, int count) = 0;

    virtual process_handle get_this_process_handle() = 0;
    virtual void abort(int32_t exit_code, const process_handle& in_process = nullptr) = 0;
    virtual edvar::string_base<char_utf16>& get_process_name(const process_handle& in_process = nullptr) = 0;
    virtual bool is_valid_process(const process_handle& in_process) = 0;
    virtual edvar::string_base<char_utf16> get_process_executable_path(const process_handle& in_process = nullptr) = 0;
    virtual edvar::string_base<char_utf16> get_start_arguments_string() = 0;
    virtual void set_process_dpi_awareness(bool is_dpi_aware) = 0;

    virtual platform_file_system& get_file_system() = 0;
    virtual platform_windowing_system& get_windowing_system() = 0;

    void register_application(memory::shared_pointer<edvar::app::application_base> application);
    void unregister_application(memory::shared_pointer<edvar::app::application_base> application);


    virtual void poll_events(edvar::app::application_base& in_application) = 0;

    virtual void create_guid(uint8_t* out_buffer) = 0;
protected:
    array<memory::shared_pointer<edvar::app::application_base>> application_registry;
};

} // namespace edvar