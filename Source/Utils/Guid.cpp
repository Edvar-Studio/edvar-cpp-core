#include "Utils/Guid.hpp"
namespace Edvar::Utils {
GuidV4 GuidV4::Invalid = GuidV4();
GuidV4::GuidV4() {
    Data.Data1 = 0;
    Data.Data2 = 0;
    Data.Data3 = 0;
    Memory::SetMemory(Data.Data4, 0, 8);
}
GuidV4::GuidV4(const Containers::String& guidString) {
    uint32_t data1 = 0;
    uint16_t data2 = 0;
    uint16_t data3 = 0;
    uint8_t data4[8] = {0};

    if (guidString.Length() != 36) {
        // Invalid GUID format
        Data.Data1 = 0;
        Data.Data2 = 0;
        Data.Data3 = 0;
        Memory::SetMemory(Data.Data4, 0, 8);
        return;
    }

    data1 = static_cast<uint32_t>(CStrings::StringToInt64(guidString.SubString(0, 8).Data(), 16));
    data2 = static_cast<uint16_t>(CStrings::StringToInt64(guidString.SubString(9, 4).Data(), 16));
    data3 = static_cast<uint16_t>(CStrings::StringToInt64(guidString.SubString(14, 4).Data(), 16));

    data4[0] = static_cast<uint8_t>(CStrings::StringToInt64(guidString.SubString(19, 2).Data(), 16));
    data4[1] = static_cast<uint8_t>(CStrings::StringToInt64(guidString.SubString(21, 2).Data(), 16));

    for (int i = 0; i < 6; ++i) {
        data4[i + 2] = static_cast<uint8_t>(CStrings::StringToInt64(guidString.SubString(24 + i * 2, 2).Data(), 16));
    }

    Data.Data1 = data1;
    Data.Data2 = data2;
    Data.Data3 = data3;
    Memory::CopyMemory(Data.Data4, data4, 8);
}
Containers::String GuidV4::ToString() const {
    return Containers::String::Format(
        u"{:HEX;ZEROFILL;WIDTH=8}-{:HEX;ZEROFILL;WIDTH=4}-{:HEX;ZEROFILL;WIDTH=4}-{:HEX;ZEROFILL;WIDTH=2}{:HEX;"
        u"ZEROFILL;WIDTH=2}-{:HEX;ZEROFILL;WIDTH=2}{:HEX;ZEROFILL;WIDTH=2}{:HEX;ZEROFILL;WIDTH=2}{:HEX;ZEROFILL;"
        u"WIDTH=2}{:HEX;ZEROFILL;WIDTH=2}{:HEX;ZEROFILL;WIDTH=2}",
        Data.Data1, Data.Data2, Data.Data3, Data.Data4[0], Data.Data4[1], Data.Data4[2], Data.Data4[3], Data.Data4[4],
        Data.Data4[5], Data.Data4[6], Data.Data4[7]);
}
bool GuidV4::operator==(const GuidV4& other) const {
    return Data.Data1 == other.Data.Data1 && Data.Data2 == other.Data.Data2 && Data.Data3 == other.Data.Data3 &&
           Memory::CompareMemory(Data.Data4, other.Data.Data4, 8) == 0;
}
GuidV4 GuidV4::GenerateNewGuid() {
    GuidV4 newGuid;
    newGuid.Data.Data1 = Math::Random::GetRandomUInt32();
    newGuid.Data.Data2 = static_cast<uint16_t>(Math::Random::GetRandomUInt32() & 0xFFFF);
    newGuid.Data.Data3 = static_cast<uint16_t>(Math::Random::GetRandomUInt32() & 0xFFFF);
    for (int i = 0; i < 8; ++i) {
        newGuid.Data.Data4[i] = static_cast<uint8_t>(Math::Random::GetRandomUInt32() & 0xFF);
    }

    // Set the version to 4 -- random
    newGuid.Data.Data3 &= 0x0FFF;
    newGuid.Data.Data3 |= (4 << 12);

    // Set the variant to RFC 4122
    newGuid.Data.Data4[0] &= 0x3F;
    newGuid.Data.Data4[0] |= 0x80;

    return newGuid;
}

} // namespace Edvar::Utils
