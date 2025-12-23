#pragma once

#include "Utils/CString.hpp"

namespace Edvar::Math {
struct LinearColor;

// SRGB Color with 8 bits per channel
struct Color {
    union {
        struct {
            uint8_t R, G, B, A;
        };
        uint8_t RGBA[4];
    };

    Color() : R(0), G(0), B(0), A(255) {}
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) : R(r), G(g), B(b), A(a) {}
    Color(float r, float g, float b, float a = 1.0f)
        : R(static_cast<uint8_t>(Math::Clamp(r * 255.0f, 0.0f, 255.0f))),
          G(static_cast<uint8_t>(Math::Clamp(g * 255.0f, 0.0f, 255.0f))),
          B(static_cast<uint8_t>(Math::Clamp(b * 255.0f, 0.0f, 255.0f))),
          A(static_cast<uint8_t>(Math::Clamp(a * 255.0f, 0.0f, 255.0f))) {}

    static Color Black;

    LinearColor ToLinearColor() const;

    static Color FromHex(const String& hexString) {
        uint32_t hex = 0;
        if (hexString.StartsWith(u"#")) {
            hex = Utils::CStrings::StringToInt64((hexString.SubString(1).Data()), 16);
        } else {
            hex = Utils::CStrings::StringToInt64(hexString.Data(), 16);
        }

        if (hexString.Length() == 6 || hexString.Length() == 7) {
            return Color(static_cast<uint8_t>((hex >> 16) & 0xFF), static_cast<uint8_t>((hex >> 8) & 0xFF),
                         static_cast<uint8_t>(hex & 0xFF), 255);
        } else if (hexString.Length() == 8 || hexString.Length() == 9) {
            return Color(static_cast<uint8_t>((hex >> 24) & 0xFF), static_cast<uint8_t>((hex >> 16) & 0xFF),
                         static_cast<uint8_t>((hex >> 8) & 0xFF), static_cast<uint8_t>(hex & 0xFF));
        } else {
            return Color();
        }
    }
};

struct LinearColor {
    union {
        struct {
            uint8_t R, G, B, A;
        };
        uint8_t RGBA[4];
    };
    LinearColor() : R(0.0f), G(0.0f), B(0.0f), A(1.0f) {}
    LinearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) : R(r), G(g), B(b), A(a) {}
    LinearColor(float r, float g, float b, float a = 1.0f)
        : R(static_cast<uint8_t>(Math::Clamp(r * 255.f, 0.0f, 255.0f))),
          G(static_cast<uint8_t>(Math::Clamp(g * 255.f, 0.0f, 255.0f))),
          B(static_cast<uint8_t>(Math::Clamp(b * 255.f, 0.0f, 255.0f))),
          A(static_cast<uint8_t>(Math::Clamp(a * 255.f, 0.0f, 255.0f))) {}

    Color ToSRGBColor() const {
        Color returnColor;
        for (int i = 0; i < 4; ++i) {
            float channel = static_cast<float>(RGBA[i]) / 255.0f;
            if (channel <= 0.0031308f) {
                channel = channel * 12.92f;
            } else {
                channel = 1.055f * Math::Power(channel, 1.0f / 2.4f) - 0.055f;
            }
            returnColor.RGBA[i] = static_cast<uint8_t>(Math::Clamp(channel * 255.0f, 0.0f, 255.0f));
        }
        return returnColor;
    }

    static LinearColor FromHex(const String& hexString) {
        uint32_t hex = 0;
        if (hexString.StartsWith(u"#")) {
            hex = Utils::CStrings::StringToInt64((hexString.SubString(1).Data()), 16);
        } else {
            hex = Utils::CStrings::StringToInt64(hexString.Data(), 16);
        }

        if (hexString.Length() == 6 || hexString.Length() == 7) {
            return LinearColor(static_cast<uint8_t>((hex >> 16) & 0xFF), static_cast<uint8_t>((hex >> 8) & 0xFF),
                               static_cast<uint8_t>(hex & 0xFF), 255);
        } else if (hexString.Length() == 8 || hexString.Length() == 9) {
            return LinearColor(static_cast<uint8_t>((hex >> 24) & 0xFF), static_cast<uint8_t>((hex >> 16) & 0xFF),
                               static_cast<uint8_t>((hex >> 8) & 0xFF), static_cast<uint8_t>(hex & 0xFF));
        } else {
            return LinearColor();
        }
    }
};

inline LinearColor Color::ToLinearColor() const {
    LinearColor returnColor;
    for (int i = 0; i < 4; ++i) {
        float channel = static_cast<float>(RGBA[i]) / 255.0f;
        if (channel <= 0.04045f) {
            channel = channel / 12.92f;
        } else {
            channel = std::pow((channel + 0.055f) / 1.055f, 2.4f);
        }
        returnColor.RGBA[i] = static_cast<uint8_t>(Math::Clamp(channel * 255.0f, 0.0f, 255.0f));
    }
    return returnColor;
}

inline Color Color::Black = Color(0, 0, 0, 1.f);
} // namespace Edvar::Math