#pragma once

namespace Biomes {
    // Biome details.
    struct BiomeInfo {
        int height;
        // Interpolation variable.
        int type;
    };

    enum BiomeType : unsigned char
    {
        Grassland, Mountain, Snow, Desert
    };

    int getGrasslandHeight(int x, int z);

    int getMountainHeight(int x, int z);

    BiomeInfo getBiomeInterpolation(int x, int z);

    float getCaveHeight(int x, int y, int z);
}
