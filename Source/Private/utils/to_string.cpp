#include "utils/to_string.h"

namespace edvar {
edvar::string_base<platform_base::char_utf16>
edvar::convertible_to_string::to_string(const edvar::string_base<platform_base::char_utf16>& options) const {
    return this->to_string();
}
} // namespace edvar
