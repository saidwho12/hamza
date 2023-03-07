const char hz_gl4_fs_triangle_fragment_shader[] = 
{
    "#version 450 core\n"
    "#extension GL_ARB_separate_shader_objects : enable\n"
    "\n"
    "layout(location=0) out vec4 outColor;\n"
    "void main(){\n"
    "    outColor = vec4(1.0);\n"
    "}\n"
};
