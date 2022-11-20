#version 450 core
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 v_pos;
layout(location = 1) in vec2 v_uv;

layout (location = 0) out vec2 uv;

void main() {
    vec2 ndc = v_pos / 2000.;
    gl_Position = vec4(ndc,0,1);
    uv = v_uv;
}