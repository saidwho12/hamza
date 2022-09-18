#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) out vec2 fragUv;

struct Vertex {
    vec3 pos;
    vec2 uv;
};

Vertex triangleVerts[6] = {
    Vertex(vec3(.5, -.5, 0.0), vec2(1.0, 1.0)),
    Vertex(vec3(-.5, .5, 0.0), vec2(0.0, 0.0)),
    Vertex(vec3(-.5, -.5, 0.0), vec2(0.0, 1.0)),
    Vertex(vec3(-.5, .5, 0.0), vec2(0.0, 0.0)),
    Vertex(vec3(.5, -.5, 0.0), vec2(1.0, 1.0)),
    Vertex(vec3(.5, .5, 0.0), vec2(1.0, 0.0))
};

void main() {
    Vertex v = triangleVerts[gl_VertexIndex];
    gl_Position = vec4(v.pos, 1.0);
    fragUv = v.uv;
}