#pragma once
#include "IRenderResource.hpp"
#include "ResourceDataFormat.hpp"

namespace Edvar::Renderer::RHI {
class IDepthStencil2DView;
}
namespace Edvar::Renderer::RHI {
class ITextureView;
class EDVAR_CPP_CORE_API ITexture : public IRenderResource {
public:
    explicit ITexture(const SharedReference<IRenderDevice>& device, const ResourceDataFormat inFormat,
                      const Math::Vector2i& inSize, const uint32_t inMipLevels = 0, const uint32_t sampleCount = 0)
        : IRenderResource(device), size(inSize), mipLevels(inMipLevels), sampleCount(sampleCount), format(inFormat) {}
    ~ITexture() override { ownedViews.Clear(); }

    Math::Vector2i GetSize() const override { return size; }
    uint32_t GetMipLevels() const override { return mipLevels; }
    uint32_t GetSampleCount() const override { return sampleCount; }
    ResourceDataFormat GetFormat() const override { return format; }

    virtual void* GetNativeHandle() const = 0;

private:
    Math::Vector2i size;
    uint32_t mipLevels;
    uint32_t sampleCount;
    ResourceDataFormat format;
    friend class ITextureView;

protected:
    Containers::List<SharedReference<ITextureView>> ownedViews;
};

class EDVAR_CPP_CORE_API ITexture2D : public ITexture {
public:
    explicit ITexture2D(const SharedReference<IRenderDevice>& device, const ResourceDataFormat inFormat,
                        const Math::Vector2i& inSize, const uint32_t inMipLevels = 0, const uint32_t sampleCount = 0)
        : ITexture(device, inFormat, inSize, inMipLevels, sampleCount) {}

    uint32_t GetDimensionsCount() const override { return 2; }
    virtual WeakPointer<ITextureView> CreateView(const ResourceDataFormat& withResourceFormat,
                                                 uint32_t withMipLevel) = 0;
    virtual WeakPointer<IDepthStencil2DView> CreateDepthStencilView(const ResourceDataFormat& withResourceFormat,
                                                                    uint32_t withMipLevel) = 0;
};

} // namespace Edvar::Renderer::RHI