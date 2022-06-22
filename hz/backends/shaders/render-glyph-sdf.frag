#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec2 fragUv;
layout (location = 0) out vec4 outColor;

void main(void) {
    outColor = vec4(fragUv,0.0,1.0);
}
