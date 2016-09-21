#pragma once

#include "JSONRigidBody.h"

class Renderer;

class SimpleCloud : public JSONRigidBody
{
public:
	SimpleCloud(std::string name);
	~SimpleCloud();

    void renderMesh(Renderer* r, unsigned int shadowMapCount) override;
	void update(double dt);
	void initialiseCloud();
	void drawCloud(float height = 100.0f);

	GLuint _CLOUD_ID;
protected:
	float getHeight(float factor, float height, int x, int z);
};

