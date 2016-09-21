#pragma once

struct TriangleFanQuadNode;

struct ChunkNode
{
	ChunkNode() :
		quad(nullptr),
		left(nullptr),
		right(nullptr),
		top(nullptr),
		bottom(nullptr)
	{}

	TriangleFanQuadNode *quad;
	ChunkNode *left;
	ChunkNode *right;
	ChunkNode *top;
	ChunkNode *bottom;
};