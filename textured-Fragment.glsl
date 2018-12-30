#version 150 core

in vec3 Color1;
in vec3 Color2;
in vec3 Color3;
in vec3 vertNormal;
in vec3 pos;
in vec2 texcoord;

in vec3 lightDir1;
in vec3 lightDir2;
in vec3 lightDir3;

out vec4 outColor;

uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;

uniform int texID;

const float ambient = .3;
void main() {
  vec3 color;
  vec3 normal = normalize(vertNormal);
  if (texID == -1)
    color = Color1;
  else if (texID == 0)
    color = texture(tex0, texcoord).rgb;
  else if (texID == 1)
    color = texture(tex1, texcoord).rgb;
  else if (texID == 2) // for keys & doors
    color = vec3(0.7,0,0);
  else if (texID == 3)
    color = vec3(0.8,0.8,0);
  else if (texID == 4)
    color = vec3(0.8,0.4,0);
  else if (texID == 5)
    color = vec3(0.1,0.1,0.1);
  else if (texID == 6)
    color = vec3(1,1,1);
  else if (texID == 7)
    color = vec3(1,.6,.6);
  else{
    outColor = vec4(1,0,0,1);
    return; //This was an error, stop lighting!
  }
  // for bump map
  if (texID==0) normal = normalize(vertNormal + texture(tex3, texcoord).rgb);
  else if(texID==1) normal = normalize(vertNormal + texture(tex2, texcoord).rgb);


  vec3 diffuseC = color*max(dot(-lightDir1,normal),0.0);
  vec3 ambC = color*ambient;
  vec3 viewDir = normalize(-pos); //We know the eye is at (0,0)! (Do you know why?)
  vec3 reflectDir = reflect(viewDir,normal);
  float spec = max(dot(reflectDir,lightDir1),0.0);
  if (dot(-lightDir1,normal) <= 0.0) spec = 0; //No highlight if we are not facing the light
  vec3 specC = .8*Color1*pow(spec,4);
  //*** for point light ***//
  diffuseC += Color2*max(dot(-lightDir2,normal),0.0) + Color3*max(dot(-lightDir3,normal),0.0);
  spec = max(dot(reflectDir,lightDir2),0.0);
  if (dot(-lightDir2,normal) <= 0.0)spec = 0;
  specC += .8*Color2*pow(spec,4);
  spec = max(dot(reflectDir,lightDir3),0.0);
  if (dot(-lightDir3,normal) <= 0.0)spec = 0;
  specC += .8*Color3*pow(spec,4);
  //*** //*** for point light ***//
  vec3 oColor = ambC+diffuseC+specC;
  outColor = vec4(oColor,1);
}
