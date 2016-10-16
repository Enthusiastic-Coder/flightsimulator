
// ChildView.cpp : implementation of the PFDView class
//

#include "stdafx.h"
#include "PFDView.h"
#include <iomanip>
#include <algorithm>
#include "OpenGLPainter.h"
#include "OpenGLFontRenderer2D.h"
#include "OpenGLPipeline.h"
#include "OpenGLRenderer.h"

#define _USE_MATH_DEFINES
#include <math.h>

#define PRIMITIVE2D(data) \
    data##.vertex2Ptr(), data##.vertex2Size()

template<class T> std::string format(std::string str, T val)
{
	char buf[128];
	sprintf(buf, str.c_str(), val);
	return buf;
}

PFDView::PFDView()
{
    _fPitch = 0.0f;
    _fBank = 0.0f;
    _fAirSpd = 0.0f;
    _fAlt = -1.0f;
    _fVSI = 0.0f;
    _fHdg = 360.0f;
	_CEN_X = 0.0f;
	_CEN_Y = 0.0f;
	_CX = 100.0f;
	_CY = 100.0f;
}

void PFDView::Initialise(HDC hdc)
{
    m_PfdHorizFreeFont.CreateBitmapFont( hdc, "Tahoma", 14, FW_NORMAL );//done
    m_PfdAirSpdFreeFont.CreateBitmapFont( hdc, "Arial", 15, FW_NORMAL );//done
    m_RadarAltBold.CreateBitmapFont( hdc, "Tahoma", 18, FW_NORMAL);//done
    m_AltLargeFreeFont.CreateBitmapFont( hdc, "Arial", 15, FW_NORMAL );
	m_AltSmallFreeFont.CreateBitmapFont( hdc, "Arial", 15, FW_NORMAL );

    _PfdHorizFreeFont.loadfile("fonts/Tahoma-9.png");
    _PfdAirSpdFreeFont.loadfile("fonts/Arial-10.png");
    _RadarAltBold.loadfile("fonts/Tahoma-11.png");
    _AltLargeFreeFont.loadfile("fonts/Arial-10.png");
    _AltSmallFreeFont.loadfile("fonts/Arial-10.png");

}


/////////////////////////////////////////////////////////////////////////////
// PFDView drawing

void PFDView::render(OpenGLPainter *painter, int cx, int cy)
{
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	glClear(GL_STENCIL_BUFFER_BIT);

	glMatrixMode( GL_PROJECTION  );
	glPushMatrix();
    glLoadIdentity();

    _CX = 100;
    _CY = 100;
	_CEN_X = cx/2;
    _CEN_Y = cy/2;

    glOrtho(0, cx, cy, 0,-1, 1);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	
	glTranslatef( _CEN_X, _CEN_Y, 0 );

	glDisable(GL_TEXTURE_2D );
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);

//    glBegin(GL_QUADS);
//        glColor3f(0,0,0);
//        glVertex2f( -_CX*1.2, _CY*1.4 );
//        glVertex2f( _CX*1.5, _CY*1.4 );
//        glVertex2f( _CX*1.5, -_CY*1.3 );
//        glVertex2f( -_CX*1.2, -_CY*1.3 );
//    glEnd();

    OpenGLPipeline& p = OpenGLPipeline::Get(painter->renderer()->camID);
    p.Push();
    OpenGLPipeline::applyScreenProjection(p, 0, 0, cx, cy);
    p.GetModel().Translate(_CEN_X, _CEN_Y,0);

    painter->beginPrimitive();
        painter->setPrimitiveColor({0,0,0,1});
        painter->fillQuad(-_CX*1.2, -_CY*1.3, 270, 270);

        painter->setPrimitiveColor({1,1,1,1});
        painter->drawQuad(-_CX*1.2, -_CY*1.3, 270, 270);
    painter->endPrimitive();

    DrawFlightModes(painter);
    DrawHorizon(painter);
    //_DrawFlightModes();
    //_DrawHorizon();
    _DrawSpd();
    _DrawAlt();
    _DrawVSI();
    _DrawHdg();

    painter->beginFont(&_PfdHorizFreeFont);
        painter->renderText(0,0, "TEST");
        painter->setFontColor({1,0,0,1});
        painter->renderText(0,20, "red text");
    painter->endFont();

    p.Pop();

    glDisable(GL_STENCIL_TEST);
    glColor3f(1,1,1);

    glMatrixMode( GL_PROJECTION  );
    glPopMatrix();
    glMatrixMode( GL_MODELVIEW  );
    glPopMatrix();

    glEnable(GL_DEPTH_TEST);

}

void PFDView::_DrawHorizon()
{
	glStencilFunc( GL_ALWAYS, 1, 0xFFFFFFFF );
	glStencilOp( GL_REPLACE, GL_REPLACE, GL_REPLACE );

//Stencil Horizon
	{ // Set stencil values.
		glColor3ub(1,0,0);
		glBegin(GL_TRIANGLE_FAN);
			for( float fAng = 0; fAng <= 360; fAng += 2.0f )
			{
				float fAngRad = fAng/180.0f * M_PI;
				float x = cos(fAngRad ) * _CX;
				float y = -sin(fAngRad ) * _CY;
				if( x < -65 ) x = -65;
				if( x > 65 ) x = 65;

				glVertex2f(x, y);
			}
		glEnd();
	}

	glStencilFunc( GL_EQUAL, 1, 0xFFFFFFFF );
	glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );

#define PIXEL_PER_PITCH 3.2f
#define PIXEL_PER_PITCH2 (PIXEL_PER_PITCH*0.8f)
#define PIXEL_PER_PITCH3 (PIXEL_PER_PITCH*0.5f)

	{//Horizon
		glPushMatrix();
        glRotatef( -_fBank, 0, 0, 1 );
		
		float dY;
		{
            if( _fPitch > 20 )
			{
                dY = 20 * PIXEL_PER_PITCH + (_fPitch-20)*PIXEL_PER_PITCH2;
			}
            else if( _fPitch  > -10 )
			{
                dY = _fPitch * PIXEL_PER_PITCH;
			}
			else
			{
                dY = -10 * PIXEL_PER_PITCH + (_fPitch+10) * PIXEL_PER_PITCH3;
			}
		}

		glTranslatef( 0, dY, 0 );

		glBegin(GL_QUADS);
			glColor3ub(30,140,242);//Sky
			glVertex2f(-100, -315 );
			glVertex2f(100, -315 );
			glVertex2f(100, 0 );
			glVertex2f(-100, 0 );

			glColor3ub(127,90,56);//Ground
			//glColor3ub(88,61,44);//Ground
			glVertex2f(-100, 0 );
			glVertex2f(100, 0 );
			glVertex2f(100, 315 );
			glVertex2f(-100, 315 );
		glEnd();

		{//Hdg Lines
			#define PIXEL_PER_HDG  2.6f

			glColor3f(1.0f,1.0f,1.0f);
			glBegin(GL_QUADS);
                int dH = (int)_fHdg % 10;
                int minHdg = (_fHdg - dH - 40);
				int maxHdg = minHdg + 80;
				int x;

				{
					for( int h = minHdg; h <= maxHdg; h+= 10 )
					{
                        x = (h - _fHdg) * PIXEL_PER_HDG;

						glVertex2f( x-1, 0 );
						glVertex2f( x, 0 );
						glVertex2f( x, 4 );
						glVertex2f( x-1, 4 );
					}
				}
			glEnd();
		}

		{ //-30 to -10
			
            float dy = 10 * PIXEL_PER_PITCH+10;
			float fLine;

			glBegin(GL_LINES);
				glColor3f(1.0f,1.0f,1.0f);
				fLine = dy + 5.0*PIXEL_PER_PITCH3;
				glVertex2f( -8, fLine );
				glVertex2f( 8, fLine );

				glColor3f(0.2f,0.9f,0.2f);
				glVertex2f( -8, fLine-1 );glVertex2f( -14, fLine-1 );
				glVertex2f( -8, fLine+1 );glVertex2f( -14, fLine+1 );

				glVertex2f( 8, fLine-1 );glVertex2f( 14, fLine-1 );
				glVertex2f( 8, fLine+1 );glVertex2f( 14, fLine+1 );

				glColor3f(1.0f,1.0f,1.0f);
				fLine = dy + 10.0*PIXEL_PER_PITCH3;
				glVertex2f( -20, fLine );
				glVertex2f( 20, fLine );

				fLine = dy + 20.0*PIXEL_PER_PITCH3;
				glVertex2f( -25, fLine );
				glVertex2f( 25, fLine );

				fLine = dy + 40.0*PIXEL_PER_PITCH3;
				glVertex2f( -30, fLine );
				glVertex2f( 30, fLine );

				fLine = dy + 60.0*PIXEL_PER_PITCH3;
				glVertex2f( -35, fLine );
				glVertex2f( 35, fLine );

				fLine = dy + 80.0*PIXEL_PER_PITCH3;
				glVertex2f( -40, fLine );
				glVertex2f( 40, fLine );
			glEnd();

            fLine = dy;
			m_PfdHorizFreeFont.RenderFontNT( -30, fLine-6, "10" );
			m_PfdHorizFreeFont.RenderFontNT( 20, fLine-6, "10");

			fLine = dy + 10.0*PIXEL_PER_PITCH3; //-20 Degrees
			m_PfdHorizFreeFont.RenderFontNT( -35, fLine-6, "20" );
			m_PfdHorizFreeFont.RenderFontNT( 25, fLine-6, "20");

			fLine = dy + 20.0*PIXEL_PER_PITCH3;//-30 degrees
			m_PfdHorizFreeFont.RenderFontNT( -40, fLine-6, "30" );
			m_PfdHorizFreeFont.RenderFontNT( 30, fLine-6, "30");

			fLine = dy + 40.0*PIXEL_PER_PITCH3;//-50 degrees
			m_PfdHorizFreeFont.RenderFontNT( -45, fLine-6, "50" );
			m_PfdHorizFreeFont.RenderFontNT( 35, fLine-6, "50");

			fLine = dy + 60.0*PIXEL_PER_PITCH3;//-70 degrees
			m_PfdHorizFreeFont.RenderFontNT( -50, fLine-6, "70" );
			m_PfdHorizFreeFont.RenderFontNT( 40, fLine-6, "70");

			fLine = dy + 80.0*PIXEL_PER_PITCH3;//-90 degrees
			m_PfdHorizFreeFont.RenderFontNT( -55, fLine-6, "90" );
			m_PfdHorizFreeFont.RenderFontNT( 45, fLine-6, "90");
		}

		{// -10 to 20
			glBegin(GL_LINES);
				glColor3f(1.0f,1.0f,1.0f);

				glVertex2d(	-100, 0 );
				glVertex2d(	100, 0 );

				float fLine;
				for( int iPitch = -10; iPitch <= 20; iPitch+=10)
				{
					fLine = -iPitch * PIXEL_PER_PITCH;

					if(iPitch )
					{	
						glVertex2f( -16, fLine );
						glVertex2f( 16, fLine );					
					}

					if(iPitch == 20) continue;
		
					fLine = -(iPitch+2.5) * PIXEL_PER_PITCH;
					glVertex2f( -4, fLine );
					glVertex2f( 4, fLine );

					fLine = -(iPitch+5) * PIXEL_PER_PITCH;
					glVertex2f( -8, fLine );
					glVertex2f( 8, fLine );

					fLine = -(iPitch+	7.5) * PIXEL_PER_PITCH;
					glVertex2f( -4, fLine );
					glVertex2f( 4, fLine );
				}
			glEnd();
		}

		{ // 0 to 20 Text
			glColor3f(1.0f,1.0f,1.0f);
			std::string str;
			float fLine = 0;
			for( int iPitch = 0; iPitch <= 20; iPitch+=10)
			{
				if( iPitch == 0 ) continue;

                fLine = -iPitch * PIXEL_PER_PITCH +10;
				str = format( "%d", abs(iPitch) );
	
				if( iPitch == 20 )
				{
					m_PfdHorizFreeFont.RenderFontNT( -35, fLine-6, str );
					m_PfdHorizFreeFont.RenderFontNT( 25, fLine-6, str);
				}
				else
				{
					m_PfdHorizFreeFont.RenderFontNT( -30, fLine-6, str );
					m_PfdHorizFreeFont.RenderFontNT( 20, fLine-6, str);
				}
			}
		}

		//20 to 30
		{
			float dy = -20 * PIXEL_PER_PITCH;
			float fLine;
			glBegin(GL_LINES);
				glColor3f(1.0f,1.0f,1.0f);
				fLine = dy - 2.5*PIXEL_PER_PITCH2;
				glVertex2f( -4, fLine ); //22.5
				glVertex2f( 4, fLine );

				fLine = dy - 5.0*PIXEL_PER_PITCH2;
				glVertex2f( -8, fLine ); //25.0
				glVertex2f( 8, fLine );

				fLine = dy - 7.5*PIXEL_PER_PITCH2;
				glVertex2f( -4, fLine ); //27.5
				glVertex2f( 4, fLine );

				fLine = dy - 10.0*PIXEL_PER_PITCH2;
				glVertex2f( -20, fLine ); //30
				glVertex2f( 20, fLine );

				glColor3f(0.2f,0.9f,0.2f);

				glVertex2f( -25, fLine-1 ); glVertex2f( -20, fLine-1 );
				glVertex2f( -25, fLine+1 ); glVertex2f( -20, fLine+1 );

				glVertex2f( 25, fLine-1 ); glVertex2f( 20, fLine-1 );
				glVertex2f( 25, fLine+1 ); glVertex2f( 20, fLine+1 );


			glEnd();

            fLine = dy - 10.0*PIXEL_PER_PITCH2+10;
			glColor3f(1.0f,1.0f,1.0f);
			m_PfdHorizFreeFont.RenderFontNT( -40, fLine-6, "30" );
			m_PfdHorizFreeFont.RenderFontNT( 30, fLine-6, "30");
		}

		{ // 50 to 90
			//for( int iPitch = 50; iPitch <= 90; iPitch+=20)
			float fLine;
            float dy = -20 * PIXEL_PER_PITCH - 30*PIXEL_PER_PITCH2+10;
			glBegin(GL_LINES);
				glColor3f(1.0f,1.0f,1.0f);

				fLine = dy;
				glVertex2f( -30, fLine );
				glVertex2f( 30, fLine );

				fLine = dy - 20.0*PIXEL_PER_PITCH2;
				glVertex2f( -35, fLine );
				glVertex2f( 35, fLine );

				fLine = dy - 40.0*PIXEL_PER_PITCH2;
				glVertex2f( -40, fLine );
				glVertex2f( 40, fLine );
			glEnd();

            fLine = dy+10;
			m_PfdHorizFreeFont.RenderFontNT( -45, fLine-6, "50" );
			m_PfdHorizFreeFont.RenderFontNT( 35, fLine-6, "50");

            fLine = dy - 20.0*PIXEL_PER_PITCH2+10; //-20 Degrees
			m_PfdHorizFreeFont.RenderFontNT( -50, fLine-6, "70" );
			m_PfdHorizFreeFont.RenderFontNT( 40, fLine-6, "70");

            fLine = dy - 40.0*PIXEL_PER_PITCH2+10;//-30 degrees
			m_PfdHorizFreeFont.RenderFontNT( -55, fLine-6, "90" );
			m_PfdHorizFreeFont.RenderFontNT( 45, fLine-6, "90");		}

		{//magenta Fly Up warning arrows from deep dive
			glColor3f(1.0f,0.0f,1.0f);
			float fLine;
			float fLine2;
			float dy = 10 * PIXEL_PER_PITCH;
			for( int iPitch = -40; iPitch >= -80; iPitch -= 20)
			{
				glBegin(GL_LINE_LOOP);
					float dy2 = dy -(iPitch+10) * PIXEL_PER_PITCH3;
					fLine = dy2 + 5 * PIXEL_PER_PITCH3;//From
					fLine2 = dy2 - 2.5 * PIXEL_PER_PITCH3;//To

					glVertex2f(-15, fLine );
					glVertex2f(-10, fLine );
					glVertex2f( 0, fLine2 );
					glVertex2f( 10, fLine );
					glVertex2f( 15, fLine );
					glVertex2f( 0, fLine2-5 );
					glVertex2f( -15, fLine );
				glEnd();
			}
		}

		{//magenta Fly Down warning arrows from steep climb
			glColor3f(1.0f,0.0f,1.0f);
			float fLine;
			float fLine2;
			float dy = -20 * PIXEL_PER_PITCH;
			for( int iPitch = 40; iPitch <= 80; iPitch += 20)
			{
				glBegin(GL_LINE_LOOP);
					float dy2 = dy -(iPitch-20) * PIXEL_PER_PITCH2;
					fLine = dy2 - 5 * PIXEL_PER_PITCH2;//From
					fLine2 = dy2 + 5.0 * PIXEL_PER_PITCH2;//To

					glVertex2f(-10, fLine );
					glVertex2f(-15, fLine );
					glVertex2f( 0, fLine2 );
					glVertex2f( 15, fLine );
					glVertex2f( 10, fLine );
					glVertex2f( 0, fLine2-5 );
					glVertex2f( -10, fLine );
				glEnd();
			}
		}

		glPopMatrix();
	}

	{ //Draw surrounding horizon bits
		glPushMatrix();
            glRotatef( -_fBank, 0, 0, 1 );

			float fy;
            fy = std::max( _fPitch * PIXEL_PER_PITCH,
                        PIXEL_PER_PITCH *( _fAlt / 80.0f * 10 + _fPitch) );
			//fy = min( _CEN_Y-105, fy );
			//fy = max( -_CEN_Y+100, fy );
            fy = std::min( 60.0f, fy );
            fy = std::max( -60.0f, fy );
			
			glBegin(GL_QUADS);
				glColor3ub(30,140,242);//Sky
				glVertex2f(-100, -215 );
				glVertex2f(100, -215 );
				glVertex2f(100, -60 );
				glVertex2f(-100, -60 );
				//glVertex2f(100, -_CEN_Y+100 );
				//glVertex2f(-100, -_CEN_Y+100 );

				glColor3ub(127,90,56);//Ground
				//glColor3ub(88,61,44);//Ground
				glVertex2f(-100, fy );
				glVertex2f(100, fy );
				glVertex2f(100, 215 );
				glVertex2f(-100, 215 );
			glEnd();

			glBegin(GL_LINES);
				glColor3f(1.0f,1.0f,1.0f) ;
				//glVertex2f( -100, -_CEN_Y+100  );
				//glVertex2f( 100, -_CEN_Y+100  );
				glVertex2f( -100, -60  );
				glVertex2f( 100, -60  );

				glVertex2f( -100, fy  );
				glVertex2f( 100, fy  );
			glEnd();

            if( _fAlt < 3.0f)
			{
                float dy = _fPitch * PIXEL_PER_PITCH;
				//if( dy +4 < _CEN_Y-100 && dy > -_CEN_Y+100 )
				if( dy +4 < 60 && dy > -60 )
				{
					glPushMatrix();
                        glTranslatef( 0, _fPitch * PIXEL_PER_PITCH, 0 );
						{//Hdg Lines
							#define PIXEL_PER_HDG  2.6f

							glColor3f(1.0f,1.0f,1.0f);
							glBegin(GL_QUADS);
                                int dH = (int)_fHdg % 10;
                                int minHdg = (_fHdg - dH - 40);
								int maxHdg = minHdg + 80;
								int x;

								for( int h = minHdg; h <= maxHdg; h+= 10 )
								{
                                    x = (h - _fHdg) * PIXEL_PER_HDG;

									glVertex2f( x-1, 0 );
									glVertex2f( x, 0 );
									glVertex2f( x, 4 );
									glVertex2f( x-1, 4 );
								}
								
							glEnd();
						}
					glPopMatrix();
				}
			}

			int dy = 210;
			glBegin(GL_LINE_LOOP);
				glVertex2f( 0, -dy/2+5 );
				glVertex2f( 4, -dy/2+11 );
				glVertex2f( -4, -dy/2+11 );
			glEnd();

			//Rudder Assymtry
			glBegin(GL_LINE_LOOP);
				glVertex2f( 4, -dy/2+11 );
				glVertex2f( 6, -dy/2+14 );
				glVertex2f( -6, -dy/2+14 );
				glVertex2f( -4, -dy/2+11 );
			glEnd();

			
			// When on ground show surrounding square and joystick position and ground tracking if ILS available
            if( _fAlt < 3.0f )
			{ 
				glColor3f(1.0f, 1.0f, 1.0f);
				glBegin(GL_LINES);
				glVertex2f( -55, -45 );	glVertex2f( -45, -45 );
				glVertex2f( -55, -45 ); glVertex2f( -55, -35 );

				glVertex2f( -55, 45 );	glVertex2f( -45, 45 );
				glVertex2f( -55, 45 ); glVertex2f( -55, 35 );

				glVertex2f( 55, -45 );	glVertex2f( 45, -45 );
				glVertex2f( 55, -45 ); glVertex2f( 55, -35 );

				glVertex2f( 55, 45 );	glVertex2f( 45, 45 );
				glVertex2f( 55, 45 ); glVertex2f( 55, 35 );

				glEnd();
			}

			glPopMatrix();

            if( _fAlt < 2500 )
			{
				std::string sAlt;
                if( _fAlt <= -5 )
				{
                    sAlt = format( "%d", (int)_fAlt - (int)_fAlt % 5  );
				}
                else if( _fAlt <= 5 )
				{
                    sAlt = format( "%d", (int)_fAlt );
				}
                else if( _fAlt <= 50 )
				{
                    sAlt = format( "%d", (int)_fAlt - (int)_fAlt % 5  );
				}
				else
				{
                    sAlt = format( "%d", (int)_fAlt - (int)_fAlt % 10  );
				}

                if( _fAlt <= 400 )
					glColor3f( 0.9f, 0.9f, 0.2f );
				else
					glColor3f( 0.2f, 0.9f, 0.2f );

                int yPos = 80;

                if( _fAlt < -99 )
					m_RadarAltBold.RenderFontNT( -13, yPos, sAlt );
                else if( _fAlt < -9 )
					m_RadarAltBold.RenderFontNT( -10, yPos, sAlt );
                else if( _fAlt < 0 )
					m_RadarAltBold.RenderFontNT( -5, yPos, sAlt );
                else if( _fAlt < 10 )
					m_RadarAltBold.RenderFontNT( -2, yPos, sAlt );
                else if( _fAlt < 100 )
					m_RadarAltBold.RenderFontNT( -5, yPos, sAlt );
                else if( _fAlt < 1000 )
					m_RadarAltBold.RenderFontNT( -9, yPos, sAlt );
				else
					m_RadarAltBold.RenderFontNT( -13, yPos, sAlt );
			}

	}

	{ //Aircraft on Horizon

		glBegin(GL_QUADS); 
			//Outer center left yellow
			glColor3f(1, 1, 0 );
			glVertex2f( -63, -3 );
			glVertex2f( -40, -3 );
			glVertex2f( -40, 3 );
			glVertex2f( -63, 3 );

			glVertex2f( -40, -3 );
			glVertex2f( -35, -3 );
			glVertex2f( -35, 10 );
			glVertex2f( -40, 10 );

			//Outer center right yellow
			glVertex2f( 63, -3 );
			glVertex2f( 40, -3 );
			glVertex2f( 40, 3 );
			glVertex2f( 63, 3 );

			glVertex2f( 40, -3 );
			glVertex2f( 35, -3 );
			glVertex2f( 35, 10 );
			glVertex2f( 40, 10 );

			//Center
			glVertex2d( -3, -3 );
			glVertex2d( 3, -3 );
			glVertex2d( 3, 3 );
			glVertex2d( -3, 3 );
		
			glColor3f(0.0f,0.0f,0.0f); // Center blackLeft
			glVertex2d( -62, -2 );
			glVertex2d( -36, -2 );
			glVertex2d( -36, 2 );
			glVertex2d( -62, 2 );

			glVertex2d( -39, -2 );
			glVertex2d( -36, -2 );
			glVertex2d( -36, 9 );
			glVertex2d( -39, 9 );

			//Center Right
			glVertex2d( 36, -2 );
			glVertex2d( 62, -2 );
			glVertex2d( 62, 2 );
			glVertex2d( 36, 2 );

			glVertex2d( 36, -2 );
			glVertex2d( 39, -2 );
			glVertex2d( 39, 9 );
			glVertex2d( 36, 9 );

			//Center
			glVertex2d( -2, -2 );
			glVertex2d( 2, -2 );
			glVertex2d( 2, 2 );
			glVertex2d( -2, 2 );
		glEnd();


		glStencilFunc( GL_ALWAYS, 2, 0xFFFFFFFF );
		glStencilOp( GL_REPLACE, GL_REPLACE, GL_REPLACE );

		//Center Yellow marker at 0 degrees around perimter of horizon
		glColor3f(1.0f,1.0f,0);
		glBegin(GL_LINE_LOOP);
			glVertex2f( -4, -84 - 25 );
			glVertex2f( 4, -84 - 25 );
			glVertex2f( 0, -74 - 25 );
		glEnd();

		glColor3f(1.0f,1.0f, 1.0f);
		glBegin(GL_LINE_STRIP);
			for( int iAng = -30; iAng <= 30; iAng ++ )
			{
				float fAngRad = iAng/180.0f * M_PI;
				float x = sin(fAngRad ) * _CX;
				float y = -cos(fAngRad ) * _CY;

				glVertex2f(x, y);
			}
		glEnd();

		// Markers around Horizon denoating 10, 20, 30, and 45 degrees.
		for( int iAng = -30; iAng < 0; iAng +=10 )
		{
			int dA = 1;
			int dH = 5;

			if( iAng == -30 )
			{
				dA = 1.5;
				dH = 10;
			}

			float fAngRad1 = (iAng-dA)/180.0f * M_PI;
			float fAngRad2 = (iAng+dA)/180.0f * M_PI;

			float fSinRad1 = sin(fAngRad1 );
			float fCosRad1 = -cos(fAngRad1 );

			float fSinRad2 = sin(fAngRad2 );
			float fCosRad2 = -cos(fAngRad2 );

			float x1 = fSinRad1 * _CX;
			float y1 = fCosRad1 * _CY;
			float x2 = fSinRad2 * _CX;
			float y2 = fCosRad2 * _CY;

			float x3 = fSinRad2 * (_CX+dH);
			float y3 = fCosRad2 * (_CY+dH);
			float x4 = fSinRad1 * (_CX+dH);
			float y4 = fCosRad1 * (_CY+dH);

			glBegin(GL_LINE_LOOP);
				glVertex2f(x1, y1);
				glVertex2f(x2, y2);
				glVertex2f(x3, y3);
				glVertex2f(x4, y4);
			glEnd();
			glBegin(GL_LINE_LOOP);
				glVertex2f(-x1, y1);
				glVertex2f(-x2, y2);
				glVertex2f(-x3, y3);
				glVertex2f(-x4, y4);
			glEnd();
		}

		{ // -45 and 45 markers around edge
			float fsin = sin(M_PI/4);
			float fcos = -cos(M_PI/4);

			float x1 = fsin * _CX;
			float y1 = fcos * _CY;

			float x2 = fsin * (_CX+10);
			float y2 = fcos * (_CY+10);

			glBegin(GL_LINES);
				glVertex2f(x1, y1 ); 
				glVertex2f(x2, y2 );

				glVertex2f(-x1, y1 ); 
				glVertex2f(-x2, y2 );
			glEnd();
		}
	}
}

void PFDView::_DrawAlt()
{ 
	// Altitude

	glStencilOp( GL_REPLACE, GL_REPLACE, GL_REPLACE );	
	glStencilFunc( GL_ALWAYS, 3, 0xFFFFFFFF );
	{ //Set stencil bits
		glBegin(GL_QUADS);
		
			glColor3f(0.0f, 0.0f, 0.0f );
			glVertex2f(125, -80);
			glVertex2f(65, -80);
			glVertex2f(65, 80);
			glVertex2f(125, 80);

		glEnd();

		glStencilFunc( GL_ALWAYS, 4, 0xFFFFFFFF );
		glBegin( GL_QUADS );
			glColor3f(0.0f, 0.0f, 0.0f );
			glVertex2f(85, -7);
			glVertex2f(110, -7);
			glVertex2f(110, 7);
			glVertex2f(85, 7);

			glVertex2f(110, -12);
			glVertex2f(125, -12);
			glVertex2f(125, 13);
			glVertex2f(110, 13);
		glEnd();
	}

    _DrawScrollAlt();/****************************/

	glStencilFunc( GL_ALWAYS, 4, 0xFFFFFFFF );
	glStencilOp( GL_REPLACE, GL_REPLACE, GL_REPLACE );
	glColor3f(1.0f,1.0f,0.0f);
	glBegin(GL_LINE_STRIP); //Outline yellow of alt read out
		glVertex2f(85, -7);
		glVertex2f(110, -7);
		glVertex2f(110, -12);
		glVertex2f(124, -12);
		glVertex2f(124, 13);
		glVertex2f(110, 13);
		glVertex2f(110, 7);
		glVertex2f(85, 7);
	glEnd();

	glStencilFunc( GL_EQUAL, 3, 0xFFFFFFFF );
	glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );

	//Altitude Bar
	glColor3f(0.4f,0.4f,0.4f );
	glBegin(GL_QUADS);
		glVertex2f( 85, -_CEN_Y + 80 );
		glVertex2f( 110, -_CEN_Y + 80 );
		glVertex2f( 110, _CEN_Y - 80 );
		glVertex2f( 85, _CEN_Y - 80 );
	glEnd();

	//Altitude
	{
#define ALT_RANGE 1000
#define PIXEL_PER_ALT_FEET 0.13333f
        int dA = (int)_fAlt % 500;
        float minAlt = _fAlt - dA - ALT_RANGE;
		float maxAlt = minAlt + 2 * ALT_RANGE;
		float ymax = (dA + ALT_RANGE) * PIXEL_PER_ALT_FEET;
		float y;

		glColor3f(1.0f, 1.0f,1.0f);
		glBegin(GL_TRIANGLES);
		for( int x = (int)minAlt; x <= maxAlt; x+= 500)
		{
			y = (minAlt - x) * PIXEL_PER_ALT_FEET + ymax;
			glVertex2f( 82, y-2 );
			glVertex2f( 85, y );
			glVertex2f( 82, y+2 );
		}
		glEnd();

		glBegin(GL_QUADS);
		for( int x = (int)minAlt; x <= maxAlt; x+= 100)
		{
			y = (minAlt - x) * PIXEL_PER_ALT_FEET + ymax;
			glVertex2f( 107, y-1 );
			glVertex2f( 110, y-1 );
			glVertex2f( 110, y+1 );
			glVertex2f( 107, y+1 );
		}
		glEnd();

		for( int x = (int)minAlt; x <= maxAlt; x+= 500)
		{
            y = (minAlt - x) * PIXEL_PER_ALT_FEET + ymax + 10;
			std::string strAlt;
			strAlt = format( "%03.0f", abs(x)/100.0 );
			m_AltLargeFreeFont.RenderFontNT( 87, y-8, strAlt );
		}
	}

	glColor3f(1.0f, 1.0f,1.0f);
	glBegin(GL_LINES);	
		glVertex2f(110, -_CEN_Y + 80 );
		glVertex2f(110, _CEN_Y - 80 );
	glEnd();
	glBegin(GL_QUADS); //Yellow line to right
		glColor3f(1.0f,1.0f,0.0f);
		glVertex2f( 67, -1 );
		glVertex2f( 80, -1 );
		glVertex2f( 80, 1 );
		glVertex2f( 67, 1 );
	glEnd();

    //return;

	glStencilFunc( GL_ALWAYS, 3, 0xFFFFFFFF );
	glBegin(GL_QUADS);	//White ends of altitude bar
		glColor3f(1.0f, 1.0f,1.0f);
		glVertex2f(85, _CEN_Y - 80 );
		glVertex2f(120, _CEN_Y - 80 );
		glVertex2f(120, _CEN_Y - 81 );
		glVertex2f(85, _CEN_Y - 81 );
    glEnd();
}

void PFDView::BuildAltTape(char buffer[][32], int line_count, int centralAlt, bool bIncreasing)
{
	char altBuf[16];
	sprintf_s( altBuf, 16, "%5d", abs(centralAlt) );

	if( altBuf[3] == ' ' ) altBuf[3]= '0';

	int altJump;

	if( bIncreasing )
		altJump = 20;
	else
		altJump = -20;

	strcpy_s(buffer[0], sizeof(buffer[0])/sizeof(char), altBuf );

	for( int i = 1; i < line_count; i++ )
	{
		strcpy_s( buffer[i], sizeof(buffer[i])/sizeof(char), buffer[i-1] );

		bool bAltPositive = (centralAlt + altJump * i) >= 0;
		
		for( int j=0; j < 3; j++ )
		{
			int digit;
			
			if( buffer[i][j] == ' ' )
				digit = 0;
			else
				digit = buffer[i][j] - '0';
			
			if(bAltPositive == bIncreasing)
			{
				if( digit == 9 ) 
					digit = 0;
				else
					digit++;
			}
			else
			{
				if( digit == 0 ) 
					digit = 9;
				else
					digit--;
			}

			buffer[i][j] = digit + '0';
		}
		int hundred;
		
		if( buffer[i][3] ==' ' )
			hundred = 0;
		else
			hundred = buffer[i][3] - '0';
		
		if(bAltPositive == bIncreasing)
		{
			if( hundred == 8 )
				hundred = 0;
			else
				hundred += 2;
		}
		else
		{
			if( hundred == 0 )
				hundred = 8;
			else
				hundred -= 2;
		}

		buffer[i][3] = hundred + '0';
	}
}

void PFDView::_DrawScrollAlt()
{
#define NUMBER_OF_ALT_LINES	4
	glStencilFunc( GL_EQUAL, 4, 0xFFFFFFFF );
	glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );

    if( _fAlt <= 400 )
		glColor3f( 0.9f, 0.9f, 0.2f );
	else
		glColor3f( 0.2f, 0.9f, 0.2f );

    int dA = (int)_fAlt % 20;
    int centralAlt = (int)_fAlt - dA;

	char rotaryAltBuf1[NUMBER_OF_ALT_LINES][32];
	char rotaryAltBuf2[NUMBER_OF_ALT_LINES][32];

	BuildAltTape( rotaryAltBuf1, NUMBER_OF_ALT_LINES, centralAlt, true);
	BuildAltTape( rotaryAltBuf2, NUMBER_OF_ALT_LINES, centralAlt, false);

#define SCROLL_TEXT_PIXEL_PER_ALT 0.5f

    float dPixel = dA * SCROLL_TEXT_PIXEL_PER_ALT - 7 + 10;

	m_AltSmallFreeFont.RenderFontNT( 111, dPixel-40*SCROLL_TEXT_PIXEL_PER_ALT, &rotaryAltBuf1[2][3] );
	m_AltSmallFreeFont.RenderFontNT( 111, dPixel-20*SCROLL_TEXT_PIXEL_PER_ALT, &rotaryAltBuf1[1][3] );
	m_AltSmallFreeFont.RenderFontNT( 111, dPixel, &rotaryAltBuf1[0][3] );
	m_AltSmallFreeFont.RenderFontNT( 111, dPixel+20*SCROLL_TEXT_PIXEL_PER_ALT, &rotaryAltBuf2[1][3] );
	m_AltSmallFreeFont.RenderFontNT( 111, dPixel+40*SCROLL_TEXT_PIXEL_PER_ALT, &rotaryAltBuf2[2][3] );

	float yPosNoChange[3];
    yPosNoChange[0] = -10 - 20 * SCROLL_TEXT_PIXEL_PER_ALT - 0.5+2 + 10;
    yPosNoChange[1] = -7 -0.5 +2  + 10;
    yPosNoChange[2] = -5 + 20 * SCROLL_TEXT_PIXEL_PER_ALT - 0.5 + 2 + 10;

	if( centralAlt < 0 )
	{
		yPosNoChange[0]++;
		yPosNoChange[1]++;
		yPosNoChange[2]++;
	}

    bool bDrawn[5] = {false};

	if( rotaryAltBuf1[0][3] == '8' )
	{
		float yPos[3];
        yPos[0] = dPixel+SCROLL_TEXT_PIXEL_PER_ALT-10 +10;
        yPos[1] = dPixel+20*SCROLL_TEXT_PIXEL_PER_ALT-9+10;
        yPos[2] = dPixel+40*SCROLL_TEXT_PIXEL_PER_ALT-9+10;

		if( centralAlt < 0 )
		{
            yPos[0] -= 4;
            yPos[1] -= 4;
            yPos[2] -= 4;
		}

		bDrawn[2] = true;
		m_AltLargeFreeFont.RenderFontNT( 102, yPos[0], rotaryAltBuf1[1][2] );
		m_AltLargeFreeFont.RenderFontNT( 102, yPos[1], rotaryAltBuf1[0][2] );
		m_AltLargeFreeFont.RenderFontNT( 102, yPos[2], rotaryAltBuf2[1][2] );

		if( rotaryAltBuf1[0][2] == '9' )
		{
			bDrawn[1] = true;
			m_AltLargeFreeFont.RenderFontNT( 94, yPos[0], rotaryAltBuf1[1][1] );
			m_AltLargeFreeFont.RenderFontNT( 94, yPos[1], rotaryAltBuf1[0][1] );
			m_AltLargeFreeFont.RenderFontNT( 94, yPos[2], rotaryAltBuf2[1][1] );

			if( rotaryAltBuf1[0][1] == '9' )
			{
				bDrawn[0] = true;
				m_AltLargeFreeFont.RenderFontNT( 86, yPos[0], rotaryAltBuf1[1][0] );
				m_AltLargeFreeFont.RenderFontNT( 86, yPos[1], rotaryAltBuf1[0][0] );
				m_AltLargeFreeFont.RenderFontNT( 86, yPos[2], rotaryAltBuf2[1][0] );
			}
		}
	}
	
	if( !bDrawn[0] )
	{
		m_AltLargeFreeFont.RenderFontNT( 86, yPosNoChange[0], rotaryAltBuf1[1][0] );
		m_AltLargeFreeFont.RenderFontNT( 86, yPosNoChange[1], rotaryAltBuf1[0][0] );
		m_AltLargeFreeFont.RenderFontNT( 86, yPosNoChange[2], rotaryAltBuf2[1][0] );
	}

	if( !bDrawn[1] )
	{
		m_AltLargeFreeFont.RenderFontNT( 94, yPosNoChange[0], rotaryAltBuf1[1][1] );
		m_AltLargeFreeFont.RenderFontNT( 94, yPosNoChange[1], rotaryAltBuf1[0][1] );
		m_AltLargeFreeFont.RenderFontNT( 94, yPosNoChange[2], rotaryAltBuf2[1][1] );
	}

	if( !bDrawn[2] )
	{
		m_AltLargeFreeFont.RenderFontNT( 102, yPosNoChange[0], rotaryAltBuf1[1][2] );
		m_AltLargeFreeFont.RenderFontNT( 102, yPosNoChange[1], rotaryAltBuf1[0][2] );
		m_AltLargeFreeFont.RenderFontNT( 102, yPosNoChange[2], rotaryAltBuf2[1][2] );
	}
}

void PFDView::_DrawVSI()
{
	//VSI

	glStencilFunc( GL_ALWAYS, 3, 0xFFFFFFFF );
	glStencilOp( GL_REPLACE, GL_REPLACE, GL_REPLACE );
	glColor3f(0.4f, 0.4f, 0.4f );
	glBegin( GL_POLYGON);
		glVertex2f( 125, -80 );
		glVertex2f( 140, -50 );
		glVertex2f( 140, 50 );
		glVertex2f( 125, 80 );
	glEnd();

#define PIXEL_PER_VSI_1000		0.05f

	glStencilFunc( GL_EQUAL, 3, 0xFFFFFFFF );
	glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP);

	glBegin(GL_QUADS);

        float fVsi = fabs(_fVSI);
		float dy = 0.0f;
		glColor3f( 0.1f, 1.0f, 0.1f );

		if( fVsi < 1050 )
		{
            dy = -PIXEL_PER_VSI_1000 * _fVSI;

			glVertex2f( 140, -2 );
			glVertex2f( 140, 2 );
			glVertex2f( 125, dy + 2 );
			glVertex2f( 125, dy - 2 );
		}
		else
		{		 
			if( fVsi > 6000 )
			{
				glColor3f( 0.9f, 0.9f, 0.2f );
			}

			dy = PIXEL_PER_VSI_1000 * (1000 +  0.5f * (fVsi-1000));

			glVertex2f( 140, -2 );
			glVertex2f( 140, 2 );
			dy = PIXEL_PER_VSI_1000 * (1000 +  0.5f * (fVsi-1000));

            if( _fVSI > 0 )
			{
				dy = -dy;
			}
			
			float resDy = dy;
			if( resDy > 75 ) resDy = 75;
			if( resDy < -75 ) resDy = -75;
			
			glVertex2f( 125, resDy+2 );
			glVertex2f( 125, resDy-2 );
		}
	glEnd();
	
	glStencilFunc( GL_ALWAYS, 3, 0xFFFFFFFF );
	if( fVsi > 100 )
	{
		if( dy > 75 ) dy = 75;
		if( dy < -75) dy = -75;
		float dy_vsi = 0.0f;
		float end_x = 142;
		glBegin(GL_QUADS);
		glColor3f(0.0f,0.0f,0.0f );
            if( _fVSI < 0 )
			{
				dy += 5;
				dy_vsi = 12;
			}
			else
			{
				dy -= 5;
				dy_vsi = -12;
			}
			
			glVertex2f(125, dy + dy_vsi );
			glVertex2f(end_x, dy + dy_vsi );
			glVertex2f(end_x, dy );
			glVertex2f(125, dy );
		glEnd();
		

		if( fVsi > 6000 )
			glColor3f( 0.9f, 0.9f, 0.2f );
		else
			glColor3f(0.1f,0.9f,0.1f);

		std::string strVSI;
		char buf[32];
		sprintf(buf, "%2.0f", fVsi / 100.0f);
		//strVSI.Format( "%2.0f", fVsi /100.0f );
		strVSI = buf;
        if( _fVSI < 0 )
            m_AltSmallFreeFont.RenderFontNT( 130, dy-1+10, strVSI );
		else
            m_AltSmallFreeFont.RenderFontNT( 130, dy + dy_vsi-1+10, strVSI );
	}


	{// VSI markings
		glColor3f(1.0f,1.0f,1.0f) ;

		glBegin(GL_LINES); 
			glVertex2f( 125, -25 );
			glVertex2f( 126, -25 );

			glVertex2f( 125, -50 );
			glVertex2f( 127, -50 );

			glVertex2f( 125, -63 );
			glVertex2f( 126, -63 );

			glVertex2f( 125, -75 );
			glVertex2f( 127, -75 );
//////////////////
			glVertex2f( 125, 25 );
			glVertex2f( 126, 25 );

			glVertex2f( 125, 50 );
			glVertex2f( 127, 50 );

			glVertex2f( 125, 63 );
			glVertex2f( 126, 63 );

			glVertex2f( 125, 75 );
			glVertex2f( 127, 75 );
		glEnd();

		glBegin(GL_QUADS);
			glVertex2f( 125, -1 );
			glVertex2f( 128, -1 );
			glVertex2f( 128, 1 );
			glVertex2f( 125, 1 );
		glEnd();
	}
}

void PFDView::_DrawSpd()
{
	 // AirSpeed
	glStencilFunc( GL_ALWAYS, 3, 0xFFFFFFFF );
	glStencilOp( GL_REPLACE, GL_REPLACE, GL_REPLACE );

	{ //Set stencil bits
		glBegin(GL_QUADS);
			glColor3f(0.0f, 0.0f, 0.0f );
			glVertex2f(-110, -80);
			glVertex2f(-65, -80);
			glVertex2f(-65, 80);
			glVertex2f(-110, 80);
		glEnd();
	}

	glStencilFunc( GL_EQUAL, 3, 0xFFFFFFFF );
	glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );

	glBegin(GL_QUADS);
		glColor3f(0.3f, 0.3f, 0.3f );
		glVertex2f( -110, -80 );
		glVertex2f( -80, -80 );
		glVertex2f( -80, 80 );
		glVertex2f( -110, 80 );

		glColor3f(1.0f,1.0f,0.0f );
		glVertex2f( -70, -3 );
		glVertex2f( -70, 3 );
		glVertex2f( -75, 1 );
		glVertex2f( -75, -1 );
	
		glVertex2f(-75,-1);
		glVertex2f(-75,1);
		glVertex2f(-85,1);
		glVertex2f(-85,-1);


		glColor3f(1.0f,1.0f,1.0f );
		glVertex2f( -110, -80 );
		glVertex2f( -72, -80 );
		glVertex2f( -72, -79 );
		glVertex2f( -110, -79 );
	glEnd();

	glBegin(GL_LINES);
		glColor3f(1.0f,1.0f,0.0f );
		glVertex2f( -85,0);
		glVertex2f( -110,0);
	glEnd();

    float fAirSpd = _fAirSpd;
    if( _fAirSpd < 30 ) fAirSpd = 30;

#define PIXEL_PER_KNOT (17/9.0)

float tick_start = (fAirSpd - 30) * PIXEL_PER_KNOT;

	glBegin(GL_QUADS);
	glColor3f(1.0f,1.0f,1.0f );
		for( int spdTick = 30; spdTick < 600; spdTick +=10 )
		{
			float tick_y = -(spdTick-20) * PIXEL_PER_KNOT+tick_start;
			glVertex2f(-85, tick_y );
			glVertex2f(-80, tick_y  );
			glVertex2f(-80, tick_y+1 );
			glVertex2f(-85, tick_y+1  );
		}

		if( tick_start > 80 )
		{
			glVertex2f( -110, 80 );
			glVertex2f( -72, 80 );
			glVertex2f( -72, 79 );
			glVertex2f( -110, 79 );
		}
	glEnd();

	glBegin(GL_LINES);
		glVertex2f( -80, tick_start );
		glVertex2f( -80, -PIXEL_PER_KNOT*600 );
	glEnd();

	//CString strSpd;
	std::string strSpd;
	for( int spdTick = 40; spdTick < 600; spdTick +=20 )
	{
		//strSpd.Format( "%03d", spdTick );
		strSpd = format("%03d", spdTick);
        m_PfdAirSpdFreeFont.RenderFontNT( -108, -(spdTick-20) * PIXEL_PER_KNOT+tick_start+2, strSpd );
		//m_AltLargeFreeFont.RenderFontNT( -108, -(spdTick-20) * PIXEL_PER_KNOT+tick_start+12, strSpd );
		//m_Tahoma8OGLFont.RenderFontNoTransform(-108, -(spdTick-20) * PIXEL_PER_KNOT+tick_start+12, strSpd.GetBuffer() );
	}
}

void PFDView::_DrawHdg()
{
	// Hdg
#define PIXEL_PER_HDG  2.6f
	glStencilOp( GL_REPLACE, GL_REPLACE, GL_REPLACE );	
	glStencilFunc( GL_ALWAYS, 1, 0xFFFFFFFF );
		

	{ //Set stencil bits
		glBegin(GL_QUADS);

			glColor3f(1.0f,1.0f,0.0f);
			glVertex2f( -1, 100 );
			glVertex2f( 1, 100 );
			glVertex2f( 1, 110 );
			glVertex2f( -1, 110 );

			glColor3f(0.4f, 0.4f, 0.4f );
			glVertex2f( -65, 110);
			glVertex2f(65, 110);
			glVertex2f(65, 135);
			glVertex2f(-65, 135);

		glEnd();
	}

	glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );	
	glStencilFunc( GL_EQUAL, 1, 0xFFFFFFFF );
	
	glBegin(GL_QUADS);
		glColor3f(1.0f,1.0f,1.0f);
		glVertex2f( -64, 110);
		glVertex2f(65, 110);
		glVertex2f(65, 111);
		glVertex2f(-64, 111);

		glVertex2f( -64, 110);
		glVertex2f(-65, 110);
		glVertex2f(-65, 135);
		glVertex2f(-64, 135);

		glVertex2f( 64, 110);
		glVertex2f( 65, 110);
		glVertex2f( 65, 135);
		glVertex2f( 64, 135);

        int dH = ((int)_fHdg) % 10;
        int minHdg = (int(_fHdg) - dH - 40);
		int maxHdg = minHdg + 80;
		int x;

		{
			for( int h = minHdg; h <= maxHdg; h+= 5 )
			{
                x = (h - _fHdg) * PIXEL_PER_HDG;

				if( h % 10 == 0 )
				{
					glVertex2f( x-1, 110 );
					glVertex2f( x, 110 );
					glVertex2f( x, 117 );
					glVertex2f( x-1, 117 );
				
				}
				else if( h % 5 == 0 )
				{
					glVertex2f( x-1, 110 );
					glVertex2f( x, 110 );
					glVertex2f( x, 114 );
					glVertex2f( x-1, 114 );
				}
			}
		}
	glEnd();
	
	std::string strHdg;
	for( int h = minHdg; h <= maxHdg; h+= 10 )
	{
        x = (h - _fHdg) * PIXEL_PER_HDG;
		if( h <= 0 )
		{
			strHdg = format( "%02.0f", (h+360)/10.0 );
		}
		else if( h >360 )
		{
			strHdg = format( "%02.0f", (h-360)/10.0 );
		}
		else
		{
			strHdg = format( "%02.0f", h/10.0 );
		}

		if( h % 30 == 0 )
		{
            m_AltLargeFreeFont.RenderFontNT( x-6, 130, strHdg );
		}
		else
		{			
            m_AltSmallFreeFont.RenderFontNT( x-5, 130, strHdg );
		}
	}

}

void PFDView::_DrawFlightModes()
{
	//glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );	
	//glStencilFunc( GL_GREATER, 1, 0xFFFFFFFF );
	//Flight Modes

	float dy = 140;
	glBegin(GL_QUADS);
	glColor3f(1.0f,1.0f,1.0f );
		glVertex2f( -61, -dy+20 );
		glVertex2f( -60, -dy+20 );
		glVertex2f( -60, -dy+60 );
		glVertex2f( -61, -dy+60 );

		glVertex2f( -6, -dy+20 );
		glVertex2f( -5, -dy+20 );
		glVertex2f( -5, -dy+60 );
		glVertex2f( -6, -dy+60 );

		glVertex2f( 49, -dy+20 );
		glVertex2f( 50, -dy+20 );
		glVertex2f( 50, -dy+60 );
		glVertex2f( 49, -dy+60 );

        glVertex2f( 104, -dy+20 );
        glVertex2f( 105, -dy+20 );
        glVertex2f( 105, -dy+60 );
        glVertex2f( 104, -dy+60 );
        glEnd();
}

void PFDView::DrawHorizon(OpenGLPainter *painter)
{
    glStencilFunc( GL_ALWAYS, 1, 0xFFFFFFFF );
    glStencilOp( GL_REPLACE, GL_REPLACE, GL_REPLACE );

    if(_horizData.vertex2Size() == 0)
    {
        for( float fAng = 0; fAng <= 360; fAng += 2.0f )
        {
            float fAngRad = fAng/180.0f * M_PI;
            float x = cos(fAngRad ) * _CX;
            float y = -sin(fAngRad ) * _CY;
            if( x < -65 ) x = -65;
            if( x > 65 ) x = 65;

            _horizData.addVertex(x,y);
        }
    }

//Stencil Horizon
    painter->beginPrimitive();
        painter->setPrimitiveColor({0,0,0,1});
        painter->fillTriangleFan(PRIMITIVE2D(_horizData));
    painter->endPrimitive();

    glStencilFunc( GL_EQUAL, 1, 0xFFFFFFFF );
    glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );

#define PIXEL_PER_PITCH 3.2f
#define PIXEL_PER_PITCH2 (PIXEL_PER_PITCH*0.8f)
#define PIXEL_PER_PITCH3 (PIXEL_PER_PITCH*0.5f)

    OpenGLPipeline& p = OpenGLPipeline::Get(painter->renderer()->camID);

    p.GetModel().Push();
    p.GetModel().Rotate(0, 0, _fBank);
    float dY;
    {
        if( _fPitch > 20 )
        {
            dY = 20 * PIXEL_PER_PITCH + (_fPitch-20)*PIXEL_PER_PITCH2;
        }
        else if( _fPitch  > -10 )
        {
            dY = _fPitch * PIXEL_PER_PITCH;
        }
        else
        {
            dY = -10 * PIXEL_PER_PITCH + (_fPitch+10) * PIXEL_PER_PITCH3;
        }
    }

    p.GetModel().Translate(0, dY, 0);

    if( _horizDataSky.vertex2Size() == 0)
    {
        _horizDataSky.addVertex(-100, -315);
        _horizDataSky.addVertex(100, -315);
        _horizDataSky.addVertex(100, 0);
        _horizDataSky.addVertex(-100, 0);
    }

    if( _horizDataGround.vertex2Size() ==0)
    {
        _horizDataGround.addVertex(-100, 0);
        _horizDataGround.addVertex(100, 0);
        _horizDataGround.addVertex(100, 315);
        _horizDataGround.addVertex(-100, 315);
    }

    painter->beginPrimitive();
        painter->setPrimitiveColor({0.12f, 0.55f, 0.95f, 1.0f});
        painter->fillQuads(PRIMITIVE2D(_horizDataSky));

        painter->setPrimitiveColor({0.5f, 0.35f, 0.22f, 1.0f});
        painter->fillQuads(PRIMITIVE2D(_horizDataGround));

    painter->endPrimitive();

    {//Hdg Lines
        #define PIXEL_PER_HDG  2.6f

        if( _horizHdgLines.vertex2Size()==0)
        {
            int dH = (int)_fHdg % 10;
            int minHdg = (_fHdg - dH - 40);
            int maxHdg = minHdg + 80;
            int x;

            for( int h = minHdg; h <= maxHdg; h+= 10 )
            {
                x = (h - _fHdg) * PIXEL_PER_HDG;

                _horizHdgLines.addVertex( x-1, 0 );
                _horizHdgLines.addVertex( x, 0 );
                _horizHdgLines.addVertex( x, 4 );
                _horizHdgLines.addVertex( x-1, 4 );
            }
        }

        painter->beginPrimitive();
            painter->setPrimitiveColor({1,1,1,1});
            painter->fillQuads(PRIMITIVE2D(_horizHdgLines));
        painter->endPrimitive();
    }


    { //-30 to -10

        float dy = 10 * PIXEL_PER_PITCH+10;
        float fLine;
       if( _horizNeg30ToNeg10.vertex2Size() == 0)
       {
            glBegin(GL_LINES);
                //glColor3f(1.0f,1.0f,1.0f);
                fLine = dy + 5.0*PIXEL_PER_PITCH3;
                _horizNeg30ToNeg10.addVertex( -8, fLine );
                _horizNeg30ToNeg10.addVertex( 8, fLine );

                //glColor3f(0.2f,0.9f,0.2f);
                _horizNeg30ToNeg10.addVertex( -8, fLine-1 );
                _horizNeg30ToNeg10.addVertex( -14, fLine-1 );
                _horizNeg30ToNeg10.addVertex( -8, fLine+1 );
                _horizNeg30ToNeg10.addVertex( -14, fLine+1 );

                _horizNeg30ToNeg10.addVertex( 8, fLine-1 );
                _horizNeg30ToNeg10.addVertex( 14, fLine-1 );
                _horizNeg30ToNeg10.addVertex( 8, fLine+1 );
                _horizNeg30ToNeg10.addVertex( 14, fLine+1 );

                //glColor3f(1.0f,1.0f,1.0f);
                fLine = dy + 10.0*PIXEL_PER_PITCH3;
                _horizNeg30ToNeg10.addVertex( -20, fLine );
                _horizNeg30ToNeg10.addVertex( 20, fLine );

                fLine = dy + 20.0*PIXEL_PER_PITCH3;
                _horizNeg30ToNeg10.addVertex( -25, fLine );
                _horizNeg30ToNeg10.addVertex( 25, fLine );

                fLine = dy + 40.0*PIXEL_PER_PITCH3;
                _horizNeg30ToNeg10.addVertex( -30, fLine );
                _horizNeg30ToNeg10.addVertex( 30, fLine );

                fLine = dy + 60.0*PIXEL_PER_PITCH3;
                _horizNeg30ToNeg10.addVertex( -35, fLine );
                _horizNeg30ToNeg10.addVertex( 35, fLine );

                fLine = dy + 80.0*PIXEL_PER_PITCH3;
                _horizNeg30ToNeg10.addVertex( -40, fLine );
                _horizNeg30ToNeg10.addVertex( 40, fLine );
            glEnd();
       }

        painter->beginPrimitive();
            painter->setPrimitiveColor({1,1,1,1});
            painter->drawLines(_horizNeg30ToNeg10.vertex2Ptr(0), 2);

            painter->setPrimitiveColor({0.2f,0.9f,0.2f,1.0f});
            painter->drawLines(_horizNeg30ToNeg10.vertex2Ptr(2), 8);

            painter->setPrimitiveColor({1,1,1,1});
            painter->drawLines(_horizNeg30ToNeg10.vertex2Ptr(8), 10);

        painter->endPrimitive();

        fLine = dy;
        painter->beginFont(&_PfdHorizFreeFont);
            painter->renderText(-30, fLine-6, "10");
            painter->renderText(20, fLine-6, "10");

            fLine = dy + 10.0*PIXEL_PER_PITCH3; //-20 Degrees
            painter->renderText( -35, fLine-6, "20" );
            painter->renderText( 25, fLine-6, "20");

            fLine = dy + 20.0*PIXEL_PER_PITCH3;//-30 degrees
            painter->renderText( -40, fLine-6, "30" );
            painter->renderText( 30, fLine-6, "30");

            fLine = dy + 40.0*PIXEL_PER_PITCH3;//-50 degrees
            painter->renderText( -45, fLine-6, "50" );
            painter->renderText( 35, fLine-6, "50");

            fLine = dy + 60.0*PIXEL_PER_PITCH3;//-70 degrees
            painter->renderText( -50, fLine-6, "70" );
            painter->renderText( 40, fLine-6, "70");

            fLine = dy + 80.0*PIXEL_PER_PITCH3;//-90 degrees
            painter->renderText( -55, fLine-6, "90" );
            painter->renderText( 45, fLine-6, "90");
        painter->endFont();
    }

    {// -10 to 20

        if(_horizNeg10ToPos20.vertex2Size() ==0)
        {
            _horizNeg10ToPos20.addVertex( -100, 0 );
            _horizNeg10ToPos20.addVertex( 100, 0 );

            float fLine;
            for( int iPitch = -10; iPitch <= 20; iPitch+=10)
            {
                fLine = -iPitch * PIXEL_PER_PITCH;

                if(iPitch )
                {
                    _horizNeg10ToPos20.addVertex( -16, fLine );
                    _horizNeg10ToPos20.addVertex( 16, fLine );
                }

                if(iPitch == 20) continue;

                fLine = -(iPitch+2.5) * PIXEL_PER_PITCH;
                _horizNeg10ToPos20.addVertex( -4, fLine );
                _horizNeg10ToPos20.addVertex( 4, fLine );

                fLine = -(iPitch+5) * PIXEL_PER_PITCH;
                _horizNeg10ToPos20.addVertex( -8, fLine );
                _horizNeg10ToPos20.addVertex( 8, fLine );

                fLine = -(iPitch+	7.5) * PIXEL_PER_PITCH;
                _horizNeg10ToPos20.addVertex( -4, fLine );
                _horizNeg10ToPos20.addVertex( 4, fLine );
            }
        }

        painter->beginPrimitive();
            painter->setPrimitiveColor({1,1,1,1});
            painter->drawLines(PRIMITIVE2D(_horizNeg10ToPos20));
        painter->endPrimitive();
    }

    { // 0 to 20 Text
        painter->beginFont(&_PfdHorizFreeFont);
        std::string str;
        float fLine = 0;
        for( int iPitch = 0; iPitch <= 20; iPitch+=10)
        {
            if( iPitch == 0 ) continue;

            fLine = -iPitch * PIXEL_PER_PITCH +10;
            str = format( "%d", abs(iPitch) );

            if( iPitch == 20 )
            {
                painter->renderText( -35, fLine-6, str );
                painter->renderText( 25, fLine-6, str);
            }
            else
            {
                painter->renderText( -30, fLine-6, str );
                painter->renderText( 20, fLine-6, str);
            }
        }
        painter->endFont();
    }

    //20 to 30
    {
        float fLine;
        float dy = -20 * PIXEL_PER_PITCH;
        if( _horizPos20ToPos30.vertex2Size() == 0)
        {
            fLine = dy - 2.5*PIXEL_PER_PITCH2;
            _horizPos20ToPos30.addVertex( -4, fLine ); //22.5
            _horizPos20ToPos30.addVertex(  4, fLine );

            fLine = dy - 5.0*PIXEL_PER_PITCH2;
            _horizPos20ToPos30.addVertex( -8, fLine ); //25.0
            _horizPos20ToPos30.addVertex( 8, fLine );

            fLine = dy - 7.5*PIXEL_PER_PITCH2;
            _horizPos20ToPos30.addVertex( -4, fLine ); //27.5
            _horizPos20ToPos30.addVertex( 4, fLine );

            fLine = dy - 10.0*PIXEL_PER_PITCH2;
            _horizPos20ToPos30.addVertex( -20, fLine ); //30
            _horizPos20ToPos30.addVertex( 20, fLine );

            _horizPos20ToPos30.addVertex( -25, fLine-1 );
            _horizPos20ToPos30.addVertex( -20, fLine-1 );
            _horizPos20ToPos30.addVertex( -25, fLine+1 );
            _horizPos20ToPos30.addVertex( -20, fLine+1 );

            _horizPos20ToPos30.addVertex( 25, fLine-1 );
            _horizPos20ToPos30.addVertex( 20, fLine-1 );
            _horizPos20ToPos30.addVertex( 25, fLine+1 );
            _horizPos20ToPos30.addVertex( 20, fLine+1 );

        }

        painter->beginPrimitive();
            painter->setPrimitiveColor({1,1,1,1});
            painter->drawLines(_horizPos20ToPos30.vertex2Ptr(0), 8);

            painter->setPrimitiveColor({0.2f,0.9f,0.2f,1});
            painter->drawLines(_horizPos20ToPos30.vertex2Ptr(8), 8);
        painter->endPrimitive();

        painter->beginFont(&_PfdHorizFreeFont);
            fLine = dy - 10.0*PIXEL_PER_PITCH2+10;
            painter->renderText( -40, fLine-6, "30" );
            painter->renderText( 30, fLine-6, "30");
        painter->endFont();
    }

    { // 50 to 90

        float fLine;
        float dy = -20 * PIXEL_PER_PITCH - 30*PIXEL_PER_PITCH2+10;

        if( _horizPos50ToPos90.vertex2Size() == 0)
        {
            fLine = dy;
            _horizPos50ToPos90.addVertex(-30, fLine );
            _horizPos50ToPos90.addVertex( 30, fLine );

            fLine = dy - 20.0*PIXEL_PER_PITCH2;
            _horizPos50ToPos90.addVertex( -35, fLine );
            _horizPos50ToPos90.addVertex( 35, fLine );

            fLine = dy - 40.0*PIXEL_PER_PITCH2;
            _horizPos50ToPos90.addVertex( -40, fLine );
            _horizPos50ToPos90.addVertex( 40, fLine );
        }

        painter->beginPrimitive();
            painter->setPrimitiveColor({1,1,1,1});
            painter->drawLines(PRIMITIVE2D(_horizPos50ToPos90));
        painter->endPrimitive();

        fLine = dy+10;
        painter->beginFont(&_PfdHorizFreeFont);
            painter->renderText( -45, fLine-6, "50" );
            painter->renderText( 35, fLine-6, "50");

        fLine = dy - 20.0*PIXEL_PER_PITCH2+10; //-20 Degrees
            painter->renderText( -50, fLine-6, "70" );
            painter->renderText( 40, fLine-6, "70");

        fLine = dy - 40.0*PIXEL_PER_PITCH2+10;//-30 degrees
            painter->renderText( -55, fLine-6, "90" );
            painter->renderText( 45, fLine-6, "90");
        painter->endFont();
    }

    {//magenta Fly Up warning arrows from deep dive
        if( _horizMagFlyUp.vertex2Size() ==0)
        {
            float fLine;
            float fLine2;
            float dy = 10 * PIXEL_PER_PITCH;
            for( int iPitch = -40; iPitch >= -80; iPitch -= 20)
            {
                float dy2 = dy -(iPitch+10) * PIXEL_PER_PITCH3;
                fLine = dy2 + 5 * PIXEL_PER_PITCH3;//From
                fLine2 = dy2 - 2.5 * PIXEL_PER_PITCH3;//To

                _horizMagFlyUp.addVertex(-15, fLine );
                _horizMagFlyUp.addVertex(-10, fLine );
                _horizMagFlyUp.addVertex( 0, fLine2 );
                _horizMagFlyUp.addVertex( 10, fLine );
                _horizMagFlyUp.addVertex( 15, fLine );
                _horizMagFlyUp.addVertex( 0, fLine2-5 );
                _horizMagFlyUp.addVertex( -15, fLine );
            }
        }

        painter->beginPrimitive();
            painter->setPrimitiveColor({1,0,1,1});
            painter->drawLineLoop(PRIMITIVE2D(_horizMagFlyUp));
        painter->endPrimitive();
    }

    {//magenta Fly Down warning arrows from steep climb
        if(_horizMagFlyDown.vertex2Size()==0)
        {
            float fLine;
            float fLine2;
            float dy = -20 * PIXEL_PER_PITCH;
            for( int iPitch = 40; iPitch <= 80; iPitch += 20)
            {
                float dy2 = dy -(iPitch-20) * PIXEL_PER_PITCH2;
                fLine = dy2 - 5 * PIXEL_PER_PITCH2;//From
                fLine2 = dy2 + 5.0 * PIXEL_PER_PITCH2;//To

                _horizMagFlyDown.addVertex(-10, fLine );
                _horizMagFlyDown.addVertex(-15, fLine );
                _horizMagFlyDown.addVertex( 0, fLine2 );
                _horizMagFlyDown.addVertex( 15, fLine );
                _horizMagFlyDown.addVertex( 10, fLine );
                _horizMagFlyDown.addVertex( 0, fLine2-5 );
                _horizMagFlyDown.addVertex( -10, fLine );
            }
        }

        painter->beginPrimitive();
            painter->setPrimitiveColor({1,0,1,1});
            painter->drawLineLoop(PRIMITIVE2D(_horizMagFlyDown));
        painter->endPrimitive();
    }
    p.GetModel().Pop();

    p.GetModel().Push();
    p.GetModel().Rotate(0, 0, _fBank);

    {
        if( _horizDataSkyGround.vertex2Size() == 0)
        {
            float fy;
            fy = std::max( _fPitch * PIXEL_PER_PITCH,
                        PIXEL_PER_PITCH *( _fAlt / 80.0f * 10 + _fPitch) );
            fy = std::min( 60.0f, fy );
            fy = std::max( -60.0f, fy );

            _horizDataSkyGround.addVertex(-100, -215 );
            _horizDataSkyGround.addVertex(100, -215 );
            _horizDataSkyGround.addVertex(100, -60 );
            _horizDataSkyGround.addVertex(-100, -60 );

            _horizDataSkyGround.addVertex(-100, fy );
            _horizDataSkyGround.addVertex(100, fy );
            _horizDataSkyGround.addVertex(100, 215 );
            _horizDataSkyGround.addVertex(-100, 215 );

            _horizDataSkyGround.addVertex( -100, -60  );
            _horizDataSkyGround.addVertex( 100, -60  );

            _horizDataSkyGround.addVertex( -100, fy  );
            _horizDataSkyGround.addVertex( 100, fy  );
        }

        painter->beginPrimitive();
            painter->setPrimitiveColor({0.12f,0.55f,0.95f,1});
            painter->drawQuads(_horizDataSkyGround.vertex2Ptr(0), 4);

            painter->setPrimitiveColor({0.5f,0.35f,0.22f,1});
            painter->drawQuads(_horizDataSkyGround.vertex2Ptr(4), 4);

            painter->setPrimitiveColor({1,1,1,1});
            painter->drawLines(_horizDataSkyGround.vertex2Ptr(8), 4);

        painter->endPrimitive();

        if( _fAlt < 3.0f)
        {
            float dy = _fPitch * PIXEL_PER_PITCH;
            if( dy +4 < 60 && dy > -60 )
            {
                p.GetModel().Push();
                p.GetModel().Translate( 0, _fPitch * PIXEL_PER_PITCH, 0 );
                    {//Hdg Lines
                        #define PIXEL_PER_HDG  2.6f

                        if( _horizData2.vertex2Size() == 0)
                        {
                            int dH = (int)_fHdg % 10;
                            int minHdg = (_fHdg - dH - 40);
                            int maxHdg = minHdg + 80;
                            int x;

                            for( int h = minHdg; h <= maxHdg; h+= 10 )
                            {
                                x = (h - _fHdg) * PIXEL_PER_HDG;

                                _horizData2.addVertex( x-1, 0 );
                                _horizData2.addVertex( x, 0 );
                                _horizData2.addVertex( x, 4 );
                                _horizData2.addVertex( x-1, 4 );
                            }
                        }

                        painter->beginPrimitive();
                            painter->setPrimitiveColor({1,1,1,1});
                            painter->fillQuads(PRIMITIVE2D(_horizData2));
                        painter->endPrimitive();
                    }
                p.GetModel().Pop();
            }
        }

        int dy = 210;
        if( _horizRudder.vertex2Size() == 0)
        {
            _horizRudder.addVertex( 0, -dy/2+5 );
            _horizRudder.addVertex( 4, -dy/2+11 );
            _horizRudder.addVertex( -4, -dy/2+11 );
//Rudder Assymtry
            _horizRudder.addVertex( 4, -dy/2+11 );
            _horizRudder.addVertex( 6, -dy/2+14 );
            _horizRudder.addVertex( -6, -dy/2+14 );
            _horizRudder.addVertex( -4, -dy/2+11 );

        }

        painter->beginPrimitive();
            painter->drawLineLoop(_horizRudder.vertex2Ptr(0), 3);
            painter->drawLineLoop(_horizRudder.vertex2Ptr(3), 4);
        painter->endPrimitive();

        // When on ground show surrounding square and joystick position and ground tracking if ILS available
        if( _fAlt < 3.0f )
        {
            glColor3f(1.0f, 1.0f, 1.0f);
            glBegin(GL_LINES);


            glEnd();

            if( _horizGroundBox.vertex2Size() == 0)
            {
                _horizGroundBox.addVertex( -55, -45 );
                _horizGroundBox.addVertex( -45, -45 );
                _horizGroundBox.addVertex( -55, -45 );
                _horizGroundBox.addVertex( -55, -35 );

                _horizGroundBox.addVertex( -55, 45 );
                _horizGroundBox.addVertex( -45, 45 );
                _horizGroundBox.addVertex( -55, 45 );
                _horizGroundBox.addVertex( -55, 35 );

                _horizGroundBox.addVertex( 55, -45 );
                _horizGroundBox.addVertex( 45, -45 );
                _horizGroundBox.addVertex( 55, -45 );
                _horizGroundBox.addVertex( 55, -35 );

                _horizGroundBox.addVertex( 55, 45 );
                _horizGroundBox.addVertex( 45, 45 );
                _horizGroundBox.addVertex( 55, 45 );
                _horizGroundBox.addVertex( 55, 35 );
            }

            painter->beginPrimitive();
                painter->setPrimitiveColor({1,1,1,1});
                painter->drawLines(PRIMITIVE2D(_horizGroundBox));
            painter->endPrimitive();
        }

    }

    p.GetModel().Pop();

    {

        float fy;
        fy = std::max( _fPitch * PIXEL_PER_PITCH,
                    PIXEL_PER_PITCH *( _fAlt / 80.0f * 10 + _fPitch) );
        fy = std::min( 60.0f, fy );
        fy = std::max( -60.0f, fy );

        if( _fAlt < 2500 )
        {
            std::string sAlt;
            if( _fAlt <= -5 )
            {
                sAlt = format( "%d", (int)_fAlt - (int)_fAlt % 5  );
            }
            else if( _fAlt <= 5 )
            {
                sAlt = format( "%d", (int)_fAlt );
            }
            else if( _fAlt <= 50 )
            {
                sAlt = format( "%d", (int)_fAlt - (int)_fAlt % 5  );
            }
            else
            {
                sAlt = format( "%d", (int)_fAlt - (int)_fAlt % 10  );
            }

            painter->beginFont(&_RadarAltBold);
            if( _fAlt <= 400 )
                painter->setFontColor({ 0.9f, 0.9f, 0.2f, 1.0f});
            else
                painter->setFontColor({ 0.2f, 0.9f, 0.2f, 1.0f});

            int yPos = 80;

            if( _fAlt < -99 )
                painter->renderText( -13, yPos, sAlt );
            else if( _fAlt < -9 )
                painter->renderText( -10, yPos, sAlt );
            else if( _fAlt < 0 )
                painter->renderText( -5, yPos, sAlt );
            else if( _fAlt < 10 )
                painter->renderText( -2, yPos, sAlt );
            else if( _fAlt < 100 )
                painter->renderText( -5, yPos, sAlt );
            else if( _fAlt < 1000 )
                painter->renderText( -9, yPos, sAlt );
            else
                painter->renderText( -13, yPos, sAlt );

            painter->endFont();
        }
    }

    { //Aircraft on Horizon

        if( _horizAirOnHoriz.vertex2Size() == 0)
        {
            //Outer center left yellow
            _horizAirOnHoriz.addVertex( -63, -3 );
            _horizAirOnHoriz.addVertex( -40, -3 );
            _horizAirOnHoriz.addVertex( -40, 3 );
            _horizAirOnHoriz.addVertex( -63, 3 );

            _horizAirOnHoriz.addVertex( -40, -3 );
            _horizAirOnHoriz.addVertex( -35, -3 );
            _horizAirOnHoriz.addVertex( -35, 10 );
            _horizAirOnHoriz.addVertex( -40, 10 );

            //Outer center right yellow
            _horizAirOnHoriz.addVertex( 63, -3 );
            _horizAirOnHoriz.addVertex( 40, -3 );
            _horizAirOnHoriz.addVertex( 40, 3 );
            _horizAirOnHoriz.addVertex( 63, 3 );

            _horizAirOnHoriz.addVertex( 40, -3 );
            _horizAirOnHoriz.addVertex( 35, -3 );
            _horizAirOnHoriz.addVertex( 35, 10 );
            _horizAirOnHoriz.addVertex( 40, 10 );

            //Center
            _horizAirOnHoriz.addVertex( -3, -3 );
            _horizAirOnHoriz.addVertex( 3, -3 );
            _horizAirOnHoriz.addVertex( 3, 3 );
            _horizAirOnHoriz.addVertex( -3, 3 );

            _horizAirOnHoriz.addVertex( -62, -2 );
            _horizAirOnHoriz.addVertex( -36, -2 );
            _horizAirOnHoriz.addVertex( -36, 2 );
            _horizAirOnHoriz.addVertex( -62, 2 );

            _horizAirOnHoriz.addVertex( -39, -2 );
            _horizAirOnHoriz.addVertex( -36, -2 );
            _horizAirOnHoriz.addVertex( -36, 9 );
            _horizAirOnHoriz.addVertex( -39, 9 );

            //Center Right
            _horizAirOnHoriz.addVertex( 36, -2 );
            _horizAirOnHoriz.addVertex( 62, -2 );
            _horizAirOnHoriz.addVertex( 62, 2 );
            _horizAirOnHoriz.addVertex( 36, 2 );

            _horizAirOnHoriz.addVertex( 36, -2 );
            _horizAirOnHoriz.addVertex( 39, -2 );
            _horizAirOnHoriz.addVertex( 39, 9 );
            _horizAirOnHoriz.addVertex( 36, 9 );

            //Center
            _horizAirOnHoriz.addVertex( -2, -2 );
            _horizAirOnHoriz.addVertex( 2, -2 );
            _horizAirOnHoriz.addVertex( 2, 2 );
            _horizAirOnHoriz.addVertex( -2, 2 );
        }

        painter->beginPrimitive();
            painter->setPrimitiveColor({1,1,0,1});
            painter->fillQuads(_horizAirOnHoriz.vertex2Ptr(0), 20);
            painter->setPrimitiveColor({0,0,0,1});
            painter->fillQuads(_horizAirOnHoriz.vertex2Ptr(20), 20);
        painter->endPrimitive();

        glStencilFunc( GL_ALWAYS, 2, 0xFFFFFFFF );
        glStencilOp( GL_REPLACE, GL_REPLACE, GL_REPLACE );

        //Center Yellow marker at 0 degrees around perimter of horizon

        if( _horizYellowMarker.vertex2Size() == 0)
        {
            _horizYellowMarker.addVertex( -4, -84 - 25 );
            _horizYellowMarker.addVertex( 4, -84 - 25 );
            _horizYellowMarker.addVertex( 0, -74 - 25 );
        }

        painter->beginPrimitive();
            painter->setPrimitiveColor({1,1,0,1});
            painter->drawLineLoop(PRIMITIVE2D(_horizYellowMarker));
        painter->endPrimitive();

        if( _horizAirOnHoriz2.vertex2Size() == 0)
        {
            for( int iAng = -30; iAng <= 30; iAng ++ )
            {
                float fAngRad = iAng/180.0f * M_PI;
                float x = sin(fAngRad ) * _CX;
                float y = -cos(fAngRad ) * _CY;
                _horizAirOnHoriz2.addVertex(x, y);
            }
        }

        painter->beginPrimitive();
            painter->setPrimitiveColor({1,1,1,1});
            painter->drawLineStrip(PRIMITIVE2D(_horizAirOnHoriz2));
        painter->endPrimitive();

        if( _horizMarkers.vertex2Size() == 0)
        {
            // Markers around Horizon denoating 10, 20, 30, and 45 degrees.
            for( int iAng = -30; iAng < 0; iAng +=10 )
            {
                int dA = 1;
                int dH = 5;

                if( iAng == -30 )
                {
                    dA = 1.5;
                    dH = 10;
                }

                float fAngRad1 = (iAng-dA)/180.0f * M_PI;
                float fAngRad2 = (iAng+dA)/180.0f * M_PI;

                float fSinRad1 = sin(fAngRad1 );
                float fCosRad1 = -cos(fAngRad1 );

                float fSinRad2 = sin(fAngRad2 );
                float fCosRad2 = -cos(fAngRad2 );

                float x1 = fSinRad1 * _CX;
                float y1 = fCosRad1 * _CY;
                float x2 = fSinRad2 * _CX;
                float y2 = fCosRad2 * _CY;

                float x3 = fSinRad2 * (_CX+dH);
                float y3 = fCosRad2 * (_CY+dH);
                float x4 = fSinRad1 * (_CX+dH);
                float y4 = fCosRad1 * (_CY+dH);

                _horizMarkers.addVertex(x1, y1);
                _horizMarkers.addVertex(x2, y2);
                _horizMarkers.addVertex(x3, y3);
                _horizMarkers.addVertex(x4, y4);

                _horizMarkers.addVertex(-x1, y1);
                _horizMarkers.addVertex(-x2, y2);
                _horizMarkers.addVertex(-x3, y3);
                _horizMarkers.addVertex(-x4, y4);
            }
        }

        painter->beginPrimitive();
            painter->drawQuads(PRIMITIVE2D(_horizMarkers));
        painter->endPrimitive();
    }


    { // -45 and 45 markers around edge
        float fsin = sin(M_PI/4);
        float fcos = -cos(M_PI/4);

        float x1 = fsin * _CX;
        float y1 = fcos * _CY;

        float x2 = fsin * (_CX+10);
        float y2 = fcos * (_CY+10);

        if( _horiz45Markers.vertex2Size() ==0)
        {
            _horiz45Markers.addVertex(x1, y1 );
            _horiz45Markers.addVertex(x2, y2 );

            _horiz45Markers.addVertex(-x1, y1 );
            _horiz45Markers.addVertex(-x2, y2 );
        }

        painter->beginPrimitive();
            painter->drawLines(PRIMITIVE2D(_horiz45Markers));
        painter->endPrimitive();
    }
}

void PFDView::DrawAlt(OpenGLPainter *painter)
{

}

void PFDView::DrawScrollAlt(OpenGLPainter *painter)
{

}

void PFDView::DrawVSI(OpenGLPainter *painter)
{

}

void PFDView::DrawHdg(OpenGLPainter *painter)
{

}

void PFDView::DrawSpd(OpenGLPainter *painter)
{

}

void PFDView::DrawFlightModes(OpenGLPainter *painter)
{
    float dy = 140;

    float pts[] = {
        -61, -dy+20,
         -60, -dy+20,
         -60, -dy+60,
         -61, -dy+60,

         -6, -dy+20,
         -5, -dy+20,
         -5, -dy+60,
         -6, -dy+60,

         49, -dy+20,
         50, -dy+20,
         50, -dy+60,
         49, -dy+60,

         104, -dy+20,
         105, -dy+20 ,
         105, -dy+60 ,
         104, -dy+60
    };

    painter->beginPrimitive();
    painter->setPrimitiveColor({1,1,1,1});
    painter->fillQuads(pts, sizeof(pts)/sizeof(pts[0])/2);
    painter->endPrimitive();
}

