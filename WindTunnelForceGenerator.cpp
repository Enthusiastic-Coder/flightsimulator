#include "StdAfx.h"
#include "WindTunnelForceGenerator.h"
#include "AeroSectionElementForceGenerator.h"
#include "AeroSectionSubElementForceGenerator.h"
#include "AeroForceGenerator.h"
#include "WorldSystem.h"


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
    COLORREF rgbColor ;

    int split = 15;
    //split = 10;

    double fCenter = 40.0f;
    double fFactor = fCenter/(split-1);

    double fVelocity = pBody->velocityBody().Magnitude()
            + pBody->toLocalGroundFrame(
                getWorld()->getWeather()->getWindFromPosition(Vector3D())).Magnitude();

    if( !_bPitchRate )
    {
        glLineWidth(2.0f);
        glBegin(GL_LINES);
    }

    glColor3f(1,1,1);

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

            for( int y= -split/2; y <= split/2; y++ )
            {
                for( int x= -split/2; x <= split/2; x++ )
                {
                    Vector3D position = position0 + x * fFactor * vPerp1 + y * fFactor * vNorml * 0.75;
                    Vector3D downwash;

                    glBegin(GL_LINE_STRIP);

                    glVertex3dv( &position.x );
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
                        rgbColor = ::ColorHLSToRGB( fRainbowFactor, lum, sat );

                        BYTE r = GetRValue(rgbColor);
                        BYTE g = GetGValue(rgbColor);
                        BYTE b = GetBValue(rgbColor);
                        glColor3ub( r,g,b);

                        position -= vel * dtPath;
                        glVertex3dv( &position.x );
                    }
                    glEnd();
                }
            }
        }
    }
    else
    {
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
                    rgbColor = ::ColorHLSToRGB( fRainbowFactor, lum, sat );

                    BYTE r = GetRValue(rgbColor);
                    BYTE g = GetGValue(rgbColor);
                    BYTE b = GetBValue(rgbColor);
                    glColor3ub( r,g,b);

                    Vector3D result = position + vel.Unit() * angularVelocity.Magnitude()/10.0;

                    glVertex3dv(&position.x);
                    glVertex3dv(&result.x);

                    Vector3D out[2];
                    GenerateArrowHead(position, result, out);
                    glVertex3dv(&out[0].x);
                    glVertex3dv(&result.x);
                    glVertex3dv(&out[1].x);
                }
            }
        }
    }

    if( !_bPitchRate )
    {
        glEnd();
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
