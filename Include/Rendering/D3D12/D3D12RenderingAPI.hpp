#pragma once
#include "../RenderingCore.hpp"

#ifdef _WIN32
struct IDXGIFactory2;
namespace Edvar::Rendering::D3D12 {
extern ::IDXGIFactory2* GDXGIFactory;
class D3D12RenderingAPI final : public IRenderingAPI {
public:
    explicit D3D12RenderingAPI();
    SharedPointer<IRenderDevice> CreatePrimaryRenderDevice() override;

    bool DoesOutputSupportHDR(const Platform::MonitorInfo& monitorInfo) override;
};
#endif
} // namespace Edvar::Rendering::D3D12