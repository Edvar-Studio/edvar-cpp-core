#pragma once
#include "ITexture.hpp"
namespace Edvar::Renderer::RHI {
class EDVAR_CPP_CORE_API ITextureView {
protected:
    ITextureView(const SharedPointer<ITexture>& texture, const ResourceDataFormat& withFormat,
                 const uint32_t withMipSlice, const uint32_t withPlaneSlice)
        : weakTexture(texture), format(withFormat), mipSlice(withMipSlice), planeSlice(withPlaneSlice) {}

public:
    bool IsValid() const { return weakTexture.IsValid(); }
    virtual SharedPointer<ITexture> GetTexture() const { return weakTexture.Lock(); }
    virtual ResourceDataFormat GetFormat() const { return format; }
    virtual uint32_t GetMipSlice() const { return mipSlice; }
    virtual uint32_t GetPlaneSlice() const { return planeSlice; }

    virtual void Destroy() {
        if (weakTexture.IsValid()) {
            for (auto Iterator = GetTexture()->ownedViews.GetIterator(); Iterator.HasNext(); ++Iterator) {
                if (Iterator->Get() == this) {
                    Iterator.Remove();
                    break;
                }
            }
        }
        weakTexture = nullptr;
    }

    virtual ~ITextureView() { ITextureView::Destroy(); }

private:
    WeakPointer<ITexture> weakTexture;
    ResourceDataFormat format;
    uint32_t mipSlice;
    uint32_t planeSlice;
};
} // namespace Edvar::Renderer::RHI
