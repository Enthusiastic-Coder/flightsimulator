#include "stdafx.h"
#include "TriangleFanQuadNode.h"
#include <cstring>

TriangleFanQuadNode::TriangleFanQuadNode(ChunkNode *pChunk, TriangleFanQuadNode *pParent, const std::pair<Vector2I, Vector2I >& rect, int quadNo) :
    _pChunk(pChunk),
    _pParent(pParent),
    _quadNo(quadNo),
    _topLeftIDX(rect.first),
    _bottomRightIDX(rect.second)
{
	std::memset(_pChild, 0, sizeof(_pChild));
	_leftIdxActive = false;
	_rightIdxActive = false;
	_topIdxActive = false;
	_bottomIdxActive = false;

	if (pParent)
		_level = pParent->_level + 1;
	else
		_level = 0;
}

TriangleFanQuadNode::~TriangleFanQuadNode()
{
	for (int i = 0; i < 4; ++i)
		if (_pChild[i])
		{
			delete _pChild[i];
			_pChild[i] = nullptr;
		}
}

TriangleFanQuadNode * TriangleFanQuadNode::newRootNode(ChunkNode *pChunk, const std::pair<Vector2I, Vector2I >& rect)
{
	return new TriangleFanQuadNode(pChunk, 0, rect, -1);
}

TriangleFanQuadNode * TriangleFanQuadNode::newChildNode(TriangleFanQuadNode *pParent, int quadNo)
{
	return new TriangleFanQuadNode(pParent->_pChunk, pParent, pParent->getChildIDX(quadNo), quadNo);
}

std::pair<Vector2I, Vector2I > TriangleFanQuadNode::getChildIDX(int quadNo) const
{
	Vector2I leftTopIDX, bottomRightIDX;

	if (quadNo == 0)
	{
		leftTopIDX = getTopLeftIDX();
		bottomRightIDX = getCenterIDX();
	}
	else if (quadNo == 1)
	{
		leftTopIDX = getTopIDX();
		bottomRightIDX = getRightIDX();
	}
	else if (quadNo == 2)
	{
		leftTopIDX = getCenterIDX();
		bottomRightIDX = getBottomRightIDX();
	}
	else if (quadNo == 3)
	{
		leftTopIDX = getLeftIDX();
		bottomRightIDX = getBottomIDX();
	}

	return std::make_pair(leftTopIDX, bottomRightIDX);
}

Vector2I TriangleFanQuadNode::getCenterIDX() const
{
	return (_topLeftIDX + _bottomRightIDX) / 2;
}

Vector2I TriangleFanQuadNode::getLeftIDX() const
{
	return Vector2I(_topLeftIDX.x, (_topLeftIDX.z + _bottomRightIDX.z) / 2);
}

Vector2I TriangleFanQuadNode::getRightIDX() const
{
	return Vector2I(_bottomRightIDX.x, (_topLeftIDX.z + _bottomRightIDX.z) / 2);
}

Vector2I TriangleFanQuadNode::getTopIDX() const
{
	return Vector2I((_topLeftIDX.x + _bottomRightIDX.x) / 2, _topLeftIDX.z);
}

Vector2I TriangleFanQuadNode::getBottomIDX() const
{
	return Vector2I((_topLeftIDX.x + _bottomRightIDX.x) / 2, _bottomRightIDX.z);
}

Vector2I TriangleFanQuadNode::getTopLeftIDX() const
{
	return _topLeftIDX;
}

Vector2I TriangleFanQuadNode::getTopRightIDX() const
{
	return Vector2I(_bottomRightIDX.x, _topLeftIDX.z);
}

Vector2I TriangleFanQuadNode::getBottomRightIDX() const
{
	return _bottomRightIDX;
}

Vector2I TriangleFanQuadNode::getBottomLeftIDX() const
{
	return Vector2I(_topLeftIDX.x, _bottomRightIDX.z);
}

std::pair<Vector2I, Vector2I> TriangleFanQuadNode::getBoundaryIDX() const
{
	return std::make_pair(getTopLeftIDX(), getBottomRightIDX());
}


