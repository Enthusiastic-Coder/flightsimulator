#include "stdafx.h"
#include "TileNode.h"
#include "ChunkNode.h"
#include "TriangleFanQuadNode.h"
#include "GPSTerrainData.h"

TileNode::TileNode(GPSTerrainData &meshObject) :
	_GPSMeshObject(meshObject),
	_maxLevel(0)
{
}

TileNode::~TileNode()
{
	clear();
}

int TileNode::maxLevel() const
{
	return _maxLevel;
}

int TileNode::chunkDim() const
{
	return _chunkDim.getX();
}

const ChunkNode& TileNode::chunkNode(int idx) const
{
	return _chunkNodes[idx];
}

const GPSTerrainData& TileNode::getGPSGroupObject() const
{
	return _GPSMeshObject;
}

size_t TileNode::size() const
{
	return _chunkNodes.size();
}

void TileNode::clear()
{
	// Clear Root children.
	for (size_t i = 0; i < _chunkNodes.size(); ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			if (_chunkNodes[i].quad->_pChild[j])
			{
				delete _chunkNodes[i].quad->_pChild[j];
				_chunkNodes[i].quad->_pChild[j] = nullptr;
			}
		}
	}
}

void TileNode::createRootChunkNodes(int chunkSize)
{
	clear();
	_chunkDim.setDims(chunkSize);
	_chunkNodes.resize(_chunkDim.product());

	for (int i = 0; i < _chunkDim.product(); ++i)
	{
		Vector2I idx = _chunkDim.toMatrixPos(i);

		if (idx.x < chunkSize - 1)
			_chunkNodes[i].right = &_chunkNodes[i + 1];

		if (idx.x > 0)
			_chunkNodes[i].left = &_chunkNodes[i - 1];

		if (idx.z < chunkSize - 1)
			_chunkNodes[i].bottom = &_chunkNodes[i + chunkSize];

		if (idx.z > 0)
			_chunkNodes[i].top = &_chunkNodes[i - chunkSize];
	}

	int tileChunkRatio = _GPSMeshObject.GetDimensions() / chunkSize;

	for (int z = 0; z < chunkSize; ++z)
	{
		for (int x = 0; x < chunkSize; ++x)
		{
			ChunkNode *pChunkNode = &_chunkNodes[_chunkDim.toArrayPos(x, z)];

			pChunkNode->quad = TriangleFanQuadNode::newRootNode(pChunkNode,
				std::make_pair(Vector2I(tileChunkRatio * x, tileChunkRatio * z),
					Vector2I(tileChunkRatio * (x + 1), tileChunkRatio * (z + 1))));
		}
	}
}

void TileNode::Build(int chunkSize, int maxLevel)
{
	_maxLevel = maxLevel;
	createRootChunkNodes(chunkSize);

	for (auto& it : _chunkNodes)
		BuildHelper(it.quad);
}

void TileNode::BuildHelper(TriangleFanQuadNode *pParentNode)
{
	if (pParentNode->_level > _maxLevel && _maxLevel)
		return;

	Vector2I diff = pParentNode->getBottomRightIDX() - pParentNode->getTopLeftIDX();

	if (diff.x == 1 || diff.z == 1)
		return;

	float fCenterHeight = GetCenterHeight(pParentNode);

	for (int i = 0; i < 4; ++i)
	{
		float fAverageHeight = CalcAverageHeight(pParentNode, i);
		float fDiff = fabs(fCenterHeight - fAverageHeight);

		if (fDiff > FLT_EPSILON || pParentNode->_level == _maxLevel)
			BuildHelper(BuildQuad(pParentNode, i));
	}
}

TriangleFanQuadNode* TileNode::BuildQuad(TriangleFanQuadNode *pParentNode, int quadNo)
{
	if (pParentNode->_pChild[quadNo] == nullptr)
		pParentNode->_pChild[quadNo] = TriangleFanQuadNode::newChildNode(pParentNode, quadNo);

	TriangleFanQuadNode *pHorAdjNode = findHorizontalAdjcentNode(pParentNode->_pChild[quadNo]);
	TriangleFanQuadNode *pVertAdjNode = findVerticalAdjcentNode(pParentNode->_pChild[quadNo]);

	if (quadNo == 0)
	{
		pParentNode->_leftIdxActive = true;
		pParentNode->_topIdxActive = true;

		if (pHorAdjNode)
			pHorAdjNode->_rightIdxActive = true;

		if (pVertAdjNode)
			pVertAdjNode->_bottomIdxActive = true;
	}
	else if (quadNo == 1)
	{
		pParentNode->_topIdxActive = true;
		pParentNode->_rightIdxActive = true;

		if (pHorAdjNode)
			pHorAdjNode->_leftIdxActive = true;

		if (pVertAdjNode)
			pVertAdjNode->_bottomIdxActive = true;
	}
	else if (quadNo == 2)
	{
		pParentNode->_rightIdxActive = true;
		pParentNode->_bottomIdxActive = true;

		if (pHorAdjNode)
			pHorAdjNode->_leftIdxActive = true;

		if (pVertAdjNode)
			pVertAdjNode->_topIdxActive = true;
	}
	else if (quadNo == 3)
	{
		pParentNode->_bottomIdxActive = true;
		pParentNode->_leftIdxActive = true;

		if (pHorAdjNode)
			pHorAdjNode->_rightIdxActive = true;

		if (pVertAdjNode)
			pVertAdjNode->_topIdxActive = true;
	}

	return pParentNode->_pChild[quadNo];
}

void TileNode::getAdjacentQuadNo(FindDirection fd, int &quadNo, bool &boundaryCrossed)
{
	static bool tablesDone = false;
	static std::vector<adjacentQuadTable> leftTable;
	static std::vector<adjacentQuadTable> rightTable;
	static std::vector<adjacentQuadTable> downTable;
	static std::vector<adjacentQuadTable> upTable;

	if (tablesDone == false)
	{
		tablesDone = true;

		leftTable.push_back(adjacentQuadTable(0, 1, true));
		leftTable.push_back(adjacentQuadTable(1, 0, false));
		leftTable.push_back(adjacentQuadTable(2, 3, false));
		leftTable.push_back(adjacentQuadTable(3, 2, true));

		rightTable.push_back(adjacentQuadTable(0, 1, false));
		rightTable.push_back(adjacentQuadTable(1, 0, true));
		rightTable.push_back(adjacentQuadTable(2, 3, true));
		rightTable.push_back(adjacentQuadTable(3, 2, false));

		upTable.push_back(adjacentQuadTable(0, 3, true));
		upTable.push_back(adjacentQuadTable(1, 2, true));
		upTable.push_back(adjacentQuadTable(2, 1, false));
		upTable.push_back(adjacentQuadTable(3, 0, false));

		downTable.push_back(adjacentQuadTable(0, 3, false));
		downTable.push_back(adjacentQuadTable(1, 2, false));
		downTable.push_back(adjacentQuadTable(2, 1, true));
		downTable.push_back(adjacentQuadTable(3, 0, true));
	}

	std::vector<adjacentQuadTable> *pTable = nullptr;

	if (fd == FindDirection::FindLeft)
		pTable = &leftTable;

	else if (fd == FindDirection::FindRight)
		pTable = &rightTable;

	else if (fd == FindDirection::FindUp)
		pTable = &upTable;

	else if (fd == FindDirection::FindDown)
		pTable = &downTable;

	if (pTable)
		for (auto& it : *pTable)
		{
			if (it._fromQuadNo == quadNo)
			{
				boundaryCrossed = it._boundaryCrossed;
				quadNo = it._toQuadNo;
				break;
			}
		}
}

TriangleFanQuadNode* TileNode::findHorizontalAdjcentNode(TriangleFanQuadNode* pNode)
{
	FindDirection fd;

	if (pNode->_quadNo == 1 || pNode->_quadNo == 2)
		fd = FindDirection::FindRight;
	else
		fd = FindDirection::FindLeft;

	return findAdjacentNode(fd, pNode);
}

TriangleFanQuadNode* TileNode::findVerticalAdjcentNode(TriangleFanQuadNode* pNode)
{
	FindDirection fd;

	if (pNode->_quadNo == 0 || pNode->_quadNo == 1)
		fd = FindDirection::FindUp;
	else
		fd = FindDirection::FindDown;

	return findAdjacentNode(fd, pNode);
}

TriangleFanQuadNode* TileNode::findAdjacentNode(FindDirection fd, TriangleFanQuadNode* pNode)
{
	std::vector<int> quadHistoryTrail;
	return findAdjacentNode(fd, quadHistoryTrail, pNode);
}

TriangleFanQuadNode* TileNode::findAdjacentNode(FindDirection fd, std::vector<int>& quadHistoryTrail, TriangleFanQuadNode* pNode)
{
	if (!pNode)
		return nullptr;

	quadHistoryTrail.push_back(pNode->_quadNo);

	if (pNode->_pParent)
		return findAdjacentNode(fd, quadHistoryTrail, pNode->_pParent);

	bool boundaryCrossed = true;

	for (auto &it : quadHistoryTrail)
		if (boundaryCrossed)
			getAdjacentQuadNo(fd, it, boundaryCrossed);

	TriangleFanQuadNode *pChild(pNode);

	for (size_t i = quadHistoryTrail.size() - 1; i != 0; --i)
	{
		if (!pChild)
			continue;

		int quadNo = quadHistoryTrail[i];

		if (quadNo == -1)
		{
			if (!boundaryCrossed)
				continue;

			if (!pChild->_pChunk)
			{
				pChild = nullptr;
				continue;
			}

			if (fd == FindDirection::FindLeft)
			{
				if (pChild->_pChunk->left)
					pChild = pChild->_pChunk->left->quad;
				else
					pChild = nullptr;
			}
			else if (fd == FindDirection::FindRight)
			{
				if (pChild->_pChunk->right)
					pChild = pChild->_pChunk->right->quad;
				else
					pChild = nullptr;
			}
			else if (fd == FindDirection::FindUp)
			{
				if (pChild->_pChunk->top)
					pChild = pChild->_pChunk->top->quad;
				else
					pChild = nullptr;
			}
			else if (fd == FindDirection::FindDown)
			{
				if (pChild->_pChunk->bottom)
					pChild = pChild->_pChunk->bottom->quad;
				else
					pChild = nullptr;
			}
		}
		else
		{
			if (pChild->_pChild[quadNo] == nullptr)
				pChild->_pChild[quadNo] = TriangleFanQuadNode::newChildNode(pChild, quadNo);

			pChild = pChild->_pChild[quadNo];
		}
	}

	return pChild;
}

float TileNode::Height(Vector2I pos) const
{
	double u = double(pos.x) / _GPSMeshObject.GetDimensions();
	double v = 1.0 - double(pos.z) / _GPSMeshObject.GetDimensions();
	return _GPSMeshObject(u, v);
}

float TileNode::GetCenterHeight(TriangleFanQuadNode *pNode) const
{
	return Height(pNode->getCenterIDX());
}

float TileNode::CalcAverageHeight(TriangleFanQuadNode *pNode, int quadNo) const
{
	int count = 0;
	float fTotalHeight = 0.0f;

	auto& vertexPair = pNode->getChildIDX(quadNo);

	Vector2I leftTopIDX = vertexPair.first;
	Vector2I bottomRightIDX = vertexPair.second;

	for (int z = leftTopIDX.z; z < bottomRightIDX.z; ++z)
		for (int x = leftTopIDX.x; x < bottomRightIDX.x; ++x, ++count)
			fTotalHeight += Height(Vector2I(x, z));

	return fTotalHeight / count;
}

std::vector<Vector2I> TileNode::BuildIndexData(unsigned int chunkNo, int level) const
{
	std::vector<Vector2I> indexList;
	BuildIndexDataHelper(level, _chunkNodes[chunkNo].quad, indexList);
	return indexList;
}

void TileNode::BuildIndexDataHelper(int level, TriangleFanQuadNode *pNode, std::vector<Vector2I> &indexTerrainList) const
{
	if (pNode == nullptr)
		return;

	int quadsArray[] = { 0, 1, 2, 3, 0 };

	for (int i = 0; i < 4; ++i)
	{
		if (!pNode->_pChild[i] && !pNode->_pChild[quadsArray[i + 1]] || pNode->_level >= level)
		{
			if (pNode->_level == _maxLevel)
			{
				if (i == 0)
				{
					indexTerrainList.push_back(pNode->getCenterIDX());
					indexTerrainList.push_back(pNode->getTopIDX());
					indexTerrainList.push_back(pNode->getTopLeftIDX());

					indexTerrainList.push_back(pNode->getCenterIDX());
					indexTerrainList.push_back(pNode->getTopRightIDX());
					indexTerrainList.push_back(pNode->getTopIDX());
				}
				else if (i == 1)
				{
					indexTerrainList.push_back(pNode->getCenterIDX());
					indexTerrainList.push_back(pNode->getRightIDX());
					indexTerrainList.push_back(pNode->getTopRightIDX());

					indexTerrainList.push_back(pNode->getCenterIDX());
					indexTerrainList.push_back(pNode->getBottomRightIDX());
					indexTerrainList.push_back(pNode->getRightIDX());
				}
				else if (i == 2)
				{
					indexTerrainList.push_back(pNode->getCenterIDX());
					indexTerrainList.push_back(pNode->getBottomIDX());
					indexTerrainList.push_back(pNode->getBottomRightIDX());

					indexTerrainList.push_back(pNode->getCenterIDX());
					indexTerrainList.push_back(pNode->getBottomLeftIDX());
					indexTerrainList.push_back(pNode->getBottomIDX());
				}
				else if (i == 3)
				{
					indexTerrainList.push_back(pNode->getCenterIDX());
					indexTerrainList.push_back(pNode->getTopLeftIDX());
					indexTerrainList.push_back(pNode->getLeftIDX());


					indexTerrainList.push_back(pNode->getCenterIDX());
					indexTerrainList.push_back(pNode->getLeftIDX());
					indexTerrainList.push_back(pNode->getBottomLeftIDX());
				}
			}
			else
			{
				if (i == 0)
				{
					indexTerrainList.push_back(pNode->getCenterIDX());
					indexTerrainList.push_back(pNode->getTopRightIDX());
					indexTerrainList.push_back(pNode->getTopLeftIDX());
				}
				else if (i == 1)
				{
					indexTerrainList.push_back(pNode->getCenterIDX());
					indexTerrainList.push_back(pNode->getBottomRightIDX());
					indexTerrainList.push_back(pNode->getTopRightIDX());
				}
				else if (i == 2)
				{
					indexTerrainList.push_back(pNode->getCenterIDX());
					indexTerrainList.push_back(pNode->getBottomLeftIDX());
					indexTerrainList.push_back(pNode->getBottomRightIDX());
				}
				else if (i == 3)
				{
					indexTerrainList.push_back(pNode->getCenterIDX());
					indexTerrainList.push_back(pNode->getTopLeftIDX());
					indexTerrainList.push_back(pNode->getBottomLeftIDX());
				}
			}
		}
		else if (!pNode->_pChild[i] && pNode->_pChild[quadsArray[i + 1]])
		{
			if (i == 0)
			{
				indexTerrainList.push_back(pNode->getCenterIDX());
				indexTerrainList.push_back(pNode->getTopIDX());
				indexTerrainList.push_back(pNode->getTopLeftIDX());
			}
			else if (i == 1)
			{
				indexTerrainList.push_back(pNode->getCenterIDX());
				indexTerrainList.push_back(pNode->getRightIDX());
				indexTerrainList.push_back(pNode->getTopRightIDX());
			}
			else if (i == 2)
			{
				indexTerrainList.push_back(pNode->getCenterIDX());
				indexTerrainList.push_back(pNode->getBottomIDX());
				indexTerrainList.push_back(pNode->getBottomRightIDX());
			}
			else if (i == 3)
			{
				indexTerrainList.push_back(pNode->getCenterIDX());
				indexTerrainList.push_back(pNode->getLeftIDX());
				indexTerrainList.push_back(pNode->getBottomLeftIDX());
			}
		}
		else if (pNode->_pChild[i] && !pNode->_pChild[quadsArray[i + 1]])
		{
			if (i == 0)
			{
				indexTerrainList.push_back(pNode->getCenterIDX());
				indexTerrainList.push_back(pNode->getTopRightIDX());
				indexTerrainList.push_back(pNode->getTopIDX());
			}
			else if (i == 1)
			{
				indexTerrainList.push_back(pNode->getCenterIDX());
				indexTerrainList.push_back(pNode->getRightIDX());
				indexTerrainList.push_back(pNode->getBottomRightIDX());
			}
			else if (i == 2)
			{
				indexTerrainList.push_back(pNode->getCenterIDX());
				indexTerrainList.push_back(pNode->getBottomIDX());
				indexTerrainList.push_back(pNode->getBottomLeftIDX());
			}
			else if (i == 3)
			{
				indexTerrainList.push_back(pNode->getCenterIDX());
				indexTerrainList.push_back(pNode->getTopLeftIDX());
				indexTerrainList.push_back(pNode->getLeftIDX());

			}
			BuildIndexDataHelper(level, pNode->_pChild[i], indexTerrainList);
		}
		else
			BuildIndexDataHelper(level, pNode->_pChild[i], indexTerrainList);
	}
}
