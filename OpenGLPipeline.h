#pragma once
#include "include_gl.h"
#include "OpenGLShaderProgram.h"
#include "Matrix4x4.h"
#include "vector3.h"
#include "vector4.h"
#include "GPSLocation.h"
#include <stack>

class OpenGLMatrixStack
{
public:

    enum MatrixType { None, View, Model, Projection };

    OpenGLMatrixStack(MatrixType type);

    const Matrix4x4D& Top() const;
    Matrix4x4D& Top();
    void Push();
    void Pop();
    void LookAt(const Vector3D& eye, const Vector3D& center, Vector3D up = Vector3D(0, 1, 0));
    void LookAt(const GPSLocation& eye, const GPSLocation& center, Vector3D up = Vector3D(0, 1, 0));
    void SetPerspective(double fovy, double aspect, double zNear, double zFar);
    void SetOrthographic(double zLeft, double zRight, double zTop, double zBottom, double zNear, double zFar);
    void ApplyShadowModelMatrix(const GPSLocation& loc, float heightAbovePlane, const Vector4D& lightDir, const QuarternionD& qPlane);
    void LoadIdentity();
    void PreScale(float x, float y, float z);
    void Scale(float x, float y, float z);
    void Translate(float x, float y, float z);
    void Rotate(float x, float y, float z);
    void TranslateLocation(const GPSLocation& loc);

    template<typename U>
    void Rotate(const Vector3<U> v)
    {
        Rotate(v.x, v.y, v.z);
    }

    template<typename U>
    void Translate(const Vector3<U> v)
    {
        Translate(v.x, v.y, v.z);
    }

private:
    std::stack<Matrix4x4D> _OpenGLMatrix;
    MatrixType _Type;
};

class OpenGLPipeline
{
public:
    OpenGLPipeline();

    static OpenGLPipeline& Get(unsigned int camID);
    void Push();
    static void ApplyMaterialToShader(OpenGLShaderProgram &progID,
                                      Vector4F ambient,
                                      Vector4F diffuse,
                                      Vector4F specualar,
                                      float shininess);
    static void ApplyLightToShader(OpenGLShaderProgram& progID,
                                   Vector4F ambient,
                                   Vector4F diffuse,
                                   Vector4F specualar,
                                   float shininess);

    static int GetVertexAttribId(OpenGLShaderProgram& progID);
    static int GetColorAttribId(OpenGLShaderProgram& progID);
    static int GetNormalAttribId(OpenGLShaderProgram& progID);
    static int GetTextureAttribId(OpenGLShaderProgram& progID);

    void bindLegacyMatrices();
    void bindMatrices(OpenGLShaderProgram& progID);
    void ApplyLightBPMV(OpenGLShaderProgram &progID);

    void Pop();
    void setLocation(GPSLocation loc);
    void setViewZoom(float zoom);
    const Frustum& GetFrustum() const;
    OpenGLMatrixStack& GetProjection();
    OpenGLMatrixStack& GetModel();
    OpenGLMatrixStack& GetView();
    Matrix4x4D GetModelView();
    Matrix4x4D GetMVP();
    Matrix4x4D GetBMVP();
    Matrix3x3F GetNormal();
private:
    std::stack<Frustum>	_Frustum;
    OpenGLMatrixStack _p;
    OpenGLMatrixStack _m;
    OpenGLMatrixStack _v;
    Matrix4x4F _zoom;
    unsigned int _camID = 0;
    GPSLocation _location;
};
