#version 430 core

layout(location = 0) out vec4 FragColor;

in vec2 v_TexCoord;
in vec3 Normal;
in vec3 FragPos; 
in vec4 DirectionalLightSpacePos;                                                   // ƽ�й�����ĵ��λ��
in vec4 SpotLightSpacePos;;
in mat3 TBN;

struct Material{
	sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_diffuse3;
	sampler2D texture_specular1;
    sampler2D texture_normal1;                                           // ������ͼ�������Ϊ��ʵ
    float Shininess;
};

struct DirLight
{
    vec3 direction;                                         // ƽ�йⷽ��

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

	/* ��Դ˥�� */
    float quadratic;
    float linear;
    float constant;
};

struct SpotLight{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 color;

    vec3 position;                                          // �۹��λ��
    vec3 direction;
    
    /* ��Դ˥�� */
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
);                                                                      // ���Դ��Ӱ�Ż�

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

    if(open_DirLight && open_PointLight && open_SpotLight)                                  // ȫ��
    {
        lighting = CalDirLight(dirLight, normal, viewDir, color);
        lighting += CalPointLight(pointLight, normal, viewDir, color);
        lighting += CalSpotLight(spotLight, viewDir ,color);
        if(gamma_correction)
            FragColor = pow(vec4(lighting, 1.0), vec4(1/2.2)); 
        else
            FragColor = vec4(lighting, 1.0); 
    }
    else if(!open_DirLight && open_PointLight && open_SpotLight)                            // ƽ�й�أ���⿪���۹⿪
    {
        lighting = CalPointLight(pointLight, normal, viewDir, color);
        lighting += CalSpotLight(spotLight, viewDir ,color);
        if(gamma_correction)
            FragColor = pow(vec4(lighting, 1.0), vec4(1/2.2)); 
        else
            FragColor = vec4(lighting, 1.0); 
    }
    else if(!open_DirLight && !open_PointLight && open_SpotLight)                            // ƽ�й�أ����أ��۹⿪
    {
        lighting = CalSpotLight(spotLight, viewDir ,color);
        if(gamma_correction)
            FragColor = pow(vec4(lighting, 1.0), vec4(1/2.2)); 
        else
            FragColor = vec4(lighting, 1.0); 
    }
    else if(!open_DirLight && open_PointLight && !open_SpotLight)                            // ƽ�й�أ���⿪���۹��
    {
        lighting = CalPointLight(pointLight, normal, viewDir, color);
        if(gamma_correction)
            FragColor = pow(vec4(lighting, 1.0), vec4(1/2.2)); 
        else
            FragColor = vec4(lighting, 1.0); 
    }
    else if(open_DirLight && !open_PointLight && open_SpotLight)                            // ƽ�й⿪�����أ��۹⿪
    {
        lighting = CalDirLight(dirLight, normal, viewDir, color);
        lighting += CalSpotLight(spotLight, viewDir ,color);
        if(gamma_correction)
            FragColor = pow(vec4(lighting, 1.0), vec4(1/2.2)); 
        else
            FragColor = vec4(lighting, 1.0); 
    }
    else if(open_DirLight && !open_PointLight && !open_SpotLight)                            // ƽ�й⿪�����أ��۹��
    {
        lighting = CalDirLight(dirLight, normal, viewDir, color);
        if(gamma_correction)
            FragColor = pow(vec4(lighting, 1.0), vec4(1/2.2)); 
        else
            FragColor = vec4(lighting, 1.0); 
    }
    else if(open_DirLight && open_PointLight && !open_SpotLight)                            // ƽ�й⿪����⿪���۹��
    {
        lighting = CalDirLight(dirLight, normal, viewDir, color);
        lighting += CalPointLight(pointLight, normal, viewDir, color);
        if(gamma_correction)
            FragColor = pow(vec4(lighting, 1.0), vec4(1/2.2));              // ٤�������һ����˵����ʾ�����ʱ�Ὣ�������ɫ��һ��2.2�η������ｫ��Ļ�ȥ����ԭ����ɫ
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


// �ֵ�Ͳ
vec3 CalSpotLight(SpotLight light,vec3 viewDir, vec3 color )
{
    vec3 normal = texture(material.texture_normal1,v_TexCoord).rgb;
    normal = normalize(normal * 2.0 - 1.0);                                          // ����ǰ�����Ѿ����˷�����ͼ����ô����ʱҲӦ���÷�����ͼ�ķ��߼���

    vec3 lightDir = normalize(light.position - FragPos);

    float diff = max( dot(lightDir, normal), 0.0f);   

    vec3 reflectDir = reflect(-lightDir, normal);                                   //�����
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.Shininess);

    vec3 ambient = light.ambient;
    vec3 diffuse = diff * light.diffuse;
    vec3 specular = light.specular * spec * texture(material.texture_specular1,v_TexCoord).rgb;
     
    float theta = dot(lightDir, normalize(-light.direction));

    if(theta > light.cutoff)                                     // ���������ֵ��������ֵ���ʱ�򣬽Ƕȷ���С���ڹ��շ�Χ��ʱ
    {
        float epsilon = light.cutoff - light.outerCutoff;
        float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0f, 1.0f);                                             // ��Ȧ��Եƽ��

        float Distance = length( light.position - FragPos);
        float attenuation = 1.0f / (light.linear * Distance + light.constant + light.quadratic * pow(Distance, 2));                 // ˥��

        float shadow = spotShadowCalculation(SpotLightSpacePos, normal);

        ambient *= intensity;
        diffuse *= intensity;
        specular *= intensity;
    
        
        return((ambient * color + (1.0 - shadow) * (diffuse * color))) * light.color;
    }
    else{                                                                                                                         // ���ڷ�Χ�ڲ��û�������
        return ambient * color;
    }
}

// �۹����Ӱ��ͼ
float spotShadowCalculation(vec4 fragPosLightSpace, vec3 norm)
{
    // �ѹ�ռ�Ƭ��λ��ת��Ϊ���пռ�ı�׼���豸����
    // ִ��͸�ӳ�������ȡ��Ӱ��ͼ���꣬�����пռ�����ķ�Χ-w��wתΪ-1��1����Ҫ��x��y��zԪ�س���������wԪ����ʵ��
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // ����ͼ����ת��[0,1]��z����ȱȽ�
    projCoords = projCoords * 0.5 + 0.5;

    float currentDepth = projCoords.z;

    vec3 normal = normalize(norm);
    vec3 lightDir = normalize(spotLight.position - FragPos);

    float bias = max(0.00025 * (1.0 - dot(normal, lightDir)), 0.00005);                                             // bias ��Ҫ����ʵ�ʵ���������΢��ֹ��Ӱʧ��
    // PCF�� ����Ӱ��Ե��ֵƽ�����������پ��
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
    
    // ��far_plane����ĵط�ȡ����Ӱ��ͼ
    if(projCoords.z >= 1.0)
        shadow = 0.0;
        
    return shadow;
}

// ���Դ
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
    
    // ��˥��, �޷���    + (1.0 - shadow) * specular * attenuation * light.color
    //return (ambient * attenuation + (1.0 - shadow) * diffuse * attenuation) * color * light.color;

    // ��˥�����޷���   + (1.0 - shadow) * specular�����䣩
    return (ambient + (1.0 - shadow) * diffuse) * color * light.color;
}

//ƽ�й�
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
    
    // �޷���     + (1.0 - shadow) * specular
    return (ambient + (1.0 - shadow) * diffuse) * color;
}


// ƽ�й���Ӱ��ͼ��ͬ�۹����Ӱ��
float derectionShadowCalculation(vec4 fragPosLightSpace, vec3 norm)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    projCoords = projCoords * 0.5 + 0.5;

    float currentDepth = projCoords.z;

    vec3 normal = normalize(norm);
    vec3 lightDir = normalize(dirLight.direction - FragPos);

    float bias = max(0.0005 * (1.0 - dot(normal, lightDir)), 0.001);                                             // bias ��Ҫ����ʵ�ʵ���
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



// ���Դ��Ӱ��ͼ
float OMNShadowCalculation(vec3 fragPos, PointLight pointLight)
{
    vec3 lightToFrag = fragPos - pointLight.position;
    float currentDepth = length(lightToFrag);
 
    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;                                                                        // ����
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(depthMap, lightToFrag + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= far_plane;   // ����[0,far_plane]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);

    return shadow;
}