#include "vboleader.h"
#include "vboworker.h"

VBOLeader::VBOLeader(
    TerrainThreadPool *threadPool,
    TerrainWorkQueue<Chunk *> *vboDataRecreationQueue,
    TerrainWorkQueue<ChunkVBOData> *newVBODataQueue
) : m_threadPool(threadPool),
    m_vboDataRecreationQueue(vboDataRecreationQueue),
    m_newVBODataQueue(newVBODataQueue)
{}

// Picks off chunks from m_vboDataRecreationQueue and spawns VBOWorker
// threads from the thread pool for each of them.
void VBOLeader::run() {
    Chunk *chunk;
    while (!m_threadPool->isDone()) {
        // TODO: interrupt wait when thread pool is shutting down.
        m_vboDataRecreationQueue->waitAndPop(chunk);

        // Launch VBOWorker thread to recreate the VBO data of this chunk. 
        m_threadPool->submit([this, chunk]{
            VBOWorker w(chunk, m_newVBODataQueue);
        });
    }
}
