#!/usr/bin/env python3
import os
import subprocess

BYTES_PER_LINE = 6

# https://www.lunarg.com/simplify-spir-v-size-reduction-with-os-option/
vksdk = os.environ['VULKAN_SDK']
glslang_validator = os.path.join(vksdk,'Bin','glslangValidator.exe')
glslc = os.path.join(vksdk,'Bin','glslc.exe')
spirv_opt = os.path.join(vksdk,'Bin','spirv-opt.exe')
spirv_remap = os.path.join(vksdk, 'Bin','spirv-remap.exe')
spirv_dis = os.path.join(vksdk, 'Bin','spirv-dis.exe') # diassembler

def embed_file_as_spirv(path: str):
    spirv_name = path + '.spv'
    
    # subprocess.run([glslang_validator, '--client','opengl100','-g', path],shell=True)
    subprocess.run([glslc, '-std=450core','--target-env=opengl4.5', '--target-spv=spv1.5', '-o', spirv_name, path],shell=True)
    subprocess.run([spirv_opt, '--loop-unswitch', '--merge-return', '-O', spirv_name, '-o', spirv_name],shell=True)
    subprocess.run([spirv_remap,'--map','all', '--strip-all', '--dce','all', '--opt', 'all', '-i', spirv_name, '-o', '.'],shell=True)

    # get the disassembly of final file
    disasm = subprocess.run([spirv_dis,spirv_name,'--comment'], stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
    disasm_txt = disasm.stdout.decode("utf-8")

    #generate c header file with spirv code and disassembly
    crumbs = os.path.split(path)
    # print(crumbs)

    name = crumbs[-1]
    print(name)
    name = 'hz_gl3_' + name
    name = name.replace('.frag','_fragment_shader')
    name = name.replace('.vert','_vertex_shader')

    with open(name+'.h', 'w') as f:
        # insert disassembly as comment
        lines = disasm_txt.splitlines()
        f.write('#if 0\n')

        for line in lines:
            f.write('// {}\n'.format(line))

        f.write('#endif\n')
        f.write('\n')
        
        # insert array of values as spir-v code
        f.write('const uint8_t %s[] = \n{' % (name))
        # draw aligned number lines

        with open(spirv_name,mode='rb') as spirv:
            cnt = 0
            while spirv.tell() != os.fstat(spirv.fileno()).st_size:
                if (cnt % BYTES_PER_LINE) == 0: f.write('\n    ')
                val = int.from_bytes (spirv.read (1), byteorder='little')
                f.write('%4s,' % format(val))
                cnt += 1

        f.write('\n};\n')
        
    os.remove(spirv_name)

def embed_file_as_glsl(path: str):
    # generate c header file with glsl code
    path_parts = os.path.split(path)
    # print(crumbs)

    name = path_parts[-1]
    print(name)
    name = 'hz_gl4_' + name
    name = name.replace('.frag','_fragment_shader')
    name = name.replace('.vert','_vertex_shader')

    with open(name+'.h', 'w') as f:
        # insert array of values as spir-v code
        f.write('const char %s[] = \n{' % (name))
        # draw aligned number lines

        with open(path,mode='r') as glsl:
            lines = glsl.readlines()
            for line in lines:
                line = line.strip('\n') # remove \n from end
                line = line.replace('"', '\\"') # replace " with \"
                f.write('\n    \"%s\\n\"' % line)

        f.write('\n};\n')

def generate_files(files, embed_spirv: bool) -> None:
    for f in files:
        if embed_spirv: embed_file_as_spirv(f)
        else: embed_file_as_glsl(f)

def embed_glsl_files(output:str, files:list[str]) -> None:
    # generate c header file with glsl code
    output_macro = output

    with open(output, 'w') as of:
        of.write('#ifndef HZ_GL3_GLSL_SHADERS_H\n')
        of.write('#define HZ_GL3_GLSL_SHADERS_H\n\n')

        for f in files:
            path_parts = os.path.split(f)
            name = path_parts[-1]
            print(name)
            name = 'hz_gl3_' + name
            name = name.replace('.frag','_fsh')
            name = name.replace('.vert','_vsh')
            with open(f,mode='r') as glsl:
                # insert array of values as spir-v code
                of.write('static const char %s[] = \n{' % (name))
                # draw aligned number lines

                lines = glsl.readlines()
                for line in lines:
                    line = line.strip('\n') # remove \n from end
                    line = line.replace('"', '\\"') # replace " with \"
                    of.write('\n\"%s\\n\"' % line)

                of.write('\n};\n\n')
        
        of.write('#endif /* HZ_GL3_GLSL_SHADERS_H */')

def main():
    print('Vulkan SDK: ' + vksdk)
    print('GLSL compiler: ' + glslc)
    print('Spirv-Opt: ' + spirv_opt)
    print('Spirv-Remap: ' + spirv_remap)
    print('Spirv-Dis: ' + spirv_dis)

    embed_glsl_files('hz_gl3_glsl_shaders.h', [
        './shaders/curve_to_sdf.vert',
        './shaders/curve_to_sdf.frag',
        './shaders/stencil_kokojima.vert',
        './shaders/stencil_kokojima.frag',
        './shaders/fs_triangle.vert',
        './shaders/fs_triangle.frag',
        './shaders/char_quad.vert',
        './shaders/char_quad.frag',
        ])

    # generate_files([
    #     'shaders/curve_to_sdf.vert',
    #     'shaders/curve_to_sdf.frag',
    #     'shaders/stencil_kokojima.vert',
    #     'shaders/stencil_kokojima.frag',
    #     'shaders/fs_triangle.vert',
    #     'shaders/fs_triangle.frag',
    #     'shaders/char_quad.vert',
    #     'shaders/char_quad.frag',
    #     ], embed_spirv=False)

if __name__ == '__main__':
    main()