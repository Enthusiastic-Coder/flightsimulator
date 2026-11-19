varying mediump vec2 lampCorner;
uniform mediump vec3 lampColour;
uniform mediump float lampAlpha;

void main()
{
    float radius = length(lampCorner);
    float core = 1.0 - smoothstep(0.13, 0.45, radius);
    float halo = 1.0 - smoothstep(0.25, 1.0, radius);
    gl_FragColor = vec4(lampColour, lampAlpha * (0.76 * core + 0.24 * halo));
}
