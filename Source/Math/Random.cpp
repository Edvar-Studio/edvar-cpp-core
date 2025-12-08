#include "Math/Random.hpp"
#include <cstdint>
#include <random>

namespace Edvar::Math::Random {
int64_t GetRandomInt64(uint64_t min, uint64_t max, Utils::Optional<uint32_t> seed) {
    static thread_local std::mt19937_64 generator;
    if (seed.HasValue()) {
        generator.seed(seed.Get());
    } else {
        static bool seeded = false;
        if (!seeded) {
            std::random_device rd;
            generator.seed(rd());
            seeded = true;
        }
    }
    std::uniform_int_distribution<int64_t> distribution(static_cast<int64_t>(min), static_cast<int64_t>(max));
    return distribution(generator);
}

uint64_t GetRandomUInt64(uint64_t min, uint64_t max, Utils::Optional<uint32_t> seed) {
    static thread_local std::mt19937_64 generator;
    if (seed.HasValue()) {
        generator.seed(seed.Get());
    } else {
        static bool seeded = false;
        if (!seeded) {
            std::random_device rd;
            generator.seed(rd());
            seeded = true;
        }
    }
    std::uniform_int_distribution<uint64_t> distribution(min, max);
    return distribution(generator);
}

int32_t GetRandomInt32(int32_t min, int32_t max, Utils::Optional<uint32_t> seed) {
    int64_t result = GetRandomInt64(static_cast<uint64_t>(min), static_cast<uint64_t>(max), seed);
    return static_cast<int32_t>(result);
}

uint32_t GetRandomUInt32(uint32_t min, uint32_t max, Utils::Optional<uint32_t> seed) {
    uint64_t result = GetRandomUInt64(static_cast<uint64_t>(min), static_cast<uint64_t>(max), seed);
    return static_cast<uint32_t>(result);
}

double GetRandomDouble(double min, double max, Utils::Optional<uint32_t> seed) {
    static thread_local std::mt19937_64 generator;
    if (seed.HasValue()) {
        generator.seed(seed.Get());
    } else {
        static bool seeded = false;
        if (!seeded) {
            std::random_device rd;
            generator.seed(rd());
            seeded = true;
        }
    }
    std::uniform_real_distribution<double> distribution(min, max);
    return distribution(generator);
}
float GetRandomFloat(float min, float max, Utils::Optional<uint32_t> seed) {
    double result = GetRandomDouble(static_cast<double>(min), static_cast<double>(max), seed);
    return static_cast<float>(result);
}
} // namespace Edvar::Math::Random