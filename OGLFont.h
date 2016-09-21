#pragma once

#include <include_gl.h>

class OGLFont
{
public:
	OGLFont(void);
	~OGLFont(void);

    void CreateBitmapFont( HDC hdc, const char *fontName, int fontSize,int fontStyle =FW_NORMAL);
    void RenderFont( int xpos, int ypos, const char *str );
	void SetOffSet( int dx, int dy);
	void OnSize(int dx, int dy );

	void RenderStart();
	void RenderEnd();

protected:
	GLuint m_Base;
	int m_dx;
	int m_dy;
	int m_SizeDx;
	int m_SizeDy;
};

class OGLFontContainer
{
public:
	OGLFontContainer( OGLFont& oglFont )
		: _oglFont( oglFont )
	{
		_oglFont.RenderStart();
	}

	~OGLFontContainer()
	{
		_oglFont.RenderEnd();
	}

private:
	OGLFont &_oglFont;
};
