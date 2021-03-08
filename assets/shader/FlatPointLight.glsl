#type vertex
#version 330 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;

out vec3 v_Normal;
out vec3 v_FragPosition;

uniform mat4 u_Transform;
uniform mat4 u_ViewProjection;

void main()
{
    v_FragPosition = vec3(u_Transform * vec4(a_Position, 1.0f));
    v_Normal = mat3(transpose(inverse(u_Transform))) * a_Normal;
    gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0f);
}


#type fragment
#version 330 core
struct PointLight {
    vec3 Position;
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;

    float Constant;
    float Linear;
    float Quadratic;
};

struct Material {
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
    float Shininess;
};

out vec4 fragColor;

in vec3 v_Normal;
in vec3 v_FragPosition;

uniform vec3 u_CameraPosition;
uniform Material u_Material;
uniform PointLight u_PointLight;

void main()
{
    vec3 norm = normalize(v_Normal);
    float distance = length(u_PointLight.Position - v_FragPosition);
    float attenuation = 1.0 / (u_PointLight.Constant + u_PointLight.Linear * distance + 
            u_PointLight.Quadratic * (distance * distance));  

    vec3 ambient = u_Material.Ambient * u_PointLight.Ambient;
    
    vec3 lightDir = normalize(u_PointLight.Position - v_FragPosition);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = u_PointLight.Diffuse * (diff * u_Material.Diffuse);

    vec3 viewDir = normalize(u_CameraPosition - v_FragPosition);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.Shininess);
    vec3 specular = u_PointLight.Specular * (spec * u_Material.Specular); 

    ambient  *= attenuation; 
    diffuse  *= attenuation;
    specular *= attenuation;  

    vec3 result = ambient + diffuse + specular;
    fragColor = vec4(result, 1.0);
}