#version 450 core
#extension GL_ARB_separate_shader_objects : enable
#define MAX_SLOTS_BUFFER_SIZE 1024

layout(location = 0) in uint a_lru_id;
layout(location = 1) in vec3 a_pos;
layout(location = 2) in vec3 a_rot;
layout(location = 3) in vec2 a_scale;

out vec2 f_texcoords;

struct slot {
    uint id;
    float u1,v1,u2,v2;
};
layout (binding = 0) uniform u_cache_slots {
    slot slots[MAX_SLOTS_BUFFER_SIZE];
};

const vec2 quad_vertices[6] = vec2[6](
    vec2(0.0,0.0),
    vec2(1.0,0.0),
    vec2(0.0,1.0),
    vec2(0.0,1.0),
    vec2(1.0,0.0),
    vec2(1.0,1.0)
);

void main(){
    vec2 q = quad_vertices[gl_VertexID];
    vec3 px = a_pos + vec3(q*a_scale,0);
    gl_Position = vec4(px/300.0,1);
    slot s = slots[a_lru_id];
    f_texcoords = mix(vec2(s.u1,s.v1),vec2(s.u2,s.v2),q);
}
