#version 450

layout(location = 0) in vec3 v_Position;

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

void main() {
    gl_Position = u_UBO.model * vec4(v_Position, 1.0);
}