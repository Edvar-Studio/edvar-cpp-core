#pragma once
#ifdef _WIN32
#    include "utils/time.h"
#    include "platform/platform_file_system.h"
namespace edvar::platform_types {
struct windows_platform_file_system : public platform_file_system {
    static windows_platform_file_system& get_instance() {
        static windows_platform_file_system instance;
        return instance;
    }

    virtual ~windows_platform_file_system() override = default;

    virtual edvar::value_or_error<platform_file_system::native_file_handle, file_operation_error>
    open_file(const edvar::string_base<char_utf16>& path,
              file_access_mode open_mode = file_access_mode::read | file_access_mode::write,
              file_share_mode share_mode = file_share_mode::allow_none) override;

    virtual edvar::value_or_error<uint32_t, file_operation_error>
    read_file(native_file_handle handle, void* buffer, uint32_t size) override;

    virtual edvar::value_or_error<uint32_t, file_operation_error>
    write_file(native_file_handle handle, const void* buffer, uint32_t size) override;

    virtual edvar::value_or_error<uint32_t, file_operation_error>
    flush_file(native_file_handle handle) override;

    virtual edvar::value_or_error<uint32_t, file_operation_error>
    close_file(native_file_handle handle) override;

    virtual bool file_exists(const edvar::string_base<char_utf16>& path) override;
    virtual edvar::error_result<file_operation_error>
    create_file(const edvar::string_base<char_utf16>& path) override;

    virtual edvar::error_result<file_operation_error>
    delete_file(const edvar::string_base<char_utf16>& path) override;

    virtual edvar::error_result<file_operation_error>
    move_file(const edvar::string_base<char_utf16>& from,
              const edvar::string_base<char_utf16>& to) override;

    virtual edvar::error_result<file_operation_error>
    copy_file(const edvar::string_base<char_utf16>& from,
              const edvar::string_base<char_utf16>& to) override;

    virtual file_time_result get_file_modification_time(const native_file_handle& handle) override;
    virtual file_time_result get_file_creation_time(const native_file_handle& handle) override;
    virtual file_time_result get_file_access_time(const native_file_handle& handle) override;

    virtual file_time_result set_file_modification_time(const native_file_handle& handle,
                                                        const edvar::time& time) override;
    virtual file_time_result set_file_creation_time(const native_file_handle& handle, const edvar::time& time) override;
    virtual file_time_result set_file_access_time(const native_file_handle& handle, const edvar::time& time) override;

    // Check if the directory exists at path. If the path points to a file, returns false.
    virtual bool directory_exists(const edvar::string_base<char_utf16>& path) override;

    // Create a directory at the specified path. If parent directories do not exist, they won't be created.
    // (non-recursive)
    virtual edvar::error_result<file_operation_error>
    create_directory(const edvar::string_base<char_utf16>& path) override;

    // Delete a directory at the specified path. Directory must be empty.
    virtual edvar::error_result<file_operation_error>
    delete_directory(const edvar::string_base<char_utf16>& path) override;

    virtual edvar::value_or_error<edvar::array<directory_content_entry>, file_operation_error>
    list_directory_contents(const edvar::string_base<char_utf16>& path) override;

    virtual edvar::string_base<char_utf16> get_current_working_directory() override;
    virtual edvar::string_base<char_utf16>
    set_current_working_directory(const edvar::string_base<char_utf16>& path) override;

    virtual edvar::string_base<char_utf16> get_temp_directory() override {
        return get_special_directory("temp");
    }

    /**
     * @brief Get a special directory by name. E.g., "Documents", "Downloads", "Desktop", etc. Will use KnownFolder
     * Folder APIs on Windows while using xdg-user-dirs on Linux.
     * Due to this being platform dependent, the names are not standardized. But some will always work:
     * 
     * - appdata (application data directory, on linux it is ~/.local/share and on windows it is %LOCALAPPDATA%),
     * - home (on linux it is /home/username and on windows it is C:\Users\username)
     * - temp (system temporary directory, on linux it is /tmp and on windows it is %TEMP%)
     * - desktop (user desktop directory, on linux it is /home/username/Desktop and on windows it is
     * C:\Users\username\Desktop)
     * - documents (user documents directory, on linux it is /home/username/Documents and on windows it is
     * C:\Users\username\Documents)
     * - downloads (user downloads directory, on linux it is /home/username/Downloads and on windows it is
     * C:\Users\username\Downloads)
     * - config (configuration files directory, on linux it is ~/.config and on windows it is %APPDATA%)
     * - logs (log files directory, on linux it is /var/log and on windows it is %LOCALAPPDATA%)
     *
     * @note The returned path may not exist. It is the caller's responsibility to create it if necessary.
     * @param name Name of the special directory.
     * @return Path to the special directory.
     */
    virtual edvar::string_base<char_utf16>
    get_special_directory(const edvar::string_base<char_utf16>& name) override;

    virtual edvar::container::array<edvar::string_base<char_utf16>>
    native_open_dialog(const edvar::string_base<char_utf16>& title,
                       const edvar::container::array<file_dialog_filter>& filters,
                       bool allow_multiple_selection = false, bool pick_folders = false, bool file_must_exist = true,
                       const edvar::string_base<char_utf16>& default_path =
                           edvar::string_base<char_utf16>(),
                       const edvar::string_base<char_utf16>& default_extension =
                           edvar::string_base<char_utf16>(),
                       platform_windowing_system::native_window_handle parent_window = nullptr) override;

    virtual edvar::container::array<edvar::string_base<char_utf16>>
    native_save_dialog(const edvar::string_base<char_utf16>& title,
                       const edvar::container::array<file_dialog_filter>& filters, bool prompt_overwrite = true,
                       const edvar::string_base<char_utf16>& default_file_name =
                           edvar::string_base<char_utf16>(),
                       const edvar::string_base<char_utf16>& default_path =
                           edvar::string_base<char_utf16>(),
                       const edvar::string_base<char_utf16>& default_extension =
                           edvar::string_base<char_utf16>(),
                       platform_windowing_system::native_window_handle parent_window = nullptr) override;
};
} // namespace edvar::platform_types
#endif