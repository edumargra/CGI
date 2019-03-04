#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the inputs to the fragment shader
// These must have the same type and name!
in vec3 vertCoordinates;
in vec3 vertNormal;
in vec2 textureCoord;

// Specify the Uniforms of the fragment shaders
uniform vec4 material;
uniform vec3 lightCoordinates;
uniform vec3 lightColor;
uniform vec3 materialColor;
uniform mat4 modelViewTransform;
uniform mat4 projectionTransform;
uniform mat3 normalTransform;
uniform sampler2D samplerUniform;

// Specify the output of the fragment shader
// Usually a vec4 describing a color (Red, Green, Blue, Alpha/Transparency)
out vec4 fColor;

void main()
{
    samplerUniform;
    textureCoord;
    //Light components computation
    vec3 L = normalize(vec3(modelViewTransform*vec4(lightCoordinates-vertCoordinates,0.0)));
    vec3 V = -normalize(vec3(modelViewTransform*vec4(vertCoordinates,1.0)));
    vec3 N = normalize(vertNormal);
    vec3 R = -reflect(L,N);
    vec3 Ia = vec3(1,1,1) * material.x;
    vec3 Id = lightColor * material.y * max(dot(L,N),0);
    vec3 Is = lightColor * material.z * pow(max(dot(R,V),0), material.w);
    fColor = texture2D(samplerUniform,textureCoord)*vec4(Ia + Id + Is,1.0);
}
