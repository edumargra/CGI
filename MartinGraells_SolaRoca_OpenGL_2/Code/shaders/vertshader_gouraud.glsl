#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the input locations of attributes
layout (location = 0) in vec3 vertCoordinates_in;
layout (location = 1) in vec3 vertNormal_in;
layout (location = 2) in vec2 vertTexCoord_in;

// Specify the Uniforms of the vertex shader
uniform mat4 modelViewTransform;
uniform mat4 projectionTransform;
uniform mat3 normalTransform;
uniform vec4 material;
uniform vec3 lightCoordinates;
uniform vec3 lightColor;

// Specify the output of the vertex stage
out vec2 vertTexCoord;
out vec3 colorI;

void main()
{
    //We know for sure that the computation of L is incorrect but we don't know what is wrong
    //Light components computation
    vec3 L = normalize(vec3(modelViewTransform*vec4(lightCoordinates,1.0)) - vec3(modelViewTransform*vec4(vertCoordinates_in,1.0)));
    vec3 V = normalize(-vec3(modelViewTransform*vec4(vertCoordinates_in,1.0)));
    vec3 N = normalize(normalTransform*vertNormal_in);
    vec3 R = -reflect(L,N);
    vec3 Ia = vec3(material.x);
    vec3 Id = lightColor * material.y * max(dot(L,N),0.0);
    vec3 Is = lightColor * material.z * pow(max(dot(R,V),0.0), material.w);
    colorI = Ia+Id+Is;
    // gl_Position is the output (a vec4) of the vertex shader
    gl_Position = projectionTransform * modelViewTransform * vec4(vertCoordinates_in, 1.0);
    //Texture
    vertTexCoord = vertTexCoord_in;
}
