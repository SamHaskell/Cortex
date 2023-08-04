#version 450

layout(location = 0) in vec3 v_Position;
layout(location = 1) in vec3 v_Color;

layout(location = 0) out vec3 f_Color;

layout(push_constant) uniform Push {
    mat4 ModelToWorld;
} push;

void main() {
    gl_Position = push.ModelToWorld * vec4(v_Position, 1.0);
    f_Color = v_Color;
}
