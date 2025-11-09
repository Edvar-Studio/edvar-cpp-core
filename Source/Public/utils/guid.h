#pragma once


namespace edvar{
struct guid{
    static guid create();
    /**
     * create a guid from a string representation.
     * string example: "c08474e7-e478-4693-8472-02d26c2f4f6d"
     */
    static optional<guid> from_string(const string& guid_string);

    guid() = default;
    guid(const guid& other) = default;
    guid& operator=(const guid& other) = default;
    uint8_t data[16]{0};

    string to_string() const;

    inline bool operator==(const guid& other) const{
        for (size_t i = 0; i < 16; ++i) {
            if (data[i] != other.data[i]) {
                return false;
            }
        }
        return true;
    }
    bool operator!=(const guid& other) const{
        return !(*this == other);
    }
};
}