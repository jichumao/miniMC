#ifndef INVENTORY_H
#define INVENTORY_H

#include "chunk.h"
#include <vector>

class Inventory
{
private:
    int size;
    int selectedBlock;
    std::vector<std::pair<BlockType, int>> blocks;

public:
    Inventory();

    void init();

    bool addBlock(BlockType blockType); // add removed block to inventory
    BlockType placeBlock(); // decrement the block by 1 and return the selected block
};

#endif // INVENTORY_H
