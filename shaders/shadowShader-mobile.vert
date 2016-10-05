attribute vec3 vertexIn;
attribute vec3 normalIn;
attribute vec2 textureIn;

uniform mat3 normalMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 modelViewProjectionMatrix;

uniform mat4 lightModelViewProjectionMatrix1;
varying vec4 lightVertexPosition1;

varying vec3 outPosition;
varying vec3 outNormal;
varying vec2 outUV;

void main()
{
	lightVertexPosition1 = lightModelViewProjectionMatrix1 * vec4(vertexIn,1.0);

        outPosition = vec3(modelViewMatrix * vec4(vertexIn,1.0));
	outNormal = normalMatrix * normalIn;
	outUV = textureIn;

	gl_Position = modelViewProjectionMatrix * vec4(vertexIn,1.0);
}
