
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

	GLuint m_iTexture;

	OGLPFDFont m_PfdHorizFreeFont;
	OGLPFDFont m_PfdAirSpdFreeFont;
	OGLPFDFont m_AltLargeFreeFont;
	OGLPFDFont m_AltSmallFreeFont;
	OGLPFDFont m_RadarAltBold;

	float m_fPitch;
	float m_fBank;
	float m_fAirSpd;
	float m_fAlt;
	float m_fVSI;
	float m_fHdg;

	float _CEN_X;
	float _CEN_Y;
	float _CX;
	float _CY;

    int maxY();

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

