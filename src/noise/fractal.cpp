#include "fractal.h"
#include <iostream>

// fractal noise based on PerlinNoise
float fractal_noise(float x, float z, int octaves, float persistence) {
    float total = 0;
    float frequency = 1;
    float amplitude = 1;
    float maxValue = 0;

    for(int i = 0; i < octaves; ++i) {
        float scaledX = x * frequency + 0.5f;
        float scaledZ = z * frequency + 0.5f;

        float perlinValue = perlinNoise(scaledX, scaledZ);
        total += perlinValue * amplitude;

        maxValue += amplitude;

        amplitude *= persistence;
        frequency *= 2;

    }


    return total/maxValue;
}
