#include "stdafx.h"
#include "RigidBodyMeshManager.h"
#include "CPPSourceCodeMeshModel.h"
#include <SDL_log.h>

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

    std::string meshObj("model.obj");

    if (!cppMeshModel->loadMesh("Models/" + sMeshName + "/" + meshObj))
        if (!cppMeshModel->Build("Models/" + sMeshName, true))
            return 0;
        else
            cppMeshModel->saveMesh("Models/" + sMeshName + "/" + meshObj);

	MeshModel* model = cppMeshModel.release();

	mc.setCGOffset(model->getCentroidPt());
	model->calcMomentOfInertia(mc);
	model->BuildVertexBuffers();
    mc.setMOIFactor(3.0f);
    Matrix3x3F moi = mc.MOI();

    SDL_Log("Mesh :%s", sMeshName.c_str());
    SDL_Log("MOI :\n [%.2f, %.2f, %.2f\n "
                "[%.2f, %.2f, %.2f]\n "
                "[%.2f, %.2f, %.2f]]",
                moi.e11, moi.e12, moi.e13,
                moi.e21, moi.e22, moi.e23,
                moi.e31, moi.e32, moi.e33
            );
	
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
