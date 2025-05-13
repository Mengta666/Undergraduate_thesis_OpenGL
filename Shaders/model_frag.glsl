#version 430 core

layout(location = 0) out vec4 FragColor;

in vec2 v_TexCoord;
in vec3 Normal;
in vec3 FragPos; 
in vec4 DirectionalLightSpacePos;                                                   // 平行光照射的点的位置
in mat3 TBN;

struct Material{
	sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_diffuse3;
	sampler2D texture_specular1;
    sampler2D texture_normal1;                                           // 法线贴图，纹理更为真实
    float Shininess;
};

struct DirLight
{
    vec3 direction;                                         // 平行光方向

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

};

struct PointLight{
	vec3 position;

    vec3 color;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	/* 光源衰减 */
    float quadratic;
    float linear;
    float constant;
};

struct SpotLight{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 position;                                          // 聚光灯位置
    vec3 direction;
    
    /* 光源衰减 */
    float quadratic;
    float linear;
    float constant;

    float cutoff;
    float outerCutoff;
};

uniform sampler2D diffuseTexture;
uniform sampler2D directionalShadowMap;
uniform samplerCube depthMap;

uniform Material material;
uniform vec3 lightPos;
uniform vec3 PointlightPos;
uniform vec3 viewPos;
uniform float far_plane;

vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float ShadowCalculation(vec3 fragPos)
{
    vec3 lightToFrag = fragPos - PointlightPos;
    float currentDepth = length(lightToFrag);
 
    float shadow = 0.0;
    float bias = 0.015;
    int samples = 20;
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(depthMap, lightToFrag + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= far_plane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
        
        
    return shadow;
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(directionalShadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos);
//    vec3 lightDir = normalize(lightPos - FragPos);                                // 会出现阴影失真
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.00005);
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(directionalShadowMap, 0);
    for(float x = -1; x <= 1; x++)
    {
        for(float y = -1; y <= 1; y++)
        {
            float pcfDepth = texture(directionalShadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 18.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z >= 1.0)
        shadow = 0.0;
        
    return shadow;
}

void main()
{
    vec3 normal = texture(material.texture_normal1,v_TexCoord).rgb;

    vec3 color = texture(material.texture_diffuse1, v_TexCoord).rgb;
    color *= texture(material.texture_diffuse2, v_TexCoord).rgb;
    color *= texture(material.texture_diffuse3, v_TexCoord).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    vec3 lightColor = vec3(0.3);
    // ambient
    vec3 ambient = 0.3 * lightColor;
    // diffuse
    vec3 lightDir = normalize(TBN * lightPos - TBN * FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(TBN * viewPos - TBN * FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), material.Shininess);
    vec3 specular = spec * lightColor * texture(material.texture_specular1, v_TexCoord).rgb;    
    // calculate shadow
    float shadow = ShadowCalculation(DirectionalLightSpacePos);                      
    vec3 lighting = (ambient + (1.0 - shadow) * diffuse) * color + (1.0 - shadow) * specular ;  
    shadow = ShadowCalculation(FragPos);
    lighting += (ambient + (1.0 - shadow) * diffuse) * color + (1.0 - shadow) * specular ; 
    
    FragColor = vec4(lighting, 1.0);
}