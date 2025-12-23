#pragma once
#include "ITextureView.hpp"

namespace Edvar::Renderer::RHI {
enum class DepthStencilViewType : uint32_t {
    None = 0,
    ReadOnlyDepth = 0x1,
    ReadOnlyStencil = 0x2,
    ReadOnlyDepthStencil = ReadOnlyDepth | ReadOnlyStencil,
};
EDVAR_CPP_CORE_ENUM_FLAG_OPERATORS(DepthStencilViewType);
class IDepthStencil : public ITextureView {
protected:
    IDepthStencil(const SharedPointer<ITexture>& texture, const ResourceDataFormat& withFormat,
                      const uint32_t& withMipSlice, const uint32_t& withPlaneSlice,
                      DepthStencilViewType withType = DepthStencilViewType::None)
        : ITextureView(texture, withFormat, withMipSlice, withPlaneSlice), type(withType) {}

public:
    DepthStencilViewType GetType() const { return type; }

private:
    DepthStencilViewType type;
};

class IDepthStencil2DView : public IDepthStencil {
public:
    IDepthStencil2DView(const SharedPointer<ITexture>& texture, const ResourceDataFormat& withFormat,
                        const uint32_t& withMipSlice, const uint32_t& withPlaneSlice, DepthStencilViewType withType)
        : IDepthStencil(texture, withFormat, withMipSlice, withPlaneSlice, withType) {}
};
} // namespace Edvar::Renderer::RHI