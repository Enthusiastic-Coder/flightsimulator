attribute vec3 vertexIn;
attribute vec2 textureIn;

uniform mat4 modelViewProjectionMatrix;
uniform vec2 viewportSize;
uniform float lampDiameter;

varying vec2 lampCorner;

void main()
{
    vec4 centre = modelViewProjectionMatrix * vec4(vertexIn, 1.0);
    float metresToPixels = viewportSize.y / max(centre.w, 1.0);
    float diameter = clamp(lampDiameter * metresToPixels, 3.1 + 2.1 * lampDiameter, 12.0 + 6.8 * lampDiameter);
    vec2 offset = textureIn * diameter / viewportSize * centre.w;
    gl_Position = centre + vec4(offset, 0.0, 0.0);
    lampCorner = textureIn;
}
