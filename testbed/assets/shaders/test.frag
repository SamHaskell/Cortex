#version 450

layout(set = 0, binding = 0) uniform SceneData {
    float time;
} u_SceneData;

layout(set = 1, binding = 0) uniform UniformBuffer {
    mat4 model;
    mat4 view;
    mat4 proj;
} u_UBO;

layout(set = 2, binding = 0) uniform sampler2D u_TexSampler1;
layout(set = 2, binding = 1) uniform sampler2D u_TexSampler2;

layout(location = 0) out vec4 o_Color;

void main() {
    o_Color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}