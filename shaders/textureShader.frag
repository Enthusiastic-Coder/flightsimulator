uniform sampler2D texID;
varying mediump vec2 outUV;
uniform lowp vec4 colorModulator;

void main()
{
    gl_FragColor = colorModulator * texture2D(texID,outUV);
}
