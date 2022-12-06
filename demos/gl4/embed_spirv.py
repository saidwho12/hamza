#!/usr/bin/env python3
import os
import subprocess

BYTES_PER_LINE = 6

# https://www.lunarg.com/simplify-spir-v-size-reduction-with-os-option/
vksdk = os.environ['VULKAN_SDK']
glslc = os.path.join(vksdk,'Bin','glslc.exe')
spirv_opt = os.path.join(vksdk,'Bin','spirv-opt.exe')
spirv_remap = os.path.join(vksdk, 'Bin','spirv-remap.exe')
spirv_dis = os.path.join(vksdk, 'Bin','spirv-dis.exe') # diassembler

def compile_file(path: str):
    spirv_name = path + '.spv'
    
    subprocess.run([glslc, '-std=450core','--target-env=opengl4.5', '--target-spv=spv1.5', '-o', spirv_name, path],shell=True)
    subprocess.run([spirv_opt, '--loop-unswitch', '--merge-return', '-O', spirv_name, '-o', spirv_name],shell=True)
    subprocess.run([spirv_remap,'--map','all', '--strip-all', '--dce','all', '--opt', 'all', '-i', spirv_name, '-o', '.'],shell=True)

    # get the disassembly of final file
    disasm = subprocess.run([spirv_dis,spirv_name,'--comment'], stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
    disasm_txt = disasm.stdout.decode("utf-8");

    #generate c header file with spirv code and disassembly
    crumbs = os.path.split(path)
    # print(crumbs)

    name = crumbs[-1]
    print(name)
    name = 'hz_gl4_' + name
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

def main():
    print('Vulkan SDK: ' + vksdk)
    print('GLSL compiler: ' + glslc)
    print('Spirv-Opt: ' + spirv_opt)
    print('Spirv-Remap: ' + spirv_remap)
    print('Spirv-Dis: ' + spirv_dis)

    compile_file('shaders/curve_to_sdf.vert')
    compile_file('shaders/curve_to_sdf.frag')
    compile_file('shaders/stencil_kokojima.vert')
    compile_file('shaders/stencil_kokojima.frag')
    compile_file('shaders/fs_triangle.vert')
    compile_file('shaders/fs_triangle.frag')


if __name__ == '__main__':
    main()