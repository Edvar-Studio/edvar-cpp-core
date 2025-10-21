#include "platform/platform_file_system.h"
#include "shobjidl_core.h"
#include "shtypes.h"

#ifdef _WIN32
#    define WIN32_LEAN_AND_MEAN
#    include "platform/windows/windows_platform_file_system.h"
#    include "utils/value_or_error.h"
#    include "utils/optional.h"
#    include "platform/platform_base.h"
#    include "utils/value_or_error.h"
#    include <windows.h>
#    include <winnt.h>
#    include <stdio.h>
#    include <DbgHelp.h>
#    include <libloaderapi.h>
#    include <Psapi.h>
#    include <winerror.h>
#    include <Knownfolders.h>
#    include <shlobj.h>
#    include <processenv.h>
namespace edvar::platform_types {

edvar::value_or_error<platform_file_system::native_file_handle, file_operation_error>
windows_platform_file_system::open_file(const edvar::string_base<platform_base::char_utf16>& path,
                                        file_access_mode open_mode, file_share_mode share_mode) {
    DWORD win32_desired_acccess = 0;
    if ((open_mode & file_access_mode::read) == file_access_mode::read) {
        win32_desired_acccess |= GENERIC_READ;
    }
    if ((open_mode & file_access_mode::write) == file_access_mode::write) {
        win32_desired_acccess |= GENERIC_WRITE;
    }
    DWORD win32_open_mode = 0;
    if ((open_mode & file_access_mode::truncate) == file_access_mode::truncate) {
        win32_open_mode = TRUNCATE_EXISTING;
    } else {
        win32_open_mode = OPEN_EXISTING;
    }

    DWORD win32_share_mode = 0;
    if ((share_mode & file_share_mode::allow_read) == file_share_mode::allow_read) {
        win32_share_mode |= FILE_SHARE_READ;
    }
    if ((share_mode & file_share_mode::allow_write) == file_share_mode::allow_write) {
        win32_share_mode |= FILE_SHARE_WRITE;
    }
    if ((share_mode & file_share_mode::allow_delete) == file_share_mode::allow_delete) {
        win32_share_mode |= FILE_SHARE_DELETE;
    }

    HANDLE file_handle = ::CreateFileW(reinterpret_cast<LPCWSTR>(path.data()), win32_desired_acccess, win32_share_mode,
                                       NULL, win32_open_mode, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file_handle == INVALID_HANDLE_VALUE) {
        DWORD error_code = GetLastError();
        // Map error code to file_operation_error
        switch (error_code) {
        case ERROR_FILE_NOT_FOUND:
            return edvar::value_or_error<native_file_handle, file_operation_error>::from_error(
                file_operation_error::file_not_found);
        case ERROR_PATH_NOT_FOUND:
            return edvar::value_or_error<native_file_handle, file_operation_error>::from_error(
                file_operation_error::invalid_path);
        case ERROR_INVALID_HANDLE:
            return edvar::value_or_error<native_file_handle, file_operation_error>::from_error(
                file_operation_error::invalid_file_handle);
        case ERROR_ACCESS_DENIED:
            return edvar::value_or_error<native_file_handle, file_operation_error>::from_error(
                file_operation_error::access_denied);
        case ERROR_SHARING_VIOLATION:
            return edvar::value_or_error<native_file_handle, file_operation_error>::from_error(
                file_operation_error::sharing_violation);
        case ERROR_FILENAME_EXCED_RANGE:
            return edvar::value_or_error<native_file_handle, file_operation_error>::from_error(
                file_operation_error::path_too_long);
        case ERROR_DISK_FULL:
            return edvar::value_or_error<native_file_handle, file_operation_error>::from_error(
                file_operation_error::disk_full);
        default:
            return edvar::value_or_error<native_file_handle, file_operation_error>::from_error(
                file_operation_error::unknown_error);
        }
    }
    return edvar::value_or_error<native_file_handle, file_operation_error>::from_value(file_handle);
}
edvar::value_or_error<platform_base::uint32, file_operation_error>
windows_platform_file_system::read_file(native_file_handle handle, void* buffer, platform_base::uint32 size) {
    DWORD bytes_read = 0;
    HANDLE win32_file_handle = reinterpret_cast<HANDLE>(handle);
    if (!::ReadFile(win32_file_handle, buffer, size, &bytes_read, NULL)) {
        DWORD error_code = GetLastError();
        // Map error code to file_operation_error
        switch (error_code) {
        case ERROR_INVALID_HANDLE:
            return edvar::value_or_error<platform_base::uint32, file_operation_error>::from_error(
                file_operation_error::invalid_file_handle);
        case ERROR_ACCESS_DENIED:
            return edvar::value_or_error<platform_base::uint32, file_operation_error>::from_error(
                file_operation_error::access_denied);
        case ERROR_SHARING_VIOLATION:
            return edvar::value_or_error<platform_base::uint32, file_operation_error>::from_error(
                file_operation_error::sharing_violation);
        default:
            return edvar::value_or_error<platform_base::uint32, file_operation_error>::from_error(
                file_operation_error::unknown_error);
        }
    }
    return edvar::value_or_error<platform_base::uint32, file_operation_error>::from_value(bytes_read);
}
edvar::value_or_error<platform_base::uint32, file_operation_error>
windows_platform_file_system::write_file(native_file_handle handle, const void* buffer, platform_base::uint32 size) {
    DWORD bytes_written = 0;
    HANDLE win32_file_handle = reinterpret_cast<HANDLE>(handle);
    if (!::WriteFile(win32_file_handle, buffer, size, &bytes_written, NULL)) {
        DWORD error_code = GetLastError();
        // Map error code to file_operation_error
        switch (error_code) {
        case ERROR_INVALID_HANDLE:
            return edvar::value_or_error<platform_base::uint32, file_operation_error>::from_error(
                file_operation_error::invalid_file_handle);
        case ERROR_ACCESS_DENIED:
            return edvar::value_or_error<platform_base::uint32, file_operation_error>::from_error(
                file_operation_error::access_denied);
        case ERROR_DISK_FULL:
            return edvar::value_or_error<platform_base::uint32, file_operation_error>::from_error(
                file_operation_error::disk_full);
        case ERROR_SHARING_VIOLATION:
            return edvar::value_or_error<platform_base::uint32, file_operation_error>::from_error(
                file_operation_error::sharing_violation);
        default:
            return edvar::value_or_error<platform_base::uint32, file_operation_error>::from_error(
                file_operation_error::unknown_error);
        }
    }
    return edvar::value_or_error<platform_base::uint32, file_operation_error>::from_value(bytes_written);
}
edvar::value_or_error<platform_base::uint32, file_operation_error>
windows_platform_file_system::flush_file(native_file_handle handle) {
    HANDLE win32_file_handle = reinterpret_cast<HANDLE>(handle);
    if (!::FlushFileBuffers(win32_file_handle)) {
        DWORD error_code = GetLastError();
        // Map error code to file_operation_error
        switch (error_code) {
        case ERROR_INVALID_HANDLE:
            return edvar::value_or_error<platform_base::uint32, file_operation_error>::from_error(
                file_operation_error::invalid_file_handle);
        case ERROR_ACCESS_DENIED:
            return edvar::value_or_error<platform_base::uint32, file_operation_error>::from_error(
                file_operation_error::access_denied);
        case ERROR_SHARING_VIOLATION:
            return edvar::value_or_error<platform_base::uint32, file_operation_error>::from_error(
                file_operation_error::sharing_violation);
        default:
            return edvar::value_or_error<platform_base::uint32, file_operation_error>::from_error(
                file_operation_error::unknown_error);
        }
    }
    return edvar::value_or_error<platform_base::uint32, file_operation_error>::from_value(0);
}
edvar::value_or_error<platform_base::uint32, file_operation_error>
windows_platform_file_system::close_file(native_file_handle handle) {
    HANDLE win32_file_handle = reinterpret_cast<HANDLE>(handle);
    if (!::CloseHandle(win32_file_handle)) {
        DWORD error_code = GetLastError();
        // Map error code to file_operation_error
        switch (error_code) {
        case ERROR_INVALID_HANDLE:
            return edvar::value_or_error<platform_base::uint32, file_operation_error>::from_error(
                file_operation_error::invalid_file_handle);
        case ERROR_ACCESS_DENIED:
            return edvar::value_or_error<platform_base::uint32, file_operation_error>::from_error(
                file_operation_error::access_denied);
        case ERROR_SHARING_VIOLATION:
            return edvar::value_or_error<platform_base::uint32, file_operation_error>::from_error(
                file_operation_error::sharing_violation);
        default:
            return edvar::value_or_error<platform_base::uint32, file_operation_error>::from_error(
                file_operation_error::unknown_error);
        }
    }
    return edvar::value_or_error<platform_base::uint32, file_operation_error>::from_value(0);
}
bool windows_platform_file_system::file_exists(const edvar::string_base<platform_base::char_utf16>& path) {
    DWORD attributes = GetFileAttributesW(reinterpret_cast<const wchar_t*>(path.data()));
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        return false;
    }
    return (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
}
edvar::error_result<file_operation_error>
windows_platform_file_system::create_file(const edvar::string_base<platform_base::char_utf16>& path) {
    HANDLE file_handle = ::CreateFileW(reinterpret_cast<LPCWSTR>(path.data()), GENERIC_WRITE, 0, NULL, CREATE_NEW,
                                       FILE_ATTRIBUTE_NORMAL, NULL);
    if (file_handle == INVALID_HANDLE_VALUE) {
        DWORD error_code = GetLastError();
        // Map error code to file_operation_error
        switch (error_code) {
        case ERROR_FILE_EXISTS:
            return edvar::error_result<file_operation_error>(file_operation_error::file_exists);
        case ERROR_PATH_NOT_FOUND:
            return edvar::error_result<file_operation_error>(file_operation_error::invalid_path);
        case ERROR_ACCESS_DENIED:
            return edvar::error_result<file_operation_error>(file_operation_error::access_denied);
        case ERROR_SHARING_VIOLATION:
            return edvar::error_result<file_operation_error>(file_operation_error::sharing_violation);
        default:
            return edvar::error_result<file_operation_error>(file_operation_error::unknown_error);
        }
    }
    ::CloseHandle(file_handle); // The file handle should be closed after creation.
    return edvar::error_result<file_operation_error>();
}
edvar::error_result<file_operation_error>
windows_platform_file_system::delete_file(const edvar::string_base<platform_base::char_utf16>& path) {
    if (!::DeleteFileW(reinterpret_cast<LPCWSTR>(path.data()))) {
        DWORD error_code = GetLastError();
        // Map error code to file_operation_error
        switch (error_code) {
        case ERROR_FILE_NOT_FOUND:
            return edvar::error_result<file_operation_error>(file_operation_error::file_not_found);
        case ERROR_PATH_NOT_FOUND:
            return edvar::error_result<file_operation_error>(file_operation_error::invalid_path);
        case ERROR_ACCESS_DENIED:
            return edvar::error_result<file_operation_error>(file_operation_error::access_denied);
        case ERROR_SHARING_VIOLATION:
            return edvar::error_result<file_operation_error>(file_operation_error::sharing_violation);
        default:
            return edvar::error_result<file_operation_error>(file_operation_error::unknown_error);
        }
    }
    return edvar::error_result<file_operation_error>();
}

edvar::error_result<file_operation_error>
windows_platform_file_system::move_file(const edvar::string_base<platform_base::char_utf16>& from,
                                        const edvar::string_base<platform_base::char_utf16>& to) {
    if (!::MoveFileW(reinterpret_cast<LPCWSTR>(from.data()), reinterpret_cast<LPCWSTR>(to.data()))) {
        DWORD error_code = GetLastError();
        // Map error code to file_operation_error
        switch (error_code) {
        case ERROR_FILE_NOT_FOUND:
            return edvar::error_result<file_operation_error>(file_operation_error::file_not_found);
        case ERROR_PATH_NOT_FOUND:
            return edvar::error_result<file_operation_error>(file_operation_error::invalid_path);
        case ERROR_ACCESS_DENIED:
            return edvar::error_result<file_operation_error>(file_operation_error::access_denied);
        case ERROR_SHARING_VIOLATION:
            return edvar::error_result<file_operation_error>(file_operation_error::sharing_violation);
        default:
            return edvar::error_result<file_operation_error>(file_operation_error::unknown_error);
        }
    }
    return edvar::error_result<file_operation_error>();
}

edvar::error_result<file_operation_error>
windows_platform_file_system::copy_file(const edvar::string_base<platform_base::char_utf16>& from,
                                        const edvar::string_base<platform_base::char_utf16>& to) {
    if (!::CopyFileW(reinterpret_cast<LPCWSTR>(from.data()), reinterpret_cast<LPCWSTR>(to.data()), FALSE)) {
        DWORD error_code = GetLastError();
        // Map error code to file_operation_error
        switch (error_code) {
        case ERROR_FILE_NOT_FOUND:
            return edvar::error_result<file_operation_error>(file_operation_error::file_not_found);
        case ERROR_PATH_NOT_FOUND:
            return edvar::error_result<file_operation_error>(file_operation_error::invalid_path);
        case ERROR_ACCESS_DENIED:
            return edvar::error_result<file_operation_error>(file_operation_error::access_denied);
        case ERROR_SHARING_VIOLATION:
            return edvar::error_result<file_operation_error>(file_operation_error::sharing_violation);
        default:
            return edvar::error_result<file_operation_error>(file_operation_error::unknown_error);
        }
    }
    return edvar::error_result<file_operation_error>();
}

file_time_result windows_platform_file_system::get_file_modification_time(const native_file_handle& handle) {
    HANDLE win32_file_handle = reinterpret_cast<HANDLE>(handle);
    FILETIME creation_time, access_time, modification_time;
    if (!GetFileTime(win32_file_handle, &creation_time, &access_time, &modification_time)) {
        DWORD error_code = GetLastError();
        // Map error code to file_operation_error
        switch (error_code) {
        case ERROR_INVALID_HANDLE:
            return file_time_result::from_error(file_operation_error::invalid_file_handle);
        case ERROR_ACCESS_DENIED:
            return file_time_result::from_error(file_operation_error::access_denied);
        case ERROR_SHARING_VIOLATION:
            return file_time_result::from_error(file_operation_error::sharing_violation);
        default:
            return file_time_result::from_error(file_operation_error::unknown_error);
        }
    }
    // Convert FILETIME to edvar::time
    ULARGE_INTEGER ull;
    ull.LowPart = modification_time.dwLowDateTime;
    ull.HighPart = modification_time.dwHighDateTime;
    // FILETIME is in 100-nanosecond intervals since January 1, 1601 (UTC)
    // edvar::time is assumed to be in nanoseconds since January 1, 1970 (UTC)
    uint64 filetime_ns = ull.QuadPart * 100; // Convert to nanoseconds
    // Calculate the difference between 1601 and 1970 in nanoseconds
    static constexpr uint64 EPOCH_DIFFERENCE_NS = 116444736000000000ULL;
    uint64 unix_time_ns = filetime_ns - EPOCH_DIFFERENCE_NS;
    return file_time_result::from_value(edvar::time(unix_time_ns));
}
file_time_result windows_platform_file_system::get_file_creation_time(const native_file_handle& handle) {
    HANDLE win32_file_handle = reinterpret_cast<HANDLE>(handle);
    FILETIME creation_time, access_time, modification_time;
    if (!GetFileTime(win32_file_handle, &creation_time, &access_time, &modification_time)) {
        DWORD error_code = GetLastError();
        // Map error code to file_operation_error
        switch (error_code) {
        case ERROR_INVALID_HANDLE:
            return file_time_result::from_error(file_operation_error::invalid_file_handle);
        case ERROR_ACCESS_DENIED:
            return file_time_result::from_error(file_operation_error::access_denied);
        case ERROR_SHARING_VIOLATION:
            return file_time_result::from_error(file_operation_error::sharing_violation);
        default:
            return file_time_result::from_error(file_operation_error::unknown_error);
        }
    }
    // Convert FILETIME to edvar::time
    ULARGE_INTEGER ull;
    ull.LowPart = creation_time.dwLowDateTime;
    ull.HighPart = creation_time.dwHighDateTime;
    // FILETIME is in 100-nanosecond intervals since January 1, 1601 (UTC)
    // edvar::time is assumed to be in nanoseconds since January 1, 1970 (UTC)
    uint64 filetime_ns = ull.QuadPart * 100; // Convert to nanoseconds
    // Calculate the difference between 1601 and 1970 in nanoseconds
    static constexpr uint64 EPOCH_DIFFERENCE_NS = 116444736000000000ULL;
    uint64 unix_time_ns = filetime_ns - EPOCH_DIFFERENCE_NS;
    return file_time_result::from_value(edvar::time(unix_time_ns));
}
file_time_result windows_platform_file_system::get_file_access_time(const native_file_handle& handle) {
    HANDLE win32_file_handle = reinterpret_cast<HANDLE>(handle);
    FILETIME creation_time, access_time, modification_time;
    if (!GetFileTime(win32_file_handle, &creation_time, &access_time, &modification_time)) {
        DWORD error_code = GetLastError();
        // Map error code to file_operation_error
        switch (error_code) {
        case ERROR_INVALID_HANDLE:
            return file_time_result::from_error(file_operation_error::invalid_file_handle);
        case ERROR_ACCESS_DENIED:
            return file_time_result::from_error(file_operation_error::access_denied);
        case ERROR_SHARING_VIOLATION:
            return file_time_result::from_error(file_operation_error::sharing_violation);
        default:
            return file_time_result::from_error(file_operation_error::unknown_error);
        }
    }
    // Convert FILETIME to edvar::time
    ULARGE_INTEGER ull;
    ull.LowPart = access_time.dwLowDateTime;
    ull.HighPart = access_time.dwHighDateTime;
    // FILETIME is in 100-nanosecond intervals since January 1, 1601 (UTC)
    // edvar::time is assumed to be in nanoseconds since January 1, 1970 (UTC)
    uint64 filetime_ns = ull.QuadPart * 100; // Convert to nanoseconds
    // Calculate the difference between 1601 and 1970 in nanoseconds
    static constexpr uint64 EPOCH_DIFFERENCE_NS = 116444736000000000ULL;
    uint64 unix_time_ns = filetime_ns - EPOCH_DIFFERENCE_NS;
    return file_time_result::from_value(edvar::time(unix_time_ns));
}

file_time_result windows_platform_file_system::set_file_modification_time(const native_file_handle& handle,
                                                                          const edvar::time& time) {
    HANDLE win32_file_handle = reinterpret_cast<HANDLE>(handle);
    // Convert edvar::time to FILETIME
    uint64 unix_time_ns = time.to_nanoseconds();
    static constexpr uint64 EPOCH_DIFFERENCE_NS = 116444736000000000ULL;
    uint64 filetime_ns = unix_time_ns + EPOCH_DIFFERENCE_NS;
    ULARGE_INTEGER ull;
    ull.QuadPart = filetime_ns / 100; // Convert to 100-nanosecond intervals
    FILETIME modification_time;
    modification_time.dwLowDateTime = ull.LowPart;
    modification_time.dwHighDateTime = ull.HighPart;
    if (!SetFileTime(win32_file_handle, NULL, NULL, &modification_time)) {
        DWORD error_code = GetLastError();
        // Map error code to file_operation_error
        switch (error_code) {
        case ERROR_INVALID_HANDLE:
            return file_time_result::from_error(file_operation_error::invalid_file_handle);
        case ERROR_ACCESS_DENIED:
            return file_time_result::from_error(file_operation_error::access_denied);
        case ERROR_SHARING_VIOLATION:
            return file_time_result::from_error(file_operation_error::sharing_violation);
        default:
            return file_time_result::from_error(file_operation_error::unknown_error);
        }
    }
    return file_time_result::from_value(time);
}
file_time_result windows_platform_file_system::set_file_creation_time(const native_file_handle& handle,
                                                                      const edvar::time& time) {
    HANDLE win32_file_handle = reinterpret_cast<HANDLE>(handle);
    // Convert edvar::time to FILETIME
    uint64 unix_time_ns = time.to_nanoseconds();
    static constexpr uint64 EPOCH_DIFFERENCE_NS = 116444736000000000ULL;
    uint64 filetime_ns = unix_time_ns + EPOCH_DIFFERENCE_NS;
    ULARGE_INTEGER ull;
    ull.QuadPart = filetime_ns / 100; // Convert to 100-nanosecond intervals
    FILETIME creation_time;
    creation_time.dwLowDateTime = ull.LowPart;
    creation_time.dwHighDateTime = ull.HighPart;
    if (!SetFileTime(win32_file_handle, &creation_time, NULL, NULL)) {
        DWORD error_code = GetLastError();
        // Map error code to file_operation_error
        switch (error_code) {
        case ERROR_INVALID_HANDLE:
            return file_time_result::from_error(file_operation_error::invalid_file_handle);
        case ERROR_ACCESS_DENIED:
            return file_time_result::from_error(file_operation_error::access_denied);
        case ERROR_SHARING_VIOLATION:
            return file_time_result::from_error(file_operation_error::sharing_violation);
        default:
            return file_time_result::from_error(file_operation_error::unknown_error);
        }
    }
    return file_time_result::from_value(time);
}
file_time_result windows_platform_file_system::set_file_access_time(const native_file_handle& handle,
                                                                    const edvar::time& time) {
    HANDLE win32_file_handle = reinterpret_cast<HANDLE>(handle);
    // Convert edvar::time to FILETIME
    uint64 unix_time_ns = time.to_nanoseconds();
    static constexpr uint64 EPOCH_DIFFERENCE_NS = 116444736000000000ULL;
    uint64 filetime_ns = unix_time_ns + EPOCH_DIFFERENCE_NS;
    ULARGE_INTEGER ull;
    ull.QuadPart = filetime_ns / 100; // Convert to 100-nanosecond intervals
    FILETIME last_access_time;
    last_access_time.dwLowDateTime = ull.LowPart;
    last_access_time.dwHighDateTime = ull.HighPart;
    if (!SetFileTime(win32_file_handle, NULL, &last_access_time, NULL)) {
        DWORD error_code = GetLastError();
        // Map error code to file_operation_error
        switch (error_code) {
        case ERROR_INVALID_HANDLE:
            return file_time_result::from_error(file_operation_error::invalid_file_handle);
        case ERROR_ACCESS_DENIED:
            return file_time_result::from_error(file_operation_error::access_denied);
        case ERROR_SHARING_VIOLATION:
            return file_time_result::from_error(file_operation_error::sharing_violation);
        default:
            return file_time_result::from_error(file_operation_error::unknown_error);
        }
    }
    return file_time_result::from_value(time);
}
edvar::error_result<file_operation_error>
windows_platform_file_system::create_directory(const edvar::string_base<platform_base::char_utf16>& path) {
    if (!::CreateDirectoryW(reinterpret_cast<LPCWSTR>(path.data()), NULL)) {
        DWORD error_code = GetLastError();
        // Map error code to file_operation_error
        switch (error_code) {
        case ERROR_ALREADY_EXISTS:
            return edvar::error_result<file_operation_error>(file_operation_error::file_exists);
        case ERROR_PATH_NOT_FOUND:
            return edvar::error_result<file_operation_error>(file_operation_error::invalid_path);
        case ERROR_ACCESS_DENIED:
            return edvar::error_result<file_operation_error>(file_operation_error::access_denied);
        case ERROR_SHARING_VIOLATION:
            return edvar::error_result<file_operation_error>(file_operation_error::sharing_violation);
        default:
            return edvar::error_result<file_operation_error>(file_operation_error::unknown_error);
        }
    }
    return edvar::error_result<file_operation_error>();
}
edvar::error_result<file_operation_error>
windows_platform_file_system::delete_directory(const edvar::string_base<platform_base::char_utf16>& path) {
    if (!::RemoveDirectoryW(reinterpret_cast<LPCWSTR>(path.data()))) {
        DWORD error_code = GetLastError();
        // Map error code to file_operation_error
        switch (error_code) {
        case ERROR_FILE_NOT_FOUND:
            return edvar::error_result<file_operation_error>(file_operation_error::file_not_found);
        case ERROR_PATH_NOT_FOUND:
            return edvar::error_result<file_operation_error>(file_operation_error::invalid_path);
        case ERROR_ACCESS_DENIED:
            return edvar::error_result<file_operation_error>(file_operation_error::access_denied);
        case ERROR_SHARING_VIOLATION:
            return edvar::error_result<file_operation_error>(file_operation_error::sharing_violation);
        default:
            return edvar::error_result<file_operation_error>(file_operation_error::unknown_error);
        }
    }
    return edvar::error_result<file_operation_error>();
}
edvar::value_or_error<edvar::array<platform_file_system::directory_content_entry>, file_operation_error>
windows_platform_file_system::list_directory_contents(const edvar::string_base<platform_base::char_utf16>& path) {
    edvar::array<directory_content_entry> entries;
    edvar::string_base<platform_base::char_utf16> search_path = path + L"\\*";
    WIN32_FIND_DATAW find_data;
    HANDLE hFind = ::FindFirstFileW(reinterpret_cast<LPCWSTR>(search_path.data()), &find_data);
    if (hFind == INVALID_HANDLE_VALUE) {
        DWORD error_code = GetLastError();
        // Map error code to file_operation_error
        switch (error_code) {
        case ERROR_FILE_NOT_FOUND:
            return edvar::value_or_error<edvar::array<directory_content_entry>, file_operation_error>::from_error(
                file_operation_error::file_not_found);
        case ERROR_PATH_NOT_FOUND:
            return edvar::value_or_error<edvar::array<directory_content_entry>, file_operation_error>::from_error(
                file_operation_error::invalid_path);
        case ERROR_ACCESS_DENIED:
            return edvar::value_or_error<edvar::array<directory_content_entry>, file_operation_error>::from_error(
                file_operation_error::access_denied);
        case ERROR_SHARING_VIOLATION:
            return edvar::value_or_error<edvar::array<directory_content_entry>, file_operation_error>::from_error(
                file_operation_error::sharing_violation);
        default:
            return edvar::value_or_error<edvar::array<directory_content_entry>, file_operation_error>::from_error(
                file_operation_error::unknown_error);
        }
    }

    do {
        directory_content_entry entry;
        entry.name = edvar::string_base<platform_base::char_utf16>(find_data.cFileName);
        entry.is_directory = (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        entries.add(entry);
    } while (::FindNextFileW(hFind, &find_data));

    ::FindClose(hFind);
    return edvar::value_or_error<edvar::array<directory_content_entry>, file_operation_error>::from_value(entries);
}
edvar::string_base<platform_base::char_utf16> windows_platform_file_system::get_current_working_directory() {
    // Get the required buffer size
    DWORD buffer_size = ::GetCurrentDirectoryW(0, NULL);
    edvar::string_base<platform_base::char_utf16> buffer;
    buffer.resize(buffer_size);
    ::GetCurrentDirectoryW(buffer_size, reinterpret_cast<LPWSTR>(buffer.data()));
    return buffer;
}
edvar::string_base<platform_base::char_utf16>
windows_platform_file_system::set_current_working_directory(const edvar::string_base<platform_base::char_utf16>& path) {
    ::SetCurrentDirectoryW(reinterpret_cast<LPCWSTR>(path.data()));
    return get_current_working_directory();
}
edvar::string_base<platform_base::char_utf16>
windows_platform_file_system::get_special_directory(const edvar::string_base<platform_base::char_utf16>& name) {
    auto folder_id = FOLDERID_Profile; // Default to Profile
    if (name == "home" || name == "user_home") {
        folder_id = FOLDERID_Profile;
    } else if (name == "desktop") {
        folder_id = FOLDERID_Desktop;
    } else if (name == "documents") {
        folder_id = FOLDERID_Documents;
    } else if (name == "downloads") {
        folder_id = FOLDERID_Downloads;
    } else if (name == "pictures") {
        folder_id = FOLDERID_Pictures;
    } else if (name == "music") {
        folder_id = FOLDERID_Music;
    } else if (name == "videos") {
        folder_id = FOLDERID_Videos;
    } else if (name == "appdata") {
        folder_id = FOLDERID_LocalAppData;
    } else if (name == "roamingappdata" || name == "roaming_appdata") {
        folder_id = FOLDERID_RoamingAppData;
    } else if (name == "programdata" || name == "program_data" || name == "commonappdata" ||
               name == "common_app_data") {
        folder_id = FOLDERID_ProgramData;
    } else if (name == "programfiles" || name == "program_files") {
        folder_id = FOLDERID_ProgramFiles;
    } else if (name == "programfilesx86" || name == "program_files_x86") {
        folder_id = FOLDERID_ProgramFilesX86;
    } else if (name == "config") {
        folder_id = FOLDERID_RoamingAppData;
    } else if (name == "logs") {
        folder_id = FOLDERID_LocalAppData;
    } else if (name == "temp") {
        DWORD length = GetTempPathW(0, NULL); // Get required buffer size
        edvar::string_base<platform_base::char_utf16> temp_path;
        temp_path.resize(length);
        GetTempPathW(length, reinterpret_cast<LPWSTR>(temp_path.data()));
        return temp_path;
    } else {
        return edvar::string_base<platform_base::char_utf16>();
    }
    wchar_t* path_buffer = nullptr;
    if (SUCCEEDED(SHGetKnownFolderPath(folder_id, 0, NULL, &path_buffer))) {
        edvar::string_base<platform_base::char_utf16> result(path_buffer);
        CoTaskMemFree(path_buffer);
        return result;
    }
    return edvar::string_base<platform_base::char_utf16>();
}
edvar::container::array<edvar::string_base<platform_base::char_utf16>>
windows_platform_file_system::native_open_dialog(const edvar::string_base<platform_base::char_utf16>& title,
                                                 const edvar::container::array<file_dialog_filter>& filters,
                                                 bool allow_multiple_selection, bool pick_folders, bool file_must_exist,
                                                 const edvar::string_base<platform_base::char_utf16>& default_path,
                                                 const edvar::string_base<platform_base::char_utf16>& default_extension,
                                                 platform_windowing_system::native_window_handle parent_window) {
    IFileOpenDialog* pFileOpen;
    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFileOpen));
    edvar::container::array<edvar::string_base<platform_base::char_utf16>> selected_paths;
    if (SUCCEEDED(hr)) {
        pFileOpen->SetTitle(reinterpret_cast<const wchar_t*>(title.data()));
        pFileOpen->SetDefaultExtension(reinterpret_cast<const wchar_t*>(default_extension.data()));
        DWORD options;
        pFileOpen->GetOptions(&options);
        options |= FOS_FORCEFILESYSTEM;
        if (allow_multiple_selection) {
            options |= FOS_ALLOWMULTISELECT;
        }
        if (pick_folders) {
            options |= FOS_PICKFOLDERS;
        }
        if (file_must_exist) {
            options |= FOS_FILEMUSTEXIST;
        }
        pFileOpen->SetOptions(options);

        if (!title.is_empty()) {
            pFileOpen->SetTitle(reinterpret_cast<LPCWSTR>(title.data()));
        }

        array<COMDLG_FILTERSPEC> comFilters;
        for (const auto& filter : filters) {
            COMDLG_FILTERSPEC comFilter;
            comFilter.pszName = reinterpret_cast<LPCWSTR>(filter.name.data());
            comFilter.pszSpec = reinterpret_cast<LPCWSTR>(filter.pattern.data());
            comFilters.add(comFilter);
        }
        if (!comFilters.is_empty()) {
            pFileOpen->SetFileTypes(static_cast<UINT>(comFilters.length()), comFilters.data());
            pFileOpen->SetFileTypeIndex(1); // Set the first filter as default
        }

        if (!default_path.is_empty()) {
            IShellItem* pDefaultItem;
            hr = SHCreateItemFromParsingName(reinterpret_cast<LPCWSTR>(default_path.data()), NULL,
                                             IID_PPV_ARGS(&pDefaultItem));
            if (SUCCEEDED(hr)) {
                pFileOpen->SetDefaultFolder(pDefaultItem);
                pDefaultItem->Release();
            }
        }

        hr = pFileOpen->Show(reinterpret_cast<HWND>(parent_window));
        if (SUCCEEDED(hr)) {
            IShellItemArray* pItemArray;
            hr = pFileOpen->GetResults(&pItemArray);
            if (SUCCEEDED(hr)) {
                DWORD item_count = 0;
                pItemArray->GetCount(&item_count);
                for (DWORD i = 0; i < item_count; ++i) {
                    IShellItem* pItem;
                    hr = pItemArray->GetItemAt(i, &pItem);
                    if (SUCCEEDED(hr)) {
                        PWSTR pszFilePath = NULL;
                        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                        if (SUCCEEDED(hr)) {
                            selected_paths.add(edvar::string_base<platform_base::char_utf16>(pszFilePath));
                            CoTaskMemFree(pszFilePath);
                        }
                        pItem->Release();
                    }
                }
                pItemArray->Release();
            }
        }
        pFileOpen->Release();
    }
    return selected_paths;
}
edvar::container::array<edvar::string_base<platform_base::char_utf16>>
windows_platform_file_system::native_save_dialog(const edvar::string_base<platform_base::char_utf16>& title,
                                                 const edvar::container::array<file_dialog_filter>& filters,
                                                 bool prompt_overwrite,
                                                 const edvar::string_base<platform_base::char_utf16>& default_file_name,
                                                 const edvar::string_base<platform_base::char_utf16>& default_path,
                                                 const edvar::string_base<platform_base::char_utf16>& default_extension,
                                                 platform_windowing_system::native_window_handle parent_window) {
    IFileSaveDialog* pFileSave;
    HRESULT hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFileSave));
    edvar::container::array<edvar::string_base<platform_base::char_utf16>> selected_paths;
    if (SUCCEEDED(hr)) {
        pFileSave->SetTitle(reinterpret_cast<const wchar_t*>(title.data()));
        pFileSave->SetDefaultExtension(reinterpret_cast<const wchar_t*>(default_extension.data()));
        DWORD options;
        pFileSave->GetOptions(&options);
        options |= FOS_FORCEFILESYSTEM;
        if (prompt_overwrite) {
            options |= FOS_OVERWRITEPROMPT;
        }
        pFileSave->SetOptions(options);

        if (!title.is_empty()) {
            pFileSave->SetTitle(reinterpret_cast<LPCWSTR>(title.data()));
        }

        array<COMDLG_FILTERSPEC> comFilters;
        for (const auto& filter : filters) {
            COMDLG_FILTERSPEC comFilter;
            comFilter.pszName = reinterpret_cast<LPCWSTR>(filter.name.data());
            comFilter.pszSpec = reinterpret_cast<LPCWSTR>(filter.pattern.data());
            comFilters.add(comFilter);
        }
        if (!comFilters.is_empty()) {
            pFileSave->SetFileTypes(static_cast<UINT>(comFilters.length()), comFilters.data());
            pFileSave->SetFileTypeIndex(1); // Set the first filter as default
        }

        if (!default_file_name.is_empty()) {
            pFileSave->SetFileName(reinterpret_cast<LPCWSTR>(default_file_name.data()));
        }

        if (!default_path.is_empty()) {
            IShellItem* pDefaultItem;
            hr = SHCreateItemFromParsingName(reinterpret_cast<LPCWSTR>(default_path.data()), NULL,
                                             IID_PPV_ARGS(&pDefaultItem));
            if (SUCCEEDED(hr)) {
                pFileSave->SetDefaultFolder(pDefaultItem);
                pDefaultItem->Release();
            }
        }

        hr = pFileSave->Show(reinterpret_cast<HWND>(parent_window));
        if (SUCCEEDED(hr)) {
            IShellItem* pItem;
            hr = pFileSave->GetResult(&pItem);
            if (SUCCEEDED(hr)) {
                PWSTR pszFilePath = NULL;
                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                if (SUCCEEDED(hr)) {
                    selected_paths.add(edvar::string_base<platform_base::char_utf16>(pszFilePath));
                    CoTaskMemFree(pszFilePath);
                }
                pItem->Release();
            }
        }
        pFileSave->Release();
    }
    return selected_paths;
}
} // namespace edvar::platform_types
#endif