#ifndef __PFD__VIEW__H__
#define __PFD__VIEW__H__

#include "OpenGLFontTexture.h"
#include "MeshData.h"

class OpenGLPainter;

template<class T> std::string format(std::string, T val);

class PFDView
{
// Construction
public:
	PFDView();

    void Initialise();
	
    void render(OpenGLPainter* painter, int cx, int cy);

    float _fPitch;
    float _fBank;
    float _fAirSpd;
    float _fAlt;
    float _fVSI;
    float _fHdg;

private:
    OpenGLFontTexture _PfdHorizFreeFont;
    OpenGLFontTexture _PfdAirSpdFreeFont;
    OpenGLFontTexture _AltLargeFreeFont;
    OpenGLFontTexture _AltSmallFreeFont;
    OpenGLFontTexture _RadarAltBold;

    float _CEN_X;
    float _CEN_Y;
    float _CX;
    float _CY;

    void BuildAltTape(char buffer[][32], int line_count, int centralAlt, bool bIncreasing);

    void DrawHorizon(OpenGLPainter* painter);
    void DrawAlt(OpenGLPainter* painter);
    void DrawScrollAlt(OpenGLPainter* painter);
    void DrawVSI(OpenGLPainter* painter);
    void DrawHdg(OpenGLPainter* painter);
    void DrawSpd(OpenGLPainter* painter);
    void DrawFlightModes(OpenGLPainter* painter);

    meshData _horizData;
    meshData _horizDataSky;
    meshData _horizDataGround;
    meshData _horizNeg30ToNeg10;
    meshData _horizNeg10ToPos20;
    meshData _horizPos20ToPos30;
    meshData _horizPos50ToPos90;
    meshData _horizMagFlyUp;
    meshData _horizMagFlyDown;
    meshData _horizDataSkyGround;
    meshData _horizData2;
    meshData _horizRudder;
    meshData _horizGroundBox;
    meshData _horizAirOnHoriz;
    meshData _horizYellowMarker;
    meshData _horizAirOnHoriz2;
    meshData _horizMarkers;
    meshData _horiz45Markers;
    meshData _FlightModeData;
    meshData _SpdMeshData;
    meshData _SpdMeshData2;
    meshData _AltDataOne;
    meshData _AltDataTwo;
    meshData _Vsi;
};

#endif //__PFD__VIEW__H__
