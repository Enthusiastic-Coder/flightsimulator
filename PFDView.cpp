
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
    data.vertex2Ptr(), data.vertex2Size()

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

void PFDView::initialise()
{
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
    glClear(GL_STENCIL_BUFFER_BIT);

    _CX = 100;
    _CY = 100;
    _CEN_X = cx/2;
    _CEN_Y = cy/2;

    glDisable(GL_TEXTURE_2D );
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);

    OpenGLPipeline& p = OpenGLPipeline::Get(painter->renderer()->camID);
    p.GetModel().Push();
    OpenGLPipeline::applyScreenProjection(p, 0, 0, cx, cy);
    p.GetModel().Translate(_CEN_X, _CEN_Y,0);

    painter->beginPrimitive();
    painter->setPrimitiveColor({0,0,0,1});
    painter->fillRect(-_CX*1.2, -_CY*1.3, 270, 270);

    painter->setPrimitiveColor({1,1,1,1});
    painter->drawRect(-_CX*1.2, -_CY*1.3, 270, 270);
    painter->endPrimitive();

    DrawFlightModes(painter);
    DrawHorizon(painter);
    DrawSpd(painter);
    DrawAlt(painter);
    DrawVSI(painter);
    DrawHdg(painter);

    p.GetModel().Pop();

    glDisable(GL_STENCIL_TEST);
    glEnable(GL_DEPTH_TEST);
}

void PFDView::BuildAltTape(char buffer[][32], int line_count, int centralAlt, bool bIncreasing)
{
    char altBuf[16];
    sprintf( altBuf, "%5d", abs(centralAlt) );

    if( altBuf[3] == ' ' ) altBuf[3]= '0';

    int altJump;

    if( bIncreasing )
        altJump = 20;
    else
        altJump = -20;

    strcpy(buffer[0], altBuf );

    for( int i = 1; i < line_count; i++ )
    {
        strcpy( buffer[i], buffer[i-1] );

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

        painter->beginPrimitive();
        painter->setPrimitiveColor({1,1,1,1});

        int dH = (int)_fHdg % 10;
        int minHdg = (_fHdg - dH - 40);
        int maxHdg = minHdg + 80;
        int x;

        for( int h = minHdg; h <= maxHdg; h+= 10 )
        {
            x = (h - _fHdg) * PIXEL_PER_HDG;
            painter->fillQuad(x-1, 0, x, 0, x, 4, x-1, 4);
        }


        painter->endPrimitive();
    }


    { //-30 to -10

        float dy = 10 * PIXEL_PER_PITCH+10;
        float fLine;
        if( _horizNeg30ToNeg10.vertex2Size() == 0)
        {
            fLine = dy + 5.0*PIXEL_PER_PITCH3;
            _horizNeg30ToNeg10.addVertex( -8, fLine );
            _horizNeg30ToNeg10.addVertex( 8, fLine );

            _horizNeg30ToNeg10.addVertex( -8, fLine-1 );
            _horizNeg30ToNeg10.addVertex( -14, fLine-1 );
            _horizNeg30ToNeg10.addVertex( -8, fLine+1 );
            _horizNeg30ToNeg10.addVertex( -14, fLine+1 );

            _horizNeg30ToNeg10.addVertex( 8, fLine-1 );
            _horizNeg30ToNeg10.addVertex( 14, fLine-1 );
            _horizNeg30ToNeg10.addVertex( 8, fLine+1 );
            _horizNeg30ToNeg10.addVertex( 14, fLine+1 );

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
        _horizMagFlyUp.clear();
        if( _horizMagFlyUp.vertex2Size() ==0)
        {
            painter->beginPrimitive();
            painter->setPrimitiveColor({1,0,1,1});

            float fLine;
            float fLine2;
            float dy = 10 * PIXEL_PER_PITCH;
            for( int iPitch = -40; iPitch >= -80; iPitch -= 20)
            {
                float dy2 = dy -(iPitch+10) * PIXEL_PER_PITCH3;
                fLine = dy2 + 5 * PIXEL_PER_PITCH3;//From
                fLine2 = dy2 - 2.5 * PIXEL_PER_PITCH3;//To

                _horizMagFlyUp.clear();
                _horizMagFlyUp.addVertex(-15, fLine );
                _horizMagFlyUp.addVertex(-10, fLine );
                _horizMagFlyUp.addVertex( 0, fLine2 );
                _horizMagFlyUp.addVertex( 10, fLine );
                _horizMagFlyUp.addVertex( 15, fLine );
                _horizMagFlyUp.addVertex( 0, fLine2-5 );
                _horizMagFlyUp.addVertex( -15, fLine );
                painter->drawLineLoop(PRIMITIVE2D(_horizMagFlyUp));

            }
        }

        painter->endPrimitive();
    }

    {//magenta Fly Down warning arrows from steep climb
        _horizMagFlyDown.clear();
        if(_horizMagFlyDown.vertex2Size()==0)
        {
            painter->beginPrimitive();
            painter->setPrimitiveColor({1,0,1,1});

            float fLine;
            float fLine2;
            float dy = -20 * PIXEL_PER_PITCH;
            for( int iPitch = 40; iPitch <= 80; iPitch += 20)
            {
                float dy2 = dy -(iPitch-20) * PIXEL_PER_PITCH2;
                fLine = dy2 - 5 * PIXEL_PER_PITCH2;//From
                fLine2 = dy2 + 5.0 * PIXEL_PER_PITCH2;//To

                _horizMagFlyDown.clear();
                _horizMagFlyDown.addVertex(-10, fLine );
                _horizMagFlyDown.addVertex(-15, fLine );
                _horizMagFlyDown.addVertex( 0, fLine2 );
                _horizMagFlyDown.addVertex( 15, fLine );
                _horizMagFlyDown.addVertex( 10, fLine );
                _horizMagFlyDown.addVertex( 0, fLine2-5 );
                _horizMagFlyDown.addVertex( -10, fLine );
                painter->drawLineLoop(PRIMITIVE2D(_horizMagFlyDown));

            }
        }

        painter->endPrimitive();
    }
    p.GetModel().Pop();

    p.GetModel().Push();
    p.GetModel().Rotate(0, 0, _fBank);

    {
        _horizDataSkyGround.clear();
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
        painter->fillQuads(_horizDataSkyGround.vertex2Ptr(0), 4);

        painter->setPrimitiveColor({0.5f,0.35f,0.22f,1});
        painter->fillQuads(_horizDataSkyGround.vertex2Ptr(4), 4);

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

                    _horizData2.clear();
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
    // Altitude

    glStencilOp( GL_REPLACE, GL_REPLACE, GL_REPLACE );
    glStencilFunc( GL_ALWAYS, 3, 0xFFFFFFFF );
    { //Set stencil bits

        if( _AltDataOne.vertex2Size() == 0)
        {
            _AltDataOne.addVertex(125, -80);
            _AltDataOne.addVertex(65, -80);
            _AltDataOne.addVertex(65, 80);
            _AltDataOne.addVertex(125, 80);

            _AltDataOne.addVertex(85, -7);
            _AltDataOne.addVertex(110, -7);
            _AltDataOne.addVertex(110, 7);
            _AltDataOne.addVertex(85, 7);

            _AltDataOne.addVertex(110, -12);
            _AltDataOne.addVertex(125, -12);
            _AltDataOne.addVertex(125, 13);
            _AltDataOne.addVertex(110, 13);
        }

        painter->beginPrimitive();
        painter->setPrimitiveColor({0,0,0,1});

        painter->fillQuads(_AltDataOne.vertex2Ptr(0), 4);

        glStencilFunc( GL_ALWAYS, 4, 0xFFFFFFFF );
        painter->fillQuads(_AltDataOne.vertex2Ptr(4), 8);
        painter->endPrimitive();
    }

    DrawScrollAlt(painter);/****************************/

    painter->beginPrimitive();

    if( _AltDataTwo.vertex2Size() == 0)
    {
        _AltDataTwo.addVertex(85, -7);
        _AltDataTwo.addVertex(110, -7);
        _AltDataTwo.addVertex(110, -12);
        _AltDataTwo.addVertex(124, -12);
        _AltDataTwo.addVertex(124, 13);
        _AltDataTwo.addVertex(110, 13);
        _AltDataTwo.addVertex(110, 7);
        _AltDataTwo.addVertex(85, 7);

        _AltDataTwo.addVertex( 85, -_CEN_Y + 80 );
        _AltDataTwo.addVertex( 110, -_CEN_Y + 80 );
        _AltDataTwo.addVertex( 110, _CEN_Y - 80 );
        _AltDataTwo.addVertex( 85, _CEN_Y - 80 );
    }

    glStencilFunc( GL_ALWAYS, 4, 0xFFFFFFFF );
    glStencilOp( GL_REPLACE, GL_REPLACE, GL_REPLACE );

    painter->setPrimitiveColor({1,1,0,1});
    painter->drawLineStrip(_AltDataTwo.vertex2Ptr(0), 8);

    glStencilFunc( GL_EQUAL, 3, 0xFFFFFFFF );
    glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );

    //Altitude Bar

    painter->setPrimitiveColor({0.4f, 0.4f, 0.4f, 1.0});
    painter->fillQuads(_AltDataTwo.vertex2Ptr(8), 4);

    //Altitude
    {
#define ALT_RANGE 1000
#define PIXEL_PER_ALT_FEET 0.13333f
        int dA = (int)_fAlt % 500;
        float minAlt = _fAlt - dA - ALT_RANGE;
        float maxAlt = minAlt + 2 * ALT_RANGE;
        float ymax = (dA + ALT_RANGE) * PIXEL_PER_ALT_FEET;
        float y;

        painter->setPrimitiveColor({1,1,1, 1.0});

        for( int x = (int)minAlt; x <= maxAlt; x+= 500)
        {
            y = (minAlt - x) * PIXEL_PER_ALT_FEET + ymax;
            painter->fillTriangle(82, y-2, 85, y, 82, y+2);
        }

        for( int x = (int)minAlt; x <= maxAlt; x+= 100)
        {
            y = (minAlt - x) * PIXEL_PER_ALT_FEET + ymax;
            painter->fillQuad(107, y-1, 110, y-1, 110, y+1, 107, y+1);
        }

        painter->endPrimitive();
        painter->beginFont(&_AltLargeFreeFont);

        for( int x = (int)minAlt; x <= maxAlt; x+= 500)
        {
            y = (minAlt - x) * PIXEL_PER_ALT_FEET + ymax + 10;
            std::string strAlt;
            strAlt = format( "%03.0f", abs(x)/100.0 );
            painter->renderText( 87, y-8, strAlt );
        }
        painter->endFont();
    }

    painter->beginPrimitive();
    painter->setPrimitiveColor({1,1,1,1});
    painter->drawLine(110, -_CEN_Y + 80, 110, _CEN_Y + 80 );

    painter->setPrimitiveColor({1,1,0,1});
    painter->fillQuad(67, -1, 80, -1, 80, 1, 67, 1);

    glStencilFunc( GL_ALWAYS, 3, 0xFFFFFFFF );
    painter->setPrimitiveColor({1,1,1,1});

    painter->fillQuad(85, _CEN_Y - 80, 120, _CEN_Y - 80,120, _CEN_Y - 81, 85, _CEN_Y - 81 );
    painter->endPrimitive();
}

void PFDView::DrawScrollAlt(OpenGLPainter *painter)
{
#define NUMBER_OF_ALT_LINES	4
    glStencilFunc( GL_EQUAL, 4, 0xFFFFFFFF );
    glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );


    int dA = (int)_fAlt % 20;
    int centralAlt = (int)_fAlt - dA;

    char rotaryAltBuf1[NUMBER_OF_ALT_LINES][32];
    char rotaryAltBuf2[NUMBER_OF_ALT_LINES][32];

    BuildAltTape( rotaryAltBuf1, NUMBER_OF_ALT_LINES, centralAlt, true);
    BuildAltTape( rotaryAltBuf2, NUMBER_OF_ALT_LINES, centralAlt, false);

#define SCROLL_TEXT_PIXEL_PER_ALT 0.5f

    float dPixel = dA * SCROLL_TEXT_PIXEL_PER_ALT - 7 + 10;

    painter->beginFont(&_AltSmallFreeFont);

    if( _fAlt <= 400 )
        painter->setFontColor({ 0.9f, 0.9f, 0.2f, 1} );
    else
        painter->setFontColor({ 0.2f, 0.9f, 0.2f, 1} );

    painter->renderText( 111, dPixel-40*SCROLL_TEXT_PIXEL_PER_ALT, &rotaryAltBuf1[2][3] );
    painter->renderText( 111, dPixel-20*SCROLL_TEXT_PIXEL_PER_ALT, &rotaryAltBuf1[1][3] );
    painter->renderText( 111, dPixel, &rotaryAltBuf1[0][3] );
    painter->renderText( 111, dPixel+20*SCROLL_TEXT_PIXEL_PER_ALT, &rotaryAltBuf2[1][3] );
    painter->renderText( 111, dPixel+40*SCROLL_TEXT_PIXEL_PER_ALT, &rotaryAltBuf2[2][3] );

    painter->endFont();

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

    painter->beginFont(&_AltLargeFreeFont);

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
        painter->renderText( 102, yPos[0], rotaryAltBuf1[1][2] );
        painter->renderText( 102, yPos[1], rotaryAltBuf1[0][2] );
        painter->renderText( 102, yPos[2], rotaryAltBuf2[1][2] );

        if( rotaryAltBuf1[0][2] == '9' )
        {
            bDrawn[1] = true;
            painter->renderText( 94, yPos[0], rotaryAltBuf1[1][1] );
            painter->renderText( 94, yPos[1], rotaryAltBuf1[0][1] );
            painter->renderText( 94, yPos[2], rotaryAltBuf2[1][1] );

            if( rotaryAltBuf1[0][1] == '9' )
            {
                bDrawn[0] = true;
                painter->renderText( 86, yPos[0], rotaryAltBuf1[1][0] );
                painter->renderText( 86, yPos[1], rotaryAltBuf1[0][0] );
                painter->renderText( 86, yPos[2], rotaryAltBuf2[1][0] );
            }
        }
    }

    if( !bDrawn[0] )
    {
        painter->renderText( 86, yPosNoChange[0], rotaryAltBuf1[1][0] );
        painter->renderText( 86, yPosNoChange[1], rotaryAltBuf1[0][0] );
        painter->renderText( 86, yPosNoChange[2], rotaryAltBuf2[1][0] );
    }

    if( !bDrawn[1] )
    {
        painter->renderText( 94, yPosNoChange[0], rotaryAltBuf1[1][1] );
        painter->renderText( 94, yPosNoChange[1], rotaryAltBuf1[0][1] );
        painter->renderText( 94, yPosNoChange[2], rotaryAltBuf2[1][1] );
    }

    if( !bDrawn[2] )
    {
        painter->renderText( 102, yPosNoChange[0], rotaryAltBuf1[1][2] );
        painter->renderText( 102, yPosNoChange[1], rotaryAltBuf1[0][2] );
        painter->renderText( 102, yPosNoChange[2], rotaryAltBuf2[1][2] );
    }

    painter->endFont();
}

void PFDView::DrawVSI(OpenGLPainter *painter)
{
    //VSI

    glStencilFunc( GL_ALWAYS, 3, 0xFFFFFFFF );
    glStencilOp( GL_REPLACE, GL_REPLACE, GL_REPLACE );

    painter->beginPrimitive();
    painter->setPrimitiveColor({0.4f, 0.4f, 0.4f, 1});
    painter->fillQuad(125, -80, 140, -50, 140, 50, 125, 80);


#define PIXEL_PER_VSI_1000		0.05f

    glStencilFunc( GL_EQUAL, 3, 0xFFFFFFFF );
    glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP);

    float fVsi = fabs(_fVSI);
    float dy = 0.0f;
    painter->setPrimitiveColor({0.1f, 1.0f, 0.1f, 1});

    if( fVsi < 1050 )
    {
        dy = -PIXEL_PER_VSI_1000 * _fVSI;

        painter->fillQuad(140, -2, 140, 2, 125, dy +2, 125, dy -2);
    }
    else
    {
        if( fVsi > 6000 )
            painter->setPrimitiveColor({0.9f, 0.9f, 0.2f});

        dy = PIXEL_PER_VSI_1000 * (1000 +  0.5f * (fVsi-1000));

        dy = PIXEL_PER_VSI_1000 * (1000 +  0.5f * (fVsi-1000));

        if( _fVSI > 0 )
            dy = -dy;

        float resDy = dy;
        if( resDy > 75 ) resDy = 75;
        if( resDy < -75 ) resDy = -75;

        painter->fillQuad(140, -2, 140, 2, 125, resDy+2, 125, resDy-2);
    }

    glStencilFunc( GL_ALWAYS, 3, 0xFFFFFFFF );
    if( fVsi > 100 )
    {
        if( dy > 75 ) dy = 75;
        if( dy < -75) dy = -75;

        float dy_vsi = 0.0f;
        float end_x = 142;
        painter->setPrimitiveColor({0,0,0,1});

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

        painter->fillQuad(125, dy + dy_vsi, end_x, dy + dy_vsi, end_x, dy, 125, dy );
        painter->endPrimitive();

        painter->beginFont(&_AltSmallFreeFont);

        if( fVsi > 6000 )
            painter->setFontColor({0.9f, 0.9f, 0.2f, 1} );
        else
            painter->setFontColor({0.1f,0.9f,0.1f});

        std::string strVSI;
        char buf[32];
        sprintf(buf, "%2.0f", fVsi / 100.0f);

        strVSI = buf;
        if( _fVSI < 0 )
            painter->renderText( 130, dy-1+10, strVSI );
        else
            painter->renderText( 130, dy + dy_vsi-1+10, strVSI );

        painter->endFont();
    }

    {// VSI markings
        painter->beginPrimitive();
        painter->setPrimitiveColor({1,1,1,1});

        if( _Vsi.vertex2Size()== 0)
        {
            _Vsi.addVertex( 125, -25 );
            _Vsi.addVertex( 126, -25 );

            _Vsi.addVertex( 125, -50 );
            _Vsi.addVertex( 127, -50 );

            _Vsi.addVertex( 125, -63 );
            _Vsi.addVertex( 126, -63 );

            _Vsi.addVertex( 125, -75 );
            _Vsi.addVertex( 127, -75 );
            //////////////////
            _Vsi.addVertex( 125, 25 );
            _Vsi.addVertex( 126, 25 );

            _Vsi.addVertex( 125, 50 );
            _Vsi.addVertex( 127, 50 );

            _Vsi.addVertex( 125, 63 );
            _Vsi.addVertex( 126, 63 );

            _Vsi.addVertex( 125, 75 );
            _Vsi.addVertex( 127, 75 );

            _Vsi.addVertex( 125, -1 );
            _Vsi.addVertex( 128, -1 );
            _Vsi.addVertex( 128, 1 );
            _Vsi.addVertex( 125, 1 );
        }

        painter->drawLines(_Vsi.vertex2Ptr(0), 16);
        painter->fillQuads(_Vsi.vertex2Ptr(16), 4);
        painter->endPrimitive();
    }
}

void PFDView::DrawHdg(OpenGLPainter *painter)
{
    // Hdg
#define PIXEL_PER_HDG  2.6f
    glStencilOp( GL_REPLACE, GL_REPLACE, GL_REPLACE );
    glStencilFunc( GL_ALWAYS, 1, 0xFFFFFFFF );


    { //Set stencil bits
        painter->beginPrimitive();
        painter->setPrimitiveColor({1,1,0,1});
        painter->fillQuad(-1, 100, 1, 100, 1, 110, -1, 110);

        painter->setPrimitiveColor({0.4f, 0.4f, 0.4f, 1});
        painter->fillQuad(-65, 110, 65, 110, 65, 135, -65, 135);
    }

    glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
    glStencilFunc( GL_EQUAL, 1, 0xFFFFFFFF );

    painter->setPrimitiveColor({1,1,1,1});
    painter->fillQuad(-64, 110, 65, 110, 65, 111, -64, 111);
    painter->fillQuad(-64, 110, -65, 110, -65, 135, -64, 135);
    painter->fillQuad(64, 110, 65, 110, 65, 135, 64, 135);

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
                painter->fillQuad(x-1, 110, x, 110, x, 117, x-1, 117);
            }
            else if( h % 5 == 0 )
            {
                painter->fillQuad(x-1, 110, x, 110, x, 114, x-1, 114);
            }
        }
    }
    painter->endPrimitive();


    painter->setFontColor({1,1,1,1});
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
            painter->beginFont(&_AltLargeFreeFont);
            painter->renderText( x-6, 130, strHdg );
            painter->endFont();
        }
        else
        {
            painter->beginFont(&_AltSmallFreeFont);
            painter->renderText( x-5, 130, strHdg );
            painter->endFont();
        }
    }

}

void PFDView::DrawSpd(OpenGLPainter *painter)
{
    // AirSpeed
    glStencilFunc( GL_ALWAYS, 3, 0xFFFFFFFF );
    glStencilOp( GL_REPLACE, GL_REPLACE, GL_REPLACE );

    if( _SpdMeshData.vertex2Size() == 0)
    {
        _SpdMeshData.addVertex(-110, -80);
        _SpdMeshData.addVertex(-65, -80);
        _SpdMeshData.addVertex(-65, 80);
        _SpdMeshData.addVertex(-110, 80);

        _SpdMeshData.addVertex( -110, -80 );
        _SpdMeshData.addVertex( -80, -80 );
        _SpdMeshData.addVertex( -80, 80 );
        _SpdMeshData.addVertex( -110, 80 );

        _SpdMeshData.addVertex( -70, -3 );
        _SpdMeshData.addVertex( -70, 3 );
        _SpdMeshData.addVertex( -75, 1 );
        _SpdMeshData.addVertex( -75, -1 );

        _SpdMeshData.addVertex(-75,-1);
        _SpdMeshData.addVertex(-75,1);
        _SpdMeshData.addVertex(-85,1);
        _SpdMeshData.addVertex(-85,-1);

        _SpdMeshData.addVertex( -110, -80 );
        _SpdMeshData.addVertex( -72, -80 );
        _SpdMeshData.addVertex( -72, -79 );
        _SpdMeshData.addVertex( -110, -79 );

        _SpdMeshData.addVertex( -85,0);
        _SpdMeshData.addVertex( -110,0);
    }

    painter->beginPrimitive();
    painter->setPrimitiveColor({0,0,0,1});
    painter->fillQuads(_SpdMeshData.vertex2Ptr(0), 4);

    glStencilFunc( GL_EQUAL, 3, 0xFFFFFFFF );
    glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );

    painter->setPrimitiveColor({0.3f,0.3f,0.3f,1});
    painter->fillQuads(_SpdMeshData.vertex2Ptr(4), 4);

    painter->setPrimitiveColor({1,1,0,1});
    painter->fillQuads(_SpdMeshData.vertex2Ptr(8), 8);

    painter->setPrimitiveColor({1,1,1,1});
    painter->fillQuads(_SpdMeshData.vertex2Ptr(16), 4);

    painter->setPrimitiveColor({1,1,0,1});
    painter->drawLines(_SpdMeshData.vertex2Ptr(20), 2);



    float fAirSpd = _fAirSpd;
    if( _fAirSpd < 30 ) fAirSpd = 30;

#define PIXEL_PER_KNOT (17/9.0)

    float tick_start = (fAirSpd - 30) * PIXEL_PER_KNOT;

    _SpdMeshData2.clear();
    if( _SpdMeshData2.vertex2Size() == 0)
    {
        for( int spdTick = 30; spdTick < 600; spdTick +=10 )
        {
            float tick_y = -(spdTick-20) * PIXEL_PER_KNOT+tick_start;
            _SpdMeshData2.addVertex(-85, tick_y );
            _SpdMeshData2.addVertex(-80, tick_y  );
            _SpdMeshData2.addVertex(-80, tick_y+1 );
            _SpdMeshData2.addVertex(-85, tick_y+1  );
        }
    }

    painter->setPrimitiveColor({1,1,1,1});
    painter->fillQuads(PRIMITIVE2D(_SpdMeshData2));

    if( tick_start > 80 )
        painter->fillQuad(-110,80, -72, 80, -72,79, -110, 79);

    painter->drawLine(-80, tick_start, -80, -PIXEL_PER_KNOT*600);

    painter->endPrimitive();

    painter->beginFont(&_PfdAirSpdFreeFont);
    painter->setFontColor({1,1,1,1});

    std::string strSpd;
    for( int spdTick = 40; spdTick < 600; spdTick +=20 )
    {
        strSpd = format("%03d", spdTick);
        painter->renderText( -108, -(spdTick-20) * PIXEL_PER_KNOT+tick_start+2, strSpd );
    }
    painter->endFont();

}

void PFDView::DrawFlightModes(OpenGLPainter *painter)
{
    float dy = 140;

    if( _FlightModeData.vertex2Size() == 0)
    {
        _FlightModeData.addVertex( -61, -dy+20);
        _FlightModeData.addVertex( -60, -dy+20);
        _FlightModeData.addVertex( -60, -dy+60);
        _FlightModeData.addVertex( -61, -dy+60);

        _FlightModeData.addVertex( -6, -dy+20);
        _FlightModeData.addVertex(  -5, -dy+20);
        _FlightModeData.addVertex(  -5, -dy+60);
        _FlightModeData.addVertex(  -6, -dy+60);

        _FlightModeData.addVertex(  49, -dy+20);
        _FlightModeData.addVertex(  50, -dy+20);
        _FlightModeData.addVertex(  50, -dy+60);
        _FlightModeData.addVertex(  49, -dy+60);

        _FlightModeData.addVertex( 104, -dy+20);
        _FlightModeData.addVertex( 105, -dy+20);
        _FlightModeData.addVertex( 105, -dy+60);
        _FlightModeData.addVertex( 104, -dy+60);
    }


    painter->beginPrimitive();
    painter->setPrimitiveColor({1,1,1,1});
    painter->fillQuads(PRIMITIVE2D(_FlightModeData));
    painter->endPrimitive();
}

