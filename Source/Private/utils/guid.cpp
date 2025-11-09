#include "utils/guid.h"
#include "platform/platform.h"

namespace edvar {
guid guid::create() {
    guid new_guid;
    platform::get().create_guid(new_guid.data);
    return new_guid;
}

optional<guid> guid::from_string(const string& guid_string) {
    guid new_guid;
    // split by '-'
    array<string> parts = guid_string.split(L"-");
    if (parts.length() != 5) {
        return optional<guid>();
    }
    // parse each part
    static uint32_t expected_lengths[5] = {8, 4, 4, 4, 12};
    uint32_t byte_index = 0;
    for (size_t i = 0; i < 5; ++i) {
        if (parts[i].length() != expected_lengths[i]) {
            return optional<guid>();
        }
        for (size_t j = 0; j < expected_lengths[i]; j += 2) {
            string byte_str = parts[i].substring(j, 2);
            value_or_error_code<uint8_t> byte_value = byte_str.to_number<uint8_t>(16);
            if (byte_value.is_error()) {
                return optional<guid>();
            }
            new_guid.data[byte_index++] = byte_value.get_value();
        }
    }
    return optional<guid>(new_guid);
}
} // namespace edvar