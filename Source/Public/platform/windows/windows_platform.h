#pragma once
#ifdef _WIN32
#    include "platform/platform_base.h"

namespace edvar::platform_types {

class windows_platform : public platform_base {
public:
    windows_platform();
    virtual ~windows_platform() override;
    virtual bool is_debugger_present() override;
    virtual void debug_break(const wchar_t* message) override;
    virtual void on_fatal(const wchar_t* message) override;
    virtual process_handle get_this_process_handle() override;
    virtual void abort(int32_t exit_code, const process_handle& in_process = nullptr) override;
    virtual bool is_valid_process(const process_handle& in_process) override;
    virtual edvar::string_base<char_utf16>& get_process_name(const process_handle& in_process = nullptr) override;
    virtual edvar::string_base<char_utf16> get_stack_trace(int skip_count, int count) override;

    virtual edvar::string_base<char_utf16>
    get_process_executable_path(const process_handle& in_process = nullptr) override;
    virtual edvar::string_base<char_utf16> get_start_arguments_string() override;
    virtual void set_process_dpi_awareness(bool is_dpi_aware) override;

    virtual platform_file_system& get_file_system() override;
    virtual platform_windowing_system& get_windowing_system() override;

    virtual void poll_events(edvar::app::application_base& in_application) override;

    virtual void create_guid(uint8_t* out_buffer) override;
};
} // namespace edvar::platform_types

#endif