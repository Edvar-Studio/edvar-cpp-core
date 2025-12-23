#include "Math/Random.hpp"
#include <random>

namespace Edvar::Math::Random {
[[maybe_unused]] static struct GeneratorWrapper {
    GeneratorWrapper() {
        /*init the generator in static initialization*/
        [[maybe_unused]] std::mt19937_64 _ = GetGenerator();
    }
    static std::mt19937_64& GetGenerator(const Utils::Optional<uint32_t>& seed = {}) {
        if (seed.HasValue()) [[unlikely]] {
            generator.seed(seed.Get());
        } else {
            static bool isSeeded = false;
            if (!isSeeded) [[unlikely]] {
                std::random_device rd;
                generator.seed(rd());
                isSeeded = true;
            }
        }
        return generator;
    };

private:
    static thread_local std::mt19937_64 generator;
} _initGeneratorWrapper;
thread_local std::mt19937_64 GeneratorWrapper::generator;
int64_t GetRandomInt64(const uint64_t min, const uint64_t max, Utils::Optional<uint32_t> seed) {
    if (seed.HasValue()) {
        GeneratorWrapper::GetGenerator().seed(seed.Get());
    } else {
        static bool seeded = false;
        if (!seeded) {
            std::random_device rd;
            GeneratorWrapper::GetGenerator().seed(rd());
            seeded = true;
        }
    }
    std::uniform_int_distribution<int64_t> distribution(static_cast<int64_t>(min), static_cast<int64_t>(max));
    return distribution(GeneratorWrapper::GetGenerator());
}

uint64_t GetRandomUInt64(const uint64_t min, const uint64_t max, Utils::Optional<uint32_t> seed) {
    if (seed.HasValue()) {
        GeneratorWrapper::GetGenerator().seed(seed.Get());
    } else {
        static bool seeded = false;
        if (!seeded) {
            std::random_device rd;
            GeneratorWrapper::GetGenerator().seed(rd());
            seeded = true;
        }
    }
    std::uniform_int_distribution<uint64_t> distribution(min, max);
    return distribution(GeneratorWrapper::GetGenerator());
}

int32_t GetRandomInt32(const int32_t min, const int32_t max, const Utils::Optional<uint32_t>& seed) {
    const int64_t result = GetRandomInt64(static_cast<uint64_t>(min), static_cast<uint64_t>(max), seed);
    return static_cast<int32_t>(result);
}

uint32_t GetRandomUInt32(const uint32_t min, const uint32_t max, const Utils::Optional<uint32_t>& seed) {
    const uint64_t result = GetRandomUInt64(static_cast<uint64_t>(min), static_cast<uint64_t>(max), seed);
    return static_cast<uint32_t>(result);
}

double GetRandomDouble(const double min, const double max, Utils::Optional<uint32_t> seed) {
    if (seed.HasValue()) {
        GeneratorWrapper::GetGenerator().seed(seed.Get());
    } else {
        static bool seeded = false;
        if (!seeded) {
            std::random_device rd;
            GeneratorWrapper::GetGenerator().seed(rd());
            seeded = true;
        }
    }
    std::uniform_real_distribution<double> distribution(min, max);
    return distribution(GeneratorWrapper::GetGenerator());
}
float GetRandomFloat(const float min, const float max, const Utils::Optional<uint32_t>& seed) {
    const double result = GetRandomDouble(static_cast<double>(min), static_cast<double>(max), seed);
    return static_cast<float>(result);
}
} // namespace Edvar::Math::Random