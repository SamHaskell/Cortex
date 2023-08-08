#version 450

layout(location = 0) in vec3 f_Color;
layout(location = 1) in vec2 f_TexCoord;

layout(set = 0, binding = 0) uniform sampler2D u_TexSampler;

layout(location = 0) out vec4 o_Color;

void main() {
    o_Color = vec4(f_Color * texture(u_TexSampler, f_TexCoord).rgb, 1.0f);
}