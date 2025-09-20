#include "containers/string.h"
#include <cwchar>
#include <wchar.h>
#include <string.h>
namespace edvar::c_string {
uint32 string_length(const wchar_t* buffer) { return static_cast<uint32>(wcslen(buffer)); }
uint32 string_length(const char* buffer) { return static_cast<uint32>(strlen(buffer)); }
} // namespace edvar::c_string