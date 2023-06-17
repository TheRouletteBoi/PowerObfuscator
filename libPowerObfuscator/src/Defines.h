#pragma once

// So PowerObfuscator symbols/functions don't show up in the PowerObfuscatorGUI App
#define POBF_API /* __attribute__((visibility("hidden"))) GCC Only */

template<int A, int C, int M>
class pobf_Random
{
public:
    pobf_Random(uint32_t seed) : m_seed(seed) {}

    uint32_t Next()
    {
        return m_seed = (A * m_seed + C) % M;
    }

private:
    uint32_t m_seed;
};