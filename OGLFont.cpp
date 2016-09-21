#include "stdafx.h"
#include "OGLFont.h"

OGLFont::OGLFont(void)
{
	m_SizeDx =0;
	m_SizeDy =0;
	m_dx = 0;
	m_dy = 0;
	m_Base=-1;
}

OGLFont::~OGLFont(void)
{
	if( m_Base != -1 )
	{
		glDeleteLists(m_Base, 96+32 );
	}
}

void OGLFont::CreateBitmapFont( HDC hdc, const char *fontName, int fontSize, int fontStyle )
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

	if( _stricmp(fontName,"symbol")== 0)
	{
        hFont = CreateFontA(fontSize, 0, 0,0 , fontStyle, false, false, false,
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
	wglUseFontBitmaps( hdc, 32*0, 96+32, m_Base );
	::SelectObject( hdc, oldFont );
	::DeleteObject( hFont);

	if( bOwnDC )
		::ReleaseDC(NULL, hdc);
}

void OGLFont::RenderStart()
{
	if( m_SizeDx ==0)
		return;

	if( m_SizeDy ==0 )
		return;

	if( m_Base ==-1 )
		return;

	glPushMatrix();
	glLoadIdentity();
	glTranslatef( 0, 0, 1 );

	glMatrixMode(GL_PROJECTION );
	glPushMatrix();

	glLoadIdentity();
	glOrtho(0, m_SizeDx, m_SizeDy,0,-1,1);

	glDisable(GL_TEXTURE_2D);

    glPushAttrib(GL_LIST_BIT);
	glListBase(m_Base-32*0);
}

void OGLFont::RenderEnd()
{
	glPopAttrib();
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void OGLFont::RenderFont( int xpos, int ypos, const char *str )
{
	glRasterPos2i( xpos+m_dx, ypos+m_dy );
	glCallLists( (GLsizei)strlen(str), GL_UNSIGNED_BYTE, str );
}

void OGLFont::SetOffSet( int dx, int dy)
{
	m_dx = dx;
	m_dy = dy;
}

void OGLFont::OnSize(int dx, int dy )
{
	m_SizeDx = dx;
	m_SizeDy = dy;
}
