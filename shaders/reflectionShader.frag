varying mediump vec2 outUV;

uniform sampler2D texID;
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

uniform mediump vec2 dims;

void main()
{
    mediump vec4 textureColor = vec4(1,1,1,1);

    if(useTexID != 0)
    {
        textureColor = texture2D(texID,outUV);
        if( textureColor.a < 0.01)
            discard;
    }

    if( debug == 0 )
    {
        gl_FragColor = material.ambient * textureColor;
        gl_FragColor += material.diffuse * textureColor;
    }
    else
    {
        gl_FragColor = vec4(1, 1, 1, 0.9);
    }
}
