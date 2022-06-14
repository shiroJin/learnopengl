#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main()
{
  float ratio = 1.00 / 1.33;
  vec3 I = normalize(Position - cameraPos);
  vec3 R = reflect(I, normalize(Normal));
  FragColor = vec4(texture(skybox, vec3(R.x, -R.y, R.z)).rgb, 1.0);
}