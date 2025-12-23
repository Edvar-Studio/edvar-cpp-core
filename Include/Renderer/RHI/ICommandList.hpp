#pragma once
#include "IBuffer.hpp"
#include "ICommandQueue.hpp"
#include "RenderingCore.hpp"
#include "ResourceDataFormat.hpp"

namespace Edvar::Renderer::RHI {
class IDepthStencil;
}
namespace Edvar::Renderer::RHI {
class ITexture;
}
namespace Edvar::Renderer::RHI {
class IRenderTarget;
}
namespace Edvar::Renderer::RHI {
class IRenderResource;
}
namespace Edvar::Renderer {
struct Viewport;
}
namespace Edvar::Renderer::RHI {
class ICommandAllocator;

struct IResourceBarrier {
    enum class BarrierType { Transition, Aliasing, UAV } type;

protected:
    IResourceBarrier(IResourceBarrier::BarrierType type) : type(type) {}
};

struct TransitionResourceBarrier : public IResourceBarrier {
    explicit TransitionResourceBarrier(IRenderResource& forResource)
        : IResourceBarrier(BarrierType::Transition), resource(&forResource), oldStateValue(ResourceState::Common),
          newStateValue(ResourceState::Common) {};
    IRenderResource* resource;
    enum class ResourceState : uint32_t {
        Common = 0,
        VertexAndConstantBuffer = 1 << 0,
        IndexBuffer = 1 << 2,
        RenderTarget = 1 << 3,
        UnorderedAccess = 1 << 4,
        DepthWrite = 1 << 4,
        DepthRead = 1 << 5,
        NonPixelShaderResource = 1 << 6,
        PixelShaderResource = 1 << 7,
        CopyDest = 1 << 8,
        CopySource = 1 << 9,
        ResolveDest = 1 << 10,
        ResolveSource = 1 << 11,
        GenericRead = 1 << 12,
        Present = Common,
        Predication = 1 << 13,
    };
    TransitionResourceBarrier& OldState(const ResourceState state) {
        oldStateValue = state;
        return *this;
    }
    TransitionResourceBarrier& NewState(const ResourceState state) {
        newStateValue = state;
        return *this;
    }
    ResourceState oldStateValue;
    ResourceState newStateValue;
};

struct AliasingResourceBarrier : public IResourceBarrier {
    explicit AliasingResourceBarrier(IRenderResource& beforeResource, IRenderResource& afterResource)
        : IResourceBarrier(BarrierType::Aliasing), resourceBefore(&beforeResource), resourceAfter(&afterResource) {}
    AliasingResourceBarrier& BeforeResource(IRenderResource& resource) {
        resourceBefore = &resource;
        return *this;
    }
    AliasingResourceBarrier& AfterResource(IRenderResource& resource) {
        resourceAfter = &resource;
        return *this;
    }
    IRenderResource* resourceBefore;
    IRenderResource* resourceAfter;
};

struct UAVResourceBarrier : public IResourceBarrier {
    explicit UAVResourceBarrier(IRenderResource& forResource)
        : IResourceBarrier(BarrierType::UAV), resource(forResource) {}
    IRenderResource& resource;
};

enum class TopologyType : uint8_t { TriangleList, TriangleStrip, LineList, LineStrip, PointList };

struct ITextureCopyLocation {
protected:
    ITextureCopyLocation(ITexture& inTexture, bool inIsIndex) : texture(&inTexture), isIndex(inIsIndex) {}

public:
    ITexture* texture;
    bool isIndex;
};

struct SubresourceFootprintTextureCopyLocation final : ITextureCopyLocation {
    SubresourceFootprintTextureCopyLocation(ITexture& inTexture, const ResourceDataFormat inFormat,
                                            const uint32_t inWidth, const uint32_t inHeight, const uint32_t inDepth,
                                            const uint32_t inRowPitch = 0, const uint64_t inOffset = 0)
        : ITextureCopyLocation(inTexture, false), offset(inOffset), format(inFormat), width(inWidth), height(inHeight),
          depth(inDepth), rowPitch(inRowPitch) {}
    uint64_t offset;
    ResourceDataFormat format;
    uint32_t width, height, depth;
    uint32_t rowPitch;
};

struct SubresourceIndexedTextureCopyLocation final : public ITextureCopyLocation {
    explicit SubresourceIndexedTextureCopyLocation(ITexture& inTexture, const uint32_t inSubresourceIndex)
        : ITextureCopyLocation(inTexture, true), subresourceIndex(inSubresourceIndex) {}
    uint32_t subresourceIndex;
};
enum class ShadingRateCombiner : uint8_t { PassThrough = 0, Override = 1, Min = 2, Max = 3, Sum = 4 };
// TODO: automatically set descriptor heaps in d3d12 command list implementation
class ICommandList : public IRenderDeviceDependent {
public:
    explicit ICommandList(const SharedReference<IRenderDevice>& device, const CommandListType inType,
                          const SharedReference<ICommandAllocator>& inCommandAllocator)
        : IRenderDeviceDependent(device), commandListType(inType), commandAllocator(inCommandAllocator) {}

    virtual void Close() = 0;
    virtual void Reset() = 0;
    virtual void Execute(ICommandQueue& commandQueue) = 0;
    virtual void ExecuteIndirect(ICommandSignature& commandSignature, uint32_t maxCommandCount,
                                 IRenderResource* argumentBuffer, uint32_t argumentBufferOffset,
                                 IRenderResource* countBuffer, uint64_t countBufferOffset);
    // State setting operations.
    virtual void SetViewport(const Viewport& viewport) = 0;
    virtual void SetScissorRect(const Math::Rectanglei& rect) = 0;
    virtual void SetRenderTargets(uint32_t numRenderTargets, const IRenderTarget* const* renderTargets,
                                  const IDepthStencil* depthStencilView) = 0;
    virtual void SetStencilReferenceValue(uint32_t referenceValue) = 0;
    virtual void SetPipelineState(IPipelineState& pipelineState) = 0;

    // Parameter operations.
    void Set32BitConstant(uint32_t slotIndex, uint32_t data, uint32_t destOffset);
    void Set32BitConstants(uint32_t slotIndex, const uint32_t* data, uint32_t destOffset, uint32_t count);
    void SetBuffer(uint32_t slotIndex, const IBuffer& buffer);
    void SetShaderResourceView(uint32_t slotIndex, const IShaderResource& bufferView);

    // Drawing operations.
    virtual void SetTopology(TopologyType topology) = 0;
    virtual void SetIndexBuffer(const IBuffer* indexBuffer, uint32_t size, uint32_t stride) = 0;
    virtual void SetVertexBuffers(uint32_t startSlot, uint32_t numBuffers, const IBuffer* const* vertexBuffers,
                                  const uint32_t* sizes, const uint32_t* strides) = 0;
    virtual void DrawInstanced(uint32_t vertexCountPerInstance, uint32_t instanceCount, uint32_t startVertex,
                               uint32_t startInstance) = 0;
    virtual void DrawIndexedInstanced(uint32_t indexCountPerInstance, uint32_t instanceCount, uint32_t startIndex,
                                      int32_t baseVertex, uint32_t startInstance) = 0;

    // Render pass operations.
    // TODO: Expand the render pass parameters to support more options.
    virtual void BeginRenderPass(uint32_t numRenderTargets, IRenderTarget* const* renderTargets,
                                 IDepthStencil* depthStencilView, bool allowWritesToUnorderedAccessView) = 0;
    virtual void EndRenderPass() = 0;

    // TODO: Ray tracing operations.

    // TODO: Variable rate shading operations.
    void SetVariableRateShadingRate(uint8_t vertical, uint8_t horizontal, ShadingRateCombiner postRasterizerCombiner,
                                    ShadingRateCombiner finalRateCombiner);
    void SetVariableRateShadingRateImage(IRenderResource* shadingRateImage);

    // Resource operations.
    virtual void ResourceBarrier(const IResourceBarrier& barrier) = 0;
    virtual void CopyBufferRegion(const IRenderResource& destBuffer, uint64_t destOffset,
                                  const IRenderResource& srcBuffer, uint64_t srcOffset, uint64_t numBytes) = 0;
    virtual void CopyTextureRegion(ITextureCopyLocation& destination, uint32_t destX, uint32_t destY, uint32_t destZ,
                                   ITextureCopyLocation& source, const Utils::Optional<Math::Boxi>& sourceBox) = 0;
    virtual void CopyResource(IRenderResource& destResource, IRenderResource& srcResource) = 0;
    enum class SubresourceResolveMode : uint8_t { Decompress, Min, Max, Average };
    virtual void ResolveSubresourceRegion(IRenderResource& destResource, uint32_t destSubresourceIndex, uint32_t destX,
                                          uint32_t destY, IRenderResource& srcResource, uint32_t srcSubresourceIndex,
                                          const Math::Rectanglei* sourceRect, ResourceDataFormat format,
                                          SubresourceResolveMode mode) = 0;

    virtual void ClearRenderTarget(IRenderTarget& renderTarget, const Math::Color& clearColor, uint32_t numRects,
                                   const Math::Rectanglei* rects) = 0;
    virtual void ClearDepthStencilView(IDepthStencil& depthStencil, float depth, bool clearDepth, bool clearStencil,
                                       uint8_t stencilClearValue, uint32_t numRects, const Math::Rectanglei* rects) = 0;
    virtual void DiscardResource(IRenderResource& resource, uint32_t numRects, const Math::Rectanglei* rects,
                                 uint32_t firstSubresource, uint32_t numSubresource) = 0;



private:
    CommandListType commandListType;
    SharedReference<ICommandAllocator> commandAllocator;
};
} // namespace Edvar::Renderer::RHI