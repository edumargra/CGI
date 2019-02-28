#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the input locations of attributes
layout (location = 0) in vec3 vertCoordinates_in;
layout (location = 1) in vec3 vertNormal_in;

// Specify the Uniforms of the vertex shader
 uniform mat4 modelViewTransform;
 uniform mat4 projectionTransform;
 uniform mat3 normalTransform;
 uniform vec3 material;
 uniform vec3 lightCoordinates;
 uniform vec3 lightColor;
 uniform vec3 lightColor2;

// Specify the output of the vertex stage
out vec3 vertNormal;
out vec3 vertMat;
out vec3 lCoord;
out vec3 lC;
out vec3 lC2;
out vec3 V;

void main()
{
    // gl_Position is the output (a vec4) of the vertex shader
    gl_Position = projectionTransform * modelViewTransform * vec4(vertCoordinates_in, 1.0);
    V = normalize(vec3(modelViewTransform * vec4(vertCoordinates_in,1.0)));
    vertNormal = vertNormal_in;
    vertMat = material;
    lCoord = lightCoordinates;
    lC = lightColor;
    lC2 = lightColor2;

}
