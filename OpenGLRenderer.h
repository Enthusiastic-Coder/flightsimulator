#ifndef OPENGLRENDERER_H
#define OPENGLRENDERER_H

#include <include_gl.h>
#include "vector3.h"

class OpenGLTexture2D;
class OpenGLVertexBuffer;
class OpenGLShaderProgram;

class Renderer
{
public:
    Renderer();
    float dt;

    unsigned int camID;
    float fLightingFraction;

    static OpenGLShaderProgram nullShader;
    //Shader Variables
    void useProgram(OpenGLShaderProgram &progId );
    OpenGLShaderProgram& progId();

    //Mesh
    void bindMaterial(Vector3F &diffuseColor, Vector3F &specularColor, float shininess);
    void bindTexture(OpenGLTexture2D*);
    void unBindBuffers();
    void unBindTexture();

    enum VertexType
    {
        Vertex = 0x1,
        Color = 0x2,
        Normal = 0x4,
        TexCoord = 0x8
    };

    void bindVertex(VertexType attribType, int attribSize, const float* buffer);
    void bindVertex(VertexType attribType, int attribSize, OpenGLVertexBuffer* buffer);
    void bindIndex(const unsigned short *buffer);
    void bindIndex(OpenGLVertexBuffer* buffer);

    void setVertexCountOffset(int count, int offSet=0);
    void setUseIndex(bool bUseIndex);
    void setPrimitiveType(int type);
    void clearPrimitive();

    void Render();


private:
    void bindGenericBuffer(bool bLocal, int attribType, int attribSize, const float* buffer);

    bool _indexBufferIsLocal;
    const unsigned short* _indexBufferPtr = 0;
    OpenGLTexture2D* _pTexture2D = 0;
    OpenGLShaderProgram*_progId = &nullShader;

    bool _bElementsIndex;
    unsigned int _count =0;
    unsigned int _offset =0;
    int _primitiveType = 0;

    int _vertexAttribId = -1;
    int _colorAttribId = -1;
    int _normalAttribId = -1;
    int _texCoordAttribId = -1;
};

class OpenGLRenderer : public Renderer
{
};


#endif // OPENGLRENDERER_H
