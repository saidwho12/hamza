#version 450 core
#extension GL_ARB_separate_shader_objects : enable

in vec2 f_texcoords;
uniform sampler2D u_sdf_texture;

layout(location=0) out vec4 outColor;

#define ALPHA_THRESHOLD 0.0003
void main(){
    vec2 uv = f_texcoords;
    float sigDist = texture(u_sdf_texture, uv).x;

#if 0
    float alpha = float(sigDist>0.5);
#else
    ivec2 sz = textureSize(u_sdf_texture, 0).xy;
    float dx = dFdx(uv.x) * sz.x; 
    float dy = dFdy(uv.y) * sz.y;
    float toPixels = 8.0/sqrt(dx * dx + dy * dy);
    float w = fwidth(sigDist);
    float alpha = smoothstep(0.5 - w, 0.5 + w, sigDist);
#endif

    if (alpha < ALPHA_THRESHOLD)
        discard;

    outColor = vec4(1.0,1.0,1.0,alpha);
}