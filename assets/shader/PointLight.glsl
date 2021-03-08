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
    v_Normal = a_Normal;
    v_FragPosition = vec3(u_Transform * vec4(a_Position, 1.0f));
    gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0f);
}


#type fragment
#version 330 core
out vec4 fragColor;

uniform vec3 u_LightColor;

void main()
{
    fragColor = vec4(u_LightColor, 1.0);
}