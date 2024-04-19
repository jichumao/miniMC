#include "blocktypeworker.h"
#include "biomes.h"


// Construction also executes zone instantiation.
BlockTypeWorker::BlockTypeWorker(
    glm::ivec2 zoneIndex,
    OpenGLContext *context,
    TerrainWorkQueue<Chunk *> *instantiatedChunksQueue
) : mp_context(context), mp_instantiatedChunksQueue(instantiatedChunksQueue) {

    instantiateChunksForZone(zoneIndex);
}

// Creates all the chunks of the zone, sets block types according to
// biome height functions, and places finished chunks in a queue shared
// with the main Terrain thread.
void BlockTypeWorker::instantiateChunksForZone(glm::ivec2 zoneIndex) {
    int minX = zoneIndex.s, maxX = zoneIndex.s + 16*4;
    int minZ = zoneIndex.t, maxZ = zoneIndex.t + 16*4;

    for(int x = minX; x < maxX; x += 16) {
        for(int z = minZ; z < maxZ; z += 16) {
            Chunk *chunk = new Chunk(mp_context, x, z);
            instantiateBiomesFor(chunk);
            mp_instantiatedChunksQueue->push(chunk);
        }
    }
}

void BlockTypeWorker::instantiateBiomesFor(Chunk *chunk) {
    glm::ivec2 xz = chunk->getXZ();
    int x = xz.s, z = xz.t;

    // Set seed
    srand(x * 32513 + z * 23223);

    for(int dx = 0; dx < 16; ++dx) {
        for(int dz = 0; dz < 16; ++dz) {

            int worldX = x + dx;
            int worldZ = z + dz;

            for (int y = 0; y < 128; ++y) {
                setBlockAt(chunk, worldX, y, worldZ, STONE);
            }

            // Determine the biome and fill accordingly.
            Biomes::BiomeInfo biomeInfo = Biomes::getBiomeInterpolation(worldX, worldZ);
            int biomeHeight = biomeInfo.height;

            for (int y = 128; y <= biomeHeight; ++y) {
                if (biomeInfo.type == Biomes::Grassland) { // More like grassland.
                    if (y == biomeHeight && biomeHeight >= 138) {
                        setBlockAt(chunk, worldX, y, worldZ, GRASS);
                    } else {
                        setBlockAt(chunk, worldX, y, worldZ, DIRT);
                    }
                } else if (biomeInfo.type == Biomes::Mountain) { // More like mountain.
                    if (y > 200 && y == biomeHeight) {
                        setBlockAt(chunk, worldX, y, worldZ, SNOW);
                    } else {
                        setBlockAt(chunk, worldX, y, worldZ, STONE);
                    }
                } else if (biomeInfo.type == Biomes::Desert) { // More like Desert

                    if (y == biomeHeight || y == biomeHeight-1) {
                        setBlockAt(chunk, worldX, y, worldZ, SAND);
                    } else {
                        setBlockAt(chunk, worldX, y, worldZ, STONE);
                    }

                } else{ // More like SNOW
                    if (y == biomeHeight) {
                        setBlockAt(chunk, worldX, y, worldZ, SNOWGRASS);
                    } else {
                        setBlockAt(chunk, worldX, y, worldZ, DIRT);
                    }
                    // if Max height < 138, then [128,138] will be filled with water, 138 is ICE
                    if (biomeHeight<138){
                        setBlockAt(chunk, worldX, 138, worldZ, ICE);
                    }
                }
            }

            for (int y = 128; y <= 138; ++y) {
                // Replace EMPTY blocks with WATER when y in [128, 138].
                if (getBlockAt(chunk, worldX, y, worldZ) == EMPTY) {
                    setBlockAt(chunk, worldX, y, worldZ, WATER);
                }
            }

            // Cave Creation
            setBlockAt(chunk, worldX, 0, worldZ, BEDROCK);

            int lavaLevel = 25;

            for (int y = 1; y < 128; ++y) {
                float h = Biomes::getCaveHeight(worldX, y, worldZ);
                if (h > 0.f) {
                    if (y <= lavaLevel){
                        setBlockAt(chunk, worldX, y, worldZ, LAVA);
                    } else {
                        setBlockAt(chunk, worldX, y, worldZ, EMPTY);
                    }
                } else {
                    setBlockAt(chunk, worldX, y, worldZ, STONE);
                }

            }

            // add other parts (Trees etc.)
            if (biomeInfo.type == Biomes::Grassland) {

                // Skip the border of the chunk
                if (dx >=2 && dz >=2 && dx <=13 && dz <= 13){
                    // 10% prob to generate a Tree for each coloum in Grassland
                    if (rand()% 1000 < 20 && getBlockAt(chunk, worldX, biomeHeight, worldZ)== GRASS && biomeHeight >138){
                        generateTree(chunk, worldX, biomeHeight, worldZ);
                    }

                    if (rand()% 1000 < 10 && getBlockAt(chunk, worldX, biomeHeight, worldZ)== GRASS && biomeHeight >138){
                        //generateTares(chunk, worldX, biomeHeight, worldZ);
                    }

                    if (rand()% 1000 < 10 && getBlockAt(chunk, worldX, biomeHeight, worldZ)== GRASS && biomeHeight >138){
                        //generateRedFlower(chunk, worldX, biomeHeight, worldZ);
                    }


                }
            }

            if (biomeInfo.type == Biomes::Snow) {
                // 10% prob to generate a Tree for each coloum in Grassland
                if (rand()% 1000 < 5 && getBlockAt(chunk, worldX, biomeHeight, worldZ)== SNOWGRASS){
                    generateIcicle(chunk, worldX, biomeHeight, worldZ);
                }
            }

            if (biomeInfo.type == Biomes::Desert&& biomeHeight >138) {

                // 10% prob to generate a Tree for each coloum in Grassland
                if (rand()% 1000 < 2 && getBlockAt(chunk, worldX, biomeHeight, worldZ)== SAND){
                    generateCacti(chunk, worldX, biomeHeight, worldZ);
                }

                if (rand()% 1000 > 998 && getBlockAt(chunk, worldX, biomeHeight, worldZ)== SAND){
                    //generateDeadBush(chunk, worldX, biomeHeight, worldZ);
                }

            }

        }
    }
}

void BlockTypeWorker::setBlockAt(Chunk *chunk, int x, int y, int z, BlockType t) {
    glm::vec2 chunkOrigin = glm::vec2(floor(x / 16.f) * 16, floor(z / 16.f) * 16);
    chunk->setBlockAt(
        static_cast<unsigned int>(x - chunkOrigin.x),
        static_cast<unsigned int>(y),
        static_cast<unsigned int>(z - chunkOrigin.y),
        t
    );
}

BlockType BlockTypeWorker::getBlockAt(Chunk *chunk, int x, int y, int z) const {
    glm::vec2 chunkOrigin = glm::vec2(floor(x / 16.f) * 16, floor(z / 16.f) * 16);
    return chunk->getBlockAt(
        static_cast<unsigned int>(x - chunkOrigin.x),
        static_cast<unsigned int>(y),
        static_cast<unsigned int>(z - chunkOrigin.y)
    );
}

void BlockTypeWorker::generateTree(Chunk *chunk, int x, int y, int z) {

    int flag = 1;
    int x0 = x - 2;
    int z0 = z - 2;
    int woodHeight = 2 + rand() % 2;

    // Search if there is engouh space for
    for (int k = 3 ; k <= 4 && flag==1; k++) {
        for (int i = 0 ; i < 5 && flag==1; i++) {
            for (int j = 0 ; j < 5 && flag==1; j++) {
                if(getBlockAt(chunk, x0 + i, y + k , z0 + j) != EMPTY){
                    flag = 0;
                    break;
                }
            }
        }
    }

    // When there is enough space for tree generation, we generate a tree
    if (flag == 1){

        for (int k = 1 ; k <= woodHeight ; k++) {
            setBlockAt(chunk, x, y + k , z, WOOD);
        }

        for (int k = woodHeight + 1; k <= woodHeight + 2 ; k++) {
            for (int i = 0 ; i < 5 ; i++) {
                    for (int j = 0 ; j < 5 ; j++) {
                        setBlockAt(chunk, x0 + i, y + k , z0 + j, LEAF);
                    }
            }
        }

        setBlockAt(chunk, x + 1, y + woodHeight + 3 , z, LEAF);
        setBlockAt(chunk, x - 1, y + woodHeight + 3 , z, LEAF);
        setBlockAt(chunk, x, y + woodHeight + 3 , z, LEAF);
        setBlockAt(chunk, x, y + woodHeight + 3 , z + 1, LEAF);
        setBlockAt(chunk, x, y + woodHeight + 3 , z - 1, LEAF);
    }


};

void BlockTypeWorker::generateIcicle(Chunk *chunk, int x, int y, int z) {

    // The height of Icicle is 4 + rand() % 2
    int num = rand() % 5;

    for (int k = 0 ; k < 4 + num ; k++) {
        setBlockAt(chunk, x, y + k , z, ICE);
    }



};

void BlockTypeWorker::generateCacti(Chunk *chunk, int x, int y, int z) {

    setBlockAt(chunk, x, y + 1 , z, CACTI);
    setBlockAt(chunk, x, y + 2 , z, CACTI);
    setBlockAt(chunk, x, y + 3 , z, CACTI);
};

void BlockTypeWorker::generateTares(Chunk *chunk, int x, int y, int z) {

    setBlockAt(chunk, x, y + 1 , z, TARES);
};

void BlockTypeWorker::generateDeadBush(Chunk *chunk, int x, int y, int z) {

    setBlockAt(chunk, x, y + 1 , z, DEADBUSH);
};

void BlockTypeWorker::generateRedFlower(Chunk *chunk, int x, int y, int z) {

    setBlockAt(chunk, x, y + 1 , z, REDFLOWER);
};

void BlockTypeWorker::generateMaze(Chunk *chunk, int x, int y, int z) {


};
