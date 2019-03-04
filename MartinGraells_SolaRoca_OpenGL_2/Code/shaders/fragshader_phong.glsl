#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the inputs to the fragment shader
// These must have the same type and name!
in vec3 vertCoordinates;
in vec3 vertNormal;

// Specify the Uniforms of the fragment shaders
uniform vec4 material;
uniform vec3 lightCoordinates;
uniform vec3 lightColor;
uniform vec3 materialColor;
uniform mat4 modelViewTransform;
uniform mat4 projectionTransform;
uniform mat3 normalTransform;
// Specify the output of the fragment shader
// Usually a vec4 describing a color (Red, Green, Blue, Alpha/Transparency)
out vec4 fColor;

void main()
{
    vec3 L = normalize(vec3(modelViewTransform*vec4(lightCoordinates-vertCoordinates,1.0)));
    vec3 N = normalize(vertNormal);
    vec3 R = -reflect(L,N);
    vec3 V = -normalize(vec3(modelViewTransform * vec4(vertCoordinates, 1.0)));
    vec3 Ia =  materialColor * material[0];
    vec3 Id =  materialColor * lightColor * material[1] * max(dot(N,L),0);
    vec3 Is =  lightColor * material[2] * pow(max(0.0,dot(R,V)),material[3]);
    fColor = vec4(Ia + Id + Is, 1.0);
}
