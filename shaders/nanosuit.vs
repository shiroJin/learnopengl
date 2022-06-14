#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out VS_OUT {
  vec3 FragPos;
  vec2 TexCoords;
  vec3 TangentLightDir;
  vec3 TangentViewPos;
  vec3 TangentFragPos;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightDir;
uniform vec3 viewPos;

void main()
{
  vec3 fragPos = vec3(model * vec4(aPos, 1.0));
  gl_Position = projection * view * vec4(fragPos, 1.0);
  vs_out.FragPos = fragPos;
  vs_out.TexCoords = aTexCoords;

  mat3 normalMatrix = transpose(inverse(mat3(model)));
  vec3 T = normalize(normalMatrix * aTangent);
  vec3 B = normalize(normalMatrix * aBitangent);
  vec3 N = normalize(normalMatrix * aNormal);
  mat3 TBN = transpose(mat3(T, B, N));

  vs_out.TangentFragPos = TBN * fragPos;
  vs_out.TangentViewPos = TBN * viewPos;
  vs_out.TangentLightDir = TBN * lightDir;
}