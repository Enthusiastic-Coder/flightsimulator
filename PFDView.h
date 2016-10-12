
#pragma once

#include "OGLPFDFont.h"
// PFDView window

template<class T> std::string format(std::string, T val);

class PFDView
{
// Construction
public:
	PFDView();

	void Initialise(HDC hdc);

    GLuint _iTexture;

	OGLPFDFont m_PfdHorizFreeFont;
	OGLPFDFont m_PfdAirSpdFreeFont;
	OGLPFDFont m_AltLargeFreeFont;
	OGLPFDFont m_AltSmallFreeFont;
	OGLPFDFont m_RadarAltBold;

    float _fPitch;
    float _fBank;
    float _fAirSpd;
    float _fAlt;
    float _fVSI;
    float _fHdg;

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
	
    void render(int cx, int cy);
};

