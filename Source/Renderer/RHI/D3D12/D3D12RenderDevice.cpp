#include "Renderer/RHI/D3D12/D3D12RenderDevice.hpp"
#ifdef _WIN32

#    include "Renderer/RHI/RenderingCore.hpp"
#    include "Renderer/RHI/D3D12/D3D12CommandAllocator.hpp"
#    include "Renderer/RHI/D3D12/D3D12CommandList.hpp"
#    include "Renderer/RHI/D3D12/D3D12CommandQueue.hpp"
#    include "Renderer/RHI/D3D12/D3D12RenderingAPI.hpp"
#    include "Renderer/RHI/D3D12/D3D12Swapchain.hpp"

#    include <d3d12.h>

namespace Edvar::Renderer::RHI::D3D12 {
static constexpr uint32_t RTV_HEAP_SIZE = 1024;
static constexpr uint32_t DSV_HEAP_SIZE = 512;
static constexpr uint32_t CBV_SRV_UAV_SHADER_VISIBLE_HEAP_SIZE = 150'000;
static constexpr uint32_t SAMPLER_HEAP_SIZE = 64;
static constexpr uint32_t CBV_SRV_UAV_HEAP_SIZE = 2048;

D3D12RenderDevice::D3D12RenderDevice(void* nativeDeviceHandle) : NativeHandle(nativeDeviceHandle) {
    RTVHeap = MakeShared<D3D12DescriptorHeap>(AsShared(), D3D12DescriptorHeapType::RenderTarget, RTV_HEAP_SIZE, false);
    DSVHeap = MakeShared<D3D12DescriptorHeap>(AsShared(), D3D12DescriptorHeapType::DepthStencil, DSV_HEAP_SIZE, false);
    CBV_SRV_UAV_ShaderVisibleHeap = MakeShared<D3D12DescriptorHeap>(AsShared(), D3D12DescriptorHeapType::CBV_SRV_UAV,
                                                                    CBV_SRV_UAV_SHADER_VISIBLE_HEAP_SIZE, true);
    CBV_SRV_UAV_Heap =
        MakeShared<D3D12DescriptorHeap>(AsShared(), D3D12DescriptorHeapType::CBV_SRV_UAV, CBV_SRV_UAV_HEAP_SIZE, false);
    SamplerHeap =
        MakeShared<D3D12DescriptorHeap>(AsShared(), D3D12DescriptorHeapType::Sampler, SAMPLER_HEAP_SIZE, true);
}
D3D12RenderDevice::~D3D12RenderDevice() { D3D12RenderDevice::Destroy(); }
void D3D12RenderDevice::DoCheckDeviceSanity() const {
    if (NativeHandle != nullptr) {
        auto* device = static_cast<ID3D12Device*>(NativeHandle);
        StaticCastSharedReference<D3D12RenderingAPI>(IRenderingAPI::GetActiveAPI())->PrintStoredMessagesToDebugger();
        const auto reason = device->GetDeviceRemovedReason();
        if (reason == S_OK) {
            return;
        }

        switch (reason) {
        case DXGI_ERROR_DEVICE_REMOVED:
        case DXGI_ERROR_DEVICE_RESET: {
            IRenderingAPI::NotifyRenderDeviceLost(*this);
        }
        default: {
            return;
        }
        }
    }
}
bool D3D12RenderDevice::HasShaderModelSupport(const ShaderModel model) const {
    if (NativeHandle != nullptr) {
        auto* device = static_cast<ID3D12Device*>(NativeHandle);
        D3D12_FEATURE_DATA_SHADER_MODEL shaderModelFeature = {};
        shaderModelFeature.HighestShaderModel = model == ShaderModel::SM6 ? D3D_SHADER_MODEL_6_0 : D3D_SHADER_MODEL_5_1;
        if (SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModelFeature,
                                                  sizeof(shaderModelFeature)))) {
            return shaderModelFeature.HighestShaderModel >=
                   (model == ShaderModel::SM6 ? D3D_SHADER_MODEL_6_0 : D3D_SHADER_MODEL_5_1);
        }
    }
    return false;
}
bool D3D12RenderDevice::HasRayTracingSupport() const {
    if (NativeHandle != nullptr) {
        auto* device = static_cast<ID3D12Device*>(NativeHandle);
        D3D12_FEATURE_DATA_D3D12_OPTIONS5 options5 = {};
        if (SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &options5, sizeof(options5)))) {
            return options5.RaytracingTier != D3D12_RAYTRACING_TIER_NOT_SUPPORTED;
        }
    }
    return false;
}
void D3D12RenderDevice::Destroy() {
    IRenderDevice::Destroy();
    if (NativeHandle != nullptr) {
        auto* device = static_cast<ID3D12Device*>(NativeHandle);
        device->Release();
        NativeHandle = nullptr;
    }
}
SharedReference<ISwapchain> D3D12RenderDevice::CreateSwapchain(const Windowing::Window& window,
                                                               ResourceDataFormat resourceDataFormat) {
    SharedReference<D3D12Swapchain> swapchain = MakeShared<D3D12Swapchain>(AsShared(), window, resourceDataFormat);
    return swapchain;
}
SharedReference<ICommandAllocator> D3D12RenderDevice::CreateCommandAllocator(CommandListType type) {
    auto* allocator = new D3D12CommandAllocator(AsShared(), type);
    return allocator;
}
SharedReference<ICommandList>
D3D12RenderDevice::CreateCommandList(CommandListType type, const SharedReference<ICommandAllocator>& allocator) {
    auto* commandList = new D3D12CommandList(AsShared(), type, allocator);
    return commandList;
}
SharedPointer<ICommandQueue> D3D12RenderDevice::GetGraphicsQueue() {
    if (!GraphicsQueue) [[unlikely]] {
        GraphicsQueue = SharedPointer(new D3D12CommandQueue(AsShared()));
    }
    return GraphicsQueue;
}
SharedPointer<ICommandQueue> D3D12RenderDevice::GetComputeQueue() {
    if (!ComputeQueue) [[unlikely]] {
        ComputeQueue = SharedPointer(new D3D12CommandQueue(AsShared(), true, CommandListType::Compute));
    }
    return ComputeQueue;
}
SharedPointer<ICommandQueue> D3D12RenderDevice::GetCopyQueue() {
    if (!CopyQueue) [[unlikely]] {
        CopyQueue = SharedPointer(new D3D12CommandQueue(AsShared(), true, CommandListType::Copy));
    }
    return CopyQueue;
}
D3D12DescriptorHeap::D3D12DescriptorHeap(const SharedReference<IRenderDevice>& device,
                                         const D3D12DescriptorHeapType heapType, const int32_t count,
                                         const bool shaderVisible)
    : IRenderDeviceDependent(device), type(heapType), allocationRange(count) {

    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.NumDescriptors = count;
    heapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    switch (type) {

    case D3D12DescriptorHeapType::RenderTarget:
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        break;
    case D3D12DescriptorHeapType::DepthStencil:
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        break;
    case D3D12DescriptorHeapType::CBV_SRV_UAV:
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        break;
    case D3D12DescriptorHeapType::Sampler:
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
        break;
    default:
        Platform::GetPlatform().OnFatalError(u"Unknown type passed for D3D12 Descriptor Heap.");
        return;
    }

    const SharedReference<D3D12RenderDevice> d3d12DeviceWrapper = StaticCastSharedReference<D3D12RenderDevice>(device);
    auto* nativeDevice = static_cast<ID3D12Device*>(d3d12DeviceWrapper->NativeHandle);
    ID3D12DescriptorHeap* heap = nullptr;
    if (FAILED(nativeDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&heap)))) {
        CheckDeviceSanity();
        heap->Release();
        allocationRange.Clear();
        return;
    }
    nativeHeapPtr = heap;
    auto [ptr] = heap->GetCPUDescriptorHandleForHeapStart();
    cpuHeapStartPtr = reinterpret_cast<void*>(ptr);
    gpuHeapStartPtr = shaderVisible ? reinterpret_cast<void*>(heap->GetGPUDescriptorHandleForHeapStart().ptr) : nullptr;
    descriptorIncreaseSize = nativeDevice->GetDescriptorHandleIncrementSize(heapDesc.Type);
}
D3D12DescriptorHeap::~D3D12DescriptorHeap() {
    if (nativeHeapPtr != nullptr) {
        auto* heap = static_cast<ID3D12DescriptorHeap*>(nativeHeapPtr);
        heap->Release();
        nativeHeapPtr = nullptr;
        descriptorIncreaseSize = 0;
        cpuHeapStartPtr = nullptr;
        allocationRange.Clear();
    }
}
D3D12DescriptorHeapAllocation D3D12DescriptorHeap::AllocateDescriptor() {
    const uint32_t index = allocationRange.Allocate();
    if (index == UINT32_MAX) {
        Platform::GetPlatform().OnFatalError(u"D3D12 Descriptor Heap: Out of descriptors.");
        return {};
    }
    D3D12_CPU_DESCRIPTOR_HANDLE handle = {};
    handle.ptr = reinterpret_cast<uint64_t>(cpuHeapStartPtr) + (static_cast<uint64_t>(index) * descriptorIncreaseSize);
    D3D12DescriptorHeapAllocation returnAllocation;
    returnAllocation.ParentHeap = SharedFromThis(this);
    returnAllocation.DescriptorIndex = index;
    return returnAllocation;
}
void D3D12DescriptorHeap::FreeDescriptor(const D3D12DescriptorHeapAllocation& allocation) {
    allocationRange.Free(allocation.DescriptorIndex);
}
bool D3D12DescriptorHeap::IsValidDescriptorAllocation(const D3D12DescriptorHeapAllocation& allocation) const {
    return allocationRange.IsAllocated(allocation.DescriptorIndex);
}
void* D3D12DescriptorHeapAllocation::GetCPUHandle() const {
    if (IsValid()) {
        return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(ParentHeap->cpuHeapStartPtr) +
                                       (static_cast<uintptr_t>(DescriptorIndex) * ParentHeap->descriptorIncreaseSize));
    }
    return nullptr;
}
void* D3D12DescriptorHeapAllocation::GetGPUHandle() const {
    if (IsValid()) {
        return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(ParentHeap->gpuHeapStartPtr) +
                                       (static_cast<uintptr_t>(DescriptorIndex) * ParentHeap->descriptorIncreaseSize));
    }
    return nullptr;
}

} // namespace Edvar::Renderer::RHI::D3D12

#endif
