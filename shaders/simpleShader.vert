attribute vec3 vertexIn;

uniform mat4 modelViewProjectionMatrix;

void main()
{
	gl_Position = modelViewProjectionMatrix * vec4(vertexIn,1.0);
}
