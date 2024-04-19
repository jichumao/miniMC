#include "vboworker.h"

// Construction also executes VBO data creation.
VBOWorker::VBOWorker(Chunk *chunk, TerrainWorkQueue<ChunkVBOData> *vboDataQueue)
    : mp_vboDataQueue(vboDataQueue)
{
    ChunkVBOData vboData(chunk);
    
    chunk->createAndFillInData(
        &vboData.opaqueIndices,
        &vboData.opaqueVertexAttrs,
        &vboData.transparentIndices,
        &vboData.transparentVertexAttrs
    );

    mp_vboDataQueue->push(vboData);
}
