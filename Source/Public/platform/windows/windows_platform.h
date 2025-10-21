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
    virtual void abort(int32 exit_code, const process_handle& in_process = nullptr) override;
    virtual bool is_valid_process(const process_handle& in_process) override;
    virtual edvar::string_base<char_utf16>& get_process_name(const process_handle& in_process = nullptr) override;
    virtual edvar::string_base<char_utf16> get_stack_trace(int skip_count, int count) override;

    virtual edvar::string_base<char_utf16>
    get_process_executable_path(const process_handle& in_process = nullptr) override;
    virtual edvar::string_base<char_utf16> get_start_arguments_string() override;

    virtual platform_file_system& get_file_system() override;
};
} // namespace edvar::platform_types

#endif