#pragma once
#include "ITextureView.hpp"

namespace Edvar::Renderer::RHI {
class IRenderTarget : public ITextureView {
public:
    IRenderTarget(const SharedPointer<ITexture>& texture, const ResourceDataFormat& withFormat, uint32_t withMipSlice,
                  uint32_t withPlaneSlice)
        : ITextureView(texture, withFormat, withMipSlice, withPlaneSlice) {}
    virtual bool CanClear() const = 0;
};

class IRenderTarget2D : public IRenderTarget {
public:
    IRenderTarget2D(const SharedPointer<ITexture>& texture, const ResourceDataFormat& withFormat, uint32_t withMipSlice,
                    uint32_t withPlaneSlice)
        : IRenderTarget(texture, withFormat, withMipSlice, withPlaneSlice) {}
    virtual Math::Vector2i GetSize() = 0;
};
} // namespace Edvar::Renderer::RHI