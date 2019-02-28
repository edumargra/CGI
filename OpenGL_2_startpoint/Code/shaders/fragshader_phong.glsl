#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the inputs to the fragment shader
// These must have the same type and name!
in vec3 V;
in vec3 vertNormal;
in vec3 vertMat;
in vec3 lCoord;
in vec3 lC;
in vec3 lC2;

// Specify the Uniforms of the fragment shaders
// uniform vec3 lightPosition; // for example

// Specify the output of the fragment shader
// Usually a vec4 describing a color (Red, Green, Blue, Alpha/Transparency)
out vec4 fNormal;

void main()
{
    vec3 Ia =  lC2 * vertMat[0];
    vec3 Id =  lC * vertMat[1] * max(dot(vertNormal,normalize(lCoord-vec3(gl_FragCoord))),0);
    vec3 Is =  lC * vertMat[2] * pow(max(0,dot(reflect(normalize(lCoord-vec3(gl_FragCoord)),vertNormal),V)),32);
    fNormal = vec4(Ia+Id+Is, 1.0);
}
