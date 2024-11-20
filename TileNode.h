#pragma once

#include "GPSTerrainData.h"
#include "TriangleFanQuadNode.h"
#include "ChunkNode.h"
#include <jibbs/matrix/MatrixArrayCoords.h>

class TileNode
{
public:
    TileNode(GPSTerrainData&);
    ~TileNode();

    void clear();
    void Build(int chunkSize, int maxLevel);
    std::vector<Vector2I> BuildIndexData(unsigned int chunkNo, int level) const;

    const ChunkNode& chunkNode(int idx) const;
    const GPSTerrainData& getGPSGroupObject() const;
    size_t size() const;
    int chunkDim() const;
    int maxLevel() const;

protected:
    void createRootChunkNodes(int chunkSize);

private:

    enum FindDirection
    {
        FindLeft,
        FindRight,
        FindUp,
        FindDown
    };

    struct adjacentQuadTable
    {
        adjacentQuadTable(int fromQuadNo, int toQuadNo, bool boundaryCrossed)
        {
            _fromQuadNo = fromQuadNo;
            _toQuadNo = toQuadNo;
            _boundaryCrossed = boundaryCrossed;
        }

        int _fromQuadNo;
        int _toQuadNo;
        bool _boundaryCrossed;
    };

    float Height(Vector2I pos) const;
    float GetCenterHeight(TriangleFanQuadNode *pNode) const;
    float CalcAverageHeight(TriangleFanQuadNode *pNode, int quadNo) const;

    void BuildHelper(TriangleFanQuadNode *pParentNode);
    TriangleFanQuadNode* BuildQuad(TriangleFanQuadNode *pParentNode, int quadNo);
    TriangleFanQuadNode* findHorizontalAdjcentNode(TriangleFanQuadNode* pNode);
    TriangleFanQuadNode* findVerticalAdjcentNode(TriangleFanQuadNode* pNode);
    TriangleFanQuadNode* findAdjacentNode(FindDirection fd, TriangleFanQuadNode* pNode);
    TriangleFanQuadNode* findAdjacentNode(FindDirection fd, std::vector<int>&, TriangleFanQuadNode* pNode);
    void getAdjacentQuadNo(FindDirection fd, int &quadNo, bool &boundaryCrossed);
    void BuildIndexDataHelper(int level, TriangleFanQuadNode *pNode, std::vector<Vector2I> &indexTerrainList) const;

private:
    int _maxLevel;
    GPSTerrainData &_GPSMeshObject;
    std::vector<ChunkNode> _chunkNodes;
    MatrixArrayCoords _chunkDim;
};
