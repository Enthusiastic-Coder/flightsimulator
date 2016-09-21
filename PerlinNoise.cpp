#include "stdafx.h"
#include "PerlinNoise.h"
#include <math.h>
#include <stdlib.h>

void Noise::generateNoise()
{
    for (int x = 0; x < noiseWidth; x++)
        for (int y = 0; y < noiseHeight; y++)
        {
            _noise[x][y] = (rand() % 32768) / 32768.0;
        }
}

double Noise::smoothNoise(double x, double y)
{
    //get fractional part of x and y
    double fractX = x - int(x);
    double fractY = y - int(y);

    //wrap around
    int x1 = (int(x) + noiseWidth) % noiseWidth;
    int y1 = (int(y) + noiseHeight) % noiseHeight;

    //neighbor values
    int x2 = (x1 + noiseWidth - 1) % noiseWidth;
    int y2 = (y1 + noiseHeight - 1) % noiseHeight;

    //smooth the noise with bilinear interpolation
    double value = 0.0;
    value += fractX       * fractY       * _noise[x1][y1];
    value += fractX       * (1 - fractY) * _noise[x1][y2];
    value += (1 - fractX) * fractY       * _noise[x2][y1];
    value += (1 - fractX) * (1 - fractY) * _noise[x2][y2];

    return value;
}

double Noise::turbulence(double x, double y, double size)
{
    double value = 0.0, initialSize = size;

    while (size >= 1)
    {
        value += smoothNoise(x / size, y / size) * size;
        size /= 2.0;
    }

    return(128.0 * value / initialSize);
}

void PerlinNoise::Prepare()
{
    SetNoise();
    OverlapOctaves();
    ExpFilter();
}

float PerlinNoise::Noise(int x, int y, int random)
{
    int n = x + y * 57 + random * 131;
    n = (n<<13) ^ n;
    return (1.0f - ( (n * (n * n * 15731 + 789221) +
                      1376312589)&0x7fffffff)* 0.000000000931322574615478515625f);
}

void PerlinNoise::SetNoise()
{
    float temp[34][34];
    int random=rand() % 5000;

    for (int y=1; y<33; y++)
        for (int x=1; x<33; x++)
        {
            temp[x][y] = 128.0f + Noise(x,  y,  random)*128.0f;
        }
    for (int x=1; x<33; x++)
    {
        temp[0][x] = temp[32][x];
        temp[33][x] = temp[1][x];
        temp[x][0] = temp[x][32];
        temp[x][33] = temp[x][1];
    }
    temp[0][0] = temp[32][32];
    temp[33][33] = temp[1][1];
    temp[0][33] = temp[32][1];
    temp[33][0] = temp[1][32];
    for (int y=1; y<33; y++)
        for (int x=1; x<33; x++)
        {
            float center = temp[x][y]/4.0f;
            float sides = (temp[x+1][y] + temp[x-1][y] + temp[x][y+1] + temp[x][y-1])/8.0f;
            float corners = (temp[x+1][y+1] + temp[x+1][y-1] + temp[x-1][y+1] + temp[x-1][y-1])/16.0f;

            _map32[((x-1)*32) + (y-1)] = center + sides + corners;
        }
}

float PerlinNoise::Interpolate(float x, float y)
{
    int Xint = (int)x;
    int Yint = (int)y;

    float Xfrac = x - Xint;
    float Yfrac = y - Yint;
    int X0 = Xint % 32;
    int Y0 = Yint % 32;
    int X1 = (Xint + 1) % 32;
    int Y1 = (Yint + 1) % 32;

    float bot = _map32[X0*32 + Y0] + Xfrac * (_map32[X1*32 + Y0] - _map32[X0*32 + Y0]);
    float top = _map32[X0*32 + Y1] + Xfrac * (_map32[X1*32 +  Y1] - _map32[X0*32 + Y1]);

    return (bot + Yfrac * (top - bot));
}

void PerlinNoise::OverlapOctaves()
{
    for (int x=0; x<256*256; x++)
        _map256[x] = 0;

    for (int octave=0; octave<4; octave++)
        for (int x=0; x<256; x++)
            for (int y=0; y<256; y++)
            {
                float scale = 1 / pow(2.0f, 3-octave);
                float noise = Interpolate(x*scale, y*scale);
                _map256[(y*256) + x] += noise / pow(2.0f, octave);
            }
}

void PerlinNoise::ExpFilter()
{
    float cover = 20.0f;
    float sharpness = 0.95f;

    for (int x=0; x<256*256; x++)
    {
        float c = _map256[x] - (255.0f-cover);
        if (c<0)     c = 0;
        _map256[x] = 255.0f - ((float)(pow(sharpness, c))*255.0f);
    }
}
