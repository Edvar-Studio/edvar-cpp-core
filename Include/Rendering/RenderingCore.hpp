#pragma once

#include "Memory/Ops.hpp"
#include "Utils/Functional.hpp"
namespace Edvar::Rendering {
class IRenderDevice;
class IRenderingAPI {
public:
    IRenderingAPI(const char16_t* name) {
        Memory::CopyMemory(
            Name, name,
            Math::Min(Utils::CStrings::Length(name) + 1, static_cast<int32_t>(sizeof(Name) / sizeof(char16_t))));
    }
    virtual ~IRenderingAPI() = default;
    const char16_t* GetName() const { return Name; }
    bool IsActiveAPI() const;

    SharedReference<IRenderDevice> GetPrimaryDevice() {
        if (!PrimaryDevice) [[unlikely]] {
            PrimaryDevice = SelectPrimaryDevice();
        }
        return PrimaryDevice;
    }

    static SharedReference<IRenderingAPI> GetActiveAPI(bool bSelectIfNull = true);
    static void SetActiveAPI(SharedReference<IRenderingAPI> newAPI);

private:
    char16_t Name[32];
    static SharedPointer<IRenderingAPI> ActiveAPI;
    SharedReference<IRenderDevice> SelectPrimaryDevice();
    SharedPointer<IRenderDevice> PrimaryDevice;
};

Utils::MultiDelegate<void(SharedReference<IRenderingAPI> oldAPI, SharedReference<IRenderingAPI> newAPI)>
    OnRenderingAPIChanged;
} // namespace Edvar::Rendering