#pragma once
#ifdef _WIN32
#include "Platform/IPlatform.hpp"


namespace Edvar::Platform{

namespace Windows{
class WindowsPlatform final : public IPlatform{
    [[nodiscard]] const char* GetName() const override{
        return "Windows";
    }
    [[nodiscard]] IPlatformThreading& GetThreading() const override;
    [[nodiscard]] IPlatformWindowing& GetWindowing() const override;
    [[nodiscard]] IPlatformInput& GetInput() const override;
};
}

using PlatformType = Windows::WindowsPlatform;

}
#endif