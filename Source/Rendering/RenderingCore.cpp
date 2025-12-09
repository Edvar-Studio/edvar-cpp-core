#include "Rendering/RenderingCore.hpp"
#include "Rendering/IRenderDevice.hpp"
#ifdef _WIN32
#    include "Rendering/D3D12/D3D12RenderingAPI.hpp"
#else
#    include "Rendering/Vulkan/VulkanRenderingAPI.hpp"
#endif
namespace Edvar::Rendering {
SharedPointer<IRenderingAPI> IRenderingAPI::ActiveAPI;
IRenderDeviceDependent::~IRenderDeviceDependent() {
    if (associatedDevice.IsValid()) [[likely]] {
        associatedDevice->dependents.Remove(AsShared());
    }
}
IRenderDeviceDependent::IRenderDeviceDependent(const SharedReference<IRenderDevice>& device)
    : associatedDevice(device) {
    device->dependents.Add(AsShared());
}
SharedReference<IRenderDevice> IRenderDeviceDependent::GetAssociatedDevice() const { return associatedDevice; }
void IRenderDeviceDependent::CheckDeviceSanity() const { GetAssociatedDevice()->DoCheckDeviceSanity(); }

void IRenderDeviceDependent::OnRenderDeviceRestored(IRenderDevice& newDevice) {
    associatedDevice = newDevice.AsShared();
}

IRenderingAPI::~IRenderingAPI() = default;
bool IRenderingAPI::IsActiveAPI() const { return GetActiveAPI() != this; }
SharedReference<IRenderDevice> IRenderingAPI::GetPrimaryDevice() {
    if (!PrimaryDevice) [[unlikely]] {
        PrimaryDevice = CreatePrimaryRenderDevice();
    }
    return PrimaryDevice;
}
SharedReference<IRenderingAPI> IRenderingAPI::GetActiveAPI(const bool bSelectIfNull) {
    if (bSelectIfNull && !ActiveAPI.IsValid()) [[unlikely]] {
#if _WIN32
        SetActiveAPI(MakeShared<D3D12::D3D12RenderingAPI>());
#else
        SetActiveAPI(MakeShared<Vulkan::VulkanRenderingAPI>());
#endif
    }
    return ActiveAPI.ToSharedReference();
}
void IRenderingAPI::SetActiveAPI(const SharedReference<IRenderingAPI>& newAPI) {
    const SharedPointer<IRenderingAPI> oldAPI = ActiveAPI;
    if (ActiveAPI) {
        // Notify listeners about the change.
    }
    ActiveAPI = newAPI;
    OnRenderingAPIChanged().Broadcast(oldAPI, ActiveAPI);
}
Utils::MultiDelegate<void(SharedReference<IRenderingAPI>, SharedReference<IRenderingAPI>)>
IRenderingAPI::OnRenderingAPIChanged() {
    static Utils::MultiDelegate<void(SharedReference<IRenderingAPI>, SharedReference<IRenderingAPI>)> delegate;
    return delegate;
}
Utils::MultiDelegate<void(SharedReference<IRenderDevice>)> IRenderingAPI::OnRenderDeviceLost() {
    static Utils::MultiDelegate<void(SharedReference<IRenderDevice>)> delegate;
    return delegate;
}
Utils::MultiDelegate<void(SharedReference<IRenderDevice>)> IRenderingAPI::OnRenderDeviceRestored() {
    static Utils::MultiDelegate<void(SharedReference<IRenderDevice>)> delegate;
    return delegate;
}
void IRenderingAPI::NotifyRenderDeviceLost(const IRenderDevice& lostDevice) {
    OnRenderDeviceLost().Broadcast(lostDevice.AsShared());
    // Attempt to recreate the primary device.
    GetActiveAPI()->PrimaryDevice->Destroy();
    GetActiveAPI()->PrimaryDevice = GetActiveAPI()->CreatePrimaryRenderDevice();
    if (!GetActiveAPI()->PrimaryDevice) {
        Platform::Get().OnFatalError(u"RenderingAPI: Failed to restore lost render device.");
        return;
    }
    OnRenderDeviceRestored().Broadcast(GetActiveAPI()->PrimaryDevice.ToSharedReference());
}
} // namespace Edvar::Rendering