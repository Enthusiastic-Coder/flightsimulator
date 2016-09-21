attribute vec3 vertexIn;
attribute vec3 normalIn;
attribute vec2 textureIn;

uniform mat3 normalMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 modelViewProjectionMatrix;

uniform mat4 lightModelViewProjectionMatrix1;
uniform mat4 lightModelViewProjectionMatrix2;
uniform mat4 lightModelViewProjectionMatrix3;

varying vec4 lightVertexPosition1;
varying vec4 lightVertexPosition2;
varying vec4 lightVertexPosition3;

varying vec3 outPosition;
varying vec3 outNormal;
varying vec2 outUV; 

varying vec4 viewTangetSpace;
varying vec4 lightTangetSpace;

uniform float nUVOffset;
uniform vec3 eyeLightDirection;

// Flat Water assumed
const vec3 tangent = vec3(1.0, 0.0, 0.0);
const vec3 normal = vec3(0.0, 1.0, 0.0);
const vec3 biTangent = vec3(0.0, 0.0, 1.0);
	

void main()
{
	lightVertexPosition1 = lightModelViewProjectionMatrix1 * vec4(vertexIn,1.0);
	lightVertexPosition2 = lightModelViewProjectionMatrix2 * vec4(vertexIn,1.0);
	lightVertexPosition3 = lightModelViewProjectionMatrix3 * vec4(vertexIn,1.0);

	outPosition = vec3(modelViewMatrix * vec4(vertexIn,1.0));
	outNormal = normalMatrix * normalIn;
	outUV = textureIn + nUVOffset;

	// Compute tangent space for the light direction
	lightTangetSpace.x = dot(eyeLightDirection, tangent);
	lightTangetSpace.y = dot(eyeLightDirection, biTangent);
	lightTangetSpace.z = dot(eyeLightDirection, normal);
	lightTangetSpace.w = 1.0;
	
	// Compute tangent space for the view direction
	viewTangetSpace.x = dot(outPosition, tangent);
	viewTangetSpace.y = dot(outPosition, biTangent);
	viewTangetSpace.z = dot(outPosition, normal);
	viewTangetSpace.w = 1.0;

	gl_Position = modelViewProjectionMatrix * vec4(vertexIn,1.0);
}