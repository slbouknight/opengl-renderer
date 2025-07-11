#version 330 core

// ----------------------------------------------------------------------------
//                lit_geometry.fs
// ----------------------------------------------------------------------------

// maximum number of point lights your app will ever support
#define MAX_POINT_LIGHTS 16

// material properties
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float     shininess;
};

// directional light data
struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// point light data
struct PointLight {
    vec3  position;
    vec3  ambient;
    vec3  diffuse;
    vec3  specular;
    float constant;
    float linear;
    float quadratic;
};

// spotlight data
struct SpotLight {
    vec3  position;
    vec3  direction;
    vec3  ambient;
    vec3  diffuse;
    vec3  specular;
    float constant;
    float linear;
    float quadratic;
    float cutOff;
    float outerCutOff;
};

uniform Material material;
uniform DirLight   dirLight;
uniform int        NR_POINT_LIGHTS;                // actual count at runtime
uniform PointLight pointLights[MAX_POINT_LIGHTS];   // fixed-size array
uniform SpotLight  spotLight;
uniform vec3       viewPos;

in vec3  FragPos;
in vec3  Normal;
in vec2  TexCoords;

out vec4 FragColor;

// ----------------------------------------------------------------------------
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir   = normalize(-light.direction);
    float diff      = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec      = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 amb  = light.ambient  * vec3(texture(material.diffuse,  TexCoords));
    vec3 dif  = light.diffuse  * diff                 * vec3(texture(material.diffuse,  TexCoords));
    vec3 spc  = light.specular * spec                * vec3(texture(material.specular, TexCoords));
    return amb + dif + spc;
}

// ----------------------------------------------------------------------------
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir   = normalize(light.position - fragPos);
    float diff      = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec      = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant 
                             + light.linear   * distance 
                             + light.quadratic * distance * distance);

    vec3 amb  = light.ambient  * vec3(texture(material.diffuse,  TexCoords));
    vec3 dif  = light.diffuse  * diff                 * vec3(texture(material.diffuse,  TexCoords));
    vec3 spc  = light.specular * spec                * vec3(texture(material.specular, TexCoords));

    amb  *= attenuation;
    dif  *= attenuation;
    spc  *= attenuation;
    return amb + dif + spc;
}

// ----------------------------------------------------------------------------
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir   = normalize(light.position - fragPos);
    float theta     = dot(lightDir, normalize(-light.direction));
    float epsilon   = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    float diff      = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec      = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant 
                             + light.linear   * distance 
                             + light.quadratic * distance * distance);

    vec3 amb  = light.ambient  * vec3(texture(material.diffuse,  TexCoords));
    vec3 dif  = light.diffuse  * diff                 * vec3(texture(material.diffuse,  TexCoords));
    vec3 spc  = light.specular * spec                * vec3(texture(material.specular, TexCoords));

    amb  *= attenuation * intensity;
    dif  *= attenuation * intensity;
    spc  *= attenuation * intensity;
    return amb + dif + spc;
}

// ----------------------------------------------------------------------------
void main()
{
    vec3 norm    = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // 1) directional
    vec3 result = CalcDirLight(dirLight, norm, viewDir);

    // 2) point lights
    for(int i = 0; i < NR_POINT_LIGHTS; ++i)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);

    // 3) spotlight
    result += CalcSpotLight(spotLight, norm, FragPos, viewDir);

    FragColor = vec4(result, 1.0);
}
