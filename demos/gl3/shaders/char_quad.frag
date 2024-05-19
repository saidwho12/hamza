#version 330 core
#extension GL_ARB_separate_shader_objects : enable

in vec2 f_texcoords;
in mediump vec4 f_color;
in mediump vec4 f_outline_color;

uniform sampler2D u_sdf_texture;

layout(location=0) out vec4 out_col;

float msaa8x(vec2 p) {
    float alpha = 0.0;
    ivec2 sz = textureSize(u_sdf_texture, 0).xy;
    float dx = dFdx(p.x);// * sz.x;
    float dy = dFdy(p.y);// * sz.y;
    vec2 ps = 1.0/vec2(dx,dy); // pixel size
    
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            vec2 uv = p + ((vec2(x,y)+0.5)/8.0-0.5)/ps;
            float sigDist = texture(u_sdf_texture, uv).x;
            float a = step(0.5,sigDist);
            
            // float toPixels = 8.0/sqrt(dx * dx + dy * dy);
            // float w = fwidth(sigDist);
            // float a = smoothstep(0.5 - w, 0.5 + w, sigDist);
            alpha += a / (8.0*8.0);
        }
    }

    return alpha;
}

#define ALPHA_THRESHOLD 0.0003
void main(){
    vec2 uv = f_texcoords;

#if 0
    float alpha = msaa8x(uv);
#else
    float sigDist = texture(u_sdf_texture, uv).x;
    ivec2 sz = textureSize(u_sdf_texture, 0).xy;
    float dx = dFdx(uv.x) * sz.x; 
    float dy = dFdy(uv.y) * sz.y;
    float toPixels = 8.0/sqrt(dx * dx + dy * dy);
    float w = fwidth(sigDist);
    float outline_w = 0.05;
    float outline_min = 0.5-outline_w-w;
    float outline_max = 0.5+outline_w+w;
    float outline = float(sigDist <= outline_max && sigDist >= outline_min);
    float alpha = outline;
    // float alpha = smoothstep(0.5 - w, 0.5 + w, sigDist);
#endif

    if (alpha < ALPHA_THRESHOLD)
        discard;

    out_col = vec4(1.0,1.0,1.0,alpha)*f_color;
}