#include "inventory.h"

Inventory::Inventory() : size(64), selectedBlock(0) {
    init();
}

void Inventory::init() {
    blocks.clear();

    // init blocks in inventory
    for (int j = 0; j < size; ++j) {
        blocks.push_back(std::make_pair(EMPTY, 0));
    }
}

bool Inventory::addBlock(BlockType blockType) {

    int firstEmptyIdx = -1;

    for (int i = 0; i < size; ++i) {
        if (blocks[i].first == blockType && blocks[i].second < 64) {
            blocks[i].second += 1;
            return true;
        }

        if (firstEmptyIdx < 0) {
            firstEmptyIdx = i;
        }
    }

    if (firstEmptyIdx >= 0) {
        blocks[firstEmptyIdx].first = blockType;
        blocks[firstEmptyIdx].second = 1;
        return true;
    }
    return false;
}

BlockType Inventory::placeBlock() {
    blocks[selectedBlock].second -= 1;
    BlockType targetBlockType = blocks[selectedBlock].first;

    if (blocks[selectedBlock].second == 0) {
        blocks[selectedBlock].first = EMPTY;
    }

    return targetBlockType;
}
