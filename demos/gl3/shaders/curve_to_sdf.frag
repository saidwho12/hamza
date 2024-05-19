#version 330 core
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec2 f_pixel_coord;
layout (location = 1) flat in vec2 f_bezier_v1;
layout (location = 2) flat in vec2 f_bezier_v2;
layout (location = 3) flat in vec2 f_bezier_c1;
layout (location = 4) flat in vec2 f_bezier_c2;
layout (location = 5) flat in int f_bezier_type;

layout (std140) uniform UboData {
    mat4 view_matrix;
    float max_sdf_distance;
};

float dot2( in vec2 v ) { return dot(v,v); }
#define saturate(x) clamp(x,0.,1.)

float cross2(in vec2 a, in vec2 b) {
    return a.x*b.y - a.y*b.x;
}

vec2 B1(in vec2 P0, in vec2 P1, in float t) {
    return P0 + t * (P1 - P0);
}

float ud_line(vec2 P0, vec2 P1, vec2 P)
{
    // P0 + t * (P1 - P0)
    vec2 dv = P1 - P0;
    float t = saturate(dot(P-P0,dv)/dot2(dv));
    return distance(P,(P0+t*dv));
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

// float ud_bezier2(vec2 P, vec2 P0, vec2 P1, vec2 P2)
// {
//     float res = 0.0;
    
//     vec2 p0 = P - P0;
//     vec2 p1 = P1 - P0;
//     vec2 p2 = P2 - 2.0*P1 + P0;

//     // declare coefficients for equation of form t^3 + at^2 + bt + c = 0
//     float a = dot2(p2);
//     float b = 3.0*dot(p1,p2);
//     float c = dot(2.0*p1,p1)-dot(p2,p0);
//     float d = -dot(p1,p0);

//     // declare y, p and q for depressed cubic form y^3 + py + q = 0 where:
//     // t = y - b/(3a)
//     // p = c/a - (b^2)/(3a^2) 
//     // q = (2b^3)/(27a^3) - (bc)/(3a^2) + d/a
//     float p = c/a - (b*b)/(3.0*a*a);
//     float q = (2.0*b*b*b)/(27.0*a*a*a) - (b*c)/(3.0*a*a) + d/a;

//     // compute the difference between maxima and inflection point, 
//     // this tells us how many roots the cubic has.
//     float z = pow(q/2.0,2.0) + pow(p/3.0,3.0);
//     float h = b/(3.0*a); // x-delta of inflextion point

//     if (z >= 0.0) {
//         // 1 root
//         z = sqrt(z);
//         vec2 x = vec2(z,-z)-q/2.0;
//         vec2 uv = sign(x)*pow(abs(x),vec2(1.0/3.0));
//         float t = clamp(uv.x+uv.y-h,0.0,1.0);
//         vec2 v = B2(P0,p1,p2,t) - P;
//         res = dot2(v);
//     } else {
//         // 3 roots
//         // Formula based on the paper on Joukowsky maps and their relation
//         // to the cubic equation.
//         // https://maa.tandfonline.com/doi/full/10.1080/00029890.2019.1528814#.YRBwTi295B1
//         // This solves the three-root case using an equilateral triangle's vertices
//         // projected onto the x-axis.
//         // h is the point of inflexion on the x-axis.
//         // h = b/(3a)
//         // tx = h+2rcos(theta)
//         // ty = h+2rcos(theta+(2pi)/3)
//         // only the first two of these roots are required.
//         float r = sqrt(p/-3.0), k = acos(q/(-2.0*r*r*r))/3.0;
//         float n = cos(k);
//         float m = cos(k+2.09439510239);
//         vec2 t = clamp(vec2(n,m)*2.0*r-h,0.0,1.0);
//         // t = clamp(t+vec2(h,-h), 0.0, 1.0);
//         vec2 vx = B2(P0,p1,p2,t.x) - P, vy = B2(P0,p1,p2,t.y) - P;
//         float xd = dot2(vx), yd = dot2(vy);
//         if (xd<yd) res=xd; else res=yd;
//     }
    
//     return sqrt(res);
// }

float udBezier( in vec2 pos, in vec2 A, in vec2 B, in vec2 C )
{    
    vec2 a = B - A;
    vec2 b = A - 2.0*B + C;
    vec2 c = a * 2.0;
    vec2 d = A - pos;

    float kk = 1.0/dot(b,b);
    float kx = kk * dot(a,b);
    float ky = kk * (2.0*dot(a,a)+dot(d,b))/3.0;
    float kz = kk * dot(d,a);      

    float res = 0.0;

    float p  = ky - kx*kx;
    float q  = kx*(2.0*kx*kx - 3.0*ky) + kz;
    float p3 = p*p*p;
    float q2 = q*q;
    float h  = q2 + 4.0*p3;

    if( h>=0.0 ) 
    {   // 1 root
        h = sqrt(h);
        vec2 x = (vec2(h,-h)-q)/2.0;

        #if 0
        // When p≈0 and p<0, h-q has catastrophic cancelation. So, we do
        // h=√(q²+4p³)=q·√(1+4p³/q²)=q·√(1+w) instead. Now we approximate
        // √ by a linear Taylor expansion into h≈q(1+½w) so that the q's
        // cancel each other in h-q. Expanding and simplifying further we
        // get x=vec2(p³/q,-p³/q-q). And using a second degree Taylor
        // expansion instead: x=vec2(k,-k-q) with k=(1-p³/q²)·p³/q
        if( abs(p)<0.001 )
        {
            float k = p3/q;              // linear approx
          //float k = (1.0-p3/q2)*p3/q;  // quadratic approx 
            x = vec2(k,-k-q);  
        }
        #endif

        vec2 uv = sign(x)*pow(abs(x), vec2(1.0/3.0));
        float t = clamp( uv.x+uv.y-kx, 0.0, 1.0 );
        vec2  q = d+(c+b*t)*t;
        res = dot2(q);
    }
    else 
    {   // 3 roots
        float z = sqrt(-p);
        float v = acos(q/(p*z*2.0))/3.0;
        float m = cos(v);
        float n = sin(v)*1.732050808;
        vec3  t = clamp( vec3(m+m,-n-m,n-m)*z-kx, 0.0, 1.0 );
        vec2  qx=d+(c+b*t.x)*t.x; float dx=dot2(qx);
        vec2  qy=d+(c+b*t.y)*t.y; float dy=dot2(qy);
        if( dx<dy ) res=dx; else res=dy;
    }
    
    return sqrt( res );
}

layout (location = 0) out vec4 outColor;

void main() {
    float t = max_sdf_distance;

    switch (f_bezier_type) {
        case 2: { //line
            t = ud_line(f_bezier_v1, f_bezier_v2, f_pixel_coord);
            break;
        }

        case 3: { //quadratic bezier
            t = udBezier(f_pixel_coord,f_bezier_v1, f_bezier_c1, f_bezier_v2);
            break;
        }

        case 4: { // cubic bezier
            break;
        }
    }

    if (t > max_sdf_distance) // discard fragments outside of curve max distance
        discard;

    t = 0.5 - (t / max_sdf_distance / 2.0);
    outColor = vec4(t,t,t,t);
}