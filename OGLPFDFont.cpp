#include "stdafx.h"
#include "OGLPFDFont.h"

#pragma comment(lib,"user32.lib")

OGLPFDFont::OGLPFDFont(void)
{
	m_dx = 0;
	m_dy = 0;
	m_Base=-1;
}

OGLPFDFont::~OGLPFDFont(void)
{
	if( m_Base != -1 )
	{
		glDeleteLists(m_Base, 96+32 );
	}
}

void OGLPFDFont::CreateBitmapFont( HDC hdc, const char *fontName, int fontSize, int fontStyle )
{
	bool bOwnDC = false;
	if( hdc==NULL )
	{
		bOwnDC =true;
		hdc = ::GetDC(NULL);
	}

	if( m_Base != -1 )
	{
		glDeleteLists(m_Base, 96+32 );
	}


	HFONT hFont;
	m_Base = glGenLists(96+32);

	if( _strcmpi(fontName,"symbol")== 0)
	{
        hFont = CreateFontA(fontSize, 0, 0, 0 , fontStyle, false, false, false,
							SYMBOL_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
								FF_DONTCARE|DEFAULT_PITCH, fontName);
	}
	else
	{
        hFont = CreateFontA(fontSize, 0, 0, 0, fontStyle, false, false, false,
			ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
								FF_DONTCARE|DEFAULT_PITCH, fontName);
	}

	if( !hFont)
		return;

	HFONT oldFont = (HFONT)::SelectObject( hdc, hFont );
	HPEN hPen = CreatePen( PS_SOLID, 1, RGB(255,255,255) );
	HPEN oldpen = (HPEN)::SelectObject( hdc, hPen );
	wglUseFontBitmaps( hdc, 32*0, 96+32, m_Base );
	::SelectObject( hdc, oldFont );
	::SelectObject( hdc, oldpen );
	::DeleteObject( hFont );
	::DeleteObject( hPen );

	if( bOwnDC )
		::ReleaseDC(NULL, hdc);
}

void OGLPFDFont::RenderFontWT( int xpos, int ypos, std::string str )
{
	if( m_Base ==-1 )
		return;

	glPushMatrix();
	glLoadIdentity();
	glTranslatef( 0, 0, 1 );

	glMatrixMode(GL_PROJECTION );
	glPushMatrix();

	glLoadIdentity();

    int width = ::GetSystemMetrics(SM_CXSCREEN);
    int height = ::GetSystemMetrics(SM_CYSCREEN);

    glOrtho(0, width, height,0,-1,1);

	glDisable(GL_TEXTURE_2D);
	glRasterPos2i( xpos + m_dx, ypos+m_dy);

	glPushAttrib(GL_LIST_BIT);
		glListBase(m_Base-32*0);
		glCallLists( (GLsizei)str.length(), GL_UNSIGNED_BYTE, str.c_str() );
	glPopAttrib();

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void OGLPFDFont::RenderFontNT(int xpos, int ypos, std::string str)
{
	//glColor3f(m_Color1,m_Color2,m_Color3);
	glRasterPos2i( xpos + m_dx, ypos+m_dy);

	glPushAttrib(GL_LIST_BIT);
		glListBase(m_Base-32*0);
		glCallLists( (GLsizei)str.length(), GL_UNSIGNED_BYTE, str.c_str() );
	glPopAttrib();
}

void OGLPFDFont::RenderFontNT(int xpos, int ypos, char ch)
{
	std::string str;
	char buf[8];
	sprintf(buf, "%c", ch);
	str.append(buf);
	RenderFontNT(xpos, ypos, str);
}

void OGLPFDFont::SetOffset( float dx, float dy)
{
	m_dx = dx;
	m_dy = dy;
}

