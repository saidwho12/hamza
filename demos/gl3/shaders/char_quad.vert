#version 330 core
#extension GL_ARB_separate_shader_objects : enable
#define MAX_SLOTS_BUFFER_SIZE 1024

layout(location = 0) in mat4 v_vp;
layout(location = 4) in vec4 v_ts;
layout(location = 5) in uvec4 v_style_vars; // u32 col, u32 outline_col, u32 glow_outline, u32 weight_shear
layout(location = 6) in uint v_lru_id;

vec4 unpack_color32(uint var){
    return vec4(
        float(var & 0xffu)/255.0,
        float((var>>8u) & 0xffu)/255.0,
        float((var>>16u) & 0xffu)/255.0,
        float((var>>24u) & 0xffu)/255.0
    );
}

out vec2 f_texcoords;
out mediump vec4 f_color;
out mediump vec4 f_outline_color;

struct slot {
    uint id;
    float u0,v0,u1,v1;
};

uniform u_cache_slots {
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

mat4 make_shear_matrix(float s)
{
    return mat4(
        1.0, 0.0, 0.0, 0.0,
        s, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
}

void main(){
    vec2 q = quad_vertices[gl_VertexID];
    uint weight_and_shear = v_style_vars[3];
    float shear = float(weight_and_shear & 0xffffu) / 65535.0;
    mat4 mvp = v_vp * make_shear_matrix(shear);

    vec3 px = vec3(mvp * vec4(v_ts.xy+q*v_ts.zw, 0.0, 1.0));
    gl_Position = vec4(px,1.0);
    slot s = slots[v_lru_id];
    f_texcoords = mix(vec2(s.u0,s.v0),vec2(s.u1,s.v1),q);

    // Pass style vars to fragment shader
    f_color = unpack_color32(v_style_vars[0]);
    f_outline_color = unpack_color32(v_style_vars[1]);
}
