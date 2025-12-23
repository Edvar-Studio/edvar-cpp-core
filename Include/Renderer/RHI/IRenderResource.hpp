#pragma once
#include "RenderingCore.hpp"
#include "ResourceDataFormat.hpp"

namespace Edvar::Renderer::RHI {
class IRenderResource : public IRenderDeviceDependent {
public:
    explicit IRenderResource(const SharedReference<IRenderDevice>& device) : IRenderDeviceDependent(device) {}
    /**
     * Gets the size of the resource.
     * @return the size of the resource as a 2D vector (width, height)
     */
    virtual Math::Vector2i GetSize() const = 0;
    /**
     * Gets the number of mip levels of the resource.
     * @return the number of mip levels of the resource
     */
    virtual uint32_t GetMipLevels() const = 0;
    /**
     * Gets the depth of the resource.
     * @return the depth of the resource, 0 if not applicable (e.g. 2D texture)
     */
    virtual uint32_t GetDepth() const { return 0; }
    /**
     * Gets the array size of the resource.
     * @return the array size of the resource, 0 if not applicable (e.g. non-array texture)
     */
    virtual uint32_t GetArraySize() const { return 0; }
    /**
     * Gets the number of dimensions of the resource.
     * @return the number of dimensions of the resource (1D, 2D, 3D)
     */
    virtual uint32_t GetDimensionsCount() const = 0;
    /**
     * Gets the sample count for multi-sampled resources.
     * @return the number of samples for multi-sampled resources.
     */
    virtual uint32_t GetSampleCount() const { return 0; }
    /**
     * Gets the data format of the resource.
     * @return the data format of the resource
     */
    virtual ResourceDataFormat GetFormat() const = 0;

    /**
     * @return if the resource allows a depth-stencil creation for this.
     */
    virtual bool IsDepthStencilAllowed() const { return false; }
    /**
     * @return if the resource allows a render-target creation for this.
     */
    virtual bool IsRenderTargetAllowed() const { return false; }
};
} // namespace Edvar::Rendering