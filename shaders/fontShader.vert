attribute vec3 vertexIn;
attribute vec2 textureIn;

uniform mat4 modelViewProjectionMatrix;

varying vec2 outUV;

void main()
{
	gl_Position = modelViewProjectionMatrix * vec4(vertexIn,1.0);
	outUV=textureIn;
}
