#version 450
#extension GL_ARB_separate_shader_objects : enable
#define dot2(v) dot(v,v)
#define saturate(x) clamp(x,0.,1.)

#define MOVETO 1 // unused
#define LINE 2
#define QUADRATIC_BEZIER 3
#define CUBIC_BEZIER 4

float cross2(in vec2 a, in vec2 b) {
    return a.x*b.y - a.y*b.x;
}

vec2 B1(in vec2 P0, in vec2 P1, in float t) {
    return P0 + t * (P1 - P0);
}

float sd_line(in vec2 P0, in vec2 P1, in vec2 P) {
    // P0 + t * (P1 - P0)
    vec2 dv = P1 - P0;
    float t = saturate(dot(P-P0,dv)/dot2(dv));
    vec2 z = (P0+t*dv) - P;
    return sign(cross2(dv,z)) * sqrt(dot2(z));
}

vec2 B2(in vec2 P0, in vec2 p1, in vec2 p2, in float t) {
    return t*t*p2 + 2.0*t*p1 + P0;
}

vec2 B3(in vec2 P0, in vec2 p1, in vec2 p2, in vec2 p3, in float t) {
    return t*t*t*p3 + 3.0*t*t*p2 + 3.0*t*p1 + P0;
}

// Taken from scholarius at https://www.shadertoy.com/view/wts3RX
#define NEWTON_ITER 2
#define HALLEY_ITER 0

float cbrt( float x ) {
    if (sign(x) == 0.0) return 0.0;
    
    float y = sign(x) * uintBitsToFloat( floatBitsToUint( abs(x) ) / 3u + 0x2a514067u );
        
    
    for( int i = 0; i < NEWTON_ITER; ++i )
        y = ( 2. * y + x / ( y * y ) ) * .333333333;

    for( int i = 0; i < HALLEY_ITER; ++i )
    {
        float y3 = y * y * y;
        y *= ( y3 + 2. * x ) / ( 2. * y3 + x );
    }
    
    return y;
}


float dist_cubic_inflexion_to_extrema(float p, float q) {
    return pow(q/2.0,2.0) + pow(p/3.0,3.0);
}

#define FLT_EPSILON 1.e-7
#define PI 3.14159

// Complex cube root
vec3 complex_cbrt(vec2 c) {
    float m = pow(dot(c,c),1.0/6.0); // magnitude
    float t = atan(c.y/c.x);// theta original angle
    float t0 = (t+2.0*PI*0.)/3.0;
    float t1 = (t+2.0*PI*1.0)/3.0;
    float t2 = (t+2.0*PI*2.0)/3.0;
    vec2 z0 = vec2(cos(t0),sin(t0))*m;
    vec2 z1 = vec2(cos(t1),sin(t1))*m;
    vec2 z2 = vec2(cos(t2),sin(t2))*m;
    return vec3(z0[0], z1[0], z2[0]);
}

vec2 complex_conjugate(vec2 z) {
    return vec2(z[0], -z[1]);
}

float sd_quadratic_bezier(vec2 P0, vec2 P1, vec2 P2, vec2 P)
{
    float res;
    
    vec2 p0 = P - P0;
    vec2 p1 = P1 - P0;
    vec2 p2 = P2 - 2.0*P1 + P0;

    // declare coefficients for equation of form t^3 + at^2 + bt + c = 0
    float a = dot2(p2);
    float b = 3.0*dot(p1,p2);
    float c = dot(2.0*p1,p1)-dot(p2,p0);
    float d = -dot(p1,p0);

    // declare y, p and q for depressed cubic form y^3 + py + q = 0 where:
    // t = y - b/(3a)
    // p = c/a - (b^2)/(3a^2) 
    // q = (2b^3)/(27a^3) - (bc)/(3a^2) + d/a
    float p = c/a - (b*b)/(3.0*a*a);
    float q = (2.0*b*b*b)/(27.0*a*a*a) - (b*c)/(3.0*a*a) + d/a;

    // compute the difference between maxima and inflection point, 
    // this tells us how many roots the cubic has.
    float z = pow(q/2.0,2.0) + pow(p/3.0,3.0);
    float h = b/(3.0*a); // x-offset of inflextion point
    float sgn = 1.0;

    if (z>=0.0) {
        // 1 root
        z = sqrt(z);
        vec2 x = vec2(z,-z)-q/2.0;
        vec2 uv = sign(x)*pow(abs(x),vec2(1.0/3.0));
        float t = clamp(uv.x+uv.y-h,0.0,1.0);
        vec2 v = B2(P0,p1,p2,t) - P;
        sgn = cross2(v, 2.*p2*t+2.*p1);
        res = dot2(v);
    } else {
        // 3 roots
        // Formula based on the paper on Joukowsky maps and their relation
        // to the cubic equation.
        // https://maa.tandfonline.com/doi/full/10.1080/00029890.2019.1528814#.YRBwTi295B1
        // This solves the three-root case using an equilateral triangle's vertices
        // projected onto the x-axis.
        // h is the point of inflexion on the x-axis.
        // h = b/(3a)
        // tx = h+2rcos(theta)
        // ty = h+2rcos(theta+(2pi)/3)
        // only the first two of these roots are required.
        float r = sqrt(p/-3.0), theta = acos(q/(-2.0*r*r*r))/3.0;
        vec2 t = 2.0*r*vec2(cos(theta), cos(theta+2.09439510239));
        t = saturate(t-h);
        
        vec2 vx = B2(P0,p1,p2,t.x) - P, vy = B2(P0,p1,p2,t.y) - P;
        float xd = dot2(vx), yd = dot2(vy);
        float sx = cross2(2.*p2*t.x+2.*p1,vx), sy = cross2(2.*p2*t.y+2.*p1,vy);
        
        if (xd<yd) { res=xd; sgn=sx; } else { res=yd; sgn=sy; }
    }
    
    // sdDistance(t) = cross2((dBn/dt)(t), P-Bn(t)) * ||Bn(t)-P||
    return sign(sgn) * sqrt(res);
}

// layout (push_constant) uniform push_constants_b {
//     float max_sdf_distance;  
// } push_constant;



layout (location = 0) in vec2 f_pixel_coord;
layout (location = 1) in flat vec2 f_bezier_v1;
layout (location = 2) in flat vec2 f_bezier_v2;
layout (location = 3) in flat vec2 f_bezier_c1;
layout (location = 4) in flat vec2 f_bezier_c2;
layout (location = 5) in flat int f_bezier_type;

layout (location = 0) out vec4 outColor;

void main() {
    float max_sdf_distance = 128.;
    float t = max_sdf_distance;

    switch (f_bezier_type) {
        case LINE: {
            t = sd_line(f_bezier_v1, f_bezier_v2, f_pixel_coord);
            break;
        }

        case QUADRATIC_BEZIER: {
            t = sd_quadratic_bezier(f_bezier_v1, f_bezier_c1, f_bezier_v2, f_pixel_coord);
            break;
        }
    }

    outColor = vec4(1,1,1,1);
    return;

    if (abs(t) > max_sdf_distance) // discard fragments outside of curve max distance
        discard;

    t = 0.5 - (abs(t) / max_sdf_distance/2.0);
    outColor = vec4(t,0,0,1);
}