#include "stdafx.h"
#include "RigidBodyMeshManager.h"
#include "CPPSourceCodeMeshModel.h"

RigidBodyMeshManager::RigidBodyMeshManager()
{
}


RigidBodyMeshManager::~RigidBodyMeshManager()
{
    unLoadAll();
}

RigidBodyMeshManager& RigidBodyMeshManager::get()
{
	static RigidBodyMeshManager m;
	return m;
}

MeshModel* RigidBodyMeshManager::getModel(std::string sMeshName, MassChannel &mc)
{
	MeshModel *model = _meshModelMap[sMeshName];
	if (model)
		mc = _massChannelMap[sMeshName];

	return model;
}

MeshModel* RigidBodyMeshManager::loadModel(std::string sMeshName, MassChannel &mc)
{
	std::unique_ptr<CPPSourceCodeMeshModel> cppMeshModel(new CPPSourceCodeMeshModel);

	if (!cppMeshModel->Build("Models\\" + sMeshName, true))
		return 0;

	MeshModel* model = cppMeshModel.release();

	mc.setCGOffset(model->getCentroidPt());
	model->calcMomentOfInertia(mc);
	model->BuildVertexBuffers();
	
	_massChannelMap[sMeshName] = mc;
	_meshModelMap[sMeshName] = model;
	
	return model;
}

void RigidBodyMeshManager::unLoad(MeshModel* meshModel)
{
    //_meshModelMap.find()
}

void RigidBodyMeshManager::unLoadAll()
{
    for (auto& it : _meshModelMap)
        delete it.second;

    _meshModelMap.clear();
}
