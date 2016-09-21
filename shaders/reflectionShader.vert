attribute vec3 vertexIn;
attribute vec3 normalIn;
attribute vec2 textureIn;

uniform mat3 normalMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 modelViewProjectionMatrix;
varying vec2 outUV;

void main()
{
	gl_Position = modelViewProjectionMatrix * vec4(vertexIn,1.0);
	outUV=textureIn;
}
