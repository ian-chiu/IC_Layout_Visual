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
    v_FragPosition = vec3(u_Transform * vec4(a_Position, 1.0));
    v_Normal = mat3(transpose(inverse(u_Transform))) * a_Normal;
    gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}


#type fragment
#version 330 core

struct DirLight {
    vec3 Direction;
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
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
uniform DirLight u_DirLight;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.Direction);
    //diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    //specular shading 
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.Shininess);
    //combine result
    vec3 ambient = light.Ambient * u_Material.Ambient;
    vec3 diffuse = light.Diffuse * (diff * u_Material.Diffuse);
    vec3 specular = light.Specular * (spec * u_Material.Specular);
    return (ambient + diffuse + specular);
}

void main()
{
    vec3 norm = normalize(v_Normal);
    vec3 viewDir = normalize(u_CameraPosition - v_FragPosition);
    // phase1: Directional lighting
    vec3 result = CalcDirLight(u_DirLight, norm, viewDir);

    fragColor = vec4(result, 1.0);
}