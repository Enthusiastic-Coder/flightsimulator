#pragma once
#include <windows.h>
#include <include_gl.h>
#include <string>

class OGLPFDFont
{
public:
	OGLPFDFont(void);
	~OGLPFDFont(void);

    void CreateBitmapFont( HDC hdc, const char *fontName, int fontSize,int fontStyle =FW_NORMAL);
	void RenderFontWT( int xpos, int ypos, std::string str );
	void RenderFontNT(int xpos, int ypos, std::string str);
	void RenderFontNT(int xpos, int ypos, char ch);
	void SetOffset( float dx, float dy);

protected:
	GLuint m_Base;
	float m_dx;
	float m_dy;
};
