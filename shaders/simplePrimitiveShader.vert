attribute vec3 vertexIn;
attribute vec4 colorIn;

uniform mat4 modelViewProjectionMatrix;

varying vec4 colorOut;

void main()
{
	gl_Position = modelViewProjectionMatrix * vec4(vertexIn,1.0);
	colorOut = colorIn;
}
