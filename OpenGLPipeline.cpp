#pragma once

#include "stdafx.h"
#include "OpenGLPipeline.h"

#include "OpenGLRenderer.h"
#include "OpenGLShaderProgram.h"

OpenGLMatrixStack::OpenGLMatrixStack(MatrixType type) :
    _Type(type)
{
    _OpenGLMatrix.push(Matrix4x4D());
    _OpenGLMatrix.top().LoadIdentity();
}

const Matrix4x4D& OpenGLMatrixStack::Top() const
{
    return _OpenGLMatrix.top();
}

Matrix4x4D& OpenGLMatrixStack::Top()
{
    return _OpenGLMatrix.top();
}

void OpenGLMatrixStack::Push()
{
    _OpenGLMatrix.push(_OpenGLMatrix.top());
}

void OpenGLMatrixStack::Pop()
{
    _OpenGLMatrix.pop();
}

void OpenGLMatrixStack::LookAt(const Vector3D& eye, const Vector3D& center, Vector3D up)
{
    Top().LookAt(eye, center, up);
}

void OpenGLMatrixStack::LookAt(const GPSLocation& eye, const GPSLocation& center, Vector3D up)
{
    Top().LookAt(eye, center, up);
}

void OpenGLMatrixStack::SetPerspective(double fovy, double aspect, double zNear, double zFar)
{
    Top().SetPerspective(fovy, aspect, zNear, zFar);
}

void OpenGLMatrixStack::SetOrthographic(double zLeft, double zRight, double zTop, double zBottom, double zNear, double zFar)
{
    Top().SetOrthographic(zLeft, zRight, zTop, zBottom, zNear, zFar);
}

void OpenGLMatrixStack::ApplyShadowModelMatrix(const GPSLocation& loc, float heightAbovePlane, const Vector4D& lightDir, const QuarternionD& qPlane)
{
    Top().ApplyShadowModelMatrix(loc, heightAbovePlane, lightDir, qPlane);
}

void OpenGLMatrixStack::LoadIdentity()
{
    Top().LoadIdentity();
}

void OpenGLMatrixStack::PreScale(float x, float y, float z)
{
    if (_Type == View)
        Top().PreScale(1.0f / x, 1.0f / y, 1.0f / z);
    else
        Top().PreScale(x, y, z);
}

void OpenGLMatrixStack::Scale(float x, float y, float z)
{
    if (_Type == View)
        Top().Scale(1.0f / x, 1.0f / y, 1.0f / z);
    else
        Top().Scale(x, y, z);
}

void OpenGLMatrixStack::Translate(float x, float y, float z)
{
    if (_Type == View)
        Top().Translate(-x, -y, -z);
    else
        Top().Translate(x, y, z);
}

void OpenGLMatrixStack::Rotate(float x, float y, float z)
{
    if (_Type == View)
        Top().RotateView(x, y, z);
    else
        Top().RotateModel(x, y, z);
}

void OpenGLMatrixStack::TranslateLocation(const GPSLocation& loc)
{
    if (_Type == View)
        Top().ApplyViewMatrix(loc);
    else
        Top().ApplyModelMatrix(loc);
}

OpenGLPipeline::OpenGLPipeline() :
    _p(OpenGLMatrixStack::Projection),
    _m(OpenGLMatrixStack::Model),
    _v(OpenGLMatrixStack::View)
{
    _Frustum.push(Frustum());
    _zoom.LoadIdentity();
}

void OpenGLPipeline::applyScreenProjection(OpenGLPipeline& p, int x, int y, int cx, int cy)
{
    p.GetProjection().LoadIdentity();
    p.GetProjection().SetOrthographic(x, x+cx, y+cy, y , -1, 1);
    p.GetModel().LoadIdentity();
    p.GetView().LoadIdentity();
}

OpenGLPipeline& OpenGLPipeline::Get(unsigned int camID)
{
    static std::map<unsigned int, OpenGLPipeline> cameras;
    static int lastcamID = -1;
    static OpenGLPipeline* lastPipeline = 0;

    if (lastcamID != camID)
    {
        lastcamID = camID;
        OpenGLPipeline& pipeline = cameras[camID];
        pipeline._camID = camID;
        lastPipeline = &pipeline;
    }

    return *lastPipeline;
}

void OpenGLPipeline::Push()
{
    _m.Push();
    _v.Push();
    _Frustum.push(Frustum());
}


void OpenGLPipeline::ApplyMaterialToShader(OpenGLShaderProgram& progID, Vector4F ambient,Vector4F diffuse,Vector4F specualar,float shininess)
{
    progID.sendUniform("material.ambient", ambient);
    progID.sendUniform("material.diffuse", diffuse);
    progID.sendUniform("material.specular", specualar);
    progID.sendUniform("material.shininess", shininess);
}

void OpenGLPipeline::ApplyLightToShader(OpenGLShaderProgram &progID, Vector4F ambient, Vector4F diffuse, Vector4F specualar, float shininess)
{
    progID.sendUniform("light0.ambient", ambient);
    progID.sendUniform("light0.diffuse", diffuse);
    progID.sendUniform("light0.specular", specualar);
    progID.sendUniform("light0.shininess", shininess);
}

int OpenGLPipeline::GetVertexAttribId(OpenGLShaderProgram &progID)
{
    return (int)progID.getAttribLocation("vertexIn");
}

int OpenGLPipeline::GetColorAttribId(OpenGLShaderProgram &progID)
{
    return (int)progID.getAttribLocation("colorIn");
}

int OpenGLPipeline::GetNormalAttribId(OpenGLShaderProgram &progID)
{
    return (int)progID.getAttribLocation("normalIn");
}

int OpenGLPipeline::GetTextureAttribId(OpenGLShaderProgram &progID)
{
    return (int)progID.getAttribLocation("textureIn");
}

void OpenGLPipeline::bindMatrices(OpenGLShaderProgram& progID )
{
    Matrix4x4D MV = GetModelView();
    Matrix4x4D MVP = GetProjection().Top() * MV;

    Frustum& frustum = const_cast<Frustum&>(_Frustum.top());

    for (int i = Frustum::Left; i < Frustum::Frustrum_Plane_Count; ++i)
        MVP.ExtractPlane(frustum, i);

    progID.sendUniform("modelViewMatrix", MV.toFloat());
    progID.sendUniform("projectionMatrix", GetProjection().Top().toFloat());
    progID.sendUniform("modelViewProjectionMatrix", MVP.toFloat());
    progID.sendUniform("normalMatrix", GetNormal());
}

void OpenGLPipeline::ApplyLightBPMV(OpenGLShaderProgram& progID)
{
    char buf[64];
    sprintf(buf, "lightModelViewProjectionMatrix%d", _camID);
    progID.sendUniform(buf, GetBMVP().toFloat());
}

void OpenGLPipeline::bindLegacyMatrices()
{
    Matrix4x4D MV = GetModelView();
    Matrix4x4D MVP = GetProjection().Top() * MV;

    Frustum& frustum = const_cast<Frustum&>(_Frustum.top());

    for (int i = Frustum::Left; i < Frustum::Frustrum_Plane_Count; ++i)
        MVP.ExtractPlane(frustum, i);

    glMatrixMode(GL_PROJECTION);
    glLoadTransposeMatrixd(GetProjection().Top().ptr());
    glMatrixMode(GL_MODELVIEW);
    glLoadTransposeMatrixd(MV.ptr());
}

void OpenGLPipeline::Pop()
{
    _Frustum.pop();
    _m.Pop();
    _v.Pop();
}

void OpenGLPipeline::setLocation(GPSLocation loc)
{
    _location = loc;
}

void OpenGLPipeline::setViewZoom(float zoom)
{
    _zoom.m33 = 1.0f / zoom;
    GetView().Scale(1.0f, 1.0f, zoom);
}

const Frustum& OpenGLPipeline::GetFrustum() const
{
    return _Frustum.top();
}

OpenGLMatrixStack& OpenGLPipeline::GetProjection()
{
    return _p;
}

OpenGLMatrixStack& OpenGLPipeline::GetModel()
{
    return _m;
}

OpenGLMatrixStack& OpenGLPipeline::GetView()
{
    return _v;
}

Matrix4x4D OpenGLPipeline::GetModelView()
{
    return _v.Top() * _m.Top();
}

Matrix4x4D OpenGLPipeline::GetMVP()
{
    return _p.Top() * GetModelView();
}

Matrix4x4D OpenGLPipeline::GetBMVP()
{
    return (BiasMatrix4x4D::get() * _p.Top() * GetModelView());

}

Matrix3x3F OpenGLPipeline::GetNormal()
{
    return _zoom * GetModelView().Inverse().Transpose().toFloat();
}

