#version 450 core
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 v_pos;
layout(location = 1) in vec2 v_uv;

layout (std140, binding = 0) uniform UboData {
    // mat4 view_matrix;
    float max_sdf_distance;
};

layout (location = 0) out vec2 uv;

void main() {
    vec4 ndc = vec4(v_pos/2000.0,0.0,0.0);//uboData.view_matrix * vec4(v_pos/2000.0f,0.0,1.0);
    gl_Position = vec4(ndc.xyz,1);
    uv = v_uv;
}