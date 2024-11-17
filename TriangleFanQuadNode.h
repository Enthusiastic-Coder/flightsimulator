#pragma once

#include <utility>
#include <jibbs/vector/vector2.h>


struct ChunkNode;

struct TriangleFanQuadNode
{
	~TriangleFanQuadNode();

	static TriangleFanQuadNode * newRootNode(ChunkNode *pChunk, const std::pair<Vector2I, Vector2I >& rect);
	static TriangleFanQuadNode * newChildNode(TriangleFanQuadNode *pParent, int quadNo);

	std::pair<Vector2I, Vector2I> getBoundaryIDX() const;
	std::pair<Vector2I, Vector2I > getChildIDX(int quadNo) const;
	Vector2I getCenterIDX() const;
	Vector2I getLeftIDX() const;
	Vector2I getRightIDX() const;
	Vector2I getTopIDX() const;
	Vector2I getBottomIDX() const;
	Vector2I getTopLeftIDX() const;
	Vector2I getTopRightIDX() const;
	Vector2I getBottomRightIDX() const;
	Vector2I getBottomLeftIDX() const;

	ChunkNode *_pChunk;
	TriangleFanQuadNode *_pParent;
	TriangleFanQuadNode *_pChild[4];
	int _quadNo;
	int _level;
	bool _leftIdxActive;
	bool _rightIdxActive;
	bool _topIdxActive;
	bool _bottomIdxActive;
	Vector2I _topLeftIDX;
	Vector2I _bottomRightIDX;

private:
	TriangleFanQuadNode(ChunkNode *pChunk, TriangleFanQuadNode *pParent, const std::pair<Vector2I, Vector2I >& rect, int quadNo);
};
