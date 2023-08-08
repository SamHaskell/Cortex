#version 450

layout(location = 0) in vec3 v_Position;
layout(location = 1) in vec3 v_Normal;
layout(location = 2) in vec3 v_Color;
layout(location = 3) in vec2 v_TexCoord;

layout(location = 0) out vec3 f_Normal;
layout(location = 1) out vec3 f_Color;
layout(location = 2) out vec2 f_TexCoord;

layout(push_constant) uniform Push {
    mat4 ModelToWorld;
} push;

void main() {
    gl_Position = push.ModelToWorld * vec4(v_Position, 1.0);
    f_Normal = v_Normal;
    f_Color = v_Color;
    f_TexCoord = v_TexCoord;
}
