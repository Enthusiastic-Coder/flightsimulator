#include "stdafx.h"
#include "SimpleCloud.h"
#include "PerlinNoise.h"
#include "OpenGLRenderer.h"
#include "GPSModelTransform.h"

SimpleCloud::SimpleCloud(std::string name) : 
	JSONRigidBody(name),
    _CLOUD_ID(0)
{
	setMass(100);
	Matrix3x3D m;
	m.LoadIdentity();
	setInertiaMatrix(m);
	setCG(Vector3D());

	setEuler(0, 0, 0);
#if defined LOCATED_AT_GIBRALTER
	setPosition(GPSLocation(36.151473, -5.339665) + Vector3F(-200, 0, 500));
#else
	setPosition(GPSLocation(51.471866, -0.465477));
#endif
}

SimpleCloud::~SimpleCloud()
{
}

void SimpleCloud::renderMesh(Renderer* r, unsigned int shadowMapCount)
{
	if (!global_fg_debug)
	{
		if (!_CLOUD_ID)
			initialiseCloud();

        glColor3f(r->fLightingFraction, r->fLightingFraction, r->fLightingFraction);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthMask(GL_FALSE);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, _CLOUD_ID);
		glEnable(GL_TEXTURE_2D);         //Render the cloud texture
        drawCloud(5000);
		//drawCloud(1000);
		//drawCloud();
		glDisable(GL_BLEND);

        glDisable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
        glDepthMask(GL_TRUE);
	}
}


void SimpleCloud::update(double dt)
{
}

void SimpleCloud::initialiseCloud()
{
    Noise n;

    n.generateNoise();

//    PerlinNoise perlinNoise;
//    perlinNoise.Prepare();

	unsigned char texture[noiseHeight][noiseWidth][4];       //Temporary array to hold texture RGB values 

	for (int x = 0; x < noiseWidth; x++)         //Set cloud color value to temporary array
		for (int y = 0; y < noiseHeight; y++)
		{
			//float color = perlinNoise.map256[i * 256 + j];
            texture[y][x][0] = 255;
            texture[y][x][1] = 255;
            texture[y][x][2] = 255;
            texture[y][x][3] = n.turbulence(x, y, 128) / 255.0 * 255;

            texture[y][x][3] = n.turbulence(x, y, 100);
            //texture[y][x][3] = perlinNoise.Noise(x, y, 128) /255.0 * 255;
		}

    glGenTextures(1, &_CLOUD_ID);           //Texture binding
    glBindTexture(GL_TEXTURE_2D, _CLOUD_ID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, noiseWidth, noiseHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glGenerateMipmap(GL_TEXTURE_2D);
}

float SimpleCloud::getHeight(float factor, float height, int x, int z)
{
    float heightFactorX = -std::abs(x) / factor + 1;
    float heightFactorZ = -std::abs(z) / factor + 1;
    return std::min(heightFactorX ,heightFactorZ)  * height;
}

void SimpleCloud::drawCloud(float height)
{
	GPSLocation l = getGPSLocation();
	l = GPSLocation((int)l._lat, (int)l._lng, 0);
	GPSModelTransform transform(getGPSLocation());
	Vector3F offset = l.offSetTo(getGPSLocation());

    float fTex = 5.0f;

	float distanceFactor = 100.0f * 1000;

	int cloudSize = 50000;

	glBegin(GL_QUADS);
	for (int z = -distanceFactor; z <= distanceFactor; z+= cloudSize)
		for (int x = -distanceFactor; x <= distanceFactor; x += cloudSize)
		{
			Vector3F pt1(x, getHeight(distanceFactor, height, x, z), z);
			Vector3F pt2(x + cloudSize, getHeight(distanceFactor, height, x + cloudSize, z), z);
			Vector3F pt3(x + cloudSize, getHeight(distanceFactor, height, x + cloudSize, z - cloudSize), z - cloudSize);
			Vector3F pt4(x, getHeight(distanceFactor, height, x, z - cloudSize), z - cloudSize);

            //glNormal3f(0, -1, 0);
			glTexCoord2f(((x + offset.x) / distanceFactor / 2 + 0.5) * fTex, ((z + offset.z) / distanceFactor / 2 + 0.5)*fTex);
			glVertex3fv(transform.LocalFlatToLocal(pt1).ptr());

			glTexCoord2f(((x + cloudSize + offset.x) / distanceFactor / 2 + 0.5) * fTex, ((z + offset.z) / distanceFactor / 2 + 0.5)*fTex);
			glVertex3fv(transform.LocalFlatToLocal(pt2).ptr());

			glTexCoord2f(((x + cloudSize + offset.x) / distanceFactor / 2 + 0.5) * fTex, ((z - cloudSize + offset.z) / distanceFactor / 2 + 0.5)*fTex);
			glVertex3fv(transform.LocalFlatToLocal(pt3).ptr());

			glTexCoord2f(((x + offset.x) / distanceFactor / 2 + 0.5) * fTex, ((z - cloudSize + offset.z) / distanceFactor / 2 + 0.5)*fTex);
			glVertex3fv(transform.LocalFlatToLocal(pt4).ptr());
		}
	glEnd();
}
