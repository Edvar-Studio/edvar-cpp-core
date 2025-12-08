#include "Rendering/RenderingCore.hpp"

namespace Edvar::Rendering {
SharedPointer<IRenderingAPI> IRenderingAPI::ActiveAPI = SharedPointer<IRenderingAPI>(nullptr);
}