#pragma once

namespace Edvar::Math {
namespace Random {
int64_t GetRandomInt64(uint64_t min = 0, uint64_t max = UINT64_MAX,
                       Utils::Optional<uint32_t> seed = Utils::Optional<uint32_t>());

uint64_t GetRandomUInt64(uint64_t min = 0, uint64_t max = UINT64_MAX,
                         Utils::Optional<uint32_t> seed = Utils::Optional<uint32_t>());

int32_t GetRandomInt32(int32_t min = 0, int32_t max = INT32_MAX,
                       Utils::Optional<uint32_t> seed = Utils::Optional<uint32_t>());
uint32_t GetRandomUInt32(uint32_t min = 0, uint32_t max = UINT32_MAX,
                         Utils::Optional<uint32_t> seed = Utils::Optional<uint32_t>());

double GetRandomDouble(double min = 0.0, double max = 1.0,
                       Utils::Optional<uint32_t> seed = Utils::Optional<uint32_t>());
float GetRandomFloat(float min = 0.0f, float max = 1.0f, Utils::Optional<uint32_t> seed = Utils::Optional<uint32_t>());

bool GetRandomBool(float trueProbability = 0.5f, Utils::Optional<uint32_t> seed = Utils::Optional<uint32_t>()) {
    float randomValue = GetRandomFloat(0.0f, 1.0f, seed);
    return randomValue < trueProbability;
}
} // namespace Random
} // namespace Edvar::Math