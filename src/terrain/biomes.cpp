#include "biomes.h"
#include "../noise/noise.h"
#include "../noise/fractal.h"

namespace Biomes {


    float getCaveHeight(int x, int y, int z) {
        float factor = 25.f;
        return perlinNoise3D(glm::vec3(float(x / factor), float(y / factor), float(z / factor)));
    }

    //  Use perlin noise effects Height range [128,170]
    int getGrasslandHeight(int x, int z) {
        // xzScaleFactor - in a certain biome, control the smooth of terrain
        float xzScaleFactor = 0.012f;
        //float noise = 0.5f + 0.5f * perlinNoise(x * xzScaleFactor, z * xzScaleFactor);
        float noise = 0.5f + 0.5f * perlinNoise(x * xzScaleFactor, z* xzScaleFactor);

        int heightRange = 170 - 128; // Range of the target height interval
        int heightOffset = static_cast<int>(noise * heightRange);

        return 128 + heightOffset - 7; // Offset to match the lower bound of the target interval
    }

    // Use fractal_noise(abs(Perlin(x, z))) Height range [150,255]
    int getMountainHeight(int x, int z) {
        float xzScaleFactor = 0.05f;

        float fract_noise = fractal_noise(x * xzScaleFactor, z * xzScaleFactor, 8, 0.6f);
        float noise = 0.5f + 0.5f * fract_noise;

        int heightRange = 255 - 150; // Range of the target height interval
        int heightOffset = static_cast<int>(noise * heightRange);

        heightOffset = glm::smoothstep(20.f, 30.f, noise * heightRange) * 0.01 + heightOffset * 0.99;

        return 150 + heightOffset + 20; // Offset to match the lower bound of the target interval
    }

    int getSnowHeight(int x, int z) {
        // xzScaleFactor - in a certain biome, control the smooth of terrain
        float xzScaleFactor = 0.012f;
        //float noise = 0.5f + 0.5f * perlinNoise(x * xzScaleFactor, z * xzScaleFactor);
        float noise = 0.5f + 0.5f * perlinNoise(x * xzScaleFactor, z* xzScaleFactor);

        int heightRange = 170 - 128; // Range of the target height interval
        int heightOffset = static_cast<int>(noise * heightRange);

        return 128 + heightOffset - 7; // Offset to match the lower bound of the target interval
    }

    int getDesertHeight(int x, int z) {
        // xzScaleFactor - in a certain biome, control the smooth of terrain
        float xzScaleFactor = 0.013f;
        float noise = 0.5f + 0.5f * fractal_noise(x * xzScaleFactor, z * xzScaleFactor, 4, 0.2f);

        int heightRange = 160 - 128; // Range of the target height interval
        int heightOffset = static_cast<int>(noise * heightRange);

        return 128 + heightOffset; // Offset to match the lower bound of the target interval
    }

    float getMoisture(int x, int z) {

        return perlinNoise(x * 0.002f + 0.001, z * 0.003f+ 0.001);
    }

    float getTemperature(int x, int z) {

        return perlinNoise(x * 0.001f + 101.001, z * 0.003f + 1.001);
    }

    // low temp low mois Mountain
    // low temp high mois Snow
    // high temp low mois Desert
    // high temp high mois Grassland
    BiomeType determineBiome(float moisture, float temperature) {
        if (temperature < 0.5f && moisture < 0.5f) {
            return BiomeType::Mountain;
        }else if (temperature > 0.5f && moisture < 0.5f) {
            return BiomeType::Desert;
        } else if (temperature > 0.5f && moisture > 0.5f) {
            return BiomeType::Grassland;
        }else{
            return BiomeType::Snow;
        }
    }

    // Use interpolation between biomes
    BiomeInfo getBiomeInterpolation(int x, int z) {
        int grasslandHeight = getGrasslandHeight(x, z);
        int mountainHeight = getMountainHeight(x, z);
        int snowHeight = getSnowHeight(x, z);
        int desertHeight = getDesertHeight(x, z);

//        // Noise to control biome height
//        float biomeHeightNoise = perlinNoise(x * 0.005f, z * 0.005f);
//        float remappedBiomeNoise = (biomeHeightNoise + 1.0f) * 0.5f; // Remap from [-1, 1] to [0, 1]
//        float biomeHeightBlend = glm::smoothstep(0.50f, 0.60f, remappedBiomeNoise);

        // Noise to control biome moisture and temperature
        float temperature = getTemperature(x, z);
        float moisture = getMoisture(x, z);

        float remappedTemperature = (temperature + 1.0f) * 0.5f;
        float remappedMoisture = (moisture + 1.0f) * 0.5f;

        BiomeType biome = determineBiome(remappedMoisture, remappedTemperature);

        // Apply smoothstep to create more defined biome borders
        float biomeBlendTemp = glm::smoothstep(0.40f, 0.60f, remappedTemperature);
        float biomeBlendMois = glm::smoothstep(0.40f, 0.60f, remappedMoisture);

//      // Use lerp to blend between the two biome heights
//      int finalHeight = static_cast<int>(glm::mix(grasslandHeight, mountainHeight, biomeHeightBlend));
        int Height1 = static_cast<int>(glm::mix(mountainHeight, desertHeight, biomeBlendTemp));
        int Height2 = static_cast<int>(glm::mix(snowHeight, grasslandHeight, biomeBlendTemp));

        int finalHeight = static_cast<int>(glm::mix(Height1, Height2, biomeBlendMois));

        return {finalHeight, biome};
    }

}
