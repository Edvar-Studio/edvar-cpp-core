#pragma once

namespace Edvar::Rendering {
enum class ResourceDataFormat : int32_t {
    Unknown,
    /**
     * 32-bit RGBA unsigned normalized format (8 bits per channel)
     * Generally used for color textures.
     */
    R8G8B8A8_UNorm = 28,
    /**
     * 32-bit RGBA signed normalized format (8 bits per channel)
     * Generally used for normal maps.
     */
    R8G8B8A8_SNorm = 31,
    /**
     * 32-bit RGBA unsigned integer format (8 bits per channel)
     * Generally used for data textures.
     */
    R8G8B8A8_UInt = 30,
    /**
     * 32-bit RGBA signed integer format (8 bits per channel)
     * Generally used for data textures.
     */
    R8G8B8A8_SInt = 32,
    /**
     * 32-bit BGRA unsigned normalized format (8 bits per channel)
     * Commonly used for swapchain back buffers.
     */
    B8G8R8A8_UNorm = 87,
    /**
     * 32-bit BGRA unsigned normalized sRGB format (8 bits per channel)
     * Commonly used for swapchain back buffers with sRGB color space.
     */
    B8G8R8A8_UNorm_SRGB = 91,
    /**
     * 64-bit RGBA floating-point format (16 bits per channel)
     * Used for high dynamic range (HDR) rendering.
     */
    R16G16B16A16_Float = 10,
    /**
     * 128-bit RGBA floating-point format (32 bits per channel)
     * Used for high precision calculations.
     */
    R32G32B32A32_Float = 2,
    /**
     * 24-bit depth and 8-bit stencil format
     * Commonly used for depth-stencil buffers.
     */
    D24_UNorm_S8_UInt = 45,
    /**
     * 32-bit floating-point depth format
     * Used for high precision depth buffers.
     */
    D32_Float = 40,

};
}