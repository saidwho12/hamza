const char hz_gl4_fs_triangle_vertex_shader[] = 
{
    "#version 450 core\n"
    "#extension GL_ARB_separate_shader_objects : enable\n"
    "const vec2 v[3] = vec2[3](vec2(-1,-1),vec2(3,-1),vec2(-1,3));\n"
    "void main(){\n"
    "    gl_Position = vec4(v[gl_VertexID],0,1);\n"
    "}\n"
};
