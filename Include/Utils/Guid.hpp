#pragma once

namespace Edvar::Utils {
class EDVAR_CPP_CORE_API GuidV4 {
public:
    GuidV4();
    explicit GuidV4(const Containers::String& guidString);

    [[nodiscard]] Containers::String ToString() const;

    bool operator==(const GuidV4& other) const;
    bool operator!=(const GuidV4& other) const { return !(*this == other); }
    static GuidV4 Invalid;

    static GuidV4 GenerateNewGuid();

    operator bool() const { return *this != Invalid; }

private:
    struct {
        uint32_t Data1;
        uint16_t Data2;
        uint16_t Data3;
        uint8_t Data4[8];
    } Data;
};
} // namespace Edvar::Utils