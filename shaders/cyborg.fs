#version 330 core
out vec4 FragColor;

in VS_OUT {
  vec3 Normal;
  vec3 Position;
} fs_in;

uniform vec3 viewPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_height1;
uniform samplerCube cubemap;

void main()
{
  float ratio = 1.00 / 1.52;
  vec3 I = normalize(fs_in.Position - viewPos);
  vec3 R = refract(I, normalize(fs_in.Normal), ratio);
  FragColor = vec4(texture(cubemap, vec3(R.x, -R.y, R.z)).rgb, 1.0);
}