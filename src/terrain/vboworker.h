#pragma once
#include "scene/chunk.h"
#include <mutex>
#include "queue.h"

// VBO data created for a chunk by a VBOWorker.
struct ChunkVBOData {
    Chunk *chunk;

    std::vector<GLuint> opaqueIndices;

    std::vector<glm::vec4> opaqueVertexAttrs;

    std::vector<GLuint> transparentIndices;

    std::vector<glm::vec4> transparentVertexAttrs;

    ChunkVBOData() {}

    ChunkVBOData(Chunk *c) : chunk(c) {}
};

// Creates VBO data for a chunk. Meant to run concurrently in a thread.
class VBOWorker {
public:
    // Construction also executes VBO data creation.
    VBOWorker(Chunk *chunk, TerrainWorkQueue<ChunkVBOData> *vboDataQueue);

private:
    // Thread-safe queue where this worker will place the VBO data that
    // it creates.
    TerrainWorkQueue<ChunkVBOData> *mp_vboDataQueue;
};
