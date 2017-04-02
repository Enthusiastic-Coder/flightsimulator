#include "stdafx.h"
#include "CircularRunwayRigidBody.h"
#include "MeshModel.h"
#include "CPPSourceCodeMeshModel.h"


CircularRunwayRigidBody::CircularRunwayRigidBody(std::string name) :
	JSONRigidBody(name)
{
}


CircularRunwayRigidBody::~CircularRunwayRigidBody()
{
}

bool CircularRunwayRigidBody::getHeightFromPosition(const GPSLocation & position, HeightData & heightData) const
{
	const CircularRunwayMeshModel* mesh = (const CircularRunwayMeshModel*)getMeshModel();

	if (mesh == nullptr)
		return false;

	const auto& boundary = mesh->getBoundary();

	Vector3F offsetPos = getGPSLocation().offSetTo(position);

	const auto& box = mesh->getBoundingBox();

	if (box.PointIsContained(offsetPos))
	{
		for (size_t i = 0; i < boundary.size(); ++i)
		{
			if (boundary[i].PointIsContained(offsetPos))
			{
				heightData.setData(offsetPos, boundary[i].getPlane());
				return true;
			}
		}
	}

	return false;
}
