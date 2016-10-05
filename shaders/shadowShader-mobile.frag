varying mediump vec3 outPosition;
varying mediump vec3 outNormal;
varying mediump vec2 outUV;

uniform int useTex;
uniform sampler2D texID;
uniform mediump vec3 eyeLightDirection;

struct MATERIAL
{	
    mediump vec4 ambient;
    mediump vec4 diffuse;
    mediump vec4 specular;
    mediump float shininess;
};

uniform MATERIAL material;

uniform mediump vec2 dims;
uniform int debug;

/////////////////////////////////////////////////////
varying mediump vec4 lightVertexPosition1;
uniform sampler2D shadowMap1;

float calcShadowValue()
{
    mediump float shadowValue = 0.0;

    mediump vec4 lightVertexPosition1W = lightVertexPosition1 / lightVertexPosition1.w;
    mediump vec2 shadowUV1;

    int sampleCount = 0;

    if(length(outPosition) < 2500.0)
    {
        for(int x=-1; x <=1; ++x)
        {
            for(int y=-1; y<=1; ++y)
            {
                sampleCount++;

                mediump vec2 offset = vec2( dims.x*float(x), dims.y*float(y) );
                shadowUV1 = lightVertexPosition1W.xy + offset;

                if( shadowUV1.x >= 0.0 && shadowUV1.x <=1.0 && shadowUV1.y >= 0.0 && shadowUV1.y <= 1.0 )
                {
                    if(lightVertexPosition1.z > 1.0)
                        shadowValue++;

                    else if( texture2D(shadowMap1,shadowUV1).r >= (lightVertexPosition1W.z-0.001))
                        shadowValue++;
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
    mediump float shadowValue = calcShadowValue();

    mediump vec3 N = normalize(outNormal);
    mediump vec3 L = normalize(eyeLightDirection);
    mediump vec3 E = normalize(-outPosition.xyz);
    mediump float NdotL = max(dot(N,L), 0.0);
    mediump vec3 HV = normalize( L + E);
    mediump float NdotHV = max( dot( N,HV),0.0 );

    mediump vec4 textureColor = texture2D(texID,outUV);
    if( useTex == 0)
        textureColor = vec4(1,1,1,1);

    if( textureColor.a < 0.00001)
        discard;

    if( debug == 0 )
    {
        gl_FragColor = material.ambient * textureColor;
        gl_FragColor += shadowValue * material.diffuse * NdotL * textureColor;
        gl_FragColor += shadowValue * material.specular * pow( NdotHV, material.shininess )/ (length(L)*1.5);
        gl_FragColor.a = textureColor.a;
    }
    else
    {
        gl_FragColor = vec4(1,1,1,0.9) * NdotL;
    }
}
