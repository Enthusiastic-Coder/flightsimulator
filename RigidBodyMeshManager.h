#pragma once

#include <map>
#include <string>
#include <MassChannel.h>

class MeshModel;

class RigidBodyMeshManager
{
public:
	~RigidBodyMeshManager();

	static RigidBodyMeshManager& get();

	MeshModel* getModel(std::string filename, MassChannel&);
	MeshModel* loadModel(std::string filename, MassChannel&);
	void unLoad(MeshModel*);

    void unLoadAll();

private:
	RigidBodyMeshManager();

	std::map<std::string,MeshModel*> _meshModelMap;
	std::map<std::string, MassChannel> _massChannelMap;
};

