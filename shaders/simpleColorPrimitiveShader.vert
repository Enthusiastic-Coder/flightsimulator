attribute vec2 vertexIn;
uniform mat4 modelViewProjectionMatrix;

void main()
{
    gl_Position = modelViewProjectionMatrix * vec4(vertexIn, 0.0 ,1.0);
}
