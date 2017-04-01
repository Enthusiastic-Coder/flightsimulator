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

	void addModel(std::string filename, MeshModel*, MassChannel&);
	MeshModel* getModel(std::string filename, MassChannel&);
	MeshModel* loadModel(std::string filename, MassChannel&);
	void unLoad(MeshModel*);

    void unLoadAll();

protected:
	void referenceModel(std::string id, MeshModel* m, MassChannel&);

private:
	RigidBodyMeshManager();

	std::map<std::string,MeshModel*> _meshModelMap;
	std::map<std::string, MassChannel> _massChannelMap;
};

