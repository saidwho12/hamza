#version 330 core
#extension GL_ARB_separate_shader_objects : enable
const vec2 v[3] = vec2[3](vec2(-1,-1),vec2(3,-1),vec2(-1,3));
void main(){
    gl_Position = vec4(v[gl_VertexID],0,1);
}
