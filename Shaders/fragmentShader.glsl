#version 430 core

layout(location = 0) out vec4 FragColor;

in vec2 v_TexCoord;
in vec3 Normal;
in vec3 FragPos; 
in vec4 DirectionalLightSpacePos;                                                   // 平行光照射的点的位置
in vec4 SpotLightSpacePos;;
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

    vec3 color;

    vec3 position;                                          // 聚光灯位置
    vec3 direction;
    
    /* 光源衰减 */
    float quadratic;
    float linear;
    float constant;

    float cutoff;
    float outerCutoff;
};

uniform sampler2D spotShadowMap;
uniform sampler2D directionalShadowMap;
uniform samplerCube depthMap;

uniform Material material;
uniform DirLight dirLight;
uniform PointLight pointLight;
uniform SpotLight spotLight;

uniform bool open_DirLight;
uniform bool open_PointLight;
uniform bool open_SpotLight;
uniform bool gamma_correction;

uniform vec3 viewPos;
uniform float far_plane;
uniform float near_plane;

vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);                                                                      // 点光源阴影优化

vec3 CalDirLight(DirLight light, vec3 norm, vec3 viewDir, vec3 color);
vec3 CalPointLight(PointLight light, vec3 normal, vec3 viewDir, vec3 color);
vec3 CalSpotLight(SpotLight light, vec3 viewDir,vec3 color );

float spotShadowCalculation(vec4 fragPosLightSpace, vec3 norm);

float OMNShadowCalculation(vec3 fragPos, PointLight pointLight);

float derectionShadowCalculation(vec4 fragPosLightSpace, vec3 norm);

void main()
{
    vec3 lighting;

    vec3 normal = texture(material.texture_normal1,v_TexCoord).rgb;

    vec3 color = texture(material.texture_diffuse1, v_TexCoord).rgb;
    color *= texture(material.texture_diffuse2, v_TexCoord).rgb;
    color *= texture(material.texture_diffuse3, v_TexCoord).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    vec3 viewDir = normalize(viewPos - FragPos);

    if(open_DirLight && open_PointLight && open_SpotLight)                                  // 全开
    {
        lighting = CalDirLight(dirLight, normal, viewDir, color);
        lighting += CalPointLight(pointLight, normal, viewDir, color);
        lighting += CalSpotLight(spotLight, viewDir ,color);
        if(gamma_correction)
            FragColor = pow(vec4(lighting, 1.0), vec4(1/2.2)); 
        else
            FragColor = vec4(lighting, 1.0); 
    }
    else if(!open_DirLight && open_PointLight && open_SpotLight)                            // 平行光关，点光开，聚光开
    {
        lighting = CalPointLight(pointLight, normal, viewDir, color);
        lighting += CalSpotLight(spotLight, viewDir ,color);
        if(gamma_correction)
            FragColor = pow(vec4(lighting, 1.0), vec4(1/2.2)); 
        else
            FragColor = vec4(lighting, 1.0); 
    }
    else if(!open_DirLight && !open_PointLight && open_SpotLight)                            // 平行光关，点光关，聚光开
    {
        lighting = CalSpotLight(spotLight, viewDir ,color);
        if(gamma_correction)
            FragColor = pow(vec4(lighting, 1.0), vec4(1/2.2)); 
        else
            FragColor = vec4(lighting, 1.0); 
    }
    else if(!open_DirLight && open_PointLight && !open_SpotLight)                            // 平行光关，点光开，聚光关
    {
        lighting = CalPointLight(pointLight, normal, viewDir, color);
        if(gamma_correction)
            FragColor = pow(vec4(lighting, 1.0), vec4(1/2.2)); 
        else
            FragColor = vec4(lighting, 1.0); 
    }
    else if(open_DirLight && !open_PointLight && open_SpotLight)                            // 平行光开，点光关，聚光开
    {
        lighting = CalDirLight(dirLight, normal, viewDir, color);
        lighting += CalSpotLight(spotLight, viewDir ,color);
        if(gamma_correction)
            FragColor = pow(vec4(lighting, 1.0), vec4(1/2.2)); 
        else
            FragColor = vec4(lighting, 1.0); 
    }
    else if(open_DirLight && !open_PointLight && !open_SpotLight)                            // 平行光开，点光关，聚光关
    {
        lighting = CalDirLight(dirLight, normal, viewDir, color);
        if(gamma_correction)
            FragColor = pow(vec4(lighting, 1.0), vec4(1/2.2)); 
        else
            FragColor = vec4(lighting, 1.0); 
    }
    else if(open_DirLight && open_PointLight && !open_SpotLight)                            // 平行光开，点光开，聚光关
    {
        lighting = CalDirLight(dirLight, normal, viewDir, color);
        lighting += CalPointLight(pointLight, normal, viewDir, color);
        if(gamma_correction)
            FragColor = pow(vec4(lighting, 1.0), vec4(1/2.2));              // 伽马矫正，一般来说，显示器输出时会将输出的颜色做一个2.2次方，这里将其改回去呈现原本颜色
        else
            FragColor = vec4(lighting, 1.0);
    }
    else if(!open_DirLight && !open_PointLight && !open_SpotLight)
    {
        if(gamma_correction)
            FragColor = pow(vec4(color, 1.0), vec4(1/2.2)); 
        else
            FragColor = vec4(color, 1.0); 
    }

}


// 手电筒
vec3 CalSpotLight(SpotLight light,vec3 viewDir, vec3 color )
{
    vec3 normal = texture(material.texture_normal1,v_TexCoord).rgb;
    normal = normalize(normal * 2.0 - 1.0);                                          // 由于前部分已经用了法线贴图，那么照射时也应该用法线贴图的法线计算

    vec3 lightDir = normalize(light.position - FragPos);

    float diff = max( dot(lightDir, normal), 0.0f);   

    vec3 reflectDir = reflect(-lightDir, normal);                                   //反射光
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.Shininess);

    vec3 ambient = light.ambient;
    vec3 diffuse = diff * light.diffuse;
    vec3 specular = light.specular * spec * texture(material.texture_specular1,v_TexCoord).rgb;
     
    float theta = dot(lightDir, normalize(-light.direction));

    if(theta > light.cutoff)                                     // 计算的余弦值，当余弦值大的时候，角度反而小，在光照范围内时
    {
        float epsilon = light.cutoff - light.outerCutoff;
        float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0f, 1.0f);                                             // 光圈边缘平滑

        float Distance = length( light.position - FragPos);
        float attenuation = 1.0f / (light.linear * Distance + light.constant + light.quadratic * pow(Distance, 2));                 // 衰减

        float shadow = spotShadowCalculation(SpotLightSpacePos, normal);

        ambient *= intensity;
        diffuse *= intensity;
        specular *= intensity;
    
        
        return((ambient * color + (1.0 - shadow) * (diffuse * color))) * light.color;
    }
    else{                                                                                                                         // 不在范围内采用环境光照
        return ambient * color;
    }
}

// 聚光灯阴影贴图
float spotShadowCalculation(vec4 fragPosLightSpace, vec3 norm)
{
    // 把光空间片段位置转换为裁切空间的标准化设备坐标
    // 执行透视除法，获取阴影贴图坐标，将裁切空间坐标的范围-w到w转为-1到1，这要将x、y、z元素除以向量的w元素来实现
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 将贴图坐标转至[0,1]，z做深度比较
    projCoords = projCoords * 0.5 + 0.5;

    float currentDepth = projCoords.z;

    vec3 normal = normalize(norm);
    vec3 lightDir = normalize(spotLight.position - FragPos);

    float bias = max(0.00025 * (1.0 - dot(normal, lightDir)), 0.00005);                                             // bias 需要根据实际调整，可轻微防止阴影失真
    // PCF， 将阴影边缘的值平均起来，减少锯齿
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(spotShadowMap, 0);
    
    for(float x = -1; x <= 1; x++)
    {
        for(float y = -1; y <= 1; y++)
        {
            float pcfDepth = texture(spotShadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // 在far_plane以外的地方取无阴影贴图
    if(projCoords.z >= 1.0)
        shadow = 0.0;
        
    return shadow;
}

// 点光源
vec3 CalPointLight(PointLight light, vec3 normal, vec3 viewDir, vec3 color)
{
    vec3 lightDir = normalize(light.position - TBN * FragPos);

    vec3 ambient = light.ambient;
    float diff = max(dot(lightDir, normal), 0.0f);
    vec3 diffuse = diff * light.diffuse * color;

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.Shininess);
    vec3 specular = spec * light.specular * texture(material.texture_specular1, v_TexCoord).rgb;

    float Distance = length( light.position - FragPos);
    float attenuation = 1.0f / (light.linear * Distance + light.constant + light.quadratic * pow(Distance, 2));

    float shadow = OMNShadowCalculation(FragPos, light);
    
    // 有衰减, 无反射    + (1.0 - shadow) * specular * attenuation * light.color
    //return (ambient * attenuation + (1.0 - shadow) * diffuse * attenuation) * color * light.color;

    // 无衰减，无反射   + (1.0 - shadow) * specular（反射）
    return (ambient + (1.0 - shadow) * diffuse) * color * light.color;
}

//平行光
vec3 CalDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 color)
{
    vec3 lightDir = normalize(light.direction - FragPos);

    vec3 ambient = light.ambient;
    float diff = max(dot(lightDir, normal), 0.0f);
    vec3 diffuse = diff * light.diffuse;

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.Shininess);
    vec3 specular = spec * light.specular * texture(material.texture_specular1, v_TexCoord).rgb;

    float shadow = derectionShadowCalculation(DirectionalLightSpacePos, normal);
    
    // 无反射     + (1.0 - shadow) * specular
    return (ambient + (1.0 - shadow) * diffuse) * color;
}


// 平行光阴影贴图（同聚光灯阴影）
float derectionShadowCalculation(vec4 fragPosLightSpace, vec3 norm)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    projCoords = projCoords * 0.5 + 0.5;

    float currentDepth = projCoords.z;

    vec3 normal = normalize(norm);
    vec3 lightDir = normalize(dirLight.direction - FragPos);

    float bias = max(0.0005 * (1.0 - dot(normal, lightDir)), 0.001);                                             // bias 需要根据实际调整
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(directionalShadowMap, 0);
    for(float x = -1; x <= 1; x++)
    {
        for(float y = -1; y <= 1; y++)
        {
            float pcfDepth = texture(directionalShadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias> pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
//    float pcfDepth = texture(directionalShadowMap, projCoords.xy).r;
//
//    if(pcfDepth < currentDepth)
//        shadow = 4.0;

    if(projCoords.z >= 1.0)
        shadow = 0.0;
        
    return shadow;
}



// 点光源阴影贴图
float OMNShadowCalculation(vec3 fragPos, PointLight pointLight)
{
    vec3 lightToFrag = fragPos - pointLight.position;
    float currentDepth = length(lightToFrag);
 
    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;                                                                        // 调整
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(depthMap, lightToFrag + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= far_plane;   // 区间[0,far_plane]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);

    return shadow;
}