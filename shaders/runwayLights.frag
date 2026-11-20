varying mediump vec2 lampCorner;
uniform mediump vec3 lampColour;
uniform mediump float lampAlpha;

void main()
{
    float radius = length(lampCorner);
    float core = 1.0 - smoothstep(0.20, 0.56, radius);
    float halo = 1.0 - smoothstep(0.25, 1.0, radius);
    gl_FragColor = vec4(lampColour, lampAlpha * (0.68 * core + 0.32 * halo));
}
