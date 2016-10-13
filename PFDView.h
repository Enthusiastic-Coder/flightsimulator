#ifndef __PFD__VIEW__H__
#define __PFD__VIEW__H__

#include "OGLPFDFont.h"
#include "OpenGLFontTexture.h"

class OpenGLPainter;

template<class T> std::string format(std::string, T val);

class PFDView
{
// Construction
public:
	PFDView();

	void Initialise(HDC hdc);
	
    void render(OpenGLPainter* r, int cx, int cy);

    float _fPitch;
    float _fBank;
    float _fAirSpd;
    float _fAlt;
    float _fVSI;
    float _fHdg;

private:
    GLuint _iTexture;

    OpenGLFontTexture _PfdHorizFreeFont;
    OpenGLFontTexture _PfdAirSpdFreeFont;
    OpenGLFontTexture _AltLargeFreeFont;
    OpenGLFontTexture _AltSmallFreeFont;
    OpenGLFontTexture _RadarAltBold;

    OGLPFDFont m_PfdHorizFreeFont;
    OGLPFDFont m_PfdAirSpdFreeFont;
    OGLPFDFont m_AltLargeFreeFont;
    OGLPFDFont m_AltSmallFreeFont;
    OGLPFDFont m_RadarAltBold;

    float _CEN_X;
    float _CEN_Y;
    float _CX;
    float _CY;

    void DrawHorizon();
    void BuildAltTape(char buffer[][32], int line_count, int centralAlt, bool bIncreasing);
    void DrawAlt();
    void DrawScrollAlt();

    void DrawVSI();
    void DrawHdg();
    void DrawSpd();
    void DrawFlightModes();
};

#endif //__PFD__VIEW__H__
