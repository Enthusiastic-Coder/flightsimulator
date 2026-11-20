varying mediump vec2 lampCorner;
uniform mediump vec3 lampColour;
uniform mediump float lampAlpha;
uniform mediump float strobe;

void main()
{
    float radius = length(lampCorner);
    float core = 1.0 - smoothstep(0.21, 0.47, radius);
    float innerGlow = 1.0 - smoothstep(0.14, 0.67, radius);
    float outerGlow = (1.0 - smoothstep(0.20, 1.0, radius)) * (1.0 - radius * 0.65);

    // A short optical star around the flash, with no rays on steady lamps.
    float ray = (1.0 - smoothstep(0.015, 0.09, min(abs(lampCorner.x), abs(lampCorner.y))))
        * (1.0 - smoothstep(0.30, 1.0, radius));
    float intensity = 0.67 * core + 0.20 * innerGlow + 0.16 * outerGlow + 0.28 * strobe * ray;
    vec3 colour = mix(lampColour, vec3(1.0), core * 0.48);
    gl_FragColor = vec4(colour, clamp(lampAlpha * intensity, 0.0, 1.0));
}
