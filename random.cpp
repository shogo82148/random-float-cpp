#include <iostream>
#include <cstdint>
#include <chrono>
#include <bit>
#include <cmath>
#include <functional>

static uint64_t state = 42;

uint64_t xorshift()
{
    uint64_t x = state;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    state = x;
    return x;
}

static constexpr int bias32 = 127;
static constexpr int shift32 = 32 - 8 - 1;
static constexpr uint32_t frac_mask32 = (1 << shift32) - 1;

static constexpr int bias64 = 1023;
static constexpr int shift64 = 64 - 11 - 1;
static constexpr uint64_t frac_mask64 = (1ULL << shift64) - 1;

float randf(std::function<uint64_t()> rand)
{
    int exp = bias32 - 1;
    uint32_t frac = 0;
    for (;;)
    {
        auto i = rand();
        auto l = std::countl_zero(i);
        exp -= l;
        if (exp <= 0)
        {
            frac = rand();
            exp = 0;
            break;
        }
        if ((64 - l) > shift32)
        {
            frac = i >> ((64 - l) - shift32 - 1);
            break;
        }
        else if (l < 64)
        {
            int s = shift32 - (64 - l) + 1;
            frac = (i << s) | (rand() & ((1 << s) - 1));
        }
    }
    return std::bit_cast<float>(uint32_t(exp) << shift32 | (frac & frac_mask32));
}

double randd(std::function<uint64_t()> rand)
{
    int exp = bias64 - 1;
    uint64_t frac = 0;
    for (;;)
    {
        auto i = rand();
        auto l = std::countl_zero(i);
        exp -= l;
        if (exp <= 0)
        {
            frac = rand();
            exp = 0;
            break;
        }
        if ((64 - l) > shift64)
        {
            frac = i >> ((64 - l) - shift64 - 1);
            break;
        }
        else if (l < 64)
        {
            int s = shift64 - (64 - l) + 1;
            frac = (i << s) | (rand() & ((1ULL << s) - 1));
        }
    }
    return std::bit_cast<double>(uint64_t(exp) << shift64 | (frac & frac_mask64));
}

int main()
{
    std::chrono::system_clock::time_point start, end;

    start = std::chrono::system_clock::now();
    for (int i = 0; i < 100000000; ++i)
    {
        randd(xorshift);
    }
    end = std::chrono::system_clock::now();

    double time = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000000.0);
    std::cout << time << "s" << std::endl;
}
