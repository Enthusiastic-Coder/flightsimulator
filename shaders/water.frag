varying mediump vec3 outPosition;
varying mediump vec3 outNormal;
varying mediump vec2 outUV;

varying mediump vec4 viewTangetSpace;
varying mediump vec4 lightTangetSpace;


uniform sampler2D texID;
uniform sampler2D reflectionMap;
uniform sampler2D normalMap;
uniform sampler2D dudvMap;

uniform mediump vec3 eyeLightDirection;

uniform mediump vec4 waterColor;
uniform mediump vec2 dims;

uniform int useTexID;
uniform int debug;

struct MATERIAL
{	
    mediump vec4 ambient;
    mediump vec4 diffuse;
    mediump vec4 specular;
    mediump float shininess;
};

uniform MATERIAL material;

/////////////////////////////////////////////////////
varying mediump vec4 lightVertexPosition1;
varying mediump vec4 lightVertexPosition2;
varying mediump vec4 lightVertexPosition3;

uniform sampler2D shadowMap1;
uniform sampler2D shadowMap2;
uniform sampler2D shadowMap3;

float calcShadowValue()
{
    mediump float shadowValue = 0.0;

    int sampleCount = 0;

    if(length(outPosition) < 2500.0)
    {
        mediump vec4 lightVertexPosition1W = lightVertexPosition1 / lightVertexPosition1.w;
        mediump vec4 lightVertexPosition2W = lightVertexPosition2 / lightVertexPosition2.w;
        mediump vec4 lightVertexPosition3W = lightVertexPosition3 / lightVertexPosition3.w;

        mediump vec2 shadowUV1;
        mediump vec2 shadowUV2;
        mediump vec2 shadowUV3;

        for(int x=-1; x <=1; ++x)
        {
            for(int y=-1; y<=1; ++y)
            {
                sampleCount++;

                vec2 offset = vec2( dims.x*float(x), dims.y*float(y) );
                shadowUV1 = lightVertexPosition1W.xy + offset;

                if( shadowUV1.x >= 0.0 && shadowUV1.x <=1.0 && shadowUV1.y >= 0.0 && shadowUV1.y <= 1.0 )
                {
                    if(lightVertexPosition1.z > 1.0)
                        shadowValue++;

                    else if( texture2D(shadowMap1,shadowUV1).r >= (lightVertexPosition1W.z-0.001))
                        shadowValue++;
                }
                else
                {
                    shadowUV2 = lightVertexPosition2W.xy + offset;
                    if( shadowUV2.x >= 0.0 && shadowUV2.x <=1.0 && shadowUV2.y >= 0.0 && shadowUV2.y <= 1.0 )
                    {
                        if(lightVertexPosition2.z > 1.0)
                            shadowValue++;
                        else if( texture2D(shadowMap2,shadowUV2).r >= (lightVertexPosition2W.z-0.001))
                            shadowValue++;
                    }
                    else
                    {
                        shadowUV3 = lightVertexPosition3W.xy + offset;

                        if( shadowUV3.x >= 0.0 && shadowUV3.x <=1.0 && shadowUV3.y >= 0.0 && shadowUV3.y <= 1.0 )
                        {
                            if(lightVertexPosition3.z > 1.0)
                                shadowValue++;
                            else
                                if( texture2D(shadowMap3,shadowUV3).r >= (lightVertexPosition3W.z-0.001))
                                    shadowValue++;
                        }
                        else
                            shadowValue++;
                    }

                }
            }
        }

        shadowValue /= float(sampleCount);

        shadowValue = max(shadowValue,0.1) + 0.05;
        //r==1.0 not in shadow  //r==0.0 it's in shadow
    }
    else
        shadowValue = 1.0;

    return shadowValue;
}
/////////////////////////////////////////////////////



void main()
{
    mediump float shadowValue = max( 0.75, calcShadowValue());
    mediump vec4 dudvValue = texture2D(dudvMap, outUV);
    dudvValue = normalize(dudvValue * 2.0 - 1.0) / 5.0;

    mediump vec4 N = texture2D(normalMap, outUV);
    N = N * 2.0 - 1.0;
    N.a = 0.0;

    mediump vec4 L = normalize(lightTangetSpace);
    mediump float NdotL = max(dot(N,L), 0.0);
    mediump vec4 E = normalize(-viewTangetSpace);
    mediump vec4 HV = normalize( lightTangetSpace + E);
    mediump float NdotHV = max( dot( N,HV),0.0 );

    //vec4 localView = -normalize(viewTangetSpace);
    //float NdotHV = max(0.0, dot(lightReflection, localView) );
    mediump vec4 lightReflection = normalize( reflect(-lightTangetSpace, N) );
    mediump vec4 invFresnelTerm = vec4( dot(N, lightReflection ) );

    mediump vec4 fresnelTerm = 1.0 - invFresnelTerm;

    mediump vec4 scrnCoord = gl_FragCoord * vec4(dims, 0.0, 1.0) + dudvValue;
    scrnCoord = clamp(scrnCoord, 0.0, 1.0);

    mediump vec4 reflectionColor =	texture2D(reflectionMap, scrnCoord.xy) * fresnelTerm * shadowValue;
    mediump vec4 seaColor = (material.ambient + material.diffuse) * invFresnelTerm;

    mediump vec4 specular = material.specular * pow(NdotHV, material.shininess ) / (length(L)*1.5);

    gl_FragColor = mix(reflectionColor, seaColor, 0.75) + specular;
}
