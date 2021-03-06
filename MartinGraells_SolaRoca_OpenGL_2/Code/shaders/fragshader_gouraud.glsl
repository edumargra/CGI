#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the inputs to the fragment shader
// These must have the same type and name!
in vec3 colorI;
in vec2 vertTexCoord;

// Specify the Uniforms of the fragment shaders
// uniform vec3 lightPosition; // for example
uniform sampler2D samplerUniform;
uniform vec3 materialColor;


// Specify the output of the fragment shader
// Usually a vec4 describing a color (Red, Green, Blue, Alpha/Transparency)
out vec4 fColor;

void main()
{
    materialColor; //Only for not having errors
    fColor = texture2D(samplerUniform,vertTexCoord)*vec4(colorI,1.0);
}
