#version 150 core

in vec3 position;
//in vec3 inColor;

//const vec3 inColor = vec3(0.f,0.7f,0.f);
const vec3 inLightDir = normalize(vec3(-20,20,-20));
const vec3 lightSource1 = vec3(0, 0, -1);
// const vec3 lightSource1 = vec3(position.x, position.y, 1);
const vec3 lightSource2 = vec3(0, 0, 2);
in vec3 inNormal;
in vec2 inTexcoord;

out vec3 Color1;
out vec3 Color2;
out vec3 Color3;
out vec3 vertNormal;
out vec3 pos;
out vec2 texcoord;

out vec3 lightDir1;
out vec3 lightDir2;
out vec3 lightDir3;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform vec3 inColor;

void main() {
   Color1 = inColor;
   gl_Position = proj * view * model * vec4(position,1.0);
   pos = (view * model * vec4(position,1.0)).xyz;
   lightDir1 = (view * vec4(inLightDir,0.0)).xyz; //It's a vector!
   //*** for point light ***//
   // vec3 lightSource1 = vec3(position.x, position.y, 5);
   Color2 = vec3(.4,.4, .3)*(1/(length(position-lightSource1)*length(position-lightSource1)));
   Color3 = vec3(0,.2,.1)*(1/(length(position-lightSource2)*length(position-lightSource2)));
   lightDir2 = (view * vec4(normalize(position-lightSource1),0.0)).xyz;;
   lightDir3 = (view * vec4(normalize(position-lightSource2),0.0)).xyz;;
   //*** for point light ***//
   vec4 norm4 = transpose(inverse(view*model)) * vec4(inNormal,0.0);
   vertNormal = normalize(norm4.xyz);
   texcoord = inTexcoord;
}
