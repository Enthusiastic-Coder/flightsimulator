#include "StdAfx.h"
#include "WindTunnelForceGenerator.h"
#include "AeroSectionElementForceGenerator.h"
#include "AeroSectionSubElementForceGenerator.h"
#include "AeroForceGenerator.h"
#include "WorldSystem.h"

typedef unsigned char uint;

#ifdef RGB
#undef RGB
#endif

struct RGB
{
    RGB():R(0),G(0),B(0) {}
    RGB(uint r, uint g, uint b): R(r), G(g), B(b){}

    uint R;
    uint G;
    uint B;
};

// This is a subfunction of HSLtoRGB
void HSLtoRGB_Subfunction(uint& c, const double& temp1, const double& temp2, const double& temp3)
{
    if((temp3 * 6) < 1)
        c = (uint)((temp2 + (temp1 - temp2)*6*temp3)*100);
    else
    if((temp3 * 2) < 1)
        c = (uint)(temp1*100);
    else
    if((temp3 * 3) < 2)
        c = (uint)((temp2 + (temp1 - temp2)*(.66666 - temp3)*6)*100);
    else
        c = (uint)(temp2*100);
    return;
}

// This function extracts the hue, saturation, and luminance from "color"
// and places these values in h, s, and l respectively.
void RGBtoHSL(const COLORREF& color,uint& h, uint& s, uint& l)
{
    uint r = (uint)GetRValue(color);
    uint g = (uint)GetGValue(color);
    uint b = (uint)GetBValue(color);

    double r_percent = ((double)r)/255;
    double g_percent = ((double)g)/255;
    double b_percent = ((double)b)/255;

    double max_color = 0;
    if((r_percent >= g_percent) && (r_percent >= b_percent))
        max_color = r_percent;
    if((g_percent >= r_percent) && (g_percent >= b_percent))
        max_color = g_percent;
    if((b_percent >= r_percent) && (b_percent >= g_percent))
        max_color = b_percent;

    double min_color = 0;
    if((r_percent <= g_percent) && (r_percent <= b_percent))
        min_color = r_percent;
    if((g_percent <= r_percent) && (g_percent <= b_percent))
        min_color = g_percent;
    if((b_percent <= r_percent) && (b_percent <= g_percent))
        min_color = b_percent;

    double L = 0;
    double S = 0;
    double H = 0;

    L = (max_color + min_color)/2;

    if(max_color == min_color)
    {
        S = 0;
        H = 0;
    }
    else
    {
        if(L < .50)
        {
            S = (max_color - min_color)/(max_color + min_color);
        }
        else
        {
            S = (max_color - min_color)/(2 - max_color - min_color);
        }
        if(max_color == r_percent)
        {
            H = (g_percent - b_percent)/(max_color - min_color);
        }
        if(max_color == g_percent)
        {
            H = 2 + (b_percent - r_percent)/(max_color - min_color);
        }
        if(max_color == b_percent)
        {
            H = 4 + (r_percent - g_percent)/(max_color - min_color);
        }
    }
    s = (uint)(S*100);
    l = (uint)(L*100);
    H = H*60;
    if(H < 0)
        H += 360;
    h = (uint)H;
}

// This function converts the "color" object to the equivalent RGB values of
// the hue, saturation, and luminance passed as h, s, and l respectively
RGB HSLtoRGB(const uint& h, const uint& s, const uint& l)
{
    uint r = 0;
    uint g = 0;
    uint b = 0;

    double L = ((double)l)/100;
    double S = ((double)s)/100;
    double H = ((double)h)/360;

    if(s == 0)
    {
        r = l;
        g = l;
        b = l;
    }
    else
    {
        double temp1 = 0;
        if(L < .50)
        {
            temp1 = L*(1 + S);
        }
        else
        {
            temp1 = L + S - (L*S);
        }

        double temp2 = 2*L - temp1;

        double temp3 = 0;
        for(int i = 0 ; i < 3 ; i++)
        {
            switch(i)
            {
            case 0: // red
                {
                    temp3 = H + .33333;
                    if(temp3 > 1)
                        temp3 -= 1;
                    HSLtoRGB_Subfunction(r,temp1,temp2,temp3);
                    break;
                }
            case 1: // green
                {
                    temp3 = H;
                    HSLtoRGB_Subfunction(g,temp1,temp2,temp3);
                    break;
                }
            case 2: // blue
                {
                    temp3 = H - .33333;
                    if(temp3 < 0)
                        temp3 += 1;
                    HSLtoRGB_Subfunction(b,temp1,temp2,temp3);
                    break;
                }
            default:
                {

                }
            }
        }
    }
    r = (uint)((((double)r)/100)*255);
    g = (uint)((((double)g)/100)*255);
    b = (uint)((((double)b)/100)*255);
    return RGB(r,g,b);
}



WindTunnelForceGenerator::WindTunnelForceGenerator() :
    _bPitchRate(true)
{
}

void WindTunnelForceGenerator::setLeftRightWing(AeroForceGenerator *lAfg, AeroForceGenerator *rAfg)
{
    _lAfg = lAfg;
    _rAfg = rAfg;
}

void WindTunnelForceGenerator::onApplyForce(Particle *p, double dt)
{
    //No Forces applied.
}

void WindTunnelForceGenerator::drawForceGenerator(GSRigidBody* b, Renderer* r)
{
    drawWindTunnelLines(b, r);
}

void WindTunnelForceGenerator::GenerateArrowHead(const Vector3D &p1, const Vector3D &p2, Vector3D out[])
{
    double ArrowHeadSize = 0.25;
    // Compute the vector along the arrow direction
    Vector3D v = (p2 - p1).Unit();

    // Compute two perpendicular vectors to v
    Vector3D  vPerp1(-v.y, v.x,0);
    Vector3D  vPerp2(v.y, -v.x,0);

    // Compute two half-way vectors
    Vector3D v1 = (v + vPerp1).Unit();
    Vector3D v2 = (v + vPerp2).Unit();

    out[0] = p2 - ArrowHeadSize * v1;
    out[1] = p2 - ArrowHeadSize * v2;
}

void WindTunnelForceGenerator::drawWindTunnelLines(GSRigidBody* pBody, Renderer *r)
{
    int hue, lum, sat;
    lum = 120;
    sat = 240;
    hue;
    RGB rgbColor;

    int split = 15;
    //split = 10;

    double fCenter = 40.0f;
    double fFactor = fCenter/(split-1);

    double fVelocity = pBody->velocityBody().Magnitude()
            + pBody->toLocalGroundFrame(
                getWorld()->getWeather()->getWindFromPosition(Vector3D())).Magnitude();

    if( _bPitchRate  )
    {
        int iSample = 20;
        //iSample = 10;
        if( fVelocity > 1)
        {
            double dtPath = split * fFactor /fVelocity / iSample;
            Vector3D position0;
            Vector3D vel;

            for( int i=0; i < iSample/2; i++ )
            {
                vel = pBody->velocityBody()
                        - pBody->toLocalGroundFrame(
                            getWorld()->getWeather()->getWindFromPosition(position0));

                position0 += vel * dtPath;
            }

            Vector3D vPerp1(vel.z, 0, -vel.x );
            vPerp1.Normalize();

            Vector3D vNorml = vel ^ vPerp1.Unit();
            vNorml.Normalize();

            for( int y = -split/2; y <= split/2; y++ )
            {
                for( int x= -split/2; x <= split/2; x++ )
                {
                    Vector3D position = position0 + x * fFactor * vPerp1 + y * fFactor * vNorml * 0.75;
                    Vector3D downwash;

                    std::vector<Vector3F> vertices;
                    std::vector<Vector4F> colors;

                    vertices.push_back(position.toFloat());
                    colors.push_back({1,1,1,1});

                    bool bDownWashed = false;

                    for( int i=0; i < iSample; i++ )
                    {
                        Vector3D angularVelocity = (pBody->angularVelocity() ^ position);

                        Vector3D vel = -pBody->toLocalGroundFrame(
                                    getWorld()->getWeather()->getWindFromPosition(position)) + angularVelocity + pBody->velocityBody();

                        //if( !bDownWashed )
                        //if( !(bDownWashed = applyDownwash(lAfg, position, downwash)) )
                        //bDownWashed = applyDownwash(rAfg, position, downwash);

                        //vel -= downwash;

                        float fRainbowFactor = (pBody->velocityBody() + angularVelocity).Magnitude() - pBody->velocityBody().Magnitude();

                        fRainbowFactor *= 40;
                        fRainbowFactor += 80;

                        if( fRainbowFactor < 1 ) fRainbowFactor = 1;
                        if( fRainbowFactor > 200 ) fRainbowFactor = 200;
                        rgbColor = HSLtoRGB(fRainbowFactor, lum, sat);

                        GLubyte r = rgbColor.R;
                        GLubyte g = rgbColor.G;
                        GLubyte b = rgbColor.B;

                        position -= vel * dtPath;
                        colors.push_back({r/255.0f,g/255.0f,b/255.0f,1});

                        vertices.push_back(position.toFloat());
                    }

                    r->bindVertex(Renderer::Vertex, 3, &vertices[0].x);
                    r->bindVertex(Renderer::Color, 4, &colors[0].x);
                    r->setPrimitiveType(GL_LINE_STRIP);
                    r->setUseIndex(false);
                    r->setVertexCountOffset(vertices.size());
                    r->Render();
                }
            }
        }
    }
    else
    {
        glLineWidth(2.0f);
        std::vector<Vector3F> vertices;
        std::vector<Vector4F> colors;

        for( int y= 0; y < split; y++ )
        {
            for( int x= 0; x < split; x++ )
            {
                for( int z = 0; z < split; z ++ )
                {
                    Vector3D position( x * fFactor - fCenter/2, y * fFactor- fCenter/2, z * fFactor - fCenter/2);

                    Vector3D angularVelocity = (pBody->angularVelocity() ^ position);
                    Vector3D vel = pBody->toLocalGroundFrame(
                                getWorld()->getWeather()->getWindFromPosition(position)) - angularVelocity;

                    float fRainbowFactor = (pBody->velocityBody().Unit() + angularVelocity.Unit()).Magnitude();

                    fRainbowFactor = (fRainbowFactor-1) * 100 + 80;
                    if( fRainbowFactor < 1 ) fRainbowFactor = 1;
                    if( fRainbowFactor > 220 ) fRainbowFactor = 220;
                    rgbColor = HSLtoRGB(fRainbowFactor, lum, sat);

                    GLubyte r = rgbColor.R;
                    GLubyte g = rgbColor.G;
                    GLubyte b = rgbColor.B;

                    Vector3D result = position + vel.Unit() * angularVelocity.Magnitude()/10.0;

                    vertices.push_back(position.toFloat());
                    colors.push_back({r/255.0f,g/255.0f,b/255.0f,1});

                    vertices.push_back(result.toFloat());
                    colors.push_back({r/255.0f,g/255.0f,b/255.0f,1});

                    Vector3D out[2];
                    GenerateArrowHead(position, result, out);

                    vertices.push_back(out[0].toFloat());
                    colors.push_back({r/255.0f,g/255.0f,b/255.0f,1});

                    vertices.push_back(result.toFloat());
                    colors.push_back({r/255.0f,g/255.0f,b/255.0f,1});

                    vertices.push_back(out[1].toFloat());
                    colors.push_back({r/255.0f,g/255.0f,b/255.0f,1});
                }
            }
        }

        r->bindVertex(Renderer::Vertex, 3, &vertices[0].x);
        r->bindVertex(Renderer::Color, 4, &colors[0].x);
        r->setPrimitiveType(GL_LINES);
        r->setUseIndex(false);
        r->setVertexCountOffset(vertices.size());
        r->Render();

        glLineWidth(1.0f);
    }
}

bool WindTunnelForceGenerator::applyDownwash(AeroForceGenerator &afg, Vector3F &position, Vector3D &downwash)
{
    for( size_t i=0; i < afg.size(); i++ )
    {
        AeroSectionForceGenerator *sefg = afg.element(i);

        if( sefg->behind(position) )
            for( size_t j= 0; j < sefg->size(); j++ )
            {
                AeroSectionElementForceGenerator *efg = sefg->element(j);
                if( efg->behind(position) )
                    for( size_t k= 0; k < efg->size(); k++ )
                    {
                        AeroSectionSubElementForceGenerator *sefg = efg->element(k);
                        if( sefg->behind(position) )
                        {
                            downwash = sefg->getDownWash();
                            return true;
                        }
                    }
            }
    }
    return false;
}
