#pragma once

namespace Edvar::Math::Random {
EDVAR_CPP_CORE_API int64_t GetRandomInt64(uint64_t min = 0, uint64_t max = UINT64_MAX,
                                          Utils::Optional<uint32_t> seed = Utils::Optional<uint32_t>());

EDVAR_CPP_CORE_API uint64_t GetRandomUInt64(uint64_t min = 0, uint64_t max = UINT64_MAX,
                                            Utils::Optional<uint32_t> seed = Utils::Optional<uint32_t>());

EDVAR_CPP_CORE_API int32_t GetRandomInt32(int32_t min = 0, int32_t max = INT32_MAX,
                                          const Utils::Optional<uint32_t>& seed = Utils::Optional<uint32_t>());
EDVAR_CPP_CORE_API uint32_t GetRandomUInt32(uint32_t min = 0, uint32_t max = UINT32_MAX,
                                            const Utils::Optional<uint32_t>& seed = Utils::Optional<uint32_t>());

EDVAR_CPP_CORE_API double GetRandomDouble(double min = 0.0, double max = 1.0,
                                          Utils::Optional<uint32_t> seed = Utils::Optional<uint32_t>());
EDVAR_CPP_CORE_API float GetRandomFloat(float min = 0.0f, float max = 1.0f,
                                        const Utils::Optional<uint32_t>& seed = Utils::Optional<uint32_t>());

inline bool GetRandomBool(const float trueProbability = 0.5f,
                          const Utils::Optional<uint32_t>& seed = Utils::Optional<uint32_t>()) {
    const float randomValue = GetRandomFloat(0.0f, 1.0f, seed);
    return randomValue < trueProbability;
}
} // namespace Edvar::Math::Random
