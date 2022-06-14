#version 330 core
out vec4 FragColor;

in VS_OUT {
  vec3 FragPos;
  vec2 TexCoords;
  vec3 TangentLightDir;
  vec3 TangentViewPos;
  vec3 TangentFragPos;
} fs_in;

struct DirLight {
  vec3 direction;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

uniform DirLight dirLight;
uniform float height_scale;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_height1;

vec2 ParallaxMapping(vec2 TexCoords, vec3 viewDir);

void main()
{
  // Obtain normal from normal texture in range [0, 1], transfrom to [-1, 1]
  vec3 normal = texture(texture_normal1, fs_in.TexCoords).rgb;
  normal = normalize(normal * 2.0 - 1.0);
  // Direct light
  vec3 lightDir = normalize(fs_in.TangentLightDir);
  vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
  vec2 texCoords = ParallaxMapping(fs_in.TexCoords, viewDir);

  // Read diffuse color
  vec3 color = texture(texture_diffuse1, texCoords).rgb;

  // Ambient
  vec3 ambient = dirLight.ambient * color;
  
  // Diffuse
  float diff = max(dot(lightDir, normal), 0.0);
  vec3 diffuse = dirLight.diffuse * diff * color;

  // Specular
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
  // vec3 halfwayDir = normalize(lightDir + viewDir);  
  // float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
  vec3 specular = dirLight.specular * spec * texture(texture_specular1, texCoords).rgb;
  
  FragColor = vec4((ambient + diffuse + specular), 1.0);
}

vec2 ParallaxMapping(vec2 TexCoords, vec3 viewDir)
{
  float height = texture(texture_height1, TexCoords).r;
  vec2 p = viewDir.xy / viewDir.z * (height * height_scale);
  return TexCoords - p;
}