#version 450

layout(location = 0) in vec2 v_Position;
layout(location = 1) in vec3 v_Color;

layout(location = 0) out vec3 f_Color;

layout(push_constant) uniform Push {
    mat2 Transform;
} push;

void main() {
    gl_Position = vec4(push.Transform * v_Position, 0.0, 1.0);
    f_Color = v_Color;
}
