#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec2 v_v1;
layout (location = 1) in vec2 v_v2;
layout (location = 2) in vec2 v_c1;
layout (location = 3) in vec2 v_c2;
layout (location = 4) in int v_type;

//layout (push_constant) uniform push_constants_b {
//    float max_sdf_distance;
//} push_constant;

// Exact BBox to a quadratic bezier ( from iq )
void bezier2_bbox(in vec2 p0, in vec2 p1, in vec2 p2 , out vec2 mi, out vec2 ma )
{
    // extremes
    mi = min(p0,p2);
    ma = max(p0,p2);

    // maxima/minima point, if p1 is outside the current bbox/hull
    if( p1.x<mi.x || p1.x>ma.x || p1.y<mi.y || p1.y>ma.y )
    {
        // p = (1-t)^2*p0 + 2(1-t)t*p1 + t^2*p2
        // dp/dt = 2(t-1)*p0 + 2(1-2t)*p1 + 2t*p2 = t*(2*p0-4*p1+2*p2) + 2*(p1-p0)
        // dp/dt = 0 -> t*(p0-2*p1+p2) = (p0-p1);

        vec2 t = clamp((p0-p1)/(p0-2.0*p1+p2),0.0,1.0);
        vec2 s = 1.0 - t;
        vec2 q = s*s*p0 + 2.0*s*t*p1 + t*t*p2;
        
        mi = min(mi,q);
        ma = max(ma,q);
    }
}

const int quad_indices[6] = {0,1,2,2,1,3};
const vec2 quad_vertices[4] = {vec2(0,0),vec2(1,0),vec2(0,1),vec2(1,1)};

layout (location = 0) out vec2 f_pixel_coord;
layout (location = 1) out flat vec2 f_bezier_v1;
layout (location = 2) out flat vec2 f_bezier_v2;
layout (location = 3) out flat vec2 f_bezier_c1;
layout (location = 4) out flat vec2 f_bezier_c2;
layout (location = 5) out flat int f_bezier_type;

void main() {
    float max_sdf_distance = 128.;
    vec2 pos = quad_vertices[gl_VertexID ];

    switch (v_type) {
        case 1: { // line
            vec2 mi = min(v_v1,v_v2);
            vec2 ma = max(v_v1,v_v2);
            mi -= max_sdf_distance;// push_constant.max_sdf_distance;
            ma += max_sdf_distance;// push_constant.max_sdf_distance;
            pos = mi + pos * (ma-mi);
            break;
        }

        default: case 2: { // quadratic bezier
            vec2 mi,ma;
            bezier2_bbox(v_v1,v_c1,v_v2,mi,ma);
            mi -=max_sdf_distance;// push_constant.max_sdf_distance;
            ma +=max_sdf_distance;// push_constant.max_sdf_distance;
            pos = mi + pos * (ma-mi);
            break;
        }
        case 3: { // cubic bezier
            break;
        }
    }

    f_pixel_coord = pos;
    f_bezier_v1 = v_v1;
    f_bezier_v2 = v_v2;
    f_bezier_c1 = v_c1;
    f_bezier_c2 = v_c2;
    f_bezier_type = v_type;

    vec2 ndc = pos / 2000.;//vec2(push_constant.mvp * vec3(pos,1.0));
    gl_Position = vec4(ndc,0,1);
}