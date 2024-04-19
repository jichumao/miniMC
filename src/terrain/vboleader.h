#pragma once
#include "scene/chunk.h"
#include "threadpool.h"
#include "vboworker.h"

// Spawns threads from a thread pool to execute VBOWorkers. 
class VBOLeader {
public:
    VBOLeader(
        TerrainThreadPool *threadPool,
        TerrainWorkQueue<Chunk *> *vboDataRecreationQueue,
        TerrainWorkQueue<ChunkVBOData> *newVBODataQueue
    );

    // Picks off chunks from m_vboDataRecreationQueue and spawns VBOWorker
    // threads from the thread pool for each of them.
    void run();

private:
    TerrainThreadPool *m_threadPool;

    // A thread-safe queue where VBOLeader will find chunks that need their
    // VBO data recreated.
    TerrainWorkQueue<Chunk *> *m_vboDataRecreationQueue;

    // A thread-safe queue needed by VBOWorkers to place the new VBO data.
    TerrainWorkQueue<ChunkVBOData> *m_newVBODataQueue;
};
