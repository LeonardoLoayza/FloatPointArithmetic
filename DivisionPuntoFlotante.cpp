#include <iostream>
#include <cstdint>
#include <cstring>
#include <cmath>

void printFloatBits(float num) {
    uint32_t bits;
    std::memcpy(&bits, &num, sizeof(float)); 

    uint32_t sign = (bits >> 31) & 1;              
    uint32_t exponent = (bits >> 23) & 0xFF;       
    uint32_t significand = bits & 0x7FFFFF;        

    std::cout << "Sign: " << sign << ", Exponent: ";
    for (int i = 7; i >= 0; i--) {
        std::cout << ((exponent >> i) & 1);
    }
    std::cout << ", Significand: ";
    for (int i = 22; i >= 0; i--) {
        std::cout << ((significand >> i) & 1);
    }
    std::cout << std::endl;
}

float divideFloat(float x, float y) {
    if (x == 0.0f) {
        return 0.0f;
    }
    if (y == 0.0f) {
        return std::numeric_limits<float>::infinity();
    }

    uint32_t bitsX, bitsY, bitsZ;
    std::memcpy(&bitsX, &x, sizeof(float));
    std::memcpy(&bitsY, &y, sizeof(float));

    uint32_t signX = (bitsX >> 31) & 1;
    uint32_t exponentX = (bitsX >> 23) & 0xFF;
    uint32_t significandX = bitsX & 0x7FFFFF;

    uint32_t signY = (bitsY >> 31) & 1;
    uint32_t exponentY = (bitsY >> 23) & 0xFF;
    uint32_t significandY = bitsY & 0x7FFFFF;

    uint32_t signZ = signX ^ signY;

    int32_t exponentZ = (int32_t)exponentX - (int32_t)exponentY + 127;

    if (exponentZ > 255) {
        std::cerr << "Overflow detected!" << std::endl;
        return std::numeric_limits<float>::infinity();
    }
    if (exponentZ < 0) {
        std::cerr << "Underflow detected!" << std::endl;
        return 0.0f;
    }

    float significandX_f = 1.0f + (significandX / (float)(1 << 23));
    float significandY_f = 1.0f + (significandY / (float)(1 << 23));
    float significandZ = significandX_f / significandY_f;

    int adjust = 0;
    while (significandZ >= 2.0f) {
        significandZ /= 2.0f;
        adjust++;
    }
    while (significandZ < 1.0f && significandZ != 0.0f) {
        significandZ *= 2.0f;
        adjust--;
    }
    exponentZ += adjust;

    if (exponentZ > 255) {
        std::cerr << "Overflow detected after normalization!" << std::endl;
        return std::numeric_limits<float>::infinity();
    }
    if (exponentZ < 0) {
        std::cerr << "Underflow detected after normalization!" << std::endl;
        return 0.0f;
    }

    significandZ -= 1.0f;
    uint32_t significandZ_bits = (uint32_t)(significandZ * (1 << 23));

    bitsZ = (signZ << 31) | ((uint32_t)exponentZ << 23) | significandZ_bits;

    float result;
    std::memcpy(&result, &bitsZ, sizeof(float));
    return result;
}

int main() {
    float x = 5.5f;
    float y = 2.0f;

    std::cout << "Dividing " << x << " by " << y << std::endl;
    std::cout << "X bits: ";
    printFloatBits(x);
    std::cout << "Y bits: ";
    printFloatBits(y);

    float result = divideFloat(x, y);
    std::cout << "Result: " << result << std::endl;
    std::cout << "Result bits: ";
    printFloatBits(result);

    float expected = x / y;
    std::cout << "Expected (standard division): " << expected << std::endl;
    std::cout << "Expected bits: ";
    printFloatBits(expected);

    return 0;
}