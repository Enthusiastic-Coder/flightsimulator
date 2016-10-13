#ifndef __OPENGL_PAINTER__H__
#define __OPENGL_PAINTER__H__

class OpenGLFontRenderer2D;
class OpenGLShaderProgram;

class OpenGLPainter
{
public:
    void selectFontRenderer(OpenGLFontRenderer2D *f);
    void selectPrimitiveShader(OpenGLShaderProgram* shader);

    void begin();

private:
    OpenGLFontRenderer2D* fontRenderer = 0 ;
    OpenGLShaderProgram* _primitiveShader = 0;

};


#endif //__OPENGL_PAINTER__H__
