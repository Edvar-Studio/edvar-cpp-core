#pragma once

#include "containers/array.h"
#include "platform/platform_windowing_system.h"
#include "platform_base.h"
namespace edvar {
struct time;
}

namespace edvar::platform_types {

enum class file_access_mode : platform_base::uint32 {
    // read mode
    read = 1 << 0,
    // write access mode
    write = 1 << 1,
    // open in binary mode, not text mode
    binary = 1 << 2,
    // should the file be truncated when opened. (erase existing contents)
    truncate = 1 << 3,
};

enum class file_share_mode : platform_base::uint32 {
    allow_none = 0,
    allow_read = 1 << 0,
    allow_write = 1 << 1,
    allow_delete = 1 << 2,
};

enum class file_operation_error : platform_base::uint32 {
    /**
     * If the path was invalid (e.g., contains illegal characters)
     */
    invalid_path,
    /**
     * If the file handle was invalid (e.g., nullptr).
     */
    invalid_file_handle,
    /**
     * If the file was not found at the specified path.
     */
    file_not_found,
    /**
     * If the file path is too long to be processed.
     */
    path_too_long,
    /**
     * If access to the file is denied.
     */
    access_denied,
    /**
     * If the disk is full and no more data can be written.
     */
    disk_full,
    /**
     * If the file is being used by another process and cannot be accessed.
     */
    sharing_violation,

    /**
     * If the file already exists at the specified path.
     */
    file_exists,

    /**
     * An unknown error occurred.
     */
    unknown_error = 0xFFFFFFFF
};

file_access_mode operator|(const file_access_mode& a, const file_access_mode& b) {
    return static_cast<file_access_mode>(static_cast<platform_base::uint32>(a) | static_cast<platform_base::uint32>(b));
}
file_access_mode operator&(const file_access_mode& a, const file_access_mode& b) {
    return static_cast<file_access_mode>(static_cast<platform_base::uint32>(a) & static_cast<platform_base::uint32>(b));
}

file_share_mode operator|(const file_share_mode& a, const file_share_mode& b) {
    return static_cast<file_share_mode>(static_cast<platform_base::uint32>(a) | static_cast<platform_base::uint32>(b));
}
file_share_mode operator&(const file_share_mode& a, const file_share_mode& b) {
    return static_cast<file_share_mode>(static_cast<platform_base::uint32>(a) & static_cast<platform_base::uint32>(b));
}

typedef edvar::value_or_error<platform_base::uint32, file_operation_error> file_buffer_result;
typedef edvar::value_or_error<edvar::time, file_operation_error> file_time_result;

struct platform_file_system {
public:
    virtual ~platform_file_system() = default;
    // Native handle for file operations. Platforms may choose a different underlying
    // representation (FILE*, HANDLE, file descriptor, etc.). nullptr means invalid.
    typedef void* native_file_handle;

    // Invalid/native sentinel for convenience
    static constexpr native_file_handle invalid_native_file_handle = nullptr;

    /**
     * @brief Open a file at the specified path.
     */
    virtual edvar::value_or_error<native_file_handle, file_operation_error>
    open_file(const edvar::string_base<platform_base::char_utf16>& path, file_access_mode open_mode,
              file_share_mode share_mode) = 0;

    // Read up to `size` bytes into `buffer`. Returns number of bytes read or error.
    virtual file_buffer_result read_file(native_file_handle handle, void* buffer, platform_base::uint32 size) = 0;

    // Write `size` bytes from `buffer`. Returns number of bytes written or error.
    virtual file_buffer_result write_file(native_file_handle handle, const void* buffer,
                                          platform_base::uint32 size) = 0;
    // Flush the file buffers to disk.
    virtual file_buffer_result flush_file(native_file_handle handle) = 0;

    // Close a native file handle.
    virtual file_buffer_result close_file(native_file_handle handle) = 0;

    // Check if a file exists at path. (Checks only for files, directories will return false)
    virtual bool file_exists(const edvar::string_base<platform_base::char_utf16>& path) = 0;
    // Create an empty file at path.
    virtual edvar::error_result<file_operation_error>
    create_file(const edvar::string_base<platform_base::char_utf16>& path) = 0;

    virtual edvar::error_result<file_operation_error>
    delete_file(const edvar::string_base<platform_base::char_utf16>& path) = 0;

    virtual edvar::error_result<file_operation_error>
    move_file(const edvar::string_base<platform_base::char_utf16>& from,
              const edvar::string_base<platform_base::char_utf16>& to) = 0;
    // Copy a file from one path to another. Will replace existing file at destination.
    virtual edvar::error_result<file_operation_error>
    copy_file(const edvar::string_base<platform_base::char_utf16>& from,
              const edvar::string_base<platform_base::char_utf16>& to) = 0;

    virtual file_time_result get_file_modification_time(const native_file_handle& handle) = 0;
    virtual file_time_result get_file_creation_time(const native_file_handle& handle) = 0;
    virtual file_time_result get_file_access_time(const native_file_handle& handle) = 0;

    virtual file_time_result set_file_modification_time(const native_file_handle& handle, const edvar::time& time) = 0;
    virtual file_time_result set_file_creation_time(const native_file_handle& handle, const edvar::time& time) = 0;
    virtual file_time_result set_file_access_time(const native_file_handle& handle, const edvar::time& time) = 0;

    // Check if the directory exists at path. If the path points to a file, returns false.
    virtual bool directory_exists(const edvar::string_base<platform_base::char_utf16>& path) = 0;

    // Create a directory at the specified path. If parent directories do not exist, they won't be created.
    // (non-recursive)
    virtual edvar::error_result<file_operation_error>
    create_directory(const edvar::string_base<platform_base::char_utf16>& path) = 0;

    // Delete a directory at the specified path. Directory must be empty.
    virtual edvar::error_result<file_operation_error>
    delete_directory(const edvar::string_base<platform_base::char_utf16>& path) = 0;
    struct directory_content_entry {
        edvar::string_base<platform_base::char_utf16> name;
        bool is_directory;
    };
    virtual edvar::value_or_error<edvar::array<directory_content_entry>, file_operation_error>
    list_directory_contents(const edvar::string_base<platform_base::char_utf16>& path) = 0;

    virtual edvar::string_base<platform_base::char_utf16> get_current_working_directory() = 0;
    virtual edvar::string_base<platform_base::char_utf16>
    set_current_working_directory(const edvar::string_base<platform_base::char_utf16>& path) = 0;

    virtual edvar::string_base<platform_base::char_utf16> get_temp_directory() = 0;

    /**
     * @brief Get a special directory by name. E.g., "Documents", "Downloads", "Desktop", etc.
     * Due to this being platform dependent, the names are not standardized.
     * But some will always work:
     * - app_data (application data directory on linux it is ~/.local/share and on windows it is %APPDATA%),
     * - user_home (in linux it is /home/username and on windows it is C:\Users\username)
     * - temp (system temporary directory in linux it is /tmp and on windows it is %TEMP%)
     * - desktop (user desktop directory in linux it is /home/username/Desktop and on windows it is
     * C:\Users\username\Desktop)
     * - documents (user documents directory in linux it is /home/username/Documents and on windows it is
     * C:\Users\username\Documents)
     * - downloads (user downloads directory in linux it is /home/username/Downloads and on windows it is
     * C:\Users\username\Downloads)
     * - config (configuration files directory on linux it is ~/.config and on windows it is %APPDATA%\Roaming)
     * - logs (log files directory on linux it is /var/log and on windows it is %PROGRAMDATA%\Logs)
     *
     * @note The returned path may not exist. It is the caller's responsibility to create it if necessary.
     * @param name Name of the special directory.
     * @return Path to the special directory.
     */
    virtual edvar::string_base<platform_base::char_utf16>
    get_special_directory(const edvar::string_base<platform_base::char_utf16>& name) = 0;
    struct file_dialog_filter {
        edvar::string_base<platform_base::char_utf16> name;
        edvar::string_base<platform_base::char_utf16> pattern; // e.g., "*.txt;*.md"
    };
    virtual edvar::container::array<edvar::string_base<platform_base::char_utf16>>
    native_open_dialog(const edvar::string_base<platform_base::char_utf16>& title,
                       const edvar::container::array<file_dialog_filter>& filters,
                       bool allow_multiple_selection = false, bool pick_folders = false, bool file_must_exist = true,
                       const edvar::string_base<platform_base::char_utf16>& default_path =
                           edvar::string_base<platform_base::char_utf16>(),
                       const edvar::string_base<platform_base::char_utf16>& default_extension =
                           edvar::string_base<platform_base::char_utf16>(),
                       platform_windowing_system::native_window_handle parent_window = nullptr) = 0;

    virtual edvar::container::array<edvar::string_base<platform_base::char_utf16>>
    native_save_dialog(const edvar::string_base<platform_base::char_utf16>& title,
                       const edvar::container::array<file_dialog_filter>& filters, bool prompt_overwrite = true,
                       const edvar::string_base<platform_base::char_utf16>& default_file_name =
                           edvar::string_base<platform_base::char_utf16>(),
                       const edvar::string_base<platform_base::char_utf16>& default_path =
                           edvar::string_base<platform_base::char_utf16>(),
                       const edvar::string_base<platform_base::char_utf16>& default_extension =
                           edvar::string_base<platform_base::char_utf16>(),
                       platform_windowing_system::native_window_handle parent_window = nullptr) = 0;
};
} // namespace edvar::platform_types