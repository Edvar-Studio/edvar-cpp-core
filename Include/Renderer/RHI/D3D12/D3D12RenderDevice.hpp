#pragma once
#ifdef _WIN32

#    include "../IRenderDevice.hpp"
#    include "Renderer/RHI/ICommandQueue.hpp"
namespace Edvar::Renderer::RHI::D3D12 {
class D3D12DescriptorHeap;
class D3D12DescriptorHeapAllocation;

class EDVAR_CPP_CORE_API D3D12RenderDevice final : public IRenderDevice {
public:
    explicit D3D12RenderDevice(void* nativeDeviceHandle);
    ~D3D12RenderDevice() override;
    void DoCheckDeviceSanity() const override;
    bool HasShaderModelSupport(ShaderModel model) const override;
    bool HasRayTracingSupport() const override;
    void Destroy() override;

    SharedReference<ISwapchain> CreateSwapchain(const Windowing::Window& window,
                                                ResourceDataFormat resourceDataFormat) override;
    SharedReference<ICommandAllocator> CreateCommandAllocator(CommandListType type) override;
    SharedReference<ICommandList> CreateCommandList(CommandListType type,
                                                    const SharedReference<ICommandAllocator>& allocator) override;

    SharedPointer<ICommandQueue> GetGraphicsQueue() override;
    SharedPointer<ICommandQueue> GetComputeQueue() override;
    SharedPointer<ICommandQueue> GetCopyQueue() override;

    void* NativeHandle;
    SharedPointer<ICommandQueue> GraphicsQueue;
    SharedPointer<ICommandQueue> ComputeQueue;
    SharedPointer<ICommandQueue> CopyQueue;

    SharedPointer<D3D12DescriptorHeap> RTVHeap;
    SharedPointer<D3D12DescriptorHeap> DSVHeap;
    SharedPointer<D3D12DescriptorHeap> CBV_SRV_UAV_ShaderVisibleHeap;
    SharedPointer<D3D12DescriptorHeap> CBV_SRV_UAV_Heap;
    SharedPointer<D3D12DescriptorHeap> SamplerHeap;
};

enum class D3D12DescriptorHeapType : uint8_t {
    RenderTarget = 0,
    DepthStencil = 1,
    CBV_SRV_UAV = 2,
    Sampler = 3,
};

struct AllocationRange {
    explicit AllocationRange(const uint32_t inMaxIndex) : maxIndex(inMaxIndex) {
        freeRanges.Add(FreeRange{.begin = 0, .end = inMaxIndex});
    }
    void Clear() {
        freeRanges.Clear();
        freeRanges.Add(FreeRange{.begin = 0, .end = maxIndex});
    }
    uint32_t Allocate() {
        for (int32_t i = 0; i < freeRanges.Length(); ++i) {
            if (auto& [begin, end] = freeRanges[i]; begin < end) {
                uint32_t allocatedIndex = begin;
                begin += 1;
                if (begin == end) {
                    freeRanges.RemoveAt(i);
                }
                if (allocatedIndex > maxIndex) {
                    Platform::GetPlatform().PrintMessageToDebugger(*String::Format(
                        u"AllocationRange: allocated index {} exceeds max index {}.", allocatedIndex, maxIndex));
                    return UINT32_MAX; // somehow this happened, out of bounds
                }
                return allocatedIndex;
            }
        }
        return UINT32_MAX;
    }
    void Free(uint32_t index) {
        // check if the range is out of bounds. We don't free in that case.
        if (index > maxIndex) {
            return;
        }
        for (int i = 0; i < freeRanges.Length(); ++i) {
            if (auto& range = freeRanges[i]; range.Contains(index)) {
                if (range.begin - 1 == index) {
                    range.begin--;
                } else if (range.end + 1 == index) {
                    range.end++;
                } else {
#    ifdef _DEBUG
                    //  the index is inside the free-range, which is an error
                    Platform::GetPlatform().PrintMessageToDebugger(
                        *String::Format(u"Allocation range error: double free of index {}.", index));
                    __debugbreak();
#    endif
                }

                return; // freed successfully
            }
        }
        // couldn't free the index, so we add a new free-range.
        freeRanges.Add(FreeRange{.begin = index, .end = index});
    }
    bool IsAllocated(const uint32_t index) const {
        for (int i = 0; i < freeRanges.Length(); ++i) {
            if (auto& range = freeRanges[i]; range.Contains(index)) {
                return false;
            }
        }
        return true;
    }

    uint32_t GetFreeCount() const {
        uint32_t count = 0;
        for (int i = 0; i < freeRanges.Length(); ++i) {
            count += (freeRanges[i].end - freeRanges[i].begin) + 1;
        }
        return count;
    }
    uint32_t GetAllocatedCount() const { return (maxIndex + 1) - GetFreeCount(); }

private:
    // range is [begin, end]
    struct FreeRange {
        uint32_t begin;
        uint32_t end;

        bool Contains(const uint32_t index) const { return begin >= index && end <= index; }
    };
    uint32_t maxIndex;
    Containers::List<FreeRange> freeRanges;
};

class D3D12DescriptorHeap final : public IRenderDeviceDependent {
public:
    explicit D3D12DescriptorHeap(const SharedReference<IRenderDevice>& device, D3D12DescriptorHeapType heapType,
                                 int32_t count, bool shaderVisible = false);
    ~D3D12DescriptorHeap() override;

    D3D12DescriptorHeapAllocation AllocateDescriptor();
    void FreeDescriptor(const D3D12DescriptorHeapAllocation& allocation);
    float GetUsedRate() const {
        const uint32_t allocated = allocationRange.GetAllocatedCount();
        const uint32_t total = allocated + allocationRange.GetFreeCount();
        if (total == 0) {
            return 0.0f;
        }
        return (static_cast<float>(allocated) / static_cast<float>(total));
    }
    bool IsValidDescriptorAllocation(const D3D12DescriptorHeapAllocation& allocation) const;

private:
    D3D12DescriptorHeapType type;
    void* nativeHeapPtr = nullptr;

    uint32_t descriptorIncreaseSize;
    void* cpuHeapStartPtr = nullptr;
    void* gpuHeapStartPtr = nullptr;

    AllocationRange allocationRange;
    friend class D3D12DescriptorHeapAllocation;
};

class D3D12DescriptorHeapAllocation {
public:
    bool IsValid() const {
        return ParentHeap.IsValid() && UINT32_MAX != DescriptorIndex &&
               ParentHeap.Lock()->IsValidDescriptorAllocation(*this);
    }

    inline void* GetCPUHandle() const;
    inline void* GetGPUHandle() const;

    void Free() {
        if (const auto parent = ParentHeap.Lock()) {
            parent->FreeDescriptor(*this);
            DescriptorIndex = UINT32_MAX;
        }
        ParentHeap = nullptr;
        DescriptorIndex = UINT32_MAX;
    }

private:
    WeakPointer<D3D12DescriptorHeap> ParentHeap;
    uint32_t DescriptorIndex =
        UINT32_MAX; // The index used to get the cpu handle, by default uint32_max which is invalid.
    friend class D3D12DescriptorHeap;
};
} // namespace Edvar::Renderer::RHI::D3D12

#endif