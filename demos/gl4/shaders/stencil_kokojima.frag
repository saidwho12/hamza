#version 450 core
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 uv;
layout (location = 0) out vec4 outColor;

void main() {
    if ((uv.x*uv.x - uv.y) >= 0.0)
        discard; // outside

    outColor = vec4( 1.0 );
}