#pragma once
#include "Platform/Windows/WindowsPlatformWindowing.hpp"

namespace Edvar::Rendering {
class IRenderDevice;
typedef Memory::SharedReference<IRenderDevice> RenderDeviceRef;

class EDVAR_CPP_CORE_API IRenderDeviceDependent : public Memory::EnableSharedFromThis<IRenderDeviceDependent> {
public:
    ~IRenderDeviceDependent() override;
    explicit IRenderDeviceDependent(const SharedReference<IRenderDevice>& device);

    SharedReference<IRenderDevice> GetAssociatedDevice() const;

    void CheckDeviceSanity() const;

    virtual void OnRenderDeviceLost() {};
    virtual void OnRenderDeviceRestored(IRenderDevice& newDevice);

private:
    SharedReference<IRenderDevice> associatedDevice;
};

class EDVAR_CPP_CORE_API IRenderingAPI : public Memory::EnableSharedFromThis<IRenderingAPI> {
public:
    explicit IRenderingAPI(const char16_t* name) {
        Memory::CopyMemory(
            Name, name,
            Math::Min(Utils::CStrings::Length(name) + 1, static_cast<int32_t>(sizeof(Name) / sizeof(char16_t))));
    }
    ~IRenderingAPI() override;
    [[nodiscard]] const char16_t* GetName() const { return Name; }
    [[nodiscard]] bool IsActiveAPI() const;

    SharedReference<IRenderDevice> GetPrimaryDevice();

    virtual bool DoesOutputSupportHDR(const Platform::MonitorInfo& monitorInfo) { return false; }

    static SharedReference<IRenderingAPI> GetActiveAPI(bool bSelectIfNull = true);
    static void SetActiveAPI(const SharedReference<IRenderingAPI>& newAPI);

    static Utils::MultiDelegate<void(SharedReference<IRenderingAPI>, SharedReference<IRenderingAPI>)>
    OnRenderingAPIChanged();

    static Utils::MultiDelegate<void(SharedReference<IRenderDevice>)> OnRenderDeviceLost();

    static Utils::MultiDelegate<void(SharedReference<IRenderDevice>)> OnRenderDeviceRestored();

    static void NotifyRenderDeviceLost(const IRenderDevice& lostDevice);

protected:
    virtual SharedPointer<IRenderDevice> CreatePrimaryRenderDevice() = 0;

private:
    char16_t Name[32]{};
    static SharedPointer<IRenderingAPI> ActiveAPI;
    SharedPointer<IRenderDevice> PrimaryDevice;
};

} // namespace Edvar::Rendering