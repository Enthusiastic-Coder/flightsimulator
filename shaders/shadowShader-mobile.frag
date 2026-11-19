varying mediump vec3 outPosition;
varying mediump vec3 outNormal;
varying highp vec2 outUV;

uniform int useTex;
uniform sampler2D texID;
uniform int groundDetailEnabled;
uniform sampler2D groundDetailTexture;
uniform highp vec2 groundDetailOrigin;
uniform highp vec2 groundDetailU;
uniform highp vec2 groundDetailV;
uniform mediump vec3 eyeLightDirection;

struct MATERIAL
{	
    mediump vec4 ambient;
    mediump vec4 diffuse;
    mediump vec4 specular;
    mediump float shininess;
};

uniform MATERIAL material;
uniform int debug;

/////////////////////////////////////////////////////
varying mediump vec4 lightVertexPosition1;
uniform sampler2D shadowMap1;

void main()
{
    //mediump float shadowValue = calcShadowValue();

    mediump vec4 lightVertexPosition1W = lightVertexPosition1/lightVertexPosition1.w;

    mediump float shadowValue = 1.0;

    if( lightVertexPosition1W.x >= 0.0 && lightVertexPosition1W.x <= 1.0
            && lightVertexPosition1W.y >= 0.0 && lightVertexPosition1W.y <= 1.0)
    {
        if (texture2D(shadowMap1,lightVertexPosition1W.xy).r < (lightVertexPosition1W.z-0.001))
            shadowValue = 0.0;
    }
    shadowValue = max(shadowValue,0.4) + 0.05;


    mediump vec3 N = normalize(outNormal);
    mediump vec3 L = normalize(eyeLightDirection);
    mediump vec3 E = normalize(-outPosition.xyz);
    mediump float NdotL = max(dot(N,L), 0.0);
    mediump vec3 HV = normalize( L + E);
    mediump float NdotHV = max( dot( N,HV),0.0 );

    mediump vec4 textureColor = texture2D(texID,outUV);
    if( useTex == 0)
        textureColor = vec4(1,1,1,1);

    mediump float detailWeight = 0.0;
    if (groundDetailEnabled != 0)
    {
        highp vec2 offset = outUV - groundDetailOrigin;
        highp vec2 detailUV = vec2(dot(offset, groundDetailU), dot(offset, groundDetailV));
        mediump vec4 detail = texture2D(groundDetailTexture, detailUV);
        detailWeight = detail.a;
        textureColor.rgb = mix(textureColor.rgb, detail.rgb, detailWeight);
    }

    //if( textureColor.a < 0.00001)
      //  discard;

    if( debug == 0 )
    {
        gl_FragColor = material.ambient * textureColor;
        gl_FragColor += shadowValue * material.diffuse * NdotL * textureColor;
        gl_FragColor += (1.0 - detailWeight) * shadowValue * material.specular * pow( NdotHV, material.shininess )/ (length(L)*1.5);
        gl_FragColor.a = textureColor.a;
    }
    else
    {
        gl_FragColor = vec4(1,1,1,0.9) * NdotL;
    }
}
