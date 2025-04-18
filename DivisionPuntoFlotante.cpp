#include <iostream>
#include <cstdint>
#include <cmath>

union Float32 {
    float f;
    struct {
        uint32_t significand : 23;
        uint32_t exponent : 8;
        uint32_t sign : 1;
    } bits;
};

void printFloatBits(float num) {
    Float32 f;
    f.f = num;
    std::cout << "Sign: " << f.bits.sign << ", Exponent: ";
    for (int i = 7; i >= 0; i--) {
        std::cout << ((f.bits.exponent >> i) & 1);
    }
    std::cout << ", Significand: ";
    for (int i = 22; i >= 0; i--) {
        std::cout << ((f.bits.significand >> i) & 1);
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

    // Extract  components 
    Float32 fx, fy, fz;
    fx.f = x;
    fy.f = y;

    // Getting the sign 
    fz.bits.sign = fx.bits.sign ^ fy.bits.sign;

    // Subtractinng the exponents
    int32_t exponent = (int32_t)fx.bits.exponent - (int32_t)fy.bits.exponent + 127;

    // CCheck for overflow/underflow
    if (exponent > 255) {
        std::cerr << "Overflow detected!" << std::endl;
        return std::numeric_limits<float>::infinity();
    }
    if (exponent < 0) {
        std::cerr << "Underflow detected!" << std::endl;
        return 0.0f;
    }
    fz.bits.exponent = exponent;

    // Divide significands
    // Add the implicit leading 1
    float significandX = 1.0f + (fx.bits.significand / (float)(1 << 23));
    float significandY = 1.0f + (fy.bits.significand / (float)(1 << 23));
    float significand = significandX / significandY;

    // Normalize 
    int adjust = 0;
    while (significand >= 2.0f) {
        significand /= 2.0f;
        adjust++;
    }
    while (significand < 1.0f && significand != 0.0f) {
        significand *= 2.0f;
        adjust--;
    }
    fz.bits.exponent += adjust;

    //  overflow/underflow check 
    if (fz.bits.exponent > 255) {
        std::cerr << "Overflow detected after normalization!" << std::endl;
        return std::numeric_limits<float>::infinity();
    }
    if (fz.bits.exponent < 0) {
        std::cerr << "Underflow detected after normalization!" << std::endl;
        return 0.0f;
    }

    // Extracting the fractional part of the significand AAAAAAAAAAAAAAAAAAAAA
    significand -= 1.0f;
    fz.bits.significand = (uint32_t)(significand * (1 << 23));


    return fz.f;
}

int main() {
    // Test var iables
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

    // Verifying
    float expected = x / y;
    std::cout << "Expected (standard division): " << expected << std::endl;
    std::cout << "Expected bits: ";
    printFloatBits(expected);

    return 0;
}

// int main() {
//     float x = 5.5f;
//     float y = 2.0f;

//     printFloatBits(x);
//     printFloatBits(y);

//     printFloatBits(result);

//     // Verifying
//     float expected = x / y;
//     printFloatBits(expected);

//     return 0;
// }