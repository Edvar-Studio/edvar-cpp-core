#include "Threading/Mutex.hpp"
#include "Platform/IPlatformThreading.hpp"

namespace Edvar::Threading {
Mutex::Mutex() { MutexImplementation = &Platform::GetPlatform().GetThreading().CreateMutex(); }
Mutex::~Mutex() { delete MutexImplementation; }

void Mutex::Lock() { MutexImplementation->Lock(); }
void Mutex::Release() { MutexImplementation->Release(); }
} // namespace Edvar::Threading