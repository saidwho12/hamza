const char hz_gl4_stencil_kokojima_fragment_shader[] = 
{
    "#version 450 core\n"
    "#extension GL_ARB_separate_shader_objects : enable\n"
    "\n"
    "layout(location = 0) in vec2 uv;\n"
    "layout (location = 0) out vec4 outColor;\n"
    "\n"
    "void main() {\n"
    "    if ((uv.x*uv.x - uv.y) >= 0.0)\n"
    "        discard; // outside\n"
    "\n"
    "    outColor = vec4( 1.0 );\n"
    "}\n"
};
