#pragma once

#define noiseWidth 256
#define noiseHeight 256

class Noise
{
public:
    void generateNoise();
    double smoothNoise(double x, double y);
    double turbulence(double x, double y, double size);
private:
    double _noise[noiseWidth][noiseHeight];
};

class PerlinNoise
{
public:
    void Prepare();
    float Noise(int x, int y, int random);

private:
    void SetNoise();
    float Interpolate(float x, float y);
    void OverlapOctaves();
    void ExpFilter();

private:
    float _map32[32 * 32];
    float _map256[256 * 256];
};

